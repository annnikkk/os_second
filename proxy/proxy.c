#include <arpa/inet.h>
#include <curl/curl.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 8192

void sigpipe_handler(int sig) {
  const char *message = "SIGPIPE\n";
  write(STDOUT_FILENO, message, strlen(message));
}

void send_error(const int sock, const int http_status) {
  char error_response[256];
  snprintf(error_response, sizeof(error_response),
           "HTTP/1.1 %d Error\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %d\r\n"
           "Connection: close\r\n\r\n"
           "Error.\n",
           http_status, (int)strlen("Error.\n"));
  send(sock, error_response, strlen(error_response), 0);
}

ssize_t read_full_request(int sock, char *buffer, size_t buffer_size) {
  ssize_t total_read = 0;
  while (total_read < buffer_size - 1) {
    ssize_t bytes_read =
        recv(sock, buffer + total_read, buffer_size - total_read - 1, 0);
    if (bytes_read <= 0)
      return bytes_read;

    total_read += bytes_read;

    if (strstr(buffer, "\r\n\r\n"))
      break;
  }

  buffer[total_read] = '\0';

  return total_read;
}


size_t body_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  int sock = *((int *)userdata);
  size_t total_size = size * nitems;

  char* data = (char*)malloc(total_size);

  char *connection_header = strstr(buffer, "Connection: keep-alive");
  if (connection_header) {
    memcpy(connection_header + strlen("Connection: "), "close", 5);
  }

  memcpy(data, buffer, total_size);

  send(sock, data, total_size, 0);

  free(data);
  return total_size;
}

int fetch_data(const char *url, int sock) {

  CURL *curl = curl_easy_init();
  if (!curl) {
    fprintf(stderr, "Failed to initialize libcurl\n");
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, body_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sock);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  const CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    printf("URL: %s\n", url);
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
    curl_easy_cleanup(curl);
    return -1;
  }

  curl_easy_cleanup(curl);
  return 0;
}

void *handle_client(void *args) {
  const int sock = *(int *)args;
  free(args);
  signal(SIGPIPE, sigpipe_handler);

  char buffer[BUFFER_SIZE];
  const ssize_t bytes_read = read_full_request(sock, buffer, sizeof(buffer));
  if (bytes_read <= 0) {
    printf("Error reading from socket\n");
    send_error(sock, 400);
    close(sock);
    return NULL;
  }

  char method[8], url[256];
  if (sscanf(buffer, "%s %s", method, url) != 2 || strcmp(method, "GET") != 0) {
    printf("Invalid request\n");
    send_error(sock, 400);
    close(sock);
    return NULL;
  }

  fetch_data(url, sock);

  close(sock);
  return NULL;
}

int main() {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Socket creation failed");
    return 1;
  }

  const int opt = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    perror("setsockopt");
    close(server_socket);
    return -1;
  }

  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
      .sin_addr.s_addr = INADDR_ANY,
  };

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("Bind failed");
    close(server_socket);
    return 1;
  }

  if (listen(server_socket, 10) == -1) {
    perror("Listen failed");
    close(server_socket);
    return 1;
  }

  printf("Proxy server listening on port %d\n", PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int *client_socket = malloc(sizeof(int));
    *client_socket =
        accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (*client_socket == -1) {
      perror("Accept failed");
      free(client_socket);
      continue;
    }

    printf("New client connected\n");

    pthread_t tid;
    if (pthread_create(&tid, NULL, handle_client, client_socket)) {
      fprintf(stderr, "main: pthread_create() failed\n");
      send_error(*client_socket, 500);
      break;
    }
    pthread_detach(tid);
  }

  close(server_socket);
  return 0;
}

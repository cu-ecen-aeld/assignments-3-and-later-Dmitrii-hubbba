#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

int status = 0;
socklen_t addr_size = 0;
int socket_fd, connection_fd = 0;
int connections_available = 5;
struct sockaddr_in address_to_print;
struct sockaddr_storage client_address;
struct addrinfo hints;
struct addrinfo *servinfo;

const size_t BUFF_SIZE = 100;
char buffer[100];
char string_to_write[100];

openlog(NULL, LOG_CONS, LOG_USER);
//COMMENT
printf("Start.\n");
syslog(LOG_INFO, "%s", "Start.");

//memset(buffer, 0, sizeof buffer);
memset(&hints, 0, sizeof hints);

hints.ai_family   = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags    = AI_PASSIVE;

if ((socket_fd = socket (PF_INET, SOCK_STREAM, 0)) == -1) return socket_fd;
//if (socket_fd == -1) return socket_fd;

if ((status = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0) return -1;

if (bind(socket_fd, (struct sockaddr *)servinfo->ai_addr, servinfo->ai_addrlen) == -1) return -1;

if (listen(socket_fd, connections_available) == -1) return -1;

addr_size = sizeof(client_address);

int bytes_received = 0;

//FILE *fd = fopen("/var/tmp/aesdsocketdata","a+");
FILE *fd = fopen("aesdsocketdata.txt", "a+");

while(1) {

connection_fd = accept(socket_fd, (struct sockaddr *)&client_address, &addr_size);//ADD check return value

printf("Accepting data.\n");

addr_size = sizeof(address_to_print);
status = getpeername(connection_fd, (struct sockaddr *)&address_to_print, &addr_size);
syslog(LOG_INFO, "Accepted connection from %s\n", inet_ntoa(address_to_print.sin_addr));
printf("Accepted connection from %s\n", inet_ntoa(address_to_print.sin_addr));

while(1) {

  memset(buffer, 0, sizeof buffer);
  memset(string_to_write, 0, sizeof string_to_write);
  bytes_received = recv(connection_fd, buffer, BUFF_SIZE - 1, 0);
  printf("Received %d bytes\n", bytes_received);

  if (bytes_received == 0) break;

  for (int i = 0; i < bytes_received; i++) {
    if (buffer[i] == '\n') {
      memcpy(string_to_write, buffer, 100);
      fprintf(fd, "%s", string_to_write);
      //printf("%s\n", string_to_write);
     // fflush(fd);
    }
  }

  fflush(fd);

  fscanf(fd, "%s", string_to_write);
  printf("Read string %s\n", string_to_write);
  send(connection_fd, string_to_write, 100, 0);
  //printf("Sent %d bytes.\n", bytes_sent);

}

//fflush(fd);

close(connection_fd);

}

close(socket_fd);

freeaddrinfo(servinfo);

return 0;

}


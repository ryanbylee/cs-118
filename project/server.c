#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 10

int main(int argc, char *argv[]) {
  char buf[2048];
  char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world</html>\r\n";
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1){
    perror("server: socket");
    return 1;
  }
  printf("socket successfully created\n");

  struct sockaddr_in servAddr;
  int servAddr_len = sizeof(servAddr);
  struct sockaddr_in clientAddr;
  int clientAddr_len = sizeof(clientAddr);

  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(PORT);
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&servAddr, servAddr_len) != 0){
    perror("server: bind");
    return 1; 
  }
  printf("socket bound to address\n");


  if (listen(sockfd, BACKLOG) != 0){
    perror("server: listen");
    return 1;
  }

  printf("listening to connections...\n");
  
  while (1){
    int singlesock = accept(sockfd, (struct sockaddr *)&servAddr, (socklen_t *) &servAddr_len);

    if (singlesock < 0){
      perror("server: accept");
      continue;
    }

    printf("connection accepted\n");

    int sockName = getsockname(singlesock, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddr_len);
    if (sockName < 0){
      perror("server: getsockname");
      continue;
    }
    if (read(singlesock, buf, 1024) < 0){
      perror("server: read\n");
      continue;
    }
    char method[1024], uri[1024], version[1024];
    printf("%s", buf);
    // sscanf(buf, "%s %s %s", method, uri, version);
    // printf("[%s:%u] %s %s %s\n", inet_ntoa(clientAddr.sin_addr),
    //         ntohs(clientAddr.sin_port), method, version, uri);
    if (write(singlesock, resp, strlen(resp)) < 0){
      perror("server: write\n");
      continue;
    }
    close(singlesock);
  }

  return 0;
}
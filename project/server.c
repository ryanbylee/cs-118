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

#define PORT "8080"
#define BACKLOG 10

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
  int sockfd, single_sockfd;
  struct addrinfo prefilled, *serverinfo, *result;

  struct sockaddr_storage client_addr;
  socklen_t size;

  char s[INET6_ADDRSTRLEN];
  int getaddrinfoErrno;
  int yes = 1;

  memset(&prefilled, 0, sizeof prefilled);

  prefilled.ai_family = AF_UNSPEC;
  prefilled.ai_socktype = SOCK_STREAM;
  prefilled.ai_flags = AI_PASSIVE;

  if ((getaddrinfoErrno = getaddrinfo(NULL, PORT, &prefilled, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(getaddrinfoErrno));
    return 1;
  }

  for (result = serverinfo; result != NULL; result = result->ai_next) {
    if ((sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
      perror("error creating socket");
      exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
      perror("error reusing socket");
      exit(1);
    }

    if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1){
      close(sockfd);
      perror("server: binding failed, trying the next address...");
      continue;
    }

    break;

  }

  freeaddrinfo(serverinfo);

  if (result == NULL){
    perror("binding failed");
    exit(1);
  }


  if (listen(sockfd, BACKLOG) == -1){
    perror("listen failed");
    exit(1);
  }
  printf("Waiting for connections...\n");


  while (1){
    size = sizeof client_addr;
    single_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &size);

    if (single_sockfd == -1){
      perror("accepting error");
      continue;
    }

    inet_ntop(client_addr.ss_family,
			get_in_addr((struct sockaddr *)&client_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

    if (send(single_sockfd, "Hello, world!", 13, 0) == -1)
				perror("send");
		close(single_sockfd);
		exit(0);
  }

  return 0;
}

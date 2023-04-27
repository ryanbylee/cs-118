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
#include <sys/stat.h>
#include <signal.h>

#define PORT 8080
#define BACKLOG 10

int main(int argc, char *argv[]) {
  const int yes = 1;
  char buf[2048];
  // char resp[] = "HTTP/1.0 200 OK\r\n"
  //               "Server: server-c\r\n"
  //               "Content-type: text/html\r\n\r\n"
  //               "<html>Hello world!</html>\r\n";
  
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

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){
    perror("server: setsockopt");
    return 1;
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(int)) < 0){
    perror("server: setsockopt");
    return 1;
  }

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
    sscanf(buf, "%s %s %s", method, uri, version);
    char uri_modified[strlen(uri)];
    if (strlen(uri) > 0){
      strncpy(uri_modified, uri + 1, strlen(uri));
    }
    
    printf("requested file: %s\n", uri_modified);
    char fileName[strlen(uri)];
    if (strlen(uri_modified) > 0){
      strncpy(fileName, uri_modified, strlen(uri));
    }
    char * token = strtok(fileName, ".");
    token = strtok(NULL, ".");
    printf("requeted file format: %s\n", token);

    //char readFile[MAXFILESIZE + 1];
    FILE *requested_file;
    if (token == NULL){
      printf("opening with binary flag\n");
      requested_file = fopen(uri_modified, "rb");
    }else{
      requested_file = fopen(uri_modified, "r");
    }
    
    //fseek(requested_file, 0, SEEK_END);


    


    char htmlresp[65];
    char plainresp[65];
    char jpgresp[65];
    char pngresp[65];
    char binaryresp[70];

    if (requested_file != NULL){
      struct stat fileStat;
      int fd = fileno(requested_file);
      fstat(fd, &fileStat);

      //TODO: Fix the sizes
      off_t size = fileStat.st_size;
      char *readFile = (char*) malloc(size);
      size_t length = fread(readFile, size, sizeof(char), requested_file);
      //readFile[size] = '\0';
      
      
      printf("successfully read file\n");
      
      char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: server-c\r\n";
      if (token == NULL){
        printf("it is binary type:\n");
        char binaryType[] = "application/octet-stream";
        sprintf(binaryresp, "Content-type: %s\r\n"
                      "Content-Length: %ld\r\n\r\n", binaryType, size);
        strcat(resp, binaryresp);
      }else if (!strcmp(token, "html") || !strcmp(token, "htm")){
        char htmlType[] = "text/html";
        sprintf(htmlresp, "Content-type: %s\r\n"
                      "Content-Length: %ld\r\n\r\n", htmlType, size);
        strcat(resp, htmlresp);
      }else if (!strcmp(token, "txt")){
        char txtType[] = "text/plain";
        sprintf(plainresp, "Content-type: %s\r\n"
                      "Content-Length: %ld\r\n\r\n", txtType, size);
        strcat(resp, plainresp);
      }else if (!strcmp(token, "jpg") || !strcmp(token, "jpeg")){
        //32000\r\n\r\n\0
        char jpegType[] = "image/jpeg";
        sprintf(jpgresp, "Content-type: %s\r\n"
                      "Content-Length: %ld\r\n\r\n", jpegType, size);
        strcat(resp, jpgresp);
      }else if (!strcmp(token, "png")){
        char pngType[] = "image/png";
        sprintf(jpgresp, "Content-type: %s\r\n"
                      "Content-Length: %ld\r\n\r\n", pngType, size);
        strcat(resp, pngresp);
      }
      printf("About to write file\n");
      // strcat(resp, readFile);
      if (write(singlesock, resp, strlen(resp)) < 0){
        perror("server: write\n");
        return 1;
      }
      printf("successfully wrote response header\n");
      if (write(singlesock, readFile, size) < 0){
        perror("server: write\n");
        return 1;
      }
      printf("successfully wrote file contents\n");
      //printf("\nresponse\n%s", resp);
      printf("%s", resp);
      
      

      // int read_bytes;
      // while (read_bytes = fread(readFile, sizeof(char), 2048, requested_file) != 0){
      //   printf("read_bytes: %d", read_bytes);
      //   if (read_bytes > 0){
      //     if (write(singlesock, readFile, read_bytes) < 0){
      //       perror("server: write");
      //       return 1;
      //     }
      //   }else{
      //     perror("server: read");
      //     return 1;
      //   }

      // }
      // if (written_bytes < size){
      //   write(singlesock, resp, strlen(resp)) < size
      //   perror("server: write\n");
      //   continue;
      // }




      //  while (totalbytes < size){
      //   char* subresp = (char*) malloc((size - totalbytes) * sizeof(char));
      //   memcpy(subresp, &resp[totalbytes], size - totalbytes);
      //   sentbytes = send(singlesock, subresp, sizeof(subresp), 0);
      //   totalbytes += sentbytes;
      //  }
      
      //close(singlesock);
      //fclose(requested_file);
      free(readFile);

    }else{
      printf("file not found\n");
      char resp[] = "HTTP/1.0 404 Not Found\r\n"
                "Server: server-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>404 Not Found</html>\r\n";
      if (write(singlesock, resp, strlen(resp)) < 0){
        perror("server: write\n");
        continue;
      }
      close(singlesock);
    }
    
    // if (write(singlesock, resp, strlen(resp)) < 0){
    //   perror("server: write\n");
    //   continue;
    // }
    // close(singlesock);
  }

  return 0;
}
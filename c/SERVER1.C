#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  int server_sockfd, client_sockfd;
  int server_len, client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* name the socket as agreed with the client */
  server_address.sin_family = AF_INET;
  /* server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); better INADDR_ANY! */
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  /* server_address.sin_port = 8888; use htons! */
  server_address.sin_port = htons(8888);
  server_len = sizeof(server_address);

  /* bind */
  bind(server_sockfd, (struct sockaddr *) &server_address, server_len);

  /* listen+accept */
  listen(server_sockfd, 5);
  while (1) {
    char ch;
    printf("server_waiting\n");
    client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);

    /* now alteration for multiple clients - fork() */
    if (fork() == 0) {

      /* since we are the client, we can now read/write via the socket */
      read(client_sockfd,&ch,1);
      ch++;
      write(client_sockfd,&ch,1);
      close(client_sockfd);
      exit(0);
    }
    else {
      /* we are the parent, close work for the client */
      close(client_sockfd);
    }
  }
}

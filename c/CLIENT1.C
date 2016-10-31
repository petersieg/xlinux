#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  int sockfd;
  int len;
  struct sockaddr_in address;
  int result;
  char ch = 'A';

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* name the socket as agreed with the server */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* address.sin_port = 8888; use htons! */
  address.sin_port = htons(8888);
  len = sizeof(address);

  /* connect */
  result = connect(sockfd, (struct sockaddr *) &address, len);
  if (result == -1) {
    perror("oops: client1");
    exit(1);
  }

  /* now we can read/write via the socket */
  write(sockfd,&ch,1);
  read(sockfd,&ch,1);
  printf("char from server = %c\n", ch);

  /* close */
  close(sockfd);
  exit(0);
}

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include<arpa/inet.h>
#include <unistd.h>
#include<netdb.h>
#include <string.h>
#define BUFFERSIZE 1024

struct sockaddr  getSockAddr(const char* hostname){
  struct hostent* host = gethostbyname(hostname);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(25);
  
  inet_pton(AF_INET, inet_ntoa(*(struct in_addr*)(host->h_addr_list[0])), &address.sin_addr);
  struct sockaddr* sock_address = (struct sockaddr*) &address;
  return *sock_address;
}

void send_command(int sock_fd, char* string){
  char* string_with_newline = (char*) malloc(sizeof(char)*(2 + strlen(string)));
  strcpy(string_with_newline, string);
  strcat(string_with_newline, "\n");
  printf("command: %s", string_with_newline);
  send(sock_fd, string_with_newline, strlen(string_with_newline), 0);
  free(string_with_newline);
  char* buffer = (char*)malloc(BUFFERSIZE + 1);
  ssize_t length;
  ssize_t i;
  ssize_t newline_character_counter = 0;
  char found_newline = 1;
  char c;
  while(1){
    length = recv(sock_fd, buffer, BUFFERSIZE, 0);
    buffer[length] = '\0';
    printf("%s", buffer);
    for(i = 0; i < length; i++){
      c = buffer[i];
      if(c == '\n' || c == '\r'){
	found_newline = 1;
	newline_character_counter = 0;
      }else if(found_newline){
	newline_character_counter += 1;
	if(newline_character_counter == 4){
	  if(c != '-'){
	    return;
	  }else{
	    found_newline = 0;
	    newline_character_counter = 0;
	  }
	}
      }
    }
  }
}

int main(){
  struct sockaddr socket_address = getSockAddr("smtp.uconn.edu");
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  connect(sock, &socket_address, sizeof(struct sockaddr));
  struct addrinfo hints;
  struct addrinfo* info;
  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  getaddrinfo(hostname, "http", &hints, &info);
  char* fqdn = info[0].ai_canonname;
  printf("fqdn: %s\n", fqdn);
  char* hello_command = (char *)malloc(sizeof(char)*(strlen(fqdn) + 7));
  char* buf = (char*) malloc(4096);
  sleep(2);
  recv(sock, buf, sizeof(buf), MSG_DONTWAIT);
  strcpy(hello_command, "ehlo ");
  strcat(hello_command, fqdn);

  send_command(sock, hello_command);
  sleep(2);
  send_command(sock, "mail from: jordan.force@uconn.edu");
  sleep(2);
  send_command(sock, "rcpt to: 8606349062@vtext.com");
  sleep(2);
  send_command(sock, "data");
  sleep(2);
  send_command(sock, "Hi Jordan, this is another test \r\n.\r");
  sleep(2);
  send_command(sock, "quit");
  freeaddrinfo(info);
}

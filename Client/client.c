#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFSIZE 100

int getFileLength(FILE* someFile);

int main(int argc, char *argv[])
{
  FILE *outBoundFile = NULL;
  int sd;
  struct sockaddr_in server_address; 
  char buffer[100];
  int portNumber;
  char serverIP[29];
  char fileName[20];
  int rc = 0;
  int readRC;
  
  if (argc < 3){
    printf ("usage is client <ipaddr> <port>\n");
    exit(1);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);

  portNumber = strtol(argv[2], NULL, 10);
  strcpy(serverIP, argv[1]);

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portNumber);
  server_address.sin_addr.s_addr = inet_addr(serverIP);

  if(connect(sd, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in)) < 0) {
    close(sd);
    perror("error connecting stream socket");
    exit(1);
  }

   while(1){

    bzero(buffer, BUFFSIZE);

    // Once connected, loop through file transfer procedure.
    printf("What is the name of the file you would like to send?: ");
    scanf("%s", fileName);
    if(strcmp(fileName, "DONE") == 0){
      return 0;
    }

    int fileNameSize = strlen(fileName);
    int converted_fileNameSize = htonl(fileNameSize);

    // 1
    rc = write(sd, &converted_fileNameSize, sizeof(converted_fileNameSize));
    bzero(buffer, BUFFSIZE);

    if(rc<0){
      printf("File name size issue");
      exit(1);
    }

    // 2
    rc =  write(sd, &fileName, fileNameSize);
    bzero(buffer, BUFFSIZE); 

    if(rc<0){
      printf("File name issue");
      exit(1);
    }

    // Open text file in pwd
    outBoundFile = fopen(fileName, "rb");

    int fileSize = getFileLength(outBoundFile);
    int converted_fileSize = htonl(fileSize);

    // 3
    rc = write(sd, &converted_fileSize, sizeof(converted_fileSize));
    bzero(buffer, BUFFSIZE);

    if(rc<0){
      printf("File size issue");
      exit(1);
    }

    int numOfBytes = 0;
    while(numOfBytes < fileSize){
      
      readRC = fread(buffer, 1, 100, outBoundFile); 
    
      // 4 
      rc = write(sd, buffer, readRC);
      if(rc <= 0){
	      perror("write");
	      exit(1);
      }
      numOfBytes += rc;
    }

    int totalBytes = 0;
    rc = read(sd, &totalBytes, sizeof(int));
    if(rc<=0){
      perror("read");
      exit(1);
    }
    
    int convertedTotalBytes = ntohl(totalBytes);

    printf("\n%d bytes written\n\n", convertedTotalBytes);

    bzero(buffer, 100);

    fclose(outBoundFile);

    // printf("What is the name of the file you would like to send?: ");
    // scanf("%s", fileName);   
  }

  close(sd);

  if (rc < 0)
    perror ("sendto");
  printf ("sent %d bytes\n", rc);

  
  return 0 ;
}

int getFileLength(FILE* someFile){
  fseek(someFile, 0, SEEK_END);
  int len = (int) ftell(someFile);
  fseek(someFile, 0, SEEK_SET);
  return len;
}

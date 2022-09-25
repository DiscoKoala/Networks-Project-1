#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

int getFileLength(FILE* someFile);

int main(int argc, char *argv[])
{
  FILE *outBoundFile = NULL;
  int sd;
  struct sockaddr_in server_address; 
  char buffer[100] = "hello world\n";
  int portNumber;
  char serverIP[29];
  char fileName[20];
  int rc = 0;
  
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
    
    printf("What is the name of the file you would like to send?: ");
    scanf("%s", fileName);

    if(strcmp(fileName, "DONE") == 0){
	    break;
    }
    // Once connected, loop through file transfer procedure.

    int fileNameSize = strlen(fileName);
    int converted_fileNameSize = htonl(fileNameSize);
    
    // 1
    rc = write(sd, &converted_fileNameSize, sizeof(converted_fileNameSize));

    if(rc<0){
      printf("File name size issue");
      exit(1);
    }

    // 2
    rc =  write(sd, &fileName, strlen(fileName));

    if(rc<0){
      printf("File name issue");
      exit(1);
    }

    // Open text file in pwd
    outBoundFile = fopen(fileName, "r");

    int fileSize = getFileLength(outBoundFile);
    int converted_fileSize = htonl(fileSize);

    // 3
    rc = write(sd, &converted_fileSize, sizeof(converted_fileSize));

    if(rc<0){
      printf("File size issue");
      exit(1);
    }

    int numOfBytes = 0;
    while(numOfBytes <= fileSize){
      
      fread(buffer, 1, 100, outBoundFile);     
      // 4 
      rc = write(sd, buffer, sizeof(buffer));
      if(rc <= 0){
	      perror("write");
	      exit(1);
      }
      numOfBytes += rc;
    }

    int totalBytes = 0;
    rc = read(sd, &totalBytes, sizeof(int));

    int convertedTotalBytes = ntohl(totalBytes);

    printf("\n%d bytes written\n\n", convertedTotalBytes);

    bzero(buffer, 100);
    rc = 0;

    // printf("What is the name of the file you would like to send?: ");
    // scanf("%s", fileName);
   
  }

  if (rc < 0)
    perror ("sendto");
  printf ("sent %d bytes\n", rc);

  fclose(outBoundFile);
  
  return 0 ;
}

int getFileLength(FILE* someFile){
  fseek(someFile, 0, SEEK_END);
  int len = (int) ftell(someFile);
  fseek(someFile, 0, SEEK_SET);
  return len;
}

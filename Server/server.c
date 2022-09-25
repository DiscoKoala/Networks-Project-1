                       #include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFSIZE 1000

void fileTransfer(int connected_sd);

int main(int argc, char *argv[]){
  
  int sd; /* socket descriptor */
  int connected_sd; /* socket descriptor */
  int rc; /* return code from recvfrom */
  struct sockaddr_in server_address;
  struct sockaddr_in from_address;
  //int flags = 0;
  socklen_t fromLength;
  int portNumber;

  if (argc < 2){
    printf ("Usage is: server <portNumber>\n");
    exit (1);
  }


  portNumber = atoi(argv[1]);
  
  sd = socket (AF_INET, SOCK_STREAM, 0);
  
  fromLength = sizeof(struct sockaddr_in);
  
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(portNumber);
  server_address.sin_addr.s_addr = INADDR_ANY;
  

  rc = bind (sd, (struct sockaddr*)&server_address, sizeof(server_address));
  if(rc<0){
    printf("Bind error\n");
    exit(1);
  }

  while(1){
    printf("%s", "Listening...\n\n");
    listen (sd, 1);
    connected_sd = accept (sd, (struct sockaddr*) &from_address, &fromLength);
    fileTransfer(connected_sd);
    // printf("%s", "Hello");
    close(connected_sd);
  // printf("%s", "Hello");
  }
  return 0;
}

void fileTransfer(int connected_sd){

  char buffer[BUFFSIZE];
  int fileSize = 0;
  int fileNameSize = 0;
  char fileName[20];

  bzero(buffer, BUFFSIZE);

  for(;;){

    // 1
    int rc = read(connected_sd, &fileNameSize, sizeof(int));
    printf("Size of file name before conversion: %d\n", rc);
    if(rc == 0){
      return;
    }
    
    fileNameSize = ntohl(fileNameSize);
    printf("Size of file name after conversion: %d\n", fileNameSize);
    
    if(rc<0){
      printf("Error receiving file name size\n");
      exit(1);
    }

    int numOfBytes = 0;
    char *ptr = buffer;
    while(numOfBytes < fileNameSize){
      // 2
      rc = read(connected_sd, ptr, fileNameSize - numOfBytes);
      printf("in loop, read %d bytes\n", rc);
      if(rc<=0){
        perror("read");
        exit(1);
      }
      numOfBytes += rc;
      ptr += rc;
    }
    strcpy(fileName, buffer);
    printf("Received file name from client: %s\n", buffer);
    printf("%s\n" , fileName);
    // bzero(buffer, BUFFSIZE);
    
    // 3
    read(connected_sd, &fileSize, sizeof(int));
    printf("Size of received file before conversion: %d\n", fileSize);

    fileSize = ntohl(fileSize);
    printf("Size of file after conversion: %d\n", fileSize);

    FILE *inBoundFile = fopen(fileName, "w");
    
    int totalBytes = 0;
    int bytesWritten = 0;
    numOfBytes = 0;
    
    while(numOfBytes < fileSize){

      // 4
      rc = read(connected_sd, buffer, fileSize - numOfBytes);
      bytesWritten = fwrite(buffer, 1, rc, inBoundFile);
      
      printf("in loop, Read %d bytes\n", rc);
      if(rc <= 0){
        perror("read");
        exit(1);
      }   

      numOfBytes += rc;
      totalBytes += rc;
    }

    fclose(inBoundFile);
    // bzero(buffer, BUFFSIZE);
    int convertedTotalBytes = htonl(totalBytes);
    write(connected_sd, &convertedTotalBytes, sizeof(convertedTotalBytes));
  }
}

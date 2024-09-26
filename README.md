# TCP-based Client-Server Application for File Transfer

The two programs form a TCP-based client-server system where the client sends files to the server, which stores them and sends an acknowledgment of the received data.

## Description

Together, the client program and the server program create a client-server file transfer system using TCP. The client connects to the server by specifying the IP address and port number, then sends a file by transmitting the file name, it's size, and the content in chunks. The server receives the file, writes it to disk, and sends an acknowledgment back to the client, confirming the total number of bytes successfully received. The client can send multiple files in one session, and the server continuously listens for new connections, allowing multiple file transfers. This system ensures reliable transmission of files over a network.

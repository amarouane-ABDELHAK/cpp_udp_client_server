/*
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>

#define BUFSIZE 1024

using namespace std;

/*
 * error - wrapper for perror
 */


void error(string msg) {
    perror(msg.c_str());
    exit(0);
}

void tcpClient(int portNumber, const char *hostname) {
    //Define the socket address
    int clientSocket, ret;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    //Clear the buffer
    bzero(buffer, sizeof(buffer));
    //Select the type of the socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    //If the socket failed
    if (clientSocket < 0) {
        printf("Error creating client socket.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");
    //Clear server Address struct before initilizing it
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNumber);
    //Assign it to localhost
    serverAddr.sin_addr.s_addr = inet_addr(hostname);
    //Connect the client to the server
    ret = connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        printf("connect function error.\n");
        exit(1);
    }

    while (1) {
        //Clear the buffer
        bzero(buffer, sizeof(buffer));
        if (recv(clientSocket, buffer, BUFSIZE, 0) < 0) {
            printf("Can't receive the message\n");
        } else {
            cout << "\n========================== Message received ==========================\n\n";
            cout << "Message:\t" << buffer << endl;
            cout << "======================================================================\n";

        }
        //Clear the buffer
        bzero(buffer, sizeof(buffer));
        //Wait for the user input
        printf("Client > ");
        string str = "";
        getline(cin, str);
        //Copy the input to the message
        strcpy(buffer, str.c_str());
        send(clientSocket, buffer, strlen(buffer), 0);
        //Block while waiting for a response
        cout << "Waiting response...." << endl;
        //Make the client disconnect bu entering :quit
        if (strcmp(buffer, ":quit") == 0) {
            close(clientSocket);
            printf("You are disconnected from the server.\n");
            exit(0);
        }


    }
}

void getPortHostName(char *serverMessage) {
    /*
     * Function to extract the IP and the port and connect to the server
     * Using TCP protocol
     * Rule: Extract the port and the host IP from the buffer
     * Input buffer: (array of char) the received message from a client
     * Return (void)
     */
    string portHost[3];
    string message(serverMessage, BUFSIZE);
    int nextToken = 0;
    //Add a colon to the end of the message to stop the wile loop
    message.append(":");

    for (int i = 0; i < message.length() - 1; i++) {
        string tokenValue("", 0);
        while (message[i] != ':') {
            tokenValue.append(string(1, message[i]));
            i++;
        }
        portHost[nextToken] = tokenValue;
        nextToken++;
    }

    sleep(1);

    tcpClient(atoi(portHost[2].c_str()), portHost[1].c_str());


}


void newUDPClient(int portNumber) {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFSIZE];

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, '\0', sizeof(serverAddr));
    socklen_t serverlen;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNumber);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bzero(buffer, sizeof(buffer));
    strcpy(buffer, "Hello Store\n");
    sendto(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //printf("[+]Data Send: %s", buffer);
    ssize_t n = recvfrom(sockfd, buffer, BUFSIZE, 0, (struct sockaddr *) &serverAddr, &serverlen);

    //cout<<"Client received datagram from "<<inet_ntoa(serverAddr.sin_addr) << endl;
    printf("Message %s \n",
            buffer);
    getPortHostName(buffer);
    if (n < 0)
        exit(1);
}


int main(int argc, char **argv) {
    /* check command line arguments */
    int portNumber = 0;
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    portNumber = atoi(argv[1]);
    //udpClient(portNumber, hostname);
    newUDPClient(portNumber);

    return 0;
}
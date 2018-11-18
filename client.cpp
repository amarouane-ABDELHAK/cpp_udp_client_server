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

void tcpClient(int portno, const char* hostname) {
    //Define the socket address
    int clientSocket, ret;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    //Clear the buffer
    bzero(buffer, sizeof(buffer));
    //Select the type of the socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    //If the socket failed
    if(clientSocket < 0){
        printf("Error creating client socket.\n");
        exit(1);
    }
    printf("Client Socket is created.\n");
    //Clear server Address struct before initilizing it
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portno);
    //Assign it to localhost
    serverAddr.sin_addr.s_addr = inet_addr(hostname);
    //Connect the client to the server
    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0){
        printf("connect function error.\n");
        exit(1);
    }

    while(1){
        //Clear the buffer
        bzero(buffer, sizeof(buffer));
        if(recv(clientSocket, buffer, 1024, 0) < 0){
            printf("Can't recieve the message\n");
        }else{
            cout<<"\n========================== Message received ==========================\n\n";
            cout<<"Message:\t"<<buffer<<endl;
            cout<<"======================================================================\n";

        }
        //Clear the buffer
        bzero(buffer, sizeof(buffer));
        //Wait for the user input
        printf("Client > ");
        string str="";
        getline(cin,str);
        //Copy the input to the message
        strcpy(buffer, str.c_str());
        send(clientSocket, buffer, strlen(buffer), 0);
        //Block while waiting for a response
        cout<<"Waiting response...."<<endl;
        //Make the client disconnect bu entering :quit
        if(strcmp(buffer, ":quit") == 0){
            close(clientSocket);
            printf("You are disconnected from the server.\n");
            exit(0);
        }




    }
}

void getPortHostName(char* serverMessage) {
    /*
 * Rule: Extract the client ID from the buffer
 * Input buffer: (array of char) the received message from a client
 * Return (int) : The extracted client ID
 */
    string portHost[3];
    string message(serverMessage, BUFSIZE);
    int nextToken = 0;
    message.append(":");
    cout<<"before "<<message<<endl;
    for(int i = 0 ; i< message.length() -1; i++) {
        cout<<"Token " <<nextToken<<endl;
        string tokenValue("", 0);
        while(message[i] != ':') {
            tokenValue.append(string(1, message[i]));

            i++;
        }
        portHost[nextToken] = tokenValue;
        nextToken ++;
    }
    cout<<"inside "<<message<<endl;
    cout<<"class "<<portHost[0]<<endl;
    cout<<"host "<<portHost[1]<<endl;
    cout<<"host "<<portHost[2]<<endl;
    sleep(2);

    tcpClient(atoi(portHost[2].c_str()), portHost[1].c_str());





    //return portHost;

}


void newUDPClient(int portNumber) {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, '\0', sizeof(serverAddr));
    socklen_t serverlen;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNumber);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);


    strcpy(buffer, "Hello Server\n");
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    //printf("[+]Data Send: %s", buffer);
    ssize_t n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *) &serverAddr, &serverlen);

    cout<<"Client received datagram from "<<inet_ntoa(serverAddr.sin_addr) << endl;
    printf("Message %s \n",
            buffer);
    getPortHostName(buffer);
    if (n < 0)
        exit(1);
}

void udpClient(int portno, char* hostname) {
    int sockfd;
    ssize_t n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    bool keepListening = true;

    char buf[BUFSIZE];



    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
//    server = gethostbyaddr((const char *) &serveraddr.sin_addr.s_addr,
//                sizeof(serveraddr.sin_addr.s_addr), AF_INET);
    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
            (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    while (keepListening) {
        bzero(buf, BUFSIZE);
        //printf("\nPlease press enter");
        //fgets(buf2, BUFSIZE, stdin);


        /* send the message to the server */
        serverlen = sizeof(serveraddr);

        n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &serveraddr, serverlen);
        if (n < 0)
            error("ERROR in sendto");

        /* print the server's reply */
        n = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *) &serveraddr, &serverlen);
        struct hostent * hostp = gethostbyaddr((const char *) &serveraddr.sin_addr.s_addr,
                sizeof(serveraddr.sin_addr.s_addr), AF_INET);
        printf("server received datagram from %s \n",
               hostp->h_name);
        if (n < 0)
            error("ERROR in recvfrom");
        sleep(1);
        cout<<buf<<endl;
        //getPortHostName(buf);
        bzero(buf, BUFSIZE);
        //cout<<"Port "<<getPortHostName(buf)[0]<< "Host "<< getPortHostName(buf)[1]<<endl;
        //char host[13] = "127.0.0.1";
        //tcpClient(8888, host);
        //break;

    }
}

int main(int argc, char **argv) {
    /* check command line arguments */
    int portno;
    char *hostname;
    if (argc != 2) {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[1]);
    //udpClient(portno, hostname);
    newUDPClient(portno);

    return 0;
}
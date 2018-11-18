/*
 * udpserver.c - A simple UDP echo server
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <sstream>

#define BUFSIZE 1024
//Predefine a port number
#define PORT 8888

//Predefine the maximum clients allowed to connect at the same time
#define MAX_CLIENTS 10

//Predefine the bufer size
#define BUFFER_SIZE 1025
using namespace std;

/*
 * error - wrapper for perror
 */
void error(string msg) {
    perror(msg.c_str());
    exit(1);
}

void showConnecetdClients(int *client_socket) {
    /*
     * Rule: Show the ID of connected clients
     * input client_socket (array of int) define the array of the client ID
     * return void
     */
    //
    cout << " Connected clients are ";
    for (int i = 0; i < MAX_CLIENTS; i++) {
        //if position is not empty show the client as a connected
        if (client_socket[i] != 0) {
            cout << "[" << client_socket[i] << "] ";
        }

    }
    cout << endl;
}

int getClientID(char *buffer) {
    /*
     * Rule: Extract the client ID from the buffer
     * Input buffer: (array of char) the received message from a client
     * Return (int) : The extracted client ID
     */
    string message(buffer, BUFSIZE);
    char delimiter[] = ":";
    string token = message.substr(0, message.find(delimiter)); // token is "scott"

    return atoi(token.c_str());


}


void udpProtocol(int portNumber) {
    int sockfd; /* socket */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    string tcpHolderMessage = "";
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    ssize_t n; /* message byte size */

    /*
     * check command line arguments
     */


    /*
     * socket: create the parent socket
     */

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
            (const void *) &optval, sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) portNumber);



    /*
     * bind: associate the parent socket with a port
     */
    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        error("ERROR on binding");

    /*
     * main loop: wait for a datagram, then echo it
     */
    clientlen = sizeof(clientaddr);
    while (1) {

        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(buf, BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0,
                (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0)
            error("ERROR in recvfrom");

        printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
        cout<<"clientaddr / "<<inet_ntoa(clientaddr.sin_addr) << endl;
        cout<<"Serverad / "<<inet_ntoa(serveraddr.sin_addr) << endl;
        /*
         * gethostbyaddr: determine who sent the datagram
         */
//        hostp = gethostbyaddr((const char *) &clientaddr.sin_addr.s_addr,
////                sizeof(clientaddr.sin_addr.s_addr), AF_INET);
//        if (hostp == NULL)
//            error("ERROR on gethostbyaddr");
//        hostaddrp = inet_ntoa(clientaddr.sin_addr);
//        if (hostaddrp == NULL)
//            error("ERROR on inet_ntoa\n");
////        printf("server received datagram from %s (%s)\n",
////                hostp->h_name, hostaddrp);
////        printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
//
//        /*
//         * sendto: echo the input back to the client
//         */
        tcpHolderMessage = "CS571:192.168.0.10:8888";

        n = sendto(sockfd, tcpHolderMessage.c_str(), BUFSIZE, 0,
                (struct sockaddr *) &clientaddr, clientlen);
        if (n < 0)
            error("ERROR in sendto");
    }
}

void tcpProtocol(int portNumber) {
    int opt = 1;
    int server_socket, addrlen, new_socket, client_socket[MAX_CLIENTS], activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[BUFFER_SIZE];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;
    //I used ostringstream to display messages without dealing with the (+) operator between ints, chars and strings
    ostringstream oss;

    //Client ID 0 defines the server
    int clientID = 0;


    //Initialise all client_socket[] to 0 (0 means not connected)
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    //create a server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed to create");
        exit(EXIT_FAILURE);
    }

    //Set the server socket to allow multiple clients

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
            sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //Define the type of the socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //Bind the socket to localhost port 8888
    if (bind(server_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket is open on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the server socket
    if (listen(server_socket, 3) < 0) {
        perror("listen function failed!");
        exit(EXIT_FAILURE);
    }
    //Accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    while (true) {
        //Clear the socket set
        FD_ZERO(&readfds);

        //Add server socket to set
        //FD_SET: Sets the bit for the file descriptor server_socket in the file descriptor set readfds.
        FD_SET(server_socket, &readfds);
        //Initilize the highest file descriptor
        max_sd = server_socket;
        //Add client sockets to set
        for (i = 0; i < MAX_CLIENTS; i++) {
            //Socket descriptor
            sd = client_socket[i];

            //If valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //We need the highest file descriptor number for the select function
            if (sd > max_sd)
                max_sd = sd;
        }
        //Wait for an activity on one of the sockets
        // It will wait indefinitely because we set timeout to NULL
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select function error");
        }

        //If something happened on the server socket ,
        //then there is a request
        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket,
                    (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                perror("accept function error");
                exit(EXIT_FAILURE);
            }
            //Inform the client that the connection is successful
            oss.str("");
            oss << "Welcome Client [" << new_socket << "] you are successfully connected\n\0";
            char message[BUFFER_SIZE];
            //Clear the message
            bzero(message, sizeof(message));
            //Copy the ostream to the message
            strcpy(message, oss.str().c_str());
            //Send the greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send function error");
            }
            //Clear the message
            bzero(message, sizeof(message));
            //Show the client ID as connected
            cout << "======== Client [" << new_socket << "] is connecetd! ======" << endl;
            // There is a new client so add it the array of sockets

            for (i = 0; i < MAX_CLIENTS; i++) {
                //If you reach the first empty position,
                //then add it to the array and break the loop
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    break;
                }

            }
            //Display the connected clients
            showConnecetdClients(client_socket);
        }
        //else there is a message from the client
        for (i = 0; i < MAX_CLIENTS; i++) {
            //sd is the client socket so I will not have to client_socket[i]
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                //Check if the client disconnected

                if ((read(sd, buffer, 1024)) == 0) {
                    //A client is disconnected display his ID
                    getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
                    cout << "Client [" << sd << "] disconnected!" << endl;
                    //Close the socket and mark its position as 0 in the list
                    close(sd);
                    client_socket[i] = 0;
                }

                    //Else if it was a message from the client
                else {
                    //Display the message and the client information on the server
                    cout << "\n========================== Message received ==========================\n";
                    cout << "Client ID:\t" << sd << endl;
                    cout << "Client IP:\t" << inet_ntoa(address.sin_addr) << endl;
                    cout << "Message:\t" << buffer << endl;
                    cout << "======================================================================\n";

                    //If the client wants to communicate with another client,
                    //get the client ID and send the message to the targeted client
                    clientID = getClientID(buffer);
                    string newbuffer(buffer);
                    oss.str("");
                    oss << "From " << sd << " " << newbuffer.replace(0, 2, ":") << endl;
                    //Check if the client ID is greater than 0
                    if (clientID > 0) {
                        send(clientID, oss.str().c_str(), strlen(oss.str().c_str()), 0);
                        bzero(buffer, sizeof(buffer));
                    }
                    //Clear the buffer
                    bzero(buffer, sizeof(buffer));

                }
                //Use a child process to wait for the user input,
                //so the connection will not be blocked

                if (fork()== 0) {
                    bzero(buffer, sizeof(buffer));
                    string str = "";
                    // To send the message to a particular client you need to write,
                    // clientID: <your message> failing to do so the message will be send to the server
                    //Get the input from the user
                    getline(cin, str);
                    //Copy the user input to the buffer
                    strcpy(buffer, str.c_str());
                    //Send to the designated client
                    send(getClientID(buffer), buffer, strlen(buffer), 0);
                    //Clear the buffer
                    bzero(buffer, sizeof(buffer));
                    //Kill the process
                    exit(0);

                }
            }

        }
    }
}


int main(int argc, char **argv) {
    int portNumber;

    cout<<"running..."<<endl;
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portNumber = atoi(argv[1]);
    pid_t pid = fork();
    cout<<"PID = "<<pid<<endl;
    if (pid== 0) {
        cout<<"running UDP"<<endl;
        udpProtocol(portNumber);

    }
    else {
        cout<<"running TCP"<<endl;
        tcpProtocol(8882);
    }
}
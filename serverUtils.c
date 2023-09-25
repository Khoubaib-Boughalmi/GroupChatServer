#include "GCS.h"

t_acceptSocket *acceptSocket(int serverSocketFd) {
    t_acceptSocket *localAcceptedSocket = malloc(sizeof(t_acceptSocket));
    localAcceptedSocket->clientAddress = NULL;
    socklen_t clientAddressLen = sizeof(*localAcceptedSocket->clientAddress);
    localAcceptedSocket->clientSocketFd = accept(serverSocketFd, (struct sockaddr *)localAcceptedSocket->clientAddress, &clientAddressLen);

    if (localAcceptedSocket->clientSocketFd < 0) {
        localAcceptedSocket->error = localAcceptedSocket->clientSocketFd;
        localAcceptedSocket->fullyAccepted = false;
        perror("accept error");
        free(localAcceptedSocket);
        return (NULL);
    }
    printf("accept success, Client Fd: %d\n", localAcceptedSocket->clientSocketFd);
    localAcceptedSocket->error = 0;
    localAcceptedSocket->fullyAccepted = true;
    return  (localAcceptedSocket);
}

void receiveIncommingRequestAndRespond (int clientSocketFd) {
    char httpReq[8192] = "";
    char *getResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>GET response</h1></body></html>";
    char *postResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>POST response</h1></body></html>";
    while (1)
    {
        memset(httpReq, 0, sizeof(httpReq));
        int bytesRecv = recv(clientSocketFd, httpReq, sizeof(httpReq), 0);
        if(!bytesRecv || bytesRecv < 0) {
            if(bytesRecv < 0)
                perror("recv error");
            else
            {
                close(clientSocketFd);
                printf("Client closed connection\n");
            }
            break;
        }
        printf("Client's Request: %s", httpReq);
        char *getReq = strstr(httpReq, "GET");
        char *postReq = strstr(httpReq, "POST");
        int bytesSent = 0;
        if(getReq != NULL) {
            bytesSent = send(clientSocketFd, getResponse, strlen(getResponse), 0);
        }
        else if(postReq != NULL) {
            bytesSent = send(clientSocketFd, postResponse, strlen(postResponse), 0);
        }
        else {
            printf("Unknown Request\n");
            bytesSent = -1;
        }
        if(bytesSent < 0) {
            perror("send error");
            break;
        }
        printf("send success\n");
    }
}

void *startAcceptingIncomingConnections(int serverSocketFd) {
    while (1)
    {
        acceptedSocket = acceptSocket(serverSocketFd);
        if(!acceptedSocket) {
            shutdown(serverSocketFd, SHUT_RDWR);
            perror("accept error");
            return (NULL);   
        }
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, receiveAndRespond, &(acceptedSocket->clientSocketFd));
    }
    return (NULL);   
}

void *receiveAndRespond(void *clientSocketFd_arg) {
  
    //receiving and responding
    int clientSocketFd = *(int *)clientSocketFd_arg;
    receiveIncommingRequestAndRespond (clientSocketFd);
    close(clientSocketFd);   
    return (NULL);
}
#include "packet.h"




int main (int argc, char const *argv[]){

    int s=0;
    struct sockaddr_in client;
    struct sockaddr_in server;
    s= creationSocket(s);

    client.sin_family = AF_INET;
    client.sin_port = htons(5000);
    inet_pton(AF_INET,argv[1] ,&client.sin_addr);
    int x =etablissementConnexionServer(s,server,client);

    printf("%d \n", x);
return 0;

}
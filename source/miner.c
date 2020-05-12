#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> 

#include "miner.h"

Miner_t * minerInit(){
    
    Miner_t *miner;
    size_t size = sizeof(Miner_t);

    miner = (Miner_t *)malloc(sizeof(Miner_t));
    CHECK(miner != NULL);

    miner->nonceSection = 0;
    miner->bloque = blockInit();

    return miner;
}

int minerConnect(Miner_t *miner, char * serverName, int portno){

    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd, ret;
    Packet_t packet;

    // Tareas de conexión del minero
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sockfd != 0);

    server = gethostbyname(serverName);
    CHECK(server != NULL);

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], 
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

    // host to net short
    serv_addr.sin_port = htons(portno);
    ret = connect(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr));
    CHECK(ret == 0);

    // Envío de paquete de saludo (por ahora, el paquete vive acá adentro)
    // le cargué el tipo así que ya debería saber como llenarlo
    packet.type = connectPool;
    mFillPacket(miner, &packet);
    
    sendPacket(sockfd, packet);

    return sockfd;
}

void mFillPacket(Miner_t *miner, Packet_t *packet){

    PacketType_t type = packet->type;

    switch (type)
    {
    case connectPool:

        packet->argswMiner.id = miner->id;
        break;
    
    case reqBlock:
        /* recibir y alocar bloque actual*/
        break;  

    case reqNonce:
        /* comenzar worker threads a minar la gilada*/
        break;
    
    case submitBlock:
        /* chequear el comportamiento y estado de lo*/
        break;

    case disconnectPool:
        /* Registro salida del minero y envío despedida con stats*/
        break;
    
    default:
        /* mensaje incorrecto */ 
        break;
    }



}

void mProcessPacket(Packet_t * packet){

    PacketType_t type = packet->type;

    switch (type)
    {
    case welcomeMiner:
        /* printear información del pool al cual me suscribí*/
        break;
    
    case sendBlock:
        /* recibir y alocar bloque actual*/
        break;  

    case sendNonce:
        /* comenzar worker threads a minar la gilada*/
        break;
    
    case checkBlock:
        /* printear info acerca del check del bloque y celebrar*/
        break;
    
    case floodStop:
        /* parar todos los threads del minero y esperar por nuevos headers*/
        break;

    case sendReward:
        /* guardar la platita en la billetera y seguir adelante con la vida de minero */ 
        break;    

    case farewellMiner:
        /* terminar procesos del lado del minero*/
        break;
    
    default:
        /* mensaje incorrecto */ 
        break;
    }

}

void minerDestroy(Miner_t *miner){

    blockDestroy(miner->bloque);

    free(miner);
}
#include "pool.h"
#include "common.h"
#include "database.h"

Pool_t * poolInit(){
    
    Pool_t *pool;
    size_t size = sizeof(Pool_t);

    pool = (Pool_t *)malloc(sizeof(Pool_t));
    CHECK(pool != NULL);

    pool->minerDb = DatabaseCreate();
    pool->minerQty = 0;

    return pool;
}

void poolDestroy(Pool_t *pool){

    DatabaseDestroy(pool->minerDb);
    free(pool);
}

// Proceso mensajes enviados por el minero
void pProcessPacket(Pool_t* pool, Packet_t *packet){

    PacketType_t type = packet->type;

    switch (type)
    {
    case connectPool:

        pool -> minerQty++;
        queuePut(pool -> minerDb);
        printf("Se ha conectado el minero", packet->arg);
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

void pFillPacket(Pool_t* pool, Packet_t * packet){

    PacketType_t type = packet->type;

    switch (type)
    {
    case welcomeMiner:

        break;
    
    case sendBlock:

        break;  

    case sendNonce:
        break;
    
    case checkBlock:
        break;
    
    case floodStop:
        break;

    case sendReward:
        break;    

    case farewellMiner:
        break;
    
    default:
        break;
    }
}

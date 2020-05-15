#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include <stdlib.h>
#define MAX_SIZE_MSG 1024

typedef enum
{   
    // miner2pool
    connectPool,
    disconnectPool,
    sendNonce, 

    // pool2miner
    welcomeMiner,
    reqNonce, 
    farewellMiner

} PacketType_t;

typedef struct __attribute__((__packed__)) Packet{
    
    size_t sz8;
    PacketType_t type;
    
    union arguments
    {   
        // miner2pool
        struct  connectPool_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_connectPool;
        
        struct  disconnectPool_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_disconnectPool;

        // pool2miner
        struct  welcomeMiner_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_welcomeMiner;
        
        struct  farewellMiner_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_farewellMiner;
    } args;

} Packet_t;




#endif //PACKET_H_
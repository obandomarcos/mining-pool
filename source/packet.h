#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include <stdlib.h>
#define MAX_SIZE_MSG 1024

typedef enum
{   
    // miner2pool
    connectPool,
    reqNonce,
    reqBlock,
    submitNonce,
    disconnectPool,

    // pool2miner
    welcomeMiner,
    sendNonce, 
    sendBlock,
    discardBlock,
    failureBlock,
    successBlock,
    floodStop,
    sendReward,
    farewellMiner

} PacketType_t;


// timestamp estaria bueno tambien

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
        
        struct  reqNonce_
        {
            int span;  // es una manera de ver la capacidad del minero

        } args_reqNonce;
        
        struct reqBlock_
        {

        } args_reqBlock;

        struct  submitNonce_
        {   
            char mensaje[MAX_SIZE_MSG];
            int32_t goldNonce;

        } args_submitNonce;

        struct  disconnectPool_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_disconnectPool;


        // pool2miner
        struct  welcomeMiner_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_welcomeMiner;

        struct  sendNonce_
        {
            int nonce;
            int section;

        } args_sendNonce;

        struct  sendBlock_
        {
            int block;

        } args_sendBlock;

        struct  sendReward_
        {   
            uint32_t norm;
            uint32_t reward;

        } args_sendReward;
        
        struct  floodStop_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_floodStop;
        
        struct  farewellMiner_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_farewellMiner;
    } args;

} Packet_t;




#endif //PACKET_H_
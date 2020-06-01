#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>
#include <stdlib.h>
#define MAX_SIZE_MSG 1024
#define BLOCKSIZE 15


// máscaras de checkeo
typedef enum
{   
    // casting
    multicastMask = 0x2000,

    // entity
    entityMask = 0x1000

} MaskType_t;

// type structure
// DEXXX where
// D : deliverance type, Multicast (1) or Unicast (0)
// E : entity type of connection : miner2pool (0) or pool2miner (1)
// X : message identifier

typedef enum
{   
    // miner2pool
    connectPool = 0x1 ,
    reqNonce = 0x2,
    reqBlock = 0x4,
    submitNonce = 0x8,
    disconnectPool = 0x10,

    // pool2miner
    // unicast
    welcomeMiner = 0x20| entityMask,
    sendNonce = 0x40| entityMask, 
    sendBlock = 0x80| entityMask,
    discardBlock = 0x11| entityMask,
    failureBlock = 0x21| entityMask,
    successBlock = 0x41| entityMask,
    farewellMiner = 0x81| entityMask,
    // multicast
    floodStop = 0x22 | entityMask | multicastMask ,
    sendReward = 0x42 | entityMask | multicastMask,
    // caso particular de farewell
    shutdownPool = 0x81 | entityMask | multicastMask,

    // uso el mismo paquete para avisar que entraron giladas por stdin
    stdInput = 0x10000, 
    timeoutVal = 0x20000,
    // enviado
    idle = 0x0
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
            int32_t nonce;
            int32_t section;

        } args_sendNonce;

        struct  sendBlock_
        {
            char block[BLOCKSIZE];
            int  difficulty;

        } args_sendBlock;

        struct  successBlock_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_successBlock;

        struct  sendReward_
        {   
            // uint32_t norm;
            float reward;
            char mensaje[MAX_SIZE_MSG];

        } args_sendReward;
        
        struct  floodStop_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_floodStop;
        
        struct  shutdownPool_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_shutdownPool;

        struct  farewellMiner_
        {
            char mensaje[MAX_SIZE_MSG];

        } args_farewellMiner;

        // shared
        struct  stdIn_
        {
            int opt;

        } args_stdIn;
    } args;

} Packet_t;

#endif //PACKET_H_
#ifndef POOL_H_
#define POOL_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <stdbool.h>

#include "packet.h"
#include "common.h"

#define BUFLEN 512
#define NPACK 10
#define PORT 9930
#define MCAST_GROUP "225.0.0.37"
#define MCAST_PORT 12345
#define BLOCKSIZE 15

typedef struct Pool
{
    int poolsock;
    struct sockaddr_in pool_addr;

    int miners;
    // esto en un futuro será una cola de mineros
    // futuro incierto por cierto
    // ahre
    struct sockaddr_in miner_addr;      // minero actual? podría ser
    struct sockaddr_in mcast_addr;

    // atributos del pool
    bool active;

    int minerDifficulty;
    int poolDifficulty;
    
    //esto debe ser un bloque
    char block[BLOCKSIZE];
    
    // Valores para recompensa
    float blockValue;
    float gainPool;

    // billetera del pool
    float poolWallet;

    // para los mineros
    int32_t nonce;
    int32_t section;

} Pool_t;

// define la dificultad del bloque
typedef enum
{
    poolDif,
    minDif,
    noDif

} BlockDiffType_t;

typedef enum 
{
    platucha = 1,
    closePool = 0,
    currMiners = 2,

} PoolInputType_t;

Pool_t * poolInit();
void poolListen(Pool_t *pool);
void poolDestroy(Pool_t * pool);
void poolSendPacket(Pool_t *pool, PacketType_t pType);
void poolProcessPacket(Pool_t *pool);
void poolVerifyBlock(Pool_t * pool, int32_t goldNonce);
void poolReloadBlock(Pool_t *pool);
void poolExecute(Pool_t * pool, PoolInputType_t inType);
// como recibo paquetes o coordino con stdin
Packet_t * packetRetrieval(Pool_t *pool);
// funciones útiles
BlockDiffType_t hashCheckBlock(Pool_t *pool, int32_t nonce);
float poolCalculateReward(Pool_t *pool);

#endif //PACKET_H_
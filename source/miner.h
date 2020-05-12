#ifndef MINER_H_
#define MINER_H_

#include "block.h"

typedef unsigned int MinerId;

/*  
*    PacketType_t : Defines receiver behaviour
*
*    server to miner
*        welcomeMiner : Saludo de bienvenida a minero i
*        sendBlock : Envío un bloque completo al minero junto con una sección del nonce a minar (primera comunicación)
*        sendNonce : Envío una nueva sección de nonce a minar
*        checkBlock : Envío una confimación de que el bloque minado tiene una dificultad mayor a la seteada por el pool
*        floodStop : Envío un stop a todos los mineros de que hay que cortar de minar hasta que llegue un nuevo bloque (significa que alguien le pegó a un bloque con dificultad D) 
*        sendReward : Recompenso al minero por su arduo trabajo
*        farewellMiner : Saludo de despedida a minero i - Le puedo pasar algunos stats como para hacer un pat in the back

*    miner to server
*        connectPool : Conecto al pool de minería el minero i. Debo enviar toda mi información para que me identifiquen en el pool
*        reqBlock : Requiero un bloque para comenzar mis actividades 
*        reqNonce : Requiero una nueva sección de nonce a minar (fixed-region nonce)
*        submitBlock : Submiteo un bloque con el nonce con una dificultad igual o mayor a d
*        disconnectPool:  Desconecto del pool al minero i. Debo enviar toda mi información para que me identifiquen en el pool
*/

typedef enum
{
    welcomeMiner,       
    sendBlock,          
    sendNonce,          
    checkBlock,         
    floodStop,          
    sendReward,        
    farewellMiner,      

    connectPool,        
    reqBlock,           
    reqNonce,           
    submitBlock,        
    disconnectPool     
} PacketType_t;

typedef struct __attribute__((__packed__))
{
    uint16_t sz8;
    PacketType_t type;
    
    union 
    {

        struct 
        {
            MinerId id;

        } argswMiner;
    
        struct 
        {
            

        } args_sBlock;
        
        struct 
        {
            
        } args_sNonce;

        struct 
        {
            
        } args_cBlock;

        struct 
        {
            
        } args_FStop;

        struct 
        {
            
        } args_sReward;

        struct 
        {
            
        } args_fMiner;

        struct 
        {
            
        } args_cPool;

        struct 
        {
            
        } args_rBlock;

        struct 
        {
            
        } args_rNonce;

        struct 
        {
            
        } args_sBlock;

        struct
        {

        } args_dPool;

    } args;
} Packet_t;



typedef struct {

    MinerId id;
    int nonceSection;
    Block_t *bloque;

} Miner_t;

void minerProcessMessage(Packet_t packet);

Miner_t * minerInit();

int minerConnect(Miner_t *minero, char * serverName, int portno);

void minerDestroy(Miner_t *miner);

#endif // MINER_H_
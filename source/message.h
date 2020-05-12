#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <stdint.h>

#define MAX_WALLET_TRANSACTIONS 20
#define MAX_LEN_MSG

/*  
*    MessageType_t : Defines receiver behaviour
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
} MessageType_t;

struct __attribute__((__packed__)) Package 
{
    uint16_t sz8;
    MessageType_t type;
    
    union 
    {

        struct 
        {
            
            char welcomeMsg[MAX_LEN_MSG];


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
};

#endif
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_WALLET_TRANSACTIONS

typedef uint32_t hashRate;      //tasa de hash
typedef unsigned int uint;

enum Types {
    cmd1,
    cmd2,
    ...

};

struct Package {
    u16 sz8;
    u16 type;
    union {

        struct {
        ...
        } argsCmd1;
    
        struct {
        ...
        } argsCmd2;
        
        struct {
        ...
        } argsCmd3;

     } args;
};

enum MessageType_t {
    
    // server to miner
    welcomeMiner,       // Saludo de bienvenida a minero i
    sendBlock,          // Envío un bloque completo al minero junto con una sección del nonce a minar (primera comunicación)
    sendNonce,          // Envío una nueva sección de nonce a minar
    checkBlock,         // Envío una confimación de que el bloque minado tiene una dificultad mayor a la seteada por el pool
    floodStop,          // Envío un stop a todos los mineros de que hay que cortar de minar hasta que llegue un nuevo bloque (significa que alguien le pegó a un bloque con dificultad D) 
    sendReward,        // Recompenso al minero por su arduo trabajo
    farewellMiner,      // Saludo de despedida a minero i - Le puedo pasar algunos stats como para hacer un pat in the back

    // miner to server
    connectPool,        // Conecto al pool de minería el minero i. Debo enviar toda mi información para que me identifiquen en el pool
    reqBlock,           // Requiero un bloque para comenzar mis actividades 
    reqNonce,           // Requiero una nueva sección de nonce a minar (fixed-region nonce)
    submitBlock,        // Submiteo un bloque con el nonce con una dificultad igual o mayor a d
    disconnectPool     // Desconecto del pool al minero i. Debo enviar toda mi información para que me identifiquen en el pool
};

// Proceso mensajes enviados por el poolServer
void minerProcessMessage(int messageType){


    switch (messageType)
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
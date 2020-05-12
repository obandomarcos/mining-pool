#include "message.h"

// Proceso mensajes enviados por el poolServer
void minerProcessMessage(MessageType_t messageType){


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
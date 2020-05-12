#include "pool.h"
#include "common.h"
#include "message.h"


// Proceso mensajes enviados por el minero
void poolProcessMessage(MessageType_t message){

    switch (message)
    {
    case connectPool:
        /* Registro conexión, envio mensaje de bienvenida con stats*/
        break;
    
    case reqBlock:
        /* recibir y alocar bloque actual*/
        break;  

    case reqNonce:
        /* comenzar worker threads a minar la gilada*/
        break;
    
    case submitBlock:
        /* printear info acerca del check del bloque y celebrar*/
        break;

    case disconnectPool:
        /* Registro salida del minero y envío despedida con stats*/
        break;
    
    default:
        /* mensaje incorrecto */ 
        break;
    }

}

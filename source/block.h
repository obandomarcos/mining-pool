#ifndef BLOCK_H_
#define BLOCK_H_

#include <stdint.h>
#include <time.h>

#define MAX_TRANSACTIONS 2000

typedef unsigned int uint;

typedef struct{

    uint32_t magic;               // número mágico que identifique mis transacciones
    int32_t prevBlockHash;      // valor de hash del bloque previo en la cadena 
    // cambiar hashInt32 marcosRoot          // referencia al árbol de Merkle 
    time_t timestamp;             // un record del momento en el que se creó el bloque
    uint difficulty;              // dificultad del target para este bloque (en Bitcoin se llama bits)
    int32_t nonce;              // nonce utilizado para generar este bloque

}  BlockHeader_t;

typedef struct{

    BlockHeader_t header; //header del bloque

} Block_t;

Block_t * blockInit();

#endif
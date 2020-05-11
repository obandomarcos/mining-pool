#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_TRANSACTIONS 2000

typedef int32_t hashInt32;
typedef unsigned int uint;

// Los bloques contienen un 
typedef struct{

    uint32_t magic;               // número mágico que identifique mis transacciones
    hashInt32 prevBlockHash;      // valor de hash del bloque previo en la cadena 
    // cambiar hashInt32 marcosRoot          // referencia al árbol de Merkle 
    time_t timestamp;             // un record del momento en el que se creó el bloque
    uint difficulty;              // dificultad del target para este bloque (en Bitcoin se llama bits)
    hashInt32 nonce;              // nonce utilizado para generar este bloque

}  BlockHeader_t;

// cada bloque tiene una copia de todas las transacciones anteriores

typedef struct{

    int64_t value;
    address_t from;
    address_t to;

} Transaction_t;

typedef struct{

    // sarasa
    uint transactionCounter;
    Transaction_t transaction[MAX_TRANSACTIONS];

} BlockTransactions_t;

typedef struct{

    BlockHeader_t header; //header del bloque
    BlockTransactions_t transactions;

} Block_t;
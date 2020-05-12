#include <stdlib.h>
#include "block.h"
#include "common.h"

Block_t * blockInit(){

    Block_t *block;
    size_t size = sizeof(Block_t);

    block = (Block_t *)malloc(sizeof(Block_t));
    CHECK(block!=NULL);

    return block;

}

void blockDestroy(Block_t *block){

    free(block);
}
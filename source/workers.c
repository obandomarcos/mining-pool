#include "workers.h"

pthread_mutex_t goldNonceMutex = PTHREAD_MUTEX_INITIALIZER;

Worker_t * workerInit(int workerQty, int32_t * pGoldNonce){
    
    int i;
    Worker_t *workers;
    
    // solo puedo recibir mensajes de la cola
    mqd_t cola = mq_open(QUEUE_NAME, O_RDONLY);
    CHECK(cola != -1);

    //cargo una cantidad de trabajadores

    workers = (Worker_t *)malloc(workerQty * sizeof(Worker_t));
    CHECK(workers != NULL);
    
    for (i=0; i<workerQty; i++){

        workers[i].reqQueue = cola;
        workers[i].processing = false;
        workers[i].id = i;
        // donde el worker tiene que escribir el goldnonce si lo encuentra
        workers[i].minerNonce = pGoldNonce;
        workers[i].goldNonce = -1;
    }

    return workers;
}

// comienzo a procesar los threads de la cola
// asigno el numero de workers que quiero hacer trabajar

void workerRun(Worker_t *workers, int workerQty){

    int ret, i;

    //checkeo si workerQty es menor al numero disponible
    CHECK(workerQty <= MAX_WORKERS);

    for (i=0; i<workerQty; i++){

        // chequeo si el worker esta apagado   
        CHECK(workers[i].processing == false);
        workers[i].processing = true;


        // creo el thread con cada worker trabajando por la suya en thread run
        ret = pthread_create(&workers[i].thread_load, NULL, &thread_run, (void *)&workers[i]);
        CHECK(ret == 0);
    }
}

// paro el array de workers

void workerStop(Worker_t *workers, int workerQty){

    int ret, i;

    for (i=0;i<workerQty;i++){
        
        workers[i].processing = false;

        ret = pthread_join(workers[i].thread_load, NULL);
        CHECK(ret == 0);
    }
}

// es lo mismo que la de arriba pero espera que terminen de minar cada worker
void workerWait(Worker_t *workers, int workerQty){

    int ret, i;

    for (i=0;i<workerQty;i++){
        
        // workers[i].processing = false;

        ret = pthread_join(workers[i].thread_load, NULL);
        CHECK(ret == 0);
    }
}

// checkeo si el gold nonce del worker cambio. si así fue, paro todo a la pija
// void workerCheck(Worker_t *workers, int workerQty)
// {   
//     int finished = 0;
//     int i = 0;

//     // si tengo el gold nonce en caca, 
//     // o sigue alguno trabajando, loopeo
//     while(*workers[i].minerNonce == -1 || finished < workerQty)
//     {
//         // Tengo que chequear el nonce y si terminaron de trabajar
//         workers[i];

//         i = (i+1)%workerQty;
//     }

// }


// hago laburar a un worker de la lista de cosas que hay que hacer. Puede terminar porque se le acabó la cola de requerimientos o porque lo encontró, pero si o si escrbo que processing terminó
void *thread_run(void *ctx)
{

    WorkUnit_t *wu = (WorkUnit_t *)malloc(sizeof(WorkUnit_t));
    Worker_t *worker = (Worker_t *) ctx;
    Context_t *context;
    int ret;
    struct mq_attr attr;
    
    // printf("%lu\n", pthread_self());

    while(worker->processing)
    {

        mq_getattr(worker->reqQueue, &attr);

        // con la cola vacia, mis worker no laburan
        if(attr.mq_curmsgs == 0)
        {

            sleep(5);
            mq_getattr(worker->reqQueue, &attr);
            if(attr.mq_curmsgs == 0){

                
                worker->processing = false;
                printf("Worker %d timed out\n", worker->id);
                
                free(wu);
                return NULL;
            }
        }
        
        // manera mañosa de recibir un WorkUnit
        ret = mq_receive(worker->reqQueue, (char *) wu, sizeof(WorkUnit_t), NULL);
        CHECK(ret >= 0);

        // me guardo el contexto
        context = (Context_t *)wu -> context;

        printf("Work unit id %d\n", wu->id);

        mq_getattr(worker->reqQueue, &attr);
        // printf("quedan %li mensajes en la cola\n", attr.mq_curmsgs);
        
        // calculo la función de hash hasta que me quede sin nonce a minar o lo encuentre
        wu -> fun(context);
        // printf("%d\n", context-> found);
        // si lo encuentro, aviso al worker que ya está de procesar
        // printf("Nonce dorado actual %d\n", *worker->minerNonce);

        if (context -> found)
        {
            
            worker->processing = false;
            // protección para doble escritura
            pthread_mutex_lock(&goldNonceMutex);
            *worker->minerNonce = context -> nonce;
            pthread_mutex_unlock(&goldNonceMutex);

            free(wu->context);
            free(wu);
            return NULL;
        }

        free(wu->context);
    }

    // ccreo que nunca sale por acá pero por warnencios
    return NULL;
}

// la idea es que para este paso todos ya esten parados
// como siempre inicializo todos(a veces puedo correrlos todos o algunos), borro todos por defecto
void workerDestroy(Worker_t *worker)
{

    int i;
    
    for(i=0; i<MAX_WORKERS; i++){

        CHECK(worker[i].processing == false);
    
    }
    // libero el array completo de workers
    free(worker);
}

// funciones del worker
void hashBlock(void *ctx)
{   
    Context_t * context = (Context_t *)ctx;
    char cNonce[SHA_DIGEST_LENGTH];
    char digest[SHA_DIGEST_LENGTH];
    char test[SHA_DIGEST_LENGTH];
    int i;

    //seteo en cero a todos los elementos del bloque de testeo

    memset(test, 0, sizeof(test));
    for(i = context->nonce; i<(context->nonce+context->section); i++)
    {
        // pego el contenido del bloque al nonce
        sprintf(cNonce, "%s%d", context->block, i);

        // obtengo el hash del bloque+nonce
        SHA1((unsigned char*)cNonce, strlen(cNonce), (unsigned char*)digest);

        if (!memcmp(test, digest, context->difficulty*sizeof(char))){
           
           // le chanto el nonce de oro y aviso por contexto que lo encontré
           context -> nonce = i;
           context -> found = true;
        }
    }   
    
    return;
}
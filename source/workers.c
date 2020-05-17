#include "workers.h"

Worker_t * workerInit(int workerQty){
    
    int i;
    Worker_t *worker;
    
    // solo puedo recibir mensajes de la cola
    mqd_t cola = mq_open(QUEUE_NAME, O_RDONLY);
    CHECK(cola != -1);

    //cargo una cantidad de trabajadores

    worker = (Worker_t *)malloc(workerQty * sizeof(Worker_t));
    CHECK(worker != NULL);
    
    for (i=0; i<workerQty; i++){

        worker[i].reqQueue = cola;
        worker[i].processing = false;
        worker[i].id = i;
    }

    return worker;
}

// comienzo a procesar los threads de la cola
// asigno el numero de workers que quiero hacer trabajar

void workerRun(Worker_t *worker, int workerQty){

    int ret, i;

    //checkeo si workerQty es menor al numero disponible
    CHECK(workerQty <= MAX_WORKERS);

    for (i=0; i<workerQty; i++){

        // chequeo si el worker esta apagado   
        CHECK(worker[i].processing == false);
        worker[i].processing = true;

        // creo el thread con cada worker trabajando por la suya en thread load
        ret = pthread_create(&worker[i].thread_load, NULL, &thread_load, (void *)&worker[i]);
        CHECK(ret == 0);
    }
}

// paro el array de workers

void workerStop(Worker_t *worker, int workerQty){

    int ret, i;

    for (i=0;i<workerQty;i++){
        
        CHECK(worker[i].processing = true);
        worker[i].processing = false;

        ret = pthread_join(worker[i].thread_load, NULL);
        CHECK(ret == 0);
    }
}

// hago laburar a un worker de la lista de cosas que hay que hacer 
void *thread_load(void *ctx){

    WorkUnit_t wu;
    Worker_t *worker = (Worker_t *) ctx;
    int ret;
    struct mq_attr attr;
    
    while(worker->processing){

        mq_getattr(worker->reqQueue, &attr);
        if(attr.mq_curmsgs == 0){

            sleep(5);
            mq_getattr(worker->reqQueue, &attr);
            if(attr.mq_curmsgs == 0){
                printf("Worker %d timed out\n", worker->id);
                break;
            }
        }
        
        // manera mañosa de hacer esto
        ret = mq_receive(worker->reqQueue, (char *) &wu, sizeof(WorkUnit_t), NULL);
        CHECK(ret >= 0);

        printf("Work unit id %d\n", wu.id);

        mq_getattr(worker->reqQueue, &attr);
        printf("quedan %li mensajes en la cola\n", attr.mq_curmsgs);

        wu.fun(wu.context);
    }

    return NULL;
}

// la idea es que para este paso todos ya esten parados
// como siempre inicializo todos(a veces puedo correrlos todos o algunos), borro todos por defecto
void workerDestroy(Worker_t *worker){

    int i;
    
    for(i=0; i<MAX_WORKERS; i++){

        CHECK(worker[i].processing == false);
    }
    // libero el array completo de workers
    free(worker);
}

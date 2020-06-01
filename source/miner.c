#include "common.h"
#include "packet.h"
#include "miner.h"
#include "workers.h"


// Creación y destrucción
Miner_t * minerCreate(char *minerId)
{ 

    struct mq_attr attr = {0, 10, sizeof(WorkUnit_t), 0};
    Miner_t * miner = (Miner_t *)malloc(sizeof(Miner_t));
    CHECK(miner != NULL);

    sprintf(miner -> minerId, "/%s",minerId);
    
    miner -> reqQueue = mq_open(miner -> minerId, O_CREAT|O_RDWR|O_EXCL,  0666, &attr);
    CHECK(miner -> reqQueue != -1);
    miner -> goldNonce = -1;
    
    miner -> workerQty = MAX_WORKERS;
    miner -> workers = NULL;
    miner -> wallet = 0;
    miner -> count = 0;

    return miner;
}

void minerDestroy(Miner_t *miner)
{
    int ret;

    close(miner -> minersock);
    close(miner -> mcastsock);

    ret = mq_unlink(miner -> minerId);
    CHECK(ret == 0);

    free(miner);
}

// Protocolo de comunicación
void minerInit(Miner_t * miner, char * pool_name)
{      
    uint yes = 1;
    struct timeval read_timeout;

    miner->active = true;

    miner -> minersock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(miner -> minersock);
    miner -> pool = gethostbyname(pool_name);
    CHECK(miner -> pool!=NULL);
    memset(&miner -> pool_addr, 0, sizeof(miner -> pool_addr));
    miner -> pool_addr.sin_family = AF_INET;
    memcpy(&miner -> pool -> h_addr, &miner -> pool_addr.sin_addr.s_addr, miner-> pool -> h_length);
    miner -> pool_addr.sin_port = htons(PORT);

    // modifico el socket para recibir multicast (estaría bueno recibir la dirección de multicast de parte del minero y que se configure)

    miner -> mcastsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(miner -> mcastsock);

    // timeout
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = 10;
    
    // estaría bueno que esto sea por otro lado -> recibir la dirección de conexión
    CHECK(setsockopt(miner -> mcastsock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout)) >= 0);

    CHECK(setsockopt(miner -> mcastsock, SOL_SOCKET, SO_REUSEADDR,&yes,sizeof(yes)) >= 0);

    memset(&miner -> mcast_addr, 0, sizeof(miner -> mcast_addr));

    miner -> mcast_addr.sin_family = AF_INET;
    miner -> mcast_addr.sin_port = htons(MCAST_PORT);
    miner -> mcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    CHECK(bind(miner -> mcastsock, (struct sockaddr*)&miner -> mcast_addr, sizeof(miner -> mcast_addr))== 0);

    // me uno al grupo multicast
    miner -> group.imr_multiaddr.s_addr = inet_addr(MCAST_GROUP);
    miner -> group.imr_address.s_addr = htonl(INADDR_ANY);
    miner -> group.imr_ifindex = 0;

    CHECK(setsockopt(miner -> mcastsock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&miner->group,sizeof(miner->group)) >= 0);

}

void minerProcessPacket(Miner_t * miner)
{
    Packet_t *packet = packetRetrieval(miner);

    // salgo si en realidad es entrada de stdin. Lo sé, es medio chancho
    if (packet -> type == stdInput)
    {   

        minerExecute(miner, packet->args.args_stdIn.opt);

        free(packet);
        return;
    }
    
    // más chancho aún
    if (packet -> type == timeoutVal)
    {   
        free(packet);
        return;
    }

    // checkeo que corresponda a mensaje p2m
    CHECK((packet->type & entityMask));

    // proceso intercambio de paquetes
    switch (packet->type)
    {   

        case welcomeMiner:

            printf("%s\n", packet->args.args_welcomeMiner.mensaje);

            break;

        case sendBlock:

            // me cargo el bloque y la dificultad
            strcpy(miner -> block, packet->args.args_sendBlock.block);
            miner -> curDifficulty = packet->args.args_sendBlock.difficulty;

            minerSendPacket(miner, reqNonce);

            break;

        case sendNonce:

            // me cargo un nuevo nonce para hashear
            miner -> nonce = packet->args.args_sendNonce.nonce;
            miner -> section = packet->args.args_sendNonce.section;
            
            printf("Laburando\n");
            minerLoadQueue(miner);
            minerRun(miner); // laburo hasta encontrar resultados ó terminar la sec
            minerDecision(miner);
            
            break;

        case successBlock:

            printf("%s\n", packet->args.args_successBlock.mensaje);
            
            break;
        
        case floodStop:
            
            printf("%s\n", packet->args.args_floodStop.mensaje);
            // limpio el minero
            minerFlush(miner);
            
            break;
        
        case sendReward:

            miner->wallet += packet->args.args_sendReward.reward;
            printf("%s\n", packet->args.args_sendReward.mensaje);
            
            printf("Para requerir un nuevo bloque, ingrese 1:\n");

            break;
            
        case farewellMiner:
            
            printf("%s\n", packet->args.args_farewellMiner.mensaje);
            break;
            
        case shutdownPool:
            
            printf("%s\n", packet->args.args_shutdownPool.mensaje);
            miner->active = false;

            break;

        default:

            perror("Tipo de paquete no manipulable\n");
            exit(1);
            
            break;
    }

    free(packet);
}

void minerSendPacket(Miner_t * miner, PacketType_t pType)
{

    Packet_t packet;
    socklen_t slen=sizeof(miner -> pool_addr);

    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    // checkeo que el tipo de entidades sea miner2pool
    CHECK(!(pType & entityMask));

    miner -> state = pType;

    switch (pType)
    {
        case connectPool:
            packet.sz8 += sizeof(packet.args.args_connectPool);
            strcpy(packet.args.args_connectPool.mensaje, "Buen día, quiero conectarme al pool\n");
            
            // recibo el hola
            break;


        case reqBlock:

            packet.sz8 += sizeof(packet.args.args_reqBlock);
            // me guardo el bloque
            break;

        case reqNonce:

            packet.sz8 += sizeof(packet.args.args_reqNonce);

            // me guardo el nonce y laburo
            break;

        case submitNonce:

            packet.sz8 += sizeof(packet.args.args_submitNonce);
            strcpy(packet.args.args_submitNonce.mensaje, "Encontré un blocardo\n");
            packet.args.args_submitNonce.goldNonce = miner->goldNonce;

            break;

        case disconnectPool:

            packet.sz8 += sizeof(packet.args.args_disconnectPool);
            strcpy(packet.args.args_farewellMiner.mensaje, "Me hinche las bolas, renuncio\n");
            
            break;
        
        case idle: 
            
            return;
        
        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }

    // envío al pool
    CHECK(sendto(miner -> minersock, &packet, packet.sz8, 0, (struct sockaddr *)&miner -> pool_addr, slen)!=-1);
}

// con esto obtengo los paquetes
Packet_t * packetRetrieval(Miner_t *miner)
{

    Packet_t * packet = (Packet_t *)malloc(sizeof(Packet_t));
    int ready, maxfdp;
    fd_set rset;
    socklen_t slenPool = sizeof(miner -> pool_addr);
    socklen_t slenMcast = sizeof(miner -> mcast_addr);
    char buffer[BUFLEN];
    struct timeval timeout;

    FD_ZERO(&rset);

    maxfdp = max(miner -> minersock, miner -> mcastsock) + 1; 

    while(1)
    {   
        FD_SET(miner -> minersock, &rset); 
        FD_SET(miner -> mcastsock, &rset);
        FD_SET(STDIN_FILENO, &rset);
        
        // si no recibo nada, timeout
        timeout.tv_sec = 4;
        timeout.tv_usec = 0;
        ready = select(maxfdp, &rset, NULL, NULL, &timeout); 

        CHECK(ready != -1);

        // si no esta listo con algo, reenvio el requerimiento
        if (ready == 0){

            printf("Timeout de recepción\n");
            // VER EL PAQUETE QUE DEVUELVO?!!!!!
            packet->type = timeoutVal;

            // si no recibo respuesta, lo reenvío con el estado guardado
            minerSendPacket(miner, miner->state);
            continue;
        }
        else 
        {   
            miner -> state = idle;
            
            // leo de stdin

            if (FD_ISSET(STDIN_FILENO, &rset)) 
            { 
                // non comprehensive sanity check acá gilardo
                read(STDIN_FILENO, buffer, BUFLEN);

                packet -> type = stdInput;
                packet -> args.args_stdIn.opt = atoi(buffer);            

                break;
            } 
            //unicast

            if (FD_ISSET(miner -> minersock, &rset)) 
            { 
                // non comprehensive sanity check acá gilardo
                CHECK(recvfrom(miner -> minersock, packet, sizeof(Packet_t), 0, (struct sockaddr*)&miner -> pool_addr , &slenPool) !=  -1); 

                // proceso el paquete
                break;
            } 

            if (FD_ISSET(miner -> mcastsock, &rset)) 
            { 
                // non comprehensive sanity check acá gilardo
                CHECK(recvfrom(miner -> mcastsock, packet, sizeof(Packet_t), 0, (struct sockaddr*)&miner -> mcast_addr , &slenMcast) !=  -1); 
                
                break;      
            }
        } 
    }

    return packet;
}

void minerExecute(Miner_t * miner, MinerInputType_t inType)
{
    
    switch (inType)
    {
        case conn:

            printf("Conexión en progreso\n");
            minerSendPacket(miner, connectPool);
            minerExecute(miner, helpStart);
            break;

        case startMining:

            minerSendPacket(miner, reqBlock);
            system("clear");
            break;

        case platita:
        
            printf("Platita actual : %f Bancoins\n", miner->wallet);
            break;

        case stopMining:

            printf("Limpiando el minero\n");
            minerFlush(miner);
            minerExecute(miner, help);
            break;

        case disc:

            printf("Desconexión en progreso\n");
            minerSendPacket(miner, disconnectPool);
            break;
        
        case closeMiner:

            minerSendPacket(miner, disconnectPool);
            printf("Anda pillo\n\n");
            minerExecute(miner, platita);
            miner -> active = false;
            break;

        case help :

            printf("Command Reference\n\n1 - Continue Mining\n2 - Stop Mining\n3 - Disconnect from Pool\n4 - Show platita\n5 - Close miner\n6 - Help\n");
            break;
        
        case helpStart:

            printf("Command Reference\n\n1 - Start Mining\n2 - Stop Mining\n3 - Disconnect from Pool\n4 - Show platita\n5 - Close miner\n6 - Help\n");
            break;
        
        default:
            perror("Error de input\n");
            minerExecute(miner, help);

            break;
    }

}

// Funcionamiento : Inicio y finalización de busqueda de Hash

// decido en el momento en que estoy con el bloque en la mano
void minerDecision(Miner_t *miner)
{
    Packet_t *packet;

    // decido si envio el goldnonce o me robo el valor del bloque

    if (miner -> goldNonce != -1)
    {
        printf("Lo encontramos\nElegir que hacer :\n1 - Mandar al pool \n2 - Quedarmelo y cobrar la platita\n");
        packet = packetRetrieval(miner);

        //me lo robo y pido otro nonce
        if (packet->args.args_stdIn.opt == 2)
        {
            miner -> wallet += ROBO_BLOQUE;
            minerFlush(miner);
            minerSendPacket(miner, reqNonce);
            
        }
        // envio el nonce al pool
        else 
        {
            minerSendPacket(miner, submitNonce);
        }
        
        free(packet);
    }
    // si no lo encontré, sigo pidiendo
    else 
    {
        miner->count++;

        if (miner->count > MAX_ITER)
        {   
            miner->count = 0;
            minerExecute(miner, help);
            minerProcessPacket(miner);
        }
        else
        {   
            minerSendPacket(miner, reqNonce);
        }
    }   

}

// cargo la cola de requerimientos a los workers
void minerLoadQueue(Miner_t *miner)
{

    int i, sec, ret, count = 0;
    WorkUnit_t wu;
    Context_t * contextMiner = NULL;

    sec = miner->section/miner->workerQty;
   
    for (i = miner -> nonce; i < (miner->nonce+miner->section); count++, i += sec)
    {   
        // creo un contexto para cada wu
        contextMiner = malloc(sizeof(Context_t));

        // pego el bloque, la sección a minar, el nonce donde comenzar, la dificultad y que todavia no está minado ~ false
        strcpy(contextMiner->block, miner->block);
        contextMiner -> section = sec;
        contextMiner -> nonce = i;
        contextMiner -> difficulty = miner -> curDifficulty;
        contextMiner -> found = false;

        // info acerca del workunit
        wu.id = count;
        wu.fun = hashBlock;
        wu.context = (void *)contextMiner;

        // subo a la cola de requerimientos el trabajo
        ret = mq_send(miner->reqQueue, (const char *)&wu, sizeof(WorkUnit_t), 0);
        CHECK(ret == 0);
    }
}

// comienzo proceso de corrida del miner
void minerRun(Miner_t *miner){

    miner-> workers = workerInit(miner -> workerQty,  &miner->goldNonce, miner->minerId);
    CHECK(miner -> workers != NULL);
    // inicializo una cantidad de workers y les paso a donde tienen que escribir
    workerRun(miner->workers, miner->workerQty);

    workerWait(miner->workers, miner->workerQty);
    // workerCheck(miner->workers, miner->workerQty);
}

// limpio la cola de mensajes y mis workers brutamente
void minerFlush(Miner_t *miner){

    int ret;
    struct mq_attr attr = {0, 10, sizeof(WorkUnit_t), 0};

    ret = mq_close(miner->reqQueue);
    CHECK(ret == 0);

    miner -> reqQueue = mq_open(miner->minerId, O_CREAT|O_RDWR,  0666, &attr);
    CHECK(miner -> reqQueue != -1);
    
    miner -> goldNonce = -1;
    miner -> count = 0;

    workerDestroy(miner->workers);
}

// Funciones útiles
int max(int x, int y) 
{ 
    if (x > y) 
        return x; 
    else
        return y; 
} 


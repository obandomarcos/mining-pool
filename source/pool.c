#include "pool.h"

Pool_t * poolInit()
{

    Pool_t * pool = (Pool_t *)malloc(sizeof(Pool_t));
    CHECK(pool != NULL);
    
    srand(time(NULL));
    poolReloadBlock(pool);

    pool -> miners = 0;
    pool -> minerDifficulty = 5;
    pool -> poolDifficulty = 5;
    pool -> section = 1000;

    // valores para repartición
    pool -> blockValue = 12.5;
    pool -> gainPool = 5;
    // arranco con cero pesitos
    pool -> poolWallet = 0;

    return pool;
}

void poolListen(Pool_t * pool)
{   
    
    pool -> poolsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    CHECK(pool -> poolsock!=-1);

    memset(&pool -> pool_addr, 0, sizeof(pool -> pool_addr));
    pool -> pool_addr.sin_family = AF_INET;
    pool -> pool_addr.sin_port = htons(PORT);
    pool -> pool_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    CHECK(bind(pool -> poolsock, (struct sockaddr *)&pool -> pool_addr, sizeof(pool -> pool_addr))!=-1);

    // como el otro lo tengo conectado a todas las interfaces, no me quiero mandar cagadas
    memset(&pool -> mcast_addr,0,sizeof(pool -> mcast_addr));
    pool -> mcast_addr.sin_family = AF_INET;
    pool -> mcast_addr.sin_addr.s_addr = inet_addr(MCAST_GROUP);
    pool -> mcast_addr.sin_port = htons(MCAST_PORT);    

    printf("Escuchando desde el pool por nuevo minero\n");

}

void poolDestroy(Pool_t * pool)
{

    close(pool -> poolsock);

    free(pool);
}

// esta sería como un método privado
void poolSendPacket(Pool_t *pool, PacketType_t pType)
{
    Packet_t packet;

    packet.type = pType;
    packet.sz8 = sizeof(packet.sz8)+sizeof(packet.type);

    // checkeo que corresponda a mensaje p2m
    CHECK(pType & entityMask);

    //  empaqueto mi info
    switch (pType)
    {
        case welcomeMiner:

            packet.sz8 += sizeof(packet.args.args_welcomeMiner);
            
            strcpy(packet.args.args_welcomeMiner.mensaje, "¡Bienvenido a Bandis!\n");
            printf("Enviando saludo...\n");

            break;

        case farewellMiner:
        
            packet.sz8 += sizeof(packet.args.args_farewellMiner);
            strcpy(packet.args.args_farewellMiner.mensaje, "Te saludamos desde Bandis\n");

            break;

        case sendBlock:

            packet.sz8 += sizeof(packet.args.args_sendBlock);

            //función get nonce
            strcpy(packet.args.args_sendBlock.block, pool-> block); 
            // Envío dificultad
            packet.args.args_sendBlock.difficulty = pool->minerDifficulty;

            break;

        case sendNonce:

            packet.sz8 += sizeof(packet.args.args_sendNonce);

            // Envío nonce
            packet.args.args_sendNonce.nonce = pool -> nonce;

            // Envío sección (se la envío porque podría ser variable la sección que entrego)            
            pool -> nonce += pool->section; 
            packet.args.args_sendNonce.section = pool -> section; 

            break;
        
        case discardBlock:
            // logear el descarte
            poolSendPacket(pool, sendNonce);

            break;

        case failureBlock:
            // logear fallos? NO , it's weird
            poolSendPacket(pool, sendNonce);

            break;

        case successBlock:
            
            printf("Encontraron el bloque\n");
            strcpy(packet.args.args_successBlock.mensaje, "Lo encontraste, ahí paro todo\n");

        // multicast acá

        case floodStop : 
            
            packet.sz8 += sizeof(packet.args.args_floodStop);
            
            //función get nonce
            strcpy(packet.args.args_floodStop.mensaje, "Basta de laburar\n");
        
            break;

        // falta esto
        case sendReward:

            packet.sz8 += sizeof(packet.args.args_sendReward);

            // le escribo todas las felicitaciones
            strcpy(packet.args.args_sendReward.mensaje, "Basta de laburar, lo encontraste\n");
            
            // le paso la recompensa por su arduo trabajo
            packet.args.args_sendReward.reward = poolCalculateReward(pool);
            break;

        default:
            perror("Tipo de paquete no manipulable\n");
            exit(1);
            break;
    }

    // distingo entre multicast y unicast
    
    switch (pType & multicastMask){

        case 0x0: // unicast
            
            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> miner_addr, (socklen_t)sizeof(pool -> miner_addr))!=-1);
            break;

        default: // multcast
            
            CHECK(sendto(pool -> poolsock, &packet, packet.sz8, 0, (struct sockaddr *)&pool -> mcast_addr, (socklen_t)sizeof(pool -> mcast_addr))!=-1);

    }
}

void poolProcessPacket(Pool_t *pool)
{

    Packet_t  packet;
    socklen_t slen = sizeof(pool -> miner_addr);

    CHECK(recvfrom(pool -> poolsock, &packet, sizeof(Packet_t), 0, (struct sockaddr *)&pool -> miner_addr, &slen) != -1);
    
    // checkeo que corresponda a mensaje m2p
    CHECK(!(packet.type & entityMask));

    switch (packet.type)
    {
        case connectPool:
            
            printf("%s", packet.args.args_welcomeMiner.mensaje);
            
            // cargar a la lista de mineross, por ahora solo sumo una
            pool -> miners += 1;

            poolSendPacket(pool, welcomeMiner);
            break;

        case disconnectPool:
            
            // getear de la lista de mineros, por ahora solo resto uno
            pool -> miners -= 1;
            
            printf("%s", packet.args.args_disconnectPool.mensaje);
            poolSendPacket(pool, farewellMiner);
            
            break;
        
        case reqNonce:
            
            printf("Pedido de nonce\n");
            poolSendPacket(pool, sendNonce); 
            break;

        case reqBlock:

            poolSendPacket(pool, sendBlock); 
            break;

        case submitNonce:

            //acá voy a decidir en base al bloque si es bueno o malo y le mando la confirmación, también guardo el address
            poolVerifyBlock(pool, packet.args.args_submitNonce.goldNonce);
            break;

        default:
            
            perror("Tipo de paquete no manipulable\n");            
            exit(1);
            break;
    }
    
    // printf("Paquete recibido desde %s:%d\n\n", 
    // inet_ntoa(pool -> miner_addr.sin_addr), ntohs(pool -> miner_addr.sin_port));
}

void poolVerifyBlock(Pool_t * pool, int32_t goldNonce)
{   
    BlockDiffType_t diffType;

    diffType = hashCheckBlock(pool, goldNonce);

    switch(diffType)
    {
        case noDif:

            poolSendPacket(pool, failureBlock);
            break;

        case minDif:
            
            // guardar el bloque y felicitar, pero que siga laburando por que la verdad es que no hizo una poronga
            poolSendPacket(pool, discardBlock);
            break;

        case poolDif:
            
            
            poolSendPacket(pool, successBlock);
            poolSendPacket(pool, floodStop);
            poolSendPacket(pool, sendReward);

            printf("Plata actual : %f\n", pool->poolWallet);
            // // aquí deberia comenzar todo de vuelta
            // poolReloadBlock(pool);

            // loggear el bloque, floodear el pool y felicitar con recompensas a todes
            break;
    }
}

// vuelvo a reiniciar todas las variables de bloque y nonce
void poolReloadBlock(Pool_t *pool)
{   
    int randint = (int)(1.0*rand()*1000/RAND_MAX);

    // Initialization, should only be called once.
    sprintf(pool -> block, "%s%d", "bloque", randint);
    pool -> nonce = 0;

}

// funcion para pool verify block
BlockDiffType_t hashCheckBlock(Pool_t *pool, int32_t nonce)
{   
    char cNonce[SHA_DIGEST_LENGTH];
    char digest[SHA_DIGEST_LENGTH];
    char test[SHA_DIGEST_LENGTH];
    
    //seteo en cero a todos los elementos del bloque de testeo
    memset(test, 0, sizeof(test));

    // pego el contenido del bloque al nonce
    sprintf(cNonce, "%s%d", pool->block, nonce);

    // obtengo el hash del bloque+nonce
    SHA1((unsigned char*)cNonce, strlen(cNonce), (unsigned char*)digest);

    if (!memcmp(test, digest, pool->minerDifficulty*sizeof(char))){

      if(!memcmp(test, digest, pool->poolDifficulty*sizeof(char)))
      { 
        
        return poolDif;
      
      }
      else{
        
        return minDif;
      
      }
    }
    else return noDif;
}

// hago función basicona que le reparta a todos por igual y me pague el resto en mi billetera
float poolCalculateReward(Pool_t *pool)
{
    float rewardMiner = (float)((pool->blockValue - pool->gainPool)/pool -> miners);
    
    pool -> poolWallet += pool->gainPool;
    
    return rewardMiner;
}

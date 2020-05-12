#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"
#include "pool_server.h"

#define BACKLOG_NUM 10
/*
 * Function:  poolOpenSock
 * --------------------
 *  opens poo
 * 
 *  socket: pool socket 
 *  portno: port where the socket will be listening
 *
 *  returns: pool socket with IPv6 protocol family and TCP reliable connection
 *          
 */

int poolOpenSock(void)
{

    int poolSockfd = socket(PF_INET, SOCK_STREAM, 0);
    
    CHECK(poolSockfd != -1);
    
    return poolSockfd;
}

/*
 * Function:  poolPortBind 
 * --------------------
 *  sets socket name and binds it an address name, using IPv4 info. 
 * 
 *  socket: pool socket 
 *  portno: port where the socket will be listening
 *
 *  returns: 
 */
void poolPortBind(int socket, int portno)
{

    int ret, val = 1;
    struct sockaddr_in pool_addr;

    memset(&pool_addr, 0, sizeof(pool_addr));
    
    ret = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
               &val, sizeof(val));
    CHECK(ret >= 0);
    
    pool_addr.sin_family = AF_INET;
    pool_addr.sin_addr.s_addr = INADDR_ANY;
    pool_addr.sin_port = htons(portno);

    ret = bind(socket, (struct sockaddr *) &pool_addr,
            sizeof(pool_addr));
    CHECK(ret <= 0);
}

/*
 * Function:  poolListen 
 * --------------------
 *  Marks socket to accept incoming connections. 
 *  BACKLOG_NUM defines the max length to which the queue of pending connections may grow. Prints a listening message on console
 *  
 *  returns: 
 */
void poolListen(int socket, int portno)
{

    int ret;
    char poolListenMes[MAX_SIZE] = "Escuchando desde el puerto :";

    ret = listen(socket, BACKLOG_NUM);
    CHECK(ret == 0);
    
    printf("%s : %d\n", poolListenMes, portno);
}


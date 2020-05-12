#ifndef POOL_SERVER_H_
#define POOL_SERVER_H_

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

int poolOpenSock(void);

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
void poolPortBind(int socket, int portno);

/*
 * Function:  poolListen 
 * --------------------
 *  Marks socket to accept incoming connections. 
 *  BACKLOG_NUM defines the max length to which the queue of pending connections may grow. Prints a listening message on console
 *  
 *  returns: 
 */
void poolListen(int socket, int portno);

#endif // POOL_SERVER_H_
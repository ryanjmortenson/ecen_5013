/** @file socket_helper.h
*
* @brief Simple helper functions for socket read/write
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

int32_t socket_recv(int32_t sockfd, void * data, uint32_t count);
int32_t socket_write(int32_t sockfd, void * data, uint32_t count);

#endif // _SOCKET_HELPER_H

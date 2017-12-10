/** @file socket_helper.h
*
* @brief Simple helper functions for socket read/write
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _SOCKET_HELPER_H
#define _SOCKET_HELPER_H

/*!
* @brief Socket helper to call receive until count bytes are received
* @param sockfd socket descriptor
* @param data where to place data
* @param count number of bytes to attemp to received
* @return number of bytes received
*/
int32_t socket_recv(int32_t sockfd, void * data, uint32_t count);

/*!
* @brief Socket helper to call write until count bytes are write
* @param sockfd socket descriptor
* @param data to be written
* @param count number of bytes to attemp to write
* @return number of bytes written
*/
int32_t socket_write(int32_t sockfd, void * data, uint32_t count);

#endif // _SOCKET_HELPER_H

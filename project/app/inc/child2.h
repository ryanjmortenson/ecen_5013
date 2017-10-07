/** @file child2.h
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _CHILD2_H
#define _CHILD2_H

/*!
* @brief Start child thread 2
* @param[out] child2 pointer the pthread_t structure to return thread info
* @return SUCCESS/FAILURE
*/
int32_t child2_init(pthread_t * child2);

#endif /* _CHILD2_H */

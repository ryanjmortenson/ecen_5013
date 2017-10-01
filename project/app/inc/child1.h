/** @file child1.h
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _CHILD1_H
#define _CHILD1_H

typedef struct file_info {
  int32_t chars;
  int32_t words;
  int32_t lines;
} file_info_t;

/*!
* @brief Start child thread 2
* @param[in] file_name File name to collect statistics on
* @param[out] child1 pointer the pthread_t structure to return thread info
* @return SUCCESS/FAILURE
*/
int32_t child1_init(char * file_name, pthread_t * child1);

/*!
* @brief Print current file stats
*/
void print_file_stats();

#endif /* _CHILD1_H */

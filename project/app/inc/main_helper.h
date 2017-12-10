/** @file main_helper.h
*
* @brief Helpers for main tasks
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _MAIN_HELPER_H
#define _MAIN_HELPER_H

#ifdef BBB
#include "led.h"
#define USR3_LED (3)
#endif // BBB

#define SIGNAL_ABORT() abort_signal = 1

typedef struct hb_setup {
  uint32_t period_seconds;
} hb_setup_t;

typedef struct hb_reg {
  timer_t timerid;
  uint32_t period_seconds;
  uint8_t in_use;
} hb_reg_t;

/*!
* @brief Handle humidity response from humidity request
* @param param msg holding humidity response
* @return NULL
*/
void * hum_rsp_handler(void * param);

/*!
* @brief Handle air response from air request
* @param param msg holding air response
* @return NULL
*/
void * air_rsp_handler(void * param);

/*!
* @brief Handle light response from light request
* @param param msg holding light response
* @return NULL
*/
void * light_rsp_handler(void * param);

/*!
* @brief Handle light response from light request
* @param param msg holding light response
* @return NULL
*/
void * is_dark_rsp_handler(void * param);

/*!
* @brief Handle temp response from temp request
* @param param msg holding temp response
* @return NULL
*/
void * temp_rsp_handler(void * param);

/*!
* @brief Handle heartbeats
* @param param msg holding heartbeat
* @return NULL
*/
void * hb_handler(void * param);

/*!
* @brief Handle heartbeat set up
* @param param msg holding heartbeat set up request
* @return NULL
*/
void * hb_setup(void * param);

#ifndef TIVA
/*!
* @brief Handle heartbeat timeout
* @param sig signal that invoke handler
* @param info provided information for handler
* @param data some data
*/
void hb_timeout_handler(int sig, siginfo_t * info, void * data);

/*!
* @brief Handle sigint and sigterm signals to shutdown gracefully
* @param sig signal that invoke handler
*/
void sigint_handler(int sig);

void init_timers();

void del_timers();
#endif // TIVA

#endif /* _MAIN_HELPER_H */

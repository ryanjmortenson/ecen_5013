#ifndef __UNIT_CIRCBUF_H__
#define __UNIT_CIRCBUF_H__

/*
 * \brief test_circbuf_init_destroy: test circbuf_init circbuf_destroy under normal operations
 *
 */
void test_circbuf_init_destroy(void **state);

/*
 * \brief test_circbuf_ops_null_ptr: test circbuf operations handle circbuf_t null pointer
 *
 */
void test_circbuf_ops_null_ptr(void **state);

/*
 * \brief test_circbuf_null_buffer: test circbuf operations handle buffer null pointer
 *
 */
void test_circbuf_null_buffer(void **state);

/*
 * \brief test_circbuf_add_remove_full: test adding then removing return the
 *                                      same values for a full buffer
 *
 */
void test_circbuf_add_remove_full(void **state);

/*
 * \brief test_circbuf_add_full: test that adding when full fails gracefully
 *
 */
void test_circbuf_add_full(void **state);

/*
 * \brief test_circbuf_remove_empty: test that removing when empty fails gracefully
 *
 *
 */
void test_circbuf_remove_empty(void **state);

/*
 * \brief test_circbuf_wrap_add: test that the circular buffer wraps correctly
 *
 */
void test_circbuf_wrap_add(void **state);


/*
 * \brief test_circbuf_wrap_remove: test that the circular buffer wraps
 *                                  correctly
 *
 */
void test_circbuf_wrap_remove(void **state);

/*
 * \brief test_circbuf_check_full: test circbuf_full function works
 *
 */
void test_circbuf_check_full(void **state);

#ifdef UNITTEST
/*
 * \brief test_circbuf_check_empty: test circbuf_empty function works
 *
 */
void test_circbuf_check_empty(void **state);
#endif // UNITTEST
#endif // __UNIT_CIRCBUF_H__

/** @file template.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include "template.h"

int8_t max8 (int8_t num1, int8_t num2)
{
    return ((num1 > num2) ? num1 : num2);
}

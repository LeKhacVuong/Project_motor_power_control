/*
 * compiler_optimize.h
 *
 *  Created on: Jun 16, 2020
 *      Author: quangnd
 */

#ifndef BOARD_COMPILER_OPTIMIZE_H_
#define BOARD_COMPILER_OPTIMIZE_H_

#define FORCE_INLINE    inline __attribute__((always_inline))
/*
#ifndef UNUSED
#define UNUSED		__attribute__((unused))
#endif
*/

#ifndef WEAK
#define WEAK		__attribute__((weak))
#endif


#if USED_OPTIMIZE_SPEED_ON
#define OPTIMIZE_SPEED_ON   __attribute__((optimize("O3")))
#else
#define OPTIMIZE_SPEED_ON
#endif

#if USED_RAMFUNC
#define RAM_FUNC  __attribute__((section(".data,\"aw\",%progbits @")))
#else
#define RAM_FUNC
#endif

#define OPTIMIZE_ATTR  FORCE_INLINE RAM_FUNC OPTIMIZE_SPEED_ON


#endif /* BOARD_COMPILER_OPTIMIZE_H_ */

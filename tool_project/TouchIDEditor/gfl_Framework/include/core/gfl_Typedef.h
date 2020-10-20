/* 
 * File:   gfl_Typedef.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:35
 */

#ifndef GFL_TYPEDEF_H
#define	GFL_TYPEDEF_H
#pragma once

// =============================================================================
// 明示的にビット長を表現した型宣言。
// =============================================================================

// 任天堂の場合は既に定義されているので、このタイプデフを定義しない
#ifndef GFL_PLATFORM_NINTENDO
// unix の場合は似たような定義があるのでそれを利用する
#ifdef GFL_PLATFORM_UNIX
#include <stdint.h>
typedef uint8_t u8; /**< 符号無８ビット変数 */
typedef uint16_t u16; /**< 符号無16ビット変数 */
typedef uint32_t u32; /**< 符号無32ビット変数 */
typedef uint64_t u64; /**< 符号無64ビット変数 */

typedef int8_t s8; /**< 符号付８ビット変数 */
typedef int16_t s16; /**< 符号付16ビット変数 */
typedef int32_t s32; /**< 符号付32ビット変数 */
typedef int64_t s64; /**< 符号付64ビット変数 */
#else // !GFL_PLATFORM_UNIX
typedef unsigned char u8; /**< 符号無８ビット変数 */
typedef unsigned short u16; /**< 符号無16ビット変数 */
typedef unsigned int u32; /**< 符号無32ビット変数 */
typedef unsigned long long u64; /**< 符号無64ビット変数 */

typedef __int8 s8; /**< 符号付８ビット変数 */
typedef signed short s16; /**< 符号付16ビット変数 */
typedef signed int s32; /**< 符号付32ビット変数 */
typedef signed long long s64; /**< 符号付64ビット変数 */
#endif // GFL_PLATFORM_UNIX

typedef float f32; /**< 短精度浮動少数点変数 */
typedef double f64; /**< 倍精度浮動少数点変数 */
typedef signed long b32; /**<	32ビットのブール */
#endif // !GFL_PLATFORM_NINTENDO

// オリジナルアサイン
typedef unsigned short f16; /**< 16ビット浮動少数点変数 */
typedef char c8; /**<	8ビットの文字変数 */

#endif	/* GFL_TYPEDEF_H */

#ifndef __GFL_MATH_CONTROL_H__
#define __GFL_MATH_CONTROL_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathControl.h
 *	@brief  数値管理　
 *	@author	tomoya takahashi
 *	@date		2010.10.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <types/include/gfl2_Typedef.h>

namespace gfl2 {
namespace math {

  template <typename T>
  inline T Abs(T x)
  {
      return (x >= 0) ? x: -x;
  }

  template <typename T, typename U, typename S>
  inline S Max(T a, U b)
  {
      return (a >= b) ? a: b;
  }

  template <typename T>
  inline T Max(T a, T b)
  {
      return (a >= b) ? a: b;
  }

  template <typename T, typename U, typename S>
  inline S Min(T a, U b)
  {
      return (a <= b) ? a: b;
  }

  template <typename T>
  inline T Min(T a, T b)
  {
      return (a <= b) ? a: b;
  }

  template <typename T, typename U, typename R, typename S>
  inline S Max(T a, U b, R c)
  {
      return (a >= b) ? ((a >= c) ? a: c) : ((b >= c) ? b: c);
  }

  template <typename T>
  inline T Max(T a, T b, T c)
  {
      return (a >= b) ? ((a >= c) ? a: c) : ((b >= c) ? b: c);
  }

  template <typename T, typename U, typename R, typename S>
  inline S Min(T a, U b, R c)
  {
      return (a <= b) ? ((a <= c) ? a: c) : ((b <= c) ? b: c);
  }

  template <typename T>
  inline T Min(T a, T b, T c)
  {
      return (a <= b) ? ((a <= c) ? a: c) : ((b <= c) ? b: c);
  }

  template <typename T>
  inline T Clamp(T x, T low, T high)
  {
      return (x >= high) ? high : ((x <= low) ? low: x);
  }

  template <typename T>
  inline T RoundUp(T x, u32 base)
  {
      return static_cast<T>( (x + (base - 1)) & ~(base - 1) );
  }

  template <typename T>
  inline void* RoundUp(void* x, u32 base)
  {
      return reinterpret_cast<void*>( RoundUp(reinterpret_cast<uptr>(x), base) );
  }

  template <typename T>
  inline const void* RoundUp(const void* x, u32 base)
  {
      return reinterpret_cast<const void*>( RoundUp(reinterpret_cast<uptr>(x), base) );
  }

  template <typename T>
  inline T RoundDown(T x, u32 base)
  {
      return static_cast<T>( x & ~(base - 1) );
  }

  template <typename T>
  inline void* RoundDown(void* x, u32 base)
  {
      return reinterpret_cast<void*>( RoundDown(reinterpret_cast<uptr>(x), base) );
  }

  template <typename T>
  inline const void* RoundDown(const void* x, u32 base)
  {
      return reinterpret_cast<const void*>( RoundDown(reinterpret_cast<uptr>(x), base) );
  }

  template <typename T>
  inline T DivUp(T x, T y)
  {
      return static_cast<T>( (x + (y - 1)) / y );
  }

  template <typename T>
  inline T ExtractBits(bit32 v, int pos, int len)
  {
      return static_cast<T>( v & (((1u << len) - 1) << pos) );
  }

  template <typename T>
  inline T ExtractBits(bit64 v, int pos, int len)
  {
      return static_cast<T>( v & (((1ull << len) - 1) << pos) );
  }
  
  template <typename T>
  inline T GetBits(bit32 v, int pos, int len)
  {
      return static_cast<T>( (v >> pos) & ((1u << len) - 1) );
  }
  
  template <typename T>
  inline T GetBits(bit64 v, int pos, int len)
  {
      return static_cast<T>( (v >> pos) & ((1ull << len) - 1) );
  }

  template <typename T>
  inline bit32 MakeBits(T v, int width, int shift)
  {
      return (static_cast<bit32>(v) & ((1u << width) - 1)) << shift;
  }
} /* end of namespace math */
} /* end of namespace gfl2 */


#endif // __GFL_MATH_CONTROL_H__

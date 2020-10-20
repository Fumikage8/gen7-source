#if !defined( __BTL_FX32_H__ )
#define __BTL_FX32_H__
//=============================================================================================
/**
 * @file  btl_fx32.h
 * @brief ポケモンXY バトルシステム WBと同様の計算結果を実現するための固定小数処理
 * @author  taya
 *
 * @date  2010.12.18  作成
 */
//=============================================================================================
#pragma once
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(btl)

typedef s32 fx32;

enum {
  FX32_INT_SIZE =  19,
  FX32_DEC_SIZE =  12,
  FX32_SHIFT    =  FX32_DEC_SIZE,

  FX32_DEC_MASK = ((1 << FX32_SHIFT) - 1),
};

#define FX32_CONST(x)    ((fx32)(((x) > 0) ? \
                                     ((x) * (1 << FX32_SHIFT) + 0.5f ) : \
                                     ((x) * (1 << FX32_SHIFT) - 0.5f )))


inline  double FXtoFloat( fx32 val )
{
  const fx32 one = (1 << FX32_SHIFT);

  fx32 decimal = val & (one - 1);

  double f_num = (double)(val >> FX32_SHIFT);
  double f_dec;
  if( decimal ){
    f_dec = (double)decimal / (double)one;
  }else{
    f_dec = 0;
  }
  #if 0
  BTL_PRINT("[FXtoFloat]  val:%08x / decimal:%08x / num:%5.1f, def:%5.4f := %7.4f\n",
        val, decimal, f_num, f_dec, f_num+f_dec);
  #endif
  return f_num + f_dec;
}
inline  fx32 FX_Mul( fx32 v1, fx32 v2)
{
  return (fx32)(((s64)(v1) * v2 + 0x800LL) >> FX32_SHIFT);
}
inline s32 FX_Whole(fx32 v)
{
    return (s32)(v >> FX32_SHIFT);
}
inline fx32 FX_Div( fx32 numer, fx32 denom )
{
  double f_numer = FXtoFloat( numer );
  double f_denom = FXtoFloat( denom );

  if( f_denom )
  {
    double val = f_numer / f_denom;
    return FX32_CONST( val );
  }
  return 0;
}
inline fx32 FX_Sqrt( fx32 val )
{
  float f_val = (f32)FXtoFloat( val );
#if 0
  f_val = gfl::math::FrSqrt( f_val );
#else
  f_val = gfl2::math::FSqrt( f_val );
  f_val = 1.0f / f_val; // 計算結果が変わらないように gfl2 に置き換え。逆数にしてるのは何で？obata
#endif
  return FX32_CONST( f_val );
}

inline fx32 FX_POW( fx32 val1, fx32 val2 )
{
  f32 v1 = (f32)FXtoFloat( val1 );
  f32 v2 = (f32)FXtoFloat( val2 );

  f32 result = ::std::pow( v1, v2 );
  return FX32_CONST( result );
}

GFL_NAMESPACE_END(btl)

#endif // __BTL_FX32_H__

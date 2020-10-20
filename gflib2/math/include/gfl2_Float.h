//======================================================================
/**
 * @file gfl2_Float.h
 * @date 2015/07/16 20:16:59
 * @author araki_syo
 * @brief 浮動小数点クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined GFLIB2_MATH_FLOAT_H_INCLUDED
#define GFLIB2_MATH_FLOAT_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(math)

/**
  * @class Float
  * @brief クラスの説明
  */
class Float
{
    GFL_FORBID_COPY_AND_ASSIGN(Float);

private:
  Float();
  ~Float();

public:
  /**
    * @fn Ceil
    * @brief 小数点第 length 位切り上げ
    * @param[in] in
    * @param[in] length
    * @return 小数点第length位で切り上げた値
    * @note 浮動小数点の誤差のため、意図した値と異なる場合があります
    */
  static f32 Ceil(f32 in, s32 length)
  {
    f32 out;
    out = in * pow(10.0f, length);
    out = (f32)(s32)(out + 0.9f);

    return out * pow(10.0f, -length);
  }

   /**
    * @fn Floor
    * @brief 小数点第 length 位切り捨て
    * @param[in] in
    * @param[in] length
    * @return 小数点第length位で切り捨てた値
    * @note 浮動小数点の誤差のため、意図した値と異なる場合があります
    */
  static f32 Floor(f32 in, s32 length)
  {
    f32 out;
    out = in * pow(10.0f, length);
    out = (f32)(s32)(out);

    return out * pow(10.0f, -length);
  }


   /**
    * @fn Round
    * @brief 小数点第 length 位四捨五入
    * @param[in] in
    * @param[in] length
    * @return 小数点第length位で四捨五入した値
    * @note 浮動小数点の誤差のため、意図した値と異なる場合があります
    */
  static f32 Round(f32 in, s32 length)
  {
    f32 out;
    out = in * pow(10.0f, length);
    out = (f32)(s32)(out + 0.5f);

    return out * pow(10.0f, -length);
  }
};

GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)

#endif // GFLIB2_MATH_FLOAT_H_INCLUDED
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsType.cpp
 *	@brief  グラフィックス環境　共通型、定数定義
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nw/ut.h>

#include <grp/gfl_GraphicsType.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					定義
*/
//-----------------------------------------------------------------------------
ColorF32 ConvertColorU8ToColorF32(const ColorU8& c)
{
  ColorF32 c_f32 = c;  // nw::ut::FloatColor::operator= ( Color8 )  // RGBA8 のカラーを FloatColor に設定する代入演算子です。
  return c_f32;
}
ColorU8  ConvertColorF32ToColorU8(const ColorF32& c)
{
  ColorU8 c_u8 = (nw::ut::Color8)c;  // nw::ut::FloatColor::operator Color8() const;  // RGBA8 カラーへのキャスト演算子です。
  return c_u8;
}

ColorF32* ColorF32Lerp(           // pOutを返す
    ColorF32*       pOut,         // 計算結果を受け取る。p1, p2と同じでも構わない。
    const ColorF32* p1,
    const ColorF32* p2,
    f32             t             // 0.0fであればp1が、1.0fであればp2が結果となる。
)
{
  pOut->r = p1->r*(1.0f-t) + p2->r*t;
  pOut->g = p1->g*(1.0f-t) + p2->g*t;
  pOut->b = p1->b*(1.0f-t) + p2->b*t;
  pOut->a = p1->a*(1.0f-t) + p2->a*t;
  return pOut;
}

ColorF32* ColorF32Scale(           // pOutを返す
    ColorF32*       pOut,          // 計算結果を受け取る。pと同じでも構わない。
    const ColorF32* p,
    f32             scale          // 掛ける数
)
{
  pOut->r = p->r * scale;
  pOut->g = p->g * scale;
  pOut->b = p->b * scale;
  pOut->a = p->a * scale;
  return pOut;
}

} /* end namespace grp */
} /* end namespace gfl */

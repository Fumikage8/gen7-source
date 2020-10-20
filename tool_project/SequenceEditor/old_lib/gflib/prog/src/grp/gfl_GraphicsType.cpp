//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsType.cpp
 *	@brief  �O���t�B�b�N�X���@���ʌ^�A�萔��`
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
 *					��`
*/
//-----------------------------------------------------------------------------
ColorF32 ConvertColorU8ToColorF32(const ColorU8& c)
{
  ColorF32 c_f32 = c;  // nw::ut::FloatColor::operator= ( Color8 )  // RGBA8 �̃J���[�� FloatColor �ɐݒ肷�������Z�q�ł��B
  return c_f32;
}
ColorU8  ConvertColorF32ToColorU8(const ColorF32& c)
{
  ColorU8 c_u8 = (nw::ut::Color8)c;  // nw::ut::FloatColor::operator Color8() const;  // RGBA8 �J���[�ւ̃L���X�g���Z�q�ł��B
  return c_u8;
}

ColorF32* ColorF32Lerp(           // pOut��Ԃ�
    ColorF32*       pOut,         // �v�Z���ʂ��󂯎��Bp1, p2�Ɠ����ł��\��Ȃ��B
    const ColorF32* p1,
    const ColorF32* p2,
    f32             t             // 0.0f�ł����p1���A1.0f�ł����p2�����ʂƂȂ�B
)
{
  pOut->r = p1->r*(1.0f-t) + p2->r*t;
  pOut->g = p1->g*(1.0f-t) + p2->g*t;
  pOut->b = p1->b*(1.0f-t) + p2->b*t;
  pOut->a = p1->a*(1.0f-t) + p2->a*t;
  return pOut;
}

ColorF32* ColorF32Scale(           // pOut��Ԃ�
    ColorF32*       pOut,          // �v�Z���ʂ��󂯎��Bp�Ɠ����ł��\��Ȃ��B
    const ColorF32* p,
    f32             scale          // �|���鐔
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

#ifndef __GFL_MATHTYPE_H__
#define __GFL_MATHTYPE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MathType.h
 *	@brief  演算型定義
 *	@author	tomoya takahashi
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nn/math.h>
#include <nw/math.h>

#include <math/gfl_MathExtendedVectorTemplate.h>

namespace gfl {
namespace math {

//-----------------------------------------------------------------------------
/**
 *					ベクトル
*/
//-----------------------------------------------------------------------------
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nn::math::VEC2> >			VEC2;
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nn::math::VEC3> >			VEC3;
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nn::math::VEC4> >			VEC4;


//-----------------------------------------------------------------------------
/**
 *					行列
*/
//-----------------------------------------------------------------------------
typedef nn::math::MTX22 MTX22;
typedef nn::math::MTX23 MTX23;
typedef nn::math::MTX33 MTX33;
typedef nn::math::MTX34 MTX34;
typedef nn::math::MTX43 MTX43;
typedef nn::math::MTX44 MTX44;


//-----------------------------------------------------------------------------
/**
 *					クオータニオン
*/
//-----------------------------------------------------------------------------
typedef nn::math::QUAT QUAT;


} /* end of namespace math */
} /* end of namespace gfl */
#endif // __GFL_MATHTYPE_H__

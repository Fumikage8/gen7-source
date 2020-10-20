#ifndef __GFL2_MATHTYPE_H__
#define __GFL2_MATHTYPE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_MathType.h
 *	@brief  演算型定義
 *	@author	tomoya takahashi
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//#include <nn.h>
//#include <nn/math.h>
#include <nw/math.h>

#include "gfl2_MathExtendedVectorTemplate.h"

namespace gfl2 {
namespace math {

//-----------------------------------------------------------------------------
/**
 *					ベクトル
*/
//-----------------------------------------------------------------------------
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nw::math::VEC2> >			VEC2;
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nw::math::VEC3> >			VEC3;
typedef ExtendedVectorTemplate< SpecializedExtendedVectorTemplate<nw::math::VEC4> >			VEC4;


//-----------------------------------------------------------------------------
/**
 *					行列
*/
//-----------------------------------------------------------------------------
typedef nw::math::MTX22 MTX22;
typedef nw::math::MTX23 MTX23;
typedef nw::math::MTX33 MTX33;
typedef nw::math::MTX34 MTX34;
typedef nw::math::MTX43 MTX43;
typedef nw::math::MTX44 MTX44;


//-----------------------------------------------------------------------------
/**
 *					クオータニオン
*/
//-----------------------------------------------------------------------------
typedef nw::math::QUAT QUAT;


} /* end of namespace math */
} /* end of namespace gfl2 */
#endif // __GFL2_MATHTYPE_H__

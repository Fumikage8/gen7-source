#ifndef __GFL_QUATERNION_H__
#define __GFL_QUATERNION_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Quaternion.h
 *	@brief  クオータニオン演算　関数定義
 *	@author	tomoya takahashi
 *	@date		2010.10.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn/math.h>

#include <math/gfl_MathType.h>


namespace gfl {
namespace math {


//-----------------------------------------------------------------------------
/**
 *        QUATERNION
 */
//-----------------------------------------------------------------------------
inline QUAT* QUATAdd( QUAT* pOut, const QUAT* q1, const QUAT* q2) { return nn::math::QUATAdd( pOut,q1,q2); }
inline QUAT* QUATSub( QUAT* pOut, const QUAT* q1, const QUAT* q2) { return nn::math::QUATSub( pOut,q1,q2); }
inline QUAT* QUATDivide( QUAT* pOut, const QUAT* q1, const QUAT* q2) { return nn::math::QUATDivide( pOut,q1,q2); }
inline QUAT* QUATMult( QUAT* pOut, const QUAT* q1, const QUAT* q2) { return nn::math::QUATMult( pOut,q1,q2); }
inline f32 QUATDot( const QUAT* q1, const QUAT* q2) { return nn::math::QUATDot( q1,q2); }
inline QUAT* QUATInverse( QUAT* pOut, const QUAT* q) { return nn::math::QUATInverse( pOut,q); }
inline QUAT* QUATScale( QUAT* pOut, const QUAT* q, f32 scale) { return nn::math::QUATScale( pOut,q,scale); }
inline QUAT* QUATNormalize( QUAT* pOut, const QUAT* q) { return nn::math::QUATNormalize( pOut,q); }
inline QUAT* QUATExp( QUAT* pOut, const QUAT* q) { return nn::math::QUATExp( pOut,q); }
inline QUAT* QUATLogN( QUAT* pOut, const QUAT* q) { return nn::math::QUATLogN( pOut,q); }
inline QUAT* QUATLerp( QUAT* pOut, const QUAT* q1, const QUAT* q2, f32 t) { return nn::math::QUATLerp( pOut,q1,q2,t); }
inline QUAT* QUATSlerp( QUAT* pOut, const QUAT* q1, const QUAT* q2, f32 t) { return nn::math::QUATSlerp( pOut,q1,q2,t); }
inline QUAT* QUATSquad( QUAT* pOut, const QUAT* p, const QUAT* a, const QUAT* b, const QUAT* q, f32 t) { return nn::math::QUATSquad( pOut,p,a,b,q,t); }

inline QUAT* QUATMakeClosest(  QUAT* pOut, const QUAT *q, const QUAT *qto ) { return nn::math::QUATMakeClosest( pOut,*q,*qto); }
inline QUAT* QUATRotAxisRad(  QUAT* pOut, const VEC3 *axis, f32 rad ) { return nn::math::QUATRotAxisRad( pOut,*axis,rad); }


  
} /* end of namespace math */
} /* end of namespace gfl */


#endif // __GFL_QUATERNION_H__

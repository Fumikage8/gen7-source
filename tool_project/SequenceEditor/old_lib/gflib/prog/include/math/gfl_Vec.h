#ifndef __GFL_VEC_H__
#define __GFL_VEC_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Vec.h
 *	@brief  ベクトル演算    関数定義
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
 *        VEC2
 */
//-----------------------------------------------------------------------------
inline bool VEC2IsZero( const VEC2* p) { return nn::math::VEC2IsZero( p); }

inline VEC2* VEC2Add( VEC2* pOut, const VEC2* p1, const VEC2* p2) { return reinterpret_cast<VEC2*>(nn::math::VEC2Add( pOut,p1,p2)); }
inline VEC2* VEC2Sub( VEC2* pOut, const VEC2* p1, const VEC2* p2) { return reinterpret_cast<VEC2*>(nn::math::VEC2Sub( pOut,p1,p2)); }
inline VEC2* VEC2Mult( VEC2* pOut, const VEC2* p1, const VEC2* p2) { return reinterpret_cast<VEC2*>(nn::math::VEC2Mult( pOut,p1,p2)); }
inline VEC2* VEC2Scale( VEC2* pOut, const VEC2* p, f32 scale) { return reinterpret_cast<VEC2*>(nn::math::VEC2Scale( pOut,p,scale)); }
inline VEC2* VEC2Lerp( VEC2* pOut, const VEC2* p1, const VEC2* p2, f32 t) { return reinterpret_cast<VEC2*>(nn::math::VEC2Lerp( pOut,p1,p2,t)); }
inline f32 VEC2Dot( const VEC2* p1, const VEC2* p2) { return nn::math::VEC2Dot( p1,p2); }
inline VEC2* VEC2Maximize( VEC2* pOut, const VEC2* p1, const VEC2* p2) { return reinterpret_cast<VEC2*>(nn::math::VEC2Maximize( pOut,p1,p2)); }
inline VEC2* VEC2Minimize( VEC2* pOut, const VEC2* p1, const VEC2* p2) { return reinterpret_cast<VEC2*>(nn::math::VEC2Minimize( pOut,p1,p2)); }
inline f32   VEC2Len(const VEC2* p) { return nn::math::VEC2Len(p); }
inline f32   VEC2LenSq(const VEC2* p) { return nn::math::VEC2LenSq(p); }
inline VEC2* VEC2Normalize( VEC2* pOut, const VEC2* p) { return reinterpret_cast<VEC2*>(nn::math::VEC2Normalize( pOut,p)); }
inline VEC2* VEC2SafeNormalize( VEC2* pOut, const VEC2* p, const VEC2& alt) { return reinterpret_cast<VEC2*>(nn::math::VEC2SafeNormalize( pOut,p,alt)); }
inline f32 VEC2DistSq( const VEC2* p1, const VEC2* p2) { return nn::math::VEC2DistSq( p1,p2); }
inline VEC2* VEC2Transform( VEC2* pOut, const MTX23* pM, const VEC2* pV) { return reinterpret_cast<VEC2*>(nn::math::VEC2Transform( pOut,pM,pV)); }

extern const VEC2 VEC2_ZERO;
extern const VEC2 VEC2_UNIT_SCALE;

//-----------------------------------------------------------------------------
/**
 *        VEC3
 */
//-----------------------------------------------------------------------------
inline void VEC3Set(VEC3* pOut, const float value) { pOut->x = pOut->y = pOut->z = value; }
inline void VEC3Set(VEC3* pOut, const VEC4& v) { pOut->x = v.x, pOut->y = v.y, pOut->z = v.z; }
inline bool VEC3IsZero( const VEC3* p) { return nn::math::VEC3IsZero( p); }
inline VEC3* VEC3Maximize( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Maximize( pOut,p1,p2)); }
inline VEC3* VEC3Minimize( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Minimize( pOut,p1,p2)); }
inline VEC3* VEC3Cross( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Cross( pOut,p1,p2)); }
inline VEC3* VEC3Normalize( VEC3* pOut, const VEC3* p) { return reinterpret_cast<VEC3*>(nn::math::VEC3Normalize( pOut,p)); }
inline VEC3* VEC3SafeNormalize( VEC3* pOut, const VEC3* p, const VEC3& alt) { return reinterpret_cast<VEC3*>(nn::math::VEC3SafeNormalize( pOut,p,alt)); }
inline f32 VEC3SquareDist( const VEC3* p1, const VEC3* p2) { return nn::math::VEC3SquareDist( p1,p2); }

inline VEC3* VEC3Add( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Add( pOut,p1,p2)); }
inline VEC3* VEC3Sub( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Sub( pOut,p1,p2)); }
inline VEC3* VEC3Mult( VEC3* pOut, const VEC3* p1, const VEC3* p2) { return reinterpret_cast<VEC3*>(nn::math::VEC3Mult( pOut,p1,p2)); }
inline VEC3* VEC3Scale( VEC3* pOut, const VEC3* p, f32 scale) { return reinterpret_cast<VEC3*>(nn::math::VEC3Scale( pOut,p,scale)); }
inline VEC3* VEC3Lerp( VEC3* pOut, const VEC3* p1, const VEC3* p2, f32 t) { return reinterpret_cast<VEC3*>(nn::math::VEC3Lerp( pOut,p1,p2,t)); }
inline f32 VEC3Dot( const VEC3* p1, const VEC3* p2) { return nn::math::VEC3Dot( p1,p2); }
inline f32 VEC3Len( const VEC3* p) { return nn::math::VEC3Len( p); }
inline f32 VEC3SquareLen( const VEC3* p) { return nn::math::VEC3SquareLen( p); }
inline f32 VEC3Dist( const VEC3* p1, const VEC3* p2) { return nn::math::VEC3Dist( p1,p2); }

extern const VEC3 VEC3_ZERO;
extern const VEC3 VEC3_UNIT_X;
extern const VEC3 VEC3_UNIT_Y;
extern const VEC3 VEC3_UNIT_Z;
extern const VEC3 VEC3_UNIT_SCALE;

//-----------------------------------------------------------------------------
/**
 *        VEC4
 */
//-----------------------------------------------------------------------------
inline bool VEC4IsZero( const VEC4* p) { return nn::math::VEC4IsZero( p); }
inline bool VEC4IsZeroWOne( const VEC4* p) { return nn::math::VEC4IsZeroWOne( p); }
inline VEC4* VEC4Add( VEC4* pOut, const VEC4* p1, const VEC4* p2) { return reinterpret_cast<VEC4*>(nn::math::VEC4Add( pOut,p1,p2)); }
inline VEC4* VEC4Sub( VEC4* pOut, const VEC4* p1, const VEC4* p2) { return reinterpret_cast<VEC4*>(nn::math::VEC4Sub( pOut,p1,p2)); }
inline VEC4* VEC4Mult( VEC4* pOut, const VEC4* p1, const VEC4* p2) { return reinterpret_cast<VEC4*>(nn::math::VEC4Mult( pOut,p1,p2)); }
inline VEC4* VEC4Scale( VEC4* pOut, const VEC4* p, f32 scale) { return reinterpret_cast<VEC4*>(nn::math::VEC4Scale( pOut,p,scale)); }
inline VEC4* VEC4Lerp( VEC4* pOut, const VEC4* p1, const VEC4* p2, f32 t) { return reinterpret_cast<VEC4*>(nn::math::VEC4Lerp( pOut,p1,p2,t)); }
inline f32 VEC4Dot( const VEC4* p1, const VEC4* p2) { return nn::math::VEC4Dot( p1,p2); }
inline f32 VEC4LenSq( const VEC4* p) { return nn::math::VEC4LenSq( p); }
inline f32 VEC4Len( const VEC4* p) { return nn::math::VEC4Len( p); }
inline VEC4* VEC4Normalize( VEC4* pOut, const VEC4* p) { return reinterpret_cast<VEC4*>(nn::math::VEC4Normalize( pOut,p)); }
inline VEC4* VEC4SafeNormalize( VEC4* pOut, const VEC4* p, const VEC4& alt) { return reinterpret_cast<VEC4*>(nn::math::VEC4SafeNormalize( pOut,p,alt)); }
inline f32 VEC4DistSq( const VEC4* p1, const VEC4* p2) { return nn::math::VEC4DistSq( p1,p2); }
inline VEC4* VEC4Maximize( VEC4* pOut, const VEC4* p1, const VEC4* p2) { return reinterpret_cast<VEC4*>(nn::math::VEC4Maximize( pOut,p1,p2)); }
inline VEC4* VEC4Minimize( VEC4* pOut, const VEC4* p1, const VEC4* p2) { return reinterpret_cast<VEC4*>(nn::math::VEC4Minimize( pOut,p1,p2)); }

extern const VEC4 VEC4_UNIT_Y;

} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_VEC_H__

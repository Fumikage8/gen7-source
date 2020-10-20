#ifndef __GFL2_MATHEXTENDEDVECTORTEMPLATE_H__
#define __GFL2_MATHEXTENDEDVECTORTEMPLATE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_MathExtendedVectorTemplate.h
 *	@brief  ベクター拡張テンプレート
 *	@author	masateru ishiguro
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//#include <nn.h>
//#include <nn/math.h>
#include <nw/math.h>

namespace gfl2 {
namespace math {

/**
 * 拡張テンプレートのデフォルト宣言
 */
template <class Implement_>
class SpecializedExtendedVectorTemplate : public Implement_
{
public:
	SpecializedExtendedVectorTemplate() : Implement_(){}
};

/**
 * nw::math::VEC2に特殊化した拡張クラス
 */
template <>
class SpecializedExtendedVectorTemplate<nw::math::VEC2> : public nw::math::VEC2
{
public:
	SpecializedExtendedVectorTemplate() : nw::math::VEC2(){}
	SpecializedExtendedVectorTemplate(const f32* p):nw::math::VEC2(p){}
	SpecializedExtendedVectorTemplate(const SpecializedExtendedVectorTemplate& v):nw::math::VEC2(v){}
	SpecializedExtendedVectorTemplate(f32 fx, f32 fy):nw::math::VEC2(fx, fy){}
	SpecializedExtendedVectorTemplate(const nw::math::VEC2& v):nw::math::VEC2(v){}
};

/**
 * nw::math::VEC3に特殊化した拡張クラス
 */
template <>
class SpecializedExtendedVectorTemplate<nw::math::VEC3> : public nw::math::VEC3
{
public:
	SpecializedExtendedVectorTemplate() : nw::math::VEC3(){}
	SpecializedExtendedVectorTemplate(const f32* p):nw::math::VEC3(p){}
	SpecializedExtendedVectorTemplate(const SpecializedExtendedVectorTemplate& v):nw::math::VEC3(v){}
	SpecializedExtendedVectorTemplate(f32 fx, f32 fy, f32 fz):nw::math::VEC3(fx, fy, fz){}
	SpecializedExtendedVectorTemplate(const nw::math::VEC3& v):nw::math::VEC3(v){}
};

/**
 * nw::math::VEC4に特殊化した拡張クラス
 */
template <>
class SpecializedExtendedVectorTemplate<nw::math::VEC4> : public nw::math::VEC4
{
public:
	SpecializedExtendedVectorTemplate() : nw::math::VEC4(){}
	SpecializedExtendedVectorTemplate(const f32* p):nw::math::VEC4(p){}
	SpecializedExtendedVectorTemplate(const SpecializedExtendedVectorTemplate& v):nw::math::VEC4(v){}
	SpecializedExtendedVectorTemplate(f32 fx, f32 fy, f32 fz, f32 fw = 1.0f):nw::math::VEC4(fx, fy, fz, fw){}
	SpecializedExtendedVectorTemplate(const nw::math::VEC3& v):nw::math::VEC4(v){}
	SpecializedExtendedVectorTemplate(const nw::math::VEC4& v):nw::math::VEC4(v){}

	//! @brief ベクトルの外積を計算し設定します。
	//!
	//! @brief lhs 外積を計算する左辺ベクトルです。
	//! @brief rhs 外積を計算する右辺ベクトルです。
	SpecializedExtendedVectorTemplate& Cross(const SpecializedExtendedVectorTemplate& lhs, const SpecializedExtendedVectorTemplate& rhs)
	{
		return *VEC4Cross(this, &lhs, &rhs);
	}

	SpecializedExtendedVectorTemplate* VEC4Cross(SpecializedExtendedVectorTemplate* pOut, const SpecializedExtendedVectorTemplate* p1, const SpecializedExtendedVectorTemplate* p2)
	{
		SpecializedExtendedVectorTemplate tmpVec;

		tmpVec.x = ( p1->y * p2->z ) - ( p1->z * p2->y );
		tmpVec.y = ( p1->z * p2->x ) - ( p1->x * p2->z );
		tmpVec.z = ( p1->x * p2->y ) - ( p1->y * p2->x );

		pOut->x = tmpVec.x;
		pOut->y = tmpVec.y;
		pOut->z = tmpVec.z;
		pOut->w = 1.0f;

		return pOut;
	}

	void Set( f32 x, f32 y, f32 z, f32 w = 1.0f )
	{
		nw::math::VEC4::Set(x, y, z, w);
	}

};

/**
 * ベクター汎用拡張テンプレートクラス
 */
template <class Implement_>
class ExtendedVectorTemplate : public Implement_
{
public:
	ExtendedVectorTemplate() : Implement_(){}
	ExtendedVectorTemplate(const Implement_& v) : Implement_(v){}
	ExtendedVectorTemplate(const f32* p) : Implement_(p){}
	ExtendedVectorTemplate(f32 fx, f32 fy ) : Implement_(fx, fy){}
	ExtendedVectorTemplate(f32 fx, f32 fy, f32 fz ) : Implement_(fx, fy, fz){}
	ExtendedVectorTemplate(f32 fx, f32 fy, f32 fz, f32 fw ) : Implement_(fx, fy, fz, fw){}

	ExtendedVectorTemplate(const nw::math::VEC2& v) : Implement_(v){}
	ExtendedVectorTemplate(const nw::math::VEC3& v) : Implement_(v){}
	ExtendedVectorTemplate(const nw::math::VEC4& v) : Implement_(v){}
};

} /* end of namespace math */
} /* end of namespace gfl2 */
#endif // __GFL2_MATHEXTENDEDVECTORTEMPLATE_H__

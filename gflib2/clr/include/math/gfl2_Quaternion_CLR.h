#ifndef GFLIB2_MATH_CLR_QUATERNION_CLR_H_INCLUDED
#define GFLIB2_MATH_CLR_QUATERNION_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <math/include/gfl2_Quaternion.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>

namespace gfl2 { namespace clr { namespace math {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		クォータニオンクラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Quaternion
{
public:

	//! @brief デフォルトコンストラクタ
	Quaternion();
	Quaternion( f32 x, f32 y, f32 z, f32 w);
	Quaternion( Quaternion^ other );
	Quaternion( gfl2::math::Quaternion& other );

	~Quaternion();
	!Quaternion();

	gfl2::math::Quaternion* GetNative(){ return m_pNativeQuaternion; }

	f32 GetElemX();
	f32 GetElemY();
	f32 GetElemZ();
	f32 GetElemW();

	void SetElem(f32 x, f32 y, f32 z, f32 w);

	void Conj( Quaternion^ q );
	void Conj();
	Quaternion^ GetConj();

	void Neg();
	void Neg( Quaternion^ t );
	Quaternion^ GetNeg();

	void Mul( Quaternion^ t, Quaternion^ s );
	void Mul( Quaternion^ t );
	f32 Dot( Quaternion^ t );

	void Inverse( Quaternion^ t );
	void Inverse();

	Quaternion^ operator-();

	void Slerp( Quaternion^ t, Quaternion^ s, f32 rate );
	void Slerp( Quaternion^ t, f32 rate );

	void RadianToQuaternion( f32 x, f32 y, f32 z );
	void RadianYXZToQuaternion( f32 x, f32 y, f32 z );
	void RadianToQuaternion( Vector^ v );

	void RadianYXZToQuaternion( Vector^ v );

	void RadianYToQuaternion( f32 yang );

	//void QuaternionToRadian( f32^ x, f32^ y, f32^ z );
	//void QuaternionToRadianYXZ( f32^ x, f32^ y, f32^ z );

	void QuaternionToRadianYXZ( Vector^ v );
	f32 QuaternionToRadianY();

	void QuaternionToRadian( Vector^ v );
	Vector^ QuaternionToRadian();

	void MatrixToQuaternion( Matrix^ mat );
	void QuaternionToMatrix( Matrix^ mat );

	Quaternion^ operator*=( Quaternion^ t );
	Quaternion^ operator*( Quaternion^ t );

	void Sub( Quaternion^ t );
	void Sub( Quaternion^ t, Quaternion^ s );

	f32 Magnitute2();
	f32 Magnitude2Recip();

	f32 Magnitude();
	f32 MagnitudeRecip();

	void Normalize();
	void Normalize( Quaternion^ t );
	f32 NormalizeRet();

	Quaternion^ operator-=( Quaternion^ t );
	Quaternion^ operator-( Quaternion^ t );

#if 0
	// 以下, gfl_QuaternionNative.hで定義されている演算子
	// 対応は後で考える
	Quaternion^ operator+=( Quaternion^ t );
	Quaternion^ operator*=( f32 f );
	Quaternion^ operator/=( f32 f );
	Quaternion^ operator+();
	Quaternion^ operator+( Quaternion^ t );
	Quaternion^ operator*( f32 f );
	Quaternion^ operator/( f32 f );
	Quaternion^ operator==( Quaternion^ t );
	Quaternion^ operator!=( Quaternion^ t );
#endif

private:
	gfl2::math::Quaternion		*m_pNativeQuaternion;
};

}}}

#endif
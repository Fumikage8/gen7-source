#ifndef GFLIB2_MATH_CLR_VECTOR_CLR_H_INCLUDED
#define GFLIB2_MATH_CLR_VECTOR_CLR_H_INCLUDED

#using <System.Drawing.dll>

#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Vector.h>

namespace gfl2 { namespace clr { namespace math { 

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		ベクタークラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Vector
{
public:

	//! @brief デフォルトコンストラクタ
	Vector();
	Vector( Vector^ other );
	Vector( const gfl2::math::Vector4& other );
	Vector( const gfl2::math::Vector& other );

	Vector( float x, float y, float z );
	//! @brief float×４からベクトルを作成
	Vector( float x, float y, float z, float w );

	~Vector();
	!Vector();

	property float x { float get( void ); void set( float value ); }
	property float y { float get( void ); void set( float value ); }
	property float z { float get( void ); void set( float value ); }
	property float w { float get( void ); void set( float value ); }
	property float default [ int ]{ float get( int index ); void set( int index, float value );	}
	gfl2::math::Vector4* GetNative(){ return m_pNativeVec; }
	gfl2::math::Vector3 GetNativeVector3(){ 
    gfl2::math::Vector4* native = this->GetNative();
    return gfl2::math::Vector3(native->x, native->y, native->z); 
  }
	gfl2::math::Vector4 GetNativeOldVector(){ 
    gfl2::math::Vector4* native = this->GetNative();
    return gfl2::math::Vector4(native->x, native->y, native->z, native->w); 
  }

	//! 要素の設定
	Vector^  Set( float x, float y, float z, float w );

	Vector^  operator =( Vector^ rhs );

	//! スカラ乗算
	static Vector^ operator * ( Vector^ vec, float t );
	//! スカラ除算
	static Vector^ operator / ( Vector^ vec, float rhs );
	//! スカラ乗算代入
	static Vector^  operator *= ( Vector^ vec, float rhs );
	//! スカラ除算代入
	static Vector^  operator /= ( Vector^ vec, float rhs );
	//! negate
	static Vector^ operator -( Vector^ vec );

	//! 加算
	static Vector^ operator +( Vector^ vec, Vector^ rhs );
	//! 減算
	static Vector^ operator -( Vector^ vec, Vector^ rhs );
	//! 加算代入
	static Vector^  operator +=( Vector^ vec, Vector^ rhs );
	//! 減算代入
	static Vector^  operator -=( Vector^ vec, Vector^ rhs );

	//!  ベクトル内積
	float Dot( Vector^ rhs );
	float Dot3( Vector^ rhs );
	//!  ベクトル外積
	Vector^ Cross( Vector^ rhs );
	//!  ベクトルの長さ
	float Length();
	float Length3();	
	//! @brief  ４次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	Vector^ Normalize();
	Vector^ Normalize3();

	//! X軸単位ベクトル(1,0,0,0)の取得
	static Vector^ GetXUnit( );
	//! Y軸単位ベクトル(0,1,0,0)の取得
	static Vector^ GetYUnit( );
	//! Z軸単位ベクトル(0,0,1,0)の取得
	static Vector^ GetZUnit( );
	//! W軸単位ベクトル(0,0,0,1)の取得
	static Vector^ GetWUnit( );
	//! ゼロベクトル(0,0,0,0)を取得
	static Vector^ GetZero( );

private:
	gfl2::math::Vector4		*m_pNativeVec;
};


}}}

#endif
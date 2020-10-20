#if !defined(GFLIB2_MATH_CLR_MATRIX_CLR_H_INCLUDED)
#define GFLIB2_MATH_CLR_MATRIX_CLR_H_INCLUDED
#pragma once

#using <System.Drawing.dll>

#include <clr/include/math/gfl2_Vector_CLR.h>
#include <math/include/gfl2_Matrix44.h>
#include <math/include/gfl2_Matrix.h>

namespace gfl2 { namespace clr { namespace math {

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■
■		行列クラス
■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
public ref class Matrix
{
public:

	//! @brief デフォルトコンストラクタ
	Matrix();
	Matrix( Matrix^  rhs );
	Matrix( const gfl2::math::Matrix44& rhs );
	Matrix( Vector^ col0_, Vector^ col1_, Vector^ col2_, Vector^ col3_ );

	~Matrix();
	!Matrix();

	//propertyでのアクセッサは書けない。
	gfl2::math::Matrix44* GetNative(){ return m_pNativeMat; }
  gfl2::math::Matrix34 GetNativeMatrix34(){
    gfl2::math::Matrix34 m = *(gfl2::math::Matrix34*)this->GetNative();
    return m;
  }
	gfl2::math::Matrix GetNativeOldMatrix(){
    gfl2::math::Matrix m = *(gfl2::math::Matrix*)this->GetNative();
    return m.Transpose();
  }

	//! 代入
	Matrix^  operator =( Matrix^ rhs );
	//! 要素の設定
	Matrix^  SetElem( int col, int row, float val );
	//! 要素の取得
	float GetElem( int col, int row );

	//! 加算
	static Matrix^ operator +( Matrix^ me, Matrix^ rhs );
	//! 減算
	static Matrix^ operator -( Matrix^ me, Matrix^ rhs );
	//! negate
	static Matrix^ operator -( Matrix^ me );

	//! スカラ乗算
	static Matrix^ operator *( Matrix^ me, float rhs );
	//! スカラ乗算代入
	static Matrix^  operator *=( Matrix^ me, float rhs );

	//! ベクトルとの乗算
	static Vector^ operator *( Matrix^ me, Vector^ rhs );
	//! マトリックスとの乗算
	static Matrix^ operator *( Matrix^ me, Matrix^ rhs );

	//! 加算代入
	static Matrix^  operator +=( Matrix^ me, Matrix^  rhs );
	//! 減算代入
	static Matrix^  operator -=( Matrix^ me, Matrix^  rhs );
	//! 乗算代入
	static Matrix^  operator *=( Matrix^ me, Matrix^  rhs );

	//! マトリクスの転置
	Matrix^ Transpose( void );
	//! 逆マトリクス
	Matrix^ Inverse( void );
	//! 逆マトリクス( アフィン変換マトリクス用  高速版 )
	Matrix^ AffineInverse( void );

	//==========================================
	//! 単位マトリクス
	static Matrix^ GetIdentity( );
	//! ゼロマトリクス
	static Matrix^ GetZero();
	//! X軸回転マトリクス
	static Matrix^ GetRotationX( float radians );
	//! Y軸回転マトリクス
	static Matrix^ GetRotationY( float radians );
	//! Z軸回転マトリクス
	static Matrix^ GetRotationZ( float radians );
	//! Z * Y * X 回転マトリクス
	static Matrix^ GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! スケーリングマトリクス
	static Matrix^ GetScaling( Vector^ scale );
	//! 平行移動マトリクス
	static Matrix^ GetTranslation( Vector^ translation );
	//! OpenGL互換 LookAt
	static Matrix^ GetLookAt( Vector^ eye, Vector^ aim, Vector^ up );
	//! Perspective
	static Matrix^ GetPerspective( float fovy, float aspect, float zNear, float zFar );
	//! Frustum
	static Matrix^ GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho
	static Matrix^ GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar );

private:

	gfl2::math::Matrix44		*m_pNativeMat;
		
};

}}}

#endif
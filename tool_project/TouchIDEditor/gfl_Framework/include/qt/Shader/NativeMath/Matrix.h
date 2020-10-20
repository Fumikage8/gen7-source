#pragma once
#if !defined( TACO_NATIVEMATH_MATRIX_H_INCLUDED )
#define TACO_NATIVEMATH_MATRIX_H_INCLUDED

#include "Vector.h"

namespace gfl { namespace maya { namespace Math {

class Matrix
{
public:
	Matrix();
	Matrix( const Matrix&  rhs );
	Matrix( const Vector& col0_, const Vector& col1_, const Vector& col2_, const Vector& col3_ );

	~Matrix();

	//! 代入
	Matrix&  operator =( const Matrix&  rhs );

	//! 配列アクセス
	Vector&  operator []( int col );
	//! 配列アクセス
	const Vector& operator []( int col ) const;
	//! 要素の設定
	Matrix&  SetElem( int col, int row, float val );
	//! 要素の取得
	float GetElem( int col, int row ) const;

	//! 加算
	Matrix operator +( const Matrix&  rhs ) const;
	//! 減算
	Matrix operator -( const Matrix&  rhs ) const;
	//! negate
	Matrix operator -( ) const;

	//! スカラ乗算
	Matrix operator *( float rhs ) const;
	//! スカラ乗算代入
	Matrix&  operator *=( float rhs );

	//! ベクトルとの乗算
	Vector operator *( const Vector& rhs ) const;
	//! マトリックスとの乗算
	Matrix operator *( const Matrix&  rhs ) const;

	//! 加算代入
	Matrix&  operator +=( const Matrix&  rhs );
	//! 減算代入
	Matrix&  operator -=( const Matrix&  rhs );
	//! 乗算代入
	Matrix&  operator *=( const Matrix&  rhs );

	//! マトリクスの転置
	Matrix Transpose( void ) const;
	//! 逆マトリクス
	Matrix Inverse( void ) const;
	//! 逆マトリクス( アフィン変換マトリクス用  高速版 )
	Matrix AffineInverse( void ) const;

	//! 単位マトリクス
	static Matrix GetIdentity( );
	//! ゼロマトリクス
	static Matrix GetZero();
	//! X軸回転マトリクス
	static Matrix GetRotationX( float radians );
	//! Y軸回転マトリクス
	static Matrix GetRotationY( float radians );
	//! Z軸回転マトリクス
	static Matrix GetRotationZ( float radians );
	//! Z * Y * X 回転マトリクス
	static Matrix GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! スケーリングマトリクス
	static Matrix GetScaling( const Vector& scale );
	//! 平行移動マトリクス
	static Matrix GetTranslation( const Vector& translation );
	//! OpenGL互換 LookAt
	static Matrix GetLookAt( const Vector& eye, const Vector& aim, const Vector& up );
	//! Perspective
	static Matrix GetPerspective( float fovy, float aspect, float zNear, float zFar );
	//! Frustum
	static Matrix GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho
	static Matrix GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar );

private:

	//! Perspective OpenGL互換(Depth=-1.0〜+1.0に射影)
	static Matrix GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );
	//! Perspective DirectX互換(Depth=0.0〜+1.0に射影)
	static Matrix GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar );
	
	//! Frustum OpenGL互換(Depth=-1.0〜+1.0に射影)
	static Matrix GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! Frustum DirectX互換(Depth=0.0〜+1.0に射影)
	static Matrix GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar );
	
	//! ortho OpenGL互換(Depth=-1.0〜+1.0に射影)
	static Matrix GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho DirectX互換(Depth=0.0〜+1.0に射影)
	static Matrix GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar );

	union
	{
		struct
		{
			Vector		col0;
			Vector		col1;
			Vector		col2;
			Vector		col3;
		};

		struct
		{
			Vector		col[4];
		};
		
	};

	
};

}}}

#endif
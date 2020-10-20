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

	//! ãü
	Matrix&  operator =( const Matrix&  rhs );

	//! zñANZX
	Vector&  operator []( int col );
	//! zñANZX
	const Vector& operator []( int col ) const;
	//! vfÌÝè
	Matrix&  SetElem( int col, int row, float val );
	//! vfÌæ¾
	float GetElem( int col, int row ) const;

	//! ÁZ
	Matrix operator +( const Matrix&  rhs ) const;
	//! ¸Z
	Matrix operator -( const Matrix&  rhs ) const;
	//! negate
	Matrix operator -( ) const;

	//! XJæZ
	Matrix operator *( float rhs ) const;
	//! XJæZãü
	Matrix&  operator *=( float rhs );

	//! xNgÆÌæZ
	Vector operator *( const Vector& rhs ) const;
	//! }gbNXÆÌæZ
	Matrix operator *( const Matrix&  rhs ) const;

	//! ÁZãü
	Matrix&  operator +=( const Matrix&  rhs );
	//! ¸Zãü
	Matrix&  operator -=( const Matrix&  rhs );
	//! æZãü
	Matrix&  operator *=( const Matrix&  rhs );

	//! }gNXÌ]u
	Matrix Transpose( void ) const;
	//! t}gNX
	Matrix Inverse( void ) const;
	//! t}gNX( AtBÏ·}gNXp  ¬Å )
	Matrix AffineInverse( void ) const;

	//! PÊ}gNX
	static Matrix GetIdentity( );
	//! [}gNX
	static Matrix GetZero();
	//! X²ñ]}gNX
	static Matrix GetRotationX( float radians );
	//! Y²ñ]}gNX
	static Matrix GetRotationY( float radians );
	//! Z²ñ]}gNX
	static Matrix GetRotationZ( float radians );
	//! Z * Y * X ñ]}gNX
	static Matrix GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! XP[O}gNX
	static Matrix GetScaling( const Vector& scale );
	//! ½sÚ®}gNX
	static Matrix GetTranslation( const Vector& translation );
	//! OpenGLÝ· LookAt
	static Matrix GetLookAt( const Vector& eye, const Vector& aim, const Vector& up );
	//! Perspective
	static Matrix GetPerspective( float fovy, float aspect, float zNear, float zFar );
	//! Frustum
	static Matrix GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho
	static Matrix GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar );

private:

	//! Perspective OpenGLÝ·(Depth=-1.0`+1.0ÉËe)
	static Matrix GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );
	//! Perspective DirectXÝ·(Depth=0.0`+1.0ÉËe)
	static Matrix GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar );
	
	//! Frustum OpenGLÝ·(Depth=-1.0`+1.0ÉËe)
	static Matrix GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! Frustum DirectXÝ·(Depth=0.0`+1.0ÉËe)
	static Matrix GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar );
	
	//! ortho OpenGLÝ·(Depth=-1.0`+1.0ÉËe)
	static Matrix GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho DirectXÝ·(Depth=0.0`+1.0ÉËe)
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
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

	//! ���
	Matrix&  operator =( const Matrix&  rhs );

	//! �z��A�N�Z�X
	Vector&  operator []( int col );
	//! �z��A�N�Z�X
	const Vector& operator []( int col ) const;
	//! �v�f�̐ݒ�
	Matrix&  SetElem( int col, int row, float val );
	//! �v�f�̎擾
	float GetElem( int col, int row ) const;

	//! ���Z
	Matrix operator +( const Matrix&  rhs ) const;
	//! ���Z
	Matrix operator -( const Matrix&  rhs ) const;
	//! negate
	Matrix operator -( ) const;

	//! �X�J����Z
	Matrix operator *( float rhs ) const;
	//! �X�J����Z���
	Matrix&  operator *=( float rhs );

	//! �x�N�g���Ƃ̏�Z
	Vector operator *( const Vector& rhs ) const;
	//! �}�g���b�N�X�Ƃ̏�Z
	Matrix operator *( const Matrix&  rhs ) const;

	//! ���Z���
	Matrix&  operator +=( const Matrix&  rhs );
	//! ���Z���
	Matrix&  operator -=( const Matrix&  rhs );
	//! ��Z���
	Matrix&  operator *=( const Matrix&  rhs );

	//! �}�g���N�X�̓]�u
	Matrix Transpose( void ) const;
	//! �t�}�g���N�X
	Matrix Inverse( void ) const;
	//! �t�}�g���N�X( �A�t�B���ϊ��}�g���N�X�p  ������ )
	Matrix AffineInverse( void ) const;

	//! �P�ʃ}�g���N�X
	static Matrix GetIdentity( );
	//! �[���}�g���N�X
	static Matrix GetZero();
	//! X����]�}�g���N�X
	static Matrix GetRotationX( float radians );
	//! Y����]�}�g���N�X
	static Matrix GetRotationY( float radians );
	//! Z����]�}�g���N�X
	static Matrix GetRotationZ( float radians );
	//! Z * Y * X ��]�}�g���N�X
	static Matrix GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! �X�P�[�����O�}�g���N�X
	static Matrix GetScaling( const Vector& scale );
	//! ���s�ړ��}�g���N�X
	static Matrix GetTranslation( const Vector& translation );
	//! OpenGL�݊� LookAt
	static Matrix GetLookAt( const Vector& eye, const Vector& aim, const Vector& up );
	//! Perspective
	static Matrix GetPerspective( float fovy, float aspect, float zNear, float zFar );
	//! Frustum
	static Matrix GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho
	static Matrix GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar );

private:

	//! Perspective OpenGL�݊�(Depth=-1.0�`+1.0�Ɏˉe)
	static Matrix GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );
	//! Perspective DirectX�݊�(Depth=0.0�`+1.0�Ɏˉe)
	static Matrix GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar );
	
	//! Frustum OpenGL�݊�(Depth=-1.0�`+1.0�Ɏˉe)
	static Matrix GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! Frustum DirectX�݊�(Depth=0.0�`+1.0�Ɏˉe)
	static Matrix GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar );
	
	//! ortho OpenGL�݊�(Depth=-1.0�`+1.0�Ɏˉe)
	static Matrix GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho DirectX�݊�(Depth=0.0�`+1.0�Ɏˉe)
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
#pragma once
#if !defined( TACO_NATIVEMATH_VECTOR_H_INCLUDED )
#define TACO_NATIVEMATH_VECTOR_H_INCLUDED

namespace gfl { namespace maya { namespace Math {

class Vector
{
public:

	static const int			ElementNum = 4;

	//! @brief �f�t�H���g�R���X�g���N�^
	Vector( );
	//! @brief �R�s�[�R���X�g���N�^
	Vector( const Vector& other );

	//! @brief float�~�S����x�N�g�����쐬
	Vector( float x, float y, float z, float w = ((float) 1.0) );

	~Vector();

	//! �v�f�̐ݒ�
	Vector&  Set( float x, float y, float z, float w = ((float) 1.0) );

	Vector&  operator =( const Vector& rhs );

	float& operator []( int index );
	const float& operator []( int index ) const;
	
	//! �X�J����Z
	Vector operator *( float rhs ) const;
	//! �X�J�����Z
	Vector operator /( float rhs ) const;
	
	//! �X�J����Z���
	Vector&  operator *=( float rhs );
	//! �X�J�����Z���
	Vector&  operator /=( float rhs );

	//! ���Z
	Vector operator +( const Vector& rhs ) const;
	//! ���Z
	Vector operator -( const Vector& rhs ) const;
	//! ���Z���
	Vector&  operator +=( const Vector& rhs );
	Vector&  operator +=( const float& rhs );
	//! ���Z���
	Vector&  operator -=( const Vector& rhs );
	Vector&  operator -=( const float& rhs );

	//! negate
	Vector operator -( ) const;

	//! ��r
	bool  operator ==( const Vector& rhs ) const;
	bool  operator !=( const Vector& rhs ) const;

	//!  �x�N�g������
	float Dot( const Vector& rhs ) const;
	float Dot3( const Vector& rhs ) const;

	//!  �x�N�g���O��
	Vector Cross( const Vector& rhs ) const;
	//!  �x�N�g���̒���
	float Length( ) const;
	float Length3( ) const;
	//! @brief  �S�����x�N�g���̐��K��( �ʏ�� )
	//! @attention  �[���x�N�g����n����NaN���Ԃ�܂��B
	Vector Normalize( ) const;
	Vector Normalize3( ) const;

	//! X���P�ʃx�N�g��(1,0,0,0)�̎擾
	static Vector GetXUnit( );
	//! Y���P�ʃx�N�g��(0,1,0,0)�̎擾
	static Vector GetYUnit( );
	//! Z���P�ʃx�N�g��(0,0,1,0)�̎擾
	static Vector GetZUnit( );
	//! W���P�ʃx�N�g��(0,0,0,1)�̎擾
	static Vector GetWUnit( );
	//! �[���x�N�g��(0,0,0,0)���擾
	static Vector GetZero( );

	union
	{
		// �ėp�I�Ɏg�p���邩��
		struct
		{
			float		x;
			float		y;
			float		z;
			float		w;
		};

		struct
		{
			float		f[4];
		};
		
	};
};

}}}

#endif
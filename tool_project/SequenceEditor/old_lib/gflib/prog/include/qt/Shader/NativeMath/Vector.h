#pragma once
#if !defined( TACO_NATIVEMATH_VECTOR_H_INCLUDED )
#define TACO_NATIVEMATH_VECTOR_H_INCLUDED

namespace gfl { namespace maya { namespace Math {

class Vector
{
public:

	static const int			ElementNum = 4;

	//! @brief デフォルトコンストラクタ
	Vector( );
	//! @brief コピーコンストラクタ
	Vector( const Vector& other );

	//! @brief float×４からベクトルを作成
	Vector( float x, float y, float z, float w = ((float) 1.0) );

	~Vector();

	//! 要素の設定
	Vector&  Set( float x, float y, float z, float w = ((float) 1.0) );

	Vector&  operator =( const Vector& rhs );

	float& operator []( int index );
	const float& operator []( int index ) const;
	
	//! スカラ乗算
	Vector operator *( float rhs ) const;
	//! スカラ除算
	Vector operator /( float rhs ) const;
	
	//! スカラ乗算代入
	Vector&  operator *=( float rhs );
	//! スカラ除算代入
	Vector&  operator /=( float rhs );

	//! 加算
	Vector operator +( const Vector& rhs ) const;
	//! 減算
	Vector operator -( const Vector& rhs ) const;
	//! 加算代入
	Vector&  operator +=( const Vector& rhs );
	Vector&  operator +=( const float& rhs );
	//! 減算代入
	Vector&  operator -=( const Vector& rhs );
	Vector&  operator -=( const float& rhs );

	//! negate
	Vector operator -( ) const;

	//! 比較
	bool  operator ==( const Vector& rhs ) const;
	bool  operator !=( const Vector& rhs ) const;

	//!  ベクトル内積
	float Dot( const Vector& rhs ) const;
	float Dot3( const Vector& rhs ) const;

	//!  ベクトル外積
	Vector Cross( const Vector& rhs ) const;
	//!  ベクトルの長さ
	float Length( ) const;
	float Length3( ) const;
	//! @brief  ４次元ベクトルの正規化( 通常版 )
	//! @attention  ゼロベクトルを渡すとNaNが返ります。
	Vector Normalize( ) const;
	Vector Normalize3( ) const;

	//! X軸単位ベクトル(1,0,0,0)の取得
	static Vector GetXUnit( );
	//! Y軸単位ベクトル(0,1,0,0)の取得
	static Vector GetYUnit( );
	//! Z軸単位ベクトル(0,0,1,0)の取得
	static Vector GetZUnit( );
	//! W軸単位ベクトル(0,0,0,1)の取得
	static Vector GetWUnit( );
	//! ゼロベクトル(0,0,0,0)を取得
	static Vector GetZero( );

	union
	{
		// 汎用的に使用するかも
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
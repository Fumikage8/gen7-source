#ifndef GFLIB2_MATH_MATRIX34_H_INCLUDED
#define GFLIB2_MATH_MATRIX34_H_INCLUDED

#include "gfl2_Vector4.h"

#if defined( GFL2_MAYA )
#include <maya/MMatrix.h>
#endif

#if defined(GF_PLATFORM_CTR)
#include <nn/math.h>
#endif

namespace gfl2 { namespace math {

class Matrix34
{
public:
	inline Matrix34();
	inline Matrix34( const Matrix34&  rhs );
	inline Matrix34( const Vector4& row0, const Vector4& row1, const Vector4& row2 );

	//! 代入
	inline Matrix34&  operator =( const Matrix34&  rhs );

	//! 配列アクセス
	inline Vector4&  operator []( int row );
	//! 配列アクセス
	inline const Vector4& operator []( int row ) const;
	//! 要素の設定
	inline Matrix34&  SetElem( int row, int col, float val );
	//! 要素の取得
	inline float GetElem( int row, int col ) const;

  //! X軸の取得
  inline Vector3 GetElemAxisX() const;
  //! Y軸の取得
  inline Vector3 GetElemAxisY() const;
  //! Z軸の取得
  inline Vector3 GetElemAxisZ() const;
  //! 位置要素の取得
  inline Vector3 GetElemPosition() const;
  //! 回転要素の取得ZYX（ラジアン）
  inline Vector3 GetElemRotationZYX() const;

  // @fix GFNMCat[5302] : ジンバルロック時に正しい値が返ってこない
  //! 回転要素の取得ZYX（ラジアン）：ジンバルロック修正版
  inline Vector3 GetElemRotationZYX_fix() const;

  //! 拡大縮小要素の取得
  inline Vector3 GetElemScaling() const;


	//! 加算
	inline Matrix34 operator +( const Matrix34&  rhs ) const;
	//! 減算
	inline Matrix34 operator -( const Matrix34&  rhs ) const;
	//! negate
	inline Matrix34 operator -( ) const;

	//! スカラ乗算
	inline Matrix34 operator *( float rhs ) const;
	//! スカラ乗算代入
	inline Matrix34&  operator *=( float rhs );

	//! ベクトルとの乗算
	inline Vector3 operator *( const Vector3& rhs ) const;
	//! マトリックスとの乗算
	inline Matrix34 operator *( const Matrix34&  rhs ) const;

	//! 加算代入
	inline Matrix34&  operator +=( const Matrix34&  rhs );
	//! 減算代入
	inline Matrix34&  operator -=( const Matrix34&  rhs );
	//! 乗算代入
	inline Matrix34&  operator *=( const Matrix34&  rhs );

	//! マトリクスの転置
	inline Matrix34 Transpose( void ) const;
	//! 逆マトリクス
	inline Matrix34 Inverse( void ) const;

	// -----------------------------------------------------------------
	// Normalize
	// -----------------------------------------------------------------
	inline void Normalize( Vector3* scl = 0 )
	{
		Vector3 vx(row[0][0], row[1][0], row[2][0]);
		Vector3 vy(row[0][1], row[1][1], row[2][1]);
		Vector3 vz(row[0][2], row[1][2], row[2][2]);

		if( !scl ){
			vx = vx.Normalize( );
			vy = vy.Normalize( );
			vz = vz.Normalize( );
		} else {
			scl->Set( vx.NormalizeRet( ),
					 vy.NormalizeRet( ),
					 vz.NormalizeRet( ) );
		}

    this->row[0].Set(vx.x, vy.x, vz.x, row[0][3]);
    this->row[1].Set(vx.y, vy.y, vz.y, row[1][3]);
    this->row[2].Set(vx.z, vy.z, vz.z, row[2][3]);
	}

	//! 単位マトリクス
	static inline Matrix34 GetIdentity( );
	//! ゼロマトリクス
	static inline Matrix34 GetZero();
	//! X軸回転マトリクス
	static inline Matrix34 GetRotationX( float radians );
	//! Y軸回転マトリクス
	static inline Matrix34 GetRotationY( float radians );
	//! Z軸回転マトリクス
	static inline Matrix34 GetRotationZ( float radians );
	//! Z * Y * X 回転マトリクス
	static inline Matrix34 GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! Z * Y * X 回転マトリクス
	static inline void GetRotationZYX(Matrix34& result, float radiansZ, float radiansY, float radiansX );
	//! スケーリングマトリクス
	static inline Matrix34 GetScaling( const Vector3& scale );
	//! 平行移動マトリクス
	static inline Matrix34 GetTranslation( const Vector3& translation );
	//! OpenGL互換 LookAt
	static inline Matrix34 GetLookAt( const Vector3& eye, const Vector3& aim, const Vector3& up );

	//!  ｗ＝１と仮定したベクトルとマトリクスとの乗算
	inline Vector3 Matrix34::TransformCoord( const Vector3& rhs ) const;
    inline Vector4 Matrix34::TransformCoord( const Vector4& rhs ) const;
	
	//!  ｗ＝０と仮定したベクトルとマトリクスとの乗算
	inline Vector3 Matrix34::TransformNormal( const Vector3& rhs ) const;
    inline Vector4 Matrix34::TransformNormal( const Vector4& rhs ) const;

    //! ベクトルとマトリクスとの乗算
    inline Vector4 Matrix34::TransformVec( const Vector4& rhs ) const;
	
	inline Matrix34 Matrix34::AbsPerElem( void ) const;

  /**
   * @brief 線形補完
   * @param v 補完する行列
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Leap(const Matrix34& m, f32 t)
  {
    this->row[0].Lerp(m.row[0], t);
    this->row[1].Lerp(m.row[1], t);
    this->row[2].Lerp(m.row[2], t);
  }

  //! 高速な乗算
  static inline void MultFast(Matrix34* out_pResult, const Matrix34& lhs, const Matrix34& rhs);

  //! 平行移動要素の設定
  inline Matrix34& SetTranslationVector( const Vector3 &translation )
  {
    row[0].w = translation.x;
    row[1].w = translation.y;
    row[2].w = translation.z;
    return *this;
  }

  static inline Matrix34 GetBillboardMatrixRotationXYZ(const Matrix34& viewMatrix );

private:

  Vector4 row[3]; // f32 m[3][4]
};

#include "Matrix34Standard.inl"

}}

#endif

#ifndef GFLIB2_MATH_MATRIX44_H_INCLUDED
#define GFLIB2_MATH_MATRIX44_H_INCLUDED

#include "gfl2_Vector4.h"
#include "gfl2_Matrix34.h"

#if defined( GFL2_MAYA )
#include <maya/MMatrix.h>
#endif

namespace gfl2 { namespace math {

class Matrix44
{
public:
	inline Matrix44();
	inline Matrix44( const Matrix44&  rhs );
	inline Matrix44( const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3 );
	inline Matrix44( const Matrix34& m34 );

	//! 代入
	inline Matrix44&  operator =( const Matrix44&  rhs );

	//! 配列アクセス
	inline Vector4&  operator []( int row );
	//! 配列アクセス
	inline const Vector4& operator []( int row ) const;
	//! 要素の設定
	inline Matrix44&  SetElem( int row, int col, float val );
	//! 要素の取得
	inline float GetElem( int row, int col ) const;

  //! 位置要素の取得
  inline Vector3 GetElemPosition() const;
  //! 回転要素の取得ZYX（ラジアン）
  inline Vector3 GetElemRotationZYX() const;
  //! 拡大縮小要素の取得
  inline Vector3 GetElemScaling() const;

	//! 加算
	inline Matrix44 operator +( const Matrix44&  rhs ) const;
	//! 減算
	inline Matrix44 operator -( const Matrix44&  rhs ) const;
	//! negate
	inline Matrix44 operator -( ) const;

	//! スカラ乗算
	inline Matrix44 operator *( float rhs ) const;
	//! スカラ乗算代入
	inline Matrix44&  operator *=( float rhs );

	//! ベクトルとの乗算
	inline Vector4 operator *( const Vector4& rhs ) const;
	//! ベクトルとの乗算
	inline Vector3 operator *( const Vector3& rhs ) const;
	//! マトリックスとの乗算
	inline Matrix44 operator *( const Matrix44&  rhs ) const;

	//! 加算代入
	inline Matrix44&  operator +=( const Matrix44&  rhs );
	//! 減算代入
	inline Matrix44&  operator -=( const Matrix44&  rhs );
	//! 乗算代入
	inline Matrix44&  operator *=( const Matrix44&  rhs );

	//! マトリクスの転置
	inline Matrix44 Transpose( void ) const;
	//! 逆マトリクス
	inline Matrix44 Inverse( void ) const;
	//! 逆マトリクス( アフィン変換(Matrix34)マトリクスとして処理  高速版 )
	inline Matrix44 AffineInverse( void ) const;
	
	//! 平行移動要素の設定
	inline Matrix44& SetTranslationVector( const Vector4 &translation )
	{
		row[0].w = translation.x;
		row[1].w = translation.y;
		row[2].w = translation.z;
		row[3].w = 1.0f;
		return *this;
	}

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
	static inline Matrix44 GetIdentity( );
	//! ゼロマトリクス
	static inline Matrix44 GetZero();
	//! X軸回転マトリクス
	static inline Matrix44 GetRotationX( float radians );
	//! Y軸回転マトリクス
	static inline Matrix44 GetRotationY( float radians );
	//! Z軸回転マトリクス
	static inline Matrix44 GetRotationZ( float radians );
	//! Z * Y * X 回転マトリクス
	static inline Matrix44 GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! スケーリングマトリクス
	static inline Matrix44 GetScaling( const Vector3& scale );
	//! 平行移動マトリクス
	static inline Matrix44 GetTranslation( const Vector3& translation );
	//! OpenGL互換 LookAt
	static inline Matrix44 GetLookAt( const Vector3& eye, const Vector3& aim, const Vector3& up );
	//! Perspective
	static inline Matrix44 GetPerspective( float fovy, float aspect, float zNear, float zFar, bool isPivot = false );
	//! Frustum
	static inline Matrix44 GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar, bool isPivot = false );
	//! ortho
	static inline Matrix44 GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar, bool isPivot = false );
	
	//static inline Matrix44 GetBillboardMatrixRotationEye( const Vector4& cameraPosition, const Vector4& targetPosition );
	//static inline Matrix44 GetBillboardMatrixRotationEyeY( const Vector4& cameraPosition, const Vector4& targetPosition );
	static inline Matrix44 GetBillboardMatrixRotationXYZ(const Matrix44& viewMatrix );
	static inline Matrix44 GetBillboardMatrixRotationY(const Matrix44& viewMatrix );

	//!  ｗ＝１と仮定したベクトルとマトリクスとの乗算
	inline Vector3 Matrix44::TransformCoord( const Vector3& rhs ) const;
	
	//!  ｗ＝０と仮定したベクトルとマトリクスとの乗算
	inline Vector3 Matrix44::TransformNormal( const Vector3& rhs ) const;

    inline Vector4 Matrix44::TransformVec( const Vector4& rhs ) const;
	
	inline Matrix44 Matrix44::AbsPerElem( void ) const;

  /**
   * @brief 線形補完
   * @param v 補完する行列
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Leap(const Matrix44& m, f32 t)
  {
    this->row[0].Lerp(m.row[0], t);
    this->row[1].Lerp(m.row[1], t);
    this->row[2].Lerp(m.row[2], t);
    this->row[3].Lerp(m.row[3], t);
  }


private:

	//! Perspective OpenGL互換(Depth=-1.0〜+1.0に射影)
	static inline Matrix44 GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );
	//! Perspective DirectX互換(Depth=0.0〜+1.0に射影)
	static inline Matrix44 GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar );
	
	//! Frustum OpenGL互換(Depth=-1.0〜+1.0に射影)
	static inline Matrix44 GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! Frustum DirectX互換(Depth=0.0〜+1.0に射影)
	static inline Matrix44 GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar );
	
	//! ortho OpenGL互換(Depth=-1.0〜+1.0に射影)
	static inline Matrix44 GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho DirectX互換(Depth=0.0〜+1.0に射影)
	static inline Matrix44 GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar );

  Vector4 row[4]; // f32 m[4][4]
};

#include "Matrix44Standard.inl"

}}

#endif

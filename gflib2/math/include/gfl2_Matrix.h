#ifndef GFLIB2_MATH_MATRIX_H_INCLUDED
#define GFLIB2_MATH_MATRIX_H_INCLUDED

#include "gfl2_Vector4.h"

#if defined( GFL2_MAYA )
#include <maya/MMatrix.h>
#endif

namespace gfl2 { namespace math {

class Matrix
{
public:
	inline Matrix();
	inline Matrix( const Matrix&  rhs );
	inline Matrix( const Vector4& col0_, const Vector4& col1_, const Vector4& col2_, const Vector4& col3_ );
#if defined( GFL2_MAYA )
  inline Matrix( const MMatrix&  rhs );
#endif

	//! 代入
	inline Matrix&  operator =( const Matrix&  rhs );
#if defined( GFL2_MAYA )
  inline Matrix&  operator =( const MMatrix&  rhs );
#endif

	//! 配列アクセス
	inline Vector4&  operator []( int col );
	//! 配列アクセス
	inline const Vector4& operator []( int col ) const;
	//! 要素の設定
	inline Matrix&  SetElem( int col, int row, float val );
	//! 要素の取得
	inline float GetElem( int col, int row ) const;

  //! 位置要素の取得
  inline Vector4 GetElemPosition() const;
  //! 回転要素の取得ZYX（ラジアン）
  inline Vector4 GetElemRotationZYX() const;
  //! 拡大縮小要素の取得
  inline Vector4 GetElemScaling() const;

	//! 加算
	inline Matrix operator +( const Matrix&  rhs ) const;
	//! 減算
	inline Matrix operator -( const Matrix&  rhs ) const;
	//! negate
	inline Matrix operator -( ) const;

	//! スカラ乗算
	inline Matrix operator *( float rhs ) const;
	//! スカラ乗算代入
	inline Matrix&  operator *=( float rhs );

	//! ベクトルとの乗算
	inline Vector4 operator *( const Vector4& rhs ) const;
	//! マトリックスとの乗算
	inline Matrix operator *( const Matrix&  rhs ) const;

	//! 加算代入
	inline Matrix&  operator +=( const Matrix&  rhs );
	//! 減算代入
	inline Matrix&  operator -=( const Matrix&  rhs );
	//! 乗算代入
	inline Matrix&  operator *=( const Matrix&  rhs );

	//! マトリクスの転置
	inline Matrix Transpose( void ) const;
	//! 逆マトリクス
	inline Matrix Inverse( void ) const;
	//! 逆マトリクス( アフィン変換マトリクス用  高速版 )
	inline Matrix AffineInverse( void ) const;

	//! 平行移動要素の設定
	inline Matrix& SetTranslationVector( const Vector4 &translation )
	{
		col[3] = Vector4(translation);
		col[3].w = 1.0f;
		return *this;
	}

	// -----------------------------------------------------------------
	// Normalize
	// -----------------------------------------------------------------
	inline void Normalize( Vector4* scl = 0 )
	{
		Vector4 vx, vy, vz;

		vx = col[0];
		vy = col[1];
		vz = col[2];

		if( !scl ){
			vx = vx.Normalize( );
			vy = vy.Normalize( );
			vz = vz.Normalize( );
		} else {
			scl->Set( vx.NormalizeRet( ),
					 vy.NormalizeRet( ),
					 vz.NormalizeRet( ) );
		}

		col[0] = vx;
		col[1] = vy;
		col[2] = vz;
	}

	//! 単位マトリクス
	static inline Matrix GetIdentity( );
	//! ゼロマトリクス
	static inline Matrix GetZero();
	//! X軸回転マトリクス
	static inline Matrix GetRotationX( float radians );
	//! Y軸回転マトリクス
	static inline Matrix GetRotationY( float radians );
	//! Z軸回転マトリクス
	static inline Matrix GetRotationZ( float radians );
	//! Z * Y * X 回転マトリクス
	static inline Matrix GetRotationZYX( float radiansZ, float radiansY, float radiansX );
	//! スケーリングマトリクス
	static inline Matrix GetScaling( const Vector4& scale );
	//! 平行移動マトリクス
	static inline Matrix GetTranslation( const Vector4& translation );
	//! OpenGL互換 LookAt
	static inline Matrix GetLookAt( const Vector4& eye, const Vector4& aim, const Vector4& up );
	//! Perspective
	static inline Matrix GetPerspective( float fovy, float aspect, float zNear, float zFar, bool isPivot = false );
	//! Frustum
	static inline Matrix GetFrustum( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho
	static inline Matrix GetOrthographic( float left, float right, float bottom, float top, float zNear, float zFar );
	
	static inline Matrix GetBillboardMatrixRotationEye( const Vector4& cameraPosition, const Vector4& targetPosition );
	static inline Matrix GetBillboardMatrixRotationEyeY( const Vector4& cameraPosition, const Vector4& targetPosition );
	static inline Matrix GetBillboardMatrixRotationXYZ(const Matrix& viewMatrix );
	static inline Matrix GetBillboardMatrixRotationY(const Matrix& viewMatrix );

	//!  ｗ＝１と仮定したベクトルとマトリクスとの乗算
	inline Vector4 Matrix::TransformCoord( const Vector4& rhs ) const;
	
	//!  ｗ＝０と仮定したベクトルとマトリクスとの乗算
	inline Vector4 Matrix::TransformNormal( const Vector4& rhs ) const;
	
	inline Matrix Matrix::AbsPerElem( void ) const;

  /**
   * @brief 線形補完
   * @param v 補完する行列
   * @param t 補完係数 0.0 ~ 1.0
   */
  inline void Leap(const Matrix& m, f32 t)
  {
    this->col[0].Lerp(m.col[0], t);
    this->col[1].Lerp(m.col[1], t);
    this->col[2].Lerp(m.col[2], t);
    this->col[3].Lerp(m.col[3], t);
  }


private:

	//! Perspective OpenGL互換(Depth=-1.0?+1.0に射影)
	static inline Matrix GetPerspectiveGL( float fovy, float aspect, float zNear, float zFar );
	//! Perspective DirectX互換(Depth=0.0?+1.0に射影)
	static inline Matrix GetPerspectiveDX( float fovy, float aspect, float zNear, float zFar );
	
	//! Frustum OpenGL互換(Depth=-1.0?+1.0に射影)
	static inline Matrix GetFrustumGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! Frustum DirectX互換(Depth=0.0?+1.0に射影)
	static inline Matrix GetFrustumDX( float left, float right, float bottom, float top, float zNear, float zFar );
	
	//! ortho OpenGL互換(Depth=-1.0?+1.0に射影)
	static inline Matrix GetOrthographicGL( float left, float right, float bottom, float top, float zNear, float zFar );
	//! ortho DirectX互換(Depth=0.0?+1.0に射影)
	static inline Matrix GetOrthographicDX( float left, float right, float bottom, float top, float zNear, float zFar );

  // C++11まではunionにPOD型しか使うことはできないので今回はunionやめます
  Vector4		col[4];
};

#include "MatrixStandard.inl"

}}

#endif

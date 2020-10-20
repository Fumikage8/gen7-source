#ifndef GFLIB2_MATH_AABB_H_INCLUDED
#define GFLIB2_MATH_AABB_H_INCLUDED

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_AABB.h
 *	@brief  AABB型バウンディングボックス
 *	@author	masateru ishiguro
 *	@date		2011.01.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <types/include/gfl2_Typedef.h>
#include <float.h>
#include <math.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_Matrix44.h>

#if defined( GFL2_MAYA )
#include <maya/MVector.h>
#include <maya/Mf32Point.h>
#endif


namespace gfl2 { namespace math {

//-----------------------------------------------------------------------------
/*!
	* @brief   AABB型バウンディングボックスクラス
	* @note    min,maxなどの変数名を使用すると、VC++のコード補完が使用できなくなる!
	*/
class AABB
{
private:
	Vector4 min_;
	Vector4 max_;
		
public:
	inline AABB();                                                 //!< コンストラクタ
	inline AABB( const Vector4& pos );                              //!< 体積０のコンストラクタ
	inline AABB( const Vector4& minValue, const Vector4& maxValue ); //!< 値からコンストラクタ
	inline AABB( const AABB& aabb );                               //!< コピーコンストラクタ
		
	inline AABB& operator=( const AABB& aabb );                    //!< 代入演算子
	inline AABB& operator=( const Vector4& vec );                   //!< 最小値と最大値を同じ値に設定
		
	inline bool IsInvalid() const;                                 //!< 無効値か取得
	inline AABB& Invalidate( void );                               //!< 無効に設定
	inline void SetZero();                                         //!< ゼロに設定
	inline void SetMinMax( const Vector4 &vec );                    //!< 最小値最大値を同じ値に設定
	inline void SetMinMax( const Vector4 &minValue, const Vector4& maxValue );//!< 最小値最大値を設定
	inline void SetMin( const Vector4& minValue );                  //!< 最小値を設定
	inline const Vector4& GetMin() const;                           //!< 最小値を取得
	inline void SetMax( const Vector4& maxValue );                  //!< 最大値を設定
	inline const Vector4& GetMax() const;                           //!< 最大値を取得
		
	inline Vector4 GetCenter() const;                               //!< 中心位置を取得
	inline Vector4 GetSum() const;                                  //!< 最小値と最大値の和を取得
	inline Vector4 GetDifference() const;                           //!< 最大値と最小値の差を取得
		
	inline bool IsContain( const Vector4& point ) const;            //!< 指定した位置を含んでいるか取得
	inline bool IsContainAny( const Vector4& point ) const;         //!< 軸ごとに比較して、１軸でも指定した位置を含んでいるか取得
	inline bool IsCross( const AABB& aabb ) const;                 //!< 一箇所でも交わっているか取得
		
	inline AABB Merge( const Vector4& vec ) const;                  //!< 要素を合成する
	inline AABB Merge( const AABB& aabb ) const;                   //!< 要素を合成する
	inline AABB Transform( const Matrix& matrix ) const;           //!< AABBを変換して合成
		
	inline AABB Offset( const Vector4& vec ) const;                 //!< オフセット移動
	inline AABB Inflate( const Vector4& vec ) const;                //!< 範囲拡張
		
	static inline AABB GetUnitAABB( void );                        //!< 単位ＡＡＢＢ( -1?+1の範囲 )を取得
		
	friend inline AABB operator *( const Matrix& matrix, const AABB& aabb );//!< operator * ( Matrix, AABB )

	class Util{
	public:

		static void CalcFrustumPlanes( math::Matrix *cullingMatrix_, const math::Matrix &viewProjMatrix )
		{
			float m[16];

			for( int i=0; i<16; ++i ) {
				m[i] = viewProjMatrix.GetElem( i/4, i%4 );
			}

			cullingMatrix_[0][0].Set( math::Vector4( m[3]-m[0], m[7]-m[4], m[11]-m[8], m[15]-m[12] ).Normalize() );
			cullingMatrix_[0][1].Set( math::Vector4( m[3]+m[0], m[7]+m[4], m[11]+m[8], m[15]+m[12] ).Normalize() );
			cullingMatrix_[0][2].Set( math::Vector4( m[3]+m[1], m[7]+m[5], m[11]+m[9], m[15]+m[13] ).Normalize() );
			cullingMatrix_[0][3].Set( math::Vector4::GetZero() );
			cullingMatrix_[1][0].Set( math::Vector4( m[3]-m[1], m[7]-m[5], m[11]-m[9], m[15]-m[13] ).Normalize() );
			cullingMatrix_[1][1].Set( math::Vector4( m[3]-m[2], m[7]-m[6], m[11]-m[10], m[15]-m[14] ).Normalize() );
			cullingMatrix_[1][2].Set( math::Vector4( m[3]+m[2], m[7]+m[6], m[11]+m[10], m[15]+m[14] ).Normalize() );
			cullingMatrix_[1][3].Set( math::Vector4::GetZero() );
			cullingMatrix_[0] = cullingMatrix_[0].Transpose();
			cullingMatrix_[1] = cullingMatrix_[1].Transpose();
			cullingMatrix_[2] = cullingMatrix_[0].AbsPerElem();
			cullingMatrix_[3] = cullingMatrix_[1].AbsPerElem();
		}

		static void CalcFrustumPlanes( math::Matrix44 *pCullingMatrix, const math::Matrix44 &viewProjMatrix )
		{
			float							m[16];
			math::Matrix44		viewProjMatrixInv = viewProjMatrix.Transpose();

			for( int i=0; i<16; ++i ) {
				m[i] = viewProjMatrixInv.GetElem( i/4, i%4 );
			}

			pCullingMatrix[0][0].Set( math::Vector4( m[3]-m[0], m[7]-m[4], m[11]-m[8], m[15]-m[12] ).Normalize() );
			pCullingMatrix[0][1].Set( math::Vector4( m[3]+m[0], m[7]+m[4], m[11]+m[8], m[15]+m[12] ).Normalize() );
			pCullingMatrix[0][2].Set( math::Vector4( m[3]+m[1], m[7]+m[5], m[11]+m[9], m[15]+m[13] ).Normalize() );
			pCullingMatrix[0][3].Set( math::Vector4::GetZero() );
			pCullingMatrix[1][0].Set( math::Vector4( m[3]-m[1], m[7]-m[5], m[11]-m[9], m[15]-m[13] ).Normalize() );
			pCullingMatrix[1][1].Set( math::Vector4( m[3]-m[2], m[7]-m[6], m[11]-m[10], m[15]-m[14] ).Normalize() );
			pCullingMatrix[1][2].Set( math::Vector4( m[3]+m[2], m[7]+m[6], m[11]+m[10], m[15]+m[14] ).Normalize() );
			pCullingMatrix[1][3].Set( math::Vector4::GetZero() );
			pCullingMatrix[0] = pCullingMatrix[0].Transpose();
			pCullingMatrix[1] = pCullingMatrix[1].Transpose();
			pCullingMatrix[2] = pCullingMatrix[0].AbsPerElem();
			pCullingMatrix[3] = pCullingMatrix[1].AbsPerElem();

			for( int i = 0; i < 4; ++i )
				pCullingMatrix[i] = pCullingMatrix[i].Transpose();
		}

		static b32 IsIntersectFrustum( const math::Matrix *cullingMatrices, const math::AABB& aabb )
		{
			math::Vector4 c = aabb.GetCenter();
			math::Vector4 d = aabb.GetMax() - c;

			math::Vector4 c0 = cullingMatrices[0].TransformCoord(c);
			math::Vector4 c1 = cullingMatrices[1].TransformCoord(c);
			math::Vector4 d0 = cullingMatrices[2].TransformNormal(d);
			math::Vector4 d1 = cullingMatrices[3].TransformNormal(d);

			c = c0 + d0;
			d = c1 + d1;
			return !c.MinPerElem(d).IsAnyMinus();
		}
		
		//  ｗ＝１と仮定したベクトルとマトリクスとの乗算
		static inline Vector3 TransformCoord( const math::Matrix44 &row, const Vector3& rhs )
		{
			return Vector3(
				row[0][0] * rhs.x + row[0][1] * rhs.y + row[0][2] * rhs.z + row[0][3],
				row[1][0] * rhs.x + row[1][1] * rhs.y + row[1][2] * rhs.z + row[1][3],
				row[2][0] * rhs.x + row[2][1] * rhs.y + row[2][2] * rhs.z + row[2][3]
			);
		}

		static b32 IsIntersectFrustum( const math::Matrix44 *pCullingMatrices, const math::AABB& rAABB )
		{
			math::Vector3 c( rAABB.GetCenter().x, rAABB.GetCenter().y, rAABB.GetCenter().z);
			math::Vector3 m( rAABB.GetMax().x, rAABB.GetMax().y, rAABB.GetMax().z);
			math::Vector3 d = m - c;

			math::Vector3 c0 = TransformCoord( pCullingMatrices[0], c );
			math::Vector3 c1 = TransformCoord( pCullingMatrices[1], c );
			math::Vector3 d0 = pCullingMatrices[2].TransformNormal(d);
			math::Vector3 d1 = pCullingMatrices[3].TransformNormal(d);

			c = c0 + d0;
			d = c1 + d1;
			return !c.MinPerElem(d).IsAnyMinus();
		}

	private:

		Util(){}
		~Util(){}
	};
};
	
//-----------------------------------------------------------------------------
//
// インライン関数の定義
//
	
/*!
	* @brief   デフォルトコンストラクタ
	*/
inline AABB::AABB()
{
}
	
/*!
	* @brief   体積０のコンストラクタ
	* @param   pos     最小値ベクトル＆最大値ベクトル
	*/
inline AABB::AABB( const Vector4& pos ) : min_( pos ), max_( pos )
{
}
	
/*!
	* @brief   値からコンストラクタ
	* @param   min     最小値ベクトル
	* @param   max     最大値ベクトル
	*/
inline AABB::AABB( const Vector4& minValue, const Vector4& maxValue ) : min_(minValue), max_(maxValue)
{
}
	
/*!
	* @brief   コピーコンストラクタ
	* @param   aabb    コピー元オブジェクト
	*/
inline AABB::AABB( const AABB& aabb ) : min_( aabb.min_ ), max_( aabb.max_ )
{
}
	
/*!
	* @brief   代入演算子
	* @param   aabb    コピー元オブジェクト
	* @return          コピーしたオブジェクト
	*/
inline AABB& AABB::operator =( const AABB& aabb )
{
	min_ = aabb.min_;
	max_ = aabb.max_;
	return *this;
}
	
/*!
	* @brief   代入演算子
	* @param   vec     ベクトル
	* @return          コピーしたオブジェクト
	*/
inline AABB& AABB::operator =( const Vector4& vec )
{
	min_ = max_ = vec;
	return *this;
}
	
/*!
	* @brief   無効値か取得
	*/
inline bool AABB::IsInvalid() const
{
	//  最大値・最小値の関係が逆転してたら無効値
	return ( max_ - min_ ).IsAnyMinus3();
}
	
/*!
	* @brief   無効に設定
	*/
inline AABB& AABB::Invalidate( void )
{
	min_ = Vector4( FLT_MAX, FLT_MAX, FLT_MAX );
	max_ = -min_;
	return (*this);
}
	
/*!
	* @brief   ゼロに設定する
	*/
inline void AABB::SetZero()
{
	min_ = max_ = Vector4::GetZero();
}
	
/*!
	* @brief   最小値・最大値を同じ値に設定
	* @param   vec     ベクトル
	*/
inline void AABB::SetMinMax( const Vector4 &vec )
{
	min_ = max_ = vec;
}
	
/*!
	* @brief   最小値・最大値をまとめて設定
	* @param   min     最小値ベクトル
	* @param   max     最大値ベクトル
	*/
inline void AABB::SetMinMax( const Vector4& minValue, const Vector4& maxValue )
{
	min_ = minValue;
	max_ = maxValue;
}
	
/*!
	* @brief   最小値ベクトルを設定
	* @param   min     最小値ベクトル
	*/
inline void AABB::SetMin( const Vector4& minValue )
{
	min_ = minValue;
}
	
/*!
	* @brief   最小値ベクトルを取得
	* @return          最小値ベクトル
	*/
inline const Vector4& AABB::GetMin() const
{
	return min_;
}
	
/*!
	* @brief   最大値ベクトルを設定
	* @param   max     最大値ベクトル
	*/
inline void AABB::SetMax( const Vector4 &maxValue )
{
	max_ = maxValue;
}
	
/*!
	* @brief   最大値ベクトルを取得
	* @return          最大値ベクトル
	*/
inline const Vector4& AABB::GetMax() const
{
	return max_;
}
	
/*!
	* @brief   中心位置を取得
	*/
inline Vector4 AABB::GetCenter() const
{
	return (min_+max_)*((float) 0.5);
}
	
/*!
	* @brief   最小値と最大値の和を取得
	*/
inline Vector4 AABB::GetSum() const
{
	return min_ + max_;
}
	
/*!
	* @brief   最大値と最小値の差を取得
	*/
inline Vector4 AABB::GetDifference() const
{
	return max_ - min_;
}
	
/*! 
	* @brief   指定した位置を取得
	* @param   point   調べる位置
	* @return  true 位置はAABB内にある false 外側。
	*/
inline bool AABB::IsContain( const Vector4& point ) const
{
	return !(point - min_).MinPerElem( max_ - point ).IsAnyMinus3();
}
	
/*! 
	* @brief   軸ごとに比較して、１軸でもAABB内にあるか取得
	* @param   point   調べる位置
	* @return  true 位置はAABB内にある false 外側。
	*/
inline bool AABB::IsContainAny( const Vector4& point ) const
{
#if defined(CDEV_WIN32)
	return
		  ( point.GetX() >= min_.GetX() && point.GetX() <= max_.GetX() ) ||
		  ( point.GetY() >= min_.GetY() && point.GetY() <= max_.GetY() ) ||
		  ( point.GetZ() >= min_.GetZ() && point.GetZ() <= max_.GetZ() );
#else
	return !(point-min_).MinPerElem( max_ - point ).IsAllMinus3();
#endif
}
	
/*!
	* @brief   １箇所でも交わっているか取得
	* @param   aabb    比較する領域
	* @return  true 交わっている false 交わっていない
	*/
inline bool AABB::IsCross( const AABB& aabb ) const
{
#if defined(CDEV_WIN32)
	return ( min_.GetX() <= aabb.max_.GetX() && max_.GetX() >= aabb.min_.GetX() &&
		  min_.GetY() <= aabb.max_.GetY() && max_.GetY() >= aabb.min_.GetY() &&
		  min_.GetZ() <= aabb.max_.GetZ() && max_.GetZ() >= aabb.min_.GetZ() );
#else
	return !(aabb.max_ - min_).MinPerElem( max_ - aabb.min_ ).IsAnyMinus3();
#endif
}
	
/*!
	* @brief   要素を合成する
	* @param   vec     合成するベクトル
	*/
inline AABB AABB::Merge( const Vector4& vec ) const
{
	return AABB(
		  min_.MinPerElem( vec ),
		  max_.MaxPerElem( vec )
	);
}
/*!
	* @brief   要素を合成する
	* @param   aabb    合成する領域
	*/
inline AABB AABB::Merge( const AABB& aabb ) const
{
	return AABB(
		  min_.MinPerElem( aabb.min_ ),
		  max_.MaxPerElem( aabb.max_ )
	);
}
/*!
	* @brief   AABBを変換して合成
	* @param   Transform   合成する領域を変換する行列
	*/
inline AABB AABB::Transform( const Matrix& matrix ) const
{
	const Vector4 minValue = GetMin();
	const Vector4 maxValue = GetMax();
	Vector4 pos = matrix.TransformCoord( min_ );
	Vector4 diff = max_ - min_;
	Vector4 dx = matrix[0].MulPerElem( diff.GetBroadcastX() );
	Vector4 dy = matrix[1].MulPerElem( diff.GetBroadcastY() );
	Vector4 dz = matrix[2].MulPerElem( diff.GetBroadcastZ() );
	return AABB( pos )
		  .Merge( pos + dx )
		  .Merge( pos + dy )
		  .Merge( pos + dy + dx )
		  .Merge( pos + dz )
		  .Merge( pos + dz + dy )
		  .Merge( pos + dz + dy + dx )
		  .Merge( pos + dz + dx );
}
	
/*!
	* @brief   オフセット移動
	* @param   vec         移動ベクトル
	*/
inline AABB AABB::Offset( const Vector4& vec ) const
{
	return AABB( min_ + vec,
		            max_ + vec );
}
/*!
	* @brief   範囲拡張
	* @param   vec         各軸拡張範囲
	*/
inline AABB AABB::Inflate( const Vector4& vec ) const
{
	return AABB( min_ - vec.AbsPerElem(),
		            max_ + vec.AbsPerElem() );
}
	
/*!
	* @brief   ユニット( -1?+1の範囲 )のＡＡＢＢを取得
	*/
inline AABB AABB::GetUnitAABB( void )
{
	return AABB(
		  Vector4( (float) -1.0, (float) -1.0, (float) -1.0, (float) 1.0 ),
		  Vector4( (float)  1.0, (float)  1.0, (float)  1.0, (float) 1.0 )
	);
}
	
/*!
	* @brief   行列をかけるオペレーション
	*/
inline AABB operator * ( const Matrix& matrix, const AABB& aabb )
{
	return aabb.Transform( matrix );
}

} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MATHAABB_H__

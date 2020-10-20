#ifndef __GFL_MATHAABB_H__
#define __GFL_MATHAABB_H__
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
#pragma once
#include "../core/gfl_Config.h"
#ifdef GFL_PLATFORM_3DS
#include <nn/math.h>

#include <math/gfl_MathType.h>
#else
#include "../gfl_Core2.h"
typedef gfl::core::VectorNative3 VEC3;
typedef gfl::core::VectorNative4 VEC4;
typedef gfl::core::MatrixSoa34 MTX34;
#endif
#include <math/gfl_MathSphere.h>
namespace gfl {
namespace math {

class AABB_VEC3;

/**
 * AABB型バウンディングボックス
 */
class AABB
{
	GFL_FORBID_COPY_AND_ASSIGN(AABB);
private:
	VEC4 m_min;
	VEC4 m_max;

public:
	inline AABB();                                                 //!< コンストラクタ
	inline AABB( const VEC4& minValue, const VEC4& maxValue ); //!< 値からコンストラクタ
    inline AABB( const AABB_VEC3& aabbV3 ); //!< 値からコンストラクタ

	inline void Set( const AABB &aabb );                          //!< 設定関数
	inline void SetZero();                                       //!< ゼロに設定
	inline void SetMinMax( const VEC4 &vec );                    //!< 最小値最大値を同じ値に設定
	inline void SetMinMax( const VEC4 &minValue, const VEC4& maxValue );//!< 最小値最大値を設定
	inline void SetMin( const VEC4& minValue );                  //!< 最小値を設定
	inline const VEC4& GetMin() const;                           //!< 最小値を取得
	inline void SetMax( const VEC4& maxValue );                  //!< 最大値を設定
	inline const VEC4& GetMax() const;                           //!< 最大値を取得

	inline VEC4 GetCenter() const;                               //!< 中心位置を取得

	inline bool IsContain( const VEC4& point ) const;            //!< 指定した位置を含んでいるか取得
	inline bool IsCross( const AABB& aabb ) const;                 //!< 一箇所でも交わっているか取得

	inline AABB Merge( const VEC4& vec ) const;                  //!< 要素を合成する
	inline AABB Merge( const AABB& aabb ) const;                   //!< 要素を合成する

	inline AABB Offset( const VEC4& vec ) const;                 //!< オフセット移動

    inline void Create(const VEC3& posi1, const VEC3& posi2); ///< 2点からAABBを作成する
    inline void Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3); ///< 3点からAABBを作成する(コリジョンで使用するため汎用性を犠牲にした)
    inline bool IsHit(const AABB& aabb) const ; ///< AABBとの衝突判定(高速)

	static inline AABB GetUnitAABB( void );                        //!< 単位ＡＡＢＢ( -1?+1の範囲 )を取得
};

/*!
 * @brief   デフォルトコンストラクタ
 */
inline AABB::AABB() : m_min(0.0f, 0.0f, 0.0f, 0.0f), m_max(0.0f, 0.0f, 0.0f, 0.0f)
{
}

/*!
 * @brief   値からコンストラクタ
 * @param   min     最小値ベクトル
 * @param   max     最大値ベクトル
 */
inline AABB::AABB( const VEC4& minValue, const VEC4& maxValue ) : m_min(minValue), m_max(maxValue)
{
}

/**
 * @brief   設定関数
 * @param aabb　値
 */
inline void AABB::Set( const AABB &aabb )
{
	m_min = aabb.m_min;
	m_max = aabb.m_max;
}

/*!
 * @brief   ゼロに設定する
 */
inline void AABB::SetZero()
{
	m_min = m_max = VEC4::Zero();
}

/*!
 * @brief   最小値・最大値を同じ値に設定
 * @param   vec     ベクトル
 */
inline void AABB::SetMinMax( const VEC4 &vec )
{
	m_min = m_max = vec;
}

/*!
 * @brief   最小値・最大値をまとめて設定
 * @param   min     最小値ベクトル
 * @param   max     最大値ベクトル
 */
inline void AABB::SetMinMax( const VEC4& minValue, const VEC4& maxValue )
{
	m_min = minValue;
	m_max = maxValue;
}

/*!
 * @brief   最小値ベクトルを設定
 * @param   min     最小値ベクトル
 */
inline void AABB::SetMin( const VEC4& minValue )
{
	m_min = minValue;
}

/*!
 * @brief   最小値ベクトルを取得
 * @return          最小値ベクトル
 */
inline const VEC4& AABB::GetMin() const
{
	return m_min;
}

/*!
 * @brief   最大値ベクトルを設定
 * @param   max     最大値ベクトル
 */
inline void AABB::SetMax( const VEC4 &maxValue )
{
	m_max = maxValue;
}

/*!
 * @brief   最大値ベクトルを取得
 * @return          最大値ベクトル
 */
inline const VEC4& AABB::GetMax() const
{
	return m_max;
}

/*!
 * @brief   中心位置を取得
 */
inline VEC4 AABB::GetCenter() const
{
	return (m_min+m_max)*((float) 0.5);
}

/*!
 * @brief   指定した位置を取得
 * @param   point   調べる位置
 * @return  true 位置はAABB内にある false 外側。
 */
inline bool AABB::IsContain( const VEC4& point ) const
{
	VEC4		contain = VEC4(point - m_min);
	contain.Minimize( contain, m_max - point );
	return !( contain.x < 0.0f || contain.y < 0.0f || contain.z < 0.0f );
}

/*!
 * @brief   １箇所でも交わっているか取得
 * @param   aabb    比較する領域
 * @return  true 交わっている false 交わっていない
 */
inline bool AABB::IsCross( const AABB& aabb ) const
{
	VEC4		cross = VEC4(aabb.m_max - m_min);
	cross.Minimize( cross, m_max - aabb.m_min );
	return !( cross.x < 0.0f || cross.y < 0.0f || cross.z < 0.0f );
}

/*!
 * @brief   要素を合成する
 * @param   vec     合成するベクトル
 */
inline AABB AABB::Merge( const VEC4& vec ) const
{
	VEC4		min;
	VEC4		max;

	max.Maximize(m_max, vec);
	min.Minimize(m_min, vec);
	
	return AABB( min,	max	);
}
/*!
 * @brief   要素を合成する
 * @param   aabb    合成する領域
 */
inline AABB AABB::Merge( const AABB& aabb ) const
{
	VEC4		min;
	VEC4		max;

	max.Maximize(m_max, aabb.m_max);
	min.Minimize(m_min, aabb.m_min);

	return AABB( min,	max	);
}
/*!
 * @brief   オフセット移動
 * @param   vec         移動ベクトル
 */
inline AABB AABB::Offset( const VEC4& vec ) const
{
	return AABB( m_min + vec,
							 m_max + vec );
}

inline void AABB::Create(const VEC3& posi1, const VEC3& posi2)
{
  if( posi1.x > posi2.x ) { m_max.x = posi1.x, m_min.x = posi2.x; }
  else                    { m_max.x = posi2.x, m_min.x = posi1.x; }

  if( posi1.y > posi2.y ) { m_max.y = posi1.y, m_min.y = posi2.y; }
  else                    { m_max.y = posi2.y, m_min.y = posi1.y; }

  if( posi1.z > posi2.z ) { m_max.z = posi1.z, m_min.z = posi2.z; }
  else                    { m_max.z = posi2.z, m_min.z = posi1.z; }
}

inline void AABB::Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3)
{
  m_min = m_max = posi1;
  if     ( m_max.x < posi2.x ) m_max.x = posi2.x;
  else if( m_min.x > posi2.x ) m_min.x = posi2.x;

  if     ( m_max.x < posi3.x ) m_max.x = posi3.x;
  else if( m_min.x > posi3.x ) m_min.x = posi3.x;

  if     ( m_max.y < posi2.y ) m_max.y = posi2.y;
  else if( m_min.y > posi2.y ) m_min.y = posi2.y;

  if     ( m_max.y < posi3.y ) m_max.y = posi3.y;
  else if( m_min.y > posi3.y ) m_min.y = posi3.y;

  if     ( m_max.z < posi2.z ) m_max.z = posi2.z;
  else if( m_min.z > posi2.z ) m_min.z = posi2.z;

  if     ( m_max.z < posi3.z ) m_max.z = posi3.z;
  else if( m_min.z > posi3.z ) m_min.z = posi3.z;
}

inline bool AABB::IsHit(const AABB& aabb) const
{
  if( m_max.x < aabb.m_min.x || m_min.x > aabb.m_max.x ) return false;
  if( m_max.z < aabb.m_min.z || m_min.z > aabb.m_max.z ) return false; // 通常のシーンであればzを先に見た方が速くはじける率が高い
  if( m_max.y < aabb.m_min.y || m_min.y > aabb.m_max.y ) return false;

  return true;
}

/*!
 * @brief   ユニット( -1?+1の範囲 )のＡＡＢＢを取得
 */
inline AABB AABB::GetUnitAABB( void )
{
	return AABB(
			VEC4( -1.0f, -1.0f, -1.0f, 1.0f ),
			VEC4(  1.0f,  1.0f,  1.0f, 1.0f )
	);
}

//---------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * @brief AABBクラス(VEC3版)
 */
//-----------------------------------------------------------------------------
#ifdef GFL_PLATFORM_3DS
class AABB_VEC3
{
public:
  VEC3 min;
  VEC3 max;

public:
  AABB_VEC3() { return ; } ///< コンストラクタ
  AABB_VEC3(const VEC3& _min, const VEC3& _max) : min(_min), max(_max) { return ; } ///< コンストラクタ
  AABB_VEC3(const VEC3& value) : min(value), max(value) { return ; } ///< コンストラクタ

  inline void Set(const VEC3& _min, const VEC3& _max) { min = _min; max = _max; } ///< 設定
  inline void Set(const VEC3& value)                  { min = max = value;      } ///< 設定
  inline void Set(const AABB& aabb)                   { VEC3Set(&min, aabb.GetMin()); VEC3Set(&max, aabb.GetMax()); } ///< 設定

  inline void Create(const VEC3& posi1, const VEC3& posi2); ///< 2点を内包するAABBを作成する
  inline void Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3); ///< 3点を内包するAABBを作成する(コリジョンで使用するため汎用性を犠牲にした)
  inline void Create(const VEC3  aposi[], const int num); ///< 複数点を内包するAABBを作成する
  inline void Create(const Sphere& sphere); ///< 球を内包するAABBを作成する
  inline void Merge(const AABB_VEC3& aabb) { VEC3Maximize(&max, &max, &aabb.max); VEC3Minimize(&min, &min, &aabb.min); } ///< 入力AABBをマージする
  inline void Margin(const float margin) { Margin(VEC3(margin, margin, margin)); } ///< マージンを加える
  inline void Margin(const VEC3& margin) { VEC3Sub(&min, &min, &margin); VEC3Add(&max, &max, &margin); } ///< マージンを加える
  inline void Scale(const float scale); ///< スケーリングを行う
  inline void Offset(const VEC3& ofst) { VEC3Add(&min, &min, &ofst); VEC3Add(&max, &max, &ofst); } ///< ずらす
  inline void Transform(const MTX34& mat); ///< 入力空間レベルのAABBへ変換する

  inline void GetCenter(VEC3* posi) const { VEC3Add(posi, &min, &max); VEC3Scale(posi, posi, 0.5f); } ///< 中心位置を取得する
  inline void GetLength(VEC3* len)  const { VEC3Sub(len, &max, &min); } ///< 長さを取得する
  inline void GetCenterAndLengthHalf(VEC3* posi, VEC3* lenHalf) const { GetCenter(posi); VEC3Sub(lenHalf, &max, posi); } ///< 中心位置と長さの半分を取得する

  inline bool IsVolumeZero(const float epsilon = 0.0f) const ///< 体積がゼロか判定する
  { return ( (max.x - min.x) <= epsilon || (max.y - min.y) <= epsilon || (max.z - min.z) <= epsilon ); }

  inline bool IsHit(const VEC3& p) const { return (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y && p.z >= min.z && p.z <= max.z); } ///< 点との衝突判定
  inline bool IsHit(const AABB_VEC3& aabb) const ; ///< AABBとの衝突判定
};

inline void AABB_VEC3::Create(const VEC3& posi1, const VEC3& posi2)
{
  if( posi1.x > posi2.x ) { max.x = posi1.x, min.x = posi2.x; }
  else                    { max.x = posi2.x, min.x = posi1.x; }

  if( posi1.y > posi2.y ) { max.y = posi1.y, min.y = posi2.y; }
  else                    { max.y = posi2.y, min.y = posi1.y; }

  if( posi1.z > posi2.z ) { max.z = posi1.z, min.z = posi2.z; }
  else                    { max.z = posi2.z, min.z = posi1.z; }
}

inline void AABB_VEC3::Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3)
{
  min = max = posi1;
  if     ( max.x < posi2.x ) max.x = posi2.x;
  else if( min.x > posi2.x ) min.x = posi2.x;

  if     ( max.x < posi3.x ) max.x = posi3.x;
  else if( min.x > posi3.x ) min.x = posi3.x;

  if     ( max.y < posi2.y ) max.y = posi2.y;
  else if( min.y > posi2.y ) min.y = posi2.y;

  if     ( max.y < posi3.y ) max.y = posi3.y;
  else if( min.y > posi3.y ) min.y = posi3.y;

  if     ( max.z < posi2.z ) max.z = posi2.z;
  else if( min.z > posi2.z ) min.z = posi2.z;

  if     ( max.z < posi3.z ) max.z = posi3.z;
  else if( min.z > posi3.z ) min.z = posi3.z;
}

inline void AABB_VEC3::Create(const VEC3 aposi[], const int num)
{
//  GFL_ASSERT(num > 0);
  if(num<=0){
    return;
  }

  min = max = aposi[0];
  for( int i = 1; i < num; i++ )
  {
    if     ( max.x < aposi[i].x ) max.x = aposi[i].x;
    else if( min.x > aposi[i].x ) min.x = aposi[i].x;

    if     ( max.y < aposi[i].y ) max.y = aposi[i].y;
    else if( min.y > aposi[i].y ) min.y = aposi[i].y;

    if     ( max.z < aposi[i].z ) max.z = aposi[i].z;
    else if( min.z > aposi[i].z ) min.z = aposi[i].z;
  }
}

inline void AABB_VEC3::Create(const Sphere& sphere)
{
  const VEC3 ofst(sphere.radius, sphere.radius, sphere.radius);
  VEC3Sub(&min, &sphere.center, &ofst);
  VEC3Add(&max, &sphere.center, &ofst);
}

inline void AABB_VEC3::Scale(const float scale)
{
  VEC3 center, lenHalf;
  GetCenterAndLengthHalf(&center, &lenHalf);
  VEC3Scale(&lenHalf, &lenHalf, scale);
  VEC3Add(&max, &center, &lenHalf);
  VEC3Sub(&min, &center, &lenHalf);
}

inline void AABB_VEC3::Transform(const MTX34& mat)
{
  const VEC3 localMin(min), localMax(max);
  for( int i = 0; i < 3; i++ ) // 全ての３つの軸に対して
  {
    min[i] = max[i] = mat.m[i][3]; // 始めに平行移動を加算

    // 小さい方の項と大きい方の項をそれぞれ合計することで幅を決定
    for( int j = 0; j < 3; j++ )
    {
      const float temp1 = mat.m[i][j]*localMin[j];
      const float temp2 = mat.m[i][j]*localMax[j];
      if( temp1 < temp2 )
      {
        min[i] += temp1;
        max[i] += temp2;
      }
      else
      {
        min[i] += temp2;
        max[i] += temp1;
      }
    }
  }
}

inline bool AABB_VEC3::IsHit(const AABB_VEC3& aabb) const
{
  if( max.x < aabb.min.x || min.x > aabb.max.x ) return false;
  if( max.z < aabb.min.z || min.z > aabb.max.z ) return false; // 通常のシーンであればzを先に見た方が速くはじける率が高い
  if( max.y < aabb.min.y || min.y > aabb.max.y ) return false;

  return true;
}

//---------------------------------------------------------------------------------------------------------------------

inline AABB::AABB( const AABB_VEC3& v ) : m_min(v.min.x, v.min.y, v.min.z, 0.0f) , m_max(v.max.x, v.max.y, v.max.z, 0.0f) { return ;
}
#else
class AABB_VEC3
{
public:
  VEC3 m_min;
  VEC3 m_max;

public:
  AABB_VEC3() { return ; } ///< コンストラクタ
  AABB_VEC3(const VEC3& _min, const VEC3& _max) : m_min(_min), m_max(_max) { return ; } ///< コンストラクタ
  AABB_VEC3(const VEC3& value) : m_min(value), m_max(value) { return ; } ///< コンストラクタ

  inline void Set(const VEC3& _min, const VEC3& _max) { m_min = _min; m_max = _max; } ///< 設定
  inline void Set(const VEC3& value)                  { m_min = m_max = value;      } ///< 設定
  inline void Set(const AABB& aabb)                   { VEC3Set(&m_min, aabb.GetMin()); VEC3Set(&m_max, aabb.GetMax()); } ///< 設定

  inline void Create(const VEC3& posi1, const VEC3& posi2); ///< 2点を内包するAABBを作成する
  inline void Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3); ///< 3点を内包するAABBを作成する(コリジョンで使用するため汎用性を犠牲にした)
  inline void Create(const VEC3  aposi[], const int num); ///< 複数点を内包するAABBを作成する
  inline void Create(const Sphere& sphere); ///< 球を内包するAABBを作成する
  inline void Merge(const AABB_VEC3& aabb) { VEC3Maximize(&m_max, &m_max, &aabb.m_max); VEC3Minimize(&m_min, &m_min, &aabb.m_min); } ///< 入力AABBをマージする
  inline void Margin(const float margin) { Margin(VEC3(margin, margin, margin)); } ///< マージンを加える
  inline void Margin(const VEC3& margin) { VEC3Sub(&m_min, &m_min, &margin); VEC3Add(&m_max, &m_max, &margin); } ///< マージンを加える
  inline void Scale(const float scale); ///< スケーリングを行う
  inline void Offset(const VEC3& ofst) { VEC3Add(&m_min, &m_min, &ofst); VEC3Add(&m_max, &m_max, &ofst); } ///< ずらす
  inline void Transform(const MTX34& mat); ///< 入力空間レベルのAABBへ変換する

  inline void GetCenter(VEC3* posi) const { VEC3Add(posi, &m_min, &m_max); VEC3Scale(posi, posi, 0.5f); } ///< 中心位置を取得する
  inline void GetLength(VEC3* len)  const { VEC3Sub(len, &m_max, &m_min); } ///< 長さを取得する
  inline void GetCenterAndLengthHalf(VEC3* posi, VEC3* lenHalf) const { GetCenter(posi); VEC3Sub(lenHalf, &m_max, posi); } ///< 中心位置と長さの半分を取得する

  inline bool IsVolumeZero(const float epsilon = 0.0f) const ///< 体積がゼロか判定する
  { return ( (m_max.x - m_min.x) <= epsilon || (m_max.y - m_min.y) <= epsilon || (m_max.z - m_min.z) <= epsilon ); }

  inline bool IsHit(const VEC3& p) const { return (p.x >= m_min.x && p.x <= m_max.x && p.y >= m_min.y && p.y <= m_max.y && p.z >= m_min.z && p.z <= m_max.z); } ///< 点との衝突判定
  inline bool IsHit(const AABB_VEC3& aabb) const ; ///< AABBとの衝突判定
};

inline void AABB_VEC3::Create(const VEC3& posi1, const VEC3& posi2)
{
  if( posi1.x > posi2.x ) { m_max.x = posi1.x, m_min.x = posi2.x; }
  else                    { m_max.x = posi2.x, m_min.x = posi1.x; }

  if( posi1.y > posi2.y ) { m_max.y = posi1.y, m_min.y = posi2.y; }
  else                    { m_max.y = posi2.y, m_min.y = posi1.y; }

  if( posi1.z > posi2.z ) { m_max.z = posi1.z, m_min.z = posi2.z; }
  else                    { m_max.z = posi2.z, m_min.z = posi1.z; }
}

inline void AABB_VEC3::Create(const VEC3& posi1, const VEC3& posi2, const VEC3& posi3)
{
  m_min = m_max = posi1;
  if     ( m_max.x < posi2.x ) m_max.x = posi2.x;
  else if( m_min.x > posi2.x ) m_min.x = posi2.x;

  if     ( m_max.x < posi3.x ) m_max.x = posi3.x;
  else if( m_min.x > posi3.x ) m_min.x = posi3.x;

  if     ( m_max.y < posi2.y ) m_max.y = posi2.y;
  else if( m_min.y > posi2.y ) m_min.y = posi2.y;

  if     ( m_max.y < posi3.y ) m_max.y = posi3.y;
  else if( m_min.y > posi3.y ) m_min.y = posi3.y;

  if     ( m_max.z < posi2.z ) m_max.z = posi2.z;
  else if( m_min.z > posi2.z ) m_min.z = posi2.z;

  if     ( m_max.z < posi3.z ) m_max.z = posi3.z;
  else if( m_min.z > posi3.z ) m_min.z = posi3.z;
}

inline void AABB_VEC3::Create(const VEC3 aposi[], const int num)
{
  if(num<=0){
    return;
  }

  m_min = m_max = aposi[0];
  for( int i = 1; i < num; i++ )
  {
    if     ( m_max.x < aposi[i].x ) m_max.x = aposi[i].x;
    else if( m_min.x > aposi[i].x ) m_min.x = aposi[i].x;

    if     ( m_max.y < aposi[i].y ) m_max.y = aposi[i].y;
    else if( m_min.y > aposi[i].y ) m_min.y = aposi[i].y;

    if     ( m_max.z < aposi[i].z ) m_max.z = aposi[i].z;
    else if( m_min.z > aposi[i].z ) m_min.z = aposi[i].z;
  }
}

inline void AABB_VEC3::Create(const Sphere& sphere)
{
  const VEC3 ofst(sphere.radius, sphere.radius, sphere.radius);
  VEC3Sub(&m_min, &sphere.center, &ofst);
  VEC3Add(&m_max, &sphere.center, &ofst);
}

inline void AABB_VEC3::Scale(const float scale)
{
  VEC3 center, lenHalf;
  GetCenterAndLengthHalf(&center, &lenHalf);
  VEC3Scale(&lenHalf, &lenHalf, scale);
  VEC3Add(&m_max, &center, &lenHalf);
  VEC3Sub(&m_min, &center, &lenHalf);
}

inline void AABB_VEC3::Transform(const MTX34& mat)
{
  const VEC3 localMin(m_min), localMax(m_max);
  for( int i = 0; i < 3; i++ ) // 全ての３つの軸に対して
  {
    m_min[i] = m_max[i] = mat.m[i][3]; // 始めに平行移動を加算

    // 小さい方の項と大きい方の項をそれぞれ合計することで幅を決定
    for( int j = 0; j < 3; j++ )
    {
      const float temp1 = mat.m[i][j]*localMin[j];
      const float temp2 = mat.m[i][j]*localMax[j];
      if( temp1 < temp2 )
      {
        m_min[i] += temp1;
        m_max[i] += temp2;
      }
      else
      {
        m_min[i] += temp2;
        m_max[i] += temp1;
      }
    }
  }
}

inline bool AABB_VEC3::IsHit(const AABB_VEC3& aabb) const
{
  if( m_max.x < aabb.m_min.x || m_min.x > aabb.m_max.x ) return false;
  if( m_max.z < aabb.m_min.z || m_min.z > aabb.m_max.z ) return false; // 通常のシーンであればzを先に見た方が速くはじける率が高い
  if( m_max.y < aabb.m_min.y || m_min.y > aabb.m_max.y ) return false;

  return true;
}

//---------------------------------------------------------------------------------------------------------------------

inline AABB::AABB( const AABB_VEC3& v ) : m_min(v.m_min.x, v.m_min.y, v.m_min.z, 0.0f) , m_max(v.m_max.x, v.m_max.y, v.m_max.z, 0.0f) { return ;
}
#endif
} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MATHAABB_H__

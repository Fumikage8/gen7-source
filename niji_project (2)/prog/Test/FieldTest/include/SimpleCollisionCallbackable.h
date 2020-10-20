//*************************************************************************************************
/**
 * @file SimpleCollisionCallbackable.h
 * @brief コリジョンモデルのコールバッククラス
 * @date 2015.02.10
 * @author saita_kazuki
 */
//*************************************************************************************************

#if !defined( SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED )
#define SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <Math/include/gfl2_Vector3.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//=================================================================================================
// コリジョンライブラリ、交差判定コールバッククラス
//=================================================================================================
class SimpleCollisionCallbackable_Intersection : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  static const u32 HIT_COUNT_MAX = 10;  ///< 衝突情報を格納する最大数

  ///< 衝突情報
  struct HIT_DATA
  {
    // @todo コリジョンモデルで使用されているのがVectorなのでここもVector。Vector3に置き換えたい。GetNearestIntersection()内部でもVector使用。
    gfl2::math::Vector intersection;                                                 ///< 交点
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;  ///< ポリゴン情報
  };

  // コンストラクタ、デストラクタ
  SimpleCollisionCallbackable_Intersection();
  virtual ~SimpleCollisionCallbackable_Intersection();

  /**
   * @brief 当たり判定コールバック関数
   */
  virtual bool CollisionModelIntersectionCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection);

  /**
   * @brief 指定座標と最も近い交点を返す
   * @param 比べる座標
   * @return 交点座標
   */
  const gfl2::math::Vector& GetNearestIntersection( const gfl2::math::Vector& pos) const ;

  /**
   * @brief 1度でも当たったかを返す
   * @retval true 当たった
   * @retval false 当たっていない
   */
  bool IsHit() const;

  /**
   * @brief 再取得可能な状態にする
   */
  void Clear( void);

private:

  u32       m_hitCount;                         // 当たった数
  HIT_DATA  m_hitDataTable[ HIT_COUNT_MAX ];    // 衝突情報バッファ
};

//=================================================================================================
// コリジョンライブラリ、高さ判定コールバッククラス
//=================================================================================================
class SimpleCollisionCallbackable_Height : public gfl2::collision::ICollisionModelCallbackable
{
public:

  static const u32 HIT_COUNT_MAX = 10;  ///< 衝突情報を格納する最大数

  ///< 衝突情報
  struct HIT_DATA
  {
    f32   height;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  // コンストラクタ、デストラクタ
  SimpleCollisionCallbackable_Height();
  virtual ~SimpleCollisionCallbackable_Height();

  /**
   * @brief 当たり判定コールバック関数
   */
  virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height);

  /**
   * @brief 指定の高さに一番近い高さを取得する
   * @note 指定した高さよりも低いところでしか当たっていない場合は指定した高さそのままを返す
   * @param 比べる高さ
   * @return 衝突ポリゴンの高さ
   */
  f32 GetNearestHeight( f32 height) const ;

  /**
   * @brief 1度でも当たったかを返す
   * @retval true 当たった
   * @retval false 当たっていない
   */
  bool IsHit() const;

  /**
   * @brief 再取得可能な状態にする
   */
  void Clear( void);

private:

  u32       m_hitCount;                         // 当たった数
  HIT_DATA  m_hitDataTable[ HIT_COUNT_MAX ];    // 衝突情報バッファ
};

//=================================================================================================
// コリジョンライブラリ、レイカスタムコールバッククラス
//=================================================================================================
class customCollisionCallback_Intersection : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  ///< 衝突情報
  struct HIT_DATA
  {
    gfl2::math::Vector intersection;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  /**
   * @brief コンストラクタ
   */
  customCollisionCallback_Intersection();

  /**
   * @brief デストラクタ
   */
  virtual ~customCollisionCallback_Intersection();

  /**
   * @brief 当たり判定コールバック関数
   */
  virtual bool CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection);

  /**
   * @brief レイの一番近い交点を取得する
   * @return 衝突データ
   */
  HIT_DATA GetHitData(void) const;

  /**
   * @brief 1度でも当たったかを返す
   * @retval true 当たった
   * @retval false 当たっていない
   */
  bool IsHit(void) const;

private:

  b32       m_isHit;
  HIT_DATA  m_hitData;

};

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // SIMPLECOLLISIONCALLBACKABLE_H_INCLUDED

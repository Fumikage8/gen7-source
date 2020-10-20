#pragma once
//=================================================================================================
/**
*  @file   RaycastCustomCallback.h
*  @brief  レイキャスト カスタムコールバック
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <Collision/include/gfl2_CollisionModel.h>

//----------------------------------------------------------------------------------------
//! @brief レイキャスト カスタムコールバッククラス
//----------------------------------------------------------------------------------------
class RaycastCustomCallback : public gfl2::collision::ICollisionModelIntersectionCallbackable
{
public:

  // 衝突情報
  struct HIT_DATA
  {
    gfl2::math::Vector4 intersection;
    const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle;
  };

  //-----------------------------------------------------------------------------
  /**
  * @brief コンストラクタ
  */
  //-----------------------------------------------------------------------------
  RaycastCustomCallback();

  //-----------------------------------------------------------------------------
  /**
  * @brief デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~RaycastCustomCallback();

  //-----------------------------------------------------------------------------
  /**
  * @brief 当たり判定コールバック関数
  */
  //-----------------------------------------------------------------------------
  virtual bool CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector4& intersection);

  //-----------------------------------------------------------------------------
  /**
  * @brief レイの一番近い交点を取得する
  * @return 衝突データ
  */
  //-----------------------------------------------------------------------------
  HIT_DATA GetHitData(void) const;

  //-----------------------------------------------------------------------------
  /**
  * @brief 1度でも当たったかを返す
  * @retval true 当たった
  * @retval false 当たっていない
  */
  //-----------------------------------------------------------------------------
  bool IsHit(void) const;

private:

  b32       m_isHit;
  HIT_DATA  m_hitData;

};
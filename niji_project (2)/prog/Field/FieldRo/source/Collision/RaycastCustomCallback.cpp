//=================================================================================================
/**
*  @file   RaycastCustomCallback.h
*  @brief  レイキャスト カスタムコールバック
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "Field/FieldRo/include/Collision/RaycastCustomCallback.h"

//-----------------------------------------------------------------------------
/**
* @brief コンストラクタ
*/
//-----------------------------------------------------------------------------
RaycastCustomCallback::RaycastCustomCallback()
: m_isHit(false)
{
}

//-----------------------------------------------------------------------------
/**
* @brief デストラクタ
*/
//-----------------------------------------------------------------------------
RaycastCustomCallback::~RaycastCustomCallback()
{
}

//-----------------------------------------------------------------------------
/**
* @brief 当たり判定コールバック関数
*/
//-----------------------------------------------------------------------------
bool RaycastCustomCallback::CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector4& intersection)
{
  m_hitData.intersection = intersection;
  m_hitData.pTriangle    = pTriangle;
  m_isHit                = true;
  return true;
}

//-----------------------------------------------------------------------------
/**
* @brief レイの一番近い交点を取得する
* @return 衝突データ
*/
//-----------------------------------------------------------------------------
RaycastCustomCallback::HIT_DATA RaycastCustomCallback::GetHitData(void) const
{
  return m_hitData;
}

//-----------------------------------------------------------------------------
/**
* @brief 1度でも当たったかを返す
* @retval true 当たった
* @retval false 当たっていない
*/
//-----------------------------------------------------------------------------
bool RaycastCustomCallback::IsHit() const
{
  return m_isHit ? true:false;
}

//*************************************************************************************************
/**
 * @file SimpleCollisionCallbackable.cpp
 * @brief コリジョンモデルのコールバッククラス
 * @date 2015.02.10
 * @author saita_kazuki
 */
//*************************************************************************************************

#include "Test/FieldTest/include/SimpleCollisionCallbackable.h"
#include <math/include/gfl2_math_control.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//=================================================================================================
// コリジョンライブラリ、交点判定コールバッククラス
//=================================================================================================

// コンストラクタ
SimpleCollisionCallbackable_Intersection::SimpleCollisionCallbackable_Intersection() :
  m_hitCount( 0 )
{
  for( u32 i = 0; i < HIT_COUNT_MAX; i++)
  {
    m_hitDataTable[i].intersection = gfl2::math::Vector::GetZero();
    m_hitDataTable[i].pTriangle = NULL;
  }
}

// デストラクタ
SimpleCollisionCallbackable_Intersection::~SimpleCollisionCallbackable_Intersection()
{
}

// 当たり判定コールバック関数
bool SimpleCollisionCallbackable_Intersection::CollisionModelIntersectionCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection)
{
  // 交点を記憶
  m_hitDataTable[m_hitCount].intersection = intersection;
  m_hitDataTable[m_hitCount].pTriangle = pTriangle;
  m_hitCount++;

  if( m_hitCount >= HIT_COUNT_MAX)
  {
    // 衝突検出終了
    return false;
  }
  else
  {
    // 衝突検出続行
    return true;
  }
}

// 指定座標と最も近い交点を返す
const gfl2::math::Vector& SimpleCollisionCallbackable_Intersection::GetNearestIntersection( const gfl2::math::Vector& pos) const
{
  gfl2::math::Vector vec = pos - m_hitDataTable[0].intersection;
  f32 resultDiff = vec.Length3();
  u32 hitIndex = 0;
  for( int i = 1; i < m_hitCount; ++i)
  {
    vec = pos - m_hitDataTable[i].intersection;
    f32 diff = vec.Length3();
    if( diff < resultDiff)
    {
      resultDiff = diff;
      hitIndex = i;
    }
  }

  return m_hitDataTable[hitIndex].intersection;
}

// 当たったか
bool SimpleCollisionCallbackable_Intersection::IsHit() const
{
  return ( m_hitCount != 0 );
}

// 再取得可能な状態にする
void SimpleCollisionCallbackable_Intersection::Clear( void)
{
  m_hitCount = 0;
}

//=================================================================================================
// コリジョンライブラリ、高さ判定コールバッククラス
//=================================================================================================

// コンストラクタ
SimpleCollisionCallbackable_Height::SimpleCollisionCallbackable_Height() :
  m_hitCount( 0 )
{
  for( u32 i = 0; i < HIT_COUNT_MAX; i++)
  {
    m_hitDataTable[i].height = 0.0f;
    m_hitDataTable[i].pTriangle = NULL;
  }
}

// デストラクタ
SimpleCollisionCallbackable_Height::~SimpleCollisionCallbackable_Height()
{
}

// 当たり判定コールバック関数 交点取得
bool SimpleCollisionCallbackable_Height::CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height)
{
  // 交点を記憶
  m_hitDataTable[m_hitCount].height = height;
  m_hitDataTable[m_hitCount].pTriangle = pTriangle;
  m_hitCount++;

  if( m_hitCount >= HIT_COUNT_MAX)
  {
    // 衝突検出終了
    return false;
  }
  else
  {
    // 衝突検出続行
    return true;
  }
}

// 指定の高さに一番近い高さを取得する
f32 SimpleCollisionCallbackable_Height::GetNearestHeight(f32 height) const
{
  // 一番近い衝突メッシュを探す
  f32 resultDiff = FLT_MAX;
  s32 hitIndex = -1;
  for( u32 i = 0; i < m_hitCount; i++)
  {
    {
      f32 diff = gfl2::math::Abs( m_hitDataTable[i].height - height);
      if( diff < resultDiff)
      {
        resultDiff = diff;
        hitIndex = i;
      }
    }
  }

  if( hitIndex != -1)
  {
    return m_hitDataTable[hitIndex].height;
  }
  return height;
}

// 当たったか
bool SimpleCollisionCallbackable_Height::IsHit() const
{
  return ( m_hitCount != 0 );
}

// 再取得可能な状態にする
void SimpleCollisionCallbackable_Height::Clear( void)
{
  m_hitCount = 0;
}

//=================================================================================================
// コリジョンライブラリ、レイカスタムコールバッククラス
//=================================================================================================

// コンストラクタ
customCollisionCallback_Intersection::customCollisionCallback_Intersection()
: m_isHit(false)
{
}

// デストラクタ
customCollisionCallback_Intersection::~customCollisionCallback_Intersection()
{
}

// 当たり判定コールバック関数 交点取得
bool customCollisionCallback_Intersection::CollisionModelIntersectionCallback(const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, gfl2::math::Vector& intersection)
{
  if( !m_isHit || m_hitData.intersection.Length() > intersection.Length() )
  {
    m_hitData.intersection  = intersection;
    m_hitData.pTriangle     = pTriangle;
    m_isHit                 = true;
  }
  return true;
}

// レイの一番近い交点を取得する
customCollisionCallback_Intersection::HIT_DATA customCollisionCallback_Intersection::GetHitData(void) const
{
  return m_hitData;
}

// 当たったか
bool customCollisionCallback_Intersection::IsHit() const
{
  return m_isHit;
}

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#include <Collision/include/gfl2_CollisionModel.h>

namespace gfl2 { namespace collision {

/**
 * @brief コンストラクタ
 */
CollisionModel::CollisionModel() :
  m_pCollisionMeshNode(NULL)
{
}

/**
 * @brief デストラクタ
 */
CollisionModel::~CollisionModel()
{
}

/**
 * @brief 初期化
 *
 * @param pCollisionMeshNode コリジョンメッシュノード
 */
void CollisionModel::Initialize(renderingengine::scenegraph::resource::ResourceNode* pCollisionMeshNode)
{
  m_pCollisionMeshNode = pCollisionMeshNode->GetChild()->SafeCast<renderingengine::scenegraph::resource::CollisionMeshResourceNode>();
}

void CollisionModel::GetHeight(const math::Vector4& position, const renderingengine::scenegraph::resource::CollisionTriangle* pOldTriangle, ICollisionModelCallbackable* pCallbackable)
{
  // コールバックオブジェクトが無ければ無視
  if (pCallbackable == NULL)
  {
    return;
  }

  // 前回のトライアングル上にいる可能性が一番高い
  if (pOldTriangle != NULL)
  {
    f32 height = 0.0f;
    if (pOldTriangle->IntersectsXZ(&height, position))
    {
      // 衝突しているのでコールバックする
      if (!pCallbackable->CollisionModelHeightCallback(pOldTriangle, height))
      {
        // 衝突検出終了
        return;
      }
    }
  }

  // 全トライアングルと衝突検出
  u32 triangleCount = m_pCollisionMeshNode->GetCollisionTriangleCount();
  for (u32 i = 0; i < triangleCount; ++i)
  {
    const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = m_pCollisionMeshNode->GetCollisionTriangle(i);

    // 衝突しているか？
    f32 height = 0.0f;
    if (pTriangle->IntersectsXZ(&height, position))
    {
      // 衝突しているのでコールバックする
      if (!pCallbackable->CollisionModelHeightCallback(pTriangle, height))
      {
        // 衝突検出終了
        return;
      }
    }
  }
}

void CollisionModel::GetIntersection(const math::Vector4& st, const math::Vector4& ed, ICollisionModelIntersectionCallbackable* pCallbackable)
{
  // コールバックオブジェクトが無ければ無視
  if (pCallbackable == NULL)
  {
    return;
  }

  // 全トライアングルと衝突検出
  gfl2::math::Vector4 nearestIntersection(0.0f,0.0f,0.0f,0.0f);
  u32                 triangleCount = m_pCollisionMeshNode->GetCollisionTriangleCount();
  b32                 isFirst       = true;

  for (u32 i = 0; i < triangleCount; ++i)
  {
    // 衝突しているか？
    math::Vector4 newIntersection;
    const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = m_pCollisionMeshNode->GetCollisionTriangle(i);
    if (pTriangle->IntersectsRay( &newIntersection, st, ed))
    {
      // レイキャストの開始点に一番近いものを確保
      gfl2::math::Vector4 temp0( nearestIntersection - st );
      gfl2::math::Vector4 temp1( newIntersection     - st );
      if( isFirst || temp0.Length3() > temp1.Length3() )
      {
        isFirst = false;
        pCallbackable->CollisionModelIntersectionCallback(pTriangle, newIntersection);
        nearestIntersection = newIntersection;
      }
    }
  }
}

void CollisionModel::GetIntersectionEx(const math::Vector4& st, const math::Vector4& ed, ICollisionModelIntersectionCallbackable* pCallbackable)
{
  // コールバックオブジェクトが無ければ無視
  if (pCallbackable == NULL)
  {
    return;
  }

  // 全トライアングルと衝突検出
  gfl2::math::Vector4 nearestIntersection(0.0f,0.0f,0.0f,0.0f);
  u32                 triangleCount = m_pCollisionMeshNode->GetCollisionTriangleCount();
  b32                 isFirst       = true;

  for (u32 i = 0; i < triangleCount; ++i)
  {
    // 衝突しているか？
    math::Vector4 newIntersection;
    const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle = m_pCollisionMeshNode->GetCollisionTriangle(i);
    if (pTriangle->IntersectsRayEx( &newIntersection, st, ed))
    {
      // レイキャストの開始点に一番近いものを確保
      gfl2::math::Vector4 temp0( nearestIntersection - st );
      gfl2::math::Vector4 temp1( newIntersection     - st );
      if( isFirst || temp0.Length3() < temp1.Length3() )
      {
        isFirst = false;
        pCallbackable->CollisionModelIntersectionCallback(pTriangle, newIntersection);
      }
    }
  }
}

s32 CollisionModel::GetIntersectionCnt(const math::Vector4& st, const math::Vector4& ed)
{
  // 全トライアングルと衝突検出
  gfl2::math::Vector4 intersection (0.0f,0.0f,0.0f,0.0f);
  u32                 triangleCount(m_pCollisionMeshNode->GetCollisionTriangleCount());
  u32                 hitCount     (0);

  for (u32 i = 0; i < triangleCount; ++i)
  {
    // 衝突しているか？
    s32 result = m_pCollisionMeshNode->GetCollisionTriangle(i)->IntersectsRayWithoutEdge( &intersection, st, ed);

    // 計測失敗
    if( result == -1 )
    {
      return -1;
    }

    // 衝突している
    if( result == 1 )
    {
      hitCount++;
    }
  }

  return hitCount;
}

}}

#ifndef GFLIB2_COLLISION_CollisionModel_H_INCLUDED
#define GFLIB2_COLLISION_CollisionModel_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>

namespace gfl2 { namespace collision {

//-----------------------------------------------------------
//! @brief コリジョンモデルコールバックインターフェース
//-----------------------------------------------------------
class ICollisionModelCallbackable
{
public:

  /**
   * @brief コンストラクタ
   */
  ICollisionModelCallbackable() {}

  /**
   * @brief デストラクタ
   */
  virtual ~ICollisionModelCallbackable() {}

  virtual bool CollisionModelHeightCallback(const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height) = 0;
};

//-----------------------------------------------------------
//! @brief コリジョンモデル交差判定コールバックインターフェース
//-----------------------------------------------------------
class ICollisionModelIntersectionCallbackable
{
public:

  /**
   * @brief コンストラクタ
   */
  ICollisionModelIntersectionCallbackable() {}

  /**
   * @brief デストラクタ
   */
  virtual ~ICollisionModelIntersectionCallbackable() {}

  virtual bool CollisionModelIntersectionCallback(const renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, math::Vector4& intersection) = 0;
};

//-----------------------------------------------------------
//! @brief コリジョンモデル
//-----------------------------------------------------------
class CollisionModel
{
public:

  /**
   * @brief コンストラクタ
   */
  CollisionModel();

  /**
   * @brief デストラクタ
   */
  ~CollisionModel();

  /**
   * @brief 初期化
   *
   * @param pCollisionMeshNode コリジョンメッシュノード
   */
  void Initialize(renderingengine::scenegraph::resource::ResourceNode* pCollisionMeshNode);

  /**
   * @brief 高さ取得
   */
  void GetHeight(const math::Vector4& position, const renderingengine::scenegraph::resource::CollisionTriangle* pOldTriangle, ICollisionModelCallbackable* pCallbackable);

  /**
   * @brief 交点取得
   */
  void GetIntersection(const math::Vector4& st, const math::Vector4& ed, ICollisionModelIntersectionCallbackable* pCallbackable);

  /**
   * @brief 交点取得 表裏も判定する版
   */
  void GetIntersectionEx(const math::Vector4& st, const math::Vector4& ed, ICollisionModelIntersectionCallbackable* pCallbackable);

  /**
   * @brief 交点数取得 交点が辺上だと無効とする版
   */
  s32 GetIntersectionCnt(const math::Vector4& st, const math::Vector4& ed);

  /**
   * @brief メッシュリソースノードを取得
   */
  renderingengine::scenegraph::resource::CollisionMeshResourceNode* GetCollisionMeshNode(void){ return m_pCollisionMeshNode; }

protected:

  renderingengine::scenegraph::resource::CollisionMeshResourceNode* m_pCollisionMeshNode;
};

}}

#endif


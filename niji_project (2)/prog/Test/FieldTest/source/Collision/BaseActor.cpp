//=================================================================================================
/**
 *  @file   BaseActor.cpp
 *  @brief  アクターベース
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "../../include/Collision/BaseActor.h"
#include "../../include/Collision/BaseShape.h"

// RTTIデバッグシステム
#include "../../include/RttiDebug/RttiManager.h"

//----------------------------------------------------------------------------------------
//! @brief アクターベース
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
BaseActor::BaseActor() : gfl2::renderingengine::scenegraph::NodeTrait<BaseActor, InstanceNode>()
, m_pAllocator    (NULL)
, m_shape         (NONE)
, m_pShape        (NULL)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
BaseActor::~BaseActor()
{
  Clear();
}

//-----------------------------------------------------------------------------
/**
*  @brief  解放
*/
//-----------------------------------------------------------------------------
void BaseActor::Clear(void)
{
  if( m_pShape )
  {
    GFL_SAFE_DELETE( m_pShape );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにスフィア2Dを追加
*
*  @param  pos        スフィアの中心位置
*  @param  r          スフィアの半径
*/
//-----------------------------------------------------------------------------
void BaseActor::createSphere2D( const gfl2::math::Vector3& pos,
                                f32                        r )
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  // スフィア形状の作成 
  SphereShape2D* sphereShape = new SphereShape2D();
  sphereShape->m_pos         = pos;
  sphereShape->m_r           = r;
  m_pShape                   = sphereShape;
  m_shape                    = SPHERE_2D;

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにスフィア3Dを追加
*
*  @param  pos        スフィアの中心位置
*  @param  r          スフィアの半径
*/
//-----------------------------------------------------------------------------
void BaseActor::createSphere3D( const gfl2::math::Vector3& pos,
                                f32                        r )
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  // スフィア形状の作成 
  SphereShape3D* sphereShape = new SphereShape3D();
  sphereShape->m_pos         = pos;
  sphereShape->m_r           = r;
  m_pShape                   = sphereShape;
  m_shape                    = SPHERE_3D;

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにBox2Dを追加
*
*  @param  pos        長方形の中心点
*  @param  u0         長方形のローカルなX軸を表す単位ベクトル
*  @param  u1         長方形のローカルなY軸を表す単位ベクトル
*  @param  e0         X軸にそった長方形の長さの半分
*  @param  e1         Y軸にそった長方形の長さの半分
*/
//-----------------------------------------------------------------------------
void BaseActor::createBox2D( const gfl2::math::Vector3& pos,
                             const gfl2::math::Vector3& u0,
                             const gfl2::math::Vector3& u1,
                             const f32                  e0,
                             const f32                  e1)
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  // スフィア形状の作成
  BoxShape2D* boxShape = new BoxShape2D();
  boxShape->m_pos      = pos;
  boxShape->m_u0       = u0;
  boxShape->m_u1       = u1;
  boxShape->m_e0       = e0;
  boxShape->m_e1       = e1;
  m_pShape             = boxShape;
  m_shape              = BOX_2D;

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにメッシュを追加
*
*  @param  pos              メッシュの中心点
*  @param  pCollisionModel  メッシュ
*/
//-----------------------------------------------------------------------------
void BaseActor::createMesh3D( const gfl2::math::Vector3&       pos,
                              gfl2::collision::CollisionModel *pCollisionModel )
{
  // コリジョンシステム全体で利用するアロケータを統一するために、↓の方法で対象アロケータを変更する(シーンにあるアクター用ノードのメモリ確保方法にあわせる)
  gfl2::gfx::IGLAllocator* pDefaultAllocator = gfl2::gfx::GLMemory::GetAllocator();
  gfl2::gfx::GLMemory::SetAllocator(m_pAllocator);

  // スフィア形状の作成
  MeshShape* meshShape         = new MeshShape();
  meshShape->m_pos             = pos;
  meshShape->m_pCollisionModel = pCollisionModel;
  m_pShape                     = meshShape;
  m_shape                      = MESH_3D;

  // 利用するアロケータを元に戻す
  gfl2::gfx::GLMemory::SetAllocator(pDefaultAllocator);

}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void BaseActor::RegistRtti(void)
{
#if USE_RTTI_DEBUG
#endif
}

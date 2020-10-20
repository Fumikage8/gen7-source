//=================================================================================================
/**
 *  @file   BaseActor.cpp
 *  @brief  アクターベース
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "ViewerProc/include/FieldStatic/Collision/BaseActor.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"

// RTTIデバッグシステム
///#include "../../include/RttiDebug/RttiManager.h"

//----------------------------------------------------------------------------------------
//! @brief アクターベース
//----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
BaseActor::BaseActor() : gfl2::renderingengine::scenegraph::NodeTrait<BaseActor, InstanceNode>()
, m_isActive      (true)
, m_pHeap         (NULL)
, m_shape         (NONE)
, m_pShape        (NULL)
, m_userData      (NULL)
, m_position      (0.0f,0.0f,0.0f)
, m_quaternion    (0.0f,0.0f,0.0f,1.0f)
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
*  @brief  衝突の結果を初期化
*/
//-----------------------------------------------------------------------------
void BaseActor::InitCollisionResult(void)
{ 
  for( u32 i=0 ; i<MAX_COLLISION_RESULT_CNT ; i++ )
  {
    m_result[i].init();
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにシリンダーを追加
*
*  @param  pos        シリンダーの中心位置
*  @param  r          シリンダーの半径
*  @param  height     シリンダーの高さ
*/
//-----------------------------------------------------------------------------
void BaseActor::CreateCylinder( const gfl2::math::Vector3& pos,
                                const f32                  r,
                                const f32                  height)
{
  // 既に基本形状を設定されている
  if( m_shape != NONE )
  {
    return;
  }

  // シリンダー形状の作成 
  CylinderShape* sphereShape = GFL_NEW(m_pHeap) CylinderShape();
  sphereShape->m_pos         = pos;
  sphereShape->m_r           = r;
  sphereShape->m_height      = height;
  m_pShape                   = sphereShape;
  m_shape                    = CYLINDER;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにスフィアを追加
*
*  @param  pos        スフィアの中心位置
*  @param  r          スフィアの半径
*/
//-----------------------------------------------------------------------------
void BaseActor::CreateSphere( const gfl2::math::Vector3& pos,
                              const f32                  r )
{
  // 既に基本形状を設定されている
  if( m_shape != NONE )
  {
    return;
  }

  // スフィア形状の作成 
  SphereShape* sphereShape = GFL_NEW(m_pHeap) SphereShape();
  sphereShape->m_pos       = pos;
  sphereShape->m_r         = r;
  m_pShape                 = sphereShape;
  m_shape                  = SPHERE;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにBoxを追加
*
*  @param  pos        長方形の中心点
*  @param  u0         長方形のローカルなX軸を表す単位ベクトル
*  @param  u1         長方形のローカルなY軸を表す単位ベクトル
*  @param  e0         X軸にそった長方形の長さの半分
*  @param  e1         Y軸にそった長方形の長さの半分
*  @param  height     長方形の高さ
*/
//-----------------------------------------------------------------------------
void BaseActor::CreateBox( const gfl2::math::Vector3& pos,
                           const gfl2::math::Vector3& u0,
                           const gfl2::math::Vector3& u1,
                           const f32                  e0,
                           const f32                  e1,
                           const f32                  height)
{
  // 既に基本形状を設定されている
  if( m_shape != NONE )
  {
    return;
  }

  // 箱形状の作成
  BoxShape* boxShape    = GFL_NEW(m_pHeap) BoxShape();
  boxShape->m_pos       = pos;
  boxShape->m_u0        = u0;
  boxShape->m_u1        = u1;
  boxShape->m_e0        = e0;
  boxShape->m_e1        = e1;
  boxShape->m_height    = height;
  m_pShape              = boxShape;
  m_shape               = BOX;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにメッシュを追加
*
*  @param  pos              メッシュの中心点
*  @param  pCollisionModel  メッシュ
*/
//-----------------------------------------------------------------------------
void BaseActor::CreateMesh( const gfl2::math::Vector3&       pos,
                            gfl2::collision::CollisionModel *pCollisionModel )
{
  // 既に基本形状を設定されている
  if( m_shape != NONE )
  {
    return;
  }

  // メッシュ形状の作成
  MeshShape* meshShape         =  GFL_NEW(m_pHeap) MeshShape();
  meshShape->m_pos             = pos;
  meshShape->m_pCollisionModel = pCollisionModel;
  m_pShape                     = meshShape;
  m_shape                      = MESH;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクターにラインを追加
*
*  @param  pos0       ラインの先端座標
*  @param  pos1       ラインの終端座標
*  @param  height     ラインの高さ
*/
//-----------------------------------------------------------------------------
void BaseActor::CreateLine( const gfl2::math::Vector3 pos0,
                            const gfl2::math::Vector3 pos1,
                            const f32                 height)
{
  // 既に基本形状を設定されている
  if( m_shape != NONE )
  {
    return;
  }

  // メッシュ形状の作成
  LineShape* lineShape   =  GFL_NEW(m_pHeap) LineShape();
  lineShape->m_pos0      = pos0;
  lineShape->m_pos1      = pos1;
  lineShape->m_height    = height;
  m_pShape               = lineShape;
  m_shape                = LINE;
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

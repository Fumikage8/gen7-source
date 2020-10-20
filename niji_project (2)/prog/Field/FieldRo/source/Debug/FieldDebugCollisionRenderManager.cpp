//======================================================================
/**
 * @file FieldDebugCollisionRenderManager.cpp
 * @date 2015/07/10 17:44:18
 * @author ikawa_hiroki
 * @brief フィールド関連コリジョン可視化管理マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Collision/BaseShape.h"

#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DEFINITION( Field::Debug::DebugCollisionRenderManager );
template class gfl2::base::SingletonAccessor< Field::Debug::DebugCollisionRenderManager >;


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Debug )

//class Camera::Area::PrimitiveCollisionDataAccessor;

/**
  * @brief コンストラクタ
  * @return なし
  * @detail 外部からのアクセス禁止
  */
DebugCollisionRenderManager::DebugCollisionRenderManager( void ) :
m_pActor( m_RenderCollisionActorInfo ),
m_nActor(0),
m_pModel( m_RenderCollisionModelInfo ),
m_nModel(0),
m_pPrim( m_RenderCollisionPrimInfo ),
m_nPrim(0),
m_pEnvSound( m_RenderEnvSoundCollisionInfo ),
m_nEnvSound(0),
m_PlyerPosY(0.0f)
{
  for(int i=0; i<MAX_RENDER_ACTOR; i++)
  {
    m_RenderCollisionActorInfo[i].pActor = NULL;
    m_RenderCollisionActorInfo[i].color = gfl2::math::Vector3(0, 0, 0);
  }
  for(int i=0; i<MAX_RENDER_MODEL; i++)
  {
    m_RenderCollisionModelInfo[i].pModel = NULL;
    m_RenderCollisionModelInfo[i].color = gfl2::math::Vector3(0, 0, 0);
  }
  for(int i=0; i<MAX_RENDER_PRIM; i++)
  {
    m_RenderCollisionPrimInfo[i].type = 0;
    m_RenderCollisionPrimInfo[i].position = gfl2::math::Vector3(0, 0, 0);
    m_RenderCollisionPrimInfo[i].pShape = NULL;
    m_RenderCollisionPrimInfo[i].color = gfl2::math::Vector3(0, 0, 0);
  }
  for(int i=0; i<MAX_RENDER_ENV_SOUND; i++)
  {
    m_RenderEnvSoundCollisionInfo[i].cpData = NULL;
    m_RenderEnvSoundCollisionInfo[i].color = gfl2::math::Vector3(0, 0, 0);
  }
}



/**
* @brief コリジョンアクターを登録
* @return コリジョンアクターリストのポインタ
* @detail 
*/
void DebugCollisionRenderManager::RegistRenderCollisionActor( BaseActor* pActor, gfl2::math::Vector4 color, gfl2::math::Vector3 positon, const gfl2::math::Vector4* pColTable, u32 colTableSize )
{
  if(m_nActor < MAX_RENDER_ACTOR)
  {
    m_pActor->pActor = pActor;
    m_pActor->color = color;
    m_pActor->pos = positon;
    m_pActor->pColorTable = pColTable;
    m_pActor->colorTableSize = colTableSize;
    m_pActor ++;
    m_nActor ++;
  }
};


/**
* @brief コリジョンアクターを登録解除
* @return なし
* @detail 
*/
void DebugCollisionRenderManager::ClearRenderCollisionActorInfo()
{
  for(u32 i=0; i<MAX_RENDER_ACTOR; i++)
  {
    m_RenderCollisionActorInfo[i].pActor = NULL;
    m_RenderCollisionActorInfo[i].color = gfl2::math::Vector4(0, 0, 0);
  }
  m_pActor = m_RenderCollisionActorInfo;
  m_nActor = 0;
}


/**
* @brief コリジョンモデルを登録
* @return なし
* @detail 
*/
void DebugCollisionRenderManager::RegistRenderCollisionModel( gfl2::collision::CollisionModel* pModel, gfl2::math::Vector4 color, b32 isHightClamp, s32 hitAttr )
{
  if(( m_nModel < MAX_RENDER_MODEL ) && ( pModel != NULL ))
  {
    m_pModel->pModel = pModel;
    m_pModel->color = color;
    m_pModel->isHightClamp = isHightClamp;
    m_pModel->hitAttr = hitAttr;
    m_pModel ++;
    m_nModel ++;
  }
}


/**
+* @brief コリジョンモデルを登録解除
* @return なし
* @detail
*/
void DebugCollisionRenderManager::ClearRenderCollisionModelInfo()
{
  for(u32 i=0; i<MAX_RENDER_MODEL; i++)
  {
    m_RenderCollisionModelInfo[i].pModel = NULL;
    m_RenderCollisionModelInfo[i].color = gfl2::math::Vector4(0, 0, 0);
  }
  m_pModel = m_RenderCollisionModelInfo;
  m_nModel = 0;
}


/**
* @brief コリジョンプリミティブを登録
* @return なし
* @detail
*/
void DebugCollisionRenderManager::RegistRenderCollisionCylinder( gfl2::math::Vector3 position, f32 radius, f32 height, gfl2::math::Vector4 color, b32 isDepthTestEnable )
{
  if(( m_nPrim < MAX_RENDER_PRIM ) && ( radius != 0.0f ))
  {
    // ヒープ領域取得
    gfl2::heap::HeapBase* pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG )->GetLowerHandle();
    
    // 確保できるかチェック
    // @note 重いがメモリ使用量を節約するため許容にしている。PreDrawで確保され、Drawで破棄されるため、この処理により他のモジュールでメモリが足りなくなる事は無い
    size_t memSize = sizeof( CylinderShape ) + pDebugHeap->CalcAllocateAdjustSize( pDebugHeap->GetDefaultAlign() ) * 1.5f; // 1.5倍は念のため
    if( memSize >= pDebugHeap->GetTotalAllocatableSize() )
    {
      return;
    }

    m_pPrim->type = COLLISION_PRIMITIVE_CYLINDER;
    m_pPrim->position = position;

    CylinderShape* pShape = GFL_NEW(pDebugHeap) CylinderShape();

    pShape->m_r = radius;
    pShape->m_height = height;

    m_pPrim->pShape = static_cast<BaseShape*>( pShape );
    m_pPrim->color = color;
    m_pPrim->isDepthTestEnable = isDepthTestEnable;
    m_pPrim ++;
    m_nPrim ++;
  }
}


/**
* @brief コリジョンモデルを登録解除
* @return なし
* @detail
*/
void DebugCollisionRenderManager::ClearRenderCollisionPrimitiveInfo()
{
  for(u32 i=0; i<MAX_RENDER_PRIM; i++)
  {
    // 生成した形状データを破棄
    if(m_RenderCollisionPrimInfo[i].type == COLLISION_PRIMITIVE_CYLINDER)
    {
      CylinderShape* pShape = static_cast<CylinderShape*>(m_RenderCollisionPrimInfo[i].pShape);
      GFL_SAFE_DELETE( pShape );
    }
    m_RenderCollisionPrimInfo[i].position = gfl2::math::Vector3(0, 0, 0);
    m_RenderCollisionPrimInfo[i].type = 0;
    m_RenderCollisionPrimInfo[i].pShape = NULL;
    m_RenderCollisionPrimInfo[i].color = gfl2::math::Vector4(0, 0, 0);
  }
  m_pPrim = m_RenderCollisionPrimInfo;
  m_nPrim = 0;
}


/**
* @brief 環境音コリジョンデータを登録
* @return なし
* @detail 
*/
void DebugCollisionRenderManager::RegistRenderEnvSoundCollision( const void* cpData, gfl2::math::Vector4 color )
{
  if(( m_nEnvSound < MAX_RENDER_ENV_SOUND ) && ( cpData != NULL ))
  {
    m_pEnvSound->cpData = cpData;
    m_pEnvSound->color = color;
    m_pEnvSound ++;
    m_nEnvSound ++;
  }
}


/**
* @brief 環境音コリジョンデータを登録解除
* @return なし
* @detail 
*/
void DebugCollisionRenderManager::ClearRenderEnvSoundCollisionInfo()
{
  for(u32 i=0; i<MAX_RENDER_ENV_SOUND; i++)
  {
    m_RenderEnvSoundCollisionInfo[i].cpData = NULL;
    m_RenderEnvSoundCollisionInfo[i].color = gfl2::math::Vector4(0, 0, 0);
  }
  m_pEnvSound = m_RenderEnvSoundCollisionInfo;
  m_nEnvSound = 0;
}


GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif  //#if PM_DEBUG



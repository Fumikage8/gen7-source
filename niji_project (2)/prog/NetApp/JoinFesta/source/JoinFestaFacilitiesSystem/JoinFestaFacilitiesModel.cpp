//==============================================================================
/**
 * @file	JoinFestaFacilitiesModel.cpp
 * @brief	ジョインフェスタ施設モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModel.h"

// ライブラリ
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <base/include/gfl2_Singleton.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> 
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h> 
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h> 
#include <model/include/gfl2_BaseModel.h> 

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/GflUse.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JoinFestaFacilitiesModel::JoinFestaFacilitiesModel( gfl2::heap::HeapBase* pHeap ) :
m_pHeap( pHeap )
,m_pModelResourceRootNode( NULL )
,m_pModel( NULL )
,m_pAllocator( NULL )
,m_AnimPackList()
,m_textureAnimeId(ANIME_ID_NULL)
{
  m_pAllocator = GFL_NEW( m_pHeap ) System::nijiAllocator( m_pHeap );
  // アニメーションパックリスト
  m_AnimPackList.Initialize( m_pHeap, 1 );
}

// デストラクタ
JoinFestaFacilitiesModel::~JoinFestaFacilitiesModel()
{
  // アニメーション破棄
  m_AnimPackList.Finalize();
  GFL_DELETE m_pAllocator;
}


//=============================================================================
/**
 *  publicメソッド
 */
//=============================================================================
/*
 * @brief セットアップ
 * @param modelBuff    モデルリソースバッファ
 * @param animationBuff アニメーションバッファ
 */
void JoinFestaFacilitiesModel::Create( void* modelBuff, void* animationBuff,
                                       const u32 animeId,
                                       const u32 textureAnimeId )
{
  Create( modelBuff, animationBuff, gfl2::math::Vector3(0.0f,0.0f,0.0f), 0.0f, animeId, textureAnimeId );
}


/**
 * @brief セットアップ
 *
 * @param modelBuff    モデルリソースバッファ
 * @param animationBuff アニメーションバッファ
 * @param position     位置
 * @param rotationRadY 回転Ｙ（ラジアン）
 */
void JoinFestaFacilitiesModel::Create( void* modelBuff, void* animationBuff, 
                                       const gfl2::math::Vector3& position,
                                       const f32 rotationRadY,
                                       const u32 animeId,
                                       const u32 textureAnimeId )
{
  // 読み込み完了　セットアップ
  // モデルリソース
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    resourceData.SetModelData( static_cast<c8*>( modelBuff ) );
    m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  }

  // アニメーション生成
  {
    c8 * pAnime;
    pAnime = reinterpret_cast<c8*>(animationBuff);
    m_AnimPackList.LoadData( 0, m_pAllocator, m_pHeap, pAnime );
  }

  // モデル生成
  {
    m_pModel = GFL_NEW( m_pHeap ) poke_3d::model::BaseModel();
    m_pModel->Create( m_pAllocator, m_pHeap, m_pModelResourceRootNode);
    m_pModel->SetAnimationPackList( &m_AnimPackList );

    // アニメメモリ確保
    if( animeId != ANIME_ID_NULL )
    {
      m_pModel->AllocateAnimation( m_pHeap, false, animeId, 0 );
      m_pModel->ChangeAnimation( animeId, 0 );
      ICHI_PRINT(">ICHI FacilitiModelAnimeID:%d \n", animeId);
    }

    // テクスチャパターンアニメID保存
    m_textureAnimeId = textureAnimeId;
    if( m_textureAnimeId != ANIME_ID_NULL )
    {
      m_pModel->AllocateAnimation( m_pHeap, false, m_textureAnimeId, 1 );
      m_pModel->ChangeAnimation( m_textureAnimeId, 1 );
      m_pModel->SetAnimationFrame( 0, 1 );
      m_pModel->SetAnimationStepFrame( 0.0f, 1 );
    } 
  }

  // 位置と角度
  {
    /*gfl2::math::Matrix34 rotTrans( gfl2::math::Matrix34::GetTranslation( position ) );
    gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( 0.0f, rotationRadY, 0.0f ) );
    gfl2::math::Quaternion rotQt;
    rotQt.MatrixToQuaternion( rotTrans * rotMat );
    m_pModel->SetRotationQuat( rotQt );*/

    m_pModel->SetRotation( 0.0f, rotationRadY, 0.0f );
    m_pModel->SetPosition( position );
    ICHI_PRINT("JoinFestaFacilitiesModel::Create\n");
    ICHI_PRINT("  pos(%.f,%.f,%.f)\n", position.x, position.y, position.z );
    ICHI_PRINT("  rotY(%.f)\n", rotationRadY );
  }

  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->AddResourceChild( m_pModelResourceRootNode );
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->AddEdgeRenderingTarget( m_pModel->GetModelInstanceNode() );
}

/**
 * @brief アニメ更新
 */
void JoinFestaFacilitiesModel::UpdateAnime()
{
  if( m_pModel == NULL || m_pModel->IsCreated() == false )
  {
    return;
  }

  if( m_pModel->HasAnimationController() )
  {
    m_pModel->UpdateAnimation();
  }
}

/**
 * @brief アニメ変更
 */
void JoinFestaFacilitiesModel::ChangeAnime( u32 animeId )
{
  m_pModel->ChangeAnimation( animeId );
}


/**
 * @brief テクスチャ変更
 */
void JoinFestaFacilitiesModel::ChangeTexture( u32 textureId )
{
  /*gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelNode = m_pModel->GetModelInstanceNode();
  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pModelNode->GetMaterialInstanceNode( 0);
  GFL_PRINT("テクスチャ数 %d \n", pMaterialNode->GetTextureNumOriginal() );

  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureNode = pMaterialNode->GetTextureResourceNodeOriginal( textureId );
  pMaterialNode->SetTextureResourceNode( 0, pTextureNode );*/

  if( m_textureAnimeId != ANIME_ID_NULL )
  {
    m_pModel->SetAnimationFrame( textureId, 1 );
    ICHI_PRINT(">ICHI ChangeTexture:%d \n", textureId);
  }
}

/**
 * @brief 破棄
 * @return trueで破棄完了
 */
bool JoinFestaFacilitiesModel::Delete()
{
  m_pModel->SetVisible( false );
  // 描画中
  if( m_pModel->CanDestroy() == false )
  {
    return false;
  }
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetUpperRenderingPipeLine()->RemoveEdgeRenderingTarget( m_pModel->GetModelInstanceNode() );
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetFieldRootNode()->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModel );
  GFL_SAFE_DELETE( m_pModelResourceRootNode  );

  return true;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)


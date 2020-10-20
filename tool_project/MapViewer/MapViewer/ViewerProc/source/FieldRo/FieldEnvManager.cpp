//===================================================================
/**
 * @file    FieldEnvManager.cpp
 * @brief   フィールドEnv管理クラス
 * @author  saita_kazuki
 * @date    2015.05.12
 */
//===================================================================

#include "ViewerProc/include/FieldRo/FieldEnvManager.h"
#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h"

#include "System/include/nijiAllocator.h"
///#include "System/include/Skybox/Skybox.h"

#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

#define ENABLE_ENV 1

GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief コンストラクタ
 */
EnvManager::EnvManager()
{
}

/**
 * @brief デストラクタ
 */
EnvManager::~EnvManager()
{
}

/**
 * @brief 初期化
 * @param 天球インスタンスのポインタ
 */
void EnvManager::Initialize( gfl2::heap::HeapBase* pHeap, System::nijiAllocator* pGLAllocator, MyRenderingPipeLine* pRenderingPipeLine, void* pEnvPackData/*, System::Skybox::Skybox* pSkybox*/)//@todo_saito
{
  m_pHeap = pHeap;
  m_pGLAllocator = pGLAllocator;

  ///m_pSkybox = pSkybox;

#if ENABLE_ENV
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( pEnvPackData);

  // EnvNode作成
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_ENV));

    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData resourceData;
    resourceData.SetBinaryEnvData( pBinaryData);

    m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pGLAllocator, m_pDrawEnvResourceRootNode);
  }

  // Envモーションノード作成
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_LIGHT));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData);

    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);
  }

  // Envモーションノード作成(FOG)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_FOG) != 0 )
  {
    // バイナリデータを取得
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_FOG) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // リソースノードをバイナリデータから作成
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // Fogアニメーション開始
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_FOG  ], 0, System::EnvAccessor::ANIMATION_BIT_FOG  );
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_FOG );
  }

  // Envモーションノード作成(ブルーム)
  if( binLinkerAccessor.GetDataSize(PACK_INDEX_MOTION_BLOOM) != 0 )
  {
    // バイナリデータを取得
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX_MOTION_BLOOM) );
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData );

    // リソースノードををバイナリデータから作成
    m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pGLAllocator, &resourceData);

    // ブルームアニメーション開始
    m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_BLOOM], 0, System::EnvAccessor::ANIMATION_BIT_BLOOM);
    m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_BLOOM );
  }

  m_EnvAccessor.Initialize( m_pHeap, m_pDrawEnvNode);
  m_EnvAccessor.ChangeAnimation( m_pDrawEnvMotionResourceRootNode[ANIMATION_INDEX_LIGHT], 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.SetAnimationLoop( true, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);

  // ブルーム描画パスの初期化
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    bloomRenderPath->SetDrawEnvNode( m_pDrawEnvNode );

    if( binLinkerAccessor.GetDataSize(PACK_INDEX_TEXTURE_BLOOM) != 0 )
    {
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
        texData.SetTextureData(static_cast<c8*>(binLinkerAccessor.GetData( PACK_INDEX_TEXTURE_BLOOM)));
        m_pDrawEnvResourceAlphaMask = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&texData);
      }
      // ブルーム描画パスに渡す
      {
        gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(m_pDrawEnvResourceAlphaMask);
        bloomRenderPath->SetAlphaMaskTexture(texResNode->GetTexture());
      }
    }
  }

#endif
}

/**
 * @brief 終了
 */
void EnvManager::Terminate()
{
  m_EnvAccessor.Terminate();

  for( u32 i = 0; i < ANIMATION_INDEX_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_pDrawEnvMotionResourceRootNode[i]);
  }
  GFL_SAFE_DELETE( m_pDrawEnvResourceRootNode);
  GFL_SAFE_DELETE( m_pDrawEnvNode );
  GFL_SAFE_DELETE( m_pDrawEnvResourceAlphaMask );
}

/**
 * @brief 更新
 * @param マップ管理座標
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 */
void EnvManager::Update( const gfl2::math::Vector3& controlPos, s32 hour, s32 minute, s32 second, gfl2::math::Matrix44& projMatrix, MyRenderingPipeLine* pRenderingPipeLine)
{
  // 天球更新
  ///m_pSkybox->Update( controlPos, hour, minute, second);

  // フォグ
  {
    // 他は変更していないので
    //   ・Zバッファ
    //   ・Zバッファなのでカメラのニアクリップ距離、ファークリップ距離は使わない
    //   ・深度バッファの範囲はデフォルトから変更しない
    // となっている。

    //Wバッファのテスト
    //param.cameraNear     = 1.0f;
    //param.cameraFar      = 1000.0f;
    //param.depthRangeNear = 0.0f;
    //param.depthRangeFar  = 1.0f;
    //param.wScale         = 1.0f/param.cameraFar;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = projMatrix;
    m_pDrawEnvNode->UpdateFog(param);
  }

  // ブルーム描画パス
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = pRenderingPipeLine->GetBloomRenderPath();
    if( bloomRenderPath )
    {
      bloomRenderPath->Update();
    }
  }

#if ENABLE_ENV
  // @todo 更新間隔は未制御
  m_EnvAccessor.SetFrame( hour, minute, second, 0, System::EnvAccessor::ANIMATION_BIT_LIGHT);
  m_EnvAccessor.Update();
#endif

}

/**
 * @brief DrawEnvNode取得
 */
gfl2::renderingengine::scenegraph::instance::DrawEnvNode* EnvManager::GetDrawEnvNode() const
{
  return m_pDrawEnvNode;
}

/**
 * @brief フォグデータの適応
 * @param pModelInstanceNode フォグを適応したいモデルインスタンス
 */
void EnvManager::ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode)
{
  u32 materialCount = pModelInstanceNode->GetMaterialNum();
  for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode*                 materialInstanceNode               = pModelInstanceNode->GetMaterialInstanceNode(materialIndex);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*                 materialResourceNode               = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*>(materialInstanceNode->GetMaterialResourceNode());
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* materialInstanceNodeAttributeParam = materialInstanceNode->GetAttributeParam();
    materialInstanceNodeAttributeParam->m_FogEnable = true;
  }

  m_pDrawEnvNode->UpdateMaterialFog(pModelInstanceNode);
}

GFL_NAMESPACE_END( Field )


/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBase.cpp
 *  @brief  フィールドエフェクト：アクションボタン
 *  @author miyachi_soichi
 *  @date   2015.06.05
 */

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include "GameSys/include/GameManager.h"

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"

#include "Field/FieldRo/include/FieldLayoutPriority.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectActionButton.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

// 
static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0x6000;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0x6000;

/**
 *  @brief  コンストラクタ
 */
EffectActionButton::EffectActionButton( void )
  :IEffectBase()
  ,m_pCameraManager( NULL )
#if FIELDEFFECT_SWITCH_LAYOUT
  , m_appUtilHeap()
  , m_pLayout( NULL )
  , m_pWindowManager( NULL )
#else
  ,m_Model()
  ,m_pAllocator( NULL )
  ,m_pModelResourceRootNode( NULL )
  ,m_pModelInstanceNode( NULL )
  ,m_pFieldRootNode( NULL )
  ,m_pRenderingPipeLine( NULL )
  ,m_fScale( 0.0f )
#endif // FIELDEFFECT_SWITCH_LAYOUT
  ,m_vPosition()
{
}

/**
 *  @brief  デストラクタ
 */
EffectActionButton::~EffectActionButton( void )
{
}

/**
 *  @brief  初期化
 */
void EffectActionButton::Initialize( SetupData &setupData )
{
  m_pCameraManager = setupData.pCameraManager;
#if FIELDEFFECT_SWITCH_LAYOUT
  m_pWindowManager = setupData.pWinMgr;
  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );
  // Layout生成
  app::util::AppRenderingManager *pRenderManager = m_pWindowManager->GetRenderingManager();
  m_pLayout = GFL_NEW( setupData.pHeap ) Field::Effect::EffectLayout( &m_appUtilHeap, pRenderManager );
  // Layout初期化
  {
    app::util::G2DUtil::LytResData resTbl[] = 
    {
      { setupData.pResource, 2, app::util::G2DUtil::ATTACH_ROOT },
    };
    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        0,
        0,
        setupData.pResource,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };
    Field::Effect::EffectLayout::SetupParam param;
    param.layoutNum = 1;
    param.lytResTbl = resTbl;
    param.resTblNum = GFL_NELEMS( resTbl );
    param.lytDataTbl = dataTbl;
    param.dataTblNum = GFL_NELEMS( dataTbl );
    m_pLayout->Initialize( param );
  }
  // 登録
  m_pWindowManager->GetUIView()->AddSubView( m_pLayout );
  SetPosition( setupData.vPos );
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_3D );
  m_pLayout->SetVisible( true );
#else
  m_pFieldRootNode = setupData.pRootNode;
  m_pRenderingPipeLine = setupData.pPipeLine;

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( 0 );
  createModel( pModelData );
  m_Model.Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode );
  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( 1 );
  createMotion( pMotionData );
  m_Model.ChangeAnimationByResourceNode( m_pMotionResourceRootNode );
  // デフォルトを補助アイコンにしておく
  SetAnimationFrame( 1 );
  // 正射影
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode );
  m_pRenderingPipeLine->GetOrthoRenderPath()->AddRenderingTarget( m_pModelInstanceNode );
  // 座標
  SetPosition( setupData.vPos );
#if defined(GF_PLATFORM_WIN32)
  m_fScale  = 1.2f;
#elif defined(GF_PLATFORM_CTR)
  m_fScale  = 1.62f;
#endif
  m_Model.SetScale( m_fScale, m_fScale, m_fScale );
#endif // FIELDEFFECT_SWITCH_LAYOUT
}

/**
 *  @brief  破棄
 */
bool EffectActionButton::Terminate( void )
{
#if FIELDEFFECT_SWITCH_LAYOUT
  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
#else
  m_pFieldRootNode->RemoveAfterTransChild( m_pModelInstanceNode );
  m_Model.Destroy();

  m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode );
  GFL_SAFE_DELETE( m_pModelResourceRootNode );
  m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode );
  GFL_SAFE_DELETE( m_pMotionResourceRootNode );

  GFL_SAFE_DELETE( m_pAllocator );
#endif // FIELDEFFECT_SWITCH_LAYOUT
  return true;
}

/**
 *  @brief  更新
 */
void EffectActionButton::updateNormal( void )
{
  updatePosition();
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectActionButton::Delete( void )
{
#if FIELDEFFECT_SWITCH_LAYOUT
  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
#else
  m_Model.SetVisible( false );
  m_pRenderingPipeLine->GetOrthoRenderPath()->RemoveRenderingTarget( m_pModelInstanceNode );
#endif // FIELDEFFECT_SWITCH_LAYOUT
}

/**
 *  @brief  座標設定
 */
void EffectActionButton::SetPosition( const gfl2::math::Vector3 &vPos )
{
  m_vPosition = vPos;
  updatePosition();
}

/**
 *  @brief  アニメーションフレーム設定：アイコン切り替え
 */
void EffectActionButton::SetAnimationFrame( u32 frame )
{
#if FIELDEFFECT_SWITCH_LAYOUT
#else
  // Enum -> フレーム数に変換する
  f32 fFrame = static_cast<f32>( frame );

  m_Model.SetAnimationFrame( fFrame );
  m_Model.SetAnimationStepFrame( 0.0f );
  m_Model.UpdateAnimation();
#endif
}

#if FIELDEFFECT_SWITCH_LAYOUT
#else
/**
 *  @brief  モデルの作成
 */
void EffectActionButton::createModel( void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode );

  // モデルインスタンス作成
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode );
}

/**
 *  @brief  モーションの作成
 */
void EffectActionButton::createMotion( void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode );
}
#endif // FIELDEFFECT_SWITCH_LAYOUT

/**
 *  @brief  座標更新
 */
void EffectActionButton::updatePosition( void )
{
  gfl2::math::Matrix44 mtx_proj;
  gfl2::math::Matrix34 mtx_view;
  m_pCameraManager->GetMainViewCamera()->GetProjectionMatrix( &mtx_proj );
  m_pCameraManager->GetMainViewCamera()->GetViewMatrix( &mtx_view );
  gfl2::math::Vector4 view_pos = mtx_view * m_vPosition;
  {
    view_pos.x = static_cast<f32>(static_cast<s32>(view_pos.x));
    view_pos.y = static_cast<f32>(static_cast<s32>(view_pos.y));
    view_pos.z = static_cast<f32>(static_cast<s32>(view_pos.z));
  }
  gfl2::math::Vector4 proj_pos = mtx_proj * view_pos;
  {
    proj_pos.x = static_cast<f32>(static_cast<s32>(proj_pos.x));
    proj_pos.y = static_cast<f32>(static_cast<s32>(proj_pos.y));
    proj_pos.z = static_cast<f32>(static_cast<s32>(proj_pos.z));
  }
  gfl2::math::Vector4 dst_pos = proj_pos;

  gfl2::math::Vector3 screen_pos( 0.0f, 0.0f, 0.0f);
  dst_pos.x = dst_pos.x / dst_pos.w;
  dst_pos.y = dst_pos.y / dst_pos.w;
  screen_pos.x = static_cast<f32>( static_cast<s32>( dst_pos.x * 200.0f ) );
  screen_pos.y = static_cast<f32>( static_cast<s32>( dst_pos.y * 120.0f ) );

#if FIELDEFFECT_SWITCH_LAYOUT
  // データの中身がないと座標設定できない
  u32 idx = 1;
  m_pLayout->SetPosition( idx, &screen_pos );
#else
  m_Model.SetPosition( screen_pos );
#endif // FIELDEFFECT_SWITCH_LAYOUT
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

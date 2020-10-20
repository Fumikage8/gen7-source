//======================================================================
/**
 * @file FinderStudioRenderSystem.cpp
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオ描画システム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioRenderSystem.h"

// gflib2
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// FinderStudioViewer
#include "../include/FinderStudioViewer.h"
#include "FinderStudioRenderPipeLine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

//コンストラクタ
FinderStudioRenderSystem::FinderStudioRenderSystem(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap,System::nijiAllocator* pNijiAllocator):
  m_pRenderPipeLine(NULL)
  ,m_pDrawEnvNode(NULL)
  ,m_pDirectionalLightNode(NULL)
  ,m_pViewMtx(NULL)
  ,m_pProjMtx(NULL)
  ,m_InitSeq(0)
  ,m_pCaptureTexture(NULL)
{
  GFL_ASSERT_STOP_MSG(pHeap,"ヒープを指定してください\n");
  GFL_ASSERT_STOP_MSG(pWorkHeap,"一時ヒープを指定してください\n");

  m_pHeap = pHeap;
  m_pWorkHeap = pWorkHeap;
  m_pNijiAllocator = pNijiAllocator;
}
//デストラクタ
FinderStudioRenderSystem::~FinderStudioRenderSystem()
{
}

// 初期化処理
bool FinderStudioRenderSystem::InitFunc()
{
  enum{
    SEQ_INIT_CREATE_RENDERPIPELINE,
    SEQ_INIT_CREATE_RENDERPIPELINE_WAIT,
    SEQ_INIT_SETUP_LIGHT
  };

  switch(m_InitSeq)
  {
  case SEQ_INIT_CREATE_RENDERPIPELINE:
    // 描画パイプラインの作成
    m_pRenderPipeLine = GFL_NEW(m_pHeap) FinderStudioRenderPipeLine(m_pHeap,m_pWorkHeap,m_pNijiAllocator);
    // エッジ・アウトラインリソースのロード
    m_pRenderPipeLine->LoadEdgeMapOutLineAsync();

    m_InitSeq++;
    // fall through

  case SEQ_INIT_CREATE_RENDERPIPELINE_WAIT:
    if(m_pRenderPipeLine->IsLoadEdgeMapOutLine())
    {
      // エッジ・アウトラインリソースのセットアップ
      m_pRenderPipeLine->SetupEdgeMapOutLine();
      // クリア用バッファの初期化
      m_pRenderPipeLine->InitializeBufferClearAlphaPath();

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_SETUP_LIGHT:
    // 上画面前提
    u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    float nearVal = 10.0f;
    float farVal = 2000.0f;

    if(displayHeight == 0)
    {
      GFL_ASSERT(0);
      displayHeight = 400;
    }

    // 射影変換行列(パースペクティブ(透視法)射影)
    // カメラシステムがないときのために用意
    m_ProvProjMtx = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
      static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
      nearVal,							// 前方投影面までの距離
      farVal,true);							// 後方投影面までの距離
    

    // ビュー行列
    // カメラシステムがないときのために用意
    gfl2::math::Vector3 eyePosition(0,250,-500);
    gfl2::math::Vector3 focusPosition(0,0,0);
    gfl2::math::Vector3 upDirection(0,1,0);
    m_ProvViewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    // ライト設定
    m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_pNijiAllocator);
	  // ライティング用に環境ノードを追加
	  {
      // ディレクショナルライト
      m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
      {
			  m_pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
			  gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
			  m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
			  m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );			

        gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
			  m_pDirectionalLightNode->SetPosition( position );
        ///gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.390278f, -0.833886, -0.390278);
        gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.752993f, -0.614799f, -0.234570f);
			  m_pDirectionalLightNode->SetDirection( direction );
        m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
      }

      // アンビエントライト
      gfl2::renderingengine::scenegraph::instance::LightNode*	pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
      {
			  pAmbientLightNode->SetNodeName( "AmbientLight02" );
			  gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()) );
			  pAmbientLightNode->SetNodeNameHash( nameHash.value );
			  pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
			  pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
		  }

      gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pNijiAllocator);
		  pLightSetNode->SetSetNo(2);
		  pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
		  pLightSetNode->AddLight( pAmbientLightNode );//LightSetNodeの子となる
		  m_pDrawEnvNode->AddLightSet( pLightSetNode );//DrawEnvNodeの子となる
	  }

	  gfl2::math::AABB envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
    m_pDrawEnvNode->SetAABB( envSpace );
	  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(m_pDrawEnvNode);

	  m_pRenderPipeLine->SetDrawEnv(m_pDrawEnvNode);

    return true;
  }

  return false;
}

// 終了処理
bool FinderStudioRenderSystem::Terminate()
{
  GFL_SAFE_DELETE(m_pRenderPipeLine);
  // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
  GFL_SAFE_DELETE(m_pDrawEnvNode);

  return true;
}

// 更新
void FinderStudioRenderSystem::Update()
{
  // これが！！ないと！！モデルが！！動かない！！
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

// 描画
void FinderStudioRenderSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  // 各種バッファを取得
  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
	gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
  // 画面サイズを取得
	u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
	u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

  switch( displayNo )
	{
	// 左画面描画
	case gfl2::gfx::CtrDisplayNo::LEFT:
    m_pRenderPipeLine->SetRenderTarget( pBackBuffer );
    m_pRenderPipeLine->SetDepthStencil( pDepthBuffer );
    m_pRenderPipeLine->SetDisplaySize( displayWidth, displayHeight );

    GFL_ASSERT(m_pViewMtx);
    GFL_ASSERT(m_pProjMtx);

    // 各行列が登録されていなければ予備の物を使用する
    if(m_pProjMtx) m_pRenderPipeLine->SetProjectionMatrix(*m_pProjMtx);
    else           m_pRenderPipeLine->SetProjectionMatrix(m_ProvProjMtx);
    if(m_pViewMtx) m_pRenderPipeLine->SetViewMatrix(*m_pViewMtx);
    else           m_pRenderPipeLine->SetViewMatrix(m_ProvViewMtx);

    // 各パスのUpdateを呼び出す
    m_pRenderPipeLine->Update();

    // 各パスのExecuteを呼び出す
    m_pRenderPipeLine->Execute();
    break;

  // 右画面描画
  case gfl2::gfx::CtrDisplayNo::RIGHT:

    break;

  // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN: 

    break;
  }

  this->transferColorBuffer(displayNo);
}

/// @brief カラーバッファ取得 リクエスト
/// @param pOutBuffer 格納先バッファ
void FinderStudioRenderSystem::RequestTransferColorBuffer(CaptureTexture* pOutTexture)
{
  m_pCaptureTexture = pOutTexture;
}

// カラーバッファをテクスチャへ転送する
void FinderStudioRenderSystem::transferColorBuffer(gfl2::gfx::CtrDisplayNo no)
{
  if(m_pCaptureTexture && no == gfl2::gfx::CtrDisplayNo::LEFT)
  {
    bool isFinish = false;
    if(m_pCaptureTexture->pTextureBuffer)
    {
      gfl2::gfx::GFGL::AddReadColorBufferToTextureCommand(m_pCaptureTexture->pTextureBuffer);
      isFinish = true;
    }
    if(m_pCaptureTexture->pColorBuffer)
    {
      gfl2::gfx::GFGL::AddTransferColorBufferCommand(m_pCaptureTexture->pColorBuffer);
      isFinish = true;
    }

    if(isFinish)
    {
      m_pCaptureTexture->isFinishCapture = true;
      m_pCaptureTexture = NULL;
    }
  }
}


GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)
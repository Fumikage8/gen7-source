//======================================================================
/**
 * @file	SequenceRenderSystem.C++
 * @brief	SequenceRenderSystem中心機能
 * @author	PETE
 * @data	2015.05.08
 */
//======================================================================
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <gfx/include/gfl2_GLMemory.h>

#include <AppLib/include/Util/AppRenderingManager.h>

#include "System/include/GflUse.h"

#include <gfx/include/gfl2_GLMemory.h>

#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

#include "DemoLib/SequencePlayer/include/SequenceRenderSystem.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

SequenceRenderSystem :: SequenceRenderSystem(SequenceViewSystem *seqViewSys)
{
	mSeqCore = seqViewSys;
	mIsFinishSetup = false;
  mUseOption = SequenceRenderingPipeLine::UseOption();
}

SequenceRenderSystem :: ~SequenceRenderSystem(void)
{
}

void SequenceRenderSystem::Initialize(void)
{
	gfl2::heap::HeapBase *memHeap = mSeqCore->GetMemHeap();
	gfl2::heap::HeapBase *devHeap = mSeqCore->GetDevHeap();

	//mOldAllocator = gfl2::gfx::GLMemory::GetAllocator();
	mAllocator = GFL_NEW( memHeap ) System::nijiAllocator(devHeap);	
	
	mRenderingPipeLine = GFL_NEW( memHeap ) SequenceRenderingPipeLine( mSeqCore , 512, 256 );
  mRenderingPipeLine->SetUseOption( mUseOption);
	mRenderingPipeLine->Initialize();

  app::util::AppRenderingManager::Description appRenderingMgrDesc;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
	mAppRenderingManager = GFL_NEW( memHeap )::app::util::AppRenderingManager();
	mAppRenderingManager->SyncCreate(appRenderingMgrDesc, mSeqCore->GetAppHeap(), NULL, mSeqCore->GetAsyncFileManager());
  GFL_ASSERT( mAppRenderingManager->IsCreated() );  // 同期生成なので mAppRenderingManager->IsCreated() では待たない。

	mSetupSeq = 0;

}

void SequenceRenderSystem::Terminate(void)
{

	mRenderingPipeLine->Terminate();
  GFL_SAFE_DELETE(m_pDrawEnvNode);
  GFL_SAFE_DELETE(m_pDrawEnvNode2);
  GFL_SAFE_DELETE(m_pDrawEnvNodeField);
	GflHeapFreeMemory(mEdgeMapRenderPathRes);
	GflHeapFreeMemory(mOutLineRenderPathRes);
  GFL_ASSERT( mAppRenderingManager->CanDestroy() );  // 同期生成なので m_pAppRenderingMgr->CanDestroy() では待たない。
	GFL_SAFE_DELETE( mAppRenderingManager );
	GFL_SAFE_DELETE( mRenderingPipeLine );

	GFL_SAFE_DELETE( mDepthStencilTexture );
	
	GFL_SAFE_DELETE( mAllocator );
}

//初期化
bool SequenceRenderSystem::Setup(void)
{
  
	gfl2::heap::HeapBase *devHeap = mSeqCore->GetDevHeap();
	gfl2::fs::AsyncFileManager *fileMng = mSeqCore->GetAsyncFileManager();
	enum
	{
		LOAD_RENDER_RES,
		WAIT_RENDER_RES,
	};

	switch( mSetupSeq )
	{
	case LOAD_RENDER_RES:
		{
			gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
			loadReq.arcId = ARCID_RENDERPATH;
			loadReq.align = 128;
			loadReq.heapForBuf = devHeap;
			loadReq.heapForReq = devHeap->GetLowerHandle();

			loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
			loadReq.ppBuf = &mEdgeMapRenderPathRes;
			fileMng->AddArcFileLoadDataReq(loadReq);

			loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
			loadReq.ppBuf = &mOutLineRenderPathRes;
			fileMng->AddArcFileLoadDataReq(loadReq);
		}
		mSetupSeq++;
		//break;  スルー
	case WAIT_RENDER_RES:
		if( fileMng->IsArcFileLoadDataFinished(&mEdgeMapRenderPathRes) == false ||
			fileMng->IsArcFileLoadDataFinished(&mOutLineRenderPathRes) == false )
		{
			return false;
		}
		mRenderingPipeLine->Setup(mEdgeMapRenderPathRes,mOutLineRenderPathRes);
		SetupEnv();
		    
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
		req.arcId				= ARCID_RENDERPATH;
		fileMng->SyncArcFileClose(req);

    mIsFinishSetup = true;
		return true;
	}
	return true;
}

//終了
bool SequenceRenderSystem::Cleanup(void)
{

	mIsFinishSetup = false;
	return true;
}

//更新
void SequenceRenderSystem::Update(void)
{
  if( mIsFinishSetup == false )
  {
    return;
  }

#if defined(GF_PLATFORM_CTR)
  f32 depthLevel = 4.0f;
  f32 factor	   = 1.0f;
  gfl2::gfx::GFGL::GetStereoMatrix(	mCamProjMtx,
    mCamViewMtx,
    depthLevel,
    factor, 
    &mProjMtx[0],
    &mViewMtx[0],
    &mProjMtx[1],
    &mViewMtx[1]);

#else
  for (u32 i = 0; i < 2; ++i)
  {
    mProjMtx[i] = mCamProjMtx;
    mViewMtx[i] = mCamViewMtx;
  }
#endif

  mRenderingPipeLine->UpdateFog();

  mAppRenderingManager->Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(&mViewMtx[0]);

  // レンダリングパイプライン更新
//  mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
//  mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
//  mRenderingPipeLine->Update();

}

//描画
void SequenceRenderSystem::PreDraw()
{
  if( mIsFinishSetup == false )
  {
    return;
  }

  mAppRenderingManager->PreDraw();

}

void SequenceRenderSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{		
	if( mIsFinishSetup == false )
	{
		return;
	}
	
	switch( displayNo )
	{
		// 左画面描画
	case gfl2::gfx::CtrDisplayNo::LEFT:
		{
			gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
			gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
			u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
			u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

			// 射影変換行列(パースペクティブ(透視法)射影)
			gfl2::math::Matrix44 projection;
			gfl2::math::Matrix34 view;

      mRenderingPipeLine->SetBaseCamera(mProjMtx[0],mViewMtx[0]);

			mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
			mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
     
			mRenderingPipeLine->SetRenderTarget( pBackBuffer );
			mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
			mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

			// Camera Update Fix:
			mRenderingPipeLine->Update();
			mRenderingPipeLine->Execute();
			break;
		} // case gfl2::gfx::CtrDisplayNo::LEFT

		// 右画面描画(立体視時のみ)
	case gfl2::gfx::CtrDisplayNo::RIGHT:
		break;
		// 下画面描画
	case gfl2::gfx::CtrDisplayNo::DOWN:
		break;
	} // switch( displayNo )
	mAppRenderingManager->Draw(displayNo);

}

void SequenceRenderSystem::SetupEnv(void)
{
	
	{
		u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
		u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
		// 射影変換行列(パースペクティブ(透視法)射影)
		mCamProjMtx = gfl2::math::Matrix44::GetPerspective(
			gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
			(f32)displayWidth / (f32)displayHeight,
			1.0f,							// 前方投影面までの距離
			12000.0f);							// 後方投影面までの距離
	}
	{
		// ビュー変換行列
		gfl2::math::Vector3 eyePosition( 256.0f, 106.0f, 465.0f );  // 視点(カメラの位置)
		gfl2::math::Vector3 focusPosition( -45.0f, 70.0f, -20.0f );  // 注視点
		gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
		mCamViewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
	}
	
  m_pDrawEnvNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mAllocator);
  m_pDrawEnvNode2			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mAllocator);
  m_pDrawEnvNodeField			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mAllocator);
	// ライティング用に環境ノードを追加
	{
/*		gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
		gfl2::renderingengine::scenegraph::instance::LightNode			*m_pDirectionalLightNode;
		gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;

		m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode();
		m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();
		m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();

		{
			m_pDirectionalLightNode->SetNodeName( "DirectionalLight01" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
			m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
			m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
			gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
			m_pDirectionalLightNode->SetPosition( position );
			gfl2::math::Vector3 direction = -position.Normalize();
			m_pDirectionalLightNode->SetDirection( direction );
			m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
		}

		{
			m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
			m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
			m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
			m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
		}

		m_pLightSetNode->SetSetNo( 0 );
		m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
		m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
		m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
  */
	}
	// ライト2
	{
	/*	gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
		gfl2::renderingengine::scenegraph::instance::LightNode			*m_pDirectionalLightNode;
		gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;

		m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode();
		m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();
		m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode();

		{
			m_pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
			m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
			m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
			gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
			m_pDirectionalLightNode->SetPosition( position );
			gfl2::math::Vector3 direction = -position.Normalize();
			m_pDirectionalLightNode->SetDirection( direction );
      m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
      

		{
			m_pAmbientLightNode->SetNodeName( "AmbientLight02" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
			m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
			m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
			m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 0.1f, 0.1f, 0.1f ) );
		}

		m_pLightSetNode->SetSetNo( 2 );
		m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
		m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
		m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
    */
	}

  {
	  gfl2::math::AABB				envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
	  m_pDrawEnvNode->SetAABB( envSpace );
	  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );
    mRenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
  }

  {
    gfl2::math::AABB				envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
    m_pDrawEnvNode2->SetAABB( envSpace );
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode2 );
    mRenderingPipeLine->AddDrawEnv2( m_pDrawEnvNode2 );
  }

  {
    gfl2::math::AABB				envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
    m_pDrawEnvNodeField->SetAABB( envSpace );
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNodeField );
    mRenderingPipeLine->AddDrawEnvField( m_pDrawEnvNodeField );
  }

	

  //スクリーン貼り付けカメラ
  {
    gfl2::math::Matrix44 projMtx;   // 透視変換行列
    gfl2::math::Matrix34 viewMtx;   // ビュー変換行列
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );

    // 射影変換行列(パースペクティブ(透視法)射影)
    projMtx = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
      (f32)displayWidth / (f32)displayHeight,
      1.0f,							// 前方投影面までの距離
      1024.0f);							// 後方投影面までの距離

    // ビュー変換行列
    gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, 0.0f );  // 視点(カメラの位置)
    gfl2::math::Vector3 focusPosition( 0.0f, 0.0f, 200.0f );  // 注視点
    gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
    viewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    mRenderingPipeLine->SetScreenCameraParam(projMtx,viewMtx);
  }
	
}



	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)


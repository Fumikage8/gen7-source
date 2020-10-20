//======================================================================
/**
 * @file  btlv_RenderSystem.cpp
 * @brief バトル描画 GLAllocatar
 * @author  ariizumi
 * @data  15/03/07
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

#include "btlv_RenderSystem.h"

#include "../btlv_core.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

GFL_NAMESPACE_BEGIN(btl)

BtlvRenderSystem::BtlvRenderSystem(BattleViewSystem *btlvCore)
{
  mBtlvCore = btlvCore;
  mIsFinishSetup = false;
}

BtlvRenderSystem::~BtlvRenderSystem()
{
}

void BtlvRenderSystem::Initialize(void)
{
  gfl2::heap::HeapBase *memHeap = mBtlvCore->GetResidentHeap();
  gfl2::heap::HeapBase *devHeap = mBtlvCore->GetResidentHeap();

  mRenderingPipeLine = GFL_NEW( memHeap ) BattleRenderingPipeLine( mBtlvCore , 512, 256 );
  mRenderingPipeLine->Initialize();

  app::util::AppRenderingManager::Description appRenderingMgrDesc;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  mAppRenderingManager = GFL_NEW( memHeap )::app::util::AppRenderingManager();
  mAppRenderingManager->SyncCreate( appRenderingMgrDesc, mBtlvCore->GetAppHeap(), NULL, mBtlvCore->GetAsyncFileManager() );
  GFL_ASSERT( mAppRenderingManager->IsCreated() );  // 同期生成なので mAppRenderingManager->IsCreated() では待たない。

  mSetupSeq = 0;
  mUpdateCamMatrix = true;
}

void BtlvRenderSystem::Terminate(void)
{
  mRenderingPipeLine->Terminate();
  GflHeapFreeMemory(mEdgeMapRenderPathRes);
  GflHeapFreeMemory(mOutLineRenderPathRes);
  GflHeapFreeMemory(mFullPowerLineRenderPathRes);
  GFL_ASSERT( mAppRenderingManager->CanDestroy() );  // 同期生成なので mAppRenderingManager->CanDestroy() では待たない。
  GFL_SAFE_DELETE( mAppRenderingManager );
  GFL_SAFE_DELETE( mRenderingPipeLine );

  GFL_SAFE_DELETE( mDepthStencilTexture );
}

//初期化
bool BtlvRenderSystem::Setup(void)
{
  gfl2::heap::HeapBase *devHeap = mBtlvCore->GetResidentHeap();
  gfl2::fs::AsyncFileManager *fileMng = mBtlvCore->GetAsyncFileManager();
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

      loadReq.datId = GARC_NijiRenderPath_FullPower_BIN;
      loadReq.ppBuf = &mFullPowerLineRenderPathRes;
      fileMng->AddArcFileLoadDataReq(loadReq);
    }
    mSetupSeq++;
    //break;  スルー
  case WAIT_RENDER_RES:
    if( fileMng->IsArcFileLoadDataFinished(&mEdgeMapRenderPathRes) == false ||
        fileMng->IsArcFileLoadDataFinished(&mOutLineRenderPathRes) == false ||
        fileMng->IsArcFileLoadDataFinished(&mFullPowerLineRenderPathRes) == false )
    {
      return false;
    }
    mRenderingPipeLine->Setup(mEdgeMapRenderPathRes,mOutLineRenderPathRes,mFullPowerLineRenderPathRes);
    SetupDebugEnv();
    SetupDebugData();
    mIsFinishSetup = true;
    return true;
  }
  return true;
}

//終了
bool BtlvRenderSystem::Cleanup(void)
{
  GFL_SAFE_DELETE(m_pDrawEnvNode);

  mIsFinishSetup = false;
  return true;
}

//更新
void BtlvRenderSystem::CalcMatrix(void)  
{
  //天球のUpdateのために分離

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
  gfl2::gfx::GFGL::GetStereoMatrix(	mCamFarProjMtx,
    mCamViewMtx,
    depthLevel,
    factor, 
    &mFarProjMtx[0],
    &mFarViewMtx[0],
    &mFarProjMtx[1],
    &mFarViewMtx[1]);
  gfl2::gfx::GFGL::GetStereoMatrix(	mCamSkyProjMtx,
    mCamViewMtx,
    depthLevel,
    factor, 
    &mSkyProjMtx[0],
    &mSkyViewMtx[0],
    &mSkyProjMtx[1],
    &mSkyViewMtx[1]);
#else
  for (u32 i = 0; i < 2; ++i)
  {
    mProjMtx[i] = mCamProjMtx;
    mViewMtx[i] = mCamViewMtx;

    mFarProjMtx[i] = mCamFarProjMtx;
    mFarViewMtx[i] = mCamViewMtx;

    mSkyProjMtx[i] = mCamSkyProjMtx;
    mSkyViewMtx[i] = mCamViewMtx;
  }
#endif
  mBtlvCore->GetBgSystem()->SetFarMtx(mFarProjMtx[0],mFarViewMtx[0]);
  mBtlvCore->GetBgSystem()->SetSkyMtx(mSkyProjMtx[0],mSkyViewMtx[0]);
  mRenderingPipeLine->GetShadowRenderPath()->AddSheneCamera( mBtlvCore->GetBgSystem()->GetFarRenderCamera() );
}

void BtlvRenderSystem::Update(void)
{
  if( mIsFinishSetup == false )
  {
    return;
  }
  mAppRenderingManager->Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(&mViewMtx[0]);

    // レンダリングパイプライン更新
  mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
  mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
  mRenderingPipeLine->Update(mViewMtx[0]);


}

//描画
void BtlvRenderSystem::PreDraw()
{
  if( mIsFinishSetup == false )
  {
    return;
  }

  mAppRenderingManager->PreDraw();
}

void BtlvRenderSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if( mIsFinishSetup == false )
  {
    return;
  }

  mRenderingPipeLine->SetIsDraw(true);
  mRenderingPipeLine->SetIsAfterDraw(false);

  // @attention 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya
  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      mRenderingPipeLine->SetRenderTarget( pBackBuffer );
      mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

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

  mRenderingPipeLine->SetIsAfterDraw(true);
    // レンダリングパイプライン更新
  mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
  mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
    // @attention 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya
  switch( displayNo )
  {
    // 左画面描画
  case gfl2::gfx::CtrDisplayNo::LEFT:
    {
      gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
      gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
      u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
      u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

      mRenderingPipeLine->SetRenderTarget( pBackBuffer );
      mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

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

  mRenderingPipeLine->SetIsDraw(false);
}

void BtlvRenderSystem::SetupDebugEnv(void)
{
  gfl2::gfx::IGLAllocator *allocator = mBtlvCore->GetGLAllocator();

  {
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
	  // 射影変換行列(パースペクティブ(透視法)射影)
	  SetCamProjMtx(
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

  m_pDrawEnvNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(allocator);
  // ライティング用に環境ノードを追加
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*m_pAmbientLightNode;

    m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(allocator);
    mLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(allocator);
    m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(allocator);

    {
  	  mLightNode->SetNodeName( "DirectionalLight01" );
  	  gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(mLightNode->GetNodeName()) );
  	  mLightNode->SetNodeNameHash( nameHash.value );
  	  mLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
  	  gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
  	  mLightNode->SetPosition( position );
  	  gfl2::math::Vector3 direction = gfl2::math::Vector3(0.0f, -1.0f,-1.0f);
  	  mLightNode->SetDirection( direction );
  	  mLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

    {
  	  m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
  	  gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
  	  m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
  	  m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
  	  m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

    m_pLightSetNode->SetSetNo( 0 );
    m_pLightSetNode->AddLight( mLightNode );//LightSetNodeの子となる
    m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
    m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
  }
  //トレーナー用ライト
  // ライト2～5
  for( int i=0;i<4;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*m_pLightSetNode;

    m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(allocator);
    mCharaLightNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(allocator);

    {
      mCharaLightNode[i]->SetNodeName( "DirectionalLight_Chara" );
      gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(mCharaLightNode[i]->GetNodeName()) );
      mCharaLightNode[i]->SetNodeNameHash( nameHash.value );
      mCharaLightNode[i]->SetLightType( gfl2::renderingengine::LightType::Directional );
      gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
      mCharaLightNode[i]->SetPosition( position );
      gfl2::math::Vector3 direction = -position.Normalize();
      mCharaLightNode[i]->SetDirection( direction );
      mCharaLightNode[i]->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }
    m_pLightSetNode->SetSetNo( LIGHTNO_CHARA_TOP+i );
    m_pLightSetNode->AddLight( mCharaLightNode[i] );//LightSetNodeの子となる
    m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
  }

  gfl2::math::AABB				envSpace( gfl2::math::Vector4(-50000.0f, -50000.0f, -50000.0f), gfl2::math::Vector4(50000.0f, 50000.0f, 50000.0f) );
  m_pDrawEnvNode->SetAABB( envSpace );
  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );

  mRenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );

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

void BtlvRenderSystem::SetCamViewMtx(const gfl2::math::Matrix34 &mtx)
{
  mCamViewMtx = mtx;
  mUpdateCamMatrix = true;
}

void BtlvRenderSystem::SetCamProjMtx(f32 fovy, f32 aspect,f32 nearVal,f32 farVal)
{
  // 射影変換行列(パースペクティブ(透視法)射影)
	mCamProjMtx    = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,farVal);
  mCamFarProjMtx = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,BgSystem::FAR_FAR_PLANE);
  mCamSkyProjMtx = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,BgSystem::SKY_FAR_PLANE);
  mUpdateCamMatrix = true;
}

//視錘台カリング用テーブル
gfl2::math::Matrix44* BtlvRenderSystem::GetFrustumCullingTable(void)
{
  if(mUpdateCamMatrix )
  {
    mUpdateCamMatrix = false;

    gfl2::math::Matrix44 viewProj = mCamProjMtx * mCamViewMtx;
    gfl2::math::AABB::Util::CalcFrustumPlanes( mFrustumCullingTable, viewProj );
  }
  return mFrustumCullingTable;
}

void BtlvRenderSystem::SetupDebugData(void)
{
}


GFL_NAMESPACE_END(btl)
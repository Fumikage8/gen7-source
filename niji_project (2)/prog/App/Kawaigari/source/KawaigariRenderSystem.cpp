//======================================================================
/**
 * @file	KawaigariRenderSystem.C++
 * @brief	KawaigariRenderSystem中心機能
 * @author	PETE
 * @data	2015.10.13
 * 
 *  2016.4.13    ishiguro  処理負荷軽減のため、RenderingPipeLine::Updateは行わず、各描画パスのDrawManagerに対して直接マトリックスを書き換えます。
 * 
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

#include "KawaigariMain.h"

#include "KawaigariRenderSystem.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/NijiRenderPath.gaix>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

//sangoの"さらにドン！"から持ってきたオフセット値
static const float RotationOffsetRadian = gfl2::math::ConvDegToRad(-28.0f);

static const float DualScreenMonsterCameraFrameMax = 30.0f;


KawaigariRenderSystem :: KawaigariRenderSystem(KawaigariMain *mainSys)
{
	mCore = mainSys;
	mIsFinishSetup = false;
  mAdjustScale = 1.0f;

  m_pLightSetNode = NULL;
  m_pDirectionalLightNode = NULL;
  m_pAmbientLightNode = NULL;
  
  m_DualScreenMonsterCameraMode = NOT_DUAL_SCREEN_MONSTER_CAMERA;
  m_DualScreenMonsterCameraFrame = 0;
}

KawaigariRenderSystem :: ~KawaigariRenderSystem(void)
{
}

void KawaigariRenderSystem::Initialize(void)
{
	gfl2::heap::HeapBase *memHeap = mCore->GetHeap();
	gfl2::heap::HeapBase *devHeap = mCore->GetHeap();
	
	mAllocator = GFL_NEW( memHeap ) System::nijiAllocator(devHeap);
		
	mRenderingPipeLine = GFL_NEW( memHeap ) KawaigariRenderingPipeLine( mCore , 512, 256 );  
	mRenderingPipeLine->Initialize();

  app::util::AppRenderingManager::Description appRenderingMgrDesc;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  appRenderingMgrDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
	mAppRenderingManager = GFL_NEW( memHeap )::app::util::AppRenderingManager();
	mAppRenderingManager->SyncCreate(appRenderingMgrDesc, mCore->GetAppHeap(), NULL, mCore->GetAsyncFileManager());
  GFL_ASSERT( mAppRenderingManager->IsCreated() );  // 同期生成なので mAppRenderingManager->IsCreated() では待たない。

	mSetupSeq = 0;

}

void KawaigariRenderSystem::Terminate(void)
{
  
//  if(m_pLightSetNode!=NULL) GFL_SAFE_DELETE(m_pLightSetNode);
 // if(m_pDirectionalLightNode!=NULL) GFL_SAFE_DELETE(m_pDirectionalLightNode);
//  if(m_pAmbientLightNode!=NULL) GFL_SAFE_DELETE(m_pAmbientLightNode);

  GFL_SAFE_DELETE(m_pDrawEnvNode);
	GflHeapFreeMemory(mEdgeMapRenderPathRes);
	GflHeapFreeMemory(mOutLineRenderPathRes);
  GFL_ASSERT( mAppRenderingManager->CanDestroy() );  // 同期生成なので m_pAppRenderingMgr->CanDestroy() では待たない。
	GFL_SAFE_DELETE( mAppRenderingManager );

  mRenderingPipeLine->Terminate();
  GFL_SAFE_DELETE( mRenderingPipeLine );

	GFL_SAFE_DELETE( mDepthStencilTexture );
	
	GFL_SAFE_DELETE( mAllocator );
}

//初期化
bool KawaigariRenderSystem::Setup(void)
{
  float nearVal = 10.0f;
  float farVal = 2000.0f;

  // EXCEPTION FOR POKEMON:
  // CHANGE NEAR & FAR VALUES
  // FIXES DRAW ERROR  
  switch(mCore->GetParameterMonsterNumber())
  {
    
    case MONSNO_PATTIIRU: // 327
    case MONSNO_DARIN: // 781
    case MONSNO_DAINOOZU: // 476
      nearVal = 16.0f;
      farVal = 6000.0f;
      break;

    case MONSNO_MABOROSI17: // 802
    case MONSNO_BIISUTOy: // 794
    case MONSNO_HANAKAMA2: // 754    
    case MONSNO_PURIZUMU: // 800 : ネクロズマ /** @fix GFMcat[456]   【ポケリフレ】合体ネクロズマの頭部のパーツがチラついて表示されます*/
    case MONSNO_BIISUTOs: // 806 : ツンデツンデ   /**  @fix GFMMcat[106]   【ポケリフレ】ツンデツンデの角石や目の部分などがチラついて表示されます  */
      nearVal = 32.0f;
      farVal = 6000.0f;
      break;
  }
  

	gfl2::heap::HeapBase *devHeap = mCore->GetHeap();
	gfl2::fs::AsyncFileManager *fileMng = mCore->GetAsyncFileManager();
	enum
	{
		LOAD_RENDER_RES,
		WAIT_RENDER_RES,
	};

	switch( mSetupSeq )
	{
	case LOAD_RENDER_RES:
    //  Render GARC:
    {	
      GFL_PRINT("Init ARCID_RENDERPATH GARC\n");

      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= ARCID_RENDERPATH;
      req.heapForFile	= devHeap->GetLowerHandle();
      req.heapForReq	= devHeap->GetLowerHandle();
      fileMng->SyncArcFileOpen(req);
    }


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

    // Pre Calculate Projection Matrixes:

    // Upper Screen:
    {    
      u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
      u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
      // 射影変換行列(パースペクティブ(透視法)射影)
      mFinalProjMtx[1] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        nearVal,							// 前方投影面までの距離
        farVal,true);							// 後方投影面までの距離

      mProjMtxTop = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        nearVal,							// 前方投影面までの距離
        farVal,false);							// 後方投影面までの距離

      mFinalSkyProjMtx[1] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        16.0f,							// 前方投影面までの距離
        btl::BgSystem::SKY_FAR_PLANE,true);							// 後方投影面までの距離

      mFinalFarProjMtx[1] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        16.0f,							// 前方投影面までの距離
        btl::BgSystem::FAR_FAR_PLANE,true);							// 後方投影面までの距離
    }

    // Lower Screen:
    {    
      u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::DOWN ] );
      u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::DOWN ] );
      // 射影変換行列(パースペクティブ(透視法)射影)
      mFinalProjMtx[0] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        nearVal,							// 前方投影面までの距離
        farVal,true);							// 後方投影面までの距離

      mProjMtxBottom = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        nearVal,							// 前方投影面までの距離
        farVal,false);							// 後方投影面までの距離

      mFinalSkyProjMtx[0] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        16.0f,							// 前方投影面までの距離
        btl::BgSystem::SKY_FAR_PLANE,true);							// 後方投影面までの距離

      mFinalFarProjMtx[0] = gfl2::math::Matrix44::GetPerspective(
        0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
        16.0f,							// 前方投影面までの距離
        btl::BgSystem::FAR_FAR_PLANE,true);							// 後方投影面までの距離
    }

    mIsFinishSetup = true;
		return true;
	}
	return true;
}

//終了
bool KawaigariRenderSystem::Cleanup(void)
{

	mIsFinishSetup = false;
	return true;
}

//更新
/*
void KawaigariRenderSystem::Update(void)
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
  
  mAppRenderingManager->Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(&mViewMtx[0]);

  // レンダリングパイプライン更新
//  mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
//  mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
//  mRenderingPipeLine->Update();

  mCore->GetBgSystem()->SetSkyMtx(mSkyProjMtx[0],mSkyViewMtx[0]);

}
*/

void KawaigariRenderSystem::CalculateCurrentCameraMatrix(bool force)
{

  static gfl2::math::Vector3 lastEye,lastFocus,lastUp;    
  static int lastFrame = 0;

  switch(m_DualScreenMonsterCameraMode)
  {
  case DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER:
    if((s32)DualScreenMonsterCameraFrameMax <= ++m_DualScreenMonsterCameraFrame)
    {
      m_DualScreenMonsterCameraMode = DUAL_SCREEN_MONSTER_CAMERA_UPPER;
      m_DualScreenMonsterCameraFrame = 0;
    }
    break;
  case DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER:
    if((s32)DualScreenMonsterCameraFrameMax <= ++m_DualScreenMonsterCameraFrame)
    {
      m_DualScreenMonsterCameraMode = DUAL_SCREEN_MONSTER_CAMERA;
      m_DualScreenMonsterCameraFrame = 0;
    }
    break;
  }

  // Camera: (Defaults at ZERO)
  gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, 0.0f );  // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition( 0.0f, 0.0f, 0.0f );  // 注視点  
  gfl2::math::Vector3 upDirection( 0.0f, 0.0f,  0.0f );  // カメラの上方向

  // Get the Current Camera Position:
  mCore->GetCamera()->GetPosition(eyePosition,focusPosition,upDirection);

  if(!force && (lastFrame==m_DualScreenMonsterCameraFrame && lastEye==eyePosition && lastFocus==focusPosition 
      && lastUp==upDirection && mCore->GetPokeModel()->GetScale().x == 1.0f)) return;
  
  lastEye = eyePosition;
  lastFocus = focusPosition;
  lastUp = upDirection;
  lastFrame = m_DualScreenMonsterCameraFrame;  

  eyePosition *= mAdjustScale;
  focusPosition *= mAdjustScale;

  // Move Camera Focus to MonsterBall during End State: (Pokemon Scale 1.0->0.0)
  focusPosition*=mCore->GetPokeModel()->GetScale().x;

  const gfl2::math::Vector3 eye2focus = focusPosition - eyePosition;
  const float xyz = eye2focus.Length();

  //@note : LengthXZ（水平方向の長さ取得）が欲しい
  const float xz = sqrtf(eye2focus.GetX() * eye2focus.GetX() + eye2focus.GetZ() * eye2focus.GetZ());

  gfl2::math::Matrix34 mat = gfl2::math::Matrix34::GetTranslation(eyePosition);

  //@note : sin/cos値からMatrixを取得するGetRotationが欲しい
  f32 s = eye2focus.GetX() / xz;
  f32 c = eye2focus.GetZ() / xz; 
  mat[0][0] = mat[2][2] = c;
  mat[0][2] = s;
  mat[2][0] = -s;

  //元の仰角分だけ回転させる
  s = -eye2focus.GetY() / xyz;
  c = xz / xyz;
  mat *= gfl2::math::Matrix34::Matrix34(
    gfl2::math::Vector4::GetZero(),
    gfl2::math::Vector4::Vector4(0.0f, c, -s, 0.0f),
    gfl2::math::Vector4::Vector4(0.0f, s,  c, 0.0f)
    );

  //さらに上を見るように回転させる
  mat *= gfl2::math::Matrix34::GetRotationX(RotationOffsetRadian);

  const gfl2::math::Vector3 focusPosition2 = mat.GetElemAxisZ() * xyz + mat.GetElemPosition();
  const f32 rate = m_DualScreenMonsterCameraFrame * gfl2::math::FRecip(DualScreenMonsterCameraFrameMax);


  // Calcuate Upper Screen:
  switch(m_DualScreenMonsterCameraMode)
  {
  case DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER:
    //さらに上を見るように回転させる
    mat *= gfl2::math::Matrix34::GetRotationX(RotationOffsetRadian);

    //やりたいことは　→　focusPosition = mat.TransformCoord(gfl2::math::Vector4(0.0f, 0.0f, xyz, 1.0f));
    focusPosition = mat.GetElemAxisZ() * xyz + mat.GetElemPosition();
    focusPosition.Lerp(focusPosition2, 1.0f - rate);
    break;

  case DUAL_SCREEN_MONSTER_CAMERA_UPPER:
    //さらに上を見るように回転させる
    mat *= gfl2::math::Matrix34::GetRotationX(RotationOffsetRadian);

    //やりたいことは　→　focusPosition = mat.TransformCoord(gfl2::math::Vector4(0.0f, 0.0f, xyz, 1.0f));
    focusPosition = mat.GetElemAxisZ() * xyz + mat.GetElemPosition();
    break;

  case DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER:
    //さらに上を見るように回転させる
    mat *= gfl2::math::Matrix34::GetRotationX(RotationOffsetRadian);

    //やりたいことは　→　focusPosition = mat.TransformCoord(gfl2::math::Vector4(0.0f, 0.0f, xyz, 1.0f));
    focusPosition = mat.GetElemAxisZ() * xyz + mat.GetElemPosition();
    focusPosition.Lerp(focusPosition2, rate);
    break;

  default:
    focusPosition = focusPosition2;
    break;
  }

  mFinalViewMtx[1] = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
  mViewMtxTop = mFinalViewMtx[1];

  // Calculate Lower Screen:

  // Get the Current Camera Position:
  mCore->GetCamera()->GetPosition(eyePosition,focusPosition,upDirection);
  eyePosition *= mAdjustScale;
  focusPosition *= mAdjustScale;

  // Move Camera Focus to MonsterBall during End State: (Pokemon Scale 1.0->0.0)
  focusPosition*=mCore->GetPokeModel()->GetScale().x;

  switch(m_DualScreenMonsterCameraMode)
  {
  case DUAL_SCREEN_MONSTER_CAMERA_GO_UPPER:
    focusPosition.Lerp(focusPosition2, rate);
    break;

  case DUAL_SCREEN_MONSTER_CAMERA_GO_LOWER:
    focusPosition.Lerp(focusPosition2, 1.0f - rate);
    break;

  case DUAL_SCREEN_MONSTER_CAMERA_UPPER:
    focusPosition = focusPosition2;
    break;
  }

  mFinalViewMtx[0] = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
  mViewMtxBottom = mFinalViewMtx[0];

}

void KawaigariRenderSystem::CalculateFixedSizeCameraViewProjMatrix(void)
{
  mCore->GetCamera()->SaveCurrentCamera();

  mCore->GetCamera()->SetLocationDirect(KG_CAMERA_EFFECT);

  int lastMode = m_DualScreenMonsterCameraMode;

  // Use Upper Camera if Dual Screen Monster:
  if(mCore->GetPokemonAI()->IsDualScreenMonster())
  {
    m_DualScreenMonsterCameraMode = DUAL_SCREEN_MONSTER_CAMERA_UPPER;
  }

  float adjustScaleBackup = mAdjustScale;

  mAdjustScale = 1.65f;
  CalculateCurrentCameraMatrix(true);

  mAdjustScale = adjustScaleBackup;

  for (u32 i = 0; i < 2; ++i)
  {
    mEffectProjMtx[i] = mFinalProjMtx[i];
    mEffectViewMtx[i] = mFinalViewMtx[i];// mCamViewMtx;  
  }

  m_DualScreenMonsterCameraMode = lastMode;
  mCore->GetCamera()->RestoreCurrentCamera();

}


void KawaigariRenderSystem::CalculateEatCameraViewProjMatrix(void)
{
  mCore->GetCamera()->SaveCurrentCamera();

  mCore->GetCamera()->SetLocationDirect(KG_CAMERA_EAT_FIXED);
  
  int lastMode = m_DualScreenMonsterCameraMode;

  // Use Upper Camera if Dual Screen Monster:
  if(mCore->GetPokemonAI()->IsDualScreenMonster())
  {
  //  m_DualScreenMonsterCameraMode = DUAL_SCREEN_MONSTER_CAMERA_UPPER;
  }

  float adjustScaleBackup = mAdjustScale;
  mAdjustScale = 1.65f;
  CalculateCurrentCameraMatrix(true);
  mAdjustScale = adjustScaleBackup;

  for (u32 i = 0; i < 2; ++i)
  {
    mEatProjMtx[i] = mFinalProjMtx[i];
    mEatViewMtx[i] = mFinalViewMtx[i];// mCamViewMtx;  
  }

  m_DualScreenMonsterCameraMode = lastMode;
  mCore->GetCamera()->RestoreCurrentCamera();
  
}

void KawaigariRenderSystem::Update(void)
{
  if( mIsFinishSetup == false )
  {
    return;
  }

  CalculateCurrentCameraMatrix();
  
  for (u32 i = 0; i < 2; ++i)
  {
    mProjMtx[i] = mFinalProjMtx[i];
    mViewMtx[i] = mFinalViewMtx[i];// mCamViewMtx;

    mSkyProjMtx[i] = mFinalSkyProjMtx[i];
    mSkyViewMtx[i] = mFinalViewMtx[i];//mCamViewMtx;

    mFarProjMtx[i] = mFinalFarProjMtx[i];
    mFarViewMtx[i] = mFinalViewMtx[i];//mCamViewMtx;
  }

  
  mAppRenderingManager->Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
 // gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseApplyBillboard(&mViewMtx[0]);

  mCore->GetPokeModel()->UpdateInterest();
  mCore->GetPokeModel()->UpdateSpring();

  // レンダリングパイプライン更新
//  mRenderingPipeLine->SetProjectionMatrix( mProjMtx[0] );
//  mRenderingPipeLine->SetViewMatrix( mViewMtx[0] );	
//  mRenderingPipeLine->Update(mViewMtx[0]);
//  mRenderingPipeLine->Update();

  mCore->GetBgSystem()->SetFarMtx(mFarProjMtx[0],mFarViewMtx[0]);
  mCore->GetBgSystem()->SetSkyMtx(mSkyProjMtx[1],mSkyViewMtx[1]);

  mCore->GetBgSystemNear()->SetFarMtx(mFarProjMtx[0],mFarViewMtx[0]);
  mCore->GetBgSystemNear()->SetSkyMtx(mSkyProjMtx[1],mSkyViewMtx[1]);

#if defined(GF_PLATFORM_CTR)
  mRenderingPipeLine->Update();
#endif


}

//描画
void KawaigariRenderSystem::PreDraw()
{
  if( mIsFinishSetup == false )
  {
    return;
  }

  mAppRenderingManager->PreDraw();

}

void KawaigariRenderSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{		
	if( mIsFinishSetup == false )
	{
		return;
	}
  
  
  // Setup Camera:
  mRenderingPipeLine->SetIsBottomScreen(false);

  if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN) // Bottom Screen:
  {
    mRenderingPipeLine->SetIsBottomScreen(true);       
    mRenderingPipeLine->SwapIDCopyBuffer();
  }
  
	switch( displayNo )
	{
		// 左画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:    
	case gfl2::gfx::CtrDisplayNo::LEFT:
		{
			gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
			gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
			u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
			u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

			// 射影変換行列(パースペクティブ(透視法)射影)
			gfl2::math::Matrix44 projection;
			gfl2::math::Matrix34 view;

      // Use Precalculated Projection Matrix:
      int off = 0;
      if(displayNo != gfl2::gfx::CtrDisplayNo::DOWN) off = 1;

      mRenderingPipeLine->SetBaseViewProjMatrix(mProjMtx[off],mViewMtx[off]);

#if defined(GF_PLATFORM_CTR)

      mRenderingPipeLine->SetProjectionMatrix( mProjMtx[off] );
      mRenderingPipeLine->SetViewMatrix( mViewMtx[off] );	

      // Setup Mame Matrixes:
      mRenderingPipeLine->SetMameViewProjectionDirect(mEatProjMtx[off],mEatViewMtx[off]);
      mRenderingPipeLine->SetEffectViewProjectionDirect(mEffectProjMtx[off],mEffectViewMtx[off]);

      mRenderingPipeLine->SetRenderTarget( pBackBuffer );
      mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
      mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

      {
        //処理負荷軽減のため、RenderingPipeLine::Updateは行わず、各描画パスのDrawManagerに対して直接マトリックスを書き換えます。
        mRenderingPipeLine->SetViewProjectionDirect( mProjMtx[off], mViewMtx[off] );
        mRenderingPipeLine->SetSkyMtxDirect(mFinalSkyProjMtx[off],mFinalViewMtx[off]);
        mRenderingPipeLine->SetFarMtxDirect(mFinalFarProjMtx[off],mFinalViewMtx[off]);

        mRenderingPipeLine->MinimalUpdate();
      }
#else
        mRenderingPipeLine->SetRenderTarget( pBackBuffer );
        mRenderingPipeLine->SetDepthStencil( pDepthBuffer );
        mRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

        mRenderingPipeLine->SetProjectionMatrix( mFinalProjMtx[off] );			
        mRenderingPipeLine->SetViewMatrix( mFinalViewMtx[off] );	
     //   mRenderingPipeLine->SetSkyMtxDirect(mFinalSkyProjMtx[off],mFinalViewMtx[off]);
     //   mRenderingPipeLine->SetFarMtxDirect(mFinalFarProjMtx[off],mFinalViewMtx[off]);

        mCore->GetBgSystem()->SetFarMtx(mFarProjMtx[off],mFarViewMtx[off]);
        mCore->GetBgSystem()->SetSkyMtx(mSkyProjMtx[off],mSkyViewMtx[off]);

        mCore->GetBgSystemNear()->SetFarMtx(mFarProjMtx[off],mFarViewMtx[off]);
        mCore->GetBgSystemNear()->SetSkyMtx(mSkyProjMtx[off],mSkyViewMtx[off]);

        // Setup Mame Matrixes:
        mRenderingPipeLine->SetMameViewProjectionDirect(mEatProjMtx[off],mEatViewMtx[off]);
        mRenderingPipeLine->SetEffectViewProjectionDirect(mEffectProjMtx[off],mEffectViewMtx[off]);

        mRenderingPipeLine->Update();
#endif
      
			mRenderingPipeLine->Execute();
			break;
		} // case gfl2::gfx::CtrDisplayNo::LEFT

		// 右画面描画(立体視時のみ)
	case gfl2::gfx::CtrDisplayNo::RIGHT:
		break;
		// 下画面描画
//	case gfl2::gfx::CtrDisplayNo::DOWN:
	//	break;
	} // switch( displayNo )
	mAppRenderingManager->Draw(displayNo);

}

void KawaigariRenderSystem::SetupEnv(void)
{
	
	{
		u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
		u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
		// 射影変換行列(パースペクティブ(透視法)射影)
		mCamProjMtx = gfl2::math::Matrix44::GetPerspective(
			0.35f,		// 視野角20°
			(f32)displayWidth / (f32)displayHeight,
			10.0f,							// 前方投影面までの距離
			2000.0f);							// 後方投影面までの距離
	}
	{
		// ビュー変換行列
		gfl2::math::Vector3 eyePosition( 0.0f, 106.0f, -465.0f );  // 視点(カメラの位置)
		gfl2::math::Vector3 focusPosition( 0.0f, 70.0f, 20.0f );  // 注視点
		gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
		mCamViewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
	}
	
	m_pDrawEnvNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mAllocator);
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
		

		m_pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(mAllocator);
		m_pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mAllocator);
		m_pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mAllocator);

		{
			m_pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
			m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
			m_pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );			

      gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
			m_pDirectionalLightNode->SetPosition( position );
      ///gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.390278f, -0.833886, -0.390278);
      gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.752993f, -0.614799f, -0.234570f);
			m_pDirectionalLightNode->SetDirection( direction );
      m_pDirectionalLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

		{
			m_pAmbientLightNode->SetNodeName( "AmbientLight02" );
			gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
			m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
			m_pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
			m_pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
		}

		m_pLightSetNode->SetSetNo( 2 );
		m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
		m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
		m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる
    
	}

	gfl2::math::AABB				envSpace( gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f) );
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
      0.35f,		// 視野角20°
      (f32)displayWidth / (f32)displayHeight,
      10.0f,							// 前方投影面までの距離
      2000.0f);							// 後方投影面までの距離

    // ビュー変換行列
    gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, 0.0f );  // 視点(カメラの位置)
    gfl2::math::Vector3 focusPosition( 0.0f, 0.0f, 200.0f );  // 注視点
    gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
    viewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    mRenderingPipeLine->SetScreenCameraParam(projMtx,viewMtx);

    SetCamProjMtx(0.35f,(f32)displayWidth / (f32)displayHeight,
      10.0f,							// 前方投影面までの距離
      2000.0f);							// 後方投影面までの距離
  }
	

}

void KawaigariRenderSystem::SetCamProjMtx(f32 fovy, f32 aspect,f32 nearVal,f32 farVal)
{
  // 射影変換行列(パースペクティブ(透視法)射影)
  mCamProjMtx    = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,farVal);
  mCamFarProjMtx = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,2000.0f); //この後上書きされてる
  mCamSkyProjMtx = gfl2::math::Matrix44::GetPerspective(fovy,aspect,nearVal,2000.0f);
}



	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)


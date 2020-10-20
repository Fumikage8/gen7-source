//======================================================================
/**
 * @file  BgViewer.cpp
 * @brief 15/04/10
 * @author  ariizumi
 * @data  戦闘背景ビュワー
 */
//======================================================================
#if PM_DEBUG

#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <str/include/gfl2_MsgData.h>
#include <math/include/gfl2_MathTriangular.h>

#include <System/include/HeapDefine.h>
#include <Print/include/PrintSystem.h>
#include <Battle/Background/include/BgSystem.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "System/include/GflUse.h"
#include <model/include/gfl2_BaseModel.h>

#include <Test/AriizumiTest/include/BgViewer.h>

#include "BgViewerLowBgFrame.h"

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_ariizumi.h>
#include <niji_conv_header/message/debug/msg_d_btlbgviewer.h>
#include "niji_conv_header/battle/background/bgsys_normal_ground_def.h"
#include "niji_conv_header/battle/background/bgsys_near_ground_def.h"
#include "niji_conv_header/battle/background/bgsys_far_ground_def.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

enum
{
  SEQ_WAIT,
  SEQ_LOAD_BG_START,
  SEQ_LOAD_BG,
  SEQ_MAIN,
  SEQ_WAIT_DELETE_START,
  SEQ_WAIT_DELETE,
};


// @note テクスチャサイズをチェックするためのAGING処理
#define TEXSIZE_AGING (0)

//============================================================================
//RenderingPipeLine
//============================================================================

void BgViewDebugPipeLine::Init(btl::BgSystem *pBgSystem)
{
  mBgSystem = pBgSystem;
  mbSkyboxDisp=true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  最初のパスから描画できる状態にする
 */
//-----------------------------------------------------------------------------
b32 BgViewDebugPipeLine::StartRenderPath(void)
{
  mRenderPathCnt = 0;
  return true;
}

//----------------------------------------------------------------------------
/**
 *  @brief  現状のパスにあったシーンを返す
 *  @return シーン
 */
//-----------------------------------------------------------------------------
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* BgViewDebugPipeLine::GetRenderPath(void)
{
  switch( mRenderPathCnt )
  {
  case 0:
    if(mbSkyboxDisp){
      return mBgSystem->GetSkyRenderPath();
    }
    break;
  case 1:
    return mBgSystem->GetRenderPath();
  case 2:
    return mBgSystem->GetFarRenderPath();
    //    break;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *  @brief  次のパスへ遷移
 *  @return trueなら次のパス、falseなら描画終了
 */
//-----------------------------------------------------------------------------
b32 BgViewDebugPipeLine::NextRenderPath(void)
{
  mRenderPathCnt++;
  if( mRenderPathCnt >= 3 )
  {
    return false;
  }
  return true;
}





BgViewerProc::BgViewerProc()
{
  mInitSeq = 0;
  mMainSeq = 0;
  mEndSeq = 0;

  mBgTypeGround = 0;
  mBgTypeNear = 0;
  mBgTypeFar = 0;


  mSecond=0;
  mMinite=0;
  mHour=0;
  
  mbSkyboxDisp = true;
	mbAnimStart=false;

}

BgViewerProc::~BgViewerProc(void)
{

}


void BgViewerProc::AppParamInit(gfl2::heap::HeapBase *devHeap)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();   //ファイルマネージャー

  mpFrameManager = GFL_NEW(devHeap) applib::frame::Manager(devHeap);

  BgViewerLowBgFrame* pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BgViewerLowBgFrame>( mpFrameManager );

  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(devHeap) app::util::Heap();
  // ローカルヒープを作る
  mpAppHeap->LocalHeapCreate(devHeap,devHeap,  0x100000,  0x400000);  //1M/4M
  // ファイル読み込み用はローカルではなくAPP_DEVICEの後方からとる
  mpAppHeap->SetFileReadHeap( devHeap );

  mpAppRenderingManager  = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  {
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  }
  mpAppRenderingManager->SyncCreate( desc, mpAppHeap, NULL, pAsyncFileManager );

  mBgViewAppParam.mpAsyncFileManager = pAsyncFileManager;
  mBgViewAppParam.mpAppRenderingManager = mpAppRenderingManager;   //レンダリングマネージャー
  mBgViewAppParam.mpAppHeap = mpAppHeap;  //appheap
  mBgViewAppParam.mpAppFrameManager = mpFrameManager;
  mBgViewAppParam.pBgViewerActionListener = this;

  pFrame->Setup(&mBgViewAppParam);

}

bool BgViewerProc::AppParamEnd(void)
{
  if( mpFrameManager->End() == true )
  {
    return false;
  }
  if( !mpAppRenderingManager->CanDestroy() )
  {
    return false;
  }



  GFL_SAFE_DELETE(  mpAppRenderingManager);

  GFL_SAFE_DELETE( mpFrameManager );

  GFL_SAFE_DELETE(  mpAppHeap);
  return true;
}


//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result BgViewerProc::InitFunc(gfl2::proc::Manager* pManager)
{
  switch( mInitSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

      //0x1C00000
      mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1000000,gfl2::heap::HEAP_TYPE_EXP,false,"BgViewer");//16M
      mBgSysHeap = GFL_CREATE_LOCAL_HEAP_NAME(mHeap,0x100000,gfl2::heap::HEAP_TYPE_EXP,false,"BgSystem");//1M

      AppParamInit(mHeap);

      mpRenderPipeLine = GFL_NEW(mHeap) BgViewDebugPipeLine();

      //@todo
      mGLAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);

      mBgSystem = GFL_NEW(mHeap)btl::BgSystem();

      mpRenderPipeLine->Init(mBgSystem);

      btl::BgSystem::INIT_PARAM initParam;
      initParam.heap = mBgSysHeap;
      initParam.workHeap = mHeap;
      initParam.fileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
#if PM_DEBUG
      initParam.useHio = true;
#endif
      mBgSystem->Initialize( initParam,mHour, mMinite , mSecond );

      {
        u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        // 射影変換行列(パースペクティブ(透視法)射影)
        mCamProjMtx = gfl2::math::Matrix44::GetPerspective(
          gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
          (f32)displayWidth / (f32)displayHeight,
          1.0f,							// 前方投影面までの距離
          120000.0f);							// 後方投影面までの距離
      }

      SetFinderCameraMode();
      UpdateCameraMatrix();
      mpRenderPipeLine->SetProjectionMatrix( mProjMtx[0] );
      mpRenderPipeLine->SetViewMatrix( mViewMtx[0] );

      mBgSystem->SetSkyMtx(mProjMtx[0],mViewMtx[0]);

      CreateDebugMenu();
      mInitSeq++;
    }
    break;

  case 1:
    if( mBgSystem->IsFinishInitialize() )
    {
      return gfl2::proc::RES_FINISH;
    }
  }
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result BgViewerProc::EndFunc(gfl2::proc::Manager* pManager)
{
  switch( mEndSeq )
  {
  case 0:
    if(mMainSeq == SEQ_MAIN)
    {
      mBgSystem->Delete();
      mEndSeq++;
    }
    else
    {
      mEndSeq++;
      mEndSeq++;
    }
    break;
  case 1:
    if( mBgSystem->IsFinishDelete() )
    {
      mEndSeq++;
    }
    break;
  case 2:
    {
      mBgSystem->Terminate();
      mEndSeq++;
    }
    break;
  case 3:
    if( !mBgSystem->IsFinishTerminate() )
    {
      break;
    }
    if(!AppParamEnd())
    {
      break;
    }
    {
      DeleteDebugMenu();
      GFL_SAFE_DELETE(mBgSystem);

      GFL_SAFE_DELETE(mpRenderPipeLine);

      GFL_SAFE_DELETE( mGLAllocator );

      GFL_DELETE_HEAP(mBgSysHeap);
      GFL_DELETE_HEAP(mHeap);
      return gfl2::proc::RES_FINISH;
    }
    break;
  }
  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result BgViewerProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);


  switch( mMainSeq )
  {
  case SEQ_WAIT:
#if TEXSIZE_AGING
		if (!((mBgTypeGround == static_cast<int>(btl::bg::BGSYS_NORMAL_GROUND_MAX)) &&
			(mBgTypeNear == static_cast<int>(btl::bg::BGSYS_NEAR_GROUND_MAX)) &&
			(mBgTypeFar == static_cast<int>(btl::bg::BGSYS_FAR_GROUND_MAX))))
		{
			mMainSeq = SEQ_LOAD_BG_START;
		}
#endif // TEXSIZE_AGING
    break;
  case SEQ_LOAD_BG_START:
    {
      mLoadStartHeap = mBgSysHeap->GetTotalFreeSize();
      btl::BgSystem::SETUP_PARAM setupParam;
      setupParam.groundType = mBgTypeGround;
			setupParam.nearType = mBgTypeNear;
			setupParam.farType = mBgTypeFar;
      setupParam.useSkyBox = mbSkyboxDisp;
#if TEXSIZE_AGING
			if (mBgTypeGround >= static_cast<int>(btl::bg::BGSYS_NORMAL_GROUND_MAX)) setupParam.groundType = 0;
			if (mBgTypeNear >= static_cast<int>(btl::bg::BGSYS_NEAR_GROUND_MAX)) setupParam.nearType = 0;
			if (mBgTypeFar >= static_cast<int>(btl::bg::BGSYS_FAR_GROUND_MAX)) setupParam.farType = 0;
#endif // TEXSIZE_AGING
			mBgSystem->StartLoad(setupParam);

      mMainSeq = SEQ_LOAD_BG;
    }
    break;

  case SEQ_LOAD_BG:
    mBgSystem->LoadUpdate();
    if( mBgSystem->IsFinishLoad() )
    {
      mBgSystem->Create();
      mBgSystem->SetVisible(true);
      mMainSeq = SEQ_MAIN;
      const int mem = mLoadStartHeap - mBgSysHeap->GetTotalFreeSize();
      GFL_RELEASE_PRINT("BgSystem Use memory size[%d(0x%x)\n",mem,mem);
    }
    break;
  case SEQ_MAIN:
    UpdateCamera();
#if TEXSIZE_AGING
		mMainSeq = SEQ_WAIT_DELETE_START;
#endif // TEXSIZE_AGING
    break;
  case SEQ_WAIT_DELETE_START:
    {
      mBgSystem->SetVisible(false);
      mBgSystem->Delete();
      mMainSeq = SEQ_WAIT_DELETE;
    }
    break;

  case SEQ_WAIT_DELETE:
    if( mBgSystem->IsFinishDelete() )
    {
#if TEXSIZE_AGING
			if (mBgTypeGround < static_cast<int>(btl::bg::BGSYS_NORMAL_GROUND_MAX))
			{
				mBgTypeGround = mBgTypeGround + 1;
			}
			else if (mBgTypeNear < static_cast<int>(btl::bg::BGSYS_NEAR_GROUND_MAX))
			{
				mBgTypeNear = mBgTypeNear + 1;
			}
			else if (mBgTypeFar < static_cast<int>(btl::bg::BGSYS_FAR_GROUND_MAX))
			{
				mBgTypeFar = mBgTypeFar + 1;
			}
#endif // TEXSIZE_AGING
      mMainSeq = SEQ_WAIT;
    }
    break;
  }

  if(mpFrameManager->Main()==applib::frame::MAINSTATUS_NULL){
    return gfl2::proc::RES_FINISH;
  }
  
  if( mBgClearType == 2)
  {
    static int cnt = 0;
    cnt++;

    GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
    setting.color.Set( (cnt%3==0?1.0f:0.0f) , (cnt%3==1?1.0f:0.0f) , (cnt%3==2?1.0f:0.0f) , 1.0f );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
    setting.color.Set( (cnt%3==0?1.0f:0.0f) , (cnt%3==1?1.0f:0.0f) , (cnt%3==2?1.0f:0.0f) , 1.0f );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);
  }
  else
  {
    GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
    setting.color.Set( 0.3f , 0.3f , 0.3f , 1.0f );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
    setting.color.Set( 0.3f , 0.3f , 0.3f , 1.0f );
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);
  }

  mSecond += 30;
  if(mSecond >= 60){
    mMinite++;
    mSecond=0;
  }
  if(mMinite >= 60){
    mHour++;
    mMinite=0;
  }
  if(mHour>=24){
    mHour=0;
  }

  
  mBgSystem->Update((s32)mHour,(s32)mMinite,(s32)mSecond);
  mpRenderPipeLine->Update();

  mpAppRenderingManager->Update();


  if( mDebugPrintTick )
  {
    mDebugPrintTickCnt++;
    if( mDebugPrintTickCnt > 60 )
    {
      mDebugPrintTickCnt = 0;
      const int update = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
      const int upnext = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
      const int draw   = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
      const int gpu    = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
      gfl2::debug::ReleasePrintCore("Tick[%6d][%6d][%6d][%6d]\n",update,upnext,draw,gpu);
    }
  }

  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief	PROCの描画処理(下準備)
 * @param pManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void BgViewerProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  mpAppRenderingManager->PreDraw();

}

/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void BgViewerProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // @todo 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya

  mpFrameManager->Draw(displayNo);
  mpAppRenderingManager->Draw(displayNo);


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

      mpRenderPipeLine->SetProjectionMatrix( mProjMtx[0] );
      mpRenderPipeLine->SetViewMatrix( mViewMtx[0] );

      mpRenderPipeLine->SetRenderTarget( pBackBuffer );
      mpRenderPipeLine->SetDepthStencil( pDepthBuffer );
      mpRenderPipeLine->SetDisplaySize( displayWidth, displayHeight );

      mpRenderPipeLine->Execute();
      break;
    } // case gfl2::gfx::CtrDisplayNo::LEFT

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    break;
    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    break;
  } // switch( displayNo )
}

inline void Rot_Deg( f32 &x , f32 &z , const f32 idx )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosDeg( &sin,&cos,idx );
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

void BgViewerProc::UpdateCamera(void)
{
  if(gfl2::debug::DebugWin_IsOpen())
  {
    return;
  }

  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  gfl2::ui::VectorDevice *stick  = uiMng->GetStick(0);
  gfl2::ui::VectorDevice *uiKey = uiMng->GetCrossKey(0);

  //カメラ操作
  static const f32 STIC_MIN_VAL = 0.3f;
  static const f32 STIC_MID_VAL = 0.8f;

  f32 tempRotH = 0;
  f32 tempRotV = 0;
  if( gfl2::math::Abs( stick->GetX() ) > STIC_MIN_VAL )
  {
    if( gfl2::math::Abs( stick->GetX() ) > STIC_MID_VAL )
    {
      tempRotH += stick->GetX()*3;
    }
    else
    {
      tempRotH += stick->GetX();
    }
  }
  if( gfl2::math::Abs( stick->GetY() ) > STIC_MIN_VAL )
  {
    if( gfl2::math::Abs( stick->GetY() ) > STIC_MID_VAL )
    {
      tempRotV += stick->GetY()*3;
    }
    else
    {
      tempRotV += stick->GetY();
    }
  }

  if (mCameraMode == 0)
  {
    //バトルモード

    if (btn->IsHold(gfl2::ui::BUTTON_R))
    {
      //カメラ視点操作
      mCamRotHorizonal += tempRotH;
      mCamRotVertical = gfl2::math::Clamp(mCamRotVertical - tempRotV, -89.0f, 89.0f);
      UpdateCameraMatrix();
    }
    else
    {
      //カメラ位置操作
      if (tempRotV != 0)
      {
        gfl2::math::Vector3 tempDir(0, 0, -tempRotV * 10);
        Rot_Deg(tempDir.x, tempDir.z, mCamRotHorizonal);
        mCamPos += tempDir;
      }
      if (tempRotH != 0)
      {
        gfl2::math::Vector3 tempDir(tempRotH * 10, 0, 0);
        Rot_Deg(tempDir.x, tempDir.z, mCamRotHorizonal);
        mCamPos += tempDir;
      }
      UpdateCameraMatrix();
    }

    if (btn->IsTrigger(gfl2::ui::BUTTON_X))
    {
      SetCameraPosTarget(gfl2::math::Vector3(256.0f, 106.0f, 465.0f), gfl2::math::Vector3(-45.0f, 70.0f, -20.0f));
      UpdateCameraMatrix();
    }
  }
  else
  {
    //ファインダーモード

    if (btn->IsHold(gfl2::ui::BUTTON_R))
    {
      if (tempRotV != 0)
      {
        mCamFovDeg = gfl2::math::Clamp(mCamFovDeg - tempRotV, 3.0f, 30.0f);
        SetFovDeg(mCamFovDeg);
      }
    }
    else
    {
      //カメラ位置操作
      mCamRotHorizonal += tempRotH;
      mCamRotVertical = gfl2::math::Clamp(mCamRotVertical + tempRotV, 10.0f, 80.0f);
      UpdateCameraMatrix();
    }
  }

  if( uiKey->IsHold(gfl2::ui::DIR_DOWN) )
  {
    mCamPos.y -= 5.0f;
    UpdateCameraMatrix();
  }
  else
  {
    if( uiKey->IsHold(gfl2::ui::DIR_UP) )
    {
      mCamPos.y += 5.0f;
      UpdateCameraMatrix();
    }
  }
  if (mCameraMode == 1)
  {
    //ファインダーのみ高さ制御
    mCamPos.y = gfl2::math::Clamp(mCamPos.y, 0.0f, 150.0f);
  }

}

void BgViewerProc::SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg)
{
  mCamPos = pos;

  gfl2::math::Vector3 camOfs = pos - trg;
  gfl2::math::Vector2 lenVec;
  lenVec.Set(camOfs.x,camOfs.z);
  f32 tempLen = lenVec.Length();

  mCamRotHorizonal = gfl2::math::Atan2Deg(-camOfs.x , camOfs.z );
  mCamRotVertical = gfl2::math::Atan2Deg(camOfs.y , tempLen );

  UpdateCameraMatrix();
}

void BgViewerProc::SetFovDeg(f32 fov)
{
  mCamFovDeg = fov;
  u32				          displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]);
  u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]);
  // 射影変換行列(パースペクティブ(透視法)射影)
  mCamProjMtx = gfl2::math::Matrix44::GetPerspective(
    gfl2::math::ConvDegToRad(mCamFovDeg),		// 視野角30°
    (f32)displayWidth / (f32)displayHeight,
    1.0f,							// 前方投影面までの距離
    120000.0f);							// 後方投影面までの距離
  UpdateCameraMatrix();
}

void BgViewerProc::UpdateCameraMatrix(void)
{
  gfl2::math::Vector3 camTrg(0, 0, -10.0f);
  gfl2::math::Vector3 camPos(0, 0, -10.0f);
  if (mCameraMode == 0)
  { 
    //バトルモード
    Rot_Deg(camTrg.z, camTrg.y, mCamRotVertical);
    Rot_Deg(camTrg.x, camTrg.z, mCamRotHorizonal);

    camTrg += mCamPos;
    camPos = mCamPos;
  }
  else
  {
    //ファインダーモード

    camPos.z = -mCamLength;
    Rot_Deg(camPos.z, camPos.y, -mCamRotVertical);
    Rot_Deg(camPos.x, camPos.z, -mCamRotHorizonal);
    camPos += mCamPos;
    camTrg = mCamPos;
  }

  {
    // ビュー変換行列
    gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
    mCamViewMtx = gfl2::math::Matrix34::GetLookAt(camPos, camTrg, upDirection);
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
  mBgSystem->SetSkyMtx(mProjMtx[0],mViewMtx[0]);
  mBgSystem->SetFarMtx(mProjMtx[0],mViewMtx[0]);
}

void BgViewerProc::SetFinderCameraMode(void)
{
  mCameraMode = 1;
  mCamPos.Set(0, 0, 0);
  mCamRotVertical = 10.0f;
  mCamRotHorizonal = 0.0f;
  mCamLength = 880.0f;
  mCamFovDeg = 30.0f;
  UpdateCameraMatrix();

}


extern void BgViewer_DebugUpdate_StartAnime( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void BgViewer_DebugUpdate_StopAnime( void* userWork  , gfl2::debug::DebugWinItem *item );
extern void BgViewer_DebugUpdate_DispCloud(void *work);
extern void BgViewer_DebugUpdate_ApplyBtlCamera(void* userWork, gfl2::debug::DebugWinItem *item);
extern void BgViewer_DebugUpdate_ApplyFinderCamera(void* userWork, gfl2::debug::DebugWinItem *item);

void BgViewerProc::CreateDebugMenu(void)
{
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"BgViewer",NULL);
  gfl2::debug::DebugWinItem *item;

  gfl2::debug::DebugWin_AddItemValueBool( topGroup ,mHeap, L"DumpTick", &mDebugPrintTick);
  item = gfl2::debug::DebugWin_AddItemValueBool( topGroup ,mHeap, L"Cloud", &mDebugCloud);
  item->SetChangedValueFunc(BgViewer_DebugUpdate_DispCloud,this);
  item = gfl2::debug::DebugWin_AddItemValueBool( topGroup ,mHeap, L"SunMoon", &mDebugSunMoon);
  item->SetChangedValueFunc(BgViewer_DebugUpdate_DispCloud,this);
  item = gfl2::debug::DebugWin_AddItemValueInt( topGroup ,mHeap, L"BgClear", &mBgClearType , 0 , 2);
  item->SetChangedValueFunc(BgViewer_DebugUpdate_DispCloud,this);

  item = gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup, mHeap, L"BtlSelfSize", &mBtlCamSize[0], print::GetMessageArcId_Debug(), GARC_debug_message_d_btlbgviewer_DAT, 0, 2, D_BTLBGVIEWER_041);
  item = gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup, mHeap, L"BtlEnemySize", &mBtlCamSize[1], print::GetMessageArcId_Debug(), GARC_debug_message_d_btlbgviewer_DAT, 0, 2, D_BTLBGVIEWER_041);
  item = gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup, mHeap, L"BtlRule", &mBtlRule, print::GetMessageArcId_Debug(), GARC_debug_message_d_btlbgviewer_DAT, 0, 2, D_BTLBGVIEWER_044);
  item = gfl2::debug::DebugWin_AddItemUpdateFunc(topGroup, mHeap, L"ApplyBtlCam", this, BgViewer_DebugUpdate_ApplyBtlCamera);
  item = gfl2::debug::DebugWin_AddItemUpdateFunc(topGroup, mHeap, L"ApplyFinderCam", this, BgViewer_DebugUpdate_ApplyFinderCamera);
}

void BgViewerProc::DeleteDebugMenu(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"BgViewer");
}

void BgViewer_DebugUpdate_DispCloud( void *work )
{
  BgViewerProc *bgViewer = static_cast<BgViewerProc*>(work);

  if (bgViewer->GetBgSystem()->GetSkyBox()->IsEnable())
  {
    if( bgViewer->mDebugCloud )
    {
      bgViewer->GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_CLOUDY,10);
    }
    else
    {
      bgViewer->GetBgSystem()->GetSkyBox()->ChangeSkyRequest(System::Skybox::Skybox::SKY_TYPE_SUNNY,10);
    }

    if( bgViewer->mDebugSunMoon )
    {
      bgViewer->GetBgSystem()->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_SUN)->SetVisible(true);
      bgViewer->GetBgSystem()->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_MOON)->SetVisible(true);
    }
    else
    {
      bgViewer->GetBgSystem()->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_SUN)->SetVisible(false);
      bgViewer->GetBgSystem()->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_MOON)->SetVisible(false);
    }
  }

  if( bgViewer->mBgClearType == 1 )
  {
    GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);
  }
  else
  {
    GameSys::GameManager::BUFFER_CLEAR_SETTING setting;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::LEFT,setting);
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,&setting);
    setting.clearTarget = GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL;
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->SetBufferClearSetting(gfl2::gfx::CtrDisplayNo::DOWN,setting);
  }
}

void BgViewer_DebugUpdate_ApplyBtlCamera(void* userWork, gfl2::debug::DebugWinItem *item)
{
  BgViewerProc *bgViewer = static_cast<BgViewerProc*>(userWork);

  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if (!btn->IsTrigger(gfl2::ui::BUTTON_A))
  {
    return;
  }

  const int mPos = bgViewer->mBtlCamSize[0];
  const int ePos = bgViewer->mBtlCamSize[1];

  gfl2::math::Vector3 pos, trg;
  f32 fov = 30.0f;

  switch (bgViewer->mBtlRule)
  {
  default:
  case 0:    ///< シングル
      //自分が縦、相手が横
    {
      //s_s,s_m,s_b
      //m_s,m_m,m_b
      //b_s,b_m,b_b
      static const gfl2::math::Vector3 posArr[3][3] =
      {
        { gfl2::math::Vector3(125.0f, 50.0f, 360.0f), gfl2::math::Vector3(155.0f, 56.0f, 380.0f), gfl2::math::Vector3(185.0f, 62.0f, 400.0f) },
        { gfl2::math::Vector3(236.0f, 110.0f, 460.0f), gfl2::math::Vector3(256.0f, 106.0f, 465.0f), gfl2::math::Vector3(276.0f, 102.0f, 470.0f) },
        { gfl2::math::Vector3(220.0f, 150.0f, 470.0f), gfl2::math::Vector3(240.0f, 140.0f, 485.0f), gfl2::math::Vector3(260.0f, 130.0f, 500.0f) },
      };
      static const gfl2::math::Vector3 trgArr[3][3] =
      {
        { gfl2::math::Vector3(-50.0f, 50.0f, -40.0f), gfl2::math::Vector3(-45.0f, 56.0f, -40.0f), gfl2::math::Vector3(-40.0f, 62.0f, -40.0f) },
        { gfl2::math::Vector3(-50.0f, 60.0f, -20.0f), gfl2::math::Vector3(-45.0f, 70.0f, -20.0f), gfl2::math::Vector3(-40.0f, 80.0f, -20.0f) },
        { gfl2::math::Vector3(0.0f, 130.0f, 90.0f), gfl2::math::Vector3(0.0f, 130.0f, 90.0f), gfl2::math::Vector3(0.0f, 130.0f, 90.0f) },
      };
      trg.Set(trgArr[mPos][ePos]);
      pos.Set(posArr[mPos][ePos]);
    }
    break;
  case 1:    ///< ダブル
    {
      //自分が縦、相手が横
      //s_s,s_m,s_b
      //m_s,m_m,m_b
      //b_s,b_m,b_b
      static const gfl2::math::Vector3 posArr[3][3] =
      {
        { gfl2::math::Vector3(180.0f, 130.0f, 480.0f), gfl2::math::Vector3(200.0f, 130.0f, 530.0f), gfl2::math::Vector3(220.0f, 120.0f, 580.0f) },
        { gfl2::math::Vector3(220.0f, 220.0f, 600.0f), gfl2::math::Vector3(260.0f, 180.0f, 630.0f), gfl2::math::Vector3(300.0f, 160.0f, 660.0f) },
        { gfl2::math::Vector3(380.0f, 340.0f, 620.0f), gfl2::math::Vector3(360.0f, 320.0f, 640.0f), gfl2::math::Vector3(400.0f, 300.0f, 700.0f) },
      };
      static const gfl2::math::Vector3 trgArr[3][3] =
      {
        { gfl2::math::Vector3(-40.0f, 20.0f, 30.0f), gfl2::math::Vector3(-25.0f, 40.0f, 0.0f), gfl2::math::Vector3(-10.0f, 70.0f, -30.0f) },
        { gfl2::math::Vector3(-40.0f, 40.0f, 30.0f), gfl2::math::Vector3(-40.0f, 80.0f, 0.0f), gfl2::math::Vector3(-40.0f, 100.0f, -40.0f) },
        { gfl2::math::Vector3(-100.0f, 0.0f, 0.0f), gfl2::math::Vector3(-80.0f, 0.0f, -20.0f), gfl2::math::Vector3(-70.0f, 30.0f, -50.0f) },
      };
      trg.Set(trgArr[mPos][ePos]);
      pos.Set(posArr[mPos][ePos]);
    }
    break;
  case 2:    ///< ロイヤル
    {
      //サイズ変動なし
      trg.Set(0, 0, -100);
      pos.Set(300, 250, 750);
    }
    break;
  }

  bgViewer->mCameraMode = 0;
  bgViewer->SetCameraPosTarget(pos, trg);
  bgViewer->SetFovDeg(fov);
}

void BgViewer_DebugUpdate_ApplyFinderCamera(void* userWork, gfl2::debug::DebugWinItem *item)
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if (!btn->IsTrigger(gfl2::ui::BUTTON_A))
  {
    return;
  }

  BgViewerProc *bgViewer = static_cast<BgViewerProc*>(userWork);

  bgViewer->SetFinderCameraMode();
}


int BgViewerProc::OnActionStart( int actionno )
{
  int addVal = 1;
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsHold(gfl2::ui::BUTTON_R) || btn->IsHold(gfl2::ui::BUTTON_L)  )
  {
    addVal = 5;
  }

  int ret=0;
  
  switch(actionno){
  case BgViewerActionListener::ACTION_READ_FIELD:
    if(mMainSeq==SEQ_WAIT){
      mMainSeq = SEQ_LOAD_BG_START;
    }
    else if(mMainSeq==SEQ_MAIN){
      mMainSeq = SEQ_WAIT_DELETE_START;
    }
    break;
  case BgViewerActionListener::ACTION_SKY_ON:
    {
      mbSkyboxDisp = true;
      mpRenderPipeLine->SetSkyboxDisp(mbSkyboxDisp);
      ret = 1;
    }
    break;
	case BgViewerActionListener::ACTION_SKY_OFF:
    {
      mbSkyboxDisp = false;
      mpRenderPipeLine->SetSkyboxDisp(mbSkyboxDisp);
    }
    break;
	case BgViewerActionListener::ACTION_ANIM_ADD:
    {
      mAnimeType += addVal;
      if(mAnimeType>=29)
      {
        mAnimeType = 0;
      }
      ret = mAnimeType;
    }
    break;
	case BgViewerActionListener::ACTION_ANIM_SUB:
    {
      mAnimeType -= addVal;
      if(mAnimeType<0){
        mAnimeType = 28;
      }
      ret = mAnimeType;
    }
    break;
  case BgViewerActionListener::ACTION_ANIM_START:
    if(!mbAnimStart){
      mBgSystem->PlayAnime(mAnimeType);
    }
    else{
      mBgSystem->StopAnime(mAnimeType);
    }
    mbAnimStart = !mbAnimStart;
    ret = mbAnimStart;
    break;

  case BgViewerActionListener::ACTION_GROUND_ADD:
    mBgTypeGround += addVal;
    if(mBgTypeGround >= (btl::bg::BGSYS_NORMAL_GROUND_MAX))
    {
      mBgTypeGround = 0;
    }
    ret = mBgTypeGround;
    break;
  case BgViewerActionListener::ACTION_GROUND_SUB:
    mBgTypeGround -= addVal;
    if(mBgTypeGround < 0)
    {
      mBgTypeGround = (btl::bg::BGSYS_NORMAL_GROUND_MAX-1);
    }
    ret = mBgTypeGround;
    break;
  case BgViewerActionListener::ACTION_NEAR_ADD:
    mBgTypeNear += addVal;
    if(mBgTypeNear >= (btl::bg::BGSYS_NEAR_GROUND_MAX))
    {
      mBgTypeNear = 0;
    }
    ret = mBgTypeNear;
    break;
  case BgViewerActionListener::ACTION_NEAR_SUB:
    mBgTypeNear -= addVal;
    if(mBgTypeNear < 0)
    {
      mBgTypeNear = (btl::bg::BGSYS_NEAR_GROUND_MAX-1);
    }
    ret = mBgTypeNear;
    break;
  case BgViewerActionListener::ACTION_FAR_ADD:
    mBgTypeFar += addVal;
    if(mBgTypeFar >= (btl::bg::BGSYS_FAR_GROUND_MAX))
    {
      mBgTypeFar = 0;
    }
    ret = mBgTypeFar;
    break;
  case BgViewerActionListener::ACTION_FAR_SUB:
    mBgTypeFar -= addVal;
    if(mBgTypeFar < 0)
    {
      mBgTypeFar = (btl::bg::BGSYS_FAR_GROUND_MAX-1);
    }
    ret = mBgTypeFar;
    break;
  case BgViewerActionListener::ACTION_ANIMREQ:
    mBgSystem->RequestAnime(mAnimeType);
    break;
  }
  return ret;
}


GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif//PM_DEBUG
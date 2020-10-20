//
// CTR - Ro Module : NijiTemplateRo
//

#if (PM_DEBUG)

#include <GameSys/include/GameProcManager.h>

#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <str/include/gfl2_MsgData.h>
#include <math/include/gfl2_MathTriangular.h>

#include <System/include/HeapDefine.h>
#include <Print/include/PrintSystem.h>
#include <Battle/Background/include/BgSystem.h>

#include <arc_index/debug_message.gaix>
#include <arc_def_index/arc_def.h>

#include "System/include/GflUse.h"
#include "../include/AnaiTestProc.h"

#include "System/include/SkyBox/Skybox.h"
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <heap/include/gfl2_heap_base.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(AnaiTest)


static const int SIZE_MY_APP = 0x400000;       ///  アプリメモリも容量管理のため分けて管理
static const int SIZE_MY_APPDEV = 0x800000;    /// アプリメモリも容量管理のため分けて管理

/**
  *
  */
AnaiTestProc::AnaiTestProc()
{
  mInitSeq = INIT_START;
  mMainSeq = 0;
  mEndSeq = 0;

  mBgTypeGround = 0;
  mBgTypeNear = 0;
  mBgTypeFar = 0;

  DebugTime = 0;//60*60*12;

}

/**
  *
  */
AnaiTestProc::~AnaiTestProc()
{

}

void AnaiTestProc::CreateMemory(void)
{

  //HEAPID_APP  本体に関するものはAPPめもりーから確保します
  gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(appHeap) app::util::Heap();

  mpAppHeap->LocalHeapCreate(appHeap,devHeap,SIZE_MY_APP,SIZE_MY_APPDEV);

  mpAppHeap->SetFileReadHeap(devHeap);

  mpAppParam = (Test::AnaiTest::APP_PARAM*)GflHeapAllocMemory(mpAppHeap->GetSystemHeap() , sizeof(Test::AnaiTest::APP_PARAM));

}


/**
  *
  */
gfl2::proc::Result AnaiTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
#if 0
  switch( mInitSeq )
  {
  case INIT_START:
    {
      CreateMemory();

      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

      //0x1C00000
      mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1000000,gfl2::heap::HEAP_TYPE_EXP,false,"AnaiTestProc");

      //@todo
      mOldAllocator = gfl2::gfx::GLMemory::GetAllocator();
      mGLAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);
      gfl2::gfx::GLMemory::SetAllocator(mGLAllocator);


      //BG初期化
      mBgSystem = GFL_NEW(mHeap)btl::BgSystem();
      btl::BgSystem::INIT_PARAM initParam;
      initParam.heap = mHeap;
      initParam.fileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
      mBgSystem->Initialize( initParam );

      //skybox初期化開始
      bool bSkyboxCreate = mBgSystem->StartSkyboxInit(mpAppHeap , initParam);
      GFL_ASSERT(bSkyboxCreate);

      {
        u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth ( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
	      // 射影変換行列(パースペクティブ(透視法)射影)
	      mCamProjMtx = gfl2::math::Matrix44::GetPerspective(
			      gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
            (f32)displayWidth / (f32)displayHeight,
			      1.0f,							// 前方投影面までの距離
			      12000.0f);							// 後方投影面までの距離
      }

      mCamRotVertical = 0;
      mCamRotHorizonal = 0;
      mCamPos.Set(0,0,0);
      
      SetCameraPosTarget(gfl2::math::Vector3(256.0f,106.0f,465.0f),gfl2::math::Vector3(-45.0f,70.0f,-20.0f));
      UpdateCameraMatrix();
      gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	

      CreateDebugMenu();
      
      mInitSeq = PREPARE_SKYBOX;//WAIT_BGSYSTEM_INIT;
    }
    break;

  case PREPARE_SKYBOX:
    if (mBgSystem->isReadySkybox())
    {
      mInitSeq = WAIT_SKYBOX_PROC;
    }
    break;

  case WAIT_SKYBOX_PROC:
    if (mBgSystem->isReadySkyboxResource())
    {
      mInitSeq =  BGSYSTEM_INIT;
    }
    break;
    
  case BGSYSTEM_INIT:
      mBgSystem->SetWeather(System::Skybox::Skybox::SKY_TYPE_SUNNY);
      mInitSeq =  WAIT_BGSYSTEM_INIT;
      break;

  case WAIT_BGSYSTEM_INIT:
    if( mBgSystem->IsFinishInitialize() )
    {
      mInitSeq = FINISHED;
    }
    break;    

  case FINISHED:
    return gfl2::proc::RES_FINISH;

  default:
    break;
  }
#endif
  return gfl2::proc::RES_CONTINUE;
}

/**
  *
  */
gfl2::proc::Result AnaiTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
#if 0
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);

  enum
  {
    SEQ_WAIT,
    SEQ_LOAD_BG,
    SEQ_MAIN,
    SEQ_WAIT_DELETE,
  };

  switch( mMainSeq )
  {
  case SEQ_WAIT:
    if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      btl::BgSystem::SETUP_PARAM setupParam;
      setupParam.groundType = mBgTypeGround;
      setupParam.nearType = mBgTypeNear;
      setupParam.farType = mBgTypeFar;
      mBgSystem->StartLoad(setupParam);

      mMainSeq = SEQ_LOAD_BG;
    }
    else
    if( btn->IsTrigger( gfl2::ui::BUTTON_START ) )
    {
      return gfl2::proc::RES_FINISH;
    }
    break;

  case SEQ_LOAD_BG:
    if( mBgSystem->IsFinishLoad() )
    {
      mBgSystem->Create();
      mMainSeq = SEQ_MAIN;
    }
    break;

  case SEQ_MAIN:
    UpdateCamera();
    if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      mBgSystem->Delete();
      mMainSeq = SEQ_WAIT_DELETE;
    }

    if ( btn->IsTrigger( gfl2::ui::BUTTONID_UP))
    {
      DebugTime++;
      DebugTime %= 24;
    }
    else
    {    
      if ( btn->IsTrigger( gfl2::ui::BUTTONID_DOWN))
      {
        DebugTime = (DebugTime<=0) ? 24 : DebugTime;
        DebugTime--;
      }
    }

    {//天球
      gfl2::math::Vector3 pos(0,200,0);
      s32 t_time = DebugTime*60*60;
      //mpAppParam->mpSkybox->Update( pos, (t_time/60/60), (t_time/60)%60, (t_time%60));
      mBgSystem->UpdateSkybox( pos, (t_time/60/60), (t_time/60)%60, (t_time%60));
    }
    break;

  case SEQ_WAIT_DELETE:
    if( mBgSystem->IsFinishDelete() )
    {
      mMainSeq = SEQ_WAIT;
    }
    break;
  }

  //BG更新
  mBgSystem->Update(btl::BgSystem::PROC_ORDER_SKYBOX_BG);
  gfl2::renderingengine::renderer::RenderingPipeLine::Update();
#endif
  return gfl2::proc::RES_CONTINUE;
}

/**
*/
void AnaiTestProc::PreDrawFunc (gfl2::proc::Manager* pManager)
{
#if 0
  //天球
  mBgSystem->PreDrawSkybox();
#endif
}


/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void AnaiTestProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
#if 0
  //天球
  mBgSystem->DrawSkybox(displayNo);


  // @todo 立体視の場合はPreDrawFuncでコマンドをセーブし、DrawFuncで実行する形にする 150218 ikuta_junya
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

      gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	

      gfl2::renderingengine::renderer::RenderingPipeLine::SetRenderTarget( pBackBuffer );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetDepthStencil( pDepthBuffer );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetDisplaySize( displayWidth, displayHeight );

      gfl2::renderingengine::renderer::RenderingPipeLine::Execute();
    } 
    break;

    // 右画面描画(立体視時のみ)
  case gfl2::gfx::CtrDisplayNo::RIGHT:
    break;

    // 下画面描画
  case gfl2::gfx::CtrDisplayNo::DOWN:
    break;
  } // switch( displayNo )
#endif
}


/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result AnaiTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}



/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result AnaiTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
#if 0
  switch( mEndSeq )
  {
  case 0:
    {
      mBgSystem->Terminate();
      mEndSeq++;
    }
    break;

  case 1:
    if( mBgSystem->IsFinishTerminate() )
    {
      DeleteDebugMenu();
      GFL_SAFE_DELETE(mBgSystem);

      //@todo
      gfl2::gfx::GLMemory::SetAllocator(mOldAllocator);
      GFL_SAFE_DELETE( mGLAllocator );

      GFL_DELETE_HEAP(mHeap);
      return gfl2::proc::RES_FINISH;
    }
    break;
  }
#endif
  return gfl2::proc::RES_CONTINUE;
}


//============================================================================
//RenderingPipeLine
//============================================================================
//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 AnaiTestProc::StartRenderPath(void)
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
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* AnaiTestProc::GetRenderPath(void)
{
  switch( mRenderPathCnt )
  {
  case 0:
    return mBgSystem->GetRenderPath();
    //break;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 AnaiTestProc::NextRenderPath(void)
{
  mRenderPathCnt++;
  if( mRenderPathCnt >= 1 )
  {
    return false;
  }
  return true;
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

void AnaiTestProc::UpdateCamera(void)
{
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

  if( btn->IsHold(gfl2::ui::BUTTON_R) )
  {
    //カメラ視点操作
    mCamRotHorizonal += tempRotH;
    mCamRotVertical = gfl2::math::Clamp( mCamRotVertical-tempRotV ,-89.0f,89.0f );
    UpdateCameraMatrix();
  }
  else
  {
    //カメラ位置操作
    if( tempRotV != 0 )
    {
      gfl2::math::Vector3 tempDir( 0,0,-tempRotV*10 );
      Rot_Deg( tempDir.x , tempDir.z , mCamRotHorizonal );
      mCamPos += tempDir;
    }
    if( tempRotH != 0 )
    {
      gfl2::math::Vector3 tempDir( tempRotH*10,0,0 );
      Rot_Deg( tempDir.x , tempDir.z , mCamRotHorizonal );
      mCamPos += tempDir;
    }
    UpdateCameraMatrix();
  }

  if( btn->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    SetCameraPosTarget(gfl2::math::Vector3(256.0f,106.0f,465.0f),gfl2::math::Vector3(-45.0f,70.0f,-20.0f));
    UpdateCameraMatrix();
  }

  if( uiKey->IsHold(gfl2::ui::DIR_DOWN) )
  {
    mCamPos.y -= 5.0f;
    UpdateCameraMatrix();
  }
  else
  if( uiKey->IsHold(gfl2::ui::DIR_UP) )
  {
    mCamPos.y += 5.0f;
    UpdateCameraMatrix();
  }

}

void AnaiTestProc::SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg)
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

void AnaiTestProc::UpdateCameraMatrix(void)
{
  gfl2::math::Vector3 camTrg( 0,0,-10.0f );
    
  Rot_Deg( camTrg.z , camTrg.y , mCamRotVertical );
  Rot_Deg( camTrg.x , camTrg.z , mCamRotHorizonal );

  camTrg +=mCamPos;

  {
    // ビュー変換行列
    gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
    mCamViewMtx = gfl2::math::Matrix34::GetLookAt(mCamPos, camTrg, upDirection);
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
}



void AnaiTestProc::DeleteDebugMenu(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"AnaiTestProc");
}

void BgViewer_DebugUpdate_StartAnime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  AnaiTestProc *work = static_cast<AnaiTestProc*>(userWork);
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);

  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    work->GetBgSystem()->PlayAnime((btl::BgSystem::BG_ANM_TYPE)work->mAnimeType);
  }
}

void BgViewer_DebugUpdate_StopAnime( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  AnaiTestProc *work = static_cast<AnaiTestProc*>(userWork);
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);

  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    work->GetBgSystem()->StopAnime((btl::BgSystem::BG_ANM_TYPE)work->mAnimeType);
  }
}


void AnaiTestProc::CreateDebugMenu(void)
{
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"AnaiTestProc",NULL);
  gfl2::str::MsgData *debugMsg = GFL_NEW( mHeap->GetLowerHandle() ) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_anai_DAT, mHeap );


  gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Ground",&mBgTypeGround,0,100);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Near",&mBgTypeNear,0,100);
  gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Far",&mBgTypeFar,0,100);
  //gfl2::debug::DebugWin_AddItemValueIntMsg( topGroup , mHeap , L"AnimeType" ,  &mAnimeType , print::GetMessageArcId_Debug(), GARC_debug_message_d_anai_DAT , 0 , btl::BgSystem::ANM_MAX , AriDebug_BgViewAnmType01 );

  gfl2::debug::DebugWin_AddItemUpdateFunc(topGroup,mHeap,L"PlayAnime",this,BgViewer_DebugUpdate_StartAnime,NULL);
  gfl2::debug::DebugWin_AddItemUpdateFunc(topGroup,mHeap,L"StopAnime",this,BgViewer_DebugUpdate_StopAnime,NULL);

  GFL_SAFE_DELETE( debugMsg );
}

GFL_NAMESPACE_END(Test)
GFL_NAMESPACE_END(AnaiTest)


#endif
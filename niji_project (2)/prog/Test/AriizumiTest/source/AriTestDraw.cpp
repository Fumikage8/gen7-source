//======================================================================
/**
 * @file  AriTestDraw.cpp
 * @brief 15/04/20
 * @author  ariizumi
 * @data  描画テスト
 */
//======================================================================
#if PM_DEBUG

#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <str/include/gfl2_MsgData.h>
#include <math/include/gfl2_MathTriangular.h>

#include <System/include/HeapDefine.h>
#include <Print/include/PrintSystem.h>
#include <Battle/Background/include/BgSystem.h>

#include "System/include/GflUse.h"

#include <Test/AriizumiTest/include/AriTestDraw.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/debug_message.gaix>
#include <arc_index/NijiRenderPath.gaix>
#include <arc_index/wazaeff_resource.gaix>
#include <niji_conv_header/message/debug/msg_d_ariizumi.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

AriTestDraw::AriTestDraw()
:mEffRenderPath()
{
  mInitSeq = 0;
  mMainSeq = 0;
  mEndSeq = 0;
}

AriTestDraw::~AriTestDraw(void)
{

}

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result AriTestDraw::InitFunc(gfl2::proc::Manager* pManager)
{
  switch( mInitSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

      //0x1C00000
      mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1000000,gfl2::heap::HEAP_TYPE_EXP,false,"BgViewer");
      mAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);

      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_WAZAEFF_RESOURCE;
      openReq.heapForFile = mHeap;
      openReq.heapForReq = mHeap->GetLowerHandle();
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileOpenReq(openReq);

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      loadReq.arcId = ARCID_WAZAEFF_RESOURCE;
      loadReq.align = 128;
      loadReq.heapForBuf = mHeap;
      loadReq.heapForReq = mHeap->GetLowerHandle();
      loadReq.heapForCompressed = mHeap->GetLowerHandle();
 
      loadReq.datId = GARC_wazaeff_resource_ew463_volcano_lz_BPTCL;
      loadReq.ppBuf = &mEffRes;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);

      mInitSeq++;
    }
    break;
  case 1:
    if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_WAZAEFF_RESOURCE) &&
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mEffRes) )
    {
      {

        mEffectHeap = GFL_NEW(mHeap)gfl2::Effect::GFHeap(mHeap);

        gfl2::Effect::Config config;
        config.SetEffectHeap( mEffectHeap );
#ifdef GF_PLATFORM_CTR
        config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif //GF_PLATFORM_CTR
        mEffectSystem = GFL_NEW(mHeap)gfl2::Effect::System(config);

        mEffectGroupIDList = GFL_NEW(mHeap) gfl2::util::List<u32>( mHeap,1);
        mEffectGroupIDList->PushFront(0);
      }
      {
        gfl2::Effect::EffectRenderPath::Config config;
        config.m_pSystem        = mEffectSystem;
        config.m_cpGroupIDList  = mEffectGroupIDList;
        mEffRenderPath.SetConfig(config);
      }
      {
        mEffectSystem->EntryResource( mEffectHeap , mEffRes , 0 );
        mEffNum = mEffectSystem->GetResource(0)->GetNumEmitterSet();

        mEffectHandle = GFL_NEW_ARRAY(mEffectHeap->GetGFHeapBase())gfl2::Effect::Handle*[mEffNum];
        for( int i=0;i<mEffNum;i++ )
        {
          mEffectHandle[i] = GFL_NEW(mEffectHeap->GetGFHeapBase())gfl2::Effect::Handle();
          mEffectSystem->CreateEmitterSetID( mEffectHandle[i] , gfl2::math::Vector3(0.0f,0.0f,0.0f) , i , 0 );
        }
      }


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

      mCamRotVertical = 0;
      mCamRotHorizonal = 0;
      mCamPos.Set(0,0,0);

      SetCameraPosTarget(gfl2::math::Vector3(0.0f,0.0f,200.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f));
      UpdateCameraMatrix();
      gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	

      mInitSeq++;
    }
    break;

  case 2:
    return gfl2::proc::RES_FINISH;
    //break;//スルー
  }
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result AriTestDraw::EndFunc(gfl2::proc::Manager* pManager)
{
  switch( mEndSeq )
  {
  case 0:
    {
      for( int i=0;i<mEffNum;i++ )
      {
        mEffectHandle[i]->GetEmitterSet()->Kill();
        GFL_SAFE_DELETE( mEffectHandle[i] );
      }
      GFL_SAFE_DELETE_ARRAY( mEffectHandle );
      mEffectSystem->ClearResource( mEffectHeap, 0);

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_WAZAEFF_RESOURCE;
      closeReq.heapForReq = mHeap->GetLowerHandle();

      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileCloseReq(closeReq);

      mEndSeq++;
    }
    break;
  case 1:
    if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_WAZAEFF_RESOURCE) )
    {
      GflHeapFreeMemory(mEffRes);

      GFL_SAFE_DELETE(mEffectGroupIDList);
      GFL_SAFE_DELETE(mEffectSystem);
      GFL_SAFE_DELETE(mEffectHeap);

      GFL_SAFE_DELETE(mAllocator);
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
gfl2::proc::Result AriTestDraw::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  
  UpdateCamera();

  gfl2::renderingengine::renderer::RenderingPipeLine::Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

  mEffectSystem->BeginFrame();
  mEffectSystem->Calc(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void AriTestDraw::PreDrawFunc( gfl2::proc::Manager* pManager )
{

}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void AriTestDraw::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
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

//============================================================================
//RenderingPipeLine
//============================================================================
//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 AriTestDraw::StartRenderPath(void)
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
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* AriTestDraw::GetRenderPath(void)
{
  switch( mRenderPathCnt )
  {
  case 0:
    return &mEffRenderPath;
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
b32 AriTestDraw::NextRenderPath(void)
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

void AriTestDraw::UpdateCamera(void)
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

void AriTestDraw::SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg)
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

void AriTestDraw::UpdateCameraMatrix(void)
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

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif//PM_DEBUG

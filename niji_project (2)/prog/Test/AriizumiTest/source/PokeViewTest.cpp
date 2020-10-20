//======================================================================
/**
 * @file  PokeViewTest.cpp
 * @brief 15/04/20
 * @author  ariizumi
 * @data  描画テスト
 */
//======================================================================
#if PM_DEBUG

#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <str/include/gfl2_MsgData.h>
#include <math/include/gfl2_MathTriangular.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>

#include <System/include/HeapDefine.h>
#include <Print/include/PrintSystem.h>
#include <Battle/Background/include/BgSystem.h>
#include <util/include/gfl2_FnvHash.h>

#include "System/include/GflUse.h"
#include "PokeTool/source/PokeModelLocal.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include <Test/AriizumiTest/include/PokeViewTest.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/debug_message.gaix>
#include <arc_index/NijiRenderPath.gaix>
#include <niji_conv_header/message/debug/msg_d_ariizumi.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

#define POKEVIEW_PRINT GFL_PRINT

enum
{
  PRINTMODE_MEMORY,
  PRINTMODE_SETTINGXML,
  PRINTMODE_RESOURCE_SIZE,
  PRINTMODE_GRP_VALUE,

  PRINTMODE_MAX,
};

PokeViewTest::PokeViewTest()
{
}

PokeViewTest::~PokeViewTest(void)
{

}

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result PokeViewTest::InitFunc(gfl2::proc::Manager* pManager)
{
  switch( mInitSeq )
  {
  case 0:
    {
      gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

      //0x1C00000
      mHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x2000000,gfl2::heap::HEAP_TYPE_EXP,false,"PokeViewer");
      mAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);
      mFPHeap = GFL_CREATE_LOCAL_HEAP_NAME(mHeap,0x100000,gfl2::heap::HEAP_TYPE_EXP,false,"PokeViewer");

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
      mMotionIdx = 0;

      SetCameraPosTarget(gfl2::math::Vector3(0.0f,0.0f,200.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f));
      UpdateCameraMatrix();
      gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
      gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	

      mInitSeq++;
    }
    break;

  case 1:
    {
      //各種ARC初期化
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_RENDERPATH;
      openReq.heapForFile = mHeap;
      openReq.heapForReq = mHeap->GetLowerHandle();
      openReq.hioUseSetting       = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
      openReq.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileOpenReq(openReq);

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      loadReq.arcId = ARCID_RENDERPATH;
      loadReq.align = 128;
      loadReq.heapForBuf = mHeap;
      loadReq.heapForReq = mHeap->GetLowerHandle();
 
      loadReq.datId = GARC_NijiRenderPath_EdgeMapResource_BIN;
      loadReq.ppBuf = &mEdgeMapRenderPathRes;
      loadReq.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);

      loadReq.datId = GARC_NijiRenderPath_OutLineResource_BIN;
      loadReq.ppBuf = &mOutLineRenderPathRes;
      loadReq.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);

      loadReq.datId = GARC_NijiRenderPath_FullPower_BIN;
      loadReq.ppBuf = &mFullPowerRenderPathRes;
      loadReq.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileLoadDataReq(loadReq);

      mInitSeq++;
    }
    break;
  case 2:
    if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileOpenFinished(ARCID_RENDERPATH) == false ||
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mEdgeMapRenderPathRes) == false ||
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mOutLineRenderPathRes) == false ||
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileLoadDataFinished(&mFullPowerRenderPathRes) == false )
    {
      return gfl2::proc::RES_CONTINUE;
    }
    {
      //RenderPath周り初期化
      mEdgeMapPath = GFL_NEW(mHeap)System::NijiEdgeMapSceneRenderPath(mAllocator,32);
      mEdgeMapPath->SetResource(mEdgeMapRenderPathRes);
      mEdgeMapPath->CreateEdgeMapTexture( mAllocator,512,256 );

      mOutLinePath = GFL_NEW(mHeap)System::NijiOutLinePostSceneRenderPath(mAllocator);
      mOutLinePath->SetResource(mOutLineRenderPathRes);

      for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
      {
        const gfl2::gfx::Texture *tex = mEdgeMapPath->GetEdgeMapTexture( i );
        mOutLinePath->SetEdgeMapTexture(i,tex);
      }

      mModelRenderPath = GFL_NEW(mHeap)System::ModelRenderPath(mAllocator,32);
      mIdModelRenderPath = GFL_NEW(mHeap)System::ModelRenderPath(mAllocator,32);

#if USE_AURA_SHADER
      mFullPowerEffectRenderPath0 = GFL_NEW(mHeap)FullPowerEffectRenderPath0(mAllocator);
      mFullPowerEffectRenderPath1 = GFL_NEW(mHeap)FullPowerEffectRenderPath1(mAllocator);
#endif //USE_AURA_SHADER
      {
        // メインとなるシーン描画パスを初期化
        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize	  = 64;
        desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

        mModelRenderPath->Initialize( mAllocator,desc );
        mIdModelRenderPath->Initialize( mAllocator,desc );
        mEdgeMapPath->Initialize( mAllocator,desc );
#if USE_AURA_SHADER
        mFullPowerEffectRenderPath0->Initialize( mAllocator,desc );
        mFullPowerEffectRenderPath0->SetResource( mAllocator,mFullPowerRenderPathRes );
        mFullPowerEffectRenderPath1->Initialize( mAllocator,desc );
        mFullPowerEffectRenderPath1->SetResource( mAllocator,mFullPowerRenderPathRes );
#endif //USE_AURA_SHADER
      }
      //歪み
      mStretchBltFrameBufferPath = GFL_NEW(mHeap)gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath(); 
      mDistortionPostRenderPath = GFL_NEW(mHeap)poke_3d::renderer::DistortionPostRenderPath();
      //歪み用
      {
        gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
        mStretchBltFrameBufferPath->CreateFrameBufferTexture(mAllocator,pBackBuffer);

        gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
        desc.m_DrawManagerDesc.m_NodeStackSize	  = 4;
        desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
        mDistortionPostRenderPath->Initialize(mAllocator, desc);
        mDistortionPostRenderPath->CreateBuffer(mAllocator, 4);
        mDistortionPostRenderPath->SetOverrideTexture( mStretchBltFrameBufferPath->GetFrameBufferTexture() );
      }

      //PokeModelSystem作成
      mHeap->SetDebugUserID(100);
      mPokeModelSys = GFL_NEW(mHeap)PokeTool::PokeModelSystem();
      mPokeModelSys->InitSystemAsync(mHeap,mHeap,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),POKE_NUM);
      mHeap->SetDebugUserID(0);
      mInitSeq++;
    }
    //break;//スルー
  case 3:
    if( mPokeModelSys->IsFinishInitSystemAsync() == false )
    {
      return gfl2::proc::RES_CONTINUE;
    }

    //PokeModelSystem初期化
    mHeap->SetDebugUserID(100);
    mPokeModelSys->CreateSystem(mAllocator,true,true);

    //管理ヒープ作成
    PokeTool::PokeModelSystem::HeapOption opt;
    opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
    opt.useIdModel = true;
    opt.useShadow = true;
    opt.useColorShader = true;
    mHeap->SetDebugUserID(101);
    mPokeModelSys->CreatePokeModelHeap(mHeap,&opt);
    mHeap->SetDebugUserID(0);

    mMainSeq = 0;
    mMonsNo = 25;
    mFormNo = 0;
    mAnimeType = 0;
    mPokeIdx = 0;
    mMotionIdx = 0;
    mEyeIdx = -1;
    mMouthIdx = -1;
    mDispIdModel = false;
    InitDebug();

#if USE_AURA_SHADER
    // 角度、注視点、注視点からの距離
    mSampleCamera.Initialize(gfl2::math::Vector3( 0,0,0 ), gfl2::math::Vector3( 0,64,0 ), 512.0f );	
#endif //USE_AURA_SHADER
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
gfl2::proc::Result PokeViewTest::EndFunc(gfl2::proc::Manager* pManager)
{
  switch( mEndSeq )
  {
  case 0:
    {
      TermDebug();
      //ポケモデルシステム破棄
      mPokeModelSys->DeletePokeModelHeap();
      mPokeModelSys->TermSystemAsync();

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_RENDERPATH;
      closeReq.heapForReq = mHeap->GetLowerHandle();

      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileCloseReq(closeReq);
      mEndSeq++;
    }
  case 1:
    if( mPokeModelSys->IsFinishTermSystemAsync()&&
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_RENDERPATH) )
    {
      GFL_SAFE_DELETE(mPokeModelSys);
      GFL_SAFE_DELETE(mOutLinePath);
      GFL_SAFE_DELETE(mEdgeMapPath);
      GFL_SAFE_DELETE(mIdModelRenderPath);
      GFL_SAFE_DELETE(mModelRenderPath);
      GFL_SAFE_DELETE(mDistortionPostRenderPath);
      GFL_SAFE_DELETE(mStretchBltFrameBufferPath);
#if USE_AURA_SHADER
      GFL_SAFE_DELETE(mFullPowerEffectRenderPath1);
      GFL_SAFE_DELETE(mFullPowerEffectRenderPath0);
#endif //USE_AURA_SHADER
      GflHeapFreeMemory(mFullPowerRenderPathRes);
      GflHeapFreeMemory(mEdgeMapRenderPathRes);
      GflHeapFreeMemory(mOutLineRenderPathRes);

      GFL_DELETE_HEAP(mFPHeap);
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
gfl2::proc::Result PokeViewTest::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  
  if( gfl2::debug::DebugWin_IsOpen() == false )
  {
#if USE_AURA_SHADER
    UpdateCameraSample();
#else
    UpdateCamera();
#endif
  }

  gfl2::renderingengine::renderer::RenderingPipeLine::SetProjectionMatrix( mProjMtx[0] );
  gfl2::renderingengine::renderer::RenderingPipeLine::SetViewMatrix( mViewMtx[0] );	
  {
    // ビルボードテスト
    gfl2::math::Matrix34 billboardYMtx;
#if USE_AURA_SHADER
    mSampleCamera.GetYBillboardMtx( &billboardYMtx );
    //mFullPowerEffectRenderPath1->SetYBillboardMtx( &billboardYMtx );
#endif
  }

  //ポケモデルシステム更新
  mPokeModelSys->Update();

  gfl2::renderingengine::renderer::RenderingPipeLine::Update();
  //モデル計算
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

  for( int i=0;i<POKE_NUM;i++)
  {
    if( mPokeModel[i] != NULL && 
      mIsCreateModel[mPokeIdx] == true &&
      mPokeModel[i]->IsEnableInterest() )
    {
      if( mIsLook )
      {
        mPokeModel[i]->SetInterestPosition(&mLookPos);
      }
      else
      {
        mPokeModel[i]->SetInterestPosition(NULL);
      }
      mPokeModel[i]->UpdateInterest();
    }
  }

  enum
  {
    SEQ_WAIT,
    SEQ_LOAD,
    SEQ_DISP,
    SEQ_TERM,

    SEQ_AUTO_START,
    SEQ_AUTO_LOAD,
    SEQ_AUTO_DISP,
    SEQ_AUTO_TERM,
    SEQ_AUTO_GO_NEXT,
  };

  switch( mMainSeq )
  {
  case SEQ_WAIT:
    if( gfl2::debug::DebugWin_IsOpen() == false )
    {
      if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        //ポケモデル作成開始
        PokeTool::PokeModel::SetupOption opt;
        opt.dataHeap = NULL;
        opt.workHeap = mHeap;
        opt.useShadow = mLoadShadow;
        opt.useIdModel = mLoadIdModel;
        opt.animeType = static_cast<PokeTool::MODEL_ANIMETYPE>(mAnimeType);

        PokeTool::SimpleParam pokeParam;
        pokeParam.monsNo = (MonsNo)mMonsNo;
        pokeParam.formNo = mFormNo;
        pokeParam.sex    = (pml::Sex)mSex;
        pokeParam.isEgg  = false;
        pokeParam.isRare = mRare;
        pokeParam.perRand = (mRandUp*65536)+mRandDown;

        mHeap->SetDebugUserID(102);
        mPokeModel[mPokeIdx] = mPokeModelSys->CreateModelSimple(&pokeParam,opt);
        mHeap->SetDebugUserID(0);
        mMainSeq = SEQ_LOAD;
      }
      if( btn->IsTrigger( gfl2::ui::BUTTON_START ) )
      {
        return gfl2::proc::RES_FINISH;
      }
      if( mActiveAutoTest == true )
      {
        mMainSeq = SEQ_AUTO_START;
      }
    }
    break;

  case SEQ_LOAD:
    if( mPokeModel[mPokeIdx]->IsFinishLoad() == true )
    {
      mPokeModelSys->GetPokeModelHeap(0)->Dump();
      mPokeModelSys->GetPokeModelHeap(0)->SetDebugModeFlg(true);
      mHeap->SetDebugUserID(103);
      //ポケモデル作成
      mPokeModel[mPokeIdx]->Create();
      //カラーシェーダーセットアップ
      mPokeModel[mPokeIdx]->SetupColorShader();
      mHeap->SetDebugUserID(0);
      if( mDispIdModel == false )
      {
        mModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetModelInstanceNode());
        mEdgeMapPath->AddEdgeRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
      }
      else
      {
        if( mPokeModel[mPokeIdx]->GetIdModel() )
        {
          mIdModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetIdModel()->GetModelInstanceNode());
          mIdModelRenderPath->AddDrawEnv(mPokeModel[mPokeIdx]->GetIdModel()->GetDrawEnvNode());
        }
      }
      if( mPokeModel[mPokeIdx]->GetShadowModel() )
      {
        mModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetShadowModel()->GetModelInstanceNode());
      }
      if( mPokeModel[mPokeIdx]->GetDistortionModel() )
      {
        mDistortionPostRenderPath->AddDistortionRenderingTarget(mPokeModel[mPokeIdx]->GetDistortionModel()->GetModelInstanceNode());
      }

      mModelRenderPath->AddDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());
      mDistortionPostRenderPath->AddDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());

      mPokeModel[mPokeIdx]->ChangeAnimationDirect(0);
      mPokeModel[mPokeIdx]->SetAutoBlink(false);
      mIsCreateModel[mPokeIdx] = true;

      ResetDebugParam();
      AddFullPowerPath();

      mMainSeq = SEQ_DISP;
    }
    break;

  case SEQ_DISP:
    {
      if( mReserveReloadFullPower == true )
      {
        if( mReserveReloadFullPowerCnt > 0 )
        {
          mReserveReloadFullPowerCnt--;
        }
        else
        {
          DeleteFullPowerPath();
          AddFullPowerPath();
          mReserveReloadFullPower = false;
        }
      }
    }
    if( gfl2::debug::DebugWin_IsOpen() == false )
    {
      if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        //カラーシェーダー破棄
        mPokeModel[mPokeIdx]->CleanColorShader();
        mModelRenderPath->RemoveDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());
        mDistortionPostRenderPath->RemoveDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());
        if( mDispIdModel == false )
        {
          mModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetModelInstanceNode());
          mEdgeMapPath->RemoveEdgeRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
        }
        else
        {
          if( mPokeModel[mPokeIdx]->GetIdModel() )
          {
            mIdModelRenderPath->RemoveDrawEnv(mPokeModel[mPokeIdx]->GetIdModel()->GetDrawEnvNode());
            mIdModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetIdModel()->GetModelInstanceNode());
          }
        }
        if( mPokeModel[mPokeIdx]->GetShadowModel() )
        {
          mModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetShadowModel()->GetModelInstanceNode());
        }
        if( mPokeModel[mPokeIdx]->GetDistortionModel() )
        {
          mDistortionPostRenderPath->RemoveDistortionRenderingTarget(mPokeModel[mPokeIdx]->GetDistortionModel()->GetModelInstanceNode());
        }

        RemoveFullPowerPath();

        mIsCreateModel[mPokeIdx] = false;
        mMainSeq = SEQ_TERM;
      }
    }
    break;
  case SEQ_TERM:
    if( mPokeModel[mPokeIdx]->CanDelete() )
    {
      DeleteFullPowerPath();
      //モデル破棄
      mPokeModelSys->DeleteModel(mPokeModel[mPokeIdx]);
      mPokeModel[mPokeIdx] = NULL;
      mMainSeq = SEQ_WAIT;
    }
    break;
    //自動テスト
  case SEQ_AUTO_START:
    {
      if( mMonsNo < mStartMonsNo )
      {
        mMonsNo = mStartMonsNo;
      }

      //ポケモデル作成開始
      PokeTool::PokeModel::SetupOption opt;
      opt.dataHeap = NULL;
      opt.workHeap = mHeap;
      opt.useShadow = mLoadShadow;
      opt.useIdModel = mLoadIdModel;
      opt.animeType = static_cast<PokeTool::MODEL_ANIMETYPE>(mAnimeType);

      PokeTool::SimpleParam pokeParam;
      pokeParam.monsNo = (MonsNo)mMonsNo;
      pokeParam.formNo = mFormNo;
      pokeParam.sex    = (pml::Sex)mSex;
      pokeParam.isEgg  = false;
      pokeParam.isRare = mRare;
      pokeParam.perRand = (mRandUp*65536)+mRandDown;

      if( mPrintMode == PRINTMODE_RESOURCE_SIZE )
      {
        POKEVIEW_PRINT( "%03d,%02d,%c,",mMonsNo,mFormNo,(mSex==PM_MALE?'M':'F') );

        gfl2::fs::AsyncFileManager *fileMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();

        const int dataIdx = mPokeModelSys->GetDataIdx( pokeParam.monsNo , pokeParam.formNo , pokeParam.sex );
        const int arcTop = dataIdx*PokeTool::BINTYPE_MAX + 1;

        for( int i=0;i<PokeTool::BINTYPE_MAX;i++ )
        {
          size_t size;
          fileMng->GetArcFile(ARCID_POKEMON_MODEL)->GetDataSize(&size,arcTop+i,mHeap);
          POKEVIEW_PRINT( "%d,",size );
        }
        for( int i=0;i<PokeTool::BINTYPE_MAX;i++ )
        {
          void *dataBuf;
          size_t size;
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
          req.arcId = ARCID_POKEMON_MODEL;
          req.align = 128;
          req.heapForBuf = mHeap;
          req.heapForReq = mHeap;
          req.heapForCompressed = mHeap->GetLowerHandle();
          req.datId = arcTop+i;
          req.ppBuf = &dataBuf;
          req.pRealReadSize = &size;
          fileMng->SyncArcFileLoadData(req);

          POKEVIEW_PRINT( "%d,",size );
          GflHeapSafeFreeMemory(dataBuf);
        }
        POKEVIEW_PRINT( "\n");

        mMainSeq = SEQ_AUTO_GO_NEXT;
      }
      else
      {
        mDrawCheckTagNum = mModelRenderPath->GetDrawManager()->GetVisibleTagNum();
        mDrawCheckUseBuffer = 0;
#if USE_DEBUG_WIN
        mDrawCheckUseBuffer = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferUseSize;
#endif  //プラットフォーム分岐

        mLoadStartHeap = mPokeModelSys->GetPokeModelHeap(0)->GetTotalFreeSize();

        mHeap->SetDebugUserID(102);
        mPokeModel[mPokeIdx] = mPokeModelSys->CreateModelSimple(&pokeParam,opt);
        mHeap->SetDebugUserID(0);
        mMainSeq = SEQ_AUTO_LOAD;
      }
    }
    break;
  case SEQ_AUTO_LOAD:
    if( mPokeModel[mPokeIdx]->IsFinishLoad() == true )
    {
      if( mPrintMode != -1 )
      {
        POKEVIEW_PRINT( "%03d,%02d,%c,",mMonsNo,mFormNo,(mSex==PM_MALE?'M':'F') );
      }
      int fileHeap = mPokeModelSys->GetPokeModelHeap(0)->GetTotalFreeSize();
      mHeap->SetDebugUserID(103);
      //ポケモデル作成
      mPokeModel[mPokeIdx]->Create();
      mPokeModel[mPokeIdx]->SetStencilReferenceOffset(1);
      //カラーシェーダーセットアップ
      mPokeModel[mPokeIdx]->SetupColorShader();
      mHeap->SetDebugUserID(0);
      if( mDispIdModel == false )
      {
        mModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetModelInstanceNode());
        mEdgeMapPath->AddEdgeRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
      }
      else
      {
        if( mPokeModel[mPokeIdx]->GetIdModel() )
        {
          mIdModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetIdModel()->GetModelInstanceNode());
          mIdModelRenderPath->AddDrawEnv(mPokeModel[mPokeIdx]->GetIdModel()->GetDrawEnvNode());
        }
      }
      if( mPokeModel[mPokeIdx]->GetShadowModel() )
      {
        mModelRenderPath->AddNode(mPokeModel[mPokeIdx]->GetShadowModel()->GetModelInstanceNode());
      }
      mModelRenderPath->AddDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());

      int loadHeap = mPokeModelSys->GetPokeModelHeap(0)->GetTotalFreeSize();
      mPokeModel[mPokeIdx]->ChangeAnimationDirect(0);
      int motHeap = mPokeModelSys->GetPokeModelHeap(0)->GetTotalFreeSize();
      mPokeModel[mPokeIdx]->ResetFaceAnime_Eye();
      int faceHeap = mPokeModelSys->GetPokeModelHeap(0)->GetTotalFreeSize();

      int fpHeap = mFPHeap->GetTotalFreeSize();
      if( mAutoEnableFullPower )
      {
        ResetDebugParam();
        AddFullPowerPath();
      }
      int fpHeapAft = mFPHeap->GetTotalFreeSize();

      if( mPrintMode == PRINTMODE_MEMORY)
      {
        POKEVIEW_PRINT( "%d,%d,%d,%d,%d,%d,",mLoadStartHeap,fileHeap,loadHeap,motHeap,faceHeap,fpHeap-fpHeapAft );
      }
      mIsCreateModel[mPokeIdx] = true;

      mWaitCnt = 0;
      mMainSeq = SEQ_AUTO_DISP;
    }
    break;
  case SEQ_AUTO_DISP:
    mWaitCnt++;
    if( mWaitCnt > 3 )
    {
      if( mPrintMode == PRINTMODE_GRP_VALUE )
      {
        int tagNum,useBuf,useReq;

        tagNum = mModelRenderPath->GetDrawManager()->GetVisibleTagNum();
        useBuf = 0;
#if USE_DEBUG_WIN
        useBuf = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferUseSize;
#endif //USE_DEBUG_WIN
        POKEVIEW_PRINT("%d,%d,%d,",tagNum-mDrawCheckTagNum,mDrawCheckUseBuffer,useBuf);
      }

      if( mAutoEnableFullPower )
      {
        RemoveFullPowerPath();
      }

      //カラーシェーダー破棄
      mPokeModel[mPokeIdx]->CleanColorShader();
      mModelRenderPath->RemoveDrawEnv(mPokeModel[mPokeIdx]->GetDrawEnvNode());
      if( mDispIdModel == false )
      {
        mModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetModelInstanceNode());
        mEdgeMapPath->RemoveEdgeRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
      }
      else
      {
        if( mPokeModel[mPokeIdx]->GetIdModel() )
        {
          mIdModelRenderPath->RemoveDrawEnv(mPokeModel[mPokeIdx]->GetIdModel()->GetDrawEnvNode());
          mIdModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetIdModel()->GetModelInstanceNode());
        }
      }
      if( mPokeModel[mPokeIdx]->GetShadowModel() )
      {
        mModelRenderPath->RemoveNode(mPokeModel[mPokeIdx]->GetShadowModel()->GetModelInstanceNode());
      }
      mIsCreateModel[mPokeIdx] = false;

      if( mPrintMode == PRINTMODE_SETTINGXML )
      {
        const PokeTool::PokeSettingData *settingData = mPokeModel[mPokeIdx]->GetSettingData();
        char sizeStr[3]={'S','M','L'};
        POKEVIEW_PRINT( "%d,%d,%c,%f,%f,%f,%f,%f,%d,%d,",settingData->cmHeight,settingData->adjustHeight,sizeStr[settingData->size]
                                                        ,settingData->camSize,settingData->camYOffset,settingData->camRotate,settingData->camRotateRightOfs,settingData->camXRotate
                                                        ,settingData->waitBRate,settingData->waitCRate);
        for( int i=0;i<8;i++ )
        {
          POKEVIEW_PRINT("%c",(settingData->eyeUsedFlags[i]?'T':'F'));
        }
        POKEVIEW_PRINT(",");
        for( int i=0;i<8;i++ )
        {
          POKEVIEW_PRINT("%c",(settingData->mouthUsedFlags[i]?'T':'F'));
        }
        POKEVIEW_PRINT(",");

        bool ret;
        gfl2::math::AABB box;
        ret = mPokeModel[mPokeIdx]->GetMotionCollision( &box );
        if( ret )
        {
          POKEVIEW_PRINT("%f,%f,%f,%f,%f,%f,",box.GetMin().x,box.GetMin().y,box.GetMin().z,box.GetMax().x,box.GetMax().y,box.GetMax().z);
        }
        else
        {
          POKEVIEW_PRINT( "---,---,---,---,---,---,");
        }

        const int NodeCheckNum[PokeTool::MODEL_NODE_MAX] =
        {
          1,//  MODEL_NODE_HEAD,      ///頭(0～複数
          1,//  MODEL_NODE_OVERHEAD,  ///頭の上(単一・必須)
          2,//  MODEL_NODE_EYE,       ///目(0～複数
          1,//  MODEL_NODE_MOUTH,     ///口(0～複数
          1,//  MODEL_NODE_HORN,      ///角(0～複数
          1,//  MODEL_NODE_CENTER,    ///中心(単一・必須)
          1,//  MODEL_NODE_FRONT,     ///前面(単一・必須)
          2,//  MODEL_NODE_HAND,      ///手(0～複数
          1,//  MODEL_NODE_TAIL,      ///尾(0～複数
          2,//  MODEL_NODE_FOOT,      ///足(0～複数
          1,//  MODEL_NODE_ATTACK01,  ///物理攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_ATTACK02,  ///物理攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_ATTACK03,  ///物理攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_ATTACK04,  ///物理攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_SHOOT01,   ///特殊攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_SHOOT02,   ///特殊攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_SHOOT03,   ///特殊攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_SHOOT04,   ///特殊攻撃点(対応するモーションがあれば1～複数
          1,//  MODEL_NODE_WIDEUSE,   ///その他用(0～複数
        };

        for( int i=0;i<PokeTool::MODEL_NODE_MAX;i++ )
        {
          for( int j=0;j<NodeCheckNum[i];j++ )
          {
            ret = mPokeModel[mPokeIdx]->IsEnableEffectNode((PokeTool::MODEL_NODE)i,j);
            if( ret )
            {
              gfl2::math::Vector3 pos;
              mPokeModel[mPokeIdx]->GetEffectNodePosition((PokeTool::MODEL_NODE)i,j,&pos);
              POKEVIEW_PRINT("%f,%f,%f,",pos.x,pos.y,pos.z);
            }
            else
            {
              POKEVIEW_PRINT( "---,---,---,");
            }          
          }
        }
      }
      if( mPrintMode != -1 )
      {
        POKEVIEW_PRINT( "END\n" );
      }
      mMainSeq = SEQ_AUTO_TERM;
    }
    break;
  case SEQ_AUTO_TERM:
    if( mPokeModel[mPokeIdx]->CanDelete() )
    {
      DeleteFullPowerPath();
      //モデル破棄
      mPokeModelSys->DeleteModel(mPokeModel[mPokeIdx]);
      mPokeModel[mPokeIdx] = NULL;
      mMainSeq = SEQ_AUTO_GO_NEXT;
    }
    break;
  case SEQ_AUTO_GO_NEXT:
    //次の設定
    {
      bool isNextMons = true;
      const PokeTool::PokeModelSystem::POKE_MNG_DATA *mngData = mPokeModelSys->GetMngData(mMonsNo);
      if( mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FEMALE )
      {
        if( mSex == PM_MALE && mFormNo == 0 )
        {
          mSex = PM_FEMALE;
          isNextMons = false;
        }
      }

      if( mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FORM_CHANGE )
      {
        if( isNextMons == true )
        {
          int fomrNum = mngData->mDataNum;
          if( mngData->mFlgData & PokeTool::PokeModelSystem::POKE_MNG_FLG_EXIST_FEMALE )
          {
            fomrNum--;
          }
          if( mFormNo < fomrNum-1 )
          {
            mFormNo++;
            mSex = PM_MALE;
            isNextMons = false;
          }
        }
      }

      if( isNextMons == true )
      {
        if( mMonsNo >= mEndMonsNo )
        {
          mActiveAutoTest = false;
        }
        else
        {
          mMonsNo++;
          mFormNo = 0;
          mSex = PM_MALE;
        }
      }
    }
    mMainSeq = SEQ_WAIT;
    break;

  }

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void PokeViewTest::PreDrawFunc( gfl2::proc::Manager* pManager )
{

}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void PokeViewTest::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
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
b32 PokeViewTest::StartRenderPath(void)
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
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* PokeViewTest::GetRenderPath(void)
{
#if USE_AURA_SHADER
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
#endif
  //ポケモン用カラーシェーダー描画
  const int pokePathNum = mPokeModelSys->GetPokeShaderPathNum();
  if( mRenderPathCnt < pokePathNum )
  {
    return mPokeModelSys->GetPokeShaderPath(mRenderPathCnt);
  }

  //メインの描画
  switch( mRenderPathCnt-pokePathNum )
  {
  case 0:
    return mEdgeMapPath;
  case 1:
    return mModelRenderPath;
  case 2:
    return mIdModelRenderPath;
#if USE_AURA_SHADER
  case 3:
    if( mDispRim )
    {
      return mFullPowerEffectRenderPath0;
    }
    break;
  case 4:
    if( mDispAura )
    {
      return mFullPowerEffectRenderPath1;
    }
    break;
#endif //USE_AURA_SHADER
  case 5:
    return mOutLinePath;
  case 6:
    return mStretchBltFrameBufferPath;
  case 7:
    return mDistortionPostRenderPath;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 PokeViewTest::NextRenderPath(void)
{
  mRenderPathCnt++;
  if( mRenderPathCnt >= 8 + mPokeModelSys->GetPokeShaderPathNum() )
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

void PokeViewTest::UpdateCamera(void)
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

/*
  if( btn->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    SetCameraPosTarget(gfl2::math::Vector3(256.0f,106.0f,465.0f),gfl2::math::Vector3(-45.0f,70.0f,-20.0f));
    UpdateCameraMatrix();
  }
*/
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
#if USE_AURA_SHADER
void  PokeViewTest::UpdateCameraSample()
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  gfl2::ui::VectorDevice *stick  = uiMng->GetStick(0);
  gfl2::ui::VectorDevice *uiKey = uiMng->GetCrossKey(0);

  // 左スティック = 回転
  mSampleCamera.ControlRotate( stick->GetX() * -0.1f , stick->GetY() * -0.05f  );

  // 十字キー = 注視点移動
  f32 moveH = 0;
  f32 moveV = 0;
  if( uiKey->IsHold(gfl2::ui::DIR_UP   ))   { moveV = -2.0f; }
  if( uiKey->IsHold(gfl2::ui::DIR_DOWN )) { moveV = 2.0f; }
  if( uiKey->IsHold(gfl2::ui::DIR_LEFT )) { moveH = -2.0f; }
  if( uiKey->IsHold(gfl2::ui::DIR_RIGHT)){ moveH = 2.0f; }
  mSampleCamera.ControlMove( moveH, moveV );

  // XYボタン = ズームイン/アウト
  f32 zoom = 0;
  if( btn->IsHold( gfl2::ui::BUTTON_X ) )   { zoom = 10.0f; }
  if( btn->IsHold( gfl2::ui::BUTTON_Y ) )   { zoom = -10.0f; }
  mSampleCamera.ControlZoom( zoom );

  mSampleCamera.GetViewMtx( &mCamViewMtx );
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
#endif
void PokeViewTest::SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg)
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

void PokeViewTest::UpdateCameraMatrix(void)
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

void PokeViewTest::ResetDebugParam(void)
{
#if USE_AURA_SHADER
  for( int i=0;i<disableMaterialInfo::DISABLE_MATERIAL_MAX;i++ )
  {
    mFPEnableMatNo[i] = -1;
  }
  int dataSize;
  const void *disableMaterialBuf = mPokeModel[mPokeIdx]->GetFullPowerDisableMaterialData(&dataSize);
  if( disableMaterialBuf )
  {
    const u8 *disableMaterialData = static_cast<const u8*>(disableMaterialBuf);
    //サイズが128でパディングされているのであまりあてにならない
    u32 dataNum = dataSize/(PokeTool::POKE_MATNAME_LEN);  

    int setIdx = 0;
    //マテリアルの書き換え処理
    for( u32 datIdx = 0 ;datIdx < dataNum ; datIdx++ )
    {
      const u8* matName = &disableMaterialData[datIdx*(PokeTool::POKE_MATNAME_LEN)];
      gfl2::util::FnvHash	hash( matName );

      if( mCheckFPEnableMaterial )
      {
        GFL_PRINT("[%s][%d]\n",matName,hash.value);
      }
      bool hit = false;
      int matNum = mPokeModel[mPokeIdx]->GetMaterialCount();
      for( int matLoop = 0; matLoop < matNum ; matLoop++)
      {
        const u32 matHash = mPokeModel[mPokeIdx]->GetModelInstanceNode()->GetMaterialInstanceNode(matLoop)->GetNameHash();
        if( matHash == hash.value )
        {
          hit = true;
          if( mCheckFPEnableMaterial )
          {
            GFL_PRINT("  [%s][%d]Match\n",mPokeModel[mPokeIdx]->GetModelInstanceNode()->GetMaterialInstanceNode(matLoop)->GetName(),matHash);
          }
          if( setIdx < disableMaterialInfo::DISABLE_MATERIAL_MAX )
          {
            mFPEnableMatNo[setIdx] = matLoop;
            setIdx++;
          }
          else
          {
            GFL_ASSERT_MSG(0,"マテリアル数が想定を超えました！[%s]\n",matName);
          }
          break;
        }
        else
        {
          if( mCheckFPEnableMaterial )
          {
            GFL_PRINT("  [%s][%d]\n",mPokeModel[mPokeIdx]->GetModelInstanceNode()->GetMaterialInstanceNode(matLoop)->GetName(),matHash);
          }
        }
      }
      if( hit == false )
      {
        if( mCheckFPEnableMaterial && matName[0] != 0 )
        {
          GFL_ASSERT_MSG(0,"Material not found![%s]\n",matName);
        }
      }
    }
  }
#endif //USE_AURA_SHADER
}

void PokeViewTest::AddFullPowerPath(void)
{
#if USE_AURA_SHADER
  // 全力技エフェクト(Rim+Aura)の外部設定
  FullPowerEffectRimMdlManager::mdlStatusSetting rimStatus;
  rimStatus.rimColorR = mRimColorR;//0.0f;
  rimStatus.rimColorG = mRimColorG;
  rimStatus.rimColorB = mRimColorB;//0.5f;
  rimStatus.rimPow = mRimPow;//4.0f;
  rimStatus.rimWidth = mRimWidth;//0.4f;
  rimStatus.rimMoveSpeed = mRimMoveSpeed;//0x400;
  rimStatus.rimMoveWidth = mRimMoveWidth;

  FullPowerEffectAuraMdlManager::mdlStatusSetting auraStatus;
  auraStatus.auraColorR = mAuraColorR;
  auraStatus.auraColorG = mAuraColorG;
  auraStatus.auraColorB = mAuraColorB;
  auraStatus.auraScaleX = mAuraScaleX*mAuraBaseScaleX;
  auraStatus.auraScaleY = mAuraScaleY*mAuraBaseScaleY;
  auraStatus.density = mAuraDensity;

  //マテリアル例外設定
  int setIdx = 0;
  for( int i=0;i<disableMaterialInfo::DISABLE_MATERIAL_MAX;i++ )
  {
    if( mFPEnableMatNo[i] != -1 )
    {
      //一応ハッシュ変換の動作テストも。
      const c8 *name = mPokeModel[mPokeIdx]->GetModelInstanceNode()->GetMaterialInstanceNode(mFPEnableMatNo[i])->GetName();
      gfl2::util::FnvHash	hash( (const u8*) name );

      rimStatus.m_disableMaterialInfo.materialHashList[rimStatus.m_disableMaterialInfo.targetNum] = hash.value;
      rimStatus.m_disableMaterialInfo.targetNum++;
      auraStatus.m_disableMaterialInfo.materialHashList[auraStatus.m_disableMaterialInfo.targetNum] = hash.value;
      auraStatus.m_disableMaterialInfo.targetNum++;
    }
  }

  mFullPowerEffectRenderPath0->AddRenderingTarget(mFPHeap,mPokeModel[mPokeIdx]->GetModelInstanceNode(),&rimStatus);
  mFullPowerEffectRenderPath1->AddRenderingTarget(mFPHeap,mPokeModel[mPokeIdx]->GetModelInstanceNode(),&auraStatus);
#endif //USE_AURA_SHADER
}
void PokeViewTest::RemoveFullPowerPath(void)
{
#if USE_AURA_SHADER
  mFullPowerEffectRenderPath0->RemoveRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
  mFullPowerEffectRenderPath1->RemoveRenderingTarget(mPokeModel[mPokeIdx]->GetModelInstanceNode());
#endif //USE_AURA_SHADER
}

void PokeViewTest::DeleteFullPowerPath(void)
{
#if USE_AURA_SHADER
  mFullPowerEffectRenderPath0->DeleteManagedModel(mPokeModel[mPokeIdx]->GetModelInstanceNode());
  mFullPowerEffectRenderPath1->DeleteManagedModel(mPokeModel[mPokeIdx]->GetModelInstanceNode());
#endif //USE_AURA_SHADER
}

void PokeViewDebug_UpdateFunc_ClipBoard( void* userWork  , gfl2::debug::DebugWinItem *item );
void PokeViewDebug_UpdateFunc_Reset( void* userWork  , gfl2::debug::DebugWinItem *item );
void PokeViewDebug_UpdateFunc_FormChange( void* userWork  , gfl2::debug::DebugWinItem *item );
void PokeViewDebug_UpdateFunc_ReloadFullPower( void* userWork  , gfl2::debug::DebugWinItem *item );

void PokeViewDebug_UpdateFunc_ZenryokuMaterial( void* userWork  , gfl2::debug::DebugWinItem *item );
wchar_t* PokeViewDebug_DrawFunc_ZenryokuMaterial( void* userWork  , gfl2::debug::DebugWinItem *item );

void PokeViewDebug_Callback_Shader( void* userWork );
void PokeViewDebug_Callback_Motion( void* userWork );
void PokeViewDebug_Callback_Face( void* userWork );
void PokeViewDebug_Callback_DispIdModel( void* userWork );

void PokeViewTest::InitDebug(void)
{
  mRimColorR  = 1.0f;
  mRimColorG  = 1.0f;
  mRimColorB  = 0.4f;
  mRimPow     = 4.0f;
  mRimWidth   = 0.4f;
  mRimMoveSpeed = 2048;
  mRimMoveWidth = 0.2f;

  mAuraColorR   = 1.0f;
  mAuraColorG   = 1.0f;
  mAuraColorB   = 0.4f;
  mAuraScaleX   = 1.0f;
  mAuraScaleY   = 1.0f;
  mAuraDensity  = 9024;
  mAuraBaseScaleX = 1.2f;
  mAuraBaseScaleY = 1.0f;

  mShaderCol.Set(1.0f,1.0f,1.0f,0.0f);
  mEnvCol.Set(1.0f,1.0f,1.0f,0.0f);

  mDispAura = true;
  mDispRim = true;
  mLoadShadow = false;
  mLoadIdModel = false;
  mAutoEnableFullPower = false;
  mCheckFPEnableMaterial = false;

  mActiveAutoTest = false;
  mPrintMode = -1;

  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"PokeViewTest",NULL);
  gfl2::debug::DebugWinItem* item;

  //---------------------------------------------------
  gfl2::debug::DebugWinGroup *zGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"Zenryoku",topGroup);
  gfl2::debug::DebugWinGroup *zMatGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"Material",zGroup);
  gfl2::debug::DebugWin_AddItemUpdateFunc(zMatGroup,mHeap,L"Reload",this,PokeViewDebug_UpdateFunc_ZenryokuMaterial,PokeViewDebug_DrawFunc_ZenryokuMaterial);
  gfl2::debug::DebugWin_AddItemUpdateFunc(zMatGroup,mHeap,L"Reload",this,PokeViewDebug_UpdateFunc_ReloadFullPower);

  item = gfl2::debug::DebugWin_AddItemValueBool(zGroup,mHeap,L"DispAura",&mDispAura);
  item = gfl2::debug::DebugWin_AddItemValueBool(zGroup,mHeap,L"DispRim",&mDispRim);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"RimR",&mRimColorR,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"RimG",&mRimColorG,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"RimB",&mRimColorB,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"RimPow",&mRimPow,0,255.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"RimWidth",&mRimWidth,0,255.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueU32(zGroup,mHeap,L"RimSpeed",&mRimMoveSpeed,0,65535);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"mRimMoveWidth",&mRimMoveWidth,0,255.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraR",&mAuraColorR,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraG",&mAuraColorG,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraB",&mAuraColorB,0,1.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraScaleX",&mAuraScaleX,0,32.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraScaleY",&mAuraScaleY,0,32.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueU32(zGroup,mHeap,L"AuraDensity",&mAuraDensity,0,65535);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraScaleX(Base)",&mAuraBaseScaleX,0,32.0f,0.1f);
  item = gfl2::debug::DebugWin_AddItemValueFloat(zGroup,mHeap,L"AuraScaleY(Base)",&mAuraBaseScaleY,0,32.0f,0.1f);
  gfl2::debug::DebugWin_AddItemUpdateFunc(zGroup,mHeap,L"ClipBoard",this,PokeViewDebug_UpdateFunc_ClipBoard);
  gfl2::debug::DebugWin_AddItemUpdateFunc(zGroup,mHeap,L"Reset",this,PokeViewDebug_UpdateFunc_Reset);
  //---------------------------------------------------
  gfl2::debug::DebugWinGroup *autoGroup = gfl2::debug::DebugWin_AddGroup(mHeap,L"AutoTest",topGroup);
  item = gfl2::debug::DebugWin_AddItemValueBool(autoGroup,mHeap,L"AutoTest",&mActiveAutoTest);
  item = gfl2::debug::DebugWin_AddItemValueIntMsg(autoGroup,mHeap,L"StartMons",&mStartMonsNo,print::GetMessageArcId(),GARC_message_monsname_DAT,1,MONSNO_MAX-2);
  item = gfl2::debug::DebugWin_AddItemValueIntMsg(autoGroup,mHeap,L"EndMons  ",&mEndMonsNo,print::GetMessageArcId(),GARC_message_monsname_DAT,1,MONSNO_MAX-2);
  item = gfl2::debug::DebugWin_AddItemValueInt(autoGroup,mHeap,L"PrintMode",&mPrintMode,-1,3);
 
  item = gfl2::debug::DebugWin_AddItemValueBool(autoGroup,mHeap,L"FullPowerShader",&mAutoEnableFullPower);
  item = gfl2::debug::DebugWin_AddItemValueBool(autoGroup,mHeap,L"CheckFPMaterial",&mCheckFPEnableMaterial);

  //---------------------------------------------------
  item = gfl2::debug::DebugWin_AddItemValueIntMsg(topGroup,mHeap,L"MonsNo",&mMonsNo,print::GetMessageArcId(),GARC_message_monsname_DAT,1,MONSNO_MAX-2);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"FormNo",&mFormNo,0,27);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Sex",&mSex,0,2);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"RandUp",&mRandUp,0,65535);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"RandDown",&mRandDown,0,65535);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"AnimeType",&mAnimeType,0,PokeTool::MODEL_ANIMETYPE_MAX-1);
  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,mHeap,L"Rare",&mRare);
  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,mHeap,L"LoadShadow",&mLoadShadow);
  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,mHeap,L"LoadIdModel",&mLoadIdModel);
   
  item = gfl2::debug::DebugWin_AddItemValueVector4(topGroup,mHeap,L"ShaderCol",&mShaderCol,0.0f,1.0f,0.1f);
  item->SetChangedValueFunc(PokeViewDebug_Callback_Shader,this);
  item = gfl2::debug::DebugWin_AddItemValueVector4(topGroup,mHeap,L"EnvCol",&mEnvCol,0.0f,1.0f,0.1f);
  item->SetChangedValueFunc(PokeViewDebug_Callback_Shader,this);

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Motion",&mMotionIdx,0,PokeTool::MODEL_ANIME_MAX-1);
  item->SetChangedValueFunc(PokeViewDebug_Callback_Motion,this);

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Face:Eye",&mEyeIdx,-1,7);
  item->SetChangedValueFunc(PokeViewDebug_Callback_Face,this);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup,mHeap,L"Face:Mouth",&mMouthIdx,-1,7);
  item->SetChangedValueFunc(PokeViewDebug_Callback_Face,this);

  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,mHeap,L"DispIdModel",&mDispIdModel);
  item->SetChangedValueFunc(PokeViewDebug_Callback_DispIdModel,this);

  gfl2::debug::DebugWin_AddItemUpdateFunc(topGroup,mHeap,L"FormChange",this,PokeViewDebug_UpdateFunc_FormChange);

  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,mHeap,L"EnableLook",&mIsLook);
  item = gfl2::debug::DebugWin_AddItemValueVector3(topGroup,mHeap,L"LookPos",&mLookPos,-1000.0f,1000.0f,1.0f);

  gfl2::debug::DebugWin_SetCurrentItem(topGroup);
}

void PokeViewTest::TermDebug(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"PokeViewTest");
}

#if defined(GF_PLATFORM_CTR)
#include <nn/dbg.h>
/*!
  @brief IS-CTR ソフトウェアで使用可能なコマンドを呼び出します。
         使用できるコマンドは、
         「スタートメニュー > IS-CTR-DEBUGGER > IS-CTR-DEBUGGERのヘルプ」の
         「機能説明 > コマンド一覧」を参照してください。

         この機能は CTR for EMULATOR を取り外している場合は使用できません。

  @param [in] fmt    コマンド文字列(フォーマット文字列)を指定します。
*/
static inline void isutilDebuggerVCommand(const char* fmt, ::std::va_list va)
{
    NN_UNUSED_VAR(fmt);
    NN_UNUSED_VAR(va);
    NN_LOG("\x1b[> ");
    (void)nn::dbg::detail::VPrintf(fmt,va);
    NN_LOG("\n");
}
static inline void isutilDebuggerCommand(const char* fmt, ...)
{
    ::std::va_list va;
    va_start(va, fmt);
    isutilDebuggerVCommand(fmt, va);
    va_end(va);
}
#endif

void PokeViewDebug_UpdateFunc_ClipBoard( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
#if defined(GF_PLATFORM_CTR)
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    isutilDebuggerCommand("!echo %.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%d | clip"
        ,work->mRimColorR
        ,work->mRimColorG
        ,work->mRimColorB
        ,work->mRimPow
        ,work->mRimWidth
        ,work->mRimMoveSpeed
        ,work->mRimMoveWidth
        ,work->mAuraColorR
        ,work->mAuraColorG
        ,work->mAuraColorB
        ,work->mAuraScaleX
        ,work->mAuraScaleY
        ,work->mAuraDensity );
  }
#endif
}

void PokeViewDebug_UpdateFunc_Reset( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {  
    PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
    work->mRimColorR  = 1.0f;
    work->mRimColorG  = 1.0f;
    work->mRimColorB  = 0.4f;
    work->mRimPow     = 4.0f;
    work->mRimWidth   = 0.4f;
    work->mRimMoveSpeed = 2048;
    work->mRimMoveWidth = 0.2f;

    work->mAuraColorR   = 1.0f;
    work->mAuraColorG   = 1.0f;
    work->mAuraColorB   = 0.4f;
    work->mAuraScaleX   = 1.0f;
    work->mAuraScaleY   = 1.0f;
    work->mAuraDensity  = 9024;
    work->mAuraBaseScaleX = 1.2f;
    work->mAuraBaseScaleY = 1.0f;
  }

}

void PokeViewDebug_UpdateFunc_FormChange( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {

    PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
    for( int i=0;i<PokeViewTest::POKE_NUM;i++ )
    {
      if( work->mPokeModel[i] )
      {
        work->mPokeModel[i]->NoLoadFormChange(work->mFormNo);
      }
    }
  }
}

void PokeViewDebug_UpdateFunc_ReloadFullPower( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
    work->RemoveFullPowerPath();
    work->mReserveReloadFullPower = true;
    work->mReserveReloadFullPowerCnt = 2;
  }
}

void PokeViewDebug_UpdateFunc_ZenryokuMaterial( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::DeviceManager *uiMng = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  gfl2::ui::VectorDevice *uiKey = uiMng->GetCrossKey(0);

  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
  PokeTool::PokeModel *poke = work->mPokeModel[0];

#if USE_AURA_SHADER
  if( poke == NULL )
  {
    return;
  }
  if( btn->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    work->mFPEnableMatArrIdx--;
    if( work->mFPEnableMatArrIdx < 0 )
    {
      work->mFPEnableMatArrIdx = disableMaterialInfo::DISABLE_MATERIAL_MAX-1;
    }
  }
  if( btn->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    work->mFPEnableMatArrIdx++;
    if( work->mFPEnableMatArrIdx >= disableMaterialInfo::DISABLE_MATERIAL_MAX )
    {
      work->mFPEnableMatArrIdx = 0;
    }
  }

  if( uiKey->IsTrigger(gfl2::ui::BUTTON_LEFT))
  {
    work->mFPEnableMatNo[work->mFPEnableMatArrIdx]--;
    if( work->mFPEnableMatNo[work->mFPEnableMatArrIdx] < -1 )
    {
      work->mFPEnableMatNo[work->mFPEnableMatArrIdx] = poke->GetMaterialCount()-1;
    }
  }
  if( uiKey->IsTrigger(gfl2::ui::BUTTON_RIGHT))
  {
    work->mFPEnableMatNo[work->mFPEnableMatArrIdx]++;
    if( work->mFPEnableMatNo[work->mFPEnableMatArrIdx] >= poke->GetMaterialCount() )
    {
      work->mFPEnableMatNo[work->mFPEnableMatArrIdx] = -1;
    }
  }
#endif //USE_AURA_SHADER

}

wchar_t* PokeViewDebug_DrawFunc_ZenryokuMaterial( void* userWork  , gfl2::debug::DebugWinItem *item )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);
  PokeTool::PokeModel *poke = work->mPokeModel[0];

  wchar_t* workStr = item->GetSysWorkStr();
#if USE_AURA_SHADER
  if( poke == NULL || work->mFPEnableMatNo[work->mFPEnableMatArrIdx] == -1 )
  {
    swprintf( workStr ,gfl2::debug::DEBUGWIN_NAME_LEN, L"[%d][----]",work->mFPEnableMatArrIdx);
  }
  else
  {
    swprintf( workStr ,gfl2::debug::DEBUGWIN_NAME_LEN, L"[%d][%s]",work->mFPEnableMatArrIdx,poke->GetModelInstanceNode()->GetMaterialInstanceNode(work->mFPEnableMatNo[work->mFPEnableMatArrIdx])->GetName());
  }
#else //USE_AURA_SHADER
  swprintf( workStr ,gfl2::debug::DEBUGWIN_NAME_LEN, L"ONLY CTR");
#endif //USE_AURA_SHADER

  return workStr;
}

void PokeViewDebug_Callback_Shader( void* userWork )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);

  for( int i=0;i<PokeViewTest::POKE_NUM;i++ )
  {
    if( work->mPokeModel[i] )
    {
      work->mPokeModel[i]->SetShaderColor(work->mShaderCol); 
      work->mPokeModel[i]->SetEnabledAmbientColor(true);
      work->mPokeModel[i]->SetAmbientColor(work->mEnvCol);
    }
  }
}

void PokeViewDebug_Callback_Motion( void* userWork )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);

  for( int i=0;i<PokeViewTest::POKE_NUM;i++ )
  {
    if( work->mPokeModel[i] )
    {
      work->mPokeModel[i]->ChangeAnimationDirect(work->mMotionIdx); 
      work->mPokeModel[i]->SetAnimationIsLoop(true);
    }
  }
}

void PokeViewDebug_Callback_Face( void* userWork )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);

  for( int i=0;i<PokeViewTest::POKE_NUM;i++ )
  {
    if( work->mPokeModel[i] )
    {
      work->mPokeModel[i]->SetFaceAnime_Eye(work->mEyeIdx);
      work->mPokeModel[i]->SetFaceAnime_Mouth(work->mMouthIdx);
    }
  }
}

void PokeViewDebug_Callback_DispIdModel( void* userWork )
{
  PokeViewTest *work = static_cast<PokeViewTest*>(userWork);

  for( int i=0;i<PokeViewTest::POKE_NUM;i++ )
  {
    if( work->mPokeModel[i] )
    {
      if( work->mDispIdModel == false )
      {
        work->mModelRenderPath->AddNode(work->mPokeModel[i]->GetModelInstanceNode());
        work->mEdgeMapPath->AddEdgeRenderingTarget(work->mPokeModel[i]->GetModelInstanceNode());
        if( work->mPokeModel[i]->GetIdModel() )
        {
          work->mIdModelRenderPath->RemoveNode(work->mPokeModel[i]->GetIdModel()->GetModelInstanceNode());
        }
      }
      else
      {
        work->mModelRenderPath->RemoveNode(work->mPokeModel[i]->GetModelInstanceNode());
        work->mEdgeMapPath->RemoveEdgeRenderingTarget(work->mPokeModel[i]->GetModelInstanceNode());
        if( work->mPokeModel[i]->GetIdModel() )
        {
          work->mIdModelRenderPath->AddNode(work->mPokeModel[i]->GetIdModel()->GetModelInstanceNode());
        }
      }
    }
  }
}

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif//PM_DEBUG

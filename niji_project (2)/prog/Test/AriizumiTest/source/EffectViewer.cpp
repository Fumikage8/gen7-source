//======================================================================
/**
 * @file  BattleMeasure.cpp
 * @brief 15/03/13
 * @author  ariizumi
 * @data  エフェクトビュワー
 */
//======================================================================
#if PM_DEBUG

#include <System/include/HeapDefine.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

#include <Test/AriizumiTest/include/AriTestBtlv.h>
#include <Test/AriizumiTest/include/EffectViewer.h>
#include <arc_def_index/arc_def.h>
#include <Battle/source/btlv/btlv_task.h>
#include <Battle/source/btlv/btlv_ui.h>
#include <Battle/source/btlv/btlv_seq_sys.h>
#include "Debug/DebugWin/include/DebugWinSystem.h"

#include "sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
#include "EffectViewerLowBgFrame.h"
#include <arc_index/wazaeff_seqfile.gaix>




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

  EffectViewerProc::EffectViewerProc()
{
  mMainSeq = 0;
}

EffectViewerProc::~EffectViewerProc(void)
{

}

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result EffectViewerProc::InitFunc(gfl2::proc::Manager* pManager)
{

  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  gfl2::heap::HeapBase *debugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT);

  switch( mMainSeq )
    {
    case 0:
      {
        //0x1C00000
        //mMemHeap = GFL_CREATE_LOCAL_HEAP_NAME(   heap, 0x400000,gfl2::heap::HEAP_TYPE_EXP,false,"BtlMeasureMem");
        //mDevHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1b20000,gfl2::heap::HEAP_TYPE_EXP,false,"BtlMeasureDev");
      
        mBtlvSystem = GFL_NEW(heap)DebugBattleViewSystem();
        mBtlvSystem->SetDebugParam(mDebugParam);
        mBtlvSystem->SetBattleProc(this);

        mMainSeq++;
      }
      //break;
    case 1:
      {
        if( mBtlvSystem->Initialize( debugHeap,heap ) == true)
          {
            mBtlvSystem->InitEffectViewerDebugMenu(debugHeap);
          
            mMainSeq = 0;
            mSubSeq = 0;
            mTermSeq = 0;
            mIsFinishSetup = false;

            {
              // 下画面アプリ用のメモリ確保と初期化(Event Heap 使用)
              gfl2::heap::HeapBase *event_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
              AppParamInit( event_heap );
            }

            InitDebugMenu();
            return gfl2::proc::RES_FINISH;
          }
      }
      break;
    }
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result EffectViewerProc::EndFunc(gfl2::proc::Manager* pManager)
{

  switch( mTermSeq )
    {
    case 0:
      TermDebugMenu();
      mBtlvSystem->TermEffectViewerDebugMenu();
      mTermSeq++;
      break;  //スルー

    case 1:

      Sound::EndBGMReq();  // Stop BGM

      // 下画面アプリ用のメモリの開放
      if( AppParamEnd() )
        {
          mTermSeq++;
        }
      break;

    case 2:
      if( mBtlvSystem->Finalize() )
        {
          GFL_SAFE_DELETE( mBtlvSystem );
          return gfl2::proc::RES_FINISH;
        }
    }

  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result EffectViewerProc::UpdateFunc(gfl2::proc::Manager* pManager)
{


  mBtlvSystem->Update();

  enum
  {
    SEQ_SETUP = 0,
    SEQ_SETUP_WAIT,
    SEQ_MAIN,
    SEQ_CLEANUP,
    SEQ_CLEANUP_WAIT,
    SEQ_END,
  };
  switch(mMainSeq)
    {
    case SEQ_SETUP:
      mBtlvSystem->CMD_StartSetup();
      mMainSeq++;
      //break;
    case SEQ_SETUP_WAIT:
      if( mBtlvSystem->CMD_WaitSetup() )
        {
          mIsFinishSetup = true;
          mBtlvSystem->ChangeRule ( mEffectViewerUiInfo.mBattleRule );
          mMainSeq++;
        }
      break;
    case SEQ_MAIN:
      {
        bool ret = UpdateMain();
        if( ret )
          {
            mIsFinishSetup = false;
            mMainSeq++;
          }
      }
      break;
    case SEQ_CLEANUP:
      {
        mBtlvSystem->CMD_StartCleanUp();
        mMainSeq++;
      }
      //break;
    case SEQ_CLEANUP_WAIT:
      if( mBtlvSystem->CMD_WaitCleanUp() )
        {
          mMainSeq++;
        }
      break;
    case SEQ_END:
      return gfl2::proc::RES_FINISH;
      //break;
    }


  // ------------------------
  // 下画面関連の処理

  //Frameシーケンス進行
  if( mpFrameManager->Main() == applib::frame::MAINSTATUS_NULL )
    {
      return gfl2::proc::RES_FINISH;
    }

  // 下画面描画アップデート
  mpAppRenderingManager->Update();



  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief       PROCの描画処理(下準備)
 * @param pManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void EffectViewerProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  mBtlvSystem->PreDraw();
  mpAppRenderingManager->PreDraw();     // 下画面用RenderingManager
}

/**
 * @brief       PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void EffectViewerProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{

  if( mIsFinishSetup )
    {
      mBtlvSystem->Draw(displayNo);
    }


  // ----------------------------
  // 下画面関連の処理

  // 上画面の描画の後に描く必要がある
  // （上画面の処理が下画面も描いているので、上書きされてしまう）
  mpFrameManager->Draw(displayNo);         // レイアウト描画の登録
  mpAppRenderingManager->Draw(displayNo);  // 実際の描画
  
}

//-------------------------------------------------------------------------
/**
 * @brief 上画面用 Update
 */
//-------------------------------------------------------------------------
enum
  {
    STANDARD_LOOP = 0,
    WAZA_SEQ_WAIT,
    CHANGE_POKE_WAIT,

    START_AUTO_TEST,
    WAIT_AUTO_TEST,
    GO_NEXT_AUTO_TEST,
  };
bool EffectViewerProc::UpdateMain(void)
{

  const gfl2::ui::DeviceManager *uiMng = mBtlvSystem->GetUIDevice();
  gfl2::ui::Button *btn = uiMng->GetButton(0);
  
  switch(mSubSeq)
    {
    case STANDARD_LOOP:
      if( gfl2::debug::DebugWin_IsOpen() )
        {
          break;
        }

      //自動テスト
      if( mEnableAutoTest == true )
      {
        mSubSeq = START_AUTO_TEST;

        gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
        mSdLog.OpenLogFile(heap,"EffViewTest");

      }

      // シーケンスエディタのファイル再生
      if( btn->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
#if GFL_FS_HIO_ENABLE
        SetWazaTarget();

        mBtlvSystem->GetWazaParam()->mId = static_cast<btl::WazaID>(mEffectViewerUiInfo.mWazaNo);
        mBtlvSystem->GetWazaParam()->mTurnType = static_cast<btl::BtlvWazaEffect_TurnType>(mEffectViewerUiInfo.mWazaSubNo);
        mBtlvSystem->SeqComFunc_PlaySequenceFileDebug();
        mBtlvSystem->GetBattleSound()->BgmVolumeDown_Waza();

        mAutoTestCmdBuf = 0;
        mRenderPathCnt = 0;
        mSubSeq++;
        break;
#endif
      }

      if( btn->IsTrigger( gfl2::ui::BUTTON_X ) )
      {
        gfl2::fs::AsyncFileManager *fileMng = mBtlvSystem->GetAsyncFileManager();
        {
          gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
          req.arcId = ARCID_WAZAEFF_RESOURCE;
          req.heapForReq = mBtlvSystem->GetResidentHeap()->GetLowerHandle();
          mBtlvSystem->GetAsyncFileManager()->SyncArcFileClose(req);
        }
        {
          gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
          req.arcId = ARCID_WAZAEFF_RESOURCE;
          req.heapForFile = mBtlvSystem->GetResidentHeap();
          req.heapForReq = mBtlvSystem->GetResidentHeap()->GetLowerHandle();
#if PM_DEBUG
          if (mBtlvSystem->GetDebugParam() && mBtlvSystem->GetDebugParam()->isHioEffResource)
          {
            req.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
            req.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
          }
#endif
          fileMng->SyncArcFileOpen(req);
        }

        mBtlvSystem->ReloadBattleData();

        Sound::PlaySE(SEQ_SE_OPEN1);
        break;
      }
      if ((btn->IsHold(gfl2::ui::BUTTON_R) || btn->IsHold(gfl2::ui::BUTTON_L)) && btn->IsTrigger(gfl2::ui::BUTTON_Y))
      {
        SetWazaTarget();
        mBtlvSystem->SeqComFunc_PlaySequenceFile(GARC_wazaeff_seqfile_er001_BSEQ);
        mSubSeq++;
      }

      {
        // 下画面UIからのアクションコマンド
        CheckUiAction();
      }
      break;


    case WAZA_SEQ_WAIT:
      {
        int frm = mBtlvSystem->GetSeqSys()->GetSeqFrame();
        if (frm > 0)
        {
          mEffectViewerUiInfo.mDispFrame = frm;

          u32 cmdBuf = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferUseSize;
          if( mAutoTestCmdBuf < cmdBuf )
          {
            mAutoTestCmdBuf = cmdBuf;
          }
          int modelNum = mBtlvSystem->GetRenderSys()->GetRenderingPipeLine()->GetModelRenderPath()->GetNodeNum();
          if( mRenderPathCnt < modelNum )
          {
            mRenderPathCnt = modelNum;
          }
        }
      }

      if( mBtlvSystem->SeqComFunc_IsFinishSequenceFile() )
        {
          GFL_PRINT("CommandBufMax[%d]ModelNumMax[%d]\n",mAutoTestCmdBuf,mRenderPathCnt);
          mSubSeq = STANDARD_LOOP;
          mBtlvSystem->GetBattleSound()->BgmVolumeReset_Waza();
        }
      break;

    case CHANGE_POKE_WAIT:
      // ポケモン読み替え待ち
      {
        if( 0 == mBtlvSystem->GetTaskSystemAlways()->GetActiveTaskNum() )
          {
            // Visible ON
            mBtlvSystem->GetPokeModel((btl::BtlvPos)mEffectViewerUiInfo.mTargetSlot)->SetVisible( true );
            mSubSeq = STANDARD_LOOP;
          }
      }
      break;
    //自動テスト
    case START_AUTO_TEST:
      {
        //ターゲット設定
        mEffectViewerUiInfo.mAttackPos = mSubIdx;

        BtlRule rule =  mBtlvSystem->GetBattleRule();
        int type = pml::wazadata::GetParam((WazaNo)mAutoPlayWaza,pml::wazadata::PARAM_ID_TARGET);
        switch( type )
        {
        case pml::wazadata::TARGET_USER:               ///< 自分のみ
        case pml::wazadata::TARGET_UNKNOWN:            ///< ゆびをふるなど特殊型
          mEffectViewerUiInfo.mDeffencePos = mSubIdx;
          mEffectViewerUiInfo.mRangeType = EffectViewerUiInfo::WAZA_RANGE_SINGLE;
          break;
        case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
          mEffectViewerUiInfo.mDeffencePos = mSubDirIdx;
          mEffectViewerUiInfo.mRangeType = EffectViewerUiInfo::WAZA_RANGE_SINGLE;
          break;
        case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
        case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
        case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
        case pml::wazadata::TARGET_ENEMY_RANDOM:       ///< 相手ポケ１体ランダム
          mEffectViewerUiInfo.mDeffencePos = mSubDirIdx;
          if( mSubDirIdx >= mSubIdx )
          {
            mEffectViewerUiInfo.mDeffencePos++;
          }
          mEffectViewerUiInfo.mRangeType = EffectViewerUiInfo::WAZA_RANGE_SINGLE;
          break;
        case pml::wazadata::TARGET_FRIEND_ALL:         ///< 味方側全ポケ
        case pml::wazadata::TARGET_SIDE_FRIEND:        ///< 自分側陣営
          mEffectViewerUiInfo.mDeffencePos = mSubIdx;
          mEffectViewerUiInfo.mRangeType = EffectViewerUiInfo::WAZA_RANGE_ENEMY;
          break;
        case pml::wazadata::TARGET_ENEMY_ALL:          ///< 相手側全ポケ
        case pml::wazadata::TARGET_ALL:                ///< 場に出ている全ポケ
        case pml::wazadata::TARGET_FIELD:              ///< 場全体（天候など）
        case pml::wazadata::TARGET_SIDE_ENEMY:         ///< 敵側陣営
        case pml::wazadata::TARGET_OTHER_ALL:          ///< 自分以外の全ポケ
          switch( rule )
          {
          case BTL_RULE_SINGLE:
            {
              int tempArr[4]={1,0,1,0};
              mEffectViewerUiInfo.mDeffencePos = tempArr[mSubIdx];
            }
            break;
          case BTL_RULE_DOUBLE:
            {
              int tempArr[4]={3,2,1,0};
              mEffectViewerUiInfo.mDeffencePos = tempArr[mSubIdx];
            }
            break;
          case BTL_RULE_ROYAL:
            {
              int tempArr[4]={2,3,0,1};
              mEffectViewerUiInfo.mDeffencePos = tempArr[mSubIdx];
            }
            break;
          case BTL_RULE_INTRUDE:
            {
              int tempArr[4]={1,0,0,0};
              mEffectViewerUiInfo.mDeffencePos = tempArr[mSubIdx];
            }
            break;
          }
          mEffectViewerUiInfo.mRangeType = EffectViewerUiInfo::WAZA_RANGE_ALL;
          break;
        }

        if( rule == BTL_RULE_INTRUDE )
        {
          if( mEffectViewerUiInfo.mAttackPos == 2 )
          {
            mEffectViewerUiInfo.mAttackPos = 3;
          }
          if( mEffectViewerUiInfo.mDeffencePos == 2 )
          {
            mEffectViewerUiInfo.mDeffencePos = 3;
          }
        }


        int seq_no = mBtlvSystem->GetWazaSequenceFile( mAutoPlayWaza, 0 );
        SetWazaTarget();
        mBtlvSystem->ChangeRule(rule);
        mBtlvSystem->GetWazaParam()->mId = static_cast<btl::WazaID>(mAutoPlayWaza);
        mBtlvSystem->GetWazaParam()->mTurnType = static_cast<btl::BtlvWazaEffect_TurnType>(0);
        mBtlvSystem->SeqComFunc_PlaySequenceFile( seq_no );
        mAutoTestCmdBuf = 0;
        mRenderPathCnt = 0;

        mSdLog.OutputLog("%3d,%d,%d",mAutoPlayWaza,mEffectViewerUiInfo.mAttackPos,mEffectViewerUiInfo.mDeffencePos);
        GFL_PRINT("WazaStart [%d][%d->%d]\n",mAutoPlayWaza,mEffectViewerUiInfo.mAttackPos,mEffectViewerUiInfo.mDeffencePos);
        mSubSeq = WAIT_AUTO_TEST;
      }
      break;
    case WAIT_AUTO_TEST:
      {
        int frm = mBtlvSystem->GetSeqSys()->GetSeqFrame();
        if (frm > 0)
        {
          mEffectViewerUiInfo.mDispFrame = frm;
          u32 cmdBuf = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->mCommandBufferUseSize;
          if( mAutoTestCmdBuf < cmdBuf )
          {
            mAutoTestCmdBuf = cmdBuf;
          }
          int modelNum = mBtlvSystem->GetRenderSys()->GetRenderingPipeLine()->GetModelRenderPath()->GetNodeNum();
          if( mRenderPathCnt < modelNum )
          {
            mRenderPathCnt = modelNum;
          }
        }
        if( mBtlvSystem->SeqComFunc_IsFinishSequenceFile() )
        {
          mSubSeq = GO_NEXT_AUTO_TEST;
          mBtlvSystem->GetBattleSound()->BgmVolumeReset_Waza();
        }
      }
      break;
    case GO_NEXT_AUTO_TEST:
      if( gfl2::debug::DebugWin_IsOpen() )
      {
        break;
      }

      mSdLog.OutputLog("%d,%d\n",mAutoTestCmdBuf,mRenderPathCnt);

      bool isFinish = true;

      if( mEnableAutoTest == true  && 
          mAutoPlayWaza <= mAutoEndWaza )
      {
        int trgNum = 1;
        int dirPatternNum = 1;
        //次への設定
        if( mEnableAllDir )
        {
          BtlRule rule =  mBtlvSystem->GetBattleRule();
          switch( rule )
          {
          case BTL_RULE_SINGLE:
            trgNum = 2;
            break;
          case BTL_RULE_DOUBLE:
          case BTL_RULE_ROYAL:
            trgNum = 4;
            break;
          case BTL_RULE_INTRUDE:
            trgNum = 3;
            break;
          }

          int type = pml::wazadata::GetParam((WazaNo)mAutoPlayWaza,pml::wazadata::PARAM_ID_TARGET);
          switch( type )
          {
          case pml::wazadata::TARGET_USER:               ///< 自分のみ
            dirPatternNum = 1;
            break;
          case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
            dirPatternNum = trgNum;
            break;
          case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
          case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
          case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
          case pml::wazadata::TARGET_ENEMY_RANDOM:       ///< 相手ポケ１体ランダム
            dirPatternNum = trgNum-1;
            break;
          case pml::wazadata::TARGET_OTHER_ALL:          ///< 自分以外の全ポケ
          case pml::wazadata::TARGET_ENEMY_ALL:          ///< 相手側全ポケ
          case pml::wazadata::TARGET_FRIEND_ALL:         ///< 味方側全ポケ
          case pml::wazadata::TARGET_ALL:                ///< 場に出ている全ポケ
            dirPatternNum = 1;
            break;
          // ポケモン以外対象
          case pml::wazadata::TARGET_FIELD:              ///< 場全体（天候など）
          case pml::wazadata::TARGET_SIDE_ENEMY:         ///< 敵側陣営
          case pml::wazadata::TARGET_SIDE_FRIEND:        ///< 自分側陣営
            dirPatternNum = 1;
            break;
          // 特殊
          case pml::wazadata::TARGET_UNKNOWN:            ///< ゆびをふるなど特殊型
            dirPatternNum = 1;
            break;
          }
        }

        mSubDirIdx++;
        if( mSubDirIdx >= dirPatternNum )
        {
          mSubDirIdx = 0;
          mSubIdx++;
          if( mSubIdx >= trgNum )
          {
            mSubIdx = 0;
            mAutoPlayWaza++;
          }
        }

        if( mAutoPlayWaza > mAutoEndWaza )
        {
          isFinish = true;
        }
        else
        {
          isFinish = false;
          mSubSeq = START_AUTO_TEST;
        }
      }
      
      if( isFinish )
      {
        mSdLog.CloseLogFile();
        mEnableAutoTest = false;
        mSubSeq = STANDARD_LOOP;
      }
      break;
    }

  // スタートボタンで抜ける
  if( btn->IsTrigger( gfl2::ui::BUTTON_START ) )
    {
      return true;
    }

  return false;
}

//-------------------------------------------------------------------------
/**
 * @brief 下画面アプリ用のメモリ確保と初期化
 */
//-------------------------------------------------------------------------
void EffectViewerProc::AppParamInit(gfl2::heap::HeapBase *devHeap)
{
  // 下画面アプリ全体用にローカルヒープを作る
  mpAppHeap = GFL_NEW(devHeap) app::util::Heap();
  //  mpAppHeap->LocalHeapCreate(devHeap,devHeap,  0x100000,  0x400000);  //1M+4M
  mpAppHeap->LocalHeapCreate(devHeap,devHeap,  0x100000,  0x200000);  // 1M+2M : どのくらい必要？　


  // Frameクラスのインスタンス作成
  mpFrameManager = GFL_NEW(devHeap) applib::frame::Manager(devHeap);
  EffectViewerLowBgFrame* pFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, EffectViewerLowBgFrame>( mpFrameManager );


  //ファイルマネージャー
  gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();

  // ファイル読み込み用はローカルではなくAPP_DEVICEの後方からとる
  mpAppHeap->SetFileReadHeap( devHeap );



  // RenderingManagerのインスタンス作成
  mpAppRenderingManager  = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();

  // RenderingManagerの初期化
  app::util::AppRenderingManager::Description desc;
  {
    // 多分上画面には描かない
    //    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  }
  mpAppRenderingManager->SyncCreate( desc, mpAppHeap, NULL, pAsyncFileManager );


  // 受け渡し用構造体にストア
  mEffectViewAppParam.mpAsyncFileManager    = pAsyncFileManager;
  mEffectViewAppParam.mpAppRenderingManager = mpAppRenderingManager;
  mEffectViewAppParam.mpAppHeap             = mpAppHeap;
  mEffectViewAppParam.mpAppFrameManager     = mpFrameManager;
  mEffectViewAppParam.mpTmpHeap             = mBtlvSystem->GetTempHeap();   // DebugKeyboad用のヒープはtmpHeapから取る
  mEffectViewAppParam.mpEffViewUiInfo       = &mEffectViewerUiInfo;         // 上画面へのアクション、パラメータ受け渡し用


  // Frameクラスの初期化で使うパラメーターのセット
  pFrame->Setup(&mEffectViewAppParam);

  // UI用情報のストア
  for (int i=0; i< EffectViewerUiInfo::POKEMON_MAX_NUM; i++)
    {
      mEffectViewerUiInfo.mUI_Pp[i] = mBtlvSystem->GetPokemonParam(i);
    }

}

//-------------------------------------------------------------------------
/**
 * @brief 下画面アプリ用のメモリの開放
 */
//-------------------------------------------------------------------------
bool EffectViewerProc::AppParamEnd(void)
{

  if( mpFrameManager->End() == true )
    {
      return false;
    }
  if( !mpAppRenderingManager->CanDestroy() )
    {
      return false;
    }

  GFL_SAFE_DELETE( mpAppRenderingManager );

  GFL_SAFE_DELETE( mpFrameManager );

  GFL_SAFE_DELETE( mpAppHeap );

  return true;
 
}


//-------------------------------------------------------------------------
/**
 * @brief 下画面UIからのアクションチェック
 */
//-------------------------------------------------------------------------
void EffectViewerProc::CheckUiAction(void)
{
  if (mEffectViewerUiInfo.IsExistAction() == false)
    {
      return;
    }

  switch( mEffectViewerUiInfo.mViewerAction )
    {
 
    case EffectViewerUiInfo::ACTION_BGM_PLAY:
      /*
        niji                  : STRM_BGM_VS_TRAINER_NJ
        momijiトレーナー戦    : STRM_BGM_VS_TRAINER_MJ＞
        momiji野生ポケモン戦  : STRM_BGM_MJ_M02＞
      */
      { Sound::StartBGMReq( STRM_BGM_MJ_M02 );}
      break;

    case EffectViewerUiInfo::ACTION_BGM_STOP:
      {Sound::EndBGMReq();}
      break;
      
    case EffectViewerUiInfo::ACTION_POKEMON_SET:
      { SetPokemon(); }
      break;

    case EffectViewerUiInfo::ACTION_WAZA_PLAY:
      {
        SetWazaTarget();
        int seq_no = mBtlvSystem->GetWazaSequenceFile( mEffectViewerUiInfo.mWazaNo, mEffectViewerUiInfo.mWazaSubNo );
        KIJIMA_PRINT ( ">>>>>Play Waza [Main: %d sub:%d] = %d\n",mEffectViewerUiInfo.mWazaNo, mEffectViewerUiInfo.mWazaSubNo, seq_no );
        mBtlvSystem->GetWazaParam()->mId = static_cast<btl::WazaID>(mEffectViewerUiInfo.mWazaNo);
        mBtlvSystem->GetWazaParam()->mTurnType = static_cast<btl::BtlvWazaEffect_TurnType>(mEffectViewerUiInfo.mWazaSubNo);
        mBtlvSystem->SeqComFunc_PlaySequenceFile( seq_no );
        mBtlvSystem->GetBattleSound()->BgmVolumeDown_Waza();
        //      mBtlvSystem->SeqComFunc_PlaySequenceFile( mEffectViewerUiInfo.mWazaNo );
        mAutoTestCmdBuf = 0;
        mRenderPathCnt = 0;
        mSubSeq++;
      }
      break;

    case EffectViewerUiInfo::ACTION_BTL_RULE_SET:
      {
        mBtlvSystem->ChangeRule ( mEffectViewerUiInfo.mBattleRule );
      }
      break;


    case EffectViewerUiInfo::ACTION_BALL_ID_SET:
      {
        mBtlvSystem->SetDebugBallID( 0, mEffectViewerUiInfo.mBall_ID[0] );
        mBtlvSystem->SetDebugBallID( 1, mEffectViewerUiInfo.mBall_ID[1] );
      }
      break;

    case EffectViewerUiInfo::ACTION_TARGET_SET:
      {
        SetWazaTarget();
      }
      break;

    default:
    case EffectViewerUiInfo::ACTION_NONE:
      break;

    }
  

  mEffectViewerUiInfo.ClearAction();

}

//-------------------------------------------------------------------------
/**
 * @brief ポケモンのセット
 */
//-------------------------------------------------------------------------
void EffectViewerProc::SetPokemon()
{

  int slot = mEffectViewerUiInfo.mTargetSlot;

  MonsNo mons_no = mEffectViewerUiInfo.mSelectPokemonNum;
  int    form = mEffectViewerUiInfo.mSelectPokemonForm;
  bool   rare = mEffectViewerUiInfo.mSelectPokemonRare;
  int    sex = mEffectViewerUiInfo.mSelectPokemonSex;

  pml::pokepara::PokemonParam *Pp = mBtlvSystem->GetPokemonParam( slot );
  btl::BTL_POKEPARAM *Bpp = mBtlvSystem->GetBattlePokeParam( (btl::BtlvPos)slot );


  DefaultPowerUpDesc defaultPowerUpDesc;
  DEFAULT_POWERUP_DESC_Clear( &defaultPowerUpDesc );

  // mPp init
  Pp->SetMonsNoDirect( mons_no );

  if( rare )
    {
      Pp->SetRare();
    }
  else
    {
      Pp->SetNotRare();
    }

  // Form
  Pp->ChangeFormNo((FormNo)form);

  // Sex
  Pp->ChangeSex((pml::Sex)sex);

  //  KIJIMA_PRINT (">>>>> Pp(2) : MonsNo = %d\n", Pp->GetMonsNo());

  // mBpp init
  btl::BTL_POKEPARAM::SetupParam setupParam;
  setupParam.srcParam    = Pp;
  setupParam.pokeID      = slot;
  setupParam.friendship  = 0;
  setupParam.fieldStatus = NULL;
  DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

  Bpp->Setup( setupParam );

  //  KIJIMA_PRINT (">>>>> Bpp(2) : MonsNo = %d\n", Bpp->GetMonsNo() );


  // Start load command
  gfl2::heap::HeapBase* heap = mBtlvSystem->GetTempHeap();
  CreateTask_ReloadPokemon( mBtlvSystem->GetTaskSystemAlways(), heap, mBtlvSystem, (btl::BtlvPos)slot );

  mBtlvSystem->GetUiSys()->ResetGaugeParam((btl::BtlvPos)slot);

  mSubSeq = CHANGE_POKE_WAIT;

}

//-------------------------------------------------------------------------
/**
 * @brief ワザの対象をセット
 */
//-------------------------------------------------------------------------
void EffectViewerProc::SetWazaTarget(void)
{

  int atk_pos = mEffectViewerUiInfo.mAttackPos;
  int def_pos = mEffectViewerUiInfo.mDeffencePos;

  mBtlvSystem->SetComTargetReset();
  mBtlvSystem->SetTargetPoke( 0, atk_pos );
  mBtlvSystem->SetTargetPoke( 1, def_pos ); // Why?
  mBtlvSystem->SetTargetTrainer( 0, atk_pos );  //トレーナーの指定追加
  mBtlvSystem->SetTargetTrainer( 1, def_pos ); 

  mBtlvSystem->SetDebugBallID( 0, mEffectViewerUiInfo.mBall_ID[0] );
  mBtlvSystem->SetDebugBallID( 1, mEffectViewerUiInfo.mBall_ID[1] );

  BtlRule b_rule =  mBtlvSystem->GetBattleRule();

  switch( mEffectViewerUiInfo.mRangeType )
    {

    case EffectViewerUiInfo::WAZA_RANGE_SINGLE:
      // ----- ワザ範囲：単体 -----
      {
        mBtlvSystem->SetDamagePoke(0,def_pos);
      }
      break;

    case EffectViewerUiInfo::WAZA_RANGE_SIDE:
      // ----- ワザ範囲：両脇 -----
      {
        switch( b_rule )
          {
          case BTL_RULE_SINGLE:    ///< シングル
            mBtlvSystem->SetDamagePoke(0,def_pos);
            break;
          
          case BTL_RULE_DOUBLE:    ///< ダブル
            switch( def_pos )
              {
              case 0:
                mBtlvSystem->SetDamagePoke(0,0);
                mBtlvSystem->SetDamagePoke(1,2);
                break;
              case 2:
                mBtlvSystem->SetDamagePoke(0,2);
                mBtlvSystem->SetDamagePoke(1,0);
                break;
              case 1:
                mBtlvSystem->SetDamagePoke(0,1);
                mBtlvSystem->SetDamagePoke(1,3);
                break;
              case 3:
                mBtlvSystem->SetDamagePoke(0,3);
                mBtlvSystem->SetDamagePoke(1,1);
                break;
              }
            break;

          case BTL_RULE_INTRUDE:    ///< 乱入
            switch( def_pos )
              {
              case 0:
                mBtlvSystem->SetDamagePoke(0,0);
                break;
              case 3:
                mBtlvSystem->SetDamagePoke(0,3);
                mBtlvSystem->SetDamagePoke(1,1);
                break;
              case 1:
                mBtlvSystem->SetDamagePoke(0,1);
                mBtlvSystem->SetDamagePoke(1,3);
                break;
              }
            break;

          case BTL_RULE_ROYAL:      ///< ロイヤル
            // Targetから見て Self -> Right -> Left の順番？
            switch( def_pos )
              {
              case 0:
                mBtlvSystem->SetDamagePoke(0,0);
                mBtlvSystem->SetDamagePoke(1,1);
                mBtlvSystem->SetDamagePoke(2,3);
                break;
              case 1:
                mBtlvSystem->SetDamagePoke(0,1);
                mBtlvSystem->SetDamagePoke(1,2);
                mBtlvSystem->SetDamagePoke(2,0);
                break;
              case 2:
                mBtlvSystem->SetDamagePoke(0,2);
                mBtlvSystem->SetDamagePoke(1,3);
                mBtlvSystem->SetDamagePoke(2,1);
                break;
              case 3:
                mBtlvSystem->SetDamagePoke(0,3);
                mBtlvSystem->SetDamagePoke(1,0);
                mBtlvSystem->SetDamagePoke(2,2);
                break;
              }
            break;
          }
      }
      break;

    case EffectViewerUiInfo::WAZA_RANGE_ENEMY:
      // ----- ワザ範囲：敵全体 -----
      {
        switch( b_rule )
          {
          case BTL_RULE_SINGLE:    ///< シングル
            {
              mBtlvSystem->SetDamagePoke(0,def_pos);
            }
            break;
          
          case BTL_RULE_DOUBLE:    ///< ダブル
            {
              switch( def_pos )
                {
                case 0:
                  mBtlvSystem->SetDamagePoke(0,0);
                  mBtlvSystem->SetDamagePoke(1,2);
                  break;
                case 2:
                  mBtlvSystem->SetDamagePoke(0,2);
                  mBtlvSystem->SetDamagePoke(1,0);
                  break;
                case 1:
                  mBtlvSystem->SetDamagePoke(0,1);
                  mBtlvSystem->SetDamagePoke(1,3);
                  break;
                case 3:
                  mBtlvSystem->SetDamagePoke(0,3);
                  mBtlvSystem->SetDamagePoke(1,1);
                  break;
                }// end switch
            }
            break;

          case BTL_RULE_ROYAL:      ///< ロイヤル
            {
              mBtlvSystem->SetDamagePoke(0,def_pos);
              int max = 4;
              int idx = 1;
              for( int i=0;i<max;i++ )
                {
                  if( i != atk_pos &&
                      i != def_pos )
                    {
                      mBtlvSystem->SetDamagePoke(idx,i);
                      idx++;
                    }
                }
            }
            break;

          case BTL_RULE_INTRUDE:    ///< 乱入
            {
              switch( def_pos )
                {
                case 0:
                  mBtlvSystem->SetDamagePoke(0,0);
                  break;
                case 3:
                  mBtlvSystem->SetDamagePoke(0,3);
                  mBtlvSystem->SetDamagePoke(1,1);
                  break;
                case 1:
                  mBtlvSystem->SetDamagePoke(0,1);
                  mBtlvSystem->SetDamagePoke(1,3);
                  break;
                }// end switch
            }
            break;
          
          } // end switch
         
      }
      break;
    
    case EffectViewerUiInfo::WAZA_RANGE_ALL:
      // ----- ワザ範囲：全体 -----
      {
      
        int max = 2;
        switch( b_rule )
          {
          case BTL_RULE_SINGLE:    ///< シングル
            max = 2;
            break;

          case BTL_RULE_INTRUDE:    ///< 乱入
            max = 3;
            break;

          case BTL_RULE_DOUBLE:    ///< ダブル
          case BTL_RULE_ROYAL :    ///< ロイヤル
            max = 4;
            break;
          }
      
        mBtlvSystem->SetDamagePoke(0,def_pos);
        int idx = 1;
        for( int i=0;i<max;i++ )
          {
            if( i != atk_pos &&
                i != def_pos )
              {
                mBtlvSystem->SetDamagePoke(idx,i);
                idx++;
              }
          }
        break;
      
      }
    
    }

    for( int i=0;i<4;i++ )
    {
      mBtlvSystem->GetBattlePokeParam(btl::BtlvPos(i))->HpPlus(999);
    }
    mBtlvSystem->GetUiSys()->GaugeUpdateForce();


    for( int i=0;i<btl::BTL_EXIST_POS_MAX;i++ )
    {
      btl::BtlvPos vpos = mBtlvSystem->GetDamagePoke(i);
      if( vpos != btl::BTL_VPOS_ERROR )
      {
        mBtlvSystem->GetBattlePokeParam(vpos)->HpMinus(999);
        mBtlvSystem->GetBattlePokeParam(vpos)->HpPlus(1);
      }
    }
    

}

extern void EffViewDebug_StartAutoTest_CallBack( void *work );

void EffectViewerProc::InitDebugMenu(void)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(heap, L"EffectViewerTest",NULL);
  gfl2::debug::DebugWinItem* item;

  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup,heap, L"AutoTest",&mEnableAutoTest);
  item->SetChangedValueFunc(EffViewDebug_StartAutoTest_CallBack,this);

  item = gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"StartWaza", &mAutoStartWaza , 1, WAZANO_MAX-1 );
  item = gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"EndWaza", &mAutoEndWaza , 1, WAZANO_MAX-1 );
  item = gfl2::debug::DebugWin_AddItemValueInt( topGroup ,heap, L"NowIndex", &mAutoPlayWaza, 1, WAZANO_MAX-1 );
  item = gfl2::debug::DebugWin_AddItemValueBool( topGroup ,heap, L"AllDir", &mEnableAllDir);


}

void EffectViewerProc::TermDebugMenu(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"EffectViewerTest");

}

void EffViewDebug_StartAutoTest_CallBack( void *work )
{
  EffectViewerProc *core = static_cast<EffectViewerProc*>(work);

  if( core->mEnableAutoTest == true )
  {
    core->mAutoPlayWaza = core->mAutoStartWaza;
    core->mSubIdx = 0;
    core->mSubDirIdx = 0;
  }
}

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif//PM_DEBUG

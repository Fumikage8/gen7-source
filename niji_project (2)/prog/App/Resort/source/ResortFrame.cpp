//=============================================================================
/**
 * @file   ResortFrame.cpp
 * @brief   継承したテスト
 * @author k.ohno
 * @date   2014.2.17
 */
//=============================================================================
#pragma once


#include "ResortConst.h"
#include "ResortDraw.h"
#include "ResortFlow.h"
#include "ResortFrame.h"
#include "ResortInitLogic.h"
#include "ResortUtility.h"
#include "lower/ResortLowerAdventureResultDraw.h"
#include "lower/ResortLowerBeansSelectDraw.h"
#include "lower/ResortLowerCourseSelectDraw.h"
#include "lower/ResortLowerFarmDraw.h"
#include "lower/ResortLowerFarmListDraw.h"
#include "lower/ResortLowerGimDraw.h"
#include "lower/ResortLowerKinomiGetEffectDraw.h"
#include "lower/ResortLowerSelectDraw.h"
#include "lower/ResortLowerUIDraw.h"
#include "parts/ResortCharaModel.h"
#include "parts/ResortTalkMessage.h"
#include "upper/ResortUpperBGDraw.h"
#include "upper/ResortUpperDraw.h"
#include "upper/ResortUpperHomeMenuDraw.h"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/islandUpperLyt.gaix>
#include <arc_index/resort.gaix>

#include "AppLib/include/Frame/AppFrameManager.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <Print/include/SystemFont.h>

#include <niji_conv_header/app/island/prog2d/islandup.h>
#include <niji_conv_header/app/island/prog2d/islandup_pane.h>
#include <niji_conv_header/app/island/prog2d/islandup_anim_list.h>

#include <GameSys/include/GameData.h>
#include <Savedata/include/ResortSave.h>

#include <arc_index/fieldwindow.gaix>
#include <niji_conv_header/message/msg_pokeresort_system.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <niji_conv_header/battle/chara/chara_model_id.h>

#include "App/Resort/include/ResortHeapDefine.h"
#include "App/AppEvent/include/PokeIsland/ResortDataManager.h"

#include "ResortDebugInfo.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )


enum{
  MM_SEQ_NONE,
  MM_SEQ_YESNO,
  MM_SEQ_END,
  MM_SEQ_END_WAIT,
};

enum {
  FADE_SEQ_NONE,
  FADE_SEQ_REQ_IN,
  FADE_SEQ_IN,
  FADE_SEQ_IN_WAIT,
  FADE_SEQ_NUM,
};

#define DRAW_OFF_LAB (0)
#define RESORT_SAVE_DEBUG   (0)

//------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 */
//------------------------------------------------------------------
ResortFrame::ResortFrame(void)
  :
  
  // ResortFlowに渡すものはNULL初期化↓
    mpUpperDraw(NULL)
  , mpUpperHomeMenuDraw(NULL)
  , mpLowerUIDraw(NULL)
  , mpLowerSelectDraw(NULL)
  , mpLowerBeansSelectDraw(NULL)
  , mpLowerFarmDraw(NULL)
  , mpLowerFarmListDraw(NULL)
  , mpLowerAdventureResultDraw(NULL)
  , mpLowerGimDraw(NULL)
  , mpLowerCourseSelectDraw(NULL)
  , mpLowerKinomiEffectDraw(NULL)  // きのみエフェクトはResortFlowに渡さないが、独立したDrawなので、ResortFlowに渡すものたちと同じように扱う
  , mSetupAfterCreateDrawFinished(false)
  // ResortFlowに渡すものはNULL初期化↑
  
  , mIsFadeInStart( false )
  , mIsFadeOutStart( false )

{
  GameSys::GameManager* gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  {
    mpAsyncFileManager = gm->GetAsyncFileManager();
  
    GameSys::GameData* gd = gm->GetGameData();
    {
      mpResortSave    = gd->GetResortSave();
      mpBoxPokeSave   = gd->GetBoxPokemon();
      mpItemSave      = gd->GetMyItem();
      mpZukanSave     = gd->GetZukanData();
    }
  }

  mFileSeq        = 0;
  mLoadCount      = 0;
  mReadSeq        = 0;
  mReadModelSeq   = 0;
  mUpdateFrameUI  = 0;
  mFadeSeq        = 0;

  {
    mTouchParam.param = 0;
    mTouchParam.frame = 0;
  }

#if RESORT_SAVE_DEBUG
  for( u32 i=0; i<Savedata::ResortSave::POKEBEANS_KIND_MAX; ++i )
  {
    Savedata::ResortSave::PokeBeansEnum kind = static_cast<Savedata::ResortSave::PokeBeansEnum>(i);
    u8 count = mpResortSave->GetPokeBeansNum( kind );

    FUKUSHIMA_PRINT( "count[%d] -> ", mpResortSave->GetPokeBeansNum( kind ) );

    if( count == 0 )
    {
      mpResortSave->SetPokeBeansNum( kind, count + 32 );
      
      FUKUSHIMA_PRINT( "count[%d]\n", mpResortSave->GetPokeBeansNum( kind ) );
    }
  }
#endif
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
ResortFrame::~ResortFrame()
{
}


void ResortFrame::Setup( APP_PARAM* pAppParam, gfl2::heap::HeapBase* rootHeap )
{
//  mpAppHeap = pAppParam->mpAppHeap;

  static const int SIZE_MY_APP = ResortHeapDefine::FRAME_HEAP_SIZE;       ///  アプリメモリも容量管理のため分けて管理
  static const int SIZE_MY_APPDEV = ResortHeapDefine::FRAME_DEV_SIZE;    /// アプリメモリも容量管理のため分けて管理

  //HEAPID_APP  本体に関するものはAPPめもりーから確保します
  //gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  {
    gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );  // ResortDebugInfo::s_ResortDebugInfo.SetHeapにセットするためにポインタをゲット
    ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_00, devHeap, "HEAPID_APP_DEVICE");
  }
#endif
  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(rootHeap) app::util::Heap();
  mpAppHeap->LocalHeapCreate(rootHeap,rootHeap,SIZE_MY_APP,SIZE_MY_APPDEV);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_01, mpAppHeap->GetSystemHeap(), "FRAME_HEAP_SIZE");
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_02, mpAppHeap->GetDeviceHeap(), "FRAME_DEV_SIZE");
#endif

  mpLayoutHeap = GFL_NEW(rootHeap) app::util::Heap();
  mpLayoutHeap->LocalHeapCreate(
    rootHeap, rootHeap,
    ResortHeapDefine::LAYOUT_HEAP_SIZE, ResortHeapDefine::LAYOUT_DEV_SIZE );

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_10, mpLayoutHeap->GetSystemHeap(), "LAYOUT_HEAP_SIZE");
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_11, mpLayoutHeap->GetDeviceHeap(), "LAYOUT_DEV_SIZE");
#endif

  mpIcon2DHeap = GFL_NEW(rootHeap) app::util::Heap();
  mpIcon2DHeap->LocalHeapCreate(
    rootHeap, rootHeap,
    ResortHeapDefine::ICON_2D_HEAP_SIZE, ResortHeapDefine::ICON_2D_DEV_SIZE );

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_12, mpIcon2DHeap->GetSystemHeap(), "ICON_2D_HEAP_SIZE");
  ResortDebugInfo::s_ResortDebugInfo.SetHeap(ResortDebugInfo::HEAP_DEF_RESORT_13, mpIcon2DHeap->GetDeviceHeap(), "ICON_2D_DEV_SIZE");
#endif

  mpAppRenderingManager = pAppParam->mpAppRenderingManager;
  mpAppParam = pAppParam;
}


//------------------------------------------------------------------
/**
 * @brief    ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

void ResortFrame::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}

enum{
  LOCAL_OPEN_START,
  LOCAL_READ_START,
  LOCAL_READ_WAIT,
  LOCAL_CLOSE_WAIT,
  LOCAL_CLOSE_END,
};


bool ResortFrame::fileReadSequece(void)
{
  bool ret = false;
  switch(mFileSeq)
  {
  case LOCAL_OPEN_START:
    { 
      if(    ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_RESORT_APPLYT) != NULL
          || ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_FIELDWINDOW_APPLYT) != NULL )
      {
        mpArcReadBuff[ARC_RESORT]    = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_RESORT_APPLYT);
        mpArcReadBuff[ARC_FIELD_WND] = ResortDataManager::GetStaticInstance()->GetData(ResortDataManager::DATA_FIELDWINDOW_APPLYT);
        mFileSeq  = LOCAL_CLOSE_END;
        mLytCount = ARC_NUM;
        ret = true;
      }
      break;
    }
  }
  return ret;
}
/*
ResortDataManagerに置き換えたのでコメントアウト。
bool ResortFrame::fileReadSequece(void)
{
  static const u32 c_ArcID[] = {
    ARCID_RESORT,
    ARCID_FIELDWINDOW,
  };
  static const u32 c_DataID[] = {
    GARC_resort_resort_APPLYT,
    GARC_fieldwindow_fieldwindow_applyt_COMP,
  };

  gfl2::fs::AsyncFileManager* pAsyncFileMan = mpAppParam->mpAsyncFileManager;

  app::util::FileAccessor::RequestData frs;

  fileReadStructSetup(&frs, c_ArcID[mLytCount], c_DataID[mLytCount], mpAppHeap, &mpArcReadBuff[mLytCount]);

  switch(mFileSeq){  ///読み込み開始をここに記述
  case LOCAL_OPEN_START:
    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      if(mLytCount == 0)
      {
        ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_FRAME_FILE_OPEN_START);
      }
#endif

      mpArcReadBuff[mLytCount]         = NULL;
      pAsyncFileMan->AddArcFileOpenReq(frs.openr);
    }
    mFileSeq = LOCAL_READ_START;
    //break;
  case LOCAL_READ_START:
    if(  pAsyncFileMan->IsArcFileOpenFinished( frs.openr.arcId ))
    {
      if( mLytCount == ARC_RESORT )
      {
        frs.loadr.heapForCompressed = NULL;
      }
      pAsyncFileMan->AddArcFileLoadDataReq(frs.loadr);
      mFileSeq = LOCAL_READ_WAIT;
    }
    else{
      break;
    }
  case LOCAL_READ_WAIT:
    if(pAsyncFileMan->IsArcFileLoadDataFinished( frs.loadr.ppBuf ) ){

      pAsyncFileMan->AddArcFileCloseReq(frs.closer);
      mFileSeq = LOCAL_CLOSE_WAIT;
    }
    else{
      break;
    }
  case LOCAL_CLOSE_WAIT:
    if(pAsyncFileMan->IsArcFileCloseFinished( frs.openr.arcId ) )
    {
      mLytCount++;

      if( mLytCount == ARC_NUM )
      {
        mFileSeq = LOCAL_CLOSE_END;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_FRAME_FILE_CLOSE_END);
#endif
        return true;
      }
      else {
        mFileSeq = LOCAL_OPEN_START;
      }
    }
    break;
  }
  return false;
}
*/

//------------------------------------------------------------------
/**
 * @brief    メッセージデータの読み込みの流れの管理
 */
//------------------------------------------------------------------
bool ResortFrame::msgDataReadSequece(void)
{
  const u32 c_ArcIDList[MSG_RESORT_NUM] = {
    print::GetMessageArcId(),
    print::GetMessageArcId_Script(),
    print::GetMessageArcId(),
  };
  static const u32 c_DataIDList[MSG_RESORT_NUM] = {
    GARC_message_pokeresort_system_DAT,
    //GARC_message_pokeresort_window_DAT,
    GARC_script_message_pokeresort_talk_DAT,
    GARC_message_iteminfo_DAT,
  };

  // 読み込みがおわっているかどうか
  if( mLoadCount >= MSG_RESORT_NUM )
  {
    return true;
  }

  gfl2::heap::HeapBase* heap = mpAppHeap->GetDeviceHeap();
  u32 arcID  = c_ArcIDList[mLoadCount];
  u32 dataID = c_DataIDList[mLoadCount];

  switch( mReadSeq )
  {
  case 0:
    {
      // ワードセット
      mWordSet = GFL_NEW(heap) print::WordSet( heap );
      GFL_ASSERT( mWordSet );

      // ワードセットローダー
      u32 id = PRELOAD_ITEMNAME;
      mWordSetLoader = GFL_NEW(heap) WordSetLoader( heap, id );
      mWordSetLoader->StartLoad( heap );

      mReadSeq++;
    } //break;

  case 1:
    {
      if( !mWordSetLoader->WaitLoad() ) break;

      mReadSeq++;
    } //break;

  case 2:
    {
      app::util::FileAccessor::FileOpen( arcID, heap, true );
      mReadSeq++;
    } // break

  case 3:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad( arcID, dataID, &mMsgBuff[mLoadCount], heap, false, 4 );
      mReadSeq++;
    } //break;

  case 4:
    {
      if( !app::util::FileAccessor::IsFileLoad( &mMsgBuff[mLoadCount] ) ) break;

      app::util::FileAccessor::FileClose( arcID, heap );
      mReadSeq++;
    } //break;

  case 5:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      // メッセージデータ
      mMsgData[mLoadCount] = GFL_NEW(heap) gfl2::str::MsgData( mMsgBuff[mLoadCount], heap );
      GFL_ASSERT( mMsgData[mLoadCount] );

      mReadSeq++;
    } //break;

  case 6:
    {
      mLoadCount++;

      mReadSeq = 2;
    }
  }

  return false;
}

//------------------------------------------------------------------
/**
 * @brief    キャラモデルの読み込み
 */
//------------------------------------------------------------------
bool ResortFrame::charaModelReadSequence(void)
{
  if( mReadModelSeq == 0 )
  {
    gfl2::heap::HeapBase* heap = mpAppHeap->GetDeviceHeap();
    {
      // キャラモデルクラスの生成
      mpCharaModel = GFL_NEW(heap) ResortCharaModel();
      {
        GFL_ASSERT( mpCharaModel );

        // キャラモデルセットアップパラメータ
        ResortCharaModel::SETUP_PARAM setupParam;
        {
          setupParam.heap         = mpAppHeap;
          setupParam.renMan       = mpAppRenderingManager;
          setupParam.afm          = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
          setupParam.arcID        = ARCID_CHARA_MODEL_BATTLE_NORMAL;
          setupParam.placeType[0] = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY; //!< UPPER_DISPLAY
        }
        // 初期化
        mpCharaModel->Initialize( setupParam );
        // キャラモデルのロード開始
        mpCharaModel->RequestLoad( CHARA_MODEL_ID_BTTR0007_00 );
      }
    }

    mReadModelSeq++;
  }
  else if( mReadModelSeq == 1 )
  {
    // 更新処理
    mpCharaModel->Update();

    // モデルの準備完了待ち
    if( mpCharaModel->IsModelReady() )
    {
      mReadModelSeq++;
    }
  }
  else {
    return true;
  }

  return false;
}




//------------------------------------------------------------------
/**
 * @brief    PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result ResortFrame::InitFunc(void)
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_00);
#endif

  GFL_ASSERT(mpAppParam);  //Setupしないうちに起動してはいけない  128

  // メッセージデータ
  if( !msgDataReadSequece() )       return applib::frame::RES_CONTINUE;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_01);
#endif

  // キャラモデル
  if( !charaModelReadSequence() )   return applib::frame::RES_CONTINUE;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_02);
#endif

  if(fileReadSequece()){
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_03);
#endif

    gfl2::heap::HeapBase* sysHeap = mpAppHeap->GetSystemHeap();
    gfl2::heap::HeapBase* devHeap = mpAppHeap->GetSystemHeap();

    print::SystemFont_CreateNumFont( sysHeap, devHeap );
    print::SystemFont_SetLytSystemNumFont();

#if DRAW_OFF_LAB
    mpResortDraw = GFL_NEW(sysHeap) ResortDraw(mpAppHeap, mpAppRenderingManager);
    mpResortDraw->Init(mpAppParam,mpArcReadBuff[ARC_ISLAND]);
    mpResortDraw->SetTitleViewUIListener(this);
#endif
//    mpMenuSampleDraw->SetMessage( debug_menu_parts_launcher_05 );

    // リゾートの初期化
    {
      mResortIndex = mpAppParam->firstOpenPos;
      
      InitResort();
    }

    {
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_04);
#endif

      // 会話メッセージ
      CreateTalkMessage();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_05);
#endif
      // BG画面(上)
      CreateDraw_UppBG();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_06);
#endif
      // メイン画面(上)
      //CreateDraw_UppMain();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_07);
#endif
      // 管理人メニュー画面(上)
      //CreateDraw_UppHomeMenu();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_08);
#endif
      // UI画面(下)
      //CreateDraw_LowUI();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_09);
#endif
      // 選択画面(下)
      //CreateDraw_LowSelect();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_10);
#endif
      // ポケマメ選択画面(下)
      //CreateDraw_LowBeans();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_11);
#endif
      // はたけ画面(下)
      //CreateDraw_LowFarm();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_12);
#endif
      // きのみリゾートリスト画面(下)
      //CreateDraw_LowFarmList();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_13);
#endif
      // 探検リゾートアイテム確認画面(下)
      //CreateDraw_LowAdvRes();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_14);
#endif
      // うんどうリゾート画面(下)
      //CreateDraw_LowGim();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_15);
#endif
      // コース選択画面(下)
      //CreateDraw_LowCourse();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_16);
#endif
      // きのみエフェクト
      //CreateDraw_LowKinomiEffect();  // メンバ関数updateCreateDrawに任せたのでコメントアウト
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_17);
#endif

      // 最初の３つはつくっておく
      for(s32 drawIndex=0; drawIndex<3; ++drawIndex)
      {
        this->updateCreateDraw();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        if(drawIndex==0) ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_03);
        if(drawIndex==1) ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_04);
        if(drawIndex==2) ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_05);
#endif
      }

      // 画面フロークラス
      CreateResortFlow();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_18);
#endif
    }

    mpAppParam->mpUIView = mpResortDraw;

    // 最初のフローをセット
    SetStartFlow();

    //GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA);

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_B_19);
#endif
    
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief    PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result ResortFrame::UpdateFunc(void)
{
  if( !mSetupAfterCreateDrawFinished )
  {
    bool createDrawFinished = false;
    if(mFadeSeq == FADE_SEQ_IN_WAIT || mFadeSeq == FADE_SEQ_NONE)
    {
      createDrawFinished = this->updateCreateDraw();
    }
    if( createDrawFinished )
    {
      this->setupAfterCreateDraw();
      mpAppParam->inputEnable = true;   //!< 入力を有効に設定
      mpUpperBGDraw->SetInputEnabled( true );
    }
  }

//  const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( mpResortFlow ){
    mpResortFlow->UpdateMenuFlow();
  }
  if(mpResortDraw){
    mpResortDraw->UpdateTree();
  }
  if(mpUpperBGDraw){
    mpUpperBGDraw->UpdateTree();
  }
  // 会話ウィンドウ
  if(mpTalkMessage){
    mpTalkMessage->Update();
  }
  // キャラモデル
  if( mpCharaModel ){
    mpCharaModel->Update();
  }

  // フェードシーケンス
  FadeSequence();
  // UIの更新
  UpdateUI();
  // タッチ後のフロー待機処理
  TouchToFlowWait();
  
  
  if(GetSubSeq()==MM_SEQ_YESNO){
    SetSubSeq(MM_SEQ_NONE);
  }

  if(GetSubSeq()==MM_SEQ_END)
  {
    if( mpAppParam->seqReq == NEXTSEQ_END )
    {
      static const gfl2::math::Vector4 scou(0.0f,0.0f,0.0f,0.0f);
      static const gfl2::math::Vector4 ecou(0.0f,0.0f,0.0f,255.0f);
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &scou, &ecou, 6 );
      mIsFadeOutStart = true;
    }
    else
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
      mIsFadeOutStart = false;
    }
    SetSubSeq(MM_SEQ_END_WAIT);
//    return applib::frame::RES_FINISH;
  }

  if(GetSubSeq()==MM_SEQ_END_WAIT)
  {
    if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) != false )
    {
      return applib::frame::RES_FINISH;
    }
  }


  return applib::frame::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief    PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void ResortFrame::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
///  if(no == gfl2::gfx::CtrDisplayNo::LEFT){
  if(mpResortDraw){
    mpResortDraw->DrawTree(no);
  }
  if(mpUpperBGDraw){
    mpUpperBGDraw->DrawTree(no);
  }
  if(mpTalkMessage){
    mpTalkMessage->Draw(no);
  }
 // }

}

//------------------------------------------------------------------
/**
 * @brief    PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result ResortFrame::EndFunc(void)
{
  if(mpResortDraw){
    if(false == mpResortDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpResortDraw );
  }
  if(mpUpperBGDraw){
    if(false == mpUpperBGDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpUpperBGDraw );
  }
  if(mpUpperHomeMenuDraw){
    if(false == mpUpperHomeMenuDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpUpperHomeMenuDraw );
  }
  if(mpUpperDraw){
    if(false == mpUpperDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpUpperDraw );
  }
  if(mpLowerUIDraw){
    if(false == mpLowerUIDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerUIDraw );
  }
  if(mpLowerSelectDraw){
    if(false == mpLowerSelectDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerSelectDraw );
  }
  if(mpLowerBeansSelectDraw){
    if(false == mpLowerBeansSelectDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerBeansSelectDraw );
  }
  if(mpLowerFarmDraw){
    if(false == mpLowerFarmDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerFarmDraw );
  }
  if(mpLowerFarmListDraw){
    if(false == mpLowerFarmListDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerFarmListDraw );
  }
  if(mpLowerAdventureResultDraw){
    if(false == mpLowerAdventureResultDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerAdventureResultDraw );
  }
  if(mpLowerGimDraw){
    if(false == mpLowerGimDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerGimDraw );
  }
  if(mpLowerCourseSelectDraw){
    if(false == mpLowerCourseSelectDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerCourseSelectDraw );
  }
  if(mpLowerKinomiEffectDraw){
    if(false == mpLowerKinomiEffectDraw->End()){
      return applib::frame::RES_CONTINUE;
    }
    GFL_SAFE_DELETE( mpLowerKinomiEffectDraw );
  }
  if(mpTalkMessage)
  {
    mpTalkMessage->Terminate();
    GFL_SAFE_DELETE( mpTalkMessage );
  }

  GFL_SAFE_DELETE( mpResortFlow );

  if( mpCharaModel )
  {
    if( !mpCharaModel->CanTerminate() ){
      return applib::frame::RES_CONTINUE;
    }
    mpCharaModel->Terminate();
    GFL_SAFE_DELETE( mpCharaModel );
  }
 
  /*
  ResortDataManagerに置き換えたのでコメントアウト。
  for( u32 i=0; i<ARC_NUM; ++i )
  {
    GflHeapFreeMemory( mpArcReadBuff[i] );
  }
  */

  print::SystemFont_ResetNumFontInfo(); //@fix GFNMCat[1083]
  print::SystemFont_DeleteNumFont();

  mpAppParam->mpUIView = NULL;

  for( u8 i=0; i<MSG_RESORT_NUM; ++i )
  {
    GFL_SAFE_DELETE( mMsgData[i] );
    GflHeapFreeMemory( mMsgBuff[i] );
  }
  GFL_SAFE_DELETE( mWordSetLoader );
  GFL_SAFE_DELETE( mWordSet );

  GFL_SAFE_DELETE(mpIcon2DHeap);
  GFL_SAFE_DELETE(mpLayoutHeap);
  GFL_SAFE_DELETE(mpAppHeap);
  
  return applib::frame::RES_FINISH;
}




//--------------------------------------------------------------------------
/**
 * @brief   ボタン押下通知
 */
//--------------------------------------------------------------------------
void ResortFrame::OnButtonAction( int button_no )
{
  if(button_no == 1){
    mpAppParam->inputEnable = false;    //!< 入力を無効に設定
    SetSubSeq( MM_SEQ_END );
    GFL_PRINT("OnButtonAction \n");
  }
  else{
    mbFarMode = true;
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   キー操作通知
 * @param    dir  キー方向
 * @retval  true = 操作実行
 * @retval  false = 操作失敗
 */
//--------------------------------------------------------------------------
bool ResortFrame::OnKeyAction( u32 dir )
{

 // SetSubSeq( MM_SEQ_END );
  return true;

}


//--------------------------------------------------------------------------
/**
 * @brief   リゾートの終了通知
 */
//--------------------------------------------------------------------------
void ResortFrame::OnEndResort( void )
{
  mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_END_RESORT );
}

//--------------------------------------------------------------------------
/**
 * @brief   リゾートの変更
 * @param   dir   移動方向(0: 左、1: 右)
 * @return 切り替え開始したらtrue 
 */
//--------------------------------------------------------------------------
bool ResortFrame::FeedResort( u8 dir )
{
  u8 nowIndex = mResortIndex;
  int lv;

#if PM_DEBUG
  if(mpAppParam->debugCameraMode){
    return false;
  }
#endif

  
  while( 1 )
  {
    if( dir == 0 )
    {
      if( nowIndex == 0 )
      {
        nowIndex = Savedata::ResortSave::RESORT_KIND_MAX - 1;
      }
      else {
        nowIndex--;
      }
    }
    else {
      nowIndex = (nowIndex + 1) % Savedata::ResortSave::RESORT_KIND_MAX;
    }

    lv = mpAppParam->pSave->GetResortLevel(static_cast<Savedata::ResortSave::PokeResortKind>(nowIndex));
    if( lv != 0 ){
      OHNO_PRINT("nowIndex %d \n",nowIndex);
      break;
    }
  }

  // リゾートの設定
  return SetResortID( nowIndex, dir, true );
}

//--------------------------------------------------------------------------
/**
 * @brief   リゾートの設定
 *
 * @param   resortID    リゾートID
 *
 * @return 切り替え開始したらtrue 
 */
//--------------------------------------------------------------------------
bool ResortFrame::SetResortID( u8 resortID,u8 dir, bool bfade  )
{
  if(mpResortChangeIslandListener)
  {
    if(mpResortChangeIslandListener->ChangeStart(resortID, bfade  ))
    {
      // 上画面の反映
      if( mpUpperDraw )
      {
        mpUpperDraw->FeedResort( resortID, dir );
      }

      // リゾートIDの保存
      {
        mResortIndex = resortID;

        if( mpResortFlow )
        {
          mpResortFlow->GetMenuParam()->resortID = resortID;
        }
      }
      return true;
    }
  }
  return false;
}


//--------------------------------------------------------------------------
/**
 * @brief   管理人メニューを開く
 */
//--------------------------------------------------------------------------
void ResortFrame::OpenHomeMenu( void )
{
#if PM_DEBUG
  if(mpAppParam->debugCameraMode){
    return;
  }
#endif
  if( mpResortFlow )
  {
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_HOME_MENU );
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   きのみをうえる
 * @param   fieldIdx    はたけインデックス
 */
//--------------------------------------------------------------------------
void ResortFrame::PlantKinomi( u8 fieldIdx, int itemkind )
{
  if( mpResortInitLogic )
  {
    mpResortInitLogic->PlantKinomi(fieldIdx, itemkind);
  }

  // 上画面への反映
  UpdateUpper( RESORT_ID_FARM );
}


//--------------------------------------------------------------------------
/**
 * @brief   上画面への反映
 */
//--------------------------------------------------------------------------
void ResortFrame::UpdateUpper( u8 resortID )
{
  if( mpUpperDraw )
  {
    mpUpperDraw->UpdateResort( resortID );
  }
}



//--------------------------------------------------------------------------
/**
 * @brief    リゾートの初期化
 */
//--------------------------------------------------------------------------
void ResortFrame::InitResort( void )
{
  mResortMax = 0;

  for( u8 i=0; i<RESORT_ID_MAX; ++i )
  {
    u8 level = mpResortSave->GetResortLevel( static_cast<Savedata::ResortSave::PokeResortKind>(i) );

    if( level > 0 )
    {
      mResortMax++;
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief    最初のフローセット
 */
//--------------------------------------------------------------------------
void ResortFrame::SetStartFlow( void )
{
  switch( mpAppParam->seqReq )
  {
  case NEXTSEQ_START:
    {
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_FIRST_EVENT );
    } break;

  case NEXTSEQ_BOX_GIM:
    {
      // グループインデックスの設定
      mpResortFlow->SetGimGroupIndex( mpAppParam->selectGroup );
      if( mpLowerGimDraw )
      {
        mpLowerGimDraw->SetGroupIndex( mpAppParam->selectGroup );
        mpLowerCourseSelectDraw->SetGimGroupIndex( mpAppParam->selectGroup );
      }
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_UNDOU_SELECT );
    } break;

  case NEXTSEQ_BOX_GIM_RE:
    {
      // グループインデックスの設定
      mpResortFlow->SetGimGroupIndex( mpAppParam->selectGroup );
      if( mpLowerGimDraw )
      {
        mpLowerGimDraw->SetGroupIndex( mpAppParam->selectGroup );
      }
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_UNDOU_TAKE_BACK );
    } break;

  // 図鑑後のフローが無くなった
  //case NEXTSEQ_ZUKAN:
  //  {
  //    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_POKE_GET );
  //  } break;
  default:
    {
      mpLowerUIDraw->SetDisplayType( LowerUIDraw::DISP_TYPE_MAIN );
    }
  }

  if( mpAppParam->seqReq != NEXTSEQ_START )
  {
    RequestFadeIn();
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   UIの更新
 */
//--------------------------------------------------------------------------
void ResortFrame::UpdateUI( void )
{
  mUpdateFrameUI++;

  if( mUpdateFrameUI < UPDATE_UI_FRAME ) return;

  // 上画面の更新
  UpdateUpper();

  switch( mResortIndex )
  {
  case RESORT_ID_FARM:
    {
      if( mpLowerFarmDraw )
      {
        mpLowerFarmDraw->SetupFarmState_Msg();
      }
    } break;

  case RESORT_ID_GIM:
    {
      if( mpLowerGimDraw )
      {
        mpLowerGimDraw->UpdateSetCount();
        mpLowerGimDraw->UpdateEndTime();
      }
    } break;
  }

  mUpdateFrameUI = 0;
}






void ResortFrame::ChageNearMode(void)
{
  if(mpResortDraw){
    mbFarMode = false;
    mpResortDraw->ChageNearMode();
  }
}

void ResortFrame::ChageFarMode(void)
{
  if(mpResortDraw){
    mpResortDraw->ChageFarMode();
  }
}


void ResortFrame::OnTouchToFlowXY(ResortTouchToFlowListener::TouchKind kind,int index, int x, int y)
{
  FUKUSHIMA_PRINT( "OnTouchToFlowXY : kind[%d], index[%d], x[%d], y[%d]\n", kind, index, x, y );

  if( !mpAppParam->inputEnable ) return;  //!< 入力が無効の時ははじく

  switch(kind)
  {
  case ResortTouchToFlowListener::TOUCH_FARM_PLANT_PLACE:
    {
      // きのみ収穫エフェクト
      if( mpLowerKinomiEffectDraw )
      {
        mpLowerKinomiEffectDraw->PlayGetEffect( LowerKinomiGetEffectDraw::ITEM_TYPE_KINOMI, index, x, y );
      }

      OHNO_PRINT("はたけたっち %d %d %d\n",index,x,y);
    } break;

  case ResortTouchToFlowListener::TOUCH_BEANS_BOTTLE:
    {
      // マメビン取得エフェクト
      if( mpLowerKinomiEffectDraw )
      {
        mpLowerKinomiEffectDraw->PlayGetEffect( LowerKinomiGetEffectDraw::ITEM_TYPE_MAME, index, x, y );
      }
      // フラグをおとす
      mpAppParam->pSave->SetBeansBottleGround( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos  , false);
    } break;
  }
}


void ResortFrame::setMinPokeBeans(int kindmin,int kindmax)
{
  int i;
  u8 minno=255;
  u8 num;
  int keepindex=-1;

  for(i=kindmin;i<=kindmax;i++){
    num = mpAppParam->pSave->GetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(i));
    if(minno > num){
      minno=num;
      keepindex=i;
    }
  }
  if(keepindex!=-1){
    mpAppParam->pSave->SetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(keepindex),1);
  }
}



void ResortFrame::setPokeBeans(u8 mame)
{
  int i;
  u8 minno=255;
  u8 num;
  int keepindex=-1;
  
  if(Savedata::ResortSave::POKEBEANS_KIND_NUM_MAX ==  mpAppParam->pSave->GetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(mame))){
    if(mame <=Savedata::ResortSave::POKEBEANS_NORMAL_07){
      setMinPokeBeans(Savedata::ResortSave::POKEBEANS_NORMAL_01, Savedata::ResortSave::POKEBEANS_NORMAL_07);
    }
    else if(mame <=Savedata::ResortSave::POKEBEANS_HEART_07){
      setMinPokeBeans(Savedata::ResortSave::POKEBEANS_HEART_01, Savedata::ResortSave::POKEBEANS_HEART_07);
    }
  }
  else{
    mpAppParam->pSave->SetPokeBeansNum(static_cast<Savedata::ResortSave::PokeBeansEnum>(mame),1);
  }

}


void ResortFrame::OnTouchToFlow(ResortTouchToFlowListener::TouchKind kind,int param)
{
  FUKUSHIMA_PRINT( "OnTouchToFlow : kind[%d], param[%d]\n", kind, param );

  if( !mpAppParam->inputEnable ) return;  //!< 入力が無効の時ははじく
  if( mTouchParam.frame > 0 ) return;

  switch(kind)
  {
  case ResortTouchToFlowListener::TOUCH_BEANS_BOX:
    {
      if( !mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) ) break;
    } // no break;
  case ResortTouchToFlowListener::TOUCH_FARM_IMPUT_LIST:
  case ResortTouchToFlowListener::TOUCH_ADVENTURE:
  case ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_GIM:
  case ResortTouchToFlowListener::TOUCH_GIM:
  case ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_HOTSPA:
    {
      mTouchParam.kind  = kind;
      mTouchParam.param = param;
      mTouchParam.frame = TOUCH_WAIT_FRAME;
    } break;

  // きのみリゾート：はたけ
  case ResortTouchToFlowListener::TOUCH_KINOMI_FARM:
    {
      if( mpLowerFarmDraw )
      {
        mpLowerFarmDraw->SetKinomiIndex( param );
      }
      if( mpLowerFarmListDraw )
      {
        mpLowerFarmListDraw->SetFieldIndex( static_cast<u8>(param / 6) );
      }
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_FARM_FIELD );
    } break;

  case ResortTouchToFlowListener::TOUCH_POKEMON:
    {
      // チュートリアル中にポケモンイベントに遷移すると面倒なのでここではじいた
      if( !mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) )
      {
        mpResortSequenceListener->ClosePokemonTouchMenu();
        break;
      }

      // レコード
      Savedata::IncRecord( Savedata::Record::RECID_POKEMON_TOUCH ); // レコード：ポケモンをタッチした回数

      if( param < 18 )    //!< ボックスポケモン
      {
        u8 resortID = mResortIndex;

        mpResortFlow->GetMenuParam()->pokemonIndex = param;

        mpResortFlow->GetMenuParam()->pokeCoreParam->CopyFrom(  *mpAppParam->pPokeParty->GetMemberPointer(0) );
          
        /*
        void (Savedata::ResortSave::*GET_POKEMON[RESORT_ID_MAX])(int, pml::pokepara::CoreParam*) = {
          &Savedata::ResortSave::GetFriendShipPokemon,    //!< ふれあい
          &Savedata::ResortSave::GetFarmPokemon,          //!< きのみ
          &Savedata::ResortSave::GetAdventurePokemon,     //!< 探検
          &Savedata::ResortSave::GetGimPokemon,           //!< うんどう
          &Savedata::ResortSave::GetHotSpaPokemon,        //!< 温泉
        };
        (mpResortSave->*GET_POKEMON[resortID])( param, mpResortFlow->GetMenuParam()->pokeCoreParam );
           */

        if( resortID == RESORT_ID_HOTSPA )
        {
          pml::pokepara::CoreParam* cp = mpResortFlow->GetMenuParam()->pokeCoreParam;
          bool isEgg = util::IsEgg( cp );

          if( isEgg )
          {
            mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_ONSEN_EGG_STATE );
          }
          else {
            mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_BOX_POKE_EVENT );
          }
        }
        else {
          mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_BOX_POKE_EVENT );
        }
      }
      else {
        u8 index = param - 18;

        mpResortFlow->GetMenuParam()->pokemonIndex = index;

        mpResortSave->GetWildPokemon( index, mpResortFlow->GetMenuParam()->pokeCoreParam );

        mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_WILD_POKE_EVENT );
      }
    } break;

  case ResortTouchToFlowListener::TOUCH_BEANS:
    {
      u8 mame = mpAppParam->pSave->GetGroundBeans(param);
      setPokeBeans(mame);
      Savedata::IncRecord( Savedata::Record::RECID_MAME_GET_CNT );                  // レコード：ポケマメをあつめた数
      Savedata::IncRecord( Savedata::Record::RECID_GLOBAL_ATTRACTION_POKEMAME );    // レコード：グローバルアトラクション
    }
    break;

  case ResortTouchToFlowListener::TOUCH_BEANS_BOTTLE:
    //まめをはいち
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_01,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_02,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_03,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_04,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_05,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_06,1);
    mpAppParam->pSave->SetPokeBeansNum(Savedata::ResortSave::POKEBEANS_NORMAL_07,1);
    // フラグをおとす
    mpAppParam->pSave->SetBeansBottleGround( (Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos  , false);

    break;

  case ResortTouchToFlowListener::TOUCH_BOARD:
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_MAME_BOX_FUREAI );
    break;

  // チュートリアル
  case ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_BEANS:
    if( mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS ) )
    {
      break;
    }
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_TUTORIAL_1_START );  // ポケマメゲット演出が終わるまでメニュー画面にはいかないようにするためFLOW_TYPE_TUTORIAL_1_STARTへ遷移する。  // kawa_resort_modify
    //@fix GFNMCat[5604]：余分な加算のため削除
    //Savedata::IncRecord( Savedata::Record::RECID_MAME_GET_CNT );                  // レコード：ポケマメをあつめた数
    //Savedata::IncRecord( Savedata::Record::RECID_GLOBAL_ATTRACTION_POKEMAME );    // レコード：グローバルアトラクション
    break;

  case ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_TREE:
    if( mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ) ||
        !mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BEANS))
    {
      break;
    }
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_TUTORIAL_2 );
    break;

  case ResortTouchToFlowListener::TOUCH_TUTORIAL_STATE_TOUCH_BOX:
    if( mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_BOX ) ||
      !mpResortSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE))
    {
      break;
    }
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_TUTORIAL_3 );
    break;

  case ResortTouchToFlowListener::ADVENTURE_INANIM_END:
    {
      // @fix GFNMCat[2718]
      mpUpperDraw->UpdateResort();
      mpUpperDraw->SetupResortOther_Adventure();
    } break;
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   タッチ後の待機
 */
//--------------------------------------------------------------------------
void ResortFrame::TouchToFlowWait( void )
{
  if( mTouchParam.frame == 0 ) return;

  // 待機フレームをカウント
  mTouchParam.frame--;

  // 待機中
  if( mTouchParam.frame > 0 ) return;

  // 待機終了
  int param = mTouchParam.param;

  switch( mTouchParam.kind )
  {
  // ポケマメ
  case ResortTouchToFlowListener::TOUCH_BEANS_BOX:
    {
      ResortFlow::FlowType c_MameBoxFlowType[] = {
        ResortFlow::FLOW_TYPE_MAME_BOX_FUREAI,
        ResortFlow::FLOW_TYPE_MAME_BOX_KINOMI,
        ResortFlow::FLOW_TYPE_MAME_BOX_TANKEN,
        ResortFlow::FLOW_TYPE_MAME_BOX_UNDOU,
        ResortFlow::FLOW_TYPE_MAME_BOX_ONSEN,
      };
      mpResortFlow->StartMenuFlow( c_MameBoxFlowType[mResortIndex] );
    }
    break;

  // きのみリゾート：かんばん
  case ResortTouchToFlowListener::TOUCH_FARM_IMPUT_LIST:
    {
      if( mpLowerFarmListDraw )
      {
        mpLowerFarmListDraw->SetFieldIndex( param );
      }
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_FARM_LIST );
    } break;

  // たんけんリゾート
  case ResortTouchToFlowListener::TOUCH_ADVENTURE:
    if(param==0)
    {
      // たんけんコース選択
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_TANKEN_SELECT );
    }
    else{
      // たんけん結果
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_TANKEN_RESULT );
    } break;

  // うんどうリゾート：
  case ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_GIM:
  // うんどうリゾート：
  case ResortTouchToFlowListener::TOUCH_GIM:
    {
      // グループを設定
      mpAppParam->selectGroup = param;  //!< 選択したグループ番号を登録
      mpLowerGimDraw->SetGroupIndex( param );
      mpResortFlow->SetGimGroupIndex(param);
      mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_UNDOU_STATE );
    }
    break;

  // おんせんリゾート：ポケモン選択
  case ResortTouchToFlowListener::TOUCH_POKEMON_SELECT_HOTSPA:
    mpAppParam->selectGroup = param;  //!< 選択したグループ番号を登録
    mpResortFlow->StartMenuFlow( ResortFlow::FLOW_TYPE_ONSEN_POKE_SELECT );
    // レコード：回数をカウント
    Savedata::IncRecord( Savedata::Record::RECID_RESORT_POKAPOKA );
    break;
  }
}




void ResortFrame::OnCallBoxGim( u8 nextSeq )
{
  OnCallResortEnd();
  mpAppParam->selectGroup = mpResortFlow->GetGimGroupIndex();
  mpAppParam->seqReq      = nextSeq;
}

void ResortFrame::OnCallBoxHotSpa( void )
{
  OnCallResortEnd();
  mpAppParam->seqReq = NEXTSEQ_BOX_HOTSPA;
}

void ResortFrame::OnCallZukan( void )
{
  OnCallResortEnd();
  mpAppParam->seqReq = NEXTSEQ_ZUKAN;
}

void ResortFrame::OnCallStatus( void )
{
  OnCallResortEnd();
  mpAppParam->seqReq = NEXTSEQ_STATUS;
}

void ResortFrame::OnCallEggDemo( void )
{
  OnCallResortEnd();
  mpAppParam->seqReq = NEXTSEQ_TAMAGO;
}

void ResortFrame::OnUpdateResort( void )
{
  InitResort();
}

void ResortFrame::OnCallResortEnd( void )
{
  if( mpLowerUIDraw ) mpLowerUIDraw->SetInputEnabled( false );

  mpAppParam->inputEnable = false;    //!< 入力を無効に設定
  mpAppParam->seqReq = NEXTSEQ_END;

  SetSubSeq( MM_SEQ_END );
}

void ResortFrame::ChangeResort( u8 resortID )
{

  SetResortID( resortID );
}

void ResortFrame::RequestFadeIn( void )
{
  mFadeSeq = FADE_SEQ_REQ_IN;
}



//--------------------------------------------------------------------------
/**
 * @brief   フェードシーケンス
 */
//--------------------------------------------------------------------------
void ResortFrame::FadeSequence( void )
{
  switch( mFadeSeq )
  {
  case FADE_SEQ_REQ_IN:
    {
      mFadeSeq = FADE_SEQ_IN;
      mIsFadeInStart = true;
    } break;

  case FADE_SEQ_IN:
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);

      mFadeSeq = FADE_SEQ_IN_WAIT;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_FADE_IN_START);
#endif
    } // no break;

  case FADE_SEQ_IN_WAIT:
    {
      gfl2::Fade::FadeManager* fadeMan = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

      if( fadeMan->IsEnd( gfl2::Fade::DISP_UPPER ) && fadeMan->IsEnd( gfl2::Fade::DISP_LOWER ) )
      {
        //mpAppParam->inputEnable = true;   //!< 入力を有効に設定  // メンバ関数updateCreateDrawが終了するまで、入力を有効にしないためコメントアウト。
        mFadeSeq = FADE_SEQ_NONE;

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
        ResortDebugInfo::s_ResortDebugInfo.PrintTime();
        ResortDebugInfo::s_ResortDebugInfo.ReleasePrintTime();
        ResortDebugInfo::s_ResortDebugInfo.PrintHeap();
#endif
      }
    } break;
  }
}

bool ResortFrame::IsFadeInStart(void)
{
  return mIsFadeInStart;
}

bool ResortFrame::IsFadeOutStart(void)
{
  return mIsFadeOutStart;
}





//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：BG画面(上)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_UppBG( void )
{
  // 生成パラメータ
  UpperBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
  }

  // 初期化パラメータ
  UpperBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
  }
  
  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpUpperBGDraw = GFL_NEW(heap) UpperBGDraw( createParam );
    {
      GFL_ASSERT( mpUpperBGDraw );
    
      mpUpperBGDraw->Init( initParam );
      mpUpperBGDraw->SetInputEnabled( false );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：メイン画面(上)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_UppMain( void )
{
  // 生成パラメータ
  UpperBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.zukanSave   = mpZukanSave;
    createParam.initLogic   = mpResortInitLogic;
  }

  // 初期化パラメータ
  UpperBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }
  
  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpUpperDraw = GFL_NEW(heap) UpperDraw( createParam );
    {
      GFL_ASSERT( mpUpperDraw );
      
      mpUpperDraw->SetWordSetLoader( mWordSetLoader );
      mpUpperDraw->SetFirstResortID( mResortIndex );
      mpUpperDraw->Init( initParam );
      
      mpUpperBGDraw->AddSubView( mpUpperDraw );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：管理人メニュー
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_UppHomeMenu( void )
{
  // 生成パラメータ
  UpperBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap       = mpLayoutHeap;
    createParam.renderMan     = mpAppRenderingManager;
    createParam.fileMan       = mpAsyncFileManager;
    createParam.resortSave    = mpResortSave;
    createParam.boxPokeSave   = mpBoxPokeSave;
  }

  // 初期化パラメータ
  UpperBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }
  
  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpUpperHomeMenuDraw = GFL_NEW(heap) UpperHomeMenuDraw( createParam );
    {
      GFL_ASSERT( mpUpperHomeMenuDraw );
      
      mpUpperHomeMenuDraw->SetCharaModel( mpCharaModel );
      mpUpperHomeMenuDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpUpperHomeMenuDraw );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：UI画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowUI( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerUIDraw = GFL_NEW(heap) LowerUIDraw( createParam );
    {
      GFL_ASSERT( mpLowerUIDraw );

      mpLowerUIDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerUIDraw->SetTitleViewUIListener( this );
      mpLowerUIDraw->SetSequenceListener( mpResortSequenceListener );
      mpLowerUIDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerUIDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：選択画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowSelect( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerSelectDraw = GFL_NEW(heap) LowerSelectDraw( createParam );
    {
      GFL_ASSERT( mpLowerSelectDraw );

      mpLowerSelectDraw->SetTitleViewUIListener( this );
      mpLowerSelectDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerSelectDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerSelectDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：ポケマメ選択画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowBeans( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.initLogic   = mpResortInitLogic;
    createParam.resortSave  = mpResortSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerBeansSelectDraw = GFL_NEW(heap) LowerBeansSelectDraw( createParam );
    {
      GFL_ASSERT( mpLowerBeansSelectDraw );

      mpLowerBeansSelectDraw->SetTitleViewUIListener( this );
      mpLowerBeansSelectDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerBeansSelectDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerBeansSelectDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：はたけ画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowFarm( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerFarmDraw = GFL_NEW(heap) LowerFarmDraw( createParam );
    {
      GFL_ASSERT( mpLowerFarmDraw );

      mpLowerFarmDraw->SetTitleViewUIListener( this );
      mpLowerFarmDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerFarmDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerFarmDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：植えるきのみのリスト画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowFarmList( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.itemSave    = mpItemSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.msgItem   = mMsgData[MSG_RESORT_ITEM_INFO];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerFarmListDraw = GFL_NEW(heap) LowerFarmListDraw( createParam );
    {
      GFL_ASSERT( mpLowerFarmListDraw );

      mpLowerFarmListDraw->SetTitleViewUIListener( this );
      mpLowerFarmListDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerFarmListDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerFarmListDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：アドベンチャー結果画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowAdvRes( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.itemSave    = mpItemSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.msgItem   = mMsgData[MSG_RESORT_ITEM_INFO];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerAdventureResultDraw = GFL_NEW(heap) LowerAdventureResultDraw( createParam );
    {
      GFL_ASSERT( mpLowerAdventureResultDraw );

      mpLowerAdventureResultDraw->SetTitleViewUIListener( this );
      mpLowerAdventureResultDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerAdventureResultDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerAdventureResultDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：うんどう状態画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowGim( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.initLogic   = mpResortInitLogic;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff   = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys    = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet   = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerGimDraw = GFL_NEW(heap) LowerGimDraw( createParam );
    {
      GFL_ASSERT( mpLowerGimDraw );

      mpLowerGimDraw->SetTitleViewUIListener( this );
      mpLowerGimDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerGimDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerGimDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：コース選択画面(下)
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowCourse( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.boxPokeSave = mpBoxPokeSave;
    createParam.initLogic   = mpResortInitLogic;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys  = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerCourseSelectDraw = GFL_NEW(heap) LowerCourseSelectDraw( createParam );
    {
      GFL_ASSERT( mpLowerCourseSelectDraw );

      mpLowerCourseSelectDraw->SetTitleViewUIListener( this );
      mpLowerCourseSelectDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerCourseSelectDraw->SetTalkMessage( mpTalkMessage );
      mpLowerCourseSelectDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerCourseSelectDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   Drawの生成：きのみ取得エフェクト
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateDraw_LowKinomiEffect( void )
{
  // 生成パラメータ
  LowerBaseDraw::CreateParam createParam;
  {
    createParam.lytHeap     = mpLayoutHeap;
    createParam.iconHeap    = mpIcon2DHeap;
    createParam.renderMan   = mpAppRenderingManager;
    createParam.resortSave  = mpResortSave;
    createParam.itemSave    = mpItemSave;
  }

  // 初期化パラメータ
  LowerBaseDraw::InitParam initParam;
  {
    initParam.lytBuff = mpArcReadBuff[ARC_RESORT];
    initParam.msgSys  = mMsgData[MSG_RESORT_SYS];
    initParam.wordSet = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpLowerKinomiEffectDraw = GFL_NEW(heap) LowerKinomiGetEffectDraw( createParam );
    {
      GFL_ASSERT( mpLowerKinomiEffectDraw );

      mpLowerKinomiEffectDraw->SetTitleViewUIListener( this );
      mpLowerKinomiEffectDraw->SetWordSetLoader( mWordSetLoader );
      mpLowerKinomiEffectDraw->Init( initParam );

      mpUpperBGDraw->AddSubView( mpLowerKinomiEffectDraw );
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   フローの生成
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateResortFlow( void )
{
  ResortFlow::DrawList drawList;
  {
    // ResortFlowに渡すものはNULL初期化しておいて下さい↓
    drawList.homeMenu       = mpUpperHomeMenuDraw;
    drawList.upperMenu      = mpUpperDraw;
    drawList.lowerUI        = mpLowerUIDraw;
    drawList.lowerSelect    = mpLowerSelectDraw;
    drawList.lowerGim       = mpLowerGimDraw;
    drawList.lowerFarmList  = mpLowerFarmListDraw;
    drawList.lowerFarm      = mpLowerFarmDraw;
    drawList.lowerCourseSel = mpLowerCourseSelectDraw;
    drawList.lowerBeansSel  = mpLowerBeansSelectDraw;
    drawList.lowerAdvRes    = mpLowerAdventureResultDraw;
    drawList.seqListener    = mpResortSequenceListener;
    drawList.flowListener   = this;
    drawList.islandListener = mpResortChangeIslandListener;
    // mpLowerKinomiEffectDraw  // きのみエフェクトはResortFlowに渡さないが、独立したDrawなので、ResortFlowに渡すものたちと同じように扱う
    // ResortFlowに渡すものはNULL初期化しておいて下さい↓
  }

  gfl2::heap::HeapBase* heap = mpAppHeap->GetSystemHeap();
  {
    mpResortFlow = GFL_NEW(heap) ResortFlow( drawList, mpAppHeap, mWordSet );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_00);
#endif
    {
      GFL_ASSERT( mpResortFlow );
      
      mpResortFlow->SetTalkMessage( mpTalkMessage );
      // 初期リゾートの設定
      mpResortFlow->GetMenuParam()->resortID = mResortIndex;    // @fix NMCat[1659]
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_01);
#endif
      mpResortFlow->SetInitLogic( mpResortInitLogic );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
      ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_D_02);
#endif
    }
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   会話メッセージの生成
 */
//--------------------------------------------------------------------------
void ResortFrame::CreateTalkMessage( void )
{
  TalkMessage::CreateParam createParam;
  {
    createParam.heap        = mpLayoutHeap;
    createParam.renderMan   = mpAppRenderingManager;
  }
  
  TalkMessage::InitParam initParam;
  {
    initParam.lytBuff = mpArcReadBuff[ARC_FIELD_WND];
    initParam.msgWnd  = mMsgData[MSG_RESORT_WND];
    initParam.wordSet = mWordSet;
  }

  gfl2::heap::HeapBase* heap = mpLayoutHeap->GetSystemHeap();
  {
    mpTalkMessage = GFL_NEW(heap) TalkMessage( createParam );
    {
      GFL_ASSERT( mpTalkMessage );
      
      mpTalkMessage->Initialize( initParam );
      // キャラモデルリスナーの登録
      mpTalkMessage->SetCharaModelListener( mpCharaModel );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   表示中の施設番号を取得
 * @return  施設番号
 */
//--------------------------------------------------------------------------
u8 ResortFrame::GetResortIndex(void)
{
  return mResortIndex;
}


//--------------------------------------------------------------------------
//
//! @brief  Drawの生成を進める
//
//--------------------------------------------------------------------------
bool ResortFrame::updateCreateDraw(void)
{
  if( mSetupAfterCreateDrawFinished ) return true;

  typedef void (ResortFrame::*CreateDrawFuncType)(void);
  CreateDrawFuncType createDrawFunc[] =
  {
    // [上画面][いつも]勝手に出るもの
    &ResortFrame::CreateDraw_UppMain,
    &ResortFrame::CreateDraw_UppHomeMenu,

    // [下画面][いつも]勝手に出るもの
    &ResortFrame::CreateDraw_LowUI,
    // [下画面][運動]ユーザがポケモンを選択した後に戻ってきたら勝手に出るもの
    &ResortFrame::CreateDraw_LowCourse,
    
    // [下画面]ユーザが操作した後に出るもの
    &ResortFrame::CreateDraw_LowKinomiEffect,
    &ResortFrame::CreateDraw_LowSelect,
    &ResortFrame::CreateDraw_LowBeans,
    &ResortFrame::CreateDraw_LowFarm,
    &ResortFrame::CreateDraw_LowFarmList,
    &ResortFrame::CreateDraw_LowAdvRes,
    &ResortFrame::CreateDraw_LowGim,
  };
  void* drawVar[] =
  {
    mpUpperDraw,
    mpUpperHomeMenuDraw,
    
    mpLowerUIDraw,
    mpLowerCourseSelectDraw,
    
    mpLowerKinomiEffectDraw,
    mpLowerSelectDraw,
    mpLowerBeansSelectDraw,
    mpLowerFarmDraw,
    mpLowerFarmListDraw,
    mpLowerAdventureResultDraw,
    mpLowerGimDraw,  
  };

  bool ret = false;

  // 1回呼ぶと、1個生成する。
  s32 drawIndex = 0;
  s32 drawNum = sizeof(drawVar)/sizeof(drawVar[0]);
  for(drawIndex=0; drawIndex<drawNum; ++drawIndex)
  {
    if(drawVar[drawIndex] == NULL)
    {
      (this->*createDrawFunc[drawIndex])();
      break;
    }
  }
  if( drawIndex >= drawNum-1 ) ret = true;
  
  return ret;
}

//--------------------------------------------------------------------------
//
//! @brief  Drawの生成が終わった後のセットアップ
//
//--------------------------------------------------------------------------
void ResortFrame::setupAfterCreateDraw(void)
{
  if( mSetupAfterCreateDrawFinished ) return;

  {
    // void ResortFrame::CreateResortFlow( void ) より移動したセットアップ
    ResortFlow::DrawList drawList;
    {
      // ResortFlowに渡すものはNULL初期化しておいて下さい↓
      drawList.homeMenu       = mpUpperHomeMenuDraw;
      drawList.upperMenu      = mpUpperDraw;
      drawList.lowerUI        = mpLowerUIDraw;
      drawList.lowerSelect    = mpLowerSelectDraw;
      drawList.lowerGim       = mpLowerGimDraw;
      drawList.lowerFarmList  = mpLowerFarmListDraw;
      drawList.lowerFarm      = mpLowerFarmDraw;
      drawList.lowerCourseSel = mpLowerCourseSelectDraw;
      drawList.lowerBeansSel  = mpLowerBeansSelectDraw;
      drawList.lowerAdvRes    = mpLowerAdventureResultDraw;
      drawList.seqListener    = mpResortSequenceListener;
      drawList.flowListener   = this;
      drawList.islandListener = mpResortChangeIslandListener;
      // mpLowerKinomiEffectDraw  // きのみエフェクトはResortFlowに渡さないが、独立したDrawなので、ResortFlowに渡すものたちと同じように扱う
      // ResortFlowに渡すものはNULL初期化しておいて下さい↓
    }
 
    mpResortFlow->SetDrawList(drawList);
  }

  {
    // void ResortFrame::SetStartFlow( void ) より移動したセットアップ
    switch( mpAppParam->seqReq )
    {
    case NEXTSEQ_BOX_GIM:
      {
        // グループインデックスの設定
        mpResortFlow->SetGimGroupIndex( mpAppParam->selectGroup );
        if( mpLowerGimDraw )
        {
          mpLowerGimDraw->SetGroupIndex( mpAppParam->selectGroup );
          mpLowerCourseSelectDraw->SetGimGroupIndex( mpAppParam->selectGroup );
        }
      } break;
    case NEXTSEQ_BOX_GIM_RE:
      {
        // グループインデックスの設定
        mpResortFlow->SetGimGroupIndex( mpAppParam->selectGroup );
        if( mpLowerGimDraw )
        {
          mpLowerGimDraw->SetGroupIndex( mpAppParam->selectGroup );
        }
      } break;
    }
  }

  mSetupAfterCreateDrawFinished = true;
}



void ResortFrame::ResortInputEnabled( bool bflg )
{
  if( mpLowerUIDraw )
  {
    mpLowerUIDraw->SetInputEnabled( bflg );
  }
}


void ResortFrame::ButtonDisplayTypeEnabled( bool bflg )
{
  if( mpLowerUIDraw )
  {
    if(bflg){
      mpLowerUIDraw->SetDisplayType(LowerUIDraw::DISP_TYPE_MAIN);
    }
    else{
      mpLowerUIDraw->SetDisplayType(LowerUIDraw::DISP_TYPE_INPUT_WAIT_NO_MASK);
    }
  }
}


bool ResortFrame::IsAnimEndGetEffect( void )
{
  if(mpLowerKinomiEffectDraw){
    return mpLowerKinomiEffectDraw->IsAnimEndGetEffect();
  }
  return true;
}

bool ResortFrame::IsButtonIdleState( void )
{
  if( mpLowerUIDraw )
  {
    if( mpLowerUIDraw->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE ){
      return true;
    }
  }
  return false;
}




GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )



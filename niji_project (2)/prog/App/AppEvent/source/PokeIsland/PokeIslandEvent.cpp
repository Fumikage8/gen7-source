//===================================================================
/**
 *	@file	    PokeIslandEvent.cpp
 *	@brief    ポケモンアイランドのイベント
 *	@author	  k.ohno
 *	@date		  2015.6.13
 */
//===================================================================

#include "App/AppEvent/include/PokeIsland/PokeIslandEvent.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>

#include "App/Resort/include/ResortProc.h"
#include "App/Resort/include/ResortAppParam.h"
#include <GameSys/include/DllProc.h>


#include "Savedata/include/ResortSave.h"
#include "System/include/HeapDefine.h"
#include <Sound/include/Sound.h>
#include <niji_conv_header/sound/SoundMiddleID.h>
#include "App/AppEvent/include/Box/BoxEvent.h"
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"
#include "NetStatic/NetEvent/include/EggHatchingEvent.h"
#include "PokeTool/include/PokeToolPokeGet.h"
#include "PokeTool/include/PokeToolPokePlace.h"
#include "PokeTool/include/PokeToolForm.h"

#include <Savedata/include/PokeDiarySave.h>

#include "App/FieldDemo/include/FieldDemoProc.h"
#include <arc_index/demo_seq_script.gaix>

#include <Field/FieldStatic/include/FieldArea.h>
#include "App/AppEvent/include/PokeIsland/ResortDataManager.h"


#define NIJI_APP_EVENT_RESORT_HEAP_CHECK (PM_DEBUG&&(0))  // コミットするときは必ず0で


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )


static App::Resort::ResortDataManager s_resortDataManager;


//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  EVSEQ_FLY_DEMO_CALL = 0,
  EVSEQ_FLY_DEMO_WAIT,
  EVSEQ_INIT,
  EVSEQ_INIT_TEST,
  EVSEQ_WAIT_TEST,
  EVSEQ_JUMP_BOX,
  EVSEQ_LOOP_BOX_GIM,
  EVSEQ_LOOP_BOX_GIM_RE,
  EVSEQ_LOOP_BOX_HOTSPA,
  EVSEQ_CALL_STATUS,
  EVSEQ_WAIT_STATUS,
  EVSEQ_CALL_ITEM,
  EVSEQ_WAIT_ITEM,
  EVSEQ_JUMP_REG_ZUKAN,
  EVSEQ_JUMP_REG_ZUKAN_WAIT,
  EVSEQ_JUMP_EGG_HATCHING,
  EVSEQ_LOOP_EGG_HATCHING,
  EVSEQ_JUMP_TITLE,
  EVSEQ_END,
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
 */
//-----------------------------------------------------------------------------

#if PM_DEBUG
bool PokeIslandEvent::mbDebug;
#endif


#if NIJI_APP_EVENT_RESORT_HEAP_CHECK
static s32 s_appEventResortHeapCheck_demoCount = 0;
#endif


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
void PokeIslandEvent::StartEvent( GameSys::GameManager* p_gameman, bool bDebug )
#else
void PokeIslandEvent::StartEvent( GameSys::GameManager* p_gameman )
#endif
{
#if PM_DEBUG
  mbDebug = bDebug;
#endif
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, PokeIslandEvent>( p_geventman );
}





//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
PokeIslandEvent::PokeIslandEvent( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mpHeap( heap ),
mSeq( 0 )
{
#if NIJI_APP_EVENT_RESORT_HEAP_CHECK
  s_appEventResortHeapCheck_demoCount = 0;
#endif
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
PokeIslandEvent::~PokeIslandEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool PokeIslandEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void PokeIslandEvent::InitFunc( GameSys::GameManager*  )
{
#if defined(GF_PLATFORM_CTR)
  OHNO_PRINT("PokeIslandEvent::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  mParam.pPokeParty = GFL_NEW(heap) pml::PokeParty(heap);  //引き渡しようのPARTY
  mParam.isStart = true;
  mParam.bgmReq = true;

}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT PokeIslandEvent::MainFunc( GameSys::GameManager* game_manager )
{
  if( App::Resort::ResortDataManager::GetStaticInstance() != NULL )
  {
    App::Resort::ResortDataManager::GetStaticInstance()->Update();
  }


  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();


  switch( mSeq ) {
  case EVSEQ_FLY_DEMO_CALL:
    {
      // CreateはPokeIslandEventかResortProcで行う。
      // UpdateはPokeIslandEventで行う。
      // DestroyはResortProcで行う。

      if( App::Resort::ResortDataManager::GetStaticInstance() == NULL )
      {
        App::Resort::ResortDataManager::SetStaticInstance(&s_resortDataManager);
        App::Resort::ResortDataManager::GetStaticInstance()->Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) );
        App::Resort::ResortDataManager::GetStaticInstance()->SetWaitUpdateCount(90);  // デモの読み込みを邪魔しないように3秒待つ
      }
    }
    {
      s32 seq_no = GARC_demo_seq_script_sd9081_fly_resort_BSEQ;
      GameSys::GameData * gd = game_manager->GetGameData();
      u8 sex = gd->GetPlayerStatusConst()->GetSex();

      m_DressUpParam = gd->GetPlayerStatusConst()->GetDressup();

      // 着せ替え用のドレスアップデータ適用
      System::DressUp::OverwrideDressUpParamRide( sex, true, &m_DressUpParam );

      //デモコール
      if (gd->CheckResortDemoShowed())
      {
        seq_no = GARC_demo_seq_script_sd9081_fly_resort_short_BSEQ;//ショート版
      }
      else
      {
        gd->SetResortDemoShowed();
      }
      
      Field::Demo::FieldDemoProc * proc = GameSys::CallFieldDemoProc( seq_no );
      proc->SetDressUpParam( &m_DressUpParam, (sex==PM_MALE) );
      proc->SetHeapSize( 0x100000, 0xD00000 );
    }
    mSeq = EVSEQ_FLY_DEMO_WAIT;
    break;

  case EVSEQ_FLY_DEMO_WAIT:
#if NIJI_APP_EVENT_RESORT_HEAP_CHECK
    {
      ++s_appEventResortHeapCheck_demoCount;
      if( s_appEventResortHeapCheck_demoCount == 180 )
      {
        gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        GFL_PRINT("HEAPID_APP_DEVICE ================================\n");
        heap->Dump();
      }
    }
#endif
    // プロセスの終了待ち
    if( game_manager->IsProcessExists() )
    {
      break;
    }

    Sound::PushBGMReq(); 
    
    /*
    暫定対処の必要がなくなったのでコメントアウト。
    //福島さん大野さんNMCat[1784]暫定対処。きのみアイコンを増やしたせいでメモリ不足になったので、フィールドを破棄してメモリを空ける。
    {
      // 現状メモリが足りないので
      Field::Area* pArea = game_manager->GetGameData()->GetFieldArea();
      pArea->Terminate();
    }
    */

    mSeq = EVSEQ_INIT;
    break;

  case EVSEQ_INIT:



#if defined(GF_PLATFORM_CTR)
    OHNO_PRINT("PokeIslandEvent::MainFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
    {
      mParam.pSave = game_manager->GetGameData()->GetResortSave();



      mpProc = GameSys::CallResortProc();
      mpProc->CreateMemory();
      mParam.mpAsyncFileManager = game_manager->GetAsyncFileManager();
      mParam.mpSkybox = game_manager->GetSkybox();
      mParam.pBoxPokemon = game_manager->GetGameData()->GetBoxPokemon();
      mParam.pMyStatus = game_manager->GetGameData()->GetPlayerStatus();
      mParam.pEventSave = game_manager->GetGameData()->GetEventWork();
      mParam.pBox = game_manager->GetGameData()->GetBOX();
      mParam.inputEnable = false; //!< 入力を無効に設定

      mParam.firstOpenPos = mParam.pSave->GetSelectResortPos();  // 最初に開く場所

      mpProc->SetAppParam( &mParam );

      mSeq = EVSEQ_INIT_TEST;
    }
    break;
  case EVSEQ_INIT_TEST:
    {
      mSeq = EVSEQ_WAIT_TEST;
    }
    break;
  case EVSEQ_WAIT_TEST:
    // プロセスの終了待ち
    if( game_manager->GetProcManager()->IsRegisterProcess(mpProc) == false ){
      mParam.isStart = false;
      mpProc = NULL;
      ResultBoxGimKerudhio(game_manager->GetGameData());
      switch(mParam.seqReq){
      default:
      case App::Resort::NEXTSEQ_END:
        mSeq = EVSEQ_JUMP_TITLE;
        break;
      case App::Resort::NEXTSEQ_BOX_GIM:
      case App::Resort::NEXTSEQ_BOX_GIM_RE:
        mSeq = EVSEQ_JUMP_BOX;
        break;
      case App::Resort::NEXTSEQ_BOX_HOTSPA:
      case App::Resort::NEXTSEQ_BOX_HOTSPA_RE:
        mSeq = EVSEQ_JUMP_BOX;
        break;
      case App::Resort::NEXTSEQ_ZUKAN:
        mSeq = EVSEQ_JUMP_REG_ZUKAN;
        break;
      case App::Resort::NEXTSEQ_STATUS:
        mSeq = EVSEQ_CALL_STATUS;
        break;
      case App::Resort::NEXTSEQ_TAMAGO:
        mSeq = EVSEQ_JUMP_EGG_HATCHING;
        break;
      }
    }
    break;

  case EVSEQ_CALL_STATUS:

    {
      mStatusAppParam.mode = App::Status::MODE_PARTY;

      mStatusAppParam.poke_index  = 0;

      PokeTool::form::UpdateKerudhioFormPP(game_manager->GetGameData(),mParam.pPokeParty->GetMemberPointer( 0 ));
      
      mStatusAppParam.party_data.SetParameter( mParam.pPokeParty );

      mStatusAppParam.time_limit = NULL;      //!< 制限時間

      mStatusAppParam.update_prohibition = true;            //!< ポケモンパラメータの更新禁止フラグ(技入れ替え・ボックスマーク)
    }

    mpStatusProc = GameSys::CallStatusProc();
    mpStatusProc->SetAppParam( &mStatusAppParam );
    mSeq = EVSEQ_WAIT_STATUS;
    break;

  case EVSEQ_WAIT_STATUS:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess( mpStatusProc ) == false )
    {
      mSeq = EVSEQ_INIT;
    }
    break;

  case EVSEQ_JUMP_BOX:
    {
      {
        // ボックスに移動するときはフィールド用に残していたメモリを解放しなければならない
        Field::Area* pArea = game_manager->GetGameData()->GetFieldArea();
        pArea->Terminate();
      }

      pml::PokeParty * party = mParam.pPokeParty;
      party->Clear(); // メンバーをけす

      App::Event::BoxEvent* pBox;
#if PM_DEBUG
      pBox = App::Event::BoxEvent::StartEvent( game_manager, mbDebug );
#else
      pBox = App::Event::BoxEvent::StartEvent( game_manager );
#endif
      switch(mParam.seqReq){
      case App::Resort::NEXTSEQ_BOX_GIM:
        pBox->SetBoxStartType(App::Box::BOX_CALL_RESORT_GIM,mParam.selectBoxGroup );
        mSeq = EVSEQ_LOOP_BOX_GIM;
        break;
      case App::Resort::NEXTSEQ_BOX_GIM_RE:
        pBox->SetBoxStartType(App::Box::BOX_CALL_RESORT_GIM,mParam.selectBoxGroup );
        mSeq = EVSEQ_LOOP_BOX_GIM_RE;
        break;
      case App::Resort::NEXTSEQ_BOX_HOTSPA:
      case App::Resort::NEXTSEQ_BOX_HOTSPA_RE:
        pBox->SetBoxStartType(App::Box::BOX_CALL_RESORT_HOTSPA,mParam.selectBoxGroup);
        mSeq = EVSEQ_LOOP_BOX_HOTSPA;
        break;
      }
    }
    break;

  case EVSEQ_LOOP_BOX_GIM:
    ResultBoxGim(true);
    mSeq = EVSEQ_INIT;
    break;
  case EVSEQ_LOOP_BOX_GIM_RE:
    ResultBoxGim(false);
    mSeq = EVSEQ_INIT;
    break;

  case EVSEQ_LOOP_BOX_HOTSPA:
    ResultBoxHotSpa();
    mSeq = EVSEQ_INIT;
    break;

  case EVSEQ_JUMP_REG_ZUKAN:
    {
      pml::PokeParty * party = mParam.pPokeParty;
      mZukanRegisterEventParam.pp =  party->GetMemberPointer( 0 );  //必ず０番目
      mZukanRegisterEventParam.callerType = PokeTool::PokeGetUtil::CallerType::HOKAKU_RESORT;

      PokeTool::PokeGetUtil::ReturnType::Tag ret = PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
        mpHeap,
        mZukanRegisterEventParam.pp,
        mZukanRegisterEventParam.callerType
        );
      GFL_ASSERT(ret==PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER);  // リゾートで捕獲の場合は必ずCALL_ZUKAN_REGISTERが返ってくる

      app::event::ZukanRegisterEvent::StartEvent(game_manager, &mZukanRegisterEventParam);
      mSeq = EVSEQ_JUMP_REG_ZUKAN_WAIT;
    }
    break;

  case EVSEQ_JUMP_REG_ZUKAN_WAIT:
    {
      //もどったら野生を消す
      pml::PokeParty * party = mParam.pPokeParty;
      party->Clear(); // メンバーをけす
      //セーブも消す
      mParam.pSave->SetWildPokemon(mParam.selectPokemonNo-Savedata::ResortSave::STOCKPOKEMON_MAX,  party->GetMemberPointer( 0 ) );
      mSeq = EVSEQ_INIT;
    }
    break;

  case EVSEQ_JUMP_EGG_HATCHING:
    {

      pml::PokeParty              *party = mParam.pPokeParty;
      pml::pokepara::PokemonParam *pPokeParam = party->GetMemberPointer( 0 );  //必ず０番目

      static NetEvent::EggHatching::EggHatchingEventParam  rEventParam;
      rEventParam.m_pTargetEgg = pPokeParam;
      rEventParam.m_inResort   = true;
      NetEvent::EggHatching::EggHatchingEvent::StartEvent( game_manager, rEventParam );
      mSeq = EVSEQ_LOOP_EGG_HATCHING;
    }
    break;
  case EVSEQ_LOOP_EGG_HATCHING:
    OHNO_PRINT("いべんとからもどってきた\n");
    {
      pml::PokeParty              *party = mParam.pPokeParty;
      pml::pokepara::PokemonParam *pPokeParam = party->GetMemberPointer( 0 );  //必ず０番目
      if(pPokeParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
        OHNO_PRINT("たまごのまま\n");
      }
      else {
    //    pPokeParam->SetResortEventStatus( pml::pokepara::RESORTEVENT_HOTSPA_STATE );
        // リゾートセーブデータへ反映
        mParam.pSave->SetHotSpaPokemon( mParam.pokemonSelectIndex, pPokeParam );

        {//
          gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
          mParam.pSave->ResultBoxHotSpa( heap , mParam.pokemonSelectIndex/6);
        }

        // メンバーから削除
        party->RemoveMember(0);
      }
    }
    mSeq = EVSEQ_INIT;
    break;


  case EVSEQ_JUMP_TITLE:
    //ポケリゾートをきどうした、場所のID
    PokeDiary::Set( PDID_COUNT_POKRESORT , mParam.firstOpenPos );

#if PM_DEBUG
    //    if(mbDebug)
    //   { // ランチャー
    //    debug::launcher::LauncherProc::InitCallParam( &m_pLauncherParam );
    //   debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( proc_manager, &m_pLauncherParam );
    // launcher->CreateStartListData<debug::startmenu::StartLauncher>();
    //    }
#endif
    mSeq = EVSEQ_END;
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void PokeIslandEvent::EndFunc( GameSys::GameManager*  )
{
  GFL_SAFE_DELETE(mParam.pPokeParty);
  Sound::PopBGMReq( Sound::BGM_FADE_FAST );
}



//--------------------------------------------------------------
// 温泉ポケモン設定処理
//--------------------------------------------------------------
void PokeIslandEvent::ResultBoxHotSpa(void )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  mParam.pSave->ResultBoxHotSpa(heap,mParam.selectBoxGroup);  // このheapはこの関数内で一時的に使われているだけのようだ。関数を抜けるまでに使った分は破棄されるようだ。
}


//--------------------------------------------------------------
// GIMポケモン設定処理
//--------------------------------------------------------------
void PokeIslandEvent::ResultBoxGim(bool binbox )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  mParam.pSave->ResultBoxGim(heap,mParam.selectBoxGroup,binbox);  // このheapはこの関数内で一時的に使われているだけのようだ。関数を抜けるまでに使った分は破棄されるようだ。
}


//--------------------------------------------------------------
// ケルディオ技設定処理
//--------------------------------------------------------------
void PokeIslandEvent::ResultBoxGimKerudhio( GameSys::GameData * gd )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  pml::pokepara::PokemonParam* pPoke = GFL_NEW_LOW(heap) pml::pokepara::PokemonParam(heap->GetLowerHandle());

  for(int index =0;index < (Savedata::ResortSave::RESORT_GIM_KIND_MAX*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX);index++){ //３種類
    mParam.pSave->GetGimPokemon(index, pPoke);
    if(!pPoke->IsNull() && !pPoke->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG ) ){
      //GFNMCat[5385]フォルムチェンジ時に図鑑登録されないバグ対処で図鑑セーブにアクセスするため、GameDataを引数に取るようになった by iwasawa
      PokeTool::form::UpdateKerudhioFormPP(gd,pPoke);
    }
    mParam.pSave->SetGimPokemon(index, pPoke);
  }
  GFL_SAFE_DELETE(pPoke);
}



GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

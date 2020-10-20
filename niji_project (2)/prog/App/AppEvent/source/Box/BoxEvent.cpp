//===================================================================
/**
 *	@file	    BoxEvent.cpp
 *	@brief    ボックスのイベント
 *	@author	  k.ohno
 *	@date		  2015.7.15
 */
//===================================================================

#include "App/AppEvent/include/Box/BoxEvent.h"
#include "App/AppEvent/include/Box/BoxEventNoNet.h"
#include "App/AppEvent/include/Box/BoxEventListener.h"

#include "System/include/HeapDefine.h"
#include <GameSys/include/GameProcManager.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "App/Box/include/BoxProc.h"
#include "App/Box/include/BoxAppSetup.h"
#include "App/BoxSearch/include/BoxSearchAppParam.h"

#include "App/StrInput/include/StrInputFrame.h"

#include <App/BoxSearch/include/BoxSearchProc.h>
#include <GameSys/include/DllProc.h>

#include "Savedata/include/MyItemSave.h"
#include "niji_conv_header/poke_lib/monsno_def.h"
#include "niji_conv_header/poke_lib/wazano_def.h"
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include "System/include/PokemonVersion.h"

#include "PokeTool/include/PokeTool.h"

#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------
enum
{
  EVSEQ_INIT,
  EVSEQ_INIT_TEST,
  EVSEQ_WAIT_TEST,
  EVSEQ_JUMP_TITLE,
  EVSEQ_CALL_SEARCH,
  EVSEQ_WAIT_SEARCH,
  EVSEQ_CALL_ITEM,
  EVSEQ_WAIT_ITEM,
  EVSEQ_CALL_NAME,
  EVSEQ_WAIT_NAME,
  EVSEQ_CALL_STATUS,
  EVSEQ_WAIT_STATUS,
	EVSEQ_CALL_TEAM_NAME,
	EVSEQ_WAIT_TEAM_NAME,
	EVSEQ_NONET_END,
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
bool BoxEvent::mbDebug;
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
BoxEvent* BoxEvent::StartEvent( GameSys::GameManager* p_gameman, bool bDebug )
#else
BoxEvent* BoxEvent::StartEvent( GameSys::GameManager* p_gameman )
#endif
{
#if PM_DEBUG
  mbDebug = bDebug;
#endif
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  BoxEvent* pEvent =  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, BoxEvent>( p_geventman );
  return pEvent; 
}





//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
BoxEvent::BoxEvent( gfl2::heap::HeapBase* heap ) :
	NetEvent::NetEventBase(heap),
mSeq( 0 )
{
  mpBoxEventListener = NULL;
  mSeq=EVSEQ_INIT;

  mParam.trade.MonsNo = MONSNO_NULL;
  mParam.trade.Gender = pml::SEX_NUM;
  mParam.trade.Level = 0;
  mParam.trade.bOn = false;
 // mParam.poke_cursor_index = 0;
  mParam.isNijiCompatibility = false;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
BoxEvent::~BoxEvent()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool BoxEvent::BootChk( GameSys::GameManager*  )
{
  return true;
}

#if PM_DEBUG


static void fill_poke( pml::PokeParty *pPokeParty, GameSys::GameData *game_data, gfl2::heap::HeapBase *heap )
{
  Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
  u32 myId = mystatus->GetID();
  pml::pokepara::PokemonParam *pp  = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, MONSNO_HUSIGIDANE, 2, myId );

  int nobuff[]={321,25};

	pPokeParty->Clear();
  
  for(int i=0; i < 1 ;i++){

  //  MonsNo monsno = static_cast<MonsNo>(i + 101);
    MonsNo monsno = static_cast<MonsNo>(nobuff[i]);

    pp->ChangeMonsNo( monsno, FORMNO_HUSIGIBANA_NORMAL );
    pp->SetGetBall( 1 );
    {
//      core->SetItem(1+i);
    }
    pp->SetDefaultWaza();

    /*
    if(i==1){
      // タマゴフラグをたてる
      pp->SetEggFlag();
      // ニックネームを "タマゴ" にする
      gfl2::str::StrBuf nickname( pml::MONS_NAME_LENGTH + pml::EOM_LENGTH, heap );
      pml::personal::GetMonsName( &nickname, MONSNO_TAMAGO );
      pp->SetNickName( &nickname );
    }
*/
    //親名設定
    mystatus->SetPokemonParent( pp, heap );
    pp->SetCassetteVersion( System::GetVersion() );
    pp->SetLangId( System::GetLang() );
		pp->SetHp(1);
		pp->SetSick(pml::pokepara::SICK_DOKU);
    
    pPokeParty->AddMember( *pp );
  }

  GFL_DELETE pp;

}


static void fill_poke_1box( Savedata::BoxPokemon *pBoxPoke, int tray, GameSys::GameData *game_data, gfl2::heap::HeapBase *heap )
{
  Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
  u32 myId = mystatus->GetID();
  pml::pokepara::CoreParam *core  = GFL_NEW(heap) pml::pokepara::CoreParam( heap, MONSNO_HUSIGIDANE, 50, myId );

//  for(int i=0;i<Savedata::BoxPokemon::TRAY_POKE_MAX;i++){
  for(int i=0;i<18;i++){

    MonsNo monsno = static_cast<MonsNo>(i+tray * Savedata::BoxPokemon::TRAY_POKE_MAX+1);
    if(monsno>=MONSNO_TAMAGO){
      monsno = static_cast<MonsNo> (i+1);
    }
    core->ChangeMonsNo( monsno, FORMNO_HUSIGIBANA_NORMAL );
    core->SetGetBall( 1+i%5 );
    if(((1+i) % 3) == 0)
    {
      core->SetItem(1+i);
    }
    core->SetDefaultWaza();

    if(monsno==12){
      // タマゴフラグをたてる
      core->SetEggFlag();
      // ニックネームを "タマゴ" にする
      gfl2::str::StrBuf nickname( pml::MONS_NAME_LENGTH + pml::EOM_LENGTH, heap );
      pml::personal::GetMonsName( &nickname, MONSNO_TAMAGO );
      core->SetNickName( &nickname );
    }

    //親名設定
    mystatus->SetPokemonParent( core, heap );
    int ver[]={
			VERSION_BLACK,
      VERSION_X,
      VERSION_SUN,
      VERSION_HOLOHOLO,
      VERSION_VC_RED};
 //   pp->SetCassetteVersion( System::GetVersion() );
    core->SetCassetteVersion( ver[i%5] );
    core->SetLangId( System::GetLang() );

//    pBoxPoke->PutPokemon( core, tray, i, heap );

    {
      u16 xy=0;
      pBoxPoke->PutPokemonEmptyTray(core, tray, heap, &xy);
      OHNO_PRINT("テスト %d\n",xy);
    }

    
  }

  GFL_DELETE core;

}

void BoxEvent::addDebugPoke(gfl2::heap::HeapBase * heap, GameSys::GameManager* game_manager)
{

  mParam.bBoxJudge=true;

  mParam.pMyItem = game_manager->GetGameData()->GetMyItem();
  mParam.pBoxData = game_manager->GetGameData()->GetBOX();
  mParam.pBoxPokemon = game_manager->GetGameData()->GetBoxPokemon();
  mParam.pTemochi2 = game_manager->GetGameData()->GetPlayerParty();



  
  fill_poke_1box(mParam.pBoxPokemon, 0, game_manager->GetGameData(), heap);
  fill_poke_1box(mParam.pBoxPokemon, 4, game_manager->GetGameData(), heap);
  fill_poke_1box(mParam.pBoxPokemon, 12, game_manager->GetGameData(), heap);

  mParam.pBoxData->SetTrayMax(Savedata::BOX::TRAY_NUM_4TH);

  mParam.pBoxData->SetTeamLock(0,true);
  
  fill_poke(mParam.pTemochi2, game_manager->GetGameData(), heap);

  mParam.pBoxData->SetTeamPokePos( 0, 0, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
  mParam.pBoxData->SetTeamPokePos( 0, 1, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
  mParam.pBoxData->SetTeamPokePos( 0, 2, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
  mParam.pBoxData->SetTeamPokePos( 0, 3, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
  mParam.pBoxData->SetTeamPokePos( 0, 4, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );
  mParam.pBoxData->SetTeamPokePos( 0, 5, Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE );

  mParam.pBoxData->SetTeamPokePos( 0, 0, 0 );
  mParam.pBoxData->SetTeamPokePos( 0, 1, 1 );
  mParam.pBoxData->SetTeamPokePos( 0, 2, 2 );
  mParam.pBoxData->SetTeamPokePos( 0, 3, 3 );

  mParam.pBoxData->SetTeamPokePos( 1, 0, 4 );
  mParam.pBoxData->SetTeamPokePos( 1, 1, 5 );
  mParam.pBoxData->SetTeamPokePos( 1, 2, 6 );
  mParam.pBoxData->SetTeamPokePos( 1, 3, 7 );
  mParam.pBoxData->SetTeamPokePos( 1, 4, 0x10 + 0 );

  {
    Savedata::MyItem* pmi = game_manager->GetGameData()->GetMyItem();
  
    pmi->Add( 17, 17 );
    pmi->Add( 18, 17 );
    pmi->Add( 19, 17 );
  }


  
}
#endif

//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void BoxEvent::InitFunc( GameSys::GameManager* game_manager )
{
#if defined(GF_PLATFORM_CTR)
  OHNO_PRINT("BoxEvent::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  mParam.pSelectPokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  mParam.pWantPokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  mParam.pTargetPokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  mParam.pMyWantPokemonParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);  //受け渡し用
  
  mpBoxAppSetup = GFL_NEW(heap) App::Box::BoxAppSetup();

#if PM_DEBUG
  if(mbDebug){
    addDebugPoke(heap,game_manager);  ///速度計測時ははずす
  }
#endif
  mpBoxAppSetup->Setup(heap,&mParam,game_manager); //セットアップは一回のみ後は保持している


  Sound::PlaySE( SEQ_SE_FLD_PC_LOGIN );

}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT BoxEvent::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  
  switch( mSeq ) {
  case EVSEQ_INIT:
    {
#if defined(GF_PLATFORM_CTR)
      OHNO_PRINT("BoxEvent::MainFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif

      /*
Event OR Proc を呼び出して戻ってきた箇所で
　IsErrorをチェックする。
　trueの場合、速やかにEvent終了させる。
　（参考：NBREvent.cpp）
*/
      if( IsError() ){
        SetOutParam();
        mParam.outJumpMode = (u8)App::Box::BOXJUMP_CANCEL;
        mSeq = EVSEQ_JUMP_TITLE;
      }
      else{
        mpProc = GameSys::CallBoxProc();
        mpProc->SetAppParam( &mParam, mpBoxAppSetup, &mBoxSearchAppParam );
        mSeq = EVSEQ_INIT_TEST;
#if defined(GF_PLATFORM_CTR)
        OHNO_PRINT("BoxEvent::DLL %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif
      }
    }
    break;
  case EVSEQ_INIT_TEST:
    {
      mSeq = EVSEQ_WAIT_TEST;
    }
    break;
  case EVSEQ_WAIT_TEST:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpProc) == false )
    {
      mpProc = NULL;
      if( IsError() ){
        SetOutParam();
        mParam.outJumpMode = (u8)App::Box::BOXJUMP_CANCEL;
        mSeq = EVSEQ_JUMP_TITLE;
      }
      else{

        switch(mParam.outJumpMode){
        case App::Box::BOXJUMP_FIND:
        case App::Box::BOXJUMP_STATUS:
        case App::Box::BOXJUMP_ITEM:
        case App::Box::BOXJUMP_NAME://box名
        case App::Box::BOXJUMP_TEAM_NAME://チーム名
          {
            App::Event::BoxEventNoNet* pBox;
            pBox = App::Event::BoxEventNoNet::StartEvent( game_manager );
            pBox->SetBoxAppSetup(mpBoxAppSetup, &mParam, &mBoxSearchAppParam);
            mSeq = EVSEQ_NONET_END;
          }
          break;
        default:
          SetOutParam();
          mSeq = EVSEQ_JUMP_TITLE;
          break;
        }
      }
    }
    break;
  case EVSEQ_NONET_END:

    mSeq = EVSEQ_INIT;

    break;

  case EVSEQ_JUMP_TITLE:
    if(mpBoxEventListener){
      mpBoxEventListener->OnTermination( mParam.outJumpMode ,mParam.outTray, mParam.outPos);
      mpBoxEventListener->OnBattleTray( mParam.nowBattleTray );
    }

    mParam.pBoxPokemon->UpdateTrayMax(heap);  //BOX数開放の確認
    mpBoxAppSetup->Shutdown();
    GFL_SAFE_DELETE(mpBoxAppSetup);
    mSeq = EVSEQ_END;

    
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}




//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void BoxEvent::EndFunc( GameSys::GameManager*  )
{
  GFL_SAFE_DELETE(mParam.pTargetPokemonParam);
  GFL_SAFE_DELETE(mParam.pWantPokemonParam);
  GFL_SAFE_DELETE(mParam.pSelectPokemonParam);
  GFL_SAFE_DELETE(mParam.pMyWantPokemonParam);
}

void BoxEvent::SetBoxStartType(App::Box::BOX_CALL_TYPE type)
{
  mParam.type = type;
}

void BoxEvent::SetBoxStartType(App::Box::BOX_CALL_TYPE type, int page)
{
  mParam.type = type;
  mParam.nowBattleTray = page;
}


void BoxEvent::TradePokemon(u16 monsno, u8 gender, u8 level)
{
  GFL_ASSERT(monsno < MONSNO_MAX);
  GFL_ASSERT(gender <= pml::SEX_NUM);
  GFL_ASSERT(level <= pml::MAX_POKE_LEVEL);

  mParam.trade.MonsNo = monsno;
  mParam.trade.Gender = gender;
  mParam.trade.Level = level;
  mParam.trade.bOn = true;
}

//--------------------------------------------------------------
/**
 * @brief   niji互換対応有効フラグ
 */
//--------------------------------------------------------------
void BoxEvent::SetNijiCompatibility()
{
  // 通信交換の時のみ設定される想定
  switch( mParam.type ){
  case App::Box::BOX_CALL_TRADE:    //P2P交換
  case App::Box::BOX_CALL_GTS:      //GTS
  case App::Box::BOX_CALL_MIRACLE:   //ミラクル交換
    mParam.isNijiCompatibility = true;
    GFL_PRINT(">BOX Niji互換対応フラグON!!!\n");
    break;
  default:
    GFL_ASSERT_MSG( 0, "想定してしないBOXモードです\n" );
  }
}


//--------------------------------------------------------------
/**
 * @brief   終了を得るリスナーをセット
 * @param   pBoxEventListener       BoxEventListener
 */
//--------------------------------------------------------------
void BoxEvent::SetBoxEventListener(BoxEventListener* pBoxEventListener)
{

  mpBoxEventListener = pBoxEventListener;

}


//--------------------------------------------------------------
/**
 * @brief   通信モジュールのセット
 * @param   P2PTradeRequestClient
 */
//--------------------------------------------------------------
void BoxEvent::SetP2PTradeRequestClient(NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient)
{

  mParam.pP2PTradeRequestClient = pP2PTradeRequestClient;

}


void BoxEvent::SetOutParam( void )
{
  if(mParam.outPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
    mParam.outPos = (u8)mParam.outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;
    mParam.outTray = (u8)App::Box::RETURN_TEMOTI;
  }
}



GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

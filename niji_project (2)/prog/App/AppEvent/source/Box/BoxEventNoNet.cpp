//===================================================================
/**
 *	@file	    BoxEventNoNet.cpp
 *	@brief    ボックスのイベント  通信非対応部分
 *	@author	  k.ohno
 *	@date		  2016.1.25
 */
//===================================================================

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
  EVSEQ_WAIT_TEST,
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


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------


BoxEventNoNet* BoxEventNoNet::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  BoxEventNoNet* pEvent =  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, BoxEventNoNet>( p_geventman );
  return pEvent;
}





//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
BoxEventNoNet::BoxEventNoNet( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap ),
mSeq( 0 )
{
  mSeq=EVSEQ_INIT;

}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
BoxEventNoNet::~BoxEventNoNet()
{
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool BoxEventNoNet::BootChk( GameSys::GameManager*  )
{
  return true;
}


//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void BoxEventNoNet::InitFunc( GameSys::GameManager* game_manager )
{
#if defined(GF_PLATFORM_CTR)
  OHNO_PRINT("BoxEvent::InitFunc %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif

  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );


  mpParty = GFL_NEW(heap) pml::PokeParty(heap);  //引き渡しようのPARTY

  mpNameStrBuf = GFL_NEW(heap) gfl2::str::StrBuf(100,heap);  //ボックス名前付け


  mSeq = EVSEQ_WAIT_TEST;

}



//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT BoxEventNoNet::MainFunc( GameSys::GameManager* game_manager )
{
  GameSys::GameProcManager* proc_manager = game_manager->GetProcManager();

// OHNO_PRINT("-%d \n",mSeq);
  switch( mSeq ) {
  case EVSEQ_INIT:
    {
      mSeq = EVSEQ_END;
      switch(mpParam->outJumpMode){
   //   case App::Box::BOXJUMP_STATUS:
      case App::Box::BOXJUMP_ITEM:
        gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        mpBoxAppSetup->SetPokemon(heap);
        break;
      }
    }
    return GameSys::GMEVENT_RES_FINISH;
  case EVSEQ_WAIT_TEST:
    {
      switch(mpParam->outJumpMode){
      case App::Box::BOXJUMP_FIND:
        mSeq = EVSEQ_CALL_SEARCH;
        break;
      case App::Box::BOXJUMP_STATUS:
        mSeq = EVSEQ_CALL_STATUS;
        break;
      case App::Box::BOXJUMP_ITEM:
        mSeq = EVSEQ_CALL_ITEM;
        break;
      case App::Box::BOXJUMP_NAME:
        mSeq = EVSEQ_CALL_NAME;
        break;
      case App::Box::BOXJUMP_TEAM_NAME:
        mSeq = EVSEQ_CALL_TEAM_NAME;
        break;
      }
    }
  //  OHNO_PRINT("--%d \n",mSeq);

    break;
  case EVSEQ_CALL_NAME:
    mParamStrInput.type = App::StrInput::TYPE_BOX_NAME;
    mParamStrInput.strBuf = mpNameStrBuf;
    mpNameStrBuf->Clear();

    mpStrProc = GameSys::CallStrInputProc();
    mpStrProc->SetAppParam( &mParamStrInput );
    mSeq = EVSEQ_WAIT_NAME;
    break;

  case EVSEQ_WAIT_NAME:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpStrProc) == false )
    {
      if(mParamStrInput.changeFlag && !mParamStrInput.isInputCancel){
        mpParam->pBoxData->SetTrayName( mpNameStrBuf->GetPtr() ,mpParam->outTray);
      }
      mpStrProc=NULL;
      mSeq = EVSEQ_INIT;
    }
    break;

  case EVSEQ_CALL_TEAM_NAME:

    mParamStrInput.type = App::StrInput::TYPE_TEAM_NAME;
    mParamStrInput.strBuf = mpNameStrBuf;
    mpNameStrBuf->Clear();

    mpStrProc = GameSys::CallStrInputProc();
    mpStrProc->SetAppParam( &mParamStrInput );
    mSeq = EVSEQ_WAIT_TEAM_NAME;
    break;

  case EVSEQ_WAIT_TEAM_NAME:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpStrProc) == false )
    {
      if(mParamStrInput.changeFlag && !mParamStrInput.isInputCancel){
        mpParam->pBoxData->SetTeamName( mpNameStrBuf->GetPtr() ,mpParam->outTeam);
      }
      mpStrProc=NULL;
      mSeq = EVSEQ_INIT;
    }
    break;

  case EVSEQ_CALL_SEARCH:
    mpCallProc = GameSys::CallBoxSearchProc();
    mpCallProc->SetAppParam( mpBoxSearchAppParam );
    mSeq = EVSEQ_WAIT_SEARCH;
    break;

  case EVSEQ_WAIT_SEARCH:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpCallProc) == false )
    {
      mpCallProc=NULL;
      if(mpBoxSearchAppParam->endMode  == App::BoxSearch::END_MODE_CANCEL){
      }
      else{
      }
      mSeq = EVSEQ_INIT;
    }
    break;


  case EVSEQ_CALL_ITEM:
    {
      app::bag::BagProc * proc = GameSys::CallBagProc();
      itemSetup( proc );
      mpBagProc = proc;
      mSeq = EVSEQ_WAIT_ITEM;
    }
    break;

  case EVSEQ_WAIT_ITEM:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess(mpBagProc) == false )
    {
      mpParam->pSelectPokemonParam->CopyFrom( *mpParty->GetMemberPointerConst(0) );
      
			OHNO_PRINT("HP %d %d\n",__LINE__,mpParam->pSelectPokemonParam->GetHp());
      
//      mpParam->bPokeChange=true;
      mSeq = EVSEQ_INIT;
    }
    break;

    //ポケモンステータス呼び出し
  case EVSEQ_CALL_STATUS:

    SetupStatusAppParam(game_manager);

    mpStatusProc = GameSys::CallStatusProc();
    mpStatusProc->SetAppParam( &mStatusAppParam );
    mSeq = EVSEQ_WAIT_STATUS;
    break;

  case EVSEQ_WAIT_STATUS:
    // プロセスの終了待ち
    if( proc_manager->IsRegisterProcess( mpStatusProc ) == false )
    {
      mpParam->outPos = mStatusAppParam.poke_index;

      if(mStatusAppParam.mode == App::Status::MODE_PARTY){
        mpParam->outPos += Savedata::BoxPokemon::TRAY_POKE_MAX;
      }
      mSeq = EVSEQ_INIT;
    }
    break;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}




void BoxEventNoNet::itemSetup( app::bag::BagProc * proc )
{
  const pml::PokeParty * const_party = mpParty;

  mPokemon.SetParameter( mpParty );
  mPokemon.SetParameter( const_party );
	OHNO_PRINT("HP %d %d\n",__LINE__,mpParam->pSelectPokemonParam->GetHp());

  mpParty->ReplaceMember( 0,*mpParam->pSelectPokemonParam );

  mParamBag.pokemon = &mPokemon;
  mParamBag.pokemon_index = 0;
  mParamBag.call_mode = app::bag::CALL_BOX;
  mParamBag.item_manager = NULL;
  mParamBag.pocket[0].table = NULL;
  proc->Setup( &mParamBag );
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void BoxEventNoNet::EndFunc( GameSys::GameManager*  )
{
  GFL_SAFE_DELETE(mpNameStrBuf);
  GFL_SAFE_DELETE(mpParty);
}


//--------------------------------------------------------------
/**
 * @brief   ボックスイベントからセットアッパーをもらう
 * @param   BoxAppSetupのポインタ
 */
//--------------------------------------------------------------
void BoxEventNoNet::SetBoxAppSetup( App::Box::BoxAppSetup* pAppSetup ,App::Box::APP_PARAM* pParam,App::BoxSearch::APP_PARAM* mpBoxSearchParam)
{
  mpBoxAppSetup = pAppSetup;
  mpParam=pParam;
  mpBoxSearchAppParam = mpBoxSearchParam;
};




void BoxEventNoNet::setTeamParty(int tray)
{
  int tpos = mpParam->outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;

  mStatusAppParam.poke_index  = 0;
  
  for(int i=0;i<Savedata::BOX::TEAM_MAX;i++){
    u16 boxno = mpParam->pBoxData->GetTeamPokePos(tray,i);
    if(Savedata::BOX::TEAM_BOX_POKEMON_ENTRY_NONE!=boxno){
      mpParam->pBoxPokemon->GetPokemon( mpParam->pTargetPokemonParam,  boxno >> 8 , boxno&0xff);
      mpParty->AddMember(*mpParam->pTargetPokemonParam);
      if(i==tpos){
        mStatusAppParam.poke_index = mpParty->GetMemberCount()-1;   //!< 最初に表示するポケモンのインデックス
      }
      mStatusAppParam.poke_lock[i] = mpParam->pBoxData->IsTeamLock( tray );//NMCat[2243]
      if( mStatusAppParam.poke_lock[i]==false){
        mStatusAppParam.poke_lock[i] = mpParam->pBoxData->IsMeLock( boxno >> 8, boxno&0xff );//NMCat[2243]追加
      }
    }
  }
  mStatusAppParam.party_data.SetParameter( mpParty );

}



void BoxEventNoNet::setGimParty(int tray)
{
  int tpos = mpParam->outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;
  mStatusAppParam.poke_index  = 0;

  for(int i=0;i<Savedata::BOX::TEAM_MAX;i++){
    mpParam->pResortSave->GetGimPokemon(i+tray*Savedata::BOX::TEAM_MAX,mpParam->pTargetPokemonParam);
    if(!mpParam->pTargetPokemonParam->IsNull()){
      mpParty->AddMember(*mpParam->pTargetPokemonParam);
    }
    if(i==tpos){
      mStatusAppParam.poke_index = mpParty->GetMemberCount()-1;   //!< 最初に表示するポケモンのインデックス
    }
  }
  mStatusAppParam.party_data.SetParameter( mpParty );
}

void BoxEventNoNet::setHotSpaParty(int tray)
{
  int tpos = mpParam->outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;
  mStatusAppParam.poke_index  = 0;

  for(int i=0;i<Savedata::BOX::TEAM_MAX;i++){
    mpParam->pResortSave->GetHotSpaPokemon(i+tray*Savedata::BOX::TEAM_MAX,mpParam->pTargetPokemonParam);
    if(!mpParam->pTargetPokemonParam->IsNull()){
      mpParty->AddMember(*mpParam->pTargetPokemonParam);
    }
    if(i==tpos){
      mStatusAppParam.poke_index = mpParty->GetMemberCount()-1;   //!< 最初に表示するポケモンのインデックス
    }
  }
  mStatusAppParam.party_data.SetParameter( mpParty );
}


void BoxEventNoNet::SetupStatusAppParam(GameSys::GameManager* game_manager)
{
  int tray=0;
  int tpos = mpParam->outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;
  mStatusAppParam.poke_index  = tpos;

  mpParty->Clear();
  mStatusAppParam.update_prohibition = false;            //!< ポケモンパラメータの更新禁止フラグ(技入れ替え・ボックスマーク)

  if(mpParam->outPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
    mStatusAppParam.mode = App::Status::MODE_PARTY;
    mStatusAppParam.poke_index  = tpos;    //!< 最初に表示するポケモンのインデックス
    tray=mpParam->outTray;

    if( App::Box::BOX_CHANGE_EDITTYPE_BATTLE_TEAM == mpBoxAppSetup->GetSelectMode())
    {
      setTeamParty(tray);
      mStatusAppParam.update_prohibition = true; //バトルチーム選択は禁止
    }
    else if( mpParam->type == App::Box::BOX_CALL_RESORT_GIM){
      setGimParty(tray);
      mStatusAppParam.resort_group=tray;
      mStatusAppParam.resort_id=Savedata::ResortSave::RESORT_KIND_GIM;
    }
    else if( mpParam->type == App::Box::BOX_CALL_RESORT_HOTSPA ){
      setHotSpaParty(tray);
      mStatusAppParam.resort_group=tray;
      mStatusAppParam.resort_id=Savedata::ResortSave::RESORT_KIND_HOTSPA;
    }
    else{
      tray = App::Box::RETURN_TEMOTI;
      mStatusAppParam.mode = App::Status::MODE_PARTY;
      mStatusAppParam.party_data.SetParameter( game_manager->GetGameData()->GetPlayerParty() );
    }
  }
  else{ //BOXぽけもん
    mStatusAppParam.mode = App::Status::MODE_BOX;
    tray=mpParam->outTray;
    mStatusAppParam.poke_index  = mpParam->outPos;
    mStatusAppParam.party_data.SetParameter( game_manager->GetGameData()->GetPlayerParty() );

    // ポケモンのロック状態を設定
    {
      Savedata::BOX* boxSave = game_manager->GetGameData()->GetBOX();

      for( u32 i=0; i<App::Status::BOX_POKE_MAX; ++i )
      {
        mStatusAppParam.poke_lock[i] = boxSave->IsMeLock( tray, i );
      }
    }

  }
  mStatusAppParam.resort_data = mpParam->pResortSave;  //リゾートのセーブデータ
  
  mStatusAppParam.box_data = mpParam->pBoxPokemon;     //!< ボックスのポケモンセーブデータ

  mStatusAppParam.box_tray_index = tray;                     //!< 表示するトレーのインデックス


  mStatusAppParam.time_limit = NULL;      //!< 制限時間


}





void BoxEventNoNet::SetOutParam( void )
{
  if(mpParam->outPos >= Savedata::BoxPokemon::TRAY_POKE_MAX){
    mpParam->outPos = (u8)mpParam->outPos - Savedata::BoxPokemon::TRAY_POKE_MAX;
    mpParam->outTray = (u8)App::Box::RETURN_TEMOTI;
  }
}



GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

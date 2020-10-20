//==============================================================================
/**
 * @file    ScriptFuncSetAppCall.cpp
 * @brief   ScriptFuncSetAppCall アプリケーションコール系
 * 
 * @author  Miyuki Iwasawa 
 */
//==============================================================================

// pml
#include <pml/include/pmlib.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameProcManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "Print/include/WordSet.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/GameEvent.h"
#include "Sound/include/Sound.h"
#include "GameSys/include/DllProc.h"
#include "PokeTool/include/PokeToolForm.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/event/EventMapJump.h"
#include "Field/FieldStatic/include/event/EventBattleCall.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"


#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetAppCall.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetListener.h"
#include "Field/FieldStatic/include/Script/ScriptFuncSetInit.h"
#include "Field/FieldStatic/include/Event/EventGameClear.h"

#include "Field/FieldStatic/include/Event/EventAppCall.h"
#include "pml/include/pml_PokeParty.h"

// trainer
#include "Trainer/Trainer/include/tr_tool.h"
#include "Trainer/Trainer/include/TrainerMessage.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

// App
#include "App/AppEvent/include/FieldStrInput/FieldStrInputEvent.h"
#include "App/StrInput/include/StrInputAppParam.h"
#include "App/StrInput/include/StrInputFrame.h"
#include "App/Box/include/BoxAppParam.h"
#include "App/AppEvent/include/Box/BoxEvent.h"
#include "App/AppEvent/include/FieldDemo/FieldDemoEvent.h"
#include "App/AppEvent/include/CaptureTutorial/CaptureTutorialEvent.h"
#include "App/AppEvent/include/Kisekae/KisekaeEvent.h"
#include "App/AppEvent/include/FieldPokeList/FieldPokeListEvent.h"
#include "App/AppEvent/include/Shop/ShopBuyEvent.h"
#include "App/AppEvent/include/Shop/ShopSellEvent.h"
#include "App/AppEvent/include/Movie/MovieEvent.h"
#include "App/AppEvent/include/Congrats/CongratsEvent.h"
#include "App/AppEvent/include/TrainerPass/TrainerPassEvent.h"

#include "App/WazaOshie/include/WazaOshieAppParam.h"
#include "App/WazaOshie/include/WazaOshieProc.h"

//技思い出し
#include "App/AppEvent/include/WazaOmoidasi/WazaOmoidasiEvent.h"

//すごい特訓
#include "App/AppEvent/include/SuperTraining/SuperTrainingEvent.h"

#include "NetStatic/NetEvent/include/EggHatchingEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionListSelectEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionResultEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerDetailEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectForScriptEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaEventLinkEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPokeStatusEvent.h"
#include "NetStatic/NetEvent/include/PhotoViewEvent.h"
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"
// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"

//FieldTrade
//#include <niji_conv_header/field/field_trade/FieldTradeData.h>//エクセル上でのID
#include <arc_index/script_event_data.gaix>
#include "App/AppEvent/include/PokeTrade/PokeTradeEvent.h"
#include "PokeTool/include/PokeToolTrade.h"

#include "PokeTool/include/PokeToolPokeGet.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "Savedata/include/MysteryGiftSave.h"

#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>
#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>
#include <niji_conv_header/field/script/ScrOffsetId.h>

// Env
#include "Field/FieldRo/include/FieldEnvManager.h"

// バトル施設
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"
#include "App/AppEvent/include/BattleTree/BattleTreeEvent.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"

//マンタインサーフ
#include "App/AppEvent/include/MantainSurf/MantainSurfEvent.h"
//ルナソルダイブ
#include "App/AppEvent/include/LunaSolDive/LunaSolDiveEvent.h"
//ファインダースタジオ
#include "App/AppEvent/include/FinderStudio/FinderStudioCaptureEvent.h"
//マイアルバム
#include "App/AppEvent/include/MyAlbum/MyAlbumEvent.h"
// バトルフェス
#include  <Field/FieldStatic/include/BattleFes/BattleFesTool.h>
// ポケモン抽選管理
#include "Savedata/include/BattleFesSave.h"

#include "niji_conv_header/field/FieldGimmickTypes.h"

#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
// バトルフェスビデオ用
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingAppParam.h"
#include "Battle/Regulation/include/Regulation.h"

#include "App/AppEvent/include/FieldMenu/FieldMenuGameData.h" // for RotomRequest

//エンディング スタッフロール　エンドロール
#include "App/AppEvent/include/Staffroll/StaffrollEvent.h"


namespace{
  #include <niji_conv_header/field/field_trade/FieldTradeData.h>//エクセル上でのID
}


/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @func        GetTable
 * @brief       命令テーブルセットを取得.
 * @author      N.Takeda
 * @date        2015/04/13(月) 20:57:50
 *
 * @return      const GFL_PAWN_FUNC_TABLE*  
 */
//------------------------------------------------------------------------------
const GFL_PAWN_FUNC_TABLE   *FuncSetAppCall::GetTable( void )
{
  static const GFL_PAWN_FUNC_TABLE s_FuncTable[] = {

    GFL_PAWN_FUNC( CallDemo_ )
    GFL_PAWN_FUNC( MoviePlay_ )

    GFL_PAWN_FUNC( CallCaptureDemo_ )
    GFL_PAWN_FUNC( CallZenryokuWazaDemo_ )
    GFL_PAWN_FUNC( CallEggDemoEvent )

    GFL_PAWN_FUNC( CallPokemonBox_ )
    GFL_PAWN_FUNC( CallBoxFieldTrade )
    GFL_PAWN_FUNC( CallPokePartyNameInput_ )
    GFL_PAWN_FUNC( CallPokeSelect_ )
    GFL_PAWN_FUNC( ResultPokeSelect_ )
    GFL_PAWN_FUNC( CallPokeWazaApp_ )
    GFL_PAWN_FUNC( WaitPokeWazaApp_ )

    GFL_PAWN_FUNC( CallWazaOmoidashi_ )
    GFL_PAWN_FUNC( ResultWazaOmoidashi_ )

    GFL_PAWN_FUNC( CallFriendlyShopBuy_ )
    GFL_PAWN_FUNC( CallFriendlyShopSell_ )
    GFL_PAWN_FUNC( CallKisekaeShopBuy_ )
    GFL_PAWN_FUNC( CallKisekaeFittingShop_ )

    GFL_PAWN_FUNC( CallSuperTrainingApp_ )
    GFL_PAWN_FUNC( ResultSuperTrainingApp_ )

    GFL_PAWN_FUNC( CallAttractionList_ )
    GFL_PAWN_FUNC( ResultAttractionList_ )
    GFL_PAWN_FUNC( CallAttractionResult_ )
    GFL_PAWN_FUNC( CallJoinFestaPlayerDetail_ )
    GFL_PAWN_FUNC( CallJoinFestaPlayerListSelectForScript_ )
    GFL_PAWN_FUNC( CallGameSync_ )
    GFL_PAWN_FUNC( CallBtlVideoWifi_ )
    GFL_PAWN_FUNC( CallGlobalAttraction_ )
    GFL_PAWN_FUNC( CallKisekaeShopJoinFesta_ )
    GFL_PAWN_FUNC( ResultKisekaeShopJoinFesta_ )
    GFL_PAWN_FUNC( CallFesCircleNameInput_ )
    /*  BattleFesCircle*/
    GFL_PAWN_FUNC( CallBFCPokeStatus_Mine_ )
    GFL_PAWN_FUNC( CallBFCPokeStatus_Lottery_ )
    GFL_PAWN_FUNC( CallBFCTrainerSelect_ )
    /*  photo  */
    GFL_PAWN_FUNC( CallPhotoView_ )

    GFL_PAWN_FUNC( PokeTradeLoad )
    GFL_PAWN_FUNC( PokeTradeIsLoaded )
    GFL_PAWN_FUNC( PokeTradeExec )
    GFL_PAWN_FUNC( PokeTradeFin )

    GFL_PAWN_FUNC( CallDendouiriDemo_ )
    GFL_PAWN_FUNC( CallGameClearEvent_ )

    GFL_PAWN_FUNC(CallRegulationSelectApp_)
    GFL_PAWN_FUNC(CallTeamSelectApp_)
    GFL_PAWN_FUNC(GetResultTeamSelectApp_)
    GFL_PAWN_FUNC(CallPokeListApp_)
    GFL_PAWN_FUNC(CallNpcSelectApp_)
    GFL_PAWN_FUNC(CallQuickMatchApp_)
    GFL_PAWN_FUNC(BinstBattleRecorder_)

    GFL_PAWN_FUNC(CallStampApp_)

    GFL_PAWN_FUNC(CallPostmanPokemonReceive_)
    GFL_PAWN_FUNC(CallZigarudeUnionFromCell_)
    
    GFL_PAWN_FUNC(CallMantainSurf_)
    GFL_PAWN_FUNC(CallLunaSolDive_)
    GFL_PAWN_FUNC(CallFinderStudioCapture_)

    GFL_PAWN_FUNC(CallBfesBattleRecorder_)
    GFL_PAWN_FUNC(CallStaffrollApp_)
    GFL_PAWN_FUNC(CallMyAlbum_)

    GFL_PAWN_FUNC_END,
  };
  
  return s_FuncTable;
}



//------------------------------------------------------------------------------
/**
* @brief       Aデモコール
*
* @param[in,out] AMX*          amx .
* @param[in]     const cell *  ptr ポインタ.
*
* @return      イベント結果(今は0固定)
*/
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallDemo_( AMX *amx, const cell * ptr )
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *pSys     = p_gman->GetFieldScriptSystem();
  App::Event::FieldDemoEvent::FieldDemoEventDesk desk;

  desk.id = ptr[1];
  desk.isFade = GFL_BOOL_CAST(ptr[2]);
  desk.demoType  = ptr[3];
  desk.isFieldOpen = GFL_BOOL_CAST(ptr[4]);
  desk.suitType = (Field::Demo::DressUpSuitType)(ptr[5]);
  
  App::Event::FieldDemoEvent* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager,  App::Event::FieldDemoEvent>( p_gman->GetGameEventManager() );

  pEvent->SetupParam( desk );

  // ウェイト
  pSys->Suspend();

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        MoviePlay_
 * @brief       MoviePlay_関数.
 * @author      N.Takeda
 * @date        2015/10/27(火) 17:30:28
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::MoviePlay_(AMX * amx, const cell * ptr)
{
  //niji にて Movie(Mobiclip) はタイトルでしか使わないため、スクリプトからの呼び出しを無効化する
#if 0
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u16 movie_id = ptr[1];
  u32 sound_id = ptr[2];
  
  App::Event::APP_PARAM AppParam;                 ///< MovieEvent内部でコピーされている.
  AppParam.movieNo = movie_id;

#if PM_DEBUG
  App::Event::MovieEvent  *ev = App::Event::MovieEvent::StartEvent( p_gman, false, sound_id );
#else
  App::Event::MovieEvent  *ev = App::Event::MovieEvent::StartEvent( p_gman, sound_id );
#endif
  ev->SetAppParam( &AppParam );
  
  p_sys->Suspend();
#endif
  GFL_ASSERT(0);
  return 0;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  捕獲デモ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallCaptureDemo_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  app::event::CaptureTutorialEvent::StartEvent( p_sys->GetGameManager(), true );
  p_sys->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  全力技デモ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallZenryokuWazaDemo_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  app::event::CaptureTutorialEvent::StartEvent( p_sys->GetGameManager(), false );
  p_sys->Suspend();
  return NULL;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief タマゴ孵化デモ呼び出し
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetAppCall::CallEggDemoEvent( AMX* amx, const cell * ptr )
{
  u32 pos = static_cast<u32>(ptr[1]);

  ScriptObject              *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager      *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem         *p_sys    = p_gman->GetFieldScriptSystem();

  pml::PokeParty              *party = p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam *pPokeParam = party->GetMemberPointer( pos );
  
  static NetEvent::EggHatching::EggHatchingEventParam  rEventParam;
  rEventParam.m_pTargetEgg = pPokeParam;
  NetEvent::EggHatching::EggHatchingEvent::StartEvent( p_gman, rEventParam );
  
  p_sys->Suspend();
  
  return NULL;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  PC(ポケモンニックネーム入力)呼び出し
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_PokePartyNameInput( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  App::StrInput::APP_PARAM *pParam = reinterpret_cast<App::StrInput::APP_PARAM*>(_pWork);
  
  p_work->SetWorkValue( SCWK_RET0, pParam->changeFlag );
  
  if (pParam->changeFlag)
  {
    pParam->cp->SetNickName( pParam->strBuf );
  }
  
  GFL_SAFE_DELETE( pParam->oldNameBuf );
  GFL_SAFE_DELETE( pParam->strBuf );
  GFL_SAFE_DELETE( pParam );
  
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ポケモンのニックネーム入力
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetAppCall::CallPokePartyNameInput_(AMX * amx, const cell * ptr )
{
  FieldScriptSystem         *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager      *p_gman = p_sys->GetGameManager();
  
  gfl2::heap::HeapBase        *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
//  u16 ret_wk_is_decide     = ptr[1];
//  u16 ret_wk_time          = ptr[2];
  u16 pos                  = ptr[3];
//  bool is_default_nickname = ptr[4];
  pml::PokeParty              *party = p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam *p_pp = party->GetMemberPointer( pos );
  
  
  App::StrInput::APP_PARAM *pParam = GFL_NEW(p_heap)App::StrInput::APP_PARAM;
  
  pParam->type = App::StrInput::TYPE_POKEMON_NAME;  // ポケモンの名前入力モード
  pParam->param = p_pp->GetMonsNo();                // ポケモンの種類No
  pParam->cp = p_pp;
  pParam->strBuf = GFL_NEW(p_heap) gfl2::str::StrBuf(System::STRLEN_POKEMON_NAME + System::EOM_LEN, p_heap);
  pml::personal::GetMonsName( pParam->strBuf, p_pp->GetMonsNo() );  ///< 種族名をセット.
  pParam->oldNameBuf = GFL_NEW(p_heap) gfl2::str::StrBuf(System::STRLEN_POKEMON_NAME + System::EOM_LEN, p_heap);
  p_pp->GetNickName( pParam->oldNameBuf );  ///< 元のニックネームをセット.
  pParam->isFieldSubProc = false;                   // 内部でHEAPID_APP_DEVICEからヒープを取得する設定です
  pParam->skipInitGraphicMemory = true;             // 内部でHEAPID_APP_DEVICEからヒープを取得する設定です
  
  app::event::FieldStrInputEvent* pEvent = app::event::FieldStrInputEvent::StartEvent( p_sys->GetGameManager() );
  pEvent->SetAppParam( pParam );
  
  p_sys->SetWaitFunc( amx, WaitFunc_PokePartyNameInput, 0, pParam );
  p_sys->Suspend();
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PC(ポケモンボックス)呼び出し
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_PokemonBox( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
//  GameSys::GameManager *p_gman = p_sys->GetGameManager();
//  ScriptObject* p_obj = p_sys->GetScriptObject();
//  Work* p_script_work = p_obj->GetWork();
  ScriptBoxEvenListener* p_listener = reinterpret_cast<ScriptBoxEvenListener*>(p_work);
  
  GFL_SAFE_DELETE(p_listener);
  return true;
}

cell AMX_NATIVE_CALL FuncSetAppCall::CallPokemonBox_(AMX * amx, const cell * ptr) 
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  ScriptBoxEvenListener* pListener = GFL_NEW(p_heap) ScriptBoxEvenListener( p_sys, App::Box::BOX_CALL_PC );
  App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( p_sys->GetGameManager() );

  pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_PC );
  pBoxEvent->SetBoxEventListener( pListener );

  p_sys->SetWaitFunc( amx, WaitFunc_PokemonBox, 0, pListener );
  p_sys->Suspend();
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  NPCとの交換用の手持ち＆ボックス呼び出し
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_BoxFieldTrade( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
//  GameSys::GameManager *p_gman = p_sys->GetGameManager();
//  ScriptObject* p_obj = p_sys->GetScriptObject();
//  Work* p_script_work = p_obj->GetWork();
  ScriptBoxEvenListener* p_listener = reinterpret_cast<ScriptBoxEvenListener*>(p_work);

  GFL_SAFE_DELETE(p_listener);
  return true;
}

cell AMX_NATIVE_CALL FuncSetAppCall::CallBoxFieldTrade(AMX * amx, const cell * ptr) 
{
  //Param
  u16 mons_no = (u16)ptr[1];
  u8  sex     = (u8)ptr[2];
  u8  level   = 0;//NPCとの交換では考慮対象外

  //Alias
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();

  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  ScriptBoxEvenListener* pListener = GFL_NEW(p_heap) ScriptBoxEvenListener( p_sys, App::Box::BOX_CALL_FIELD_TRADE );
  App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( p_sys->GetGameManager() );

  pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_FIELD_TRADE );
  pBoxEvent->SetBoxEventListener( pListener );
  pBoxEvent->TradePokemon(mons_no, sex, level);

  p_sys->SetWaitFunc( amx, WaitFunc_BoxFieldTrade, 0, pListener );
  p_sys->Suspend();
  return NULL;
}

#if 0
/* ------------------------------------------------------------------------- */
/**
 * @brief ポケモン選択呼び出し用コールバック
 *
 * @param wk コールバックワーク
 * @param p_gman ゲームマネージャー
 */
/* ------------------------------------------------------------------------- */
static void callback_pokeselect( EventAppCall::CALLBACK_WORK* wk, GameSys::GameManager* p_gman )
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  ScriptObject* p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  Work* p_work = p_obj->GetWork();

  app::pokelist::Result* p_result = static_cast<app::pokelist::Result*>( wk->p_param );

  s32 select = p_result->poke_index;

  // ポケモンリストの終了の扱いを変更 2012/08/27 by nakahiro
  if( p_result->endMode == app::pokelist::END_MODE_RETURN || 
      p_result->endMode == app::pokelist::END_MODE_EXIT ||
      p_result->endMode == app::pokelist::END_MODE_CANCEL ){
    select = SCR_POKE_SELECT_RET_CANCEL;
  }
/*
  if( p_result->canceled )
  {
    select = SCR_POKE_SELECT_RET_CANCEL;
  }
*/
  
  p_work->SetEventWorkValue( wk->ret_wk1, select ); 
  
  GflHeapSafeFreeMemory( wk->p_context );
  GflHeapSafeFreeMemory( wk->p_param );
}
#endif

/* ------------------------------------------------------------------------- */
/**
 * @brief ポケモン選択呼び出し
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetAppCall::CallPokeSelect_(AMX* amx, const cell * ptr )
{
  FieldScriptSystem     *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman = p_sys->GetGameManager();
  SystemSingletones     *p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  pml::PokeParty        *p_party= p_gman->GetGameData()->GetPlayerParty();

  u32   mode    = ptr[1];
  u32   param   = ptr[2];
  WazaNo waza_no = (WazaNo)(ptr[4]); 

  GFL_ASSERT( !p_swk->m_pTemp );
  p_swk->m_pTemp = GFL_NEW(p_heap) App::PokeList::CONTEXT_PARAM;
  
  app::event::FieldPokeListEvent * pEvent = app::event::FieldPokeListEvent::StartEvent( p_gman );
  
  App::PokeList::CONTEXT_PARAM    *pPokeListContext = reinterpret_cast<App::PokeList::CONTEXT_PARAM*>(p_swk->m_pTemp);
  switch (mode)
  {
  case SCR_POKE_SELECT_MODE_NORMAL:
    pPokeListContext->mode = App::PokeList::MODE_FIELD;
    break;
  case SCR_POKE_SELECT_MODE_JOIN:
    pPokeListContext->mode = App::PokeList::MODE_JOIN;
    break;
  case SCR_POKE_SELECT_MODE_SIMPLE :
    pPokeListContext->mode = App::PokeList::MODE_SIMPLE;
    break;
  case SCR_POKE_SELECT_MODE_SIMPLE2 :
  {
    const u32   pokelistCapacity  = GFL_NELEMS(pPokeListContext->poke_select_enable);
    u32         bitMask           = 0x01;

    pPokeListContext->mode = App::PokeList::MODE_SIMPLE2;
    /*  mask  */
    GFL_ASSERT(pokelistCapacity <= (sizeof(param) * 8 /*bits*/));
    memset(pPokeListContext->poke_select_enable, 0, sizeof(pPokeListContext->poke_select_enable));
    for(u32 monsterIndex = 0; monsterIndex < pokelistCapacity; ++monsterIndex)
    {
      if(param & bitMask)
      {
        pPokeListContext->poke_select_enable[monsterIndex] = 1;
      }
      bitMask = bitMask << 1;
    }
  }
    break;
  case SCR_POKE_SELECT_MODE_WAZAOSHIE :
    pPokeListContext->mode = App::PokeList::MODE_WAZAOSHIE;
    {
      enum{
        PARAM_DISABLE,
        PARAM_ABLE,
        PARAM_ALREADY,
      };

      //技を教えられるかをこちらで設定
      u32 poke_num = p_party->GetMemberCount();
      for(u32 i = 0; i < poke_num; ++i){
        pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer( i );

        //一応NULLチェック
        if(p_pp == NULL){
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          continue;
        }

        //タマゴは不可
        if(p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG )){
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          continue;
        }

        u32 waza_oshie_mode = param;
        u8 ret = FuncSetInit::CheckWazaOshieMons( p_pp, waza_oshie_mode );
        switch(ret){
        case 0://NG
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          break;
        case 1://OK
          pPokeListContext->waza_set_param[i] = PARAM_ABLE;
          break;
        case 2://ALREADY
          pPokeListContext->waza_set_param[i] = PARAM_ALREADY;
          break;
        }
      }
    }
    break;
  case SCR_POKE_SELECT_MODE_WAZAOSHIE_BP :
    pPokeListContext->mode = App::PokeList::MODE_WAZAOSHIE;
    {
      enum{
        PARAM_DISABLE,
        PARAM_ABLE,
        PARAM_ALREADY,
      };

      //技を教えられるかをこちらで設定
      u32 poke_num = p_party->GetMemberCount();
      for(u32 i = 0; i < poke_num; ++i){
        pml::pokepara::PokemonParam* p_pp = p_party->GetMemberPointer( i );

        //一応NULLチェック
        if(p_pp == NULL){
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          continue;
        }

        //タマゴは不可
        if(p_pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG )){
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          continue;
        }

        u32 waza_oshie_mode = param;
        u8 ret = FuncSetInit::CheckWazaOshieMons( p_pp, waza_oshie_mode, waza_no );
        switch(ret){
        case 0://NG
          pPokeListContext->waza_set_param[i] = PARAM_DISABLE;
          break;
        case 1://OK
          pPokeListContext->waza_set_param[i] = PARAM_ABLE;
          break;
        case 2://ALREADY
          pPokeListContext->waza_set_param[i] = PARAM_ALREADY;
          break;
        }
      }
    }
    break;

  case SCR_POKE_SELECT_MODE_BATTLE :
    pPokeListContext->mode = App::PokeList::MODE_BATTLE;
    break;
  case SCR_POKE_SELECT_MODE_SODATEYA :
    pPokeListContext->mode = App::PokeList::MODE_SODATEYA;
    break;
/*    
    GameSys::GameData * gd = p_gman->GetGameData();
    pPokeListContext->my_status = gd->GetPlayerStatus();

    pPokeListContext->friend_party.SetParameter( gd->GetPlayerParty() );
    pPokeListContext->enemy_party_0.SetParameter( gd->GetPlayerParty() );
    pPokeListContext->enemy_party_1.SetParameter( gd->GetPlayerParty() );
*/
  }
  pEvent->SetPokeListAppParam( pPokeListContext );
  
  p_sys->Suspend();
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        ResultPokeSelect_
 * @brief       ResultPoke選択関数.
 * @author      N.Takeda
 * @date        2015/10/01(木) 15:22:04
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::ResultPokeSelect_(AMX* amx, const cell * ptr )
{
  FieldScriptSystem     *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman = p_sys->GetGameManager();
  SystemSingletones     *p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject          *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                  *p_script_work = p_obj->GetWork();
  
  u32   ret_wk       = ptr[1];
  
  GFL_ASSERT( p_swk->m_pTemp );
  
  App::PokeList::CONTEXT_PARAM    *pPokeListContext = reinterpret_cast<App::PokeList::CONTEXT_PARAM*>(p_swk->m_pTemp);
  s32 idx = pPokeListContext->poke_index;
  
  if( pPokeListContext->end_mode == App::PokeList::END_MODE_CANCEL ){
    idx = SCR_POKE_SELECT_RET_CANCEL;
  }
  p_script_work->SetTempWorkValue( ret_wk, idx );
  
  GFL_SAFE_DELETE( pPokeListContext );
  p_swk->m_pTemp = NULL;

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        CallPokeWazaApp_
 * @brief       CallPokeWazaApp_関数.
 * @author      N.Takeda
 * @date        2015/09/29(火) 19:09:25
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallPokeWazaApp_(AMX* amx, const cell * ptr )
{
  FieldScriptSystem     *pThis = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman = pThis->GetGameManager();
  SystemSingletones     *p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  
  s32 iType = ptr[1];
  s32 iIndex = ptr[2];
  s32 iWazaNo = ptr[3];
  
  GFL_ASSERT( !p_swk->m_pTemp );
  GFL_ASSERT( !p_swk->m_pProc );
  p_swk->m_pTemp = GFL_NEW(p_heap) App::PokeList::CONTEXT_PARAM;
  
  //  r:/home/niji/git_program/niji_project/prog/App/WazaOshie/include/WazaOshieAppParam.h(63): struct APP_PARAM {
  App::WazaOshie::APP_PARAM    *pWazaOshieAppParam = reinterpret_cast<App::WazaOshie::APP_PARAM*>(p_swk->m_pTemp);
  
  pWazaOshieAppParam->mode = iType; // 0覚え:　1:忘れ
  pWazaOshieAppParam->select_poke_index = iIndex;
  switch( iType )
  {
    case 0: ///< 覚え
      pWazaOshieAppParam->waza_no = static_cast<WazaNo>( iWazaNo );
      pWazaOshieAppParam->upper_draw = true;//上画面の描画：技覚えの時のみ行う
      break;
    case 1: ///< 忘れ
      pWazaOshieAppParam->upper_draw = true;
      break;
  }
  
  p_swk->m_pProc = GameSys::CallWazaOshieProc();
  (reinterpret_cast<App::WazaOshie::Proc*>(p_swk->m_pProc))->SetAppParam( pWazaOshieAppParam );
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        WaitPokeWazaApp_
 * @brief       待ちPokeWazaApp関数.
 * @author      N.Takeda
 * @date        2015/10/01(木) 19:31:35
 *
 * @param[in,out] AMX*          amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::WaitPokeWazaApp_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem     *pThis = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman = pThis->GetGameManager();
  SystemSingletones     *p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject          *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                  *p_script_work = p_obj->GetWork();
  
  s32 ret_wk_removed = ptr[1];
  s32 ret_wk_waza_idx = ptr[2];
  
  // このイベントで扱うprocがなくなるのを待つ
  if( p_gman->GetProcManager()->IsRegisterProcess( reinterpret_cast<App::WazaOshie::Proc*>(p_swk->m_pProc) ) == false )
  {
    App::WazaOshie::APP_PARAM    *pWazaOshieAppParam = reinterpret_cast<App::WazaOshie::APP_PARAM*>(p_swk->m_pTemp);
    //pWazaOshieAppParam->waza_no;        //!< in:新しく覚える技No out:忘れる技No
    //pWazaOshieAppParam->end_mode //!< 終了モード  0:キャンセル 1:技を決定
    
    p_script_work->SetTempWorkValue( ret_wk_removed, (pWazaOshieAppParam->end_mode != 0) );
    p_script_work->SetTempWorkValue( ret_wk_waza_idx, pWazaOshieAppParam->select_index );
    
    GFL_ASSERT( p_swk->m_pTemp );
    
    GFL_SAFE_DELETE( p_swk->m_pTemp );
    p_swk->m_pProc = NULL;
    return true;
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        CallWazaOmoidashi_
 * @brief       技思い出しアプリ
 * @author      Ohhira Syo
 * @date        2015/12/25(金) 11:30:00
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallWazaOmoidashi_(AMX* amx, const cell * ptr)
{
  //Param
  u32 poke_slot = ptr[1];

  //Alias
  GameSys::GameManager*        p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  pml::PokeParty*              p_party= p_gman->GetGameData()->GetPlayerParty();
  SystemSingletones*           p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase*        p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  FieldScriptSystem*           p_sys  = p_gman->GetFieldScriptSystem();

  //APP_PARAM
  App::Event::WazaOmoidasiEvent::RESULT_PARAM* pAppParam;
  {
    //New
    {
      pAppParam = GFL_NEW(p_heap) App::Event::WazaOmoidasiEvent::RESULT_PARAM;
    }

    //使いまわすのでTempに保持
    {
      GFL_ASSERT( !p_swk->m_pTemp );
      p_swk->m_pTemp = pAppParam;
    }
  }

  //Call
  {
    App::Event::WazaOmoidasiEvent* pEvent = App::Event::WazaOmoidasiEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );

    pEvent->SetParam(poke_slot, pAppParam);
  }

  //スクリプトは強制中断
  //- 次にスクリプトが呼ばれる時はアプリが終わってるので、即座にResultWazaOmoidashi_を呼べば値が取得可能
  {
    p_sys->Suspend();
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ResultWazaOmoidashi_
 * @brief       技思い出しアプリのリザルト取得
 * @author      Ohhira Syo
 * @date        2015/12/25(金) 11:30:00
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::ResultWazaOmoidashi_(AMX* amx, const cell * ptr)
{
  //Param
  u32 ret_wazano = ptr[1];

  //Alias
  GameSys::GameManager*        p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  SystemSingletones*           p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject*                p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                        p_script_work = p_obj->GetWork();

  //AppParam
  App::Event::WazaOmoidasiEvent::RESULT_PARAM* pAppParam = reinterpret_cast<App::Event::WazaOmoidasiEvent::RESULT_PARAM*>(p_swk->m_pTemp);

  //result
  {
    p_script_work->SetTempWorkValue( ret_wazano, pAppParam->set_waza);
  }

  //Del
  {
    GFL_SAFE_DELETE( p_swk->m_pTemp );
  }

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallFriendlyShopBuy_
 * @brief       通常のショップ呼び出し.
 * @author      N.Takeda
 * @date        2015/10/19(月) 21:28:34
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallFriendlyShopBuy_(AMX * amx, const cell * ptr)
{
//  C:/work/sangoScript/sango_project/prog/src/field/script/ScriptFuncSetAppCall.cpp(3783): cell AMX_NATIVE_CALL FuncSetAppCall::CallFriendlyShopBuy(AMX * amx, const cell * ptr)
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u16 shop_id = ptr[1];
  u16 howmany_msg = ptr[2];
  u16 really_msg = ptr[3];
  u16 really_msg2 = ptr[4];
  u16 hereitis_msg = ptr[5];
  u16 youcanttake_msg = ptr[6];
  u16 nomoney_msg = ptr[7];
  s32 shopType = ptr[8];
  
  App::Shop::ShopType eType[] = {
    App::Shop::TYPE_FR_SHOP,
    App::Shop::TYPE_BP_SHOP,
    App::Shop::TYPE_BP_WAZA,
};

  App::Event::ShopBuyEvent * ev = App::Event::ShopBuyEvent::StartEvent( p_gman );
  ev->SetupHeap( p_gman->GetFieldmap()->GetFieldExtHeap()/*gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE)*/ );
  ev->SetupParam(
    eType[ shopType ],
    shop_id,
    howmany_msg,        // howmany_msg       メッセージ：いくつ買う？
    really_msg,         // really_msg        メッセージ：xxコでxx円です
    really_msg2,        // really_msg2       メッセージ：xxコでxx円です（複数形）
    hereitis_msg,       // hereitis_msg      メッセージ：まいど
    youcanttake_msg,    // youcanttake_msg   メッセージ：持ちきれない
    nomoney_msg,        // nomoney_msg       メッセージ：お金が足りない
    0 );                // selected_msg      メッセージ：今日は終了しました（フェスショップ用）
  
  p_sys->Suspend();
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallFriendlyShopSell_
 * @brief       ショップ売りモード呼び出し.
 * @author      N.Takeda
 * @date        2015/10/19(月) 21:28:32
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallFriendlyShopSell_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  //App::Event::ShopSellEvent * ev = 
  App::Event::ShopSellEvent::StartEvent( p_gman );

  p_sys->Suspend();
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallKisekaeShopBuy_
 * @brief       着せ替え購入.
 * @author      N.Takeda
 * @date        2015/10/26(月) 12:33:34
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallKisekaeShopBuy_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  u16 shop_id = ptr[1];
  
  static App::Kisekae::APP_PARAM  param;
  //  着せ替えを購入
  param.runType = App::Kisekae::EVENT_RUN_TYPE_SHOP;
  //  ショップidを指定
  param.typeData.shop.shopId = static_cast<app::util::Kisekae::KisekaeShopID>(shop_id);//app::util::Kisekae::KISEKAE_SHOP_ID_0;
  
  App::Event::KisekaeEvent::StartEvent( p_gman, &param );
  
  p_sys->Suspend();
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallKisekaeFittingShop_
 * @brief       着せ替え試着アプリ.
 * @author      N.Takeda
 * @date        2015/11/10(火) 15:24:53
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallKisekaeFittingShop_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  
  static App::Kisekae::APP_PARAM  param;
  //  着せ替えを購入
  param.runType = App::Kisekae::EVENT_RUN_TYPE_DRESS_UP;
  
  App::Event::KisekaeEvent::StartEvent( p_gman, &param );
  
  p_sys->Suspend();
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallSuperTrainingApp_
 * @brief       すごい特訓アプリ（パラメータ選択）
 * @author      Ohhira Syo
 * @date        2015/12/25(金) 11:30:00
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallSuperTrainingApp_(AMX* amx, const cell * ptr)
{
  //Param
  u32 poke_slot = ptr[1];
  u32 item_no   = ptr[2];

  //Alias
  GameSys::GameManager*        p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  pml::PokeParty*              p_party= p_gman->GetGameData()->GetPlayerParty();
  pml::pokepara::PokemonParam* p_pp   = p_party->GetMemberPointer( poke_slot );
  SystemSingletones*           p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase*        p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  FieldScriptSystem*           p_sys  = p_gman->GetFieldScriptSystem();

  //APP_PARAM
  App::SuperTraining::APP_PARAM* pAppParam;
  {
    //New
    {
      pAppParam = GFL_NEW(p_heap) App::SuperTraining::APP_PARAM;
    }

    //使いまわすのでTempに保持
    {
      GFL_ASSERT( !p_swk->m_pTemp );
      p_swk->m_pTemp = pAppParam;
    }

    //設定
    {
      //対象ポケモンの設定
      pAppParam->in.pPokeParam = p_pp;

      //使用アイテムの設定
      switch(item_no){
      case ITEM_GINNOOUKAN:
        pAppParam->in.useCrown    = App::SuperTraining::USE_CROWN_SILVER;
        break;
      case ITEM_KINNOOUKAN:
        pAppParam->in.useCrown    = App::SuperTraining::USE_CROWN_GOLD;
        break;
      }
    }
  }

  //Call
  {
    App::Event::SuperTrainingEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager), pAppParam );
  }

  //スクリプトは強制中断
  //- 次にスクリプトが呼ばれる時はアプリが終わってるので、即座にResultSuperTrainingApp_を呼べば値が取得可能
  {
    p_sys->Suspend();
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        ResultSuperTrainingApp_
 * @brief       すごい特訓アプリ（パラメータ選択）のリザルト取得
 * @author      Ohhira Syo
 * @date        2015/12/25(金) 11:30:00
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::ResultSuperTrainingApp_(AMX* amx, const cell * ptr)
{
  //Param
  u32 ret_result = ptr[1];
  cell* ret_param; amx_GetAddr( amx, ptr[2], &ret_param );

  //Alias
  GameSys::GameManager*        p_gman = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  SystemSingletones*           p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject*                p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work*                        p_script_work = p_obj->GetWork();

  //AppParam
  App::SuperTraining::APP_PARAM* pAppParam = reinterpret_cast<App::SuperTraining::APP_PARAM*>(p_swk->m_pTemp);

  //キャンセル
  if(pAppParam->out.result == App::SuperTraining::APP_RESULT_CANCEL){
    //result : false
    p_script_work->SetTempWorkValue( ret_result, 0);
  }

  //決定
  if(pAppParam->out.result == App::SuperTraining::APP_RESULT_GO_TRAINING){
    //result : true
    p_script_work->SetTempWorkValue( ret_result, 1);

    //param
    for(int i = 0; i < pml::pokepara::POWER_NUM; ++i){
      ret_param[i] = pAppParam->out.bPowerUpArray[i];
    }
  }

  //Del
  {
    GFL_SAFE_DELETE( p_swk->m_pTemp );
  }

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @brief       アトラクション一覧アプリ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallAttractionList_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();
  SystemSingletones*    pSwk          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  GFL_ASSERT( !pSwk->m_pTemp );

  pSwk->m_pTemp = GFL_NEW(p_heap) NetApp::JoinFestaAttractionListSelect::APP_RESULT;

  NetApp::JoinFestaAttractionListSelect::APP_RESULT* appResult = reinterpret_cast<NetApp::JoinFestaAttractionListSelect::APP_RESULT*>(pSwk->m_pTemp);
  NetEvent::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectEvent::StartEvent( pScriptSystem->GetGameManager(), appResult ); 
  pScriptSystem->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アトラクション一覧アプリ戻り値
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::ResultAttractionList_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();
  SystemSingletones*    pSwk          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject*         pScript       = ScriptObject::GetClass<ScriptObject*>(amx);
  Work*                 pScriptWork    = pScript->GetWork();
  
  u32   ret_wk       = ptr[1];
  
  GFL_ASSERT( pSwk->m_pTemp );

  NetApp::JoinFestaAttractionListSelect::APP_RESULT* appResult = reinterpret_cast<NetApp::JoinFestaAttractionListSelect::APP_RESULT*>(pSwk->m_pTemp);

  // ワークに結果保存
  pScriptWork->SetTempWorkValue( ret_wk, appResult->bIsSelectedJoinFestaAttractionData );
  
  GFL_SAFE_DELETE( pSwk->m_pTemp );
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       アトラクション結果アプリ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallAttractionResult_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();

  NetApp::JoinFestaAttractionResult::APP_PARAM appParam;
  NetEvent::JoinFestaAttractionResult::JoinFestaAttractionResultEvent::StartEvent( pScriptSystem->GetGameManager(), appParam ); 
  pScriptSystem->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ジョインフェスタのプレイヤー詳細アプリ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallJoinFestaPlayerDetail_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 

  if( jfPersonal != NULL )
  {
    NetApp::JoinFestaPlayerDetail::APP_PARAM appParam;
    appParam.playerData.Copy( *jfPersonal );

    // @fix GFNMCat[1464]：最新データ受信していたらそちらを使う
    {
      // 最新データがあるか
      NetAppLib::JoinFesta::JoinFestaPersonalData* newPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetPersonalData( jfPersonal->GetJoinFestaFriendKey() );
      if( newPersonal != NULL )
      {
        ICHI_PRINT(">ICHI 最新データの方を使用\n");
        appParam.playerData.Copy( *newPersonal );
      }
    }

    NetEvent::JoinFestaPlayerDetail::JoinFestaPlayerDetailEvent::StartEvent( pScriptSystem->GetGameManager(), appParam ); 
    pScriptSystem->Suspend();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ジョインフェスタプレイヤー選択呼び出し結果
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_JoinFestaPlayerListSelectForScript( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  // 結果パラメータ
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT *pAppResult = reinterpret_cast<NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT*>(_pWork);

  // 人物選択したか
  bool isSelected = false;
  // P2Pで既に出ている人物か
  bool isP2pPersonal = false;
  // 選択した人物がリストにいなかったか
  bool isSelectedPersonNotFound = false;

  // 選んでいたら、Lockして話しかけた事にする
  if( pAppResult->bIsSelectedJoinFestaPersonalData == true )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pTargetPersonal = pJoinFestaPersonalDataManager->GetPersonalData(  pAppResult->pSelectJoinFestaPersonalData->GetJoinFestaFriendKey() );

    if( pTargetPersonal != NULL )
    { // いた

      if( label == 0 )
      {// リクルーターで選んだ人
        pJoinFestaPersonalDataManager->SetRecruiterPersonal( pTargetPersonal );
      }
      else
      {// トークイベント用に選んだ人
        pJoinFestaPersonalDataManager->SetScriptSelectPersonal( pTargetPersonal );
      }


      isSelected = true;
      ICHI_PRINT(">ICHI プレイヤーリストで選択した人物確定 \n");

      NetAppLib::JoinFesta::JoinFestaPersonalData& lastP2pPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetLastP2pPersonal();
      if( lastP2pPersonal.IsSameFriendKey( pTargetPersonal->GetJoinFestaFriendKey() ) )
      {
        isP2pPersonal = true;
      }
    }
    else
    {
      isSelectedPersonNotFound = true;
    }
  }
  
  p_work->SetWorkValue( SCWK_RET0, isSelected );
  p_work->SetWorkValue( SCWK_RET1, isP2pPersonal );
  p_work->SetWorkValue( SCWK_RET2, isSelectedPersonNotFound );
  
  GFL_SAFE_DELETE( pAppResult->pSelectJoinFestaPersonalData );
  GFL_SAFE_DELETE( pAppResult );
  
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief       ジョインフェスタのプレイヤー選択アプリ呼び出し
 * @param       第1引数　起動モード 0:リクルーター選択モード 1:トークイベントの人選択モード
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallJoinFestaPlayerListSelectForScript_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem         *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager      *p_gman = p_sys->GetGameManager();  
  gfl2::heap::HeapBase      *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  u32 mode = ptr[1];

  // 起動パラメータ
  NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM appParam;

  if( mode == 0 )
  {
    appParam.bootMode = NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_DEFAULT;
  }
  else
  {
    appParam.bootMode = NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_LIMIT;
    appParam.listSize = 8;
  }
  
  // 結果パラメータ
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT *pAppResult = GFL_NEW(p_heap) NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT();
  pAppResult->pSelectJoinFestaPersonalData = GFL_NEW(p_heap) NetAppLib::JoinFesta::JoinFestaPersonalData();

  NetEvent::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptEvent::StartEvent( p_gman, appParam, pAppResult );
  
  p_sys->SetWaitFunc( amx, WaitFunc_JoinFestaPlayerListSelectForScript, mode, pAppResult );
  p_sys->Suspend();
  
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       ゲームシンクアプリ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallGameSync_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();

  // ジョインフェスタからイベント起動する際のイベント起動
  NetEvent::JoinFestaEventLinkEvent* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaEventLinkEvent>( pGameManager->GetGameEventManager() );
  pEvent->SetRequestID( Field::SubScreen::OUT_REQ_ID_GAME_SYNC  );
  pScriptSystem->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       インターネット版バトルビデオ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallBtlVideoWifi_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();

  // ジョインフェスタからイベント起動する際のイベント起動
  NetEvent::JoinFestaEventLinkEvent* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaEventLinkEvent>( pGameManager->GetGameEventManager() );
  pEvent->SetRequestID( Field::SubScreen::OUT_REQ_ID_WIFI_BTLVIDEO  );
  pScriptSystem->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief       グローバルアトラクションアプリ呼び出し
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallGlobalAttraction_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem* pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();

  // ジョインフェスタからイベント起動する際のイベント起動
  NetEvent::JoinFestaEventLinkEvent* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaEventLinkEvent>( pGameManager->GetGameEventManager() );
  pEvent->SetRequestID( Field::SubScreen::OUT_REQ_ID_GLOBAL_ATTRACTION  );
  pScriptSystem->Suspend();
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        CallKisekaeShopJoinFesta_
 * @brief       着せ替え色染めアプリ
 * @param       第1引数：ショップID
 * @param       第2引数：色ID
 * @param       第3引数：きのみ使用ショップかどうか
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallKisekaeShopJoinFesta_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();
  SystemSingletones*    pSwk          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  GFL_ASSERT( !pSwk->m_pTemp );
  
  u32 shopId  = ptr[1];
  s32 colorId = ptr[2];
  bool bUseKinomiDiscount = GFL_BOOL_CAST(ptr[3]);

  pSwk->m_pTemp = GFL_NEW(p_heap) App::Kisekae::APP_PARAM;
  
  App::Kisekae::APP_PARAM* appParam = reinterpret_cast<App::Kisekae::APP_PARAM*>(pSwk->m_pTemp);
  // 着せ替えの色染め
  appParam->runType = App::Kisekae::EVENT_RUN_TYPE_COLOR_SHOP;
  // ショップidを指定
  appParam->typeData.colorShop.shopId = shopId;
  // 染めるID
  appParam->typeData.colorShop.colorId = (bUseKinomiDiscount ? colorId : App::Kisekae::ALL_COLOR_SELECT );
  // きのみ割引
  appParam->typeData.colorShop.bUseKinomiDiscount = bUseKinomiDiscount;

  ICHI_PRINT(">ICHI CallKiseakeApp shopID:%d colorId:%d isKinomi:%d \n",
    appParam->typeData.colorShop.shopId,
    appParam->typeData.colorShop.colorId,
    appParam->typeData.colorShop.bUseKinomiDiscount
    );

  App::Event::KisekaeEvent::StartEvent( pGameManager, appParam );
  
  pScriptSystem->Suspend();
  
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief       着せ替え色染めアプリ戻り値
 * @retrun      きのみモードで染めた場合true
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::ResultKisekaeShopJoinFesta_(AMX* amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  GameSys::GameManager* pGameManager  = pScriptSystem->GetGameManager();
  SystemSingletones*    pSwk          = pGameManager->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  ScriptObject*         pScript       = ScriptObject::GetClass<ScriptObject*>(amx);
  Work*                 pScriptWork    = pScript->GetWork();
  
  u32   ret_wk       = ptr[1];
  
  GFL_ASSERT( pSwk->m_pTemp );

  App::Kisekae::APP_PARAM* appParam = reinterpret_cast<App::Kisekae::APP_PARAM*>(pSwk->m_pTemp);

  // ワークに結果保存
  pScriptWork->SetTempWorkValue( ret_wk, appParam->output.bKinomiChangeColor );
  
  GFL_SAFE_DELETE( pSwk->m_pTemp );
  
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  フェスサークル名呼び出し
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_FesCircleNameInput( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  App::StrInput::APP_PARAM *pParam = reinterpret_cast<App::StrInput::APP_PARAM*>(_pWork);
  
  p_work->SetWorkValue( SCWK_RET0, pParam->changeFlag );

  
  if (pParam->changeFlag)
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
    pJoinFestaDataSave->SetJoinFestaNameString( pParam->strBuf );
  }
  
  GFL_SAFE_DELETE( pParam->strBuf );
  GFL_SAFE_DELETE( pParam );
  
  return true;
}



/* ------------------------------------------------------------------------- */
/**
 * @brief フェスサークル名入力
 */
/* ------------------------------------------------------------------------- */
cell AMX_NATIVE_CALL FuncSetAppCall::CallFesCircleNameInput_(AMX * amx, const cell * ptr )
{
  FieldScriptSystem         *p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager      *p_gman = p_sys->GetGameManager();

  gfl2::heap::HeapBase      *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  
  
  App::StrInput::APP_PARAM *pParam = GFL_NEW(p_heap)App::StrInput::APP_PARAM;
  
  pParam->type = App::StrInput::TYPE_FESCIRCLE_NAME;  // フェスサークル名前入力モード
  pParam->strBuf = GFL_NEW(p_heap) gfl2::str::StrBuf(NetApp::JoinFesta::JoinFestaDefine::STRLEN_JOIN_FESTA_NAME + System::EOM_LEN, p_heap);
  pParam->strBuf->Clear();
  pParam->isFieldSubProc = false;                   // 内部でHEAPID_APP_DEVICEからヒープを取得する設定です
  pParam->skipInitGraphicMemory = true;             // 内部でHEAPID_APP_DEVICEからヒープを取得する設定です
  
  app::event::FieldStrInputEvent* pEvent = app::event::FieldStrInputEvent::StartEvent( p_sys->GetGameManager() );
  pEvent->SetAppParam( pParam );
  
  p_sys->SetWaitFunc( amx, WaitFunc_FesCircleNameInput, 0, pParam );
  p_sys->Suspend();
  
  return NULL;
}



/* ------------------------------------------------------------------------- */
/**
 * @brief ポケモンステータス表示
 */
/* ------------------------------------------------------------------------- */
bool FuncSetAppCall::WaitFunc_CallPokeStatus( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param* pParam  = reinterpret_cast<NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param*>(_pWork);

  for (u32 partnerIndex = 0; partnerIndex < GFL_NELEMS(pParam->partnerInfos); ++partnerIndex)
  {
    GFL_SAFE_DELETE(pParam->partnerInfos[partnerIndex].pTarget);
    GFL_SAFE_DELETE(pParam->partnerInfos[partnerIndex].pTrainerInfo);
  }

  // フォーカスID設定
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  p_work->SetWorkValue(SCWK_RET0, pParam->focusPokemonIndex);

  GFL_SAFE_DELETE(pParam);
  
  return true;
}

/* ------------------------------------------------------------------------- */
/**
* @brief ポケモンステータス表示
*/
/* ------------------------------------------------------------------------- */
bool FuncSetAppCall::WaitFunc_CallPokeStatus_Lottery(FieldScriptSystem* p_sys, u32 label, void* _pWork)
{
  if (label == CALL_POKESTATUS_LOTTERY_CREATE)
  {
    p_sys->GetGameManager()->GetGameData()->GetBattleFesManager()->DeleteBattleFesPokeLotterySystem();
  }

  return WaitFunc_CallPokeStatus(p_sys, 0, _pWork);
}

//------------------------------------------------------------------
/**
  *  @brief    BFC:自分のポケモンを表示
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallBFCPokeStatus_Mine_(AMX * amx, const cell * ptr)
{
  GFL_PRINT("%s\n", __FUNCTION__);

  FieldScriptSystem*      p_sys   = FieldScriptSystem::GetInstance();
  gfl2::heap::HeapBase*   p_heap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  GameSys::GameManager*   p_gman  = p_sys->GetGameManager();

  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param*   pParam  = GFL_NEW(p_heap) NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param();  /*  WaitFunc_CallPokeStatusでDELETE  */


  {
    pParam->focusPokemonIndex = 0;
    GameSys::GameData* pGameData = p_gman->GetGameData();
    Field::EventWork* ev = pGameData->GetEventWork();
    bool isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
    Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
    u32 target_index = 0;
    /** 適切なPokeparaを渡して下さい。pokeparaは保存する必要はありません。  */
    if (pBattleFesSave->IsExistRentalPokemonData())
    {
      pml::pokepara::CoreParam* p_coreParam = GFL_NEW_LOW(p_heap) pml::pokepara::CoreParam(p_heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
      pBattleFesSave->GetRentalPokemonData(p_coreParam);
      pParam->partnerInfos[target_index].pTarget = GFL_NEW(p_heap) pml::pokepara::PokemonParam(p_heap, *p_coreParam);
      pParam->partnerInfos[target_index].pTrainerInfo = GFL_NEW(p_heap) BattleFesTool::BFCTrainerInfo();
      pParam->partnerInfos[target_index].pTrainerInfo->Setup(*GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData());
      GFL_SAFE_DELETE(p_coreParam);
      ++target_index;
    }
    for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
    {
      if (pBattleFesSave->IsPartner(index))
      {
        pml::pokepara::CoreParam* p_coreParam = GFL_NEW_LOW(p_heap) pml::pokepara::CoreParam(p_heap->GetLowerHandle(), MONSNO_NULL, 0, 0);
        pBattleFesSave->GetPartnerPokemonData(index, p_coreParam);
        pParam->partnerInfos[target_index].pTarget = GFL_NEW(p_heap) pml::pokepara::PokemonParam(p_heap, *p_coreParam);
        pParam->partnerInfos[target_index].pTrainerInfo = GFL_NEW(p_heap) BattleFesTool::BFCTrainerInfo();
        pParam->partnerInfos[target_index].pTrainerInfo->Setup(*pBattleFesSave, index);
        GFL_SAFE_DELETE(p_coreParam);
        ++target_index;
      }
    }
    if (target_index <= 0)
    {
      GFL_ASSERT(0);
      return 0;
    }
    // WaitFunc_CallPokeStatusの終了処理でGFL_DELETE(pTarget)してくれる
  }


  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::StartEvent(p_gman, pParam);

  p_sys->SetWaitFunc( amx, WaitFunc_CallPokeStatus, 0, pParam );
  p_sys->Suspend();

  return 0;
}


//------------------------------------------------------------------
/**
  *  @brief    BFC:抽選結果3体の中のポケモンを表示
  *  @param    [in] ptr[0]:  抽選結果3体中のIndex
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallBFCPokeStatus_Lottery_(AMX * amx, const cell * ptr)
{
  GFL_PRINT("%s\n", __FUNCTION__);

  FieldScriptSystem*      p_sys         = FieldScriptSystem::GetInstance();
  gfl2::heap::HeapBase*   p_heap        = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  GameSys::GameManager*   p_gman        = p_sys->GetGameManager();

  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param*   pParam  = GFL_NEW(p_heap) NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param();  /*  WaitFunc_CallPokeStatusでDELETE  */

  u32 label = CALL_POKESTATUS_LOTTERY_NO_CREATE;

  {
    pParam->focusPokemonIndex = static_cast<u32>(ptr[1]);
    if (p_gman->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem() == NULL)
    {
      p_gman->GetGameData()->GetBattleFesManager()->CreateBattleFesPokeLotterySystem(p_heap);
      label = CALL_POKESTATUS_LOTTERY_CREATE;
    }
    BattleFes::BattleFesPokeLotterySystem* p_battleFesPokeLotterySystem = p_gman->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem();
    /** 適切なPokeparaを渡して下さい。pokeparaは保存する必要はありません。  */
    for (u32 index(0); index < BattleFes::BattleFesDefine::POKEMON_MAX; ++index)
    {
      pParam->partnerInfos[index].pTarget = GFL_NEW(p_heap) pml::pokepara::PokemonParam(p_heap, MONSNO_NULL, 0, 0);
      pParam->partnerInfos[index].pTarget->CopyFrom(*p_battleFesPokeLotterySystem->GetPokemon(index));
    }
  }


  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::StartEvent(p_gman, pParam);

  p_sys->SetWaitFunc( amx, WaitFunc_CallPokeStatus_Lottery, label, pParam );
  p_sys->Suspend();

  return 0;
}


/* ------------------------------------------------------------------------- */
/**
  *  @brief   (BattleFesCircle用)トレーナー選択
  */
/* ------------------------------------------------------------------------- */
bool FuncSetAppCall::WaitFunc_CallBFCTrainerSelect( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pResult = reinterpret_cast<NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT*>(_pWork);
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();

  if(pResult)
  {
    GFL_PRINT("%s: isSelected[%d]\n", __FUNCTION__,  pResult->bIsSelectedJoinFestaPersonalData);
    bool result = pResult->bIsSelectedJoinFestaPersonalData;
    if(result)
    {
      u32 validDataNum = 0;   /*  選択されたJFPersonalData数: [0,2]  */
      
      /*    */
      if(pResult->pSelectJoinFestaPersonalData->IsEnable())
      {
        ++validDataNum;
        if(pResult->pSelectJoinFestaPersonalData_2->IsEnable())
        {
          ++validDataNum;
        }
      }
      GFL_PRINT("%s: validDataNum[%d]\n", __FUNCTION__,  validDataNum);
      p_work->SetWorkValue(SCWK_RET0, validDataNum);
      
      if(validDataNum)
      {
        /** create BFC pokepara    */
        gfl2::heap::HeapBase*   p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
        GameSys::GameManager*   p_gman = p_sys->GetGameManager();
        GameSys::GameData* pGameData = p_gman->GetGameData();
        Field::EventWork* ev = pGameData->GetEventWork();
        bool isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
        Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
        // 仲間のデータを削除
        for (u32 index(0); index < BattleFes::BattleFesDefine::PARTNER_TRAINER_MAX; ++index)
        {
          pBattleFesSave->ClearPartnerData(index, p_heap);
        }
        if (1 <= validDataNum)
        {
            // 登録
            BattleFesTool::SetJFPersonalToBattleFesSave(*pBattleFesSave, 0, *(pResult->pSelectJoinFestaPersonalData),   p_heap->GetLowerHandle());
        }
          
        if (2 <= validDataNum)
        {
            // 登録
            BattleFesTool::SetJFPersonalToBattleFesSave(*pBattleFesSave, 1, *(pResult->pSelectJoinFestaPersonalData_2), p_heap->GetLowerHandle());
        }
      }
    }
    else
    {
      p_work->SetWorkValue(SCWK_RET0, 0);
    }

    GFL_SAFE_DELETE(pResult->pSelectJoinFestaPersonalData);
    GFL_SAFE_DELETE(pResult->pSelectJoinFestaPersonalData_2);
    GFL_SAFE_DELETE(pResult);
    return true;
  }
  p_work->SetWorkValue(SCWK_RET0, 0);
  return true;
}


//------------------------------------------------------------------
/**
  *  @brief   (BattleFesCircle用)トレーナー選択
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallBFCTrainerSelect_(AMX * amx, const cell * ptr )
{
  GFL_PRINT("%s\n", __FUNCTION__);

  FieldScriptSystem*      p_sys         = FieldScriptSystem::GetInstance();
  gfl2::heap::HeapBase*   p_heap        = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );  /*  子EventとしてStatusを呼ぶことがあるのでLowerにしない  */
  GameSys::GameManager*   p_gman        = p_sys->GetGameManager();

  NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM   appParam;
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pResult  = GFL_NEW(p_heap) NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT();  /*  WaitFuncでDELETE  */

  /*  init param  */
  {
    appParam.bootMode = NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_BattleFesCircle;
  }

  /*  init result  */
  {
    pResult->pSelectJoinFestaPersonalData     = GFL_NEW(p_heap) NetAppLib::JoinFesta::JoinFestaPersonalData();
    pResult->pSelectJoinFestaPersonalData_2   = GFL_NEW(p_heap) NetAppLib::JoinFesta::JoinFestaPersonalData();
    pResult->bIsSelectedJoinFestaPersonalData = false;
  }


  /*  call  */
  NetEvent::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptEvent::StartEvent(p_gman, appParam, pResult);

  p_sys->SetWaitFunc( amx, WaitFunc_CallBFCTrainerSelect, 0, pResult );
  p_sys->Suspend();

  return 0;
}


//------------------------------------------------------------------
/**
  *  @brief    写真viewのwaitFunc
  */
//------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_CallPhotoView( FieldScriptSystem* p_sys, u32 label, void* _pWork )
{
  gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>*  pPhotoInfoList  = reinterpret_cast<gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>*>(_pWork);
  
  /*  newフラグの状態を反映  */
  if(pPhotoInfoList)
  {
    gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>::Iterator       itr     = pPhotoInfoList->Begin();
    const gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>::Iterator itrEnd  = pPhotoInfoList->End();

    for(;itr != itrEnd; ++itr)
    {
      if(itr->IsShowed())
      {
        const NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION  jfRelations[] =
        {
          NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST,
          NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP
        };
        
        NetAppLib::JoinFesta::JoinFestaPersonalDataManager*   pJFPDManager  = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
        NetAppLib::JoinFesta::JoinFestaPersonalData*          pJFPD;
        
        for(u32 index = 0; index < GFL_NELEMS(jfRelations); ++index)
        {
          pJFPD = pJFPDManager->GetPersonalData(jfRelations[index], itr->key);
          if(pJFPD)
          {
            pJFPD->SetNewPhotoOffEx(itr->photoData.deco_param.crc);
          }
        }
      }
    }
  }
  
  GFL_SAFE_DELETE(pPhotoInfoList);
  
  return  true;
}


//------------------------------------------------------------------
/**
  *  @brief    写真view
  */
//------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallPhotoView_(AMX * amx, const cell * ptr )
{
  GFL_PRINT("%s\n", __FUNCTION__);

  // 話しかけた相手のJFパーソナルデータ
  NetAppLib::JoinFesta::JoinFestaPersonalData*  pJFPersonal   = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetScriptTalkFieldPersonalData(); 
  gfl2::heap::HeapBase*                         pHeapBase     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );  /*  子EventとしてStatusを呼ぶことがあるのでLowerにしない  */
  FieldScriptSystem*                            pSciptSys     = FieldScriptSystem::GetInstance();
  GameSys::GameManager*                         pGameManager  = pSciptSys->GetGameManager();
  

  if(pJFPersonal)
  {
    NetEvent::PhotoView::PhotoViewEvent::Param                  eventParam;
    gfl2::util::List<NetApp::PhotoView::APP_PARAM::PhotoInfo>*  pPhotoInfoList  = NetApp::PhotoView::APP_PARAM::CreatePhotoInfoList(pHeapBase, 1);

    eventParam.appParam.m_pPlayList = pPhotoInfoList;
    eventParam.appParam.RegisterJoinFestaPersonalData(*pJFPersonal);

    /*  call  */
    NetEvent::PhotoView::PhotoViewEvent::StartEvent(pGameManager, eventParam);

    pSciptSys->SetWaitFunc( amx, WaitFunc_CallPhotoView, 0, pPhotoInfoList );
    pSciptSys->Suspend();
  }

 return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：実際の交換の準備開始
 * @brief       まずはエクセルからのデータ取得開始
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::PokeTradeLoad( AMX * amx, const cell * ptr )
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  //データの格納先
  void** ppDataBuf = &p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile = p_heap->GetLowerHandle();
    openReq.heapForArcSrc = p_heap->GetLowerHandle();
    openReq.heapForReq = p_heap->GetLowerHandle();
    pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId = GARC_script_event_data_field_trade_BIN;
    loadReq.ppBuf = ppDataBuf;
    loadReq.heapForBuf = p_heap;
    loadReq.heapForReq = p_heap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;
    //loadReq.pBufSize = &m_BufferSize;
    loadReq.align = 4;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：実際の交換の準備待機
 * @brief       エクセルからのデータ読み込み待ち
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::PokeTradeIsLoaded( AMX * amx, const cell * ptr )
{
  {
    //Alias
    GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

    //データの格納先
    void** ppDataBuf = &p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

    // 読み込み完了待ち
    if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
      return false;
    }
    if( !pAsyncFileManager->IsArcFileLoadDataFinished( ppDataBuf ) ){
      return false;
    }

    //アーカイブ閉じる
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      closeReq.heapForReq = p_heap->GetLowerHandle();
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }
  }

  //--Execでやっていた処理をこちらに移す（CallAppFieldCloseを呼んだ後にPokeTradeExecを呼べるように）

  {
    //Param
    u32 select_pos  = ptr[1];
    u32 select_index  = ptr[2];
    u32 data_index  = ptr[3];

    //Alias
    GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameData* p_game_data = p_gman->GetGameData();
//    FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
    SystemSingletones* p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    //データの格納先
    void* pDataBuf = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

    //指定されたIndexのデータ
    FieldTradeData* pData = &static_cast<FieldTradeData*>(pDataBuf)[data_index];

    //NPCの名前
    gfl2::str::StrBuf npc_name_buf( 32, p_heap->GetLowerHandle() );
    //貰うポケモンのニックネーム
    gfl2::str::StrBuf nick_name_buf( 32, p_heap->GetLowerHandle() );
    {
      //ExtraMsgLoad_によって確保されたものを使うので、スクリプトにて事前にロードしてから実行する事

      //ExtraMsgLoad_で確保されたもの
      gfl2::str::MsgData* pMsgData = GFL_NEW( p_heap ) gfl2::str::MsgData( p_swk->m_pMsgLoadWork->pMsgBuf, p_heap );

      pMsgData->GetString(pData->ParentName, npc_name_buf);
      pMsgData->GetString(pData->NickName, nick_name_buf);

      GFL_SAFE_DELETE( pMsgData );
    }

    //エクセルのデータ→登録用データ
    pml::pokepara::PokemonParam* p_pp;
    {
      //- specでの基本設定
      pml::pokepara::InitialSpec spec;
      {
      spec.monsno     = static_cast<MonsNo>(pData->MonsNo);
        spec.formno     = pData->Form;
        spec.level      = pData->Level;
        spec.id       = pData->ParentID;
        spec.tokusei_index  = pData->Tokusei;
        spec.seikaku    = static_cast<pml::pokepara::Seikaku>(pData->Seikaku);
        spec.sex      = pData->Sex;
        if(0 <= pData->TalentHP){
          spec.talentPower[ pml::pokepara::POWER_HP ] = pData->TalentHP;
        }else{
          spec.talentPower[ pml::pokepara::POWER_HP ] = System::GflUse::GetPublicRand(32);
        }
        if(0 <= pData->TalentAT){
          spec.talentPower[ pml::pokepara::POWER_ATK ] = pData->TalentAT;
        }else{
          spec.talentPower[ pml::pokepara::POWER_ATK ] = System::GflUse::GetPublicRand(32);
        }
        if(0 <= pData->TalentDF){
          spec.talentPower[ pml::pokepara::POWER_DEF ] = pData->TalentDF;
        }else{
          spec.talentPower[ pml::pokepara::POWER_DEF ] = System::GflUse::GetPublicRand(32);
        }
        if(0 <= pData->TalentSA){
          spec.talentPower[ pml::pokepara::POWER_SPATK ] = pData->TalentSA;
        }else{
          spec.talentPower[ pml::pokepara::POWER_SPATK ] = System::GflUse::GetPublicRand(32);
        }
        if(0 <= pData->TalentSD){
          spec.talentPower[ pml::pokepara::POWER_SPDEF ] = pData->TalentSD;
        }else{
          spec.talentPower[ pml::pokepara::POWER_SPDEF ] = System::GflUse::GetPublicRand(32);
        }
        if(0 <= pData->TalentAG){
          spec.talentPower[ pml::pokepara::POWER_AGI ] = pData->TalentAG;
        }else{
          spec.talentPower[ pml::pokepara::POWER_AGI ] = System::GflUse::GetPublicRand(32);
        }

        spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
      }
      //- specを元に仮生成
      {
        p_pp = GFL_NEW(p_heap) pml::pokepara::PokemonParam(p_heap, spec );
      }
      //- さらなる値の反映
      {
        //アイテム無指定の場合はITEM_DUMMY_DATAを設定する
        p_pp->SetItem(pData->Item);

        {
          bool bFastMode = p_pp->StartFastMode();

          {
            p_pp->SetParentName( &npc_name_buf );

            p_pp->SetParentSex( static_cast<pml::Sex>(pData->ParentSex) );

            p_pp->SetNickName(&nick_name_buf);

            p_pp->SetMemories(pml::pokepara::MEMORIES_LEVEL_WITH_PARENT, pData->MemoriesLevel);
            p_pp->SetMemories(pml::pokepara::MEMORIES_CODE_WITH_PARENT, pData->MemoriesCode);
            p_pp->SetMemories(pml::pokepara::MEMORIES_DATA_WITH_PARENT, pData->MemoriesData);
            p_pp->SetMemories(pml::pokepara::MEMORIES_FEEL_WITH_PARENT, pData->MemoriesFeel);

            p_pp->SetGetBall( item::BALLID_MONSUTAABOORU );

            switch (System::GetLang())
            {
            case POKEMON_LANG_ENGLISH:
              if(0 <= pData->LangCodeEN){
                p_pp->SetLangId(pData->LangCodeEN);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_FRANCE:
              if(0 <= pData->LangCodeFR){
                p_pp->SetLangId(pData->LangCodeFR);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_SPAIN:
              if(0 <= pData->LangCodeSP){
                p_pp->SetLangId(pData->LangCodeSP);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_ITALY:
              if(0 <= pData->LangCodeIT){
                p_pp->SetLangId(pData->LangCodeIT);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_GERMANY:
              if(0 <= pData->LangCodeGE){
                p_pp->SetLangId(pData->LangCodeGE);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_JAPAN:
              if(0 <= pData->LangCodeJP){
                p_pp->SetLangId(pData->LangCodeJP);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_KOREA:
              if(0 <= pData->LangCodeKO){
                p_pp->SetLangId(pData->LangCodeKO);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            case POKEMON_LANG_CHINA:
              if(0 <= pData->LangCodeCH){
                p_pp->SetLangId(pData->LangCodeCH);
              }else{
                p_pp->SetLangId(System::GetLang());
              }
            default:
              p_pp->SetLangId(System::GetLang());
            }
          }

          p_pp->EndFastMode( bFastMode );
        }
      }
    }

    // 交換しました、親データ更新
    {
      PokeTool::UpdatePokeParamAfterTrade( p_pp, p_game_data->GetPlayerStatus() );
    }

    // 図鑑登録
    {
      Savedata::ZukanData * zukan = p_game_data->GetZukanData();
      zukan->SetPokeGet( *p_pp );
    }

    //交換開始
    App::Event::PokeTradeEventParam* pParam = GFL_NEW( p_heap ) App::Event::PokeTradeEventParam();
    {
      pParam->pRecvPokemon = p_pp;
      pParam->tray = select_pos;
      pParam->pos = select_index;
      gfl2::str::StrNCopy( pParam->npcName, npc_name_buf.GetPtr(), App::Event::STRLEN_POKETRADE_NPC_NAME );
    }
    //StartEvent自体はExecで行う
//    App::Event::PokeTradeEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), &param );

    //スクリプトは一旦停止
    //- これもStartEventの後に行うためにExecで行う
//    p_sys->Suspend();

    //ここではまだ削除できないので、Exec側で行う
//    GFL_SAFE_DELETE(p_pp);

    //pTempで指すやつの変更
    //- 古いのは削除
    GflHeapSafeFreeMemory(pDataBuf);
    //- 新しいのはEvent用Param
    p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = pParam;

  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：実際の交換の実行
 * @brief       実際の交換～図鑑表示までを行うアプリの起動
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::PokeTradeExec( AMX * amx, const cell * ptr )
{
  //FieldTradeConverter.rbによって構造体と配列風アクセスは自動提供されるのでそれを利用する

  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();

  //イベント開始
  App::Event::PokeTradeEventParam* pParam = static_cast<App::Event::PokeTradeEventParam*>(p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp);
  App::Event::PokeTradeEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), pParam );

  //スクリプトは一旦停止
  p_sys->Suspend();

  pml::pokepara::PokemonParam* p_pp = pParam->pRecvPokemon;
  GFL_SAFE_DELETE(p_pp);

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        NPCとのポケモン交換：実際の交換の終了
 * @brief       読み込んだデータの削除
 * @author      ohhira_syo
 * @date        2015/11/27(金) 11:29:00
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::PokeTradeFin( AMX * amx, const cell * ptr )
{
  //Alias
  GameSys::GameManager* p_gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  //データの格納先
  void* pDataBuf = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp;

  //読み込んだデータは不要になるので削除
  GFL_SAFE_DELETE(pDataBuf);

  //一応NULLに戻しておく
  p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones()->m_pTemp = NULL;

  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallDendouiriDemo_
 * @brief       殿堂入りデモ呼び出し.
 * @author      Miyuki Iwasawa
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallDendouiriDemo_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  bool isAlreadyClear = GFL_BOOL_CAST( ptr[1] );

  //クリア時のセーブデータ反映処理 NMCat[1007]
  if( isAlreadyClear ){
    EventGameClear::SetGameClearStatus( p_gman, p_heap, EventGameClear::GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI );
  }else{
    EventGameClear::SetGameClearStatus( p_gman, p_heap, EventGameClear::GAME_CLEAR_SAVE_TYPE_DENDOUIRI_1ST );
  }
  
  App::Event::CongratsEvent::StartEvent( p_gman, isAlreadyClear );

  p_sys->Suspend();
  
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @func        CallGameCrearEvent_
 * @brief       ゲームクリアシーケンス呼び出し.　スクリプトは強制終了される
 * @author      Miyuki Iwasawa
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallGameClearEvent_(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj    = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman   = p_obj->GetGameManager();
  FieldScriptSystem       *p_sys    = p_gman->GetFieldScriptSystem();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  bool isAlreadyClear = GFL_BOOL_CAST( ptr[1] );

  EventGameClear::SetGameClearStatus( p_gman, p_heap, EventGameClear::GAME_CLEAR_SAVE_TYPE_ENDING );  //NMCat[1007]
  Field::EventGameClear* ev = Field::EventGameClear::Create( p_gman, isAlreadyClear );

  p_sys->EntryNextEvent( ev );
  
  p_sys->ForceEnd();
  p_sys->Suspend();
  
  return 0;
}

//-----------------------------------------------------------------------------
/**
 *  @brief レギュレーション選択アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallRegulationSelectApp_(AMX * amx, const cell * ptr)
{
  Savedata::BattleInstMode mode = static_cast<Savedata::BattleInstMode>( ptr[1] );

  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Field::EventWork* pEventWork = p_gman->GetGameData()->GetEventWork();
  Savedata::BattleInstSave* save = p_gman->GetGameData()->GetBattleInstSave();

  // アプリ呼び出し設定。バトルルール設定は施設、ロイヤルでは不要
  NetApp::RegulationSelect::APP_PARAM param;
  param.pRegulationDrawInfo = p_inst->GetRegulationDrawInfo();
  switch( mode)
  {
  case Savedata::BATTLE_INST_MODE_TREE:
    param.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_CENTER;
    param.bSingleNormalClearFlag = pEventWork->CheckEventFlag(FE_BINST_CLEAR_SINGLE);
    param.bDoubleNormalClearFlag = pEventWork->CheckEventFlag(FE_BINST_CLEAR_DOUBLE);
    param.bMultiNormalClearFlag = pEventWork->CheckEventFlag(FE_BINST_CLEAR_MULTI);
    break;

  case Savedata::BATTLE_INST_MODE_ROYAL:
    param.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_ROYAL;
    param.bBattleRoyalNormalClearFlag = ( save->GetReleaseRankRoyal() > Savedata::BATTLE_ROYAL_RANK_NORMAL );
    param.bBattleRoyalSuperClearFlag  = ( save->GetReleaseRankRoyal() > Savedata::BATTLE_ROYAL_RANK_SUPER  );
    param.bBattleRoyalHyperClearFlag  = ( save->GetReleaseRankRoyal() > Savedata::BATTLE_ROYAL_RANK_HYPER  );
    break;

  default:
    GFL_ASSERT_MSG( 0, "Invalid Mode");
    break;
  }

  // 結果格納用ワーク取得
  NetApp::RegulationSelect::APP_RESULT* result = p_inst->GetRegulationSelectResult();
  result->pSelectRegulationDrawInfo = p_inst->GetRegulationDrawInfo();

  NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( p_gman, param, result );

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief チーム選択アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallTeamSelectApp_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  NetApp::TeamSelect::EVENT_APP_PARAM* param = p_inst->GetTeamSelectEventAppParam();

  // 入力パラメーター設定
  param->in.pRegulationDrawInfo = p_inst->GetRegulationDrawInfo();
  param->in.bAddLocalHavePokeParty = true;
  // QRは選択可能
  param->in.bAddQRPokeParty = true;
  param->in.tournamentType = NetApp::TeamSelect::TOURNAMENT_TYPE_NONE;
  param->in.bOpenRegulationButton = true;//レギュレーション詳細に遷移できるか

  // 出力パラメーター設定
  param->out.pPokeParty = p_inst->GetSelectPokeParty();

  NetEvent::TeamSelect::TeamSelectEvent::StartEvent( p_gman, param );

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief チーム選択アプリ結果取得
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::GetResultTeamSelectApp_( AMX* amx, const cell* ptr)
{
  u32 is_select_ret_wk = ptr[1];

  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  Work* p_work = p_obj->GetWork();
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  bool is_select = (p_inst->GetTeamSelectEventAppParam()->out.result == NetApp::TeamSelect::EVENT_APP_RESULT_TEAM_SELECT);
  p_work->SetTempWorkValue( is_select_ret_wk, is_select );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief ポケモン選択アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallPokeListApp_(AMX * amx, const cell * ptr)
{
  Savedata::BattleInstMode mode = static_cast<Savedata::BattleInstMode>( ptr[1] );

  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Regulation& reg = p_inst->GetRegulationDrawInfo()->GetRegulation();

  App::PokeList::CONTEXT_PARAM* param = p_inst->GetPokeListContextParam();

  // 共通設定
  {
    param->mode           = App::PokeList::MODE_JOIN;
    param->my_status      = p_gman->GetGameData()->GetSaveData()->GetMyStatus();
    param->list_break     = false;
    param->cancel_flag    = true;

    // @fix 森本さん要望：バトルロイヤルもチーム選択の結果からポケモンを選択するのでツリーの処理と同じになりました
    param->my_party.SetParameter( p_inst->GetSelectPokeParty());
  }

  // JOIN設定
  {
    Regulation::BATTLE_RULE rule = reg.GetBattleRule();

#if 0
    switch( mode)
    {
    case Savedata::BATTLE_INST_MODE_TREE:
      if( rule == Regulation::BATTLE_RULE_SINGLE)
      {
        param->join_upp_mode = App::PokeList::JOIN_UPP_MODE_SINGLE;
      }
      else if( rule == Regulation::BATTLE_RULE_DOUBLE)
      {
        param->join_upp_mode = App::PokeList::JOIN_UPP_MODE_TAG;
      }
      else if( rule == Regulation::BATTLE_RULE_MULTI)
      {
        param->join_upp_mode = App::PokeList::JOIN_UPP_MODE_TAG;
      }
      else
      {
        GFL_ASSERT_MSG( 0, "Invalid Battle Rule");
      }
      break;

    case Savedata::BATTLE_INST_MODE_ROYAL:
      param->join_upp_mode = App::PokeList::JOIN_UPP_MODE_ROYALE;
      break;

    default:
      GFL_ASSERT_MSG( 0, "Invalid Mode");
      break;
    }
#endif

    param->join_upp_mode = App::PokeList::JOIN_UPP_MODE_BTL_TREE;

    param->join_count_min = reg.GetValueParam( Regulation::SELECT_NUM_LO);
    param->join_count_max = reg.GetValueParam( Regulation::SELECT_NUM_HI);

    for( u32 i = 0; i < App::PokeList::PARTY_NUM; ++i)
    {
      param->selected_wait[i]   = 1;
      param->qrcord[i]          = 1;
    }
  }

  app::event::FieldPokeListEvent * p_event = app::event::FieldPokeListEvent::StartEvent( p_gman );
  p_event->SetPokeListAppParam( param );

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief NPC選択アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallNpcSelectApp_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
  Savedata::BattleInstSave* save = p_gman->GetGameData()->GetBattleInstSave();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();

  App::BattleTree::APP_PARAM* param = p_inst->GetBattleTreeAppParam();

  gfl2::str::MsgData* pTrainerMsgData = p_inst->GetTrainerMessageData();
  if( pTrainerMsgData == NULL)
  {
    GFL_ASSERT_MSG( 0, "Trainer MsgData is null\n");
    return NULL;
  }

  // あらかじめ確保
  gfl2::str::StrBuf strBuf( pml::PERSON_NAME_BUFFER_SIZE, p_heap);

  // 登録数取得
  param->in.npcSelectListNum = save->GetRegistScoutTrainerDataNum();

  // トレーナーデータ作成
  for( u32 i = 0; i < param->in.npcSelectListNum; ++i)
  {
    // スカウトセーブデータ取得
    u16 trainerID;
    u16 pokeID1,pokeID2;
    u8 tokuseiIndex1,tokuseiIndex2;
    save->GetScoutTrainerData( i, &trainerID, &pokeID1, &pokeID2, &tokuseiIndex1, &tokuseiIndex2 ); 

    // トレーナーデータ取得
    BattleInst::BINST_TRAINER_ROM_DATA* td;
    u32 arc_id = ARCID_BATTLE_TREE_TRAINER;
    size_t size = BattleInstTool::GetTrainerRomDataSize( p_heap, pAsyncFileManager, arc_id, trainerID);
    td = reinterpret_cast<BattleInst::BINST_TRAINER_ROM_DATA*>( GflHeapAllocMemoryLowAlign( p_heap, size, 4 ) );
    BattleInstTool::GetTrainerRomData( p_heap, pAsyncFileManager, arc_id, trainerID, td, size);

    // ポケモンデータ取得
    BattleInst::BINST_POKEMON_ROM_DATA pd[ App::BattleTree::NPC_TEMOCHI_POKE_MAX ];
    BattleInstTool::GetPokemonRomData( p_heap, pAsyncFileManager, ARCID_BATTLE_TREE_POKE, pokeID1, &pd[0]);
    BattleInstTool::GetPokemonRomData( p_heap, pAsyncFileManager, ARCID_BATTLE_TREE_POKE, pokeID2, &pd[1]);

    // トレーナータイプ設定
    param->in.aNpcSelectItem[i].type = static_cast<trainer::TrType>( td->tr_type);

    // トレーナー名設定
    pTrainerMsgData->GetString( trainerID, strBuf);
    gfl2::str::StrNCopy( param->in.aNpcSelectItem[i].aName, strBuf.GetPtr(), pml::PERSON_NAME_BUFFER_SIZE );

    // 手持ちポケモン設定
    for( u32 pokeCnt = 0; pokeCnt < App::BattleTree::NPC_TEMOCHI_POKE_MAX; ++pokeCnt)
    {
      PokeTool::SimpleParam* pSimpleParam = &(param->in.aNpcSelectItem[i].aPokeSimpleParam[pokeCnt]);
      pSimpleParam->monsNo = static_cast<MonsNo>( pd[pokeCnt].mons_no);
      pSimpleParam->formNo = static_cast<FormNo>( pd[pokeCnt].form_no);
      pSimpleParam->sex    = pml::SEX_MALE;
    }

    // 使いまわすためクリアする
    strBuf.Clear();

    // バッファ解放
    GflHeapFreeMemory( td);
  }


  App::Event::BattleTreeEvent::StartEvent( p_gman, param );

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief 通信クイック接続アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallQuickMatchApp_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

  NetEvent::QuickMatch::EVENT_PARAM* param = p_inst->GetQuickMatchEventParam();

  NetEvent::QuickMatch::QuickMatchEvent::StartEventForBattleTree( p_gman, param );

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief スタンプ押印演出アプリ呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallStampApp_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();

  u32 stamp_id = ptr[1];

  App::Event::TrainerPassEvent* p_event = App::Event::TrainerPassEvent::StartEvent( p_gman);
  p_event->SetStampMode( stamp_id);

  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  バトルレコーダーを呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::BinstBattleRecorder_( AMX *amx, const cell * ptr )
{
  u16 battle_count = ptr[1];
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager* p_gman = p_obj->GetGameManager();

  if( p_gman->GetBattleInstManager()->GetBattleInst()->IsCanSaveBattleRecord() )
  {
    App::Event::EventBattleInstBattleVideoRecord::StartEvent( p_gman, battle_count );
  }

  pScriptSystem->Suspend();

  return NULL;
}



//-----------------------------------------------------------------------------
/**
 *  @brief  不思議な贈り物ポケモン受け取り 呼び出し
 */
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_PokemonReceive( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  app::event::ZukanRegisterEventParam* pParam = reinterpret_cast<app::event::ZukanRegisterEventParam*>(p_work);
 
  GFL_SAFE_DELETE(pParam->pp);
  GFL_SAFE_DELETE(pParam);
  return true;
}

cell AMX_NATIVE_CALL FuncSetAppCall::CallPostmanPokemonReceive_(AMX * amx, const cell * ptr) 
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  app::event::ZukanRegisterEventParam* pParam = GFL_NEW(p_heap) app::event::ZukanRegisterEventParam();
  Savedata::MysteryGiftSave * pGiftSave = p_gdata->GetSaveData()->GetMysteryGiftSave();
  bool check_mode = GFL_BOOL_CAST(ptr[1]);

  pParam->pp = GFL_NEW(p_heap) pml::pokepara::PokemonParam( p_heap );

  pGiftSave->GetTopPokeGiftData( pParam->pp, p_heap, false );

  {
    //追加モード
    Savedata::MysteryGiftSave::PokemonGiftType type = pGiftSave->GetTopPokeGiftDataType(false);

    if( type == Savedata::MysteryGiftSave::POKEMON_GIFT_TYPE_EGG ){
      pParam->callerType = PokeTool::PokeGetUtil::CallerType::HAIFU_TAMAGO;
    }else if( type == Savedata::MysteryGiftSave::POKEMON_GIFT_TYPE_MINE ){
      pParam->callerType = PokeTool::PokeGetUtil::CallerType::HAIFU_NANASHI;
    }else{
      pParam->callerType = PokeTool::PokeGetUtil::CallerType::HAIFU;
    }
  }

  {
    //図鑑登録と登録アプリコールチェック
    PokeTool::PokeGetUtil::ExData exData;
    gfl2::system::Date date;

    const Savedata::GameTime* p_gtime = p_gdata->GetGameTime();
    p_gtime->GetDate( &date );

    exData.exDataUnion.exDataForHaifu.placeNo = pGiftSave->GetTopPokeGiftDataPlaceID( false );
    exData.exDataUnion.exDataForHaifu.year = date.GetYear();
    exData.exDataUnion.exDataForHaifu.month = date.GetMonth();
    exData.exDataUnion.exDataForHaifu.day = date.GetDay();

    PokeTool::PokeGetUtil::ReturnType::Tag ret_type = 
      PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
      p_heap->GetLowerHandle(),
      pParam->pp,
      pParam->callerType,
      NULL,
      &exData);

    //チェックモードならここで帰る
    if( check_mode )
    {
      GFL_SAFE_DELETE(pParam->pp);
      GFL_SAFE_DELETE(pParam);
      return ( ret_type == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER );
    }
  }
  {
    //図鑑登録アプリを呼び出す

    app::event::ZukanRegisterEvent::StartEvent( p_gman, pParam );

    p_sys->SetWaitFunc( amx, WaitFunc_PokemonReceive, 0, pParam );
    p_sys->Suspend();
  }

  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  ジガルデ合成時受け取り 呼び出し
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallZigarudeUnionFromCell_(AMX * amx, const cell * ptr) 
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  FormNo form        = static_cast<FormNo>(ptr[1]);
  u8     level       = (ptr[2]);


  app::event::ZukanRegisterEventParam* pParam = GFL_NEW(p_heap) app::event::ZukanRegisterEventParam();

  pParam->pp = PokeTool::form::ZigarudeMakeFromCell( p_gdata, form, level,p_heap );
  pParam->callerType = PokeTool::PokeGetUtil::CallerType::MORAU_NANASHI;

  //図鑑登録と登録アプリコールチェック 名無しをもらうときは、必ずアプリコールがあるので戻り値チェックを省略
  PokeTool::PokeGetUtil::ReturnType::Tag ret_type = 
    PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
    p_heap->GetLowerHandle(),
    pParam->pp,
    pParam->callerType,
    NULL,
    NULL);

  //図鑑登録アプリを呼び出す
  app::event::ZukanRegisterEvent::StartEvent( p_gman, pParam );

  p_sys->SetWaitFunc( amx, WaitFunc_PokemonReceive, 0, pParam );
  p_sys->Suspend();

  return ( ret_type == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER );
}

//-----------------------------------------------------------------------------
/**
*  @brief  マンタインサーフアプリ待ち
*/
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_MantainSurf(FieldScriptSystem* p_sys, u32 label, void* _pWork)
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  App::Event::MANTAIN_SURF_PARAM *p_param = reinterpret_cast<App::Event::MANTAIN_SURF_PARAM*>(_pWork);
  //p_work->SetWorkValue(SCWK_RET0, pParam->changeFlag);

  //コースIDに対応する保持レコードと今回のレコードを比較して、更新していたら、上書きをする
  Savedata::Misc* p_misc = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();
  u32 score = p_misc->GetSurfRecord(p_param->in_corse_id);
  p_work->SetWorkValue(SCWK_RET0, p_param->out_score);//今回のスコアセット
  if (score < p_param->out_score)
  {
    //セーブデータ更新
    p_misc->SetSurfRecord(p_param->in_corse_id, p_param->out_score);
  }

  if (p_param->in_corse_id != Field::COURSE_ID_TUTORIAL)
  {
    //ロトポン仕様　マンタインサーフを遊んだことにする（チュートリアル以外）
    App::FieldMenu::PlayData::SetRotomRequestAchieved(Savedata::FieldMenu::ROTOM_REQ_MANTAIN_SURF);
    //レコード加算
    Savedata::IncRecord(Savedata::Record::RECID_MANTASURF_RIDE_CNT);
  }
  GFL_SAFE_DELETE(p_param);

  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  マンタインサーフ 呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallMantainSurf_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  //パラメータセット
  App::Event::MANTAIN_SURF_PARAM* p_param = GFL_NEW_LOW(p_heap) App::Event::MANTAIN_SURF_PARAM();
  p_param->in_tutorial = ptr[2];//チュートリアルか
  p_param->in_corse_id = ptr[1];//コースID
  p_param->in_score = 0;
  p_param->out_score = 0;

  //アプリ実行前に現在のスコアを一時保存
  u32 score = 0;
  //コースごとにワークから取得
  Savedata::Misc* p_misc = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();
  score = p_misc->GetSurfRecord(p_param->in_corse_id);
  //ゲームデータのテンポラリにセット
  p_gman->GetGameData()->SetSurfScore(score);
  //アプリ内UIにわたすために現在のスコアをセット
  p_param->in_score = score;
  //アプリを呼び出す
  App::Event::MantainSurfEvent *p_event = App::Event::MantainSurfEvent::StartEvent(p_gman);
  p_event->SetupAppParam(p_param);
  p_sys->SetWaitFunc(amx, WaitFunc_MantainSurf, 0, p_param);
  p_sys->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ルナソルダイブアプリ待ち
*/
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_LunaSolDive(FieldScriptSystem* p_sys, u32 label, void* _pWork)
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  App::Event::LUNASOL_DIVE_PARAM *pParam = reinterpret_cast<App::Event::LUNASOL_DIVE_PARAM*>(_pWork);
  p_work->SetWorkValue(SCWK_RET0, pParam->out_zone_id);
  p_work->SetTempWorkValue(SCWK_RET1, static_cast<u32>(pParam->out_pos.x));
  p_work->SetTempWorkValue(SCWK_RET2, static_cast<u32>(pParam->out_pos.y));
  p_work->SetTempWorkValue(SCWK_RET3, static_cast<u32>(pParam->out_pos.z));
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  p_gman->GetGameData()->SetLunaSolDiveDistance(pParam->out_distance);
  p_gman->GetGameData()->SetLunaSolDiveMonsno(pParam->out_monsno);

  //ロトポン仕様　ルナソルダイブを遊んだことにする（通常）
  App::FieldMenu::PlayData::SetRotomRequestAchieved(Savedata::FieldMenu::ROTOM_REQ_LUNASOL_DIVE);
  //レコード加算
  Savedata::IncRecord(Savedata::Record::RECID_LUNASOL_DIVE_CNT);
  //距離ベスト
  u32 best_distance = Savedata::GetRecord(Savedata::Record::RECID_LUNASOL_BESTSCORE);
  if (best_distance < pParam->out_distance)
  {
    Savedata::SetRecord(Savedata::Record::RECID_LUNASOL_BESTSCORE, pParam->out_distance);
  }

  GFL_PRINT("zone:%d pos:(%f,%f,%f) dis:%f mons:%d", pParam->out_zone_id, pParam->out_pos.x, pParam->out_pos.y, pParam->out_pos.z, pParam->out_distance, pParam->out_monsno);
  GFL_SAFE_DELETE(pParam);

  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ルナソルダイブ 呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallLunaSolDive_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  //パラメータセット
  App::Event::LUNASOL_DIVE_PARAM* p_param = GFL_NEW_LOW(p_heap) App::Event::LUNASOL_DIVE_PARAM();
  p_param->in_first_mode = GFL_BOOL_CAST(ptr[1]);//イベント初回起動か
  p_param->out_distance = 0;
  p_param->out_distortion_type = LunaSolDive::DISTORTION_TYPE__UB;
  p_param->out_rarelity = 0;
  p_param->out_zone_id = 0;
  p_param->out_pos.x = 0;
  p_param->out_pos.y = 0;
  p_param->out_pos.z = 0;
  p_param->out_monsno = 0;

  //アプリを呼び出す
  App::Event::LunaSolDiveEvent *p_event = App::Event::LunaSolDiveEvent::StartEvent(p_gman);
  p_event->SetupAppParam(p_param);
  p_sys->SetWaitFunc(amx, WaitFunc_LunaSolDive, 0, p_param);
  p_sys->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ファインダースタジオ(写真を撮る) 呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallFinderStudioCapture_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();

  //アプリを呼び出す
  app::event::FinderStudioCaptureEvent::StartEvent(p_gman);

  p_sys->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  バトルレコーダーアプリ待ち
*/
//-----------------------------------------------------------------------------
bool FuncSetAppCall::WaitFunc_BfesBattleRecorder(FieldScriptSystem* p_sys, u32 label, void* _pWork)
{
  ScriptObject* p_obj = p_sys->GetScriptObject();
  Work* p_work = p_obj->GetWork();
  NetEvent::BattleVideoRecording::EVENT_APP_PARAM* pParam = reinterpret_cast<NetEvent::BattleVideoRecording::EVENT_APP_PARAM*>(_pWork);
  GFL_SAFE_DELETE(pParam->in.appInParam.pRecorderSaveData);
  GFL_SAFE_DELETE(pParam);
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  // CallBfesBattleRecorder_の後処理
  Regulation* pBtlRegulation = p_gman->GetGameData()->GetBattleFesManager()->GetRegulation();
  pBtlRegulation->DeleteData();
  GFL_SAFE_DELETE(pBtlRegulation);
  p_gman->GetGameData()->GetBattleFesManager()->SetRegulation(NULL);

  return true;
}

//-----------------------------------------------------------------------------
/**
*  @brief  バトルレコーダーを呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallBfesBattleRecorder_(AMX *amx, const cell * ptr)
{
  FieldScriptSystem*    pScriptSystem = FieldScriptSystem::GetInstance();
  ScriptObject *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager* p_gman = p_obj->GetGameManager();
  GameSys::GameData* pGameData = p_gman->GetGameData();
  Field::EventWork* ev = pGameData->GetEventWork();
  bool isEvent = ev->CheckEventFlag( SYS_FLAG_BFES_ROCKET_NOTTORI );
  Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave( isEvent );

  // バトルビデオ録画用パラメーター初期化
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
  Regulation* pBtlRegulation = GFL_NEW(p_heap) Regulation(p_heap);
  pBtlRegulation->LoadData(Regulation::PRESET_BATTLE_FES);
  p_gman->GetGameData()->GetBattleFesManager()->SetRegulation(pBtlRegulation);

  ExtSavedata::BattleRecorderSaveData* pBattleRecorderSaveData = GFL_NEW(p_heap) ExtSavedata::BattleRecorderSaveData(p_heap, p_gman);
  pBattleRecorderSaveData->CreateSaveData(p_gman->GetGameData()->GetBattleFesManager()->GetSetupParam(), pBtlRegulation);
  // バトルフェスバトルビデオの場合は、連勝数の箇所にランクを入れる。→表示される箇所はバトルビデオアプリ。
  pBattleRecorderSaveData->SetBattleCounter( static_cast<s32>(pBattleFesSave->GetRank()) );

  NetEvent::BattleVideoRecording::EVENT_APP_PARAM* pBattleVideoRecordingParam = GFL_NEW(p_heap) NetEvent::BattleVideoRecording::EVENT_APP_PARAM();
  pBattleVideoRecordingParam->in.appInParam.pRecorderSaveData = pBattleRecorderSaveData;

  NetEvent::BattleVideoRecording::Event::StartEvent(p_gman, pBattleVideoRecordingParam);

  pScriptSystem->SetWaitFunc(amx, WaitFunc_BfesBattleRecorder, 0, pBattleVideoRecordingParam);
  pScriptSystem->Suspend();

  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  スタッフロールアプリを呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallStaffrollApp_(AMX *amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  //アプリを呼び出す
  App::Event::StaffrollEvent *p_event = App::Event::StaffrollEvent::StartEvent(p_gman);
  p_sys->Suspend();
  return 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief  マイアルバム 呼び出し
*/
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetAppCall::CallMyAlbum_(AMX * amx, const cell * ptr)
{
  FieldScriptSystem* p_sys = FieldScriptSystem::GetInstance();
  GameSys::GameManager* p_gman = p_sys->GetGameManager();

  //アプリを呼び出す
  App::Event::MyAlbumEvent::StartEvent(p_gman);

  p_sys->Suspend();

  return NULL;
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )


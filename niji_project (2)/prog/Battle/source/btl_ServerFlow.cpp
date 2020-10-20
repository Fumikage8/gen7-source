//=============================================================================================
/**
 * @file  btl_ServerFlow.cpp
 * @brief ポケモンWB バトルシステム サーバコマンド生成処理
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include <Savedata/include/MyStatus.h>
#include <AppLib/include/Tool/app_tool_Gauge.h>

#include "./btl_Common.h"
#include "./btl_Str.h"
#include "./btl_Calc.h"
#include "./btl_Event.h"
#include "./btl_Server.h"
#include "./btl_Tables.h"
#include "./btl_Sick.h"
#include "./btl_ui.h"
#include "./btl_BattleRule.h"
#include "./btl_RecordSave.h"
#include "./btl_DiarySave.h"

#include "./btl_server_cmd.h"
#include "./btl_ServerFlow.h"
#include "./btl_mainmodule.h"
#include "./btl_SVCL_ACTION.h"
#include "./btl_ZenryokuWaza.h"
#include "./btl_SideEffectStatus.h"
#include "./btl_PosEffect.h"
#include "./btl_PosEffectStatus.h"
#include "./btl_RoyalRankingCalculator.h"
#include "./btl_IntrudeSystem.h"
#include "./btl_ReinforceSystem.h"
#include "./btl_BattleResultJudge.h"
#include "./btl_ActionPriority.h"
#include "./btl_Oteire.h"

#include "./handler/hand_waza.h"
#include "./handler/hand_tokusei.h"
#include "./handler/hand_item.h"
#include "./handler/hand_side.h"
#include "./handler/hand_pos.h"
#include "./handler/hand_zenryoku.h"
#include "./handler/hand_default_power_up.h"

#include <niji_conv_header/sound/SoundMiddleID.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

//momiji追加
//ロトムカウント追加
#include <AppLib/include/Tool/AppToolRotomFriendship.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"
#include "./btl_ZenryokuWazaStatus.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )//'u8' : ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)



/**
 * @brief 乱入パラメータをクリアする
 * @param param  操作対象のインスタンス
 */
void ServerFlow::IntrudeParam_Clear( IntrudeParam* param ) const
{
  param->callClientID  = static_cast<BTL_CLIENT_ID>( BTL_CLIENTID_NULL );
  param->callPokeID    = BTL_POKEID_NULL;
  param->callPokePos   = BTL_POS_NULL;
  param->appearPokePos = BTL_POS_NULL;
  param->intrudeType   = BTL_INTRUDE_TYPE_INTRUDE;
}





ServerFlow::ServerFlow(
  BTL_SERVER* server, MainModule* mainModule, POKECON* pokeCon,
  SCQUE* que,         HeapHandle heapHandle ) : 
  m_intrudeSystem( NULL ),
  m_reinforceSystem( NULL )
{
  m_server      = server;
  m_pokeCon     = pokeCon;
  m_mainModule  = mainModule;
  m_numActOrder = 0;
  m_turnCount   = 0;
  m_flowResult  = RESULT_DEFAULT;
  m_que         = que;
  m_heapHandle  = heapHandle;
  m_fieldStatus = pokeCon->GetFieldStatus();

  m_actionRecorder = GFL_NEW( heapHandle ) ActionRecorder();

  if( mainModule->IsReinforceEnable() )
  {
    m_reinforceSystem = GFL_NEW( heapHandle ) ReinforceSystem( mainModule, pokeCon );
  }
  else if( mainModule->CanIntrudeBattle() )
  {
    m_intrudeSystem = GFL_NEW( heapHandle ) IntrudeSystem( mainModule, pokeCon, mainModule->GetIntrudePokeParamGenerator() );
  }

  IntrudeParam_Clear( &m_intrudeParam );
  m_intrudeParam.appearPokeParam = NULL;

  if( ( m_intrudeSystem   != NULL ) ||
      ( m_reinforceSystem != NULL ) )
  {
    m_intrudeParam.appearPokeParam = GFL_NEW( heapHandle ) pml::pokepara::PokemonParam( heapHandle );
  }

  clearWorks();
}

ServerFlow::~ServerFlow()
{
  GFL_SAFE_DELETE( m_intrudeParam.appearPokeParam );
  GFL_SAFE_DELETE( m_reinforceSystem );
  GFL_SAFE_DELETE( m_intrudeSystem );
  GFL_SAFE_DELETE( m_actionRecorder );
}

void ServerFlow::ResetSystem( void )
{
  clearWorks();
}

void ServerFlow::clearWorks( void )
{
  EVENT_InitSystem();

  m_wazaRec.Init();
  m_deadRec.Init( m_pokeCon );
  m_posPoke.Init( m_mainModule, m_pokeCon );
  m_partyAllDeadRecorder.Clear();
  m_royalRankingContainer.Clear();
  m_actionRecorder->Clear();

  gfl2::std::MemClear( m_pokeInFlag, sizeof(m_pokeInFlag) );
  gfl2::std::MemClear( m_memberChangeCount, sizeof(m_memberChangeCount) );
  ItemChangeCounter_Clear();

  m_affCounter.Clear();
  PSetStack_Init();
  m_escInfo.Clear();
  m_HEManager.Init();
  m_eventWork.Init();
  m_pokemonBattleInRegister.Clear();
  TimeLimit_Clear( &m_timeLimit );

  m_nigeruCount = 0;
  m_wazaEffIdx = 0;
  m_fQuitBattleRequest = false;
  m_fMemberOutIntr = false;
  m_fWinBGMPlayWild = false;
  m_fEscMsgDisped = false;
  m_cmdBuildStep = 0;
  m_thruDeadMsgPokeID = BTL_POKEID_NULL;
  m_prevExeWaza = WAZANO_NULL;
  m_isPrevExeZenryokuWaza = false;
  m_getPokePos = BTL_POS_NULL;
  m_simulationCounter = 0;
  m_turnCheckStep = 0;
  m_wazaRankEffSerial = 0;
  *m_fMemberChangeReserve = false;
  m_isRecoverZenryoku = false;
  m_isUsedItemPon = false;
  m_isUsedTukamaePon = false;

  #if PM_DEBUG
  m_fDebugClack = false;
  #endif
}

//=============================================================================================
/**
 * サーバコマンド生成（バトル開始直後）
 *
 * @retval  bool    とくせい発動などのイベントがあればtrue（録画チャプタ用）
 */
//=============================================================================================
bool ServerFlow::StartBtlIn( void )
{
  SVCL_WORK* cw;
  u32 i, posIdx;
  bool fAnyEvent = false;

  SCQUE_Init( m_que );

  {
    const BTL_FIELD_SITUATION* fSit = m_mainModule->GetFieldSituation();
    if( fSit->weather != BTL_WEATHER_NONE )
    {
      //@バトル変更箇所 ここには戻り値があるが・・・
      if( scproc_ChangeWeather( fSit->weather, BTL_WEATHER_TURN_PERMANENT, 0, BTL_POKEID_NULL ) == CHANGE_WEATHER_OK ){
        fAnyEvent = true;
      }
    }
  }

  if (m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES))
  {
    const u8 START_FIELD_TURN = 8;
    BtlFesField btlFesField = m_mainModule->GetBtlFesField();
    BtlWeather weather = BTL_WEATHER_NONE;
    BtlGround ground = BTL_GROUND_NONE;
    FieldStatus::EffectType effectType = FieldStatus::EFF_NULL;
    u16 strID = 0;
    switch (btlFesField)
    {
    case BATTLE_FES_FIELD_SHINE:
      weather = BTL_WEATHER_SHINE;
      break;
    case BATTLE_FES_FIELD_RAIN:
      weather = BTL_WEATHER_RAIN;
      break;
    case BATTLE_FES_FIELD_SNOW:
      weather = BTL_WEATHER_SNOW;
      break;
    case BATTLE_FES_FIELD_SAND:
      weather = BTL_WEATHER_SAND;
      break;
    case BATTLE_FES_FIELD_STORM:
      weather = BTL_WEATHER_STORM;
      break;
    case BATTLE_FES_FIELD_DAY:
      weather = BTL_WEATHER_DAY;
      break;
    case BATTLE_FES_FIELD_TURBULENCE:
      weather = BTL_WEATHER_TURBULENCE;
      break;
    case BATTLE_FES_FIELD_GRASS:
      effectType = FieldStatus::EFF_GROUND;
      ground = BTL_GROUND_GRASS;
      strID = BTL_STRID_STD_GrassGround_Start;
      break;
    case BATTLE_FES_FIELD_MIST:
      effectType = FieldStatus::EFF_GROUND;
      ground = BTL_GROUND_MIST;
      strID = BTL_STRID_STD_MistGround_Start;
      break;
    case BATTLE_FES_FIELD_ELEKI:
      effectType = FieldStatus::EFF_GROUND;
      ground = BTL_GROUND_ELEKI;
      strID = BTL_STRID_STD_ElecField_Start;
      break;
    case BATTLE_FES_FIELD_PHYCHO:
      effectType = FieldStatus::EFF_GROUND;
      ground = BTL_GROUND_PHYCHO;
      strID = BTL_STRID_STD_PhychoField_Start;
      break;
    }
    
    if (weather != BTL_WEATHER_NONE)
    {
      if (scproc_ChangeWeather(weather, START_FIELD_TURN, 0, BTL_POKEID_NULL) == CHANGE_WEATHER_OK)
      {
        fAnyEvent = true;
      }
    }
    else if (ground != BTL_GROUND_NONE && effectType != FieldStatus::EFF_NULL && strID > 0)
    {
      BTL_SICKCONT cont = SICKCONT_MakeTurn(BTL_POKEID_NULL, START_FIELD_TURN);
      BTL_HANDEX_STR_PARAMS exStr;
      HANDEX_STR_Clear(&exStr);
      HANDEX_STR_Setup(&exStr, BTL_STRTYPE_STD, strID);
      if (scproc_Battlefes_addFieldEffect(effectType, ground, cont, &exStr))
      {
        fAnyEvent = true;
      }
    }
  }

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    cw = m_server->GetClientWork( i );
    if( cw == NULL ){ continue; }

    const u32 coverPosNum = m_mainModule->GetClientCoverPosNum( cw->myID );
    for(posIdx=0; posIdx<coverPosNum; ++posIdx)
    {
      BTL_POKEPARAM* bpp = cw->party->GetMemberData( posIdx );
      if( (bpp!=NULL) && !bpp->IsDead() )
      {
        scproc_MemberInCore( i, posIdx, posIdx );
      }
    }
  }

//  BTL_EVENT_ReOrderFactorsByPokeAgility( wk );
  if( scproc_AfterMemberIn() ){
    return true;
  }

  return fAnyEvent;
}

void ServerFlow::StartTurn_Boot( void )
{
  m_cmdBuildStep = 0;
  m_numEndActOrder = 0;
}
//=============================================================================================
/**
 * サーバコマンド生成（通常の１ターン分コマンド生成）
 *
 * @retval  Result
 */
//=============================================================================================
ServerFlow::Result ServerFlow::StartTurn( const SVCL_ACTION* clientAction )
{
  m_flowResult =  RESULT_DEFAULT;
  SCQUE_Init( m_que );

  // [デバッグ用] サーバー機による不正コマンド書き換えテスト
  #if PM_DEBUG
  if( !m_mainModule->IsRecordPlayMode()
  &&  m_mainModule->CheckImServerMachine()
  ){
    if( ui::CheckKeyCont(BUTTON_R) && ui::CheckKeyCont(BUTTON_Y) )
    {
      m_fDebugClack = true;
      BTL_PRINT("*** Debug Clack ON ***\n");
    }
  }
  #endif


  if( m_cmdBuildStep == 0 )
  {
    m_actionRecorder->StartTurn();
    m_deadRec.StartTurn();
    FlowFlg_ClearAll();
    EVENT_StartTurn();

    m_server->InitChangePokemonReq();
    EVENTVAR_CheckStackCleared();

    clearPokeCantActionFlag();
    scproc_ResetSpActPriority();

    generateActionOrder( clientAction, m_actOrder, &m_numActOrder, &m_recalcActOrderNeed, &m_recalcActOrderIndex );
    
    scproc_OnTurnStart();

    m_cmdBuildStep = 1;
  }

  m_numEndActOrder = ActOrderProc_Main( m_numEndActOrder );

  if( isTurnEnd( m_flowResult ) )
  {
    scproc_OnTurnEnd();

    if( m_flowResult == RESULT_DEFAULT )
    {
      if( scproc_IntrudeCall() )
      {
        m_flowResult = RESULT_INTRUDE;
      }
    }
  }

  return m_flowResult;
}

/**
 * @brief 場にいるポケモンの特殊優先度をリセットする
 */
void ServerFlow::scproc_ResetSpActPriority( void )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    scPut_SetSpActPriority( bpp, BTL_SPPRI_DEFAULT );
  }
}

/**
 * @brief ポケモンの特殊優先度を設定する
 * @param poke      設定対象のポケモン
 * @param priority  設定する特殊優先度
 */
void ServerFlow::scPut_SetSpActPriority( BTL_POKEPARAM* poke, u8 priority )
{
  poke->SetSpActPriority( priority );
  SCQUE_PUT_OP_SET_SPACT_PRIORITY( m_que, poke->GetID(), BTL_SPPRI_DEFAULT );
}

/**
 * @brief ターン開始時の処理
 */
void ServerFlow::scproc_OnTurnStart( void )
{
  m_fQuitBattleRequest = false;
  updateIntrudeSystemOnTurnStart();
  registerFrontPokemonBattleIn();
  scproc_SetFrontPokeDirt_ByWeather();
  SCQUE_PUT_ACT_TurnStart( m_que );
}

/**
 * @brief 場に出ている全てのポケモンについて、天候による汚れをセットする
 */
void ServerFlow::scproc_SetFrontPokeDirt_ByWeather( void )
{
  const BtlWeather weather = scEvent_GetWeather();
  
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_WORK fps;
  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    scproc_SetPokeDirt_ByWeather( bpp, weather );
  }
}

/**
 * @brief 指定したポケモンについて、天候による汚れをセットする
 *
 * @param pokeParam  操作対象ポケモン
 * @param weather    天候
 */
void ServerFlow::scproc_SetPokeDirt_ByWeather( BTL_POKEPARAM* pokeParam, BtlWeather weather )
{
  if( calc::IsRainWeather( weather ) )
  {
    BTL_PRINT( "[ServerFlow] 天候「あめ」の場に出た( pokeId=%d )\n", pokeParam->GetID() );
    scPut_SetDirt( pokeParam, pml::pokepara::DIRT_TYPE_WATER );
  }

  if( weather == BTL_WEATHER_SAND )
  {
    BTL_PRINT( "[ServerFlow] 天候「すなあらし」の場に出た( pokeId=%d )\n", pokeParam->GetID() );
    scPut_SetDirt( pokeParam, pml::pokepara::DIRT_TYPE_SAND );
  }
}

/**
 * @brief ポケモンに汚れをセットする
 * @param poke  汚れをセットするポケモンのID
 * @param dirt  セットする汚れの種類
 */
void ServerFlow::scPut_SetDirt( BTL_POKEPARAM* poke, DirtType dirt )
{
  poke->SetDirtType( dirt );

  u8 pokeID = poke->GetID();
  SCQUE_PUT_OP_SET_DIRT( m_que, pokeID, dirt );
}

/**
 *  全ポケモンの反動で動けない状態フラグをクリアし、反動で動けないターンであることを表すターンフラグをセットする
 */
void ServerFlow::clearPokeCantActionFlag( void )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    if( bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_CANT_ACTION) )
    {
      scPut_ResetContFlag( bpp, BTL_POKEPARAM::CONTFLG_CANT_ACTION );
      scPut_SetTurnFlag( bpp, BTL_POKEPARAM::TURNFLG_CANT_ACTION );
    }
  }
}

/**
 * @brief 全クライアントのアクションから、処理順に並んだアクション配列を生成する
 * @param[in]   clientAction        全クライアントのアクション
 * @param[out]  actionOrder         処理順に並んだアクションの展開先の配列
 * @param[out]  actionOrderNum      処理順に並んだアクションの総数
 * @param[out]  recalcActOrderNeed  アクションの実行時に行動順番を再計算する必要があるか？
 * @param[out]  headWazaActIndex    アクションの実行時に行動順番を再計算する必要があるアクションのインデックス
 */
void ServerFlow::generateActionOrder( const SVCL_ACTION* clientAction, ACTION_ORDER_WORK* actionOrder, u8* actionOrderNum, bool* recalcActOrderNeed, u8* recalcActOrderIndex )
{
  // 全クライアントの行動を m_actOrder に格納
  *actionOrderNum = storeClientAction( clientAction, actionOrder );

  // 全アクションの処理順序を確定
  sortClientAction( actionOrder, *actionOrderNum, false );

  // メガシンカするポケモンのアクションについて、行動優先度再計算フラグをセット
  // メガ進化するポケモンがいる場合、メガ進化後のパラメータ・特性を考慮した行動順番にする必要があります。
  setRecalcPriorityRequestByMegaEvolve( actionOrder, *actionOrderNum );

  // 先頭の技アクションのインデックスを検索
  // メガ進化するポケモンがいる場合、メガ進化後のパラメータ・特性を考慮した行動順番にする必要があります。
  // ここで検索したインデックスは、メガ進化した後、最初の技を実行する前に、再度、行動順番をソートするために使用されます。
  *recalcActOrderNeed = getHeadWazaActIndex( recalcActOrderIndex, actionOrder, actionOrderNum );
}

/**
 * @brief メガシンカによる行動優先度の再計算フラグをセットする
 * @param[in,out] actionOrder     操作対象のアクション配列
 * @param[in]     actionOrderNum  操作対象のアクション配列の要素数
 */
void ServerFlow::setRecalcPriorityRequestByMegaEvolve( ACTION_ORDER_WORK* actionOrder, u8 actionOrderNum ) const
{
  for( u32 i=0; i<actionOrderNum; ++i )
  {
    if( actionOrder[i].action.gen.cmd == BTL_ACTION_MEGAEVO )
    {
      u8 pokeID = actionOrder[i].bpp->GetID();
      setRecalcPriorityRequest( actionOrder, actionOrderNum, pokeID, BTL_ACTION_FIGHT );
    }
  }
}

/**
 * @brief 指定したポケモン・行動のアクションに対して、行動優先度再計算フラグをセットする
 * @param[in,out] actionOrder     操作対象のアクション配列
 * @param[in]     actionOrderNum  操作対象のアクション配列の要素数
 * @param[in]     targetPokeID    行動優先度再計算フラグをセットする対象ポケモンのID
 * @param[in]     targetAction    行動優先度再計算フラグをセットする対象の行動
 */
void ServerFlow::setRecalcPriorityRequest( ACTION_ORDER_WORK* actionOrder, u8 actionOrderNum, u8 targetPokeID, BtlAction targetAction ) const
{
  for( u32 i=0; i<actionOrderNum; ++i )
  {
    u8 pokeID = actionOrder[i].bpp->GetID();
    BtlAction action = static_cast<BtlAction>( actionOrder[i].action.gen.cmd );

    if( ( pokeID == targetPokeID ) &&
        ( action == targetAction ) )
    {
      actionOrder[i].fRecalcPriority = true;
    }
  }
}

/**
 * @brief 先頭に在る技アクションのインデックスを取得する
 * @param[out]  headWazaActIndex  先頭に在る技アクションのインデックス
 * @param       actionOrder       処理順に並んだアクション配列
 * @param       actionOrderNum    処理順に並んだアクションの総数
 * @retval true   技アクションが存在する場合
 * @retval false  技アクションが存在しない場合
 */
bool ServerFlow::getHeadWazaActIndex( u8* headWazaActIndex, const ACTION_ORDER_WORK* actionOrder, u8* actionOrderNum ) const
{
  for( u32 i=0; i<*actionOrderNum; ++i )
  {
    if( ( actionOrder[i].action.gen.cmd == BTL_ACTION_FIGHT ) ||
        ( actionOrder[i].action.gen.cmd == BTL_ACTION_SKIP ) )
    {
      *headWazaActIndex = i;
      return true;
    }
  }

  return false;
}

/**
 * @brief ターン開始時における、IntrudeSystem の更新
 */
void ServerFlow::updateIntrudeSystemOnTurnStart( void )
{
  if( m_intrudeSystem == NULL )
  {
    return;
  }
  const u8   prevCallPokeId = m_intrudeParam.callPokeID;
  const bool prevAppeared   = ( m_intrudeParam.appearPokePos != BTL_POS_NULL );
  m_intrudeSystem->SetPrevCallResult( prevCallPokeId, prevAppeared );
  m_intrudeSystem->ClearBatugunDamageRecord();
}

/**
 * @brief 場にいる全てのポケモンについて、「このターン内において場に出た」ことを登録する
 */
void ServerFlow::registerFrontPokemonBattleIn( void )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    u8 pokeId = bpp->GetID();
    m_pokemonBattleInRegister.Register( pokeId );
  }
}

/**
 * @brief ServerFlow::Result から、１ターンが終了したかどうかをチェックする
 * @param flowResult  チェック対象のフロー計算結果
 * @retval true   １ターンが終了した
 * @retval false  １ターンが終了していない
 */
bool ServerFlow::isTurnEnd( Result flowResult ) const
{
  if( ( m_flowResult == RESULT_DEFAULT           ) ||
      ( m_flowResult == RESULT_BTL_QUIT_SHOWDOWN ) ||
      ( m_flowResult == RESULT_BTL_QUIT_ESCAPE   ) ||
      ( m_flowResult == RESULT_BTL_QUIT_CAPTURE  ) )
  {
    return true;
  }

  return false;
}

/**
 * @brief ターン終了時のコマンド生成処理
 */
void ServerFlow::scproc_OnTurnEnd( void )
{
  scEvent_TurnEnd();
  scproc_IncrementPokemonTotalTurnCount();
  scproc_UpdateCurrentRoyalRanking();
  scPut_IncBattleTurnCount();

  ItemChangeCounter_Clear();
}

/**
* @brief ターン終了イベント
*/
void ServerFlow::scEvent_TurnEnd(void)
{
  EVENTVAR_Push();
    EVENT_CallHandlers(this, BTL_EVENT_TURN_END);
  EVENTVAR_Pop();
}

/**
 * @brief 各ポケモンの、場に出たターンの総数をインクリメントする
 */
void ServerFlow::scproc_IncrementPokemonTotalTurnCount( void )
{
  for( u8 pokeId=0; pokeId<BTL_POKEID_MAX; ++pokeId )
  {
    if( m_pokemonBattleInRegister.Check( pokeId ) )
    {
      scPut_IncTotalTurnCount( pokeId );
    }
  }

  m_pokemonBattleInRegister.Clear();
}

/**
 * @brief サーバーコマンド出力( 場に出たターンの総数をインクリメント )
 * @param pokeId  コマンド出力対象ポケモンのID
 */
void ServerFlow::scPut_IncTotalTurnCount( u8 pokeId )
{
  BTL_PRINT( "[ServerFlow] 場に出たターンの総数をインクリメント pokeId=%d\n", pokeId );
  BTL_POKEPARAM* pokeParam = m_pokeCon->GetPokeParam( pokeId );
  pokeParam->IncTotalTurnCount();
  SCQUE_PUT_OP_IncPokeTurnCount( m_que, pokeId );

}

/**
 * @brief バトルロイヤルの現在順位を更新する
 */
void ServerFlow::scproc_UpdateCurrentRoyalRanking( void )
{
  if( m_mainModule->GetRule() != BTL_RULE_ROYAL )
  {
    return;
  }

  RoyalRankingCalculator::UpdateRanking( &m_royalRankingContainer, *m_mainModule, *m_pokeCon, m_partyAllDeadRecorder, m_timeLimit );

  const u8 rank_client0 = m_royalRankingContainer.GetClientRanking( 0 );
  const u8 rank_client1 = m_royalRankingContainer.GetClientRanking( 1 );
  const u8 rank_client2 = m_royalRankingContainer.GetClientRanking( 2 );
  const u8 rank_client3 = m_royalRankingContainer.GetClientRanking( 3 );
  SCQUE_PUT_OP_SetCurrentRoyalRanking( m_que, rank_client0, rank_client1, rank_client2, rank_client3 );

  BTL_PRINT( "[ServerFlow] バトルロイヤルの現在順位を更新しました( client0=%d, client1=%d, client2=%d, client3=%d\n", rank_client0, rank_client1, rank_client2, rank_client3 );
}



/**
 *
 */
void ServerFlow::ContinueAfterPokeChange_Boot( void )
{
  m_cmdBuildStep = 0;
}
//=============================================================================================
/**
 * サーバコマンド生成（ターン途中のポケモン入れ替え選択後、継続）
 *
 * @param   wk
 *
 * @retval  Result
 */
//=============================================================================================
ServerFlow::Result ServerFlow::ContinueAfterPokeChange( const SVCL_ACTION* clientAction )
{
  m_flowResult = RESULT_DEFAULT;
  SCQUE_Init( m_que );

  if( m_cmdBuildStep == 0 )
  {
    BTL_N_Printf( DBGSTR_SVFL_StartAfterPokeChange );

    m_server->InitChangePokemonReq();
    EVENTVAR_CheckStackCleared();

    m_actOrderStep = 0;
    m_cmdBuildStep = 1;

    if( ActOrderProc_OnlyPokeIn(clientAction) ){
      m_flowResult = RESULT_LEVELUP;
      return m_flowResult;
    }

    // @fix NMCat[428]「ききかいひ」「にげごし」でポケモンを繰り出す前に相手が行動してしまう。
    if( m_flowResult != RESULT_DEFAULT )
    {
      return m_flowResult;
    }
  }

  m_numEndActOrder = ActOrderProc_Main( m_numEndActOrder );

  if( isTurnEnd( m_flowResult ) )
  {
    scproc_OnTurnEnd();

    if( m_flowResult == RESULT_DEFAULT )
    {
      if( scproc_IntrudeCall() )
      {
        m_flowResult = RESULT_INTRUDE;
      }
    }
  }

  return m_flowResult;
}
void ServerFlow::StartAfterPokeIn_Boot( void )
{
  m_cmdBuildStep = 0;
}
//=============================================================================================
/**
 * サーバコマンド生成（ポケモン死亡・生き返りによるターン最初の入場処理）
 *
 * @param   wk
 *
 * @retval  Result
 */
//=============================================================================================
ServerFlow::Result ServerFlow::StartAfterPokeIn( const SVCL_ACTION* clientAction )
{
  const BTL_ACTION_PARAM* action;
  u32 i;
  u8 numDeadPoke = 0;

  m_flowResult =  RESULT_DEFAULT;
  SCQUE_Init( m_que );

  if( m_cmdBuildStep == 0 )
  {
    m_server->InitChangePokemonReq();
    EVENTVAR_CheckStackCleared();

    BTL_N_Printf( DBGSTR_SVFL_StartAfterPokeIn );

    numDeadPoke = m_deadRec.GetCount( 0 );

    generateActionOrder( clientAction, m_actOrder, &m_numActOrder, &m_recalcActOrderNeed, &m_recalcActOrderIndex );
    m_numEndActOrder = 0;

    // まずは「ポケモン」コマンドからの入れ替え処理
    for(i=0; i<m_numActOrder; i++)
    {
      action = &m_actOrder[i].action;
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      if( !m_actOrder[i].bpp->IsDead() )
      {
        BTL_N_Printf( DBGSTR_SVFL_AfterPokeIn_Alive,
                m_actOrder[i].clientID, action->change.posIdx, action->change.memberIdx );

        scproc_MemberChange( m_actOrder[i].bpp, action->change.memberIdx );
        m_actOrder[i].fDone = true;
      }
    }
    // 続けて空きを埋める繰り出し
    for(i=0; i<m_numActOrder; i++)
    {
      action = &m_actOrder[i].action;
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      if( m_actOrder[i].bpp->IsDead() )
      {
        BTL_N_Printf( DBGSTR_SVFL_AfterPokeIn_Dead,
                action->change.posIdx, action->change.memberIdx );

        scproc_MemberInForCover( m_actOrder[i].clientID, action->change.posIdx, action->change.memberIdx, true );
        m_actOrder[i].fDone = true;
      }
    }

    m_cmdBuildStep = 1;
  }

  scproc_AfterMemberIn();


  // NMCat[1024] この時点でポケモン入れ替えの要求がある場合、以降のひん死チェックを通さず、
  // まずは入れ替えポケモンの指定を実行する。
  if( m_flowResult == RESULT_POKE_CHANGE ){
    return  m_flowResult;
  }

  scproc_CheckExpGet();
  reqChangePokeForServer( &m_clientIDRec );

  Result result = RESULT_DEFAULT;
  {
    u8 numDeadPokeAfter = m_deadRec.GetCount( 0 );
    if( numDeadPoke == numDeadPokeAfter )
    {
      result = m_flowResult;  // @fix NMCat[429] 瀕死後の入れ替え処理で、「ききかいひ」「にげごし」の効果が発動するとはまってしまう
    }
    else
    {
      if( !scproc_CheckShowdown() )
      {
        scPut_IllusionSet( m_clientIDRec );
        result = RESULT_POKE_COVER;
      }
      else
      {
        result = RESULT_BTL_QUIT_SHOWDOWN;
      }
    }
  }

  if( isTurnEnd( result ) )
  {
    scproc_OnTurnEnd();

    if( result == RESULT_DEFAULT )
    {
      if( scproc_IntrudeCall() )
      {
        result = RESULT_INTRUDE;
      }
    }
  }

  return result;
}

//=============================================================================================
/**
 * @brief サーバコマンド生成( 乱入ポケモン入場 )
 *
 * @retval RESULT_DEFAULT  
 */
//=============================================================================================
void ServerFlow::IntrudePokeIn( void )
{
  SCQUE_Init( m_que );

  scproc_IntrudeAppear();
}

/**
 * @brief 乱入ポケモン入場処理
 */
void ServerFlow::scproc_IntrudeAppear( void )
{
  // 乱入ポケモンの登場位置を管理対象として拡張
  m_posPoke.ExtendPos( *m_mainModule, m_intrudeParam.appearPokePos );

  // 入場処理
  u8 posIndex = m_mainModule->BtlPosToPosIdx( m_intrudeParam.appearPokePos );
  int nextPokeIndex = checkIntrudePokeIndex( m_pokeCon );
  if( nextPokeIndex < 0 ){
    GFL_ASSERT(0);
    nextPokeIndex = posIndex;
  }
  m_pokeCon->IntrudePokeID_Clear();
  scproc_MemberInCore( m_intrudeParam.callClientID, posIndex, (u8)nextPokeIndex );

  // 登場演出
  SCQUE_PUT_ACT_IntrudeAppear( m_que, m_intrudeParam.callPokePos, m_intrudeParam.appearPokePos );

  scproc_AfterMemberIn();

  // 死亡記録をクリア
  BTL_POKEPARAM* appearPoke = m_pokeCon->GetFrontPokeData( m_intrudeParam.appearPokePos );
  u8 appearPokeId = appearPoke->GetID();
  m_deadRec.Relive( appearPokeId );
  appearPoke->ClearDeadCause();

  // 乱入回数をインクリメント
  if( m_intrudeSystem != NULL )
  {
    m_intrudeSystem->IncIntruceCount();
    m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_RANNNYU );
  }

  // レコード：色違いポケモンと出会った回数
  // @fix NMCat[4283] 乱入時に「色違いのポケモンに出会った回数」のレコードがカウントされていません
  if (appearPoke->GetViewSrcData()->IsRare())
  {
    m_mainModule->RECORDDATA_Inc(Savedata::Record::RECID_RARE_COLOUR_ENC_COUNT);
  }
}
/**
 * @brief pokeCon に記録された乱入ポケモンIDを元に、そのポケモンのパーティ内Indexを取得する
 * @param[in]  pokeCon   
 * @return  新規乱入ポケモンのパーティ内Index（見つからない場合はマイナス値）
 */
int ServerFlow::checkIntrudePokeIndex( const POKECON* pokeCon )
{
  u8 intrudePokeID = pokeCon->IntrudePokeID_Get();
  if( intrudePokeID == BTL_POKEID_NULL )
  {
    GFL_ASSERT(0);
    return -1;
  }

  u8 clientID = PokeID::PokeIdToClientId( intrudePokeID );
  return pokeCon->FindPokemon( clientID, intrudePokeID );
}

//----------------------------------------------------------------------------------
/**
 * コマンド生成コア（選択アクション配列から）
 *
 * @param   wk
 * @param   startOrderIdx   開始アクションIndex
 *
 * @retval  u32   処理し終えたアクション数
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::ActOrderProc_Main( u32 startOrderIdx )
{
  u32 i;
  u8 fExpGet, fShowDown;

  for(i=startOrderIdx; i<m_numActOrder; i++)
  {

    // 行動順番を再計算する
    // ( メガ進化したポケモンがいる場合に、メガ進化後のパラメータ・特性を考慮した行動順番にするため )
    if( ( m_recalcActOrderNeed ) &&
        ( i == m_recalcActOrderIndex ) )
    {
      sortClientAction( m_actOrder, m_recalcActOrderIndex, m_numActOrder-1, true );
    }

    ActOrder_Proc( &m_actOrder[i] );

    // レベルアップコマンド発生
    fExpGet = scproc_CheckExpGet();

    // 決着（大爆発など同時全滅のケースは、死亡レコードを見れば解決する）
    fShowDown = scproc_CheckShowdown();

    if( fShowDown && isSkipBattleAfterShowdown() ){
      m_flowResult = RESULT_BTL_QUIT_SHOWDOWN;
      return i+1;
    }

    if( m_flowResult == RESULT_BTL_QUIT_CAPTURE ){
      return i+1;
    }

    if( m_flowResult == RESULT_POKE_CHANGE ){
      // 途中交代しようとしてるなら死亡ポケの分はリクエスト出しちゃだめ
      return i+1;
    }

    if( fExpGet ){
      m_flowResult = RESULT_LEVELUP;
      return i+1;
    }

    if( m_flowResult == RESULT_BTL_QUIT_ESCAPE ){
      continue;
    }

    GFL_ASSERT_MSG( m_flowResult==RESULT_DEFAULT, "result=%d\n", m_flowResult );
    BTL_N_Printf( DBGSTR_SVFL_QueWritePtr, m_que->writePtr );
  }

  // ターンチェック
  if( m_flowResult == RESULT_DEFAULT )
  {
    u8 numDeadPoke;

    // ターンチェック処理
    TurnCheckResult turnCheckResult = scproc_TurnCheck();
    fShowDown = scproc_CheckShowdown();

    if( fShowDown )
    {
      m_flowResult = RESULT_BTL_QUIT_SHOWDOWN;
      return m_numActOrder;
    }

    if( turnCheckResult == TURNCHECK_RESULT_LEVELUP )
    {
      m_flowResult = RESULT_LEVELUP;
      return m_numActOrder;
    }
    if( turnCheckResult == TURNCHECK_RESULT_POKECHANGE )
    {
      m_flowResult = RESULT_POKE_CHANGE;
      return m_numActOrder;
    }
    if( turnCheckResult == TURNCHECK_RESULT_QUIT )
    {
      m_flowResult = RESULT_BTL_QUIT_ESCAPE;
      return m_numActOrder;
    }

    numDeadPoke = m_deadRec.GetCount( 0 );
    // 死亡・生き返りなどでポケ交換の必要があるかチェック
    if( (m_deadRec.IsRelivedPokePuttableEmptyPos(m_posPoke))
    ||  (numDeadPoke != 0)
    ){
      reqChangePokeForServer( &m_clientIDRec );
      scPut_IllusionSet( m_clientIDRec );

      m_flowResult = RESULT_POKE_COVER;
      return m_numActOrder;
    }

    m_flowResult = RESULT_DEFAULT;
  }

  return m_numActOrder;
}

/**
 * @brief 決着が着いた後、そのターンの残りの戦闘処理をスキップするか？
 * @retval true   決着が着いた後は、残りの戦闘処理をスキップする
 * @retval false  決着が着いた後も、残りの戦闘処理を行う
 */
bool ServerFlow::isSkipBattleAfterShowdown( void ) const
{
  BtlRule rule = m_mainModule->GetRule();
  return BattleRule::IsSkipBattleAfterShowdown( rule );
}

//----------------------------------------------------------------------------------
/**
 * コマンド生成（ポケモン入場アクションのみ処理）
 *
 * @param   wk
 *
 * @retval  bool  入場後の処理でポケモンが死亡->経験値取得でレベルアップした場合にtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlow::ActOrderProc_OnlyPokeIn( const SVCL_ACTION* clientAction )
{
  SVCL_WORK* clwk;
  BTL_ACTION_PARAM action;
  u32 i, j, actionCnt;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = m_server->GetClientWork( i );
    if( clwk == NULL ){
      continue;
    }

    actionCnt = SVCL_ACTION_GetNumAction( clientAction, i );
    for(j=0; j<actionCnt; ++j)
    {
      action = SVCL_ACTION_Get( clientAction, i, j );
      if( action.gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action.change.depleteFlag ){ continue; }

      BTL_N_Printf( DBGSTR_SVFL_PokeChangeRootInfo,
            i, action.change.posIdx, action.change.memberIdx );

      scproc_MemberInForChange( i, action.change.posIdx, action.change.memberIdx, true );
    }
  }
  scproc_AfterMemberIn( );
  return scproc_CheckExpGet();
}

//----------------------------------------------------------------------------------
/**
 * 現在のパーティデータ比較し、決着がついたか判定
 *
 * @param   wk
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_CheckShowdown( void ) const
{
  u8 pokeExist[ BTL_SIDE_NUM ];
  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) {
    pokeExist[ side ] = false;
  }

  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( m_mainModule->IsExistClient(i) )
    {
      const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( i );
      u8 side = m_mainModule->GetClientSide( i );
      u8 aliveCnt = party->GetAliveMemberCount();
      if( aliveCnt )
      {
        pokeExist[ side ] = true;
      }
    }
  }

  for( u32 side=0; side<BTL_SIDE_NUM; ++side ) 
  {
    if( ( m_mainModule->IsSideExist( side ) ) &&
        ( pokeExist[ side ] == false ) ) {
      return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlow::checkPlayerSideAlive( void )
{
  for(u32 i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( m_mainModule->IsExistClient(i) )
    {
      u8 side = m_mainModule->GetClientSide( i );
      u8 playerSide = m_mainModule->GetClientSide( m_mainModule->GetPlayerClientID() );

      if( side == playerSide )
      {
        BTL_PARTY* party = m_pokeCon->GetPartyData( i );
        if( party->GetAliveMemberCount() )
        {
          return true;
        }
      }
    }
  }
  return false;
}

//=============================================================================================
/**
 * サーバコマンド生成（次のポケモンを出す or にげる -> にげるを選んだ時の処理）
 *
 * @param   wk
 *
 * @retval  bool    逃げることが出来たらtrue
 */
//=============================================================================================
bool ServerFlow::MakePlayerEscapeCommand( void )
{
  u8 clientID = m_mainModule->GetPlayerClientID();
  BTL_POKEPARAM* bpp = m_pokeCon->GetClientPokeData( clientID, 0 );

  SCQUE_Init( m_que );

  if( scproc_NigeruCmd_Root(bpp) )
  {
    scproc_OnTurnEnd();
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------------
/**
 * サーバに対し、各クライアントの空き位置を通知
 *
 * @param   wk
 *
 * @retval  bool  誰かが死んでいて交換する必要ある場合はtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlow::reqChangePokeForServer( CLIENTID_REC* rec )
{
  u8 posAry[ BTL_POSIDX_MAX ];
  u8 empty_pos_cnt, clientID, i;
  u8 result = false;

  rec->count = 0;

  for(clientID=0; clientID<BTL_CLIENT_MAX; ++clientID)
  {
    empty_pos_cnt = m_posPoke.GetClientEmptyPos( clientID, posAry );
    if( empty_pos_cnt )
    {
      // 空いている位置を全てサーバへ通知
      BTL_N_Printf( DBGSTR_SVFL_ChangePokeReqInfo, clientID, empty_pos_cnt );
      for(i=0; i<empty_pos_cnt; ++i)
      {
        m_server->RequestChangePokemon( posAry[i] );
        BTL_N_PrintfSimple( DBGSTR_csv, posAry[i] );
      }
      BTL_N_PrintfSimple( DBGSTR_LF );

      rec->clientID[ rec->count++ ] = clientID;
      result = true;
    }
  }
  return result;
}
//=============================================================================================
/**
 * 逃げたクライアントIDを取得
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
const EscapeInfo* ServerFlow::GetEscapeInfoPointer( void ) const
{
  return &(m_escInfo);
}
//=============================================================================================
/**
 * 捕獲したポケモンの位置を取得
 *
 * @param   wk
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos ServerFlow::GetCapturedPokePos( void ) const
{
  return m_getPokePos;
}

//----------------------------------------------------------------------------------
/**
 * [Proc] ターン最初の「たたかう」コマンド処理前に実行する処理
 */
//----------------------------------------------------------------------------------
void ServerFlow::scproc_BeforeFirstFight( void )
{
  u32 hem_state, i;
  WazaID waza;

  hem_state = m_HEManager.PushState();
  for(i=0; i<m_numActOrder; ++i)
  {
    waza = ActOrder_GetWazaID( &m_actOrder[i] );
    if( waza != WAZANO_NULL )
    {
      BTL_POKEPARAM* bpp = m_actOrder[i].bpp;
      if( handler::waza::Add(bpp, waza, bpp->GetValue(BTL_POKEPARAM::BPP_AGILITY)) )
      {
        scEvent_BeforeFight( bpp, waza );
        handler::waza::Remove( bpp, waza );
      }
    }
  }
//  scproc_HandEx_Root( ITEM_DUMMY_DATA );
  m_HEManager.PopState( hem_state );
}




//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

/**
 * @brief 全クライアントの行動内容を ACTION_ORDER_WORK 配列に格納する
 *
 * @param[in]   clientAction    クライアントアクションデータハンドラ
 * @param[out]  order           アクションオーダー配列
 *
 * @retval アクションオーダーの総数
 */
u8 ServerFlow::storeClientAction( const SVCL_ACTION* clientAction, ACTION_ORDER_WORK* order )
{
  SVCL_WORK* clwk;
  u8 orderNum = 0;

  // 全ポケモンの行動内容をワークに格納
  for( u8 clientId=0; clientId<BTL_CLIENT_MAX; ++clientId )
  {
    clwk = m_server->GetClientWork( clientId );
    if( clwk == NULL ){ continue; }

    u8 numAction = SVCL_ACTION_GetNumAction( clientAction, clientId );
    u8 pokeIdx = 0;
    BTL_N_Printf( DBGSTR_SVFL_ActionSortInfo, clientId, numAction );
    for( u8 actionIndex=0; actionIndex<numAction; actionIndex++ )
    {
      order[ orderNum ].action = SVCL_ACTION_Get( clientAction, clientId, actionIndex );

      if( BTL_ACTION_IsDeplete( &order[ orderNum ].action ) ){
        break;
      }

      BtlAction action = BTL_ACTION_GetAction( &order[ orderNum ].action );
      BTL_PRINT("Client[%d] Acttion %d/%d = %d, pokeIdx=%d\n", clientId, actionIndex+1, numAction, action, pokeIdx );

      switch( action )
      {
      case BTL_ACTION_MEGAEVO:
        order[ orderNum ].bpp = clwk->party->GetMemberData( order[ orderNum ].action.mega_evo.pokeIdx );
        break;

      case BTL_ACTION_CHANGE:
        order[ orderNum ].bpp = clwk->party->GetMemberData( order[ orderNum ].action.change.posIdx );
        ++pokeIdx;
        break;

      default:
        order[ orderNum ].bpp = clwk->party->GetMemberData( pokeIdx );
        ++pokeIdx;
        break;
      }

      order[ orderNum ].clientID        = clientId;
      order[ orderNum ].fDone           = false;
      order[ orderNum ].fIntrCheck      = false;
      order[ orderNum ].fRecalcPriority = false;

      ActionDesc_Clear( &( order[ orderNum ].actionDesc ) );
      ++orderNum;
    }
  }

  return orderNum;
}


/**
 * @brief 全アクションの処理順序を確定する
 *
 * @param[in,out]  order       全アクションが格納されている配列
 * @param[in]      orderNum    アクションの総数
 * @param[in]      onlyRecalc  再計算リクエスト対象のみ、優先度を計算する
 */
void ServerFlow::sortClientAction( ACTION_ORDER_WORK* order, u8 orderNum, bool onlyRecalc )
{
  if( orderNum <= 0 )
  {
    return;
  }
  sortClientAction( order, 0, orderNum-1, onlyRecalc );
}

/**
 * @brief 指定区間のアクションの処理順序を確定する
 *
 * @param[in,out]  order       全アクションが格納されている配列
 * @param[in]      headIndex   ソート対象区間の先頭インデックス
 * @param[in]      tailIndex   ソート対象区間の末尾インデックス
 * @param[in]      onlyRecalc  再計算リクエスト対象のみ、優先度を計算する
 */
void ServerFlow::sortClientAction( ACTION_ORDER_WORK* order, u8 headIndex, u8 tailIndex, bool onlyRecalc )
{
  // 各アクションの行動プライオリティ値を生成
  for( u32 i=headIndex; i<=tailIndex; ++i )
  {
    // 再計算リクエスト対称でないならスキップ
    if( onlyRecalc && !order[i].fRecalcPriority )
    {
      continue;
    }

    order[i].priority = scproc_CalcActionPriority( order[i] );
  }

  // プライオリティ値によるソート
  sortActionSub( order, headIndex, tailIndex );

  
  // プライオリティ操作イベント呼び出し
  for( u32 i=headIndex; i<=tailIndex; ++i )
  {
    // 再計算リクエスト対称でないならスキップ
    if( onlyRecalc && !order[i].fRecalcPriority )
    {
      continue;
    }

    if( order[i].action.gen.cmd == BTL_ACTION_FIGHT )
    {
      u32 hem_state = m_HEManager.PushState();
      u8 spPri = scEvent_CheckSpecialActPriority( order[i].bpp );
      order[i].priority = ActPri_SetSpPri( order[i].priority, spPri );
      scPut_SetSpActPriority( order[i].bpp, spPri );
      m_HEManager.PopState( hem_state );
    }
  }

  // 再度、プライオリティ値によるソート
  sortActionSub( order, headIndex, tailIndex );
}

/**
 * @brief 行動優先度を計算する
 * @param order  計算対象の行動
 * @return 行動優先度
 */
u32 ServerFlow::scproc_CalcActionPriority( const ACTION_ORDER_WORK& order )
{
  const BtlCompetitor competitor = m_mainModule->GetCompetitor();
  const WazaID waza = ActOrder_GetWazaID( &order );

  // すばやさ
  u32 hem_state = m_HEManager.PushState();
  u16 agility = scEvent_CalcAgility( order.bpp, true );
  m_HEManager.PopState( hem_state );

  // 技優先度
  u8 wazaPri = 0;
  if( order.action.gen.cmd == BTL_ACTION_FIGHT )
  {
    u32 hem_state = m_HEManager.PushState();
    wazaPri = scEvent_GetWazaPriority( waza, order.bpp );
    m_HEManager.PopState( hem_state );
  }

  return ActPri_Calc( competitor, order.bpp, &(order.action), order.clientID, agility, waza, wazaPri );
}

//----------------------------------------------------------------------------------
/**
 * アクション内容ワークをプライオリティ値降順にソートする
 *
 * @param   order
 * @param   numOrder
 */
//----------------------------------------------------------------------------------
void ServerFlow::sortActionSub( ACTION_ORDER_WORK* order, u8 headIndex, u8 tailIndex )
{
  ACTION_ORDER_WORK tmp;

  for(u32 i=headIndex; i<tailIndex; ++i)
  {
    for(u32 j=i+1; j<=tailIndex; ++j)
    {
      if( order[i].priority > order[j].priority ){
        continue;
      }
      if( order[i].priority == order[j].priority ){ // 全く同じプライオリティ値があったらランダム
        if( calc::GetRand(2) == 0 ){
          continue;
        }
      }

      tmp = order[i];
      order[i] = order[j];
      order[j] = tmp;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * なかよし度を判定（野生戦＆通常トレーナー戦のみ、値を返す／それ以外は０を返す）
 *
 * @param   bpp
 *
 * @retval  u8    なかよし度
 */
//----------------------------------------------------------------------------------
u8 ServerFlow::getFriendship( const BTL_POKEPARAM* bpp )
{
  return m_mainModule->GetPokeFriendship( bpp );
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザプライオリティ取得
 *
 * @param   wk
 * @param   waza
 * @param   bpp
 *
 * @retval  u8  (0 origin）
 */
//----------------------------------------------------------------------------------
u8 ServerFlow::scEvent_GetWazaPriority( WazaID waza, const BTL_POKEPARAM* bpp )
{
  int pri = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_PRIORITY ) - pml::wazadata::WAZAPRI_MIN;
  pml::PokeType  waza_type = WAZADATA_GetType( waza );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, waza_type );
    EVENTVAR_SetValue( BTL_EVAR_WAZA_PRI, pri );
    EVENT_CallHandlers( this, BTL_EVENT_GET_WAZA_PRI );
    pri = EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
  EVENTVAR_Pop();

  return pri;
}

//--------------------------------------------------------------------------
/**
 * [Event] すばやさ取得
 *
 * @param   wk
 * @param   attacker
 * @param   fTrickRoomEnable
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------
u16 ServerFlow::scEvent_CalcAgility( const BTL_POKEPARAM* attacker, bool fTrickRoomEnable )
{
  u32 agi = attacker->GetValue( BTL_POKEPARAM::BPP_AGILITY );
  
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_AGILITY, agi );
    EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, true ); // まひチェックフラグとして使っている
    EVENTVAR_SetValue( BTL_EVAR_TRICK_FLAG, false );
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );

    EVENT_CallHandlers( this, BTL_EVENT_CALC_AGILITY );

    agi = EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    {
      fx32 ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
      agi = calc::MulRatio( agi, ratio );
    }
    if( attacker->GetPokeSick() == pml::pokepara::SICK_MAHI )
    {
      if( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
      }
    }
    if( agi > BTL_CALC_AGILITY_MAX ){
      agi = BTL_CALC_AGILITY_MAX;
    }

    // トリックルームでひっくりかえる
    if( fTrickRoomEnable )
    {
      if( EVENTVAR_GetValue(BTL_EVAR_TRICK_FLAG) ){
        agi = BTL_CALC_AGILITY_MAX - agi;
      }
    }

  EVENTVAR_Pop();

  return agi;
}

// 指定ポケモンのアクション内容データポインタを取得
ACTION_ORDER_WORK* ServerFlow::ActOrderTool_SearchByPokeID(u8 pokeID, bool bSkipMegaEvo )
{
  return ActOrderTool_SearchByPokeID_Core( pokeID, 0, bSkipMegaEvo );
}
// 指定ポケモンのアクション内容データポインタを取得（指定ポインタ以降から）
btl::ACTION_ORDER_WORK* ServerFlow::ActOrderTool_SearchNextByPokeID( const ACTION_ORDER_WORK* start, u8 pokeID, bool bSkipMegaEvo )
{
  u32 i;
  for(i=0; i<m_numActOrder; ++i)
  {
    if( (uptr)(&(m_actOrder[i])) == (uptr)(start) ){
      ++i;
      break;
    }
  }
  return ActOrderTool_SearchByPokeID_Core( pokeID, i, bSkipMegaEvo );
}
// ActOrderTool_SearchByPokeID, ActOrderTool_SearchNextByPokeID から呼び出される共通処理
// 指定 Index から該当アクション内容データポインタを検索して返す（なければNULL）
btl::ACTION_ORDER_WORK*
  ServerFlow::ActOrderTool_SearchByPokeID_Core( u8 pokeID, u8 startIdx, bool bSkipMegaEvo )
{
  for( u32 i=startIdx; i<m_numActOrder; ++i)
  {
    // メガシンカはスキップする
    if( bSkipMegaEvo )
    {
      if( BTL_ACTION_IsMegaEvoParam(&(m_actOrder[i].action)) ){
        continue;
      }
    }

    if( m_actOrder[i].bpp->GetID() == pokeID ){
      return &(m_actOrder[i]);
    }
  }
  return NULL;
}


// 未実行の指定ワザ使用アクション内容データポインタを取得
ACTION_ORDER_WORK*  ServerFlow::ActOrderTool_SearchByWazaID( WazaID waza, u8 startIndex )
{
  for(u32 i=startIndex; i<m_numActOrder; ++i)
  {
    if( m_actOrder[i].fDone == false )
    {
      if( (BTL_ACTION_GetAction(&m_actOrder[i].action) == BTL_ACTION_FIGHT)
      &&  (BTL_ACTION_GetWazaID(&m_actOrder[i].action, m_actOrder[i].bpp) == waza)
      ){
        return &m_actOrder[i];
      }
    }
  }
  return NULL;
}

/**
 * @brief 指定したポケモンについて、全ての行動が終了しているかチェックする
 * @param pokeID  チェック対象ポケモンのID
 * @retval true   終了している
 * @retval false  終了していない
 */
bool ServerFlow::ActOrderTool_CheckAllActDoneByPokeID( u8 pokeID ) const
{
  for( u32 i=0; i<m_numActOrder; ++i )
  {
    if( ( m_actOrder[i].fDone == false ) &&
        ( m_actOrder[i].bpp->GetID() == pokeID ) )
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------
/**
 * 合体ワザ対象となるアクション内容データポインタを取得（存在しない場合NULL）
 *
 * @param   wk
 * @param   waza        対象ワザID
 * @param   pokeID      先に合体ワザを打つポケID（このポケの味方のみが対象となる）
 * @param   targetPos   先に合体ワザを打つポケが狙っている位置（同位置を狙うワザのみ対象となる）
 *
 * @retval  ACTION_ORDER_WORK*
 */
//----------------------------------------------------------------------------------
ACTION_ORDER_WORK* ServerFlow::ActOrderTool_SearchForCombiWaza( WazaID waza, u8 pokeID, BtlPokePos targetPos )
{
  GFL_UNUSED( targetPos );

  ACTION_ORDER_WORK* p = ActOrderTool_SearchByWazaID( waza, 0 );
  while( p != NULL )
  {
    u8 checkPokeID = p->bpp->GetID();
    if( (m_mainModule->IsFriendPokeID(pokeID, checkPokeID))
    &&  ( checkPokeID != pokeID )
    &&  (!p->bpp->IsDead())
    ){
      return p;
    }

    {
      u8 idx = ActOrderTool_GetIndex( p );
      p = ActOrderTool_SearchByWazaID( waza, idx + 1 );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * アクション内容データポインタから自身のIndexを取得
 *
 * @param   wk
 * @param   actOrder
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8 ServerFlow::ActOrderTool_GetIndex( const ACTION_ORDER_WORK* actOrder )
{
  int i;
  for(i=0; i<m_numActOrder; ++i)
  {
    if( &(m_actOrder[i]) == actOrder ){
      return i;
    }
  }
  GFL_ASSERT(0);
  return m_numActOrder;
}
//----------------------------------------------------------------------------------
/**
 * 指定アクションを、指定index以降の未実行列の先頭に割り込ませる
 *
 * @param   wk
 * @param   actOrder
 * @param   intrIndex   割り込み許可する最小のindex（これ以降に割り込み許可）
 *
 * @retval  int   割り込んだIndex （割り込めなければ-1)
 */
//----------------------------------------------------------------------------------
int ServerFlow::ActOrderTool_Intr( const ACTION_ORDER_WORK* actOrder, u32 intrIndex )
{
  int prevOrder, topOrder, i;

  // 自分自身の元々のIndexと、未実行列の先頭Indexをサーチ
  prevOrder = topOrder = -1;
  for(i=intrIndex; i<m_numActOrder; ++i)
  {
    if( &(m_actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }
  for(i=intrIndex; i<m_numActOrder; ++i)
  {
    if( m_actOrder[i].fDone == false){
      topOrder = i;
      break;
    }
  }

  // どちらも有効値＆自分自身が未実行の先頭より後の順番なら並べ替え
  if( (prevOrder>=0) && (topOrder>=0)
  &&  (prevOrder > topOrder)
  ){
    m_actOrderTmp = *actOrder;
    while( prevOrder > topOrder ){
      m_actOrder[prevOrder] = m_actOrder[prevOrder-1];
      --prevOrder;
    }
    m_actOrder[topOrder] = m_actOrderTmp;
    return topOrder;
  }

  return -1;
}

//----------------------------------------------------------------------------------
/**
 * 指定アクションを、指定index以降の未実行列の先頭に挿入する
 *
 * @param   wk
 * @param   actOrder
 * @param   insertIndex   割り込み許可する最小のindex（これ以降に挿入許可）
 *
 * @retval  int   割り込んだIndex （割り込めなければ-1)
 */
//----------------------------------------------------------------------------------
s32 ServerFlow::ActOrderTool_Insert( const ACTION_ORDER_WORK& actOrder, u32 insertIndex )
{
  // 空きが無いなら失敗
  if( GFL_NELEMS(m_actOrder) <= m_numActOrder )
  {
    return -1;
  }

  // insertIndex 以降にある未実行アクションの先頭インデックスを検索
  s32 headIndex = m_numActOrder;
  for( u32 i=insertIndex; i<m_numActOrder; ++i )
  {
    if( m_actOrder[i].fDone == false )
    {
      headIndex = i;
      break;
    }
  }

  // 挿入位置以降のアクションを後ろにズラす
  for( s32 i=m_numActOrder-1; headIndex<=i; --i )
  {
    m_actOrder[i+1] = m_actOrder[i];
  }

  m_actOrder[ headIndex ] = actOrder;
  m_numActOrder++;
  return headIndex;
}

//----------------------------------------------------------------------------------
/**
 * 指定アクションを、最後に回す
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
void ServerFlow::ActOrderTool_SendToLast( const ACTION_ORDER_WORK* actOrder )
{
  // 自分自身の元々のIndex
  int prevOrder = -1;
  int i;

  for(i=0; i<m_numActOrder; ++i)
  {
    if( &(m_actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }

  if( prevOrder >= 0 )
  {
    m_actOrderTmp = *actOrder;
    for(i=prevOrder; i<(m_numActOrder-1); ++i)
    {
      m_actOrder[i] = m_actOrder[i+1];
    }
    m_actOrder[i] = m_actOrderTmp;
  }

  for(i=0; i<m_numActOrder; ++i)
  {
    BTL_PRINT("*ActOrder[%d] : act=%d, pokeID=%d\n", i, m_actOrder[i].action.gen.cmd, m_actOrder[i].bpp->GetID());
  }

}

//----------------------------------------------------------------------------------
/**
 *  アクション内容の実行
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
BtlAction ServerFlow::ActOrder_Proc( ACTION_ORDER_WORK* actOrder )
{
  if( actOrder->fDone == false )
  {
    BTL_ACTION_PARAM action = actOrder->action;

    PSetStack_Push();

    ActOrder_Proc_Core( actOrder );

    // おどりこ、さいはい等による割り込みアクション情報をクリア
    if( actOrder->actionDesc.insertInfoID != InsertWazaInfoStorage::ID_NULL ){
      m_insertWazaInfoStorage.RemoveParam( actOrder->actionDesc.insertInfoID );
    }

    PSetStack_Pop();

    BTL_N_Printf( DBGSTR_SVFL_ActOrderEnd, bpp->GetID(), bpp->GetMonsNo(), actOrder );
    return static_cast<BtlAction>( action.gen.cmd );
  }

  return BTL_ACTION_NULL;
}

/**
 *  アクション内容の実行
 *
 * @param   actOrder
 */
void ServerFlow::ActOrder_Proc_Core( ACTION_ORDER_WORK* actOrder )
{
  if( actOrder->fDone ) {
    GFL_ASSERT(0);
    return;
  }

  actOrder->fDone = true;

  BTL_POKEPARAM* bpp = actOrder->bpp;
  BTL_ACTION_PARAM action = actOrder->action;
  BTL_N_Printf( DBGSTR_SVFL_ActOrderStart, bpp->GetID(), bpp->GetMonsNo(), actOrder );


  // 既に誰かの逃げが確定している時、逃げコマンド処理以外は実行しない
  if( (m_flowResult == RESULT_BTL_QUIT_ESCAPE)
  &&  (action.gen.cmd != BTL_ACTION_ESCAPE)
  ){
    return;
  }

  // 死んでたら実行しない
  if( bpp->IsDead() ){
    return;
  }

  // 場にいないポケは実行しない
  if(!m_posPoke.IsExist(bpp->GetID()))
  {
    return;
  }

  // フリーフォール状態のポケは実行しない
  if( bpp->CheckSick( pml::wazadata::WAZASICK_FREEFALL) )
  {
    if( (action.gen.cmd != BTL_ACTION_ESCAPE)
    ||  (m_mainModule->GetEscapeMode() != BTL_ESCAPE_MODE_CONFIRM)
    )
    {
      if( bpp->COUNTER_Get(BTL_POKEPARAM::COUNTER_MAMORU) ){
        scPut_SetBppCounter( bpp, BTL_POKEPARAM::COUNTER_MAMORU, 0 );
      }
      scEvent_ActProcCanceled( bpp, &action );
      return;
    }
  }

  // おどりこ、さいはい等による割り込みアクション情報を出力する
  if( actOrder->actionDesc.insertInfoID != InsertWazaInfoStorage::ID_NULL ){
    scPut_InsertWazaInfo( m_insertWazaInfoStorage.GetParam(actOrder->actionDesc.insertInfoID) );
  }

  scproc_ActStart( bpp, &(actOrder->action) );
  bpp->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_ACTION_START );

  switch( action.gen.cmd ){
  case BTL_ACTION_MEGAEVO:
    BTL_PRINT("メガ進化します\n");
    scproc_MegaEvo( bpp );
    break;

  case BTL_ACTION_FIGHT:
    if( !FlowFlg_Get(FLOWFLG_FIRST_FIGHT) ){
      scproc_BeforeFirstFight();
      FlowFlg_Set( FLOWFLG_FIRST_FIGHT );
    }
    BTL_N_Printf( DBGSTR_SVFL_ActOrder_Fight, bpp->GetID(), action.fight.waza, action.fight.targetPos );
    m_currentSabotageType = checkSabotageType( bpp );
    scproc_Fight( bpp, &action, &(actOrder->actionDesc), ActPri_ToHandlerPri(actOrder->priority), ActPri_ToWazaOrgPri(actOrder->priority) );
    break;

  case BTL_ACTION_ITEM:
    BTL_N_Printf( DBGSTR_SVFL_ActOrder_Item, action.item.number, action.item.targetIdx);
    scproc_UseTrainerItem( bpp, action.item.number, action.item.param, action.item.targetIdx );
    break;

  case BTL_ACTION_CHANGE:
    BTL_Printf( DBGSTR_SVFL_ActOrder_Change, action.change.posIdx, action.change.memberIdx);
    scproc_MemberChange( bpp, action.change.memberIdx );
    scproc_AfterMemberIn();
    break;
  case BTL_ACTION_ESCAPE:
    BTL_N_Printf( DBGSTR_SVFL_ActOrder_Escape );
    if( scproc_NigeruCmd_Root( bpp ) ){
      m_flowResult = RESULT_BTL_QUIT_ESCAPE;
    }
    break;
  case BTL_ACTION_RECPLAY_TIMEOVER:
    m_flowResult = RESULT_BTL_QUIT_SHOWDOWN;
    break;
  case BTL_ACTION_SKIP:
    scPut_CantAction( bpp );
    break;
  case BTL_ACTION_NULL:
    BTL_N_Printf( DBGSTR_SVFL_ActOrder_Dead );
    break;
  }

  if( (action.gen.cmd == BTL_ACTION_FIGHT) || (action.gen.cmd == BTL_ACTION_ITEM) )
  {
    bpp->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_ACTION_DONE );
    scPut_SetContFlag( bpp, BTL_POKEPARAM::CONTFLG_ACTION_DONE );
  }

  // 逃げることが確定したならもうアクション終了イベントハンドラは呼び出さない
  if( m_flowResult != RESULT_BTL_QUIT_ESCAPE )
  {
    u32 hem_state = m_HEManager.PushState();
    scEvent_ActProcEnd( bpp, static_cast<BtlAction>(action.gen.cmd) );
    m_HEManager.PopState( hem_state );
  }
}

/**
 * @brief トレーナーの道具使用
 */
void ServerFlow::scproc_UseTrainerItem( BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 shortPokeID )
{
  TrItemResult result = m_flowSub.TrainerItemProc( this, bpp, itemID, actParam, shortPokeID, &m_capturedContext );

  switch( result ){
  case TIITEM_RESULT_CAPTURED:
    scproc_CheckExpGetByCaptured( MainModule::PokeIDtoClientID(bpp->GetID()) );
    m_flowSub.CreateCapturedCmd( m_que, &m_capturedContext );
    break;

  case TRITEM_RESULT_ESCAPE:
    if( scproc_NigeruCore( bpp, true, true) ){
      m_flowResult = RESULT_BTL_QUIT_ESCAPE;
    }
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * アクション実行開始通知処理
 *
 * @param   wk
 * @param   bpp
 * @param   actionCmd
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_ActStart( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_ActProcStart( bpp, action );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] アクション実行開始
 *
 * @param   wk
 * @param   bpp
 * @param   actionCmd
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_ActProcStart( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action )
{
  WazaID  waza = BTL_ACTION_GetOriginalWazaID( action );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION, action->gen.cmd );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENT_CallHandlers( this, BTL_EVENT_ACTPROC_START );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] アクション実行の終了
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_ActProcEnd( const BTL_POKEPARAM* bpp, BtlAction actionCmd )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION, actionCmd );
    EVENT_CallHandlers( this, BTL_EVENT_ACTPROC_END );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] アクション実行のキャンセル
 *
 * @param   bpp        対象ポケモン
 * @param   actionCmd  アクション内容
 * @detail  現状アクション実行がキャンセルされる原因はフリーフォールのみだが
 *          将来的には他の原因でキャンセルされた場合にもこのイベントを使うことを想定
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_ActProcCanceled( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* actionCmd )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, BTL_ACTION_GetWazaID(actionCmd, bpp) );
    EVENT_CallHandlers( this, BTL_EVENT_ACTPROC_CANCELED );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * 命令無視チェック
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  SabotageType
 */
//----------------------------------------------------------------------------------
ServerFlow::SabotageType  ServerFlow::checkSabotageType( const BTL_POKEPARAM* bpp )
{
  if( m_mainModule->GetRule() == BTL_RULE_ROYAL )
  {
    return SABOTAGE_NONE;
  }

  if( m_mainModule->IsCompetitorScenarioMode()
  &&  (MainModule::PokeIDtoClientID(bpp->GetID()) == (m_mainModule->GetPlayerClientID()))
  ){
    const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();
    const Savedata::MyStatus* status = m_mainModule->GetPlayerStatus();
    if( !status->IsMyPokemon(*pp) )
    {
      u16 lv_poke = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL );
      u16 lv_border = m_mainModule->GetMaxFollowPokeLevel();
      u16 rand;

      if( lv_poke <= lv_border ){
        return SABOTAGE_NONE;
      }

      rand = calc::GetRand( (lv_poke + lv_border + 1) );
      if( rand < lv_border ){
        return SABOTAGE_NONE;
      }

      if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) ){
        return SABOTAGE_DONT_ANY;
      }

      // WBでは命令無視タイプ「他の技を出す」パターンを廃止
      #if 0
      rand = calc::GetRand( (lv_poke + lv_border + 1) );
      if( rand < lv_border ){
        if( BPP_WAZA_GetUsableCount(bpp) > 1 ){
          return SABOTAGE_OTHER_WAZA;
        }
      }
      #endif

      rand = calc::GetRand( 256 );
      if( rand < (lv_poke - lv_border) )
      {
        if( !scEvent_CheckInemuriFail(bpp) ){
          return SABOTAGE_GO_SLEEP;
        }
      }

      rand -= (lv_poke - lv_border);
      if( rand < (lv_poke - lv_border) ){
        return SABOTAGE_CONFUSE;
      }

      return SABOTAGE_DONT_ANY;
    }
  }

  return SABOTAGE_NONE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 命令無視によるいねむり成否チェック
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool    いねむり失敗する場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckInemuriFail( const BTL_POKEPARAM* bpp )
{
  bool fFail;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_INEMURI );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  return fFail;
}

//--------------------------------------------------------------
/**
 *  アクションの割り込み実行（ポケモンID指定）
 *  ※割り込んで即座に実行（現状は“おいうち”にのみ使用）
 */
//--------------------------------------------------------------
bool ServerFlow::ActOrder_IntrProc( u8 intrPokeID, u8 targetPokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( intrPokeID, false );

  if( actOrder )
  {

    // 既に処理済みアクションを見つけた場合、次のアクションをサーチする
    while( actOrder->fDone )
    {
      actOrder = ActOrderTool_SearchNextByPokeID( actOrder, intrPokeID, false );
      if( !actOrder ){
        break;
      }
    }

    if( actOrder && actOrder->fDone == false )
    {
      // アンコール状態で違うワザを選んでいた場合は割り込み不可
      //（とんぼがえり入れ替えなどの場合に起こりうる）
      {
        WazaID encoreWaza = checkEncoreWazaChange( actOrder->bpp, &(actOrder->action), &(actOrder->actionDesc) );
        if( encoreWaza != WAZANO_NULL )
        {
          return true;
        }
      }

      // メガ進化処理
      if( BTL_ACTION_GetAction(&actOrder->action) == BTL_ACTION_MEGAEVO )
      {
        ActOrder_Proc( actOrder );
        actOrder = ActOrderTool_SearchNextByPokeID( actOrder, intrPokeID, false );
      }

      actOrder->actionDesc.isOiutiInterruptAction = true;

      {
        BtlPokePos  targetPos = m_posPoke.GetPokeExistPos( targetPokeID );
        BTL_N_Printf( DBGSTR_SVFL_ActIntr, actOrder, actOrder->bpp->GetID());
        BTL_ACTION_ChangeFightTargetPos( &actOrder->action, targetPos );
        ActOrder_Proc( actOrder );
        return true;
      }
    }
  }

  return false;
}
//--------------------------------------------------------------
/**
 *  アクションの割り込み予約（ポケモンID指定）
 *  ※割り込んで、現在処理しているアクションの直後に実行
 * （現状は“おさきにどうぞ”と合体ワザにのみ使用）
 */
//--------------------------------------------------------------
bool ServerFlow::ActOrder_IntrReserve( u8 intrPokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID(intrPokeID, true);
  if( actOrder && (actOrder->fDone == false) ){
    if( ActOrderTool_Intr(actOrder, 0) >= 0 ){
      return true;
    }
  }
  return false;
}
//--------------------------------------------------------------
/**
 *  アクションの割り込み実行（使用ワザ指定）
 *  ※割り込んで、現在処理しているアクションの直後に実行（現状は“りんしょう”にのみ使用）
 */
//--------------------------------------------------------------
bool ServerFlow::ActOrder_IntrReserveByWaza( WazaID waza )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByWazaID( waza, 0 );
  int intrIdx = 0;
  bool result = false;
  while( actOrder )
  {
    intrIdx = ActOrderTool_Intr( actOrder, intrIdx );
    if( intrIdx >= 0 ){
      actOrder = ActOrderTool_SearchByWazaID( waza, ++intrIdx );
      result = true;
    }else{
      break;
    }
  }
  return result;
}
//--------------------------------------------------------------
/**
 *  アクションを最後に回す（ポケモンID指定）
 */
//--------------------------------------------------------------
bool ServerFlow::ActOrder_SendLast( u8 pokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID(pokeID, true);
  if( actOrder && (actOrder->fDone == false) ){
    BTL_PRINT("poke=%d, act=%d を最後に送るよ\n", actOrder->bpp->GetID(), actOrder->action.gen.cmd);
    ActOrderTool_SendToLast( actOrder );
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
 *  未実行アクションを実行済みにする（ポケモンID指定）
 */
//--------------------------------------------------------------
void ServerFlow::ActOrder_ForceDone( u8 pokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( pokeID, true );
  while( actOrder )
  {
    actOrder->fDone = true;
    actOrder = ActOrderTool_SearchNextByPokeID( actOrder, pokeID, true );
  }
}
/**
 *  ワザエフェクト発動管理：初期化
 */
void ServerFlow::wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl )
{
  ctrl->effectWazaID = WAZANO_NULL;
  ctrl->attackerPos = BTL_POS_NULL;
  ctrl->targetPos = BTL_POS_NULL;
  ctrl->effectIndex = 0;
  ctrl->fEnable = false;
  ctrl->fDone = false;

  wazaEffCtrl_ClearSubEffectParams( ctrl );
}
/**
 *  ワザエフェクト発動管理：基本パラメータ設定
 */
void ServerFlow::wazaEffCtrl_Setup( WAZAEFF_CTRL* ctrl, const BTL_POKEPARAM* attacker, const PokeSet* rec )
{
  u32 target_max = rec->GetCountMax();

  ctrl->attackerPos = m_posPoke.GetPokeExistPos( attacker->GetID() );
  ctrl->targetPos = BTL_POS_NULL;
  ctrl->fEnable = false;
  ctrl->fDone = false;
  wazaEffCtrl_ClearSubEffectParams( ctrl );

  // ターゲットが単体なら明確なターゲット位置情報を記録
  if( target_max == 1 )
  {
    if( rec->GetCount() )
    {
      const BTL_POKEPARAM* bpp = rec->Get( 0 );
      if( bpp ){
        ctrl->targetPos = m_posPoke.GetPokeExistPos( bpp->GetID() );
      }
    }
  }
}
/**
 *  ワザエフェクト発動管理：攻撃ポケ位置のみ差し替え（遅延ワザ対応）
 */
void ServerFlow::wazaEffCtrl_ChangeAtkPos( WAZAEFF_CTRL* ctrl, BtlPokePos atkPos )
{
  ctrl->attackerPos = atkPos;
}
/**
 *  ポケモン存在＆生存チェック一時関数
 */
bool ServerFlow::IsBppExist( const BTL_POKEPARAM* bpp ) const
{
  if( bpp != NULL ){
    return !(bpp->IsDead());
  }
  return false;
}


//----------------------------------------------------------------------------------------------
// 戦闘に出ているポケモンデータ順番アクセス処理
//----------------------------------------------------------------------------------------------

/**
 *  順番アクセス：ワーク初期化
 */
void ServerFlow::FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw )
{
  fpsw->clientIdx = 0;
  fpsw->pokeIdx = 0;
  fpsw->endFlag = true;

  {
    SVCL_WORK* cw;
    u8 i, j;

    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      cw = m_server->GetClientWork( i );
      if( cw == NULL ){ continue; }

      const u32 coverPosNum = m_mainModule->GetClientCoverPosNum( cw->myID );
      for(j=0; j<coverPosNum; ++j)
      {
        if( IsBppExist( cw->party->GetMemberDataConst(j)) )
        {
          fpsw->clientIdx = i;
          fpsw->pokeIdx = j;
          fpsw->endFlag = false;
          return;
        }
      }
    }
  }
}

/**
 *  順番アクセス：次のポケモンデータを取得（falseが返ったら終了）
 */
bool ServerFlow::FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_POKEPARAM** bpp )
{
  if( fpsw->endFlag )
  {
    return false;
  }
  else
  {
    BTL_POKEPARAM* nextPoke = NULL;
    SVCL_WORK* cw = m_server->GetClientWork( fpsw->clientIdx );
    GFL_ASSERT_STOP(cw);

    *bpp = cw->party->GetMemberData( fpsw->pokeIdx );
    fpsw->pokeIdx++;

    while( fpsw->clientIdx < BTL_CLIENT_MAX )
    {
      cw = m_server->GetClientWork( fpsw->clientIdx );
      if( cw )
      {
        u8 posMax = m_mainModule->GetClientCoverPosNum( cw->myID );
        while( fpsw->pokeIdx < posMax )
        {
          nextPoke = cw->party->GetMemberData( fpsw->pokeIdx );
          if( IsBppExist(nextPoke) )
          {
            return true;
          }
          fpsw->pokeIdx++;
        }
      }
      fpsw->clientIdx++;
      fpsw->pokeIdx = 0;
    }
    fpsw->endFlag = true;
    return true;
  }
}
//======================================================================================================
// サーバーフロー処理
//======================================================================================================

//----------------------------------------------------------------------------------
/**
 * ムーブ動作コア
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx1
 * @param   posIdx2
 * @param   fActCmd
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MoveCore( u8 clientID, u8 posIdx1, u8 posIdx2, bool fResetMove )
{
  BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );
  BtlPokePos pos1, pos2;

  pos1 = m_mainModule->GetClientPokePos( clientID, posIdx1 );
  pos2 = m_mainModule->GetClientPokePos( clientID, posIdx2 );

  m_posPoke.Swap( pos1, pos2 );
  party->SwapMembers( posIdx1, posIdx2 );

  if( !fResetMove )
  {
    SCQUE_PUT_ACTOP_MemberMove( m_que, clientID, pos1, pos2 );

    {
      BTL_POKEPARAM* bpp1 = party->GetMemberData( posIdx1 );
      BTL_POKEPARAM* bpp2 = party->GetMemberData( posIdx2 );
      if( !bpp1->IsDead() ){
        bpp1->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_MOVED );
      }
      if( !bpp2->IsDead() ){
        bpp2->TURNFLAG_Set(  BTL_POKEPARAM::TURNFLG_MOVED );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ムーブ動作直後イベント呼び出し（ルート）
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx1
 * @param   posIdx2
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_AfterMove( u8 clientID, u8 posIdx1, u8 posIdx2 )
{
  BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );

  const BTL_POKEPARAM* bpp1 = party->GetMemberDataConst( posIdx1 );
  const BTL_POKEPARAM* bpp2 = party->GetMemberDataConst( posIdx2 );

  scproc_AfterMoveCore( bpp1 );
  scproc_AfterMoveCore( bpp2 );
}
//----------------------------------------------------------------------------------
/**
 * ムーブ動作直後イベント呼び出し
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_AfterMoveCore( const BTL_POKEPARAM* bpp )
{
  if( !bpp->IsDead() )
  {
    u32 hem_state = m_HEManager.PushState();
    scEvent_AfterMove( bpp );
    m_HEManager.PopState( hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ムーブ動作直後
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_AfterMove( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_AFTER_MOVE );
  EVENTVAR_Pop();
}


//-----------------------------------------------------------------------------------
// サーバーフロー：「にげる」
//-----------------------------------------------------------------------------------
/**
 * 逃げる処理ルート
 *
 *  @retval 逃げられたらtrue, 逃げ失敗で false
 */
bool  ServerFlow::scproc_NigeruCmd_Root( BTL_POKEPARAM* bpp )
{
  u8 clientID = MainModule::PokeIDtoClientID( bpp->GetID() );
  u8 playerClientID = m_mainModule->GetPlayerClientID();

  bool result = scproc_NigeruCmdSub( bpp, false, false );

  if( result )
  {
    return true;
  }

  if( clientID == playerClientID ){
    m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_NIGERU_SIPPAI );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_EscapeFail );
  }else{
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_EscapeFailed, bpp->GetID() );
  }

  if( clientID == playerClientID )
  {
    if( ++(m_nigeruCount) > NIGERU_COUNT_MAX ){
      m_nigeruCount = NIGERU_COUNT_MAX;
    }
  }

  return false;
}
/**
 * 逃げるコマンド選択処理下請け
 *
 * @param fSkipNigeruCalc  trueなら、素早さによる成功判定をスキップする
 * @param forceNigeru      trueなら、必ず逃げられる
 *
 *  @retval 逃げられたらtrue, 逃げ失敗でfalse
 */
bool ServerFlow::scproc_NigeruCmdSub( BTL_POKEPARAM* bpp, bool fSkipNigeruCalc, bool forceNigeru )
{
  bool fForceNigeru = forceNigeru;

  if( m_mainModule->GetEscapeMode() == BTL_ESCAPE_MODE_WILD )
  {
    // 強制逃げ発動チェック
    if( !fForceNigeru )
    {
      fForceNigeru = scproc_checkForceNigeru( bpp );
    }

    // プレイヤーは、強制逃げ状態だったら逃げ計算を免除
    u8 escClientID = MainModule::PokeIDtoClientID( bpp->GetID() );
    if( escClientID == m_mainModule->GetPlayerClientID() )
    {
      if( fForceNigeru ){
        fSkipNigeruCalc = true;
      }
    }
    // 野生ポケモンは無条件に逃げ計算を免除
    else{
      fSkipNigeruCalc = true;
    }
  }
  else{
    fSkipNigeruCalc = true;
    fForceNigeru = true;
  }

  // L or R 押しっぱなしで強制的に逃げる
  #if PM_DEBUG
  if( m_mainModule->GetUIManager()->GetButton(0)->IsHold(gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) )
  {
    fSkipNigeruCalc = true;
    fForceNigeru = true;
  }
  #endif

/*
#if DEBUG_AUTOTEST
  // オートテスト中も強制逃げる
  if ( xy_debug::DebugAutoTest_IsTestBoot() )
  {
    fSkipNigeruCalc = true;
    fForceNigeru = true;
  }
#endif 
*/

  if( !fSkipNigeruCalc )
  {
    const BTL_POKEPARAM* bppEnemy = m_pokeCon->GetClientPokeDataConst( BTL_CLIENTID_SA_ENEMY1, 0 );
    if( bppEnemy )
    {
      u16 myAgi = bpp->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );
      u16 enemyAgi = bppEnemy->GetValue_Base( BTL_POKEPARAM::BPP_AGILITY );

      if( myAgi <= enemyAgi )
      {
        u32 value = ((myAgi << FX32_SHIFT) / enemyAgi * 128) >> FX32_SHIFT;
        value += (m_nigeruCount * 30);
        if( calc::GetRand(256) >= value ){
          return false;
        }
      }
    }
  }

  return scproc_NigeruCore( bpp, fForceNigeru, false );
}
/**
 *  確率計算や、各種逃げ妨害要素を無視してにげることが出来る状態をチェック
 */
bool ServerFlow::scproc_checkForceNigeru( const BTL_POKEPARAM* bpp )
{
  if( bpp->IsMatchType(POKETYPE_GHOST) ){
    return true;
  }

  return scEvent_SkipNigeruCalc( bpp );
}

//----------------------------------------------------------------------------------
/**
 * [Event] 逃げる時の確率計算処理をスキップするか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool    スキップするならtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_SkipNigeruCalc( const BTL_POKEPARAM* bpp )
{
  if( !bpp->IsDead() )
  {
    bool result = false;
    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
      EVENT_CallHandlers( this, BTL_EVENT_SKIP_NIGERU_CALC );
      result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    EVENTVAR_Pop();
    return result;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * 逃げる処理コア
 *
 * @param   wk
 * @param   bpp
 * @param   fNigeruCmd
 * @param   fUseItem    バッグから逃げアイテムを使用した場合 true
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_NigeruCore( BTL_POKEPARAM* bpp, bool fForceNigeru, bool fUseItem )
{
  u8 escapeClientID = MainModule::PokeIDtoClientID( bpp->GetID() );

  // 野生戦は逃げる判定
  if( m_mainModule->GetEscapeMode() == BTL_ESCAPE_MODE_WILD )
  {
    u8 playerClientID = m_mainModule->GetPlayerClientID();
    u8 fEscapeEnemy = ( !m_mainModule->IsFriendClientID( escapeClientID, playerClientID) );

    // 強制的に逃がす場合はチェックを行わない
    if( !fForceNigeru )
    {
      // 逃げ禁止チェック
      if( !bpp->IsDead() )
      {
        u32 hem_state = m_HEManager.PushState();
        bool fForbid = scEvent_CheckNigeruForbid( bpp );
        m_HEManager.PopState( hem_state );
        if( fForbid ){
          return false;
        }
      }
    }

    // ここまで来たら逃げ確定
    if( !(m_fEscMsgDisped) )
    {
      bool fSpMsgDisped;

      if( !fUseItem ){
        // 特殊な逃げメッセージチェック
        u32 hem_state = m_HEManager.PushState();
          fSpMsgDisped = scEvent_NigeruExMessage( bpp );
        m_HEManager.PopState( hem_state );
      }else{
        fSpMsgDisped = false;
      }

      // 何もなければ標準メッセージ
      if( !fSpMsgDisped )
      {
        if( fEscapeEnemy )
        {
          // 本来なら野生戦以外はココにこないが、デバッグLR逃げだと来て見栄えが悪いので括る。
          if( m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD ){
            SCQUE_PUT_MSG_STD_SE( m_que, BTL_STRID_STD_EscapeWild, SEQ_SE_BT_NIGERU, bpp->GetID() );
          }
        }
        else
        {
          SCQUE_PUT_MSG_STD_SE( m_que, BTL_STRID_STD_EscapeSuccess, SEQ_SE_BT_NIGERU );
        }
      }

      m_fEscMsgDisped = true;
    }

  }
  // 通信対戦中は特殊メッセージ
  else if( m_mainModule->GetCommMode() != BTL_COMM_NONE )
  {
    if( !m_fEscMsgDisped )
    {
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Retired );
      m_fEscMsgDisped = true;
    }
  }

  // サブウェイ戦・通信対戦などは無条件に逃げる
  m_escInfo.Add( escapeClientID );
  return true;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 逃げる封じチェック処理
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool  逃げる封じが発動していたらtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckNigeruForbid( const BTL_POKEPARAM* bpp )
{
  bool result = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, result );
    BTL_SICKEVENT_CheckEscapeForbit( bpp );
    EVENT_CallHandlers( this, BTL_EVENT_NIGERU_FORBID );
    result = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 逃げることが成功したときに特殊メッセージを表示するか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool    特殊メッセージを表示するならtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_NigeruExMessage( const BTL_POKEPARAM* bpp )
{
  bool result = false;
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    EVENT_CallHandlers( this, BTL_EVENT_NIGERU_EXMSG );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();
  return result;
}

//----------------------------------------------------------------------------------
/**
 * メンバー入場コア
 *
 * @param   wk
 * @param   clientID      クライアントID
 * @param   posIdx        入場位置 index
 * @param   nextPokeIdx   入場させるポケモンの現パーティ内 index
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MemberInCore( u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
  BTL_PARTY* party;
  BTL_POKEPARAM* bpp;
  u8 pokeID;

  party = m_pokeCon->GetPartyData( clientID );

  {
    SCQUE_PUT_OP_SetIllusionForParty( m_que, clientID );
    m_mainModule->SetIllusionForParty( party, clientID );
  }

  if( posIdx != nextPokeIdx ){
    // クライアントへの通知は SCQUE_PUT_OP_MemberIn
    party->SwapMembers( posIdx, nextPokeIdx );
  }
  bpp = party->GetMemberData( posIdx );
  pokeID = bpp->GetID();
//  TAYA_Printf("MemberIn pokeID=%d\n", pokeID);

  m_mainModule->RegisterZukanSeeFlag( bpp );

  handler::tokusei::Add( bpp );
  handler::item::Add( bpp );
  handler::default_power_up::Add( bpp );
  bpp->SetAppearTurn( m_turnCount );
  bpp->Clear_ForIn();
  m_pokeInFlag[ pokeID ] = true;
  m_pokemonBattleInRegister.Register( pokeID );

  SCQUE_PUT_OP_MemberIn( m_que, clientID, posIdx, nextPokeIdx, m_turnCount );
  {
    BtlPokePos  pos = m_mainModule->GetClientPokePos( clientID, posIdx );
    m_posPoke.PokeIn( m_mainModule, pos, pokeID, m_pokeCon );
  }

  {
    const BtlWeather weather = scEvent_GetWeather();
    scproc_SetPokeDirt_ByWeather( bpp, weather );
  }

  scPut_PublishClientInformation_AppeardPokemon( *bpp );
}

/**
 * @brief ポケモンが場に出たことを他クライアントに公開する
 * @param appeardPoke  公開するポケモン
 *
 * @note 場に出たポケモンの特性が「イリュージョン」なら、イリュージョン対象のポケモンが場に出たことにします
 */
void ServerFlow::scPut_PublishClientInformation_AppeardPokemon( const BTL_POKEPARAM& appeardPoke )
{
  u8 publishPokeId = ( appeardPoke.IsFakeEnable() ) ? ( appeardPoke.GetFakeTargetPokeID() ) : ( appeardPoke.GetID() );
  SCQUE_PUT_OP_PublishClientInformation_AppearedPokemon( m_que, publishPokeId );
}


//----------------------------------------------------------------------------------
/**
 * @brief メンバー入れ替え
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx
 * @param   next_poke_idx
 * @param   fPutMsg
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MemberInForChange( u8 clientID, u8 posIdx, u8 next_poke_idx, bool fPutMsg )
{
  scproc_MemberInCore( clientID, posIdx, next_poke_idx );
  SCQUE_PUT_ACT_MemberIn( m_que, clientID, posIdx, next_poke_idx, fPutMsg );

  // 交代で出てきたフラグをセット
  BTL_POKEPARAM* appearPoke = m_pokeCon->GetClientPokeData( clientID, posIdx );
  scPut_SetTurnFlag( appearPoke, BTL_POKEPARAM::TURNFLG_APPEARED_BY_POKECHANGE );
}

//----------------------------------------------------------------------------------
/**
 * @brief 空き位置へのポケモン投入
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx
 * @param   next_poke_idx
 * @param   fPutMsg
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MemberInForCover( u8 clientID, u8 posIdx, u8 next_poke_idx, bool fPutMsg )
{
  scproc_MemberInCore( clientID, posIdx, next_poke_idx );
  SCQUE_PUT_ACT_MemberIn( m_que, clientID, posIdx, next_poke_idx, fPutMsg );
}


//----------------------------------------------------------------------------------
/**
 * サーバーフロー：ポケモン入場イベント処理
 *
 * @param   wk
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_AfterMemberIn( void )
{
  FRONT_POKE_SEEK_WORK fps;
  PokeSet* pokeSet;
  BTL_POKEPARAM* bpp;
  u32 hem_state;
  u8  fAnyEvent;

  fAnyEvent = false;

  pokeSet = &m_pokesetMemberInProc;
  pokeSet->Clear();
  FRONT_POKE_SEEK_InitWork( &fps );

  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    u8 pokeID = bpp->GetID();
    if( m_pokeInFlag[ pokeID ] )
    {
      pokeSet->Add( bpp );
      m_pokeInFlag[ pokeID ] = false;
    }
  }
  pokeSet->SortByAgility( this );

  // デフォルト能力アップ
  pokeSet->SeekStart();
  while( (bpp = pokeSet->SeekNext()) != NULL )
  {
    hem_state = m_HEManager.PushState();
      scEvent_AfterMemberIn( bpp, BTL_EVENT_MEMBER_IN_DEFAULT_POWERUP );
    m_HEManager.PopState( hem_state );
  }

  // バトンタッチよびだし
  pokeSet->SeekStart();
  while( (bpp = pokeSet->SeekNext()) != NULL )
  {
    hem_state = m_HEManager.PushState();
      scEvent_AfterMemberIn( bpp, BTL_EVENT_MEMBER_IN_BATONTOUCH );
    m_HEManager.PopState( hem_state );
  }


  hem_state = m_HEManager.PushState();
    scEvent_AfterMemberInPrev();
    if( scproc_HandEx_Result() != HandExResult_NULL )
    {
      fAnyEvent = true;
    }
  m_HEManager.PopState( hem_state );
  
  pokeSet->SeekStart();
  while( (bpp = pokeSet->SeekNext()) != NULL )
  {
    hem_state = m_HEManager.PushState();
      scEvent_AfterMemberIn( bpp, BTL_EVENT_MEMBER_IN );
      if( scproc_HandEx_Result() != HandExResult_NULL )
      {
        fAnyEvent = true;
      }
    m_HEManager.PopState( hem_state );
  }

  //@バトル変更箇所 カイグラ進化専用
  //SBTS[2532] 処理の順番をMEMBER_INの前から後に変更
  //根本の原因は技による退場イベントのの中からここが呼ばれると、btl_EventのEventStackPtrが2の状態でイベントが動く。
  //その場合EventStackPtrが2の時に登録されたfactorはEventStackPtrが2より大きくないと動かない。
  //通常で呼ばれた場合は1で登録されEventを抜けるときにクリアされるので問題なかった。
  //正しい処理としてはフォルムが変わった後に、特性変更を動かすこと。ただ、それはSBTS[102]の処理で消していた。
  //なのでその処理を復活させる。その場合元の位置だとカイグラを出すと大雨、大日照り、大雨、大日照りと2セットイベントが起こる(SBTS[102]の症状)
  //本来フォルムチェンジはBTL_EVENT_MEMBER_INの後にscEvent_AfterMemberInCompで行われていたのでその手前に持っていく。
  //で、元のカイグラの時におこる日照り、雨を所持道具とMonsNoでブロックする。
  pokeSet->SeekStart();
  while( (bpp = pokeSet->SeekNext()) != NULL )
  {
    hem_state = m_HEManager.PushState();
      scEvent_AfterMemberIn( bpp , BTL_EVENT_MEMBER_IN_EVO );
      if( scproc_HandEx_Result() != HandExResult_NULL )
      {
        fAnyEvent = true;
      }
    m_HEManager.PopState( hem_state );
  }

  hem_state = m_HEManager.PushState();
    scEvent_AfterMemberInComp();
    if( scproc_HandEx_Result() != HandExResult_NULL )
    {
      fAnyEvent = true;
    }
  m_HEManager.PopState( hem_state );

  return fAnyEvent;
}
//--------------------------------------------------------------------------
/**
 * [Event] メンバー入場イベント
 *
 * @param   wk
 * @param   bpp
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_AfterMemberIn( const BTL_POKEPARAM* bpp, BtlEventType eventID )
{
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventBegin, bpp->GetID() );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, eventID );
  EVENTVAR_Pop();
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventEnd, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Event] メンバー入場イベント
 *
 * @param   wk
 * @param   bpp
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_SkillCall( const BTL_POKEPARAM* bpp )
{
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventBegin, bpp->GetID() );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlersTargetType( this, BTL_EVENT_MEMBER_IN, BTL_EVENT_FACTOR_TOKUSEI );
  EVENTVAR_Pop();
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventEnd, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Event] メンバー入場イベント直前
 *
 * @param   wk
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_AfterMemberInPrev( void )
{
  EVENTVAR_Push();
    EVENT_CallHandlers( this, BTL_EVENT_MEMBER_IN_PREV );
  EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] メンバー入場イベント全員分終了
 *
 * @param   wk
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_AfterMemberInComp( void )
{
  EVENTVAR_Push();
    EVENT_CallHandlers( this, BTL_EVENT_MEMBER_IN_COMP );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Put] メンバー交替時の「○○ もどれ！」などのメッセージ表示コマンド
 *
 * @param   wk
 * @param   bpp   引っ込めるポケモンパラメータ
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_MemberOutMessage( BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );

  SCQUE_PUT_ACT_MemberOutMsg( m_que, clientID, pokeID );
}
//-----------------------------------------------------------------------------------
// サーバーフロー：メンバー交替
//-----------------------------------------------------------------------------------
void  ServerFlow::scproc_MemberChange( BTL_POKEPARAM* outPoke, u8 nextPokeIdx )
{
  u8 outPokeID = outPoke->GetID();
  u8 clientID  = m_mainModule->PokeIDtoClientID( outPokeID );
  BtlPokePos pos;

  if( m_posPoke.IsExist(outPokeID) )
  {
    scPut_MemberOutMessage( outPoke );
    if( !scproc_MemberOutForChange(outPoke, false) ){
      return;
    }
    pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, outPokeID );
  }
  else
  {
    pos = m_posPoke.GetPokeLastPos( outPokeID );
  }

  if( m_memberChangeCount[clientID] < MEMBER_CHANGE_COUNT_MAX ){
    m_memberChangeCount[clientID]++;
  }

  u8 posIdx = 0;
  m_mainModule->BtlPosToClientID_and_PosIdx( pos, &clientID, &posIdx );
  scproc_MemberInForChange( clientID, posIdx, nextPokeIdx, true );


}
//----------------------------------------------------------------------------------
/**
 * メンバーを場から退場させる（入れ替え処理前）
 *
 * @param   wk
 * @param   bpp           退場させるポケモンパラメータ
 * @param   fIntrDisable  割り込みアクション（おいうちなど）を禁止するフラグ
 *
 * @retval  bool    退場できたらtrue／割り込まれて死んだりしたらfalse
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_MemberOutForChange( BTL_POKEPARAM* outPoke, bool fIntrDisable )
{
  const u8 outPokeID = outPoke->GetID();

  // 割り込みチェック
  if( !fIntrDisable )
  {
    u32 intrPokeCount = scEvent_MemberChangeIntr( outPoke );
    if( intrPokeCount )
    {
      u32 i;
      m_fMemberOutIntr = true;
      for(i=0; i<intrPokeCount; ++i)
      {
        ActOrder_IntrProc( m_MemberOutIntrPokeID[i], outPokeID );
        if( outPoke->IsDead() ){
          break;
        }
      }
      m_fMemberOutIntr = false;
    }
  }

  // @fix NMCat[3210] ダブルバトルにて「ききかいひ」を含む特定の手順を踏むと技選択終了直後にとまる
  if( m_posPoke.IsExist( outPokeID ) &&
     !outPoke->IsDead() )
  {
    scproc_MemberOutCore( outPoke, BTLEFF_POKEMON_MODOSU );
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * メンバー退場（入れ替え・吹き飛ばし・死亡）共通処理
 *
 * @param   wk
 * @param   outPoke
 * @param   effectNo   エフェクトナンバー(0=エフェクトなし）
 *
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MemberOutCore( BTL_POKEPARAM* outPoke, u16 effectNo )
{
  u8 pokeID = outPoke->GetID();

  {
    BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
    if( pos != BTL_POS_NULL )
    {
      SCQUE_PUT_ACT_MemberOut( m_que, pos, effectNo );
    }
  }

  ActOrder_ForceDone( pokeID );

  {
    u32 hem_state = m_HEManager.PushState();
    scEvent_MemberOutFixed( outPoke );
    m_HEManager.PopState( hem_state );
  }

  scproc_ClearPokeDependEffect( outPoke );
  outPoke->Clear_ForOut();
  SCQUE_PUT_OP_OutClear( m_que, outPoke->GetID() );

  m_posPoke.PokeOut( pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Event] メンバー退場の確定
 *
 * @param   wk
 * @param   outPoke
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_MemberOutFixed( BTL_POKEPARAM* outPoke )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, outPoke->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_MEMBER_OUT_FIXED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ターン中に受けたアンコールで出す技を書き換える必要がある場合、書き換え後のワザナンバー取得
 *
 * @param   bpp
 * @param   action
 *
 * @retval  WazaID    書き換え後ワザナンバー／書き換え不要ならWAZANO_NULL
 */
//----------------------------------------------------------------------------------
btl::WazaID ServerFlow::checkEncoreWazaChange( const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* action, const ActionDesc* actionDesc )
{
  // Z技なら、アンコールの影響を受けない
  if( BTL_ACTION_IsFightWithZenryokuWaza( action ) )
  {
    return WAZANO_NULL;
  }

  // 「おどりこ」「さいはい」による追加アクションならアンコールの影響を受けない（結果として失敗させる）
  if( actionDesc->isOdorikoReaction || actionDesc->isSaihaiReaction ){
    return WAZANO_NULL;
  }

  if( (bpp->CheckSick( pml::wazadata::WAZASICK_ENCORE))
  &&  (BTL_ACTION_GetAction(action) == BTL_ACTION_FIGHT)
  ){
    WazaNo actWazaNo = BTL_ACTION_GetWazaID( action, bpp );
    if( actWazaNo != WAZANO_WARUAGAKI )
    {
      WazaNo encoreWaza = static_cast<WazaNo>(SICKCONT_GetParam( bpp->GetSickCont(pml::wazadata::WAZASICK_ENCORE) ));
      if( (encoreWaza != actWazaNo )
      &&  (bpp->WAZA_GetPP_ByNumber(encoreWaza))
      ){
        return encoreWaza;
      }
    }
  }
  return WAZANO_NULL;
}

/**
 * @brief 出そうとしたワザをアンコール効果により強制的に差し替えられた時のイベント処理
 */
void ServerFlow::scEvent_ReplaceActWaza( const BTL_POKEPARAM* attacker, WazaID originWaza, WazaID nextWaza )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ORG_WAZAID, originWaza );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, nextWaza );
    EVENT_CallHandlers( this, BTL_EVENT_REPLACE_ACT_WAZA );
  EVENTVAR_Pop();
}


//-----------------------------------------------------------------------------------
// サーバーフロー：メガ進化
//-----------------------------------------------------------------------------------
void  ServerFlow::scproc_MegaEvo( BTL_POKEPARAM* poke )
{
  //@バトル変更箇所 (下記のコメントのみ
  //ここに近い処理がscproc_HandEx_changeFormのゲンシカイキに入っています
  
  u8 pokeID = poke->GetID();
  TokuseiNo  prevTok = (TokuseiNo)(poke->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

  //@バトル変更箇所
  //SBTS[1917] メガシンカの時の特殊天候取り消し
  scEvent_ChangePokeBefore(poke);
  
  handler::tokusei::Remove( poke );
  // ヒカリドラゴン進化
  if (poke->GetMonsNo() == MONSNO_PURIZUMU &&
      (poke->GetFormNo() == FORMNO_PURIZUMU_FORM_S || poke->GetFormNo() == FORMNO_PURIZUMU_FORM_L))
  {
    poke->UltraBurst();
  }
  else
  {
    poke->MegaEvo();
  }
  
  handler::tokusei::Add( poke );

  SCQUE_PUT_ACTOP_MegaEvo( m_que, pokeID );

  TokuseiNo  nextTok = (TokuseiNo)(poke->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

  // メガゲンガーは「テレキネシス」を落とす
  if( poke->GetMonsNo() == MONSNO_GENGAA )
  {
    if( poke->CheckSick( pml::wazadata::WAZASICK_TELEKINESIS ) ){
      scPut_CureSick( poke, pml::wazadata::WAZASICK_TELEKINESIS, NULL );
    }
  }


  scproc_AfterTokuseiChanged_Event( poke );
  scproc_AfterTokuseiChanged_Item( poke, prevTok, nextTok );

  m_mainModule->RegisterZukanSeeFlag( poke );

}
//-----------------------------------------------------------------------------------
/**
 * @brief サーバーフロー：「たたかう」ルート
 * @param actionKind  ポケモンの行動の種類
 */
//-----------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight( BTL_POKEPARAM* attacker, BTL_ACTION_PARAM* action, ActionDesc* actionDesc, u32 handlerSubPri, int wazaPri )
{
  REQWAZA_WORK  reqWaza;
  WazaID  orgWaza, actWaza, zenryokuWazaBase, zenryokuWaza;
  BtlPokePos  orgTargetPos, actTargetPos;
  BTL_POKEPARAM::ContFlag  tameFlag;
  u8 fWazaEnable, fWazaLock, fTameLock, fReqWaza, fPPDecrement, orgWazaIdx, fActZenryokuWaza;
  ZenryokuWazaKind zenryokuWazaKind;

  m_fWazaFailMsgDisped = false;
  m_fWazaFailMsgRoundUp = false;

  tameFlag = attacker->CONTFLAG_CheckWazaHide();

  wazaEffCtrl_Init( m_wazaEffCtrl );
  wazaRobParam_Init( m_magicCoatParam );

  reqWaza.wazaID = WAZANO_NULL;
  reqWaza.targetPos = BTL_POS_NULL;

  /* アンコール状態で違うワザを選んでいた場合は強制書き換え */
  {
    WazaID encoreWaza = checkEncoreWazaChange( attacker, action, actionDesc );
    if( encoreWaza != WAZANO_NULL )
    {
      WazaID  originWaza = BTL_ACTION_GetWazaID( action, attacker );
      scEvent_ReplaceActWaza( attacker, originWaza, encoreWaza );
      BtlPokePos targetPos = calc::DecideWazaTargetAuto( m_mainModule, m_pokeCon, attacker, encoreWaza );
      BTL_ACTION_SetFightParam( action, encoreWaza, targetPos );
    }
  }


  orgWaza = BTL_ACTION_GetOriginalWazaID( action );
  orgWazaIdx = attacker->WAZA_SearchIdx( BTL_ACTION_GetOriginalWazaID( action ) );
  actWaza = orgWaza;
  orgTargetPos = action->fight.targetPos;
  actTargetPos = orgTargetPos;
  fWazaEnable = false;
  fPPDecrement = false;
  fWazaLock = attacker->CheckSick( pml::wazadata::WAZASICK_WAZALOCK );
  fTameLock = attacker->CheckSick( pml::wazadata::WAZASICK_TAMELOCK );
  fActZenryokuWaza = BTL_ACTION_IsFightWithZenryokuWaza( action );
  zenryokuWazaBase = WAZANO_NULL;
  zenryokuWaza = WAZANO_NULL;
  zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_NULL;
  BTL_PRINT("Fight pokeID=%d, waza=%d, targetPos=%d, handlerPri=%08x\n", attacker->GetID(), orgWaza, orgTargetPos, handlerSubPri);


  handler::waza::Add( attacker, actWaza, handlerSubPri );

  do {

    // 全力技による技変更( 派生技を考慮しない )
    // 
    // PPが足りない場合など、技失敗時に技名をメッセージで表示するが、( 例.「ピカチュウの１０まんボルト！」「しかしPPがなかった！」 )
    // 全力技を撃とうとして元技のPPが足りない場合、( 元技ではなく )全力技の名前を表示する必要があります。
    // そのため、この時点で全力技が何なのか？を "一旦" 決定します。
    // なお、
    // 派生技を全力技として使用した場合、派生後の技を全力技として撃つ必要があるため、
    // 派生技の解決後、再度、全力技が何なのか？を再計算します。
    if( fActZenryokuWaza ) 
    {
      zenryokuWazaBase  = actWaza;
      scEvent_GetZenryokuWaza( &zenryokuWaza, &zenryokuWazaKind, attacker, zenryokuWazaBase );
      if( zenryokuWaza != WAZANO_NULL ) 
      {
        handler::waza::Remove( attacker, actWaza );

        actWaza = zenryokuWaza;

        handler::waza::Add( attacker, actWaza, handlerSubPri );
      }
    }

    // ＠momijiバトル修正箇所
    scproc_StartWazaSeq(attacker, actWaza);

    // 「全力ワザを使うことを選択した」のに「全力ワザは使えない状態」の場合、ワザ出し失敗とする
    if( fActZenryokuWaza && (zenryokuWaza == WAZANO_NULL) )
    {
      scPut_WazaExecuteFailMsg( attacker, actWaza, false, SV_WAZAFAIL_UNEXPECTED_ERROR );
      break;
    }

    // ワザ出し失敗判定１（ポケモン系状態異常＆こんらん、メロメロ、ひるみ）
    {
      scEvent_GetWazaParam( actWaza, attacker, m_wazaParam );
      m_wazaParam->wazaPri = wazaPri;
      m_wazaParam->orgWazaID = orgWaza;
      m_wazaParam->zenryokuWazaKind = zenryokuWazaKind;

      if( scproc_Fight_CheckWazaExecuteFail_1st( attacker, m_wazaParam, fWazaLock||fTameLock, NULL, actionDesc ) )
      {
        break; 
      }
    }


    // 全力技開始演出
    if( fActZenryokuWaza ) 
    {
      ZenryokuEffectMessage effectMessage;
      scproc_FakeBreak( attacker, NULL );                                                                 // イリュージョンを解除する
      scproc_ZenryokuEffect( attacker, orgWaza, &effectMessage );                                         // 全力技効果
      SCQUE_PUT_OP_SetZenryokuWazaUsed( m_que, attacker->GetID() );                                       // 全力技を使用済みにする
      SCQUE_PUT_ACT_ZenryokuWazaBegin( m_que, attacker->GetID(), actWaza, actTargetPos, effectMessage );  // 全力技開始演出
      scproc_ZenryokuWazaStartMessage( *attacker, zenryokuWazaKind );                                     // 全力技開始メッセージ
    }


    // 派生ワザ呼び出しパラメータチェック＆失敗判定
    if( !scEvent_GetReqWazaParam(attacker, orgWaza, orgTargetPos, &reqWaza) )
    {
      scPut_WazaMsg( attacker, orgWaza, fActZenryokuWaza );
      scEvent_GetWazaParam( orgWaza, attacker, m_wazaParamOrg );

      m_flowSub.RegisterTargets( this, attacker, orgTargetPos, m_wazaParamOrg, m_psetTargetOrg );

      if( orgWazaIdx != PTL_WAZA_MAX )
      {
        scproc_decrementPPUsedWaza( attacker, orgWaza, orgWazaIdx, m_psetTargetOrg );
        fPPDecrement = true;
      }

      scproc_WazaExecuteFailed( attacker, orgWaza, false, SV_WAZAFAIL_OTHER );
      break;
    }

    // 派生技による技変更
    fReqWaza = ( reqWaza.wazaID != WAZANO_NULL );
    if( fReqWaza )
    {
      if( fActZenryokuWaza )
      {
        handler::waza::Remove( attacker, actWaza );
      }

      actWaza = reqWaza.wazaID;
      actTargetPos = reqWaza.targetPos;

      handler::waza::Add( attacker, actWaza, handlerSubPri );
    }

    // 全力技による技変更
    // 派生技を全力技として使用した場合、派生後の技を全力技に変更する必要があるため、ここで再計算する
    if( fReqWaza && fActZenryokuWaza ) 
    {
      zenryokuWazaBase  = actWaza;
      scproc_DecideZenryokuWazaByReqWaza( &zenryokuWaza, &zenryokuWazaKind, attacker, zenryokuWazaBase );
      if( zenryokuWaza != WAZANO_NULL ) 
      {
        handler::waza::Remove( attacker, actWaza );

        actWaza = zenryokuWaza;

        // @fix GFNMCat[5791] ダブルバトルで「Ｚねごと」を使用するとアサート
        if((zenryokuWazaKind == pml::waza::ZENRYOKUWAZA_KIND_GENERAL) &&
           (actTargetPos == BTL_POS_NULL))
        {
          actTargetPos = calc::DecideWazaTargetAuto(m_mainModule, m_pokeCon, attacker, actWaza);
        }

        handler::waza::Add( attacker, actWaza, handlerSubPri );
      }
    }

    // 派生技の技演出
    if( fReqWaza )
    {
      if( fActZenryokuWaza )
      {
        handler::waza::Add( attacker, orgWaza, handlerSubPri );   // 派生元になった技のハンドラが必要
        scproc_ZenryokuReqWazaMessage( attacker, orgWaza, zenryokuWazaBase, actWaza );
        handler::waza::Remove( attacker, orgWaza );
      }
      else
      {
        BtlPokePos reqTargetPos = convertReqWazaTargetPos( orgWaza, orgTargetPos );
        scPut_WazaMsg( attacker, orgWaza, false );
        scPut_ReqWazaEffect( attacker, orgWaza, reqTargetPos, zenryokuWazaKind );
      }
    }


    // ワザパラメータ確定
    scEvent_GetWazaParam( orgWaza, attacker, m_wazaParamOrg );
    scEvent_GetWazaParam( actWaza, attacker, m_wazaParam );
    if( fReqWaza )
    {
      m_wazaParam->fReqWaza = true;
      m_wazaParam->orgWazaID = orgWaza;
    }
    if( fActZenryokuWaza )
    {
      m_wazaParam->zenryokuWazaKind = zenryokuWazaKind;
      if( zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_SPECIAL )
      {
        m_wazaParam->wazaPower  = WAZADATA_GetParam( zenryokuWazaBase, pml::wazadata::PARAM_ID_ZENRYOKU_POWER );
        m_wazaParam->damageType = WAZADATA_GetDamageType( zenryokuWazaBase );
      }
    }
    m_wazaParam->wazaPri = wazaPri;


    // 技パラメータ確定後イベント
    {
      bool isAddOrgWazaHandler = false;

      if( orgWaza != actWaza )
      {
        isAddOrgWazaHandler = handler::waza::Add( attacker, orgWaza, handlerSubPri );  // 派生元になった技のハンドラが必要
      }

      scEvent_AfterWazaParamFixed( attacker, orgWaza, actWaza, fActZenryokuWaza );

      if( isAddOrgWazaHandler )
      {
        handler::waza::Remove( attacker, orgWaza );
      }
    }


    // 派生された技の失敗判定
    if( fReqWaza )
    {
      SV_WazaFailCause failCause = scproc_Fight_CheckReqWazaFail( attacker, m_wazaParam );
      if( failCause != SV_WAZAFAIL_NULL )
      {
        scproc_WazaExecuteFailed( attacker, m_wazaParam->wazaID, fActZenryokuWaza, failCause );

        if( orgWazaIdx != PTL_WAZA_MAX )
        {
          scproc_decrementPPUsedWaza( attacker, orgWaza, orgWazaIdx, m_psetTargetOrg );
          fPPDecrement = true;
        }

        break;
      }
    }


    // ワザ対象をワークに取得
    m_psetTargetOrg->Clear();
    m_psetTarget->Clear();
    BTL_PRINT("[scproc_fight] atkPokeID=%d, actTargetPos=%d\n", attacker->GetID(), actTargetPos);
    m_flowSub.RegisterTargets( this, attacker, actTargetPos, m_wazaParam, m_psetTargetOrg );
    {
      u8 defTargetCnt = m_psetTargetOrg->GetCountMax();
      m_psetTargetOrg->RemoveDisablePoke( m_posPoke );
      m_psetTargetOrg->CopyAlive( m_psetTarget );
      m_psetTarget->SetDefaultTargetCount( defTargetCnt );
    }

    // 使ったワザのPP減らす
    // ( 前ターンからロックされている場合は減らさない )
    // (「おどりこ」によって発動した技なら減らさない )
    if( (!(fWazaLock || fTameLock) ) &&
        (!actionDesc->isOdorikoReaction) )
    {
      if( orgWazaIdx != PTL_WAZA_MAX )
      {
        scproc_decrementPPUsedWaza( attacker, orgWaza, orgWazaIdx, m_psetTargetOrg );
        fPPDecrement = true;
      }
    }

    // ワザメッセージ出力確定
    scproc_WazaCall_Decide( attacker, m_wazaParamOrg, m_wazaParam );

    // 合体ワザ（後発）の発動チェック
    scEvent_CheckCombiWazaExe( attacker, m_wazaParam );

    // ワザメッセージ出力
    scproc_WazaMsg( attacker, orgWaza, actWaza, m_wazaParam->zenryokuWazaKind, m_psetTarget );

    // ワザ出し失敗判定（ワザメッセージ後・ふんじん前）
    if( scproc_Fight_CheckWazaExecuteFail_2nd( attacker, actWaza , m_wazaParam, m_psetTarget ) ) 
    {
       break;
    }

    // ワザ出し失敗判定（ふんじん）
    if( scproc_Fight_CheckWazaExecuteFail_Funjin( attacker, m_wazaParam )){
       break;
    }


    // ここまで来たらPP減少は確定
    m_prevExeWaza = actWaza;
    m_isPrevExeZenryokuWaza = fActZenryokuWaza;

    // レコード( 技を使用した回数 )をインクリメント
    RecordSave::IncWazaUseCount( *m_mainModule, *attacker, m_wazaParam->zenryokuWazaKind );

    // 命中率上昇チェック
    if( attacker->CheckSick( pml::wazadata::WAZASICK_HITRATIO_UP) )
    {
      scPut_CureSick( attacker, pml::wazadata::WAZASICK_HITRATIO_UP, NULL );
      attacker->TURNFLAG_Set(  BTL_POKEPARAM::TURNFLG_HITRATIO_UP );
    }

    // ワザ出し失敗判定３（ＰＰ減少後）
    if( scproc_Fight_CheckWazaExecuteFail_3rd( attacker, actWaza , m_wazaParam, m_psetTarget )){
       break;
    }

    // 時間差発動ワザの準備処理
    if( scproc_Fight_CheckDelayWazaSet( attacker, actWaza, actTargetPos, &fWazaEnable, m_wazaParam->zenryokuWazaKind ) ){
      break;
    }

    // 合体ワザ（先発）の準備処理
    if( scproc_Fight_CheckCombiWazaReady( attacker, actWaza, actTargetPos) ){ break; }

    // ワザ出し記録
    m_wazaRec.Add( actWaza, m_turnCount, attacker->GetID() );
    scproc_WazaExe_Decide( attacker, m_wazaParam, BTL_EVENT_WAZA_EXE_DECIDE );

    // ワザ乗っ取り判定
    if( scproc_Check_WazaRob( attacker, actWaza, fActZenryokuWaza, m_psetTarget, m_wazaRobParam) )
    {
      BTL_N_Printf( DBGSTR_SVFL_YokodoriDetermine, attacker->GetID(), actWaza );
      scproc_WazaRobRoot( attacker, actWaza, fActZenryokuWaza );
    }
    // 乗っ取られず通常処理
    else
    {
      fWazaEnable = scproc_Fight_WazaExe( attacker, m_wazaParam, m_psetTarget, actionDesc );
    }

  }ONCE;

  if( !fWazaEnable ){
    m_actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_MASTER );
  }

  // ワザプロセス修了した
  attacker->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_WAZAPROC_DONE );

  // PP消費したらワザ使用記録を更新
  if( fPPDecrement || (fWazaLock && attacker->GetWazaLockID()==actWaza) || (actWaza == WAZANO_WARUAGAKI) )
  {
    attacker->UpdateWazaProcResult( actTargetPos, m_wazaParam->wazaType, fWazaEnable, fActZenryokuWaza, actWaza, orgWaza );
    SCQUE_PUT_OP_UpdateWazaProcResult( m_que, attacker->GetID(), actTargetPos, m_wazaParam->wazaType, fWazaEnable, fActZenryokuWaza, actWaza, orgWaza );
  }
  // 使用記録を更新しない場合、「まもる」カウンタをリセットする
  else
  {
    // @fix NMCat[3272] 特性「おどりこ」が発動すると「まもる」「みきり」が連続して成功する事について
    //「おどりこ」で出た技では「まもる」カウンタをリセットしないようにする
    if( attacker->COUNTER_Get(BTL_POKEPARAM::COUNTER_MAMORU) && 
       !actionDesc->isOdorikoReaction )
    {
      scPut_SetBppCounter( attacker, BTL_POKEPARAM::COUNTER_MAMORU, 0 );
    }
  }

  // 溜めワザ解放ターンの失敗に対処
  if( (tameFlag != BTL_POKEPARAM::CONTFLG_NULL) && (attacker->CONTFLAG_CheckWazaHide() != BTL_POKEPARAM::CONTFLG_NULL) )
  {
    scproc_TameLockClear( attacker );
  }
  if( attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_TAMEHIDE_OFF ) )
  {
    SCQUE_PUT_ACT_TameWazaHide( m_que, attacker->GetID(), false );
    scproc_FreeFall_CheckRelease( attacker, true, true );
  }

  scproc_EndWazaSeq( attacker, actWaza, fWazaEnable, actionDesc );

  handler::waza::Remove( attacker, orgWaza );
  handler::waza::Remove( attacker, actWaza );

  scproc_MagicCoat_Root( actWaza );

  // 全力技終了演出
  if( fActZenryokuWaza ) 
  {
    SCQUE_PUT_ACT_ZenryokuWazaEnd( m_que, attacker->GetID() );
  }
}
/**
 * 他ワザ呼び出しワザのエフェクト処理のためターゲット指定を適切なものに書き換える
 */
BtlPokePos ServerFlow::convertReqWazaTargetPos( WazaID orgWaza, BtlPokePos defaultTargetPos ) const
{
  if( defaultTargetPos == BTL_POS_NULL )
  {
    // シングル・ローテの場合のみ、ユーザーが対象を指定するシーケンスが無いためここにきます
    if( WAZADATA_GetParam(orgWaza, pml::wazadata::PARAM_ID_TARGET) == pml::wazadata::TARGET_ENEMY_SELECT )
    {
      // 効果範囲が「相手側１体指定」の場合のみ、正しい位置を通知します。
      return m_mainModule->GetOpponentPokePos( defaultTargetPos, 0 );
    }
  }
  return defaultTargetPos;
}

/**
 * @brief 全力効果処理
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      originalWazaID  全力技の元になった技
 * @param[out] effectMessage   全力効果発動時に表示するメッセージ
 */
void ServerFlow::scproc_ZenryokuEffect( BTL_POKEPARAM* zenryokuUsePoke, WazaID originalWazaID, ZenryokuEffectMessage* effectMessage )
{
  ZenryokuWazaEffect zenryokuEffect   = pml::wazadata::ZENRYOKUWAZA_EFFECT_NONE;
  WazaRankEffect     rankEffect   = pml::wazadata::RANKEFF_NULL;
  u8                 rankUpVolume = 0;
  scEvent_GetZenryokuEffect( &zenryokuEffect, &rankEffect, &rankUpVolume, zenryokuUsePoke, originalWazaID );

  // 能力ランク上昇
  if( scproc_ZenryokuEffect_RankUp( zenryokuUsePoke, originalWazaID, rankEffect, rankUpVolume, effectMessage ) ) {
    return;
  }

  // 能力ランクダウンをリセット
  if( zenryokuEffect == pml::wazadata::ZENRYOKUWAZA_EFFECT_RESET_RANK_DOWN )
  {
    if( scproc_ZenryokuEffect_RankRecover( zenryokuUsePoke, effectMessage ) ) {
      return;
    }
  }

  // 自分のHP全回復
  if( zenryokuEffect == pml::wazadata::ZENRYOKUWAZA_EFFECT_RECOVER_HP ) 
  {
    if( scproc_ZenryokuEffect_HpRecover( zenryokuUsePoke, effectMessage ) ) {
      return;
    }
  }

  // 次に場に出たポケモンのHPを全回復
  if( zenryokuEffect == pml::wazadata::ZENRYOKUWAZA_EFFECT_RECOVER_HP_POS ) 
  {
    if( scproc_ZenryokuEffect_HpRecover_Pos( zenryokuUsePoke, effectMessage ) ) {
      return;
    } 
  }

  // 全力効果「このゆびとまれ」
  if( zenryokuEffect == pml::wazadata::ZENRYOKUWAZA_EFFECT_KONOYUBITOMARE )
  {
    if( scproc_ZenryokuEffect_Konoyubitomare( zenryokuUsePoke, effectMessage ) ) {
      return;
    }
  }

  // 全力効果が無いなら、メッセージ「すごいオーラを　からだに　まとった！」
  effectMessage->strId  = BTL_STRID_SET_AuraEffect_None;
  effectMessage->param1 = zenryokuUsePoke->GetID();
}

/**
 * @brief 全力効果を取得する
 * @param[out] zenryokuEffect  取得した全力効果の格納先
 * @param[out] rankEffect      全力効果に対応する能力ランク( 指定した全力効果に能力ランク上昇効果が無い場合、pml::wazadata::RANKEFF_NULL )
 * @param[out] rankUpVolume    全力効果に対応する能力ランクの上昇量
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      originalWazaID  全力技の元になった技
 */
void ServerFlow::scEvent_GetZenryokuEffect(
  ZenryokuWazaEffect*  zenryokuEffect, 
  WazaRankEffect*      rankEffect, 
  u8*                  rankUpVolume, 
  const BTL_POKEPARAM* zenryokuUsePoke, 
  WazaID               originalWazaID )
{
  *zenryokuEffect = WAZADATA_GetZenryokuWazaEffect( originalWazaID );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, zenryokuUsePoke->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_ZENRYOKU_EFFECT, *zenryokuEffect );
    EVENT_CallHandlers( this, BTL_EVENT_GET_ZENRYOKU_EFFECT );
    *zenryokuEffect = static_cast<ZenryokuWazaEffect>( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_EFFECT ) );
  EVENTVAR_Pop();

  getRankEffectByZenryokuEffect( *zenryokuEffect, rankEffect, rankUpVolume );
}

/**
 * @brief 全力効果から、対応する能力ランク上昇効果を取得する
 * @param[in]  zenryokuEffect    全力効果
 * @param[out] rankEffect    全力効果に対応する能力ランク( 指定した全力効果に能力ランク上昇効果が無い場合、pml::wazadata::RANKEFF_NULL )
 * @param[out] rankUpVolume  全力効果に対応する能力ランクの上昇量
 */
void ServerFlow::getRankEffectByZenryokuEffect( ZenryokuWazaEffect zenryokuEffect, WazaRankEffect* rankEffect, u8* rankUpVolume ) const
{
  static const struct 
  {
    ZenryokuWazaEffect     zenryokuEffect;
    WazaRankEffect rankEffect;
    u8             rankUpVolume;
  }
  RANKUP_DESC[] = 
  {
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP1,         pml::wazadata::RANKEFF_ATTACK,         1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP2,         pml::wazadata::RANKEFF_ATTACK,         2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_ATTACK_UP3,         pml::wazadata::RANKEFF_ATTACK,         3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP1,        pml::wazadata::RANKEFF_DEFENCE,        1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP2,        pml::wazadata::RANKEFF_DEFENCE,        2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_DEFENSE_UP3,        pml::wazadata::RANKEFF_DEFENCE,        3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP1,      pml::wazadata::RANKEFF_SP_ATTACK,      1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP2,      pml::wazadata::RANKEFF_SP_ATTACK,      2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_ATTACK_UP3,      pml::wazadata::RANKEFF_SP_ATTACK,      3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP1,     pml::wazadata::RANKEFF_SP_DEFENCE,     1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP2,     pml::wazadata::RANKEFF_SP_DEFENCE,     2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_DEFENSE_UP3,     pml::wazadata::RANKEFF_SP_DEFENCE,     3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP1,     pml::wazadata::RANKEFF_AGILITY,        1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP2,     pml::wazadata::RANKEFF_AGILITY,        2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AGILITY_UP3,     pml::wazadata::RANKEFF_AGILITY,        3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP1,         pml::wazadata::RANKEFF_HIT,            1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP2,         pml::wazadata::RANKEFF_HIT,            2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_HIT_UP3,         pml::wazadata::RANKEFF_HIT,            3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP1,       pml::wazadata::RANKEFF_AVOID,          1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP2,       pml::wazadata::RANKEFF_AVOID,          2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_SP_AVOID_UP3,       pml::wazadata::RANKEFF_AVOID,          3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP1,    pml::wazadata::RANKEFF_MULTI5,         1 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP2,    pml::wazadata::RANKEFF_MULTI5,         2 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_MULTI5_RANK_UP3,    pml::wazadata::RANKEFF_MULTI5,         3 },
    { pml::wazadata::ZENRYOKUWAZA_EFFECT_CRITICAL_RATIO_UP1, pml::wazadata::RANKEFF_CRITICAL_RATIO, 1 },
  };

  *rankEffect   = pml::wazadata::RANKEFF_NULL;
  *rankUpVolume = 0;

  for( u32 i=0; i<GFL_NELEMS(RANKUP_DESC); ++i )
  {
    if( RANKUP_DESC[i].zenryokuEffect == zenryokuEffect ) {
      *rankEffect   = RANKUP_DESC[i].rankEffect;
      *rankUpVolume = RANKUP_DESC[i].rankUpVolume;
      return;
    }
  }
}

/**
 * @brief 全力効果処理( 能力ランク上昇 )
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      originalWazaID  全力技の元になった技
 * @param      rankEffect      全力効果に対応する能力ランク
 * @param      rankUpVolume    全力効果に対応する能力ランクの上昇量
 * @param[out] effectMessage   全力効果発動時に表示するメッセージ
 * @retval true   能力ランク上昇効果が適用された
 * @retval false  能力ランク上昇効果が適用されなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_RankUp( 
  BTL_POKEPARAM*         zenryokuUsePoke,
  WazaID                 originalWazaID, 
  WazaRankEffect         rankEffect, 
  u8                     rankUpVolume,
  ZenryokuEffectMessage* effectMessage )
{
  // 急所率
  if( rankEffect == pml::wazadata::RANKEFF_CRITICAL_RATIO ) {
    return scproc_ZenryokuEffect_RankUp_Critical( zenryokuUsePoke, rankUpVolume, effectMessage );
  }

  // 攻撃・防御・特防・特攻・素早さ
  if( rankEffect == pml::wazadata::RANKEFF_MULTI5 ) {
    return scproc_ZenryokuEffect_RankUp_Multi5( zenryokuUsePoke, rankUpVolume, effectMessage );
  }

  // 攻撃・防御・特防・特攻・素早さ・命中・回避　のどれか１つ
  if( ( pml::wazadata::RANKEFF_ORIGIN <= rankEffect ) &&
      ( rankEffect < pml::wazadata::RANKEFF_MAX ) ) {
    return scproc_ZenryokuEffect_RankUp_Base( zenryokuUsePoke, rankEffect, rankUpVolume, effectMessage );
  }

  return false;
}

/**
 * @brief 全力効果処理( 急所ランク上昇 )
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      rankUpVolume    急所ランク上昇量
 * @param[out] effectMessage   全力効果発動時に表示するメッセージ
 * @retval true   急所ランクが上昇した
 * @retval false  急所ランクが上昇しなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_RankUp_Critical( BTL_POKEPARAM* zenryokuUsePoke, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage )
{
  if( !zenryokuUsePoke->CONTFLAG_Get( BTL_POKEPARAM::CONTFLG_KIAIDAME ) )
  {
    scPut_SetContFlag( zenryokuUsePoke, BTL_POKEPARAM::CONTFLG_KIAIDAME );
    effectMessage->strId  = BTL_STRID_SET_AuraEffect_Rankup_Critical;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    return true;
  }
  return false;
}

/**
 * @brief 全力効果処理( 攻撃・防御・特防・特攻・素早さ　ランク上昇 )
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      rankUpVolume    ランク上昇量
 * @param[out] effectMessage   全力効果発動時に表示するメッセージ
 * @retval true   いずれかのランクが上昇した
 * @retval false  どのランクも上昇しなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_RankUp_Multi5( BTL_POKEPARAM* zenryokuUsePoke, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage )
{
  bool isEffective = false;
  for( u32 rankEffect=pml::wazadata::RANKEFF_ORIGIN; rankEffect<pml::wazadata::RANKEFF_BASE_MAX; ++rankEffect ) 
  {
    if( zenryokuUsePoke->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>( rankEffect ), rankUpVolume ) )
    {
      isEffective = true;
      scPut_RankEffect( zenryokuUsePoke, static_cast<WazaRankEffect>( rankEffect ), rankUpVolume, ITEM_DUMMY_DATA, false, false );
    }
  }

  if( isEffective )
  {
    effectMessage->strId  = BTL_STRID_SET_AuraEffect_Rankup_Multi5;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    effectMessage->param2 = rankUpVolume;
  }

  return isEffective;
}

/**
 * @brief 全力効果処理( 攻撃・防御・特防・特攻・素早さ・命中・回避　のどれか１つ　ランク上昇 )
 * @param      zenryokuUsePoke 全力技を発動するポケモン
 * @param      rankEffect      上昇させる能力ランク
 * @param      rankUpVolume    ランク上昇量
 * @param[out] effectMessage   全力効果発動時に表示するメッセージ
 * @retval true   ランクが上昇した
 * @retval false  ランクが上昇しなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_RankUp_Base( BTL_POKEPARAM* zenryokuUsePoke, WazaRankEffect rankEffect, u8 rankUpVolume, ZenryokuEffectMessage* effectMessage )
{
  if( zenryokuUsePoke->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>( rankEffect ), rankUpVolume ) ) 
  {
    scPut_RankEffect( zenryokuUsePoke, rankEffect, rankUpVolume, ITEM_DUMMY_DATA, false, false );

    effectMessage->strId = BTL_STRID_SET_AuraEffect_Rankup_ATK;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    effectMessage->param2 = rankEffect;
    effectMessage->param3 = rankUpVolume;
    return true;
  }
  return false;
}

/**
 * @brief 全力効果処理( 能力ランクダウンをリセット )
 * @param      zenryokuUsePoke  全力技を発動するポケモン
 * @param[out] effectMessage    全力効果発動時に表示するメッセージ
 * @retval true   全力効果が有効だった
 * @retval false  全力効果が有効でなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_RankRecover( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage )
{
  if( zenryokuUsePoke->RankRecover() )
  {
    SCQUE_PUT_OP_RankRecover( m_que, zenryokuUsePoke->GetID() );

    effectMessage->strId = BTL_STRID_SET_AuraEffect_RankRecover;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    return true;
  }
  return false;
}

/**
 * @brief 全力効果処理( 自分のHP全回復 )
 * @param      zenryokuUsePoke  全力技を発動するポケモン
 * @param[out] effectMessage    全力効果発動時に表示するメッセージ
 * @retval true   全力効果が有効だった
 * @retval false  全力効果が有効でなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_HpRecover( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage )
{
  if( scproc_RecoverHP_CheckFailBase( zenryokuUsePoke ) )
  {
    return false;
  }

  u32 maxHP = zenryokuUsePoke->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
  u32 currentHP = zenryokuUsePoke->GetValue( BTL_POKEPARAM::BPP_HP );
  u32 recoverHP = maxHP - currentHP;
  static const bool skipSpFailCheck = true;
  if( scproc_RecoverHP( zenryokuUsePoke, recoverHP, skipSpFailCheck, false, false ) ) 
  {
    effectMessage->strId = BTL_STRID_SET_AuraEffect_HPRecover;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    return true;
  }
  return false;
}

/**
 * @brief 全力効果処理( 次に場に出た味方のHP全回復 )
 * @param      zenryokuUsePoke  全力技を発動するポケモン
 * @param[out] effectMessage    全力効果発動時に表示するメッセージ
 * @retval true   全力効果が有効だった
 * @retval false  全力効果が有効でなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_HpRecover_Pos( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage )
{
  u8         pokeId  = zenryokuUsePoke->GetID();
  BtlPokePos pokePos = Hnd_PokeIDtoPokePos( pokeId );

  PosEffect::EffectParam effectParam;
  // @fix NMCat[4277] インターネット通信での対戦において「Zおきみやげ」「Zすてゼリフ」を使用した際に通信切断が発生する。
  effectParam.Raw.param1 = 0;
  if( scproc_AddPosEffect( pokePos, BTL_POSEFF_ZENRYOKU_EFFECT_REPAIR_HP, effectParam, pokeId, NULL, 0 ) )
  {
    effectMessage->strId = BTL_STRID_SET_AuraEffect_FriendHPRecover;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    return true;
  }
  return false;
}

/**
 * @brief 位置エフェクトを追加する
 * @param effectPos      追加する場所
 * @param posEffect      追加する位置エフェクト
 * @param effectParam    追加する位置エフェクトのパラメータ
 * @param dependPokeId   追加する位置エフェクトの、イベントファクタの DependID
 * @param factorParam    追加する位置エフェクトの、イベントファクタのパラメータ配列
 * @param factorParamNum 追加する位置エフェクトの、イベントファクタのパラメータ数
 * @retval true   位置エフェクトを追加した
 * @retval false  位置エフェクトを追加できなかった
 */
bool ServerFlow::scproc_AddPosEffect( 
  BtlPokePos                    effectPos, 
  BtlPosEffect                  posEffect, 
  const PosEffect::EffectParam& effectParam, 
  u8                            dependPokeId, 
  const s32*                    factorParam,
  u8                            factorParamNum )
{
  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( effectPos, posEffect );

  if( posEffectStatus->SetEffect( effectParam ) )
  {
    handler::pos::Add( posEffect, effectPos, dependPokeId, factorParam, factorParamNum );
    SCQUE_OP_StartPosEffect( m_que, posEffect, effectPos, effectParam.Raw.param1 );
    return true;
  }
  return false;
}

/**
 * @brief 全力効果処理( このゆびとまれ )
 * @param      zenryokuUsePoke  全力技を発動するポケモン
 * @param[out] effectMessage    全力効果発動時に表示するメッセージ
 * @retval true   全力効果が有効だった
 * @retval false  全力効果が有効でなかった
 */
bool ServerFlow::scproc_ZenryokuEffect_Konoyubitomare( BTL_POKEPARAM* zenryokuUsePoke, ZenryokuEffectMessage* effectMessage )
{
  if( handler::zenryoku::Add( zenryokuUsePoke, pml::wazadata::ZENRYOKUWAZA_EFFECT_KONOYUBITOMARE ) ) 
  {
    effectMessage->strId = BTL_STRID_SET_AuraEffect_Konoyubitomare;
    effectMessage->param1 = zenryokuUsePoke->GetID();
    return true;
  }
  return false;
}

/**
 * @brief 全力技開始メッセージ「○○の　ぜんりょくパワー！」
 * @param      zenryokuUsePoke    全力技を使用するポケモン
 * @param[out] zenryokuWazaKind   全力技の種類
 */
void ServerFlow::scproc_ZenryokuWazaStartMessage( const BTL_POKEPARAM& zenryokuUsePoke, ZenryokuWazaKind zenryokuWazaKind )
{
  // 専用全力技は演出の中で表示する
  if( zenryokuWazaKind == pml::waza::ZENRYOKUWAZA_KIND_SPECIAL )
  {
    return;
  }
  
  scPut_ZenryokuWazaStartMessage( zenryokuUsePoke );
}

/**
 * @brief 全力技開始メッセージ「○○の　ぜんりょくパワー！」
 * @param zenryokuUsePoke  全力技を使用するポケモン
 */
void ServerFlow::scPut_ZenryokuWazaStartMessage( const BTL_POKEPARAM& zenryokuUsePoke )
{
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_ZenryokuPower, zenryokuUsePoke.GetID() );
}

/**
 * @brief 派生された技から、全力技を決定する
 * @param[out] zenryokuWaza        全力技
 * @param[out] zenryokuWazaKind    全力技の種類
 * @param      attacker            技を使用するポケモン
 * @param      actWaza             実行する技
 */
void ServerFlow::scproc_DecideZenryokuWazaByReqWaza( WazaID* zenryokuWaza, ZenryokuWazaKind* zenryokuWazaKind, const BTL_POKEPARAM* attacker, WazaID actWaza )
{
  // 実行する技が派生された技なら、
  // 専用全力技の条件を無視して、派生された技の技データに設定されている全力技になる
  *zenryokuWazaKind = WAZADATA_IsDamage( actWaza ) ? ( pml::waza::ZENRYOKUWAZA_KIND_GENERAL ) : ( pml::waza::ZENRYOKUWAZA_KIND_BOOST );
  *zenryokuWaza = WAZADATA_GetZenryokuWazaNo( actWaza );
}

/**
 * @brief 全力技取得イベント
 * @param[out] zenryokuWaza      全力技
 * @param[out] zenryokuWazaKind  全力技の種類
 * @param      attacker          技を使用するポケモン
 * @param      actWaza           実行する技
 */
void ServerFlow::scEvent_GetZenryokuWaza( WazaID* zenryokuWaza, ZenryokuWazaKind* zenryokuWazaKind, const BTL_POKEPARAM* attacker, WazaID actWaza )
{
  *zenryokuWaza = ZenryokuWaza::GetZenryokuWazaNo( *attacker, actWaza );
  *zenryokuWazaKind = ZenryokuWaza::GetZenryokuWazaKind( *attacker, actWaza );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_ZENRYOKU_WAZAID, *zenryokuWaza );
    EVENTVAR_SetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND, *zenryokuWazaKind );
    EVENT_CallHandlers( this, BTL_EVENT_GET_ZENRYOKU_WAZA );
    *zenryokuWaza = static_cast<WazaID>( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZAID ) );
    *zenryokuWazaKind = static_cast<ZenryokuWazaKind>( EVENTVAR_GetValue( BTL_EVAR_ZENRYOKU_WAZA_KIND ) );
  EVENTVAR_Pop();
}

/**
 * @brief 技パラメータ確定後イベント
 * @param attacker          技を使用するポケモン
 * @param orgWazaID         選択した技
 * @param actWazaID         実行する技
 * @param isActZenryokuWaza 実行する技が全力技か？
 */
void ServerFlow::scEvent_AfterWazaParamFixed( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, bool isActZenryokuWaza )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ORG_WAZAID, orgWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, actWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG, isActZenryokuWaza );
    EVENT_CallHandlers( this, BTL_EVENT_AFTER_WAZA_PARAM_FIXED );
  EVENTVAR_Pop();
}

/**
 * @brief 派生技を全力技として使用し、派生された技を全力ブーストするメッセージを表示する
 * @param attacker        派生技を使用するポケモン
 * @param reqWaza         派生技
 * @param actWaza         派生される技
 * @param actZenryokuWaza 派生される技の全力技
 */
void ServerFlow::scproc_ZenryokuReqWazaMessage( const BTL_POKEPARAM* attacker, WazaID reqWaza, WazaID actWaza, WazaID actZenryokuWaza )
{
  if( scEvent_CheckZenryokuReqWazaMsgCustom( attacker, reqWaza, actWaza, actZenryokuWaza, &m_strParam ) )
  {
    handexSub_putString( &m_strParam );
    HANDEX_STR_Clear( &m_strParam );
  }
}


//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
ServerFlow::SV_WazaFailCause ServerFlow::scproc_Fight_CheckReqWazaFail( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam )
{
  // かいふくふうじチェック
  if( ( attacker->CheckSick(pml::wazadata::WAZASICK_KAIHUKUHUUJI ) &&  
      ( WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_KaifukuHuuji ) ) ) ) 
  {
    return SV_WAZAFAIL_KAIHUKUHUUJI;
  }

  // じごくづきチェック
  if( attacker->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI ) &&
      WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_Sound ) )
  {
    return SV_WAZAFAIL_ZIGOKUDUKI;
  }

  // じゅうりょくチェック
  if( ( m_fieldStatus->CheckEffect( FieldStatus::EFF_JURYOKU ) ) &&  
      ( WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_Flying ) ) )
  {
    return SV_WAZAFAIL_JURYOKU;
  }

  // スカイバトルチェック
  if( isSkyBattleFailWaza( wazaParam->wazaID ) )
  {
    return SV_WAZAFAIL_SKYBATTLE;
  }

  return SV_WAZAFAIL_NULL;
}
//----------------------------------------------------------------------------------
/**
 * マジックコート処理ルート
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MagicCoat_Root( WazaID actWaza )
{
  BTL_POKEPARAM *robPoke, *targetPoke;

  BTL_N_Printf( DBGSTR_SVFL_MagicCoatStart, m_magicCoatParam->robberCount);

  for(u32 i=0; i<m_magicCoatParam->robberCount; ++i)
  {
    robPoke = m_pokeCon->GetPokeParam( m_magicCoatParam->robberPokeID[i] );
    targetPoke = m_pokeCon->GetPokeParam( m_magicCoatParam->targetPokeID[i] );

    BTL_N_Printf( DBGSTR_SVFL_MagicCoatReflect,
      m_magicCoatParam->robberPokeID[i],
      m_magicCoatParam->targetPokeID[i],
      m_magicCoatParam->targetPos[i] );

    if( robPoke->IsFightEnable() )
    {
      // 跳ね返し確定イベント
      u32 hem_state = m_HEManager.PushState();
      scEvent_WazaReflect( robPoke, targetPoke, actWaza );
      m_HEManager.PopState( hem_state );

      // その後、跳ね返し処理
      // ワザパラメータ差し替え
      scEvent_GetWazaParam( actWaza, robPoke, m_wazaParam );
      m_wazaParam->fMagicCoat = true;
      m_flowSub.RegisterTargets( this, robPoke, m_magicCoatParam->targetPos[i], m_wazaParam, m_psetRobTarget );

      EVENT_SleepFactorMagicMirrorUser( robPoke->GetID() );

        handler::waza::Add( robPoke, actWaza, robPoke->GetValue( BTL_POKEPARAM::BPP_AGILITY) );
        {
          ActionDesc actionDesc;
          ActionDesc_Clear( &actionDesc );
          actionDesc.isMagicCoatReaction = true;
          scproc_Fight_WazaExe( robPoke, m_wazaParam, m_psetRobTarget, &actionDesc );
        }
        handler::waza::RemoveForce( robPoke, actWaza );

      EVENT_WakeFactorMagicMirrorUser( robPoke->GetID() );
    }
  }
}

void ServerFlow::wazaRobParam_Init( WAZA_ROB_PARAM* param )
{
  param->robberCount = 0;
  for(u32 i=0; i<GFL_NELEMS(param->robberPokeID); ++i){
    param->robberPokeID[i] = BTL_POKEID_NULL;
  }
}
void ServerFlow::wazaRobParam_Add( WAZA_ROB_PARAM* param, u8 robberPokeID, u8 targetPokeID, BtlPokePos targetPos )
{
  if( param->robberCount < (GFL_NELEMS(param->robberPokeID)) )
  {
    param->robberPokeID[ param->robberCount ] = robberPokeID;
    param->targetPokeID[ param->robberCount ] = targetPokeID;
    param->targetPos[ param->robberCount ] = targetPos;
    param->robberCount++;
  }
}
//----------------------------------------------------------------------------------
/**
 * @brief ワザ乗っ取り処理ルート
 *
 * @param   attacker        技を乗っ取られるポケモン
 * @param   waza            乗っ取られる技
 * @param   isZenryokuWaza  乗っ取る技が全力技か？
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaRobRoot( BTL_POKEPARAM* attacker, WazaID actWaza, bool isZenryokuWaza )
{
  // 誰かが乗っ取った
  if( m_wazaRobParam->robberCount )
  {
    u8 robPokeID = m_wazaRobParam->robberPokeID[ 0 ];
    BTL_POKEPARAM* robPoke = m_pokeCon->GetPokeParam( robPokeID );

    // 乗っ取り（乗っ取ったポケが主体になり、指定されたターゲットにワザを出す）
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_WazaRob( robPoke, attacker, actWaza );
      m_HEManager.PopState( hem_state );

      // ワザパラメータ差し替え
      scEvent_GetWazaParam( actWaza, robPoke, m_wazaParam );
      m_flowSub.RegisterTargets( this, robPoke, m_wazaRobParam->targetPos[0], m_wazaParam, m_psetRobTarget );

      BTL_N_Printf( DBGSTR_SVFL_YokodoriExe, robPokeID, m_wazaRobParam->targetPos[0] );

      // かいふくふうじチェック
      if( robPoke->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI)
      &&  (WAZADATA_GetFlag(actWaza, pml::wazadata::WAZAFLAG_KaifukuHuuji))
      ){
        scPut_WazaExecuteFailMsg( robPoke, actWaza, isZenryokuWaza, SV_WAZAFAIL_KAIHUKUHUUJI );
        return;
      }

      // じごくづきチェック
      if( robPoke->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI ) &&
          WAZADATA_GetFlag( actWaza, pml::wazadata::WAZAFLAG_Sound ) )
      {
        scPut_WazaExecuteFailMsg( robPoke, actWaza, isZenryokuWaza, SV_WAZAFAIL_ZIGOKUDUKI );
        return;
      }

      handler::waza::Add( robPoke, actWaza, robPoke->GetValue( BTL_POKEPARAM::BPP_AGILITY) );
      {
        ActionDesc actionDesc;
        ActionDesc_Clear( &actionDesc );
        actionDesc.isYokodoriRobAction = true;
        scproc_Fight_WazaExe( robPoke, m_wazaParam, m_psetRobTarget, &actionDesc );
      }
      handler::waza::RemoveForce( robPoke, actWaza );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] 合体ワザの成立チェック
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_CheckCombiWazaExe( const BTL_POKEPARAM* attacker, WAZAPARAM* wazaParam )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENT_CallHandlers( this, BTL_EVENT_COMBIWAZA_CHECK );
    wazaParam->wazaType = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ワザメッセージ確定イベント呼び出し処理
 *
 * @param attacker       技を撃つポケモン
 * @param wazaParamOrg   実行する技の元技( ゆびをふる等 )
 * @param wazaParamAct   実行する技
 */
//----------------------------------------------------------------------------------
void ServerFlow::scproc_WazaCall_Decide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParamOrg, const WAZAPARAM* wazaParamAct )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_WazaCallDecide( attacker, wazaParamOrg, wazaParamAct );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザメッセージ確定
 *
 * @param attacker       技を撃つポケモン
 * @param wazaParamOrg   実行する技の元技( ゆびをふる等 )
 * @param wazaParamAct   実行する技
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_WazaCallDecide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParamOrg, const WAZAPARAM* wazaParamAct )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParamAct->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_ORG_WAZAID, wazaParamOrg->wazaID );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_CALL_DECIDE );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ワザだし確定イベント呼び出し処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExe_Decide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlEventType evType )
{
  u32 hem_state = m_HEManager.PushState();

  scEvent_WazaExeDecide( attacker, wazaParam, evType );
//  scproc_HandEx_Root( ITEM_DUMMY_DATA );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザだし確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaExeDecide( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlEventType evType  )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENT_CallHandlers( this, evType );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * 必要なら、合体ワザの発動準備処理を行う
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_Fight_CheckCombiWazaReady( BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos )
{
  static const WazaID CombiWazaTbl[] = {
    WAZANO_HONOONOTIKAI,  WAZANO_MIZUNOTIKAI,  WAZANO_KUSANOTIKAI,
  };
  u32 i;

  for(i=0; i<GFL_NELEMS(CombiWazaTbl); ++i)
  {
    if( CombiWazaTbl[i] == waza ){
      BTL_N_Printf( DBGSTR_SVFL_CombiWazaCheck, waza);
      break;
    }
  }
  if( (i != GFL_NELEMS(CombiWazaTbl))
  &&  (!attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_COMBIWAZA_READY))
  &&  (!attacker->CombiWaza_IsSetParam())
  ){
    enum {
      COMBI_MAX = BTL_POSIDX_MAX - 1,   // 合体ワザを一緒に撃ってくれる論理上の最大ポケ数
    };
    ACTION_ORDER_WORK*  orderWork[ COMBI_MAX ];
    u32 orderCnt = 0;
    u8 pokeID = attacker->GetID();

    for(i=0; i<GFL_NELEMS(CombiWazaTbl); ++i)
    {
      if( CombiWazaTbl[i] != waza )
      {
        orderWork[ orderCnt ] = ActOrderTool_SearchForCombiWaza( CombiWazaTbl[i], pokeID, targetPos );
        if( orderWork[ orderCnt ] != NULL )
        {
          BTL_N_Printf( DBGSTR_SVFL_CombiWazaFound,
                    targetPos, CombiWazaTbl[i], orderWork[orderCnt]->bpp->GetID() );
          if( ++orderCnt >= COMBI_MAX ){ break; }
        }
      }
    }

    if( orderCnt )
    {
      BTL_POKEPARAM* combiPoke;
      u8 idx, idxMin, combiPokeID;

      idxMin = BTL_EXIST_POS_MAX;
      combiPoke = orderWork[ 0 ]->bpp;

      for(i=0; i<orderCnt; ++i)
      {
        idx = ActOrderTool_GetIndex( orderWork[i] );
        if( idx < idxMin ){
          idxMin = idx;
          combiPoke = orderWork[i]->bpp;
        }
      }

      combiPokeID = combiPoke->GetID();
      BTL_N_Printf( DBGSTR_SVFL_CombiDecide, pokeID, combiPokeID );

      attacker->TURNFLAG_Set(  BTL_POKEPARAM::TURNFLG_COMBIWAZA_READY );
      combiPoke->CombiWaza_SetParam( pokeID, waza );
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_CombiWazaReady, pokeID, combiPokeID );

      ActOrder_IntrReserve( combiPokeID );
      return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * 必要なら、時間差発動ワザの準備処理を行う。
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 * @param   zWazaKind   実行する技の全力技タイプ
 *
 * @retval  bool      時間差発動ワザ準備処理を行った場合true
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_Fight_CheckDelayWazaSet( BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos, u8* fWazaEnable, ZenryokuWazaKind zWazaKind )
{
  u32 hem_state = m_HEManager.PushState();
  bool result = scEvent_CheckDelayWazaSet( attacker, &targetPos );

  *fWazaEnable = false;

  if( result )
  {
    BtlPokePos  atPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, attacker->GetID() );
    if( scproc_HandEx_Result() == HandExResult_Enable )
    {
      SCQUE_PUT_ACT_WazaEffect( m_que, atPos, targetPos, waza, BTLV_WAZAEFF_DELAY_START, false, zWazaKind );
      scproc_Fight_DecideDelayWaza( attacker );
      *fWazaEnable = true;
    }
    else{
      scPut_WazaFail( attacker, waza );
    }
  }

  m_HEManager.PopState( hem_state );
  return result;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 時間差ワザ準備チェック
 *
 * @param   wk
 * @param   attacker
 * @param   targetPos
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckDelayWazaSet( const BTL_POKEPARAM* attacker, BtlPokePos* targetPos )
{
  bool result;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEPOS, *targetPos );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_DELAY_WAZA );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    *targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
  EVENTVAR_Pop();

  return result;
}
//----------------------------------------------------------------------------------
/**
 * 時間差発動ワザの実行確定（ワザエフェクト表示後）
 *
 * @param   wk
 * @param   attacker
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_DecideDelayWaza( const BTL_POKEPARAM* attacker )
{
  u32 hem_state = m_HEManager.PushState();

    scEvent_DecideDelayWaza(  attacker );

  m_HEManager.PopState( hem_state );
}

//----------------------------------------------------------------------------------
/**
 * [Event] 時間差ワザ発動確定
 *
 * @param   wk
 * @param   attacker
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_DecideDelayWaza( const BTL_POKEPARAM* attacker )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_DECIDE_DELAY_WAZA );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ワザシーケンス開始処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_StartWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_StartWazaSeq(  attacker, waza );
  m_HEManager.PopState( hem_state );
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザシーケンス開始
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_StartWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASEQ_START );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ワザシーケンス終了処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_EndWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza, bool fWazaEnable, const ActionDesc* actionDesc )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_EndWazaSeq( attacker, waza, fWazaEnable, actionDesc );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザシーケンス終了
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_EndWazaSeq( const BTL_POKEPARAM* attacker, WazaID waza, bool fWazaEnable, const ActionDesc* actionDesc )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_GEN_FLAG, fWazaEnable );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION, actionDesc->isOdorikoReaction );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASEQ_END );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ乗っ取り確定
 *
 * @param   wk
 * @param   robPoke       乗っ取った側
 * @param   orgAtkPoke    元々、ワザを出した側
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaRob( const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, robPoke->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, orgAtkPoke->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASEQ_ROB );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] マジックコート跳ね返し確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaReflect( const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, robPoke->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, orgAtkPoke->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASEQ_REFRECT );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * わざ乗っ取りチェック
 *
 * @param[in]   attacker        技を撃つポケモン
 * @param[in]   waza            使用する技
 * @param[in]   isZenryokuWaza  使用する技が全力技か？
 * @param[in]   rec             本来のターゲット群
 * @param[out]  robParam        乗っ取った場合、その詳細を保持
 *
 * @retval true   技乗っ取りが成立した
 * @retval false  技乗っ取りが成立しなかった
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_Check_WazaRob( 
  const BTL_POKEPARAM* attacker,
  WazaID waza, 
  bool isZenryokuWaza,
  const PokeSet* rec,
  WAZA_ROB_PARAM* robParam )
{
  u8 robberPokeID = BTL_POKEID_NULL;
  u8 robTargetPokeID = BTL_POKEID_NULL;

  scEvent_CheckWazaRob(  attacker, waza, isZenryokuWaza, rec, &robberPokeID, &robTargetPokeID );
  if( robberPokeID != BTL_POKEID_NULL )
  {
    if( robTargetPokeID != BTL_POKEID_NULL ){
      robParam->targetPos[0] = m_posPoke.GetPokeExistPos( robTargetPokeID );
    }else{
      robParam->targetPos[0] = BTL_POS_NULL;
    }

    BTL_N_Printf( DBGSTR_SVFL_YokodoriInfo, robTargetPokeID, robParam->targetPos[0]);

    robParam->robberPokeID[0] = robberPokeID;
    robParam->robberCount = 1;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 自分以外のポケが出したワザを乗っ取る判定
 *
 * @param   attacker        [in]  技を撃つポケモン
 * @param   waza            [in]  使用する技
 * @param   isZenryokuWaza      [in]  使用する技が全力技か？
 * @param   targetRec       [in]  本来のターゲット群
 * @param   robberPokeID    [out] 乗っ取ったポケモンID
 * @param   robTargetPokeID [out] 乗っ取った場合の対象ポケモンID
 * @param   strParam        [out] 乗っ取った場合のメッセージパラメータ
 *
 * @retval  bool    乗っ取ったらtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scEvent_CheckWazaRob(
  const BTL_POKEPARAM* attacker, 
  WazaID waza,
  bool isZenryokuWaza,
  const PokeSet* targetRec,
  u8* robberPokeID,
  u8* robTargetPokeID )
{
  u32 targetCnt = targetRec->GetCount();
  u32 i;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG, isZenryokuWaza );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i)
      {
        bpp = targetRec->Get( i );
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
      }
    }
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID, BTL_POKEID_NULL );
    EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEID_NULL );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_WAZA_ROB );

    *robberPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID );
    *robTargetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  EVENTVAR_Pop();

  return (*robberPokeID) != BTL_POKEID_NULL;
}
// 他ワザ呼び出しするワザのエフェクトを発動
void ServerFlow::scPut_ReqWazaEffect( BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos, ZenryokuWazaKind zWazaKind )
{
  BtlPokePos  atkPos;
  u8 pokeID = bpp->GetID();

  atkPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );

  SCQUE_PUT_ACT_WazaEffect( m_que, atkPos, targetPos, waza, 0, false, zWazaKind );
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlow::checkPlayersPoke( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  if( MainModule::PokeIDtoClientID(pokeID) == m_mainModule->GetPlayerClientID() )
  {
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * ワザ出しレコード更新
 *
 * @param   wk
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExeRecordUpdate( const BTL_POKEPARAM* bpp, WazaID waza )
{
  if( checkPlayersPoke(bpp) )
  {
    if( waza == WAZANO_HANERU ){
      m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_WAZA_HANERU );
      return;
    }
    if( waza == WAZANO_WARUAGAKI ){
      m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_WAZA_WARUAGAKI );
      return;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * 「たたかう」-> ワザ出し成功以降の処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   actionDesc   アクションの詳細情報
 *
 * @retval  bool  ワザが無効に終わった場合false／それ以外はtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_Fight_WazaExe( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targetRec, const ActionDesc* actionDesc )
{
  WazaID waza = wazaParam->wazaID;

  WazaCategory  category = WAZADATA_GetCategory( waza );
  u8 pokeID = attacker->GetID();
  u8 fEnable = true;
  u8 fDamageWaza = false;
  u8 fMigawariHit = false;

  WAZAEFF_RESERVE_POS  que_reserve_pos;

  BTL_N_Printf( DBGSTR_SVFL_WazaExeStart, attacker->GetID(), waza );

  // レコードデータ更新
  scproc_WazaExeRecordUpdate( attacker, waza );

  // １ターン溜めワザの発動チェック
  if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Tame) )
  {
    TameWazaResult tameResult = scproc_Fight_TameWazaExe( attacker, targetRec, wazaParam );
    switch( tameResult ){
    case TAMEWAZA_START_FAIL:    ///< 溜め開始ターン：失敗
      return false;
    case TAMEWAZA_START_OK:      ///< 溜め開始ターン：成功
      return true;
    case TAMEWAZA_RELEASE_FAIL:  ///< 溜め解放ターン：失敗
      scEvent_TameReleaseFailed( attacker, targetRec, wazaParam );
      return false;
    case TAMEWAZA_RELEASE_OK:    ///< 溜め解放ターン：成功
    default:
      break;
    }
  }

  // ダメージ受けポケモンワークをクリアしておく
  m_psetDamaged->Clear();

  // ワザエフェクトコマンド生成用にバッファ領域を予約しておく
  reserveWazaEffPos( &que_reserve_pos );
  wazaEffCtrl_Setup( m_wazaEffCtrl, attacker, targetRec );

  // ワザ出し処理開始イベント
  {
    u32 hem_state = m_HEManager.PushState();
    WazaForceEnableMode enableMode = scEvent_WazaExecuteStart( actionDesc, attacker, wazaParam , targetRec );
    m_HEManager.PopState( hem_state );
    if( enableMode != WAZAENABLE_NONE )
    {
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
      if( enableMode == WAZAENABLE_QUIT )
      {
        scPut_WazaEffect( waza, m_wazaEffCtrl, que_reserve_pos, false, wazaParam->zenryokuWazaKind ); // ここではダメージ発生しないはず
        return true;
      }
    }
  }

  switch( category ){
  case pml::wazadata::CATEGORY_SIMPLE_DAMAGE:
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT_USER:
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT:
  case pml::wazadata::CATEGORY_DAMAGE_SICK:
  case pml::wazadata::CATEGORY_DRAIN:
    fDamageWaza = true;
    fMigawariHit = true;
    break;
  case pml::wazadata::CATEGORY_ICHIGEKI:
    fMigawariHit = true;
    break;
  }


  // 最初からターゲットが存在しないなら、失敗
  //
  // @fix GFNMCat[1288] 乱入後に隣の立ち位置を攻撃した際、技が失敗したメッセージが表示されない
  // ここで失敗するのは、乱入発生後、「たいあたり」など、対象を1体選択する技を使用する際に、味方の位置を選択した場合が該当します。
  // ( 乱入後の対象選択画面がダブルと同じ画面なので選択できる。しかし味方は存在しない )
  {
    BtlRule rule = m_mainModule->GetRule();
    if( ( rule == BTL_RULE_INTRUDE ) &&
        ( wazaParam->targetType == pml::wazadata::TARGET_OTHER_SELECT ) &&
        ( targetRec->GetCountMax() == 0 ) )
    {
      if( !(m_fWazaFailMsgDisped) ){
        scPut_WazaFail( attacker, waza );
      }
      fEnable = false;
    }
  }

  if( fEnable )
  {
    // 死んでるポケモンは除外
    targetRec->RemoveDisablePoke( m_posPoke );
    
    // ムーブ直後に自分単体がターゲットならハズレ
    scproc_CheckMovedPokeAvoid( attacker, targetRec, m_wazaParam, fDamageWaza );
    
    // 最初は居たターゲットが残っていない->うまく決まらなかった、終了
    if( scproc_IsTargetRemovedAll(attacker, m_wazaParam, targetRec) )
    {
      BTL_PRINT("最初はいたターゲットが残っていないので失敗\n");
      if( !(m_fWazaFailMsgDisped) ){
        scPut_WazaFail( attacker, waza );
      }
      fEnable = false;
    }
  }

  if( fEnable )
  {
    // ここから先、ハズレ・無効の原因表示はその先に任せる

    // そらをとぶなど、画面に見えていない状態のハズレチェック
    if( category != pml::wazadata::CATEGORY_ICHIGEKI ){  // 一撃ワザは独自の回避判定を行うので
      flowsub_CheckPokeHideAvoid( m_wazaParam, attacker, targetRec );
    }

    // 技のタイプ相性をチェック
    flowsub_CheckTypeAffinity( m_wazaParam, attacker, targetRec, &m_dmgAffRec );

    // 防御系の技による無効化チェック
    flowsub_CheckNotEffect_Guard( m_wazaParam, attacker, targetRec, m_dmgAffRec, m_actionRecorder );

    // 特性による無効化チェック
    flowsub_CheckNotEffect_Tokusei( m_wazaParam, attacker, targetRec, m_dmgAffRec, m_actionRecorder );

    // タイプによる無効化チェック
    if( (fMigawariHit)
    ||  (scEvent_WazaAffineCheckEnable(m_wazaParam, attacker))
    ){
      flowsub_CheckNoEffect_TypeAffinity( m_wazaParam, attacker, targetRec, m_dmgAffRec, m_actionRecorder );
    }

    // 技とポケモンの相性による無効化チェック
    flowsub_CheckNotEffect_Affinity( m_wazaParam, attacker, targetRec, m_dmgAffRec );

    // カテゴリ「シンプルシック」の無効化チェック
    if( category == pml::wazadata::CATEGORY_SIMPLE_SICK )
    {
      flowsub_CheckNoEffect_SimpleSick( m_wazaParam, attacker, targetRec );
    }

    // カテゴリ「シンプルエフェクト」の無効化チェック
    if( category == pml::wazadata::CATEGORY_SIMPLE_EFFECT )
    {
      flowsub_CheckNoEffect_SimpleEffect( m_wazaParam, attacker, targetRec );
    }

    if( category != pml::wazadata::CATEGORY_ICHIGEKI ){  // 一撃ワザは独自の回避判定を行うので
      flowsub_checkWazaAvoid( m_wazaParam, attacker, targetRec, m_actionRecorder );
    }

    // 最初は居たターゲットが残っていない -> 無効イベント呼び出し後終了
    if( scproc_IsTargetRemovedAll(attacker, m_wazaParam, targetRec) )
    {
      scproc_WazaExe_NotEffective( pokeID, waza, *actionDesc );
      fEnable = false;
    }
    else
    {
      // ダメージワザ以外のみがわり除外チェック
      scproc_MigawariExclude( m_wazaParam, attacker, targetRec, fMigawariHit );
      // ターゲットが残っていない -> 無効イベント呼び出し後終了
      if( scproc_IsTargetRemovedAll(attacker, m_wazaParam, targetRec) )
      {
        scPut_WazaFail( attacker, waza );
        scproc_WazaExe_NotEffective( pokeID, waza, *actionDesc );
        fEnable = false;
      }
    }
  }

  if( fEnable )
  {
    BTL_PRINT("ワザ=%d, カテゴリ=%d\n", m_wazaParam->wazaID, category );

    if( fDamageWaza ){
      scproc_Fight_Damage_Root( actionDesc, m_wazaParam, attacker, targetRec, m_dmgAffRec, false );
    }
    else{

      switch( category ){
      case pml::wazadata::CATEGORY_SIMPLE_EFFECT:
        scproc_Fight_SimpleEffect( m_wazaParam, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_SIMPLE_SICK:
        scproc_Fight_SimpleSick( waza, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_EFFECT_SICK:
        scproc_Fight_EffectSick( m_wazaParam, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_ICHIGEKI:
        scproc_Fight_Ichigeki( actionDesc, m_wazaParam, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_PUSHOUT:
        scproc_Fight_PushOut( waza, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_SIMPLE_RECOVER:
        scproc_Fight_SimpleRecover( m_wazaParam, attacker, targetRec );
        break;
      case pml::wazadata::CATEGORY_FIELD_EFFECT:
        scput_Fight_FieldEffect( m_wazaParam, attacker );
        break;
      case pml::wazadata::CATEGORY_SIDE_EFFECT:
      case pml::wazadata::CATEGORY_OTHERS:
        scput_Fight_Uncategory( m_wazaParam, attacker, targetRec );
        break;
      default:
        break;
      }

      scproc_Fight_UnDamageProcEnd( m_wazaParam, attacker );
    }

    // ワザ効果あり→エフェクトコマンド生成などへ
    if( wazaEffCtrl_IsEnable(m_wazaEffCtrl) )
    {
      if( !wazaEffCtrl_IsDone(m_wazaEffCtrl) )
      {
        scPut_WazaEffect( waza, m_wazaEffCtrl, que_reserve_pos, false, m_wazaParam->zenryokuWazaKind );
      }

      m_wazaRec.SetEffectiveLast();

      // 反動で動けなくなる処理
      if( !attacker->IsDead() ){
        if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Tire) ){
          scPut_SetContFlag( attacker, BTL_POKEPARAM::CONTFLG_CANT_ACTION );
        }
      }

      scproc_WazaExe_Effective( pokeID, waza, *actionDesc );
      scproc_WazaExe_SetDirt( attacker, m_wazaParam, targetRec );

    }else{
      scproc_WazaExe_NotEffective( pokeID, waza, *actionDesc );
      fEnable = false;
    }

  }
  else{
    m_actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_MASTER );

    // 無効でもエフェクトのみ発動する（じばく・だいばくはつ対応）
    if( wazaEffCtrl_IsEnable(m_wazaEffCtrl) ){
      scPut_WazaEffect( waza, m_wazaEffCtrl, que_reserve_pos, true, m_wazaParam->zenryokuWazaKind );
    }
  }

  scproc_WazaExe_Done( pokeID, waza, *actionDesc );
  scproc_CheckDeadCmd( attacker, false );

  return fEnable;
}

/**
 * @brief 技実行 --> ポケモンに汚れをセットする
 * @param attacker   技を実行したポケモン
 * @param wazaParam  実行した技
 * @param targetRec  技ターゲット
 */
void ServerFlow::scproc_WazaExe_SetDirt( 
  BTL_POKEPARAM* attacker,
  const WAZAPARAM* wazaParam, 
  PokeSet* targetRec )
{
  // 技を撃ったポケモンに汚れをセット
  scproc_SetDirt( attacker, m_wazaParam );


  // 技を受けたポケモンに汚れをセット
  BTL_POKEPARAM* defender;

  targetRec->SeekStart();
  while( ( defender = targetRec->SeekNext() ) != NULL )
  {
    // 自分で自分に撃った技は除く
    if( defender->GetID() == attacker->GetID() ) {
      continue;
    }

    scproc_SetDirt( defender, m_wazaParam );    
  }
}

/**
 * @brief 使用した or 受けた技に応じた永久フラグをセットする
 * @param poke       技を使用した( 受けた )ポケモン
 * @param wazaParam  使用した( 受けた )技
 */
void ServerFlow::scproc_SetDirt( BTL_POKEPARAM* poke, const WAZAPARAM* wazaParam )
{
  const DirtType dirtType = WAZADATA_GetDirtType( wazaParam->wazaID );
  if( dirtType == pml::pokepara::DIRT_TYPE_NONE )
  {
    return;
  }

  const u8 rate = WAZADATA_GetDirtRate( wazaParam->wazaID );
  const u8 rnd = calc::GetRand( 100 );
  if( rnd < rate )
  {
    scPut_SetDirt( poke, dirtType );
  }
}

//----------------------------------------------------------------------------------
/**
 * ムーブ後に自分単体に当たってしまうワザはハズレるチェック
 *
 * @param   wk
 * @param   attacker
 * @param   targetRec
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_CheckMovedPokeAvoid( const BTL_POKEPARAM* attacker, PokeSet* targetRec, const WAZAPARAM* wazaParam, bool fDamageWaza )
{
  if( targetRec->GetCountMax() == 1 )
  {
    BTL_POKEPARAM* bppTarget = targetRec->Get( 0 );
    if( (bppTarget != NULL)
    &&  (bppTarget->GetID() == attacker->GetID())
    &&  (wazaParam->targetType != pml::wazadata::TARGET_USER)
    ){
      if( (attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_MOVED))
      ||  (fDamageWaza)
      ){
        BTL_PRINT("[SVFLOW]ムーブ直後の自分狙いはハズれます(POKEID=%d)\n", bppTarget->GetID());
        targetRec->Remove( bppTarget );
      }
    }
  }
}
/**
 * 最初は１体以上のターゲットがいたが、現在ターゲットがいなくなっている AND ワザ失敗とみなす判定
 * @param attacker
 * @param wazaParam
 * @param targets
 * @return 失敗とみなす場合は TRUE
 */
bool ServerFlow::scproc_IsTargetRemovedAll( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, const PokeSet* targets )
{
  if( targets->IsRemovedAll() )
  {
    if( !scEvent_IsRemoveAllFailCancel(attacker, wazaParam) )
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 種々の条件チェックによりターゲットが残っていないケースでもワザ処理を実行するかチェック
 * @param attacker
 * @param wazaParam
 * @return 実行する場合 true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_IsRemoveAllFailCancel( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam )
{
  bool bCancel = false;

  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, bCancel );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_REMOVEALL_CANCEL );
    bCancel = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return bCancel;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザのタイプ相性による無効チェックを行うか判定（通常はダメージ技以外、行わない）
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 *
 * @retval  bool    行う場合はtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_WazaAffineCheckEnable( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker )
{
  bool fAffineCheck = false;

  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_AFFINITY_ENABLE );
    fAffineCheck = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return fAffineCheck;
}
//----------------------------------------------------------------------------------
/**
 * みがわり中のポケモンをターゲットから除外
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   target
 * @param   fDamage
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_MigawariExclude( const WAZAPARAM* wazaParam,
    const BTL_POKEPARAM* attacker, PokeSet* target, bool fDamage )
{
  BTL_POKEPARAM* bpp;

  target->SeekStart();
  while( (bpp = target->SeekNext() ) != NULL )
  {
    if( (attacker != bpp)
    &&  (bpp->MIGAWARI_IsExist())
    ){
      // XYより 非ダメージ技でも身代わり貫通フラグを有効にする
      if( (!fDamage) && (WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_MigawariThru)==false) )
//      if( WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_MigawariThru)==false )
      {
        if( !scEvent_CheckMigawariThrew(attacker, bpp, wazaParam->wazaID) ){
          target->Remove( bpp );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] みがわりの効果を貫通するかチェック
 *
 * @param   attacker    攻撃ポケ
 * @param   defender    ターゲットポケ（みがわり中）
 * @param   waza        攻撃ポケの出したワザＩＤ
 *
 * @retval  bool    貫通する場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckMigawariThrew( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza  )
{
  bool bThrew = WAZADATA_GetFlag( waza, pml::wazadata::WAZAFLAG_MigawariThru );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, bThrew );
    EVENT_CallHandlers( this, BTL_EVENT_MIGAWARI_THREW );
    bThrew = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return bThrew;
}
//----------------------------------------------------------------------------------
/**
 * ワザ出した->有効だった時のイベント呼び出し
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 * @param   actionDesc  アクションの詳細情報
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExe_Effective( u8 pokeID, WazaID waza, const ActionDesc& actionDesc )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_WazaExeEnd_Common(  pokeID, waza, actionDesc, BTL_EVENT_WAZA_EXECUTE_EFFECTIVE );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ワザ出した->無効だった時のイベント呼び出し
 *
 * @param   wk
 * @param   pokeID      ワザを出したポケモンID
 * @param   waza
 * @param   actionDesc  アクションの詳細情報
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExe_NotEffective( u8 pokeID, WazaID waza, const ActionDesc& actionDesc )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_WazaExeEnd_Common(  pokeID, waza, actionDesc, BTL_EVENT_WAZA_EXECUTE_NO_EFFECT );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ワザ出し最終イベント呼び出し
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 * @param   actionDesc  アクションの詳細情報
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExe_Done( u8 pokeID, WazaID waza, const ActionDesc& actionDesc )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_WazaExeEnd_Common(  pokeID, waza, actionDesc, BTL_EVENT_WAZA_EXECUTE_DONE );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ出し終了共通
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   actionDesc  アクションの詳細情報
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaExeEnd_Common( u8 pokeID, WazaID waza, const ActionDesc& actionDesc, BtlEventType eventID )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_YOKODORI_ROB_ACTION, actionDesc.isYokodoriRobAction );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_MAGICCOAT_REACTION, actionDesc.isMagicCoatReaction );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION, actionDesc.isOdorikoReaction );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_OIUTI_INTERRUPT_ACTION, actionDesc.isOiutiInterruptAction );
    EVENT_CallHandlers( this, eventID );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * 必中状態かどうかチェック
 *
 * @param   attacker
 * @param   target
 *
 * @retval  bool    必中状態ならtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlow::IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  if( m_posPoke.IsExistFrontPos(m_mainModule, attacker->GetID() )
  &&  (attacker->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_NOOGAADO)
  ){
    return true;
  }
  if( m_posPoke.IsExistFrontPos(m_mainModule, target->GetID())
  &&  (target->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_NOOGAADO)
  ){
    return true;
  }

  if( attacker->CheckSick(pml::wazadata::WAZASICK_MUSTHIT) ){
    return true;
  }
  if( attacker->CheckSick(pml::wazadata::WAZASICK_MUSTHIT_TARGET) )
  {
    u8 targetPokeID = static_cast<u8>(attacker->GetSickParam( pml::wazadata::WAZASICK_MUSTHIT_TARGET ));
    if( targetPokeID == target->GetID() ){
      return true;
    }
  }

  return false;
}
//--------------------------------------------------------------------------
/**
 * @brief 技発動前の、タイプ相性チェック
 *
 * @param      wazaParam   実行する技のパラメータ
 * @param      attacker    技を出すポケモン
 * @param      targets     技対象ポケモンコンテナ
 * @param[out] affRec      タイプ相性の格納先
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckTypeAffinity( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets, DmgAffRec* affRec )
{
  BTL_POKEPARAM* bpp;

  affRec->Init();

  targets->SeekStart();
  while( (bpp = targets->SeekNext() ) != NULL )
  {
    static const bool ONLY_ATTACKER = false;
    bool isNoEffectByFloatingStatus = false;
    BtlTypeAff aff = scProc_checkWazaDamageAffinity( attacker, bpp, wazaParam, ONLY_ATTACKER, &isNoEffectByFloatingStatus );
    affRec->Add( bpp->GetID(), aff, isNoEffectByFloatingStatus );
  }
}
//--------------------------------------------------------------------------
/**
 * ワザ発動前の、相性による無効化チェック
 *
 * @param      wk
 * @param[in]  attacker       ワザを出すポケモン
 * @param[io]  targets        ワザ対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 * @param[out] actionRecorder 技を出すポケモンの行動結果の記録先( 不要なら NULL )
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckNoEffect_TypeAffinity( const WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, ActionRecorder* actionRecorder )
{
  if( targets->GetCount() <= 0 )
  {
    return;
  }

  BTL_POKEPARAM* bpp;

  targets->SeekStart();
  while( (bpp = targets->SeekNext() ) != NULL )
  {
    u8 targetPokeId = bpp->GetID();
    BtlTypeAff aff = affRec.GetIfEnable( targetPokeId );
    bool isNoEffectByFloatingStatus = affRec.IsNoEffectByFloatingStatus( targetPokeId );

    if( aff == pml::battle::TypeAffinity::TYPEAFF_0 )
    {
      if( isNoEffectByFloatingStatus ) 
      {
        scproc_WazaNoEffectByFlying( bpp );
      }
      else 
      {
        scPut_WazaNoEffect( bpp );
      }

      targets->Remove( bpp );
      m_affCounter.CountUp( *m_mainModule, attacker, bpp, pml::battle::TypeAffinity::TYPEAFF_0 );
      if( MainModule::PokeIDtoClientID(attacker->GetID()) == m_mainModule->GetPlayerClientID() ){
        m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_WAZA_MUKOU );
      }
    }
  }

  // 技ターゲットが居なくなったら、タイプ相性によって技に失敗したことを記録
  if( ( actionRecorder != NULL ) &&
      ( targets->IsRemovedAll() ) )
  {
    actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_TYPE );
  }
}
//----------------------------------------------------------------------------------
/**
 * @brief ワザの相性計算
 *
 * @param      attacker                    攻撃側ポケモン
 * @param      defender                    防御側ポケモン
 * @param      wazaParam                   使用する技のパラメータ
 * @param      fOnlyAttacker               true なら、防御側ポケモンの特性等を考慮しない
 * @param[out] isNoEffectByFloatingStatus  防御側ポケモンの「ふゆう」状態により、技が無効化された場合 true が格納される
 *
 * @return  技の相性
 */
//----------------------------------------------------------------------------------
BtlTypeAff ServerFlow::scProc_checkWazaDamageAffinity(
  const BTL_POKEPARAM* attacker, 
  const BTL_POKEPARAM* defender, 
  const WAZAPARAM*     wazaParam, 
  bool                 fOnlyAttacker,
  bool*                isNoEffectByFloatingStatus )
{
  // 無属性の攻撃（わるあがき）は、常に等倍でヒット
  if( wazaParam->wazaType == POKETYPE_NULL ){
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }

  BtlTypeAff affinity = scEvent_CheckDamageAffinity( *attacker, *defender, wazaParam->wazaType, fOnlyAttacker );

  // 「ふゆう」チェック
  if( !fOnlyAttacker ) {
    affinity = scproc_RewiteWazaAffinityByFloatingStatus( attacker, defender, wazaParam->wazaType, affinity, isNoEffectByFloatingStatus );
  }

  // 相性上書き
  affinity = scEvent_RewriteWazaAffinity( attacker, defender, wazaParam->wazaType, affinity );

  return affinity;
 
}

//----------------------------------------------------------------------------------
/**
 * @brief 防御側ポケモンの「ふゆう」状態による、技相性の上書き
 * @param      attacker                    攻撃側ポケモン
 * @param      defender                    防御側ポケモン
 * @param      wazaType                    使用する技のタイプ
 * @param      affinity                    上書きする前の相性
 * @param[out] isNoEffectByFloatingStatus  防御側ポケモンの「ふゆう」状態により、技が無効化された場合 true が格納される
 * @return 上書き後の相性
 */
//----------------------------------------------------------------------------------
BtlTypeAff ServerFlow::scproc_RewiteWazaAffinityByFloatingStatus( 
  const BTL_POKEPARAM* attacker, 
  const BTL_POKEPARAM* defender, 
  pml::PokeType        wazaType,
  BtlTypeAff           affinity,
  bool*                isNoEffectByFloatingStatus )
{
  *isNoEffectByFloatingStatus = false;

  // もともとの相性が無効の場合
  if( affinity == pml::battle::TypeAffinity::TYPEAFF_0 )
  {
    // 地面ワザなら受けたポケ=飛行タイプのはずなので、「ふゆう状態」チェック
    if( wazaType == POKETYPE_JIMEN )
    {
      // 「ふゆう状態」でないなら等倍ヒット
      if( !scproc_CheckFloating( defender, true ) ) {
        return pml::battle::TypeAffinity::TYPEAFF_1;
      }
    }

    return affinity;
  }

  // 以下、もともとの相性が有効の場合

  // 地面ワザ以外はそのまま
  if( wazaType != POKETYPE_JIMEN ) {
    return affinity;
  }

  // 地面ワザでも「ふゆう状態」でなければそのまま
  if( !scproc_CheckFloating( defender, false ) ) {
    return affinity;
  }
  if( scEvent_AffineFloatingCancel( attacker, defender ) ) {
    return affinity;
  }

  // ふゆう状態なら相性無効
  *isNoEffectByFloatingStatus = true;
  return pml::battle::TypeAffinity::TYPEAFF_0;
}

//----------------------------------------------------------------------------------
/**
 *  相性計算時、ふゆう状態のチェックを行わない判定
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scEvent_AffineFloatingCancel( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  bool bCancel = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, bCancel );
    EVENT_CallHandlers( this, BTL_EVENT_AFFINE_FLOATING_CANCEL );

    bCancel =  EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  EVENTVAR_Pop();

  return bCancel;
}

//----------------------------------------------------------------------------------
/**
 * @brief 技相性の上書きイベント
 * @param attacker  攻撃側ポケモン
 * @param defender  防御側ポケモン
 * @param wazaType  使用する技のタイプ
 * @param affinity  上書きする前の相性
 * @return 上書き後の相性
 */
//----------------------------------------------------------------------------------
BtlTypeAff ServerFlow::scEvent_RewriteWazaAffinity( 
  const BTL_POKEPARAM* attacker, 
  const BTL_POKEPARAM* defender, 
  pml::PokeType        wazaType,
  BtlTypeAff           affinity )
{
  BtlTypeAff result;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_TYPEAFF, affinity );
    EVENT_CallHandlers( this, BTL_EVENT_REWRITE_AFFINITY );

    result =  (BtlTypeAff)EVENTVAR_GetValue( BTL_EVAR_TYPEAFF );

  EVENTVAR_Pop();

  return result;
}

//--------------------------------------------------------------------------
/**
 * @brief 技とポケモンの相性による無効化チェック
 *
 * @param   wk
 * @param   attacker  [in] ワザを出すポケモン
 * @param   targets   [io] ワザ対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckNotEffect_Affinity( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  PokeSet* targets, const DmgAffRec& affRec )
{
  BTL_POKEPARAM* bpp;

  // Lv1 無効化チェック（攻撃ポケが必中状態なら無効化できない）
  scproc_checkNoEffect_sub(wazaParam, attacker, targets, &affRec, BTL_EVENT_NOEFFECT_CHECK_L1, true );

  // Lv2 無効化チェック（攻撃ポケが必中状態でも無効化）
  scproc_checkNoEffect_sub(wazaParam, attacker, targets, &affRec, BTL_EVENT_NOEFFECT_CHECK_L2, false );

  // タイプごとの特殊チェック（「まもる」より低プライオリティ）
  targets->SeekStart();
  while( (bpp = targets->SeekNext()) != NULL )
  {
    if( scproc_checkNoEffect_typeSp(wazaParam, attacker, bpp) ){
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, bpp->GetID() );
      targets->Remove( bpp );
    }
  }
}

/**
 * @brief 防御系の技効果により技を受けないポケモンを、技ターゲットから除外する
 *
 * @param wazaParam  使用する技のパラメータ
 * @param attacker   技を使用するポケモン
 * @param targets    技のターゲット
 * @param affRec     
 */
void ServerFlow::flowsub_CheckNotEffect_Guard( 
  const WAZAPARAM* wazaParam, 
  BTL_POKEPARAM* attacker,
  PokeSet* targets, 
  const DmgAffRec& affRec, 
  ActionRecorder* actionRecorder )
{
  // フィールド効果による無効化チェック
  scproc_checkNoEffect_sub(wazaParam, attacker, targets, &affRec, BTL_EVENT_NOEFFECT_CHECK_FIELD, false );

  // 防御系ワザ効果による失敗を確認するためこの時点でターゲットが残っているかどうか記憶しておく
  // （フィールド効果による無効化をチェックしないのは、フィールド効果はワザではないため）
  bool isExistTarget = !targets->IsRemovedAll();

  // 防御系サイドエフェクトによる無効化チェック：開始
  scEvent_checkNoEffectBySideEffectGuard_Begin( wazaParam, attacker, targets );

  // 防御系サイドエフェクトによる無効化チェック
  scproc_checkNoEffect_sub(wazaParam, attacker, targets, &affRec, BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD, false );


  // 防御系サイドエフェクトによる無効化チェック：終了
  scEvent_checkNoEffectBySideEffectGuard_End( wazaParam, attacker, targets );

  // まもるチェック
  scproc_checkMamoruNotEffect( wazaParam, attacker, targets, &affRec );

  // 効果チェック前に存在していたターゲットが完全にいなくなっていたら防御系ワザ効果による失敗を記録する
  if( isExistTarget && targets->IsRemovedAll() && actionRecorder )
  {
    actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_GUARD_WAZA );
  }

}


/**
 * @brief 「まもる」チェック
 *        「まもる」状態により わざを受けないポケモンを、わざターゲットから除外する
 *
 * @param wazaParam  使用する技のパラメータ
 * @param attacker   技を使用するポケモン
 * @param targets    技のターゲット
 * @param affRec     
 */
void ServerFlow::scproc_checkMamoruNotEffect( 
  const WAZAPARAM* wazaParam, 
  BTL_POKEPARAM* attacker,
  PokeSet* targets, 
  const DmgAffRec* affRec )
{
  BTL_POKEPARAM* bpp;
  if( WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_Mamoru) )
  {
    targets->SeekStart();
    while( (bpp = targets->SeekNext()) != NULL )
    {
      bool bMamoruEffect = bpp->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_MAMORU );
      if( !bMamoruEffect ){
        bMamoruEffect = scproc_checkNoEffect_core( wazaParam, attacker, bpp, affRec, BTL_EVENT_NOEFFECT_CHECK_MAMORU );
      }
      else if( bpp->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU_ONLY_DAMAGE_WAZA) ){
        if( !WAZADATA_IsDamage(wazaParam->wazaID) ){
          bMamoruEffect = false;
        }
      }
      if( bMamoruEffect )
      {
        targets->Remove( bpp );

        if( !m_fWazaFailMsgDisped ){
          scproc_MamoruSuccess( attacker, bpp, wazaParam );
        }
        m_fWazaFailMsgDisped = false;
      }
    }
  }
}


/**
 * @brief 特性により技を受けないポケモンを、技ターゲットから除外する
 *
 * @param      wazaParam  使用する技のパラメータ
 * @param      attacker   技を使用するポケモン
 * @param      targets    技のターゲット
 * @param      affRec     
 * @param[out] actionRecorder  技を使用するポケモンの行動結果の格納先( 不要ならNULL )
 */
void ServerFlow::flowsub_CheckNotEffect_Tokusei( 
  const WAZAPARAM* wazaParam, 
  BTL_POKEPARAM* attacker,
  PokeSet* targets, 
  const DmgAffRec& affRec,
  ActionRecorder* actionRecorder )
{
  if( targets->GetCount() <= 0 )
  {
    return;
  }

  scproc_checkNoEffect_sub( wazaParam, attacker, targets, &affRec, BTL_EVENT_NOEFFECT_CHECK_TOKUSEI, false );

  // 技ターゲットが居なくなったら、特性によって技に失敗したことを記録
  if( ( actionRecorder != NULL ) &&
      ( targets->IsRemovedAll() ) )
  {
    actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_TOKUSEI );
  }
}


//--------------------------------------------------------------------------
/**
 * @brief 技カテゴリ「シンプルシック」の無効化チェック
 *
 * @param[in]     wazaParam  使用する技
 * @param[in]     attacker   技を出すポケモン
 * @param[in,out] targets    技対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckNoEffect_SimpleSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  bool isEffective = false;
  BTL_POKEPARAM* bpp;


  targets->SeekStart();
  while( (bpp = targets->SeekNext()) != NULL )
  {
    if( !scproc_checkNoEffect_SimpleSick( wazaParam->wazaID, attacker, bpp ) )
    {
      targets->Remove( bpp );
    }
    else{
      isEffective = true; // １体でも効果があったらtrue
    }
  }

  // 効果が何もないが失敗メッセージを出していない場合はここで１度だけ出す（ほろびのうた対策）
  if( !isEffective && !m_fWazaFailMsgDisped && m_fWazaFailMsgRoundUp ){
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
  }
}

/**
 * @brief 技カテゴリ「シンプルシック」の技の効果があるかチェック
 * @param waza      使用する技
 * @param attacker  技を撃つポケモン
 * @param target    技ターゲット
 * @retval true   効果がある場合
 * @retval false  効果がない場合
 */
bool ServerFlow::scproc_checkNoEffect_SimpleSick( WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick = static_cast<WazaSick>( WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SICK ) );
  SICKCONT_PARAM contParam = WAZADATA_GetSickCont( waza );
  
  BTL_SICKCONT sickCont;
  calc::WazaSickContToBppSickCont( contParam, attacker, &sickCont );

  if( sick == pml::wazadata::WAZASICK_SPECIAL_CODE )
  {
    sick = scEvent_DecideSpecialWazaSick(  attacker, target, &m_strParam );
    if( (sick == pml::wazadata::WAZASICK_NULL) || (sick == pml::wazadata::WAZASICK_SPECIAL_CODE) )
    {
      return false;
    }
  }

  if( !scproc_AddSickCheckFail( target, attacker, sick, sickCont, SICK_CAUSE_OTHER, WAZASERIAL_NULL, SICK_OW_NONE , true, false ) )
  {
    return true;
  }
  
  return false;  
}

//--------------------------------------------------------------------------
/**
 * @brief 技カテゴリ「シンプルエフェクト」の無効化チェック
 *
 * @param[in]     wazaParam  使用する技
 * @param[in]     attacker   技を出すポケモン
 * @param[in,out] targets    技対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckNoEffect_SimpleEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  BTL_POKEPARAM* target;
  u32 wazaSerial = wazaSerial_Inc();
  u32 numTargetPoke = targets->GetCount();

  targets->SeekStart();
  while( (target = targets->SeekNext()) != NULL )
  {
    m_simpleEffectFailManager.Start();

    // まずは判定だけ行い失敗リアクションのためのコマンド生成は行わない
    bool isEffective = scproc_WazaRankEffect_Common( wazaParam, attacker, target, wazaSerial, true, true );
    SimpleEffectFailManager::Result  failResult = m_simpleEffectFailManager.GetResult();

    m_simpleEffectFailManager.End();

    if( !isEffective )
    {
      // 効果がまったく無かった場合、対象が１体ならここで失敗リアクションコマンド生成
      if( numTargetPoke == 1 ){
        putSimpleEffectFailMsg( attacker, target, wazaParam, wazaSerial, failResult );
      }
      targets->Remove( target );
    }


  }

  // 複数体いたターゲットが0になっていたらまとめて失敗メッセージを１回
  if( (numTargetPoke > 1) && targets->IsRemovedAll() ){
    scPut_WazaFail( attacker, wazaParam->wazaID );
    m_fWazaFailMsgDisped = true;
  }
}

/**
 * @brief シンプルエフェクトの失敗メッセージ出力
 * @param failResult  失敗理由コード
 */
void   ServerFlow::putSimpleEffectFailMsg( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, u32 wazaSerial, SimpleEffectFailManager::Result failResult )
{
  switch( failResult ){
  case SimpleEffectFailManager::RESULT_STD:
    scPut_WazaFail( attacker, wazaParam->wazaID );
    m_fWazaFailMsgDisped = true;
    break;

  case SimpleEffectFailManager::RESULT_NO_EFFECT:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, target->GetID() );
    m_fWazaFailMsgDisped = true;
    break;

  case SimpleEffectFailManager::RESULT_OTHER_EFFECTS:
    scproc_WazaRankEffect_Common( wazaParam, attacker, target, wazaSerial, true , false );
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief 防御系サイドエフェクトによるワザ無効化チェック：開始処理
 * @param wazaParam  使用する技のパラメータ
 * @param attacker   技を使用するポケモン
 * @param targets    技のターゲット
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_checkNoEffectBySideEffectGuard_Begin( 
  const WAZAPARAM* wazaParam, 
  BTL_POKEPARAM* attacker,
  PokeSet* targets )
{
  EVENTVAR_Push();
    const u8 targetCnt = targets->GetCount();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );

    for( u8 i=0; i<targetCnt; ++i )
    {
      const BTL_POKEPARAM* bpp = targets->Get( i );
      EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
    }

    EVENT_CallHandlers( this, BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_BEGIN );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * @brief 防御系サイドエフェクトによるワザ無効化チェック：終了処理
 * @param wazaParam  使用する技のパラメータ
 * @param attacker   技を使用するポケモン
 * @param targets    技のターゲット
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_checkNoEffectBySideEffectGuard_End( 
  const WAZAPARAM* wazaParam, 
  BTL_POKEPARAM* attacker, 
  PokeSet* targets )
{
  EVENTVAR_Push();
    const u8 targetCnt = targets->GetCount();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );

    for( u8 i=0; i<targetCnt; ++i )
    {
      const BTL_POKEPARAM* bpp = targets->Get( i );
      EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
    }

    EVENT_CallHandlers( this, BTL_EVENT_NOEFFECT_CHECK_SIDEEFF_GUARD_END );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 *  「まもる」等が成功してワザを防いだ直後の処理
 */
//----------------------------------------------------------------------------------
void ServerFlow::scproc_MamoruSuccess( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam )
{
  scPut_EffectByPokePos( target, BTLEFF_GUARD );
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Mamoru, target->GetID() );

  u32 hem_state = m_HEManager.PushState();
    scEvent_MamoruSuccess( attacker, target, wazaParam );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 *  [Event]  「まもる」等が成功してワザを防いだ直後
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_MamoruSuccess( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
    EVENT_CallHandlers( this, BTL_EVENT_MAMORU_SUCCESS );
  EVENTVAR_Pop();
}

/**
 * ワザ発動前の、対象別無効化チェック：下請け
 * @param      wazaParam
 * @param      attacker
 * @param[io]  targets        この中に無効になるポケモンがいれば除外する
 * @param      affRec
 * @param      eventID
 * @param      bCheckMustHit 攻撃ポケモンの必中状態をチェックするか？
 */
void ServerFlow::scproc_checkNoEffect_sub( const WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec* affRec,
  BtlEventType eventID,  bool bCheckMustHit )
{
  BTL_POKEPARAM* bpp;

  targets->SeekStart();
  while( (bpp = targets->SeekNext()) != NULL )
  {
    if( bCheckMustHit && IsMustHit(attacker, bpp) )
    {
      continue;
    }

    if( scproc_checkNoEffect_core(wazaParam, attacker, bpp, affRec, eventID) )
    {
      targets->Remove( bpp );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ワザ無効チェック＆原因表示（イベント使用）
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   target
 * @param   eventID
 *
 * @retval  bool    無効だったらtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_checkNoEffect_core( const WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, const DmgAffRec* affRec, BtlEventType eventID )
{
  u32 hem_state = m_HEManager.PushState();

  bool fNoReaction = false;
  bool fNoEffect;

  fNoEffect = scEvent_CheckNotEffect( wazaParam, eventID, attacker, target, affRec, &m_strParam, &fNoReaction );
  if( fNoEffect )
  {
    // 文字を出すだけのリアクション
    if( HANDEX_STR_IsEnable(&m_strParam) ){
      handexSub_putString( &m_strParam );
      HANDEX_STR_Clear( &m_strParam );
      m_fWazaFailMsgDisped = fNoReaction;
    }
    else{
      // HandExを利用したリアクション
      if( !scproc_HandEx_Result() )
      {
        if( !fNoReaction )
        {
          // 何もリアクションなければ「効果がないようだ…」
          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, target->GetID() );
          m_fWazaFailMsgDisped = true;
        }
      }
      else
      {
        BTL_PRINT(" ノーリアクションフラグ=%d\n", fNoReaction );
        if( !fNoReaction ){
          m_fWazaFailMsgDisped = true;
        }
      }
    }
  }

  m_HEManager.PopState( hem_state );
  return fNoEffect;
}

//----------------------------------------------------------------------------------
/**
 * @brief ワザ無効チェック＆原因表示（タイプごとの特殊処理：ＸＹからの仕様）
 *
 * @param wazaParam  使用する技
 * @param attacker   攻撃するポケモン
 * @param target     技を受けるポケモン
 *
 * @retval true   技は無効
 * @retval false  技は有効
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_checkNoEffect_typeSp( 
  const WAZAPARAM*     wazaParam,
  const BTL_POKEPARAM* attacker, 
  const BTL_POKEPARAM* target )
{
  if( ( target->IsMatchType( POKETYPE_KUSA ) ) &&
      ( tables::IsBoujinGuardWaza( wazaParam->wazaID ) ) ) 
  {
    return true;
  }

  if( target->IsMatchType(POKETYPE_GHOST) )
  {
    if( ( wazaParam->wazaID == WAZANO_TOOSENBOU ) || 
        ( wazaParam->wazaID == WAZANO_KUROIMANAZASI ) ) 
    {
      return true;
    }
  }

  if( ( target->IsMatchType( POKETYPE_KOORI ) ) && 
      ( wazaParam->wazaID == WAZANO_ZETTAIREIDO ) ) 
  {
    return true;
  }

  {
    const u8 attackerID = attacker->GetID();
    const u8 targetID = target->GetID();
    if(  ( target->IsMatchType( POKETYPE_AKU ) ) &&
         ( wazaParam->fItazuraGokoro ) &&
        !( m_mainModule->IsFriendPokeID( attackerID, targetID ) ) )   // @fix NMCat[17] 「いたずらごころ」所持の悪タイプが、自分対象の変化技を使用すると失敗してしまう)
    {
      return true;
    }
  }

  return false;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ無効チェック（とくせい等、個別ケースによる無効化）
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   customMsg   無効化した場合に特定のメッセージを出力する場合のパラメータ
 *
 * @retval  bool    無効な場合true
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckNotEffect( const WAZAPARAM* wazaParam, BtlEventType eventID,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const DmgAffRec* affRec, BTL_HANDEX_STR_PARAMS* customMsg, bool* fNoReaction )
{
  bool fNoEffect = false;
  BtlTypeAff  dmgAff = affRec->GetIfEnable( defender->GetID() );

  HANDEX_STR_Clear( customMsg );

  BTL_PRINT("[svf] checkNotEffect atkID=%d, defID=%d, wazaID=%d\n",
          attacker->GetID(), defender->GetID(), wazaParam->wazaID );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK,   attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF,   defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID,       wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE,    wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_PRI,     wazaParam->wazaPri );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_TYPE,  wazaParam->targetType );
    EVENTVAR_SetConstValue( BTL_EVAR_MAGICCOAT_FLAG, wazaParam->fMagicCoat );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, static_cast<int>((uptr)customMsg) );
    EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, dmgAff );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG, ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL ) );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, eventID );
    fNoEffect = EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
    (*fNoReaction) = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return fNoEffect;
}
//--------------------------------------------------------------------------
/**
 *  「そらをとぶ」など画面から一時的に消えている状態で外れるケースをチェック
 *  命中しなかった場合は targets から除外
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_CheckPokeHideAvoid( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, PokeSet* targets )
{
  BTL_POKEPARAM* defender;

  targets->SeekStart();
  while( (defender = targets->SeekNext()) != NULL )
  {
    if( IsMustHit(attacker, defender) ){
      continue;
    }
    bool bEnableAvoidMsg = true;
    if( scEvent_CheckPokeHideAvoid(attacker, defender, wazaParam->wazaID, &bEnableAvoidMsg) )
    {
      targets->Remove( defender );
      if( bEnableAvoidMsg ){
        scPut_WazaAvoid( defender, wazaParam->wazaID );
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * ワザ命中判定 -> 命中しなかった場合は targets から除外
 *
 * @param      wk
 * @param[in]  attacker       ワザを出すポケモン
 * @param[io]  targets        ワザ対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 * @param[out] actionRecorder 技を出すポケモンの行動結果の記録先( 不要なら NULL )
 *
 */
//--------------------------------------------------------------------------
void ServerFlow::flowsub_checkWazaAvoid(
  const WAZAPARAM* wazaParam, 
  const BTL_POKEPARAM* attacker, 
  PokeSet* targets, 
  ActionRecorder* actionRecorder )
{
  BTL_POKEPARAM* bpp;

  if( targets->GetCount() <= 0 )
  {
    return;
  }

  // 対象が最初からワザを打ったポケモン１体のワザは命中率などによる判定を行わない
  if( (targets->GetCountMax() == 1)
  &&  (targets->Get(0) == attacker)
  ){
    return;
  }

  bool bFriendshipActive;
  targets->SeekStart();
  while( (bpp = targets->SeekNext()) != NULL )
  {
    if( scEvent_SkipAvoidCheck(attacker, bpp, wazaParam) ){
      continue;
    }
    if( !scEvent_CheckHit(attacker, bpp, wazaParam, &bFriendshipActive) )
    {
      targets->Remove( bpp );
      if( bFriendshipActive ){
        scPut_WazaAvoidByFriendship( bpp, wazaParam->wazaID );
      }else{
        scPut_WazaAvoid( bpp, wazaParam->wazaID );
      }
    }
  }

  // 技ターゲットが居なくなったら、命中判定で失敗したことを記録
  if( ( actionRecorder != NULL ) &&
      ( targets->IsRemovedAll() ) )
  {
    actionRecorder->SetAction( attacker->GetID(), ActionRecorder::ACTION_WAZA_FAILED_HIT_PERCENTAGE );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ命中判定をスキップするかチェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 *
 * @retval  bool    スキップする場合はtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_SkipAvoidCheck( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  bool fSkip = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_SKIP_AVOID_CHECK );
    fSkip = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return fSkip;
}

//--------------------------------------------------------------------------
/**
 * [Event] 出したワザが対象に当たるか判定（一撃必殺以外のポケモン対象ワザ）
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param[out] bFriendshipActive  なかよし度による補正が掛かったかを返す( 不要なら NULL )
 *
 * @retval  bool  当たる場合はtrue
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, bool* bFriendshipActive )
{
  if( bFriendshipActive ){
    *bFriendshipActive = false;
  }

  if( IsMustHit(attacker, defender) ){
    return true;
  }

  if( scEvent_IsCalcHitCancel(attacker, defender, wazaParam->wazaID) ){
    return true;
  }

  if( defender->CheckSick(pml::wazadata::WAZASICK_TELEKINESIS) ){
    return true;
  }

  {
    u8 fAvoidFlat;
    s8 hitRank, avoidRank, totalRank;
    u32 totalPer, wazaHitPer;
    fx32 ratio;

    wazaHitPer = scEvent_getHitPer( BTL_EVENT_WAZA_HIT_RATIO, attacker, defender, wazaParam );

    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, false );
      EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, attacker->GetValue(BTL_POKEPARAM::BPP_HIT_RATIO) );
      EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, defender->GetValue(BTL_POKEPARAM::BPP_AVOID_RATIO) );
      EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
      EVENT_CallHandlers( this, BTL_EVENT_WAZA_HIT_RANK );

      hitRank = EVENTVAR_GetValue( BTL_EVAR_HIT_RANK );
      avoidRank = EVENTVAR_GetValue( BTL_EVAR_AVOID_RANK );
      fAvoidFlat = EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
      // 「みやぶる」をされたポケモンは回避率上昇を無視
      if( ((defender->CheckSick(pml::wazadata::WAZASICK_MIYABURU)) && (avoidRank > BTL_CALC_HITRATIO_MID))
      ||  (fAvoidFlat)
      ){
        avoidRank = BTL_CALC_HITRATIO_MID;
      }

      if( attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_HITRATIO_UP) ){
        EVENTVAR_MulValue( BTL_EVAR_RATIO, FX32_CONST(1.2) );
      }

      ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );

    EVENTVAR_Pop();

    totalRank = roundValue( (int)(BTL_CALC_HITRATIO_MID + hitRank - avoidRank), BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );
    totalPer  = calc::HitPer( wazaHitPer, totalRank );

    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo1, attacker->GetID(), hitRank, wazaHitPer );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo2, defender->GetID(), avoidRank );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo3, totalRank, totalPer, ratio );

    totalPer = calc::MulRatio( totalPer, ratio );
    if( totalPer > 100 ){
      totalPer = 100;
    } 

    // なかよし度による補正
    if( getFriendship(defender) >= 200 )
    {
      totalPer = (totalPer > 10)? (totalPer - 10) : 0;
      if( bFriendshipActive ){
        *bFriendshipActive = true;
      }
    }

    // デバッグで強制的に100にする
    if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_HIT100PER) ){
      totalPer = 100;
    }
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo4, totalPer);

    return perOccur( totalPer );
  }
}
//----------------------------------------------------------------------------------
/**
 * ワザ効果あることが確定→必要な演出コマンドの生成
 *
 * @param   wk
 * @param   waza
 * @param   defPokeID
 * @param   que_reserve_pos
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_WazaEffect( WazaID waza, WAZAEFF_CTRL* effCtrl, const WAZAEFF_RESERVE_POS& que_reserve_pos, bool bSyndDamageEffect, ZenryokuWazaKind zWazaKind )
{
  WazaID  effWazaID = wazaEffCtrl_GetEffectWazaID( effCtrl );
  if( effWazaID == WAZANO_NULL ){
    effWazaID = waza;
    BTL_PRINT("ワザエフェクト記録なし, 書き換え\n");
  }
  BTL_PRINT("エフェクト用ワザid=%d\n", effWazaID);

  if( wazaEffCtrl_IsValidSubEffectParams(effCtrl) ){
    scPut_WazaSubEffect( effCtrl, que_reserve_pos.prevEffectCmd );
    wazaEffCtrl_ClearSubEffectParams( effCtrl );
  }

  if( que_reserve_pos.mainEffectCmd != WAZAEFF_RESERVE_POS::INVALID_VALUE ){
    scmd::SCQUE_PUT_ReservedPos( m_que, que_reserve_pos.mainEffectCmd, SC_ACT_WAZA_EFFECT,
          effCtrl->attackerPos, effCtrl->targetPos, effWazaID, effCtrl->effectIndex, bSyndDamageEffect, zWazaKind );
  }

  effCtrl->effectIndex = BTLV_WAZAEFF_INDEX_DEFAULT;
  effCtrl->fDone = true;
}
/**
 * ワザ演出直前のサブ演出（きのみ）コマンド作成
 *
 * @param effCtrl
 */
void  ServerFlow::scPut_WazaSubEffect( const WAZAEFF_CTRL* effCtrl, u16 reservedQuePos )
{
  scmd::SCQUE_PUT_ReservedPos( m_que, reservedQuePos, SC_ACT_KINOMI_PRE_WAZA,
          effCtrl->subEff.pokeCnt, effCtrl->subEff.pokeID_1, effCtrl->subEff.pokeID_2,
          effCtrl->subEff.pokeID_3, effCtrl->subEff.pokeID_4, effCtrl->subEff.pokeID_5 );
}

//----------------------------------------------------------------------------------
/**
 * 必要なら、１ターン溜めワザの溜めターン処理を行う。
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  bool      溜めターン処理を行った場合true
 */
//----------------------------------------------------------------------------------
ServerFlow::TameWazaResult  ServerFlow::scproc_Fight_TameWazaExe( BTL_POKEPARAM* attacker, const PokeSet* targetRec, const WAZAPARAM* wazaParam )
{
  if( !attacker->CheckSick( pml::wazadata::WAZASICK_TAMELOCK) )
  {
    BtlPokePos  atPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, attacker->GetID() );
    WazaID  waza = wazaParam->wazaID;

    // 溜めターン失敗判定
    if( scEvent_CheckTameFail(attacker, targetRec) )
    {
      scPut_WazaFail( attacker, waza );
      return TAMEWAZA_START_FAIL;
    }

    // 溜めターンスキップ判定
    if( !scEvent_CheckTameTurnSkip(attacker, waza) )
    {
      if( scproc_TameStartTurn(attacker, atPos, targetRec, wazaParam) )
      {
        BTL_SICKCONT  sickCont = SICKCONT_MakeTurnParam( attacker->GetID(), 2, waza );
        scPut_AddSick( attacker, pml::wazadata::WAZASICK_TAMELOCK, sickCont );
      }
      // スキップしない時はここでreturn
      return TAMEWAZA_START_OK;
    }

    if( !scproc_TameStartTurn(attacker, atPos, targetRec, wazaParam) ){
      return TAMEWAZA_START_FAIL;
    }
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_TameSkip( attacker, waza );
      m_HEManager.PopState( hem_state );
    }
  }

  {
    bool fSucceed;
    wazaEffCtrl_SetEffectIndex( m_wazaEffCtrl, BTLV_WAZAEFF_TAME_RELEASE );
    {
      u32 hem_state = m_HEManager.PushState();
      fSucceed = scEvent_TameRelease( attacker, targetRec, wazaParam->wazaID );
      m_HEManager.PopState( hem_state );
    }
    attacker->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_TAMEHIDE_OFF );
    scproc_TameLockClear( attacker );

    return (fSucceed)? TAMEWAZA_RELEASE_OK : TAMEWAZA_RELEASE_FAIL;
  }
}
//----------------------------------------------------------------------------------
/**
 * 溜め開始ターン処理
 *
 * @param   wk
 * @param   attacker
 * @param   atPos
 * @param   targetRec
 * @param   waza
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_TameStartTurn( BTL_POKEPARAM* attacker, BtlPokePos atPos, const PokeSet* targetRec, const WAZAPARAM* wazaParam )
{
  bool fSuccess, fFailMsgDisped = false;
  u8 hideTargetPokeID = BTL_POKEID_NULL;

  u32 hem_state = m_HEManager.PushState();
  fSuccess = scEvent_TameStart( attacker, targetRec, wazaParam, &hideTargetPokeID, &fFailMsgDisped );

  if( fSuccess )
  {
    BtlPokePos targetPos = m_defaultTargetPos;
    if( targetRec->GetCount() )
    {
      BTL_POKEPARAM* target = targetRec->Get( 0 );
      GFL_ASSERT_STOP(target);
      targetPos = m_posPoke.GetPokeExistPos( target->GetID() );
    }
    SCQUE_PUT_ACT_WazaEffect( m_que, atPos, targetPos, wazaParam->wazaID, BTLV_WAZAEFF_TAME_START, false, wazaParam->zenryokuWazaKind );
  }
  m_HEManager.PopState( hem_state );

  if( fSuccess )
  {
    hem_state = m_HEManager.PushState();
    scEvent_TameStartFixed( attacker );
    m_HEManager.PopState( hem_state );

    if( attacker->IsWazaHide() ){
      SCQUE_PUT_ACT_TameWazaHide( m_que, attacker->GetID(), true );
    }
    if( hideTargetPokeID != BTL_POKEID_NULL ){
      SCQUE_PUT_ACT_TameWazaHide( m_que, hideTargetPokeID, true );
    }
  }
  else
  {
    if( !fFailMsgDisped ){
      scPut_WazaFail( attacker, wazaParam->wazaID );
    }
  }

  return fSuccess;
}
void  ServerFlow::scproc_TameLockClear( BTL_POKEPARAM* attacker )
{
  if( attacker->CheckSick( pml::wazadata::WAZASICK_TAMELOCK) )
  {
    scPut_CureSick( attacker, pml::wazadata::WAZASICK_TAMELOCK, NULL );
    attacker->TURNFLAG_Set(  BTL_POKEPARAM::TURNFLG_TAMEHIDE_OFF );
  }

  {
    BTL_POKEPARAM::ContFlag contFlag = attacker->CONTFLAG_CheckWazaHide();
    while( contFlag != BTL_POKEPARAM::CONTFLG_NULL )
    {
      scPut_ResetContFlag( attacker, contFlag );
      contFlag = attacker->CONTFLAG_CheckWazaHide();
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * フリーフォール溜めターン開始処理
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_FreeFall_Start( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, bool* fFailMsgDisped )
{
  *fFailMsgDisped = false;

  // 対象が死んでる・みがわり・ワザ効果で場から消えてる場合は失敗
  if( (!target->IsDead())
  &&  (!target->MIGAWARI_IsExist())
  &&  (!target->IsWazaHide())
  ){
    // 重すぎるポケは失敗
    if( Hnd_GetWeight(target->GetID()) >= 2000 ){
      scPut_Message_Set( target, BTL_STRID_SET_FreeFallFail );
      *fFailMsgDisped = true;
      return false;
    }

    // 守っていたら失敗
    if( target->TURNFLAG_Get(BTL_POKEPARAM::TURNFLG_MAMORU) ){
      scproc_MamoruSuccess( attacker, target, wazaParam );
      *fFailMsgDisped = true;
      return false;
    }

    // 特殊要因で失敗（たたみがえしなど）
    if( scproc_FreeFall_Start_CheckGuard(attacker, target, wazaParam) ){
      *fFailMsgDisped = true;
      return false;
    }

    // ここまで来たらば成功
    {
      BTL_SICKCONT  cont;
      u8 counterValue = BTL_POKEPARAM::PokeIDtoFreeFallCounter( target->GetID() );

      scPut_SetContFlag( attacker, BTL_POKEPARAM::CONTFLG_SORAWOTOBU );
      scPut_SetContFlag( target, BTL_POKEPARAM::CONTFLG_SORAWOTOBU );
      scPut_SetBppCounter( attacker, BTL_POKEPARAM::COUNTER_FREEFALL, counterValue );

      cont = SICKCONT_MakePoke( attacker->GetID(), attacker->GetID() );
      scproc_AddSickCore( target, attacker, pml::wazadata::WAZASICK_FREEFALL, cont, false, false, NULL );
      return true;
    }
  }
  return false;
}
/**
 *  フリーフォールの溜めターンがガードされるか判定
 */
bool ServerFlow::scproc_FreeFall_Start_CheckGuard( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam )
{
  u32 hem_state = m_HEManager.PushState();

  HANDEX_STR_Clear( &m_strParam );

  bool bGuard = scEvent_Check_FreeFallStart_Guard( attacker, target, wazaParam, &m_strParam );
  if( bGuard )
  {
    // HandExを利用したリアクション
    //scproc_HandEx_Result();
    handexSub_putString( &m_strParam );
    HANDEX_STR_Clear( &m_strParam );
  }

  m_HEManager.PopState( hem_state );

  return bGuard;
}
/**
 *【Event】フリーフォールの溜めターンをガードできるかチェック
 */
bool ServerFlow::scEvent_Check_FreeFallStart_Guard( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, BTL_HANDEX_STR_PARAMS* strParam )
{
  bool bGuard;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID,     wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE,  wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_PRI,   wazaParam->wazaPri );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(strParam) );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, false );

    /*
     *  書き換えられても何もしないが BTL_EVENT_NOEFFECT_CHECK_MAMORU とフォーマットを同一にして
     *  共通のハンドラによる実装が可能なようにしておく（たたみがえし対応）
     */
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );

    EVENT_CallHandlers( this, BTL_EVENT_FREEFALL_START_GUARD );
    bGuard = EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
  EVENTVAR_Pop();

  return bGuard;
}
/**
 * 溜め解放ターンに何らかの理由でワザを失敗した時の後始末
 */
void ServerFlow::scEvent_TameReleaseFailed( BTL_POKEPARAM* attacker, PokeSet* targetRec, const WAZAPARAM* wazaParam )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID,     wazaParam->wazaID );
    EVENT_CallHandlers( this, BTL_EVENT_TAMERELEASE_FAILED );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * フリーフォールでポケモンをつかんだ状態ならリリースする処理
 *
 * @param   bpp            フリーフォール使用中のポケモンパラメータ
 * @param   bAppearSelf    使用中ポケの姿を再表示する
 * @param   bAppearTarget  捕まれている側のポケモンの姿を再表示する
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_FreeFall_CheckRelease( BTL_POKEPARAM* bpp, bool bAppearSelf, bool bAppearTarget )
{
  u8 capturedPokeID = BTL_POKEPARAM::FreeFallCounterToPokeID( bpp->COUNTER_Get(BTL_POKEPARAM::COUNTER_FREEFALL) );
  if( capturedPokeID != BTL_POKEID_NULL )
  {
    BTL_POKEPARAM* capturedBpp = m_pokeCon->GetPokeParam( capturedPokeID );

    if( capturedBpp->CheckSick( pml::wazadata::WAZASICK_FREEFALL) )
    {
      scPut_CureSick( capturedBpp, pml::wazadata::WAZASICK_FREEFALL, NULL );
      scPut_ResetContFlag( capturedBpp, BTL_POKEPARAM::CONTFLG_SORAWOTOBU );

      if( bAppearTarget ){
        this->Hnd_FreeFallAppearTarget( capturedPokeID );
      }
    }

    scPut_SetBppCounter( bpp, BTL_POKEPARAM::COUNTER_FREEFALL, 0 );
    if( bAppearSelf )
    {
      if( !bpp->IsDead() ){
        SCQUE_PUT_ACT_TameWazaHide( m_que, bpp->GetID(), false );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * @brief ワザ出し失敗チェック
 *
 * @param   attacker       技を使用するポケモン
 * @param   wazaParam      使用する技のパラメータ
 * @param   fWazaLock      技ロック状態か？
 * @param   targets        技ターゲット
 * @param   actionDesc     アクションの詳細情報
 *
 * @retval  SV_WazaFailCause    失敗した場合失敗コード／成功の場合 SV_WAZAFAIL_NULL
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_Fight_CheckWazaExecuteFail_1st( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u8 fWazaLock, const PokeSet* targets, const ActionDesc* actionDesc )
{
  bool             isZenryokuWaza = ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
  SV_WazaFailCause cause = scproc_Fight_CheckWazaExecuteFail_1st_sub( attacker, wazaParam, fWazaLock, targets, actionDesc );
  PokeSick         sick  = attacker->GetPokeSick();

  if( cause != SV_WAZAFAIL_NULL )
  {
    BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_1, attacker->GetID(), cause);
    scproc_WazaExecuteFailed( attacker, wazaParam->wazaID, isZenryokuWaza, cause );
    return true;
  }
  else
  {
    // 眠ってるのに失敗せず->眠りエフェクトだけ表示する
    if( sick == pml::pokepara::SICK_NEMURI )
    {
      scPut_WazaExecuteFailMsg(  attacker, wazaParam->wazaID, isZenryokuWaza, SV_WAZAFAIL_NEMURI );
      scPut_EffectByPokePos( attacker, BTLEFF_NEMURI );
    }
    // ワザでこおりが溶ける
    else if( sick == pml::pokepara::SICK_KOORI )
    {
      bool fWazaMelt = WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_KooriMelt );
      if( fWazaMelt ) {
        scPut_CurePokeSick( attacker, static_cast<WazaSick>(sick), false );
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KoriMeltWaza, attacker->GetID(), wazaParam->wazaID );
      }
    }
  }
  return false;
}

/**
 * @brief ワザ出し失敗チェック
 *
 * @param   attacker       技を使用するポケモン
 * @param   wazaParam      使用する技のパラメータ
 * @param   fWazaLock      技ロック状態か？
 * @param   targets        技ターゲット
 * @param   actionDesc     アクションの詳細情報
 *
 * @retval  SV_WazaFailCause    失敗した場合失敗コード／成功の場合 SV_WAZAFAIL_NULL
 */
ServerFlow::SV_WazaFailCause ServerFlow::scproc_Fight_CheckWazaExecuteFail_1st_sub( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u8 fWazaLock, const PokeSet* targets, const ActionDesc* actionDesc )
{
  bool isZenryokuWaza = ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
  PokeSick sick = pml::pokepara::SICK_NULL;

  // ねむりの解除チェック
  scproc_CheckWazaExe_NemuriCure( attacker, wazaParam->wazaID );
  sick = attacker->GetPokeSick();
  if( sick == pml::pokepara::SICK_NEMURI )
  {
    if( !scEvent_ExeFailThrew( attacker, wazaParam->wazaID, SV_WAZAFAIL_NEMURI) ){
      return SV_WAZAFAIL_NEMURI;
    }
  }
  // こおりの解除チェック
  bool fWazaMelt = scproc_CheckWazaExe_KooriCure( attacker, wazaParam->wazaID );
  sick = attacker->GetPokeSick();
  if( (sick == pml::pokepara::SICK_KOORI) && (!fWazaMelt) ){
    return SV_WAZAFAIL_KOORI;
  }

  // 命令無視判定
  if( m_currentSabotageType == SABOTAGE_DONT_ANY ) {
    return SV_WAZAFAIL_SABOTAGE;
  }
  if( m_currentSabotageType == SABOTAGE_GO_SLEEP ) {
    return SV_WAZAFAIL_SABOTAGE_GO_SLEEP;
  }

  // PPがゼロ
  if( !fWazaLock && !actionDesc->isOdorikoReaction )
  {
    u8 wazaIdx = attacker->WAZA_SearchIdx( wazaParam->orgWazaID );
    if( (wazaIdx != PTL_WAZA_MAX)
    &&  (attacker->WAZA_GetPP(wazaIdx) == 0)
    ){
      return SV_WAZAFAIL_PP_ZERO;
    }
  }

  // スカイバトル判定
  if( isSkyBattleFailWaza(wazaParam->wazaID) ){
    return SV_WAZAFAIL_SKYBATTLE;
  }

  // イベント失敗チェックレベル１
  SV_WazaFailCause cause = scEvent_CheckWazaExecute(  attacker, wazaParam->wazaID, BTL_EVENT_WAZA_EXECUTE_CHECK_1ST , wazaParam, targets );
  if( cause != SV_WAZAFAIL_NULL ){ 
    return cause; 
  }

  // 集中切れ（きあいパンチ失敗）による失敗チェック
  if( attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_KIAI_SHRINK) &&
      ( wazaParam->wazaID == WAZANO_KIAIPANTI ) )
  {
    return SV_WAZAFAIL_KIAI_SHRINK;
  }

  // ひるみによる失敗チェック
  if( attacker->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_SHRINK) ){
    return SV_WAZAFAIL_SHRINK;
  }

  // かなしばり判定
  if( !isZenryokuWaza &&
       attacker->CheckSick( pml::wazadata::WAZASICK_KANASIBARI) )
  {
    if( wazaParam->wazaID != WAZANO_WARUAGAKI )
    {
      BTL_SICKCONT  sickCont = attacker->GetSickCont( pml::wazadata::WAZASICK_KANASIBARI );
      if( SICKCONT_GetParam(sickCont) == wazaParam->wazaID ){
        return SV_WAZAFAIL_KANASIBARI;
      }
    }
  }

  // かいふくふうじチェック
  if( !isZenryokuWaza
  &&   attacker->CheckSick( pml::wazadata::WAZASICK_KAIHUKUHUUJI)
  &&  (WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_KaifukuHuuji))
  ){
    return SV_WAZAFAIL_KAIHUKUHUUJI;
  }

  // じごくづきチェック
  if( attacker->CheckSick( pml::wazadata::WAZASICK_ZIGOKUDUKI ) &&
      WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_Sound ) &&
      !isZenryokuWaza )
  {
    return SV_WAZAFAIL_ZIGOKUDUKI;
  }

  // じゅうりょくチェック
  if( m_fieldStatus->CheckEffect(FieldStatus::EFF_JURYOKU)
  &&  (WAZADATA_GetFlag(wazaParam->wazaID, pml::wazadata::WAZAFLAG_Flying))
  && !isZenryokuWaza
  ){
    return SV_WAZAFAIL_JURYOKU;
  }

  // わるあがき以外をチェック
  if( wazaParam->wazaID != WAZANO_WARUAGAKI )
  {
    // アンコールによる失敗チェック
    if( !isZenryokuWaza &&
         attacker->CheckSick( pml::wazadata::WAZASICK_ENCORE) )
    {
      WazaID prevWaza = static_cast<WazaNo>(SICKCONT_GetParam( attacker->GetSickCont(pml::wazadata::WAZASICK_ENCORE) ));
      if( (prevWaza != WAZANO_WARUAGAKI)
      &&  (prevWaza != wazaParam->wazaID )
      ){
        return SV_WAZAFAIL_WAZALOCK;
      }
    }
    // ワザロックによる失敗チェック
    if( (attacker->CheckSick(pml::wazadata::WAZASICK_WAZALOCK))
    &&  (attacker->GetPrevWazaID() != wazaParam->wazaID)
    ){
      return SV_WAZAFAIL_WAZALOCK;
    }
    // こだわりロックによる失敗チェック
    if( isWazaFailByKodawariLock(attacker, wazaParam->wazaID) ){
      return SV_WAZAFAIL_WAZALOCK;
    }

    // ちょうはつによる失敗チェック
    if( attacker->CheckSick( pml::wazadata::WAZASICK_TYOUHATSU)
    &&  !isZenryokuWaza
    &&  !WAZADATA_IsDamage(wazaParam->wazaID)
    ){
      return SV_WAZAFAIL_TYOUHATSU;
    }
    // ふういんによる失敗チェック
    if( !isZenryokuWaza &&
         m_fieldStatus->CheckEffect(FieldStatus::EFF_FUIN) )
    {
      if( m_fieldStatus->CheckFuin(*m_mainModule, m_pokeCon, attacker, wazaParam->wazaID) )
      {
        return SV_WAZAFAIL_FUUIN;
      }
    }
  }


  // こんらん自爆判定
  if( scproc_Fight_CheckConf(attacker) ){
    return SV_WAZAFAIL_KONRAN;
  }

  // マヒ判定
  if( sick == pml::pokepara::SICK_MAHI ){
    if( calc::IsOccurPer(BTL_MAHI_EXE_PER) ){
      return SV_WAZAFAIL_MAHI;
    }
  }
  // メロメロ判定
  if( scproc_Fight_CheckMeroMero( attacker) ){
    return SV_WAZAFAIL_MEROMERO;
  }

  return SV_WAZAFAIL_NULL;
}
/**
 * こだわりロックにより失敗するかどうか判定
 * @param attacker  ワザを出そうとしているポケモン
 * @param waza      出そうとしているワザID
 * @return  失敗する場合は true 
 */
bool ServerFlow::isWazaFailByKodawariLock( const BTL_POKEPARAM* attacker, WazaID waza ) const
{
  if( waza == WAZANO_WARUAGAKI ){
    return false;
  }
  // @fix MMCat[132] こだわり系アイテムを持たせて「マジックルーム」を使用すると、こだわり状態が解除されない
  if (m_fieldStatus->CheckEffect(FieldStatus::EFF_MAGICROOM))
  {
    return false;
  }
  // @fix MMCat[132]の類似案件 さしおさえ状態なら失敗しない
  if (attacker->CheckSick(pml::wazadata::WAZASICK_SASIOSAE)){
    return false;
  }
  if( !attacker->CheckSick(pml::wazadata::WAZASICK_KODAWARI) ){
    return false;
  }
  if( attacker->GetKodawariWazaID() == waza ){
    return false;
  }

  return true;
}

/**
 * スカイバトルで失敗するケースの判定
 * @param waza
 * @return  スカイバトル中＆スカイバトル失敗ワザ、という場合はtrue
 */
bool ServerFlow::isSkyBattleFailWaza( WazaID waza )
{
  if( m_mainModule->IsSkyBattle() )
  {
    if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_SkyBattleFail) )
    {
      return true;
    }
  }
  return false;
}
/**
 * ワザ出し失敗判定（ふんじん）
 */
bool  ServerFlow::scproc_Fight_CheckWazaExecuteFail_Funjin( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam )
{
  if( (attacker->CheckSick(pml::wazadata::WAZASICK_FUNJIN))
  &&  (wazaParam->wazaType == POKETYPE_HONOO)
  ){
    u32 maxHP = attacker->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
    u32 damage = roundMin( calc::MulRatioInt(maxHP, 25), 1 );

    scPut_EffectByPokePos( attacker, BTLEFF_FUNJIN );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FunjinBakuhatsu, wazaParam->wazaID );

    if( scproc_SimpleDamage_CheckEnable(attacker, damage, DAMAGE_CAUSE_OTHER) )
    {
      const BTL_SICKCONT& sickcont = attacker->GetSickCont( pml::wazadata::WAZASICK_FUNJIN );
      u8 funjinCausePokeID = SICKCONT_GetCausePokeID( sickcont );
      scproc_SimpleDamage_Core( attacker, damage, DAMAGE_CAUSE_SICK_FUNJIN, funjinCausePokeID, NULL, true );
      scproc_CheckDeadCmd( attacker, false );
    }

    return true;
  }
  return false;
}
/**
 * ワザ出し失敗判定２（ワザメッセージ後・ふんじん前）
 * @param targets  技のターゲット
 */
bool  ServerFlow::scproc_Fight_CheckWazaExecuteFail_2nd( BTL_POKEPARAM* attacker, WazaID waza , const WAZAPARAM* wazaParam, const PokeSet* targets )
{
  if( waza != WAZANO_WARUAGAKI )
  {
    SV_WazaFailCause  cause = scEvent_CheckWazaExecute( attacker, waza, BTL_EVENT_WAZA_EXECUTE_CHECK_2ND , wazaParam, targets );

    if( cause != SV_WAZAFAIL_NULL )
    {
      BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_2, attacker->GetID(), cause);
      bool isZenryokuWaza = ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
      scproc_WazaExecuteFailed( attacker, waza, isZenryokuWaza, cause );
      return true;
    }
  }

  return false;
}
/**
 * ワザ出し失敗判定３（ワザメッセージ後・ふんじん後）
 * @param targets  技のターゲット
 */
bool  ServerFlow::scproc_Fight_CheckWazaExecuteFail_3rd( BTL_POKEPARAM* attacker, WazaID waza , const WAZAPARAM* wazaParam, const PokeSet* targets )
{
  if( waza != WAZANO_WARUAGAKI )
  {
    SV_WazaFailCause  cause = scEvent_CheckWazaExecute( attacker, waza, BTL_EVENT_WAZA_EXECUTE_CHECK_3RD , wazaParam, targets );

    if( cause != SV_WAZAFAIL_NULL )
    {
      BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_2, attacker->GetID(), cause);
      bool isZenryokuWaza = ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
      scproc_WazaExecuteFailed( attacker, waza, isZenryokuWaza, cause );
      return true;
    }
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * [Event]  特定ワザ出し失敗要因を無視するか判定
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 * @param   waza
 *
 * @retval  bool    無視する場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_ExeFailThrew( const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  bool fThrew = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_FAIL_CAUSE, cause );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_FAIL_THREW );
    fThrew = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  return fThrew;
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザ出し失敗判定
 *
 * @param   wk
 * @param   attacker    攻撃ポケモンパラメータ
 * @param   waza        出そうとしているワザ
 * @param   targets     技ターゲット( ターゲットが確定していないなら NULL )
 *
 * @retval  SV_WazaFailCause
 */
//--------------------------------------------------------------------------
ServerFlow::SV_WazaFailCause  ServerFlow::scEvent_CheckWazaExecute( BTL_POKEPARAM* attacker, WazaID waza, BtlEventType eventID, const WAZAPARAM* wazaParam, const PokeSet* targets )
{
  SV_WazaFailCause cause ;

  int wazaPri = ( wazaParam == NULL ) ? ( 0 ) : ( wazaParam->wazaPri );
  ZenryokuWazaKind zWazaKind = ( wazaParam == NULL ) ? ( pml::waza::ZENRYOKUWAZA_KIND_NULL ) : ( wazaParam->zenryokuWazaKind );
  WazaDamageType damageType = ( wazaParam == NULL ) ? ( WAZADATA_GetDamageType( waza ) ) : ( wazaParam->damageType );
  const u8 targetCnt = ( targets == NULL ) ? ( 0 ) : ( targets->GetCount() );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_KIND, zWazaKind );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_PRI, wazaPri );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, damageType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NULL );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    for( u8 i=0; i<targetCnt; ++i )
    {
      const BTL_POKEPARAM* bpp = targets->Get( i );
      EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
    }
    if( wazaParam )
    {
      EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    }
    EVENT_CallHandlers( this, eventID );
    cause = static_cast<SV_WazaFailCause>(EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE ));
  EVENTVAR_Pop();

  return cause;
}
//----------------------------------------------------------------------------------
/**
 * こんらん自爆チェック＆処理
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  bool    自爆した場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_Fight_CheckConf( BTL_POKEPARAM* attacker )
{
  if( m_currentSabotageType == SABOTAGE_CONFUSE ){
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Sabotage1, attacker->GetID() );
    return true;
  }

  if( attacker->CheckSick(pml::wazadata::WAZASICK_KONRAN) )
  {
    if( attacker->CheckKonranWakeUp() ){
      scPut_CureSick( attacker, pml::wazadata::WAZASICK_KONRAN, NULL );
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KonranCure, attacker->GetID() );
      return false;
    }
    scPut_ConfCheck( attacker );
    return calc::IsOccurPer( BTL_CONF_EXE_RATIO );
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * メロメロによるワザだし失敗判定
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  bool    失敗した場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_Fight_CheckMeroMero( BTL_POKEPARAM* attacker )
{
  if( attacker->CheckSick(pml::wazadata::WAZASICK_MEROMERO) )
  {
    scPut_MeromeroAct( attacker );
    if( calc::IsOccurPer(BTL_MEROMERO_EXE_PER) )
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------
/**
 * ワザ出し判定時のねむり解除チェック
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  bool    解除されたらtrue
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_CheckWazaExe_NemuriCure( BTL_POKEPARAM* attacker, WazaID waza )
{
  GFL_UNUSED( waza );
  PokeSick sick = attacker->GetPokeSick( );
  if( sick == pml::wazadata::WAZASICK_NEMURI )
  {
    if( attacker->CheckNemuriWakeUp() )
    {
      scPut_CurePokeSick( attacker, pml::wazadata::WAZASICK_NEMURI, true );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ワザ出し判定時のこおり解除チェック
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  bool      ワザの効果で解除される時のみtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_CheckWazaExe_KooriCure( BTL_POKEPARAM* attacker, WazaID waza )
{
  PokeSick sick = attacker->GetPokeSick( );
  bool f_cured = false;
  bool f_wazaMelt = false;

  if( sick == pml::pokepara::SICK_KOORI )
  {
    if( !scEvent_IsWazaMeltMustFail(attacker, waza) )
    {
      if( WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_KooriMelt) ){
        f_wazaMelt = true;
      }
    }
    if( !f_wazaMelt )
    {
      if( calc::IsOccurPer(BTL_KORI_MELT_PER) ){
        f_cured = true;
      }
    }
  }

  if( f_cured ){
    scPut_CurePokeSick( attacker, static_cast<WazaSick>(sick), true );
  }

  return f_wazaMelt;
}
//----------------------------------------------------------------------------------
/**
 * ワザ出し判定時のこおり解除失敗チェックイベント
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  bool      ワザ出しによるこおり解除を確実に失敗させる場合 true
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scEvent_IsWazaMeltMustFail( const BTL_POKEPARAM* attacker, WazaID waza )
{
  bool  bMustFail = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, bMustFail );
    EVENT_CallHandlers( this, BTL_EVENT_FAIL_WAZAMELT_CHECK );

    bMustFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  EVENTVAR_Pop();

  return bMustFail;

}

//----------------------------------------------------------------------------------
/**
 * [Proc] ワザ出し失敗時処理
 *
 * @param   attacker        技を失敗したポケモン
 * @param   waza            失敗した技
 * @param   isZenryokuWaza  失敗した技が全力技か？
 * @param   fail_cause      失敗した原因
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaExecuteFailed(
  BTL_POKEPARAM*   attacker,
  WazaID           waza,
  bool             isZenryokuWaza, 
  SV_WazaFailCause fail_cause )
{
  // こんらんなら自爆ダメージチェック
  if( fail_cause == SV_WAZAFAIL_KONRAN )
  {
    scproc_CalcConfDamage( attacker );
    if( scproc_CheckDeadCmd(attacker, false) ){
      if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() ){
        return;
      }
    }
  }
  else
  {
    // 失敗メッセージ
    scPut_WazaExecuteFailMsg( attacker, waza, isZenryokuWaza, fail_cause );

    // 演出
    switch( fail_cause ){
    case SV_WAZAFAIL_NEMURI:     scPut_EffectByPokePos( attacker, BTLEFF_NEMURI ); break;
    case SV_WAZAFAIL_MAHI:       scPut_EffectByPokePos( attacker, BTLEFF_MAHI ); break;
    case SV_WAZAFAIL_KOORI:      scPut_EffectByPokePos( attacker, BTLEFF_KOORI ); break;
//    case SV_WAZAFAIL_MEROMERO:   scPut_EffectByPokePos( attacker, BTLEFF_MEROMERO ); break;

    case SV_WAZAFAIL_SABOTAGE_GO_SLEEP:
      {
        BTL_SICKCONT cont = calc::MakeDefaultPokeSickCont( pml::pokepara::SICK_NEMURI, attacker->GetID() );

        SCQUE_PUT_ACT_TameWazaHide( m_que, attacker->GetID(), false );
        scPut_AddSick( attacker, pml::wazadata::WAZASICK_NEMURI, cont );
        scPut_EffectByPokePos( attacker, BTLEFF_NEMURI );
      }
      break;
    }
  }

  {
    u32 hem_state = m_HEManager.PushState();
    scEvent_CheckWazaExeFail( attacker, waza, fail_cause );
    m_HEManager.PopState( hem_state );
  }
}
//--------------------------------------------------------------------------
/**
 * @brief ワザ出し失敗メッセージ表示
 * @param bpp             技を使用するポケモン
 * @param waza            使用する技
 * @param isZenryokuWaza  使用する技が全力技か？
 * @param cause           失敗の原因
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaExecuteFailMsg( 
  BTL_POKEPARAM*   bpp, 
  WazaID           waza, 
  bool             isZenryokuWaza,
  SV_WazaFailCause cause )
{
  u8 pokeID = bpp->GetID();

  switch( cause ){
  case SV_WAZAFAIL_NEMURI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NemuriAct, pokeID );
    if( m_mainModule->GetPokeFriendship(bpp) >= 150 ){
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_Sleep );
    }
    break;
  case SV_WAZAFAIL_MAHI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_MahiAct, pokeID );
    break;
  case SV_WAZAFAIL_KOORI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KoriAct, pokeID );
    break;
  case SV_WAZAFAIL_SHRINK:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_ShrinkExe, pokeID );
    break;
  case SV_WAZAFAIL_KIAI_SHRINK:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KiaiShrink, pokeID );
    break;
  case SV_WAZAFAIL_MEROMERO:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_MeromeroExe, pokeID );
    break;
  case SV_WAZAFAIL_KANASIBARI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KanasibariWarn, pokeID, waza );
    break;
  /*
  case SV_WAZAFAIL_NAMAKE:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Namake, pokeID );
    break;
  */
  case SV_WAZAFAIL_ENCORE:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Namake, pokeID );
    break;
  case SV_WAZAFAIL_TYOUHATSU:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_ChouhatuWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_ICHAMON:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_IchamonWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_FUUIN:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_FuuinWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_KAIHUKUHUUJI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KaifukuFujiWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_ZIGOKUDUKI:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Zigokuduki_Warn, pokeID );
    break;
  case SV_WAZAFAIL_TRAPSHELL:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_TrapShell_Failed, pokeID );
    break;
  case SV_WAZAFAIL_HPFULL:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_HPFull, pokeID );
    break;
  case SV_WAZAFAIL_FUMIN:
    SCQUE_PUT_TOKWIN_IN( m_que, pokeID );
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Fumin, pokeID );
    SCQUE_PUT_TOKWIN_OUT( m_que, pokeID );
    break;
  case SV_WAZAFAIL_JURYOKU:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_JyuryokuWazaFail, pokeID, waza );
    break;
  case SV_WAZAFAIL_SABOTAGE:
    if( bpp->CheckNemuri( BTL_POKEPARAM::NEMURI_CHECK_ONLY_SICK ) ){
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Sabotage_Sleeping, pokeID  );
    }else{
      u8 rnd = calc::GetRand( 4 );
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Sabotage1 + rnd, pokeID  );
    }
    break;
  case SV_WAZAFAIL_SABOTAGE_GO_SLEEP:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Sabotage_GoSleep, pokeID  );
    break;

  case SV_WAZAFAIL_PP_ZERO:
    scPut_WazaMsg( bpp, waza, isZenryokuWaza );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_PP_Short, pokeID );
    break;

  case SV_WAZAFAIL_SKYBATTLE:
    scPut_WazaMsg( bpp, waza, isZenryokuWaza );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_SkyBattleFail );
    break;

  //@バトル変更箇所
  //sango追加↓
  case SV_WAZAFAIL_OOAME:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_RainStormFail );
    break;
  case SV_WAZAFAIL_OOHIDERI:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_DayFail );
    break;
  case SV_WAZAFAIL_IJIGEN_FOOPA:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_IjigemRushFail_Foopa, pokeID );
    break;
  case SV_WAZAFAIL_IJIGEN_OTHER:
  case SV_WAZAFAIL_DARKHOLE_OTHER:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_IjigemRushFail_Another, pokeID );
    break;
  //sango追加↑
  
  case SV_WAZAFAIL_WAZALOCK:
    // NMCat[404] 「おどりこ」→「はなびらのまい」でロックがかかった同ターンに自分のワザを出した場合の失敗メッセージを出す
    scPut_WazaMsg( bpp, waza, isZenryokuWaza );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail, pokeID );
    break;

  case SV_WAZAFAIL_UNEXPECTED_ERROR:
    scPut_WazaMsg( bpp, waza, isZenryokuWaza );
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail, pokeID );
    break;

  case SV_WAZAFAIL_TOKUSEI:
  case  SV_WAZAFAIL_NO_REACTION:
    // とくせいの場合、各ハンドラに任せる
    break;

  default:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail, pokeID );
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * [Put] ポケモン内部カウンタ値をセット
 *
 * @param   wk
 * @param   bpp
 * @param   counterID
 * @param   value
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_SetBppCounter( BTL_POKEPARAM* bpp, BTL_POKEPARAM::Counter counterID, u8 value )
{
  SCQUE_PUT_OP_SetPokeCounter( m_que, bpp->GetID(), counterID, value );
  bpp->COUNTER_Set( counterID, value );
}
//----------------------------------------------------------------------------------
/**
 * [Put] ポケモン内部カウンタ値をインクリメント
 *
 * @param   bpp        カウンタ操作対象ポケモン
 * @param   counterID  インクリメント対象のカウンタ
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_IncBppCounter( BTL_POKEPARAM* bpp, BTL_POKEPARAM::Counter counterID )
{
  if( bpp == NULL )
  {
    return;
  }
  bpp->COUNTER_Inc( counterID );
  u8 value = bpp->COUNTER_Get( counterID );
  SCQUE_PUT_OP_SetPokeCounter( m_que, bpp->GetID(), counterID, value );
}
//----------------------------------------------------------------------------------
/**
 * [Put] 相手を倒した数をインクリメント
 *
 * @param bpp  インクリメント対象ポケモン
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_IncKillCount( BTL_POKEPARAM* bpp )
{
  if( bpp == NULL )
  {
    return;
  }
  bpp->IncKillCount();
  SCQUE_PUT_OP_IncKillCount( m_que, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Proc] 使ったワザのPP減少
 *
 * @param   wk
 * @param   attacker  攻撃ポケモンパラメータ
 * @param   wazaIdx   使ったワザインデックス
 * @param   rec       ダメージを受けたポケモンパラメータ群
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scproc_decrementPPUsedWaza( BTL_POKEPARAM* attacker,
  WazaID waza, u8 wazaIdx, PokeSet* rec )
{
  u8 vol = scEvent_DecrementPPVolume( attacker, wazaIdx, waza, rec );
  scproc_decrementPP( attacker, wazaIdx, vol );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ使用後のデクリメントするPP値を取得
 *
 * @param   wk
 * @param   attacker  ワザ使用ポケモン
 * @param   rec       ワザによってダメージを受けたポケモンパラメータ群
 *
 * @retval  u32       デクリメントPP値
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_DecrementPPVolume( const BTL_POKEPARAM* attacker,
  u8 wazaIdx, WazaID waza, PokeSet* rec )
{
  u32 volume = 1;

  EVENTVAR_Push();
    {
      u32 i = 0;
      BTL_POKEPARAM* bpp;
      EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, rec->GetCount() );

      while( (bpp=rec->Get(i)) != NULL )
      {
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
        ++i;
      }
    }
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_IDX, wazaIdx );
//    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false ); // 真ワザワーク対象フラグとする
    EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );

    EVENT_CallHandlers( this, BTL_EVENT_DECREMENT_PP );
    volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );

  EVENTVAR_Pop();

  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ワザのPP減少
 *
 * @param   wk
 * @param   bpp       対象ポケモンパラメータ
 * @param   wazaIdx   減少させるワザインデックス
 * @param   volume    減少量
 *
 * @retval  bool      減少させたらtrue／死亡時など減少させられなかったらfalse
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_decrementPP( BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume )
{
  u16 restPP = bpp->WAZA_GetPP( wazaIdx );

  if( volume >= restPP ){
    volume = static_cast<u8>(restPP);
  }

  TAYA_Printf("restPP=%d, volume=%d\n", restPP, volume );

  if( volume )
  {
    scPut_DecrementPP( bpp, wazaIdx, volume );
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------------
/**
 * [Put] 使ったワザのPPデクリメント
 *
 * @param   wk
 * @param   attacker
 * @param   wazaIdx
 * @param   vol
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_DecrementPP( BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol )
{
  if( !m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_PP_CONST) )
  {
    u8 pokeID = attacker->GetID();

    attacker->WAZA_DecrementPP( wazaIdx, vol );
    SCQUE_PUT_OP_PPMinus( m_que, pokeID, wazaIdx, vol );

    attacker->WAZA_SetUsedFlag( wazaIdx );
    SCQUE_PUT_OP_WazaUsed( m_que, pokeID, wazaIdx );
  }
}
//----------------------------------------------------------------------
// サーバーフロー：「たたかう」> ダメージワザ
//----------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_Root( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, bool fDelayAttack )
{
  u32  dmg_sum = 0;
  WAZAEFF_RESERVE_POS  que_reserve_pos;

  FlowFlg_Clear( FLOWFLG_SET_WAZAEFFECT );
  scproc_Fight_DamageProcStart( attacker, wazaParam, targets );

  // ダメージ受けポケモンワークをクリアしておく
  m_psetDamaged->Clear();

  scEvent_HitCheckParam(attacker, wazaParam->wazaID,wazaParam->zenryokuWazaKind, m_hitCheckParam, fDelayAttack );

  // ダメージワザ回復化 コイツは現在「プレゼント」でしか使われていない
  scproc_Fight_Damage_ToRecover( attacker, wazaParam, targets );

  if( targets->GetCount() )
  {
    if( HITCHECK_IsPluralHitWaza(m_hitCheckParam)
    &&  (targets->GetCountMax() == 1)
    ){
      dmg_sum = scproc_Fight_Damage_PluralCount(wazaParam, attacker, targets, affRec, &que_reserve_pos );
    }
    else{
      dmg_sum = scproc_Fight_Damage_SingleCount( wazaParam, attacker, targets, affRec, &que_reserve_pos, fDelayAttack );
      scmd::SCQUE_PutWazaSyncChapter( m_que );
    }

    scproc_Fight_Damage_Kickback( attacker, wazaParam->wazaID, dmg_sum );
  }

  scproc_Fight_DamageProcEnd( actionDesc, wazaParam, attacker, m_psetDamaged, dmg_sum, fDelayAttack );

  if( que_reserve_pos.IsValid() )
  {
    wazaEffCtrl_SetEnableDummy( m_wazaEffCtrl );
    scPut_WazaEffect( wazaParam->wazaID, m_wazaEffCtrl, que_reserve_pos, true, wazaParam->zenryokuWazaKind );
  }
}
/**
 * ダメージ計算結果をワークに取得
 * @param[out]  dmgRec
 * @param       attacker
 * @param       target
 * @param       wazaParam
 * @param[in]   affRec
 * @param       dmg_ratio
 * @param[out]  affCounter  [バトル検定用] 効果レベルカウンタ
 */
void ServerFlow::CALCDMG_Set( CALC_DAMAGE_REC* dmgRec, BTL_POKEPARAM* attacker, PokeSet* target,
  const WAZAPARAM* wazaParam, const DmgAffRec& affRec, fx32 dmg_ratio , AffCounter* affCounter )
{
  BTL_POKEPARAM* bpp;
  u32 c;
  u16 damage;
  bool bFriendshipAct = false;

  dmgRec->dmg_ratio = dmg_ratio;
  dmgRec->real_hit_count = 0;
  dmgRec->migawari_hit_count = 0;
  c = 0;

  target->SeekStart();
  while( (bpp = target->SeekNext() ) != NULL )
  {
    bool bMigawariEffective = bpp->MIGAWARI_IsExist();
    if( bMigawariEffective )
    {
      if( scEvent_CheckMigawariThrew(attacker, bpp, wazaParam->wazaID) ){
        bMigawariEffective = false;
      }
    }

    dmgRec->record[c].pokeID = bpp->GetID();
    dmgRec->record[c].fCritical = scEvent_CheckCritical( attacker, bpp, wazaParam->wazaID, &bFriendshipAct );
    dmgRec->record[c].fCriticalByFriendship = (bFriendshipAct == true);
    dmgRec->record[c].affine = affRec.Get( dmgRec->record[c].pokeID );
    dmgRec->record[c].fFixDamage = scEvent_CalcDamage( attacker, bpp, wazaParam,
        static_cast<BtlTypeAff>(dmgRec->record[c].affine),  dmg_ratio, dmgRec->record[c].fCritical, false, bMigawariEffective, &damage );
    dmgRec->record[c].damage = damage;

    // 固定ダメージなら相性等倍、クリティカル無しとして処理
    if( dmgRec->record[c].fFixDamage )
    {
      dmgRec->record[c].fCritical = false;
      dmgRec->record[c].fCriticalByFriendship = false;
      dmgRec->record[c].affine = pml::battle::TypeAffinity::TYPEAFF_1;
    }

    dmgRec->record[c].fMigawari = bMigawariEffective;

    if( !(dmgRec->record[c].fMigawari) )
    {
      u16 round_damage = roundDamage( bpp, dmgRec->record[c].damage );
      dmgRec->record[c].koraeru = scEvent_CheckKoraeru( attacker, bpp, true, &round_damage );
      dmgRec->record[c].damage = round_damage;
      dmgRec->real_hit_count++;
    }
    else
    {
      dmgRec->record[c].damage = damage;
      dmgRec->record[c].koraeru = BTL_POKEPARAM::KORAE_NONE;
      dmgRec->migawari_hit_count++;
    }
//    TAYA_Printf("ダメージ記録：ポケ(%d)に対するダメージ=%d\n", bpp->GetID(), dmgRec->record[c].damage );
    if( affCounter ){
      affCounter->CountUp( *m_mainModule, attacker, bpp, static_cast<BtlTypeAff>(dmgRec->record[c].affine) );
    }

    ++c;
  }
  dmgRec->total_hit_count = c;
}


//----------------------------------------------------------------------------------
/**
 * ダメージ計算結果に格納されたターゲットポケモン数を取得（実体・みがわりの合計）
 *
 * @param   dmgRec
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::CALCDMG_GetCount( const CALC_DAMAGE_REC* dmgRec )
{
  return dmgRec->total_hit_count;
}
//----------------------------------------------------------------------------------
/**
 * みがわりヒットしたポケモンの情報のみを配列に出力
 *
 * @retval  u32   みがわりヒットしたポケモン数
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::CALCDMG_GetMigawariHitPoke( const CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, CriticalType* criticalFlagAry )
{
  u32 i, c;
  for(i=0, c=0; i<rec->total_hit_count; ++i)
  {
    if( rec->record[i].fMigawari )
    {
      bppAry[c] = m_pokeCon->GetPokeParam( rec->record[i].pokeID );
      dmgAry[c] = rec->record[i].damage;
      affAry[c] = static_cast<BtlTypeAff>(rec->record[i].affine);
      criticalFlagAry[c] = (rec->record[i].fCritical)? CRITICAL_NORMAL : CRITICAL_NONE;
      ++c;
    }
  }
  return c;
}
//----------------------------------------------------------------------------------
/**
 * みがわり・実体問わずヒットしたポケモンの情報を配列に出力
 *
 * @retval  u32   ヒットしたポケモン数
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::CALCDMG_GetAllHitPoke( const CALC_DAMAGE_REC* rec, BTL_POKEPARAM** bppAry )
{
  for(u32 i=0; i<rec->total_hit_count; ++i)
  {
    bppAry[i] = m_pokeCon->GetPokeParam( rec->record[i].pokeID );
  }
  return rec->total_hit_count;
}
//----------------------------------------------------------------------------------
/**
 * 実体ヒットしたポケモンの情報のみを配列に出力
 *
 * @retval  u32   ヒットしたポケモン数
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::CALCDMG_GetRealHitPoke( const CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, CriticalType* criticalFlagAry, BTL_POKEPARAM::KoraeruCause* koraeCauseAry )
{
  u32 i, c;
  for(i=0, c=0; i<rec->total_hit_count; ++i)
  {
    if( !rec->record[i].fMigawari )
    {
      bppAry[c] = m_pokeCon->GetPokeParam( rec->record[i].pokeID );
      dmgAry[c] = rec->record[i].damage;
      affAry[c] = static_cast<BtlTypeAff>(rec->record[i].affine);
      if( rec->record[i].fCritical )
      {
        criticalFlagAry[c] = (rec->record[i].fCriticalByFriendship)? CRITICAL_FRIENDSHIP : CRITICAL_NORMAL;
      }
      else
      {
        criticalFlagAry[c] = CRITICAL_NONE;
      }

      koraeCauseAry[c] = static_cast<BTL_POKEPARAM::KoraeruCause>(rec->record[i].koraeru);
      ++c;
    }
  }
  return c;
}
//----------------------------------------------------------------------------------
/**
 * ダメージ合計を取得
 *
 * @param   rec
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::CALCDMG_GetDamageSum( const CALC_DAMAGE_REC* rec )
{
  u32 sum = 0;
  for(u32 i=0; i<rec->total_hit_count; ++i)
  {
    sum += rec->record[i].damage;
  }
  return sum;
}
//--------------------------------------------------------
/**
 *  ワザエフェクト位置を記録する
 */
//--------------------------------------------------------
void ServerFlow::reserveWazaEffPos( WAZAEFF_RESERVE_POS* reservePos )
{
  reservePos->prevEffectCmd = scmd::SCQUE_RESERVE_Pos( m_que, SC_ACT_KINOMI_PRE_WAZA );
  reservePos->mainEffectCmd = scmd::SCQUE_RESERVE_Pos( m_que, SC_ACT_WAZA_EFFECT );
}

//------------------------------------------------------------------------------
/**
* １回ヒットワザ（対象は１体以上）
*/
//------------------------------------------------------------------------------
u32  ServerFlow::scproc_Fight_Damage_SingleCount( const WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec, WAZAEFF_RESERVE_POS* que_reserve_pos, bool fDelayAttack )
{
  FLAG_SET flagSet;
  fx32 dmg_ratio = getDamageRatioByTargetCount( *targets );
  u32 dmg_sum = 0;

  // 複数対象のワザか判定
  flagSet.hitPluralPoke = (targets->GetCount() > 1);
  flagSet.delayAttack = fDelayAttack;

  // 対象をすばやさ順ソート
//  BTL_POKESET_SortByAgility( targets, wk );

  // 敵・味方を別々のSetにコピー
  targets->CopyFriends( *m_mainModule, attacker, m_psetFriend );
  targets->CopyEnemys( *m_mainModule, attacker, m_psetEnemy );

  // ダメージ計算結果をワークに保存
  CALCDMG_Set(m_calcDmgFriend, attacker, m_psetFriend, wazaParam, affRec, dmg_ratio, &m_affCounter );
  CALCDMG_Set(m_calcDmgEnemy, attacker, m_psetEnemy, wazaParam, affRec, dmg_ratio, &m_affCounter );

  reserveWazaEffPos( que_reserve_pos );


  if( m_psetFriend->GetCount() )
  {
    dmg_sum += scproc_Fight_Damage_side( wazaParam, attacker, m_psetFriend, m_calcDmgFriend,
                  m_hitCheckParam, flagSet );
    if( dmg_sum )
    {
      if( checkPlayersPoke(attacker) ){
        m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_TEMOTI_MAKIZOE );
      }
    }
  }
  if( m_psetEnemy->GetCount() )
  {
    dmg_sum += scproc_Fight_Damage_side( wazaParam, attacker, m_psetEnemy, m_calcDmgEnemy,
      m_hitCheckParam, flagSet );
  }

  return dmg_sum;
}

/**
 * @brief 技ターゲットから、ダメージ補正率を取得する
 * @param targets  技ターゲット
 * @return ダメージ補正率
 */
fx32 ServerFlow::getDamageRatioByTargetCount( const PokeSet& targets ) const
{
  if( targets.GetCountMax() == 1 ) 
  {
    return BTL_CALC_DMG_TARGET_RATIO_NONE;
  }

  BtlRule rule = m_mainModule->GetRule();
  if( rule == BTL_RULE_ROYAL )
  {
    return BTL_CALC_DMG_TARGET_RATIO_PLURAL_ROYAL;
  }
  
  return BTL_CALC_DMG_TARGET_RATIO_PLURAL;
}

//------------------------------------------------------------------------------
/**
* 複数回ヒットワザ（対象は１体のみ）
*/
//------------------------------------------------------------------------------
u32  ServerFlow::scproc_Fight_Damage_PluralCount(const WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, PokeSet* targets, const DmgAffRec& affRec , WAZAEFF_RESERVE_POS* que_reserve_pos)
{
  BTL_POKEPARAM* bpp;
  u32 hitCount;
  u32 dmg_sum = 0;
  WazaSick  pokeSick;
  BtlPokePos  targetPos = BTL_POS_NULL;
  FLAG_SET flagSet;

  bpp = targets->Get( 0 );
  GFL_ASSERT_STOP(bpp);

  m_thruDeadMsgPokeID = bpp->GetID();

  // 複数回ヒットはエフェクトにカメラ演出を入れず、ズームインコマンドを生成する
  if( m_mainModule->IsWazaEffectEnable() )
  {
    targetPos = m_posPoke.GetPokeExistPos( bpp->GetID() );
    if( targetPos != BTL_POS_NULL ){
      SCQUE_PUT_ACT_EffectByVector( m_que, m_posPoke.GetPokeExistPos( attacker->GetID() ),
                                    targetPos, BTLEFF_ZOOM_IN );
    }
  }

  flagSet.raw = 0;
  fx32 dmg_ratio = BTL_CALC_DMG_TARGET_RATIO_NONE;
  bool bPluralHitException = HITCHECK_IsPluralHitException( m_hitCheckParam );

  hitCount = 0;
  for(u32 i=0; i<m_hitCheckParam->countMax; ++i)
  {
    pokeSick = static_cast<WazaSick>(attacker->GetPokeSick());

    // ワザエフェクトコマンド生成
    bool bExceptionEffect = ((bPluralHitException) && (i > 0));

    // 通常の複数回攻撃ワザ演出 または （おやこあいなどによる）拡張型の複数回ワザ１回目の演出
    if( !bExceptionEffect )
    {
      if( i == 0 ){
        reserveWazaEffPos( que_reserve_pos );
      }else{
        SCQUE_PUT_ACT_WazaEffect( m_que,
          m_wazaEffCtrl->attackerPos, m_wazaEffCtrl->targetPos, wazaParam->wazaID, i, true, wazaParam->zenryokuWazaKind );
      }
      dmg_ratio = BTL_CALC_DMG_TARGET_RATIO_NONE;
    }
    // 拡張型の複数回ワザ２回目以降の演出
    else
    {
      dmg_ratio = BTL_CALC_DMG_TARGET_RATIO_QUAT;
      // この後、ダメージ演出などで同様のヒットマーク演出が入ることになり
      // 同じエフェクトが続いてしまうので、何も出さないようにします 2013.03.22  taya
      #if 0
      SCQUE_PUT_ACT_ExPlural2ndHit( m_que, m_wazaEffCtrl->targetPos, BTLEFF_SIMPLE_HIT );
      BTL_PRINT(" i=%d/%d, 拡張ヒット２回の演出コマンド\n", i, m_hitCheckParam->countMax);
      #endif
    }

    CALCDMG_Set(m_calcDmgEnemy, attacker, targets, wazaParam, affRec, dmg_ratio,
                              (i==0)? &m_affCounter : NULL );

    dmg_sum += scproc_Fight_Damage_side( wazaParam, attacker, targets, m_calcDmgEnemy,
                    m_hitCheckParam, flagSet );
    ++hitCount;
    scmd::SCQUE_PutWazaSyncChapter( m_que );

    if( bpp->IsDead() ){ break; }
    if( attacker->IsDead() ){ break; }

    //
    scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_HP );

    // 攻撃したことにより攻撃側が眠ってしまったらブレイク
    if( (attacker->GetPokeSick() == pml::pokepara::SICK_NEMURI)
    &&  (pokeSick != pml::pokepara::SICK_NEMURI)
    ){
      break;
    }
    // 毎回ヒットチェックを行うワザは外れたらブレイク
    if( m_hitCheckParam->fCheckEveryTime ){
      if( !scEvent_CheckHit(attacker, bpp, wazaParam, NULL) ){ break; }
    }
  }

  if( hitCount>0 ){
    scPut_DamageAff( static_cast<BtlTypeAff>(m_hitCheckParam->pluralHitAffinity) );
    BTL_PRINT("%d回あたった\n",hitCount);
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Hit_PluralTimes, hitCount );
  }

  if( bpp->IsDead() ){
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Dead, bpp->GetID() );
  }

  // カメラズームインをリセット
  if( targetPos != BTL_POS_NULL ){
    SCQUE_PUT_ACT_EffectSimple( m_que, BTLEFF_ZOOM_IN_RESET );
  }

  m_thruDeadMsgPokeID = BTL_POKEID_NULL;

  return dmg_sum;
}


//----------------------------------------------------------------------------------
/**
 * １陣営に対するダメージ処理
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   targets
 * @param   dmg_ratio
 *
 * @retval  u32  発生したダメージ量
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scproc_Fight_Damage_side( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  PokeSet* targets, const CALC_DAMAGE_REC* dmgRec, HITCHECK_PARAM* hitCheckParam, FLAG_SET flagSet )
{
  u8 damaged_poke_cnt;

  damaged_poke_cnt = CALCDMG_GetCount( dmgRec );

  if( damaged_poke_cnt )
  {
    u32 dmg_sum = scproc_Fight_damage_side_core( attacker, targets, dmgRec, wazaParam,
                  hitCheckParam, dmgRec->dmg_ratio, flagSet );
    scproc_WazaDamageSideAfter( attacker, wazaParam, dmg_sum );
    return dmg_sum;
  }

  return 0;
}
/**
 *  １陣営ダメージ処理コア
 */
u32  ServerFlow::scproc_Fight_damage_side_core(
    BTL_POKEPARAM* attacker, PokeSet* targets, const CALC_DAMAGE_REC* dmgRec,
    const WAZAPARAM* wazaParam, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet )
{
  GFL_UNUSED( targets );
  GFL_UNUSED( dmg_ratio );

  DamageProcParamStack::Params* params = m_damageProcParamStack.Push();

  u32 dmg_sum;
  u8  poke_cnt;
  int i;
  BtlPokePos atkPos = m_posPoke.GetPokeExistPos( attacker->GetID() );

  dmg_sum = CALCDMG_GetDamageSum( dmgRec );

  // 身代わりダメージ処理
  poke_cnt = CALCDMG_GetMigawariHitPoke( dmgRec, params->bpp, params->dmg, params->affAry, params->criticalTypes );
  for(i=0; i<poke_cnt; ++i)
  {
    if( params->bpp[i]->MIGAWARI_IsExist() )
    {
      u16 add_damage = scproc_Migawari_Damage( attacker, params->bpp[i], params->dmg[i], params->affAry[i], params->criticalTypes[i], wazaParam );
      m_psetDamaged->AddWithDamage( params->bpp[i], add_damage, true );  // ダメージ0として記録
      dmg_sum -= (params->dmg[i] - add_damage);
    }
  }

  // ダメージ確定イベントコール（複数回ヒットワザなら初回のみ）
  if( HITCHECK_IsFirstTime(hitCheckParam) )
  {
    poke_cnt = CALCDMG_GetAllHitPoke( dmgRec, params->bpp );
    for(i=0; i<poke_cnt; ++i){
      scproc_Fight_Damage_Determine( attacker, params->bpp[i], wazaParam );
    }
  }
  hitCheckParam->count++;

  // 実体ヒットした分をコピー（みがわりだけだったらここでリターン）
  poke_cnt = CALCDMG_GetRealHitPoke( dmgRec, params->bpp, params->dmg, params->affAry, params->criticalTypes, params->koraeru_cause );
  if( poke_cnt == 0 )
  {
    // 身代わりヒット時にも攻撃側ポケひん死チェックイベントは呼び出す
    if( CALCDMG_GetCount(dmgRec) ){
      scproc_checkAttackerDead( attacker, wazaParam );
    }
    m_damageProcParamStack.Pop();
    return dmg_sum;
  }

  // ダメージ不正書き換えテスト
  #if PM_DEBUG
  if( m_fDebugClack )
  {
    if( m_mainModule->CheckImServerMachine() ){
      for(u32 di=0; di<poke_cnt; ++di){
        params->dmg[di] = 999;
      }
    }
  }
  #endif

  // ダメージコマンド出力
  scproc_PrevWazaDamage( wazaParam, attacker, poke_cnt, params->bpp );
  scPut_WazaDamagePlural( attacker, wazaParam, poke_cnt, params->affAry, params->bpp, params->dmg, flagSet.hitPluralPoke );

  if( !HITCHECK_IsPluralHitWaza(hitCheckParam) ){
    // １回ヒットなら相性メッセージ即表示
    scPut_WazaAffinityMsg( attacker, poke_cnt, params->affAry, params->bpp, flagSet.hitPluralPoke );
  }else{
    // 複数回ヒットワザの場合は相性値を記録するだけ
    HITCHECK_SetPluralHitAffinity( hitCheckParam, params->affAry[0] );
  }
  scproc_CriticalMsg( attacker, wazaParam, poke_cnt, params->bpp, params->criticalTypes , flagSet.hitPluralPoke );

  // ダメージ記録
  for(i=0; i<poke_cnt; ++i)
  {
    m_psetDamaged->AddWithDamage( params->bpp[i], params->dmg[i], false );
    wazaDmgRec_Add( atkPos, attacker, params->bpp[i], wazaParam, params->dmg[i] );
    params->bpp[i]->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_DAMAGED );
    applyWazaDamageToIntrudeSystem( *(params->bpp[i]), params->affAry[i] );
  }

  // こらえた反応
  BTL_PRINT("こらえた反応を見ます 全%d体\n", poke_cnt);
  for(i=0; i<poke_cnt; ++i)
  {
    BTL_PRINT("  idx:%d, cause=%d\n",i, params->koraeru_cause[i]);
    if( params->koraeru_cause[i] != BTL_POKEPARAM::KORAE_NONE ){
      scproc_Koraeru( params->bpp[i], params->koraeru_cause[i] );
    }
  }

  // 追加効果
  for(i=0; i<poke_cnt; ++i)
  {
    scproc_CheckShrink( wazaParam, attacker, params->bpp[i] );
    scproc_Damage_Drain( wazaParam, attacker, params->bpp[i], params->dmg[i] );
    scproc_WazaAdditionalEffect( wazaParam, attacker, params->bpp[i], params->dmg[i], false );
  }
  scproc_WazaAdditionalEffect_User( wazaParam, attacker, false );

  // リアクション処理
  for(i=0; i<poke_cnt; ++i)
  {
    scproc_WazaDamageReaction( attacker, params->bpp[i], wazaParam, params->affAry[i], params->dmg[i], (params->criticalTypes[i]!=CRITICAL_NONE), false );
  }

  // ひんしチェック
  bool bCheckDeadCmdPlural = checkDeadCmdPlural( params->bpp, poke_cnt, params->pluralDeadIdx );
  MainModule::PGLRecParam  pgl_param( attacker, wazaParam->orgWazaID );

  scproc_checkAttackerDead( attacker, wazaParam );

  for(i=0; i<poke_cnt; ++i)
  {
    scproc_CheckDeadCmd( params->bpp[i], bCheckDeadCmdPlural, &pgl_param );
  }
  if( bCheckDeadCmdPlural ){
    scproc_CheckDeadCmdAfterForPlural( params->bpp, poke_cnt, params->pluralDeadIdx );
  }

  scproc_CheckDeadCmd( attacker, false );
  m_damageProcParamStack.Pop();

  return dmg_sum;
}

/**
 * ダメージワザ対象のひん死チェック前に攻撃側のひん死チェックを行う
 * @param attacker
 * @param wazaParam
 */
void ServerFlow::scproc_checkAttackerDead( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam )
{
  if( scEvent_CheckAttackerDeadPreTarget(attacker, wazaParam) ){
    scproc_CheckDeadCmd( attacker, false );
  }
}

/**
 * [Event] ダメージワザ対象のひん死チェック前に攻撃側のひん死チェックを行うかどうか判定
 * @param attacker
 * @param wazaParam
 */
bool  ServerFlow::scEvent_CheckAttackerDeadPreTarget( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam )
{
  bool result = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_ATTACKER_DEAD );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return result;
}

//----------------------------------------------------------------------------------
/**
 * ダメージ無効化（＆回復化）してしまうポケモンの処理。処理後、該当ポケモンをtargetsから除外
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 * @param   targets
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_ToRecover( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets )
{
  BTL_POKEPARAM* bpp;

  // 回復チェック -> 特定タイプのダメージを無効化、回復してしまうポケを targets から除外
  targets->SeekStart();
  while( (bpp = targets->SeekNext()) != NULL )
  {
    if( scEvent_CheckDmgToRecover(attacker, bpp, wazaParam) )
    {
      u32 hem_state;

      wazaEffCtrl_SetEnable( m_wazaEffCtrl );

      hem_state = m_HEManager.PushState();
      scEvent_DmgToRecover(  attacker, bpp );
//      scproc_HandEx_Root( ITEM_DUMMY_DATA );

      m_HEManager.PopState( hem_state );
      targets->Remove( bpp );
    }
  }
}
/**
 *  効果は○○だ　メッセージ出力
 */
void ServerFlow::scPut_DamageAff( BtlTypeAff affinity )
{
  if( affinity < pml::battle::TypeAffinity::TYPEAFF_1 ){
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_AffBad );
  }
  else if(affinity > pml::battle::TypeAffinity::TYPEAFF_1){
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_AffGood );
  }
}
/**
 * 最高で残りＨＰの範囲に収まるようにダメージ最終補正
 */
u32 ServerFlow::roundDamage( const BTL_POKEPARAM* bpp, u32 damage )
{
  u32 hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
  if( damage > hp ){
    damage = hp;
  }
  return damage;
}
//----------------------------------------------------------------------------------
/**
 * ワザによるダメージを与えることが確定した（みがわりも含む）
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 * @param   targets
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_Determine(
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  u32 hem_state = m_HEManager.PushState();

  scEvent_WazaDamageDetermine( attacker, defender, wazaParam );

  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるダメージを与えることが確定した
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaDamageDetermine( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );

    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_DETERMINE );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ・混乱自爆によるダメージを「こらえる」かどうかチェック
 * ※「こらえる」= HPが1残る
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   fWazaDamage   ワザによるダメージの場合true／混乱ダメージならfalseを指定
 * @param   damage
 *
 * @retval  BTL_POKEPARAM::KoraeruCause
 */
//----------------------------------------------------------------------------------
BTL_POKEPARAM::KoraeruCause  ServerFlow::scEvent_CheckKoraeru( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, bool fWazaDamage, u16* damage )
{
  if( defender->GetValue(BTL_POKEPARAM::BPP_HP) > (*damage) )
  {
    return BTL_POKEPARAM::KORAE_NONE;
  }
  else
  {
    BTL_POKEPARAM::KoraeruCause  cause = BTL_POKEPARAM::KORAE_NONE;

    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_GEN_FLAG,  fWazaDamage );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_KORAERU_CAUSE, cause );
      EVENT_CallHandlers( this, BTL_EVENT_KORAERU_CHECK );
      cause = static_cast<BTL_POKEPARAM::KoraeruCause>( EVENTVAR_GetValue( BTL_EVAR_KORAERU_CAUSE ) );
    EVENTVAR_Pop();

    // かわいがり効果判定
    if( cause == BTL_POKEPARAM::KORAE_NONE )
    {
      u32 friendship = getFriendship( defender );
      u32 per = 0;
      if( friendship >= 255 ){
        per = 25;
      }else if( friendship >= 200 ){
        per = 20;
      }else if( friendship >= 150 ){
        per = 15;
      }else if( friendship >= 100 ){
        per = 10;
      }
      if( calc::IsOccurPer(per) ){
        cause = BTL_POKEPARAM::KORAE_FRIENDSHIP;
      }
    }

    if( cause != BTL_POKEPARAM::KORAE_NONE ){
      *damage = defender->GetValue(BTL_POKEPARAM::BPP_HP) - 1;
    }

    return cause;
  }
}
//----------------------------------------------------------------------------------
/**
 * 「こらえる」原因表示＆アイテム消費等の処理
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Koraeru( BTL_POKEPARAM* bpp, BTL_POKEPARAM::KoraeruCause cause )
{
  u8 pokeID = bpp->GetID();

  switch( cause ){
  case BTL_POKEPARAM::KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;

  case BTL_POKEPARAM::KORAE_FRIENDSHIP:
    {
      u8 clientID = MainModule::PokeIDtoClientID( pokeID );
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_Korae, pokeID, clientID );
      SCQUE_PUT_ACT_FriendshipEffect( m_que, pokeID, FREFF_HEART );
    }
    break;

  case BTL_POKEPARAM::KORAE_ITEM:
  default:
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_KoraeruExe( bpp, cause );
//      scproc_HandEx_Root( ITEM_DUMMY_DATA );
      m_HEManager.PopState( hem_state );
    }
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] こらえる動作発動
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_KoraeruExe( BTL_POKEPARAM* bpp, BTL_POKEPARAM::KoraeruCause cause )
{
  GFL_UNUSED( cause );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_KORAERU_EXE );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージ演出直後（みがわりは除外されている）
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   poke_cnt
 * @param   bppAry
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_PrevWazaDamage( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry )
{
  u32 hem_state = m_HEManager.PushState();

  scEvent_PrevWazaDamage( wazaParam, attacker, poke_cnt, bppAry );

  m_HEManager.PopState( hem_state );
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザダメージ演出直後（みがわりは除外されている）
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   poke_cnt
 * @param   bppAry
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_PrevWazaDamage( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry )
{
  GFL_UNUSED( wazaParam );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, poke_cnt );
    for(u32 i=0; i<poke_cnt; ++i)
    {
      EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bppAry[i]->GetID() );
    }
    EVENT_CallHandlers( this, BTL_EVENT_PREV_WAZA_DMG );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * ワザダメージレコードを追加
 *
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   damage
 */
//----------------------------------------------------------------------------------
void ServerFlow::wazaDmgRec_Add( BtlPokePos atkPos, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, u16 damage )
{
  BTL_POKEPARAM::WAZADMG_REC rec;
  u8 atkPokeID = attacker->GetID();
  u8 defPokeID = defender->GetID();

  BTL_POKEPARAM::WAZADMG_REC_Setup( &rec, atkPokeID, atkPos, wazaParam->wazaID, wazaParam->wazaType, damage, wazaParam->damageType );
  defender->WAZADMGREC_Add( &rec );

  SCQUE_OP_AddWazaDmgRec( m_que, defPokeID, atkPokeID, atkPos, wazaParam->wazaType, wazaParam->damageType, wazaParam->wazaID, damage );
}

//----------------------------------------------------------------------------------
/**
 * @brief 発生したダメージを、乱入システムに適用する
 * @param defender   ダメージを受けたポケモン
 * @param affinity   効果相性
 */
//----------------------------------------------------------------------------------
void ServerFlow::applyWazaDamageToIntrudeSystem( const BTL_POKEPARAM& defender, BtlTypeAff affinity )
{
  if( m_intrudeSystem == NULL )
  {
    return;
  }

  if( pml::battle::TypeAffinity::TYPEAFF_2 <= affinity )
  {
    m_intrudeSystem->RecordBatugunDamage( defender.GetID() );
  }
}

//--------------------------------------------------------------------------
/**
 * ワザダメージ後の追加効果処理
 *
 * @param   wk
 * @param   waza        使用ワザ
 * @param   attacker    攻撃ポケモンパラメータ
 * @param   defender    防御ポケモンパラメータ
 * @param   damage      与えたダメージ
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scproc_WazaAdditionalEffect( const WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage, bool fMigawriHit )
{
  GFL_UNUSED( damage );

  WazaCategory category = WAZADATA_GetCategory( wazaParam->wazaID );

  switch( category ){
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT:
    if( !fMigawriHit ){
      scproc_Fight_Damage_AddEffect( wazaParam, attacker, defender );
    }
    break;
  case pml::wazadata::CATEGORY_DAMAGE_SICK:
    if( !fMigawriHit ){
      scproc_Fight_Damage_AddSick( wazaParam, attacker, defender );
    }
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * ワザダメージ後の、技を撃ったポケモンに対する追加効果処理
 *
 * @param   wk
 * @param   waza        使用ワザ
 * @param   attacker    攻撃ポケモンパラメータ
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scproc_WazaAdditionalEffect_User( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, bool fMigawriHit )
{
  WazaCategory category = WAZADATA_GetCategory( wazaParam->wazaID );

  switch( category ){
  case pml::wazadata::CATEGORY_DAMAGE_EFFECT_USER:
    scproc_Fight_Damage_AddEffect( wazaParam, attacker, attacker );
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージ１回１陣営毎の終了処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   affinity
 * @param   damage
 * @param   critical_flag
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaDamageSideAfter( BTL_POKEPARAM* attacker,
  const WAZAPARAM* wazaParam, u32 damage )
{
  u32 hem_state = m_HEManager.PushState();

  scEvent_WazaDamageSideAfter(  attacker, wazaParam, damage );
  m_HEManager.PopState( hem_state );
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザダメージを与え終わった後の追加処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaDamageSideAfter( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u32 damage )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage );
    EVENT_CallHandlers( this, BTL_EVENT_WAZADMG_SIDE_AFTER );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * アイテム反応チェック
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_CheckItemReaction( BTL_POKEPARAM* bpp, BtlItemReaction reactionType )
{
  if( bpp->GetItem() != ITEM_DUMMY_DATA )
  {
    u32 hem_state = m_HEManager.PushState();

    BTL_N_Printf( DBGSTR_SVFL_CheckItemReaction, bpp->GetID());

    scEvent_CheckItemReaction( bpp, reactionType );
    m_HEManager.PopState( hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event]アイテム反応チェック
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_CheckItemReaction( const BTL_POKEPARAM* bpp, BtlItemReaction reactionType )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ITEM_REACTION, reactionType );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_ITEM_REACTION );
  EVENTVAR_Pop();
}
//------------------------------------------------------------------
// サーバーフロー：ダメージワザシーケンス開始
//------------------------------------------------------------------
void  ServerFlow::scproc_Fight_DamageProcStart( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_DamageProcStart( attacker, wazaParam, targets );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ダメージワザ処理開始
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_DamageProcStart( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* targets )
{
  u32 targetCnt = targets->GetCount();

  EVENTVAR_Push();

    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      BTL_POKEPARAM* bpp;
      for( u32 i=0; i<targetCnt; ++i )
      {
        bpp = targets->Get( i );
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
      }
    }

    EVENT_CallHandlers( this, BTL_EVENT_DAMAGEPROC_START );

  EVENTVAR_Pop();
}
//------------------------------------------------------------------
// サーバーフロー：ダメージワザシーケンス終了
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   wazaParam   ワザパラメータ
 * @param   attacker    攻撃ポケ
 * @param   targets     ダメージを受けたポケモン群（みがわり除く）
 * @param   dmg_sum     みがわり分も含めたダメージトータル
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_DamageProcEnd( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets, u32 dmgTotal, bool fDelayAttack )
{
  GFL_UNUSED( dmgTotal );

  scproc_Fight_Damage_KoriCure( wazaParam, attacker, targets );
  scproc_Fight_Damage_FriendActPinch( targets );

  {
    u32 hem_state = m_HEManager.PushState();

      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, false, BTL_EVENT_DAMAGEPROC_END_HIT_L1 );
      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, true,  BTL_EVENT_DAMAGEPROC_END_HIT_REAL );
      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, false, BTL_EVENT_DAMAGEPROC_END_HIT_L2 );

      // ダメージ反応アイテム
      {
        BTL_POKEPARAM* bpp;
        targets->SeekStart();
        while( (bpp = targets->SeekNext()) != NULL )
        {
          scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_HP );
        }
      }

      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, false, BTL_EVENT_DAMAGEPROC_END_HIT_L3 );
      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, false, BTL_EVENT_DAMAGEPROC_END_HIT_L4 );
      scEvent_DamageProcEndSub( attacker, targets, actionDesc, wazaParam, fDelayAttack, true,  BTL_EVENT_DAMAGEPROC_END_HIT_L5 );

      scEvent_DamageProcEnd(  attacker, targets, wazaParam, fDelayAttack );
    m_HEManager.PopState( hem_state );
  }
}

/**
 *  技ダメージ後、なかよし度の高いポケモンがピンチになっていたら特殊演出
 */
void ServerFlow::scproc_Fight_Damage_FriendActPinch( PokeSet* targets )
{
  u32 count = targets->GetCount();
  for(u32 i=0; i<count; ++i)
  {
    const BTL_POKEPARAM* bpp = targets->Get( i );
    if( (getFriendship(bpp) >= 150)
    &&  !(bpp->IsDead())
    ){
      u32 now_hp = bpp->GetValue( BTL_POKEPARAM::BPP_HP );
      u32 max_hp = bpp->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
      if( ::app::tool::Gauge::GetDottoColor(now_hp, max_hp)  == ::app::tool::Gauge::GAUGE_COLOR_RED )
      {
        u8 pokeID = bpp->GetID();
        SCQUE_PUT_ACT_FriendshipEffectWithMsg( m_que, pokeID, FREFF_FURIMUKI, BTL_STRTYPE_STD, BTL_STRID_STD_FR_Pinch, pokeID, 0 );

      }
    }
  }
}

/**
 *  Event] ダメージワザ処理終了（ヒット時のみ下請け）
 */
void  ServerFlow::scEvent_DamageProcEndSub( const BTL_POKEPARAM* attacker, PokeSet* targets,
  const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, bool fDelayAttack, bool fRealHitOnly, BtlEventType eventID )
{
  const BTL_POKEPARAM* bpp;
  u32 damage, damage_sum, target_cnt, hit_cnt, i;
  bool fHit;

  target_cnt = targets->GetCount();
  hit_cnt = damage_sum = 0;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DELAY_ATTACK_FLAG, fDelayAttack );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
    EVENTVAR_SetConstValue( BTL_EVAR_OIUTI_INTERRUPT_FLAG, actionDesc->isOiutiInterruptAction );

    for(i=0; i<target_cnt; ++i)
    {
      fHit = false;
      bpp = targets->Get( i );
      if( fRealHitOnly )
      {
        fHit = targets->GetDamageReal( bpp, &damage );
      }
      else
      {
        fHit = targets->GetDamage( bpp, &damage );
      }

      if( fHit )
      {
        damage_sum += damage;
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+hit_cnt), bpp->GetID() );
        ++hit_cnt;
      }
    }
    if( hit_cnt )
    {
      EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, hit_cnt );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_RINPUNGUARD_FLG, false );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_TIKARAZUKU_FLG, false );
      EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage_sum );
      EVENT_CallHandlers( this, BTL_EVENT_DAMAGEPROC_END_HIT_PREV );

      if( EVENTVAR_GetValue(BTL_EVAR_TIKARAZUKU_FLG) == false ){
        EVENT_CallHandlers( this, eventID );
      }
    }
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] ダメージワザ処理終了
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_DamageProcEnd( const BTL_POKEPARAM* attacker, PokeSet* targets,
  const WAZAPARAM* wazaParam, bool fDelayAttack )
{
  GFL_UNUSED( targets );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DELAY_ATTACK_FLAG, fDelayAttack );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENT_CallHandlers( this, BTL_EVENT_DAMAGEPROC_END );
  EVENTVAR_Pop();
}

//------------------------------------------------------------------
// サーバーフロー：非ダメージワザシーケンス終了
//------------------------------------------------------------------
void ServerFlow::scproc_Fight_UnDamageProcEnd( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker )
{
  scEvent_UnDamageProcEnd( wazaParam, attacker );
}
void  ServerFlow::scEvent_UnDamageProcEnd( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENT_CallHandlers( this, BTL_EVENT_UNDAMAGEPROC_END );
  EVENTVAR_Pop();
}


//------------------------------------------------------------------
// サーバーフロー：ダメージ受け後の処理 > ひるみチェック
//------------------------------------------------------------------
void  ServerFlow::scproc_CheckShrink( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender )
{
  u32 waza_per = scEvent_GetWazaShrinkPer(  wazaParam->wazaID, attacker );

  scproc_AddShrinkCore( defender, waza_per );
}
//------------------------------------------------------------------
// サーバーフロー：ダメージ受け後の処理 > ほのおワザで「こおり」が治る処理
// 追加効果処理の後に置くこと。でないと「こおりがとけた」→「やけどになった」が起きてしまう。
//------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_KoriCure( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  GFL_UNUSED( attacker );

  if( wazaParam->wazaType == POKETYPE_HONOO )
  {
    BTL_POKEPARAM* bpp;

    targets->SeekStart();
    while( (bpp = targets->SeekNext()) != NULL )
    {
      if( (bpp->GetPokeSick() == pml::pokepara::SICK_KOORI)
      &&  (targets->GetDamageType(bpp) == PokeSet::DMGTYPE_REAL)
      ){
        scPut_CurePokeSick( bpp, pml::wazadata::WAZASICK_KOORI, true );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 対象ポケモンにひるみ効果を与える
 *
 * @param   wk
 * @param   target
 * @param   per     確率（パーセンテージ）
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_AddShrinkCore( BTL_POKEPARAM* target, u32 per )
{
  bool fShrink;

  if( target->TURNFLAG_Get( BTL_POKEPARAM::TURNFLG_KIAI_READY) )
  {
    target->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_KIAI_SHRINK );
    return true;
  }
  else
  {
    fShrink = scEvent_CheckShrink( target, per );
  }

  if( fShrink )
  {
    target->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_SHRINK );
    return true;
  }
  else if( per >= 100 )
  {
    // 確率100％なのに失敗したら原因表示へ
    u32 hem_state = m_HEManager.PushState();
    scEvent_FailShrink(  target );
    m_HEManager.PopState( hem_state );
  }
  return false;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザダメージに応じたHP吸い取り効果
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_Drain( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  u32 hem_state;

  if( WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_DRAIN )
  {
    BTL_POKEPARAM* bpp;
    u32 damage, recoverHP;

    hem_state = m_HEManager.PushState();
    targets->SeekStart();
    while( (bpp = targets->SeekNext()) != NULL )
    {
      if( targets->GetDamage(bpp, &damage) )
      {
        recoverHP = (WAZADATA_GetParam(waza, pml::wazadata::PARAM_ID_DAMAGE_RECOVER_RATIO) * damage) / 100;

        if( recoverHP > 0 )
        {
          if( scproc_DrainCore( attacker, bpp, recoverHP, false ) )
          {
            scPut_Message_Set( bpp, BTL_STRID_SET_Drain );
          }
        }
      }
    }
    m_HEManager.PopState( hem_state );
  }
}
void  ServerFlow::scproc_Damage_Drain( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
  if( WAZADATA_GetCategory(wazaParam->wazaID) == pml::wazadata::CATEGORY_DRAIN )
  {
    u32 ratio = WAZADATA_GetParam( wazaParam->wazaID, pml::wazadata::PARAM_ID_DAMAGE_RECOVER_RATIO );
    u32 recoverHP = calc::MulRatioInt( damage, ratio );

    if( scproc_DrainCore(attacker, defender, recoverHP, false ) )
    {
      scPut_Message_Set( defender, BTL_STRID_SET_Drain );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * HP吸い取り処理コア
 *
 * @param   wk
 * @param   attacker    吸い取る側ポケID
 * @param   target      吸い取られる側ポケID（NULLでも可：「ねをはる」を大きなねっこ対象にするため利用）
 * @param   drainHP
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_DrainCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP, bool fSkipSpFailCheck )
{
  u32 hem_state = m_HEManager.PushState();
  bool result = false;

  drainHP = scEvent_RecalcDrainVolume( attacker, target, drainHP );
  if( drainHP > 0 )
  {
    if( !scproc_RecoverHP_CheckFailBase(attacker) )
    {
      result = scproc_RecoverHP( attacker, drainHP, fSkipSpFailCheck, true, true );
    }
  }
//  else{
//    scproc_HandEx_Root( ITEM_DUMMY_DATA );
//  }
  m_HEManager.PopState( hem_state );
  return result;
}

//------------------------------------------------------------------
// サーバーフロー下請け： たたかう > ダメージワザ系 > ダメージ値計算
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * [Event] ダメージ計算
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   typeAff
 * @param   targetDmgRatio
 * @param   criticalFlag
 * @param   dstDamage       [out] 計算結果
 *
 * @retval  bool     固定ダメージ値（防御・攻撃等の値と関係なく与えるダメージ）の場合、true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CalcDamage(
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, bool criticalFlag, bool fSimurationMode, bool bMigawariEffective, u16* dstDamage )
{
  enum {
    PRINT_FLG = true,
  };
  u32  rawDamage = 0;
  bool fFixDamage = false;

  EVENTVAR_Push();

  EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, typeAff );
  EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
  EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
  EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
  EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
  EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
  EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
  EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
  EVENTVAR_SetConstValue( BTL_EVAR_WAZA_PRI, wazaParam->wazaPri );
  EVENTVAR_SetConstValue( BTL_EVAR_MIGAWARI_FLAG, bMigawariEffective );
  EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE, 0 );
  EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE_FLAG, false );

  EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_PROC1 );
  EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_PROC2 );
  rawDamage = EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE );
  fFixDamage = EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE_FLAG );

  if( !fFixDamage )
  {
    u32 fxDamage;
    u16 atkPower, defGuard, wazaPower;
    u8 atkLevel;
    bool bYakedoDisable;
    // 各種パラメータから素のダメージ値計算
    wazaPower = scEvent_getWazaPower( attacker, defender, wazaParam );
    atkPower  = scEvent_getAttackPower( attacker, defender, wazaParam, criticalFlag, &bYakedoDisable );
    defGuard  = scEvent_getDefenderGuard( attacker, defender, wazaParam, criticalFlag );
    {
      atkLevel = attacker->GetValue( BTL_POKEPARAM::BPP_LEVEL );
      fxDamage = calc::DamageBase( wazaPower, atkPower, atkLevel, defGuard );

      BTL_N_PrintfEx(PRINT_FLG, DBGSTR_CALCDMG_WazaParam, wazaParam->wazaID, wazaParam->wazaType);
      BTL_N_PrintfEx(PRINT_FLG, DBGSTR_CALCDMG_BaseDamage, fxDamage);
    }
    // ワザ対象数による補正
    if( targetDmgRatio != BTL_CALC_DMG_TARGET_RATIO_NONE ){
      fxDamage  = calc::MulRatio( fxDamage, targetDmgRatio );
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RangeHosei, fxDamage, targetDmgRatio);
      BTL_PRINT("  ダメージ計算：(%04x)で補正 -> %d\n", targetDmgRatio, fxDamage );
    }else{
      BTL_PRINT("  ダメージ計算：補正なし -> %d\n", fxDamage );
    }
    // 天候補正
    {
      BtlWeather weather = scEvent_GetWeather();
      fx32 weatherDmgRatio = calc::GetWeatherDmgRatio( weather, wazaParam->wazaType );
      if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
      {
        fxDamage = calc::MulRatio( fxDamage, weatherDmgRatio );
      }
    }
    // 素ダメージ値を確定
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RAW_DAMAGE,
        wazaPower, atkLevel, atkPower, defGuard, fxDamage );

    // クリティカルで1.5倍
    if( criticalFlag ){
      fxDamage = (fxDamage * 15) / 10;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_Critical, fxDamage);
    }
    //ランダム補正(100〜85％)
    if( !m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_DMG_RAND_OFF) )
    {
      u16 ratio;
      if( !fSimurationMode ){
        ratio = 100 - calc::GetRand( 16 );
      }else{
        ratio = 85;
      }
      fxDamage = (fxDamage * ratio) / 100;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RandomHosei, ratio, fxDamage);
    }
    // タイプ一致補正
    if( wazaParam->wazaType != POKETYPE_NULL )
    {
      fx32 ratio = scEvent_CalcTypeMatchRatio( attacker, wazaParam->wazaType );
      fxDamage = calc::MulRatio( fxDamage, ratio );
      if( ratio != FX32_CONST(1) ){
        BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeMatchHosei, (ratio*100>>FX32_SHIFT), fxDamage);
      }
    }
    // タイプ相性計算
    fxDamage = calc::AffDamage( fxDamage, typeAff );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeAffInfo, typeAff, fxDamage);
    // やけど補正
    if( (wazaParam->damageType == pml::wazadata::DMG_PHYSIC)
    &&  (attacker->GetPokeSick() == pml::pokepara::SICK_YAKEDO)
    &&  (!bYakedoDisable)
    ){
      fxDamage = (fxDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
    }
    rawDamage = fxDamage;

    // 各種ハンドラによる倍率計算
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
    EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WALL_EFFECT_FLAG_PHYSIC, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WALL_EFFECT_FLAG_SPECIAL, false );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_PROC3 );
    {
      fx32 ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
      rawDamage = EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
      rawDamage = calc::MulRatio( rawDamage, ratio );
      if( rawDamage == 0 ){
        rawDamage = 1;
      }
    }

  }

  // Z技に対する倍率計算
  //「ばけのかわ」などでダメージを防いだ場合( rawDamage == 0 の場合 )には適用しない
  if( ( m_wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL ) &&
      ( rawDamage != 0 ) )
  {
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_RATIO_EX, FX32_CONST(1) );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_PROC_Z );
    fx32 ratio = static_cast<fx32>( EVENTVAR_GetValue( BTL_EVAR_RATIO_EX ) );
    rawDamage = calc::MulRatio( rawDamage, ratio );
    if( rawDamage == 0 ){
      rawDamage = 1;
    }
  }
  
  BTL_PRINT("  最終ダメージ値：%d\n", rawDamage );

  EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_PROC_END );

  EVENTVAR_Pop();

  *dstDamage = rawDamage;
  return fFixDamage;
}
//---------------------------------------------------------------------------------------------
// ワザダメージ処理後の反動処理（ダメージ×反動率）
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_Kickback( BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage )
{
  const u8 attackerID = attacker->GetID();

  if( attacker->IsDead() ||
    ( m_posPoke.GetPokeExistPos( attackerID ) == BTL_POS_NULL ) )
  {
    return;
  }

  u32  damage;
  bool fMustEnable;

  damage = scEvent_CalcKickBack( attacker, waza, wazaDamage, &fMustEnable );

  if( damage )
  {
    HANDEX_STR_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_ReactionDmg );
    HANDEX_STR_AddArg( &m_strParam, attackerID );
    if( (fMustEnable)
    ||  (scproc_SimpleDamage_CheckEnable(attacker, damage, DAMAGE_CAUSE_OTHER))
    ){
      scproc_SimpleDamage_Core(attacker, damage, DAMAGE_CAUSE_WAZA_KICKBACK, attackerID, &m_strParam, true );
    }
  }
}
//---------------------------------------------------------------------------------------------
// ワザ以外のダメージ共通処理
//---------------------------------------------------------------------------------------------
/**
 *  ワザ以外ダメージ：有効チェック（有効ならtrue）
 */
bool  ServerFlow::scproc_SimpleDamage_CheckEnable( BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause )
{
  if( !scEvent_CheckEnableSimpleDamage(bpp, damage, damageCause) )
  {
    return false;
  }
  return true;
}
/**
 * @brief ワザ以外ダメージ：実行
 * @param bpp                ダメージを受けるポケモン
 * @param damage             ダメージ量
 * @param damageCause        ダメージの原因
 * @param damageCausePokeID  ダメージの原因となったポケモンのID
 * @param str                表示する文字列( 不要なら NULL )
 * @param doDeadProcess      死亡した場合に退場処理を実行するか？
 */
bool ServerFlow::scproc_SimpleDamage_Core( BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause, u8 damageCausePokeID, BTL_HANDEX_STR_PARAMS* str, bool doDeadProcess )
{
  int value = (int)damage * -1;

  if( value )
  {
    scEvent_SimpleDamage_Before( *bpp, damage );

    scPut_SimpleHp( bpp, value, damageCause, damageCausePokeID, true );
    bpp->TURNFLAG_Set( BTL_POKEPARAM::TURNFLG_DAMAGED );

    if( str != NULL ){
      handexSub_putString( str );
      HANDEX_STR_Clear( str );
    }

    scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_HP );

    scEvent_SimpleDamage_After( *bpp, damage );

    if( doDeadProcess &&
        scproc_CheckDeadCmd( bpp, false ) )
    {
      if( scproc_CheckShowdown() )
      {
        return true;
      }
    }

    return true;
  }

  return false;
}

/**
 * @brief [Event] ワザ以外ダメージを受ける直前
 * @param bpp     ダメージを受けるポケモン
 * @param damage  ダメージ量
 */
void ServerFlow::scEvent_SimpleDamage_Before( const BTL_POKEPARAM& bpp, u32 damage )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp.GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage );
    EVENT_CallHandlers( this, BTL_EVENT_SIMPLE_DAMAGE_BEFORE );
  EVENTVAR_Pop();
}

/**
 * @brief [Event] ワザ以外ダメージを受けた後( アイテム反応後 )
 * @param bpp     ダメージを受けるポケモン
 * @param damage  ダメージ量
 */
void ServerFlow::scEvent_SimpleDamage_After( const BTL_POKEPARAM& bpp, u32 damage )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp.GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage );
    EVENT_CallHandlers( this, BTL_EVENT_SIMPLE_DAMAGE_AFTER );
  EVENTVAR_Pop();
}

/**
 * @brief [Event] ワザ以外ダメージを防いだ場合
 * @param bpp                ダメージを防いだポケモン
 * @param damageCause        ダメージの原因
 * @param damageCausePokeID  ダメージの原因となったポケモンのID
 */
void ServerFlow::scEvent_SimpleDamage_Failed( const BTL_POKEPARAM& bpp, DamageCause damageCause, u8 damageCausePokeID )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, bpp.GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_CAUSE, damageCause );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_CAUSE_POKEID, damageCausePokeID );
    EVENT_CallHandlers( this, BTL_EVENT_SIMPLE_DAMAGE_FAILED );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Proc] 装備アイテム使用
 *
 * @param   bpp
 *
 * @retval  bool  装備アイテム使用したらtrue／装備なし・使用できない場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_UseItemEquip( BTL_POKEPARAM* bpp )
{
  u32 itemID = bpp->GetItem();
  u32 hem_state_1st;
  bool result = false;

  if( m_posPoke.IsExist(bpp->GetID()) )
  {
    hem_state_1st = m_HEManager.PushState();
    result = !scEvent_CheckItemEquipFail( bpp, itemID );
    if( result )
    {
      u32  itemChangedCount = ItemChangeCounter_Get( bpp );

      u32 hem_state_2nd;
      u16 que_reserve_pos;
      que_reserve_pos = scmd::SCQUE_RESERVE_Pos( m_que, SC_ACT_USE_ITEM );

      hem_state_2nd = m_HEManager.PushStateUseItem( itemID );
        scEvent_ItemEquip( bpp );
        if( scproc_HandEx_Result() != HandExResult_Enable ){
          result = false;
        }
      m_HEManager.PopState( hem_state_2nd );

      if( result )
      {
        bool  bUseNuts = ::item::ITEM_CheckNuts( itemID );
        scmd::SCQUE_PUT_ReservedPos( m_que, que_reserve_pos, SC_ACT_USE_ITEM, bpp->GetID(), bUseNuts );

        if( calc::ITEM_GetParam(itemID, item::ITEM_DATA::PRM_ID_SPEND) )
        {
          // イベント実行前からアイテム変更が起こっていないならここでアイテム削除処理
          if( ItemChangeCounter_Get(bpp) == itemChangedCount ){
            scproc_ItemChange( bpp, ITEM_DUMMY_DATA, true );
          }
          scproc_AfterItemEquip( bpp, itemID, true );
        }
      }
    }

    m_HEManager.PopState( hem_state_1st );
  }
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 装備アイテム使用可否チェック
 *
 * @param   bpp
 * @param   itemID
 *
 * @retval  bool    特殊な原因で使用できなかったらtrue／それ以外false
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckItemEquipFail( const BTL_POKEPARAM* bpp, u16 itemID )
{
  bool failFlag = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_ITEMEQUIP_FAIL );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  return failFlag;
}


//----------------------------------------------------------------------------------
/**
 * 装備アイテム効果発動後の処理
 *
 * @param   bpp
 * @param   itemID
 *
 */
//----------------------------------------------------------------------------------
void ServerFlow::scproc_AfterItemEquip( BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomiAte )
{
  if( bCheckKinomiAte )
  {
    if( ::item::ITEM_CheckNuts(itemID) )
    {
      scPut_SetPermFlag( bpp, BTL_POKEPARAM::PERMFLAG_ATE_KINOMI );
    }
  }
  scproc_CallAfterItemEquipEvent( bpp, itemID, bCheckKinomiAte );
}
void ServerFlow::scproc_CallAfterItemEquipEvent( BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomi )
{
  BTL_PRINT(" call after itemEquip Event, pokeID=%d, kinomiFlg=%d\n", bpp->GetID(), bCheckKinomi);
  u32 hem_state = m_HEManager.PushState();
    scEvent_AfterItemEquip( bpp, itemID, bCheckKinomi );
  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] 装備アイテム使用後イベント
 *
 * @param   bpp
 * @param   itemID
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_AfterItemEquip( const BTL_POKEPARAM* bpp, u16 itemID, bool bCheckKinomi )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    EVENTVAR_SetConstValue( BTL_EVAR_KINOMI_FLAG, bCheckKinomi );
    EVENT_CallHandlers( this, BTL_EVENT_AFTER_ITEMEQUIP );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
// アイテム書き換え共通処理
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_ItemChange( BTL_POKEPARAM* bpp, u16 nextItemID, bool fConsume )
{
  u8 pokeID = bpp->GetID();
  u16 prevItemID = bpp->GetItem();
  u32 hem_state;

  // アイテム書き換え確定ハンドラ呼び出し
  hem_state = m_HEManager.PushState();
  scEvent_ItemSetDecide( bpp, nextItemID );
  m_HEManager.PopState( hem_state );
  if( nextItemID == ITEM_DUMMY_DATA ){
    scPut_SetContFlag( bpp, BTL_POKEPARAM::CONTFLG_ITEM_LOSE );
  }

  handler::item::Remove( bpp );
  SCQUE_PUT_OP_SetItem( m_que, pokeID, nextItemID );
  bpp->SetItem( nextItemID );

  SCQUE_PUT_OP_PublishClientInformation_HavePokemonItem( m_que, pokeID, ( nextItemID != ITEM_DUMMY_DATA ) );

  // アイテム書き換え完了ハンドラ呼び出し
  if( nextItemID != ITEM_DUMMY_DATA ){
    handler::item::Add( bpp );
  }

  hem_state = m_HEManager.PushState();
  scEvent_ItemSetFixed( bpp );
  m_HEManager.PopState( hem_state );

  ItemChangeCounter_Increment( bpp );

  // 消費レコード
  if( fConsume )
  {
    bpp->ConsumeItem( prevItemID );
    SCQUE_PUT_OP_ConsumeItem( m_que, pokeID, prevItemID );
    scPut_SetTurnFlag( bpp, BTL_POKEPARAM::TURNFLG_ITEM_CONSUMED );
  }
}
//---------------------------------------------------------------------------------------------
/**
 * @brief ポケモンを強制的に瀕死にする
 * @param bpp            瀕死にするポケモン
 * @param atkPokeID      攻撃したポケモンのID
 * @param recParam       ひん死記録をPGL集計データにするためのパラメータ（NULLの時、相手ポケモンなどのデータを不明として記録する）
 * @param doDeadProcess  瀕死になったポケモンの退場処理を行うか？
 */
//---------------------------------------------------------------------------------------------
void ServerFlow::scproc_KillPokemon(BTL_POKEPARAM* bpp, u8 atkPokeID, DamageCause deadCause, const MainModule::PGLRecParam* recParam, bool doDeadProcess )
{
  scEvent_KillHandler( bpp, atkPokeID );
  scPut_KillPokemon( bpp, atkPokeID, deadCause, 0 );

  if( doDeadProcess )
  {
    scproc_CheckDeadCmd( bpp, false, recParam );
  }
}
/**
 * [Event] 強制ひん死処理が呼び出されたことを告知
 */
void  ServerFlow::scEvent_KillHandler( const BTL_POKEPARAM* target, u8 atkPokeID )
{
  u8  targetPokeID = target->GetID();

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, targetPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    EVENT_CallHandlers( this, BTL_EVENT_KILL_HANDLER );
  EVENTVAR_Pop();
}

//---------------------------------------------------------------------------------------------
// サーバーフロー：ダメージワザの追加効果による状態異常
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_AddSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick;
  BTL_SICKCONT sick_cont;

  sick = scEvent_CheckWazaAddSick( wazaParam->wazaID, attacker, target, &sick_cont );
  if( sick != pml::wazadata::WAZASICK_NULL )
  {
    if( !target->IsDead() ){
      scproc_Fight_WazaSickCore( attacker, target, wazaParam->wazaID, sick, sick_cont, SICK_CAUSE_OTHER, WAZASERIAL_NULL, false, false );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザ追加効果による状態異常の発生チェック
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   [out] pSickCont   継続効果
 *
 * @retval  WazaSick          状態異常ID
 */
//--------------------------------------------------------------------------
WazaSick  ServerFlow::scEvent_CheckWazaAddSick( WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_SICKCONT* pSickCont )
{
  BTL_SICKCONT sickCont;
  WazaSick sick = static_cast<WazaSick>(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SICK ));
  SICKCONT_PARAM  waza_contParam = WAZADATA_GetSickCont( waza );
  int per = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SICK_PER );
  u8 fFail = false;

  calc::WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_SICK_CONT_ADRS, reinterpret_cast<int>( &sickCont ) );
    EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );

    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    EVENT_CallHandlers( this, BTL_EVENT_ADD_SICK );

    sick = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
    per  = EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( sick == pml::wazadata::WAZASICK_SPECIAL_CODE ){
      sick = pml::wazadata::WAZASICK_NULL;
    }

  EVENTVAR_Pop();

  if( !fFail && per > 0)
  {
    if( sick != pml::wazadata::WAZASICK_NULL )
    {
      if( perOccur(per) ){
        *pSickCont = sickCont;
        return sick;
      }
      // デバッグ機能により必ず発生
      if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_MUST_TUIKA) ){
        *pSickCont = sickCont;
        return sick;
      }
    }
  }
  return  pml::wazadata::WAZASICK_NULL;
}

//--------------------------------------------------------------------------
/**
 * 【Event】特殊なワザハンドラによる追加効果の確率変動チェック
 *
 * @param   atkPokeID
 * @param   defPokeID
 * @param   defaultPer    デフォルトの確率（％）
 *
 * @retval  u32            変動後の確率（％）
 */
//--------------------------------------------------------------------------
u32  ServerFlow::scEvent_CheckSpecialWazaAdditionalPer(
  u8 atkPokeID, u8 defPokeID, u32 defaultPer )
{
  u32 resultPer;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    EVENTVAR_SetValue( BTL_EVAR_ADD_PER, defaultPer );
    EVENT_CallHandlers( this, BTL_EVENT_SP_ADDITIONAL_PER );

    resultPer  = EVENTVAR_GetValue( BTL_EVAR_ADD_PER );

  EVENTVAR_Pop();

  return resultPer;
}

//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる直接の状態異常
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_SimpleSick( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  BTL_POKEPARAM* target;
  WazaSick sick;
  SICKCONT_PARAM contParam;
  bool fSucceed;
  u32  targetCnt;

  sick = static_cast<WazaSick>(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SICK ));
  contParam = WAZADATA_GetSickCont( waza );
  fSucceed = false;

  targetCnt = targetRec->GetCount();

  if( targetCnt )
  {
    targetRec->SeekStart();
    while( (target = targetRec->SeekNext()) != NULL )
    {
      BTL_SICKCONT sickCont;
      calc::WazaSickContToBppSickCont( contParam, attacker, &sickCont );
      if( scproc_Fight_WazaSickCore( attacker, target, waza, sick, sickCont, SICK_CAUSE_OTHER, WAZASERIAL_NULL, true, false) ){
        wazaEffCtrl_SetEnable( m_wazaEffCtrl );
        fSucceed = true;  // ターゲットが居て、１体でも状態異常にかかれば成功
      }
    }

    if( !fSucceed )
    {
      if( !m_fWazaFailMsgDisped ){
        scPut_WazaFail( attacker, waza );
      }
    }
  }
  else{
    // 既にターゲットが無くなっている->しかしうまく決まらなかった
    scPut_WazaFail( attacker, waza );
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる状態異常共通
//---------------------------------------------------------------------------------------------
bool ServerFlow::scproc_Fight_WazaSickCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BTL_SICKCONT sickCont, SickCause cause, u32 wazaSerial, bool fAlmost, bool fOtherEffectEnabled )
{
  GFL_UNUSED( waza );

  HANDEX_STR_Clear( &m_strParam );

  if( sick == pml::wazadata::WAZASICK_SPECIAL_CODE )
  {
    sick = scEvent_DecideSpecialWazaSick(  attacker, target, &m_strParam );
    if( (sick == pml::wazadata::WAZASICK_NULL) || (sick == pml::wazadata::WAZASICK_SPECIAL_CODE) ){
      return false;
    }
  }
  else{
    scEvent_GetWazaSickAddStr( sick, attacker, target, &m_strParam );
  }
  scEvent_WazaSickCont( attacker, target, sick, &sickCont );

  {
    // 特殊メッセージが何も用意されていなければ、scproc_AddSick 内で、
    // できるだけ標準メッセージを出すようにしている。
    bool fDefaultMsg = !HANDEX_STR_IsEnable(&m_strParam);

    if( scproc_AddSickRoot(target, attacker, sick, sickCont, cause, wazaSerial, fAlmost, fOtherEffectEnabled, fDefaultMsg) )
    {
      if( !fDefaultMsg )
      {
        handexSub_putString( &m_strParam );
        HANDEX_STR_Clear( &m_strParam );
      }
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによる特殊処理の状態異常ID＆成功時メッセージを決定する
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   str
 *
 * @retval  WazaSick
 */
//----------------------------------------------------------------------------------
WazaSick  ServerFlow::scEvent_DecideSpecialWazaSick(
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str )
{
  WazaSick sickID;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(str) );
    EVENTVAR_SetValue( BTL_EVAR_SICKID, pml::wazadata::WAZASICK_NULL );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASICK_SPECIAL );
    sickID = static_cast<WazaSick>(EVENTVAR_GetValue( BTL_EVAR_SICKID ));
  EVENTVAR_Pop();
  return sickID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによる通常の状態異常に対する成功時メッセージを決定する
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   str
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_GetWazaSickAddStr( WazaSick sick,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(str) );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASICK_STR );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによる状態異常の継続パラメータ調整
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   sick
 * @param   sickCont    [io]調整前パラメータ／調整後の結果も格納
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaSickCont( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BTL_SICKCONT* sickCont )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_SetValue( BTL_EVAR_SICK_CONT_ADRS, reinterpret_cast<int>( sickCont ) );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASICK_PARAM );
  EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [proc] 状態異常処理ルート
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fAlmost            失敗した時に原因メッセージを表示する
 *
 * @retval  bool       成功した場合true
 */
//--------------------------------------------------------------------------
bool ServerFlow::scproc_AddSickRoot( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, const BTL_SICKCONT& sickCont, SickCause sickCause, u32 wazaSerial, bool fAlmost, bool fOtherEffectEnabled, bool fDefaultMsgEnable )
{
  if( !scproc_AddSickCheckFail(target, attacker, sick, sickCont, sickCause, wazaSerial, SICK_OW_NONE, fAlmost, fOtherEffectEnabled) )
  {
    scproc_AddSickCore( target, attacker, sick, sickCont, fDefaultMsgEnable, false, NULL );
    return true;
  }
  return false;
}
/**
* 状態異常成否チェック
*/
bool ServerFlow::scproc_AddSickCheckFail( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, const BTL_SICKCONT& sickCont, SickCause cause, u32 wazaSerial, SickOverWriteMode overWriteMode, bool fDispFailResult, bool fOtherEffectEnabled )
{
  AddSickFailCode fail_code = ADDSICK_FAIL_NULL;
  
  // 上書きルールによる失敗チェック
  fail_code = addsick_check_fail_overwrite( target, sick, overWriteMode );

  // 基本ルールによる失敗チェック
  if( fail_code == ADDSICK_FAIL_NULL )
  {
    if( !scEvent_CheckAddSickFailStdSkip( target, attacker, sick, cause ) )
    {
      fail_code = addsick_check_fail_std( target, sick, sickCont );
    }
  }

  if( fail_code != ADDSICK_FAIL_NULL )
  {
    BTL_N_Printf( DBGSTR_SVFL_AddSickFailCode, fail_code );
    if( fDispFailResult 
    &&  ((cause != SICK_CAUSE_WAZA_EFFECT_SICK)||(fail_code != ADDSICK_FAIL_OTHER)||(fOtherEffectEnabled==false))
    ){
      scPut_AddSickFail( target, fail_code, sick, cause );
    }
    return true;
  }
  // とくせいなど特殊条件による失敗
  else
  {
    u32 hem_state = m_HEManager.PushState();

    bool fFail = scEvent_WazaSick_CheckFail( attacker, target, sick, cause, &fDispFailResult );
    BTL_PRINT("[svflow] fFail=%d, fDispFailResult=%d\n", fFail, fDispFailResult );

    if( fFail )
    {
      if( fDispFailResult )
      {
        scEvent_AddSick_Failed( target, attacker, sick, cause, wazaSerial );
        m_fWazaFailMsgDisped = true;
      }
    }

    m_HEManager.PopState( hem_state );
    return fFail;
  }
}

/**
 * @brief 状態異常失敗チェック( 上書きルール )
 * @param target         チェック対象のポケモン
 * @param sick           チェック対象の状態異常
 * @param overWriteMode  状態異常の上書きモード
 * @retval ADDSICK_FAIL_NULL     状態異常に出来る
 * @retval ADDSICK_FAIL_ALREADY  既に同じ状態異常になっているので失敗
 * @retval ADDSICK_FAIL_OTHER    他の基本状態異常になっているので失敗
 */
ServerFlow::AddSickFailCode ServerFlow::addsick_check_fail_overwrite( const BTL_POKEPARAM* target, WazaSick sick, SickOverWriteMode overWriteMode )
{
  // すでに同じ状態異常になっているなら失敗
  if( target->CheckSick(sick) )
  {
    if( overWriteMode != SICK_OW_STRONG )
    {
      return ADDSICK_FAIL_ALREADY;
    }
  }

  // すでに基本状態異常になっているなら、他の基本状態異常にはならない
  if( ( sick < pml::pokepara::SICK_MAX ) &&
      ( target->GetPokeSick() != pml::pokepara::SICK_NULL ) )
  {
    if( overWriteMode == SICK_OW_NONE )
    {
      return ADDSICK_FAIL_OTHER;
    }
  }

  return ADDSICK_FAIL_NULL;
}

/**
 *  状態異常失敗チェック（基本ルール）
 */
ServerFlow::AddSickFailCode ServerFlow::addsick_check_fail_std( const BTL_POKEPARAM* target, WazaSick sick, const BTL_SICKCONT& sickCont )
{
  GFL_UNUSED( sickCont );

  // てんこう「はれ」の時に「こおり」にはならない
  if( (calc::IsShineWeather( scEvent_GetWeather() ))//@バトル変更箇所
  &&  (sick == pml::pokepara::SICK_KOORI)
  ){
    return ADDSICK_FAIL_OTHER;
  }

  // はがね or どくタイプは、「どく」にならない
  if( sick==pml::wazadata::WAZASICK_DOKU )
  {
    PokeTypePair type = target->GetPokeType();
    if( PokeTypePair_IsMatch(type, POKETYPE_HAGANE)
    ||  PokeTypePair_IsMatch(type, POKETYPE_DOKU)
    ){
      return ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // ほのおタイプは、「やけど」にならない
  if( sick==pml::wazadata::WAZASICK_YAKEDO )
  {
    PokeTypePair type = target->GetPokeType();
    if( PokeTypePair_IsMatch(type, POKETYPE_HONOO) ){
      return ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // こおりタイプは、「こおり」にならない
  if( sick==pml::wazadata::WAZASICK_KOORI )
  {
    PokeTypePair type = target->GetPokeType();
    if( PokeTypePair_IsMatch(type, POKETYPE_KOORI) ){
      return ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // くさタイプは、「やどりぎのタネ」にならない
  if( sick==pml::wazadata::WAZASICK_YADORIGI )
  {
    PokeTypePair type = target->GetPokeType();
    if( PokeTypePair_IsMatch(type, POKETYPE_KUSA) ){
      return ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // でんきタイプは、「マヒ」にならない
  if( sick==pml::wazadata::WAZASICK_MAHI )
  {
    PokeTypePair type = target->GetPokeType();
    if( PokeTypePair_IsMatch(type, POKETYPE_DENKI) ){
      return ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // 基本状態異常の時には「あくび」にならない
  if( sick == pml::wazadata::WAZASICK_AKUBI )
  {
    if( target->GetPokeSick() != pml::pokepara::SICK_NULL ){
      return ADDSICK_FAIL_OTHER;
    }
  }

  // マルチタイプ＆バトルスイッチは「いえき」にならない
  if( sick == pml::wazadata::WAZASICK_IEKI )
  {
    TokuseiNo tokusei = static_cast<TokuseiNo>( target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ) );

    if( tables::IsNeverChangeTokusei(tokusei) ){
      return ADDSICK_FAIL_OTHER;
    }
  }

  return ADDSICK_FAIL_NULL;
}

/**
 * @brief [Event] 基本ルールによる状態異常の失敗チェックをスキップするか？
 * @param target    状態異常になるポケモン
 * @param attacker  状態異常にする技を撃つポケモン( 技による状態異常チェックでない場合 NULL )
 * @param sick      チェック対象の状態異常
 * @param cause     状態異常の原因
 * @retval true   スキップする
 * @retval false  スキップしない
 */
bool ServerFlow::scEvent_CheckAddSickFailStdSkip( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker, WazaSick sick, SickCause cause )
{
  if( attacker == NULL )
  {
    return false;
  }

  bool skipEnable = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_SetConstValue( BTL_EVAR_SICK_CAUSE, cause );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, skipEnable );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_ADDSICK_FAIL_STD_SKIP );
    skipEnable = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();
  return skipEnable;
}

//----------------------------------------------------------------------------------
/**
 * 状態異常処理コア（確定後）
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fDefaultMsgEnable
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_AddSickCore( BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, const BTL_SICKCONT& sickCont, bool fDefaultMsgEnable, bool fItemReactionDisable, const BTL_HANDEX_STR_PARAMS* exStr )
{
  scPut_AddSick( target, sick, sickCont );

  // エフェクトの用意されている異常はそれを表示
  switch( sick ){
  case pml::wazadata::WAZASICK_DOKU:     scPut_EffectByPokePos( target, BTLEFF_DOKU ); break;
  case pml::wazadata::WAZASICK_YAKEDO:   scPut_EffectByPokePos( target, BTLEFF_YAKEDO ); break;
  case pml::wazadata::WAZASICK_MAHI:     scPut_EffectByPokePos( target, BTLEFF_MAHI ); break;
  case pml::wazadata::WAZASICK_KOORI:    scPut_EffectByPokePos( target, BTLEFF_KOORI ); break;
  case pml::wazadata::WAZASICK_NEMURI:   scPut_EffectByPokePos( target, BTLEFF_NEMURI ); break;
  case pml::wazadata::WAZASICK_KONRAN:   scPut_EffectByPokePos( target, BTLEFF_KONRAN ); break;
  case pml::wazadata::WAZASICK_MEROMERO: scPut_EffectByPokePos( target, BTLEFF_MEROMERO ); break;
  }

  // 状態異常確定標準メッセージ出力
  if( fDefaultMsgEnable )
  {
    BTL_SICK_MakeDefaultMsg( sick, sickCont, target, &m_strParam );
    handexSub_putString( &m_strParam );
    HANDEX_STR_Clear( &m_strParam );
  }
  else if( exStr != NULL ){
    handexSub_putString( exStr );
  }

  // シェイミがランドフォルムに戻る
  if( (sick == pml::wazadata::WAZASICK_KOORI)
  &&  (target->GetMonsNo() == MONSNO_SHEIMI)
  &&  (target->GetFormNo() == FORMNO_SHEIMI_SKY)
  ){
    //@バトル変更箇所
    //SBTS[1916] フォルム戻り時の特殊天候取り消し
    scEvent_ChangePokeBefore(target);

    handler::tokusei::Remove( target );
    target->ChangeForm( FORMNO_SHEIMI_LAND );
    SCQUE_PUT_ACT_ChangeForm( m_que, target->GetID(), FORMNO_SHEIMI_LAND, false );
    handler::tokusei::Add( target );

    scPut_Message_Set( target, BTL_STRID_SET_ChangeForm );
  }

  // 状態異常確定イベントコール
  {
    u32 hem_state = m_HEManager.PushState();

    if( calc::IsBasicSickID(sick) ){
      scEvent_PokeSickFixed( target, attacker, static_cast<PokeSick>(sick), sickCont );
    }
    else if( sick == pml::wazadata::WAZASICK_IEKI )
    {
      scEvent_IekiFixed( target );
      if( target->GetValue(BTL_POKEPARAM::BPP_TOKUSEI) == TOKUSEI_KINTYOUKAN )
      {
        scproc_KintyoukanMoved( target );
      }
    }else {
      scEvent_WazaSickFixed( target, attacker, sick );
    }
    m_HEManager.PopState( hem_state );
  }

  // アイテム反応イベントへ
  if( !fItemReactionDisable ){
    scproc_CheckItemReaction( target, BTL_ITEMREACTION_SICK );
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] 天候チェック
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//----------------------------------------------------------------------------------
BtlWeather  ServerFlow::scEvent_GetWeather( void )
{
  bool fDisable = false;
  EVENTVAR_Push();
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_WEATHER_CHECK );
    fDisable = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  if( fDisable ){
    return BTL_WEATHER_NONE;
  }

  return m_fieldStatus->GetWeather();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 体重変化倍率取得（ライトメタル・ヘヴィメタルなど）
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
fx32 ServerFlow::svEvent_GetWaitRatio( const BTL_POKEPARAM* bpp )
{
  fx32 ratio;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    EVENT_CallHandlers( this, BTL_EVENT_WEIGHT_RATIO );
    ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
  EVENTVAR_Pop();

  return ratio;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 状態異常を失敗するケースのチェック
 *
 * @param   wk
 * @param   target
 * @param   sick
 *
 * @retval  bool    失敗する場合はtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_WazaSick_CheckFail( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaSick sick, SickCause cause, bool* bCallFailedEvent )
{
  bool fFail = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, (attacker!=NULL)? attacker->GetID() : BTL_POKEID_NULL );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_SetConstValue( BTL_EVAR_SICK_CAUSE, cause );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_ALMOST_FLAG, *bCallFailedEvent );
    EVENT_CallHandlers( this, BTL_EVENT_ADDSICK_CHECKFAIL );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    *bCallFailedEvent = EVENTVAR_GetValue( BTL_EVAR_ALMOST_FLAG );
  EVENTVAR_Pop();
  return fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 状態異常失敗が確定した
 *
 * @param   wk
 * @param   target
 * @param   sick
 */
//----------------------------------------------------------------------------------
void ServerFlow::scEvent_AddSick_Failed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick, SickCause cause, u32 wazaSerial )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_SetConstValue( BTL_EVAR_SICK_CAUSE, cause );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_SERIAL, wazaSerial );
    EVENT_CallHandlers( this, BTL_EVENT_ADDSICK_FAILED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ポケモン系状態異常確定
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_PokeSickFixed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
  PokeSick sick, BTL_SICKCONT sickCont )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    {
      u8 atkPokeID = (attacker!=NULL)? attacker->GetID() : BTL_POKEID_NULL;
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENTVAR_SetConstValue( BTL_EVAR_SICK_CONT_ADRS, reinterpret_cast<int>( &sickCont ) );
    EVENT_CallHandlers( this, BTL_EVENT_POKESICK_FIXED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ系状態異常確定
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaSickFixed( const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    {
      u8 atkPokeID = (attacker!=NULL)? attacker->GetID() : BTL_POKEID_NULL;
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    EVENT_CallHandlers( this, BTL_EVENT_WAZASICK_FIXED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] いえきによる特性無効化の確定
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_IekiFixed( const BTL_POKEPARAM* target )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, target->GetID() );
    EVENT_ForceCallHandlers( this, BTL_EVENT_IEKI_FIXED );
  EVENTVAR_Pop();
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ダメージワザの追加効果によるランク効果
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Damage_AddEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  if( !target->IsDead() )
  {
    if( scEvent_CheckAddRankEffectOccur(wazaParam, attacker, target) )
    {
      u32 wazaSerial = wazaSerial_Inc();
      scproc_WazaRankEffect_Common( wazaParam, attacker, target, wazaSerial, false , false );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザによる追加ランク効果の発生チェック
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   waza
 *
 * @retval  bool    発生したらtrue
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckAddRankEffectOccur( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  u8 per = WAZADATA_GetParam( wazaParam->wazaID, pml::wazadata::PARAM_ID_RANKPER_1 );
  u8 failFlag;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );

    EVENT_CallHandlers( this, BTL_EVENT_ADD_RANK_TARGET );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    per = EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  EVENTVAR_Pop();

  if( !failFlag )
  {
    if( perOccur(per) ){
      return true;
    }
    // デバッグ機能により必ず発生
    if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_MUST_TUIKA) ){
      return true;
    }
  }

  return false;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる直接のランク効果
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_SimpleEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  BTL_POKEPARAM* target;
  bool bEffective = false;
  u32  wazaSerial = wazaSerial_Inc();

  targetRec->SeekStart();
  while( (target = targetRec->SeekNext()) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wazaParam, attacker, target, wazaSerial, true, false) )
    {
      bEffective = true;
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }
  }

  if( !bEffective
  &&  !m_fWazaFailMsgDisped
  ){
    scPut_WazaFail( attacker, wazaParam->wazaID );
  }
}
/**
 *  能力ランク効果処理のシリアルをインクリメント
 */
u32 ServerFlow::wazaSerial_Inc( void )
{
  m_wazaRankEffSerial++;
  if( m_wazaRankEffSerial == 0 ){
    m_wazaRankEffSerial = 1;
  }
  return m_wazaRankEffSerial;
}
//--------------------------------------------------------------------------
/**
 * ワザによる（直接・追加共通）ランク増減効果の処理
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 *
 * @retval  bool    ランク増減効果が発生したらtrue
 */
//--------------------------------------------------------------------------
bool ServerFlow::scproc_WazaRankEffect_Common( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u32 wazaSerial, bool fAlmost, bool fOnlyCheck )
{
  bool ret = false;
  u32 eff_cnt;
  u8 atkPokeID = attacker->GetID();
  u8 bMigawariThrew = WAZADATA_GetFlag( wazaParam->wazaID, pml::wazadata::WAZAFLAG_MigawariThru );

  if( !bMigawariThrew ){
    bMigawariThrew = scEvent_CheckMigawariThrew( attacker, target, wazaParam->wazaID );
  }

  eff_cnt = WAZADATA_GetRankEffectCount( wazaParam->wazaID );
  for(u32 i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;
    bool fEffective = false;

    scEvent_GetWazaRankEffectValue( wazaParam->wazaID, i, attacker, target, &effect, &volume );

    if( effect == pml::wazadata::RANKEFF_NULL ){
      m_simpleEffectFailManager.AddCause( SimpleEffectFailManager::CAUSE_SELF );
      continue;
    }

    if( effect != pml::wazadata::RANKEFF_NULL )
    {
      if( effect != pml::wazadata::RANKEFF_MULTI5 )
      {
        fEffective = ( fOnlyCheck ) ? 
          ( scproc_RankEffectCore_CheckEffective( atkPokeID, target, effect, volume, RANKEFFECT_CAUSE_OTHER, atkPokeID, ITEM_DUMMY_DATA, wazaSerial, fAlmost, bMigawariThrew, true ) ) :
          ( scproc_RankEffectCore( atkPokeID, target, effect, volume, RANKEFFECT_CAUSE_OTHER, atkPokeID, ITEM_DUMMY_DATA, wazaSerial, fAlmost, true, NULL, bMigawariThrew, true ) );
      }
      else
      {
        for( u32 e=pml::wazadata::RANKEFF_ORIGIN; e<pml::wazadata::RANKEFF_BASE_MAX; ++e )
        {
          bool is_effective = ( fOnlyCheck ) ? 
            ( scproc_RankEffectCore_CheckEffective( atkPokeID, target, static_cast<WazaRankEffect>(e), volume, RANKEFFECT_CAUSE_OTHER, atkPokeID, ITEM_DUMMY_DATA, wazaSerial, fAlmost, bMigawariThrew, true ) ) : 
            ( scproc_RankEffectCore( atkPokeID, target, static_cast<WazaRankEffect>(e), volume, RANKEFFECT_CAUSE_OTHER, atkPokeID, ITEM_DUMMY_DATA, wazaSerial, fAlmost, true, NULL, bMigawariThrew, true ) );

          if( is_effective )
          {
            fEffective = true;
          }
        }
      }
      if( fEffective )
      {
        if( !fOnlyCheck )
        {
          u32 hem_state = m_HEManager.PushState();
          scEvent_WazaRankEffectFixed( target, wazaParam->wazaID, effect, volume );
          m_HEManager.PopState( hem_state );
        }
        ret = true;
      }
    }
  }
  return ret;
}

//--------------------------------------------------------------------------
/**
 * [Event] ワザによる（直接・追加）ランク増減効果を取得
 *
 * @param   wk
 * @param   waza
 * @param   idx
 * @param   attacker
 * @param   target
 * @param   effect    [out] 効果種類
 * @param   volume    [out] 効果値
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_GetWazaRankEffectValue( WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume )
{
  *effect = WAZADATA_GetRankEffect( waza, idx, volume );
  BTL_PRINT("ポケ[%d],技(%d) の Rank効果[%d] = %d\n", attacker->GetID(), waza, idx, *effect );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, *effect );
    EVENTVAR_SetValue( BTL_EVAR_VOLUME, *volume );
    EVENTVAR_SetValue( BTL_EVAR_RATIO, 1 );
    EVENT_CallHandlers( this, BTL_EVENT_GET_RANKEFF_VALUE );
    *effect = static_cast<WazaRankEffect>(EVENTVAR_GetValue( BTL_EVAR_STATUS_TYPE ));
    *volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    {
      u8 ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
      if( ratio > 1 ){
        (*volume) *= ratio;
      }
    }
  EVENTVAR_Pop();

  if( *effect == pml::wazadata::RANKEFF_SP ){
    *effect = pml::wazadata::RANKEFF_NULL;
  };
}

//--------------------------------------------------------------------------
/**
 * ランク増減効果のコア（ワザ以外もこれを呼び出す）
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   cause            ランク増減効果の原因
 * @param   wazaUsePokeID    ワザによるランク増減ならワザ使用者ポケID（それ以外は BTL_POKEID_NULL）
 * @param   itemID           アイテム使用によるランク増減ならアイテムID指定（それ以外は ITEM_DUMMY_DATA を指定）
 * @param   fAlmost          特殊要因で効果失敗した時、そのメッセージを表示する
 * @param   fStdMsg          標準メッセージを出力する（○○は××があがった！など）
 * @param   bMigawariThrew   みがわりを貫通するフラグ
 * @param   fViewEffect      ランク増減演出を再生するか？
 *
 * @retval  true   ランク増減効果が有効だった
 * @retval  false  ランク増減効果が無効だった
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scproc_RankEffectCore(
  u8 atkPokeID, 
  BTL_POKEPARAM* target,
  WazaRankEffect effect,
  int volume, 
  RankEffectCause cause,
  u8 wazaUsePokeID,
  u16 itemID, 
  u32 rankEffSerial,
  bool fAlmost,
  bool fStdMsg, 
  const BTL_HANDEX_STR_PARAMS* preMsg,
  bool bMigawariThrew,
  bool fViewEffect )
{
  bool isEffective = scproc_RankEffectCore_CheckEffective( atkPokeID, target, effect, volume, cause, wazaUsePokeID, itemID, rankEffSerial, fAlmost, bMigawariThrew, false );

  if( isEffective )
  {
    if( preMsg )
    {
      handexSub_putString( preMsg );
    }

    volume = scEvent_RankValueChange( target, effect, wazaUsePokeID, itemID, volume );
    scPut_RankEffect( target, effect, volume, itemID, fStdMsg, fViewEffect );

    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_RankEffect_Fix( atkPokeID, target, effect, volume );
      m_HEManager.PopState( hem_state );
    }
  }

  return isEffective;
}

//--------------------------------------------------------------------------
/**
 * ランク増減効果のコア有効チェック（ワザ以外もこれを呼び出す）
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   cause            ランク増減効果の原因
 * @param   wazaUsePokeID    ワザによるランク増減ならワザ使用者ポケID（それ以外は BTL_POKEID_NULL）
 * @param   itemID           アイテム使用によるランク増減ならアイテムID指定（それ以外は ITEM_DUMMY_DATA を指定）
 * @param   fAlmost          特殊要因で効果失敗した時、そのメッセージを表示する
 * @param   bMigawariThrew   みがわりを貫通するフラグ
 * @param   fCheckOnly       判定のみを行い、結果メッセージなどのリアクションコマンドを生成しない
 *
 * @retval  true   ランク増減効果が有効だった
 * @retval  false  ランク増減効果が無効だった
 */
//--------------------------------------------------------------------------
bool ServerFlow::scproc_RankEffectCore_CheckEffective(
  u8 atkPokeID, 
  BTL_POKEPARAM* target,
  WazaRankEffect effect,
  int volume, 
  RankEffectCause cause,
  u8 wazaUsePokeID,
  u16 itemID, 
  u32 rankEffSerial,
  bool fAlmost,
  bool bMigawariThrew,
  bool bCheckOnly )
{
  // 決着がついていたら何もしない
  if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() ){
    return false;
  }

  volume = scEvent_RankValueChange( target, effect, wazaUsePokeID, itemID, volume );
  BTL_N_Printf( DBGSTR_SVFL_RankEffCore, target->GetID(), effect, volume, atkPokeID );

  // すでに限界ならそれを表示
  if( !target->IsRankEffectValid(static_cast<BTL_POKEPARAM::ValueID>(effect), volume) )
  {
    if( fAlmost && !bCheckOnly ){
      scPut_RankEffectLimit( target, effect, volume );
      m_fWazaFailMsgDisped = true;
    }
    m_simpleEffectFailManager.AddCause( SimpleEffectFailManager::CAUSE_LIMIT );
    return false;
  }

  BTL_PRINT("[migawari threw] %d\n", bMigawariThrew);
  // ターゲットがみがわり状態の時、自分以外からの作用は受けない
  if( target->MIGAWARI_IsExist() && !(bMigawariThrew) )
  {
    BTL_PRINT("身代わりがいて、貫通できない場合です\n");
    u8 targetPokeID = target->GetID();
    if( atkPokeID != targetPokeID )
    {
      if( fAlmost && !bCheckOnly ){
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, targetPokeID );
      }
      m_simpleEffectFailManager.AddCause( SimpleEffectFailManager::CAUSE_MIGAWARI );
      return false;
    }
  }

  {
    if( scEvent_CheckRankEffectSuccess( target, effect, atkPokeID, volume, cause, rankEffSerial) )
    {
      // ここまで来たらランク効果発生
      BTL_PRINT("実質ランク効果 = %d\n", volume);
      return true;
    }
    else
    {
      BTL_PRINT("実質ランク効果なし（失敗）\n");
      m_simpleEffectFailManager.AddCause( SimpleEffectFailManager::CAUSE_OTHER_EFFECTS );
      if( fAlmost && !bCheckOnly)
      {
        m_fWazaFailMsgDisped = true;
        u32 hem_state = m_HEManager.PushState();
        scEvent_RankEffect_Failed(  target, rankEffSerial );
        m_HEManager.PopState( hem_state );
      }
      return false;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ランク効果の増減値を変化させる
 *
 * @param   wk
 * @param   target
 * @param   rankType
 * @param   wazaUsePokeID
 * @param   itemID
 * @param   volume
 *
 * @retval  int   変化後の増減値
 */
//----------------------------------------------------------------------------------
int  ServerFlow::scEvent_RankValueChange( const BTL_POKEPARAM* target, WazaRankEffect rankType,
  u8 wazaUsePokeID, u16 itemID, int volume )
{
  GFL_UNUSED( itemID );
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_VOLUME, volume );
    EVENT_CallHandlers( this, BTL_EVENT_RANKVALUE_CHANGE );
    volume  = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  EVENTVAR_Pop();
  return volume;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ランク効果＆状態異常を同時に与えるワザ
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_EffectSick( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  WazaSick sick;
  SICKCONT_PARAM contParam;
  BTL_SICKCONT sickCont;
  BTL_POKEPARAM* target;

  sick = static_cast<WazaSick>(WAZADATA_GetParam( wazaParam->wazaID, pml::wazadata::PARAM_ID_SICK ));
  contParam = WAZADATA_GetSickCont( wazaParam->wazaID );

  calc::WazaSickContToBppSickCont( contParam, attacker, &sickCont );
  u32  wazaSerial = wazaSerial_Inc();

  targetRec->SeekStart();
  while( (target = targetRec->SeekNext()) != NULL )
  {
    bool isRankEffectEnable = false;
    if( scproc_WazaRankEffect_Common( wazaParam, attacker, target, wazaSerial, true, false) ){
      isRankEffectEnable = true;
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }
    if( scproc_Fight_WazaSickCore( attacker, target, wazaParam->wazaID, sick, sickCont, SICK_CAUSE_WAZA_EFFECT_SICK, wazaSerial, true, isRankEffectEnable) ){
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：HP回復
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_SimpleRecover( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  GFL_UNUSED( attacker );

  BTL_POKEPARAM* target;
  u8  pokeID;

  targetRec->SeekStart();
  while( (target = targetRec->SeekNext()) != NULL )
  {
    pokeID = target->GetID();
    if( !scproc_RecoverHP_CheckFailBase(target) )
    {
      u32 recoverHP = scEvent_CalcWazaRecoverHP( attacker, target, wazaParam->wazaID );
      bool skipSpFailCheck = ( wazaParam->zenryokuWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
      if( scproc_RecoverHP(target, recoverHP, skipSpFailCheck, true, true) )
      {
        wazaEffCtrl_SetEnable( m_wazaEffCtrl );
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_HP_Recover, pokeID );
      }
    }
    else
    {
      if( target->IsHPFull() ){
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_HPFull, pokeID );
      }
    }
  }
}

/**
 *  HP回復ルート
 * @param fSkipSpFailCheck  特殊条件によるHP回復可否チェックをスキップするか？
 */
bool ServerFlow::scproc_RecoverHP( BTL_POKEPARAM* bpp, u16 recoverHP, bool fSkipSpFailCheck, bool fDispFailMsg, bool isEffectEnable )
{
  if( fSkipSpFailCheck ||
      !scproc_RecoverHP_CheckFailSP( bpp, fDispFailMsg) )
  {
    scproc_RecoverHP_Core( bpp, recoverHP, isEffectEnable );
    return true;
  }
  return false;
}
/**
 *  HP回復可否チェック（基本条件）
 */
bool  ServerFlow::scproc_RecoverHP_CheckFailBase( const BTL_POKEPARAM* bpp )
{
  if( !bpp->IsFightEnable() ){
    return true;
  }
  if( bpp->IsHPFull() ){
    return true;
  }
  return false;
}
/**
 *  HP回復可否チェック（特殊条件）
 */
bool  ServerFlow::scproc_RecoverHP_CheckFailSP( const BTL_POKEPARAM* bpp, bool fDispFailMsg )
{
  if( bpp->CheckSick(pml::wazadata::WAZASICK_KAIHUKUHUUJI) )
  {
    if( fDispFailMsg )
    {
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KaifukuFujiExe, bpp->GetID() );
      m_fKaifukuFujiMsgDisped = true;
    }
    return true;
  }
  return false;
}
/**
 *  HP回復処理コア
 */
void ServerFlow::scproc_RecoverHP_Core( BTL_POKEPARAM* bpp, u16 recoverHP, bool isEffectEnable )
{
  u8 pokeID = bpp->GetID();
  BtlPokePos pos = m_posPoke.GetPokeExistPos( pokeID );
  if( isEffectEnable && ( pos != BTL_POS_NULL ) )
  {
    SCQUE_PUT_ACT_EffectByPos( m_que, pos, BTLEFF_HP_RECOVER );
  }
  scPut_SimpleHp( bpp, recoverHP, DAMAGE_CAUSE_OTHER, BTL_POKEID_NULL, isEffectEnable );
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：一撃ワザ処理
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_Ichigeki( const ActionDesc* actionDesc, const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  BTL_POKEPARAM* target;
  u8 atkLevel, defLevel, targetPokeID;
  BtlTypeAff aff;
  u32 i = 0;

  atkLevel = attacker->GetValue( BTL_POKEPARAM::BPP_LEVEL );

  for ever
  {
    target = targets->Get( i++ );
    if( target == NULL ){ break; }
    if( target->IsDead() ){ continue; }

    targetPokeID = target->GetID();

    if( !IsMustHit(attacker, target) )
    {
      // そらをとぶなどによるハズレ
      bool bEnableAvoidMsg=true;
      if( scEvent_CheckPokeHideAvoid(attacker, target, wazaParam->wazaID, &bEnableAvoidMsg) ){
        if( bEnableAvoidMsg ){
          scPut_WazaAvoid( target, wazaParam->wazaID );
        }
        continue;
      }
    }

    // 自分よりレベルが高いポケモンには無効
    defLevel = target->GetValue( BTL_POKEPARAM::BPP_LEVEL );
    if( atkLevel < defLevel ){
      scPut_WazaNoEffectIchigeki( target );
      continue;
    }

    // ワザ相性による無効化
    aff = m_dmgAffRec.Get( targetPokeID );
    if( aff == pml::battle::TypeAffinity::TYPEAFF_0 ){
      scPut_WazaNoEffectIchigeki( target );
      continue;
    }

    {
      u32 hem_state = m_HEManager.PushState();

      if( scEvent_IchigekiCheck(attacker, target, wazaParam) )
      {
        BtlPokePos atkPos = m_posPoke.GetPokeExistPos( attacker->GetID() );
        u16  damage = target->GetValue( BTL_POKEPARAM::BPP_HP );
        BtlTypeAffAbout  affAbout = calc::TypeAffAbout( aff );
        bool fMigawari = false;

        wazaEffCtrl_SetEnable( m_wazaEffCtrl );

        if( target->MIGAWARI_IsExist() )
        {
           if( !scEvent_CheckMigawariThrew(attacker, target, wazaParam->wazaID) ){
             fMigawari = true;
           }
        }

        if( fMigawari )
        {
          damage = scproc_Ichigeki_Migawari( target, wazaParam, affAbout );
          targets->AddWithDamage( target, damage, true );
        }
        else
        {
          bool fGuard = scEvent_CheckIchigekiGuard( attacker, target, wazaParam );

          if( fGuard )
          {
            damage = 0;
          }
          else
          {
            BTL_POKEPARAM::KoraeruCause korae_cause = scEvent_CheckKoraeru(  attacker, target, true, &damage );

            if( korae_cause == BTL_POKEPARAM::KORAE_NONE )
            {
              scproc_Ichigeki_Succeed( target, attacker, wazaParam , affAbout );
            }
            else
            {
              scproc_Ichigeki_Korae( target, attacker, wazaParam, affAbout, korae_cause , damage );
            }
          }

          targets->AddWithDamage( target, damage, false );
          wazaDmgRec_Add( atkPos, attacker, target, wazaParam, damage );
        }

        scproc_WazaDamageReaction( attacker, target, wazaParam, aff, damage, false, fMigawari );

        {
          MainModule::PGLRecParam   pgl_rec(attacker, wazaParam->orgWazaID );
          scproc_CheckDeadCmd( target, false, &pgl_rec );
        }

        scproc_Fight_DamageProcEnd( actionDesc, wazaParam, attacker, targets, damage, false );
      }
      else
      {
        if( !scproc_HandEx_Result() ){
          scPut_WazaAvoid( target, wazaParam->wazaID );
        }
      }
       m_HEManager.PopState( hem_state );
    }

  }/* while(1) */
}

/**
 * @brief みがわりチェック後の一撃必殺技無効化チェック
 * @retval true   無効化する
 * @retval false  無効化しない
 */
bool ServerFlow::scEvent_CheckIchigekiGuard( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  bool fGuard = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, fGuard );
    EVENT_CallHandlers( this, BTL_EVENT_ICHIGEKI_GUARD );
    fGuard = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return fGuard;
}

//--------------------------------------------------------------------------
/**
 * 一撃必殺ワザ成功
 */
//--------------------------------------------------------------------------
void ServerFlow::scproc_Ichigeki_Succeed( BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout )
{
  u8 pokeID = target->GetID();

  target->HpZero();
  SCQUE_PUT_OP_HpZero( m_que, pokeID );

  SCQUE_PUT_ACT_WazaDamage( m_que, pokeID, affAbout, wazaParam->wazaID );
  SCQUE_PUT_ACT_WazaIchigeki( m_que, pokeID );

  if( target->IsDead() )
  {
    target->SetDeadCause( DAMAGE_CAUSE_ICHIGEKI, attacker->GetID() );
  }

  scEvent_IchigekiSucceed( attacker, target );
}
/************************************************************************/
/* 一撃必殺ワザ成功時イベント                                           */
/************************************************************************/
void ServerFlow::scEvent_IchigekiSucceed( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_ICHIGEKI_SUCCEED );
  EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * 一撃必殺ワザこらえた
 */
//--------------------------------------------------------------------------
void ServerFlow::scproc_Ichigeki_Korae( BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam,
    BtlTypeAffAbout affAbout, BTL_POKEPARAM::KoraeruCause korae_cause, u16 damage )
{
  scPut_DecreaseHP( target, damage, false, DAMAGE_CAUSE_ICHIGEKI, attacker->GetID() );

//  SCQUE_PUT_ACT_SimpleHP( m_que, target->GetID() );
  SCQUE_PUT_ACT_WazaDamage( m_que, target->GetID(), affAbout, wazaParam->wazaID );

  scproc_Koraeru( target, korae_cause );
}
//----------------------------------------------------------------------------------
/**
 * 一撃必殺ワザ みがわり破壊
 */
//----------------------------------------------------------------------------------
u16  ServerFlow::scproc_Ichigeki_Migawari( BTL_POKEPARAM* target, const WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout )
{
  u8 pokeID = target->GetID();
  u16 damage = target->MIGAWARI_GetHP();

  SCQUE_PUT_ACT_WazaIchigeki( m_que, pokeID );
  SCQUE_PUT_ACT_WazaDamage( m_que, pokeID, affAbout, wazaParam->wazaID );
  scproc_Migawari_Delete( target );

  return damage;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ふきとばしワザ処理
//---------------------------------------------------------------------------------------------
void  ServerFlow::scproc_Fight_PushOut( WazaID waza, BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  BTL_POKEPARAM *target;
  bool fFailMsgDisped = false;

  u16  effectNo = scEvent_CheckPushOutEffectNo( attacker, waza );

  targetRec->SeekStart();
  while( (target = targetRec->SeekNext()) != NULL )
  {
    if( scproc_PushOutCore(attacker, target, false, &fFailMsgDisped, effectNo, false, NULL) )
    {
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }
    else if( fFailMsgDisped == false ){
      scPut_WazaFail( attacker, waza );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ふきとばし処理コア
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   fForceChange  強制的に入れ替えモードで実行（falseならルール等に応じて自動判別）
 * @param   effectNo      成功時エフェクトナンバー（0 = 標準エフェクト）
 * @param   fIgnoreLevel  レベル差による失敗判定を行わない
 * @param   fFailMsgDisped  [out] 特殊な失敗条件発生、原因を表示したらtrue
 *
 * @retval  bool    成功時true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_PushOutCore( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  bool fForceChange, bool* fFailMsgDisped, u16 effectNo, bool fIgnoreLevel, BTL_HANDEX_STR_PARAMS* succeedMsg )
{
  PushOutEffect   eff;

  *fFailMsgDisped = false;

  if( fForceChange ){
    eff = PUSHOUT_EFFECT_CHANGE;
  }else{
    eff = check_pushout_effect( attacker );
  }

  if( eff == PUSHOUT_EFFECT_MUSTFAIL ){
    return false;
  }else{

    BtlPokePos targetPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, target->GetID() );

    // 対象が場にいなければ失敗
    if( targetPos == BTL_POS_NULL ){
      return false;
    }

    // 対象が死んでたら失敗
    if( target->IsDead() ){
      return false;
    }

    // フリーフォールで捕まれてる場合は失敗
    if( target->CheckSick( pml::wazadata::WAZASICK_FREEFALL) ){
      return false;
    }
    // フリーフォールで捕んでいる場合も失敗
    if( target->IsUsingFreeFall() ){
      return false;
    }

    // 特殊要因で失敗
    {
      u32 hem_state = m_HEManager.PushState();
      u8 fFailed = scEvent_CheckPushOutFail( attacker, target );
      if( fFailed )
      {
        if( scproc_HandEx_Result() != HandExResult_NULL ){
          (*fFailMsgDisped) = true;
        }
      }
      m_HEManager.PopState( hem_state );

      if( fFailed ){
        return false;
      }
    }

    // 通常処理
    {
      u8 clientID, posIdx;
      m_mainModule->BtlPosToClientID_and_PosIdx( targetPos, &clientID, &posIdx );

      // 強制入れ替え効果
      if( eff == PUSHOUT_EFFECT_CHANGE )
      {
        SVCL_WORK* clwk;
        int nextPokeIdx;

        clwk = m_server->GetClientWork( clientID );
        nextPokeIdx = get_pushout_nextpoke_idx( clwk );

        if( nextPokeIdx >= 0 )
        {
          BTL_POKEPARAM* nextPoke = m_pokeCon->GetClientPokeData( clientID, nextPokeIdx );
          u8 nextPokeID = nextPoke->GetID();

          scproc_MemberOutCore( target, effectNo );
          if( succeedMsg != NULL ){
            handexSub_putString( succeedMsg );
          }
          scproc_MemberInForChange( clientID, posIdx, nextPokeIdx, false );

          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_PushOutChange, nextPokeID );
          scproc_AfterMemberIn();
        }
        else{
          return false;
        }
      }
      // バトル離脱効果
      else
      {
        clientID = MainModule::PokeIDtoClientID( attacker->GetID() );

        // 対象の方がレベル上だったら失敗
        if( !fIgnoreLevel )
        {
          u8 atkLevel = attacker->GetValue( BTL_POKEPARAM::BPP_LEVEL );
          u8 tgtLevel = target->GetValue( BTL_POKEPARAM::BPP_LEVEL );
          if( tgtLevel > atkLevel ){
            return false;
          }
        }

        m_escInfo.Add( clientID );

        scproc_MemberOutCore( target, effectNo );
        m_flowResult = RESULT_BTL_QUIT_ESCAPE;
      }
    }
    return true;
  }
}
//--------------------------------------------------------------------------
/**
 * 「ふきとばし」系のワザ効果をバトルのルール等から判定する
 */
//--------------------------------------------------------------------------
ServerFlow::PushOutEffect     ServerFlow::check_pushout_effect( const BTL_POKEPARAM* attacker )
{
  // ぬし戦・試練バトルでは失敗
  if( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ||
      m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_SIREN_BATTLE ) ||
      // @fix GFMMCat[209] VSヒカリドラゴン戦で、にげることができる
      // @fix GFMMCat[547] 合体ネクロズマ戦にて逃げられる
      m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_NO_CAPTURE ) )
  {
    return PUSHOUT_EFFECT_MUSTFAIL;
  }

  BtlRule rule = m_mainModule->GetRule();
  BtlCompetitor competitor = m_mainModule->GetCompetitor();

  switch( rule ){
  case BTL_RULE_SINGLE:
    if( competitor == BTL_COMPETITOR_WILD ){
      return PUSHOUT_EFFECT_ESCAPE;
    }else{
      return PUSHOUT_EFFECT_CHANGE;
    }

  default:
    return PUSHOUT_EFFECT_CHANGE;
  }
}
//--------------------------------------------------------------------------
/**
 * 「ふきとばし」系のワザで強制入れかえが発生した時、
 * 次に出すポケモンのパーティ内インデックスをランダムで決める
 * 出せるポケモンが控えに居ない場合、-1 を返す
 *
 * @param   wk
 * @param   clwk
 *
 * @retval  int  次に出すポケモンのパーティ内Index : 出せるポケが居ない場合 -1
 */
//--------------------------------------------------------------------------
int ServerFlow::get_pushout_nextpoke_idx( const SVCL_WORK* clwk )
{
  const BTL_POKEPARAM* bpp;
  u32 i, startIdx, count, members;
  u8 list[ BTL_PARTY_MEMBER_MAX ];

  members = clwk->party->GetMemberCount();
  startIdx = m_mainModule->GetClientBenchPosIndex( clwk->myID );

  for(i=startIdx, count=0; i<members; ++i)
  {
    bpp = clwk->party->GetMemberDataConst( i );
    if( bpp->IsFightEnable() ){
      list[count++] = i;
    }
  }

  if( count )
  {
    i = calc::GetRand( count );
    return list[ i ];
  }

  return -1;
}
//--------------------------------------------------------------------------
/**
 * [Event] ふきとばしワザエフェクトナンバーチェック
 * @param attacker
 * @param waza
 */
//--------------------------------------------------------------------------
u16 ServerFlow::scEvent_CheckPushOutEffectNo( const BTL_POKEPARAM* attacker, WazaID waza )
{
  u16  effectNo = 0;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_EFFECT_NO, effectNo );
    EVENT_CallHandlers( this, BTL_EVENT_PUSHOUT_EFFECT_NO );
    effectNo = EVENTVAR_GetValue( BTL_EVAR_EFFECT_NO );
  EVENTVAR_Pop();

  return effectNo;
}

//--------------------------------------------------------------------------
/**
 * [Event] ふきとばしワザ失敗チェック
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  bool      失敗ならtrue
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckPushOutFail( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  GFL_UNUSED( attacker );
  bool failFlag = false;
  EVENTVAR_Push();
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_PUSHOUT );
    BTL_SICKEVENT_CheckPushOutFail( this, target );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  return failFlag;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：フィールドエフェクト効果
//---------------------------------------------------------------------------------------------
void ServerFlow::scput_Fight_FieldEffect( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker )
{
  BtlWeather  weather = WAZADATA_GetWeather( wazaParam->wazaID );
  if( weather != BTL_WEATHER_NONE )
  {
    u8 turn_upcnt = scEvent_WazaWeatherTurnUp( weather, attacker );

    //@バトル変更箇所 sango特殊天候失敗処理。 戻り値の追加
    const ChangeWeatherResult result = scproc_ChangeWeather(weather, BTL_WEATHER_TURN_DEFAULT+turn_upcnt, turn_upcnt, attacker->GetID() );
    if( result == CHANGE_WEATHER_OK ){
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }
    else if( result == CHANGE_WEATHER_FAIL_SANGO_WEATHER )
    {
      //@バトル変更箇所 sango特殊天候失敗処理。 特性の方にも同じ処理がある。
      BTL_HANDEX_STR_PARAMS strParam;
      HANDEX_STR_Clear(&strParam);//Coverity[10973]初期化忘れ
      switch( m_fieldStatus->GetWeather() )
      {
        case BTL_WEATHER_STORM:    ///< おおあめ
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_RainStormFail_Weather );
          break;
        case BTL_WEATHER_DAY:      ///< おおひでり
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_DayFail_Weather );
          break;
        case BTL_WEATHER_TURBULENCE:  ///< 乱気流
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_TurbulenceFail_Weather );
          break;
        default:
          GFL_ASSERT_MSG(0,"新天候じゃないのに来てる。[%d]\n",m_fieldStatus->GetWeather());
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_WazaFail );  //保険
          break;
      }
      handexSub_putString( &strParam );
    }else{
      scPut_WazaFail( attacker, wazaParam->wazaID );
    }
  }
  else
  {
    u32 hem_state = m_HEManager.PushState();
    bool fSucceed;
    scEvent_FieldEffectCall( attacker, wazaParam->wazaID );
    fSucceed = (scproc_HandEx_Result( ) == HandExResult_Enable);
    m_HEManager.PopState( hem_state );

    if( fSucceed ){
      wazaEffCtrl_SetEnable( m_wazaEffCtrl );
    }else{
      scPut_WazaFail( attacker, wazaParam->wazaID );
    }
  }
}
/**
 *  天候変化処理
 * @param weather      天候
 * @param turn         天候の継続ターン数
 * @param turnUpCount  天候の継続ターン数のうち、道具や特性などで増加したターン数
 * @param causePokeID  天候を変化させたポケモンのID
 */
ServerFlow::ChangeWeatherResult ServerFlow::scproc_ChangeWeather( BtlWeather weather, u8 turn, u8 turnUpCount, u8 causePokeID )
{
  const ChangeWeatherResult result = scproc_ChangeWeatherCheck(weather, turn);
  if( result == CHANGE_WEATHER_OK )
  {
    scproc_ChangeWeatherCore( weather, turn, turnUpCount, causePokeID );
  }
  return result;
}
/**
 *  天候変化可否チェック
 */
ServerFlow::ChangeWeatherResult ServerFlow::scproc_ChangeWeatherCheck( BtlWeather weather, u8 turn )
{
  if( weather >= BTL_WEATHER_MAX ){
    return CHANGE_WEATHER_FAIL;
  }
  
  //@バトル変更箇所 sango特殊天候失敗処理。 上位の天候は下位の天候をブロック
  if( m_fieldStatus->GetWeather() == BTL_WEATHER_STORM ||    ///< おおあめ
      m_fieldStatus->GetWeather() == BTL_WEATHER_DAY ||      ///< おおひでり
      m_fieldStatus->GetWeather() == BTL_WEATHER_TURBULENCE )  ///< 乱気流
  {
    if( weather == BTL_WEATHER_SHINE ||    ///< はれ
        weather == BTL_WEATHER_RAIN ||     ///< あめ
        weather == BTL_WEATHER_SNOW ||     ///< あられ
        weather == BTL_WEATHER_SAND )     ///< すなあらし
    {
      return CHANGE_WEATHER_FAIL_SANGO_WEATHER;
    }
  }
  
  if( m_fieldStatus->GetWeather() == weather )
  {
    if( (turn != BTL_WEATHER_TURN_PERMANENT)
    ||  (m_fieldStatus->GetWeatherRemainingTurn() == BTL_WEATHER_TURN_PERMANENT)
    ){
      return CHANGE_WEATHER_FAIL;
    }
  }

  return CHANGE_WEATHER_OK;
}
/**
 *  天候変化コア
 * @param weather      天候
 * @param turn         天候の継続ターン数
 * @param turnUpCount  天候の継続ターン数のうち、道具や特性などで増加したターン数
 * @param causePokeID  天候を変化させたポケモンのID
 */
void ServerFlow::scproc_ChangeWeatherCore( BtlWeather weather, u8 turn, u8 turnUpCount, u8 causePokeID )
{
  m_fieldStatus->SetWeather( weather, turn, turnUpCount, causePokeID );
  SCQUE_PUT_ACTOP_WeatherStart( m_que, weather, turn, turnUpCount, causePokeID );
  scproc_ChangeWeatherAfter( weather );
}
/**
 *  天候変化直後イベント呼び出し
 */
void  ServerFlow::scproc_ChangeWeatherAfter( BtlWeather weather )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_AfterChangeWeather( weather );
  m_HEManager.PopState( hem_state );

  scproc_SetFrontPokeDirt_ByWeather();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによる天候変化のターン数増加チェック
 *
 * @param   wk
 * @param   weather
 * @param   wk
 * @param   weather
 * @param   turn
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
u8  ServerFlow::scEvent_WazaWeatherTurnUp( BtlWeather weather, const BTL_POKEPARAM* attacker )
{
  u8 cnt;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, 0 );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_WEATHER_TURNCNT );
    cnt = EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
  EVENTVAR_Pop();
  return cnt;
}

//--------------------------------------------------------------------------
/**
 * 【Event】天候の変化チェック
 *
 * @param   wk
 * @param   weather 天候
 * @param   turn    [io]継続ターン数
 *
 * @retval  bool    変化する場合はtrue
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckChangeWeather( BtlWeather weather, u8* turn )
{
  GFL_ASSERT(weather != BTL_WEATHER_NONE);

  {
    bool fFail;

    EVENTVAR_Push();
      EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
      EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, *turn );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
      EVENT_CallHandlers( this, BTL_EVENT_WEATHER_CHANGE );
      weather = static_cast<BtlWeather>(EVENTVAR_GetValue( BTL_EVAR_WEATHER ));
      *turn = EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
      fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    EVENTVAR_Pop();
    return !fFail;
  }
}
//
bool  ServerFlow::scproc_FieldEffectCore( FieldStatus::EffectType effect, const BTL_SICKCONT& contParam, bool fAddDependTry )
{
  if( m_fieldStatus->AddEffect(effect, contParam) )
  {
    SCQUE_PUT_OP_AddFieldEffect( m_que, effect, contParam );
    return true;
  }
  else if( fAddDependTry )
  {
    u8 dependPokeID = SICKCONT_GetPokeID( contParam );
    if( dependPokeID != BTL_POKEID_NULL )
    {
      m_fieldStatus->AddDependPoke( effect, dependPokeID );
      SCQUE_PUT_OP_AddFieldEffectDepend( m_que, effect, dependPokeID );
      return true;
    }
  }
  return false;
}
/**
 * @brief 地形効果を変更する
 * @param pokeID     地形を変更するポケモン( 自然に消滅する場合など、ポケモンが主体でないなら BTL_POKEID_NULL )
 * @param ground     新しい地形効果
 * @param contParam  新しい地形効果の継続パラメータ
 */
bool ServerFlow::scproc_ChangeGround( u8 pokeID, BtlGround ground, const BTL_SICKCONT& contParam )
{
  if( m_fieldStatus->ChangeGround(ground, contParam) )
  {
    SCQUE_OP_ChangeGround( m_que, ground, contParam );
    return true;
  }

  return false;
}

/**
 * @brief [Event] 地形効果変更前
 * @param      pokeID     地形を変更するポケモン( 自然に消滅する場合など、ポケモンが主体でないなら BTL_POKEID_NULL )
 * @param      ground     新しい地形効果
 * @param[out] contParam  新しい地形効果の継続パラメータ
 */
void ServerFlow::scEvent_ChangeGroundBefore( u8 pokeID, BtlGround ground, BTL_SICKCONT* contParam )
{
  EVENTVAR_Push();

    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_GROUND, ground );
    EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, 0 );
    EVENT_CallHandlers( this, BTL_EVENT_CHANGE_GROUND_BEFORE );

    s32 addTurn = EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
    if( 0 < addTurn )
    {
      SICKCONT_IncTurn( contParam, addTurn );
      SICKCONT_AddParam( contParam, addTurn );
    }

  EVENTVAR_Pop();
}

/**
 * @brief [Event] 地形効果変更後
 * @param pokeID  地形を変更するポケモン( 自然に消滅する場合など、ポケモンが主体でないなら BTL_POKEID_NULL )
 * @param ground  新しい地形効果
 */
void ServerFlow::scEvent_ChangeGroundAfter( u8 pokeID, BtlGround ground )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_GROUND, ground );
    EVENT_CallHandlers( this, BTL_EVENT_CHANGE_GROUND_AFTER );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] フィールドエフェクト追加処理呼び出し
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_FieldEffectCall( const BTL_POKEPARAM* attacker, WazaID waza )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    EVENT_CallHandlers( this, BTL_EVENT_FIELD_EFFECT_CALL );
  EVENTVAR_Pop();
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：分類しきれないワザ
//---------------------------------------------------------------------------------------------
void ServerFlow::scput_Fight_Uncategory( const WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, PokeSet* targets )
{
  // ターゲットは居たはずだがモロモロの理由で残っていない場合は終了
  if( scproc_IsTargetRemovedAll(attacker, wazaParam, targets) ){
    return;
  }

  switch( wazaParam->wazaID ){
  case WAZANO_SUKIRUSUWAPPU:
    scput_Fight_Uncategory_SkillSwap( attacker, targets );
    break;
  case WAZANO_MIGAWARI:
    if( scproc_Migawari_Create(attacker) )
    {
      wazaEffCtrl_SetEnableDummy( m_wazaEffCtrl );
    }
    break;
  default:
    {
      u32 hem_state = m_HEManager.PushState();
      HandExResult result = HandExResult_NULL;
      bool fFailMsgEnable;

      result = scproc_HandEx_Result();
      BTL_PRINT("Uncategory resultIni=%d\n", result);


      if( scEvent_UnCategoryWaza(wazaParam, attacker, targets, &fFailMsgEnable) )
      {
        result = scproc_HandEx_Result();
        BTL_PRINT("Uncategory result=%d\n", result);
        if( result == HandExResult_Enable )
        {
          wazaEffCtrl_SetEnable( m_wazaEffCtrl );
        }
      }
      BTL_PRINT("fMsgEnable = %d,  disped=%d\n",fFailMsgEnable, m_fWazaFailMsgDisped);
      if( (result == HandExResult_NULL) || (result == HandExResult_Fail) )
      {
        if( fFailMsgEnable && (!(m_fWazaFailMsgDisped)) )
        {
          SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
        }
      }

      m_HEManager.PopState( hem_state );
    }
    break;
  }
}
// スキルスワップ
void ServerFlow::scput_Fight_Uncategory_SkillSwap( BTL_POKEPARAM* attacker, PokeSet* targetRec )
{
  BTL_POKEPARAM* target = targetRec->Get( 0 );
  TokuseiNo atk_tok, tgt_tok;

  atk_tok = static_cast<TokuseiNo>(attacker->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));
  tgt_tok = static_cast<TokuseiNo>(target->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

  if( (!tables::CheckSkillSwapFailTokusei(atk_tok))
  &&  (!tables::CheckSkillSwapFailTokusei(tgt_tok))
  ){
    u8 atkPokeID = attacker->GetID();
    u8 tgtPokeID = target->GetID();

    wazaEffCtrl_SetEnable( m_wazaEffCtrl );

    SCQUE_PUT_ACTOP_SwapTokusei( m_que, atkPokeID, tgtPokeID, tgt_tok, atk_tok );
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_SkillSwap, atkPokeID );

    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_ChangeTokuseiBefore(  atkPokeID, atk_tok, tgt_tok );
      scEvent_ChangeTokuseiBefore(  tgtPokeID, tgt_tok, atk_tok );
      m_HEManager.PopState( hem_state );
    }
    attacker->ChangeTokusei( tgt_tok );
    target->ChangeTokusei( atk_tok );
    handler::tokusei::Swap( attacker, target );

//    if( atk_tok != tgt_tok )
    scproc_AfterTokuseiChanged_Event( attacker );
    scproc_AfterTokuseiChanged_Event( target );

    scproc_AfterTokuseiChanged_Item( attacker, atk_tok, tgt_tok );
    scproc_AfterTokuseiChanged_Item( target,   tgt_tok, atk_tok );
  }
  else{
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
  }
}
/**
 *  とくせい変化後の処理：イベント呼び出し
 */
void ServerFlow::scproc_AfterTokuseiChanged_Event( const BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  u32 hem_state = m_HEManager.PushState();
    scEvent_ChangeTokuseiAfter( pokeID );
  m_HEManager.PopState( hem_state );
}

/**
 * とくせい変化後の処理：アイテム使用チェック
 */
void ServerFlow::scproc_AfterTokuseiChanged_Item( BTL_POKEPARAM* bpp, TokuseiNo prevTokusei, TokuseiNo nextTokusei )
{
  // ぶきよう・きんちょうかんが入れ替わったらアイテム発動チェック
  if( !bpp->CheckSick( pml::wazadata::WAZASICK_IEKI) )
  {
    if( (prevTokusei == TOKUSEI_BUKIYOU)
    ||  (nextTokusei == TOKUSEI_KUISINBOU)
    ){
      scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_GEN );
    }
    if( prevTokusei == TOKUSEI_KINTYOUKAN ){
      scproc_KintyoukanMoved( bpp );
    }
  }
}

/**
 *  とくせい「きんちょうかん」を持つポケモンが場から消えた・またはとくせいを失った時の処理
 */
void  ServerFlow::scproc_KintyoukanMoved( const BTL_POKEPARAM* bppMoved )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  u8 movedPokeID = bppMoved->GetID();
  u8 pokeID;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    pokeID = bpp->GetID();
    BTL_PRINT("緊張感きえたポケ=%d、ポケ=%dをチェック\n", movedPokeID, pokeID);

    if( !m_mainModule->IsFriendPokeID(movedPokeID, pokeID) )
    {
      BTL_PRINT("味方じゃないですのでチェックします\n");

      scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_GEN );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * みがわり - 作成
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_Migawari_Create( BTL_POKEPARAM* bpp )
{
  if( !bpp->MIGAWARI_IsExist() )
  {
    int migawariHP = calc::QuotMaxHP( bpp, 4 );
    if( bpp->GetValue( BTL_POKEPARAM::BPP_HP) > migawariHP )
    {
      BtlPokePos pos = m_posPoke.GetPokeExistPos( bpp->GetID() );
      if( pos != BTL_POS_NULL )
      {
        scPut_SimpleHp( bpp, -migawariHP, DAMAGE_CAUSE_MIGAWARI, bpp->GetID(), true );
        scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_HP );

        bpp->MIGAWARI_Create( migawariHP );
        SCQUE_PUT_OP_MigawariCreate( m_que, bpp->GetID(), migawariHP );
        SCQUE_PUT_ACT_MigawariCreate( m_que, pos );
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_MigawariAppear, bpp->GetID() );
        return true;
      }
    }
    // 失敗メッセージ
    scPut_Message_StdEx( BTL_STRID_STD_MigawariFail, 0, NULL );
  }
  else{
    // すでに出ていたメッセージ
    scPut_Message_Set( bpp, BTL_STRID_SET_MigawariExist );
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * みがわり - ダメージ処理
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16 実際に与えたダメージ値
 */
//----------------------------------------------------------------------------------
u16  ServerFlow::scproc_Migawari_Damage( BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 damage,
  BtlTypeAff aff, CriticalType fCritical, const WAZAPARAM* wazaParam )
{

  SCQUE_PUT_ACT_MigawariDamage( m_que, target->GetID(), aff, wazaParam->wazaID );
  scPut_Message_Set( target, BTL_STRID_SET_MigawariDamage );
  scPut_WazaAffinityMsg( attacker, 1, &aff, &target, false );
  scproc_CriticalMsg( attacker, wazaParam, 1, &target, &fCritical , false );

  if( target->MIGAWARI_AddDamage(&damage) )
  {
    scproc_Migawari_Delete( target );
  }

  scproc_Damage_Drain( wazaParam, attacker, target, damage );
  scproc_WazaAdditionalEffect( wazaParam, attacker, target, damage, true );
  scproc_WazaAdditionalEffect_User( wazaParam, attacker, true );
  scproc_WazaDamageReaction( attacker, target, wazaParam, aff, damage, fCritical, true );

  return damage;
}
//----------------------------------------------------------------------------------
/**
 * みがわり - 削除処理
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_Migawari_Delete( BTL_POKEPARAM* bpp )
{
  u8 pokeID = bpp->GetID();
  BtlPokePos pos = m_posPoke.GetPokeExistPos( pokeID );

  scPut_Message_Set( bpp, BTL_STRID_SET_MigawariDestract );

  bpp->MIGAWARI_Delete();
  SCQUE_PUT_OP_MigawariDelete( m_que, pokeID );
  SCQUE_PUT_ACT_MigawariDelete( m_que, pos );
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージに対する反応処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   affinity
 * @param   damage
 * @param   critical_flag
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaDamageReaction( BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, bool critical_flag, bool fMigawari )
{
  u32 hem_state = m_HEManager.PushState();

  scEvent_WazaDamageReaction(  attacker, defender, wazaParam, affinity, damage, critical_flag, fMigawari );
  m_HEManager.PopState( hem_state );
}

//--------------------------------------------------------------------------
/**
 * [Event] ワザダメージ受けた後の反応処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 * @param   criticalFlag
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaDamageReaction(
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, bool fCritical, bool fMigawari )
{
  u8 defPokeID = defender->GetID();

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_ORG_WAZAID, wazaParam->orgWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, aff );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage );
    EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, fCritical );
    EVENTVAR_SetConstValue( BTL_EVAR_MIGAWARI_FLAG, fMigawari );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_RINPUNGUARD_FLG, false );

    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_REACTION_PREV );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_REACTION );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_REACTION_L2 );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_DMG_REACTION_L3 );

  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 未分類ワザ処理コール
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   targets
 *
 * @retval  bool      反応したワザハンドラがあればtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_UnCategoryWaza( const WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, PokeSet* targets, bool* fFailMsgEnable )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_KIND, wazaParam->zenryokuWazaKind );
    {
      BTL_POKEPARAM* bpp;
      u8 targetMax, targetCnt, i;

      targetMax = targets->GetCountMax();
      targetCnt = targets->GetCount();
      EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG,  true );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, true );

      BTL_N_Printf( DBGSTR_SVFL_UncategoryWazaInfo, attacker->GetID(), targetCnt, targetMax);

      for(i=0; i<targetCnt; ++i){
        bpp = targets->Get( i );
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
      }
      EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      if( targetMax )
      {
        EVENT_CallHandlers( this, BTL_EVENT_UNCATEGORIZE_WAZA );
      }
      else{
        EVENT_CallHandlers( this, BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET );
      }

      (*fFailMsgEnable) = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );

    }
  EVENTVAR_Pop();

  return (scproc_HandEx_Result() != HandExResult_NULL);
}


//==============================================================================
// サーバーフロー：ターンチェックルート
//==============================================================================

/**
 *  POKESETに全戦闘ポケを格納 -> 素早さ順ソート
 */
void ServerFlow::storeFrontPokeOrderAgility( PokeSet* pokeSet )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  pokeSet->Clear();

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) ){
    pokeSet->Add( bpp );
  }
  pokeSet->SortByAgility( this );
}

/**
 * @brief ターンチェック
 * @return ターンチェックの結果
 */
ServerFlow::TurnCheckResult ServerFlow::scproc_TurnCheck( void )
{
  enum
  {
    SEQ_STORE_TARGET,               // ターンチェック対象のポケモンをリストアップ
    SEQ_WEATHER,                    // 天候
    SEQ_WEATHER_TERMINATE,          // 天候( ターンチェック中止 )                 ※ダメージによって決着がついた場合など
    SEQ_WEATHER_POKECHANGE,         // 天候( ポケモン入れ替え )                   ※特性「にげごし」が発動した場合など。レベルアップ処理の後、ここで対応する
    SEQ_WEATHER_UPDATE_TARAGET,     // 天候( ターンチェック対象のポケモンを更新 ) ※ポケモン入れ替えが発生している可能性があるため
    SEQ_KAWAIGARI_EFFECT,           // かわいがり戦闘効果
    SEQ_EVENT_BEGIN,                // ターンチェックイベント開始
    SEQ_EVENT_BEGIN_TERMINATE,      // ターンチェックイベント開始( ターンチェック中止 )                 ※ダメージによって決着がついた場合など
    SEQ_EVENT_BEGIN_POKECHANGE,     // ターンチェックイベント開始( ポケモン入れ替え )                   ※特性「にげごし」が発動した場合など。レベルアップ処理の後、ここで対応する
    SEQ_EVENT_BEGIN_UPDATE_TARAGET, // ターンチェックイベント開始( ターンチェック対象のポケモンを更新 ) ※ポケモン入れ替えが発生している可能性があるため
    SEQ_SICK,                       // 状態異常
    SEQ_SICK_TERMINATE,             // 状態異常( ターンチェック中止 )                 ※ダメージによって決着がついた場合など
    SEQ_SICK_POKECHANGE,            // 状態異常( ポケモン入れ替え )                   ※特性「にげごし」が発動した場合など。レベルアップ処理の後、ここで対応する
    SEQ_SICK_UPDATE_TARAGET,        // 状態異常( ターンチェック対象のポケモンを更新 ) ※ポケモン入れ替えが発生している可能性があるため
    SEQ_SIDE,                       // サイドエフェクト
    SEQ_SIDE_UPDATE_TARAGET,        // サイドエフェクト( ターンチェック対象のポケモンを更新 )
    SEQ_FIELD,                      // フィールドエフェクト
    SEQ_FIELD_UPDATE_TARAGET,       // フィールドエフェクト( ターンチェック対象のポケモンを更新 )
    SEQ_EVENT_END,                  // ターンチェックイベント終了
    SEQ_EVENT_END_TERMINATE,        // ターンチェックイベント終了                                       ※ダメージによって決着がついた場合など
    SEQ_EVENT_END_POKECHANGE,       // ターンチェックイベント終了( ポケモン入れ替え )                   ※特性「にげごし」が発動した場合など。レベルアップ処理の後、ここで対応する
    SEQ_EVENT_END_UPDATE_TARAGET,   // ターンチェックイベント終了( ターンチェック対象のポケモンを更新 ) ※ポケモン入れ替えが発生している可能性があるため
    SEQ_EVENT_END_ROTOM_POWER_Z,    // ターンチェックイベント終了※ロトムZパワー回復処理
    SEQ_EVENT_DONE,                 // ターンチェックイベント完了
  };

  PokeSet* pokeSet = m_psetTarget;  // このPOKESETに対象ポケモンを格納する

  switch( m_turnCheckStep )
  {
  case SEQ_STORE_TARGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;

    #if BTL_OLD_IGNORE
    scproc_turncheck_CommSupport();
    #endif
    /* fallthru */

  case SEQ_WEATHER:
    m_turnCheckStep++;
    SCQUE_PUT_ACT_MsgWinHide( m_que, 0 );
    if( scproc_turncheck_weather(pokeSet) )
    {
      return TURNCHECK_RESULT_LEVELUP;
    }
    /* fallthru */

  case SEQ_WEATHER_TERMINATE:
    m_turnCheckStep++;
    if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() )
    {
      return TURNCHECK_RESULT_DONE; 
    }
    if( m_fQuitBattleRequest )
    {
      return TURNCHECK_RESULT_QUIT;
    }
    /* fallthru */

  case SEQ_WEATHER_POKECHANGE:
    m_turnCheckStep++;
    if( m_server->IsPokemonChangeRequestExist() )
    {
      return TURNCHECK_RESULT_POKECHANGE;
    }
    /* fallthru */

  case SEQ_WEATHER_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_KAWAIGARI_EFFECT:
    m_turnCheckStep++;
    scproc_turncheck_friendship_cure( pokeSet );
    /* fallthru */

  case SEQ_EVENT_BEGIN:
    m_turnCheckStep++;
    if( scproc_turncheck_sub(pokeSet, BTL_EVENT_TURNCHECK_BEGIN) )
    {
      return TURNCHECK_RESULT_LEVELUP;
    }
    /* fallthru */

  case SEQ_EVENT_BEGIN_TERMINATE:
    m_turnCheckStep++;
    if( scproc_CheckShowdown()&& isSkipBattleAfterShowdown() )
    {
      return TURNCHECK_RESULT_DONE;
    }
    if( m_fQuitBattleRequest )
    {
      return TURNCHECK_RESULT_QUIT;
    }
    /* fallthru */

  case SEQ_EVENT_BEGIN_POKECHANGE:
    m_turnCheckStep++;
    if( m_server->IsPokemonChangeRequestExist() )
    {
      return TURNCHECK_RESULT_POKECHANGE;
    }
    /* fallthru */

  case SEQ_EVENT_BEGIN_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_SICK:
    m_turnCheckStep++;
    if( scproc_turncheck_sick(pokeSet) )
    {
      return TURNCHECK_RESULT_LEVELUP;
    }
    /* fallthru */

  case SEQ_SICK_TERMINATE:
    m_turnCheckStep++;
    if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() )
    { 
      return TURNCHECK_RESULT_DONE;
    }
    if( m_fQuitBattleRequest )
    {
      return TURNCHECK_RESULT_QUIT;
    }
    /* fallthru */

  case SEQ_SICK_POKECHANGE:
    m_turnCheckStep++;
    if( m_server->IsPokemonChangeRequestExist() )
    {
      return TURNCHECK_RESULT_POKECHANGE;
    }
    /* fallthru */

  case SEQ_SICK_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_SIDE:
    scproc_turncheck_side( );
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_SIDE_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_FIELD:
    scproc_turncheck_field( );
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_FIELD_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_EVENT_END:
    m_turnCheckStep++;
    if( scproc_turncheck_sub( pokeSet, BTL_EVENT_TURNCHECK_END) )
    {
      return TURNCHECK_RESULT_LEVELUP;
    }
    /* fallthru */

  case SEQ_EVENT_END_TERMINATE:
    m_turnCheckStep++;
    if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() )
    { 
      return TURNCHECK_RESULT_DONE;
    }
    if( m_fQuitBattleRequest )
    {
      return TURNCHECK_RESULT_QUIT;
    }
    /* fallthru */

  case SEQ_EVENT_END_POKECHANGE:
    m_turnCheckStep++;
    if( m_server->IsPokemonChangeRequestExist() )
    {
      return TURNCHECK_RESULT_POKECHANGE;
    }
    /* fallthru */

  case SEQ_EVENT_END_UPDATE_TARAGET:
    storeFrontPokeOrderAgility( pokeSet );  // POKESET格納->素早さ順ソート
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_EVENT_END_ROTOM_POWER_Z:
    scproc_RecoverZenryoku();
    m_turnCheckStep++;
    /* fallthru */

  case SEQ_EVENT_DONE:
    scproc_turncheck_sub( pokeSet, BTL_EVENT_TURNCHECK_DONE );

    // 全ポケモンのターンフラグをクリア
    m_turnCheckStep++;
    {
      BTL_POKEPARAM* bpp;
      pokeSet->SeekStart();
      while( (bpp = pokeSet->SeekNext()) != NULL )
      {
        bpp->TurnCheck();
        bpp->CombiWaza_ClearParam();
        SCQUE_PUT_OP_TurnCheck( m_que, bpp->GetID() );
      }
    }

    // 分離イベントファクター削除
    EVENT_RemoveIsolateFactors();

    m_turnCheckStep = 0;
    m_simulationCounter = 0;
    /* fallthru */

  default:
    return TURNCHECK_RESULT_DONE;
  }
}

/**
 *  ターンチェック：なかよし度による状態異常回復処理
 */
void ServerFlow::scproc_turncheck_friendship_cure( PokeSet* pokeSet )
{
  BTL_POKEPARAM* bpp;

  pokeSet->SeekStart();
  while( (bpp = pokeSet->SeekNext()) != NULL )
  {
    if( getFriendship(bpp) >= 150 )
    {
      PokeSick sick = bpp->GetPokeSick();
      if( sick != pml::pokepara::SICK_NULL )
      {
        if( calc::IsOccurPer(20) )
        {
          scPut_CurePokeSick( bpp, (WazaSick)sick, false );

          u8 pokeID = bpp->GetID();
          u8 clientID = MainModule::PokeIDtoClientID( pokeID );

          switch( sick ){
          case pml::pokepara::SICK_NEMURI:  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_CureNemuri, pokeID, clientID ); break;
          case pml::pokepara::SICK_DOKU:    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_CureDoku,   pokeID, clientID ); break;
          case pml::pokepara::SICK_YAKEDO:  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_CureYakedo, pokeID, clientID ); break;
          case pml::pokepara::SICK_MAHI:    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_CureMahi,   pokeID, clientID ); break;
          case pml::pokepara::SICK_KOORI:   SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_CureKoori,  pokeID, clientID ); break;
          }
          SCQUE_PUT_ACT_FriendshipEffect( m_que, pokeID, FREFF_HEART );
        }
      }
    }
  }
}
/**
 *  ターンチェック：汎用
 *
 * @retval  bool    誰かが死亡->レベルアップ発生した場合にtrue
 */
bool  ServerFlow::scproc_turncheck_sub( PokeSet* pokeSet, BtlEventType event_type )
{
  BTL_POKEPARAM* bpp;
  u32 hem_state;

  pokeSet->SeekStart();

  hem_state = m_HEManager.PushState();
    scEvent_TurnCheck( BTL_POKEID_NULL, event_type );
  m_HEManager.PopState( hem_state );

  if( !scproc_CheckShowdown() || !isSkipBattleAfterShowdown() )
  {
    while( (bpp = pokeSet->SeekNext()) != NULL )
    {
      hem_state = m_HEManager.PushState();
        scEvent_TurnCheck( bpp->GetID(), event_type );
      m_HEManager.PopState( hem_state );

      scproc_CheckDeadCmd( bpp, false );

      // バトル離脱リクエストが発行されたら、途中で終了する
      // ( 野生戦のターンチェックでのダメージ( くろいヘドロ等 )により特性「にげごし」が発動した場合 )
      if( m_fQuitBattleRequest )
      {
        break;
      }

      if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() ){
        break;
      }
    }
  }

  return scproc_CheckExpGet();
}
void  ServerFlow::scEvent_TurnCheck( u8 pokeID, BtlEventType event_type )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENT_CallHandlers( this, event_type );
  EVENTVAR_Pop();
}
//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > 状態異常
//------------------------------------------------------------------
bool  ServerFlow::scproc_turncheck_sick( PokeSet* pokeSet )
{
  u8 pokeIDList[ BTL_EXIST_POS_MAX ];
  u32 pokeCnt, i, p;
  WazaSick sick;
  BTL_POKEPARAM* bpp;

  // 送信用バッファクリア
  for(i=0; i<GFL_NELEMS(pokeIDList); ++i){
    pokeIDList[ i ] = 0;
  }

  pokeCnt = pokeSet->GetCount();
  for(i=0; i<pokeCnt; ++i)
  {
    bpp = pokeSet->Get( i );
    pokeIDList[ i ] = bpp->GetID();
  }


  i = 0;
  for ever
  {
    sick = tables::GetTurnCheckWazaSickByOrder( i++ );
    if( sick != pml::wazadata::WAZASICK_NULL )
    {
      BTL_SICKCONT oldCont;

      for(p=0; p<pokeCnt; ++p)
      {
        bpp = m_pokeCon->GetPokeParam( pokeIDList[p] );
        if( !bpp->IsDead() && m_posPoke.IsExist( bpp->GetID() ) )
        {
          bool fCured;
          if( bpp->WazaSick_TurnCheck(sick, &oldCont, &fCured) )
          {
//            BTL_PRINT("%d体目、PokeID=%d, 状態異常:%d です\n", p, pokeIDList[p], sick);
            u32  hem_state = m_HEManager.PushState();
              BTL_SICK_TurnCheckCallback( bpp, sick, oldCont, fCured, this );
            m_HEManager.PopState( hem_state );
          }
        }

        // バトル離脱リクエストが発行されたら、途中で終了する
        // ( 野生戦で状態異常ダメージにより特性「にげごし」が発動した場合 )
        if( m_fQuitBattleRequest )
        {
          break;
        }
      }

      if( scproc_CheckShowdown() && isSkipBattleAfterShowdown() )
      {
        break;
      }
    }
    else
    {
      break;
    }
  }

//    BPP_WazaSick_TurnCheck( bpp, BTL_SICK_TurnCheckCal  BTL_POKEPARAM* bpp;lback, wk );
  {
    u32 packID = calc::PokeIDx6_Pack32bit( pokeIDList );
    SCQUE_PUT_OP_WazaSickTurnCheck( m_que, pokeCnt, i, packID );
  }

  return scproc_CheckExpGet();
}
//--------------------------------------------------------------------------------
/**
 * ターンチェックで状態異常によるダメージが発生するたびにコールバックされる
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//--------------------------------------------------------------------------------
void ServerFlow::SickDamageRecall( BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  u32 hem_state = m_HEManager.PushState();
  bool fEnable = false;
  damage = scEvent_SickDamage(  bpp, sickID, damage );
  if( damage )
  {
    DamageCause damageCause = DAMAGE_CAUSE_OTHER;
    switch( sickID ){
    case pml::wazadata::WAZASICK_DOKU:   damageCause = DAMAGE_CAUSE_SICK_DOKU;   break;
    case pml::wazadata::WAZASICK_YAKEDO: damageCause = DAMAGE_CAUSE_SICK_YAKEDO; break;
    case pml::wazadata::WAZASICK_NOROI:  damageCause = DAMAGE_CAUSE_SICK_NOROI;  break;
    case pml::wazadata::WAZASICK_AKUMU:  damageCause = DAMAGE_CAUSE_SICK_AKUMU;  break;
    }

    const BTL_SICKCONT& sickcont = bpp->GetSickCont( sickID );
    u8 sickCausePokeID = SICKCONT_GetCausePokeID( sickcont );

    if( scproc_SimpleDamage_CheckEnable( bpp, damage, damageCause ) )
    {
      fEnable = true;
      switch( sickID ){
      case pml::wazadata::WAZASICK_DOKU:
        scPut_EffectByPokePos( bpp, BTLEFF_DOKU );
        break;
      case pml::wazadata::WAZASICK_YAKEDO:
        scPut_EffectByPokePos( bpp, BTLEFF_YAKEDO );
        break;
      case pml::wazadata::WAZASICK_NOROI:
        scPut_EffectByPokePos( bpp, BTLEFF_NOROI );
        break;
      case pml::wazadata::WAZASICK_AKUMU:
        scPut_EffectByPokePos( bpp, BTLEFF_AKUMU );
        break;
      }
      HANDEX_STR_Clear( &m_strParam );
      BTL_SICK_MakeSickDamageMsg( &m_strParam, bpp, sickID );
      scproc_SimpleDamage_Core( bpp, damage, damageCause, sickCausePokeID, &m_strParam, true );
    }
  }

  if(!fEnable){
//    scproc_HandEx_Root( ITEM_DUMMY_DATA );
  }

  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] 状態異常によるターンチェックダメージ
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_SickDamage( BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sickID );
    EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
    EVENT_CallHandlers( this, BTL_EVENT_SICK_DAMAGE );
    damage = EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
  EVENTVAR_Pop();
  return damage;
}
//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > サイドエフェクト
//------------------------------------------------------------------
void  ServerFlow::scproc_turncheck_side( void )
{
  for( u32 side_val=0; side_val<BTL_SIDE_NUM; ++side_val )
  {
    for( u32 effect_val=BTL_SIDEEFF_START; effect_val<BTL_SIDEEFF_MAX; ++effect_val )
    {
      BtlSide       side   = static_cast<BtlSide>( side_val );
      BtlSideEffect effect = static_cast<BtlSideEffect>( effect_val );

      SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, effect );

      if( sideEffectStatus->GetAddCount() )
      {
        const BTL_SICKCONT& contParam = sideEffectStatus->GetContParam();
        if( (contParam.type == pml::wazadata::SICKCONT_TURN)
        ||  (contParam.type == pml::wazadata::SICKCONT_POKETURN)
        ){
          scPut_SideEffect_IncTurnCount( side, effect );

          if( sideEffectStatus->IsTurnPassed() )
          {
            scPut_SideEffect_Remove( side, effect );
            scPut_SideEffectOffMsg( side, effect );
          }
        }
      }
    }
  }
}

void ServerFlow::scPut_SideEffectOffMsg( BtlSide side, BtlSideEffect sideEffect )
{
  int strID = -1;

  switch( sideEffect ){
  case BTL_SIDEEFF_REFRECTOR:       strID = BTL_STRID_STD_ReflectorOff; break;
  case BTL_SIDEEFF_HIKARINOKABE:    strID = BTL_STRID_STD_HikariNoKabeOff; break;
  case BTL_SIDEEFF_AURORAVEIL:      strID = BTL_STRID_STD_AuroraVeilOff; break;
  case BTL_SIDEEFF_SINPINOMAMORI:   strID = BTL_STRID_STD_SinpiNoMamoriOff; break;
  case BTL_SIDEEFF_SIROIKIRI:       strID = BTL_STRID_STD_SiroiKiriOff; break;
  case BTL_SIDEEFF_OIKAZE:          strID = BTL_STRID_STD_OikazeOff; break;
  case BTL_SIDEEFF_OMAJINAI:        strID = BTL_STRID_STD_OmajinaiOff; break;
  case BTL_SIDEEFF_MAKIBISI:        strID = BTL_STRID_STD_MakibisiOff; break;
  case BTL_SIDEEFF_DOKUBISI:        strID = BTL_STRID_STD_DokubisiOff; break;
  case BTL_SIDEEFF_STEALTHROCK:     strID = BTL_STRID_STD_StealthRockOff; break;
  case BTL_SIDEEFF_NEBANEBANET:     strID = BTL_STRID_STD_NebaNebaNetOff; break;
  case BTL_SIDEEFF_RAINBOW:         strID = BTL_STRID_STD_RainbowOff; break;
  case BTL_SIDEEFF_BURNING:         strID = BTL_STRID_STD_BurningOff; break;
  case BTL_SIDEEFF_MOOR:            strID = BTL_STRID_STD_MoorOff; break;
  default:
    break;
  }

  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( m_que, strID, side );
  }
}

//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > フィールドエフェクト
//------------------------------------------------------------------
void  ServerFlow::scproc_turncheck_field( void )
{
  m_fieldStatus->TurnCheck( this );
  SCQUE_PUT_OP_TurnCheckField( m_que );
}
//- static void turncheck_field_callback( FieldStatus::EffectType effect, void* arg )
void ServerFlow::TurnCheckCallback_Field( FieldStatus::EffectType effect )
{
  scproc_FieldEff_End( effect );
}
void  ServerFlow::scproc_FieldEff_End( FieldStatus::EffectType effect )
{
  int strID = -1;
  switch( effect ){
  case FieldStatus::EFF_TRICKROOM:   strID = BTL_STRID_STD_TrickRoomOff; break; ///< トリックルーム
  case FieldStatus::EFF_JURYOKU:     strID = BTL_STRID_STD_JyuryokuOff; break;  ///< じゅうりょく
  case FieldStatus::EFF_WONDERROOM:  strID = BTL_STRID_STD_WonderRoom_End; break;
  case FieldStatus::EFF_MAGICROOM:   strID = BTL_STRID_STD_MagicRoom_End; break;
  case FieldStatus::EFF_MIZUASOBI:   strID = BTL_STRID_STD_MizuasobiOff; break;
  case FieldStatus::EFF_DOROASOBI:   strID = BTL_STRID_STD_DoroasobiOff; break;

  case FieldStatus::EFF_GROUND:
    switch( m_fieldStatus->GetGround() ){
    case BTL_GROUND_GRASS:  strID = BTL_STRID_STD_GrassGround_End; break;
    case BTL_GROUND_MIST:   strID = BTL_STRID_STD_MistGround_End; break;
    case BTL_GROUND_ELEKI:  strID = BTL_STRID_STD_ElecField_End; break;
    case BTL_GROUND_PHYCHO: strID = BTL_STRID_STD_PhychoField_End; break;
    }
    break;
  }

  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( m_que, strID );
  }
  SCQUE_PUT_OP_RemoveFieldEffect( m_que, effect );

  // マジックルームが切れたらアイテム反応チェック
  if( effect == FieldStatus::EFF_MAGICROOM )
  {
    BTL_POKEPARAM* bpp;

    storeFrontPokeOrderAgility( m_psetTmp );
    m_psetTmp->SeekStart();
    while( (bpp = m_psetTmp->SeekNext()) != NULL )
    {
      if( bpp->IsFightEnable() )
      {
        scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_GEN );
      }
    }
  }
  // グラウンド効果が切れたら基本状態に戻す
  else if( effect == FieldStatus::EFF_GROUND )
  {
    BtlGround baseGrnd = m_fieldStatus->GetBaseGround();
    BTL_SICKCONT cont = SICKCONT_MakePermanent( BTL_POKEID_NULL );
    scproc_ChangeGround( BTL_POKEID_NULL, baseGrnd, cont );
    SCQUE_PUT_ACT_EffectField( m_que, BTL_GROUND_NONE );
  }
}

//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > 天候
//------------------------------------------------------------------
bool  ServerFlow::scproc_turncheck_weather( PokeSet* pokeSet )
{
  BtlWeather weather = m_fieldStatus->TurnCheckWeather();

  // ターンチェックにより天候が終わった
  if( weather != BTL_WEATHER_NONE )
  {
    SCQUE_PUT_ACTOP_WeatherEnd( m_que, weather );
    scproc_ChangeWeatherAfter( BTL_WEATHER_NONE );

    BtlWeather defaultWeather = m_mainModule->GetDefaultWeather();
    if( defaultWeather != BTL_WEATHER_NONE )
    {
      //@バトル変更箇所 ここには戻り値があるが、sango特殊天候上書きは起きないため無視
      scproc_ChangeWeather( defaultWeather, BTL_WEATHER_TURN_PERMANENT, 0, BTL_POKEID_NULL );
    }
    return false;
  }
  // 天候が継続
  else
  {
    BTL_POKEPARAM* bpp;
    bool fCameraReset = false;

    weather = scEvent_GetWeather();
    pokeSet->SeekStart();
    while( (bpp = pokeSet->SeekNext()) != NULL )
    {
      if( (!bpp->IsDead())
      &&  (!bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_ANAWOHORU))
      &&  (!bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_DIVING))
      ){
        u32 hem_state = m_HEManager.PushState();
        int  damage = calc::RecvWeatherDamage( bpp, weather );
        damage = scEvent_CheckWeatherReaction( bpp, weather, damage );

//        if( !scproc_HandEx_Result() )
        {
          if( damage )
          {
            u8 damageCausePokeID = m_fieldStatus->GetWeatherCausePokeID();
            scPut_WeatherDamage( bpp, weather, damage, damageCausePokeID );
            fCameraReset = true;
          }
        }

        m_HEManager.PopState( hem_state );
        scproc_CheckDeadCmd( bpp, false );
      }

      // バトル離脱リクエストが発行されたら、途中で終了する
      // ( 野生戦で天候ダメージにより特性「にげごし」が発動した場合 )
      if( m_fQuitBattleRequest )
      {
        break;
      }
    }
    // 出口で確実にカメラを元に戻す。
    if(fCameraReset){
      scproc_ViewEffect( BTLEFF_CAMERA_INIT_ORTHO_NO_WAIT, BTL_POS_NULL, BTL_POS_NULL, false, 0 );
    }
    SCQUE_PUT_OP_INC_WEATHER_PASSED_TURN( m_que );
    return scproc_CheckExpGet();
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] 天候リアクションチェック
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
int  ServerFlow::scEvent_CheckWeatherReaction( const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage )
{
  u8  failFlag;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENTVAR_SetValue( BTL_EVAR_DAMAGE,  damage );

    EVENT_CallHandlers( this, BTL_EVENT_WEATHER_REACTION );
    damage = EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  if( failFlag ){
    return 0;
  }
  return damage;
}
//----------------------------------------------------------------------------------
/**
 * [Put]天候によるダメージ処理
 *
 * @param   wk
 * @param   bpp
 * @param   weather
 * @param   damage
 * @param   damageCausePokeID  ダメージの原因となったポケモンのID( 存在しないなら BTL_POKEID_NULL )
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_WeatherDamage( BTL_POKEPARAM* bpp, BtlWeather weather, int damage, u8 damageCausePokeID )
{
  u8 pokeID = bpp->GetID();

  switch( weather ){
  case BTL_WEATHER_SAND:
    HANDEX_STR_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Sand );
    HANDEX_STR_AddArg( &m_strParam, pokeID );
    break;
  case BTL_WEATHER_SNOW:
    HANDEX_STR_Setup( &m_strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Snow );
    HANDEX_STR_AddArg( &m_strParam, pokeID );
    break;
  default:
    HANDEX_STR_Clear( &m_strParam );
    break;
  }

  if( damage > 0 )
  {
    if( scproc_SimpleDamage_CheckEnable(bpp, damage, DAMAGE_CAUSE_OTHER) )
    {
      handexSub_putString( &m_strParam );
      HANDEX_STR_Clear( &m_strParam );

      scproc_ViewEffect( BTLEFF_WEATHER_DAMAGE, Hnd_PokeIDtoPokePos(pokeID), BTL_POS_NULL, false, 0 );
      // このダメージでひんしになるのであれば、カメラを元に戻す。
      if( bpp->GetValue(BTL_POKEPARAM::BPP_HP) <= damage ){
        scproc_ViewEffect( BTLEFF_CAMERA_INIT_ORTHO_NO_WAIT, BTL_POS_NULL, BTL_POS_NULL, false, 0 );
      }

      scproc_SimpleDamage_Core( bpp, damage, DAMAGE_CAUSE_WEATHER, damageCausePokeID, NULL, true );
    }
  }
}



/**
 * @brief バトルの総ターン数をインクリメントする
 */
void ServerFlow::scPut_IncBattleTurnCount( void )
{
  if( m_turnCount < BTL_TURNCOUNT_MAX )
  {
    m_turnCount++;
    SCQUE_PUT_OP_IncBattleTurnCount( m_que );
  }
}


/**
 * @brief 「援軍・乱入」呼び出し
 * @retval true   援軍 or 乱入が発生した場合
 * @retval false  援軍 or 乱入が発生しなかった場合
 */
bool ServerFlow::scproc_IntrudeCall( void )
{
  bool doCall   = false;
  bool doAppear = false;

  if( m_reinforceSystem != NULL )
  {
    scproc_IntrudeCall_Reinforce( &doCall, &doAppear, &m_intrudeParam, BTL_CLIENT_ENEMY1 );
  }
  else if( m_intrudeSystem != NULL )
  {
    scproc_IntrudeCall_Help( &doCall, &doAppear, &m_intrudeParam, BTL_CLIENT_ENEMY1 );
  }

  if( !doCall )
  {
    BTL_PRINT( "[ServerFlow] 助けを呼びませんでした\n" );
    return false;
  }

  // 助けを呼ぶ演出
  SCQUE_PUT_ACT_IntrudeCall( m_que, m_intrudeParam.callPokePos );
  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_Intrude_Wait );
  BTL_PRINT( "[ServerFlow] 助けを呼びました\n" );

  if( !doAppear )
  {
    SCQUE_PUT_ACT_IntrudeNotAppeared( m_que, m_intrudeParam.callPokePos );
    BTL_PRINT( "[ServerFlow] 助けが来ませんでした\n" );
    return false;
  }

  return true;
}

/**
 * @brief 「援軍」呼び出し
 * @param[out] doCall        助けを呼ぶか？
 * @param[out] doAppear      助けが来るか？
 * @param[out] intrudeParam  乱入パラメータ
 * @param      callClientId  助けを呼ぶクライアントのID
 */
void ServerFlow::scproc_IntrudeCall_Reinforce( bool* doCall, bool* doAppear, IntrudeParam* intrudeParam, BTL_CLIENT_ID callClientId )
{
  if( m_reinforceSystem == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  IntrudeParam_Clear( intrudeParam );
  intrudeParam->callClientID = callClientId;
  intrudeParam->intrudeType  = BTL_INTRUDE_TYPE_REINFORCE;

  m_reinforceSystem->CallHelp( 
    callClientId, 
    doCall,
    doAppear,
    &( intrudeParam->callPokeID ), 
    &( intrudeParam->callPokePos ),
    &( intrudeParam->appearPokePos ),
    intrudeParam->appearPokeParam );
}

/**
 * @brief 「乱入」呼び出し
 * @param[out] doCall        助けを呼ぶか？
 * @param[out] doAppear      助けが来るか？
 * @param[out] intrudeParam  乱入パラメータ
 * @param      callClientId  助けを呼ぶクライアントのID
 */
void ServerFlow::scproc_IntrudeCall_Help( bool* doCall, bool* doAppear, IntrudeParam* intrudeParam, BTL_CLIENT_ID callClientId )
{
  if( m_intrudeSystem == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  BtlWeather weather = scEvent_GetWeather();

  IntrudeParam_Clear( intrudeParam );
  intrudeParam->callClientID = callClientId;
  intrudeParam->intrudeType  = BTL_INTRUDE_TYPE_INTRUDE;

  m_intrudeSystem->CallHelp( 
    weather,
    callClientId, 
    doCall,
    doAppear,
    &( intrudeParam->callPokeID ), 
    &( intrudeParam->callPokePos ),
    &( intrudeParam->appearPokePos ),
    intrudeParam->appearPokeParam );
}


//----------------------------------------------------------------------------------
/**
 * 指定ポケモンを含むチームが全てひん死処理に入ったかチェック（BGM再生判断）
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//----------------------------------------------------------------------------------
bool ServerFlow::checkPokeDeadFlagAllOn( u8 pokeID )
{
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );
  BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );
  u32 members = party->GetMemberCount();

  for(u32 i=0; i<members; ++i)
  {
    BTL_POKEPARAM* bpp = party->GetMemberData(i);
    pokeID = bpp->GetID();
    if( !m_deadRec.IsDeadNow(pokeID) ){
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * scproc_CheckDeadCmd のひん死処理を複数体まとめてチェックするべきかどうか判定
 */
//--------------------------------------------------------------------------
bool ServerFlow::checkDeadCmdPlural( BTL_POKEPARAM** bpp, u32 poke_cnt, u8* bDead )
{
  for(u32 i=0; i<poke_cnt; ++i){
    bDead[i] = false;
  }
  return false;
}
/**
 * scproc_CheckDeadCmd のひん死処理を複数対まとめてチェックした際、飛ばしたコマンドの代替処理
 */
void ServerFlow::scproc_CheckDeadCmdAfterForPlural( BTL_POKEPARAM** bpp, u32 poke_cnt, u8* bDead )
{
  u32 dead_poke_cnt = 0;
  for(u32 i=0; i<poke_cnt; ++i){
    if( bDead[i] ){
      dead_poke_cnt++;
    }
  }
  scmd::SCQUE_PUT_ExArgStart( m_que, dead_poke_cnt );
  for(u32 i=0; i<poke_cnt; ++i)
  {
    if( bDead[i] ){
        scmd::SCQUE_PUT_ArgOnly( m_que, bpp[i]->GetID() );
    }
  }

  SCQUE_PUT_ACT_DeadPlural( m_que, dead_poke_cnt );
}


//--------------------------------------------------------------------------
/**
 * 対象ポケモンが死んでいたら死亡処理＆必要コマンド生成
 *
 * @param   poke          チェック対象ポケモン
 * @param   bPluralMode   複数体同時ひんしチェックの途中かどうか（群れバトルのみ）
 * @param   pgl_param     ひん死記録をPGL集計データにするためのパラメータ（NULLの時、相手ポケモンなどのデータを不明として記録する）
 *
 * @retval  bool    対象ポケモンが死亡し、必要なコマンド生成がされたら true
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scproc_CheckDeadCmd( BTL_POKEPARAM* poke, bool bPluralMode, const MainModule::PGLRecParam* pgl_param )
{
  u8 pokeID = poke->GetID();

  if( !m_deadRec.IsDeadNow(pokeID) )
  {
    if( poke->IsDead() )
    {
      BTL_N_Printf( DBGSTR_SVFL_PokeDead, pokeID);

      scPut_NotifyPGLRecord( poke, pgl_param );

      // ポケモンが死亡したことを記録する
      recordPokeDead( pokeID );

      // ラストのシン・ムは「○○は　たおれた」メッセージを表示しない
      // 複数回攻撃を受けている最中のポケモンも同様
      if( ((m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_LEGEND_EX) == false) ||
          (MainModule::PokeIDtoClientID(pokeID) == BTL_CLIENT_PLAYER))
      &&  (m_thruDeadMsgPokeID != pokeID)
      ){
        if( !bPluralMode ){
           SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Dead, pokeID );
        }
      }

      const u8 deadCausePokeID = poke->GetDeadCausePokeID();
      const DamageCause deadCause = poke->GetDeadCause();
      const bool isKillCountInc = isKillCountIncrementEnable( pokeID, deadCausePokeID, deadCause );
      const bool isKillCountEffect = isKillCountEffectEnable( isKillCountInc );

      if( !bPluralMode )
      {
        SCQUE_PUT_ACT_Dead( m_que, pokeID, isKillCountEffect );
        scproc_ClearPokeDependEffect( poke );
      }

      scEvent_PokeDeadActionAfter( pokeID );

      poke->Clear_ForDead();

      // プレイヤーのポケモンが死んだ時になつき度計算
      if( MainModule::PokeIDtoClientID(pokeID) == m_mainModule->GetPlayerClientID() )
      {
        int deadPokeLv = poke->GetValue( BTL_POKEPARAM::BPP_LEVEL );
        int enemyMaxLv = checkExistEnemyMaxLevel();
        bool fLargeDiffLevel = ( (deadPokeLv + 30) <= enemyMaxLv );

        BTL_N_Printf( DBGSTR_SVFL_DeadDiffLevelCheck, deadPokeLv, enemyMaxLv);
        m_mainModule->ReflectNatsukiDead( poke, fLargeDiffLevel );
      }

      m_posPoke.PokeOut( pokeID );

      // ポケモンを倒した数をカウント
      if( isKillCountInc )
      {
        BTL_POKEPARAM* deadCausePoke = m_pokeCon->GetPokeParam( deadCausePokeID );
        scPut_IncKillCount( deadCausePoke );
      }

      // レコード更新
      {
        u8 clientID = MainModule::PokeIDtoClientID( pokeID );
        u8 playerClientID = m_mainModule->GetPlayerClientID();
        if( clientID == playerClientID )
        {
          m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_TEMOTI_KIZETU );
        }
        else if( !m_mainModule->IsFriendClientID(clientID, playerClientID) )
        {
          m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_KILL_POKE );
          m_mainModule->RECORDDATA_Inc( Savedata::Record::RECID_DAYCNT_KILL );

          //momiji追加
          //ロトムカウント追加
          if (!m_mainModule->IsRecordPlayMode())
          {
            App::Tool::RotomFriendship::AddCharacterCounter(Savedata::FieldMenu::ROTOM_CHAR_BELLIGERENT,1);
          }
        }
      }

      scEvent_PokeDeadAfter( pokeID );

      return true;
    }
  }
  else
  {
    BTL_N_Printf( DBGSTR_SVFL_DeadAlready, pokeID);
  }

  return false;
}

/**
 * @brief 「相手を倒した数」のインクリメントが発生するか？
 * @param deadPokeID       倒れたポケモンのID
 * @param deadCausePokeID  倒したポケモンのID
 * @param deadCause        倒れた原因
 * @retval true   発生する
 * @retval false  発生しない
 */
 bool ServerFlow::isKillCountIncrementEnable( u8 deadPokeID, u8 deadCausePokeID, DamageCause deadCause ) const
 {
   // 倒れたポケモンと倒したポケモンが同じ陣営なら、発生しない
   if( m_mainModule->IsFriendPokeID( deadPokeID, deadCausePokeID ) )
   {
     return false;
   }

   if( ( deadCausePokeID != BTL_POKEID_NULL ) &&
       ( deadCausePokeID != deadPokeID ) )
   {
     return true;
   }

   return false;
 }

 /**
  * @brief 「相手を倒した数」が加算される演出を表示するか？
  * @param isKillCountInc  倒した数のインクリメントが発生するか？
  * @retval true   演出を表示する
  * @retval false  演出を表示しない
  */
 bool ServerFlow::isKillCountEffectEnable( bool isKillCountInc ) const
 {
   if( !isKillCountInc )
   {
     return false;
   }

   const BtlRule rule = m_mainModule->GetRule();
   return ( rule == BTL_RULE_ROYAL );
 }

/**
 * PGL Record 通知を各クライアントに行わせる
 * @param defPoke
 * @param pgl_param
 */
void ServerFlow::scPut_NotifyPGLRecord( const BTL_POKEPARAM* defPoke, const MainModule::PGLRecParam* pgl_param )
{
  // 通信対応させるためにこちらを有効にする必要がある
  #if 1
  u8 defPokeID = defPoke->GetID();
  u8 atkPokeID = BTL_POKEID_NULL;
  WazaID  waza = WAZANO_NULL;

  if( pgl_param )
  {
    atkPokeID = pgl_param->attacker->GetID();
    waza      = pgl_param->wazaID;
  }
  SCQUE_PUT_OP_PGLRecord( m_que, defPokeID, atkPokeID, waza );
  #else
  // こちらは通信非対応の動作
  m_mainModule->NotifyPGLRecord( defPoke, pgl_param );
  #endif
}

/**
 * @brief ポケモンが死亡したことを記録する
 * @param deadPokeId  死亡したポケモンのID
 */
void ServerFlow::recordPokeDead( u8 deadPokeId )
{
  // ポケモンの死亡を記録
  m_deadRec.Add( deadPokeId );

  // パーティの全滅を記録
  {
    const u8         clientId = m_mainModule->PokeIDtoClientID( deadPokeId );
    const BTL_PARTY* party    = m_pokeCon->GetPartyDataConst( clientId );
    if( party->GetAliveMemberCount() == 0 )
    {
      m_partyAllDeadRecorder.RecordPartyAllDead( clientId );
    }
  }

  // メインモジュールに通知（乱入ポケモンID管理用）
  m_mainModule->NotifyPokemonDead( deadPokeId );
}

/**
 * @brief [Event] ポケモンが死亡して退場した直後( 死亡したポケモンの退場処理を行う前 )
 * @param deadPokeID  死亡したポケモンのID
 */
void ServerFlow::scEvent_PokeDeadActionAfter( u8 deadPokeID )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, deadPokeID );
    EVENT_CallHandlers( this, BTL_EVENT_DEAD_ACTION_AFTER );
  EVENTVAR_Pop();
}

/**
 * @brief [Event] ポケモンが死亡後の退場処理が終了した後
 * @param deadPokeID  死亡したポケモンのID
 */
void ServerFlow::scEvent_PokeDeadAfter( u8 deadPokeID )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, deadPokeID );
    EVENT_CallHandlers( this, BTL_EVENT_DEAD_AFTER );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * 相手サイドの場に出ているポケモンの内、最大レベルを取得
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
u32 ServerFlow::checkExistEnemyMaxLevel( void )
{
  u32 maxLv = 1;
  u32 posEnd = m_mainModule->GetValidPosMax();
  u8  playerClientID = m_mainModule->GetPlayerClientID();
  u8  clientID;
  u32 pos;

  for(pos=0; pos<=posEnd; ++pos)
  {
    clientID = m_mainModule->BtlPosToClientID( pos );
    if( clientID == BTL_CLIENTID_NULL ){
      continue;
    }

    if( !m_mainModule->IsFriendClientID(playerClientID, clientID) )
    {
      u8 pokeID = m_posPoke.GetExistPokeID( pos );
      if( pokeID != BTL_POKEID_NULL )
      {
        const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
        u32 lv = bpp->GetValue( BTL_POKEPARAM::BPP_LEVEL );
        if( lv > maxLv ){
          maxLv = lv;
        }
      }
    }
  }

  return maxLv;
}

//--------------------------------------------------------------------------
/**
 * 特定ポケモン依存の状態異常・サイドエフェクト等、各種ハンドラをクリアする
 */
//--------------------------------------------------------------------------
void  ServerFlow::scproc_ClearPokeDependEffect( BTL_POKEPARAM* poke )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 dead_pokeID = poke->GetID();

  scEvent_BeforeDead( poke );
  scproc_FreeFall_CheckRelease( poke, true, true );

  BTL_PRINT("[svf] ClearPokeDependEffct ... pokeID=%d", dead_pokeID);
  BTL_PRINT(" tokusei remove start..\n");
  handler::tokusei::Remove( poke );
  BTL_PRINT(" item remove start..\n");
  handler::item::Remove( poke );
  BTL_PRINT(" item removed\n");
  handler::default_power_up::Remove( poke );

  handler::waza::RemoveForceAll( poke );

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext(&fps, &bpp) )
  {
    bpp->CureWazaSickDependPoke( dead_pokeID );
    SCQUE_PUT_OP_CureSickDependPoke( m_que, bpp->GetID(), dead_pokeID );
  }
  m_fieldStatus->RemoveDependPokeEffect( dead_pokeID );
  SCQUE_PUT_OP_DeleteDependPokeFieldEffect( m_que, dead_pokeID );

  if( !scproc_CheckShowdown() || !isSkipBattleAfterShowdown() )
  {
    u16 tokusei = poke->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE );
    if( tokusei == TOKUSEI_KINTYOUKAN )
    {
      scproc_KintyoukanMoved( poke );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] 死亡直前通知
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_BeforeDead( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_NOTIFY_DEAD );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * 死亡ポケレコードを見て必要なら経験値取得の処理を行う
 *
 * @retval  bool    経験値取得処理によりレベルアップしたら（取得条件を満たしていたら）true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_CheckExpGet( void )
{
  bool bLevelUp = false;

  // 決着がついているなら「お手入れボタン」の表示を開始する
  if( scproc_CheckShowdown() &&
      isSkipBattleAfterShowdown() &&
      checkPlayerSideAlive() ) 
  {
    scproc_DisplayOteireButton();
  }

  if( m_mainModule->IsExpSeqEnable() )
  {
    BTL_PARTY* party = m_pokeCon->GetPartyData( m_mainModule->GetPlayerClientID() );

    for(u32 i=0; i<GFL_NELEMS(m_calcExpResult); ++i){
      m_calcExpResult[i].Clear();
    }

    const BtlRule rule = m_mainModule->GetRule();
    u32 deadPokeCnt = m_deadRec.GetCount( 0 );
    bool bGetExp = false;
    for(u32 i=0; i<deadPokeCnt; ++i)
    {
      if( !m_deadRec.GetExpCheckedFlag(0, i) )
      {
        u8 pokeID = m_deadRec.GetPokeID( 0, i );
        if( m_mainModule->PokeIDtoSide(pokeID) == BTL_SIDE_2ND )
        {
          const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
          if( m_flowSub.CalcExp( rule, this, party, bpp, m_calcExpWork ) )
          {
            for(u32 j=0; j<GFL_NELEMS(m_calcExpResult); ++j){
              m_calcExpResult[j].Marge( m_calcExpWork[j] );
            }
            bGetExp = true;
          }
        }
        m_deadRec.SetExpCheckedFlag( 0, i );
      }
    }

    // 野生戦でプレイヤー勝利ならこの時点（経験値取得の前）でBGM再生コマンド
    if( bGetExp )
    {
      if( (m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD)
      && !(m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ))  // ぬし戦は、勝利演出の中で勝利BGMを再生します
      &&  (m_fWinBGMPlayWild == false)
      &&  scproc_CheckShowdown()
      &&  checkPlayerSideAlive()
      ){
        u32 WinBGM = m_mainModule->GetWinBGMNo();
        SCQUE_PUT_ACT_PlayWinBGM( m_que, WinBGM );
        m_fWinBGMPlayWild = true;
      }

      bLevelUp = scproc_GetExp( party, m_calcExpResult );
    }
  }
  return bLevelUp;
}

/**
 * @brief 「お手入れボタン」の表示を開始する
 */
void ServerFlow::scproc_DisplayOteireButton( void )
{
  // ルールにより表示できない
  if( !m_mainModule->IsOteireEnable() )
  {
    return;
  }

  // 誰も汚れていないなら、表示しない
  {
    const BTL_PARTY* party = m_pokeCon->GetPartyDataConst( BTL_CLIENT_PLAYER );
    if( !Oteire::IsOteireTargetPokeExist( *party ) )
    {
      return;
    }
  }

  // お手入れボタンを表示
  SCQUE_PUT_ACT_OteireButtonDisplay( m_que );
}

//----------------------------------------------------------------------------------
/**
 * 捕獲ポケモン情報を見て必要なら経験値取得の処理を行う
 *
 * @retval  bool    経験値取得処理によりレベルアップしたら（取得条件を満たしていたら）true
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_CheckExpGetByCaptured( u8 playerClientID )
{
  // 「お手入れボタン」の表示
  scproc_DisplayOteireButton();

  if( (m_mainModule->IsExpSeqEnable())
  &&  (m_getPokePos != BTL_POS_NULL)
  ){
    const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( m_getPokePos );
    if( bpp )
    {
      scproc_CreateFurimukiCmd_FrontPoke( playerClientID );

      BTL_PARTY* party = m_pokeCon->GetPartyData( m_mainModule->GetPlayerClientID() );

      for(u32 i=0; i<GFL_NELEMS(m_calcExpResult); ++i){
        m_calcExpResult[i].Clear();
      }

      const BtlRule rule = m_mainModule->GetRule();
      m_flowSub.CalcExp( rule, this, party, bpp, m_calcExpWork );
      for(u32 i=0; i<GFL_NELEMS(m_calcExpResult); ++i){
        m_calcExpResult[i].Marge( m_calcExpWork[i] );
      }

      return scproc_GetExp( party, m_calcExpResult );
    }
  }
  return false;
}
/**
 *  プレイヤー側の場に出ているポケモンに振り向きアクションをさせるコマンド生成
 */
void ServerFlow::scproc_CreateFurimukiCmd_FrontPoke( u8 playerClientID )
{
  u32 frontPosCount = m_mainModule->GetClientFrontPosCount( playerClientID );
  for(u32 i=0; i<frontPosCount; ++i)
  {
    BtlPokePos  pos = m_mainModule->GetClientPokePos( playerClientID, i );
    if( pos == BTL_POS_NULL ){
      continue;
    }
    u8 pokeID = m_posPoke.GetExistPokeID( pos );
    if( pokeID == BTL_POKEID_NULL ){
      continue;
    }

    const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParamConst( pokeID );
    if( getFriendship(bpp) >= 150 ){
      SCQUE_PUT_ACT_FriendshipEffect( m_que, pokeID, FREFF_FURIMUKI );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * パーティ内ポケモンが経験値を取得する要素があるか判定
 *
 * @param[in     party                  パーティデータ
 * @param[in]    calcExp                パーティに対する経験値計算結果ワーク
 * @param[in]    idx                    何番目のポケモンを調べるか (0 org)
 * @param[out]   exp                    獲得できる経験値（※この値が0で戻り値がNULLでない場合、努力値のみが入る）
 * @param[out]   bGakusyuSoutiOnly      獲得した経験値が「がくしゅうそうち」の効果によるもののみである場合 true を格納する
 *
 * @retval  BTL_POKEPARAM*              対象ポケモンデータ
 */
//----------------------------------------------------------------------------------
BTL_POKEPARAM* ServerFlow::checkRightsOfGettinExp( BTL_PARTY* party, const ServerFlowSub::CALC_EXP_WORK* calcExp, u32 idx, u32* exp, bool *bGakusyuSoutiOnly )
{
  *exp = 0;
  *bGakusyuSoutiOnly = false;

  BTL_POKEPARAM* bpp = party->GetMemberData( idx );
  if( !bpp ){
    return NULL;
  }

  if( !bpp->IsFightEnable() ){
    return NULL;
  }

  if( !calcExp[idx].exp ){
    return NULL;
  }

  if( bpp->GetValue(BTL_POKEPARAM::BPP_LEVEL) >= pml::MAX_POKE_LEVEL ){
    return bpp;
  }

  *exp = calcExp[idx].exp;
  *bGakusyuSoutiOnly = calcExp[idx].bGakusyuSoutiOnly;
  return bpp;
}

//----------------------------------------------------------------------------------
/**
 * 経験値取得処理コマンド生成
 *
 * @param   party       自分パーティ（経験値入る側）
 * @param   calcExp     経験値計算結果ワーク
 *
 * @retval  bool    経験値取得処理によりレベルアップしたら（取得条件を満たしていたら）true
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scproc_GetExp( BTL_PARTY* party, const ServerFlowSub::CALC_EXP_WORK* calcExp )
{
  bool result = false;
  bool bGakusyuSoutiEnable = false;

  BTL_POKEPARAM* bpp;
  u32   exp;
  bool  bGakusyuSoutiOnly;
  u8    pokeID;

  for(u32 i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    bpp = checkRightsOfGettinExp( party, calcExp, i, &exp, &bGakusyuSoutiOnly );
    if( !bpp ){
      continue;
    }

    pokeID = bpp->GetID();

    scproc_AddEffortPower( bpp, &calcExp[i] );
    SCQUE_PUT_ACT_Naderu( m_que, pokeID );

    if( exp )
    {
      if( !bGakusyuSoutiOnly )
      {
        u16 strID = (calcExp[i].fBonus)? BTL_STRID_STD_GetExp_Bonus : BTL_STRID_STD_GetExp;
        SCQUE_PUT_MSG_STD( m_que, strID, pokeID, exp );
        scproc_AddExp( bpp, exp, &m_lvupInfo );
      }
      else
      {
        bGakusyuSoutiEnable = true;
      }

      m_mainModule->RECORDDATA_Add( Savedata::Record::RECID_DAYCNT_EXP, exp );
      result = true;
    }
  }

  if( bGakusyuSoutiEnable )
  {
    scPut_Message_StdEx( BTL_STRID_STD_GetExp_Bench, 0, NULL );
    for(uint32_t i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
    {
      bpp = checkRightsOfGettinExp( party, calcExp, i, &exp, &bGakusyuSoutiOnly );
      if( exp && bGakusyuSoutiOnly )
      {
        scproc_AddExp( bpp, exp, &m_lvupInfo );
      }
    }
  }

  return result;
}

/**
 * @brief 経験値加算
 *
 * @param      poke          経験値を加算するポケモン
 * @param      exp           加算する経験値
 * @param[out] levelUpInfo   レベルアップ時に各種パラメータの上昇値を格納する
 */
void ServerFlow::scproc_AddExp( BTL_POKEPARAM* poke, u32 exp, BTL_LEVELUP_INFO* levelUpInfo )
{
  u32 restExp = exp;

  for ever 
  {
    if( poke->AddExp( &restExp, levelUpInfo ) )
    {
      DiarySave::SetLevelUp( *poke );
    }
    else
    {
      break;
    }
  }

  u8 pokeID = poke->GetID();
  SCQUE_PUT_ACT_AddExp( m_que, pokeID, exp );
}

/**
 *  ポケモンに努力値を加算する
 */
void ServerFlow::scproc_AddEffortPower( BTL_POKEPARAM* bpp, const ServerFlowSub::CALC_EXP_WORK* calcExp )
{
  const bool isFull_before = bpp->IsEffortValueFull();

  // この順番は変えてはいけません（HP->POW->DEF->SP_POW->SP_DEF->AGI）
  bpp->AddEffortPower( pml::pokepara::POWER_HP,    calcExp->hp );
  bpp->AddEffortPower( pml::pokepara::POWER_ATK,   calcExp->pow );
  bpp->AddEffortPower( pml::pokepara::POWER_DEF,   calcExp->def );
  bpp->AddEffortPower( pml::pokepara::POWER_SPATK, calcExp->sp_pow );
  bpp->AddEffortPower( pml::pokepara::POWER_SPDEF, calcExp->sp_def );
  bpp->AddEffortPower( pml::pokepara::POWER_AGI,   calcExp->agi );

  // なぜならクライアントもこのコマンドにより同じ順番で処理しているためです
  // （努力値飽和チェックの影響で、順番が変わると結果が変わりうるのです）
  u8 pokeID = bpp->GetID();
  SCQUE_PUT_OP_Doryoku( m_que, pokeID, calcExp->hp, calcExp->pow, calcExp->def, calcExp->sp_pow, calcExp->sp_def, calcExp->agi );

  BTL_PRINT("poke=%d, 加算する努力値 : hp=%d, pow=%d, def=%d, spow=%d, sdef=%d, agi=%d\n",
          pokeID, calcExp->hp, calcExp->pow, calcExp->def, calcExp->sp_pow, calcExp->sp_def, calcExp->agi );

  // ダイアリー
  const bool isFull_after = bpp->IsEffortValueFull();
  if( !isFull_before && isFull_after )
  {
    DiarySave::SetEffortValueFull( *bpp );
  }
}

/**
 * 溜めワザにより画面から消えている状態を元に戻す
 * @param bpp                 対象となるポケモンパラメータ
 * @param hideContFlag        対象となる溜めワザ状態フラグ
 * @param bOmitCancelAction   このポケモンの現ターン行動をキャンセルする処理を行わない場合 true 
 * @return  指定された状態フラグを落とす処理を実行した場合 true を返す
 */
bool  ServerFlow::scproc_TameHideCancel( BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag hideContFlag, bool bOmitCancelAction )
{
  if( bpp->CONTFLAG_Get(hideContFlag) )
  {
    u8 pokeID = bpp->GetID();

    scPut_ResetContFlag( bpp, hideContFlag );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_TAMELOCK) ) {
      scPut_CureSick( bpp, pml::wazadata::WAZASICK_TAMELOCK, NULL );
    }
    SCQUE_PUT_ACT_TameWazaHide( m_que, pokeID, false );

    if( !bOmitCancelAction ){
      ActOrder_ForceDone( pokeID );
    }
    return true;
  }
  return false;
}
//---------------------------------------------------------------------------------------------
//  数値関連関数群
//---------------------------------------------------------------------------------------------
bool ServerFlow::perOccur( u8 per )
{
  return calc::GetRand( 100 ) < per;
}

//---------------------------------------------------------------------------------------------
//  イベントワーク確保
//---------------------------------------------------------------------------------------------
void ServerFlow::EventWorkStack::Init( void )
{
  m_sp = 0;
}
void* ServerFlow::EventWorkStack::Push( u32 size )
{
  GFL_ASSERT( m_sp < EVENT_WORK_DEPTH );

  while( size % 4 ){ ++size; }

  {
    u32 usedSize;
    u32 i;

    for(i=0, usedSize=0; i<m_sp; ++i)
    {
      usedSize += m_size[ i ];
    }

    if( (usedSize + size) < sizeof(m_work) )
    {
      void* adrs;
      adrs = &m_work[ usedSize ];
      m_size[ m_sp ] = size;
      m_sp++;
      return adrs;
    }
    else
    {
      GFL_ASSERT(0);
      return NULL;
    }
  }
}
void ServerFlow::EventWorkStack::Pop( void* adrs )
{
  GFL_ASSERT( m_sp );

  m_sp--;

  {
    u32 used_size = 0;

    for(u32 i=0; i<m_sp; ++i)
    {
      used_size += m_size[ i ];
    }

    if( adrs != &(m_work[used_size]) )
    {
      GFL_ASSERT(0); // 最後に確保されたワーク以外の領域が先に解放されようとしている
    }
  }
}

//---------------------------------------------------------------------------------------------
//
// コマンド出力層
// サーバーコマンドをバッファに生成 ＆ 必要ならそれに伴うポケモンパラメータ操作を行う
// イベント層の関数呼び出しは行われない
//
//---------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * [Put] 指定ポケモン位置にエフェクト発動
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_EffectByPokePos( const BTL_POKEPARAM* bpp, u16 effectNo )
{
  u8 pokeID = bpp->GetID();
  BtlPokePos pos = m_posPoke.GetPokeExistPos(pokeID );
  if( pos != BTL_POS_NULL )
  {
    SCQUE_PUT_ACT_EffectByPos( m_que, pos, effectNo );
  }
}

//--------------------------------------------------------------------------
/**
 * [Put] 「○○は　はんどうでうごけない」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_CantAction( BTL_POKEPARAM* bpp )
{
  scPut_Message_Set( bpp, BTL_STRID_SET_CantAction );
}
//--------------------------------------------------------------------------
/**
 * 「○○はこんらんしている」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_ConfCheck( const BTL_POKEPARAM* bpp )
{
  scPut_EffectByPokePos( bpp, BTLEFF_KONRAN );
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KonranAct, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * 「○○は△△にメロメロだ」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_MeromeroAct( const BTL_POKEPARAM* bpp )
{
  u8 target_pokeID = static_cast<u8>(bpp->GetSickParam( pml::wazadata::WAZASICK_MEROMERO ));
  scPut_EffectByPokePos( bpp, BTLEFF_MEROMERO );
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_MeromeroAct, bpp->GetID(), target_pokeID );
}
//--------------------------------------------------------------------------
/**
 * こんらんによる自爆ダメージ処理
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_ConfDamage( BTL_POKEPARAM* bpp, u32 damage )
{
  const BTL_SICKCONT& sickcont = bpp->GetSickCont( pml::wazadata::WAZASICK_KONRAN );
  const u8 damageCausePokeID = SICKCONT_GetCausePokeID( sickcont );
  const u8 pokeID = bpp->GetID();

  // こんらんはマジックガードの影響を受けないのでSimpleHPを直接呼んでいる  taya
  SCQUE_PUT_ACT_ConfDamage( m_que, pokeID );
  scPut_SimpleHp( bpp, (int)(damage)*-1, DAMAGE_CAUSE_KONRAN, damageCausePokeID, true );
  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_KonranExe, pokeID );

}
//--------------------------------------------------------------------------
/**
 * ワザ出し判定時のポケモン系状態異常回復処理
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_CurePokeSick( BTL_POKEPARAM* bpp, WazaSick sick, bool fStdMsg )
{
  u8 pokeID = bpp->GetID();

  bpp->CurePokeSick();
  SCQUE_PUT_OP_CurePokeSick( m_que, pokeID );
  SCQUE_PUT_ACT_SickIcon( m_que, pokeID, pml::pokepara::SICK_NULL );

  if( fStdMsg )
  {
    switch( sick ){
    case pml::pokepara::SICK_NEMURI:
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NemuriWake, pokeID );
      break;
    case pml::pokepara::SICK_KOORI:
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KoriMelt, pokeID );
      break;
    default:
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_PokeSick_Cure, pokeID );
      break;
    }
  }
}
//--------------------------------------------------------------------------
/**
 * @brief ワザメッセージ表示（○○の××こうげき！など）
 * 
 * @param attacker        技を使用するポケモン
 * @param waza            使用する技
 * @param isZenryokuWaza  使用する技が全力技か？
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaMsg( const BTL_POKEPARAM* attacker, WazaID waza, bool isZenryokuWaza )
{
  SCQUE_PUT_MSG_WAZA( m_que, attacker->GetID(), waza, isZenryokuWaza );
}
//----------------------------------------------------------------------------------
/**
 * [Put]状態異常回復処理のコマンド生成
 *
 * @param   wk
 * @param   bpp       対象ポケモン
 * @param   sick_id   状態異常コード
 * @param   oldCont   [out] 回復前の継続パラメータ（不要ならNULL)
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_CureSick( BTL_POKEPARAM* bpp, WazaSick sick, BTL_SICKCONT* oldCont )
{
  if( sick != pml::wazadata::WAZASICK_NULL )
  {
    u8 pokeID = bpp->GetID();

    if( oldCont ){
      *oldCont = bpp->GetSickCont( sick );
    }

    if( sick < pml::pokepara::SICK_MAX )
    {
      bpp->CurePokeSick();
      SCQUE_PUT_OP_CurePokeSick( m_que, pokeID );
      if ( m_posPoke.GetPokeExistPos(pokeID) != BTL_POS_NULL ){
        SCQUE_PUT_ACT_SickIcon( m_que, pokeID, pml::pokepara::SICK_NULL );
      }
    }
    else
    {
      bpp->CureWazaSick( sick );
      SCQUE_PUT_OP_CureWazaSick( m_que, pokeID, sick );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 拡張状態異常コードを、該当するコードに変換する
 *
 * @param   bpp
 * @param   exCode    [io]
 *
 * @retval  WazaSick    該当コード／該当コードなしの場合、pml::wazadata::WAZASICK_NULLを返す
 */
//----------------------------------------------------------------------------------
WazaSick ServerFlow::trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx* exCode )
{
  WazaSick result = pml::wazadata::WAZASICK_NULL;
  PokeSick poke_sick = bpp->GetPokeSick( );

  switch( (*exCode) ){
  case WAZASICK_EX_POKEFULL:
    // ポケモン系状態異常になっているならそれを治す
    if( poke_sick != pml::wazadata::WAZASICK_NULL ){
      result = static_cast<WazaSick>( poke_sick );
    }
    *exCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NULL);
    break;

  case WAZASICK_EX_POKEFULL_PLUS:
    // ポケモン系状態異常になっているならそれを治す
    if( poke_sick != pml::wazadata::WAZASICK_NULL ){
      result = static_cast<WazaSick>( poke_sick );
      *exCode = static_cast<BtlWazaSickEx>( pml::wazadata::WAZASICK_KONRAN );
      break;
    }
    if( bpp->CheckSick(pml::wazadata::WAZASICK_KONRAN) ){
      result = pml::wazadata::WAZASICK_KONRAN;
    }
    *exCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NULL);
    break;

  case WAZASICK_EX_MENTAL:  // メンタルハーブ
    result = calc::CheckMentalSick( bpp );
    if( result == pml::wazadata::WAZASICK_NULL ){
      *exCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NULL);
    }
    return result;

  default:
    if( bpp->CheckSick( static_cast<WazaSick>(*exCode)) ){
      result = static_cast<WazaSick>( (*exCode) );
    }
    *exCode = static_cast<BtlWazaSickEx>(pml::wazadata::WAZASICK_NULL);
    break;
  }

  return result;
}
//--------------------------------------------------------------------------
/**
 * 「しかしうまくきまらなかった」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaFail( const BTL_POKEPARAM* attacker, WazaID waza )
{
  GFL_UNUSED( attacker );
  GFL_UNUSED( waza );
  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
}
//--------------------------------------------------------------------------
/**
 * 「○○には当たらなかった」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaAvoid( const BTL_POKEPARAM* defender, WazaID waza )
{
  GFL_UNUSED( waza );
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_WazaAvoid, defender->GetID() );
}
//--------------------------------------------------------------------------
/**
 * 「○○には当たらなかった」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaAvoidByFriendship( const BTL_POKEPARAM* defender, WazaID waza )
{
  GFL_UNUSED( waza );

  u8 pokeID = defender->GetID();
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );

  SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_Avoid, pokeID, clientID );
  SCQUE_PUT_ACT_FriendshipEffect( m_que, pokeID, FREFF_HEART );
}
//--------------------------------------------------------------------------
/**
 * 「○○にはこうかがないようだ」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaNoEffect( const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, defender->GetID() );
}
//--------------------------------------------------------------------------
/**
 * 「○○には  ぜんぜんきいてない」表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaNoEffectIchigeki( const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NotEffect_Ichigeki, defender->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザによるダメージコマンド生成
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaDamagePlural( const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam,
  u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp, const u16* damage, bool fTargetPlural )
{
  GFL_UNUSED( fTargetPlural );

  BtlTypeAffAbout  affAbout;
  u8               affGoodCnt, affBadCnt;
  u32 i;

  affGoodCnt = affBadCnt = 0;

  // HP減少コマンド生成＆効果音用の相性判定
  for(i=0; i<poke_cnt; ++i)
  {
    scPut_DecreaseHP( bpp[i], damage[i], true, DAMAGE_CAUSE_WAZA, attacker->GetID() );
    if( aff[i] > pml::battle::TypeAffinity::TYPEAFF_1 ){
      ++affGoodCnt;
    }
    if( aff[i] < pml::battle::TypeAffinity::TYPEAFF_1 ){
      ++affBadCnt;
    }
  }

  // ダメージエフェクトコマンド生成---------------
  if( affGoodCnt ){
    affAbout = pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE;
  }else{
    affAbout = (affBadCnt)? pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE : pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL;
  }

    // まず拡張引数として対象ポケモンIDをセット
    const u8 ex_arg_num = poke_cnt * 2;
    scmd::SCQUE_PUT_ExArgStart( m_que, ex_arg_num );
    for(i=0; i<poke_cnt; ++i){
      scmd::SCQUE_PUT_ArgOnly( m_que, bpp[i]->GetID() );
      scmd::SCQUE_PUT_ArgOnly( m_que, aff[i] );
    }

    // 続けて本コマンドセット
    SCQUE_PUT_ACT_WazaDamagePlural( m_que, poke_cnt, affAbout, wazaParam->wazaID );
}
//--------------------------------------------------------------------------
/**
 * [Put] 相性メッセージ生成
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_WazaAffinityMsg( const BTL_POKEPARAM* attacker, u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp,  bool fTargetPlural )
{
  u8               affGoodCnt, affBadCnt;
  u32 i;

  affGoodCnt = affBadCnt = 0;

  // 効果音用の相性判定
  for(i=0; i<poke_cnt; ++i)
  {
    if( aff[i] > pml::battle::TypeAffinity::TYPEAFF_1 ){ ++affGoodCnt; }
    if( aff[i] < pml::battle::TypeAffinity::TYPEAFF_1 ){ ++affBadCnt; }
  }

  // 相性メッセージコマンド生成
  if( fTargetPlural )
  {
    SetMonsNoPokeID  pokeID( m_pokeCon );

    if( affGoodCnt )
    {
      for(i=0; i<poke_cnt; ++i)
      {
        if( aff[i] > pml::battle::TypeAffinity::TYPEAFF_1 ){
          pokeID.Add(bpp[i]->GetID() );
        }
      }
      switch( affGoodCnt ){
      case 1: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffGood_1, pokeID[0] ); break;
      case 2: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffGood_2, pokeID[0], pokeID[1] ); break;
      case 3:
      default:
          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffGood_3, pokeID[0], pokeID[1], pokeID[2] ); break;
      }

      scPut_AffGoodFriendshipMsg( attacker );

    }
    if( affBadCnt )
    {
      pokeID.Clear();
      for(i=0; i<poke_cnt; ++i)
      {
        if( aff[i] < pml::battle::TypeAffinity::TYPEAFF_1 ){
          pokeID.Add( bpp[i]->GetID() );
        }
      }
      switch( affBadCnt ){
      case 1: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffBad_1, pokeID[0] ); break;
      case 2: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffBad_2, pokeID[0], pokeID[1] ); break;
      case 3:
      default:
          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_AffBad_3, pokeID[0], pokeID[1], pokeID[2] ); break;
      }
    }
  }
  else
  {
    if( affGoodCnt )
    {
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_AffGood );
      scPut_AffGoodFriendshipMsg( attacker );
    }
    else if( affBadCnt ){
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_AffBad );
    }
  }
}
/**
 * [Put] なかよし度が高いポケモンが効果バツグンメッセージにつづけて「やったね！」表示
 * @param bpp
 */
void ServerFlow::scPut_AffGoodFriendshipMsg( const BTL_POKEPARAM* bpp )
{
  // かわいがりメッセージ「やったね！」
  if( getFriendship(bpp) >= 150 )
  {
    u8  pokeID = bpp->GetID();
    SCQUE_PUT_ACT_FriendshipEffectWithMsg( m_que, pokeID, FREFF_FURIMUKI,
                          BTL_STRTYPE_STD, BTL_STRID_STD_FR_AffGood, pokeID, 0 );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] クリティカルメッセージコマンド生成
 */
//--------------------------------------------------------------------------
void ServerFlow::scproc_CriticalMsg( BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, u32 poke_cnt, BTL_POKEPARAM** bpp, const CriticalType* criticalType, bool fTargetPlural )
{
  for(u32 i=0; i<poke_cnt; ++i)
  {
    if( criticalType[i] != CRITICAL_NONE )
    {
      if( fTargetPlural ){
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_CriticalHit, bpp[i]->GetID() );
      }
      else
      {
        if( criticalType[i] == CRITICAL_NORMAL ){
          SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_CriticalHit );
        }else {
          u8 atkPokeID = attacker->GetID();
          u8 clientID = MainModule::PokeIDtoClientID( atkPokeID );
          SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_FR_Critical, atkPokeID, clientID );
          SCQUE_PUT_ACT_FriendshipEffect( m_que, atkPokeID, FREFF_HEART );
        }
      }
    }

    scEvent_AfterCritical( wazaParam, attacker, bpp[i] );
  }
}

/**
 * @brief [Event] 急所メッセージ後
 * @param wazaParam
 * @param attacker
 * @param defender
 */
void ServerFlow::scEvent_AfterCritical( const WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
    EVENT_CallHandlers( this, BTL_EVENT_AFTER_CRITICAL );
  EVENTVAR_Pop();
}


//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ表示開始
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_TokWin_In( const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( m_que, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ消去開始
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_TokWin_Out( const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( m_que, bpp->GetID() );
}
//--------------------------------------------------------------------------
/**
 * [Put] これ以上、能力ランクがあがらない（さがらない）メッセージ
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_RankEffectLimit( const BTL_POKEPARAM* target, WazaRankEffect effect, int volume )
{
  u8 pokeID = target->GetID();

  if( volume > 0 ){
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_RankupMax_ATK, pokeID, effect );
  }else{
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_RankdownMin_ATK, pokeID, effect );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ランク増減効果が発生
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_RankEffect( BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, bool fStdMsg, bool fViewEffect )
{
  u8 pokeID = target->GetID();

  if( volume > 0 )
  {
    volume = target->RankUp( static_cast<BTL_POKEPARAM::ValueID>(effect), volume );
    SCQUE_PUT_OP_RankUp( m_que, pokeID, effect, volume );

    if( fViewEffect )
    {
      SCQUE_PUT_ACT_RankUp( m_que, pokeID, effect, volume );
    }

    if( fStdMsg )
    {
      if( itemID == ITEM_DUMMY_DATA ){
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Rankup_ATK, pokeID, effect, volume );
      }else{
        SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_UseItem_Rankup_ATK, pokeID, itemID, effect, volume );
      }
    }
  }
  else
  {
    volume *= -1;
    volume = target->RankDown( static_cast<BTL_POKEPARAM::ValueID>(effect), volume );
    SCQUE_PUT_OP_RankDown( m_que, pokeID, effect, volume );

    if( fViewEffect )
    {
      SCQUE_PUT_ACT_RankDown( m_que, pokeID, effect, volume );
    }

    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Rankdown_ATK, pokeID, effect, volume );
  }
}

//--------------------------------------------------------------------------
/**
 * [Put] シンプルHP増減エフェクト
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_SimpleHp( BTL_POKEPARAM* bpp, int value, DamageCause damageCause, u8 damageCausePokeID, bool fEffectEnable )
{
  u8 pokeID = bpp->GetID();

  if( value > 0 )
  {
    bpp->HpPlus( value );
    SCQUE_PUT_OP_HpPlus( m_que, pokeID, value );
  }
  else if( value < 0 )
  {
    value *= -1;
    scPut_DecreaseHP( bpp, value, false, damageCause, damageCausePokeID );
  }

  // 場にいないポケも回復させることがあるため
  if( fEffectEnable
  &&  (m_posPoke.GetPokeExistPos(pokeID) != BTL_POS_NULL)
  ){
    SCQUE_PUT_ACT_SimpleHP( m_que, pokeID );
  }
  else{
    BTL_Printf("エフェクト有効フラグ=%d, エフェクトを生成しません\n", fEffectEnable);
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ポケモン強制ひん死
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_KillPokemon( BTL_POKEPARAM* bpp, u8 atkPokeID, DamageCause deadCause, u8 effType )
{
  u8 pokeID = bpp->GetID();

  bpp->HpZero();
  SCQUE_PUT_OP_HpZero( m_que, pokeID );
  SCQUE_PUT_ACT_Kill( m_que, pokeID, effType );

  if( bpp->IsDead() )
  {
    bpp->SetDeadCause( deadCause, atkPokeID );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] 状態異常化
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_AddSick( BTL_POKEPARAM* target, WazaSick sick, const BTL_SICKCONT& sickCont )
{
  u8 pokeID = target->GetID();

//  TAYA_Printf("SickContRaw=%d, type=%d\n", sickCont.raw, sickCont.type);
  target->SetWazaSick( sick, sickCont );
  SCQUE_PUT_OP_SetSick( m_que, pokeID, sick, sickCont );

  // 状態異常アイコン付加
  if( sick < pml::pokepara::SICK_MAX ){
    if( (sick == pml::pokepara::SICK_DOKU) && SICKCONT_IsMoudokuCont(sickCont) ){
      sick = static_cast<WazaSick>(pml::pokepara::SICK_MAX);
    }
    SCQUE_PUT_ACT_SickIcon( m_que, pokeID, static_cast<PokeSick>(sick) );
  }
}

/**
 *  状態異常化の失敗原因を表示
 */
void ServerFlow::scPut_AddSickFail( const BTL_POKEPARAM* target, AddSickFailCode failCode, WazaSick sick, SickCause cause )
{
  u8 pokeID = target->GetID();

  switch( failCode ){
  case ADDSICK_FAIL_ALREADY:
    switch( sick ){
    case pml::wazadata::WAZASICK_DOKU:   SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_DokuAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_YAKEDO: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_YakedoAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_MAHI:   SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_MahiAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_NEMURI: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NemuriAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_KOORI:  SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KoriAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_KONRAN: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_KonranAlready, pokeID ); break;
    case pml::wazadata::WAZASICK_KAIHUKUHUUJI: SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_WazaFailPoke, pokeID ); break;
    case pml::wazadata::WAZASICK_HOROBINOUTA: // ほろびのうたは失敗メッセージをまとめて１回にする
      m_fWazaFailMsgRoundUp = true;
      return;
    default:
      SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
    }
    m_fWazaFailMsgDisped = true;
    return;

  case ADDSICK_FAIL_NO_EFFECT:
    if( cause == SICK_CAUSE_WAZA_EFFECT_SICK )
    {
      s16  specificStrID = Hnd_GetSpecificSickFailStrID( sick );
      if( specificStrID >= 0 )
      {
        SCQUE_PUT_MSG_SET( m_que, specificStrID, pokeID );
        m_fWazaFailMsgDisped = true;
        return;
      }
    }
    SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_NoEffect, pokeID );
    m_fWazaFailMsgDisped = true;
    return;

  default:
    SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
    m_fWazaFailMsgDisped = true;
    break;
  }
}

//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_Message_Set( const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = bpp->GetID();
  SCQUE_PUT_MSG_SET( m_que, strID, pokeID );
}

//--------------------------------------------------------------------------
/**
 * [Put] 標準メッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_Message_StdEx( u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_STD( m_que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_STD( m_que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_STD( m_que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_STD( m_que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_STD( m_que, strID, args[0], args[1], args[2], args[3] );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] 標準メッセージ表示（引数拡張＋SE再生）
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_Message_StdSE( u16 strID, u32 SENo, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_STD_SE( m_que, strID, SENo );
    break;
  case 1:
    SCQUE_PUT_MSG_STD_SE( m_que, strID, SENo, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_STD_SE( m_que, strID, SENo, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_STD_SE( m_que, strID, SENo, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_STD_SE( m_que, strID, SENo, args[0], args[1], args[2], args[3] );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_Message_SetEx( u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_SET( m_que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_SET( m_que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_SET( m_que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_SET( m_que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_SET( m_que, strID, args[0], args[1], args[2], args[3] );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示（引数拡張＋SE）
 */
//--------------------------------------------------------------------------
void ServerFlow::scPut_Message_SetSE( u16 strID, u32 SENo, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_SET_SE( m_que, strID, SENo );
    break;
  case 1:
    SCQUE_PUT_MSG_SET_SE( m_que, strID, SENo, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_SET_SE( m_que, strID, SENo, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_SET_SE( m_que, strID, SENo, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_SET_SE( m_que, strID, SENo, args[0], args[1], args[2], args[3] );
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] ポケモンHPを減らす
 *
 * @param   bpp                対象ポケモン
 * @param   value              減少量
 * @param   bWazaDmg           技ダメージによる減少ならtrueを指定する->複数体ヒット時、ワザエフェクトに同期させる対象となる。
 * @param   damageCause        ダメージの原因
 * @param   damageCausePokeID  ダメージの原因となったポケモンのID
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_DecreaseHP( BTL_POKEPARAM* bpp, u32 value, bool bWazaDamage, DamageCause damageCause, u8 damageCausePokeID )
{
  if( !m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_HP_CONST) )
  {
    bpp->HpMinus( value );

    if( bpp->IsDead() )
    {
      bpp->SetDeadCause( damageCause, damageCausePokeID );
    }

    if( !bWazaDamage ){
      SCQUE_PUT_OP_HpMinus( m_que, bpp->GetID(), value );
    }else{
      SCQUE_PUT_OP_HpMinusForSyncWazaEffect( m_que, bpp->GetID(), value );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] PP回復
 *
 * @param   wk
 * @param   bpp
 * @param   wazaIdx
 * @param   volume
 * @param   itemID    アイテムを使った効果の場合はアイテムID
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_RecoverPP( BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, bool fOrgWaza )
{
  u8 pokeID = bpp->GetID();
  BtlPokePos pos = m_posPoke.GetPokeExistPos(pokeID);
  if (pos != BTL_POS_NULL)
  {
    SCQUE_PUT_ACT_EffectByPos(m_que, pos, BTLEFF_PP_RECOVER);
  }
  if( fOrgWaza )
  {
    bpp->WAZA_IncrementPP_Org( wazaIdx, volume );
    SCQUE_PUT_OP_PPPlus_Org( m_que, pokeID, wazaIdx, volume );
  }
  else
  {
    bpp->WAZA_IncrementPP( wazaIdx, volume );
    SCQUE_PUT_OP_PPPlus( m_que, pokeID, wazaIdx, volume );
  }
}

//----------------------------------------------------------------------------------
/**
 * [Put] 道具使用アクション
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_UseItemAct( BTL_POKEPARAM* bpp )
{
  u16 itemID = bpp->GetItem();
  u8  bNuts = ::item::ITEM_CheckNuts( itemID );
  SCQUE_PUT_ACT_USE_ITEM( m_que, bpp->GetID(), bNuts );
}

//----------------------------------------------------------------------------------
/**
 * [Put] 継続フラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_SetContFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag flag )
{
  bpp->CONTFLAG_Set( flag );
  SCQUE_PUT_OP_SetContFlag( m_que, bpp->GetID(), flag );
}

//----------------------------------------------------------------------------------
/**
 * [Put] 永続フラグセット
 *
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_SetPermFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::PermFlag flag )
{
  bpp->PERMFLAG_Set( flag );
  SCQUE_PUT_OP_SetPermFlag( m_que, bpp->GetID(), flag );
}

//----------------------------------------------------------------------------------
/**
 * [Put] 継続フラグクリア
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_ResetContFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::ContFlag flag )
{
  bpp->CONTFLAG_Clear( flag );
  SCQUE_PUT_OP_ResetContFlag( m_que, bpp->GetID(), flag );
}

//----------------------------------------------------------------------------------
/**
 * [Put] ターンフラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_SetTurnFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::TurnFlag flag )
{
  bpp->TURNFLAG_Set( flag );
  SCQUE_PUT_OP_SetTurnFlag( m_que, bpp->GetID(), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] ターンフラグリセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_ResetTurnFlag( BTL_POKEPARAM* bpp, BTL_POKEPARAM::TurnFlag flag )
{
  bpp->TURNFLAG_ForceOff( flag );
  SCQUE_PUT_OP_ResetTurnFlag( m_que, bpp->GetID(), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] イリュージョン再設定
 *
 * @param   wk
 * @param   rec
 */
//----------------------------------------------------------------------------------
void ServerFlow::scPut_IllusionSet( const CLIENTID_REC& rec )
{
  BTL_PARTY* party;
  for(u32 i=0; i<rec.count; ++i)
  {
    party = m_pokeCon->GetPartyData( rec.clientID[i] );
    SCQUE_PUT_OP_SetIllusionForParty( m_que, rec.clientID[i] );
    m_mainModule->SetIllusionForParty(  party, rec.clientID[i] );
  }
}


/**
 * @brief サーバーコマンド出力
 *        サイドエフェクトを追加する
 *
 * @param      side        追加する陣営[ BTL_SIDE_1ST, BTL_SIDE_NUM )
 * @param      sideEffect  追加するサイドエフェクト
 * @param      contParam   追加するワイドエフェクトの継続パラメータ
 *
 * @retval true   サイドエフェクトを追加( 重ねがけ )した
 * @retval false  サイドエフェクトを追加( 重ねがけ )しなかった
 */
bool ServerFlow::scPut_SideEffect_Add( BtlSide side, BtlSideEffect sideEffect, const BTL_SICKCONT& contParam )
{
  // 操作対象は単一陣営でないとダメ
  if( !PokemonPosition::IsUnitSide( side ) )
  {
    GFL_ASSERT(0);
    return false;
  }

  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  if( sideEffectStatus->AddEffect( contParam ) ) 
  {
    handler::side::Add( side, sideEffect, contParam );
    SCQUE_PUT_OP_SideEffect_Add( m_que, side, sideEffect, contParam );
    return true;
  }
  return false;
}

/**
 * @brief サーバーコマンド出力
 *        サイドエフェクトを解除する
 *
 * @param      side        解除する陣営[ BTL_SIDE_1ST, BTL_SIDE_NUM )
 * @param      sideEffect  解除するサイドエフェクト
 *
 * @retval true   サイドエフェクトを解除した
 * @retval false  サイドエフェクトを解除しなかった( もともと存在しなかった )
 */
bool ServerFlow::scPut_SideEffect_Remove( BtlSide side, BtlSideEffect sideEffect )
{
  // 操作対象は単一陣営でないとダメ
  if( !PokemonPosition::IsUnitSide( side ) )
  {
    GFL_ASSERT(0);
    return false;
  }

  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  if( sideEffectStatus->RemoveEffect() ) 
  {
    handler::side::Remove( side, sideEffect );
    SCQUE_PUT_OP_SideEffect_Remove( m_que, side, sideEffect );
    return true;
  }
  return false;
}

/**
 * @brief サーバーコマンド出力
 *        サイドエフェクトの経過ターン数をインクリメントする
 *
 * @param side        操作対象の陣営
 * @param sideEffect  操作対象のサイドエフェクト
 */
void ServerFlow::scPut_SideEffect_IncTurnCount( BtlSide side, BtlSideEffect sideEffect )
{
  SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatus( side, sideEffect );
  sideEffectStatus->IncTurnCount();
  SCQUE_PUT_OP_SideEffect_IncTurnCount( m_que, side, sideEffect );
}


//---------------------------------------------------------------------------------------------
//
// 【Event】
//
// メイン処理層での判定に必要な各種条件の決定を行う。
// ハンドラ類の呼び出しはイベント層からのみ行われる。
// ポケモンパラメータの操作は行わない。
// コマンド出力は行わない。
//
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * [Event] メンバー入れ替え前に特定ポケモンのアクションを割り込ませるかチェック
 *
 * @param   wk
 * @param   outPoke
 *
 * @retval  u32        割り込みたいポケモン数
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_MemberChangeIntr( const BTL_POKEPARAM* outPoke )
{
  u32 i;

  // １度、未処理ポケモンの「おいうち」ワザハンドラを登録しておく
  for(i=0; i<m_numActOrder; ++i)
  {
    if( (m_actOrder[i].bpp != outPoke) && !(m_actOrder[i].fDone) )
    {
      WazaID waza = ActOrder_GetWazaID( &m_actOrder[i] );
      if( waza == WAZANO_OIUTI ){
        if( handler::waza::Add( m_actOrder[i].bpp, waza, m_actOrder[i].bpp->GetValue(BTL_POKEPARAM::BPP_AGILITY) ) ){
          m_actOrder[i].fIntrCheck = true;
        }
      }
    }
  }

  m_MemberOutIntrPokeCount = 0;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1, outPoke->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_MENBERCHANGE_INTR );
  EVENTVAR_Pop();

  // 未処理ポケモンの「おいうち」ワザハンドラを削除
  for(i=0; i<m_numActOrder; ++i)
  {
    if(m_actOrder[i].fIntrCheck)
    {
      WazaID waza = ActOrder_GetWazaID( &m_actOrder[i] );
      if( waza == WAZANO_OIUTI ){
        handler::waza::RemoveForce( m_actOrder[i].bpp, waza );
      }
      m_actOrder[i].fIntrCheck = false;
    }
  }

  return m_MemberOutIntrPokeCount;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 他のワザを呼び出すワザなら、呼び出すワザのパラメータをセットする
 *
 * @param   wk
 * @param   attacker
 * @param   orgWazaid
 * @param   orgTargetPos
 * @param   reqWaza         [io] 既にアクション処理順序が設定されている
 *
 * @retval  bool    他のワザを呼び出すワザ自体を出すのに失敗する場合false／それ以外true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_GetReqWazaParam( BTL_POKEPARAM* attacker,
  WazaID orgWazaId, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza )
{
  GFL_UNUSED( orgWazaId );

  bool failFlag = false;
  BTL_PRINT("[svflow] GetReqWazaParam  orgWazaID=%d, orgTargetPos=%d\n", orgWazaId, orgTargetPos);

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEPOS_ORG, orgTargetPos );
    EVENTVAR_SetValue( BTL_EVAR_WAZAID, WAZANO_NULL );
    EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );

    EVENT_CallHandlers( this, BTL_EVENT_REQWAZA_PARAM );

    reqWaza->wazaID = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_WAZAID ));
    reqWaza->targetPos = EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  EVENTVAR_Pop();

  return !failFlag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] “たたかう”を選んだときの特殊優先度を判定
 *
 * @param   wk
 * @param   attacker
 *
 */
//----------------------------------------------------------------------------------
u8  ServerFlow::scEvent_CheckSpecialActPriority( const BTL_POKEPARAM* attacker )
{
  u8 spPri = attacker->GetSpActPriority();

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_SP_PRIORITY, spPri );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_SP_PRIORITY );
    spPri = EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY );
  EVENTVAR_Pop();

  return spPri;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ターン最初のワザ処理シーケンス直前
 *
 * @param   wk
 * @param   bpp
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_BeforeFight( const BTL_POKEPARAM* bpp, WazaID waza )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    EVENT_CallHandlers( this, BTL_EVENT_BEFORE_FIGHT );
  EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] こんらんダメージ計算
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16   ダメージ値
 */
//--------------------------------------------------------------------------
u32  ServerFlow::scproc_CalcConfDamage( BTL_POKEPARAM* attacker )
{
  u32 atk, guard, level, ratio;
  u16 dmg;
  BTL_POKEPARAM::KoraeruCause  korae_cause;

  atk = attacker->GetValue(BTL_POKEPARAM::BPP_ATTACK );
  guard = attacker->GetValue( BTL_POKEPARAM::BPP_DEFENCE );
  level = attacker->GetValue( BTL_POKEPARAM::BPP_LEVEL );

  dmg = calc::DamageBase( 40, atk, level, guard );

  ratio = 100 - calc::GetRand( 16 );
  dmg = (dmg * ratio) / 100;
  if( dmg == 0 ){
    dmg = 1;
  }
  scEvent_FixConfDamage( attacker, &dmg );

  korae_cause = scEvent_CheckKoraeru( attacker, attacker, false, &dmg );
  scPut_ConfDamage( attacker, dmg );

  if( korae_cause != BTL_POKEPARAM::KORAE_NONE ){
    scproc_Koraeru( attacker, korae_cause );
  }

  scEvent_ConfDamageReaction( attacker, attacker );
  return dmg;
}

/**
 * [Event] こんらんダメージ補正イベント
 */
void ServerFlow::scEvent_FixConfDamage( const BTL_POKEPARAM* poke, u16* damage )
{
  u16 fixedDamage = *damage;
  bool fFixDamage = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, poke->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE, fixedDamage );
    EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE_FLAG, fFixDamage );
    EVENT_CallHandlers( this, BTL_EVENT_FIX_CONF_DAMAGE );
    fixedDamage = EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE );
    fFixDamage = EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE_FLAG );
  EVENTVAR_Pop();

  if( fFixDamage )
  {
    *damage = fixedDamage;
  }
}

/**
 * [Event] こんらんダメージ後のリアクション
 *
 * @param  attacker
 * @param  defender
 */
void ServerFlow::scEvent_ConfDamageReaction( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_CONF_DAMAGE_REACTION );
  EVENTVAR_Pop();
}

/**
 * @brief 技メッセージ出力
 *
 * @param attacker         技を出すポケモン
 * @param orgWazaID        呼び出し元の技（ex. ゆびをふる）
 * @param actWazaID        実行する技
 * @param actZenryokuKind  実行する技の全力技分類
 * @param targets          技ターゲット
 */
void ServerFlow::scproc_WazaMsg(
  const BTL_POKEPARAM* attacker, 
  WazaID               orgWazaID, 
  WazaID               actWazaID, 
  ZenryokuWazaKind     actZenryokuKind,
  const PokeSet*       targets )
{
  if( !scproc_CheckWazaMsgEnable( attacker, orgWazaID, actWazaID, actZenryokuKind, targets ) )
  {
    return;
  }

  if( scproc_CheckWazaMsgCustom( attacker, orgWazaID, actWazaID, m_wazaParam->zenryokuWazaKind, &m_strParam ) )
  {
    handexSub_putString( &m_strParam );
    HANDEX_STR_Clear( &m_strParam );
  }
  else
  {
    bool isZenryokuWaza = ( actZenryokuKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
    scPut_WazaMsg( attacker, actWazaID, isZenryokuWaza );
  }
}

/**
 * @brief 技メッセージを出力するか？
 *
 * @param attacker         技を出すポケモン
 * @param orgWazaID        呼び出し元の技（ex. ゆびをふる）
 * @param actWazaID        実行する技
 * @param actZenryokuKind  実行する技の全力技分類
 * @param targets          技ターゲット
 *
 * @retval true  技メッセージを出力する
 * @retval false 技メッセージを出力しない
 */
bool ServerFlow::scproc_CheckWazaMsgEnable( const BTL_POKEPARAM* attacker, WazaID orgWazaID, WazaID actWazaID, ZenryokuWazaKind actZenryokuKind, const PokeSet* targets ) const
{
  // 汎用・専用全力技は、技エフェクトの中で、
  // 技ごとに任意のタイミングで技名を表示するため、ここでは技メッセージを出力しない
  // ただし、技エフェクトをスキップする場合は技エフェクトが再生されないため、技メッセージを出力する
  if( m_mainModule->IsWazaEffectEnable() &&
     ( ( actZenryokuKind == pml::waza::ZENRYOKUWAZA_KIND_GENERAL ) || 
       ( actZenryokuKind == pml::waza::ZENRYOKUWAZA_KIND_SPECIAL ) ) )
  {
    // @fix NMCat[1482] 味方に対して攻撃技のZワザを使用する際、Zワザ名のMSGが表示されない
    // 味方対象エフェクトOFF && ターゲットが味方１体 なら、技エフェクトが再生されないため、技メッセージを出力する
    if( !WAZADATA_GetFlag( actWazaID, pml::wazadata::WAZAFLAG_EffectToFriend ) )
    {
      if( targets->GetCount() == 1 )
      {
        const BTL_POKEPARAM* target = targets->Get( 0 );

        if( target != NULL )
        {
          u8 attackerPokeID = attacker->GetID();
          u8 targetPokeID = target->GetID();

          if( ( attackerPokeID != targetPokeID ) &&
              ( m_mainModule->IsFriendPokeID( attackerPokeID, targetPokeID ) ) )
          {
            return true;
          }
        }
      }
    }

    return false;
  }

  return true;
}

/**
 * @brief 他ワザ呼び出し時など、独自のワザメッセージを出力するか判定
 *
 * @param      attacker        技を出すポケモン
 * @param      orgWazaID       呼び出し元の技（ex. ゆびをふる）
 * @param      actWazaID       実行する技
 * @param      actZenryokuKind     実行する技の全力技分類
 * @param[out] str             独自技メッセージパラメータを格納
 *
 * @retval true    独自技メッセージを出力する場合
 * @retval false   汎用技メッセージを出力する場合
 */
bool ServerFlow::scproc_CheckWazaMsgCustom(
  const BTL_POKEPARAM*   attacker, 
  WazaID                 orgWazaID, 
  WazaID                 actWazaID, 
  ZenryokuWazaKind       actZenryokuKind,
  BTL_HANDEX_STR_PARAMS* str )
{
  bool isActZenryokuWaza = ( actZenryokuKind != pml::waza::ZENRYOKUWAZA_KIND_NULL );
  if( scEvent_CheckWazaMsgCustom( attacker, orgWazaID, actWazaID, isActZenryokuWaza, str ) ) 
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 他ワザ呼び出し時など、独自のワザメッセージを出力するか判定
 *
 * @param      attacker            技を出すポケモン
 * @param      orgWazaID           呼び出し元の技（ex. ゆびをふる）
 * @param      actWazaID           実行する技
 * @param      isActZenryokuWaza   実行する技が全力技か？
 * @param[out] str                 独自技メッセージパラメータを格納
 *
 * @retval  bool    独自技メッセージを出力する場合はtrue
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scEvent_CheckWazaMsgCustom( 
  const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, 
  WazaID actWazaID, 
  bool isActZenryokuWaza,
  BTL_HANDEX_STR_PARAMS* str )
{
  GFL_UNUSED( orgWazaID );

  bool result;

  HANDEX_STR_Clear( str );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, actWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_FLAG, isActZenryokuWaza );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)str );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_REQWAZA_MSG );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return result;
}

//----------------------------------------------------------------------------------
/**
 * [Event] 全力派生技による技返還メッセージについて、独自のメッセージを出力するか判定
 *
 * @param      attacker           技を出すポケモン
 * @param      reqWazaID          派生技の技ID( ex.ゆびをふる )
 * @param      actWazaID          派生された技の技ID
 * @param      actZenryokuWazaID  派生された技の全力技
 * @param[out] str                独自メッセージパラメータを格納
 *
 * @retval true   独自のメッセージを出力する
 * @retval false  汎用のメッセージを出力する
 */
//----------------------------------------------------------------------------------
bool ServerFlow::scEvent_CheckZenryokuReqWazaMsgCustom( 
  const BTL_POKEPARAM* attacker,
  WazaID reqWazaID, 
  WazaID actWazaID, 
  WazaID actZenryokuWazaID, 
  BTL_HANDEX_STR_PARAMS* str )
{
  bool result;

  HANDEX_STR_Clear( str );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ORG_WAZAID, actWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZAID, actZenryokuWazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)str );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_REQWAZA_ZENRYOKU_MSG );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return result;
}

//--------------------------------------------------------------------------
/**
 * [Event] 発動するワザのパラメータを構造体にセット
 *
 * @param   waza
 * @param   attacker
 * @param   [out] param
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_GetWazaParam( WazaID waza, const BTL_POKEPARAM* attacker, WAZAPARAM* param )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WAZA_TYPE, WAZADATA_GetType(waza) );
    EVENTVAR_SetValue( BTL_EVAR_USER_TYPE, attacker->GetPokeType() );
    EVENTVAR_SetValue( BTL_EVAR_DAMAGE_TYPE, WAZADATA_GetDamageType(waza) );
    EVENTVAR_SetValue( BTL_EVAR_WAZA_TOUCH_FLAG, WAZADATA_GetFlag(waza, pml::wazadata::WAZAFLAG_Touch) );
    EVENTVAR_SetValue( BTL_EVAR_TARGET_TYPE, WAZADATA_GetParam(waza, pml::wazadata::PARAM_ID_TARGET) );
    EVENTVAR_SetValue( BTL_EVAR_ITAZURAGOKORO_EFFECT, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, false );
    BTL_SICKEVENT_CheckAttackType( attacker );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_PARAM );

    // ワザタイプのみもう１度変更の余地あり
    {
      EVENTVAR_RecoveryRewriteOncePermission( BTL_EVAR_WAZA_TYPE );
      EVENT_CallHandlers( this, BTL_EVENT_WAZA_PARAM_2ND );
    }

    param->wazaID           = waza;
    param->orgWazaID        = waza;
    param->wazaType         = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType         = EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
    param->damageType       = static_cast<WazaDamageType>(EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE ));
    param->touchFlag        = static_cast<bool>(EVENTVAR_GetValue( BTL_EVAR_WAZA_TOUCH_FLAG ));
    param->targetType       = static_cast<WazaTarget>(EVENTVAR_GetValue( BTL_EVAR_TARGET_TYPE ));
    param->wazaPri          = 0;
    param->flags            = 0;
    param->zenryokuWazaKind = pml::waza::ZENRYOKUWAZA_KIND_NULL;
    param->wazaPower        = WAZADATA_GetPower( waza );
    param->fItazuraGokoro   = EVENTVAR_GetValue( BTL_EVAR_ITAZURAGOKORO_EFFECT );
    if( EVENTVAR_GetValue(BTL_EVAR_FLAT_FLAG) ){
      param->wazaType = POKETYPE_NULL;
    }
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザだし失敗確定
 *
 * @param   bpp
 * @param   waza
 * @param   cause
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_CheckWazaExeFail( const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_FAIL_CAUSE, cause );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_EXECUTE_FAIL );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ出し成功確定
 *
 * @param   attacker
 * @param   waza
 *
 * @retval  WazaForceEnableMode  特殊条件により、強制的に有効と判断（エフェクト発動）するためのパターン指定子
 */
//----------------------------------------------------------------------------------
ServerFlow::WazaForceEnableMode ServerFlow::scEvent_WazaExecuteStart( const ActionDesc* actionDesc, const BTL_POKEPARAM* attacker, const WAZAPARAM* wazaParam, PokeSet* rec )
{
  u32 targetCnt = rec->GetCount();
  WazaForceEnableMode  enableMode;
  WazaID effectWazaID;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_YOKODORI_ROB_ACTION, actionDesc->isYokodoriRobAction );
    EVENTVAR_SetConstValue( BTL_EVAR_ACTION_DESC_IS_ODORIKO_REACTION, actionDesc->isOdorikoReaction );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      const BTL_POKEPARAM* bpp;
      for(u32 i=0; i<targetCnt; ++i){
        bpp = rec->Get( i );
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), bpp->GetID() );
      }
    }
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_ENABLE_MODE, WAZAENABLE_NONE );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_EFFECT_WAZAID, WAZANO_NULL );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_EXE_START );
    enableMode = static_cast<WazaForceEnableMode>(EVENTVAR_GetValue( BTL_EVAR_ENABLE_MODE ));
    effectWazaID = static_cast<WazaNo>(EVENTVAR_GetValue( BTL_EVAR_EFFECT_WAZAID ));
  EVENTVAR_Pop();


  if( effectWazaID != WAZANO_NULL ){
    wazaEffCtrl_ChangeEffectWazaID( m_wazaEffCtrl, effectWazaID );
  }

  return enableMode;
}
//----------------------------------------------------------------------------------
/**
 * [Event] まもる・みきりの無効化チェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  bool    無効化する場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckMamoruBreak( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  GFL_UNUSED( defender );
  GFL_UNUSED( waza );

  bool result = false;
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_MAMORU_BREAK );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン失敗判定
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  bool    失敗ならtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckTameFail( const BTL_POKEPARAM* attacker, const PokeSet* targetRec )
{
  bool fFail = false;
  const BTL_POKEPARAM* target = targetRec->Get( 0 );


  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, (target!=NULL)? target->GetID() : BTL_POKEID_NULL );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_TAMETURN_FAIL );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  return fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターンスキップ判定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  bool    溜めターンスキップする場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckTameTurnSkip( const BTL_POKEPARAM* attacker, WazaID waza )
{
  bool result = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_TAMETURN_SKIP );
    result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン発動チェック
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_TameStart( const BTL_POKEPARAM* attacker, const PokeSet* targetRec, const WAZAPARAM* wazaParam, u8* hideTargetPokeID, bool* fFailMsgDisped )
{
  const BTL_POKEPARAM* target;
  u32 targetCnt;
  bool fFail = false;

  targetCnt = targetRec->GetCount();

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    if( targetCnt )
    {
      for(u32 i=0; i<targetCnt; ++i)
      {
        target = targetRec->Get( i );
        EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), target->GetID() );
      }
    }
    else{
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1, BTL_POKEID_NULL );
    }
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_MSG_FLAG, false );
    EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (uint32_t)(wazaParam) );

    EVENT_CallHandlers( this, BTL_EVENT_TAME_START );

    if( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
      *hideTargetPokeID = EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    }else{
      *hideTargetPokeID = BTL_POKEID_NULL;
    }
    *fFailMsgDisped = EVENTVAR_GetValue( BTL_EVAR_MSG_FLAG );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  EVENTVAR_Pop();

  return !fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン発動確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_TameStartFixed( const BTL_POKEPARAM* attacker )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_TAME_START_FIXED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターンスキップ確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_TameSkip( const BTL_POKEPARAM* attacker, WazaID waza )
{
  GFL_UNUSED( waza );

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_TAME_SKIP );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン発動確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_TameRelease( const BTL_POKEPARAM* attacker, const PokeSet* rec, WazaID waza )
{
  GFL_UNUSED( waza );

  const BTL_POKEPARAM* target;
  u32 targetCnt;
  u8 fFail = false;

  targetCnt = rec->GetCount();

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    for(u32 i=0; i<targetCnt; ++i)
    {
      target = rec->Get( i );
      EVENTVAR_SetConstValue( static_cast<BtlEvVarLabel>(BTL_EVAR_POKEID_TARGET1+i), target->GetID() );
    }
    EVENT_CallHandlers( this, BTL_EVENT_TAME_RELEASE );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( !fFail ){
      EVENT_CallHandlers( this, BTL_EVENT_TAME_RELEASE_FIXED );
    }
  EVENTVAR_Pop();

  return (!fFail);
}
//----------------------------------------------------------------------------------
/**
 * [Event] そらをとぶ、ダイビング等、場から隠れているポケモンへのヒットチェック
 *
 * @param      attacker
 * @param      defender
 * @param      waza
 * @param[out] bEnableAvoidMsg   ヒットしなかった場合、ワザをハズした時の標準メッセージを出力するなら true
 *
 * @retval  bool    隠れているためヒットしない場合にtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckPokeHideAvoid(const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, bool* bEnableAvoidMsg )
{
  GFL_UNUSED( waza );

  bool avoidFlag = false;
  *bEnableAvoidMsg = true;

  BTL_POKEPARAM::ContFlag  hideState = defender->CONTFLAG_CheckWazaHide();
  if( hideState != BTL_POKEPARAM::CONTFLG_NULL )
  {
    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKE_HIDE, hideState );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AVOID_FLAG, true );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_MSG_FLAG, (*bEnableAvoidMsg) );

      EVENT_CallHandlers( this, BTL_EVENT_CHECK_POKE_HIDE );
      if( EVENTVAR_GetValue(BTL_EVAR_AVOID_FLAG) ){
        avoidFlag = true;
      }
      *bEnableAvoidMsg = EVENTVAR_GetValue( BTL_EVAR_MSG_FLAG );
    EVENTVAR_Pop();
  }

  return avoidFlag;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザの命中チェック時、確率計算をスキップして命中させるかチェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  bool        確率計算をスキップする場合はtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_IsCalcHitCancel( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  GFL_UNUSED( defender );

  bool result = pml::wazadata::IsAlwaysHit( waza );

  if( !result )
  {
    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
      EVENT_CallHandlers( this, BTL_EVENT_CALC_HIT_CANCEL );
      result = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    EVENTVAR_Pop();
  }
  return result;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ的中率(0-100)取得
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  u32    命中率 (じゅうりょく等、命中率上昇の効果を受けて100超過する場合もあり得る）
 */
//--------------------------------------------------------------------------
u32  ServerFlow::scEvent_getHitPer( BtlEventType eventType, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  u32 per = WAZADATA_GetParam( wazaParam->wazaID, pml::wazadata::PARAM_ID_HITPER );
  fx32 ratio = FX32_CONST(1);

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_HIT_PER, per );
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    EVENT_CallHandlers( this, eventType );
    per = EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
    ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
  EVENTVAR_Pop();

  per = calc::MulRatio( per, ratio );

  return per;
}
//--------------------------------------------------------------------------
/**
 * [Event] 出したワザが対象に当たるか判定（一撃必殺ワザ）
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_IchigekiCheck( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  u8 atkLevel = attacker->GetValue( BTL_POKEPARAM::BPP_LEVEL );
  u8 defLevel = defender->GetValue( BTL_POKEPARAM::BPP_LEVEL );

  // 攻撃側レベルが防御側レベル未満なら常に失敗
  if( atkLevel < defLevel )
  {
    return false;
  }

  {
    bool ret = false;
    bool fFail = false;

    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
      EVENT_CallHandlers( this, BTL_EVENT_ICHIGEKI_CHECK );
      fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    EVENTVAR_Pop();

    // とくせい等による強制無効化があれば失敗
    if( fFail ){
      return false;
    }

    if( IsMustHit(attacker, defender) ){
      ret = true;
    }else{
      u8 per = scEvent_getHitPer( BTL_EVENT_WAZA_HIT_RATIO_ICHIGEKI, attacker, defender, wazaParam );
      BTL_Printf("デフォ命中率=%d\n", per);
      per += (atkLevel - defLevel);
      ret = perOccur( per );
      BTL_Printf("レベル補正命中率=%d, hit=%d\n", per, ret);
    }
    return ret;
  }
}
//--------------------------------------------------------------------------
/**
 * [Event] 特定タイプのダメージワザで回復してしまうポケの判定
 *
 * @param   wk
 * @param   defender
 * @param   waza
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckDmgToRecover( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const WAZAPARAM* wazaParam )
{
  bool ret = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, ret );
    EVENT_CallHandlers( this, BTL_EVENT_DMG_TO_RECOVER_CHECK );
    ret = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();
  return ret;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ダメージワザ回復化決定
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_DmgToRecover( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_DMG_TO_RECOVER_FIX );
  EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] クリティカルヒット発生チェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckCritical( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, bool* bFriendshipAct )
{
  bool flag = false;

  u16  rank = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_CRITICAL_RANK );

  bool bMustCritical = ( rank == pml::wazadata::CRITICAL_MUST );
  if( !bMustCritical ){
    rank += attacker->GetCriticalRank();
  }

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_CRITICAL_RANK, rank );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_CRITICAL_CHECK );
    if( !EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG ) )
    {
      if( bMustCritical ){
        flag = true;
      }else{
        rank = roundMax( EVENTVAR_GetValue(BTL_EVAR_CRITICAL_RANK), BTL_CALC_CRITICAL_MAX );
        int  ratio = (getFriendship(attacker) < 255)? 1 : 2;

        flag = calc::CheckCritical( static_cast<u8>(rank), ratio );

        #if PM_DEBUG
        if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_MUST_CRITICAL) ){
          flag = true;
        }
        if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_DISABLE_CRITICAL) ){
          flag = false;
        }
        #endif

        if( flag )
        {
          *bFriendshipAct = (ratio == 2);
        }
      }


    }
    else{
      flag = false;
    }
  EVENTVAR_Pop();
  return flag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるダメージを与えた後の反動計算（ダメージ×反動率）
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   damage
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_CalcKickBack( const BTL_POKEPARAM* attacker, WazaID waza, u32 damage, bool* fMustEnable )
{
  u8 pokeID = attacker->GetID();
  u8 ratio = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_DAMAGE_REACTION_RATIO );
  u8 ratio_ex = 0;
  u8 fail_flag = false;
  u8 anti_fail_flag = false;
  ReactionType  reactionType = SV_REACTION_NONE;

  *fMustEnable = false;

  if( ratio )
  {
    reactionType = SV_REACTION_DAMAGE;
  }
  else
  {
    ratio = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_HP_REACTION_RATIO );
    if(  ratio ){
      reactionType = SV_REACTION_HP;
    }
  }

  //ドレイン系は反動にならないようにする
  if( WAZADATA_GetCategory(waza) == pml::wazadata::CATEGORY_DRAIN )
  {
    return 0;
  }

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, pokeID );
    EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    EVENTVAR_SetValue( BTL_EVAR_RATIO_EX, 0 );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );

    EVENT_CallHandlers( this, BTL_EVENT_CALC_KICKBACK );

    ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
    fail_flag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    anti_fail_flag = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    ratio_ex = EVENTVAR_GetValue( BTL_EVAR_RATIO_EX );
  EVENTVAR_Pop();

  if( !anti_fail_flag )
  {
    if( static_cast<bool>(fail_flag) == true ){
      return 0;
    }
  }
  ratio += ratio_ex;

  if( ratio )
  {
    TAYA_Printf("反動ratio=%08x\n", ratio);
    if( reactionType == SV_REACTION_HP )
    {
      u32 maxHP = attacker->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
      damage = roundMin( calc::MulRatioInt(maxHP, ratio), 1 );
      TAYA_Printf( "反動 HP type .. maxHP=%d, damage = %d\n", maxHP, damage );
    }
    else{
      TAYA_Printf( "反動 DMG type .. damage = %d\n", damage );

      if( damage ){
        damage = roundMin( calc::MulRatioInt(damage, ratio), 1 );
      }

    }

    (*fMustEnable) = anti_fail_flag;

    return damage;
  }
  return 0;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 装備アイテム使用
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ItemEquip( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_USE_ITEM );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 一時的に登録したアイテムの使用（ついばむなど）
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ItemEquipTmp( const BTL_POKEPARAM* bpp, u8 atkPokeID )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    EVENT_ForceCallHandlers( this, BTL_EVENT_USE_ITEM_TMP );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * ポケモン単体ふゆう状態のチェック
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scproc_CheckFloating( const BTL_POKEPARAM* bpp, bool fHikouCheck )
{
  // じゅうりょくが効いていたら誰も浮けないのでチェックしない
  if( !m_fieldStatus->CheckEffect(FieldStatus::EFF_JURYOKU) )
  {
    if( scEvent_CheckFloating(bpp, fHikouCheck) ){
      return true;
    }
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ふゆう状態チェック
 *
 * @param   wk
 * @param   bpp
 * @param   fHikouCheck   タイプ「ひこう」をふゆう要素としてチェックするか？
 *
 * @retval  bool    有効であればtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckFloating( const BTL_POKEPARAM* bpp, bool fHikouCheck )
{
  // ここでは"じゅうりょく"の有無をチェックしない。
  // （じゅうりょく発動直後に「じめんにおちた」テキスト表示のチェックに使いたいので）
  {
    u8 floatFlag = (fHikouCheck)? bpp->IsMatchType(POKETYPE_HIKOU) : false;
    u8 failFlag = false;

    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, floatFlag );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
      EVENT_CallHandlers( this, BTL_EVENT_CHECK_FLYING );
      BTL_SICKEVENT_CheckFlying( bpp );
      floatFlag = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
      failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    EVENTVAR_Pop();

    if( failFlag ){
      return false;
    }
    return floatFlag;
  }
}
//----------------------------------------------------------------------------------
/**
 * ふゆう状態で地面ワザ無効だった時のメッセージ表示
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_WazaNoEffectByFlying( const BTL_POKEPARAM* bpp )
{
  u32 hem_state = m_HEManager.PushState();
  scEvent_WazaNoEffectByFlying( bpp );

  if( scproc_HandEx_Result() != HandExResult_Enable ){
    scPut_WazaNoEffect( bpp );
  }

  m_HEManager.PopState( hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ふゆう状態による地面ワザ無効化反応
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaNoEffectByFlying( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_NOEFF_BY_FLYING );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ相性計算
 *
 * @param   attacker      攻撃側のポケモン
 * @param   defender      防御側のポケモン
 * @param   wazaType      使用する技のタイプ
 * @param   onlyAttacker  true なら、防御側の特性等を考慮しない
 *
 * @return  技タイプ x 防御ポケモン　の相性
 */
//----------------------------------------------------------------------------------
BtlTypeAff  ServerFlow::scEvent_CheckDamageAffinity( 
  const BTL_POKEPARAM& attacker, 
  const BTL_POKEPARAM& defender, 
  pml::PokeType        wazaType,
  bool                 onlyAttacker )
{
  // 無属性の攻撃( わるあがき )は、常に等倍でヒット
  if( wazaType == POKETYPE_NULL ) {
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }

  BtlTypeAff affinity;
  PokeTypePair defPokeTypePair = defender.GetPokeType();
  pml::PokeType  defPokeType = PokeTypePair_GetType1( defPokeTypePair );

  affinity = scEvent_CheckDamageAffinity( attacker, defender, wazaType, defPokeType, onlyAttacker );
  if( !PokeTypePair_IsPure(defPokeTypePair) )
  {
    pml::PokeType  type2 = PokeTypePair_GetType2( defPokeTypePair );
    if( (type2 != POKETYPE_NULL) && (type2 != defPokeType) ){
      BtlTypeAff    aff2 = scEvent_CheckDamageAffinity( attacker, defender, wazaType, type2, onlyAttacker );
      affinity = calc::TypeAffMul( affinity, aff2 );
    }

    pml::PokeType  typeEx = PokeTypePair_GetTypeEx( defPokeTypePair );
    if( (typeEx != POKETYPE_NULL) && (typeEx != defPokeType) && (typeEx != type2) ){
      BtlTypeAff    affEx = scEvent_CheckDamageAffinity( attacker, defender, wazaType, typeEx, onlyAttacker );
      affinity = calc::TypeAffMul( affinity, affEx );
    }
  }

  return affinity;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ相性計算( 単タイプのみ考慮 )
 *
 * @param   attacker       攻撃側のポケモン
 * @param   defender       防御側のポケモン
 * @param   waza_type      技のタイプ
 * @param   defenderType   相性計算する防御側ポケモンのタイプ
 * @param   onlyAttacker   true なら、防御側の特性等を考慮しない
 *
 * @return  技タイプ x 防御ポケモンのタイプ　の相性
 */
//----------------------------------------------------------------------------------
BtlTypeAff  ServerFlow::scEvent_CheckDamageAffinity( 
  const BTL_POKEPARAM& attacker, 
  const BTL_POKEPARAM& defender, 
  pml::PokeType        waza_type, 
  pml::PokeType        defenderType,
  bool                 onlyAttacker )
{
  const BtlEventType checkEvent = ( onlyAttacker ) ? ( BTL_EVENT_CHECK_AFFINITY_ONLY_ATTACKER ) : ( BTL_EVENT_CHECK_AFFINITY );

  bool  flatFlag, flatMasterFlag;
  pml::PokeType   waza_typeEx;
  BtlTypeAff      force_affinity;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker.GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender.GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKE_TYPE, defenderType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, waza_type );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLATMASTER_FLAG, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_TYPEAFF, pml::battle::TypeAffinity::TYPEAFF_NULL );

    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WAZA_TYPE_EX, POKETYPE_NULL );

    BTL_SICKEVENT_CheckNotEffectByType( &defender );
    EVENT_CallHandlers( this, checkEvent );

    flatFlag       = EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
    flatMasterFlag = EVENTVAR_GetValue( BTL_EVAR_FLATMASTER_FLAG );
    waza_typeEx    = EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE_EX );
    force_affinity = (BtlTypeAff)(EVENTVAR_GetValue( BTL_EVAR_TYPEAFF ));
  EVENTVAR_Pop();

  if( flatMasterFlag ){
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }

  BtlTypeAff      affinity;
  if( force_affinity != pml::battle::TypeAffinity::TYPEAFF_NULL ){
    affinity = force_affinity;
  }
  else{
    affinity = calc::TypeAff( waza_type, defenderType );
  }

  if( waza_typeEx != POKETYPE_NULL )
  {
    BtlTypeAff  affEx = calc::TypeAff( waza_typeEx, defenderType );
    affinity = calc::TypeAffMul(  affinity, affEx );
  }

  if( (affinity == pml::battle::TypeAffinity::TYPEAFF_0) && (flatFlag) ){
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }

  return affinity;
}
//----------------------------------------------------------------------------------
/**
 * エフェクト生成
 *
 * @param   wk
 * @param   effectNo    エフェクトナンバー
 * @param   pos_from    エフェクト開始位置（不要ならBTL_POS_NULL)
 * @param   pos_to      エフェクト終了位置（不要ならBTL_POS_NULL)
 * @param   fQueResereved   予約済みQue領域を利用するフラグ
 * @param   reservedPos     予約済みQue位置
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scproc_ViewEffect( u16 effectNo, BtlPokePos pos_from, BtlPokePos pos_to, bool fQueResereved, u32 reservedPos )
{
  enum {
    EFF_SIMPLE = 0, // 位置指定なし
    EFF_POS,        // 開始位置のみ指定
    EFF_VECTOR,     // 開始・終端位置ともに指定
  };

  int effType;

  if( pos_to != BTL_POS_NULL ){
    effType = EFF_VECTOR;
  }else if( pos_from != BTL_POS_NULL ) {
    effType = EFF_POS;
  }else{
    effType = EFF_SIMPLE;
  }


  // 予約領域へ書き込み
  if( fQueResereved )
  {
    switch( effType ){
    case EFF_SIMPLE:
      scmd::SCQUE_PUT_ReservedPos( m_que, reservedPos, SC_ACT_EFFECT_SIMPLE, effectNo );
      break;

    case EFF_POS:
      scmd::SCQUE_PUT_ReservedPos( m_que, reservedPos, SC_ACT_EFFECT_BYPOS,
          pos_from, effectNo );
      break;

    case EFF_VECTOR:
      scmd::SCQUE_PUT_ReservedPos( m_que, reservedPos, SC_ACT_EFFECT_BYVECTOR,
          pos_from, pos_to, effectNo );
      break;
    }
  }
  // 最後尾に追記
  else
  {
    switch( effType ){
    case EFF_SIMPLE:
      SCQUE_PUT_ACT_EffectSimple( m_que, effectNo );
      break;

    case EFF_POS:
      SCQUE_PUT_ACT_EffectByPos( m_que, pos_from, effectNo );
      break;

    case EFF_VECTOR:
      SCQUE_PUT_ACT_EffectByVector( m_que, pos_from, pos_to, effectNo );
      break;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ使用後、PPをデクリメントした後の反応
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  bool    アイテム使用反応があればtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_DecrementPP_Reaction( const BTL_POKEPARAM* attacker, u8 wazaIdx )
{
  bool flag = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID,   attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_IDX, wazaIdx );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, flag );
    EVENT_CallHandlers( this, BTL_EVENT_DECREMENT_PP_DONE );
    flag = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return flag;
}
//--------------------------------------------------------------------------
/**
 * [Event] 複数回ヒットワザのチェック処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   param            [out] ヒットチェックパラメータ
 *
 */
//--------------------------------------------------------------------------
void  ServerFlow::scEvent_HitCheckParam(const BTL_POKEPARAM* attacker, WazaID waza, ZenryokuWazaKind zenryokuWazaKind, HITCHECK_PARAM* param , bool fDelayAttack)
{
  u8 max = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_HITCOUNT_MAX );
  u8 hit_cnt;

  if( max > 1 ){
    hit_cnt = calc::HitCountStd( max );
  }else{
    max = 1;
    hit_cnt = 1;
  }

  param->count = 0;
  param->pluralHitAffinity = pml::battle::TypeAffinity::TYPEAFF_1;


  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_HITCOUNT_MAX, max );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetConstValue( BTL_EVAR_ZENRYOKU_WAZA_KIND, zenryokuWazaKind );
    EVENTVAR_SetConstValue( BTL_EVAR_DELAY_ATTACK_FLAG, fDelayAttack );
    EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, hit_cnt );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );   // 最大までヒットさせるフラグ
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AVOID_FLAG, false ); // １回ヒットごとに命中判定を行うフラグ

    EVENT_CallHandlers( this, BTL_EVENT_WAZA_HIT_COUNT );

    hit_cnt = EVENTVAR_GetValue( BTL_EVAR_HITCOUNT );

    // もともと複数回ヒットのワザ
    if( max > 1 )
    {
      // 強制的に最大回数までヒット
      if( (max <= 5)
      &&  ( EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
      ){
        param->countMax = max;
        param->fCheckEveryTime = false;
      }
      else
      {
        param->countMax = hit_cnt;
        param->fCheckEveryTime = EVENTVAR_GetValue( BTL_EVAR_AVOID_FLAG );
      }
      param->fPluralHitWaza = true;
    }
    // もともと単発ヒットワザだが、何かの効果で複数回ヒットあつかいにする
    else if( hit_cnt > 1 )
    {
      param->countMax = hit_cnt;
      param->fCheckEveryTime = EVENTVAR_GetValue( BTL_EVAR_AVOID_FLAG );
      param->fPluralHitWaza = false;
      BTL_PRINT("複数ヒットチェック : PTN_B ... %d回ヒット\n", hit_cnt);
    }
    // 通常の単体ヒットワザとして扱う
    else
    {
      param->countMax = 1;
      param->fCheckEveryTime = false;
      param->fPluralHitWaza = false;
    }

  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ威力取得
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   wazaParam
 *
 * @retval  u16   ワザ威力値
 */
//----------------------------------------------------------------------------------
u16  ServerFlow::scEvent_getWazaPower(
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam )
{
  u16 power = wazaParam->wazaPower;

  fx32 ratio;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TOUCH_FLAG, wazaParam->touchFlag );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AURA_BREAK_FLAG,  false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AURA_RESERVE_FLAG,  false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WAZA_POWER, power );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_POWER_BASE );

    EVENTVAR_SetMulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.0), FX32_CONST(0.01f), FX32_CONST(512) );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_POWER );
    power = EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    ratio = (fx32)EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER_RATIO );
  EVENTVAR_Pop();

  power = calc::MulRatio( power, ratio );
  BTL_N_Printf( DBGSTR_SVFL_WazaPower, wazaParam->wazaID, power, ratio);
  return power;
}

//--------------------------------------------------------------------------
/**
 * [Event] ワザ以外のダメージ有効／無効チェック
 *
 * @param   wk
 * @param   bpp
 * @param   damage       ダメージ量
 * @param   damageCause  ダメージの原因
 *
 * @retval  bool    有効ならtrue
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckEnableSimpleDamage( const BTL_POKEPARAM* bpp, u32 damage, DamageCause damageCause )
{
  GFL_UNUSED(damage);

  bool flag;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_CAUSE, damageCause );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, true );
    EVENT_CallHandlers( this, BTL_EVENT_SIMPLE_DAMAGE_ENABLE );
    flag = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();

  return flag;
}

// 攻撃側能力値取得
u16  ServerFlow::scEvent_getAttackPower( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const WAZAPARAM* wazaParam, bool criticalFlag, bool* bYakedoDisable )
{
  BTL_POKEPARAM::ValueID vid = (wazaParam->damageType == pml::wazadata::DMG_SPECIAL)?
                          BTL_POKEPARAM::BPP_SP_ATTACK : BTL_POKEPARAM::BPP_ATTACK;

  {
    const BTL_POKEPARAM* calc_attacker = attacker;
    fx32 ratio;
    u16 power;

    EVENTVAR_Push();
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
      EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
      EVENTVAR_SetValue( BTL_EVAR_SWAP_POKEID, BTL_POKEID_NULL );
      EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, false );    //

      EVENT_CallHandlers( this, BTL_EVENT_ATTACKER_POWER_PREV );

      {
        u8 swapPokeID = EVENTVAR_GetValue( BTL_EVAR_SWAP_POKEID );
        if( swapPokeID != BTL_POKEID_NULL ){
          calc_attacker = m_pokeCon->GetPokeParam( swapPokeID );
        }
      }

      // クリティカルチェック
      if( EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG ) ){
        power = calc_attacker->GetValue_Base( vid );
      }
      else
      {
        if( criticalFlag ){
          power = calc_attacker->GetValue_Critical( vid );
        }else{
          power = calc_attacker->GetValue( vid );
        }
      }

      EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
      EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
      EVENTVAR_SetValue( BTL_EVAR_POWER, power );
      EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
      EVENTVAR_SetRewriteOnceValue( BTL_EVAR_DISABLE_YAKEDO_FLAG, false );    // やけど無効化フラグ
      EVENT_CallHandlers( this, BTL_EVENT_ATTACKER_POWER );
      power = EVENTVAR_GetValue( BTL_EVAR_POWER );
      ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
      power = calc::MulRatio( power, ratio );
      *bYakedoDisable = EVENTVAR_GetValue( BTL_EVAR_DISABLE_YAKEDO_FLAG );

      BTL_N_Printf( DBGSTR_SVFL_AtkPower, power, criticalFlag, ratio );
    EVENTVAR_Pop();
    return power;
  }
}

// 防御側能力値取得
u16  ServerFlow::scEvent_getDefenderGuard(
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const WAZAPARAM* wazaParam, bool criticalFlag )
{
  BTL_POKEPARAM::ValueID vid = (wazaParam->damageType == pml::wazadata::DMG_SPECIAL)?
              BTL_POKEPARAM::BPP_SP_DEFENCE : BTL_POKEPARAM::BPP_DEFENCE;
  WazaDamageType dmgType = wazaParam->damageType;
  fx32 ratio = FX32_CONST(1);
  u16 guard;
  u8 forceFlatFlag;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_VID, vid );
    EVENTVAR_SetValue( BTL_EVAR_VID_SWAP_CNT, 0 );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_DEFENDER_GUARD_PREV );

    // 特殊要因によって防御・特防をスワップして評価する
    if( EVENTVAR_GetValue(BTL_EVAR_VID_SWAP_CNT) & 1 )
    {
      vid = (vid == BTL_POKEPARAM::BPP_DEFENCE)? BTL_POKEPARAM::BPP_SP_DEFENCE : BTL_POKEPARAM::BPP_DEFENCE;
      dmgType = (vid == BTL_POKEPARAM::BPP_DEFENCE)? pml::wazadata::DMG_PHYSIC : pml::wazadata::DMG_SPECIAL;
    }
    forceFlatFlag = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  EVENTVAR_Pop();


  if( forceFlatFlag ){
    guard = defender->GetValue_Base( vid );
  }else{
    if( criticalFlag ){
      guard = defender->GetValue_Critical( vid );
    }else{
      guard = defender->GetValue( vid );
    }
  }

  // てんこう「すなあらし」の時、いわタイプのとくぼう1.5倍
  if( (scEvent_GetWeather() == BTL_WEATHER_SAND)
  &&  (defender->IsMatchType(POKETYPE_IWA))
  &&  (vid == BTL_POKEPARAM::BPP_SP_DEFENCE)
  ){
    guard = calc::MulRatio( guard, FX32_CONST(1.5) );
  }

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defender->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, dmgType );
    EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, ratio, FX32_CONST(0.1), FX32_CONST(32) );
    EVENT_CallHandlers( this, BTL_EVENT_DEFENDER_GUARD );
    guard = EVENTVAR_GetValue( BTL_EVAR_GUARD );
    ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
  EVENTVAR_Pop();

  guard = calc::MulRatio( guard, ratio );
  BTL_N_Printf( DBGSTR_SVFL_DefGuard, guard, criticalFlag, ratio);
  return guard;
}
//--------------------------------------------------------------------------
/**
 * [Event] 攻撃ポケとワザのタイプ一致補正率を計算
 *
 * @param   wk
 * @param   attacker
 * @param   waza_type
 *
 * @retval  fx32    補正率
 */
//--------------------------------------------------------------------------
fx32  ServerFlow::scEvent_CalcTypeMatchRatio( const BTL_POKEPARAM* attacker, pml::PokeType waza_type )
{
  bool f_match = attacker->IsMatchType( waza_type );
  fx32 ratio = FX32_CONST(1);

  EVENTVAR_Push();
  {
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, attacker->GetID() );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, f_match );
    EVENT_CallHandlers( this, BTL_EVENT_TYPEMATCH_CHECK );
    f_match = EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );

    if( f_match ){
      ratio = FX32_CONST(1.5f);
    }

    EVENTVAR_SetConstValue( BTL_EVAR_TYPEMATCH_FLAG, f_match );
    EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    EVENT_CallHandlers( this, BTL_EVENT_TYPEMATCH_RATIO );

    ratio = (fx32)EVENTVAR_GetValue( BTL_EVAR_RATIO );
  }
  EVENTVAR_Pop();
  return ratio;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ追加効果によるひるみ確率取得
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_GetWazaShrinkPer( WazaID waza, const BTL_POKEPARAM* attacker )
{
  bool fFail, fCritical;
  u32 defaultPer = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_SHRINK_PER );
  u32 addPer = 0;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ADD_DEFAULT_PER, defaultPer );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_CRITICAL_FLAG, false );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENTVAR_SetValue( BTL_EVAR_ADD_PER, 0 );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_SHRINK_PER );

    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    fCritical = EVENTVAR_GetValue( BTL_EVAR_CRITICAL_FLAG );
    addPer = EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  EVENTVAR_Pop();

  if( fFail && (defaultPer != 0) ){
    return 0;
  }

  {
    u32 per = defaultPer;
    if( defaultPer == 0 ){
      per += addPer;
    }
    if( fCritical ){
      per *= 2;
    }
    return per;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ひるみ発生判定
 *
 * @param   wk
 * @param   target
 * @param   waza    ワザ追加効果による場合ワザID（それ以外WAZANO_NULL）
 * @param   per     ひるみ確率
 *
 * @retval  bool    ひるむならtrue
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckShrink( const BTL_POKEPARAM* target, u32 per )
{
  bool fail_flag = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fail_flag );
    EVENT_CallHandlers( this, BTL_EVENT_SHRINK_CHECK );

    per = EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fail_flag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  if( fail_flag ){
    return false;
  }
  else if( per > 0 )
  {
    if( m_mainModule->GetDebugFlag(BTL_DEBUGFLAG_MUST_TUIKA) ){
      return true;
    }

    return perOccur( per );
  }
  return false;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ひるみ失敗原因表示
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_FailShrink( const BTL_POKEPARAM* target )
{
  EVENTVAR_Push();
    EVENTVAR_SetValue( BTL_EVAR_POKEID, target->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_SHRINK_FAIL );
  EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の最終成否チェック
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   cause               ランク増減効果の原因
 * @param   wazaRankEffSerial   ワザによる効果の場合、そのワザ１発ごとのシリアルナンバー(1〜）
 *
 * @retval  bool      成功する場合true
 */
//--------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckRankEffectSuccess( const BTL_POKEPARAM* target,
  WazaRankEffect effect, u8 wazaUsePokeID, int volume, RankEffectCause cause, u32 wazaRankEffSerial )
{
  bool failFlag = false;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effect );
    EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    EVENTVAR_SetConstValue( BTL_EVAR_RANKEFFECT_CAUSE, cause );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_SERIAL, wazaRankEffSerial );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    EVENT_CallHandlers( this, BTL_EVENT_RANKEFF_LAST_CHECK );
    failFlag = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  return !failFlag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の失敗確定
 *
 * @param   wk
 * @param   bpp
 * @param   wazaRankEffSerial   ワザによる効果の場合、そのワザ１発ごとのシリアルナンバー(1〜）
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_RankEffect_Failed( const BTL_POKEPARAM* bpp, u32 wazaRankEffSerial )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZA_SERIAL, wazaRankEffSerial );
    EVENT_CallHandlers( this, BTL_EVENT_RANKEFF_FAILED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の成功確定
 *
 * @param   wk
 * @param   atkPokeID   効果を発生させたワザ・とくせいの主体ポケID（居ない場合BTL_POKEID_NULL）
 * @param   bpp
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_RankEffect_Fix( u8 atkPokeID, const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    EVENT_CallHandlers( this, BTL_EVENT_RANKEFF_FIXED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるランク増減効果の成功確定
 *
 * @param   wk
 * @param   target
 * @param   wazaID
 * @param   effectID
 * @param   volume
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_WazaRankEffectFixed( const BTL_POKEPARAM* target,
  WazaID wazaID, WazaRankEffect effectID, int volume )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaID );
    EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effectID );
    EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    EVENT_CallHandlers( this, BTL_EVENT_WAZA_RANKEFF_FIXED );
  EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] ドレイン攻撃のドレインHP量を再計算
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   volume     デフォルト回復量
 *
 * @retval  int     回復量
 */
//--------------------------------------------------------------------------
u16  ServerFlow::scEvent_RecalcDrainVolume( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u16 volume )
{
  fx32 ratio;
  u8   targetPokeID = (target!=NULL)? target->GetID() : BTL_POKEID_NULL;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, targetPokeID );
    EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );

    EVENT_CallHandlers( this, BTL_EVENT_CALC_DRAIN );
    volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
    if( volume != 0 ){
      volume = calc::MulRatio_OverZero( volume, ratio );
    }

    EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, volume );
    EVENT_CallHandlers( this, BTL_EVENT_CALC_DRAIN_END );
    volume = EVENTVAR_GetValue( BTL_EVAR_VOLUME );

  EVENTVAR_Pop();

  //-------

  return volume;
}

//----------------------------------------------------------------------------------
/**
 * 【Event】天候の変化後イベント
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_AfterChangeWeather( BtlWeather weather )
{
  bool fDisable = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, false );
    EVENT_CallHandlers( this, BTL_EVENT_WEATHER_CHANGE );
    fDisable = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( !fDisable ){
      EVENT_CallHandlers( this, BTL_EVENT_WEATHER_CHANGE_AFTER );
    }
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるHP回復量計算
 *
 * @param   attacker  ワザ使用ポケ
 * @param   target    回復対象ポケ
 * @param   waza      ワザナンバー
 *
 * @retval  u32       HP回復量
 */
//----------------------------------------------------------------------------------
u32  ServerFlow::scEvent_CalcWazaRecoverHP( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  u32 base_ratio = WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_HP_RECOVER_RATIO );
  fx32 ex_ratio;

  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, attacker->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, target->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    EVENTVAR_SetValue( BTL_EVAR_RATIO, 0 );
    EVENT_CallHandlers( this, BTL_EVENT_RECOVER_HP_RATIO );
    ex_ratio = EVENTVAR_GetValue( BTL_EVAR_RATIO );
  EVENTVAR_Pop();

  {
    u32 maxHP = target->GetValue( BTL_POKEPARAM::BPP_MAX_HP );
    u32 volume;

//    TAYA_Printf("maxHP=%d, ex_ratio=%08x, baes_ratio=%08x\n", maxHP, ex_ratio, base_ratio );

    if( ex_ratio ){
      volume = calc::MulRatio( maxHP, ex_ratio );
    }else{
      volume = calc::MulRatioInt( maxHP, base_ratio );
    }
    if( volume == 0 ){
      volume = 1;
    }else if( volume > maxHP ){
      volume = maxHP;
    }

    return volume;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] アイテムをセット（あるいは消去）されるチェック
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool    失敗する場合true
 */
//----------------------------------------------------------------------------------
bool  ServerFlow::scEvent_CheckItemSet( const BTL_POKEPARAM* bpp, u16 itemID )
{
  bool failed = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failed );
    EVENT_CallHandlers( this, BTL_EVENT_ITEMSET_CHECK );
    failed = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();

  return failed;
}
//----------------------------------------------------------------------------------
/**
 * [Event] アイテムをセット（あるいは消去）されるのを失敗
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ItemSetFailed( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_ITEMSET_FAILED );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] アイテム書き換え決定
 *
 * @param   wk
 * @param   bpp
 * @param   nextItemID   書き換え後のアイテムナンバー
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ItemSetDecide( const BTL_POKEPARAM* bpp, u16 nextItemID )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENTVAR_SetConstValue( BTL_EVAR_ITEM, nextItemID );
    EVENT_CallHandlers( this, BTL_EVENT_ITEMSET_DECIDE );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] アイテム書き換え完了
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ItemSetFixed( const BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_ITEMSET_FIXED );
  EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * [Event] とくせい変更が確定（変更の直前）
 *
 * @param   wk
 * @param   pokeID
 * @param   tokuseiID
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ChangeTokuseiBefore( u8 pokeID, TokuseiNo prev_tokuseiID, u16 next_tokuseiID )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_PREV, prev_tokuseiID );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_NEXT, next_tokuseiID );
    EVENT_CallHandlers( this, BTL_EVENT_CHANGE_TOKUSEI_BEFORE );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] とくせいが変更された
 *
 * @param   wk
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_ChangeTokuseiAfter( u8 pokeID )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    EVENT_CallHandlers( this, BTL_EVENT_CHANGE_TOKUSEI_AFTER );
  EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] サイドエフェクト継続パラメータ調整
 *
 * @param   wk
 * @param   param->effect
 * @param   param->side
 * @param   &param->cont
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_CheckSideEffectParam( u8 userPokeID, BtlSideEffect effect, BtlSide side, BTL_SICKCONT* cont )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, userPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_SIDE, side );
    EVENTVAR_SetConstValue( BTL_EVAR_SIDE_EFFECT, effect );
    EVENTVAR_SetValue( BTL_EVAR_SICK_CONT_ADRS, reinterpret_cast<int>( cont ) );
    EVENT_CallHandlers( this, BTL_EVENT_CHECK_SIDEEFF_PARAM );
  EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] エアロック開始通知
 *
 * @param   wk
 * @param   param->effect
 * @param   param->side
 * @param   &param->cont
 */
//----------------------------------------------------------------------------------
void  ServerFlow::scEvent_NotifyAirLock( void )
{
  EVENTVAR_Push();
    EVENT_CallHandlers( this, BTL_EVENT_NOTIFY_AIRLOCK );
  EVENTVAR_Pop();
}

//--------------------------------------------------------------------------------------
/**
 * 指定されたクライアントのポケモンが、バトル中に何回「ひん死」になったか取得
 * @param clientID
 * @return
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::GetDeadPokeCountByClientID( u8 clientID ) const
{
  return m_deadRec.GetDeadCountByClientID( clientID );
}

//--------------------------------------------------------------------------------------
/**
 * @brief 乱入するポケモンの立ち位置を取得する
 */
//--------------------------------------------------------------------------------------
BtlIntrudeType ServerFlow::GetIntrudeType( void ) const
{
  return m_intrudeParam.intrudeType;
}

//--------------------------------------------------------------------------------------
/**
 * @brief 乱入するポケモンの立ち位置を取得する
 */
//--------------------------------------------------------------------------------------
BtlPokePos ServerFlow::GetIntrudePokePos( void ) const
{
  return m_intrudeParam.appearPokePos;
}

//--------------------------------------------------------------------------------------
/**
 * @brief 乱入するポケモンのパラメータを取得する
 */
//--------------------------------------------------------------------------------------
const pml::pokepara::PokemonParam& ServerFlow::GetIntrudePokeParam( void ) const
{
  return *( m_intrudeParam.appearPokeParam );
}

//----------------------------------------------------------------------------------------------------------
// 以下、ハンドラからの応答受信関数とユーティリティ群
//----------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] MainModule を取得する
 */
//--------------------------------------------------------------------------------------
const MainModule* ServerFlow::Hnd_GetMainModule( void ) const
{
  return m_mainModule;
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール]指定IDのポケモンパラメータを返す
 *
 * @param   pokeID
 *
 * @retval  const BTL_POKEPARAM*
 */
//--------------------------------------------------------------------------------------
const BTL_POKEPARAM* ServerFlow::Hnd_GetPokeParam( u8 pokeID )
{
  return m_pokeCon->GetPokeParam( pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定ポケIDを持つポケモンが戦闘に出ているかチェックし、出ていたらその戦闘位置を返す
 *                    （ローテーションの場合は前衛のみ）
 *
 * @param   server
 * @param   pokeID
 *
 * @retval  BtlPokePos    出ている場合は戦闘位置ID／出ていない場合は BTL_POS_NULL
 */
//--------------------------------------------------------------------------------------
BtlPokePos ServerFlow::Hnd_GetExistFrontPokePos( u8 pokeID )
{
  BtlPokePos pos = m_posPoke.GetPokeExistPos( pokeID );
  BTL_PRINT("　ポケ[%d] がいる位置 = %d\n", pokeID, pos );
  if( m_mainModule->IsFrontPos(pos) ){
    return pos;
  }
  return BTL_POS_NULL;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定ポケIDを持つポケモンが最後に出ているた位置IDを返す
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos ServerFlow::Hnd_GetPokeLastPos( u8 pokeID )
{
  return m_posPoke.GetPokeLastPos( pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定位置に存在しているポケIDを返す（存在していない場合は BTL_POKEID_NULL）
 *
 * @param   wk
 * @param   pos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetExistPokeID( BtlPokePos pos )
{
  return m_posPoke.GetExistPokeID( pos );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 場に出ている全ての相手側ポケモンIDを配列に格納する
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    場に出ているポケモン数
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetAllOpponentFrontPokeID( u8 basePokeID, u8* dst )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 cnt = 0;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext( &fps, &bpp ) )
  {
    if( !m_mainModule->IsFriendPokeID( basePokeID, bpp->GetID() ) )
    {
      dst[ cnt++ ] = bpp->GetID();
    }
  }
  return cnt;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定のとくせいを持つポケモンが戦闘に出ているかチェック
 *
 * @param   wk
 * @param   tokusei
 *
 * @retval  bool    出ていたらtrue
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckExistTokuseiPokemon( TokuseiNo tokusei )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext( &fps, &bpp ) )
  {
    if( bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == tokusei )
    {
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------------------
/**
 * @brief [ハンドラ用ツール] 相性計算シミュレート結果を返す
 *
 * @param   atkPokeID     攻撃側ポケモンのID
 * @param   defPokeID     防御側ポケモンのID
 * @param   waza          使用する技
 *
 * @return  技の相性
 */
//--------------------------------------------------------------------------------------
BtlTypeAff ServerFlow::Hnd_SimulationAffinity( u8 atkPokeID, u8 defPokeID, WazaID waza )
{
  static const bool ONLY_ATTACKER = false;
  return this->Hnd_SimulationAffinityCore( atkPokeID, defPokeID, waza, ONLY_ATTACKER );
}

//--------------------------------------------------------------------------------------
/**
* @brief [ハンドラ用ツール] 相性計算シミュレート結果を返す(ポケモンチェンジAI用)
*
* @param   atkPokeID     攻撃側ポケモンのID
* @param   defPokeID     防御側ポケモンのID
* @param   waza          使用する技
*
* @return  技の相性
*/
//--------------------------------------------------------------------------------------
BtlTypeAff ServerFlow::Hnd_SimulationAffinityCanBench( u8 atkPokeID, u8 defPokeID, WazaID waza )
{
  static const bool ONLY_ATTACKER = false;
  return this->Hnd_SimulationAffinityCanBenchCore( atkPokeID, defPokeID, waza, ONLY_ATTACKER );
}

//--------------------------------------------------------------------------------------
/**
 * @brief [ハンドラ用ツール] 相性計算シミュレート結果を返す( 攻撃側からは知ることが出来ない、防御側ポケモンの特性等を考慮しない )
 *
 * @param   atkPokeID     攻撃側ポケモンのID
 * @param   defPokeID     防御側ポケモンのID
 * @param   waza          使用する技
 *
 * @return  技の相性
 */
//--------------------------------------------------------------------------------------
BtlTypeAff ServerFlow::Hnd_SimulationAffinityOnlyAttacker( u8 atkPokeID, u8 defPokeID, WazaID waza )
{
  static const bool ONLY_ATTACKER = true;
  return this->Hnd_SimulationAffinityCanBenchCore( atkPokeID, defPokeID, waza, ONLY_ATTACKER );
}

/**
 * @brief [ハンドラ用ツール] 相性計算シミュレート結果を返す
 *
 * @param   atkPokeID     攻撃側ポケモンのID
 * @param   defPokeID     防御側ポケモンのID
 * @param   waza          使用する技
 * @param   onlyAttacker  true なら、防御側ポケモンの特性等を考慮しない
 *
 * @return  技の相性
 */
BtlTypeAff ServerFlow::Hnd_SimulationAffinityCore( u8 atkPokeID, u8 defPokeID, WazaID waza, bool onlyAttacker )
{
  const BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( atkPokeID );
  const BTL_POKEPARAM* defender = m_pokeCon->GetPokeParam( defPokeID );
  WAZAPARAM  wazaParam;
  BtlTypeAff result;

  m_simulationCounter++;
  handler::waza::Add( attacker, waza, attacker->GetValue( BTL_POKEPARAM::BPP_AGILITY ) );

  if( defender->IsFakeEnable() )
  {
    defender = m_mainModule->GetFakeTargetPokeParam( m_pokeCon, defender );
  }
  scEvent_GetWazaParam(  waza, attacker, &wazaParam );

  bool isNoEffectByFloatingStatus = false;
  result = scProc_checkWazaDamageAffinity( attacker, defender, &wazaParam, onlyAttacker, &isNoEffectByFloatingStatus );

  handler::waza::Remove( attacker, waza );
  m_simulationCounter--;

  return result;
}

/**
* @brief [ハンドラ用ツール] 控えポケモン正確版 UI＆AI専用 相性計算シミュレート結果を返す
*
* @param   atkPokeID     攻撃側ポケモンのID
* @param   defPokeID     防御側ポケモンのID
* @param   waza          使用する技
*
* @return  技の相性
*/
BtlTypeAff ServerFlow::Hnd_SimulationAffinityCanBenchCore( u8 atkPokeID, u8 defPokeID, WazaID waza, bool onlyAttacker )
{
  const BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam(atkPokeID);
  const BTL_POKEPARAM* defender = m_pokeCon->GetPokeParam(defPokeID);
  WAZAPARAM  wazaParam;
  BtlTypeAff result;

  m_simulationCounter++;

  const u32 FRONT_POKEMON_MAX = 2;  // 場に出ているポケモン最大数
  BTL_POKEPARAM* frontAttacker[FRONT_POKEMON_MAX] = { NULL, NULL };
  u32 targetsPokeIDIndex = 0;       // 自分が管理している場に出ているポケモン数（attackerが控えの場合使用）
  bool isAttackerBench = false;

  // 控えポケモン又は退場済みポケモンの場合
  if (m_posPoke.GetPokeExistPos(atkPokeID) == BTL_POS_NULL)
  {
    isAttackerBench = true;
    // @fix MMCat[160]マルチバトルでパートナーのポケモンを選択するととまる
    const u8 myClientID = m_mainModule->PokeIDtoClientID(atkPokeID);
    const u8 frontPosNum = m_mainModule->GetClientFrontPosCount(myClientID);
    // 場に出ているポケモン検索
    for (u32 posIndex(0); posIndex < frontPosNum; ++posIndex)
    {
      BtlPokePos pos = m_mainModule->GetClientPokePos(myClientID, posIndex);
      BTL_POKEPARAM* poke = m_pokeCon->GetFrontPokeData(pos);
      // 場に出ている
      if (m_posPoke.GetExistPokeID(pos) != BTL_POKEID_NULL && poke != NULL)
      {
        frontAttacker[targetsPokeIDIndex] = poke;
        // 場に出ているポケモンの特性と道具を一旦休止する
        handler::tokusei::RotationSleep(frontAttacker[targetsPokeIDIndex]);
        handler::item::RotationSleep(frontAttacker[targetsPokeIDIndex]);
        ++targetsPokeIDIndex;
      }
    }
    // 控えポケモン又は退場済みポケモンの特性と道具を追加
    handler::tokusei::Add(attacker);
    handler::item::Add(attacker);
  }
  handler::waza::Add(attacker, waza, attacker->GetValue(BTL_POKEPARAM::BPP_AGILITY));

  if (defender->IsFakeEnable())
  {
    defender = m_mainModule->GetFakeTargetPokeParam(m_pokeCon, defender);
  }
  scEvent_GetWazaParam(waza, attacker, &wazaParam);

  bool isNoEffectByFloatingStatus = false;
  result = scProc_checkWazaDamageAffinity(attacker, defender, &wazaParam, onlyAttacker, &isNoEffectByFloatingStatus);

  handler::waza::Remove(attacker, waza);
  // 控えポケモン又は退場済みポケモンの場合
  if (isAttackerBench)
  {
    // 控えポケモン又は退場済みポケモンの特性と道具を破棄
    handler::tokusei::Remove(attacker);
    handler::item::Remove(attacker);
    // 場に出ているポケモンの特性と道具を復帰
    for (u32 index(0); index < targetsPokeIDIndex; ++index)
    {
      // 場に出ているポケモンの特性と道具を復帰する
      handler::tokusei::RotationWakeNoAdd(frontAttacker[index]);
      handler::item::RotationWakeNoAdd(frontAttacker[index]);
    }
  }
  m_simulationCounter--;

  return result;
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] ダメージ計算シミュレート結果を返す
 *
 * @param   flowWk
 * @param   atkPokeID       攻撃ポケID
 * @param   defPokeID       防御ポケID
 * @param   waza            ワザ
 * @param   fAffinity       相性計算するか（trueなら等倍で計算）
 * @param   fEnableRand     乱数有効
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::Hnd_SimulationDamage( u8 atkPokeID, u8 defPokeID, WazaID waza, bool fAffinity, bool fEnableRand )
{
  if( ( waza != WAZANO_NULL )
  &&  ( WAZADATA_IsDamage(waza) )
  ){
    const BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( atkPokeID );
    const BTL_POKEPARAM* defender = m_pokeCon->GetPokeParam( defPokeID );

    BtlTypeAff  aff;
    WAZAPARAM  wazaParam;
    u16  damage;

    m_simulationCounter++;

    if( defender->IsFakeEnable() )
    {
      defender = m_mainModule->GetFakeTargetPokeParam( m_pokeCon, defender );
    }

    //
    if( fAffinity ){
      aff = Hnd_SimulationAffinity( atkPokeID, defPokeID, waza );
    }else{
      aff = pml::battle::TypeAffinity::TYPEAFF_1;
    }

    scEvent_GetWazaParam(  waza, attacker, &wazaParam );

    scEvent_CalcDamage(  attacker, defender, &wazaParam, aff,
      BTL_CALC_DMG_TARGET_RATIO_NONE, false, !fEnableRand, false, &damage );

    m_simulationCounter--;


    return damage;
  }
  else{
    return 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] シミュレーション用呼び出しかどうかを判定
 *
 * @param   wk
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsSimulationMode( void )
{
  return m_simulationCounter != 0;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 自クライアントが担当している戦闘位置の数を返す
 *
 * @param   wk
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetClientCoverPosCount( u8 pokeID )
{
  SVCL_WORK* clwk;
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );

  clwk = m_server->GetClientWork( clientID );
  GFL_ASSERT_STOP( clwk );

  return m_mainModule->GetClientCoverPosNum( clwk->myID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 自クライアントパーティ内、控えの開始Indexを取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetMyBenchIndex( u8 pokeID )
{
  return Hnd_GetClientCoverPosCount( pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 自クライアントパーティに、（戦闘可能な）控えのポケモンがいるか
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsExistBenchPoke( u8 pokeID )
{
  u8 fightEnableBenchPokeNum = this->getFightEnableBenchPokeNum( pokeID );
  return ( 0 < fightEnableBenchPokeNum );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 自クライアントパーティに、（入れ替え可能な）控えのポケモンがいるか
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval true   入れ替え可能な控えのポケモンが存在する
 * @retval false  入れ替え可能な控えのポケモンが存在しない
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsExistChangeEnablePoke( u8 pokeID )
{
  BTL_CLIENT_ID clientID = static_cast<BTL_CLIENT_ID>( m_mainModule->PokeIDtoClientID( pokeID ) );
  u8 changeRequestPokeNum = m_server->GetChangeRequestPokeCount( clientID );
  u8 fightEnableBenchPokeNum = this->getFightEnableBenchPokeNum( pokeID );
  return( changeRequestPokeNum < fightEnableBenchPokeNum );
}

/**
 * @brief 自クライアントのパーティに、戦闘可能な控えポケモンの数を取得する
 * @param pokeID  チェック対象ポケモンのID
 */
u8 ServerFlow::getFightEnableBenchPokeNum( u8 pokeID )
{
  const BTL_PARTY* party = Hnd_GetPartyData( pokeID );
  u8 startIdx = Hnd_GetMyBenchIndex( pokeID );
  return ( party->GetAliveMemberCountRear(startIdx) );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 既にメンバー入れ替えアクションを実行しようとしているポケモンがいるか判定
 *
 * @retval  true   入れ替えを実行しようとしているポケモンがいる
 * @retval  false  入れ替えを実行しようとしているポケモンがいない
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckReservedMemberChangeAction( void ) const
{
  return ( *m_fMemberChangeReserve );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 該当位置にいる生存しているポケモンIDを配列に格納＆数を返す
 *
 * @param   wk
 * @param   exPos
 * @param   dst_pokeID    [out]格納先配列
 *
 * @retval  u8    格納した数
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_ExpandPokeID( BtlExPos exPos, u8* dst_pokeID )
{
  u8 pos[ BTL_EXIST_POS_MAX ];
  BTL_POKEPARAM* bpp;
  u8 max, num, i;

  max = m_mainModule->ExpandBtlPos( exPos, pos );

  for(i=0, num=0; i<max; ++i)
  {
    bpp = m_pokeCon->GetFrontPokeData( pos[i] );
    if( IsBppExist(bpp) ){
      dst_pokeID[num++] = bpp->GetID();
    }
  }
  return num;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定ポケモンを含むパーティデータを取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_PARTY*
 */
//--------------------------------------------------------------------------------------
const BTL_PARTY* ServerFlow::Hnd_GetPartyData( u8 pokeID )
{
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );
  return m_pokeCon->GetPartyDataConst( clientID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] マルチ時、味方クライアントのパーティデータ取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_PARTY*  味方クライアントのパーティデータ（非マルチ時はNULL）
 */
//--------------------------------------------------------------------------------------
const BTL_PARTY* ServerFlow::Hnd_GetFriendPartyData( u8 pokeID )
{
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );
  u8 friendClientID = m_mainModule->GetFriendCleintID( clientID );
  if( m_mainModule->IsExistClient(friendClientID) )
  {
    return m_pokeCon->GetPartyDataConst( friendClientID );
  }
  return NULL;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定ポケモンの戦闘位置を返す
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos ServerFlow::Hnd_PokeIDtoPokePos( u8 pokeID ) const
{
  return m_mainModule->PokeIDtoPokePos( m_pokeCon, pokeID );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定位置のポケモンIDを返す
 *
 * @param   wk
 * @param   pokePos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_PokePosToPokeID( u8 pokePos )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetFrontPokeDataConst( pokePos );
  if( bpp ){
    return bpp->GetID();
  }
  return BTL_POKEID_NULL;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定ポケモンが「ゆびをふる」など他のワザを呼び出した際、ターゲットを自動で決める
 * ※ターゲット選択の必要が無いワザの場合、BTL_POS_NULL が返る
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos ServerFlow::Hnd_ReqWazaTargetAuto( u8 pokeID, WazaID waza )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  BtlPokePos targetPos;

  targetPos = calc::DecideWazaTargetAuto( m_mainModule, m_pokeCon, bpp, waza );
  return targetPos;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 現ターンに選択されたアクション内容を取得
 *
 * @param   wk
 * @param   pokeID
 * @param   dst       [out]
 *
 * @retval  bool    正しく取得できたらtrue（現ターン、参加していないポケなどが指定されたら指定のとくせいを持つ）
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_GetThisTurnAction( u8 pokeID, BTL_ACTION_PARAM* dst )
{
  for(u32 i=0; i<m_numActOrder; ++i)
  {
    if( m_actOrder[i].bpp->GetID() == pokeID )
    {
      // メガシンカは返さない
      if( !BTL_ACTION_IsMegaEvoParam(&m_actOrder[i].action) )
      {
        *dst = m_actOrder[i].action;
        return true;
      }
    }
  }
  return false;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 自分のアクションが全体の何番目かを取得
 *
 * @param   wk
 * @param   pokeID
 * @param   myOrder       [out] 自分の順位
 * @param   totalAction   [out] 全アクション数
 *
 * @retval  bool    正しく取得できたらtrue（現ターン、参加していないポケなどが指定されたらtrue）
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsMyActOrderLast( u8 pokeID )
{
  u32 i;
  u32 otherPokeActCount, endActCount;

  otherPokeActCount = endActCount = 0;
  for(i=0; i<m_numActOrder; ++i)
  {
    if( m_actOrder[i].bpp->GetID() != pokeID )
    {
      ++otherPokeActCount;
      if( m_actOrder[i].fDone ){
        ++endActCount;
      }
    }
  }

  if( otherPokeActCount == endActCount ){
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------------
/**
 * @brief [ハンドラ用ツール] 指定したポケモンについて、全ての行動が終了しているかチェックする
 * @param pokeID  チェック対象ポケモンのID
 * @retval true   終了している
 * @retval false  終了していない
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsMyAllActDone( u8 pokeID ) const
{
  return ActOrderTool_CheckAllActDoneByPokeID( pokeID );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 直前に出されたワザを取得
 *
 * @param   wk
 *
 * @retval  WazaID
 */
//--------------------------------------------------------------------------------------
WazaID ServerFlow::Hnd_GetPrevExeWaza( void ) const
{
  return m_prevExeWaza;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 直前に出されたワザが全力技か？
 *
 * @retval true   直前に出された技が全力技の場合
 * @retval false  直前に出された技が全力技でない場合
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsPrevExeZenryokuWaza( void ) const
{
  return m_isPrevExeZenryokuWaza;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 全ハンドラ共有のテンポラリワーク先頭アドレスを返す
 *
 * @param   wk
 *
 * @retval  void*
 */
//--------------------------------------------------------------------------------------
void* ServerFlow::Hnd_GetTmpWork( u32 size )
{
  GFL_ASSERT(size < sizeof(m_handlerTmpWork) );
  return m_handlerTmpWork;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 出たワザレコードのポインタを取得
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
const WazaRec* ServerFlow::Hnd_GetWazaRecord( void )
{
  return &m_wazaRec;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] Que領域予約
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
u16 ServerFlow::Hnd_ReserveQuePos( ServerCmd cmd )
{
  return scmd::SCQUE_RESERVE_Pos( m_que, cmd );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 死亡ポケレコードのポインタを取得
 *
 * @param   wk
 *
 * @retval  const BTL_DEADREC*
 */
//--------------------------------------------------------------------------------------
const DeadRec* ServerFlow::Hnd_GetDeadRecord( void )
{
  return &m_deadRec;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 現在の経過ターン数を取得
 *
 * @param   wk
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 ServerFlow::Hnd_GetTurnCount( void ) const
{
  return m_turnCount;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] メンバー交替カウンタの値を取得
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetPokeChangeCount( u8 clientID )
{
  return m_memberChangeCount[ clientID ];
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] ポケモンが場にいるか
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsExistPokemon( u8 pokeID )
{
  return m_posPoke.IsExist(pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] バトルルール取得
 *
 * @param   wk
 *
 * @retval  BtlRule
 */
//--------------------------------------------------------------------------------------
BtlRule ServerFlow::Hnd_GetRule( void )
{
  return m_mainModule->GetRule();
}
//--------------------------------------------------------------------------------------
/**
 *  [ハンドラ用ツール] 対戦者タイプを取得
 *
 * @param   wk
 *
 * @retval  BtlCompetitor
 */
//--------------------------------------------------------------------------------------
BtlCompetitor ServerFlow::Hnd_GetCompetitor( void )
{
  return m_mainModule->GetCompetitor();
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] グラウンドタイプ（フィールドワザ効果）を取得
 *
 * @retval  BtlGround
 */
//--------------------------------------------------------------------------------------
BtlGround  ServerFlow::Hnd_GetGround( void )
{
  return m_fieldStatus->GetGround();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] フィールドの状態を取得
 */
//--------------------------------------------------------------------------------------
const BgComponentData&  ServerFlow::Hnd_GetBgComponentData( void ) const
{
  const BTL_FIELD_SITUATION* fieldSituation = m_mainModule->GetFieldSituation();
  return fieldSituation->bgComponent;
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 前衛位置の数を返す
 *
 * @param   pokeID    クライアントID（このクライアントが属する陣営の前衛位置数を返す）
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::Hnd_GetFrontPosNum( u8 clientID )
{
  return m_mainModule->GetFrontPosNum( clientID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] マルチモードかどうかチェック
 *
 * @param   wk
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsMultiMode( void )
{
  return m_mainModule->IsMultiMode();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 進化前ポケモンかどうかチェック
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckSinkaMae( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  const pml::pokepara::PokemonParam* pp = bpp->GetSrcData();

  return pp->HaveEvolutionRoot();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 素早さ計算
 *
 * @param   bpp                 対象ポケパラ
 * @param   fTrickRoomApply     トリックルーム効果が発動していた場合、適用するか？
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 ServerFlow::Hnd_CalcAgility( const BTL_POKEPARAM* bpp, bool fTrickRoomApply )
{
  return scEvent_CalcAgility( bpp, fTrickRoomApply );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] フィールドエフェクト有無チェック
 *
 * @param   effType   エフェクト指定
 *
 * @retval  bool    指定されたエフェクトが有効ならtrue
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckFieldEffect( FieldStatus::EffectType effType ) const
{
  return m_fieldStatus->CheckEffect( effType );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 素早さ計算->場に出ているポケモン内でのランクを返す
 *
 * @param   wk
 * @param   bpp_target
 *
 * @retval  u16 ランク（0〜）
 */
//--------------------------------------------------------------------------------------
u16 ServerFlow::Hnd_CalcAgilityRank( const BTL_POKEPARAM* bpp_target, bool fTrickRoomEnable )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u16 rank, agi, target_agi;
  u8 target_pokeID;

  target_agi = Hnd_CalcAgility( bpp_target, fTrickRoomEnable );
  target_pokeID = bpp_target->GetID();
  rank = 0;

  FRONT_POKE_SEEK_InitWork( &fps );
  while( FRONT_POKE_SEEK_GetNext( &fps, &bpp ) )
  {
    if( bpp->GetID() == target_pokeID ){
      continue;
    }
    agi = Hnd_CalcAgility(  bpp, fTrickRoomEnable );
    if( agi > target_agi ){
      ++rank;
    }
  }

  return rank;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] “浮いている”状態チェック
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsFloatingPoke( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  return scproc_CheckFloating( bpp, true );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] アイテム利用可否チェック
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckItemUsable( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );

  if( bpp->GetValue(BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE) == TOKUSEI_BUKIYOU ){
    return false;
  }
  if( bpp->CheckSick(pml::wazadata::WAZASICK_SASIOSAE) ){
    return false;
  }
  if( m_fieldStatus->CheckEffect(FieldStatus::EFF_MAGICROOM) )
  {
    return false;
  }

  return true;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 利用可能な状態にある所持アイテムを取得
 *
 * @param   pokeID
 *
 * @retval  u16    アイテムID
 */
//--------------------------------------------------------------------------------------
u16 ServerFlow::Hnd_GetUsableItem( u8 pokeID )
{
  if( Hnd_CheckItemUsable(pokeID) )
  {
    const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
    return bpp->GetItem();
  }
  return ITEM_DUMMY_DATA;
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 天候取得
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//--------------------------------------------------------------------------------------
BtlWeather ServerFlow::Hnd_GetWeather( void )
{
  return scEvent_GetWeather();
}

//@バトル変更箇所(エアロックを無視した本来の天気の取得 退場時のsango新天候戻しチェックにのみ使用
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] エアロック抜きの天候取得
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//--------------------------------------------------------------------------------------
BtlWeather ServerFlow::Hnd_GetWeather_True( void )
{
  return m_fieldStatus->GetWeather();
}

//@バトル変更箇所(sango新天候戻しでデフォルト天候がチェックしたかった。
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] デフォルト天候取得
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//--------------------------------------------------------------------------------------
BtlWeather ServerFlow::Hnd_GetDefaultWeather( void )
{
  return m_mainModule->GetDefaultWeather();
}

//--------------------------------------------------------------------------------------
/**
 * @brief 現在の天候の原因となったポケモンのIDを取得する
 * @retval BTL_POKEID_NULL  原因となったポケモンが存在しない場合
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetWeatherCausePokeID( void ) const
{
  return m_fieldStatus->GetWeatherCausePokeID();
}


//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] デバッグフラグ取得
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_GetDebugFlag( BtlDebugFlag flag )
{
  return m_mainModule->GetDebugFlag( flag );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定サイドエフェクトが働いているか判定
 *
 * @param   wk
 * @param   BtlPokePos      pos
 * @param   BtlSideEffect
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsExistSideEffect( BtlSide side, BtlSideEffect sideEffect ) const
{
  return handler::side::IsExist( side, sideEffect );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] サイドエフェクトの状態取得
 *
 * @param side         取得対象の陣営
 * @param sideEffect   取得対象のサイドエフェクト
 */
//--------------------------------------------------------------------------------------
const SideEffectStatus* ServerFlow::Hnd_GetSideEffectStatus( BtlSide side, BtlSideEffect sideEffect ) const
{
  const SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatusConst( side, sideEffect );
  return sideEffectStatus;
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定サイドエフェクトのカウンタを取得
 *
 * @param   wk
 * @param   BtlPokePos      pos
 * @param   BtlSideEffect
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::Hnd_GetSideEffectCount( BtlPokePos pos, BtlSideEffect sideEffect ) const
{
  BtlRule rule = m_mainModule->GetRule();
  BtlSide side = MainModule::PosToSide( rule, pos );
  const SideEffectStatus* sideEffectStatus = m_pokeCon->GetSideEffectStatusConst( side, sideEffect );
  return sideEffectStatus->GetAddCount();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定位置エフェクトの有無を確認
 *
 * @param   wk
 * @param   pos
 * @param   effect
 *
 * @retval  bool  指定された位置エフェクトが存在していたら true 
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsExistPosEffect( BtlPokePos pos, BtlPosEffect effect )
{
  return handler::pos::IsRegistered( effect, pos );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定クライアントの味方クライアントIDを返す（味方が居ない時は BTL_CLIENTID_NULL）
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 ServerFlow::Hnd_GetFriendClientID( u8 clientID )
{
  return m_mainModule->GetFriendCleintID( clientID );
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] メンバー入れ替え時の割り込みアクション解決中であるかチェック
 *
 * @param   wk
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsMemberOutIntr( void )
{
  return m_fMemberOutIntr;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] メンバー入れ替え時の割り込みリクエスト
 *
 * @param   wk
 * @param   pokeID    割り込みたいポケID
 */
//--------------------------------------------------------------------------------------
void ServerFlow::Hnd_AddMemberOutIntr( u8 pokeID )
{
  if( m_MemberOutIntrPokeCount < GFL_NELEMS(m_MemberOutIntrPokeID) )
  {
    m_MemberOutIntrPokeID[ m_MemberOutIntrPokeCount++ ] = pokeID;
  }
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] そらをとぶ・ダイビング・フリーフォール等で画面から消えている状態のポケモンかどうか判定
 *
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsTameHidePoke( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  return bpp->IsWazaHide();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] フリーフォール実行側のポケモンかどうか判定
 *
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsFreeFallUserPoke( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
  return bpp->IsUsingFreeFall();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] フリーフォール実行側・または捕まれている側のポケモンかどうか判定
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_IsFreeFallPoke( u8 pokeID )
{
  if( Hnd_IsFreeFallUserPoke(pokeID) ){
    return true;
  }
  else{
    const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );
    if( bpp->CheckSick(pml::wazadata::WAZASICK_FREEFALL) ){
      return true;
    }
  }
  return false;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 体重取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::Hnd_GetWeight( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( pokeID );

  fx32 ratio = svEvent_GetWaitRatio( bpp );
  u32  weight = bpp->GetWeight();

  weight = calc::MulRatio( weight, ratio );
  if( weight < BTL_POKE_WEIGHT_MIN ){
    weight = BTL_POKE_WEIGHT_MIN;
  }
  BTL_PRINT(" pokeID:%d の weight = %d\n", pokeID, weight );
  return weight;
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] ランク効果シリアル値を取得
 *
 * @param   wk
 *
 * @retval  U32
 */
//--------------------------------------------------------------------------------------
u32 ServerFlow::Hnd_GetRankEffSerial( void )
{
  return wazaSerial_Inc();
}
//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 決着判定
 *
 * @param   wk
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::Hnd_CheckShowDown( void )
{
  return ( scproc_CheckShowdown() && isSkipBattleAfterShowdown() );
}

//--------------------------------------------------------------------------------------
/**
 * [ハンドラ用ツール] 指定のクライアントIDにトレーナー（プレイヤーorNPC）が存在するかチェック
 *
 * @param   clientID
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool ServerFlow::IsClientTrainerExist( u8 clientID ) const
{
  return m_mainModule->IsClientTrainerExist( clientID );
}




//=============================================================================================
/**
 * [ハンドラからの操作呼び出し]
 *
 * @param   wk
 * @param   effIndex
 */
//=============================================================================================
void ServerFlow::Hnd_SetWazaEffectIndex( u8 effIndex )
{
  wazaEffCtrl_SetEffectIndex( m_wazaEffCtrl, effIndex );
}
//=============================================================================================
/**
 * [ハンドラからのシステム呼び出し]
 *  スタンドアローンバトルで、プレイヤーポケモンの使用したハンドラから呼び出された場合のみ
 *  おこづかいを上乗せする（ネコにこばん専用）
 *
 * @param   wk
 * @param   volume    上乗せ額
 * @param   pokeID    ワザ使用ポケモンID
 *
 * @retval  bool      条件を満たした（上乗せした）場合にtrueが返る
 */
//=============================================================================================
bool ServerFlow::Hnd_AddBonusMoney( u32 volume, u8 pokeID )
{
  GFL_UNUSED( pokeID );

  if( m_mainModule->IsCompetitorScenarioMode() )
  {
    m_server->AddBonusMoney( volume );
    return true;
  }
  return false;
}
//=============================================================================================
/**
 * [ハンドラからのシステム呼び出し]
 * スタンドアロンバトルで、終了時にもらえるお金を倍にする（効果は１度だけ）
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void ServerFlow::Hnd_SetMoneyDblUp( u8 pokeID, MoneyDblUpCause cause )
{
  u8 clientID = MainModule::PokeIDtoClientID( pokeID );

  if( (m_mainModule->IsCompetitorScenarioMode())
  &&  (clientID == m_mainModule->GetPlayerClientID())
  ){
    m_server->SetMoneyDblUp( cause );
  }
}
//=============================================================================================
/**
 * [ハンドラからの操作呼び出し]  マジックコート跳ね返し登録
 *
 * @param   wk
 * @param   pokeID          マジックコートを使った側のポケモンID
 * @param   targetPokeID    跳ね返す先のポケモンID
 */
//=============================================================================================
//void BTL_SVFRET_AddMagicCoatAction( ServerFlow* wk, u8 pokeID, u8 targetPokeID )
void ServerFlow::Hnd_AddMagicCoatAction( u8 pokeID, u8 targetPokeID )
{
  BtlPokePos targetPos = m_posPoke.GetPokeExistPos( targetPokeID );
  wazaRobParam_Add( m_magicCoatParam, pokeID, targetPokeID, targetPos );
}
//=============================================================================================
/**
 * [ハンドラからの操作呼び出し]  フリーフォール溜めターン処理
 *
 * @param   wk
 * @param   atkPokeID
 * @param   targetPokeID
 *
 * @retval  bool    成功すればtrue
 */
//=============================================================================================
bool ServerFlow::Hnd_FreeFallStart( u8 atkPokeID, u8 targetPokeID, const WAZAPARAM* wazaParam, bool* fFailMsgDisped )
{
  BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( atkPokeID );
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( targetPokeID );

  return scproc_FreeFall_Start( attacker, target, wazaParam, fFailMsgDisped );
}
//=============================================================================================
/**
 * [ハンドラからの操作呼び出し]  フリーフォール解放ターン処理
 *
 * @param   atkPokeID
 * @param   bAppearSelf
 * @param   bAppearTArget
 *
 */
//=============================================================================================
void ServerFlow::Hnd_FreeFallRelease( u8 atkPokeID, bool bAppearSelf, bool bAppearTArget )
{
  BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( atkPokeID );

  scproc_FreeFall_CheckRelease( attacker, bAppearSelf, bAppearTArget );
}
//=============================================================================================
/**
 * [ハンドラからの操作呼び出し]  フリーフォール解放後、相手ポケモンの表示復帰処理
 *
 * @param targetPokeID
 */
//=============================================================================================
void ServerFlow::Hnd_FreeFallAppearTarget( u8 targetPokeID )
{
  if( m_posPoke.IsExist(targetPokeID) )
  {
    const BTL_POKEPARAM* target = m_pokeCon->GetPokeParamConst( targetPokeID );
    SCQUE_PUT_ACT_TameWazaHide( m_que, targetPokeID, false );
    scPut_Message_Set( target, BTL_STRID_SET_FreeFall_End );
  }
}

//=============================================================================================
/**
 * [btl_server_flow_subから操作呼び出し]  装備アイテム効果処理
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
bool ServerFlow::Hnd_UseItemEquip( BTL_POKEPARAM* bpp, u8* fFailMsgDisped )
{
  bool result;

  m_fKaifukuFujiMsgDisped = false;
  result = scproc_UseItemEquip( bpp );
  *fFailMsgDisped = m_fKaifukuFujiMsgDisped;

  return result;
}
//=============================================================================================
/**
 * [btl_server_flow_subから操作呼び出し]  メンバー入場イベント（とくせいのみ）呼び出し
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  bool
 */
//=============================================================================================
void ServerFlow::Hnd_EventSkillCall(  const BTL_POKEPARAM* bpp )
{
  scEvent_SkillCall( bpp );
}

//=============================================================================================
/**
 * 各種の逃げ防止効果や、確率計算をスキップしてかならず逃げることが出来る状態かチェックする
 *
 * @param   pokeID
 *
 * @retval  bool    必ず逃げることが出来るなら true
 */
//=============================================================================================
bool ServerFlow::Hnd_CheckForceNigeru( u8 pokeID )
{
  const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParamConst( pokeID );
  return scproc_checkForceNigeru( bpp );
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  メガ進化可能なポケモンナンバーとアイテムの組み合わせかどうか判定
 *
 * @param   mons_no
 * @param   item_no
 *
 * @retval  bool    メガ進化可能なら true
 */
//=============================================================================================
bool ServerFlow::Hnd_IsMegaEvolveItem( MonsNo mons_no, u16 item_no )
{
  return m_evolveManager.IsMegaEvolveItem( mons_no, item_no );
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  ワザエフェクトの直前にきのみを食べるエフェクトを差し込む
 *
 * @param   pokeID
 */
//=============================================================================================
void ServerFlow::Hnd_EnableWazaSubEffect( u8 pokeID )
{
  wazaEffCtrl_AddSubEffectPoke( m_wazaEffCtrl, pokeID );
}


//=============================================================================================
/**
 * [ハンドラ用ツール]  任意ワザエフェクト実行
 * @param atkPos      発動ポケ位置
 * @param defPos      対象ポケ位置
 * @param waza        ワザナンバー
 * @param turnType    ターンタイプ
 * @param bSyncEffect 関連エフェクト（ダメージ処理など）のコマンドと同期するかどうか
 */
//=============================================================================================
void ServerFlow::Hnd_PlayWazaEffect( BtlPokePos atkPos, BtlPokePos defPos, WazaID waza, BtlvWazaEffect_TurnType turnType, bool bSyncEffect )
{
  SCQUE_PUT_ACT_WazaEffect( m_que, atkPos, defPos, waza, turnType, bSyncEffect, pml::waza::ZENRYOKUWAZA_KIND_NULL );
}
//=============================================================================================
/**
 * [ハンドラ用ツール] スカイバトルモードかどうか判定
 * @return  スカイバトルなら true
 */
//=============================================================================================
bool ServerFlow::Hnd_IsSkyBattleMode( void ) const
{
  return m_mainModule->IsSkyBattle();
}
//=============================================================================================
/**
 * [ハンドラ用ツール] 特殊なワザの追加効果発動チェック（てんのめぐみ効果などを考慮）
 * @param   atkPokeID
 * @param   defPokeID
 * @param   defaultPer    デフォルトの発動率（％）
 * @return  確率チェックし、発動するなら true を返す
 */
//=============================================================================================

bool  ServerFlow::Hnd_CheckOccurAdditionalEffect( u8 atkPokeID, u8 defPokeID, u32 defaultPer )
{
  u32 per = scEvent_CheckSpecialWazaAdditionalPer( atkPokeID, defPokeID, defaultPer );
  if( calc::GetRand(100) < per ){
    return true;
  }

  #if PM_DEBUG
  // 「かならず追加効果」のデバッグ設定に対応
  if( Hnd_GetDebugFlag(BTL_DEBUGFLAG_MUST_TUIKA) ){
    return true;
  }
  #endif

  return false;
}
//=============================================================================================
/**
 * [ハンドラ用ツール]  位置エフェクトハンドラが自滅する時に通知を受ける（全クライアントに通知）
 * @param effect    エフェクトタイプ
 * @param pos       エフェクトのあった位置ID
 */
//=============================================================================================
void   ServerFlow::Hnd_NotifyRemovePosEffect( BtlPosEffect effect, BtlPokePos pos )
{
  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( pos, effect );
  posEffectStatus->RemoveEffect();

  SCQUE_OP_RemovePosEffect( m_que, effect, pos );
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  位置エフェクトのパラメータを更新する
 * @param effect       エフェクトタイプ
 * @param pos          エフェクトのあった位置ID
 * @param effectParam  エフェクトのパラメータ
 */
//=============================================================================================
void ServerFlow::Hnd_UpdatePosEffectParam( BtlPosEffect effect, BtlPokePos pos, const PosEffect::EffectParam& effectParam )
{
  PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatus( pos, effect );
  posEffectStatus->SetEffectParam( effectParam );
  SCQUE_OP_UpdatePosEffectParam( m_que, effect, pos, effectParam.Raw.param1 );
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  位置エフェクトのパラメータを取得する
 * @param effect  エフェクトタイプ
 * @param pos     エフェクトのあった位置ID
 */
//=============================================================================================
const PosEffect::EffectParam& ServerFlow::Hnd_GetPosEffectParam( BtlPosEffect effect, BtlPokePos pos ) const
{
  const PosEffectStatus* posEffectStatus = m_pokeCon->GetPosEffectStatusConst( pos, effect );
  return posEffectStatus->GetEffectParam();
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  ポケモンの行動結果をチェックする
 * @param pokeId        チェック対象ポケモンのID
 * @param backTurnCount 遡るターン数( 0～RECORD_TURN_NUM-1, 0なら現在のターン )
 * @param actionId      チェック対象の行動結果
 * @retval true   指定した行動結果に該当する
 * @retval false　指定した行動結果に該当しない
 */
//=============================================================================================
bool ServerFlow::Hnd_CheckActionRecord( u8 pokeId, u8 backTurnCount, ActionRecorder::ActionID actionId ) const
{
  return m_actionRecorder->CheckAction( pokeId, backTurnCount, actionId );
}

//=============================================================================================
/**
 * [ハンドラ用ツール]  ステータスフラグを取得する
 * @param statusFlag  取得対象のステータスフラグ
 */
//=============================================================================================
bool ServerFlow::Hnd_GetStatusFlag( BTL_STATUS_FLAG statusFlag ) const
{
  return m_mainModule->GetSetupStatusFlag( statusFlag );
}

//=============================================================================================
/**
 * @brief [ハンドラ用ツール] 直後にポケモンの行動を挿入する
 * @param pokeId           技を撃つポケモン
 * @param wazaNo           実行する技
 * @param taragetPos       技ターゲットの位置
 * @param isZenryokuWaza   実行する技を全力技として使用するか？
 * @param actionDesc       アクションの詳細情報
 * @param infoID           アクション実行前に表示する情報ID
 * @retval true   挿入に成功した場合
 * @retval false  挿入に失敗した場合
 */
//=============================================================================================
bool ServerFlow::Hnd_InsertWazaAction( u8 pokeId, WazaNo wazaNo, BtlPokePos targetPos, bool isZenryokuWaza, const ActionDesc& actionDesc, uint infoID )
{
  ACTION_ORDER_WORK actOrder;
  BTL_ACTION_SetFightParam( &(actOrder.action), wazaNo, targetPos, isZenryokuWaza );

  actOrder.bpp        = m_pokeCon->GetPokeParam( pokeId );
  actOrder.clientID   = MainModule::PokeIDtoClientID( pokeId );
  actOrder.fDone      = false;
  actOrder.fIntrCheck = false;
  actOrder.actionDesc = actionDesc;
  actOrder.actionDesc.insertInfoID = infoID;
  actOrder.priority   = scproc_CalcActionPriority( actOrder );

  s32 insertIndex = ActOrderTool_Insert( actOrder, 0 );
  return ( 0 <= insertIndex );
}

//=============================================================================================
/**
 * @brief [ハンドラ用ツール] バトルロイヤルの順位コンテナを取得する
 */
//=============================================================================================
const RoyalRankingContainer& ServerFlow::Hnd_GetRoyalRaningContainer( void ) const
{
  return m_royalRankingContainer;
}
/**
 * @brief  [ハンドラ用ツール]  状態異常コードに対応する、個別失敗メッセージIDを返す
 * @param[in]  sickID   状態異常コード
 * @return   s16   有効なメッセージがあれば、そのメッセージID（0以上の値）／それ以外は0未満の値
 * @detail   個別失敗メッセージ = 汎用メッセージ「○○にはこうかがない」ではなく、「○○はどくにならない」など。
 */
s16 ServerFlow::Hnd_GetSpecificSickFailStrID( WazaSick sickID ) const
{
  switch( sickID ){
  case pml::wazadata::WAZASICK_MAHI:   return BTL_STRID_SET_MahiNoEffect;
  case pml::wazadata::WAZASICK_NEMURI: return BTL_STRID_SET_NemuriNoEffect;
  case pml::wazadata::WAZASICK_KOORI:  return BTL_STRID_SET_KoriNoEffect;
  case pml::wazadata::WAZASICK_YAKEDO: return BTL_STRID_SET_YakedoNoEffect;
  case pml::wazadata::WAZASICK_DOKU:   return BTL_STRID_SET_DokuNoEffect;
  case pml::wazadata::WAZASICK_KONRAN: return BTL_STRID_SET_KonranNoEffect;

  default:
    return -1;
  }
}



//=============================================================================================
/**
 * @brief 制限時間情報をセットする
 * @param timeLimit  制限時間情報
 */
//=============================================================================================
void ServerFlow::SetTimeLimit( const TimeLimit& timeLimit )
{
  TimeLimit_Copy( &m_timeLimit, timeLimit );
}

//=============================================================================================
/**
 * @brief 戦闘結果判定
 *
 * @param[out] judgeCause  勝敗判定の判定理由
 *
 * @retval  BtlResult
 */
//=============================================================================================
BtlResult ServerFlow::ChecBattleResult( ResultCause* judgeCause ) const
{
  // @fix GFNMCat[1587] 「バトルビデオ選択画面」で「バトルロイヤル」の対戦結果を見ると4位のみ「LOSE」で他は「WIN」と表示される
  // バトルロイヤルは、通常の戦闘と勝敗判定の方法が異なる（倒したポケモンの数で判定する）
  if( m_mainModule->GetRule() == BTL_RULE_ROYAL )
  {
    return BattleResultJudge::CheckBattleResult_Royal( *m_mainModule, m_royalRankingContainer, judgeCause );
  }

  return BattleResultJudge::CheckBattleResult( *m_mainModule, *m_pokeCon, m_deadRec, m_timeLimit, judgeCause );
}



/*--------------------------------------------------------------------------------------------*/
/* POKESET Stack : Action毎にPush,Popして各用途のPokeSetを個別に用意、利用するための仕組み    */
/*--------------------------------------------------------------------------------------------*/
// 初期化
void ServerFlow::PSetStack_Init( void )
{
  m_pokesetStackPtr = 0;
  psetstack_setup( 0, true );
}
// Push
void ServerFlow::PSetStack_Push( void )
{
  if( m_pokesetStackPtr < (GFL_NELEMS(m_pokesetUnit)-1) )
  {
    m_pokesetStackPtr++;
    psetstack_setup( m_pokesetStackPtr, true );
  }
  else{
    GFL_ASSERT(0);
  }
}
// Pop
void ServerFlow::PSetStack_Pop( void )
{
  if( m_pokesetStackPtr )
  {
    m_pokesetStackPtr--;
    psetstack_setup( m_pokesetStackPtr, false );
  }
  else
  {
    GFL_ASSERT(0);
  }
}
// 下請けカレント要素クリア
void ServerFlow::psetstack_setup( u32 sp, bool fClear )
{
  POKESET_STACK_UNIT* unit = &(m_pokesetUnit[ sp ]);

  m_psetTargetOrg  = &unit->TargetOrg;
  m_psetTarget     = &unit->Target;
  m_psetFriend     = &unit->Friend;
  m_psetEnemy      = &unit->Enemy;
  m_psetDamaged    = &unit->Damaged;
  m_psetRobTarget  = &unit->RobTarget;
  m_psetTmp        = &unit->psetTmp;
  m_wazaEffCtrl    = &unit->effCtrl;
  m_wazaParam      = &unit->wazaParam;
  m_wazaParamOrg   = &unit->wazaParamOrg;
  m_hitCheckParam  = &unit->hitCheck;
  m_wazaRobParam   = &unit->wazaRobParam;
  m_magicCoatParam = &unit->magicCoatParam;
  m_calcDmgFriend  = &unit->calcDmgFriend;
  m_calcDmgEnemy   = &unit->calcDmgEnemy;
  m_defaultTargetPos = unit->defaultTargetPos;
  m_fMemberChangeReserve = &unit->fMemberChangeReserve;
  m_fWazaFailMsgDisped   = unit->fWazaFailMsgDisped;

  if( fClear ){
    m_psetTargetOrg->Clear();
    m_psetTarget->Clear();
    m_psetFriend->Clear();
    m_psetEnemy->Clear();
    m_psetDamaged->Clear();
    m_psetRobTarget->Clear();
    m_psetTmp->Clear();
    wazaEffCtrl_Init( m_wazaEffCtrl );
    gfl2::std::MemClear( m_wazaParam, sizeof(*(m_wazaParam)) );
    gfl2::std::MemClear( m_wazaParamOrg, sizeof(*(m_wazaParamOrg)) );
    gfl2::std::MemClear( m_hitCheckParam, sizeof(*(m_hitCheckParam)) );
    gfl2::std::MemClear( m_wazaRobParam, sizeof(*(m_wazaRobParam)) );
    gfl2::std::MemClear( m_magicCoatParam, sizeof(*(m_magicCoatParam)) );
    gfl2::std::MemClear( m_calcDmgFriend, sizeof(*(m_calcDmgFriend)) );
    gfl2::std::MemClear( m_calcDmgEnemy, sizeof(*(m_calcDmgEnemy)) );

    m_defaultTargetPos = BTL_POS_NULL;
    *m_fMemberChangeReserve = false;
    m_fWazaFailMsgDisped = true;
  }
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//=============================================================================================
/**
 * HandEx ワーク１件取得
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  void*
 */
//=============================================================================================
void* ServerFlow::HANDEX_Push( EventHandlerExhibition eq_type, u8 userPokeID )
{
  return m_HEManager.PushWork( eq_type, userPokeID );
}
void ServerFlow::HANDEX_PushRun( EventHandlerExhibition eq_type, u8 userPokeID )
{
  void* work = HANDEX_Push( eq_type, userPokeID );
  HANDEX_Pop( work );
}
//=============================================================================================
/**
 * HandEx ワーク１件実行
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  void
 */
//=============================================================================================
void ServerFlow::HANDEX_Pop( void* handExWork )
{
  handEx_Exe( (BTL_HANDEX_PARAM_HEADER*)handExWork );
  m_HEManager.PopWork( handExWork );
}


//----------------------------------------------------------------------------------
/**
 * イベント実行結果を取得
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
HandExResult  ServerFlow::scproc_HandEx_Result( void )
{
  if( m_HEManager.IsUsed() )
  {
    bool fSucceed = m_HEManager.GetTotalResult();
    return (fSucceed)? HandExResult_Enable : HandExResult_Fail;
  }

  return HandExResult_NULL;
}
HandExResult ServerFlow::HandEx_Result( void )
{
  return scproc_HandEx_Result();
}

void ServerFlow::handEx_Exe( BTL_HANDEX_PARAM_HEADER* handEx_header )
{
  u16 useItemID = m_HEManager.GetUseItemNo();
  u8 fPrevSucceed;

  if(  m_HEManager.IsUsed() ){
    fPrevSucceed = m_HEManager.GetPrevResult();
  }else{
    fPrevSucceed = true;
  }

  if( handEx_header->failSkipFlag && !fPrevSucceed ){
    BTL_N_PrintfEx( PRINT_CHANNEL_HEMSYS, DBGSTR_SVFL_HandExContFail );
    return;
  }
  if( handEx_header->autoRemoveFlag ){
    const BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParamConst( handEx_header->userPokeID );
    if( bpp->IsDead() ){
      BTL_N_PrintfEx( PRINT_CHANNEL_HEMSYS, DBGSTR_SVFL_HandExContDead );
      return;
    }
  }

  switch( handEx_header->equip ){

  case BTL_HANDEX_TOKWIN_IN:          fPrevSucceed = scproc_HandEx_TokWinIn( handEx_header ); break;
  case BTL_HANDEX_TOKWIN_OUT:         fPrevSucceed = scproc_HandEx_TokWinOut( handEx_header ); break;
  case BTL_HANDEX_USE_ITEM:           fPrevSucceed = scproc_HandEx_useItem( handEx_header ); break;
  case BTL_HANDEX_RECOVER_HP:         fPrevSucceed = scproc_HandEx_recoverHP( handEx_header, useItemID ); break;
  case BTL_HANDEX_DRAIN:              fPrevSucceed = scproc_HandEx_drain( handEx_header ); break;
  case BTL_HANDEX_DAMAGE:             fPrevSucceed = scproc_HandEx_damage( handEx_header ); break;
  case BTL_HANDEX_SHIFT_HP:           fPrevSucceed = scproc_HandEx_shiftHP( handEx_header ); break;
  case BTL_HANDEX_RECOVER_PP:         fPrevSucceed = scproc_HandEx_recoverPP( handEx_header, useItemID ); break;
  case BTL_HANDEX_DECREMENT_PP:       fPrevSucceed = scproc_HandEx_decrementPP( handEx_header ); break;
  case BTL_HANDEX_CURE_SICK:          fPrevSucceed = scproc_HandEx_cureSick( handEx_header, useItemID ); break;
  case BTL_HANDEX_ADD_SICK:           fPrevSucceed = scproc_HandEx_addSick( handEx_header ); break;
  case BTL_HANDEX_RANK_EFFECT:        fPrevSucceed = scproc_HandEx_rankEffect( handEx_header, useItemID ); break;
  case BTL_HANDEX_SET_RANK:           fPrevSucceed = scproc_HandEx_setRank( handEx_header ); break;
  case BTL_HANDEX_RECOVER_RANK:       fPrevSucceed = scproc_HandEx_recoverRank( handEx_header ); break;
  case BTL_HANDEX_RESET_RANK:         fPrevSucceed = scproc_HandEx_resetRank( handEx_header ); break;
  case BTL_HANDEX_RESET_RANKUP:       fPrevSucceed = scproc_HandEx_resetRankUp( handEx_header ); break;
  case BTL_HANDEX_SET_STATUS:         fPrevSucceed = scproc_HandEx_setStatus( handEx_header ); break;
  case BTL_HANDEX_KILL:               fPrevSucceed = scproc_HandEx_kill( handEx_header ); break;
  case BTL_HANDEX_DEAD_CHECK:         fPrevSucceed = scproc_HandEx_deadCheck( handEx_header ); break;
  case BTL_HANDEX_CHANGE_TYPE:        fPrevSucceed = scproc_HandEx_changeType( handEx_header ); break;
  case BTL_HANDEX_EX_TYPE:            fPrevSucceed = scproc_HandEx_exType( handEx_header ); break;
  case BTL_HANDEX_MESSAGE:            fPrevSucceed = scproc_HandEx_message( handEx_header ); break;
  case BTL_HANDEX_SET_TURNFLAG:       fPrevSucceed = scproc_HandEx_setTurnFlag( handEx_header ); break;
  case BTL_HANDEX_RESET_TURNFLAG:     fPrevSucceed = scproc_HandEx_resetTurnFlag( handEx_header ); break;
  case BTL_HANDEX_SET_CONTFLAG:       fPrevSucceed = scproc_HandEx_setContFlag( handEx_header ); break;
  case BTL_HANDEX_RESET_CONTFLAG:     fPrevSucceed = scproc_HandEx_resetContFlag( handEx_header ); break;
  case BTL_HANDEX_SET_PERMFLAG:       fPrevSucceed = scproc_HandEx_setPermFlag( handEx_header ); break;
  case BTL_HANDEX_SIDEEFF_ADD:        fPrevSucceed = scproc_HandEx_sideEffectAdd( handEx_header ); break;
  case BTL_HANDEX_SIDEEFF_REMOVE:     fPrevSucceed = scproc_HandEx_sideEffectRemove( handEx_header ); break;
  case BTL_HANDEX_SIDEEFF_SLEEP:      fPrevSucceed = scproc_HandEx_sideEffectSleep( handEx_header ); break;
  case BTL_HANDEX_ADD_FLDEFF:         fPrevSucceed = scproc_HandEx_addFieldEffect( handEx_header ); break;
  case BTL_HANDEX_CHANGE_WEATHER:     fPrevSucceed = scproc_HandEx_changeWeather( handEx_header ); break;
  case BTL_HANDEX_REMOVE_FLDEFF:      fPrevSucceed = scproc_HandEx_removeFieldEffect( handEx_header ); break;
  case BTL_HANDEX_POSEFF_ADD:         fPrevSucceed = scproc_HandEx_PosEffAdd( handEx_header ); break;
  case BTL_HANDEX_CHANGE_TOKUSEI:     fPrevSucceed = scproc_HandEx_tokuseiChange( handEx_header ); break;
  case BTL_HANDEX_SET_ITEM:           fPrevSucceed = scproc_HandEx_setItem( handEx_header ); break;
  case BTL_HANDEX_CHECK_ITEM_EQUIP:   fPrevSucceed = scproc_HandEx_CheckItemEquip( handEx_header ); break;
  case BTL_HANDEX_ITEM_SP:            fPrevSucceed = scproc_HandEx_ItemSP( handEx_header ); break;
  case BTL_HANDEX_CONSUME_ITEM:       fPrevSucceed = scproc_HandEx_consumeItem( handEx_header ); break;
  case BTL_HANDEX_SWAP_ITEM:          fPrevSucceed = scproc_HandEx_swapItem( handEx_header ); break;
  case BTL_HANDEX_UPDATE_WAZA:        fPrevSucceed = scproc_HandEx_updateWaza( handEx_header ); break;
  case BTL_HANDEX_COUNTER:            fPrevSucceed = scproc_HandEx_counter( handEx_header ); break;
  case BTL_HANDEX_DELAY_WAZADMG:      fPrevSucceed = scproc_HandEx_delayWazaDamage( handEx_header ); break;
  case BTL_HANDEX_QUIT_BATTLE:        fPrevSucceed = scproc_HandEx_quitBattle( handEx_header ); break;
  case BTL_HANDEX_CHANGE_MEMBER:      fPrevSucceed = scproc_HandEx_changeMember( handEx_header ); break;
  case BTL_HANDEX_BATONTOUCH:         fPrevSucceed = scproc_HandEx_batonTouch( handEx_header ); break;
  case BTL_HANDEX_ADD_SHRINK:         fPrevSucceed = scproc_HandEx_addShrink( handEx_header ); break;
  case BTL_HANDEX_RELIVE:             fPrevSucceed = scproc_HandEx_relive( handEx_header ); break;
  case BTL_HANDEX_SET_WEIGHT:         fPrevSucceed = scproc_HandEx_setWeight( handEx_header ); break;
  case BTL_HANDEX_PUSHOUT:            fPrevSucceed = scproc_HandEx_pushOut( handEx_header ); break;
  case BTL_HANDEX_INTR_POKE:          fPrevSucceed = scproc_HandEx_intrPoke( handEx_header ); break;
  case BTL_HANDEX_INTR_WAZA:          fPrevSucceed = scproc_HandEx_intrWaza( handEx_header ); break;
  case BTL_HANDEX_SEND_LAST:          fPrevSucceed = scproc_HandEx_sendLast( handEx_header ); break;
  case BTL_HANDEX_SWAP_POKE:          fPrevSucceed = scproc_HandEx_swapPoke( handEx_header ); break;
  case BTL_HANDEX_HENSIN:             fPrevSucceed = scproc_HandEx_hensin( handEx_header ); break;
  case BTL_HANDEX_FAKE_BREAK:         fPrevSucceed = scproc_HandEx_fakeBreak( handEx_header ); break;
  case BTL_HANDEX_JURYOKU_CHECK:      fPrevSucceed = scproc_HandEx_juryokuCheck( handEx_header ); break;
  case BTL_HANDEX_TAMEHIDE_CANCEL:    fPrevSucceed = scproc_HandEx_TameHideCancel( handEx_header ); break;
  case BTL_HANDEX_ADD_EFFECT:         fPrevSucceed = scproc_HandEx_effectByPos( handEx_header ); break;
  case BTL_HANDEX_VANISH_MSGWIN:      fPrevSucceed = scproc_HandEx_vanishMsgWin( handEx_header ); break;
  case BTL_HANDEX_CHANGE_FORM:        fPrevSucceed = scproc_HandEx_changeForm( handEx_header ); break;
  case BTL_HANDEX_SET_EFFECT_IDX:     fPrevSucceed = scproc_HandEx_setWazaEffectIndex( handEx_header ); break;
  case BTL_HANDEX_WAZAEFFECT_ENABLE:  fPrevSucceed = scproc_HandEx_setWazaEffectEnable( handEx_header ); break;
  case BTL_HANDEX_FRIENDSHIP_EFFECT:  fPrevSucceed = scproc_HandEx_friendshipEffect( handEx_header ); break;
  case BTL_HANDEX_INSERT_WAZA_ACTION: fPrevSucceed = scproc_HandEx_insertWazaAction( handEx_header ); break;
  case BTL_HANDEX_SET_DIRT:           fPrevSucceed = scproc_HandEx_setDirt( handEx_header ); break;

  default:
    GFL_ASSERT_MSG(0, "illegal handEx type = %d, userPokeID=%d", handEx_header->equip, handEx_header->userPokeID);
  }

  BTL_N_PrintfEx( PRINT_CHANNEL_HEMSYS, DBGSTR_SVFL_HandExSetResult, fPrevSucceed );
  m_HEManager.SetResult( fPrevSucceed );
}
/**
 * とくせいウィンドウ表示
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_TokWinIn( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  if( m_posPoke.IsExist(param_header->userPokeID) )
  {
    BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
    scPut_TokWin_In( pp_user );
    return 1;
  }
  return 0;
}
/**
 * とくせいウィンドウ消去
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_TokWinOut( const BTL_HANDEX_PARAM_HEADER* param_header )
{
//  if( m_posPoke.IsExist( param_header->userPokeID) )
  {
    BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
    scPut_TokWin_Out( pp_user );
    return 1;
  }
//  return 0;
}
/**
 * ポケモンHP回復
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_recoverHP( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RECOVER_HP* param = (const BTL_HANDEX_PARAM_RECOVER_HP*)param_header;

  u8 result = 0;
/// 道具使用による実行もあるので、場にいなくても有効
//  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );

    if( !scproc_RecoverHP_CheckFailBase(pp_target) )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( pp_user );
      }

      if( param->fFailCheckThru == false )
      {
        result = !scproc_RecoverHP_CheckFailSP( pp_target, true );
      }
      else{
        result = true;
      }

      if( result )
      {
        scproc_RecoverHP_Core( pp_target, param->recoverHP, true );

        if( param->exStr.type != BTL_STRTYPE_NULL ){
            handexSub_putString( &(param->exStr) );
        }else if( itemID != ITEM_DUMMY_DATA ){
          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_UseItem_RecoverHP, param->pokeID, itemID );
        }
      }

      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( pp_user );
      }
    }
  }
  return result;
}
/**
 * ポケモンHP回復（ドレイン系）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_drain( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_DRAIN* param = (BTL_HANDEX_PARAM_DRAIN*)param_header;
  BTL_POKEPARAM* pp_damaged = NULL;
  if( param->damagedPokeID != BTL_POKEID_NULL ){
    pp_damaged = m_pokeCon->GetPokeParam( param->damagedPokeID );
  }

  if( m_posPoke.IsExist( param->recoverPokeID) )
  {
    BTL_POKEPARAM* pp_recover = m_pokeCon->GetPokeParam( param->recoverPokeID );

    if( !pp_recover->IsDead() )
    {
      if( scproc_DrainCore( pp_recover, pp_damaged, param->recoverHP, param->fFailCheckThru ) ){
        if( param->exStr.type != BTL_STRTYPE_NULL ){
          handexSub_putString( &(param->exStr) );
        }
        return 1;
      }
    }
  }
  return 0;
}
/**
 * ポケモンにダメージ
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_damage( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;

  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );
    BTL_POKEPARAM* pp_user = NULL;
    if( param_header->userPokeID != BTL_POKEID_NULL ){
      pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
    }

    if( !pp_target->IsDead() )
    {
      if( (!(param->fAvoidHidePoke))
      ||  (!pp_target->IsWazaHide()) )
      {
        bool isEnable = scproc_SimpleDamage_CheckEnable(pp_target, param->damage, param->damageCause);

        if( param_header->tokwin_flag ){
          scPut_TokWin_In( pp_user );
        }

        if( isEnable )
        {
          if( param->fExEffect ){
            scproc_ViewEffect( param->effectNo, param->pos_from, param->pos_to, false, 0 );
          }
          scproc_SimpleDamage_Core( pp_target, param->damage, param->damageCause, param->damageCausePokeID, &param->exStr, !param->fDisableDeadProcess );
        }
        else
        {
          scEvent_SimpleDamage_Failed( *pp_target, param->damageCause, param->damageCausePokeID );
        }

        if( param_header->tokwin_flag ){
          scPut_TokWin_Out( pp_user );
        }

        return ( isEnable ) ? ( 1 ) : ( 0 );
      }
    }
  }
  return 0;
}
/**
 * ポケモンHP増減（回復／ダメージと見なさない）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_shiftHP( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SHIFT_HP* param = (BTL_HANDEX_PARAM_SHIFT_HP*)param_header;
  u8 result = 0;

  for(u32 i=0; i<param->poke_cnt; ++i)
  {
    if( m_posPoke.IsExist(param->pokeID[i]) )
    {
      BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID[i] );
      if( !pp_target->IsDead() )
      {
        scPut_SimpleHp( pp_target, param->volume[i], param->damageCause, param_header->userPokeID, !param->fEffectDisable );
        if( !(param->fItemReactionDisable) ){
          scproc_CheckItemReaction( pp_target, BTL_ITEMREACTION_HP );
        }
        result = 1;
      }
    }
  }
  return result;
}
/**
 * ポケモンPP回復
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_recoverPP( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  GFL_UNUSED( itemID );

  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

/// 道具使用による実行もあるので、場にいなくても有効
//  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );

    if( pp_target->IsFightEnable() || (param->fDeadPokeEnable) )
    {
      bool fOrgWaza = (!(param->fSurfacePP));
      if( !pp_target->WAZA_IsPPFull(param->wazaIdx, fOrgWaza) )
      {
        scPut_RecoverPP( pp_target, param->wazaIdx, param->volume, fOrgWaza );
        handexSub_putString( &param->exStr );
        return 1;
      }
    }
  }
  return 0;
}
/**
 * ポケモンPP減少
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_decrementPP( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

// 大爆発->おんねん があるので、場にいなくても有効
//  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
    if( (!target->IsDead()) || (param->fDeadPokeEnable) )
    {
      if( scproc_decrementPP( target, param->wazaIdx, param->volume) )
      {
        handexSub_putString( &param->exStr );
        if( scEvent_DecrementPP_Reaction( target, param->wazaIdx ) )
        {
          scproc_UseItemEquip( target );
        }
      }
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモン状態異常回復
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_cureSick( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_CURE_SICK* param = (BTL_HANDEX_PARAM_CURE_SICK*)param_header;
  BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
  u8 result = 0;

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( pp_user );
  }

  BTL_PRINT("状態異常回復を %d 体文、チェック\n", param->poke_cnt);
  for(u32 i=0; i<param->poke_cnt; ++i)
  {
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID[i] );
    if( pp_target->IsFightEnable() )
    {
      BtlWazaSickEx exCode;
      BTL_SICKCONT oldCont;
      WazaSick check_sick;

      BTL_PRINT("pokeID_%d は戦えるポケです\n", param->pokeID[i]);

      exCode = param->sickCode;
      while( (check_sick = trans_sick_code(pp_target, &exCode)) != pml::wazadata::WAZASICK_NULL )
      {
        scPut_CureSick( pp_target, check_sick, &oldCont );
        if( !param->fStdMsgDisable )
        {
          if( BTL_SICK_MakeDefaultCureMsg(check_sick, oldCont, pp_target, itemID, &m_strParam) )
          {
            handexSub_putString( &m_strParam );
            HANDEX_STR_Clear( &m_strParam );
          }
        }
        else
        {
          handexSub_putString( &param->exStr );
        }
        if( check_sick == pml::wazadata::WAZASICK_SASIOSAE ){
          scproc_CheckItemReaction( pp_target, BTL_ITEMREACTION_GEN );
        }
        result = 1;
      }
    }
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( pp_user );
  }

  return result;
}
/**
 * ポケモン状態異常化
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_addSick( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)param_header;
  BTL_POKEPARAM* pp_user = (param_header->userPokeID != BTL_POKEID_NULL)?
                      m_pokeCon->GetPokeParam(param_header->userPokeID) : NULL;

  BTL_POKEPARAM* target;
  u8 fDefaultMsg = ( !(HANDEX_STR_IsEnable(&param->exStr)) && !(param->fStdMsgDisable) );

  if( m_posPoke.IsExist( param->pokeID) )
  {
    target = m_pokeCon->GetPokeParam( param->pokeID );
    if( !target->IsDead() 
    ||  param->fEffectiveToDeadPoke
    ){
      BTL_N_Printf( DBGSTR_SVFL_HandEx_AddSick, param->pokeID, param->sickID, param->sickCont.raw, param->fAlmost );

      if( !scproc_AddSickCheckFail( target, pp_user, param->sickID, param->sickCont, param->sickCause, WAZASERIAL_NULL, static_cast<SickOverWriteMode>(param->overWriteMode), param->fAlmost, false) )
      {
        if( param->header.tokwin_flag ){
          scPut_TokWin_In( pp_user );
        }

        scproc_AddSickCore( target, pp_user, param->sickID, param->sickCont, fDefaultMsg,
            param->fItemReactionDisable, &param->exStr );

        if( param->header.tokwin_flag ){
          scPut_TokWin_Out( pp_user );
        }

        return 1;
      }
    }
  }

  return 0;
}
/**
 * ポケモンランク効果（段階数を指定し、上昇または下降させる。相手のとくせい等により失敗することがあり得る）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_rankEffect( const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RANK_EFFECT* param = (BTL_HANDEX_PARAM_RANK_EFFECT*)param_header;
  BTL_POKEPARAM* pp_user = NULL;
  if( param_header->userPokeID != BTL_POKEID_NULL ){
    pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
  }
  BTL_POKEPARAM* pp_target;
  u8 fEffective = false;
  u8 result = 0;
  u32 i;

  for(i=0; i<param->poke_cnt; ++i)
  {
    if( m_posPoke.IsExist(param->pokeID[i]) )
    {
      pp_target = m_pokeCon->GetPokeParam( param->pokeID[i] );
      if( !(pp_target ->IsDead())
      &&  pp_target->IsRankEffectValid( static_cast<BTL_POKEPARAM::ValueID>(param->rankType), param->rankVolume )
      ){
        BTL_PRINT("[ランク効果]  POKEID=%d に効きます\n", pp_target->GetID());
        fEffective = true;
        break;
      }
    }
  }
  if( fEffective && param_header->tokwin_flag && pp_user){
    scPut_TokWin_In( pp_user );
  }

  for(i=0; i<param->poke_cnt; ++i)
  {
    if( m_posPoke.IsExist(param->pokeID[i]) )
    {
      pp_target = m_pokeCon->GetPokeParam( param->pokeID[i] );
      if( !pp_target->IsDead() )
      {
        const BTL_HANDEX_STR_PARAMS* preMsg = NULL;
        if( param->fMsgPreEffect ){
          preMsg = &param->exStr;
        }

        bool bMigawariThrew = param->bMigawariThrew;
        if( !bMigawariThrew && (param_header->userPokeID != BTL_POKEID_NULL) )
        {
          const BTL_POKEPARAM* pp_attacker = m_pokeCon->GetPokeParamConst( param_header->userPokeID );
          bMigawariThrew = scEvent_CheckMigawariThrew( pp_attacker, pp_target, WAZANO_NULL );
        }

        if( scproc_RankEffectCore( param_header->userPokeID, pp_target, param->rankType, param->rankVolume, param->cause,
          BTL_POKEID_NULL, itemID, param->effSerial, param->fAlmost, !(param->fStdMsgDisable), preMsg, bMigawariThrew, !(param->fViewEffectDisable) )
        ){
          if( !param->fMsgPreEffect ){
            handexSub_putString( &param->exStr );
          }
          result = 1;
        }
      }
    }
  }

  if( fEffective && param_header->tokwin_flag && pp_user){
    scPut_TokWin_Out( pp_user );
  }

  return result;
}
/**
 * ポケモンランク強制セット（指定値に強制的に書き換える。失敗しない）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setRank( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_RANK* param = (BTL_HANDEX_PARAM_SET_RANK*)param_header;

  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );
    if( !pp_target->IsDead() )
    {
      pp_target->RankSet( BTL_POKEPARAM::BPP_ATTACK_RANK,     param->attack );
      pp_target->RankSet( BTL_POKEPARAM::BPP_DEFENCE_RANK,    param->defence );
      pp_target->RankSet( BTL_POKEPARAM::BPP_SP_ATTACK_RANK,  param->sp_attack );
      pp_target->RankSet( BTL_POKEPARAM::BPP_SP_DEFENCE_RANK, param->sp_defence );
      pp_target->RankSet( BTL_POKEPARAM::BPP_AGILITY_RANK,    param->agility );
      pp_target->RankSet( BTL_POKEPARAM::BPP_HIT_RATIO,       param->hit_ratio );
      pp_target->RankSet( BTL_POKEPARAM::BPP_AVOID_RATIO,     param->avoid_ratio );
      pp_target->SetCriticalRank( param->critical_rank );

      BTL_PRINT("ひっくりかえしたパラメータ[pokeID=%d] atk=%d, def=%d hit=%d\n", param->pokeID, param->attack, param->defence, param->hit_ratio);

      SCQUE_PUT_OP_RankSet8( m_que,
        param->pokeID, param->attack, param->defence, param->sp_attack, param->sp_defence, param->agility,
                       param->hit_ratio, param->avoid_ratio, param->critical_rank );
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンランクの内、マイナスになっているもののみフラットに戻す
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_recoverRank( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RECOVER_RANK* param = (BTL_HANDEX_PARAM_RECOVER_RANK*)param_header;
  BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );
  if( !pp_target->IsDead() )
  {
    u8 result;
    SCQUE_PUT_OP_RankRecover( m_que, param->pokeID );
    result = pp_target->RankRecover();
    return result;
  }
  return 0;
}
/**
 * ポケモンランクを全てフラットに戻す
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_resetRank( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANK* param = (BTL_HANDEX_PARAM_RESET_RANK*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = m_pokeCon->GetPokeParam( param->pokeID[i] );
    if( !pp_target->IsDead() ){
      SCQUE_PUT_OP_RankReset( m_que, param->pokeID[i] );
      pp_target->RankReset();
      result = 1;
    }
  }
  return result;
}
/**
 * 上昇している能力ランクを全てフラットに戻す( 下降している能力ランクは変化しない )
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_resetRankUp( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANKUP* param = (BTL_HANDEX_PARAM_RESET_RANKUP*)param_header;
  BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );
  if( !pp_target->IsDead() )
  {
    u8 result;
    SCQUE_PUT_OP_RankUpReset( m_que, param->pokeID );
    result = pp_target->RankUpReset();
    return result;
  }
  return 0;
}
/**
 * ポケモン能力値（攻撃、防御等）を指定値に書き換え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setStatus( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_STATUS* param = (BTL_HANDEX_PARAM_SET_STATUS*)param_header;
  BTL_POKEPARAM* bpp;

  if( m_posPoke.IsExist( param->pokeID) )
  {
    bpp = m_pokeCon->GetPokeParam( param->pokeID );
    if( param->fAttackEnable ){
      bpp->SetBaseStatus( BTL_POKEPARAM::BPP_ATTACK, param->attack );
      SCQUE_PUT_OP_SetStatus( m_que, param->pokeID,BTL_POKEPARAM::BPP_ATTACK, param->attack );
    }
    if( param->fDefenceEnable ){
      bpp->SetBaseStatus( BTL_POKEPARAM::BPP_DEFENCE, param->defence );
      SCQUE_PUT_OP_SetStatus( m_que, param->pokeID, BTL_POKEPARAM::BPP_DEFENCE, param->defence );
    }
    if( param->fSpAttackEnable ){
      bpp->SetBaseStatus( BTL_POKEPARAM::BPP_SP_ATTACK, param->sp_attack );
      SCQUE_PUT_OP_SetStatus( m_que, param->pokeID, BTL_POKEPARAM::BPP_SP_ATTACK, param->sp_attack );
    }
    if( param->fSpDefenceEnable ){
      bpp->SetBaseStatus( BTL_POKEPARAM::BPP_SP_DEFENCE, param->sp_defence );
      SCQUE_PUT_OP_SetStatus( m_que, param->pokeID, BTL_POKEPARAM::BPP_SP_DEFENCE, param->sp_defence );
    }
    if( param->fAgilityEnable ){
      bpp->SetBaseStatus( BTL_POKEPARAM::BPP_AGILITY, param->agility );
      SCQUE_PUT_OP_SetStatus( m_que, param->pokeID, BTL_POKEPARAM::BPP_AGILITY, param->agility );
    }

    handexSub_putString( &param->exStr );

    return 1;
  }
  return 0;
}
/**
 * ポケモンを強制的にひん死にさせる
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_kill( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)param_header;

  if( m_posPoke.IsExist(param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = m_pokeCon->GetPokeParam( param->pokeID );
    if( (!pp_target->IsDead())
    ||  (param->fDeadPokeEnable)
    ){
      // 使用ポケ＆ワザが特定できる場合はPGLレコードを記録
      const BTL_POKEPARAM* attacker = NULL;
      if( (param_header->userPokeID != BTL_POKEID_NULL)
      &&  (param->recordWazaID != WAZANO_NULL)
      ){
        attacker = m_pokeCon->GetPokeParamConst( param_header->userPokeID );
      }

      MainModule::PGLRecParam  pgl_rec_param( attacker, param->recordWazaID );

      handexSub_putString( &param->exStr );
      scproc_KillPokemon(pp_target, param_header->userPokeID, param->deadCause, (attacker!=NULL)? &pgl_rec_param : NULL , !param->fDisableDeadProcess );
      return 1;
    }
  }
  return 0;
}

/**
 * ポケモンが瀕死なら、退場処理を行う
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_deadCheck( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DEAD_CHECK* param = (BTL_HANDEX_PARAM_DEAD_CHECK*)param_header;

  if( m_posPoke.IsExist(param->pokeID) )
  {
    BTL_POKEPARAM* poke = m_pokeCon->GetPokeParam( param->pokeID );
    scproc_CheckDeadCmd( poke, false );
    return 1;
  }
  return 0;
}

/**
 * ポケモンタイプ変更
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_changeType( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TYPE* param= (const BTL_HANDEX_PARAM_CHANGE_TYPE*)param_header;

  if( m_posPoke.IsExist(param->pokeID) )
  {
    BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
    if( (!bpp->IsDead() )
    &&  (!tables::IsTypeChangeForbidPoke(bpp->GetMonsNo()))
    ){
      SCQUE_PUT_OP_ChangePokeType( m_que, param->pokeID, param->next_type, param->exTypeCause );
      PokeTypePair  prev_type = bpp->GetPokeType();
      bool bChanged = bpp->ChangePokeType( param->next_type, param->exTypeCause );

      if( bChanged && param_header->tokwin_flag ){
        scPut_TokWin_In( bpp );
      }

      if( !(param->fStdMsgDisable) )
      {
        if( (PokeTypePair_IsPure(param->next_type))
        &&  (param->next_type != prev_type)
        ){
          pml::PokeType type_pure = PokeTypePair_GetType1( param->next_type );
          SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_ChangePokeType, param->pokeID, type_pure );
        }
        else if( param->fPutFailMsg ){
          SCQUE_PUT_MSG_STD( m_que, BTL_STRID_STD_WazaFail );
        }
      }

      if( bChanged && param_header->tokwin_flag ){
        scPut_TokWin_Out( bpp );
      }

      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンタイプ拡張
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_exType( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_EX_TYPE* param= (const BTL_HANDEX_PARAM_EX_TYPE*)param_header;

  if( m_posPoke.IsExist( param->pokeID) )
  {
    BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
    if( (!bpp->IsDead() )
    &&  (!bpp->IsMatchType(param->ex_type))
    ){
      SCQUE_PUT_OP_ExPokeType( m_que, param->pokeID, param->ex_type, param->exTypeCause );
      bpp->ExPokeType( param->ex_type, param->exTypeCause );
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_ExPokeType, param->pokeID, param->ex_type );
      return 1;
    }
  }
  return 0;
}


/**
 * SETメッセージ表示
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_message( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);
  const BTL_POKEPARAM* pp_user = NULL;
  if( param_header->userPokeID != BTL_POKEID_NULL ){
    pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
  }

  if( param_header->tokwin_flag && (pp_user!=NULL) ){
    scPut_TokWin_In( pp_user );
  }

  handexSub_putString( &param->str );

  if( param_header->tokwin_flag && (pp_user!=NULL) ){
    scPut_TokWin_Out( pp_user );
  }

  if( HANDEX_STR_IsFailMsg(&param->str) ){
    m_fWazaFailMsgDisped = true;
  }

  return 1;
}

/**
 * ポケモンターンフラグセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setTurnFlag( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  if( !bpp->IsDead() ){
    bpp->TURNFLAG_Set( param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモンターンフラグリセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_resetTurnFlag( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  if( !bpp->IsDead() ){
    bpp->TURNFLAG_ForceOff( param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモン継続フラグセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setContFlag( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  if( !bpp->IsDead() ){
    scPut_SetContFlag( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモン継続フラグリセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_resetContFlag( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  if( !bpp->IsDead() ){
    scPut_ResetContFlag( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモン永続フラグセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setPermFlag( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_PERMFLAG* param = (const BTL_HANDEX_PARAM_SET_PERMFLAG*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  scPut_SetPermFlag( bpp, param->flag );
  return 1;
}
/**
 * サイドエフェクト追加
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_sideEffectAdd( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SIDEEFF_ADD* param = (BTL_HANDEX_PARAM_SIDEEFF_ADD*)(param_header);
  BTL_SICKCONT cont = param->cont;

  scEvent_CheckSideEffectParam( param_header->userPokeID, param->effect, param->side, &cont );

  // 操作対象の陣営を展開する
  BtlSide targetSide[ BTL_SIDE_NUM ];
  u8 targetSideNum = 0;
  m_mainModule->ExpandSide( targetSide, &targetSideNum, param->side );

  // 展開した全ての陣営に対して、サイドエフェクトを追加する
  u8 result = 0;
  for( u8 i=0; i<targetSideNum; ++i )
  {
    if( scPut_SideEffect_Add( targetSide[i], param->effect, cont) )
    {
      // @fix NMCat[1910] バトルロイヤルでまきびし系統の技を使用した際の不具合
      // サイドエフェクトのメッセージは、args[0] に対象陣営IDが登録されていて、
      // str::MakeStringStdWithArgArray で、「みかたの～」と「あいての～」を出し分けています。
      // args[0] が複数の陣営を表す陣営IDであった場合、
      // バトルロイヤルでは、str::MakeStringStdWithArgArray の段階で、どの陣営に対するメッセージなのかの判断が出来ません。
      // ( よって、すべての陣営に対するメッセージが「みかたの～」と表示されてしまいます )
      // そのため、
      // サイドエフェクトのメッセージの args[0] には対象陣営IDが登録されている！という前提で、
      // args[0] を展開後の対象陣営に差し替えます。
      if( param->fReplaceExStrArgs0ByExpandSide )
      {
        param->exStr.args[0] = targetSide[i];
      }

      handexSub_putString( &param->exStr );
      result = 1;
    }
  }
  return result;
}
/**
 * サイドエフェクトハンドラ消去
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_sideEffectRemove( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = (const BTL_HANDEX_PARAM_SIDEEFF_REMOVE*)(param_header);
  u8 result = 0;

  // 操作対象の陣営を展開する
  BtlSide targetSide[ BTL_SIDE_NUM ];
  u8 targetSideNum = 0;
  m_mainModule->ExpandSide( targetSide, &targetSideNum, param->side );

  // 展開した全ての陣営に対して、サイドエフェクトを削除する
  for( u8 i=0; i<targetSideNum; ++i )
  {
    if( scproc_HandEx_sideEffectRemoveCore( targetSide[i], param->flags ) )
    {
      result = 1;
    }
  }

  return result;
}

/**
 * @brief サイドエフェクトハンドラ消去
 * @param side               サイドエフェクトを消去する陣営
 * @param sideEffectBitFlag  消去するサイドエフェクトを表すビットフラグ
 * @retval true   サイドエフェクトを消去した
 * @retval false  サイドエフェクトを消去しなかった
 */
bool  ServerFlow::scproc_HandEx_sideEffectRemoveCore( BtlSide side, const u8* sideEffectBitFlag )
{
  bool removed = false;

  for(u32 i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    if( calc::BITFLG_Check( sideEffectBitFlag, i ) )
    {
      BtlSideEffect effectType = static_cast<BtlSideEffect>(i);
      if( scPut_SideEffect_Remove( side, effectType ) )
      {
        scPut_SideEffectOffMsg( side, effectType );
        removed = true;
      }
    }
  }

  return removed;
}

/**
 * サイドエフェクト一時停止・再開処理
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_sideEffectSleep( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_SLEEP* param = (const BTL_HANDEX_PARAM_SIDEEFF_SLEEP*)(param_header);
  u8 result = 0;

  // 操作対象の陣営を展開する
  BtlSide targetSide[ BTL_SIDE_NUM ];
  u8 targetSideNum = 0;
  m_mainModule->ExpandSide( targetSide, &targetSideNum, param->side );

  // 展開した全ての陣営に対して、サイドエフェクトを操作する
  for( u8 i=0; i<targetSideNum; ++i )
  {
    if( scproc_HandEx_sideEffectSleepCore( targetSide[i], param->flags, param->bWeak ) )
    {
      result = 1;
    }
  }

  return result;

}

/**
 * @brief サイドエフェクト一時停止・再開処理
 * @param side               サイドエフェクトを消去する陣営
 * @param sideEffectBitFlag  消去するサイドエフェクトを表すビットフラグ
 * @param wakeUpFlag         true:再開, false:一時停止
 * @retval true   サイドエフェクトを一時停止・再開した
 * @retval false  サイドエフェクトを一時停止・再開しなかった
 */
bool  ServerFlow::scproc_HandEx_sideEffectSleepCore( BtlSide side, const u8* sideEffectBitFlag, bool wakeUpFlag )
{
  bool result = 0;

  for(u32 i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    if( calc::BITFLG_Check( sideEffectBitFlag, i ) )
    {
      BtlSideEffect effectType = static_cast<BtlSideEffect>(i);
      bool  bEffective = false;
      if( wakeUpFlag ){
        bEffective = handler::side::Weak( side, effectType );
      }else{
        bEffective = handler::side::Sleep( side, effectType );
      }
      if( bEffective ){
        result = true;
      }
    }
  }
  return result;
}

/**
 * フィールドエフェクトハンドラ追加
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_addFieldEffect( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ADD_FLDEFF* param = (BTL_HANDEX_PARAM_ADD_FLDEFF*)(param_header);
  const BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );

  bool fSucceed;

  if( param_header->tokwin_flag )
  {
    scPut_TokWin_In( pp_user );
  }

  if( param->effect != FieldStatus::EFF_GROUND )
  {
    fSucceed = scproc_FieldEffectCore( param->effect, param->cont, param->fAddDependPoke );
  }
  else
  {
    scEvent_ChangeGroundBefore( param_header->userPokeID, param->ground, &(param->cont) );

    fSucceed = scproc_ChangeGround( param_header->userPokeID, param->ground, param->cont );
    if( fSucceed ){
      SCQUE_PUT_ACT_MsgWinHide( m_que, 0 );
      SCQUE_PUT_ACT_EffectField( m_que, param->ground );
    }
  }

  if( param_header->tokwin_flag )
  {
    scPut_TokWin_Out( pp_user );
  }

  if( fSucceed )
  {
    handexSub_putString( &param->exStr );
  }

  if( ( param->effect == FieldStatus::EFF_GROUND ) &&
      ( fSucceed ) )
  {
    scEvent_ChangeGroundAfter( param_header->userPokeID, param->ground );
  }

  return fSucceed;
}
/**
 * フィールドエフェクトハンドラ削除
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_removeFieldEffect( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_REMOVE_FLDEFF* param = (const BTL_HANDEX_PARAM_REMOVE_FLDEFF*)(param_header);

  if( m_fieldStatus->RemoveEffect(param->effect) ){
    scproc_FieldEff_End( param->effect );
    return 1;
  }
  return 0;
}
/**
 * 天候変化
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_changeWeather( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_WEATHER* param = (const BTL_HANDEX_PARAM_CHANGE_WEATHER*)(param_header);
  const BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );
  bool result = false;

  if( param->weather != BTL_WEATHER_NONE )
  {
    //@バトル変更箇所 天候変化ブロック。特性の処理位置
    const ChangeWeatherResult result_weather = scproc_ChangeWeatherCheck( param->weather, param->turn);
    if( result_weather == CHANGE_WEATHER_OK )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( pp_user );
      }
      scproc_ChangeWeatherCore( param->weather, param->turn, param->turnUpCount, param_header->userPokeID );
      handexSub_putString( &param->exStr );
      result = true;
      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( pp_user );
      }
    }
    else if( result_weather == CHANGE_WEATHER_FAIL_SANGO_WEATHER )
    {
      //処理を入れないと何も出ないで終わってしまう
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( pp_user );
      }
      //@バトル変更箇所 sango特殊天候失敗処理。 技の方にも同じ処理がある。
      BTL_HANDEX_STR_PARAMS strParam;
      HANDEX_STR_Clear(&strParam);//Coverity[10975]初期化忘れ
      switch( m_fieldStatus->GetWeather() )
      {
        case BTL_WEATHER_STORM:    ///< おおあめ
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_RainStormFail_Weather );
          break;
        case BTL_WEATHER_DAY:      ///< おおひでり
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_DayFail_Weather );
          break;
        case BTL_WEATHER_TURBULENCE:  ///< 乱気流
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_TurbulenceFail_Weather );
          break;
        default:
          GFL_ASSERT_MSG(0,"新天候じゃないのに来てる。[%d]\n",m_fieldStatus->GetWeather());
          HANDEX_STR_Setup( &strParam , BTL_STRTYPE_STD, BTL_STRID_STD_WazaFail );  //保険
          break;
      }
      handexSub_putString( &strParam );
      
      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( pp_user );
      }
    }
  }
  // エアロック
  else if( param->fAirLock )
  {
    if( param_header->tokwin_flag ){
      scPut_TokWin_In( pp_user );
    }

    handexSub_putString( &param->exStr );
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_NotifyAirLock();
      m_HEManager.PopState( hem_state );
    }

    result = true;

    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( pp_user );
    }
  }
  //@バトル変更箇所 sango新規天候の特性持ち主退場時の処理
  else
  {
    BtlWeather weather = m_fieldStatus->GetWeather();
    m_fieldStatus->EndWeather();
    SCQUE_PUT_ACTOP_WeatherEnd( m_que, weather );
    scproc_ChangeWeatherAfter( BTL_WEATHER_NONE );

    BtlWeather defaultWeather = m_mainModule->GetDefaultWeather();
    if( defaultWeather != BTL_WEATHER_NONE )
    {
      //@バトル変更箇所 ここには戻り値があるが、sango特殊天候上書きは起きないため無視
      scproc_ChangeWeather( defaultWeather, BTL_WEATHER_TURN_PERMANENT, 0, BTL_POKEID_NULL );
    }
  }

  return result;
}
//----------------------------------------------------------------------------------
// 文字出力共通処理
//----------------------------------------------------------------------------------
bool ServerFlow::handexSub_putString( const BTL_HANDEX_STR_PARAMS* strParam )
{
  if( strParam->fSEAdd == false )
  {
    switch( strParam->type ){
    case BTL_STRTYPE_STD:
      scPut_Message_StdEx( strParam->ID, strParam->argCnt, strParam->args );
      return true;

    case BTL_STRTYPE_SET:
      scPut_Message_SetEx( strParam->ID, strParam->argCnt, strParam->args );
      return true;
    }
  }
  else
  {
    u32 SENo = (u32)(strParam->args[ BTL_STR_ARG_MAX - 1 ]);
    switch( strParam->type ){
    case BTL_STRTYPE_STD:
      scPut_Message_StdSE( strParam->ID, SENo, strParam->argCnt, strParam->args );
      return true;

    case BTL_STRTYPE_SET:
      scPut_Message_SetSE( strParam->ID, SENo, strParam->argCnt, strParam->args );
      return true;
    }
  }
  return false;
}
/**
 * 位置エフェクトハンドラ追加
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_PosEffAdd( const BTL_HANDEX_PARAM_HEADER* param_header )
{  
  s32 factorParam[ POSEFF_PARAM_MAX ];
  u8  factorParamNum = 0;
  scproc_HandEx_GetPosEffectFactorParam( factorParam, &factorParamNum, param_header );

  const BTL_HANDEX_PARAM_POSEFF_ADD* param     = (const BTL_HANDEX_PARAM_POSEFF_ADD*)( param_header );
  const BtlPokePos                   targetPos = static_cast<BtlPokePos>( param->pos );
  const BtlPosEffect                 posEffect = static_cast<BtlPosEffect>( param->effect );
  if( scproc_AddPosEffect( targetPos, posEffect, param->effectParam, param_header->userPokeID, factorParam, factorParamNum ) )
  {
    return 1;
  }

  return 0;
}

/**
 * @brief BTL_HANDEX_PARAM_HEADER　から、位置エフェクトのイベントファクタに渡すパラメータを取得する
 * @param[out] factorParam        位置エフェクトのイベントファクタに渡すパラメータの格納先
 * @param[out] factorParamNum     位置エフェクトのイベントファクタに渡すパラメータの数
 * @param      handExParamHeader  
 */
void ServerFlow::scproc_HandEx_GetPosEffectFactorParam(
  s32*                           factorParam,
  u8*                            factorParamNum, 
  const BTL_HANDEX_PARAM_HEADER* handExParamHeader ) const
{
  const BTL_HANDEX_PARAM_POSEFF_ADD* param           = (const BTL_HANDEX_PARAM_POSEFF_ADD*)( handExParamHeader );
  BtlPosEffect                       posEffect       = static_cast<BtlPosEffect>( param->effect );
  const PosEffect::EffectParam&      effectParam     = param->effectParam;
  PosEffect::EffectParamType         effectParamType = PosEffect::GetEffectParamType( posEffect );

  switch( effectParamType )
  {
  case PosEffect::PARAM_TYPE_DELAY_ATTACK:
    factorParam[0] = this->Hnd_GetTurnCount() + effectParam.DelayAttack.execTurnMax;
    factorParam[1] = effectParam.DelayAttack.wazaNo;
    factorParam[2] = this->Hnd_PokeIDtoPokePos( handExParamHeader->userPokeID );
    *factorParamNum = 3;
    break;

  case PosEffect::PARAM_TYPE_DELAY_RECOVER:
    factorParam[0] = param->fSkipHpRecoverSpFailCheck;
    *factorParamNum = 1;
    break;

  default:
    *factorParamNum = 0;
    break;
  }
}


/**
 * ポケモンとくせい書き換え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_tokuseiChange( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = (const BTL_HANDEX_PARAM_CHANGE_TOKUSEI*)(param_header);

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
  TokuseiNo prevTokusei = (TokuseiNo)(bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

  if( tables::IsNeverChangeTokusei(prevTokusei) ){
    return 0;
  }

  if( bpp->IsDead() ){
    return 0;
  }

  if( !((param->fSameTokEffective) || ( param->tokuseiID != prevTokusei )) ){
    return 0;
  }

  u8 result = 0;
  bool enable = scEvent_CheckTokuseiChangeEnable( param->pokeID, static_cast<TokuseiNo>( param->tokuseiID ), param->cause );

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_IN( m_que, param_header->userPokeID );
  }

  if( enable )
  {
    SCQUE_PUT_ACTOP_ChangeTokusei( m_que, param->pokeID, param->tokuseiID );
    handexSub_putString( &param->exStr );

    // とくせい書き換え直前イベント
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_ChangeTokuseiBefore(  param->pokeID, prevTokusei, param->tokuseiID );
      m_HEManager.PopState( hem_state );

    }

    handler::tokusei::Remove( bpp );
    bpp->ChangeTokusei( static_cast<TokuseiNo>(param->tokuseiID) );
    SCQUE_PUT_OP_ChangeTokusei( m_que, param->pokeID, param->tokuseiID );
    handler::tokusei::Add( bpp );

    SCQUE_PUT_TOKWIN_OUT( m_que, param->pokeID );

    // とくせい書き換え完了イベント
    if( prevTokusei != param->tokuseiID )
    {
      u32 hem_state = m_HEManager.PushState();
      scEvent_ChangeTokuseiAfter(  param->pokeID );
      m_HEManager.PopState( hem_state );
    }

    result = 1;
  }
  else
  {
    scEvent_TokuseiChangeFailed( param->pokeID, static_cast<TokuseiNo>( param->tokuseiID ), param->cause );
    result = 0;
  }

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_OUT( m_que, param_header->userPokeID );
  }

  if( enable )
  {
    scproc_AfterTokuseiChanged_Item( bpp, prevTokusei, static_cast<TokuseiNo>(param->tokuseiID) );
  }

  return result;
}

/**
 * @brief [Event] 特性の変更が可能かチェック
 * @param targetPokeID  変更対象ポケモンのID
 * @param nextTokusei   変更後の特性
 * @param cause         変更の原因
 * @retval true  特性を変更して良い
 * @retval false 特性を変更してはいけない
 */
bool ServerFlow::scEvent_CheckTokuseiChangeEnable( u8 targetPokeID, TokuseiNo nextTokusei, TokuseiChangeCause cause )
{
  bool fFail = false;
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, targetPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_NEXT, nextTokusei );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_CHANGE_CAUSE, cause );
    EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    EVENT_CallHandlers( this, BTL_EVENT_TOKUSEI_CHANGE_CHECKFAIL );
    fFail = EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  EVENTVAR_Pop();
  return !fFail;
}

/**
 * @brief [Event] 特性の変更が失敗した
 * @param targetPokeID  変更対象ポケモンのID
 * @param nextTokusei   変更後の特性
 * @param cause         変更の原因
 * @retval true  特性を変更して良い
 * @retval false 特性を変更してはいけない
 */
void ServerFlow::scEvent_TokuseiChangeFailed( u8 targetPokeID, TokuseiNo nextTokusei, TokuseiChangeCause cause )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, targetPokeID );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_NEXT, nextTokusei );
    EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_CHANGE_CAUSE, cause );
    EVENT_CallHandlers( this, BTL_EVENT_TOKUSEI_CHANGE_FAILED );
  EVENTVAR_Pop();
}


/**
 * ポケモン装備アイテム書き換え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setItem( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_ITEM* param = (const BTL_HANDEX_PARAM_SET_ITEM*)(param_header);

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  // 自分自身へのリクエストでなければアイテムセットの失敗チェック
  if( param_header->userPokeID != param->pokeID )
  {
    u32 hem_state = m_HEManager.PushState();
    u8  failed = scEvent_CheckItemSet( bpp, param->itemID );
    m_HEManager.PopState( hem_state );

    if( failed )
    {
      hem_state = m_HEManager.PushState();
      scEvent_ItemSetFailed( bpp );
      m_HEManager.PopState( hem_state );
      return 0;
    }
  }

  // ここまで来たら成功
  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_IN( m_que, param_header->userPokeID );
  }

  {
    u16  defaultItemID = bpp->GetItem();
    handexSub_putString( &param->exStr );

    // 消費情報をクリア
    if( param->fClearConsume )
    {
      bpp->ClearConsumedItem();
      SCQUE_PUT_OP_ClearConsumedItem( m_que, param->pokeID );
    }

    scproc_ItemChange( bpp, param->itemID, false );
    if( (param->itemID == ITEM_DUMMY_DATA)
    &&  param->fCallConsumedEvent
    ){
      scproc_CallAfterItemEquipEvent( bpp, defaultItemID, true );
    }
  }

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_OUT( m_que, param_header->userPokeID );
  }

  if( param->fClearConsumeOtherPoke )
  {
    BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->clearConsumePokeID );
    target->ClearConsumedItem();
    SCQUE_PUT_OP_ClearConsumedItem( m_que, param->clearConsumePokeID );
  }

  scproc_CheckItemReaction( bpp, BTL_ITEMREACTION_FULL );
  return 1;
}
/**
 * ポケモン装備アイテム交換
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_swapItem( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SWAP_ITEM* param = (const BTL_HANDEX_PARAM_SWAP_ITEM*)(param_header);

  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
  BTL_POKEPARAM* self = m_pokeCon->GetPokeParam( param_header->userPokeID );
  u16 selfItem = self->GetItem();
  u16 targetItem = target->GetItem();

  BTL_PRINT("[svf]アイテム交換処理 対象ポケID: %d <->%d\n", param_header->userPokeID, param->pokeID );

  // 対象の能力で失敗するケースをチェック
  {
    u32 hem_state = m_HEManager.PushState();
    u8  failed = scEvent_CheckItemSet( target, selfItem );
    m_HEManager.PopState( hem_state );
    if( failed )
    {
      hem_state = m_HEManager.PushState();
      scEvent_ItemSetFailed( target );
      m_HEManager.PopState( hem_state );
      return 0;
    }
  }

  // ここまで来たら成功
  if( param_header->tokwin_flag ){
    scPut_TokWin_In( self );
  }
  handexSub_putString( &param->exStr );
  handexSub_putString( &param->exSubStr1 );
  handexSub_putString( &param->exSubStr2 );
  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( self );
  }

  {
    scproc_ItemChange( self, targetItem, false );
    scproc_ItemChange( target, selfItem, false );
  }

  scproc_CheckItemReaction( self, BTL_ITEMREACTION_GEN );
  scproc_CheckItemReaction( target, BTL_ITEMREACTION_GEN );

  // 野生ポケモンから道具を奪った回数をカウントする
  if( ( param->fIncRecordCount_StealItemFromWildPoke ) &&
      ( targetItem != ITEM_DUMMY_DATA ) &&
      ( m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD ) )
  {
    const u8 clientID = MainModule::PokeIDtoClientID( param->pokeID );
    if( clientID == BTL_CLIENT_ENEMY1 )
    {
      RecordSave::IncStealItemFromWildPoke( *m_mainModule );
    }
  }

  return 1;
}
/**
 * ポケモン装備アイテム発動チェック
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_CheckItemEquip( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP* param = (const BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP*)(param_header);
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  scproc_CheckItemReaction( bpp, param->reactionType );
  return 1;
}
/**
 * 装備アイテム使用
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_useItem( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_USE_ITEM* param = (const BTL_HANDEX_PARAM_USE_ITEM*)(param_header);
  BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );

  if( !pp_user->IsDead() || param->fUseDead )
  {
    if( param->fSkipHPFull && pp_user->IsHPFull() ){
      return 0;
    }

    if( scproc_UseItemEquip(pp_user) ){
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンに、指定アイテムの効果を強制発動
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_ItemSP( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ITEM_SP* param = (BTL_HANDEX_PARAM_ITEM_SP*)param_header;
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
  if( !bpp->IsDead() )
  {
    BTL_EVENT_FACTOR* factor = handler::item::TMP_Add( bpp, param->itemID );
    if( factor )
    {
      u32  hem_state;
      bool bEffective = false;
      u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos( m_que, SC_ACT_USE_ITEM );

      hem_state = m_HEManager.PushState();
        scEvent_ItemEquipTmp(  bpp, param_header->userPokeID );
        if( scproc_HandEx_Result() == HandExResult_Enable )
        {
          bEffective = true;
          u8 bNuts = ::item::ITEM_CheckNuts( param->itemID );
          scmd::SCQUE_PUT_ReservedPos( m_que, que_reserve_pos, SC_ACT_USE_ITEM, param->pokeID, bNuts );
        }

      m_HEManager.PopState( hem_state );
      handler::item::TMP_Remove( factor );

      if( param->bAteKinomi
      &&  ::item::ITEM_CheckNuts( param->itemID )
      ){
        scPut_SetPermFlag( bpp, BTL_POKEPARAM::PERMFLAG_ATE_KINOMI );
      }
      if( bEffective ){
        scproc_AfterItemEquip( bpp, param->itemID, true );
      }

      return 1;
    }
  }
  return 0;
}
/**
 * ポケモン自分のアイテム消費＆メッセージ表示
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_consumeItem( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CONSUME_ITEM* param = (const BTL_HANDEX_PARAM_CONSUME_ITEM*)param_header;
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param_header->userPokeID );

  u16 itemID = bpp->GetItem();

  bool bAction = (param->fNoAction == false);
  if( bAction )
  {
    scPut_UseItemAct( bpp );
  }
  handexSub_putString( &param->exStr );
  scproc_ItemChange( bpp, ITEM_DUMMY_DATA, true );
  BTL_PRINT("アイテム消費処理, pokeID=%d, bAteKinomi=%d\n", param_header->userPokeID, !(param->fNoAteKinomi));
  scproc_AfterItemEquip( bpp, itemID, !(param->fNoAteKinomi) );

  return 1;
}
/**
 * ポケモンわざ書き換え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_updateWaza( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_UPDATE_WAZA* param = (const BTL_HANDEX_PARAM_UPDATE_WAZA*)(param_header);

  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
  SCQUE_PUT_OP_UpdateWaza( m_que, param->pokeID, param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  target->WAZA_UpdateID( param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  return 1;
}
/**
 * ポケモンカウンタ値書き換え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_counter( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_COUNTER* param = (const BTL_HANDEX_PARAM_COUNTER*)(param_header);

  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
  scPut_SetBppCounter( target, static_cast<BTL_POKEPARAM::Counter>(param->counterID), param->value );
  return 1;
}
/**
 * 時間差ワザダメージ
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_delayWazaDamage( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_DELAY_WAZADMG* param = (const BTL_HANDEX_PARAM_DELAY_WAZADMG*)param_header;

  BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( param->attackerPokeID );
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->targetPokeID );

  WAZAPARAM   wazaParam;
  WAZAEFF_CTRL  ctrlBackup;
  u8 result;

  scEvent_GetWazaParam( param->wazaID, attacker, &wazaParam );

  // ワザ対象をワークに取得
  m_psetDamaged->Clear();
  m_psetTmp->Clear();
  m_psetTmp->Add( target );
  m_psetTmp->SetDefaultTargetCount( 1 );

  // 場にいないポケモンには当たらない（補正なし）
  m_psetTmp->RemoveDisablePoke( m_posPoke );
  if( m_psetTmp->IsRemovedAll() ){
    scPut_WazaFail( attacker, wazaParam.wazaID );
    return 0;
  }

  // 対象ごとの無効チェック＆回避チェック
  flowsub_CheckTypeAffinity( &wazaParam, attacker, m_psetTmp, &m_dmgAffRec );
  flowsub_CheckPokeHideAvoid( &wazaParam, attacker, m_psetTmp );
  flowsub_CheckNotEffect_Guard( &wazaParam, attacker, m_psetTmp, m_dmgAffRec, NULL );
  flowsub_CheckNotEffect_Tokusei( &wazaParam, attacker, m_psetTmp, m_dmgAffRec, NULL );
  flowsub_CheckNoEffect_TypeAffinity( &wazaParam, attacker, m_psetTmp, m_dmgAffRec, NULL );
  flowsub_CheckNotEffect_Affinity( &wazaParam, attacker, m_psetTmp, m_dmgAffRec );
  flowsub_checkWazaAvoid( &wazaParam, attacker, m_psetTmp, NULL );
  // 最初は居たターゲットが残っていない -> 何もせず終了
  if( m_psetTmp->IsRemovedAll() ){
    return 0;
  }

  // ワザエフェクト管理のバックアップを取り、システム初期化
  ctrlBackup = *(m_wazaEffCtrl);
  wazaEffCtrl_Init( m_wazaEffCtrl );
  wazaEffCtrl_Setup( m_wazaEffCtrl, attacker, m_psetTmp );
  wazaEffCtrl_ChangeAtkPos( m_wazaEffCtrl, param->attackerPos );
  wazaEffCtrl_SetEffectIndex( m_wazaEffCtrl, BTLV_WAZAEFF_DELAY_ATTACK );

  {
    ActionDesc actionDesc;
    ActionDesc_Clear( &actionDesc );
    scproc_Fight_Damage_Root( &actionDesc, &wazaParam, attacker, m_psetTmp, m_dmgAffRec, true );
  }

  // ワザ効果あり確定→演出表示コマンド生成などへ
  result = wazaEffCtrl_IsEnable( m_wazaEffCtrl );

  *(m_wazaEffCtrl) = ctrlBackup;
  return result;
}
/**
 * バトル離脱
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_quitBattle( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  u8 result = 0;
  BTL_HANDEX_PARAM_QUIT_BATTLE* param = (BTL_HANDEX_PARAM_QUIT_BATTLE*)param_header;
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param_header->userPokeID );

  // @fix NMCat[314] 技「フリーフォール」中に特性「にげごし」「ききかいひ」が発動する
  // フリーフォール中なら、離脱できない
  if( Hnd_IsFreeFallPoke( param_header->userPokeID ) )
  {
    return 0;
  }

  if( param_header->tokwin_flag )
  {
    scPut_TokWin_In( bpp );
  }

  if( scproc_NigeruCmdSub(bpp, true, param->fForceNigeru) )
  {
    m_flowResult = RESULT_BTL_QUIT_ESCAPE;
    m_fQuitBattleRequest = true;
    result = 1;
  }

  if( param_header->tokwin_flag )
  {
    scPut_TokWin_Out( bpp );
  }

  return result;
}
/**
 * メンバー入れ替え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_changeMember( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  u8 result = 0;
  BTL_HANDEX_PARAM_CHANGE_MEMBER* param = (BTL_HANDEX_PARAM_CHANGE_MEMBER*)param_header;
  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );

  // @fix NMCat[75] ターンエンド処理で複数の「ききかいひ」「にげごし」の発動条件を満たしても、1匹しか特性効果が発動しない
  // 対処：複数の「ききかいひ」が同時に発動するように、(m_flowResult == RESULT_POKE_CHANGE)　の時も、入れ替えが発生するように修正
  if( (!scproc_CheckShowdown() || !isSkipBattleAfterShowdown())
  &&  (!Hnd_IsFreeFallPoke(param->pokeID))
  &&  ( (m_flowResult == RESULT_DEFAULT) || (m_flowResult == RESULT_POKE_CHANGE) )
  ){
    if( param_header->tokwin_flag ){
      scPut_TokWin_In( bpp );
    }
    
    handexSub_putString( &param->preStr );
    if( scproc_MemberOutForChange(bpp, param->fIntrDisable) )
    {
      BtlPokePos pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, param->pokeID );

      m_server->RequestChangePokemon( pos );
      handexSub_putString( &param->exStr );
      m_flowResult = RESULT_POKE_CHANGE;
      *m_fMemberChangeReserve = true;
      result = 1;
    }

    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( bpp );
    }
  }

  return result;
}
/**
 * バトンタッチ
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_batonTouch( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_BATONTOUCH* param = (BTL_HANDEX_PARAM_BATONTOUCH*)param_header;

  BTL_POKEPARAM* user = m_pokeCon->GetPokeParam( param->userPokeID );
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->targetPokeID );

  if( user->CheckSick( pml::wazadata::WAZASICK_IEKI) )
  {
    scEvent_IekiFixed( target );
  }

  target->CopyBatonTouchParams( user );
  SCQUE_PUT_OP_BatonTouch( m_que, param->userPokeID, param->targetPokeID );
  if( target->MIGAWARI_IsExist() )
  {
    BtlPokePos pos = m_posPoke.GetPokeExistPos( param->targetPokeID );
    SCQUE_PUT_ACT_MigawariCreate( m_que, pos );
  }
  return 1;
}
/**
 * ひるませる
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_addShrink( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ADD_SHRINK* param = (BTL_HANDEX_PARAM_ADD_SHRINK*)param_header;

  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
  if( scproc_AddShrinkCore( target, param->per) ){
    return 1;
  }
  return 0;
}
/**
 * 瀕死から回復
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_relive( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RELIVE* param = (BTL_HANDEX_PARAM_RELIVE*)param_header;
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );

  target->HpPlus( param->recoverHP );
  target->ClearDeadCause();
  SCQUE_PUT_OP_HpPlus( m_que, param->pokeID, param->recoverHP );
  m_deadRec.Relive( param->pokeID );
  handexSub_putString( &param->exStr );

  {
    BtlPokePos targetPos = m_mainModule->PokeIDtoPokePos( m_pokeCon, param->pokeID );
    if( targetPos != BTL_POS_NULL )
    {
      u8 clientID = MainModule::PokeIDtoClientID( param->pokeID );
      u8 posIdx = m_mainModule->BtlPosToPosIdx( targetPos );
      scproc_MemberInForCover( clientID, posIdx, posIdx, true );
      scproc_AfterMemberIn( );

    }
  }

  return 1;
}
/**
 * 体重をセット
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setWeight( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_WEIGHT* param = (BTL_HANDEX_PARAM_SET_WEIGHT*)param_header;
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );

  target->SetWeight( param->weight );
  SCQUE_PUT_OP_SetWeight( m_que, param->pokeID, param->weight );
  BTL_PRINT("ポケ %d の体重を %d にセット\n", param->pokeID, param->weight);
  handexSub_putString( &param->exStr );
  return 1;
}
/**
 * 場から吹き飛ばす
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_pushOut( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_PUSHOUT* param = (BTL_HANDEX_PARAM_PUSHOUT*)param_header;
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam( param->pokeID );
  BTL_POKEPARAM* attacker = m_pokeCon->GetPokeParam( param_header->userPokeID );

  bool fFailMsgDisped;

  if( scproc_PushOutCore( attacker, target, param->fForceChange, &fFailMsgDisped,
        param->effectNo, param->fIgnoreLevel, &param->exStr) )
  {
    return 1;
  }
  return 0;
}
/**
 * 指定ポケモンの行動を割り込みさせる
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_intrPoke( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_POKE* param = (BTL_HANDEX_PARAM_INTR_POKE*)param_header;

  if( ActOrder_IntrReserve(param->pokeID) ){
    handexSub_putString( &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * 指定ワザ使用ポケモンの行動を割り込みさせる
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_intrWaza( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_WAZA* param = (BTL_HANDEX_PARAM_INTR_WAZA*)param_header;

  if( ActOrder_IntrReserveByWaza(param->waza) ){
    return 1;
  }
  return 0;
}
/**
 * 指定ポケモンの行動順を最後に回す
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_sendLast( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SEND_LAST* param = (BTL_HANDEX_PARAM_SEND_LAST*)param_header;

  if( ActOrder_SendLast(param->pokeID) ){
    handexSub_putString( &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * 指定ポケモン同士の場所入れ替え
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_swapPoke( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SWAP_POKE* param = (BTL_HANDEX_PARAM_SWAP_POKE*)param_header;

  if( param->pokeID1 != param->pokeID2 )
  {
    u8 clientID = MainModule::PokeIDtoClientID( param->pokeID1 );
    if( clientID == MainModule::PokeIDtoClientID(param->pokeID2) )
    {
      const BTL_POKEPARAM* bpp1 = m_pokeCon->GetPokeParam( param->pokeID1 );
      const BTL_POKEPARAM* bpp2 = m_pokeCon->GetPokeParam( param->pokeID2 );
      if( !bpp1->IsDead() && !bpp2->IsDead() )
      {
        const BTL_PARTY* party = m_pokeCon->GetPartyData( clientID );
        s32 posIdx1 = party->FindMember(  bpp1 );
        s32 posIdx2 = party->FindMember( bpp2 );
        if( (posIdx1 >= 0) && (posIdx2 >= 0) )
        {
          scproc_MoveCore( clientID, static_cast<u8>(posIdx1), static_cast<u8>(posIdx2), false );
          handexSub_putString( &param->exStr );
          scproc_AfterMove( clientID, static_cast<u8>(posIdx1), static_cast<u8>(posIdx2) );
          return 1;
        }
      }
    }
  }
  return 0;
}

//@バトル変更箇所
//新規天候特性持ちが変身するときに天気を取り消すチェック
//既存のイベントは怖いから新規で作る
void ServerFlow::scEvent_ChangePokeBefore( BTL_POKEPARAM* bpp )
{
  EVENTVAR_Push();
    EVENTVAR_SetConstValue( BTL_EVAR_POKEID, bpp->GetID() );
    EVENT_CallHandlers( this, BTL_EVENT_CHANGE_POKE_BEFORE );
  EVENTVAR_Pop();
}

/**
 * ヘッダポケモンが指定ポケモンに変身
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_hensin( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_HENSIN* param = (BTL_HANDEX_PARAM_HENSIN*)param_header;

  BTL_POKEPARAM* user = m_pokeCon->GetPokeParam(  param_header->userPokeID );
  BTL_POKEPARAM* target = m_pokeCon->GetPokeParam(  param->pokeID );

  if( (!user->IsFakeEnable())
  &&  (!target->IsFakeEnable())
  &&  (!user->IsWazaHide())
  ){
    TokuseiNo prevTokusei = (TokuseiNo)(user->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));
    if( user->HENSIN_Set(target) )
    {
      u8 usrPokeID = user->GetID();
      u8 tgtPokeID = target->GetID();
      
      //@バトル変更箇所(SBTS[1617])
      scEvent_ChangePokeBefore(user);
      
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( user );
      }

      handler::waza::RemoveForceAll( user );
      handler::tokusei::Remove( user );
      handler::tokusei::Add( user );
      SCQUE_PUT_ACT_Hensin( m_que, usrPokeID, tgtPokeID );

      handexSub_putString( &param->exStr );
      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( user );
      }

      TokuseiNo  nextTokusei = (TokuseiNo)(user->GetValue( BTL_POKEPARAM::BPP_TOKUSEI_EFFECTIVE));
      if( prevTokusei != nextTokusei )
      {
        scproc_AfterTokuseiChanged_Event( user );
        scproc_AfterTokuseiChanged_Item( user, prevTokusei, nextTokusei );
      }
      return 1;
    }
  }
  return 0;
}
/**
 * イリュージョン解除
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_fakeBreak( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_FAKE_BREAK* param = (BTL_HANDEX_PARAM_FAKE_BREAK*)param_header;

  if( m_posPoke.IsExist(param->pokeID) )
  {
    BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
    return scproc_FakeBreak( bpp, &param->exStr );
  }
  return 0;
}

/**
 * @brief イリュージョンを解除する
 * @param[in,out] poke          イリュージョンを解除するポケモン
 * @param[in]     breakMessage  イリュージョンを解除した場合に表示する特殊メッセージ( NULL ならデフォルトメッセージが表示されます )
 * @retval true    イリュージョンを解除した
 * @retval false   イリュージョンを解除しなかった
 */
bool ServerFlow::scproc_FakeBreak( BTL_POKEPARAM* poke, const BTL_HANDEX_STR_PARAMS* breakMessage )
{
  if( poke->IsFakeEnable() )
  {
    poke->FakeDisable();
    SCQUE_PUT_OP_PublishClientInformation_AppearedPokemon( m_que, poke->GetID() );
    SCQUE_PUT_ACT_FakeDisable( m_que, poke->GetID() );

    if( ( breakMessage != NULL ) &&
        ( breakMessage->type != BTL_STRTYPE_NULL ) )
    {
      handexSub_putString( breakMessage );
    }
    else
    {
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_Illusion_Break, poke->GetID() );
    }

    return true;
  }

  return false;
}

void ServerFlow::scproc_RecoverZenryoku(void)
{
  const BtlCompetitor competitor = m_mainModule->GetCompetitor();
  // 通信している、または既に回復済み、バトル施設、ロトム有効度３未満なら、何もしない
  if (m_mainModule->GetCommMode() != BTL_COMM_NONE || 
      m_isRecoverZenryoku ||
      competitor == BTL_COMPETITOR_INST ||
      competitor == BTL_COMPETITOR_COMM ||
      m_mainModule->GetRotomRank() < Savedata::FieldMenu::ROTOM_RANK_3)
  {
    return;
  }

  // Zパワー回復発動条件を満たしていない場合
  if (!m_mainModule->GetGameManager()->GetRotomPowerManager()->IsRotomPower() && !m_isUsedItemPon)
  {
    return;
  }

  u8 myClientID = m_mainModule->GetPlayerClientID();
  ZenryokuWazaStatus* zenryokuWazaStatus = m_mainModule->GetZenryokuWazaStatus(myClientID);

  // 前ターンに全力技使用、現ターンにZ回復条件が満たされた場合
  if (zenryokuWazaStatus->IsZenryokuWazaUsed() && m_isUsedItemPon)
  {
    m_isRecoverZenryoku = true;
    zenryokuWazaStatus->SetCanSecondZenryokuWaza();
    u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos(m_que, SC_ACT_EFFECT_SIMPLE);
    scmd::SCQUE_PUT_ReservedPos(m_que, que_reserve_pos, SC_ACT_EFFECT_SIMPLE, BTLEFF_Z_ROTOM_POWER);
    SCQUE_PUT_MSG_STD(m_que, BTL_STRID_STD_ZROTOM);  // ロトムずかんの　おうえんで　Ｚパワーが　つかえるようになった！
    return;
  }

  // ↓手持ちポケモンが全力技を使用したか？処理
  BTL_PARTY* party = m_pokeCon->GetPartyData(myClientID);
  const u8 party_member_max = party->GetMemberCount();
  // 自分の手持ちポケモン検索
  for (u8 memberIndex(0); memberIndex < party_member_max; ++memberIndex)
  {
    BTL_POKEPARAM* poke = party->GetMemberData(memberIndex);
    if (poke != NULL)
    {
      ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID(poke->GetID(), true);
      if (actOrder != NULL)
      {
        if (BTL_ACTION_GetAction(&actOrder->action) == BTL_ACTION_FIGHT)
        {
          if (actOrder->action.fight.zenryokuWazaFlag)
          {
            m_isRecoverZenryoku = true;
            zenryokuWazaStatus->SetCanSecondZenryokuWaza();
            u16  que_reserve_pos = scmd::SCQUE_RESERVE_Pos(m_que, SC_ACT_EFFECT_SIMPLE);
            scmd::SCQUE_PUT_ReservedPos(m_que, que_reserve_pos, SC_ACT_EFFECT_SIMPLE, BTLEFF_Z_ROTOM_POWER);
            SCQUE_PUT_MSG_STD(m_que, BTL_STRID_STD_ZROTOM);  // ロトムずかんの　おうえんで　Ｚパワーが　つかえるようになった！
            break;
          }
        }
      }
    }
  }
}

/**
* バトルフェスバトル専用フィールドエフェクトハンドラ追加
* @return 成功時 1 / 失敗時 0
*/
bool ServerFlow::scproc_Battlefes_addFieldEffect(const FieldStatus::EffectType effect, const BtlGround ground, BTL_SICKCONT& cont, const BTL_HANDEX_STR_PARAMS* exStr)
{
  bool fSucceed;

  if (effect != FieldStatus::EFF_GROUND)
  {
    fSucceed = scproc_FieldEffectCore(effect, cont, false);
  }
  else
  {
    scEvent_ChangeGroundBefore(BTL_POKEID_NULL, ground, &(cont));

    fSucceed = scproc_ChangeGround(BTL_POKEID_NULL, ground, cont);
    if (fSucceed){
      SCQUE_PUT_ACT_MsgWinHide(m_que, 0);
      SCQUE_PUT_ACT_EffectField(m_que, ground);
    }
  }

  if (fSucceed)
  {
    handexSub_putString(exStr);
  }

  if ((effect == FieldStatus::EFF_GROUND) &&
    (fSucceed))
  {
    scEvent_ChangeGroundAfter(BTL_POKEID_NULL, ground);
  }

  return fSucceed;
}

/**
 * じゅうりょく発動時チェック（そらとぶ・ダイビング＆ふゆうフラグを落とす）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_juryokuCheck( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  u8 pokeID[ BTL_EXIST_POS_MAX ];
  u8 cnt, pos, fFall, i;
  BtlExPos exPos;
  BTL_POKEPARAM* bpp;

  pos = m_mainModule->PokeIDtoPokePos( m_pokeCon, param_header->userPokeID );
  exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, pos );
  cnt = Hnd_ExpandPokeID( exPos, pokeID );
  for(i=0; i<cnt; ++i)
  {
    bpp = m_pokeCon->GetPokeParam( pokeID[i] );
    fFall = false;
    if( bpp->CONTFLAG_Get(BTL_POKEPARAM::CONTFLG_SORAWOTOBU) )
    {
      bool bFreeFallCapturing = bpp->CheckSick( pml::wazadata::WAZASICK_FREEFALL );
      scproc_TameHideCancel( bpp, BTL_POKEPARAM::CONTFLG_SORAWOTOBU, bFreeFallCapturing );
      if( bpp->IsUsingFreeFall() )
      {
        scproc_FreeFall_CheckRelease( bpp, false, true );
      }
      fFall = true;
    }
    if( scEvent_CheckFloating(bpp, true) )
    {
      fFall = true;
    }
    if( bpp->CheckSick( pml::wazadata::WAZASICK_FLYING) )
    {
      scPut_CureSick( bpp, pml::wazadata::WAZASICK_FLYING, NULL );
      fFall = true;
    }
    if( bpp->CheckSick(pml::wazadata::WAZASICK_TELEKINESIS) )
    {
      scPut_CureSick( bpp, pml::wazadata::WAZASICK_TELEKINESIS, NULL );
      fFall = true;
    }

    if( fFall ){
      SCQUE_PUT_MSG_SET( m_que, BTL_STRID_SET_JyuryokuFall, pokeID[i] );
    }
  }

  return 1;
}
/**
 * 溜めワザ消え状態のキャンセル（そらとぶ・ダイビング等）
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_TameHideCancel( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TAMEHIDE_CANCEL* param = (const BTL_HANDEX_PARAM_TAMEHIDE_CANCEL*)param_header;

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
  if( !bpp->IsDead() )
  {
    if( scproc_TameHideCancel(bpp, param->flag, false) )
    {
      handexSub_putString( &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * 位置指定エフェクト生成
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_effectByPos( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_EFFECT* param = (const BTL_HANDEX_PARAM_ADD_EFFECT*)(param_header);

  if( param->fMsgWinVanish ){
    SCQUE_PUT_ACT_MsgWinHide( m_que, 0 );
  }

  scproc_ViewEffect( param->effectNo, param->pos_from, param->pos_to, param->fQueReserve, param->reservedQuePos );
  handexSub_putString( &param->exStr );

  return 1;
}
/**
 * メッセージウィンドウ表示オフ
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_vanishMsgWin( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  GFL_UNUSED( param_header );
  SCQUE_PUT_ACT_MsgWinHide( m_que, 0 );
  return 1;
}
/**
 * フォルムナンバーチェンジ
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_changeForm( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_FORM* param = (const BTL_HANDEX_PARAM_CHANGE_FORM*)param_header;

  BTL_POKEPARAM* bpp = m_pokeCon->GetPokeParam( param->pokeID );
  if( (!bpp->IsDead())
  &&  (!bpp->HENSIN_Check())
  ){
    //@バトル変更箇所
    //ゲンシカイキはメガシンカに近いので、処理を完全に分ける！
    //scproc_MegaEvoのコピペなので注意
    if( bpp->GetMonsNo() == MONSNO_KAIOOGA ||
        bpp->GetMonsNo() == MONSNO_GURAADON )
    {
      TokuseiNo  prevTok = (TokuseiNo)(bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));

      handler::tokusei::Remove( bpp );
      bpp->GensiKaiki( param->formNo );
      handler::tokusei::Add( bpp );

      SCQUE_PUT_ACT_ChangeForm( m_que, param->pokeID, param->formNo, param->fDontResetFormByOut );
      //GFSBTS[1736]メッセージ追加
      handexSub_putString( &param->exStr );

      TokuseiNo  nextTok = (TokuseiNo)(bpp->GetValue( BTL_POKEPARAM::BPP_TOKUSEI ));
      
      //SBTS[102]対応 メンバー入場と、特性書き換えで2重に登録されてしまうため。
      //MemberInの方をブロックすると、遅いグラードン、早いアルファカイオーガを出した場合、グラードンからイベントが動くし、
      //まきびしのdmg処理とかをMemberInでやっているので触りたくない
      //SBTS[2532] 対処詳細はscproc_AfterMemberIn関数のコメントを見てください。
      scproc_AfterTokuseiChanged_Event( bpp );
      scproc_AfterTokuseiChanged_Item( bpp, prevTok, nextTok );

      m_mainModule->RegisterZukanSeeFlag( bpp );
    }
    else
    {
      u8 currentForm = bpp->GetFormNo();
      if( currentForm != param->formNo )
      {
        if( param_header->tokwin_flag ){
          scPut_TokWin_In( bpp );
        }

        bpp->ChangeForm( param->formNo, param->fDontResetFormByOut );
        SCQUE_PUT_ACT_ChangeForm( m_que, param->pokeID, param->formNo, param->fDontResetFormByOut );
        handexSub_putString( &param->exStr );

        if( param_header->tokwin_flag ){
          scPut_TokWin_Out( bpp );
        }

        m_mainModule->RegisterZukanSeeFlag( bpp );
      }
      return 1;
    }
  }
  return 0;
}
/**
 * ワザエフェクトインデックス変更
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setWazaEffectIndex( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_EFFECT_IDX* param = (const BTL_HANDEX_PARAM_SET_EFFECT_IDX*)param_header;

  wazaEffCtrl_SetEffectIndex( m_wazaEffCtrl, param->effIndex );
  return 1;
}
/**
 * ワザエフェクトを強制的に有効にする
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_setWazaEffectEnable( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  //const BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE* param = (const BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE*)param_header;

  wazaEffCtrl_SetEnable( m_wazaEffCtrl );
  return 1;
}
/**
 * なかよし度エフェクト呼び出し
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_friendshipEffect( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT* param = (const BTL_HANDEX_PARAM_FRIENDSHIP_EFFECT*)param_header;

  #if 0
  //  handexSub_putString( &param->exStr );
  SCQUE_PUT_ACT_FriendshipEffect( m_que, param->pokeID, param->effType );
  #else
  GFL_ASSERT(param->exStr.argCnt <= 2);
  SCQUE_PUT_ACT_FriendshipEffectWithMsg( 
    m_que, 
    param->pokeID, 
    param->effType,
    param->exStr.type, 
    param->exStr.ID, 
    static_cast<u16>( param->exStr.args[0] ),
    static_cast<u16>( param->exStr.args[1] ) );
  #endif

  return 1;
}

/**
 * @brief 直後に技アクションを追加する
 * @return 成功時 1 / 失敗時 0
 */
u8  ServerFlow::scproc_HandEx_insertWazaAction( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* param = (const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION*)param_header;
  uint  infoID = m_insertWazaInfoStorage.AddParam( param );
  bool isSucceeded = Hnd_InsertWazaAction( param->actPokeID, param->actWazaNo, param->targetPos, false, param->actionDesc, infoID );

  if( !isSucceeded ){
    m_insertWazaInfoStorage.RemoveParam( infoID );
    return 0;
  }
  return 1;
}
// 割り込み実行ワザアクションの前に、情報表示コマンドを生成する
void ServerFlow::scPut_InsertWazaInfo( const BTL_HANDEX_PARAM_INSERT_WAZA_ACTION* param )
{
  if( param != NULL )
  {
    const BTL_HANDEX_PARAM_HEADER* param_header = (const BTL_HANDEX_PARAM_HEADER*)( param );
    const BTL_POKEPARAM* pp_user = m_pokeCon->GetPokeParam( param_header->userPokeID );

    if( param_header->tokwin_flag )
    {
      scPut_TokWin_In( pp_user );
    }

    handexSub_putString( &(param->successMessage) );

    if( param_header->tokwin_flag )
    {
      scPut_TokWin_Out( pp_user );
    }
  }
}

/**
 * @brief ポケモンに汚れをセットする
 * @return 成功時 1 / 失敗時 0
 */
u8 ServerFlow::scproc_HandEx_setDirt( const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_DIRT* param = (const BTL_HANDEX_PARAM_SET_DIRT*)param_header;
  BTL_POKEPARAM* poke = m_pokeCon->GetPokeParam( param->pokeID );
  scPut_SetDirt( poke, param->dirtType );
  return true;
}

void ServerFlow::UseItemPon(void)///<～ポン使用
{
  m_isUsedItemPon = true;
}

bool ServerFlow::IsUsedItemPon(void) const///<～ポン使用しているか
{
  return m_isUsedItemPon;
}

fx32 ServerFlow::RotomPowerCalcCapture(fx32 ratio_fx) const///<ロトムパワーによる捕獲率補正
{
  f32 ratio = FXtoFloat(ratio_fx);

  f64 calc_hokakuritu = ratio;

  if (m_isUsedTukamaePon)
  {
    const f32 POWER_CAPTURE = 2.5f;
    GFL_ASSERT((ratio * POWER_CAPTURE) < FLT_MAX);
    calc_hokakuritu = ratio * POWER_CAPTURE;
  }

  BTL_PRINT("assist_power_manager:パワー：捕獲率 %f -> %f\n", ratio, calc_hokakuritu);
  ratio = static_cast<f32>(calc_hokakuritu);
  ratio_fx = FX32_CONST(ratio);
  return ratio_fx;
}
void ServerFlow::UseTukamaePon(void)///<つかまえポン使用
{
  m_isUsedTukamaePon = true;
}
bool ServerFlow::IsUsedTukamaePon(void)const///<つかまえポン使用しているか
{
  return m_isUsedTukamaePon;
}

GFL_NAMESPACE_END(btl)


#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )//'u8' : ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif



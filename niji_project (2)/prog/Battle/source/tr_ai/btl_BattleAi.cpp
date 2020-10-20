/**
 * @file btl_BattleAi.cpp
 * @brief 戦闘AI
 * @author obata_toshihiro
 * @date 2015.04.01
 */

#include "../btl_mainmodule.h"
#include "../btl_ServerFlow.h"
#include "../btl_client.h"
#include "../btl_pokecon.h"
#include "../btl_Heap.h"
#include "../btl_tables.h"
#include "./btl_BattleAiSystem.h"
#include "./btl_AiJudge.h"
#include "./btl_AiItemJudge.h"
#include "./btl_AiPokeChangeJudge.h"
#include "./btl_AiWazaJudge.h"
#include "./btl_BattleAi.h"
#include "./btl_SenarioAiZenryokuWazaSelector.h"

#include <System/include/Timer/gfl2_PerformanceCounter.h>




GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        インスタンスの生成に使用するヒープ
 * @param setupParam  セットアップパラメータ
 */
//---------------------------------------------------------------------------
BattleAi::BattleAi( HeapHandle heap, const BattleAi::SetupParam& setupParam ) : 
  m_mainModule( setupParam.mainModule ),
  m_pokecon( setupParam.pokecon ),
  m_client( NULL ),
  m_procPoke( NULL ),
  m_heapForScriptExec( NULL ),
  m_randGenerator( NULL ),
  m_itemJudge( NULL ),
  m_pokeChangeJudge( NULL ),
  m_wazaJudge( NULL ),
  m_senarioZenryokuWazaSelector( NULL ),
  m_judgeTargetItem( ITEM_DUMMY_DATA ),
  m_pokeChangeTarget( 0 ),
  m_selectedWazaIndex( 0 ),
  m_selectedTargetPos( 0 ),
  m_canUseZenryokuWaza( false ),
  m_result(),
  m_turnCount( 0 ),
  m_seq( 0 ),
  m_isFinished( true )
{
  HeapHandle heapForScriptBinary = BattleAiSystem::GetHeapHandle();
  HeapHandle heapForFileRead = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );

  m_randGenerator = GFL_NEW( heap ) AIRandSys();
  m_randGenerator->Initialize( setupParam.randSeed );

  m_script = GFL_NEW( heap ) AiScript( heapForScriptBinary, heapForFileRead );
  m_wazaJudge = GFL_NEW( heap ) AiWazaJudge( heap, m_script, setupParam.mainModule, setupParam.serverFlow, setupParam.pokecon, setupParam.randSeed,setupParam.scriptNoBit );
  m_itemJudge = GFL_NEW( heap ) AiItemJudge( heap, m_script, setupParam.mainModule, setupParam.serverFlow, setupParam.pokecon, setupParam.randSeed, setupParam.scriptNoBit );
  m_pokeChangeJudge = GFL_NEW( heap ) AiPokeChangeJudge( heap, m_script, setupParam.mainModule, setupParam.serverFlow, setupParam.pokecon, setupParam.randSeed, setupParam.scriptNoBit );

  {
    SenarioAiZenryokuWazaSelector::SetupParam senarioZenryokuParam;
    senarioZenryokuParam.mainModule = setupParam.mainModule;
    senarioZenryokuParam.pokecon    = setupParam.pokecon;
    senarioZenryokuParam.serverFlow = setupParam.serverFlow;
    senarioZenryokuParam.randSys    = m_randGenerator;
    m_senarioZenryokuWazaSelector = GFL_NEW( heap ) SenarioAiZenryokuWazaSelector( senarioZenryokuParam );
  }
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
BattleAi::~BattleAi()
{
  this->DeleteHeapForScriptExec();
  GFL_SAFE_DELETE( m_senarioZenryokuWazaSelector );
  GFL_SAFE_DELETE( m_randGenerator );
  GFL_SAFE_DELETE( m_wazaJudge );
  GFL_SAFE_DELETE( m_itemJudge );
  GFL_SAFE_DELETE( m_pokeChangeJudge );
  GFL_SAFE_DELETE( m_script );
}

//---------------------------------------------------------------------------
/**
 * @brief 実行するスクリプトを変更する
 * @param scriptNoBit  実行するAIスクリプトNo.のビットフラグ
 */
//---------------------------------------------------------------------------
void BattleAi::ChangeScript( u32 scriptNoBit )
{
  m_wazaJudge->ChangeAiBit( scriptNoBit );
  m_itemJudge->ChangeAiBit( scriptNoBit );
  m_pokeChangeJudge->ChangeAiBit( scriptNoBit );
}


//---------------------------------------------------------------------------
/**
 * @brief 行動選択が終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//---------------------------------------------------------------------------
bool BattleAi::IsActionSelectFinished( void ) const
{
  return m_isFinished;
}

//---------------------------------------------------------------------------
/**
 * @brief 行動選択の結果を取得する
 */
//---------------------------------------------------------------------------
const BattleAi::Result& BattleAi::GetResult( void ) const
{
  return m_result;
}

//---------------------------------------------------------------------------
/**
 * @biref 行動選択を開始する
 * @param heapForScriptExec  スクリプトの実行に使用する一時ヒープ
 * @param startParam         開始パラメータ
 */
//---------------------------------------------------------------------------
void BattleAi::StartActionSelect( HeapHandle heapForScriptExec, const BattleAi::StartParam& startParam )
{
  m_isFinished = false;
  m_seq = 0;
  m_judgeTargetItem = startParam.itemId;
  m_canUseZenryokuWaza = startParam.canUseZenryokuWaza;
  m_client = m_mainModule->GetClientByPokeID( startParam.pokeId );
  m_procPoke = m_pokecon->GetPokeParamConst( startParam.pokeId );

  for( u32 i=0; i<PTL_WAZA_MAX; ++i ) 
  {
    m_isWazaSelectable[i] = ( startParam.selectableWazaFlags[i] != 0 );
  }

  for( u32 i=0; i<BattleAi::AIACT_NUM; ++i ) 
  {
    m_actionData[i].valuation = ACTION_VALUATION_PROHIBIT;
    m_actionData[i].score =AiJudge::SCORE_FLAT;
  }

  this->CreateHeapForScriptExec( heapForScriptExec );
}

/**
 * @brief AIスクリプトを実行するためのヒープを確保する
 * @param heap  AIスクリプトの実行に使用するヒープ
 */
void BattleAi::CreateHeapForScriptExec( HeapHandle heap )
{
  if( m_heapForScriptExec )
  {
    GFL_ASSERT(0);
    this->DeleteHeapForScriptExec();
  }

  if( Heap::HEAPSIZE_AI_EXEC <= heap->GetTotalAllocatableSize() )
  {
    m_heapForScriptExec = GFL_CREATE_LOCAL_HEAP_NAME( heap->GetLowerHandle(), Heap::HEAPSIZE_AI_EXEC, gfl2::heap::HEAP_TYPE_EXP, false , "BtlAi" );
    m_wazaJudge->AttackHeapForScriptExec( m_heapForScriptExec );
    m_itemJudge->AttackHeapForScriptExec( m_heapForScriptExec );
    m_pokeChangeJudge->AttackHeapForScriptExec( m_heapForScriptExec );
  }
  else
  {
    GFL_PRINT( "AIを実行するためのヒープが確保できませんでした\n" );
    GFL_ASSERT(0);
  }
}

/**
 * @brief AIスクリプトを実行するためのヒープを解放する
 */
void BattleAi::DeleteHeapForScriptExec( void )
{
  if( m_heapForScriptExec == NULL )
  {
    return;
  }

  GFL_DELETE_HEAP( m_heapForScriptExec );
  m_heapForScriptExec = NULL;
}

/**
 * @brief AIスクリプトの実行が可能か？
 * @retval true   実行できる
 * @retval false  実行できない
 */
bool BattleAi::CanScriptExec( void ) const
{
  if( m_heapForScriptExec == NULL )
  {
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------
/**
 * @brief 行動選択メインループ
 */
//---------------------------------------------------------------------------
void BattleAi::UpdateActionSelect( void )
{
  enum {
    SEQ_START,
    SEQ_ITEM_START,            // 選択肢「道具」の採点：開始
    SEQ_ITEM_WAIT,             // 選択肢「道具」の採点：終了待ち
    SEQ_POKECHANGE_START,      // 選択肢「入れ替え」の採点：開始
    SEQ_POKECHANGE_WAIT,       // 選択肢「入れ替え」の採点：終了待ち
    SEQ_WAZA_START,            // 選択肢「技」の採点：開始
    SEQ_WAZA_WAIT,             // 選択肢「技」の採点：終了待ち
    SEQ_DECIDE_ACT,            // 行動を決定する
    SEQ_SENARIO_ZENRYOKU_WAZA, // 決定した行動が「ワザ」だった時、全力技を使うかどうかを判定する( シナリオ限定 )
    SEQ_DONE,
  };

  if( this->IsActionSelectFinished() ) {
    return;
  }

  switch( m_seq )
  {
  case SEQ_START:

    // 特殊な行動選択をチェック
    if( this->CheckSpecialAction( &m_result ) )
    {
      m_seq = SEQ_DONE;
      return;
    }

    if( this->CanScriptExec() )
    {
      m_seq = SEQ_ITEM_START;
    }
    else
    {
      m_seq = SEQ_DONE;  // ヒープが確保できなかったなら、何もしない
    }
    return;

  case SEQ_ITEM_START:
    if( this->IsItemUseEnable() ) 
    {
      m_itemJudge->StartJudge( m_procPoke, m_judgeTargetItem );
      m_seq = SEQ_ITEM_WAIT;
      return;
    }
    else
    {
      m_seq = SEQ_POKECHANGE_START;
      return;
    }

  case SEQ_ITEM_WAIT:
    this->UpdateJudge( m_itemJudge );
    if( m_itemJudge->IsJudgeFinished() )
    {
      m_actionData[ AIACT_ITEM ].valuation = ACTION_VALUATION_SCORE;
      m_actionData[ AIACT_ITEM ].score = m_itemJudge->GetScore();
      m_seq = SEQ_POKECHANGE_START;
    }
    return;


  case SEQ_POKECHANGE_START:
    if( this->IsPokeChangeEnable() ) 
    {
      m_pokeChangeJudge->StartJudge( m_procPoke );
      m_seq = SEQ_POKECHANGE_WAIT;
      return;
    }
    else
    {
      m_seq = SEQ_WAZA_START;
      return;
    }

  case SEQ_POKECHANGE_WAIT:
    this->UpdateJudge( m_pokeChangeJudge );
    if( m_pokeChangeJudge->IsJudgeFinished() )
    {
      const AiPokeChangeJudge::JudgeResult result = m_pokeChangeJudge->GetJudgeResult();
      if( result.isPokeChangeEnable ) {
        m_actionData[ AIACT_POKECHANGE ].valuation = ACTION_VALUATION_SCORE;
        m_actionData[ AIACT_POKECHANGE ].score = result.score;
        m_pokeChangeTarget = result.inPokePartyIndex;
      }
      m_seq = SEQ_WAZA_START;
    }
    return;


  case SEQ_WAZA_START:
    if( this->IsWazaUseEnable() )
    {
      u8 myID  = m_procPoke->GetID();
      u8 mypos = m_mainModule->PokeIDtoPokePos( m_pokecon, myID );
      m_wazaJudge->SetJudgeParam( m_isWazaSelectable, mypos, myID, this->GetItemNoForWazaJudge() );
      m_wazaJudge->StartJudge();
      m_seq = SEQ_WAZA_WAIT;
      return;
    }
    else
    {
      m_actionData[ AIACT_WARUAGAKI ].valuation = ACTION_VALUATION_SCORE;
      m_actionData[ AIACT_WARUAGAKI ].score = AiJudge::SCORE_FLAT;
      m_seq = SEQ_DECIDE_ACT;
      return;
    }


  case SEQ_WAZA_WAIT:
    this->UpdateJudge( m_wazaJudge );
    if( !( m_wazaJudge->IsJudgeFinished() ) ) {
      return;
    }
    this->setActionDataByWazaJudge( m_wazaJudge );
    m_seq = SEQ_DECIDE_ACT;
    return;

  case SEQ_DECIDE_ACT:
    this->DecideAction();
    m_seq = (m_canUseZenryokuWaza)? SEQ_SENARIO_ZENRYOKU_WAZA  :  SEQ_DONE;
    return;

  case SEQ_SENARIO_ZENRYOKU_WAZA:
    if( m_result.action == AIACT_WAZA )
    {
      if( m_senarioZenryokuWazaSelector->ShouldUseZenryokuWaza(m_procPoke, m_selectedWazaIndex, m_selectedTargetPos) )
      {
        m_result.wazaZenryokuFlag = true;
      }
    }
    m_seq = SEQ_DONE;
    return;

  case SEQ_DONE:
    this->DeleteHeapForScriptExec();
    m_turnCount++;
    m_isFinished = true;
    return;

  default:
    GFL_ASSERT(0);
    m_seq = SEQ_DONE;
    return;
  }
}

/**
 * @brief  使用ワザ判定処理後、最終判定のための ActionData 内容を更新する
 * @param[in]  wazaJudge AI使用ワザ判定結果
 * @detail     wazaJudge 内容に応じて m_actionData 内容を更新している
 */
void BattleAi::setActionDataByWazaJudge( const AiWazaJudge* wazaJudge )
{
  if( wazaJudge->IsEnemyEscape() ) {
    m_actionData[ AIACT_ESCAPE ].valuation = ACTION_VALUATION_FORCE;
    return;
  }

  if( !( wazaJudge->IsWazaSelected() ) ) {
    m_actionData[ AIACT_WARUAGAKI ].valuation = ACTION_VALUATION_SCORE;
    m_actionData[ AIACT_WARUAGAKI ].score = AiJudge::SCORE_FLAT;
    return;
  }

  m_actionData[ AIACT_WAZA ].valuation = ACTION_VALUATION_SCORE;
  m_actionData[ AIACT_WAZA ].score = wazaJudge->GetSelectedWazaScore();
  wazaJudge->GetSelectedWaza( &m_selectedWazaIndex, &m_selectedTargetPos );

  TAYA_PRINT("[ai] clientID=%d, selectedWazaIdx=%d, targetPos=%d\n",
    m_client->GetClientID(), m_selectedWazaIndex, m_selectedTargetPos );
}

/**
 * @brief 特殊な行動選択をチェックする
 * @param[out] result  選択した行動の格納先
 * @retval true   特殊な行動を選択した
 * @retval false  特殊な行動を選択しなかった
 */
bool BattleAi::CheckSpecialAction( Result* result ) const
{
  // 初めてのウツロイド戦なら、５ターン目に逃げる
  // 特性「かげふみ」などにより逃げられない場合、逃げる→失敗を繰り返さないようにするため、
  // ５ターン目でだけ、逃げる選択をします。
  // ５ターン目に逃げられなかったなら、その後は戦います。
  if( ( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_FIRST_UTUROIDO ) ) &&
      ( m_turnCount == 4 ) )
  {
    result->action = AIACT_ESCAPE;
    return true;
  }

  return false;
}

/**
 * @brief 「道具使用」が可能か？
 * @retval true   可能
 * @retval false  可能でない
 */
bool BattleAi::IsItemUseEnable( void ) const
{
  return ( m_judgeTargetItem != ITEM_DUMMY_DATA );
}

/**
 * @brief 「ポケモン入れ替え」が可能か？
 * @retval true   可能
 * @retval false  可能でない
 */
bool BattleAi::IsPokeChangeEnable( void ) const
{
  // 野生戦なら、入れ替えない
  if( m_mainModule->GetCompetitor() == BTL_COMPETITOR_WILD ) {
    return false;
  }

  // 逃げ・交換禁止状態なら、入れ替えない
  {
    u8  prohibit_pokeID = BTL_POKEID_NULL;
    u16 prohibit_tokID  = TOKUSEI_NULL;
    if( m_client->isForbidPokeChange( m_procPoke, &prohibit_pokeID, &prohibit_tokID ) != BTL_CLIENT::CANTESC_NULL ){
      return false;
    }
  }

  // もう交換できるメンバーが居ないなら、入れ替えない
  if( !( m_client->IsPuttablePokemonExist() ) ) {
    return false;
  }

  // 入れ替えを考慮すべき相手が居ないなら、入れ替えない
  if( !( this->IsEnemyExist() ) ) {
    return false;
  }

  return true;
}

/**
 * @brief 入れ替えを考慮すべき相手が存在するか？
 * @retval true   存在する
 * @retval false  存在しない
 */
bool BattleAi::IsEnemyExist( void ) const
{
  BtlPokePos basePos = m_mainModule->PokeIDtoPokePos( m_pokecon, m_procPoke->GetID() );
  BtlExPos exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, basePos );
  u8 aryPokeID[ BTL_POSIDX_MAX ];
  u8 enemyCount = m_mainModule->ExpandExistPokeID( m_pokecon, exPos, aryPokeID );
  return ( 0 < enemyCount );
}


/**
 * @brief 「技の使用」が可能か？
 * @retval true   可能
 * @retval false  可能でない
 */
bool BattleAi::IsWazaUseEnable( void ) const
{
  return ( 0 < this->GetSelectableWazaCount() );
}

/**
 * @brief 技使用判定で参照する「使用候補」の道具No.を取得する
 */
u16 BattleAi::GetItemNoForWazaJudge( void ) const
{
  if( m_actionData[ AIACT_ITEM ].valuation == ACTION_VALUATION_PROHIBIT ) {
    return ITEM_DUMMY_DATA;
  }
  return m_judgeTargetItem;
}

/**
 * @brief 判定計算を更新する
 * @param judge  更新対象の判定計算
 */
void BattleAi::UpdateJudge( AiJudge* judge )
{
  static u64 AI_TICK_LIMIT = 10000;  // 進行時間単位（Tick）

  u64 tick_begin = gfl2::system::Timer::PerformanceCounter::GetTick();

  for ever
  {
    judge->UpdateJudge();

    if( judge->IsJudgeFinished() ) {
      return;
    }

    if( (gfl2::system::Timer::PerformanceCounter::GetTick() - tick_begin) > AI_TICK_LIMIT ){
      return;
    }
  }
}

/**
 * @brief 使用できる技の数を取得する
 */
u32 BattleAi::GetSelectableWazaCount( void ) const
{
  u32 count = 0;
  for( u32 i=0; i<PTL_WAZA_MAX; ++i ) 
  {
    if( m_isWazaSelectable[i] ) {
      ++count;
    }
  }
  return count;
}

/**
 * @brief 採点結果から、行動を決定する
 */
void BattleAi::DecideAction( void )
{
  AiAction action = this->GetBestAction();
  this->SetupActionParam( &m_result, action );
}

/**
 * @brief 最も優先度の高い行動を取得する
 * @retval AIACT_NULL  選択可能な行動が存在しない場合
 */
BattleAi::AiAction BattleAi::GetBestAction( void ) const
{
  AiAction action = AIACT_NULL;
  
  action = this->GetForcedAction();
  if( action != AIACT_NULL ) {
    return action;
  }

  action = this->GetBestScoredAction();
  if( action != AIACT_NULL ) {
    return action;
  }

  return AIACT_NULL;
}

/**
 * @brief 強制状態のアクションを取得する
 * @retval AIACT_NULL  選択可能な行動が存在しない場合
 */
BattleAi::AiAction BattleAi::GetForcedAction( void ) const
{
  AiAction bestActions[ AIACT_NUM ];
  u32 bestActionNum = 0;

  for( u32 act=0; act<AIACT_NUM; ++act )
  {
    if( m_actionData[ act ].valuation == ACTION_VALUATION_FORCE ) {
      bestActions[ bestActionNum ] = static_cast<AiAction>( act );
      ++bestActionNum;
    }
  }

  return this->selectForcedActionByPriority( bestActions, bestActionNum );
}

/**
 * @brief  強制状態のアクションが複数あった場合に、優先順位の高いものを選ぶ
 * @param[in]  candidates     候補となるアクション配列
 * @param[in]  numCandidates  候補となるアクション数（candidates の要素数）
 * @retval     candidates 中に、有効な行動IDが１つ以上ある場合 -> その中で最も優先度の高いアクション / それ以外->AIACT_NULL
 **/
BattleAi::AiAction
  BattleAi::selectForcedActionByPriority( const AiAction* candidates, u32 numCandidates )
{
  // プライオリティ高い順に並べておく
  static const AiAction priorityTable[] = {
    AIACT_WAZA,         // 技を使う
    AIACT_ESCAPE,       // にげる
    AIACT_ITEM,         // 道具を使う
    AIACT_POKECHANGE,   // ポケモンを入れ替える
    AIACT_WARUAGAKI,    // わるあがき
    AIACT_NULL,
  };

  const AiAction* action = priorityTable;
  while( *action != AIACT_NULL )
  {
    if( tables::IsInclude(*action, candidates, numCandidates) ){
      return *action;
    }
    ++action;
  }

  return AIACT_NULL;
}


/**
 * @brief 最も点数の高い行動を取得する（最高点の行動が複数ある場合はランダムで選択する）
 * @retval AIACT_NULL  選択可能な行動が存在しない場合
 */
BattleAi::AiAction BattleAi::GetBestScoredAction( void ) const
{
  AiAction bestActions[ AIACT_NUM ];
  u32 bestActionNum = 0;

  bestActionNum = this->storeBestScoredActions( bestActions, GFL_NELEMS(bestActions) );
  return this->SelectActionAtRandom( bestActions, bestActionNum );
}

/**
 * @brief 最高点の行動IDを配列に格納する（最高点の行動が複数あるケースを考慮し配列になっている）
 * @param[out]  dest          最高点の行動IDを格納するための配列（AIACT_NUM 以上の要素数が必要）
 * @param[in]   numDestArray  dest の要素数
 * @retval destに格納した行動IDの数
 * @detail 最高点の行動は m_actionData 配列の内容から検索している。
 */
u32 BattleAi::storeBestScoredActions( AiAction* dest, u32 numDestArray ) const
{
  s32 bestScore = 0;
  u32 numStored = 0;

  for( u32 act=0; act<GFL_NELEMS(m_actionData); ++act )
  {
    if( m_actionData[ act ].valuation != ACTION_VALUATION_SCORE ) {
      continue;
    }

    if( ( numStored == 0 ) ||
      ( bestScore < m_actionData[ act ].score ) )
    {
      bestScore = m_actionData[ act ].score;
      dest[0] = static_cast<AiAction>( act );
      numStored = 1;
    }
    else if( bestScore == m_actionData[ act ].score )
    {
      dest[ numStored ] = static_cast<AiAction>( act );
      ++numStored;
    }
  }
  return numStored;
}



/**
 * @brief アクションをランダムに選択する
 * @param actionArray  選択候補アクションの配列
 * @param actionNum    選択候補の数
 * @return  ランダムに選択したアクション
 * @retval AIACT_NULL  選択可能な行動が存在しない場合
 */
BattleAi::AiAction BattleAi::SelectActionAtRandom( AiAction* actionArray, u32 actionNum ) const
{
  if( actionNum == 0 ) {
    return AIACT_NULL;
  }

  if( actionNum == 1 ) {
    return actionArray[0];
  }

  u32 rand = m_randGenerator->Next( actionNum );
  return actionArray[ rand ];
}

/**
 * @brief 指定した行動を取るように、行動パラメータをセットアップする
 * @param[out] destParam    行動パラメータの格納先
 * @param      action       セットアップ対象の行動
 */
void BattleAi::SetupActionParam( Result* destParam, AiAction action ) const
{
  destParam->action = action;

  switch( action )
  {
  case BattleAi::AIACT_ITEM:
    destParam->itemId = m_judgeTargetItem;
    break;

  case BattleAi::AIACT_POKECHANGE:
    destParam->changePokeIndex = m_pokeChangeTarget;
    break;

  case BattleAi::AIACT_WAZA:
    destParam->wazaIndex = m_selectedWazaIndex;
    destParam->wazaTargetPos = m_selectedTargetPos;
    destParam->wazaZenryokuFlag = false;
    break;
  }
}


GFL_NAMESPACE_END( btl )
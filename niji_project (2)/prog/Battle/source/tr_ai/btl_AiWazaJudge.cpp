//============================================================================================
/**
 * @file  btl_AiWazaJudge.cpp
 * @brief 戦闘AI 選択肢「技」の採点
 * @author  taya
 * @date  06.04.25
 *
 *  2011.02.25 WB->XYへ丸ごと移植
 */
//============================================================================================
#include "../btl_MainModule.h"
#include "../btl_PokeCon.h"
#include "../btl_Calc.h"
#include "../btl_SideEff.h"
#include "../btl_ServerFlow.h"
#include "./btl_BattleAiSystem.h"
#include "./btl_AiScript.h"
#include "./btl_AiScriptHandler.h"
#include "./btl_AiScriptCommandHandler.h"
#include "./btl_AiWazaJudge.h"


#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)

#include "./tr_ai_cmd.h"

/**
 * macro
 */
#define  LOADING_AI_USE_THREAD   (1)

//-------------------------------------------------------
/**
 *  定数
 */
//-------------------------------------------------------
enum
{
  TR_AI_VM_STACK_SIZE = 16, ///<使用するスタックのサイズ
  TR_AI_VM_REG_SIZE   = 8,    ///<使用するレジスタの数
  WAZAPOINT_FLAT = 100,     //技繰り出し判定用点数の初期値
};



//--------------------------------------------------------------
// ctor / dtor
//--------------------------------------------------------------
AiWazaJudge::AiWazaJudge( 
  HeapHandle heapHandle,
  AiScript* aiScript, 
  const MainModule* mainModule, 
  ServerFlow* svfWork, 
  const POKECON* pokeCon, 
  const u32 randSeed,
  u32 ai_bit )
 : AiJudge( 
    BTL_AISCRIPT_NO_WAZA_MIN,
    BTL_AISCRIPT_NO_WAZA_MAX,
    ai_bit )
 , m_mainModule(mainModule)
 , m_svfWork(svfWork)
 , m_pokeCon(pokeCon)
 , m_randGenerator( NULL )
 , m_script( aiScript )
 , m_scriptHandler( NULL )
 , m_scriptCommandHandler( NULL )
{
#if defined(GF_PLATFORM_CTR)
  NN_COMPILER_ASSERT( WAZADATA_DMG_PHYSIC  == pml::wazadata::DMG_PHYSIC );
  NN_COMPILER_ASSERT( WAZADATA_DMG_SPECIAL == pml::wazadata::DMG_SPECIAL );
#endif

  m_randGenerator = GFL_NEW( heapHandle ) AIRandSys();
  m_randGenerator->Initialize( randSeed );

  m_scriptHandler = GFL_NEW( heapHandle ) AiScriptHandler();
  m_scriptCommandHandler = GFL_NEW( heapHandle ) AiScriptCommandHandler( heapHandle, mainModule, svfWork, pokeCon, randSeed );
}

AiWazaJudge::~AiWazaJudge()
{
  GFL_SAFE_DELETE( m_randGenerator );
  GFL_SAFE_DELETE( m_scriptHandler );
  GFL_SAFE_DELETE( m_scriptCommandHandler );
}



//=============================================================================================
/**
 * AIワザ選択１体分開始
 *
 * @param[in]   usableWazaFlags
 * @param[in]   pos
 * @param[in]   pokeID
 * @param[in]   itemId            使用候補の道具
 */
//=============================================================================================
void  AiWazaJudge::SetJudgeParam( const bool* usableWazaFlags, BtlPokePos pos, u8 pokeID, u16 itemId )
{
  m_pokeID = pokeID;
  m_atkPos = pos;
  m_itemId = itemId;

  m_atkPoke = m_pokeCon->GetPokeParamConst( pokeID );
  m_atkClientID = m_mainModule->PokeIDtoClientID( pokeID );

  for(u32 i=0; i<GFL_NELEMS(m_usableWazaFlags); ++i){
    m_usableWazaFlags[i] = usableWazaFlags[i];
  }
}

void AiWazaJudge::StartJudge( void )
{
  m_AIStep     = 0;
  m_bDecided   = false;
  m_bFinished  = false;
  m_scriptCommandHandler->ResetEscape();
}

bool AiWazaJudge::IsJudgeFinished( void ) const
{
  return m_bFinished;
}

//============================================================================================
/**
 * @brief VMメイン
 */
//============================================================================================
void AiWazaJudge::UpdateJudge( void )
{
  if( this->IsJudgeFinished() ) {
    return;
  }

  subProc_Core();
}
// ----------------------------------------------------
/**
 *  SubProcess core
 */
// ----------------------------------------------------
void  AiWazaJudge::subProc_Core()
{
  enum {
    AISTEP_START,
    AISTEP_INIT,
    AISTEP_SWITCH_SCRIPT,
    AISTEP_CHECK_RUNNABLE_SCRIPT,
    AISTEP_SCRIPT_START,
    AISTEP_SETUP_WAZA,
    AISTEP_SCRIPT_WAIT,
    AISTEP_SWITCH_WAZA,
    AISTEP_SWITCH_TARGET,
    AISTEP_DONE,
  };


  switch( m_AIStep )
  {
  case AISTEP_START:
    wazaScore_Reset();
    this->ResetScriptNo();
    m_AIStep = AISTEP_INIT;
    break;

  case AISTEP_DONE:
    wazaScore_DecideBest();
    m_bFinished = true;
    break;

  case AISTEP_SWITCH_SCRIPT:
    this->UpdateScriptNo();
    m_AIStep = AISTEP_INIT;

    /* fallthru */
  case AISTEP_INIT:

#if PM_DEBUG
    // デバッグ時、スクリプトロード毎に最新状態を反映させる
    //m_script->Reset();
#endif

    if( this->IsAllScriptFinished() )
    {
      m_AIStep = AISTEP_DONE;
      break;
    }
    m_currentTargetIdx = 0;
    m_currentWazaPos = 0;
    m_bTargetSideFriend = false;
    m_AIStep = AISTEP_CHECK_RUNNABLE_SCRIPT;

    /* fallthru */

  /* スクリプト実行可能な条件が整っているかチェック */
  case AISTEP_CHECK_RUNNABLE_SCRIPT:
    m_defPos = updateTargetPos( m_bTargetSideFriend, m_currentTargetIdx );
    m_currentWazaNo = m_atkPoke->WAZA_GetID( m_currentWazaPos );
    GFL_PRINT_EX( PRINT_FLAG, "ワザ[%d] ターゲット位置[%d] スクリプト実行可能か判定\n", m_currentWazaNo, m_defPos );
    // 使用不可のワザ？
    if( !m_usableWazaFlags[ m_currentWazaPos ] ){
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  waza[%d] は使えない\n", m_currentWazaNo);
      m_AIStep = AISTEP_SWITCH_WAZA;
      return;
    }
    // すでに計算対象外と判定されている？
    if( wazaScore_IsScoreless(m_currentWazaPos, m_defPos) ){
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  targetPos[%d], waza[%d] は計算対象外と判断されている\n", m_defPos, m_currentWazaNo);
      m_AIStep = AISTEP_SWITCH_TARGET;
      return;
    }
    // 範囲対象外のターゲットを指している？
    if( !this->isTargettingCoveragePos(m_currentWazaNo, m_defPos) ){
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  targetPos[%d] はワザ[%d]範囲の対象外\n", m_defPos, m_currentWazaNo);
      wazaScore_SetScoreless( m_currentWazaPos, m_defPos );
      m_AIStep = AISTEP_SWITCH_TARGET;
      return;
    }
    // 対象ポケモンがひん死？
    m_defPoke = decideTargetPoke( m_defPos );
    if( m_defPoke->IsDead() ){
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  targetPos[%d] にいるポケモンはひん死なのでスキップ\n", m_defPos );
      wazaScore_SetScoreless( m_currentWazaPos, m_defPos );
      m_AIStep = AISTEP_SWITCH_TARGET;
      return;
    }
    m_AIStep = AISTEP_SCRIPT_START;
    /* fallthru */


  case AISTEP_SCRIPT_START:
    m_currentWazaNo = m_atkPoke->WAZA_GetID( m_currentWazaPos );
    m_scriptCommandHandler->StoreUsedWaza( m_defPoke );
    {
      AiScriptHandler::ScriptStartParam startParam;
      startParam.heapForScriptExec = this->GetHeapForScriptExec();
      startParam.script = m_script;
      startParam.scriptNo = this->GetCurrentScriptNo();
      startParam.commandHandler = m_scriptCommandHandler;
      startParam.commandParam.attackPoke = m_atkPoke;
      startParam.commandParam.defensePoke = m_defPoke;
      startParam.commandParam.currentWazaIndex = m_currentWazaPos;
      startParam.commandParam.currentWazaNo = m_currentWazaNo;
      startParam.commandParam.currentItemNo =  m_itemId;
      m_scriptHandler->StartScript( startParam );
    }
    m_AIStep = AISTEP_SCRIPT_WAIT;

    /* fallthru */

  /* スクリプトを実行する */
  case  AISTEP_SCRIPT_WAIT:
    if( m_scriptHandler->WaitScript() )
    {
      AiScript::Result result = m_scriptHandler->GetScriptResult();
      wazaScore_Add( m_currentWazaPos, m_defPos, result.score );
      m_AIStep = AISTEP_SWITCH_TARGET; // 終わったらターゲット切り替え
      return;
    }
    break;

  /* 想定対象ポケモンを切り替える */
  case  AISTEP_SWITCH_TARGET:
    if( this->incrementTargetIndex() ){
      // 次のターゲット見つかればスクリプト実行可否チェックへ
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  ターゲット切り替えました -> 味方フラグ:%d, idx:%d\n", m_bTargetSideFriend, m_currentTargetIdx );
      m_AIStep = AISTEP_CHECK_RUNNABLE_SCRIPT;
      return;
    }
    GFL_PRINT_EX( PRINT_FLAG, "[AI]  もうターゲット全チェックした -> ワザ切り替え\n");
    m_AIStep = AISTEP_SWITCH_WAZA;
    /* fallthru : もうターゲットを最後までチェックしたならワザ切り替えへ */

  /* 採点するワザを切り替える */
  case  AISTEP_SWITCH_WAZA:
    m_currentTargetIdx = 0;
    m_bTargetSideFriend = false;
    ++m_currentWazaPos;
    // ワザもう無い場合はスクリプト切り替えへ
    if( m_currentWazaPos >= m_atkPoke->WAZA_GetCount() ){
      GFL_PRINT_EX( PRINT_FLAG, "[AI]  このスクリプトでチェックするワザはもう無い -> スクリプト切り替え\n");
      m_AIStep = AISTEP_SWITCH_SCRIPT;
      return;
    }
    m_AIStep = AISTEP_CHECK_RUNNABLE_SCRIPT;
    return;

  }

  return;

}
/**
 * AIのチェック対象となる相手ポケモンIndexをインクリメントする
 * @return もう対象がいない時に false / それ以外は true
 */
bool AiWazaJudge::incrementTargetIndex( void )
{
  const BtlRule rule = m_mainModule->GetRule();

  u32 frontPosCount;
  if( rule == BTL_RULE_ROYAL )
  {
    frontPosCount = m_mainModule->GetEnemyClientNum( m_atkClientID );
  }
  else if( !m_bTargetSideFriend )
  {
    frontPosCount = m_mainModule->GetOpponentFrontPosNum( m_atkClientID );
  }
  else
  {
    frontPosCount = m_mainModule->GetFrontPosNum( m_atkClientID );
  }

  ++m_currentTargetIdx;

  if( m_currentTargetIdx < frontPosCount ){
    return true;
  }

  // 相手サイドをチェックし終えた後、自分以外の味方が場にいるルールなら対象を味方サイドに切り替え
  if( !m_bTargetSideFriend )
  {
    BtlRule rule = m_mainModule->GetRule();
    if( (rule != BTL_RULE_SINGLE)
    &&  (rule != BTL_RULE_ROYAL)
    ){
      m_bTargetSideFriend = true;
      m_currentTargetIdx = 0;
      return true;
    }
  }

  return false;
}


/**
 * ワザ点数計算の対象位置を取得
 * @param bFriendSide   味方側を対象とする場合 true
 * @param targetIdx     場に出ている内、何体目を対象とするか（ 0 org ）
 * @return  BtlPokePos  対象位置ID
 */
BtlPokePos  AiWazaJudge::updateTargetPos( bool bFriendSide, u8 targetIdx ) const
{
  // 対象位置を取得
  if( !bFriendSide )
  {
    return m_mainModule->GetOpponentPokePos( m_atkPos, targetIdx );
  }
  else
  {
    BtlRule rule = m_mainModule->GetRule();
    return m_mainModule->GetFriendPokePos( rule, m_atkPos, targetIdx );
  }
}

/**
 * ワザの対象として正しい範囲をターゲットしているか判定
 *
 * @return 正しい範囲をターゲットしていたら true
 */
bool AiWazaJudge::isTargettingCoveragePos( WazaNo waza_no, BtlPokePos targetPos )
{
  if( m_atkPos == targetPos ){
    return false;
  }
  return true;
}

/**
 * ワザの効果範囲パラメータから、対象としてはいけない位置を狙っていた場合に補正する
 * @param targetPos   AI計算結果によるベストスコアの対象位置ID
 * @param atkPos      ワザを打つポケモンの位置ID
 * @param atkPoke     ワザを打つポケモンパラメータ
 * @param wazaIdx     ワザIndex
 * @return    補正後の対象位置ID
 */
BtlPokePos AiWazaJudge::correctTargetPos( BtlPokePos targetPos, BtlPokePos atkPos, const BTL_POKEPARAM* atkPoke, u8 wazaIdx ) const
{
  WazaID  waza = atkPoke->WAZA_GetID( wazaIdx );
  if( waza != WAZANO_NULL )
  {
    const BtlRule rule = m_mainModule->GetRule();
    const pml::wazadata::WazaTarget  targetArea = (pml::wazadata::WazaTarget)(WAZADATA_GetParam( waza, pml::wazadata::PARAM_ID_TARGET ));
    const bool bTargetFriend = MainModule::IsFriendPokePos( rule, targetPos, atkPos );

    switch( targetArea ){

      case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
        // 味方以外を狙っていたら自分を狙う
        if( !bTargetFriend ){
          targetPos = atkPos;
        }
        break;


      case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
        // 味方以外を狙っていたら自分以外で最もスコアの高い味方を狙う
        if( !bTargetFriend ){
          BtlPokePos correctPos = searchBestScorePos( wazaIdx, atkPos, MainModule::PosToSide(rule,atkPos) );
          if( (correctPos != atkPos) && (correctPos != BTL_POS_NULL) ){
            targetPos = correctPos;
          }
        }
        break;

      case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
        // 味方を狙っていたら最もスコアの高い敵側のポケモンを狙う
        if( bTargetFriend ){
          BtlSide    side = m_mainModule->GetOpponentSide(MainModule::PosToSide(rule,atkPos) );
          BtlPokePos correctPos = searchBestScorePos( wazaIdx, atkPos, side );
          if( correctPos != BTL_POS_NULL ){
            targetPos = correctPos;
          }
        }
        break;

      case pml::wazadata::TARGET_USER:               ///< 自分のみ
        targetPos = atkPos;
        break;
    }
  }

  return targetPos;
}
/**
 * 計算済みワークから最高得点の対象位置IDを探して返す
 * @param  wazaIdx    検査対象のワザIndex
 * @param  atkPos     攻撃ポケ位置ID（この位置IDは選ばない）
 * @param  side       検査対象陣営ID
 */
BtlPokePos AiWazaJudge::searchBestScorePos( u8 wazaIdx, BtlPokePos atkPos, BtlSide side ) const
{
  const BtlRule rule = m_mainModule->GetRule();
  BtlPokePos pos = BTL_POS_NULL;
  int        bestScore = 0;
  bool       bFound = false;

  for(u32 p=0; p<BTL_POS_NUM; ++p)
  {
    if( m_wazaScoreStatus[p][wazaIdx] == STATUS_DISABLE ){
      continue;
    }
    if( MainModule::PosToSide(rule,(BtlPokePos)p) != side ){
      continue;
    }
    if( (BtlPokePos)p == atkPos ){
      continue;
    }

    if( !bFound || (m_wazaScore[p][wazaIdx] > bestScore) )
    {
      bestScore = m_wazaScore[p][wazaIdx];
      pos = (BtlPokePos)p;
      bFound = true;
    }
  }
  return pos;
}

/**
 *  対象ポケモン位置を取得
 */
const BTL_POKEPARAM*  AiWazaJudge::decideTargetPoke( BtlPokePos target_pos )
{
  return m_pokeCon->GetFrontPokeDataConst( target_pos );
}


/**
 *  ワザごとの得点ワークをリセットする
 */
void AiWazaJudge::wazaScore_Reset( void )
{
  for(u32 i=0; i<BTL_POS_NUM; ++i)
  {
    for(u32 j=0; j<PTL_WAZA_MAX; ++j)
    {
      m_wazaScore[i][j] = AiJudge::SCORE_FLAT;
      m_wazaScoreStatus[i][j] = STATUS_DISABLE;
    }
  }
}
/**
 *  ワザごとの得点ワークにAI結果を加減算する
 */
int  AiWazaJudge::wazaScore_Add( u8 wazaIdx, BtlPokePos targetPos, int score )
{
  if( wazaIdx < PTL_WAZA_MAX && targetPos < BTL_POS_NUM )
  {
    m_wazaScoreStatus[targetPos][wazaIdx] = STATUS_NORMAL;
    m_wazaScore[targetPos][wazaIdx] += score;
    return m_wazaScore[targetPos][wazaIdx];
  }
  else
  {
    GFL_ASSERT(0);
    return 0;
  }
}
/**
 *  ワザごとの得点ワークに無効値（計算しない）を設定
 */
void  AiWazaJudge::wazaScore_SetScoreless( u8 wazaIdx, BtlPokePos targetPos )
{
  if( wazaIdx < PTL_WAZA_MAX && targetPos < BTL_POS_NUM )
  {
    m_wazaScoreStatus[targetPos][wazaIdx] = STATUS_DISCOURAGE;
    m_wazaScore[targetPos][wazaIdx] = AiJudge::SCORE_DISCOURAGE;
  }
  else
  {
    GFL_ASSERT(0);
  }
}
/**
 *  ワザごとの得点ワークに無効値（計算しない）が設定されているか判定
 */
bool  AiWazaJudge::wazaScore_IsScoreless( u8 wazaIdx, BtlPokePos targetPos )
{
  if( wazaIdx < PTL_WAZA_MAX && targetPos < BTL_POS_NUM )
  {
    if( (m_wazaScoreStatus[targetPos][wazaIdx] == STATUS_DISCOURAGE)
    &&  (m_wazaScore[targetPos][wazaIdx] == AiJudge::SCORE_DISCOURAGE)
    ){
      return true;
    }
  }
  else{
    GFL_ASSERT(0);
  }
  return false;
}



/**
 *  全AI実行後、選択するワザ（最も評価の高いワザ）を１つ、ワークに保存する
 */
void AiWazaJudge::wazaScore_DecideBest( void )
{
  const BtlRule rule = m_mainModule->GetRule();
  bool bFirstScore = true;
  int top_score = AiJudge::SCORE_FLAT;
  u32 top_score_count  = 0;

  // 最高点を得た[攻撃相手：ワザ]の組み合わせをカウント
  for(u32 p=0; p<BTL_POS_NUM; ++p)
  {
    for(u32 i=0; i<PTL_WAZA_MAX; ++i)
    {
      if( m_wazaScoreStatus[p][i] == STATUS_NORMAL )
      {
        // FLAT を割っている味方攻撃は行わない
        if( MainModule::IsFriendPokePos( rule, m_atkPos, (BtlPokePos)p )
        &&  (m_wazaScore[p][i] < AiJudge::SCORE_FLAT)
        ){
          m_wazaScoreStatus[p][i] = STATUS_DISCOURAGE;
          continue;
        }

        if( bFirstScore || m_wazaScore[p][i] > top_score ){
          bFirstScore = false;
          top_score = m_wazaScore[p][i];
          top_score_count = 1;
        }
        else if( m_wazaScore[p][i] == top_score ){
          ++top_score_count;
        }
        else if( m_wazaScore[p][i] > top_score ){
          top_score = m_wazaScore[p][i];
          top_score_count = 1;
        }
      }
    }
  }

  // デバッグ出力
  #if 1
  BTL_PRINT("=== top_score = %d, count=%d ==\n", top_score, top_score_count);
  if( top_score_count ){
    for(u32 p=0; p<BTL_POS_NUM; ++p){
      for(u32 i=0; i<PTL_WAZA_MAX; ++i){
        #if 1
        if( (m_wazaScoreStatus[p][i]==STATUS_NORMAL) && m_wazaScore[p][i] == top_score ){ BTL_PRINT("  wazaIdx=%d, targetPos=%d\n", i, p ); }
        #else
        BTL_PRINT("  pos[%d] wazaIdx[%d] : status=%d, score=%d\n", p, i, m_wazaScoreStatus[p][i], m_wazaScore[p][i]);
        #endif
      }
    }
  }
  #endif

  // 最高点の中からランダムで決定
  if( top_score_count )
  {
    u32 rnd_idx = m_randGenerator->Next( top_score_count );

    for(u32 p=0; p<BTL_POS_NUM; ++p)
    {
      for(u32 i=0; i<PTL_WAZA_MAX; ++i)
      {
        if( (m_wazaScoreStatus[p][i]==STATUS_NORMAL) && (m_wazaScore[p][i] == top_score) )
        {
          if( rnd_idx == 0 )
          {
            m_selectTargetPos = this->correctTargetPos( p, m_atkPos, m_atkPoke, i );
            m_selectWazaPos   = i;
            m_selectWazaScore = m_wazaScore[p][i];
            m_bDecided = true;
            BTL_PRINT("[ AI ] 計算で決めたターゲット : %d->%d, wazaIdx:%d\n", p, m_selectTargetPos, m_selectWazaPos );
            return;
          }
          else{
            --rnd_idx;
          }
        }
      }
    }
  }
  else
  {

    // AIが何も回っていないケース（野生戦）などでここに来る。ランダムで決定。
    u8  wazaPos[ PTL_WAZA_MAX ];

    u32 wazaCnt = m_atkPoke->WAZA_GetCount();
    u32 usableCnt = 0;
    for(u32 i=0; i<wazaCnt; ++i)
    {
      if( m_usableWazaFlags[i] ){
        wazaPos[ usableCnt++ ] = i;
      }
    }
    if( usableCnt )
    {
      int rand = m_randGenerator->Next( usableCnt );
      m_selectWazaScore = AiJudge::SCORE_FLAT;
      m_selectWazaPos   = wazaPos[rand];
      WazaID waza       = m_atkPoke->WAZA_GetID( m_selectWazaPos );

      RandContext randContext;
      m_selectTargetPos = calc::DecideWazaTargetAutoForClient( m_mainModule, m_pokeCon, m_atkPoke, waza, &randContext );
      BTL_PRINT("[ AI ] ランダムで決めたターゲット : %d\n", m_selectTargetPos);
      m_bDecided = true;
    }
    else{
      m_bDecided = false;
    }
  }
}



//==========================================================================
//==========================================================================

//=============================================================================================
/**
 * @brief 相手が「逃げる」を選択したかどうか判定
 *
 * @retval  bool
 */
//=============================================================================================
bool AiWazaJudge::IsEnemyEscape( void ) const
{
  return m_scriptCommandHandler->IsEscapeSelected();
}

//-----------------------------------------------------------------------------
/**
 * @brief 使用する技を選択したか？
 * @retval true   選択した
 * @retval false  判定ルーチンのミスにより何も決定されなかったなど特殊な状況
 */
//-----------------------------------------------------------------------------
bool AiWazaJudge::IsWazaSelected( void ) const
{
  return m_bDecided;
}

//-----------------------------------------------------------------------------
/**
 * @brief 選択した技の点数を取得する
 */
//-----------------------------------------------------------------------------
s32 AiWazaJudge::GetSelectedWazaScore( void ) const
{
  return m_selectWazaScore;
}

//-----------------------------------------------------------------------------
/**
 * @brief AIが決定した技のインデックス、対象位置を取得する
 * @param[out]   wazaIdx    技のインデックスの格納先
 * @param[out]   targetPos  技の対象位置の格納先
 */
//-----------------------------------------------------------------------------
void AiWazaJudge::GetSelectedWaza( u8* wazaIdx, BtlPokePos* targetPos ) const
{
  if( !m_bDecided ) {
    return;
  }

  *wazaIdx = m_selectWazaPos;
  *targetPos = m_selectTargetPos;
}


GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif

#ifdef LOADING_AI_USE_THREAD
  #undef  LOADING_AI_USE_THREAD
#endif



//=============================================================================
/**
 * @file   btl_SenarioAiZenryokuWazaSelector.cpp
 * @date   2016/01/13 15:21:30
 * @author obata_toshihiro
 * @brief  シナリオ中のゲーム内トレーナーが全力技を選択するためのAI
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_SenarioAiZenryokuWazaSelector.h"

#include <pml/include/pml_Waza.h>

#include "../btl_mainmodule.h"
#include "../btl_ServerFlow.h"
#include "../btl_client.h"
#include "../btl_pokeparam.h"
#include "../btl_ZenryokuWaza.h"
#include "../btl_ZenryokuWazaStatus.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param setupParam  セットアップパラメータ
 */
//-----------------------------------------------------------------------------
SenarioAiZenryokuWazaSelector::SenarioAiZenryokuWazaSelector( const SetupParam& setupParam ) : 
  m_mainModule( setupParam.mainModule ),
  m_pokecon( setupParam.pokecon ),
  m_serverFlow( setupParam.serverFlow ),
  m_randSys( setupParam.randSys )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
SenarioAiZenryokuWazaSelector::~SenarioAiZenryokuWazaSelector()
{
}


//-------------------------------------------------------------------------
/**
 * @brief ワザ決定後、そのワザを全力技として使うかどうかを判定する
 * @param[in]  attacker   ワザを撃つポケモンのパラメータ
 * @param[in]  wazaIdx    撃とうとしているワザのインデックス
 * @param[in]  targetPos  ワザ対象の位置ID
 * @return     全力ワザを使うなら true
 */
//-------------------------------------------------------------------------
bool SenarioAiZenryokuWazaSelector::ShouldUseZenryokuWaza( const BTL_POKEPARAM* attacker, u8 wazaIdx, BtlPokePos targetPos ) const
{
  const ZenryokuWazaStatus* zenryokuWazaStatus = getClientZenryokuWazaStatus( attacker );

  // すでに全力ワザを使った後なら、もう撃てない
  if( zenryokuWazaStatus->IsZenryokuWazaUsed() ){
    return false;
  }

  // 全力ワザ条件を満たしていないので撃たない
  if( !this->isZenryokuWazaEnable(attacker, wazaIdx, zenryokuWazaStatus) ){
    return false;
  }

  // タイプ相性によっては撃たない選択をする（ランダム性あり）
  if( !this->shouldCancelByTypeAffinity(attacker, wazaIdx, targetPos) ){
    return false;
  }

  return true;
}

/**
 * @brief 特定ポケモンのクライアントが保持する全力ワザステータス情報を取得
 * @param[in]  attacker   ワザを撃つポケモンのパラメータ
 * @return     全力ワザステータス情報
 */
const ZenryokuWazaStatus* 
  SenarioAiZenryokuWazaSelector::getClientZenryokuWazaStatus( const BTL_POKEPARAM* attacker ) const

{
  const BTL_CLIENT*         client             = m_mainModule->GetClientByPokeID( attacker->GetID() );
  const ZenryokuWazaStatus* zenryokuWazaStatus = client->GetZenryokuWazaStatus();
  return zenryokuWazaStatus;
}

/**
 * @brief 選択されたワザを全力ワザとして使えるかどうか判定
 * @param[in]  attacker   ワザを撃つポケモンのパラメータ
 * @param[in]  wazaIdx    選択されたワザのインデックス
 * @param[in]  zenryokuWazaStatus  attacker のクライアントが保持する全力ワザステータス
 * @return     全力ワザとして使えるなら true
 */
bool SenarioAiZenryokuWazaSelector::isZenryokuWazaEnable( const BTL_POKEPARAM* attacker, u8 wazaIdx, const ZenryokuWazaStatus* zenryokuWazaStatus ) const
{
  // 使用条件を満たしていない
  if( !zenryokuWazaStatus->CanUseZenryokuWaza(*attacker, wazaIdx) ){
    return false;
  }

  // 元技のPPがゼロなら、ｚ技として選択できない
  if( attacker->WAZA_GetPP(wazaIdx) == 0 ){
    return false;
  }

  // NPCは専用全力技を使用できない！
  u8 clientID = m_mainModule->PokeIDtoClientID( attacker->GetID() );
  if( m_mainModule->IsClientNPC( clientID ) )
  {
    WazaNo wazano = attacker->WAZA_GetID( wazaIdx );
    ZenryokuWazaKind zenryokuWazaKind = ZenryokuWaza::GetZenryokuWazaKind( *attacker, wazano );
    if( zenryokuWazaKind == pml::waza::ZENRYOKUWAZA_KIND_SPECIAL ){
      return false;
    }
  }

  return true;
}

/**
 * @brief 選択されたワザと対象の相性に応じて、全力ワザ化のキャンセルを行うかどうかを判定する
 * @param[in]  attacker   ワザを撃つポケモンのパラメータ
 * @param[in]  wazaIdx    選択されたワザのインデックス
 * @param[in]  targetPos  ワザ対象の位置ID
 * @return     全力ワザ可をキャンセルする場合は true
 */
bool SenarioAiZenryokuWazaSelector::shouldCancelByTypeAffinity( const BTL_POKEPARAM* attacker, u8 wazaIdx, BtlPokePos targetPos ) const
{
  // 無効位置なら判定不能なのでキャンセル扱いにしない
  if( targetPos == BTL_POS_NULL ){
    return false;
  }

  const BTL_POKEPARAM* target   = m_pokecon->GetFrontPokeDataConst( targetPos );
  u8  attackerPokeID   = attacker->GetID();
  u8  targetPokeID     = target->GetID();
  WazaNo      wazano    = attacker->WAZA_GetID( wazaIdx );

  // 味方に撃つ場合、相性は重視せずに選ばれている可能性が高いので等倍扱いにする
  // 単体ポケモンを選択するワザでない場合も、ここで相性判定を行う意義が薄いので等倍扱いとする
  BtlTypeAff  affinity;
  if( m_mainModule->IsFriendPokeID(attackerPokeID, targetPokeID)
  ||  !isSingleTargetWaza(wazano)
  ){
    affinity  = pml::battle::TypeAffinity::TYPEAFF_1;
  }
  // 単体選択するタイプのワザを敵に撃つケースでは相性シミュレートを行う
  else
  {
    affinity = m_serverFlow->Hnd_SimulationAffinity( attackerPokeID, targetPokeID, wazano );
  }

  // 相性値をもとにランダム判定
  if( !this->decideZenryokuWazaUseAtRandom(affinity) ){
    return false;
  }

  return true;
}

/**
 * @brief 全力技のタイプ相性に応じた確率で、全力技を使用するかどうかを決定する
 * @param typeAffinity  全力技のタイプ相性
 * @retval true   全力技を使用する
 * @retval false  全力技を使用しない
 */
bool SenarioAiZenryokuWazaSelector::decideZenryokuWazaUseAtRandom( BtlTypeAff typeAffinity ) const
{
  //「全力技を使用しやすくなる」フラグが設定されているなら、タイプ相性が無効でなければ使用する
  if( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_AI_TEND_USE_ZPOWER ) )
  {
    return ( typeAffinity != pml::battle::TypeAffinity::TYPEAFF_0 );
  }

  u32 rand = m_randSys->Next(100);

  if( typeAffinity < pml::battle::TypeAffinity::TYPEAFF_1 )
  {
    return ( rand < 25 );
  }
  
  if( pml::battle::TypeAffinity::TYPEAFF_1 == typeAffinity )
  {
    return ( rand < 75 );
  }

  return ( rand < 80 );
}

/**
 * @brief 単体ポケモンを選択して使うワザかどうかを判定する
 * @param[in] wazaID  ワザナンバー
 * @retval 単体ポケモンを選択して使うワザなら true
 */
bool SenarioAiZenryokuWazaSelector::isSingleTargetWaza( WazaNo wazaID )
{
  pml::wazadata::WazaTarget  targetArea = (pml::wazadata::WazaTarget)(WAZADATA_GetParam( wazaID, pml::wazadata::PARAM_ID_TARGET ));
  switch( targetArea ){
  case pml::wazadata::TARGET_OTHER_SELECT:       ///< 通常ポケ（１体選択）
  case pml::wazadata::TARGET_FRIEND_USER_SELECT: ///< 自分を含む味方ポケ（１体選択）
  case pml::wazadata::TARGET_FRIEND_SELECT:      ///< 自分以外の味方ポケ（１体選択）
  case pml::wazadata::TARGET_ENEMY_SELECT:       ///< 相手側ポケ（１体選択）
    return true;

  default:
    return false;
  }
}



GFL_NAMESPACE_END( btl )
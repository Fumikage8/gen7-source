//=============================================================================================
/**
 * @file    btl_AffCounter.cpp
 * @brief   ポケモンXY バトルシステム ワザ相性カウンタ（バトル検定用）
 * @author  taya
 *
 * @date  2011.01.07  作成
 */
//=============================================================================================

#include  "btl_MainModule.h"
#include  "btl_AffCounter.h"

GFL_NAMESPACE_BEGIN(btl)

//=========================================================
/**
 *  コンストラクタ・デストラクタ
 */
//=========================================================
AffCounter::AffCounter( void )
 :
  m_putVoid(0),
  m_putAdvantage(0),
  m_putDisadvantage(0),
  m_recvVoid(0),
  m_recvAdvantage(0),
  m_recvDisadvantage(0)
{

}

//=============================================================================================
/**
 * カウンタクリア
 */
//=============================================================================================
void AffCounter::Clear( void )
{
  m_putVoid = 0;
  m_putAdvantage = 0;
  m_putDisadvantage = 0;
  m_recvVoid = 0;
  m_recvAdvantage = 0;
  m_recvDisadvantage = 0;
}

//=============================================================================================
/**
 * 攻撃結果に応じてカウンタインクリメント
 *
 * @param   mainModule  参照する MainModule
 * @param   attacker
 * @param   defender
 * @param   affinity
 */
//=============================================================================================
void AffCounter::CountUp( const MainModule& mainModule, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affinity )
{
  u8 atkPokeID = attacker->GetID();
  u8 defPokeID = defender->GetID();

  u8 atkClientID = MainModule::PokeIDtoClientID( atkPokeID );
  u8 defClientID = MainModule::PokeIDtoClientID( defPokeID );

  u8 fEnemyAttack = !(mainModule.IsFriendPokeID( atkPokeID, defPokeID ));

  u16* pCnt = NULL;

  // プレイヤーが攻撃側
  if( atkClientID == BTL_CLIENT_PLAYER )
  {
    if( fEnemyAttack )
    {
      if( affinity == pml::battle::TypeAffinity::TYPEAFF_0 )
      {
        pCnt = &(m_putVoid);
      }
      else if( affinity > pml::battle::TypeAffinity::TYPEAFF_1 )
      {
        pCnt = &(m_putAdvantage);
      }
      else if( affinity < pml::battle::TypeAffinity::TYPEAFF_1 )
      {
        pCnt = &(m_putDisadvantage);
      }
    }
  }
  // プレイヤーが防御側
  else if( defClientID == BTL_CLIENT_PLAYER )
  {
    if( fEnemyAttack )
    {
      if( affinity == pml::battle::TypeAffinity::TYPEAFF_0 )
      {
        pCnt = &(m_recvVoid);
      }
      else if( affinity > pml::battle::TypeAffinity::TYPEAFF_1 )
      {
        pCnt = &(m_recvAdvantage);
      }
      else if( affinity < pml::battle::TypeAffinity::TYPEAFF_1 )
      {
        pCnt = &(m_recvDisadvantage);
      }
    }
  }

  if( pCnt != NULL )
  {
    if( (*pCnt) < COUNTER_MAX ){
      ++(*pCnt);
    }
  }
}
GFL_NAMESPACE_END(btl)





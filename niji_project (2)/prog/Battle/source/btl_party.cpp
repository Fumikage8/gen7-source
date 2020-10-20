//=============================================================================================
/**
 * @file  btl_party.h
 * @brief ポケモンXY バトルシステム ポケモンパーティクラス
 * @author  taya
 *
 * @date  2010.12.18  作成
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_mainmodule.h"
#include "btl_party.h"

GFL_NAMESPACE_BEGIN(btl)

//-----------------------------------------------------------------------
/**
 *  constructor / destructor
 */
//-----------------------------------------------------------------------
BTL_PARTY::BTL_PARTY( void )
 : m_memberCount(0), m_numCoverPos(0)
{
  for(u32 i=0; i<GFL_NELEMS(m_pMember); ++i)
  {
    m_pMember[ i ] = NULL;
  }
}

BTL_PARTY::~BTL_PARTY()
{

}


//=============================================================================================
/**
 * 内容を初期化
 */
//=============================================================================================
void BTL_PARTY::Initialize( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_pMember); ++i)
  {
    m_pMember[ i ] = NULL;
  }
  m_memberCount = 0;
}



//==============================================================================
/**
 * メンバー１体追加
 *
 * @param[in]   member    追加するメンバーポインタ
 */
//==============================================================================
void BTL_PARTY::AddMember( BTL_POKEPARAM* member )
{
  GFL_ASSERT(m_memberCount < BTL_PARTY_MEMBER_MAX );

  m_pMember[ m_memberCount++ ] = member;
}
//==============================================================================
/**
 * 戦える（生きていてタマゴでない）ポケモンを先頭に移動（初期化後に１度だけ行う）
 */
//==============================================================================
void BTL_PARTY::MoveAlivePokeToFirst( void )
{
  u32 idx, checkedCount;

  idx = checkedCount = 0;
  while( checkedCount < m_memberCount )
  {
    if( m_pMember[idx]->IsFightEnable() ){
      ++idx;
    }
    else{
      MoveLastMember( idx );
    }
    ++checkedCount;
  }
}
//==============================================================================
/**
 *  メンバーを最後尾に移動（初期化時に戦えないポケモンに適用)
 *
 * @param[in]   idx
 */
//==============================================================================
void BTL_PARTY::MoveLastMember( u8 idx )
{
  GFL_ASSERT(idx<m_memberCount);

  {
    BTL_POKEPARAM* tmp = m_pMember[ idx ];
    while( idx < (m_memberCount-1) )
    {
      m_pMember[ idx ] = m_pMember[ idx + 1 ];
      ++idx;
    }
    m_pMember[ idx ] = tmp;
  }
}
//==============================================================================
/**
 * メンバー数を返す
 *
 * @retval  u8  メンバー数
 */
//==============================================================================
u8 BTL_PARTY::GetMemberCount( void ) const
{
  return m_memberCount;
}
//==============================================================================
/**
 *  戦えるメンバー数を返す
 *
 * @retval  u8  戦えるメンバー数
 */
//==============================================================================
u8 BTL_PARTY::GetAliveMemberCount( void ) const
{
  int i, cnt;

  for(i=0, cnt=0; i<m_memberCount; i++)
  {
    if( m_pMember[i]->IsFightEnable() ){
      ++cnt;
    }
  }
  return cnt;
}
//==============================================================================
/**
 * 指定Index以降の戦えるメンバー数を返す
 *
 * @param   startIdx
 *
 * @retval  u8
 */
//==============================================================================
u8 BTL_PARTY::GetAliveMemberCountRear( u8 startIdx ) const
{
  int i, cnt;
  for(i=startIdx, cnt=0; i<m_memberCount; i++)
  {
    if( m_pMember[i]->IsFightEnable() ){
      ++cnt;
    }
  }
  return cnt;
}
//===============================================================================
/**
 * ひん死のメンバー数を返す
 *
 * @retval  u8    ひん死のメンバー数
 */
//===============================================================================
u8 BTL_PARTY::GetDeadMemberCount( void ) const
{
  u8 aliveCount = this->GetAliveMemberCount();
  if( aliveCount <= m_memberCount ){
    return m_memberCount - aliveCount;
  }
  GFL_ASSERT(0);
  return 0;
}

//===============================================================================
/**
 * @brief メンバー数が最大か？
 * @retval true   メンバー数が最大
 * @retval false  メンバー数が最大でない
 */
//===============================================================================
bool BTL_PARTY::IsFull( void ) const
{
  return ( BTL_PARTY_MEMBER_MAX <= this->GetMemberCount() );
}

//==============================================================================
/**
 * メンバーIndex からメンバーデータポインタ取得
 *
 * @param   party
 * @param   idx
 *
 * @retval  BTL_POKEPARAM*
 */
//==============================================================================
BTL_POKEPARAM* BTL_PARTY::GetMemberData( u8 idx )
{
  if( idx < m_memberCount ){
    return m_pMember[ idx ];
  }
  return NULL;
}
//==============================================================================
/**
 * メンバーIndex からメンバーデータポインタ取得（const版）
 *
 * @param   idx
 *
 * @retval  BTL_POKEPARAM*
 */
//==============================================================================
const BTL_POKEPARAM* BTL_PARTY::GetMemberDataConst( u8 idx ) const
{
  if( idx < m_memberCount ){
    return m_pMember[ idx ];
  }
  return NULL;
}
//==============================================================================
/**
 * メンバー入れ替え
 *
 * @param   idx1
 * @param   idx2
 */
//==============================================================================
void BTL_PARTY::SwapMembers( u8 idx1, u8 idx2 )
{
  GFL_ASSERT(idx1<m_memberCount);
  GFL_ASSERT(idx2<m_memberCount);
  {
    BTL_POKEPARAM* tmp = m_pMember[ idx1 ];
    m_pMember[ idx1 ] = m_pMember[ idx2 ];
    m_pMember[ idx2 ] = tmp;
//    BTL_N_PrintfEx(true, DBGSTR_MAIN_SwapPartyMember, idx1, idx2);
  }
}
//==============================================================================
/**
 * 特定ポケモンのパーティ内Indexを返す（データポインタ指定）
 *
 * @param   party
 * @param   pokeID
 *
 * @retval  int   パーティ内Index（パーティに指定IDのポケモンが存在しない場合 -1）
 */
//==============================================================================
int BTL_PARTY::FindMember( const BTL_POKEPARAM* param ) const
{
  int i;
  for(i=0; i<m_memberCount; ++i)
  {
    if( m_pMember[i] == param )
    {
      return i;
    }
  }
  return -1;
}
//==============================================================================
/**
 * 特定ポケモンのパーティ内Indexを返す（PokeID指定）
 *
 * @param   party
 * @param   pokeID
 *
 * @retval  int   パーティ内Index（パーティに指定IDのポケモンが存在しない場合 -1）
 */
//==============================================================================
int BTL_PARTY::FindMemberByPokeID( u8 pokeID ) const // GetMemberIndex と重複している
{
  int i;
  for(i=0; i<m_memberCount; i++)
  {
    if( m_pMember[i]->GetID() == pokeID ){
      return i;
    }
  }
  return -1;
}
//==============================================================================
/**
 * 生きていて戦える先頭のポケモンデータを返す
 *
 * @param   party
 *
 * @retval  int   パーティ内Index（存在しない場合 NULL）
 */
//==============================================================================
BTL_POKEPARAM* BTL_PARTY::GetAliveTopMember( void )
{
  for(int i=0; i<m_memberCount; ++i)
  {
    if( m_pMember[i]->IsFightEnable() ){
      return m_pMember[i];
    }
  }
  return NULL;
}
//==============================================================================
/**
 * メンバーのIndexを返す
 * @param pokeID
 * @return メンバーIndex（メンバーに含まれていない時 BTL_PARTY_MEMBER_MAX）
 */
//==============================================================================
u8 BTL_PARTY::GetMemberIndex( u8 pokeID ) const
{
  for(int i=0; i<m_memberCount; ++i)
  {
    if( m_pMember[i]->GetID() == pokeID ){
      return i;
    }
  }
  return BTL_PARTY_MEMBER_MAX;
}

//==============================================================================
/**
 * @brief 特性「イリュージョン」の対象となるポケモンのインデックスを返す
 * @retval 0  特性「イリュージョン」が有効でない場合
 */
//==============================================================================
u8 BTL_PARTY::GetIllusionTargetMemberIndex( void ) const
{
  // 最後尾にいて生きてるポケがイリュージョン対象
  const u8 memberNum = this->GetMemberCount();
  for( u8 memberIndex=(memberNum-1); memberIndex>0; --memberIndex )
  {
    const BTL_POKEPARAM* member = this->GetMemberDataConst( memberIndex );
    if( member->IsFightEnable() ) {
      return memberIndex;
    }
  }
  return 0;
}

//==============================================================================
/**
 * @brief 倒したポケモンの数の合計を取得する
 */
//==============================================================================
u16 BTL_PARTY::GetTotalKillCount( void ) const
{
  u16 killCount = 0;
 
  const u8 memberCount = this->GetMemberCount();
  for( u8 memberIndex=0; memberIndex<memberCount; ++memberIndex )
  {
    const BTL_POKEPARAM* pokeParam = this->GetMemberDataConst( memberIndex );
    killCount += pokeParam->GetKillCount();
  }

  return killCount;
}


GFL_NAMESPACE_END(btl)






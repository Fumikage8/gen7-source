//==============================================================================
/**
 * @file    PokeListIntermediateData.cpp
 * @author  fukushima_yuya
 * @date    2015.03.31
 * @brief   PokeParamを取得するための中間データクラス
 */
//==============================================================================


// niji
#include "App/PokeList/include/PokeListIntermediateData.h"

#include <Battle/source/btlv/btlv_core.h>

// pml
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
IntermediateData::IntermediateData( void )
: m_pPokeParty( NULL )
, m_pBtlParty( NULL )
, m_CreateBattleData( false )
{
  for( u8 i=0; i<MEMBER_MAX; ++i )
  {
    m_pBattleParam[i] = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
IntermediateData::~IntermediateData( void )
{
  DeleteBattleData();
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのセット
 * @param   pPokeParty    ポケモンパーティ
 */
//------------------------------------------------------------------------------
void IntermediateData::SetParameter( pml::PokeParty* pPokeParty )
{
  m_pPokeParty = pPokeParty;
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのセット
 * @param   pPokeParty    ポケモンパーティ
 */
//------------------------------------------------------------------------------
void IntermediateData::SetParameter( const pml::PokeParty* pPokeParty )
{
  m_cpPokeParty = pPokeParty;
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのセット
 * @param   pBtlParty     バトルシステム　ポケモンパーティ
 */
//------------------------------------------------------------------------------
void IntermediateData::SetParameter( const btl::BTL_PARTY* pBtlParty )
{
  m_pBtlParty = pBtlParty;
}
void IntermediateData::SetParameter( const btl::BTL_PARTY* pBtlParty, gfl2::heap::HeapBase* heap )
{
  m_pBtlParty = pBtlParty;

  u8 memberCount = m_pBtlParty->GetMemberCount();

  // バトル用パラメータの破棄
  DeleteBattleData();

  // バトル用パラメータの生成
  CreateBattleData( heap, memberCount );

  // バトル用パラメータの設定
  for( u8 i=0; i<memberCount; ++i )
  {
    SetBattleData( i );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   パラメータが無効かどうか
 *
 * @return  "true  == 無効"
 * @return  "false == 有効"
 */
//------------------------------------------------------------------------------
bool IntermediateData::IsInvalid( void ) const
{
  if( m_pPokeParty )  return false;
  if( m_cpPokeParty ) return false;
  if( m_pBtlParty )   return false;
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパーティの取得
 * @return  pml::PokePartyクラスのポインタ
 */
//------------------------------------------------------------------------------
pml::PokeParty* IntermediateData::GetPokeParty( void )
{
  return m_pPokeParty;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパーティの取得
 * @return  const pml::PokePartyクラスのポインタ
 */
//------------------------------------------------------------------------------
const pml::PokeParty* IntermediateData::GetPokePartyConst( void )
{
  return m_cpPokeParty;
}

//------------------------------------------------------------------------------
/**
   * @brief   バトル用ポケモンパーティの取得
   * @return  btl::BTL_PARTYクラスのポインタ
 */
//------------------------------------------------------------------------------
const btl::BTL_PARTY* IntermediateData::GetBtlParty( void )
{
  return m_pBtlParty;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの取得
 * @return  pml::pokepara::PokemonParamクラスのポインタ
 */
//------------------------------------------------------------------------------
pml::pokepara::PokemonParam* IntermediateData::GetPoke( u8 poke_index )
{
  if( m_pPokeParty )
  {
    return m_pPokeParty->GetMemberPointer( poke_index );
  }
  else if( m_cpPokeParty )
  {
    //return m_cpPokeParty->GetMemberPointer( poke_index );
    return NULL;
  }
  else if( m_pBtlParty )
  {
    //  やりたくはないがcastでconst外しをする
    //btl::BTL_PARTY* pBtlPart  = const_cast<btl::BTL_PARTY*>( m_pBtlParty );

    //btl::BTL_POKEPARAM* pBtlPokeParam = pBtlPart->GetMemberData( poke_index );
    //return const_cast<pml::pokepara::PokemonParam*>( pBtlPokeParam->GetSrcData() );

    return m_pBattleParam[poke_index];
  }

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンの取得
 * @return  const pml::pokepara::PokemonParamクラスのポインタ
 */
//------------------------------------------------------------------------------
const pml::pokepara::PokemonParam* IntermediateData::GetPokeConst( u8 poke_index )
{
  if( m_pPokeParty )
  {
    return m_pPokeParty->GetMemberPointerConst( poke_index );
  }
  else if( m_cpPokeParty )
  {
    return m_cpPokeParty->GetMemberPointerConst( poke_index );
  }
  else if( m_pBtlParty )
  {
    //const btl::BTL_POKEPARAM* pBtlPokeParam = m_pBtlParty->GetMemberDataConst( poke_index );
    //return pBtlPokeParam->GetSrcData();
    return m_pBattleParam[poke_index];
  }

  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ポケモンの取得(バトル専用)
 * @return  const pml::pokepara::PokemonParamクラスのポインタ
 */
//------------------------------------------------------------------
const pml::pokepara::PokemonParam* IntermediateData::GetPoke_Battle( u8 poke_index )
{
  if( m_pBtlParty )
  {
    const btl::BTL_POKEPARAM* pBtlPokeParam = m_pBtlParty->GetMemberDataConst( poke_index );
    if( pBtlPokeParam != NULL )
    {
      return pBtlPokeParam->GetSrcData();
    }
  }

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief   パーティのメンバー数の取得
 * @return  パーティのメンバー数
 */
//------------------------------------------------------------------------------
u32 IntermediateData::GetMemberCount( void )
{
  if( m_pPokeParty )
  {
    return m_pPokeParty->GetMemberCount();
  }
  else if( m_cpPokeParty )
  {
    return m_cpPokeParty->GetMemberCount();
  }
  else if( m_pBtlParty )
  {
    return static_cast<u32>( m_pBtlParty->GetMemberCount() );
  }

  // パーティ情報が登録されていない
  GFL_ASSERT(0);
  return 0;
}



//------------------------------------------------------------------------------
/**
 * @brief   バトル用パラメータの生成
 */
//------------------------------------------------------------------------------
void IntermediateData::CreateBattleData( gfl2::heap::HeapBase * heap, u8 memberCount )
{
  if( m_CreateBattleData ) return;

  btl::BTL_PARTY* pBtlPart = const_cast<btl::BTL_PARTY*>( m_pBtlParty );

  for( u8 i=0; i<memberCount; ++i )
  {
    btl::BTL_POKEPARAM* pBtlPokeParam = pBtlPart->GetMemberData( i );
    const pml::pokepara::PokemonParam* pp = pBtlPokeParam->GetSrcData();

    // 受け渡し用のポケモンパラメータの生成
    //  一時使用なのでヒープは後方参照にしておく
    m_pBattleParam[i] = GFL_NEW_LOW(heap) pml::pokepara::PokemonParam( heap->GetLowerHandle(), *pp );
  }

  m_CreateBattleData = true;
}

//------------------------------------------------------------------------------
/**
 * @brief   バトル用パラメータの破棄
 */
//------------------------------------------------------------------------------
void IntermediateData::DeleteBattleData( void )
{
  if( !m_CreateBattleData ) return;

  for( u8 i=0; i<MEMBER_MAX; ++i )
  {
    GFL_SAFE_DELETE( m_pBattleParam[i] );
  }

  m_CreateBattleData = false;
}

//------------------------------------------------------------------------------
/**
 * @brief   戦闘中のデータの反映
 */
//------------------------------------------------------------------------------
void IntermediateData::SetBattleData( u8 index )
{
  if( !m_CreateBattleData ) return;

  // バトル用のデータを取得
  btl::BTL_PARTY* pBtlPart  = const_cast<btl::BTL_PARTY*>( m_pBtlParty );
  btl::BTL_POKEPARAM* pBtlPokeParam = pBtlPart->GetMemberData( index );

  // 戦闘時のデータを反映
  bool fastmode_flag = m_pBattleParam[index]->StartFastMode();
  {
    m_pBattleParam[index]->SetExp( pBtlPokeParam->GetValue( btl::BTL_POKEPARAM::BPP_EXP ) );
    m_pBattleParam[index]->SetHp( pBtlPokeParam->GetValue( btl::BTL_POKEPARAM::BPP_HP ) );
    m_pBattleParam[index]->SetSick( pBtlPokeParam->GetPokeSick() );
    m_pBattleParam[index]->SetItem( pBtlPokeParam->GetItem() );

    // @fix GFNMCat[3645] 技のＰＰ値もバトルから取得して設定しないとＰＰ回復した時に反映しない
    u32 wazaNum = pBtlPokeParam->WAZA_GetCount_Org();
    for( u32 waza_i = 0; waza_i < wazaNum; ++waza_i )
    {
      m_pBattleParam[ index ]->SetWaza( waza_i, pBtlPokeParam->WAZA_GetID_Org( waza_i ) );

      // GFNMCat[5019]
      m_pBattleParam[ index ]->SetWazaPPUpCount(  waza_i, pBtlPokeParam->GetSrcData()->GetWazaPPUpCount(waza_i) );

      m_pBattleParam[ index ]->SetWazaPP( waza_i, pBtlPokeParam->WAZA_GetPP_Org( waza_i ) );
    }
  }
  m_pBattleParam[index]->EndFastMode( fastmode_flag );
}


//------------------------------------------------------------------
/**
 * @brief   状態異常・猛毒か
 * @retval  true  = 猛毒
 * @retval  false = それ以外
 */
//------------------------------------------------------------------
bool IntermediateData::IsSickMoudoku( u8 index )
{
  if( m_pBtlParty == NULL )
  {
    return false;
  }
  return m_pBtlParty->GetMemberDataConst(index)->CheckMoudoku();
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

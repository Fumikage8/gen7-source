//======================================================================
/**
 * @file    StatusPokemonContainer.cpp
 * @date    2015/12/01 19:11:08
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ポケモンコンテナ
 * note     ポケパーティとボックスの違いに関係なくポケモンパラメータを取得するためのクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusPokemonContainer.h"

#include <Savedata/include/BoxPokemonSave.h>

// pml
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   workHeap    ヒープ
 */
//------------------------------------------------------------------------------
PokemonContainer::PokemonContainer( gfl2::heap::HeapBase* workHeap )
  : m_pHeap( workHeap )
  , m_pParty( NULL )
  , m_pBoxData( NULL )
  , m_BoxTrayIndex( 0 )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   パーティデータのセット
 * @param   party   ポケパーティ
 */
//------------------------------------------------------------------------------
pml::PokeParty* PokemonContainer::GetPokeParty( void ) const
{
  return m_pParty;
}


//------------------------------------------------------------------------------
/**
 * @brief   パーティデータのセット
 * @param   party   ポケパーティ
 */
//------------------------------------------------------------------------------
void PokemonContainer::SetPokeParty( pml::PokeParty* party )
{
  m_pParty = party;
  m_pMethod_GetPokemon = &PokemonContainer::GetPokemonFromPokeParty;
  m_pMethod_NextPokeIndex = &PokemonContainer::GetNextPokeIndexFromPokeParty;
  m_pMethod_PrevPokeIndex = &PokemonContainer::GetPrevPokeIndexFromPokeParty;
  m_pMethod_UpdatePokemon = &PokemonContainer::UpdatePokemonOfPokeParty;
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスデータのセット
 * @param   boxData     ボックスデータ
 * @param   trayIndex   トレイインデックス
 */
//------------------------------------------------------------------------------
void PokemonContainer::SetBoxData( Savedata::BoxPokemon* boxData, u32 trayIndex )
{
  m_pBoxData = boxData;
  m_pMethod_GetPokemon = &PokemonContainer::GetPokemonFromBoxData;
  m_pMethod_NextPokeIndex = &PokemonContainer::GetNextPokeIndexFromBoxData;
  m_pMethod_PrevPokeIndex = &PokemonContainer::GetPrevPokeIndexFromBoxData;
  m_pMethod_UpdatePokemon = &PokemonContainer::UpdatePokemonOfBoxData;
  m_BoxTrayIndex = trayIndex;
}


//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの取得
 * @param   index   インデックス
 * @param   buffer  ポケモンパラメータの格納先
 */
//------------------------------------------------------------------------------
void PokemonContainer::GetPokemon( u32 index, pml::pokepara::PokemonParam* buffer )
{
  GFL_ASSERT_STOP( m_pMethod_GetPokemon );

  ( this->*m_pMethod_GetPokemon )( index, buffer );
}

//------------------------------------------------------------------------------
/**
 * @brief   次のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  次のインデックス
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetNextPokeIndex( u32 currentIndex )
{
  GFL_ASSERT_STOP( m_pMethod_NextPokeIndex );

  return ( this->*m_pMethod_NextPokeIndex )( currentIndex );
}

//------------------------------------------------------------------------------
/**
 * @brief   前のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  前のインデックス
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetPrevPokeIndex( u32 currentIndex )
{
  GFL_ASSERT_STOP( m_pMethod_PrevPokeIndex );

  return ( this->*m_pMethod_PrevPokeIndex )( currentIndex );
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの更新
 * @param   index   更新対象インデックス
 * @param   pp      更新するパラメータ
 */
//------------------------------------------------------------------------------
void PokemonContainer::UpdatePokemon( u32 index, const pml::pokepara::PokemonParam& pp )
{
  GFL_ASSERT_STOP( m_pMethod_UpdatePokemon );

  ( this->*m_pMethod_UpdatePokemon )( index, pp );
}


//==============================================================================
/**
 * @brief   ポケパーティ
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの取得
 * @param   index   インデックス
 * @param   buffer  ポケモンパラメータの格納先
 */
//------------------------------------------------------------------------------
void PokemonContainer::GetPokemonFromPokeParty( u32 index, pml::pokepara::PokemonParam* buffer )
{
  const pml::pokepara::PokemonParam* pp = m_pParty->GetMemberPointerConst( index );

  buffer->CopyFrom( *pp );
}

//------------------------------------------------------------------------------
/**
 * @brief   次のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  次のインデックス
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetNextPokeIndexFromPokeParty( u32 currentIndex )
{
  u32 memberCount = m_pParty->GetMemberCount();

  currentIndex = (currentIndex + 1) % memberCount;

  return currentIndex;
}

//------------------------------------------------------------------------------
/**
 * @brief   前のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  前のインデックス
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetPrevPokeIndexFromPokeParty( u32 currentIndex )
{
  u32 memberCount = m_pParty->GetMemberCount();

  if( currentIndex == 0 )
  {
    currentIndex = memberCount - 1;
  }
  else {
    currentIndex--;
  }

  return currentIndex;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの更新
 * @param   index   更新対象インデックス
 * @param   pp      更新するパラメータ
 */
//------------------------------------------------------------------------------
void PokemonContainer::UpdatePokemonOfPokeParty( u32 index, const pml::pokepara::PokemonParam& pp )
{
  m_pParty->GetMemberPointer(index)->CopyFrom( pp );
}


//==============================================================================
/**
 * @brief   ボックスデータ
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの取得
 * @param   index   インデックス
 * @param   buffer  ポケモンパラメータの格納先
 */
//------------------------------------------------------------------------------
void PokemonContainer::GetPokemonFromBoxData( u32 index, pml::pokepara::PokemonParam* buffer )
{
  m_pBoxData->GetPokemon( buffer, m_BoxTrayIndex, index );
}

//------------------------------------------------------------------------------
/**
 * @brief   次のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  次のインデックス
 *
 * @note    次のポケモンがいなければさらに次へ進め、いたポケモンのインデックスを記憶する。
 *          最後まで検索して見つからない場合は大丈夫なインデックスを返す
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetNextPokeIndexFromBoxData( u32 currentIndex )
{
  pml::pokepara::CoreParam param( m_pHeap );

  u32 index = currentIndex;
  u32 safeIndex = index;
  bool res = false;

  // 次のポケモンへ進める
  do
  {
    ++index;  //!< 次のインデックスへ

    // 最後まで検索して見つからなければ大丈夫な番号にする
    if( index >= Savedata::BoxPokemon::TRAY_POKE_MAX )
    {
      index = safeIndex;    //!< 大丈夫な番号にする
      break;
    }

    // ポケモンが存在するかをチェック
    res = m_pBoxData->CheckPokemon( m_BoxTrayIndex, index, &param );

    // 存在する場合は大丈夫なインデックスを更新する
    if( res ) safeIndex = index;

  } while( !res );

  return index;
}

//------------------------------------------------------------------------------
/**
 * @brief   前のインデックスの取得
 * @param   currentIndex    現在のインデックス
 * @return  前のインデックス
 *
 * @note    前のポケモンがいなければさらに前へ戻し、いたポケモンのインデックスを記憶する。
 *          先頭まで検索して見つからない場合は大丈夫なインデックスを返す
 */
//------------------------------------------------------------------------------
u32 PokemonContainer::GetPrevPokeIndexFromBoxData( u32 currentIndex )
{
  pml::pokepara::CoreParam param( m_pHeap );

  s32 index = static_cast<s32>(currentIndex);
  u32 safeIndex = index;
  bool res = false;

  // 前のポケモンへ戻る
  do
  {
    --index;  //!< 前のインデックスへ

    // 先頭まで検索して見つからなければ大丈夫な番号にする
    if( index < 0 )
    {
      index = safeIndex;    //!< 大丈夫な番号にする
      break;
    }

    // ポケモンが存在するかをチェック
    res = m_pBoxData->CheckPokemon( m_BoxTrayIndex, index, &param );

    // 存在する場合は大丈夫なインデックスを更新する
    if( res ) safeIndex = index;

  } while( !res );

  return static_cast<u32>(index);
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンパラメータの更新
 * @param   index   更新対象インデックス
 * @param   pp      更新するパラメータ
 */
//------------------------------------------------------------------------------
void PokemonContainer::UpdatePokemonOfBoxData( u32 index, const pml::pokepara::PokemonParam& pp )
{
  m_pBoxData->UpdatePokemonConst( &pp, m_BoxTrayIndex, index );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

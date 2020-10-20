//=============================================================================
/**
 * @file    UnionPokemonSave.cpp
 * @brief   セーブデータ：合体ポケモンの退避領域
 * @author  Hiroyuki Nakamura
 * @date    2012.06.13
 */
//=============================================================================

// module
#include "Savedata/include/UnionPokemonSave.h"
// gflib2
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(Savedata)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
UnionPokemon::UnionPokemon(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
UnionPokemon::~UnionPokemon()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   セーブデータのクリア
 *
 * @param   heap  クリアに使用するテンポラリヒープ
 */
//-----------------------------------------------------------------------------
void UnionPokemon::Clear( gfl2::heap::HeapBase * heap )
{
  pml::pokepara::PokemonParam * pp = GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_NULL, 0, 0 );

  for( u32 i=0; i<POKEID_MAX; i++ )
  {
    pp->Serialize_Full( poke.pokemonParam[i] );
  }

  GFL_DELETE pp;
}

//-----------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   id    データID
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void UnionPokemon::InitPokemonData( PokemonID id, gfl2::heap::HeapBase * heap )
{
  pml::pokepara::PokemonParam * pp = GFL_NEW_LOW( heap ) pml::pokepara::PokemonParam( heap, MONSNO_NULL, 0, 0 );
  pp->Serialize_Full( poke.pokemonParam[id] );
  GFL_DELETE pp;
}

//-----------------------------------------------------------------------------
/**
 * @brief   PokemonParamをセット
 *
 * @param   id  データID
 * @param   pp  セットするPokemonParam
 */
//-----------------------------------------------------------------------------
void UnionPokemon::SetPokemon( PokemonID id, pml::pokepara::PokemonParam * pp )
{
#if PM_DEBUG
  MonsNo mons = pp->GetMonsNo();
  switch( id )
  {
  case POKEID_KYUREMU:
    if( mons != MONSNO_ZEKUROMU && mons != MONSNO_RESIRAMU )
    {
      GFL_ASSERT( 0 );
    }
    break;
  case POKEID_NEKUROZUMA_S:
    if( mons != MONSNO_NIKKOU )
    {
      GFL_ASSERT( 0 );
    }
    break;
  case POKEID_NEKUROZUMA_L:
    if( mons != MONSNO_GEKKOU )
    {
      GFL_ASSERT( 0 );
    }
    break;
  }
#endif  // PM_DEBUG
  pp->Serialize_Full( poke.pokemonParam[id] );
}

//-----------------------------------------------------------------------------
/**
 * @brief   PokemonParamを取得
 *
 * @param   id  データID
 * @param   pp  取得場所
 */
//-----------------------------------------------------------------------------
void UnionPokemon::GetPokemon( PokemonID id, pml::pokepara::PokemonParam * pp )
{
  pp->Deserialize_Full( poke.pokemonParam[id] );
}


GFL_NAMESPACE_END(Savedata)

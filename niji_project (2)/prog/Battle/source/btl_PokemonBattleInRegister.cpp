//=============================================================================
/**
 * @file   btl_PokemonBattleInRegister.cpp
 * @date   2015/08/21 19:10:44
 * @author obata_toshihiro
 * @brief  １ターンの期間内において、各ポケモンが場に出たかどうかの記録
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_PokemonBattleInRegister.h"

GFL_NAMESPACE_BEGIN( btl )




//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
PokemonBattleInRegister::PokemonBattleInRegister( void )
{
  this->Clear();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
PokemonBattleInRegister::~PokemonBattleInRegister()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンが場に出た( 出ていた )ことを記録する
 * @param pokeId  記録対象ポケモンのID
 */
//-----------------------------------------------------------------------------
void PokemonBattleInRegister::Register( u8 pokeId )
{
  if( pokeId == BTL_POKEID_NULL ) 
  {
    return;
  }

  if( BTL_POKEID_MAX <= pokeId )
  {
    GFL_ASSERT(0);
    return;
  }

  m_isPokemonBattleIn[ pokeId ] = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンが場に出た( 出ていた )かどうか取得する
 * @param pokeId  チェック対象ポケモンのID
 * @retval true   場に出た( 出ていた )
 * @retval false  場に出ていない
 */
//-----------------------------------------------------------------------------
bool PokemonBattleInRegister::Check( u8 pokeId ) const
{
  if( pokeId == BTL_POKEID_NULL ) 
  {
    return false;
  }

  if( BTL_POKEID_MAX <= pokeId )
  {
    GFL_ASSERT(0);
    return false;
  }

  return m_isPokemonBattleIn[ pokeId ];
}

//-----------------------------------------------------------------------------
/**
 * @brief 記録をクリアする
 */
//-----------------------------------------------------------------------------
void PokemonBattleInRegister::Clear( void )
{
  for( u32 i=0; i<BTL_POKEID_MAX; ++i )
  {
    m_isPokemonBattleIn[i] = false;
  }
}



GFL_NAMESPACE_END( btl )
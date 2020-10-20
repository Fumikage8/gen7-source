//=============================================================================
/**
 * @file   btl_DiarySave.cpp
 * @date   2016/02/17 17:46:14
 * @author obata_toshihiro
 * @brief  ポケモンダイアリー
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Savedata/include/PokeDiarySave.h>
#include "./btl_DiarySave.h"
#include "./btl_pokeparam.h"

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief ダイアリー( 努力値が最大になった )をセットする
 * @param poke  努力値を取得したポケモン
 */
//-----------------------------------------------------------------------------
void DiarySave::SetEffortValueFull( const BTL_POKEPARAM& poke )
{
  u16 monsno = poke.GetMonsNo();
  PokeDiary::Set( PDID_COUNT_EFFORT, monsno );
}

//-----------------------------------------------------------------------------
/**
 * @brief ダイアリー( レベルアップした )をセットする
 * @param poke  レベルアップしたポケモン
 */
//-----------------------------------------------------------------------------
void DiarySave::SetLevelUp( const BTL_POKEPARAM& poke )
{
  u16 monsno = poke.GetMonsNo();
  PokeDiary::Set( PDID_COUNT_LVUP, monsno );
}


GFL_NAMESPACE_END( btl )
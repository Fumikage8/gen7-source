//=============================================================================
/**
 * @file   btl_RecordSave.cpp
 * @date   2016/02/15 18:22:51
 * @author obata_toshihiro
 * @brief  レコードデータ
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <pml/include/item/item.h>
#include <Savedata/include/Record.h>
#include "./btl_mainmodule.h"
#include "./btl_pokeparam.h"
#include "./btl_RecordSave.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief 技を使用した回数をインクリメントする
 * @param attacker   技を使用したポケモン
 * @param zWazaKind  使用した技の全力技分類
 */
//-----------------------------------------------------------------------------
void RecordSave::IncWazaUseCount( const MainModule& mainModule, const BTL_POKEPARAM& attacker, ZenryokuWazaKind zWazaKind )
{
  // @fix NMCat[232] 主人公以外のCPUが使用した「Z技」の回数がレコードデータに加算されている
  const u8 playerClientID = mainModule.GetPlayerClientID();
  const u8 attackerClientID = MainModule::PokeIDtoClientID( attacker.GetID() );
  if( playerClientID != attackerClientID )
  {
    return;
  }

  mainModule.RECORDDATA_Inc( Savedata::Record::RECID_WAZA_CNT );

  if( zWazaKind != pml::waza::ZENRYOKUWAZA_KIND_NULL )
  {
    mainModule.RECORDDATA_Inc( Savedata::Record::RECID_ZWAZA_CNT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ボールを投げた回数をインクリメントする
 */
//-----------------------------------------------------------------------------
void RecordSave::IncBallThrowCount( const MainModule& mainModule )
{
  mainModule.RECORDDATA_Inc( Savedata::Record::RECID_THROW_BALL );
}

//-----------------------------------------------------------------------------
/**
 * @brief 回復アイテムを使用した回数をインクリメントする
 * @param itemID  使用した道具のID
 */
 //----------------------------------------------------------------------------
void RecordSave::IncRecoverItemUseCount( const MainModule& mainModule, u16 itemID )
{
  const u16 itemParam = calc::ITEM_GetParam( itemID, item::ITEM_DATA::PRM_ID_B_FUNC );

  if( itemParam == ITEMUSE_BTL_RECOVER )
  {
    mainModule.RECORDDATA_Inc( Savedata::Record::RECID_HEAL_KIDOU );
  }
}

//----------------------------------------------------------------------------
/**
 * @brief 野生のポケモンから道具を盗んだ回数をインクリメントする
 */
//----------------------------------------------------------------------------
void RecordSave::IncStealItemFromWildPoke( const MainModule& mainModule )
{
  mainModule.RECORDDATA_Inc( Savedata::Record::RECID_STEAL_ITEM );
}

//----------------------------------------------------------------------------
/**
 * @brief 技「ねこにこばん」で取得した金額を加算する
 * @param competitor  対戦相手
 * @param result      対戦結果
 * @param money       加算する金額
 */
//----------------------------------------------------------------------------
void RecordSave::AddNekonikobanTotal( const MainModule& mainModule, BtlResult result, u32 money )
{
  if( ( mainModule.IsCompetitorScenarioMode() ) &&
      ( result == BTL_RESULT_WIN ) )
  {
    mainModule.RECORDDATA_Add( Savedata::Record::RECID_NEKONIKOBAN, money );
  }
}


GFL_NAMESPACE_END( btl )
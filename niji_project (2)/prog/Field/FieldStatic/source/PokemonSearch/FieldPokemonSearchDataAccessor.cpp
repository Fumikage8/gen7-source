//======================================================================
/**
 * @file FieldPokemonSearchDataAccessor.cpp
 * @date 2015/12/09
 * @author saita_kazuki
 * @brief ポケモンサーチデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/PokemonSearch/FieldPokemonSearchDataAccessor.h"

// gfl2
#include <debug/include/gfl2_Assert.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

// script
#include "FieldScript/include/EventScriptCall.h"
#include "FieldScript/include/FieldScriptSystem.h"

// savedata
#include "Savedata/include/EventWork.h"

// system
#include "System/include/GflUse.h"

// conv_header
#include <niji_conv_header/field/field_resident/pokemon_search.h>
#include <niji_conv_header/field/script/inc/poke_search.inc>

// reference_files
#include <niji_reference_files/script/FieldPawnTypes.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( PokemonSearch )

/**
 * @brief コンストラクタ
 * @param pData ポケモンサーチバイナリデータ。FieldResidentから取得
 */
DataAccessor::DataAccessor( void* pData)
  : m_pData( NULL)
  , m_pCacheData( NULL)
{
  this->SetData( pData);
}

/**
 * @brief デストラクタ
 */
DataAccessor::~DataAccessor()
{
}

/**
 *  @brief データセット
 *  @param pData ポケモンサーチバイナリデータ。FieldResidentから取得
 */
void DataAccessor::SetData( void* pData)
{
  GFL_ASSERT_MSG( pData, "Set binary data is null.");
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( pData);

  m_pData = reinterpret_cast<AllData*>( binLinkerAccessor.GetData( BL_IDX_POKEMON_SEARCH_POKEMON_SEARCH_BIN));
  GFL_ASSERT_MSG( m_pData, "Set binary data is invalid.");
}

/**
 * @brief サーチ用バトルコールをするか
 * @param areaID エリアID
 * @param encountID エンカウントID
 * @param pEventWork イベントワーク
 * @retval true サーチ用バトルコールをする
 * @retval false 通常バトルコールをする
 * @note 内部でデータテーブルの検索を行う
 */
bool DataAccessor::IsSearchBattleCall( u16 areaID, u32 encountID, const EventWork* pEventWork)
{
  // データ検索
  m_pCacheData = this->SearchData( areaID, encountID, pEventWork);

  // 見つからなければfalse
  if( m_pCacheData == NULL)
  {
    return false;
  }

  // 抽選
  if( this->IsLottery( m_pCacheData->prob) == false)
  {
    return false;
  }

  return true;
}

/**
 * @brief サーチ用バトルコール
 * @param pGameManager ゲームマネージャー
 * @note IsSearchBattleCallでtrueが返ってきたときに呼ぶこと
 */
void DataAccessor::CallSearchBattle( GameSys::GameManager* pGameManager)
{
  if( m_pCacheData == NULL)
  {
    GFL_ASSERT_MSG( 0, "Search battle call is failed.");
    return;
  }

  ScriptType type = static_cast<ScriptType>( m_pCacheData->scriptType);
  u32 scriptID = this->GetScriptID( type );
  if( scriptID == Field::FieldScript::SCRID_NULL )
  {
    return;
  }

  EventScriptCall::CallScript( pGameManager, scriptID, NULL, NULL, m_pCacheData->symbolEncountID, m_pCacheData->flag);
}

/**
 * @brief サーチ用バトルスクリプト予約
 * @param pGameManager ゲームマネージャー
 * @param pScriptSystem スクリプト予約に使用するスクリプトシステム
 */
void DataAccessor::ReserveSearchBattle( FieldScript::FieldScriptSystem* pScriptSystem )
{
  if( m_pCacheData == NULL)
  {
    GFL_ASSERT_MSG( 0, "Search battle call is failed.");
    return;
  }

  ScriptType type = static_cast<ScriptType>( m_pCacheData->scriptType);
  u32 scriptID = this->GetScriptID( type );
  if( scriptID == Field::FieldScript::SCRID_NULL )
  {
    return;
  }

  pScriptSystem->SetReserveScript( scriptID, m_pCacheData->symbolEncountID, m_pCacheData->flag );
}

/**
 * @brief シンボルエンカウントデータIDの取得
 * @param flagID 検索のキーとするフラグID
 * @return シンボルエンカウントデータID
 */
u32 DataAccessor::GetSymbolEncountDataID( u32 flagID) const
{
  for( u32 i = 0; i < m_pData->header.num; ++i)
  {
    if( m_pData->data[i].flag == flagID)
    {
      return m_pData->data[i].symbolEncountID;
    }
  }
  GFL_ASSERT_MSG( 0, "Not found pokemon search data\n");
  return 0;
}

/**
 * @brief シンボルエンカウントデータIDの取得
 * @return シンボルエンカウントデータID
 * @note IsSearchBattleCallでtrueが返ってきたときに呼ぶこと
 * @fix GFNMCat[1222][1223] ポケモンサーチのエンカウント回避チェックで使用するため追加
 */
u32 DataAccessor::GetSymbolEncountDataID() const
{
  if( m_pCacheData == NULL)
  {
    GFL_ASSERT_MSG( 0, "Search battle call is failed.");
    return 0;
  }

  return m_pCacheData->symbolEncountID;
}

/**
 * @brief データ検索
 * @param areaID エリアID
 * @param encountID エンカウントID
 * @param pEventWork イベントワーク
 * @return 一致したデータ。見つからなければNULL
 */
DataAccessor::CoreData* DataAccessor::SearchData( u16 areaID, u32 encountID, const EventWork* pEventWork) const
{
  for( u32 i = 0; i < m_pData->header.num; ++i)
  {
    // エリアとエンカウントIDが一致していなければ、さらに先を調べる
    if( this->IsMatchArea( areaID, encountID, m_pData->data[i]) == false)
    {
      continue;
    }

    // フラグが立っていなければ、さらに先を調べる
    if( this->IsEnableFlag( m_pData->data[i].flag, pEventWork) == false)
    {
      continue;
    }

    // フラグまで立っていたら検索終了
    return &m_pData->data[i];
  }
  return NULL;
}

/**
 * @brief エリアIDとエンカウントエリアIDが一致しているか
 * @param areaID エリアID
 * @param encountID エンカウントID
 * @param coreData コアデータ
 * @retval true 一致している
 * @retval false 一致していない
 */
bool DataAccessor::IsMatchArea( u16 areaID, u32 encountID, const CoreData& coreData) const
{
  if( coreData.areaID == areaID && coreData.encountID == encountID)
  {
    return true;
  }
  return false;
}

/**
 * @brief フラグが有効か
 * @param flag フラグID
 * @param pEventWork イベントワーク
 * @retval true フラグON
 * @retval false フラグOFF
 */
bool DataAccessor::IsEnableFlag( u16 flag, const EventWork* pEventWork) const
{
  if( EventWork::CheckIDFlagRenge( flag) == false)
  {
    GFL_ASSERT_MSG( 0, "Flag id is out of range.");
    return false;
  }
  return pEventWork->CheckEventFlag( flag);
}

/**
 * @brief 確率抽選
 * @param prob 確率
 * @retval true 当選
 * @retval false 落選
 */
bool DataAccessor::IsLottery( u8 prob) const
{
  GFL_ASSERT_MSG( prob <= 100, "Probability is more than 100.");
  u32 rnd = System::GflUse::GetPublicRand( 100);
  if( rnd < prob)
  {
    return true;
  }
  return false;
}

/**
 * @brief スクリプトID取得
 * @param type スクリプトの種類
 * @return 対応したスクリプトID
 */
u32 DataAccessor::GetScriptID( ScriptType type ) const
{
  switch( type)
  {
  case SCRIPT_TYPE_UB:
    return SCRID_POKE_SEARCH_UB_BATTLE;
  case SCRIPT_TYPE_SCAN:
    return SCRID_POKE_SEARCH_SCAN_BATTLE;
  default:
    GFL_ASSERT_MSG( 0, "Script type is invalid.");
    return Field::FieldScript::SCRID_NULL;
  }
}

GFL_NAMESPACE_END( PokemonSearch )
GFL_NAMESPACE_END( Field )

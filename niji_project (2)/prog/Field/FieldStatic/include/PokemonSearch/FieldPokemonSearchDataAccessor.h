//======================================================================
/**
 * @file FieldPokemonSearchDataAccessor.h
 * @date 2015/12/09
 * @author saita_kazuki
 * @brief ポケモンサーチデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_POKEMON_SEARCH_DATA_ACCESSOR_H_INCLUDED__
#define __FIELD_POKEMON_SEARCH_DATA_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


// 前方参照
namespace Field {
  class EventWork;
  namespace FieldScript {
    class FieldScriptSystem;
  }
}
namespace GameSys {
  class GameManager;
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( PokemonSearch )

/**
 * @brief 呼び出すスクリプトの種類
 */
enum ScriptType
{
  SCRIPT_TYPE_UB,       ///< ビースト
  SCRIPT_TYPE_SCAN,     ///< QRスキャン
};

/**
 * @class ポケモンサーチデータアクセサー
 */
class DataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( DataAccessor );

public:

  /**
   * @brief コンストラクタ
   * @param pData ポケモンサーチバイナリデータ。FieldResidentから取得
   */
  DataAccessor( void* pData);

  /**
   * @brief デストラクタ
   */
  virtual ~DataAccessor();

  /**
   *  @brief データセット
   *  @param pData ポケモンサーチバイナリデータ。FieldResidentから取得
   */
  void SetData( void* pData);

  /**
   * @brief サーチ用バトルコールをするか
   * @param areaID エリアID
   * @param encountID エンカウントID
   * @param pEventWork イベントワーク
   * @retval true サーチ用バトルコールをする
   * @retval false 通常バトルコールをする
   * @note 内部でデータテーブルの検索を行う
   */
  bool IsSearchBattleCall( u16 areaID, u32 encountID, const EventWork* pEventWork);

  /**
   * @brief サーチ用バトルコール
   * @param pGameManager ゲームマネージャー
   * @note IsSearchBattleCallでtrueが返ってきたときに呼ぶこと
   */
  void CallSearchBattle( GameSys::GameManager* pGameManager);

  /**
   * @brief サーチ用バトルスクリプト予約
   * @param pScriptSystem スクリプト予約に使用するスクリプトシステム
   * @note IsSearchBattleCallでtrueが返ってきたときに呼ぶこと
   */
  void ReserveSearchBattle( FieldScript::FieldScriptSystem* pScriptSystem );

  /**
   * @brief シンボルエンカウントデータIDの取得
   * @param flagID 検索のキーとするフラグID
   * @return シンボルエンカウントデータID
   */
  u32 GetSymbolEncountDataID( u32 flagID) const ;

  /**
   * @brief シンボルエンカウントデータIDの取得
   * @return シンボルエンカウントデータID
   * @note IsSearchBattleCallでtrueが返ってきたときに呼ぶこと
   */
  u32 GetSymbolEncountDataID() const ;

private:

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  /**
   * @brief ヘッダーデータ
   */
  struct HeaderData
  {
    u32   num;              ///< データ数
  };

  /**
   * @brief コアデータ
   */
  struct CoreData
  {
    u16   id;               ///< ポケモンサーチデータID
    u16   areaID;           ///< エリアID
    u32   encountID;        ///< エンカウントID
    u32   flag;             ///< フラグID
    u32   symbolEncountID;  ///< シンボルエンカウントID
    u8    scriptType;       ///< 呼び出すスクリプトの種類
    u8    prob;             ///< 確率
    u8    padding[2];
  };

  /**
   * @brief バイナリ全体
   */
  struct AllData
  {
    HeaderData header;      ///< ヘッダー
    CoreData   data[];      ///< コアデータ配列
  };

GFL_WARNING_WIN32_POP

private:

  CoreData* SearchData( u16 areaID, u32 encountID, const EventWork* pEventWork) const ;
  bool IsMatchArea( u16 areaID, u32 encountID, const CoreData& coreData) const ;
  bool IsEnableFlag( u16 flag, const EventWork* pEventWork) const ;
  bool IsLottery( u8 prob) const ;
  u32 GetScriptID( ScriptType type ) const ;

private:

  AllData*      m_pData;        ///< データ
  CoreData*     m_pCacheData;   ///< キャッシュデータ
};

GFL_NAMESPACE_END( PokemonSearch )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_POKEMON_SEARCH_DATA_ACCESSOR_H_INCLUDED__

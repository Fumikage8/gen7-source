//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		FieldItemUse.h
 *	@brief  フィールドアイテム私用チェック
 *	@author	Masayuki Onoue
 *	@date		2012.12.09
 *  @date   2015.11.30 niji用に移植 tomoya
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELDITEMUSE_H__ )
#define __FIELDITEMUSE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace GameSys {
  class GameData;
  class GameEventManager;
  class GameManager;
} // namespace GameSys


namespace Field {

  class Fieldmap;

class FieldItemUse
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldItemUse);
  
  public:
  //--------------------------------------------------------------
  /// FLDITEM_IDX フィールドアイテムインデックス
  //--------------------------------------------------------------
  typedef enum
  {
    //FLDITEM_IDX_GAKUSYUUSOUTI= 0, ///< 学習装置
//    FLDITEM_IDX_HOROGURAMUt,      ///< ホログラムツイッター
//    FLDITEM_IDX_DAUZINGUMASIN,    ///< ダウジング
//    FLDITEM_IDX_TAUNMAPPU,        ///< タウンマップ
    FLDITEM_IDX_AMAIMITU,         ///< あまいみつ
//    FLDITEM_IDX_BATORUREKOODAA,   ///< バトルレコーダー
    FLDITEM_IDX_ANANUKENOHIMO,    ///< 穴ぬけの紐
    FLDITEM_IDX_TURIZAO,          ///< 釣竿
//    FLDITEM_IDX_BOUKENNNOKOKOROE, ///< ぼうけんの心得
//    FLDITEM_IDX_POROKKUKEESU,     ///< ポロックケース
    //FLDITEM_IDX_MUGENNOHUE,       ///< 無限の笛
    //FLDITEM_IDX_ZITENSYA_MACH,    ///< マッハ自転車
    //FLDITEM_IDX_ZITENSYA_DIRT,    ///< ダート自転車

    FLDITEM_IDX_TAMAGOHUKAPON,  ///< タマゴふかポン
    FLDITEM_IDX_YASUURIPON,     ///< やすうりポン
    FLDITEM_IDX_OKODUKAIPON,    ///< おこづかいポン
    FLDITEM_IDX_KEIKENTIPON, 	  ///< けいけんちポン
    FLDITEM_IDX_NATUKIPON,      ///< なつきポン
    FLDITEM_IDX_SOUGUUPON,      ///< そうぐうポン
    FLDITEM_IDX_ONMITUPON, 	    ///< おんみつポン

    FLDITEM_IDX_MAX,              ///< 最大
  }FLDITEM_IDX;

  //--------------------------------------------------------------
  /// FLDITEM_RET 使用チェックの戻り値
  //--------------------------------------------------------------
  typedef enum
  {
    FLDITEM_RET_USE_OK = 0,          ///< 使用可能
    FLDITEM_RET_USE_NG,              ///< 使用不可（ここでは使えません）
    FLDITEM_RET_ALREADY_USE_ITEM,    ///< 既にアイテムを使用している
  }FLDITEM_RET;

  //-------------------------------------
  ///	スキル使用可能ワーク
  //=====================================
  typedef struct {
    u16 zone_id;                    ///< ゾーンID
    u32 enable_item;                ///< アイテムが有効かどうか
    GameSys::GameData* p_gdata;  ///< ゲームデータ
  } ITEM_USE_CHECK_WORK;
 
  //-------------------------------------
  ///	スキル使用フラグ
  //=====================================
  typedef struct {
    u32 item_no;                            ///< アイテムナンバー
    Fieldmap* p_fieldmap;                   ///< フィールドマップ
    GameSys::GameEventManager* p_evman;  ///< イベントマネージャ
    ITEM_USE_CHECK_WORK check_wk;           ///< チェック用ワーク
  } ITEM_USE_WORK;

  private:

  //---------------------------
  /// アイテム使用チェック関数ポインタテーブル
  typedef FieldItemUse::FLDITEM_RET (*ITEM_CHECK)(const ITEM_USE_CHECK_WORK& scwk);
  //---------------------------
  /// アイテム使用関数ポインタテーブル
  typedef void (*ITEM_USE)(const ITEM_USE_WORK* p_wk);

  //-------------------------------------
  ///	アイテム使用・チェック一元管理テーブル
  //=====================================
  struct ItemUseCheckTable
  {
    FLDITEM_IDX idx;              ///< アイテムインデックス
    u32 item_no;                  ///< アイテムナンバー
    ITEM_USE    item_use_func;    ///< アイテム使用関数
    ITEM_CHECK  item_check_func;  ///< アイテム使用可能チェック関数
  };
  
  static const u32 ITEM_USE_CHECK_NUM = 15;   ///< チェックテーブル数
  static const ItemUseCheckTable sc_item_use_check_tbl[ITEM_USE_CHECK_NUM]; ///< チェック関数テーブル

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アイテムの使用状況をセットする
   *
   * @param use_wk        ワーク
   * @param item_index    アイテムインデックス
   */
  /* -------------------------------------------------------------------------*/
  static void SetEnableItem( ITEM_USE_CHECK_WORK& use_wk ,u8 item_index );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アイテムチェック関数を取得する
   *
   * @param idx アイテムインデックス
   *
   * @return チェック関数
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::ITEM_CHECK GetCheckFunc( FLDITEM_IDX idx );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アイテム使用関数を取得する
   *
   * @param idx アイテムインデックス
   *
   * @returnアイテム呼び出し関数
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::ITEM_USE GetItemFunc( FLDITEM_IDX idx );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief スキルが使用できるかチェック
   *
   * @param use_wk  ワーク
   * @param idx     スキルインデックス
   *
   * @return trueの時使用できる
   */
  /* -------------------------------------------------------------------------*/
  static bool IsItemEnable( const ITEM_USE_CHECK_WORK& use_wk ,FLDITEM_IDX idx );

  /* -------------------------------------------------------------------------*/
  /*
   * @brief 学習装置実行
   */
  /* -------------------------------------------------------------------------*/
  //static void ItemUse_Gakusyuusouti( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 学習装置使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_Gakusyuusouti( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief スキルを呼び出す
   *
   * @param p_wk  スキル使用ワーク
   * @param idx   呼び出したいスキルインデックス
   *
   * @return 実行できた場合はtrue
   */
  /* -------------------------------------------------------------------------*/
  static bool CallItemByIndex( const ITEM_USE_WORK* p_wk, FLDITEM_IDX idx );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アイテム番号からアイテムインデックスを取得する
   *
   * @param item_no     アイテム番号
   *
   * @return スキルインデックス
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_IDX GetItemIndexByItemNo( u32 item_no );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief スキルの使用可能状況を
   *
   * @param use_wk  チェックワーク
   * @param idx     スキルインデックス
   *
   * @return  スキル使用可能状況
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemUseCheckByIndex( const ITEM_USE_CHECK_WORK& use_wk ,FLDITEM_IDX idx );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief マッハ自転車使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_ZitensyaMach( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ダート自転車使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_ZitensyaDirt( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 自転車実行
   */
  /* -------------------------------------------------------------------------*/
  //static void ItemUse_Zitensya( const ITEM_USE_WORK* p_wk );
  /* -------------------------------------------------------------------------*/
  /**
   * @brief ホログラムツイッター使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
//  static FieldItemUse::FLDITEM_RET ItemCheck_HoroguramuT( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ホログラムツイッター実行
   */
  /* -------------------------------------------------------------------------*/
//  static void ItemUse_HoroguramuT( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief ダウジングマシン使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_Dauzingumasin( const ITEM_USE_CHECK_WORK& use_wk );
  /* -------------------------------------------------------------------------*/
  /**
   * @brief ダウジングマシン実行
   */
  /* -------------------------------------------------------------------------*/
  //static void ItemUse_Dauzingumasin( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief タウンマップ使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
//  static FieldItemUse::FLDITEM_RET ItemCheck_Taunmappu( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief タウンマップ実行
   */
  /* -------------------------------------------------------------------------*/
//  static void ItemUse_Taunmappu( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief あまいみつ使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_Amaimitu( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief あまいみつ実行
   */
  /* -------------------------------------------------------------------------*/
  static void ItemUse_Amaimitu( const ITEM_USE_WORK* p_wk );
  
  /* -------------------------------------------------------------------------*/
  /**
   * @brief バトルレコーダー使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_Batorurekoodaa( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 穴ぬけの紐使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_Ananukenohimo( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 穴ぬけの紐実行
   */
  /* -------------------------------------------------------------------------*/
  static void ItemUse_Ananukenohimo( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief バトルレコーダー実行
   */
  /* -------------------------------------------------------------------------*/
  //static void ItemUse_Batorurekoodaa( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣竿の使用チェック
   *
   * @param use_wk  チェック用ワーク
   *
   * @return 使用チェックの戻り値
   */
  /* -------------------------------------------------------------------------*/
  //static FieldItemUse::FLDITEM_RET ItemCheck_Turizao( const ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣竿実行
   */
  /* -------------------------------------------------------------------------*/
  //static void ItemUse_Turizao( const ITEM_USE_WORK* p_wk );

  /* -------------------------------------------------------------------------*/
  /**
  * @brief たまごふかポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_TamagohukaPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief やすうりポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_YasuuriPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief おこづかいポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_OkodukaiPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief けいけんちポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_KeikentiPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief なつきポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_NatukiPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief そうぐうポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_SouguuPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief おんみつポン使用チェック
  *
  * @param use_wk  チェック用ワーク
  *
  * @return 使用チェックの戻り値
  */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemCheck_OnmituPon(const ITEM_USE_CHECK_WORK& use_wk);
  /* -------------------------------------------------------------------------*/
  /**
  * @brief ロトポン実行
  */
  /* -------------------------------------------------------------------------*/
  static void ItemUse_Rotopon( const ITEM_USE_WORK* p_wk );


  public:
  /* -------------------------------------------------------------------------*/
  /**
   * @brief ワークを初期化する
   *
   * @param p_fieldmap    フィールドマップ
   * @param use_wk        チェック結果を格納するワーク
   */
  /* -------------------------------------------------------------------------*/
  static void InitializeWork( Fieldmap* p_fieldmap,ITEM_USE_CHECK_WORK& use_wk );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief スキルの使用可能状況を
   *
   * @param use_wk    チェックワーク
   * @param item_no   アイテム番号
   *
   * @return  アイテム使用可能状況
   */
  /* -------------------------------------------------------------------------*/
  static FieldItemUse::FLDITEM_RET ItemUseCheck( const ITEM_USE_CHECK_WORK& use_wk ,u32 item_no );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アイテムを呼び出す
   *
   * @param p_game_manager  ゲームマネージャ
   * @param item_no         呼び出したいアイテム番号
   *
   * @return 実行できた場合はtrue
   */
  /* -------------------------------------------------------------------------*/
  static bool CallItem( GameSys::GameManager* p_game_manager, u32 item_no );

};

} // namespace field

#endif		// __FIELDITEMUSE_H__


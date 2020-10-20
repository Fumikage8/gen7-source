﻿//============================================================================================
/**
 * @file    item.h
 * @brief   アイテムデータ処理
 * @author  Hiroyuki Nakamura
 * @date    11.05.23
 *
 * class    item::ITEM_DATA::
 */
//============================================================================================
#ifndef __PML_ITEM_DATA_H__
#define __PML_ITEM_DATA_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_MsgData.h>

#include "niji_conv_header/poke_lib/item/itemsym.h"
#include "niji_conv_header/poke_lib/item/itempocket_def.h"

/**
 * @namespace item
 */
namespace item {

#include "niji_conv_header/poke_lib/item/monsball_id.h"

#define ITEM_WAZAMACHINE_ERROR  ( 0xffffffff )    ///< 技マシンチェックエラーコード



//---------------------------------------------------------------------
/**
 * @class ITEM_DATA
 * @brief アイテムデータクラス
 */
//---------------------------------------------------------------------
class ITEM_DATA {
public:

  /// パラメータID
  typedef enum {
    PRM_ID_ITEMNUMBER = 0,          ///< アイテム番号

    PRM_ID_PRICE,                   ///< 買値
    PRM_ID_EQUIP,                   ///< 装備効果
    PRM_ID_ATTACK,                  ///< 威力
    PRM_ID_TUIBAMU_EFF,             ///< ついばむ効果
    PRM_ID_NAGE_EFF,                ///< なげつける効果
    PRM_ID_NAGE_ATC,                ///< なげつける威力
    PRM_ID_SIZEN_ATC,               ///< しぜんのめぐみ威力
    PRM_ID_SIZEN_TYPE,              ///< しぜんのめぐみタイプ
    PRM_ID_IMP,                     ///< 重要
    PRM_ID_CNV,                     ///< 便利ボタン
    PRM_ID_F_POCKET,                ///< FIELD保存先（ポケット番号）
    PRM_ID_B_POCKET,                ///< BATTLE保存先（ポケット番号）
    PRM_ID_F_FUNC,                  ///< FIELD機能
    PRM_ID_B_FUNC,                  ///< BATTLE機能
    PRM_ID_WORK_TYPE,               ///< ワークタイプ
    PRM_ID_ITEM_TYPE,               ///< アイテム種類
    PRM_ID_SPEND,                   ///< 消費するか（戦闘装備品用）
    PRM_ID_USE_SPEND,               ///< 使用時に消費するか ( 消費しない場合 = 1 )
    PRM_ID_SORT,                    ///< ソート番号

    PRM_ID_WORK,                    ///< 汎用ワーク

    // ここから個別ワーク
    // 使用時はワークタイプ(PRM_ID_WORK_TYPE)に注意してください
    // 回復アイテム
    PRM_ID_SLEEP_RCV = PRM_ID_WORK, ///< 眠り回復
    PRM_ID_POISON_RCV,              ///< 毒回復
    PRM_ID_BURN_RCV,                ///< 火傷回復
    PRM_ID_ICE_RCV,                 ///< 氷回復
    PRM_ID_PARALYZE_RCV,            ///< 麻痺回復
    PRM_ID_PANIC_RCV,               ///< 混乱回復
    PRM_ID_MEROMERO_RCV,            ///< メロメロ回復
    PRM_ID_ABILITY_GUARD,           ///< 能力ガード
    PRM_ID_DEATH_RCV,               ///< 瀕死回復
    PRM_ID_ALL_DEATH_RCV,           ///< 全員瀕死回復
    PRM_ID_LV_UP,                   ///< レベルアップ
    PRM_ID_EVOLUTION,               ///< 進化
    PRM_ID_ATTACK_UP,               ///< 攻撃力アップ
    PRM_ID_DEFENCE_UP,              ///< 防御力アップ
    PRM_ID_SP_ATTACK_UP,            ///< 特攻アップ
    PRM_ID_SP_DEFENCE_UP,           ///< 特防アップ
    PRM_ID_AGILITY_UP,              ///< 素早さアップ
    PRM_ID_HIT_UP,                  ///< 命中率アップ
    PRM_ID_CRITICAL_UP,             ///< クリティカル率アップ
    PRM_ID_PP_UP,                   ///< PPアップ
    PRM_ID_PP_3UP,                  ///< PPアップ（３段階）
    PRM_ID_PP_RCV,                  ///< PP回復
    PRM_ID_ALL_PP_RCV,              ///< PP回復（全ての技）
    PRM_ID_HP_RCV,                  ///< HP回復
    PRM_ID_HP_EXP,                  ///< HP努力値アップ
    PRM_ID_POWER_EXP,               ///< 攻撃努力値アップ
    PRM_ID_DEFENCE_EXP,             ///< 防御努力値アップ
    PRM_ID_AGILITY_EXP,             ///< 素早さ努力値アップ
    PRM_ID_SP_ATTACK_EXP,           ///< 特攻努力値アップ
    PRM_ID_SP_DEFENCE_EXP,          ///< 特防努力値アップ
    PRM_ID_EXP_LIMIT_FLAG,          ///< 努力値限界制御
    PRM_ID_FRIEND1,                 ///< なつき度１
    PRM_ID_FRIEND2,                 ///< なつき度２
    PRM_ID_FRIEND3,                 ///< なつき度３
    PRM_ID_HP_EXP_POINT,            ///< HP努力値の値
    PRM_ID_POWER_EXP_POINT,         ///< 攻撃努力値の値
    PRM_ID_DEFENCE_EXP_POINT,       ///< 防御努力値の値
    PRM_ID_AGILITY_EXP_POINT,       ///< 素早さ努力値の値
    PRM_ID_SP_ATTACK_EXP_POINT,     ///< 特攻努力値の値
    PRM_ID_SP_DEFENCE_EXP_POINT,    ///< 特防努力値の値
    PRM_ID_HP_RCV_POINT,            ///< HP回復値の値
    PRM_ID_PP_RCV_POINT,            ///< pp回復値の値
    PRM_ID_FRIEND1_POINT,           ///< なつき度1の値
    PRM_ID_FRIEND2_POINT,           ///< なつき度2の値
    PRM_ID_FRIEND3_POINT,           ///< なつき度3の値
  }ITEM_PRM_ID;

  /// ワークタイプの種類
  enum {
    WKTYPE_NORMAL = 0,  ///< 通常
    WKTYPE_POKEUSE,     ///< ポケモンに使う
    WKTYPE_BALL,        ///< モンスターボール
  };

  // HP 回復量定義 (下記以外なら、その値分だけ回復する）
  enum HP_RECOVER_TYPE {
    ITEM_RECOVER_HP_FULL = 255,   ///< 全回復
    ITEM_RECOVER_HP_HALF = 254,   ///< MAXの半分回復
    ITEM_RECOVER_HP_QUOT = 253,   ///< MAXの1/4回復
  };

  // PP 回復量定義 (下記以外なら、その値分だけ回復する）
  enum PP_RECOVER_TYPE {
    ITEM_RECOVER_PP_FULL = 127,   ///< 全回復
  };


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ（空データ）
   *
   * @param   clearFlag   TRUE = データクリア
   */
  //--------------------------------------------------------------------------------------------
  ITEM_DATA( bool clearFlag = false );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ（データ作成+）
   *
   * @param   itemID    アイテム番号
   * @param   pHeap     ヒープ
   */
  //--------------------------------------------------------------------------------------------
  ITEM_DATA( int itemID, gfl2::heap::HeapBase * pHeap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ~ITEM_DATA(void);

private:
  struct ITEM_PRM {
    u16 price;          // 買値
    u8  eqp;            // 装備効果
    u8  atc;            // 威力

    u8  tuibamu_eff;    // ついばむ効果
    u8  nage_eff;       // なげつける効果
    u8  nage_atc;       // なげつける威力
    u8  sizen_atc;      // しぜんのめぐみ威力

    u16 sizen_type:5;   // しぜんのめぐみタイプ
    u16 imp:1;          // 重要
    u16 cnv_btn:1;      // 便利ボタン
    u16 fld_pocket:4;   // FIELD保存先（ポケット番号）
    u16 btl_pocket:5;   // BATTLE保存先（ポケット番号）
    u8  field_func;     // FIELD機能
    u8  battle_func;    // BATTLE機能

    u8  work_type;      // ワークタイプ
    u8  type;           // アイテム種類
    u8	spend:4;				// 消費するか ( バトルの装備アイテム用 )
    u8	use_no_spend:4;	// 使用の際、消費するか ( 1 = 消費しない )

    u8  sort;           // ソート番号

    u8  work[20];       // 汎用ワーク
  };
  ITEM_PRM  prm;
  int id;

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテムデータ作成
   *
   * @param   itemID    アイテム番号
   * @param   pHeap     ヒープ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void Create( int itemID, gfl2::heap::HeapBase * pHeap );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテムデータ作成（ファイル指定）
   *
   * @param   itemID    アイテム番号
   * @param   arcFile   ファイル
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void Create( int itemID, const char *garcPath );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   パラメータ取得
   *
   * @param   prmID     パラメータID
   *
   * @return  パラメータ
   */
  //--------------------------------------------------------------------------------------------
  int GetParam( ITEM_PRM_ID prmID ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテム名取得 ( MsgData指定 )
   *
   * @param   md      MsgData
   * @param   buf     アイテム名格納先バッファ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
  void ITEM_GetItemName( gfl2::str::MsgData * md, gfl2::str::StrBuf * buf );
#endif

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテム名取得
   *
   * @param   buf     アイテム名格納先バッファ
   * @param   pHeap   ヒープ
   *
   * @return  none
   *
   * @li  以下の3つの関数の集合体
   *    gfl2::str::MsgData * ITEM_CreateItemNameMsgData(...);
   *    void ITEM_GetItemName(...);
   *    void ITEM_DeleteItemNameMsgData(...);
   */
  //--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
  void ITEM_GetItemName( gfl2::str::StrBuf * buf, gfl2::heap::HeapBase * pHeap );
#endif

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテム説明取得 ( MsgData指定 )
   *
   * @param   md      MsgData
   * @param   buf     アイテム名格納先バッファ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
//  void ITEM_GetInfo( gfl2::str::MsgData * md, gfl2::str::StrBuf * buf );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アイテム説明取得
   *
   * @param   buf     アイテム説明格納先バッファ
   * @param   pHeap   ヒープ
   *
   * @return  none
   *
   * @li      以下の3つの関数の集合体
   *            gfl2::str::MsgData * ITEM_CreateInfoMsgData(...);
   *            void ITEM_GetInfo(...);
   *            void ITEM_DeleteInfoMsgData(...);
   */
  //--------------------------------------------------------------------------------------------
//  void ITEM_GetInfo( gfl2::str::StrBuf * buf, gfl2::heap::HeapBase * pHeap );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   常駐アイテムデータ作成
	 *
	 * @param   heap   ヒープ
	 *
	 * @return  none
	 *
	 * @li	グローバルなポインタに展開するので、複数回やらないように注意！
	 */
	//--------------------------------------------------------------------------------------------
	static void CreateExpandData( gfl2::heap::HeapBase * heap );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   常駐アイテムデータ削除
	 *
	 * @param   none
	 *
	 * @return  none
	 */
	//--------------------------------------------------------------------------------------------
	static void DeleteExpandData(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief   パラメータ取得（常駐データから）
	 *
	 * @param   item		アイテム番号
	 * @param		prmID		パラメータID
	 *
	 * @return  パラメータ
	 *
	 * @li	以下、有効パラメータID
	 *				PRM_ID_F_POCKET		フィールドポケット番号
	 */
	//--------------------------------------------------------------------------------------------
	static int GetParam( u32 item, ITEM_DATA::ITEM_PRM_ID prmID );

};  // ITEM_DATA


//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名のMsgDataを作成
 *
 * @param   type    読み込みタイプ
 * @param   pHeap   ヒープ
 *
 * @return  gfl2::str::MsgData
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
extern gfl2::str::MsgData * ITEM_CreateItemNameMsgData( gfl2::str::MsgData::LoadType type, gfl2::heap::HeapBase * pHeap );
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム名のMsgDataを削除
 *
 * @param   md      MsgData
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#if 0 // @note WordSetを使用して取得してください
extern void ITEM_DeleteItemNameMsgData( gfl2::str::MsgData * md );
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム説明のMsgDataを作成
 *
 * @param   type    読み込みタイプ
 * @param   pHeap   ヒープ
 *
 * @return  gfl2::str::MsgData
 */
//--------------------------------------------------------------------------------------------
#if 0
extern gfl2::str::MsgData * ITEM_CreateInfoMsgData( gfl2::str::MsgData::LoadType type, gfl2::heap::HeapBase * pHeap );
#endif
//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテム説明のMsgDataを削除
 *
 * @param   md      MsgData
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
//extern void ITEM_DeleteInfoMsgData( gfl2::str::MsgData * md );

//--------------------------------------------------------------------------------------------
/**
 * アイテムID -> ボールID 変換
 *
 * @param   itemID    アイテムID
 *
 * @retval  BALL_ID   ボールID（指定されたアイテムIDがボール以外の場合、BALLID_NULL）
 */
//--------------------------------------------------------------------------------------------
extern BALL_ID ITEM_GetBallID( u16 itemID );

//--------------------------------------------------------------------------------------------
/**
 * ボールID -> アイテムNo 変換
 *
 * @param   BALL_ID   ボールID
 * @retval  itemID    アイテムNo（指定されたアイテムIDがボール以外の場合、ITEM_DUMMY_DATA
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_BallID2ItemID( BALL_ID ballID );

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシンかどうか
 *
 * @param   item  アイテム番号
 *
 * @retval  "true = 技マシン"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckWazaMachine( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシンで覚えられる技を取得
 *
 * @param   item  アイテム番号
 *
 * @return  技番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetWazaNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief   選択された技がポケモン交換に適さない技かどうか（秘伝技かどうか）
 *
 * @param   waza  技番号
 *
 * @retval  "true = 交換に適さない技(秘伝技"
 * @retval  "false = 交換に適する技(秘伝技以外"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckHidenWaza( u16 waza );

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号取得
 *
 * @param   item  アイテム番号
 *
 * @return  技マシン番号
 *
 * @li  秘伝マシンは通常の技マシンの後にカウントされる
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetWazaMashineNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief   秘伝マシン番号取得
 *
 * @param   item  アイテム番号
 *
 * @return  秘伝マシン番号
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetHidenMashineNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン数取得（通常＋秘伝）
 *
 * @param   none
 *
 * @return  通常技マシン数と秘伝マシン数の合計
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetWazaMashineMax(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief   通常技マシン数取得
 *
 * @param   none
 *
 * @return  技マシン数
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetNormalWazaMashineMax(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief   秘伝マシン数取得
 *
 * @param   none
 *
 * @return  秘伝マシン数
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetHidenMashineMax(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号から技を取得
 *
 * @param   no  技マシン番号
 *
 * @return  技番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetWazaMashineWaza( u32 no );

//--------------------------------------------------------------------------------------------
/**
 * @brief   技マシン番号からアイテム番号を取得
 *
 * @param   no	技マシン番号
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetWazaMashineItemNo( u32 no );

//--------------------------------------------------------------------------------------------
/**
 * アイテムが木の実かどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = 木の実"
 * @retval  "FALSE = 木の実以外"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckNuts( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * 木の実の番号を取得
 *
 * @param item  アイテム番号
 *
 * @return  木の実番号（0origin)
 * @caution ITEM_CheckNuts同様、内部でリニアサーチしているので処理負荷には注意
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetNutsNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * 指定IDの木の実のアイテム番号を取得
 *
 * @param id    木の実のID
 *
 * @return  アイテム番号
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetNutsID( u8 id );

//--------------------------------------------------------------------------------------------
/**
 * 木の実の数を取得
 *
 * @param   none
 *
 * @return  木の実の数
 */
//--------------------------------------------------------------------------------------------
extern u32 ITEM_GetNutsMax(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief メガストーンかどうかを判定
 *
 * @param item  アイテム番号
 *
 * @retval  true=メガストーン, false=それ以外
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckMegaStone( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムがジュエルかどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval  "TRUE = ジュエル"
 * @retval  "FALSE = ジュエル以外"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckJewel( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテムがピースかどうかをチェック
 *
 * @param item  アイテム番号
 *
 * @retval true   ピース
 * @retval false  ピース以外"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckPiece( u16 item );

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetPieceTable
 * @brief   ピースのテーブルを取得
 * @date    2015.11.04
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
extern const u16 * ITEM_GetPieceTable( u32 * max );

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_CheckBeads
 * @brief   アイテムがビーズかをチェック
 * @date    2015.11.04
 *
 * @retval  true  = ビーズ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
extern bool ITEM_CheckBeads( u16 item );

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetBeadsTable
 * @brief   ビーズのテーブルを取得
 * @date    2016.01.13
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
extern const u16 * ITEM_GetBeadsTable( u32 * max );

//-----------------------------------------------------------------------------
/**
 * @func    ITEM_GetRotoponTable
 * @brief   ロトポンのテーブルを取得
 * @date    2017.02.01
 *
 * @param   max   テーブル要素数格納場所
 *
 * @return  テーブル
 */
//-----------------------------------------------------------------------------
extern const u16 * ITEM_GetRotoponTable( u32 * max );

//--------------------------------------------------------------------------------------------
/**
 * @brief   アイテムをポケモンに持たせられるか
 *
 * @param   item    アイテム番号
 *
 * @retval  "true = 可"
 * @retval  "false = 不可"
 */
//--------------------------------------------------------------------------------------------
extern bool ITEM_CheckPokeAdd( u16 item );


} // item

#endif // __PML_ITEM_DATA_H__

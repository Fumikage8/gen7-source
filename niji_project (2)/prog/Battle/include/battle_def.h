//=============================================================================================
/**
 * @file    battle_def.h
 * @brief   XY バトルシステム  バトル外部へ公開する定数
 * @author  taya
 *
 * @date  2010.12.21  作成
 */
//=============================================================================================
#if !defined __BATTLE_DEF_H__
#define __BATTLE_DEF_H__


enum {
  PTL_WAZA_MAX     = 4,
  PTL_SEX_MALE     = 0,
  PTL_SEX_FEMALE   = 1,
  PTL_SEX_UNKNOWN  = 2,
//  PTL_WAZA_MAX     = pml::MAX_WAZA_NUM,

  //niji最終は101,パッチで102 nijiのみのバージョンアップも考え、間を空けておく
  BTL_NET_SERVER_VERSION = 110,         //外部でも参照するので、btl_net.cppから移動 by soga
  BTLREC_OPERATION_BUFFER_SIZE = 0xc00,
};

enum PM_Season{
  PMSEASON_SPRING,
  PMSEASON_SUMMER,
  PMSEASON_AUTUMN,
  PMSEASON_WINTER,
};

//--------------------------------------------------------------
/**
 *  対戦ルール
 */
//--------------------------------------------------------------
enum BtlRule{

  BTL_RULE_SINGLE,    ///< シングル
  BTL_RULE_DOUBLE,    ///< ダブル
  BTL_RULE_ROYAL,     ///< バトルロイヤル
  BTL_RULE_INTRUDE,   ///< 乱入( 1vs2　野生シングル戦の途中から派生する )

};


//--------------------------------------------------------------
/**
 *  マルチモード
 */
//--------------------------------------------------------------

enum BtlMultiMode_tag{

  BTL_MULTIMODE_NONE = 0, ///< 非マルチ
  BTL_MULTIMODE_PP_PP,    ///< プレイヤー２人 vs プレイヤー２人（通信マルチ）
  BTL_MULTIMODE_PP_AA,    ///< プレイヤー２人 vs AI２人（AI通信マルチ）
  BTL_MULTIMODE_PA_AA,    ///< プレイヤー１人＆AI１人 vs AI２人（AIマルチ）
  BTL_MULTIMODE_P_AA,     ///< プレイヤー１人 VS AI２人（AIタッグ）
  BTL_MULTIMODE_P_P_P_P,  ///< プレイヤー１人 VS プレイヤー１人 VS プレイヤー１人 VS プレイヤー１人（バトルロイヤルマルチ）
};

#if !defined PAWN_COMPILER
typedef u8 BtlMultiMode;
#endif


//--------------------------------------------------------------
/**
 *  録画データ利用モード
 */
//--------------------------------------------------------------

enum BtlRecordDataMode_tag{

  BTL_RECORDDATA_NONE = 0, ///< 録画データを利用しない・設定されていない
  BTL_RECORDDATA_PLAY,     ///< 録画データを再生する
  BTL_RECORDDATA_FIGHT,    ///< 録画データトレーナーと対戦する（模擬戦）

};

//--------------------------------------------------------------
/**
 *  ロイヤル用の各プレイヤーのコーナー色
 */
//--------------------------------------------------------------
enum CornerColor
{
  CORRER_COLOR_BLUE = 0,
  CORRER_COLOR_YELLOW,
  CORRER_COLOR_GREEN,
  CORRER_COLOR_RED
};

//--------------------------------------------------------------
/**
 * @brief AIスクリプトの識別子
 */
//--------------------------------------------------------------
enum BtlAiScriptNo
{
  // 技
  BTL_AISCRIPT_NO_WAZA_BASIC,     // 基本AI
  BTL_AISCRIPT_NO_WAZA_STRONG,    // 攻撃型AI
  BTL_AISCRIPT_NO_WAZA_EXPERT,    // エキスパートAI
  BTL_AISCRIPT_NO_WAZA_DOUBLE,    // ダブルバトルAI
  BTL_AISCRIPT_NO_WAZA_ALLOWANCE, // 手加減AI
  BTL_AISCRIPT_NO_WAZA_ROYAL,     // バトルロイヤルAI
  BTL_AISCRIPT_NO_WAZA_INTRUDE,   // 乱入AI

  // 道具
  BTL_AISCRIPT_NO_ITEM_BASIC,

  // 入れ替え
  BTL_AISCRIPT_NO_POKECHANGE_BASIC,

  BTL_AISCRIPT_NO_NUM,  // 総数
  BTL_AISCRIPT_NO_NULL, // 無効値

  BTL_AISCRIPT_NO_WAZA_MIN = BTL_AISCRIPT_NO_WAZA_BASIC,   // 技選択スクリプトの最小値
  BTL_AISCRIPT_NO_WAZA_MAX = BTL_AISCRIPT_NO_WAZA_INTRUDE, // 技選択スクリプトの最大値
  BTL_AISCRIPT_NO_ITEM_MIN = BTL_AISCRIPT_NO_ITEM_BASIC,   // 道具スクリプトの最小値
  BTL_AISCRIPT_NO_ITEM_MAX = BTL_AISCRIPT_NO_ITEM_BASIC,   // 道具スクリプトの最大値
  BTL_AISCRIPT_NO_POKECHANGE_MIN = BTL_AISCRIPT_NO_POKECHANGE_BASIC, // 入れ替えスクリプトの最小値
  BTL_AISCRIPT_NO_POKECHANGE_MAX = BTL_AISCRIPT_NO_POKECHANGE_BASIC, // 入れ替えスクリプトの最大値
};

//--------------------------------------------------------------
/**
 * @brief AIスクリプトのビットフラグ
 */
//--------------------------------------------------------------
enum
{
  BTL_AISCRIPT_BIT_WAZA_BASIC       = 1 << BTL_AISCRIPT_NO_WAZA_BASIC,       // 基本AI
  BTL_AISCRIPT_BIT_WAZA_STRONG      = 1 << BTL_AISCRIPT_NO_WAZA_STRONG,      // 攻撃型AI
  BTL_AISCRIPT_BIT_WAZA_EXPERT      = 1 << BTL_AISCRIPT_NO_WAZA_EXPERT,      // エキスパートAI
  BTL_AISCRIPT_BIT_WAZA_DOUBLE      = 1 << BTL_AISCRIPT_NO_WAZA_DOUBLE,      // ダブルバトルAI
  BTL_AISCRIPT_BIT_WAZA_ALLOWANCE   = 1 << BTL_AISCRIPT_NO_WAZA_ALLOWANCE,   // 手加減AI
  BTL_AISCRIPT_BIT_WAZA_ROYAL       = 1 << BTL_AISCRIPT_NO_WAZA_ROYAL,       // バトルロイヤルAI
  BTL_AISCRIPT_BIT_WAZA_INTRUDE     = 1 << BTL_AISCRIPT_NO_WAZA_INTRUDE,     // 乱入AI
  BTL_AISCRIPT_BIT_ITEM_BASIC       = 1 << BTL_AISCRIPT_NO_ITEM_BASIC,       // 道具
  BTL_AISCRIPT_BIT_POKECHANGE_BASIC = 1 << BTL_AISCRIPT_NO_POKECHANGE_BASIC, // 入れ替え
};


#if !defined PAWN_COMPILER
typedef u8 BtlRecordDataMode;
#endif


#endif  // #define __BATTLE_DEF_H__



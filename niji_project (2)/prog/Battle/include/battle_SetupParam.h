//=============================================================================================
/**
 * @file  battle_SetupParam.h
 * @brief ポケモンXY バトルシステム セットアップパラメータ構造＆ツール関数
 * @author  taya
 *
 * @date  2010.12.17  作成
 */
//=============================================================================================
#ifndef __BATTLE_SETUPPARAM_H__
#define __BATTLE_SETUPPARAM_H__

#include <str/include/gfl2_Str.h>
#include <heap/include/gfl2_heap.h>
#include <PokeTool/include/PokeToolMega.h>
#include "GameSys/include/NijiTime.h"

#include "Battle/include/battle_def.h"
#include "Battle/include/battle_DefaultPowerUp.h"
#include "Battle/include/battle_SetupTrainer.h"
#include "Battle/include/battle_SetupParam_Intrude.h"
#include "Battle/include/battle_SetupParam_Reinforce.h"
#include "Battle/include/battle_SetupParam_Record.h"
#include "Battle/include/btlv/btlv_debug_param.h"

#include "Savedata/include/FieldMenuSave.h"

//BgComponentData
#include "Battle/Background/include/BgSetupUtil.h"

// レギュレーション
#include "Battle/Regulation/include/Regulation.h"

namespace GameSys {
  class GameManager;
  class GameData;
}

namespace trainer{
  class TrainerTypeData;
}

namespace Savedata {
  class ConfigSave;
  class MyStatus;
  class MyItem;
  class ZukanData;
}

namespace gflnet2
{
  namespace p2p
  {
    class NetGame;
  }
}

namespace Field {
namespace Encount {
class PokeSet;
}
}

//----------------------------------------------------------------------------------

#include "battle_ResultData.h" // for BtlResult

//--------------------------------------------------------------
/**
 *  対戦相手
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMPETITOR_WILD,    ///< 野生
  BTL_COMPETITOR_TRAINER, ///< ゲーム内トレーナー
  BTL_COMPETITOR_INST,    ///< バトル施設トレーナー
  BTL_COMPETITOR_COMM,    ///< 通信対戦

  BTL_COMPETITOR_DEMO_CAPTURE,  ///< 捕獲デモ ※使われていないので注意！ 2013/03/19 by nakahiro

  BTL_COMPETITOR_MAX,

}BtlCompetitor;

//--------------------------------------------------------------
/**
 *  通信タイプ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMM_NONE,     ///< 通信しない
  BTL_COMM_WIRELESS, ///< 3DS無線通信
  BTL_COMM_WIFI,     ///< Wi-Fi通信
  BTL_COMM_IR,       ///< 赤外線通信

}BtlCommMode_tag;

typedef u8 BtlCommMode;

//--------------------------------------------------------------
/**
 *  天候
 */
//--------------------------------------------------------------
typedef enum {

  BTL_WEATHER_NONE = 0,   ///< 天候なし

  BTL_WEATHER_SHINE,    ///< はれ
  BTL_WEATHER_RAIN,     ///< あめ
  BTL_WEATHER_SNOW,     ///< あられ
  BTL_WEATHER_SAND,     ///< すなあらし

  //@バトル変更箇所 田谷さん確認済み
  BTL_WEATHER_STORM,    ///< おおあめ
  BTL_WEATHER_DAY,      ///< おおひでり
  BTL_WEATHER_TURBULENCE,  ///< 乱気流

  BTL_WEATHER_MAX,

}BtlWeather_tag;

typedef u8 BtlWeather;



typedef enum
{
  BTL_CLIENT_PLAYER,
  BTL_CLIENT_ENEMY1,
  BTL_CLIENT_PARTNER,
  BTL_CLIENT_ENEMY2,
  BTL_CLIENT_NUM,
  BTL_CLIENT_NULL = BTL_CLIENT_NUM,

  // バトルロイヤル用の定義
  BTL_CLIENT_ROYAL1    = BTL_CLIENT_PLAYER,                                // バトルロイヤルのクライアント1
  BTL_CLIENT_ROYAL2    = BTL_CLIENT_ENEMY1,                                // バトルロイヤルのクライアント2
  BTL_CLIENT_ROYAL3    = BTL_CLIENT_PARTNER,                               // バトルロイヤルのクライアント3
  BTL_CLIENT_ROYAL4    = BTL_CLIENT_ENEMY2,                                // バトルロイヤルのクライアント4
  BTL_CLIENT_ROYAL_MIN = BTL_CLIENT_ROYAL1,                                // バトルロイヤルにおけるクライアントIDの最小値
  BTL_CLIENT_ROYAL_MAX = BTL_CLIENT_ROYAL4,                                // バトルロイヤルにおけるクライアントIDの最大値
  BTL_CLIENT_ROYAL_NUM = BTL_CLIENT_ROYAL_MAX - BTL_CLIENT_ROYAL_MIN + 1,  // バトルロイヤルにおけるクライアントの数

}BTL_CLIENT_ID;

//-----------------------------------------------------------------------------------
/**
 * フィールドの状態から決定されるバトルシチュエーションデータ
 */
//-----------------------------------------------------------------------------------
typedef struct {

  btl::BgComponentData  bgComponent;

  BtlWeather    weather;

  // 天球時間固定用(バトル開始時の時間が常に入っているfix_timeが有効な時)
  u8               hour;
  u8               minute;

  ///地名ID（なつき度計算用）
  u16         place_id;
  ///トレーナー戦で相手トレーナーのポケモンレベルを調整(FM管理の通常トレーナーにのみ適用)
  s8          trPokeLvAdjust;
  // バトルビデオでの天球時間固定用(fix_timeが有効な時)
  b8          fix_time;

  // プレイヤー方向（天球方向用）
  f32         player_rotate; // 0-359 度

  // 天球の状態
  u32         sky_type;           // 天球の空の種類
  bool        is_cloud_enable;    // 浮雲が有効になっているか

}BTL_FIELD_SITUATION;

//-----------------------------------------------------------------------------------
/**
 * フィールドからバトルに引き渡すべきフラグ群
 */
//-----------------------------------------------------------------------------------
typedef enum {
  //フラグ数は32まで
  BTL_STATUS_FLAG_FISHING            = (1 <<  0),  ///< 釣り戦闘ならTRUE
  BTL_STATUS_FLAG_PCNAME_OPEN        = (1 <<  1),  ///< パソコンの名前がオープンされているか？
  BTL_STATUS_FLAG_LEGEND             = (1 <<  2),  ///< 伝説級ポケモンとのエンカウントならTRUE(Btlエンカウントメッセージ変化)
  BTL_STATUS_FLAG_MOVE_POKE          = (1 <<  3),  ///< 移動ポケモンとのエンカウントならTRUE
  BTL_STATUS_FLAG_HIGH_LV_ENC        = (1 <<  4),  ///< ハイレベルエンカウントならTRUE(図鑑の埋まり方に比例して捕獲率が変動/BW世代の濃い草むら用だがバトルで実装済み)
  BTL_STATUS_FLAG_BOXFULL            = (1 <<  5),  ///< 手持ち・ボックスが満杯でもう捕獲できない時にTRUE
  BTL_STATUS_FLAG_WILD_TALK          = (1 <<  6),  ///< 話しかけでの野生戦の時にTRUE(Btlエンカウントメッセージ変化)
  BTL_STATUS_FLAG_NO_LOSE            = (1 <<  7),  ///< ゲームオーバーにならないバトルならTRUE
  BTL_STATUS_FLAG_LEGEND_EX          = (1 <<  8),  ///< シナリオラストのXY戦専用（経験値なし・倒したメッセージ表示なし）
  BTL_STATUS_FLAG_WCS_FINAL          = (1 <<  9),  ///< WCS決勝
  BTL_STATUS_FLAG_CAMERA_OFF         = (1 << 10),  ///< カメラ動作オフ
  BTL_STATUS_FLAG_CONFIG_SHARE       = (1 << 11),  ///< コンフィグ設定を親機のもので行う
  BTL_STATUS_FLAG_ENDING_BATTLE      = (1 << 12),  ///< エンディングのAZ特殊戦闘（経験値なし）
  BTL_STATUS_FLAG_SWIM               = (1 << 13),  ///< 波乗り状態で戦闘に入ったらTRUE(ダイブボール用)波乗り＋ダイビング
  BTL_STATUS_FLAG_VS_NUSI            = (1 << 14),  ///< ぬし戦( 捕獲不可・逃げる不可・オーラ効果付与・テキスト変化・AI強化 )
  BTL_STATUS_FLAG_SIREN_BATTLE       = (1 << 15),  ///< 試練中の戦闘( 捕獲不可・逃げる不可 )
  BTL_STATUS_FLAG_OTEIRE_DISABLE     = (1 << 16),  ///< お手入れ禁止( 戦闘終了時、お手入れボタンが表示されない )
  BTL_STATUS_FLAG_VS_ULTRA_BEAST     = (1 << 17),  ///< ウルトラビースト戦( AI強化・所属ワードなし )
  BTL_STATUS_FLAG_VS_BOSS            = (1 << 18),  ///< 代表ビースト戦
  BTL_STATUS_FLAG_RIDEWARE_LAND      = (1 << 19),  ///< ライド衣装(陸)を着ている
  BTL_STATUS_FLAG_RIDEWARE_SWIM      = (1 << 20),  ///< ライド衣装(水)を着ている
  BTL_STATUS_FLAG_IN_POKE_SANCTUARY  = (1 << 21),  ///< 保護区での戦闘( 捕獲不可 )
  BTL_STATUS_FLAG_VS_FIRST_UTUROIDO  = (1 << 22),  ///< 初めてのウツロイド戦( 相手が５ターン目に逃げる )
  BTL_STATUS_FLAG_AI_TEND_USE_ZPOWER = (1 << 23),  ///< 敵AIがZパワーを使用しやすくなる
  BTL_STATUS_FLAG_BATTLE_FES         = (1 << 24),  ///< バトルフェスサークルでのバトル
  BTL_STATUS_FLAG_TRAINER_NO_BALL    = (1 << 25),  ///< トレーナー戦でボールにポケモンが入っていない(momiji:ビースト世界対応+ハウ特殊導入
  BTL_STATUS_FLAG_WIN_BGM_CONTINUE   = (1 << 26),  ///< WinBGM継続再生する際のオプション
  BTL_STATUS_FLAG_VS_NEKUROZUMA      = (1 << 27),  ///< ネクロズマ戦専用処理(現状オーラ＋効果表記しない)
  BTL_STATUS_FLAG_NO_CAPTURE         = (1 << 28),  ///< 合体ネクロズマ、ヒカリドラゴン戦専用 捕獲不可戦闘（＋逃げる不可）
  BTL_STATUS_FLAG_TRAINER_NO_RETURN_BALL = (1 << 29),  ///< トレーナー戦でボールにポケモンが戻らない(momiji:ビースト世界対応
  BTL_STATUS_FLAG_NO_WILD_MESSAGE    = (1 << 30),  ///< 合体ネクロズマ、ヒカリドラゴン戦専用処理（やせいの～を使用しない）
}BTL_STATUS_FLAG;

//-------------------------------------
/// 下記はshooter_item.cの中でビットが足りるかコンパイラアサートをしています
//  直接BITをMAXに変換しないのは、企画の方がエクセルをいじるとMAXが勝手にかわる場合があるため
//  それを防ぐようにしています
//  （例えば、配信用レギュレーションでも使われているので容易に変えられないようにしています）
//=====================================
enum {
  SHOOTER_ITEM_BIT_TBL_MAX  = 7,
};
//-------------------------------------
/// シューターアイテム使用許可テーブル
//    直接BATTLE_SETUP_PARAMの構造体に載せられるように
//    外部公開しています。
//=====================================
typedef struct
{
  u8  bit_tbl[ SHOOTER_ITEM_BIT_TBL_MAX ];  //シューターでの禁止道具
  u8  shooter_use;                          //シューターを使用するかどうか
} SHOOTER_ITEM_BIT_WORK;

//-----------------------------------------------------------------------------------
/**
 * フィールドエンカウントエフェクトデータ構造体
 */
//-----------------------------------------------------------------------------------
typedef struct
{
  u32 eff_kind;   //エンカウント種類(野性とかトレーナーとか)
}BSP_ENCOUNT_EFFECT_FIELD;

//-----------------------------------------------------------------------------------
/**
 * エンカウントエフェクトデータ構造体
 */
//-----------------------------------------------------------------------------------
typedef struct
{
  u32 eff_kind;   //エンカウント種類(野性とかトレーナーとか)
  u32 enc_msg;    //エンカウントメッセージID BattleEncountMsgId型
}BSP_ENCOUNT_EFFECT;

//-----------------------------------------------------------------------------------
/**
 * フィールドから引き渡されるバトル演出パラメータセット(エンカウントエフェクト、エンカウントメッセージ、BGMなど)
 */
//-----------------------------------------------------------------------------------
typedef struct {
  BSP_ENCOUNT_EFFECT_FIELD  enceff_fld; //フィールドで使うパラメータ
  BSP_ENCOUNT_EFFECT        enceff;     //バトルで使うパラメータ
  u32 bgm_default;
  u32 bgm_win;
}BSP_BATTLE_EFFECT;

//--------------------------------------------------------------
/**
 *  バトル後ポケモン状態コード
 */
//--------------------------------------------------------------

typedef enum {

  BTL_POKESTATE_NORMAL = 0, ///< 異常なし
  BTL_POKESTATE_SICK,       ///< 状態異常
  BTL_POKESTATE_DEAD,       ///< ひん死

}BtlPokeStatCode_tag;

typedef u8 BtlPokeStatCode;

//--------------------------------------------------------------
/**
 *  ポケモンひん死の時の処理タイプ
 */
//--------------------------------------------------------------
typedef enum {
  BTL_CHANGEMODE_IREKAE = 0,  ///< いれかえ
  BTL_CHANGEMODE_KATINUKI,    ///< かちぬき
}BtlChangeMode;

//--------------------------------------------------------------
/**
*  バトルフェスフィールド効果タイプ
*/
//--------------------------------------------------------------
typedef enum {
  BATTLE_FES_FIELD_NONE = 0,  ///< フィールド効果なし
  BATTLE_FES_FIELD_SHINE,     ///< はれ
  BATTLE_FES_FIELD_RAIN,      ///< あめ
  BATTLE_FES_FIELD_SNOW,      ///< あられ
  BATTLE_FES_FIELD_SAND,      ///< すなあらし
  BATTLE_FES_FIELD_ELEKI,     ///< エレキフィールド
  BATTLE_FES_FIELD_MIST,      ///< ミストフィールド
  BATTLE_FES_FIELD_PHYCHO,    ///< サイコフィールド
  BATTLE_FES_FIELD_GRASS,     ///< グラスフィールド
  BATTLE_FES_FIELD_DAY,       ///< おおひざし
  BATTLE_FES_FIELD_STORM,     ///< おおあめ
  BATTLE_FES_FIELD_TURBULENCE,///< らんきりゅう

  BATTLE_FES_FIELD_MAX        ///< ＭＡＸ
}BtlFesField;


//--------------------------------------------------------------
/**
 *  デバッグフラグ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_DEBUGFLAG_MUST_TUIKA = 0,         ///< 追加効果の発生率を100％にする
  BTL_DEBUGFLAG_MUST_TOKUSEI,           ///< 一部とくせいの効果発生率を100％にする
  BTL_DEBUGFLAG_MUST_ITEM,              ///< 一部アイテムの効果発生率を100％にする
  BTL_DEBUGFLAG_MUST_CRITICAL,          ///< 打撃ワザのクリティカル発生率を100％にする
  BTL_DEBUGFLAG_MUST_CRITICAL_CAPTURE,  ///< 必ず捕獲クリティカル
  BTL_DEBUGFLAG_HP_CONST,               ///< HP減らない
  BTL_DEBUGFLAG_PP_CONST,               ///< PP減らない
  BTL_DEBUGFLAG_HIT100PER,              ///< 命中率が常に100％
  BTL_DEBUGFLAG_DMG_RAND_OFF,           ///< ダメージのランダム補正をオフ
  BTL_DEBUGFLAG_SKIP_BTLIN,             ///< 入場エフェクトスキップ（非通信時のみ）
  BTL_DEBUGFLAG_AI_CTRL,                ///< AIを起動せずにプレイヤーが操作する
  BTL_DEBUGFLAG_DISABLE_CRITICAL,       ///< 急所にあたらない（BTL_DEBUGFLAG_MUST_CRITICALより優先）
  BTL_DEBUGFLAG_INTRUDE_FORCE_CALL,     ///< 乱入( 必ず助けを呼ぶ )
  BTL_DEBUGFLAG_INTRUDE_FORCE_APPEAR,   ///< 乱入( 必ず助けに来る )

  BTL_DEBUGFLAG_MAX,

}BtlDebugFlag;

//--------------------------------------------------------------
/**
 *  定数
 */
//--------------------------------------------------------------
enum {
  BTL_BONUS_ITEM_COUNT_MAX = 3,   ///< 背景破壊で手に入るボーナスアイテムの最大数
  BTL_EV_SCENE_POKE_LOG_ENEMY_NUM_MAX = 2,  ///< BATTLE_SETUP_PARAMのevScenePokeLogEnemyMonsNoの要素数
};

//-----------------------------------------------------------------------------------
/**
 * PGL 集計データ
 */
//-----------------------------------------------------------------------------------
typedef struct {
  u16  deadPokeNumber;    ///< ひん死になった場合はポケモンナンバーが入る／ならなければ MONSNO_NULL
  u16  deadPokeForm;      ///< ひん死になった時のフォルムナンバー
  u16  finishWazaNumber;  ///< 相手のワザによってひん死になったらワザナンバーが入る／それ以外 WAZANO_NULL
  u16  attackPokeNumber;  ///< そのワザを出した相手ポケモンナンバー
  u16  attackPokeForm;    ///< そのワザを出した相手ポケモンフォルムナンバー
}BATTLE_PGL_RECORD;

typedef struct {
  BATTLE_PGL_RECORD   myParty[ pml::PokeParty::MAX_MEMBERS ];      ///< 自分パーティ
  BATTLE_PGL_RECORD   enemysParty[ pml::PokeParty::MAX_MEMBERS ];  ///< 相手パーティ
}BATTLE_PGL_RECORD_SET;


//-----------------------------------------------------------------------------------
/**
 * バトル検定集計データ
 */
//-----------------------------------------------------------------------------------
typedef struct {
  u32  TurnNum;       // かかったターン数
  u16  HPSum;         // 生き残りポケモンのHP総和
  u16  PokeChgNum;    // 交代回数
  u16  VoidAtcNum;    // 効果がない技を出した回数
  u16  WeakAtcNum;    // ばつぐんの技を出した回数
  u16  ResistAtcNum;  // いまひとつの技を出した回数
  u16  VoidNum;       // 効果がない技を受けた回数
  u16  ResistNum;     // いまひとつの技を受けた回数
  u16  WinTrainerNum; // 倒したトレーナー数
  u16  WinPokeNum;    // 倒したポケモン数
  u16  LosePokeNum;   // 倒されたポケモン数
  u16  UseWazaNum;    // 使用した技の数
}BATTLE_KENTEI_RESULT;

//-------------------------------------
/// 大会演出用データ
//=====================================
struct BATTLE_CONVENTION_INFO{
  u32 CommBattle;   ///< 通信での対戦回数  99999
  u32 Egg;          ///< タマゴ孵化回数    99999
  u32 BattleHouse;  ///< バトルハウス最高連勝数  9999
  u32 TrialHouse;   ///< トライアルハウス最高スコア  9999
  u32 CaptureNum;   ///< 捕獲したポケモンの数   99999
  u32 Evolution;    ///< 進化させたポケモンの数 99999
  u32 Bp;           ///< 勝ち取ったBP累計      999999
  u32 Judge;        ///< ジャッジに判定してもらった回数 999999
  u32 Sparing;      ///< スパーリングクリア回数 99999
  u32 Pointup;      ///< ポイントアップ使用回数 9999
  u32 Dendou;       ///< 殿堂入りした回数       9999

  // コンストラクタ
  BATTLE_CONVENTION_INFO(){
    STATIC_ASSERT( sizeof(BATTLE_CONVENTION_INFO) == 44 );
  }
};


/**
 * @brief 通信対戦の見せ合いにおいて、相手に見せたデータと、その選出結果
 */
struct MiseaiData
{
  pml::PokeParty* partyFullMember;                                     // 相手に見せたパーティ( 選出されなかったポケモンを含む )
  s8              memberSelectedIndex[ pml::PokeParty::MAX_MEMBERS ];  // 各メンバーが何番目のポケモンとして選出されたか( 値は0オリジン、選出されていない場合は負数 )

  /**
   * @brief コンストラクタ
   */
  MiseaiData( void ) :
    partyFullMember( NULL )
  {
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i ) {
      memberSelectedIndex[i] = -1;
    }
  }
};


/**
 * @brief ポケモン毎の戦闘結果
 */
struct PokeResult
{
  u16                     totalTurnCount; // 場に出たターンの総数
  pml::pokepara::DirtType dirtType;       // 汚れの種類
};


/**
 * @brief バトル録画のヘッダ情報
 */
struct BattleRecordHeader
{
  u16              turnNum;  // 総ターン数
  BtlRecordResult1 result1;  // 対戦結果１
  BtlRecordResult2 result2;  // 対戦結果２
};


/**
 * @brief パーティごとのパラメータ
 */
struct PartyParam
{
  DefaultPowerUpDesc defaultPowerUpDesc[ pml::PokeParty::MAX_MEMBERS ];  // ポケモン毎の能力アップ設定
};

/**
 * @brief バトルロイヤルの結果
 */
struct BattleRoyalResult
{
  u8  clientRanking[ BTL_CLIENT_NUM ];                         // 各クライアントの順位( 0オリジン )
  u8  killCount[ BTL_CLIENT_NUM ];                             // 各クライアントが倒したポケモンの数
  u16 maxHp[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ];  // 各ポケモンの最大HP
  u16 restHp[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ]; // 各ポケモンの残りHP
  s8  lastPokePartyIndex[ BTL_CLIENT_NUM ];                    // 戦闘終了時に場にいたポケモンのパーティ内インデックス( 全滅して場にポケモンがいなかった場合 -1 )
};


//-----------------------------------------------------------------------------------
/**
 *  バトルセットアップパラメータ
 */
//-----------------------------------------------------------------------------------
typedef struct {

  //バトルルール
  BtlCompetitor   competitor;
  BtlRule         rule;

  //フィールドの状態から決定されるバトルシチュエーションデータ
  BTL_FIELD_SITUATION            fieldSituation;
  BSP_BATTLE_EFFECT              btlEffData;      ///< エンカウントエフェクト+BGM
  pml::pokepara::EvolveSituation evolveSituation; ///< 進化のための状況

  //通信データ
  BtlCommMode              commMode;
  BtlMultiMode             multiMode;                    ///< マルチ対戦モード（ダブルバトル時のみ有効）
  u8                       commPos;                      ///< 通信対戦なら自分の立ち位置（非通信時は常に0を指定）
  BtlRecordDataMode        recordDataMode;               ///< TRUEだと録画データ再生モード
  MiseaiData*              miseaiData[ BTL_CLIENT_NUM ]; ///< 見せ合いデータ
  bool                     isPlayerRatingDisplay;        ///< プレイヤのレーティングを表示するか？
  bool                     isLiveRecSendEnable;          ///< ライブ録画送信可能か
#if PM_DEBUG
  bool                     isLiveRecPlayMode;            ///< ライブ録画再生モードか
#endif

  // [io] パーティデータ・対戦相手ステータス
  //（通信バトル後、録画データ生成のために操作プレイヤー以外のデータが格納される）
  pml::PokeParty*          party[ BTL_CLIENT_NUM ];            ///< 戦闘に出るポケモンのみを含むパーティ
  PartyParam               partyParam[ BTL_CLIENT_NUM ];       ///< 各パーティごとの設定
  Savedata::MyStatus*      playerStatus[ BTL_CLIENT_NUM ];
  u16                      playerRating[ BTL_CLIENT_NUM ];     ///< プレイヤのレーティング( 通信対戦でのみ有効 )

  // トレーナーデータ
  BSP_TRAINER_DATA*       tr_data[BTL_CLIENT_NUM]; ///< AIトレーナーのデータ


  // セーブデータ系
  GameSys::GameData*      gameData;     ///< GameData
  Savedata::MyItem*       itemData;     ///< アイテムデータ
  Savedata::ZukanData*    zukanData;    ///< 図鑑データ
  Savedata::ConfigSave*   configData;   ///< Configデータ
  u8                      bGakusyuSouti;///< がくしゅうそうち有効フラグ
  u8                      megaFlags[ PokeTool::Mega::MEGA_SEE_FLAG_BYTES ];///<メガ進化見たフラグ列


  // 制限時間設定
  u16         LimitTimeGame;        ///< 試合制限時間（秒）         [ 0 = 無制限 ]
  u16         LimitTimeClient;      ///< 持ち時間（秒）             [ 0 = 無制限 ]
  u16         LimitTimeCommand;     ///< コマンド選択制限時間（秒） [ 0 = 無制限 ]
  bool        bEnableTimeStop;      ///< ふた閉じ時にタイマーを止めるか？(赤外線ライブ大会のみ有効)

  // 各種ステータスフラグ
  SHOOTER_ITEM_BIT_WORK  shooterBitWork;      ///< シューター使用制限パラメータ
  u8                     maxFollowPokeLevel;  ///< 何レベルのポケモンまで命令無視をしないか？
  u16                    commNetIDBit;        ///< 通信対戦時のNetIDBitフラグ
  u32                    btl_status_flag;     ///< 各種ステータスフラグ(定義値 BTL_STATUS_FLAG型)
  f32                    moneyRate;           ///< 賞金レート(明示的に設定しなければ1.0)

  // 特殊バトル
  bool                   bSkyBattle;         ///< TRUE なら スカイバトルである
  bool                   bSakasaBattle;      ///< TRUE なら さかさバトルである
  bool                   bMustCapture;       ///< TRUE なら 野生戦で必ずポケモンを捕まえられる
  bool                   bNoGainBattle;      ///< TRUE なら お金・経験値が入らない

  // 乱入
  IntrudeBattleParam   intrudeParam;   ///< 乱入の設定
  ReinforceBattleParam reinforceParam; ///< 援軍の設定

  //VCT
  bool bVoiceChat;                         ///< ボイスチャットしていたらTRUE。それ以外はFALSE

  // 大会演出用データ
  BATTLE_CONVENTION_INFO  conventionInfo[ 2 ];  ///< ２人分しか必要ない

  // デバッグ系
  u16         DebugFlagBit;          ///< 各種デバッグフラグ

  //ロトム関係
  bool bEnableRotom;                          ///<ロトムが有効か？
  Savedata::FieldMenu::RotomRank  rotomRank;  ///<ロトムの仲良し度

  // バトルフェスフィールド関連
  BtlFesField btlFesFieldType;  ///< バトルフェスのフィールド効果タイプ

  //----- 以下、バトルの結果格納パラメータ ----
  int         getMoney;         ///< 手に入れたお小遣い
  BtlResult   result;           ///< 勝敗結果
  u8          capturedPokeIdx;  ///< 捕獲したポケモンのメンバー内インデックス（partyEnemy1 内）
  u8          commServerVer;    ///< 通信対戦時のサーババージョン（接続したマシン中、最後バージョン）

  u8*                recBuffer;        ///< 録画データ生成先
  u32                recDataSize;      ///< 録画データサイズ
  u32                recRandSeed;
  BattleRecordHeader recHeader;        ///< 録画データのヘッダ情報

  // -- バトル検定結果 --
  BATTLE_KENTEI_RESULT    kenteiResult;

  // バトルロイヤルの結果
  BattleRoyalResult battleRoyalResult;


  u8 cmdIllegalFlag       : 1;  // コマンド不正チェックフラグ（子機のみ操作。親機は常にFALSE）
  u8 recPlayCompleteFlag  : 1;  // 録画再生モードで最後まで再生できたらTRUE。それ以外はFALSE
  u8 WifiBadNameFlag      : 1;  // Wifiで対戦相手が不正名だったらTRUE。それ以外はFALSE
  u8 zenryokuWazaUsedFlag : 1;  // 全力技を使用したか？
  u8 isDisconnectOccur    : 1;  // 切断が発生したらtrue、発生していなければfalse

  bool oteireFlag;          //「お手入れ」ボタンが押されたか？
  u8   oteireTargetIndex;   //「お手入れ」対象ポケモンのパーティ内インデックス

  u8 fightPokeIndex[ pml::PokeParty::MAX_MEMBERS ];      ///< 戦闘に出たポケモンのパーティ内Indexと同じ位置がTRUEになる（それ以外はFALSE）
  u8 turnedLvUpPokeIndex[ pml::PokeParty::MAX_MEMBERS ]; ///< さかさ進化条件を満たしたポケモンに対してTRUEになる
  PokeResult pokeResult[ pml::PokeParty::MAX_MEMBERS ];  ///< ポケモン毎の戦闘結果

  // ひん死・状態異常など結果格納
  BtlPokeStatCode  party_state[ BTL_CLIENT_NUM ][ pml::PokeParty::MAX_MEMBERS ];
  u32              restHPRatio[ BTL_CLIENT_NUM ];

  // PGL 用記録
  BATTLE_PGL_RECORD_SET   PGL_Record;
  
  //@バトル変更箇所
  //sangoテレビナビ用データ格納
  BATTLE_TVNAVI_DATA tvNaviData;


#if PM_DEBUG
  btl::BTLV_DEBUG_PARAM  btlvDebugParam;
#endif

  //6末
  static int pokeDecoType;
#if PM_DEBUG
	static bool isDebugUseMomijiOptimize;	// momijiでの処理負荷最適化を利用するか？
#endif
}BATTLE_SETUP_PARAM;

/*
 *  @brief  戦闘フィールドシチュエーションデータデフォルト初期化（室内設定・ライトなし）
 */
inline void BATTLE_SETUP_FIELD_SITUATION_Init( BTL_FIELD_SITUATION* sit )
{
  btl::GetBgComponentData( btl::bg::FAR_TYPE_NONE , btl::bg::NEAR_TYPE_NONE , &sit->bgComponent );

  //録画再生のために時間を入れておく。fix_timeが立っていなければ使われない
  GameSys::AlolaTime time;
  time.Initialize();
  sit->hour   = time.GetHour();
  sit->minute = time.GetMinute();
  sit->place_id = 0;
  sit->player_rotate = 0;
  sit->fix_time = false;
  sit->sky_type = 0;
  sit->is_cloud_enable = false;
  sit->weather = BTL_WEATHER_NONE;
  sit->trPokeLvAdjust = 0;
}

    extern void load_tr_type_data( BSP_TRAINER_DATA* tr_data, u32 tr_type, gfl2::heap::HeapBase* pTempHeap );

//=============================================================================================
/**
 * バトルエフェクトパラメータ取得
 *
 * @param[in]    btl_eff_id
 * @param[out]   pBtlEff
 */
//=============================================================================================
extern void BATTLE_EFFECT_GetParam( BattleEffectId btl_eff_id, BSP_BATTLE_EFFECT *pBtlEff );


//=============================================================================================
/**
 *  @brief  戦闘パラメータワークの内部初期化処理
 *  @param  bp  確保済みのBATTLE_SETUP_PARAM構造体型ワークへのポインタ
 *
 *  ＊デフォルトのパラメータで構造体を初期化します。バトルタイプに応じて、必要な初期化を追加で行ってください
 *  ＊使い終わったら必ずBATTLE_PARAM_Release()関数で解放処理をしてください
 */
//=============================================================================================
extern void BATTLE_SETUP_Init( BATTLE_SETUP_PARAM* bp );

//=============================================================================================
/**
 * 野生戦用パラメータセット
 *
 * @param[out]   bp           パラメータセットする構造体ポインタ
 * @param[in]    gameMan
 * @param[in]    partyEnemy
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    reinforcePokeSet  援軍がない場合NULL, 援軍がある場合, エンカウントデータを持っている PokeSet
 * @param[in]    pHeap
 */
//=============================================================================================
extern void BATTLE_SETUP_Wild
(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager          *gameMan,
  const pml::PokeParty         *partyEnemy,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  BattleEffectId                btl_eff_id,
  Field::Encount::PokeSet*      reinforcePokeSet,
  gfl2::heap::HeapBase          *pHeap
);

//=============================================================================================
/**
 * 通常トレーナー戦用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    partyEnemy
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer
(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap
);

//=============================================================================================
/**
 * トレーナー戦（タッグ）用パラメータセット  /  タッグ = プレイヤー vs AIx2
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    trID_1
 * @param[in]    trID_2
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer_Tag(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID_1,
  trainer::TrainerID            trID_2,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap
);


//=============================================================================================
/**
 * トレーナー戦（AIマルチ）用パラメータセット  /  AIマルチ = プレイヤー&AI vs AIx2
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    trID_friend
 * @param[in]    trID_1
 * @param[in]    trID_2
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_Trainer_Multi(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  trainer::TrainerID            trID_friend,
  trainer::TrainerID            trID_1,
  trainer::TrainerID            trID_2,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  gfl2::heap::HeapBase          *pHeap
);

//=============================================================================================
/**
 * トレーナー戦　バトルハウス　用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    btl_tr_no
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    my_party
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_BattleHouseTrainer(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap );

//=============================================================================================
/**
 * トレーナー戦　バトルハウスマルチ　用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    my_party
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_BattleHouseTrainer_Multi(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap );

//=============================================================================================
/**
 * トレーナー戦　ロイヤル施設　用パラメータセット
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    my_party
 * @param[in]    pHeap
 */
//=============================================================================================
void BTL_SETUP_BattleRoyalInst(
  BATTLE_SETUP_PARAM            *bp,
  GameSys::GameManager          *gameMan,
  const BTL_FIELD_SITUATION     *sit,
  pml::PokeParty                *my_party,
  gfl2::heap::HeapBase          *p_heap );

//=============================================================================================
/**
 * 通信対戦（マルチ以外）セットアップ
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    commMode
 * @param[in]    commPos
 * @param[in]    pHeap
 */
//=============================================================================================
void BATTLE_SETUP_Comm
(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlCommMode                  commMode,
  u8                           commPos,
  gfl2::heap::HeapBase          *pHeap
);

//=============================================================================================
/**
 * 通信マルチモード（通常）へ変換
 * ※ BATTLE_SETUP_Comm の直後に呼び出すこと
 *
 * @param[out]   bp
 */
//=============================================================================================
void BATTLE_SETUP_ToMultiMode( BATTLE_SETUP_PARAM *bp );

//=============================================================================================
/**
 * 通信マルチモード（AIマルチ）へ変換
 *
 * @param   bp
 * @param   gameData
 * @param   trid_enemy1
 * @param   trid_enemy2
 * @param   competitor
 * @param   pHeap
 */
//=============================================================================================
void BATTLE_SETUP_ToAIMultiMode(
  BATTLE_SETUP_PARAM*   bp,
  GameSys::GameData* gameData,
  trainer::TrainerID    trid_enemy1,
  trainer::TrainerID    trid_enemy2,
  BtlCompetitor         competitor,
  gfl2::heap::HeapBase*  pHeap );

//=============================================================================================
/**
 * バトルハウス通信対戦セットアップ
 *
 * @param[out]   bp
 * @param[in]    gameMan
 * @param[in]    sit
 * @param[in]    rule
 * @param[in]    my_party
 * @param[in]    comm_party
 * @param[in]    isParent
 * @param[in]    pHeap
 */
//=============================================================================================
void BATTLE_SETUP_BattleHouseComm(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  BtlRule                       rule,
  pml::PokeParty               *my_party,
  pml::PokeParty               *comm_party,
  u8                           isParent,
  gfl2::heap::HeapBase          *pHeap );

//=============================================================================================
/**
 * バトルハウス通信マルチモード（AIマルチ）へ変換
 *
 * @param   bp
 * @param   pHeap
 */
//=============================================================================================
void BATTLE_SETUP_ToAIMultiModeForBattleHouse(
  BATTLE_SETUP_PARAM* bp,
  GameSys::GameManager* gameMan,
  gfl2::heap::HeapBase* pHeap );

//=============================================================================================
/**
 * パラメータクリア（動的に確保したメンバポインタの解放など）
 *
 * @param[out]   bp
 */
//=============================================================================================
extern void BATTLE_SETUP_Clear( BATTLE_SETUP_PARAM  *bp );



/**
 *  @brief  バトルパラム　バトルステータスフラグセット
 */
inline void BATTLE_PARAM_SetBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f)
{
  bp->btl_status_flag |= status_f;
}

/**
 *  @brief  バトルパラム バトルステータスフラグチェック
 */
inline void BATTLE_PARAM_ResetBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f )
{
   bp->btl_status_flag &= ~status_f;
}

/**
 *  @brief  バトルパラム バトルステータスフラグチェック
 */
inline bool BATTLE_PARAM_CheckBtlStatusFlag( const BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f )
{
  return (bp->btl_status_flag & status_f?true:false);
}

/**
 * バトルエフェクトパラメータ セット
 *
 * @param[out]   bp
 * @param[in]    btl_eff_id
 *
 * @note  BTL_SETUP_Trainer系は初期化時にトレーナータイプデータから
 *        初期値が決定されているため、後から上書きしたい時のみ呼び出す
 */
extern void BATTLE_PARAM_SetBattleEffect( BATTLE_SETUP_PARAM* bp, BattleEffectId btl_eff_id );

/**
 *  制限時間設定
 *
 * @param[out]  bsp
 * @param[in]   GameLimitSec      試合時間制限（秒／０なら無制限）
 * @param[in]   ClientLimitSec    持ち時間（秒／０なら無制限）
 * @param[in]   CommandLimitSec   コマンド選択時間制限（秒／０なら無制限）
 */
void BATTLE_PARAM_SetTimeLimit( BATTLE_SETUP_PARAM* bsp, u32 GameLimitSec, u32 ClientLimitSec, u32 CommandLimitSec );

/**
 *  スカイバトル設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetSkyBattle( BATTLE_SETUP_PARAM* bsp );

/**
 *  さかさバトル設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetSakasaBattle( BATTLE_SETUP_PARAM* bsp );

/**
 * 経験値・お金が入らないモード設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetNoGainMode( BATTLE_SETUP_PARAM* bsp );

/**
 *  必ず捕獲モード設定
 *
 * @param[out]  bsp
 */
void BATTLE_PARAM_SetMustCaptureMode( BATTLE_SETUP_PARAM* bsp );
/**
 *  賞金レート設定
 *
 * @param[out]  bsp
 *
 * @note  野生戦は賞金が発生しないので関係ない(ネコにこばんには影響しない)
 */
void BATTLE_PARAM_SetMoneyRate( BATTLE_SETUP_PARAM* bsp, f32 rate );

/**
 * ポケモンパーティを外部からセット
 *
 * @param[out]  bsp
 * @param[in]   party
 * @param[in]   id
 */
void BATTLE_PARAM_SetPokeParty( BATTLE_SETUP_PARAM* bsp, const pml::PokeParty * party, BTL_CLIENT_ID id );

/**
 * @breif   バトル引数にレギュレーションを設定
 *          必ずすべてのバトルパラメータを設定した後に呼んでください
 *
 * @param[out]  bsp
 * @param[in]   reg
 * @param[in]   heap
 *
 * @note 内部で行っていること
 *        ・制限時間設定
 *        ・ニックネーム補正
 *        ・レベル補正
 *        ・カメラモード設定
 *        ・逆さバトル
 */
void BATTLE_PARAM_SetRegulation( BATTLE_SETUP_PARAM* bsp, Regulation* reg, gfl2::heap::HeapBase* heap );

/**
 * @breif   レート値をセット
 *
 * @param[out]  bsp
 * @param[in]   rate1   BTL_CLIENT_PLAYERのレート値
 * @param[in]   rate2   BTL_CLIENT_ENEMY1のレート値
 */
void BATTLE_PARAM_SetRatingValue( BATTLE_SETUP_PARAM* bsp, u16 rate1, u16 rate2 );

/**
 * @brief セットアップ済みパラメータに録画バッファを生成
 * @param[out]  bsp
 * @param[in]   heap
 */
void BATTLE_PARAM_AllocRecBuffer( BATTLE_SETUP_PARAM* bsp, gfl2::heap::HeapBase* heap );

/*
 *  @brief デバッグ用フラグリセット
 */
void BTL_SETUP_ResetDebugFlag( BATTLE_SETUP_PARAM* bsp );

/*
 *  @brief デバッグ用フラグセット
 */
void BTL_SETUP_SetDebugFlag( BATTLE_SETUP_PARAM* bsp, BtlDebugFlag flag );

/*
 *  @brief デバッグ用フラグ取得
 */
bool BTL_SETUP_GetDebugFlag( const BATTLE_SETUP_PARAM* bsp, BtlDebugFlag flag );

/*
 *  @brief ボイスチャットする場合呼ぶ
 */
void BTL_SETUP_VoiceChatOn(BATTLE_SETUP_PARAM *bp);


/**
 *  @brief バトルクライントIDからコーナー色を取得
 */
CornerColor GetCornerColorFromBtlClientId( BTL_CLIENT_ID btlClietnId );


//=============================================================================================
/**
 * @brief  録画データから構築した BATTLE_SETUP_PARAM を、録画対戦（模擬戦）モードにコンバートする
 * @param[io]  bsp              録画データから構築した BATTLE_SETUP_PARAM （コンバートされ、内容が書き換わります）
 * @param[in]  targetClientID   模擬戦で戦いたいトレーナーデータが格納されている ClientID （0 or 1）
 * @param[in]  gameData         プレイヤーのMyStatus, Partyデータを取得するためのデータ
 * @param[in]  pHeap            必要に応じて対戦相手用の MyStatus を構築するためのヒープ
 * @retval     bool     正しくコンバートされたら true
 */
//=============================================================================================
bool BTL_SETUP_ConvertToRecordFightMode( BATTLE_SETUP_PARAM* bsp, u8 targetClientID, GameSys::GameData* gameData, gfl2::heap::HeapBase* pHeap );

void BTL_SETUP_BattleFesTrainer(
  BATTLE_SETUP_PARAM           *bp,
  GameSys::GameManager      *gameMan,
  const BTL_FIELD_SITUATION    *sit,
  pml::PokeParty               *my_party,
  gfl2::heap::HeapBase          *pHeap);

GFL_NAMESPACE_BEGIN(btl)
namespace setup{
  extern void common_base( BATTLE_SETUP_PARAM* dst, GameSys::GameManager* gameMan, const BTL_FIELD_SITUATION* sit );
}
GFL_NAMESPACE_END(btl)


#endif


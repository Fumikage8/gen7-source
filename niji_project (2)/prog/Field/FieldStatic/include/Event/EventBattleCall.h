//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EventBattleCall.h
 *  @brief バトル呼び出し
 *  @author tomoya takahshi
 *  @date 2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#if !defined( EVENT_BATTLE_CALL_H_INCLUDED )
#define EVENT_BATTLE_CALL_H_INCLUDED
#pragma once

// gamesys
#include "GameSys/include/GameEvent.h"

// battle
#include "Battle\include\battle_SetupParam.h"

// conv_header
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/battle/battle_effect_def.h>
#include <niji_conv_header/trainer/trid_def.h>
#include <niji_conv_header/field/attribute/attribute_id.h>
#include <niji_conv_header/battle/background/bgsys_near_type_def.h>
#include <niji_conv_header/battle/background/bgsys_far_type_def.h>
#include "niji_conv_header/field/WeatherKindDef.h"

// sound
#include "Sound/include/Sound.h"

// field
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/FieldLocation.h"

// inst
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"

// 前方参照
namespace Field{

  class ZoneDataLoader;

  namespace Encount {
    class PokeSet;
    class EncountEffect;
  }

  namespace Event {
    class BattleDllReadThread;
  }
}

GFL_NAMESPACE_BEGIN( Field )


//-----------------------------------------------------------------------------
/**
 *          関数宣言
*/
//-----------------------------------------------------------------------------

// フィールドシチュエーション
extern void SetUpFieldSituation( BTL_FIELD_SITUATION* outSit, GameSys::GameManager* p_gman, const ZoneDataLoader ** cpp_zone_loader, Attr in_attr, btl::bg::bgsys_far_typeID farType = btl::bg::FAR_TYPE_NONE, btl::bg::bgsys_near_typeID nearType = btl::bg::NEAR_TYPE_NONE, s8 trPokeLvAdjust = 0 );

//バトル天候
static BtlWeather GetBattleWeather( GameSys::GameManager* p_gman );

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

///EventBattleCall::Call～()のシーケンス制御オプション。ポケモンパラメータ調整のフラグとは別
enum EventBattleCallOption{
  EVBTL_CALLOPT_NONE                        = 0,        //デフォルト
  EVBTL_CALLOPT_NOT_SCRIPT                  = 1 << 0,   //非スクリプトからのコール
  EVBTL_CALLOPT_SKIP_ENCEFF                 = 1 << 1,   //フィールド側エンカウントエフェクトスキップ。エフェクトは呼び出し元が管理
  EVBTL_CALLOPT_SKIP_FIELD_CLOSE            = 1 << 2,   //フィールド破棄シーケンススキップ。バトル後のフィールド復帰は普通に行う
  EVBTL_CALLOPT_SKIP_FIELD_OPEN             = 1 << 3,   //フィールド生成シーケンススキップ。
  EVBTL_CALLOPT_PLAY_BTL_BGM                = 1 << 4,   //SKIP_ENCEFFと合わせて使うとバトルBGMの再生のみ行う
  EVBTL_CALLOPT_WITH_FADEOUT                = 1 << 5,   //SKIP_ENCEFFと合わせて使うとフィールドクローズと共にブラックフェードを行う
  EVBTL_CALLOPT_POKE_RIDE                   = 1 << 6,   //ポケライド状態にする
  EVBTL_CALLOPT_SKIP_REFLE                  = 1 << 7,   //お手入れ画面スキップ
  EVBTL_CALLOPT_ATTR_POKESET                = 1 << 8,   //pokesetのアトリビュートを使う
  EVBTL_CALLOPT_NOT_INTRUDE                 = 1 << 9,   //バトル中に乱入が起きないようにする
  EVBTL_CALLOPT_ENCEFF_EXTERNAL             = 1 << 10,  //フィールド側エンカウントエフェクトを外部で初期化、再生を行った場合に指定する
  EVBTL_CALLOPT_NO_LOSE                     = 1 << 11,  //全滅無しバトル
  EVBTL_CALLOPT_SKIP_BTL_BGM                = 1 << 12,  //バトルBGM再生スキップ
  EVBTL_CALLOPT_TEND_USE_ZPOWER             = 1 << 13,  //【トレーナー戦のみ】AIがZ技を使いやすくなる（効果なし以外は打つ）
  EVBTL_CALLOPT_RUN_ENEMY_TURN              = 1 << 14,  //【ポケモン戦のみ】規定ターン(5ターン)で相手が逃げるAIが有効になる
  EVBTL_CALLOPT_SKIP_BGM_RECOVER            = 1 << 15,  //WinBGMをフィールドに戻るときに継続する。勝利時のみ有効で利用許可制
  EVBTL_CALLOPT_CAN_NOT_CAPTURE             = 1 << 16,  //momiji追加　合体ネクロズマ、光ドラゴンで使用。捕獲できない
  EVBTL_CALLOPT_AURA                        = 1 << 17,  //momiji追加　光ドラゴンで使用。オーラを纏う設定にする
  EVBTL_CALLOPT_NO_WILD_MESSAGE             = 1 << 18,  //momiji追加　合体ネクロズマ、光ドラゴンで使用。やせいの～メッセージを使用しない
};

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

struct TrainerBattleParam{
  BtlRule rule;  //<バトルルール
  trainer::TrainerID  partnerID;
  trainer::TrainerID  trID0;
  trainer::TrainerID  trID1;
  u32 exFlag;  //<TrainerBtlFlag
  btl::bg::bgsys_far_typeID bgFarType;
  btl::bg::bgsys_near_typeID bgNearType;
  BattleEffectId btlEffId;
  u32 retBGMID;
};

struct SakasaBattleParam{
  btl::bg::bgsys_far_typeID bgFarType;
  btl::bg::bgsys_near_typeID bgNearType;
  //u8      pokemonNum;         // 相手のポケモン数
  u8      level;              // 相手のレベル
  u8      powerRand;          // 相手の個体値
};

//****************************************************************************
/**
 * @class バトル呼び出しイベント
*/
//****************************************************************************
class EventBattleCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleCall); // コピーコンストラクタ＋代入禁止

public:
  
  typedef void (*CALLBACK_FUNC)( GameSys::GameManager * p_gman );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  野性戦
   *
   *  @param  p_eventman
   *  @param  p_gameman
   *  @param  poke_set
   *  @param  call_option EVBTL_CALLOPT_NONE他 イベントシーケンス制御オプション
   *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合に指定
   *  @param  cp_party 外部から確保済みのPokePartyを指定
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallWild( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option = EVBTL_CALLOPT_NONE, const pml::PokeParty *  cp_party = NULL );
  static GameSys::GameEvent* CallWildEx( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option, u32 ret_bgm_id, const pml::PokeParty * cp_party = NULL );

  //----------------------------------------------------------------------------
  /**
   *  @brief  野性戦イベント生成
   *
   *  @param  p_eventman
   *  @param  p_gameman
   *  @param  poke_set
   *  @param  call_option EVBTL_CALLOPT_NONE他 イベントシーケンス制御オプション
   *  @param  ret_bgm_id  バトル勝利後に任意のBGMを新規再生したい場合に指定
   */
  //-----------------------------------------------------------------------------
  static EventBattleCall* CreateWild( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option, u32 ret_bgm_id = Sound::SOUND_ITEM_ID_NONE );

  //----------------------------------------------------------------------------
  /**
   *  @brief  トレーナー戦
   *  @param  p_gameman ゲームマネージャ
   *  @param  param     トレーナーバトル設定パラメータ
   *  @param  call_option EVBTL_CALLOPT_NONE他 イベントシーケンス制御オプション
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallTrainer( GameSys::GameManager* p_gameman, const TrainerBattleParam& param, u32 call_option = EVBTL_CALLOPT_NONE );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  デフォルト設定でのバトル呼び出し（通常）
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* Call( GameSys::GameEventManager* p_eventman, BATTLE_SETUP_PARAM* p_param );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  戦闘結果から勝ち負けの判定
   */
  //-----------------------------------------------------------------------------
  static bool IsLoseResult( BtlResult result );
  static bool IsGameoverResult( BtlResult result, const pml::PokeParty* pParty, bool* is_overwrite );
  static bool IsWinResult( BtlResult result );
  static bool IsLoseTrainerResult( BtlResult result );
  static u32 GetWildBattleResult( BtlResult result );
 
  // コンストラクタデストラクタ
  EventBattleCall( gfl2::heap::HeapBase* heap ) :
    GameSys::GameEvent( heap ),
    m_Seq(0),
    m_Option(0),
    m_pParam(NULL),
    m_SetupDefault(false),
    m_NotScriptEvent(false),
    m_NoFadeIn(false),
    m_NoLoseBattle(false),
    m_SkipBGMStart(false),
    m_BtlEnemyParty(NULL),
    m_beforePartyPokeNum(0),
    m_ExFlag(0),
    m_ReturnBGMID(Sound::SOUND_ITEM_ID_NONE),
    m_Attr(ATTR_ERROR),
    m_pCallBackFieldClose(NULL),
    m_cpZoneDataLoader(NULL),
    m_pEncountEffect( NULL),
    m_trainerID0( trainer::TRID_NULL),
    m_trainerID1( trainer::TRID_NULL),
    m_weather( weather::FORCE_WEATHER_NONE)
#if PM_DEBUG
    ,m_isBattleThrough( false)
    ,m_isBattleHio(false)
    ,m_isBattleWeakEnemy(false)
#endif
    {}

  virtual ~EventBattleCall(){}



  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);

  // フィールドクローズ時のコールバック
  void SetCallBackFieldClose( CALLBACK_FUNC p_func );
  // ゾーンデータローダーのセット
  void SetZoneDataLoader( const ZoneDataLoader * cp_zone_loader );

protected:

  // デフォルト設定で呼び出し
  void SetUpWildPoke( GameSys::GameManager* p_gman, Encount::PokeSet& poke_set, u32 call_option, const pml::PokeParty * cp_party = NULL );
  void SetUpTrainer( GameSys::GameManager* p_gman, const TrainerBattleParam& param, u32 call_option );
  void SetUpPlayerFormStatus( GameSys::GameManager* p_gman );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  戦闘戻り時のBGM指定(全滅時は全滅フロー用の遷移が走るため勝利時のみ有効)
   *  @note 呼び出さない、あるいは、Sound::SOUND_ITEM_ID_NONEが指定された場合(←意味はないが)なら素直にPushされた曲に戻る
   */
  //-----------------------------------------------------------------------------
  void SetReturnBGMID( u32 bgm_id );
  
  //デフォルト設定でのワーク破棄
  void RemoveDefault(void);

  // バトルパラメータを登録
  void SetUpBattleParam( BATTLE_SETUP_PARAM * p_param );

  //戦闘結果をgamedata等に反映する
  bool ReflectBattleResult( GameSys::GameManager* p_gman );

protected:

  // シーケンス
  s32 m_Seq;
  u32 m_Option; //シーケンス制御等のオプションフラグ

  // バトルパラメータ
  BATTLE_SETUP_PARAM * m_pParam;


  // 以下、デフォルト設定用
  bool m_SetupDefault;
  bool m_NotScriptEvent;
  bool m_NoFadeIn;
  bool m_NoLoseBattle;
  bool m_SkipBGMStart;  //通常トレーナー・野生(ランダム、シンボル)でのみ有効
  BATTLE_SETUP_PARAM m_DefaultParam;
  BTL_FIELD_SITUATION              m_BtlFieldSit;
  pml::PokeParty*                  m_BtlEnemyParty;
  u8                               m_SetPokemonIndex[pml::PokeParty::MAX_MEMBERS];  ///< 受け渡したポケモンのインデックス
  u8                               m_beforePartyPokeNum;  ///< 戦闘前の自分のポケパーティのメンバー数を覚えておく
  u32 m_ExFlag;  ///< セットアップ時の拡張設定フラグ
  u32 m_ReturnBGMID;  ///< バトル終了後にどの曲に戻すか？(初期化値のSound::SOUND_ITEM_ID_NONEなら素直にPushされた曲に戻る)
  Attr m_Attr; ///< 指定アトリビュート。ない場合はプレイヤーの足元をとる

  CALLBACK_FUNC m_pCallBackFieldClose;
  // ゾーンデータローダー
  const ZoneDataLoader * m_cpZoneDataLoader;

  Location m_playerLocation;

  Encount::EncountEffect*         m_pEncountEffect;
  trainer::TrainerID              m_trainerID0;
  trainer::TrainerID              m_trainerID1;

  weather::WeatherKind            m_weather;

#if PM_DEBUG
  bool      m_isBattleThrough;
  bool      m_isBattleHio;
  bool      m_isBattleRareEffect;
  bool      m_isBattleWeakEnemy;
#endif // PM_DEBUG

};  // class EventBattleCall


//****************************************************************************
/**
 *          クラス宣言
*/
//****************************************************************************
class EventBattleInstBattleCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstBattleCall); // コピーコンストラクタ＋代入禁止

public:
  // コンストラクタデストラクタ
  EventBattleInstBattleCall( gfl2::heap::HeapBase* heap ) : 
    GameSys::GameEvent( heap ),
    m_pEncountEffect( NULL),
    m_Seq(0),
    m_pParam(NULL),
    m_pBattleVideoRecordingParam(NULL),
    m_pBattleRecorderSaveData(NULL),
    isEndBattleBGM(false)
  {}

  virtual ~EventBattleInstBattleCall(){}

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);

  // バトルパラメータを登録
  void SetUpBattleParam( BATTLE_SETUP_PARAM * p_param );


private:

  Encount::EncountEffect*         m_pEncountEffect;

  // シーケンス
  s32 m_Seq;
  // バトルパラメータ
  BATTLE_SETUP_PARAM * m_pParam;

  Location m_playerLocation;

  // ロイヤルのみ使用
  NetEvent::BattleVideoRecording::EVENT_APP_PARAM*    m_pBattleVideoRecordingParam;   //! バトルビデオ録画アプリのパラメータ 
  ExtSavedata::BattleRecorderSaveData*                m_pBattleRecorderSaveData;      //! バトルビデオ拡張セーブクラス

  bool isEndBattleBGM; //! バトルBGMを戻し済みか

};  // class EventBattleInstBattleCall  

//****************************************************************************
/**
*          クラス宣言
*/
//****************************************************************************
class EventBattleFesBattleCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleFesBattleCall); // コピーコンストラクタ＋代入禁止

public:
  // コンストラクタデストラクタ
  EventBattleFesBattleCall(gfl2::heap::HeapBase* heap) :
    GameSys::GameEvent(heap),
    m_pEncountEffect(NULL),
    m_Seq(0),
    m_pParam(NULL),
    m_pBattleVideoRecordingParam(NULL),
    m_pBattleRecorderSaveData(NULL),
    isEndBattleBGM(false)
  {}

  virtual ~EventBattleFesBattleCall(){}

  //--------------------------------------------------------------
  /**
  * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
  * @param   p_gman       ゲームマネージャークラス
  * @retval  true  起動してよい
  * @retval  false 起動しない
  */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
  * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
  * @param   p_gman       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   p_gman       ゲームマネージャークラス
  * @return  GMEVENT_RESULT  イベント制御関数の戻り値
  */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   p_gman       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);

  // バトルパラメータを登録
  void SetUpBattleParam(BATTLE_SETUP_PARAM * p_param);


private:

  Encount::EncountEffect*         m_pEncountEffect;

  // シーケンス
  s32 m_Seq;
  // バトルパラメータ
  BATTLE_SETUP_PARAM * m_pParam;

  Location m_playerLocation;

  // 
  NetEvent::BattleVideoRecording::EVENT_APP_PARAM*    m_pBattleVideoRecordingParam;   //! バトルビデオ録画アプリのパラメータ 
  ExtSavedata::BattleRecorderSaveData*                m_pBattleRecorderSaveData;      //! バトルビデオ拡張セーブクラス

  bool isEndBattleBGM; //! バトルBGMを戻し済みか

};  // class EventBattleFesBattleCall  


GFL_NAMESPACE_END( Field )

#endif  // EVENT_BATTLE_CALL_H_INCLUDED


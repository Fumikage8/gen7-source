//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventPokemonRide.h
 *	@brief  イベントポケモンライド
 *	@author	tomoya takahashi
 *	@date		2015.06.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_POKEMON_RIDE_H__ )
#define __EVENT_POKEMON_RIDE_H__
#pragma once

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectRideLight.h"

#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"


#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

GFL_NAMESPACE_BEGIN(Field);

GFL_NAMESPACE_BEGIN(MoveModel)
  class FieldMoveModelManager;
  class FieldMoveModelPlayer;
GFL_NAMESPACE_END(MoveModel)

#define EFFECT_LIGHT 0
#define EVENT_RIDE_MOTION 0

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class PokemonRideOnOffDirection;

/**
MapJumpで使いたいため、FieldStatic/include/Event/FieldEventPokemonRideTool.hにいどう
// UI側とモデル側とすり合わせて全ライド分のIDを用意
enum RIDE_POKEMON_ID
{
  RIDE_KENTAROSU,
  RIDE_SAMEHADA,
  RIDE_MUURANDO,
  RIDE_LAPLACE,
  RIDE_KAIRIKY,
  RIDE_RIZAADON,
  RIDE_ROBA2,

  RIDE_POKEMON_NUM,

  RIDE_NO_POKEMON = 0xffffffff,
};


enum RIDE_POKEMON_MODEL_ID
{
  RIDE_MODEL_KENTAROSU = CHARA_MODEL_ID_PM0128_00,
  RIDE_MODEL_SAMEHADA = CHARA_MODEL_ID_PM0319_00,
  RIDE_MODEL_MUURANDO = CHARA_MODEL_ID_PM0508_00,
  RIDE_MODEL_LAPLACE = CHARA_MODEL_ID_PM0131_00,
  RIDE_MODEL_KAIRIKY = CHARA_MODEL_ID_PM0068_00,
  RIDE_MODEL_ROBA2 = CHARA_MODEL_ID_PM0840_00
};
**/


enum RIDE_CHECK_RESULT
{
  RCR_OK,                       ///< ライドに乗れる/降りれる
  RCR_ERROR_ZONE,               ///< ゾーンの設定がライド無効
  RCR_ERROR_SPACE_SMALE,        ///< 障害物があり、乗れない/降りれない
  RCR_ERROR_GROUND_ATTRIBUTE,   ///< 地形の状況により乗れない/降りれない
  RCR_ERROR_NAMINORI_JUMP,      ///< 波乗り/降りジャンプの不正
  RCR_ERROR_CAN_NOT_NOW,        ///< 今はつかえない
  RCR_ERROR_OTHERS,             ///< その他の理由で乗れない/降りれない

  RCR_MAX,  ///< 無効値
};


/**
 *  @class のる
 */
class EventPokemonRideOn : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokemonRideOn); //コピーコンストラクタ＋代入禁止

public:

  enum EVENT_STATUS   // イベントフロー状態
  {
    EVENT_RIDE_START,
    EVENT_RIDE_WAIT,
    EVENT_RIDE_END,
  };

  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventPokemonRideOn(gfl2::heap::HeapBase* p_heap) : GameSys::GameEvent(p_heap),
    m_pokemonID( RIDE_NO_POKEMON ),
    m_isUseEffect(true)
  {
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventPokemonRideOn()
  {
  }


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
    * @brief   乗るポケモンを設定
    * @param   pokemonID    乗るポケモン
    */
  //--------------------------------------------------------------
  void SetPokemonRideOnID( GameSys::GameManager* pGameMng, RIDE_POKEMON_ID pokemonID );

  //--------------------------------------------------------------
  /**
    * @brief   ポケモンに乗れるかどうかチェック
    * @param   pokemonID    チェックするポケモン
    */
  //--------------------------------------------------------------
  static RIDE_CHECK_RESULT IsPokemonRideOnEnable( GameSys::GameManager* pGameMng, RIDE_POKEMON_ID pokemonID, b32 isNaminoriAngleCheck = true );

  //--------------------------------------------------------------
  /**
    * @brief   ポケモンに乗れるかチェックの結果からNGメッセージを取得する。
    */
  //--------------------------------------------------------------
  static u32 GetRcrMessageId( RIDE_CHECK_RESULT result );


  //--------------------------------------------------------------
  /**
    * @brief   ポケモンに乗れるかチェックの結果からNGメッセージを流すスクリプトIDを取得
    */
  //--------------------------------------------------------------
  static u32 GetRcrScriptId( RIDE_CHECK_RESULT result );


#if PM_DEBUG
  //--------------------------------------------------------------
  /**
    * @brief   デバッグ用 乗るポケモンを自動で設定
    * @return   乗るポケモンのID
    */
  //--------------------------------------------------------------
  static Field::RIDE_POKEMON_ID DebugAutoSelectRidePokemon( GameSys::GameManager* pGameMng );


#endif //PM_DEBUG

  static bool CheckRideFlag( const GameSys::GameManager* cpGameMng, RIDE_POKEMON_ID pokeID );


  /**
   *  @brief ライド用のドレスアップパラメータを設定
   // @fix NMCat[181] 水上ライド中にバッグ画面等を開閉すると、フィールド上のライド衣装が陸上用のものになる
   */
  static void GetPokemonRideDressUpParam( GameSys::GameManager* pGameMng, poke_3d::model::DressUpParam* p_dressupParam, b32 is_fieldmap_none = false );

  /**
  *  @brief 宇宙服用のドレスアップパラメータを設定
  */
  static void GetSpaceSuitDressUpParam( GameSys::GameManager* pGameMng, poke_3d::model::DressUpParam* p_dressupParam );



  /**
   *  @brief 波乗りジャンプ先に壁にぶつからずに進むことが出来るかチェックする。
   */
  static b32 IsEnableNaminoriWallJumpGroundRide( Field::MoveModel::FieldMoveModelPlayer* pPlayer, const gfl2::math::Vector3& jumpEndPos );

private:

  // 地面のアトリビュートの取得
  static u32 GetPositionAttr( GameSys::GameManager* pGameMng, const gfl2::math::Vector3& pos );

protected:

  b32 m_isUseEffect;

private:

  PokemonRideOnOffDirection * m_pDirection;
  RIDE_POKEMON_ID m_pokemonID;
};


/**
 *  @class おりる
 */
class EventPokemonRideOff : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokemonRideOff); //コピーコンストラクタ＋代入禁止

public:
  enum EVENT_STATUS   // イベントフロー状態
  {
    EVENT_RIDE_START,
    EVENT_RIDE_WAIT,
    EVENT_RIDE_END,
  };


  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventPokemonRideOff(gfl2::heap::HeapBase* p_heap) : GameSys::GameEvent(p_heap),
    m_isUseEffect(true), m_isForce(false)
  {
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventPokemonRideOff()
  {
  }


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
    * @brief   ポケモンから降りられるかどうかチェック
    */
  //--------------------------------------------------------------
  static RIDE_CHECK_RESULT IsPokemonRideOffEnable( GameSys::GameManager* pGameMng );

  //--------------------------------------------------------------
  /**
    * @brief   ポケモンに降りれるかチェックの結果からNGメッセージを取得する。
    */
  //--------------------------------------------------------------
  static u32 GetRcrMessageId( RIDE_CHECK_RESULT result );

  //--------------------------------------------------------------
  /**
    * @brief   ポケモンに降りれるかチェックの結果からスクリプトIdを取得する。
    */
  //--------------------------------------------------------------
  static u32 GetRcrScriptId( RIDE_CHECK_RESULT result );


protected:

  b32 m_isUseEffect;
  b32 m_isForce;

private:

  PokemonRideOnOffDirection * m_pDirection;
};

/**
 *  @class のる(演出なし)
 */
class EventPokemonRideOnWithoutEffect : public EventPokemonRideOn
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokemonRideOnWithoutEffect); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventPokemonRideOnWithoutEffect(gfl2::heap::HeapBase* p_heap) : EventPokemonRideOn(p_heap)
  {
    m_isUseEffect = false;
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventPokemonRideOnWithoutEffect()
  {
  }
};

/**
 *  @class おりる(演出なし)
 */
class EventPokemonRideOffWithoutEffect : public EventPokemonRideOff
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokemonRideOffWithoutEffect); //コピーコンストラクタ＋代入禁止

  friend FieldScript::FuncSetCommon;  // Forceモードの使用は、使える場所を限定する。

public:

  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventPokemonRideOffWithoutEffect(gfl2::heap::HeapBase* p_heap) : EventPokemonRideOff(p_heap)
  {
    m_isUseEffect = false;
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventPokemonRideOffWithoutEffect()
  {
  }


private:
  // 強制下ろしモードにする。
  // その場で確実にライドから降ります。
  void SetForce( b32 flag ){ m_isForce = flag; }
};


#if 0
/**
 *  @class 吸着してから乗る
 */
class EventPokemonRideOnBeforeMagnetPos : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventPokemonRideOnBeforeMagnetPos); //コピーコンストラクタ＋代入禁止

public:
  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventPokemonRideOnBeforeMagnetPos(gfl2::heap::HeapBase* p_heap) : GameSys::GameEvent(p_heap),
    m_magnetPos(0,0,0),
    m_pokemonID( RIDE_NO_POKEMON )
  {
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventPokemonRideOnBeforeMagnetPos()
  {
  }


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
    * @brief   乗るポケモンを設定
    * @param   pokemonID    乗るポケモン
    */
  //--------------------------------------------------------------
  void SetPokemonRideOnID( GameSys::GameManager* pGameMng, RIDE_POKEMON_ID pokemonID, const gfl2::math::Vector3& magnet_pos );


private:
  gfl2::math::Vector3 m_magnetPos;
  RIDE_POKEMON_ID m_pokemonID;
};
#endif

GFL_NAMESPACE_END(Field);


#endif	// __EVENT_POKEMON_RIDE_H__


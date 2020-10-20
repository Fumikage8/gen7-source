//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventPokemonRideTool.h
 *	@brief  イベントポケモンライド　常駐のツール
 *	@author	tomoya takahashi
 *	@date		2015.12.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_POKEMON_RIDE_TOOL_H__ )
#define __EVENT_POKEMON_RIDE_TOOL_H__
#pragma once


// gfl2
#include <math/include/gfl2_Easing.h>

#include "GameSys/include/GameManager.h"

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>
//PlayerForm
#include "niji_reference_files/script/FieldPawnTypes.h"

GFL_NAMESPACE_BEGIN(Field);

GFL_NAMESPACE_BEGIN(MoveModel);
class FieldMoveModelPlayer;
struct FieldMoveModelHeaderWork;
struct FieldMoveModelHeaderResource;
GFL_NAMESPACE_END(MoveModel);

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

class EventPokemonRideTool
{
public:

  //--------------------------------------------------------------
  /**
    * @brief   乗っているポケモンを取得
    * @return   pokemonID    乗っているポケモン
    */
  //--------------------------------------------------------------
  static RIDE_POKEMON_ID GetPokemonRideOnID( GameSys::GameManager* pGameMng );

  //--------------------------------------------------------------
  /**
    * @brief   乗っているポケモンを取得
    * @return   pokemonID    乗っているポケモン
    */
  //--------------------------------------------------------------
  static RIDE_POKEMON_ID GetPokemonRideOnID( Field::MoveModel::FieldMoveModelPlayer* pPlayer, b32 is_fieldmap_none = false );

  //--------------------------------------------------------------
  /**
    * @brief   キャラクターIDと対になる、RIDE_POKEMON_IDを取得
    * @return   id    キャラクターID
    */
  //--------------------------------------------------------------
  static RIDE_POKEMON_ID GetCharactorId_to_RIDE_POKEMON_ID( u32 id );
  
  //--------------------------------------------------------------
  /**
    * @brief   RIDE_POKEMON_IDと対になる、キャラクターIDを取得
    * @return   pokemonID    乗っているポケモン
    */
  //--------------------------------------------------------------
  static u32 GetRIDE_POKEMON_ID_to_CharactorId( RIDE_POKEMON_ID pokemonID );


  //--------------------------------------------------------------
  /**
    * @brief   RIDE_POKEMON_IDと対になる、プレイヤーフォルムを取得
    * @return   pokemonID    乗っているポケモン
    */
  //--------------------------------------------------------------
  static PlayerForm GetRIDE_POKEMON_ID_to_PlayerForm( RIDE_POKEMON_ID pokemonID );

  //--------------------------------------------------------------
  /**
    * @brief   ライドポケモンの生成ヘッダーワーク　ヘッダーリソース生成
    */
  //--------------------------------------------------------------
  static void CreatePokemonRideWorld( RIDE_POKEMON_ID pokemonID, Field::MoveModel::FieldMoveModelHeaderWork* p_headerwork, Field::MoveModel::FieldMoveModelHeaderResource* p_headerresource );


  //--------------------------------------------------------------
  /**
    * @brief   波乗り状態チェック
    */
  //--------------------------------------------------------------
  static b32 IsNaminori( RIDE_POKEMON_ID pokemonID );

  //--------------------------------------------------------------
  /**
    * @brief   地上系ライドかチェック
    */
  //--------------------------------------------------------------
  static b32 IsGroundRide( RIDE_POKEMON_ID pokemonID );


  // ライドイベント中のカメラ設定

  // 陸→海
  struct CAMERA_SETTING_RAND_TO_SEA
  {
    // カメラ移動1:着地点へのカメラ移動
    u32 startFrame;                               // 開始待ち時間
    u32 moveFrame;                                // 移動時間 
    gfl2::math::Easing::EaseFunc  moveEaseFunc;   // 進行に使用するイージング
    
    // カメラ移動2:海のカメラ設定への補間
    u32 toFieldStartFrame;                              // 開始待ち時間
    u32 toFieldIpoFrame;                                // フィールドカメラへの補間時間
    gfl2::math::Easing::EaseFunc toFieldIpoEaseFunc;    // フィールドカメラへの補間に使用するイージング
  };
  
  // 海->陸
  struct CAMERA_SETTING_SEA_TO_RAND
  {
    u32 startFrame;                             // 開始待ち時間
    u32 moveFrame;                              // 移動時間 
    gfl2::math::Easing::EaseFunc moveEaseFunc;  // 移動に使用するイージング
  };

  static CAMERA_SETTING_SEA_TO_RAND  s_CameraMoveSettingSeaToRand;    // 海->陸へのカメラ移動設定
  static CAMERA_SETTING_RAND_TO_SEA  s_CameraMoveSettingRandToSea;    // 陸->海へのカメラ移動設定
};

GFL_NAMESPACE_END(Field);


#endif	// __EVENT_POKEMON_RIDE_TOOL_H__


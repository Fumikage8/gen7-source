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

#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"

#include "GameSys/include/GameData.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

GFL_NAMESPACE_BEGIN(Field);

// 海->陸へのカメラ移動設定
EventPokemonRideTool::CAMERA_SETTING_SEA_TO_RAND EventPokemonRideTool::s_CameraMoveSettingSeaToRand =
{
  // 地面の固さを印象つけるために、すばやく補間する
  0,
  25,
  gfl2::math::Easing::OUT_CUBIC
};

// 陸->海へのカメラ移動設定
EventPokemonRideTool::CAMERA_SETTING_RAND_TO_SEA EventPokemonRideTool::s_CameraMoveSettingRandToSea = 
{
  // カメラ移動1:着地点へのカメラ移動
  // 予め想定している動きなので、自機のジャンプが実行されるよりも先に動かす。
  0,
  15,
  gfl2::math::Easing::INOUT_QUAD,

  // カメラ移動2:海のカメラ設定への補間
  // エフェクト開始～終了にあわせて補間する
  16,
  35,
  gfl2::math::Easing::INOUT_SINE,
};


//--------------------------------------------------------------
/**
  * @brief   乗っているポケモンを取得
  * @return   pokemonID    乗っているポケモン
  */
//--------------------------------------------------------------
RIDE_POKEMON_ID EventPokemonRideTool::GetPokemonRideOnID( Field::MoveModel::FieldMoveModelPlayer*  pPlayer, b32 is_fieldmap_none )
{
  u32 characterId = CHARA_MODEL_ID_PC0001_00;

  
  if( !is_fieldmap_none )
  {
    // 動作モデルマネージャーを取得
    MoveModel::FieldMoveModel* pModel = pPlayer->GetOnMoveModel();
    if( !pModel )
    {
      return RIDE_NO_POKEMON;
    }

    characterId = pModel->GetCharacterId();
  }
  else
  {
    // Fieldmapが無いときの取得
    if( pPlayer->IsRideMoveModelPrevClearResource() )
    {
      characterId = pPlayer->GetRideMoveModelHeaderResourcePrevClearResource().characterId;
    }
  }

  return GetCharactorId_to_RIDE_POKEMON_ID( characterId );
}
//--------------------------------------------------------------
/**
  * @brief   乗っているポケモンを取得
  * @return   pokemonID    乗っているポケモン
  */
//--------------------------------------------------------------
RIDE_POKEMON_ID EventPokemonRideTool::GetPokemonRideOnID( GameSys::GameManager* pGameMng )
{
  Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager = pGameMng->GetGameData()->GetFieldCharaModelManager();
  Field::MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pFieldCharaModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  b32 is_fieldmap_none  = (pGameMng->GetFieldmap() == NULL);

  return GetPokemonRideOnID(pPlayer, is_fieldmap_none);
}

//--------------------------------------------------------------
/**
* @brief   キャラクターIDと対になる、RIDE_POKEMON_IDを取得
* @return   id    キャラクターID
*/
//--------------------------------------------------------------
RIDE_POKEMON_ID EventPokemonRideTool::GetCharactorId_to_RIDE_POKEMON_ID( u32 id )
{
  switch( id )
  {
  case RIDE_MODEL_KENTAROSU :
    return RIDE_KENTAROSU;
  case RIDE_MODEL_SAMEHADA :
    return RIDE_SAMEHADA;
  case RIDE_MODEL_MUURANDO :
    return RIDE_MUURANDO;
  case RIDE_MODEL_LAPLACE :
    return RIDE_LAPLACE;
  case RIDE_MODEL_KAIRIKY :
    return RIDE_KAIRIKY;
  case RIDE_MODEL_ROBA2:
    return RIDE_ROBA2;
  case CHARA_MODEL_ID_PC0001_00:
    return RIDE_NO_POKEMON;
  default :
    GFL_ASSERT(0);
    return RIDE_NO_POKEMON;
  }
}

//--------------------------------------------------------------
/**
  * @brief   RIDE_POKEMON_IDと対になる、キャラクターIDを取得
  * @return   pokemonID    乗っているポケモン
  */
//--------------------------------------------------------------
u32 EventPokemonRideTool::GetRIDE_POKEMON_ID_to_CharactorId( RIDE_POKEMON_ID pokemonID )
{
  u32 chractor_id = 0;
  switch(pokemonID)
  {
  case RIDE_KENTAROSU : 
    chractor_id = RIDE_MODEL_KENTAROSU;
    break;

  case RIDE_SAMEHADA : 
    chractor_id = RIDE_MODEL_SAMEHADA;
    break;

  case RIDE_MUURANDO : 
    chractor_id = RIDE_MODEL_MUURANDO;
    break;

  case RIDE_LAPLACE :
    chractor_id = RIDE_MODEL_LAPLACE;
    break;

  case RIDE_KAIRIKY :
    chractor_id = RIDE_MODEL_KAIRIKY;
    break;
  case RIDE_ROBA2:
    chractor_id = RIDE_MODEL_ROBA2;
    break;
  case RIDE_NO_POKEMON : 
    GFL_ASSERT( 0 );
    chractor_id = RIDE_MODEL_KENTAROSU;
    break;

  default :
    GFL_ASSERT( 0 );
    chractor_id = RIDE_MODEL_KENTAROSU;
    break;
  };

  return chractor_id;
}

//--------------------------------------------------------------
/**
  * @brief   RIDE_POKEMON_IDと対になる、プレイヤーフォルムを取得
  * @return   pokemonID    乗っているポケモン
  */
//--------------------------------------------------------------
PlayerForm EventPokemonRideTool::GetRIDE_POKEMON_ID_to_PlayerForm( RIDE_POKEMON_ID pokemonID )
{
  PlayerForm form = PLAYER_FORM_NORMAL;
  switch(pokemonID)
  {
  case RIDE_KENTAROSU : 
    form = PLAYER_FORM_RIDE_KENTAROSU;
    break;

  case RIDE_SAMEHADA : 
    form = PLAYER_FORM_RIDE_SAMEHADAA;
    break;

  case RIDE_MUURANDO : 
    form = PLAYER_FORM_RIDE_MUURANDO;
    break;

  case RIDE_LAPLACE :
    form = PLAYER_FORM_RIDE_RAPURASU;
    break;

  case RIDE_KAIRIKY :
    form = PLAYER_FORM_RIDE_KAIRIKII;
    break;
  case RIDE_ROBA2:
    form = PLAYER_FORM_RIDE_ROBA2;
    break;
  case RIDE_NO_POKEMON : 
    form = PLAYER_FORM_NORMAL;
    break;

  default :
    GFL_ASSERT( 0 );
    form = PLAYER_FORM_NORMAL;
    break;
  };

  return form;
}

//--------------------------------------------------------------
/**
  * @brief   ライドポケモンの生成ヘッダーワーク　ヘッダーリソース生成
  */
//--------------------------------------------------------------
void EventPokemonRideTool::CreatePokemonRideWorld( RIDE_POKEMON_ID pokemonID, Field::MoveModel::FieldMoveModelHeaderWork* p_headerwork, Field::MoveModel::FieldMoveModelHeaderResource* p_headerresource )
{
  p_headerwork->Init();
  p_headerresource->Init();

  p_headerwork->moveCodeId = Field::MoveModel::FIELD_MOVE_CODE_NONE;
  p_headerresource->pos    = gfl2::math::Vector3(0,0,0);
  p_headerresource->qua    = gfl2::math::Quaternion(0,0,0,1);
  p_headerresource->characterId = GetRIDE_POKEMON_ID_to_CharactorId(pokemonID);
}

//--------------------------------------------------------------
/**
  * @brief   波乗り状態チェック
  */
//--------------------------------------------------------------
b32 EventPokemonRideTool::IsNaminori( RIDE_POKEMON_ID pokemonID )
{
  if( (pokemonID == RIDE_SAMEHADA) || (pokemonID == RIDE_LAPLACE) )
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------
/**
  * @brief   地上系ライドかチェック
  */
//--------------------------------------------------------------
b32 EventPokemonRideTool::IsGroundRide( RIDE_POKEMON_ID pokemonID )
{
  switch( pokemonID)
  {
  case RIDE_KENTAROSU:
  case RIDE_MUURANDO:
  case RIDE_KAIRIKY:
  case RIDE_ROBA2:
    return true;
  }
  return false;
}



GFL_NAMESPACE_END(Field);

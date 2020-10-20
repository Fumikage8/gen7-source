//======================================================================
/**
 * @file FieldLocation.cpp
 * @date 2015/08/27 21:27:29
 * @author takahashi_tomoya
 * @brief ロケーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "Field/FieldStatic/include/FieldLocation.h"

#include <niji_conv_header/field/world/world_id.h>
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"

GFL_NAMESPACE_BEGIN(Field)

Location::Location() : 
  worldId(WORLD_ERROR),
  zoneId(ZONEID_ERROR),
  position(0,0,0),
  quaternion(0,0,0,1),
  formNo(RIDE_NO_POKEMON)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   LOCATIONの初期化
 */
//-----------------------------------------------------------------------------
void Location::Initialize( void )
{
  this->worldId = WORLD_ERROR;
  this->zoneId = ZONEID_ERROR;
  this->position.Set(0,0,0);
  this->quaternion.Set(0,0,0,1);
  this->formNo = RIDE_NO_POKEMON;
}

//----------------------------------------------------------------------------
/**
 *	@brief  空ロケーションかチェック
 */
 //-----------------------------------------------------------------------------
bool Location::IsClear() const 
{
  if( 
    (this->worldId == WORLD_ERROR) ||
    (this->zoneId == ZONEID_ERROR) )
  {
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
/**
 *	@brief  locationにデータを設定する。
 *
 *	@param	worldID           ワールドID
 *	@param	zoneID            ゾーンID
 *	@param  pos               ポジション
 *  @param  quaternion        クオータニオン
 */
 //-----------------------------------------------------------------------------
void Location::Set( WorldID worldID, ZoneID zoneID, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& quaternion )
{
  this->worldId  = worldID;
  this->zoneId   = zoneID;
  this->position = pos;
  this->quaternion = quaternion;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ロケーションのセーブデータを設定
 */
//-----------------------------------------------------------------------------
void Location::Set( const Location::SaveData& data )
{
  this->worldId = data.world_id;
  this->zoneId = data.zone_id;
  this->position.Set( data.position[0], data.position[1], data.position[2] );
  this->quaternion = gfl2::math::Quaternion( data.quaternion[0], data.quaternion[1], data.quaternion[2], data.quaternion[3] );
  this->formNo  = data.form_no;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ロケーションのセーブデータを設定
 */
//-----------------------------------------------------------------------------
void Location::Get( Location::SaveData* p_data ) const
{
  p_data->world_id = this->worldId;
  p_data->zone_id   = this->zoneId;
  p_data->position[0] = this->position[0];
  p_data->position[1] = this->position[1];
  p_data->position[2] = this->position[2];
  p_data->quaternion[0] = this->quaternion[0];
  p_data->quaternion[1] = this->quaternion[1];
  p_data->quaternion[2] = this->quaternion[2];
  p_data->quaternion[3] = this->quaternion[3];
  p_data->form_no     = this->formNo; 
}


GFL_NAMESPACE_END(Field)

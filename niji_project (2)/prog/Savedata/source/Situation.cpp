//======================================================================
/**
 * @file Situation.cpp
 * @date 2015/08/28 10:25:13
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "Savedata/include/Situation.h"


GFL_NAMESPACE_BEGIN(Savedata)
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------------------
Situation::Situation(void)
{
  this->Initialize();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
Situation::~Situation(void)
{
}


//----------------------------------------------------------------------------
/**
 *	@brief  状況データの初期化
 *
 *	@param	  p_data    データポインタ
 */
//-----------------------------------------------------------------------------
void Situation::Initialize( void )
{
  Data* p_data = &m_Data;
  
  gfl2::std::MemClear( p_data, sizeof(Data) );

  p_data->lastZoneID = Field::ZONEID_ERROR;

  // FieldLocationの初期化
  Field::Location location;
  location.Get( &p_data->start_loc );
  location.Get( &p_data->escape_loc );
  location.Get( &p_data->special_loc );
}

//-----------------------------------------------------------------------------
/**
 * @brief    JF行き来　データコピー用関数
 */
//-----------------------------------------------------------------------------
void Situation::CopyData( const Situation& data, b32 joinFestaReturn )
{
  if( joinFestaReturn )
  {
    // カウント系は引き継ぐ
    m_Data.egg_step_count      = data.m_Data.egg_step_count;     ///<タマゴ孵化カウンタ
    m_Data.friendly_step_count = data.m_Data.friendly_step_count;  ///<なつき度上昇用カウンタ
    m_Data.kawaigari_step_count= data.m_Data.kawaigari_step_count;  ///<かわいがり度上昇用カウンタ
  }
  else
  {
    gfl2::std::MemCopy( &(data.m_Data), &m_Data, GetDataSize() );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  セーブ復帰位置の取得
 */
//-----------------------------------------------------------------------------
Field::Location::SaveData* Situation::GetStartLocation( void )
{
  return &m_Data.start_loc;
}

//----------------------------------------------------------------------------
/**
 *	@brief  脱出先のセット
 */
//-----------------------------------------------------------------------------
void Situation::SetStartLocation( Field::Location* location )
{
  location->Get(&m_Data.start_loc);
}

//----------------------------------------------------------------------------
/**
 *	@brief  特殊復帰位置の取得
 */
//-----------------------------------------------------------------------------
Field::Location::SaveData* Situation::GetSpecialLocation( void )
{
  return &m_Data.special_loc;
}

//----------------------------------------------------------------------------
/**
 *	@brief  特殊復帰位置のセット
 */
//-----------------------------------------------------------------------------
void Situation::SetSpecialLocation( Field::Location* location )
{
  location->Get(&m_Data.special_loc);
}

//----------------------------------------------------------------------------
/**
 *	@brief  脱出先の取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return 脱出先のロケーションポインタ
 */
//-----------------------------------------------------------------------------
Field::Location::SaveData* Situation::GetEscapeLocation( void )
{
  return &m_Data.escape_loc;
}

//----------------------------------------------------------------------------
/**
 *	@brief  脱出先のセット
 */
//-----------------------------------------------------------------------------
void Situation::SetEscapeLocation( Field::Location* location )
{
  location->Get(&m_Data.escape_loc);
}


//----------------------------------------------------------------------------
/**
 *	@brief  ワープの取得
 *
 *	@param	  p_data    データポインタ
 *	@param    worpID    ワープID
 */
//-----------------------------------------------------------------------------
void Situation::SetWarpID( u16 worpID )
{
  m_Data.warp_id = worpID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワープの取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return ワープID
 */
//-----------------------------------------------------------------------------
u16 Situation::GetWarpID( void ) const
{
  return m_Data.warp_id;
}


#if USE_BIIDORO
//----------------------------------------------------------------------------
/**
 *	@brief  ビードロタイプの設定
 *
 *	@param  type  ビードロタイプ
 */
//-----------------------------------------------------------------------------
void Situation::SetBiidoroType( BIIDORO_TYPE type )
{
  m_Data.biidoro_type = type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ビードロタイプの取得
 *
 *	@return type  ビードロタイプ
 */
//-----------------------------------------------------------------------------
u8 Situation::GetBiidoroType( void ) const
{
  return m_Data.biidoro_type;
}

#endif  // USE_BIIDORO


//----------------------------------------------------------------------------
/**
 *	@brief  天気の設定
 *
 *	@param	  p_data      データポインタ
 *	@param    weather     天気
 */
//-----------------------------------------------------------------------------
void Situation::SetWeatherID( u32 weather )
{
  m_Data.weather_id  = weather;
}

//----------------------------------------------------------------------------
/**
 *	@brief  天気の取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return 天気
 */
//-----------------------------------------------------------------------------
u32 Situation::GetWeatherID( void ) const
{
  return m_Data.weather_id;
}

//---------------------------------------------------------------------------- 
/**
 *	@brief  一つ前にいたZoneIDの設定
 *
 *	@param	  p_data      データポインタ
 *	@param    lastZoneID  一つ前のZoneID
 */
//-----------------------------------------------------------------------------
void Situation::SetLastZoneID( Field::ZoneID lastZoneID )
{
  m_Data.lastZoneID = lastZoneID;
}

//----------------------------------------------------------------------------
/**
 *	@brief  一つ前にいたZoneID取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return 一つ前にいたZoneID
 */
//-----------------------------------------------------------------------------
Field::ZoneID Situation::GetLastZoneID( void ) const
{
  return  static_cast<Field::ZoneID>(m_Data.lastZoneID);
}


//----------------------------------------------------------------------------
/**
 *	@brief  タマゴ孵化カウンタの設定
 *
 *	@param	  p_data      データポインタ
 *	@param    egg_count   タマゴ孵化カウント
 */
//-----------------------------------------------------------------------------
void Situation::SetEggStepCount( u32 egg_count )
{
  m_Data.egg_step_count = egg_count;
}

//----------------------------------------------------------------------------
/**
 *	@brief  タマゴ孵化カウンタの取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return タマゴ孵化カウンタ
 */
//-----------------------------------------------------------------------------
u32 Situation::GetEggStepCount( void ) const
{
  return m_Data.egg_step_count;
}


//----------------------------------------------------------------------------
/**
 *	@brief  なつき度上昇用カウンタの設定
 *
 *	@param	  p_data      データポインタ
 *	@param    step_count  なつき度上昇用カウンタ
 */
//-----------------------------------------------------------------------------
void Situation::SetFriendlyStepCount( u16 step_count )
{
  m_Data.friendly_step_count = step_count;
}

//----------------------------------------------------------------------------
/**
 *	@brief  なつき度上昇用カウンタの取得
 *
 *	@param	  p_data    データポインタ
 *
 *	@return なつき度上昇用カウンタ
 */
//-----------------------------------------------------------------------------
u16 Situation::GetFriendlyStepCount( void ) const
{
  return m_Data.friendly_step_count;
}

//----------------------------------------------------------------------------
/**
 *	@brief  かわいがりパラメータ上昇用ｶｳﾝﾀの設定
 */
//-----------------------------------------------------------------------------
void Situation::SetKawaigariStepCount( u16 step_count )
{
  m_Data.kawaigari_step_count = step_count;
}


//----------------------------------------------------------------------------
/**
 *	@brief  かわいがりパラメータ上昇用ｶｳﾝﾀの取得
 */
//-----------------------------------------------------------------------------
u16 Situation::GetKawaigariStepCount( void ) const
{
  return m_Data.kawaigari_step_count;
}


#if 0
//----------------------------------------------------------------------------
/**
 *	@brief  自機情報の設定
 *
 *	@param	  p_data    データポインタ
 *	@param    cp_player 自機情報
 */
//-----------------------------------------------------------------------------
void Situation::SetPlayerWork( const Field::PlayerWork* cp_player )
{
  m_Data.player = *cp_player;
}


//----------------------------------------------------------------------------
/**
 *	@brief  自機情報の取得
 *
 *	@param	  cp_data    データポインタ
 */
//-----------------------------------------------------------------------------
const Field::PlayerWork& Situation::GetPlayerWorkConst( void ) const
{
  return m_Data.player;
}
Field::PlayerWork* Situation::GetPlayerWork( void ) 
{
  return &m_Data.player;
}
#endif

//-----------------------------------------------------------------------------
/**
 * @brief   コンティニュー時にマップ遷移するかどうか
 *
 * @retval  true    マップ遷移を伴う（ゲームクリア後）
 * @retval  false   マップ遷移なし（通常時）
 */
//-----------------------------------------------------------------------------
bool Situation::HasWarpContinueRequest( void ) const
{
  return m_Data.is_warp_continue;
}

//-----------------------------------------------------------------------------
/**
 * @brief   コンティニュー時マップ遷移リクエストの設定
 *
 * @param[in] flag  true=マップ遷移リクエストする　false=しない
 */
//-----------------------------------------------------------------------------
void Situation::SetWarpContinueRequest( bool flag )
{
  m_Data.is_warp_continue = flag;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief イベント用スカイボックス時間有効フラグ
 *
 * @param flag true=有効, false=無効
 */
/* ------------------------------------------------------------------------- */
void Situation::SetSkyboxEventTimeFlag( bool flag )
{
  m_Data.event_skybox_time_flag = flag;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief イベント用スカイボックス時間有効フラグを取得
 *
 * @return 
 */
/* ------------------------------------------------------------------------- */
bool Situation::GetSkyboxEventTimeFlag( void ) const
{
  return m_Data.event_skybox_time_flag;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief イベント用スカイボックス時間指定
 *
 * @param hour 時間
 * @param minute 分
 */
/* ------------------------------------------------------------------------- */
void Situation::SetSkyboxEventTime( u8 hour, u8 minute )
{
  m_Data.event_skybox_time_hour = hour;
  m_Data.event_skybox_time_minute = minute;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief イベント用スカイボックス時間取得
 *
 * @return 時間
 */
/* ------------------------------------------------------------------------- */
u8 Situation::GetSkyboxEventTimeHour( void ) const
{
  return m_Data.event_skybox_time_hour;
}
/* ------------------------------------------------------------------------- */
/**
 * @brief イベント用スカイボックス分取得
 *
 * @return 分
 */
/* ------------------------------------------------------------------------- */
u8 Situation::GetSkyboxEventTimeMinute( void ) const
{
  return m_Data.event_skybox_time_minute;
}


GFL_NAMESPACE_END(Savedata)

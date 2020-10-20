//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   GameTime.cpp
 *  @brief  ゲーム内時間データ
 *  @author tomoya takahahi
 *  @date   2011.09.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Savedata/include/GameTime.h"
#include "GameSys/include/NijiTime.h"


namespace Savedata
{


#if defined(GF_PLATFORM_WIN32)
# define NANOSECONDS   (0)
#else
# define NANOSECONDS   nn::cfg::GetUserTimeOffset().GetNanoSeconds()
#endif


const s64 GameTime::ALOLA_OFFSET_NONE = 9999999;//1日は86400秒


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
GameTime::GameTime(void)
{
  this->Initialize();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
GameTime::~GameTime(void)
{
}


//----------------------------------------------------------------------------
/**
 *  @brief  ゲーム時間の初期化
 */
//-----------------------------------------------------------------------------
void GameTime::Initialize( void )
{
  Data* p_data = &m_Data;

  gfl2::std::MemClear( p_data, sizeof(Data) );  //一度0クリア
  
  GameSys::DeviceDate deviceDate;
  gfl2::system::Date*  date = deviceDate.GetDate();                       ///< 今の日時を取得
  p_data->use_flag = DATA_ID;                     ///< オリジナルIDを埋め込む。
  this->SetDate( date );
  p_data->sv_day = 0;
  p_data->start_sec = date->GetDateTimeToSecond(); ///< ゲーム開始日時決定
  p_data->clear_sec = 0;
  p_data->penalty_time = 0;
  p_data->rtc_offset = NANOSECONDS;
  clearAlolaOffset( &p_data->alola_offset );
  clearAlolaOffset( &p_data->alola_otherworld_offset );
}

//----------------------------------------------------------------------------
/**
 *  @brief  機能しているかのフラグ
 *
 *  @retval true    機能している。
 *  @retval false   機能していない。
 */
//-----------------------------------------------------------------------------
bool GameTime::IsUseFlag( void ) const
{
  if( m_Data.use_flag == DATA_ID ){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  今の日時を保存
 *
 *  @param  p_date      保存領域
 *  @param  cp_dateTime 今の日時
 */
//-----------------------------------------------------------------------------
void GameTime::SetDate( const gfl2::system::Date* cp_dateTime )
{
  Data * p_data = &m_Data;
  
  p_data->sv_date.year  = cp_dateTime->GetYear();
  p_data->sv_date.month = cp_dateTime->GetMonth();
  p_data->sv_date.day   = cp_dateTime->GetDay();
  p_data->sv_date.hour  = cp_dateTime->GetHour();
  p_data->sv_date.minute  = cp_dateTime->GetMinute();
  p_data->sv_date.second  = cp_dateTime->GetSecond();
  p_data->sv_date.millisecond = cp_dateTime->GetMilliSecond();
}


//----------------------------------------------------------------------------
/**
*  @brief  日時の取得
*
*  @param  cp_date    セーブ日時
*  @param  p_dateTime  情報格納先
*/
//-----------------------------------------------------------------------------
void GameTime::GetDate(gfl2::system::Date* p_dateTime) const
{
  const Data* cp_data = &m_Data;
  gfl2::system::Date date(
    cp_data->sv_date.year,
    cp_data->sv_date.month,
    cp_data->sv_date.day,
    cp_data->sv_date.hour,
    cp_data->sv_date.minute,
    cp_data->sv_date.second,
    cp_data->sv_date.millisecond);
  gfl2::system::Date::Copy(&date, p_dateTime);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゲーム内時間（曜日）　曜日単位のイベント更新に利用を取得
 *
 *  @return u32 ゲーム内曜日
 */
//-----------------------------------------------------------------------------
#if 0 //@attention nijiでは使用しないがライブラリとして用意したい
nn::fnd::Week GameTime::GetWeek( void ) const
{
  const Data* cp_data = &m_Data;
  gfl2::system::Date date(
    cp_data->sv_date.year,
    cp_data->sv_date.month,
    cp_data->sv_date.day,
    cp_data->sv_date.hour,
    cp_data->sv_date.minute,
    cp_data->sv_date.second,
    cp_data->sv_date.milliSecond);
  return date.GetWeek();
}
#endif


//----------------------------------------------------------------------------
/**
 *  @brief  ゲーム開始時間の取得
 */
//-----------------------------------------------------------------------------
s64 GameTime::GetStartSec( void ) const
{
  return m_Data.start_sec;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゲーム開始時間の取得
 */
//-----------------------------------------------------------------------------
void GameTime::SetStartSec( s64 start_sec )
{
  m_Data.start_sec = start_sec;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゲームクリア時間の設定
 */
//-----------------------------------------------------------------------------
void GameTime::SetClearSec( s64 clear_sec )
{
  m_Data.clear_sec = clear_sec;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゲームクリア時間の設定
 */
//-----------------------------------------------------------------------------
s64 GameTime::GetClearSec( void ) const
{
  return m_Data.clear_sec;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ペナルティモードチェック
 *
 *  @param  cp_data データ
 *
 *  @retval true  ペナルティモード
 *  @retval false じゃない
 */
//-----------------------------------------------------------------------------
bool GameTime::IsPenaltyMode( void ) const
{
  if( m_Data.penalty_time != 0 ){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ペナルティ時間の設定
 *
 *  @param  p_data データ
 */
//-----------------------------------------------------------------------------
void GameTime::SetPenaltyTime( void )
{
  /// ペナルティ時間をセットする
  m_Data.penalty_time = 60 * 24;  ///< 24時間

  /// 時間経過を今からにする
  GameSys::DeviceDate deviceDate;
  gfl2::system::Date* date = deviceDate.GetDate();

  /// 今を設定
  this->SetDate( date );
  m_Data.sv_day = static_cast<u32>(date->GetDateTimeToDay());
}

//----------------------------------------------------------------------------
/**
 *  @brief  ペナルティ時間　カウントダウン
 *
 *  @param  p_data        データ
 *  @param  diff_minute   経過分
 */
//-----------------------------------------------------------------------------
void GameTime::CountDownPenaltyTime( int diff_minute )
{
  /// セーブデータにゴミが入っていた場合の対策
  if (m_Data.penalty_time > 24 * 60) {
    m_Data.penalty_time = 24 * 60;
  }

  if (m_Data.penalty_time < static_cast<u32>(diff_minute)) {
    m_Data.penalty_time = 0;
  } else {
    m_Data.penalty_time -= diff_minute;
    //TOMOYA_PRINT( "ペナルティタイム　カウントダウン %d\n", m_Data.penalty_time );
  }
}


//-----------------------------------------------------------------------------
/**
 *    RTC(リアルタイムクロック)改変オフセットのチェック
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  RTC改変オフセットが一致しているかチェック
 *  @retval true    一致している
 *  @retval false   一致していない
 */
//-----------------------------------------------------------------------------
bool GameTime::IsIdentifyRTCOffset( void ) const
{
  return (NANOSECONDS == m_Data.rtc_offset);
}

//----------------------------------------------------------------------------
/**
 *  @brief  RTC改変オフセットの値を更新する。
 */
//-----------------------------------------------------------------------------
void GameTime::UpdateRTCOffet( void )
{
  m_Data.rtc_offset = NANOSECONDS;
}

//----------------------------------------------------------------------------
/**
 *  @brief alola時間改変オフセットが存在しているか？　計算できるか？
 */
//-----------------------------------------------------------------------------
bool GameTime::IsAlolaOffsetExists( void ) const
{
  return isAlolaOffsetExists( m_Data.alola_offset );

}



//----------------------------------------------------------------------------
/**
 *  @brief alola時間改変オフセットの設定
 */
//-----------------------------------------------------------------------------
void GameTime::SetAlolaOffset( s64 alola_offset )
{
  m_Data.alola_offset = alola_offset;
}

//----------------------------------------------------------------------------
/**
 *  @brief alola時間改変オフセットの取得
 */
//-----------------------------------------------------------------------------
s64 GameTime::GetAlolaOffset( void ) const
{
  GFL_ASSERT( IsAlolaOffsetExists() );
  return m_Data.alola_offset;
}


//----------------------------------------------------------------------------
/**
 *  @brief alola別世界時間改変オフセットが存在しているか？
 */
//-----------------------------------------------------------------------------
bool GameTime::IsAlolaOtherWorldOffsetExists( void ) const
{
  return isAlolaOffsetExists( m_Data.alola_otherworld_offset );
}

//----------------------------------------------------------------------------
/**
 *  @brief alola別世界時間改変オフセットの設定
 */
//-----------------------------------------------------------------------------
void GameTime::SetAlolaOtherWorldOffset( s64 alola_offset )
{
  m_Data.alola_otherworld_offset = alola_offset;
}


//----------------------------------------------------------------------------
/**
 *  @brief alola別世界時間改変オフセットの取得
 */
//-----------------------------------------------------------------------------
s64 GameTime::GetAlolaOtherWorldOffset( void ) const
{
  GFL_ASSERT( IsAlolaOtherWorldOffsetExists() );
  return m_Data.alola_otherworld_offset;
}

//----------------------------------------------------------------------------
/**
 *  @brief alola別世界時間改変オフセットのクリア
 */
//-----------------------------------------------------------------------------
void GameTime::ClearAlolaOtherWorldOffset( void )
{
  clearAlolaOffset( &m_Data.alola_otherworld_offset );
}






//----------------------------------------------------------------------------
// private
//-----------------------------------------------------------------------------

/**
 *  @brief アローラオフセットの初期化
 */
void GameTime::clearAlolaOffset( s64* offset )
{
  *offset = ALOLA_OFFSET_NONE;
}

/**
 *  @brief アローラオフセットが存在しているかチェック
 */
bool GameTime::isAlolaOffsetExists( const s64 offset )
{
  if( offset != ALOLA_OFFSET_NONE )
  {
    return true;
  }

  return false;
}

} // namespace Savedata

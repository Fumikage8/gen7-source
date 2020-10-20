//============================================================================================
/**
 * @brief	PlayTime.cpp
 * @brief	プレイ時間操作
 * @date	2011.12.16
 */
//============================================================================================
#include <debug/include/gfl2_Assert.h>
#include "Savedata/include/PlayTime.h"
#include "Savedata/include/GameTime.h"


GFL_NAMESPACE_BEGIN(Savedata)

//---------------------------------------------------------------------------
/**
 * @brief	コンストラクタ
 */
//---------------------------------------------------------------------------
PlayTime::PlayTime( void )
{
	this->Initialize();
}

//---------------------------------------------------------------------------
/**
 * @brief	デストラクタ
 */
//---------------------------------------------------------------------------
PlayTime::~PlayTime( void )
{
}

//---------------------------------------------------------------------------
/**
 * @brief	プレイ時間の初期化処理
 */
//---------------------------------------------------------------------------
void PlayTime::Initialize( void )
{
	gfl2::std::MemClear( &m_playTime, sizeof(m_playTime) );
}

//---------------------------------------------------------------------------
/**
 * @brief	プレイ時間のセット
 * @param	hour	時間
 * @param	min		分
 * @param	sec		秒
 */
//---------------------------------------------------------------------------
void PlayTime::SetPlayTime( u32 hour, u32 min, u32 sec )
{
  GFL_ASSERT(hour <= PLAYTIME_HOUR_MAX);
  GFL_ASSERT(min  <= PLAYTIME_MINUTE_MAX);
  GFL_ASSERT(sec  <= PLAYTIME_SECOND_MAX);
	m_playTime.hour   = hour;
	m_playTime.minute = min;
	m_playTime.second = sec;
}

//---------------------------------------------------------------------------
/**
 * @brief プレイ時間のカウントアップ
 * @param	passed_sec	経過時間
 */
//---------------------------------------------------------------------------
void PlayTime::CountUp( u32 passed_sec )
{
	if( (m_playTime.hour == PLAYTIME_HOUR_MAX)
	&&	(m_playTime.minute == PLAYTIME_MINUTE_MAX)
	&&	(m_playTime.second == PLAYTIME_SECOND_MAX)
	){
		return;
	}

	s32 hour = m_playTime.hour;
	s32 min  = m_playTime.minute;
	s32 sec  = m_playTime.second + passed_sec;

	if( sec > PLAYTIME_SECOND_MAX )
	{
		min = m_playTime.minute + (sec / (PLAYTIME_SECOND_MAX+1));
		sec %= (PLAYTIME_SECOND_MAX+1);
		if( min > PLAYTIME_MINUTE_MAX )
		{
			hour = m_playTime.hour + (min / (PLAYTIME_MINUTE_MAX+1));
			min %= (PLAYTIME_MINUTE_MAX+1);
			if( hour >= PLAYTIME_HOUR_MAX )
			{
				hour = PLAYTIME_HOUR_MAX;
				min = PLAYTIME_MINUTE_MAX;
				sec = PLAYTIME_SECOND_MAX;
			}
		}
	}

	m_playTime.hour  = hour;
	m_playTime.minute = min;
	m_playTime.second = sec;
}

//---------------------------------------------------------------------------
/**
 * @brief 時間の取得
 */
//---------------------------------------------------------------------------
u16 PlayTime::GetPlayHour( void ) const
{
	return m_playTime.hour;
}

//---------------------------------------------------------------------------
/**
 * @brief 分の取得
 * @return
 */
//---------------------------------------------------------------------------
u8 PlayTime::GetPlayMinute( void ) const
{
	return m_playTime.minute;
}

//---------------------------------------------------------------------------
/**
 * @brief 秒の取得
 * @return
 */
//---------------------------------------------------------------------------
u8 PlayTime::GetPlaySecond( void ) const
{
	return m_playTime.second;
}


//---------------------------------------------------------------------------
/**
 * @brief		セーブした日付と時間を設定
 * @return	none
 */
//---------------------------------------------------------------------------
void PlayTime::SetSaveTime( Savedata::GameTime* gameTime )
{
  gfl2::system::Date date;
  gameTime->GetDate( &date );

	m_playTime.save_date.save_year   = date.GetYear();
	m_playTime.save_date.save_month  = date.GetMonth();
	m_playTime.save_date.save_day    = date.GetDay();
	m_playTime.save_date.save_hour   = date.GetHour();
	m_playTime.save_date.save_minute = date.GetMinute();
}

//---------------------------------------------------------------------------
/**
 * @brief		セーブした年を取得
 * @param		ptime		プレイ時間へのポインタ
 * @return	年
 */
//---------------------------------------------------------------------------
u32 PlayTime::GetSaveYear( void )
{
	return m_playTime.save_date.save_year;
}

//---------------------------------------------------------------------------
/**
 * @brief		セーブした月を取得
 * @param		ptime		プレイ時間へのポインタ
 * @return	月
 */
//---------------------------------------------------------------------------
u32 PlayTime::GetSaveMonth( void )
{
	return m_playTime.save_date.save_month;
}

//---------------------------------------------------------------------------
/**
 * @brief		セーブした日を取得
 * @param		ptime		プレイ時間へのポインタ
 * @return	日
 */
//---------------------------------------------------------------------------
u32 PlayTime::GetSaveDay( void )
{
	return m_playTime.save_date.save_day;
}

//---------------------------------------------------------------------------
/**
 * @brief		セーブした時を取得
 * @param		ptime		プレイ時間へのポインタ
 * @return	時
 */
//---------------------------------------------------------------------------
u32 PlayTime::GetSaveHour( void )
{
	return m_playTime.save_date.save_hour;
}

//---------------------------------------------------------------------------
/**
 * @brief		セーブした分を取得
 * @param		ptime		プレイ時間へのポインタ
 * @return	分
 */
//---------------------------------------------------------------------------
u32 PlayTime::GetSaveMinute( void )
{
	return m_playTime.save_date.save_minute;
}

//---------------------------------------------------------------------------
/**
 * @brief   セーブ実行時間の退避
 */
//---------------------------------------------------------------------------
void PlayTime::PushSaveTime( void )
{
  m_saveTimeBackup = m_playTime.save_date;
}


//---------------------------------------------------------------------------
/**
 * @brief   セーブ実行時間の復帰
 */
//---------------------------------------------------------------------------
void PlayTime::PopSaveTime( void )
{
  m_playTime.save_date = m_saveTimeBackup;
}


GFL_NAMESPACE_END(Savedata)

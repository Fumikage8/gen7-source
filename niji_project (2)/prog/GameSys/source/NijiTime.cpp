//======================================================================
/**
 * @file AlolaTime.cpp
 * @date 2015/10/21 11:54:56
 * @author takahashi_tomoya
 * @brief アローラ時間
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "GameSys/include/NijiTime.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata\include\EventWork.h"

#include "Field/FieldStatic/include/Time/EvTime.h"

GFL_NAMESPACE_BEGIN(GameSys)

const u32 AlolaTime::SUN_RESET_HOUR     = 6;    // サンバージョンの1日リセット時間
const u32 AlolaTime::MOON_RESET_HOUR   = 18;   // ムーンバージョンの1日リセット時間


const u32 SUN_START_HOUR    = 10;    // サンバージョンの1日リセット時間
const u32 MOON_START_HOUR   = 22;   // ムーンバージョンの1日リセット時間


const s32 INijiDate::DAY_SECOND     = 60 * 60 * 24;    // 1日の秒数
const s32 INijiDate::HOUR_SECOND    = 60 * 60;         // １時間の秒数
const s32 INijiDate::MINUTE_SECOND  = 60;              // 1分の秒数


gfl2::system::Date AlolaTime::m_sFakeDate;  ///< フェイク日時
b32 AlolaTime::m_sIsFakeDate = false;

/**
*  @コンストラクタ
 */
AlolaTime::AlolaTime() : INijiDate()
 ,m_Date()
 ,m_AlolaOffset(Savedata::GameTime::ALOLA_OFFSET_NONE)
{
#if PM_DEBUG
	/*
	// ユニットテスト 1日づつずらしながら、時間の設定を10年分行う。
	b32 unit_error = false;
	for (u32 i = 0; i < 360 * 10; ++i)
	{
		gfl2::system::Date date1;
		gfl2::system::Date date2;

		SetSecondToDate(&date1, i * 60 * 60 * 24);
		SetSecondToDate(&date2, (i + 1) * 60 * 60 * 24);
		
		s64 date2_second = GetDateToSecond(date2);

		SetSecondToDate(&date1, date2_second);

		s64 date1_second = GetDateToSecond(date1);;

		if( date2_second != date1_second )
		{
			GFL_RELEASE_PRINT( "uterror [%d] ", i );
			GFL_RELEASE_PRINT( "%ld != ", date1_second );
			GFL_RELEASE_PRINT("%ld\n", date2_second);
			unit_error = true;
		}
	}
	
	GFL_ASSERT(unit_error == false);


	// ユニットテスト ランダムずらし
	unit_error = false;
	for (u32 i = 0; i < 360 * 10; ++i)
	{
		gfl2::system::Date date1;
		gfl2::system::Date date2;

		u32 rand = System::GflUse::GetPublicRand(40);

		SetSecondToDate(&date1, (i * 60 * 60 * 24));
		SetSecondToDate(&date2, (i + System::GflUse::GetPublicRand(40)) * 60 * 60 * 24);

		s64 date2_second = GetDateToSecond(date2);

		SetSecondToDate(&date1, date2_second);

		s64 date1_second = GetDateToSecond(date1);

		if( date2_second != date1_second )
		{
			GFL_RELEASE_PRINT("uterror [%d]->[%d]\n", i, i + rand);
			GFL_RELEASE_PRINT("%ld != ", date1_second);
			GFL_RELEASE_PRINT("%ld\n", date2_second);
			unit_error = true;
		}
	}

	GFL_ASSERT(unit_error == false);
	*/
#endif
}

/**
 *  @デストラクタ
 */
AlolaTime::~AlolaTime()
{
}


/**
 *  @brief 現在のカントー時間から初期化
 *  @param  cpGameData
 */
void AlolaTime::Initialize()
{
  GameManager * pGameMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  GFL_ASSERT_STOP(pGameMan);

  GameSys::GameData* cpGameData = pGameMan->GetGameData();

  // フェイク時間の反映
  if( !m_sIsFakeDate )
  {
    gfl2::system::Date::GetNow( &m_Date );

  }
  else
  {
    gfl2::system::Date::Copy( &m_sFakeDate, &m_Date );
  }

  // アローラーオフセットを取得
  // 世界軸のチェック
  if( !CheckAlolaOtherWorld(cpGameData) )
  {
    GFL_ASSERT( cpGameData->GetGameTime()->IsAlolaOffsetExists() );
    m_AlolaOffset = cpGameData->GetGameTime()->GetAlolaOffset();
  }
  else
  {
    GFL_ASSERT( cpGameData->GetGameTime()->IsAlolaOtherWorldOffsetExists() );
    m_AlolaOffset = cpGameData->GetGameTime()->GetAlolaOtherWorldOffset();
  }

  // アローラ時間にする。
  AddDateSecond( &m_Date, m_AlolaOffset );

}


/**
*  @brief 現在のカントー時間から初期化
*  @param  アローラオフセット指定版
*/
void AlolaTime::Initialize( s64 alola_offset )
{
  gfl2::system::Date::GetNow( &m_Date );
  m_AlolaOffset = alola_offset;

  // アローラ時間にする。
  AddDateSecond( &m_Date, m_AlolaOffset );
}


/**
*  @brief 指定のカントー時間とアローラオフセットから初期化
*/
void AlolaTime::Initialize( const gfl2::system::Date & date, s64 alola_offset )
{
  gfl2::system::Date::Copy( &date, &m_Date );
  m_AlolaOffset = alola_offset;

  // アローラ時間にする。
  AddDateSecond( &m_Date, m_AlolaOffset );
}


/**
 *  @brief アローラ時間 時間取得
 */
s32 AlolaTime::GetHour() const
{
  GFL_ASSERT( m_AlolaOffset != Savedata::GameTime::ALOLA_OFFSET_NONE );
  return m_Date.GetHour();
}


/**
 *  @brief アローラ時間　分取得
 */
s32 AlolaTime::GetMinute() const
{
  GFL_ASSERT( m_AlolaOffset != Savedata::GameTime::ALOLA_OFFSET_NONE );
  return m_Date.GetMinute();
}

/**
 *  @brief アローラ時間　秒取得
 */
s32 AlolaTime::GetSecond() const
{
  GFL_ASSERT( m_AlolaOffset != Savedata::GameTime::ALOLA_OFFSET_NONE );
  return m_Date.GetSecond();
}


/**
 *  @brief アローラ時間　ミリ秒取得
 */
s32 AlolaTime::GetMilliSecond() const
{
  GFL_ASSERT( m_AlolaOffset != Savedata::GameTime::ALOLA_OFFSET_NONE );
  return m_Date.GetMilliSecond();
}

/**
 *  @brief 現在時間の取得
 */
void AlolaTime::Update()
{
  Initialize();
}


#if PM_DEBUG
/**
 * @brief  時間と分をFixする
 * @param  hour    時
 * @param  minute  分
 */
void AlolaTime::FixFakeTime( s32 hour, s32 minute )
{
  gfl2::system::Date date;
  gfl2::system::Date::Copy( &m_Date, &date );

  // アローラ時間を指定時間にする。
  date.SetHour(hour);
  date.SetMinute(minute);
  date.SetSecond(0); //< @fix NMCat[1925] 時間固定イベントで別イベント開始時の秒数が固定される
  
  // カントー時間に変更
  INijiDate::SubDateSecond( &date, m_AlolaOffset );

  // カントー時間を設定
  gfl2::system::Date::FixFakeTime( date.GetHour(), date.GetMinute() );
}
#endif



/**
 *  @brief 製品版用　アローラ限定のFakeTime
 */
void AlolaTime::FixAlolaOnlyFakeTime( s32 hour, s32 minute )
{
  gfl2::system::Date::Copy( &m_Date, &m_sFakeDate );

  // アローラ時間を指定時間にする。
  m_sFakeDate.SetHour(hour);
  m_sFakeDate.SetMinute(minute);
  m_sFakeDate.SetSecond(0); //< @fix NMCat[1925] 時間固定イベントで別イベント開始時の秒数が固定される
  
  // カントー時間に変更
  INijiDate::SubDateSecond( &m_sFakeDate, m_AlolaOffset );
  m_sIsFakeDate = true;
}

/**
 *  @brief 製品版用　アローラ限定のFakeTimeのクリア
 */
void AlolaTime::ClearAlolaOnlyFakeTime(void)
{
  m_sIsFakeDate = false;
}

// フェイク時間　セーブ復帰処理
b32 AlolaTime::GetFixAlolaOnlyFakeTimeSaveParam( u8* hour, u8* minute, u8* second )
{
  s32 s_hour = m_sFakeDate.GetHour();
  s32 s_minute = m_sFakeDate.GetMinute();
  s32 s_second = m_sFakeDate.GetSecond();

  GFL_ASSERT( (s_hour >= 0) && (s_hour <= 24) );
  GFL_ASSERT( (s_minute >= 0) && (s_minute <= 60) );
  GFL_ASSERT( (s_second >= 0) && (s_second <= 60) );


  *hour = static_cast<u8>(m_sFakeDate.GetHour());
  *minute = static_cast<u8>(m_sFakeDate.GetMinute());
  *second = static_cast<u8>(m_sFakeDate.GetSecond());

  return m_sIsFakeDate;
}

void AlolaTime::SetFixAlolaOnlyFakeTimeSaveParam( b8 flag, u8 hour, u8 minute, u8 second )
{
  m_sIsFakeDate = flag;
  if( m_sIsFakeDate )
  {
    gfl2::system::Date::GetNow( &m_sFakeDate );
    m_sFakeDate.SetHour(hour);
    m_sFakeDate.SetMinute(minute);
    m_sFakeDate.SetSecond( second );  // @fix NMCat[983]アローラ時間固定イベントで時間が戻ることがある。 alolaOffsetは秒で計算しているため、元の秒数まで保存して復元しないと時間が異なる。
  }
}




/**
 *  @brief アローラ時間補正オフセットを求める。
 */
s64 AlolaTime::GetAlolaOffset( u32 rom_version, AlolaOtherWorldType type )
{
  gfl2::system::Date date;
  gfl2::system::Date alola_date;
  gfl2::system::Date::GetNow(&date);
  gfl2::system::Date::Copy(&date, &alola_date);
  s32 now_hour = date.GetHour();  // @fix NMCat[605] ゲーム内時間の反映が資料と異なることについて

  s64 alola_offset = 0;
  b32 is_resetMinute = true;

  s32 hour;
  if( type == ALOLA_OTHER_WOLRD_TYPE_GAMESTART )
  {
    hour = GetRomGameStartHour(rom_version);
  }
  else if( type == ALOLA_OTHER_WOLRD_TYPE_NORMAL )
  {
    is_resetMinute = false; //分以下をリセットしない。
    hour = GetRomAlolaHour(rom_version, now_hour);
  }
  else
  {
    hour = GetOtherWorldTypeGameStartHour( type );
  }


  if( now_hour < hour )
  {
    // 時間をhourにするのみ
    alola_date.SetHour(hour);
  }
  else
  {
    //@fix GFNMCat[2490] 1日のずらし方を秒単位の加算で行なうように修正 月を跨ぐケース等を想定
    // 1日ずらしたうえでhourにする
    s64 now_s = alola_date.GetDateTimeToSecond();
    now_s += 24 * 60 * 60;  //1日秒
    alola_date.SetDateTimeToSecond(now_s);
    alola_date.SetHour( hour );
  }
  if(is_resetMinute)
  {
    alola_date.SetMinute(0);
    alola_date.SetSecond(0);
    alola_date.SetMilliSecond(0);
  }

  alola_offset = SubDate( &alola_date, &date );

  return alola_offset;
}


/**
 *  @brief アローラ別世界時間のオフセットを設定する。
 */
void AlolaTime::StartAlolaOtherWorldOffset( GameSys::GameData* p_gdata, AlolaTime::AlolaOtherWorldType otherworld_type )
{
  s32 alola_otherworld_offset = GetAlolaOffset( GET_VERSION(), otherworld_type );

  if( !p_gdata->GetGameTime()->IsAlolaOtherWorldOffsetExists() )
  {
    p_gdata->GetGameTime()->SetAlolaOtherWorldOffset( alola_otherworld_offset );
    *(p_gdata->GetEventWork()->GetEventWorkAdrs( WK_SYS_ALOLA_TIME_TYPE )) = SYS_WK_ALOLA_TIME_TYPE_REVERSE;
  }
  else
  {
    // クリアする前に再度開始した。
    GFL_ASSERT(0);
  }
}


/**
 *  @brief アローラ時間に戻す。
 */
void AlolaTime::ClearAlolaOtherWorldOffset( GameSys::GameData* p_gdata )
{
  p_gdata->GetGameTime()->ClearAlolaOtherWorldOffset();
  *(p_gdata->GetEventWork()->GetEventWorkAdrs( WK_SYS_ALOLA_TIME_TYPE )) = SYS_WK_ALOLA_TIME_TYPE_NORMAL;
}


/**
 *  @brief アローラ時間が別世界状態かチェック
 */
b32 AlolaTime::CheckAlolaOtherWorld( const GameSys::GameData* cp_gdata )
{
  return CheckAlolaOtherWorld( cp_gdata->GetEventWork() );
}

b32 AlolaTime::CheckAlolaOtherWorld( const Field::EventWork* cp_evwork )
{
  if( *(cp_evwork->GetEventWorkAdrs( WK_SYS_ALOLA_TIME_TYPE )) == SYS_WK_ALOLA_TIME_TYPE_REVERSE )
  {
    return true;
  }
  else if( *(cp_evwork->GetEventWorkAdrs(WK_SYS_ALOLA_TIME_TYPE)) != SYS_WK_ALOLA_TIME_TYPE_NORMAL )
  {
    // 0か1じゃ無いとおかしい 
    GFL_ASSERT( *(cp_evwork->GetEventWorkAdrs(WK_SYS_ALOLA_TIME_TYPE)) == SYS_WK_ALOLA_TIME_TYPE_NORMAL );
  }

  return false;

}

/**
 *  @brief アローラ別世界時間に行く能力を使えるかチェック
 */
b32 AlolaTime::IsAlolaTimeJumpOtherWorldSkillUse( MonsNo /*monsNo*/, const GameSys::GameData* cp_gdata )
{
  // 反転世界では使えない
  if( CheckAlolaOtherWorld( cp_gdata ) )
  {
    return false;
  }

  return true;
}

/**
 *  @brief ロムバージョンのゲーム時間開始　Hour取得
 */
s32 AlolaTime::GetRomGameStartHour( u32 rom_version )
{
  if( rom_version == VERSION_SUN2 )
  {
    return SUN_START_HOUR;
  }
  else if( rom_version == VERSION_MOON2 )
  {
    return MOON_START_HOUR;
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  return SUN_START_HOUR;
}

/**
 *  @brief ロムバージョンのゲーム時間　Hour取得(通常状態)
 */
s32 AlolaTime::GetRomAlolaHour( u32 rom_version, s32 now_hour )
{
  if( rom_version == VERSION_SUN2 )
  {
    return now_hour;
  }
  else if( rom_version == VERSION_MOON2 )
  {
    return (now_hour + 12) % 24;
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  return now_hour;
}


/**
 *  @brief 別世界時間タイプごとの開始 Hour取得:
 */
s32 AlolaTime::GetOtherWorldTypeGameStartHour( AlolaOtherWorldType type )
{
  if( type == ALOLA_OTHER_WOLRD_TYPE_NIKKOU )
  {
    return SUN_RESET_HOUR;
  }
  else if( type == ALOLA_OTHER_WOLRD_TYPE_GEKKOU )
  {
    return MOON_RESET_HOUR;
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }

  return SUN_RESET_HOUR;
}


/**
 *  @brief Dateの内容をsecond秒数分引く
 */
void INijiDate::SubDateSecond( gfl2::system::Date * pDate, s32 second )
{
  s64 dateSecond;
  //TOMOYA_PRINT( "Sub Second %d\n", second );
  dateSecond = GetDateToSecond( *pDate );
  dateSecond -= static_cast<s64>(second);
  SetSecondToDate( pDate, dateSecond );
}

/**
 *  @brief Dateの内容をsecond秒数分足す
 */
void INijiDate::AddDateSecond( gfl2::system::Date * pDate, s32 second )
{
  s64 dateSecond;
  //TOMOYA_PRINT( "Sub Second %d\n", second );
  dateSecond = GetDateToSecond( *pDate );
  dateSecond += static_cast<s64>(second);
  SetSecondToDate( pDate, dateSecond );
}

/**
 *  @brief DateとDateの引き算 (差分の秒数を返す。)
 */
s64  INijiDate::SubDate( const gfl2::system::Date * cpDate0, const gfl2::system::Date * cpDate1 )
{
  // s64 には秒で計算すると 約296533308798年分の時間を保持することが出来る。　今はせいぜい2015年なので足し算、引き算には耐えることが出来る。
  s64 dateSecond0;
  s64 dateSecond1;
  dateSecond0 = GetDateToSecond( *cpDate0 );
  dateSecond1 = GetDateToSecond( *cpDate1 );
  dateSecond0 -= dateSecond1;
  
  return dateSecond0;
}


// Date情報を秒に置き換える。 Win32 はうるう年　非対応なので注意
s64 INijiDate::GetDateToSecond( const gfl2::system::Date & cpDate )
{
  s64 second = 0;

  /*
  TOMOYA_PRINT( "year %d month %d day %d hour %d minute %d second %d\n", 
    cpDate.GetYear(),
    cpDate.GetMonth(),
    cpDate.GetDay(),
    cpDate.GetHour(),
    cpDate.GetMinute(),
    cpDate.GetSecond()
    );
    */

#if defined (GF_PLATFORM_WIN32)
  // Win版は月を30固定で計算。　年を360で計算している。
  second = cpDate.GetSecond();
  second += cpDate.GetMinute() * static_cast<s64>(MINUTE_SECOND);
  second += cpDate.GetHour() * static_cast<s64>(HOUR_SECOND);
  second += static_cast<s64>(cpDate.GetDay()) * static_cast<s64>(DAY_SECOND);
  second += static_cast<s64>(cpDate.GetMonth()) * static_cast<s64>(DAY_SECOND * 30);
  second += static_cast<s64>(cpDate.GetYear()) * static_cast<s64>(DAY_SECOND * 30 * 12);

  //TOMOYA_PRINT( "second %ld\n", second );
  return second;
#elif defined (GF_PLATFORM_CTR)

  // CTR版はライブラリを使用する。
  nn::fnd::DateTime date( cpDate.GetYear(), cpDate.GetMonth(), cpDate.GetDay(), cpDate.GetHour(), cpDate.GetMinute(), cpDate.GetSecond(), cpDate.GetMilliSecond() );

  nn::fnd::TimeSpan time = date - nn::fnd::DateTime::MIN_DATETIME;

  //TOMOYA_PRINT( "second %ld\n", time.GetSeconds() );
  return time.GetSeconds();

#endif
}


// 秒を元に、Dateを復元する。
void INijiDate::SetSecondToDate( gfl2::system::Date * pDate, s64 second )
{
  //TOMOYA_PRINT( "second %ld\n", second );

#if defined (GF_PLATFORM_WIN32)
  s32 year    = static_cast<s32>(second / static_cast<s64>(DAY_SECOND * 30 * 12));  
  second -= static_cast<s64>(year) * static_cast<s64>(DAY_SECOND * 30 * 12);
  //TOMOYA_PRINT( "year second %ld\n", second );

  s32 month   = static_cast<s32>(second / static_cast<s64>(DAY_SECOND * 30));       
  second -= static_cast<s64>(month) * static_cast<s64>(DAY_SECOND * 30);
  //TOMOYA_PRINT( "month second %ld\n", second );

  s32 day     = static_cast<s32>(second / static_cast<s64>(DAY_SECOND));      
  second -= static_cast<s64>(day) * static_cast<s64>(DAY_SECOND);
  //TOMOYA_PRINT( "day second %ld\n", second );

  s32 hour    = static_cast<s32>(second / static_cast<s64>(HOUR_SECOND));  
  second -= static_cast<s64>(hour) * static_cast<s64>(HOUR_SECOND);
  //TOMOYA_PRINT( "hour second %ld\n", second );

  s32 minute  = static_cast<s32>(second / static_cast<s64>(MINUTE_SECOND));         
  second -= static_cast<s64>(minute) * static_cast<s64>(MINUTE_SECOND);
  //TOMOYA_PRINT( "minute second %ld\n", second );

  pDate->SetYear(year);
  pDate->SetMonth(month);
  pDate->SetDay(day);
  pDate->SetHour(hour);
  pDate->SetMinute(minute);
  pDate->SetSecond(second);

#elif defined (GF_PLATFORM_CTR)

  // CTR版はライブラリを使用する。
  nn::fnd::TimeSpan time = nn::fnd::TimeSpan::FromSeconds( second );
  nn::fnd::DateTime date = nn::fnd::DateTime::MIN_DATETIME + time;

  //@fix まずDayの情報を初期化 月によりない日があるため。
	//@fix GFCHECK[2983]  常に有効化
  pDate->SetDay(1);

  pDate->SetYear(date.GetYear());
  pDate->SetMonth(date.GetMonth());
  pDate->SetDay(date.GetDay());
  pDate->SetHour(date.GetHour());
  pDate->SetMinute(date.GetMinute());
  pDate->SetSecond(date.GetSecond());
  pDate->SetMilliSecond(0);

#endif
/*  
  TOMOYA_PRINT( "year %d month %d day %d hour %d minute %d second %d\n", 
    pDate->GetYear(),
    pDate->GetMonth(),
    pDate->GetDay(),
    pDate->GetHour(),
    pDate->GetMinute(),
    pDate->GetSecond()
    );
  */  
}


#if PM_DEBUG

/**
 *  @brief ログ出力
 */
void INijiDate::DebugLog(void) const
{
  const gfl2::system::Date* pDate = this->GetDateConst();
  DebugLog( *pDate );
}

void INijiDate::DebugLog(const gfl2::system::Date& date)
{
  GFL_PRINT( "year %d month %d day %d hour %d minute %d second %d\n", 
    date.GetYear(),
    date.GetMonth(),
    date.GetDay(),
    date.GetHour(),
    date.GetMinute(),
    date.GetSecond()
    );
}
#endif


GFL_NAMESPACE_END(GameSys)

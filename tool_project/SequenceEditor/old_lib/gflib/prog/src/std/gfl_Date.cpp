//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Date.cpp
 *	@brief  日時管理クラス
 *	@author	tomoya takahashi
 *	@date		2011.09.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <std/gfl_Date.h>

namespace gfl
{
namespace std
{

  // 日時コア情報
  bool              DateTime::m_Initialize = false;
  nn::fnd::DateTime DateTime::m_CoreDate;
  s32               DateTime::m_Wait = 0;
  
#if GFL_DEBUG

  DateTime::FakeMode DateTime::m_FakeMode = DateTime::FAKE_NONE;
  u32      DateTime::m_Rate     = 1;
  s64      DateTime::m_BeginSec = 0;
  s64      DateTime::m_CoreSec = 0;
  s64      DateTime::m_RecSec   = 0;
  nn::fnd::DateTime DateTime::m_FakeDate;
    
#endif
  
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


  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化
   */
  //-----------------------------------------------------------------------------
  void DateTime::Initialize( void )
  {
    // 今の時間取得
    nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
    m_CoreDate = date;
    m_Wait = 0;

    // 初期化完了
    m_Initialize = true;
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  更新処理
   *
   *	@param isForceUpdate trueを指定することで強制更新
   */
  //-----------------------------------------------------------------------------
  void DateTime::Update( bool isForceUpdate )
  {
    if( !m_Initialize ){
      GFL_ASSERT( m_Initialize );
      return ;
    }
    
    // 通常データ更新
    {
      ++m_Wait;
      if( (m_Wait >= sc_DATE_UPDATE_WAIT) || isForceUpdate ){
        // 今の時間取得
        nn::fnd::DateTime date = nn::fnd::DateTime::GetNow();
        m_CoreDate = date;
        m_Wait = 0;
      }
    }

#if GFL_DEBUG
    if( m_FakeMode )
    {
      UpdateFake();
    }
#endif
    
  }



#if GFL_DEBUG

  //----------------------------------------------------------------------------
  /**
   *	@brief  フェイクタイム開始
   *
   *	@param	u32 rate  進むスピードの倍数
   */
  //-----------------------------------------------------------------------------
  void DateTime::StartFakeTime( u32 rate )
  {
    if(m_FakeMode == FAKE_NONE){
      m_FakeDate = m_CoreDate;
    }
    
    m_FakeMode = FAKE_FAST;
    m_Rate     = rate;

    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_FakeDate - date2000;
    m_BeginSec = time.GetSeconds();
    time = m_CoreDate - date2000;
    m_CoreSec  = time.GetSeconds();
    m_RecSec   = m_CoreSec;

  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  フェイクモード終了
   */
  //-----------------------------------------------------------------------------
  void DateTime::StopFakeTime( void )
  {
    m_FakeMode = FAKE_NONE;
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  時間と分をFixさせる。
   *
   *	@param	hour    時間
   *	@param	minute  分
   */
  //-----------------------------------------------------------------------------
  void DateTime::FixFakeTime( s32 hour, s32 minute )
  {
    m_FakeMode = FAKE_FIX;
    m_FakeDate = m_CoreDate;
    m_FakeDate = m_FakeDate.ReplaceHour( hour );
    m_FakeDate = m_FakeDate.ReplaceMinute( minute );
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  日時をすべて指定してとめる。
   */
  //-----------------------------------------------------------------------------
  void DateTime::FixFakeDate( const DateTime* p_date )
  {
    m_FakeMode = FAKE_FIX;
    m_FakeDate = p_date->m_Date;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  Fake時間の更新
   */
  //-----------------------------------------------------------------------------
  void DateTime::UpdateFake( void )
  {
    s64 now, fake;

    if( m_FakeMode != DateTime::FAKE_FAST ){
      return ;
    }

    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_CoreDate - date2000;
    now = time.GetSeconds();

    if( now == m_RecSec ){
      return ;
    }

    // 偽時間の算出
    fake = m_BeginSec + ((now - m_CoreSec) * m_Rate);
    m_RecSec = now;
    m_FakeDate = date2000 + nn::fnd::TimeSpan::FromSeconds( fake );
    
  }
    
  //-----------------------------------------------------------------------------
  /**
   *	@brief  FakeModeの取得
   */
  //-----------------------------------------------------------------------------
  DateTime::FakeMode DateTime::GetFakeMode( void )
  {
    return m_FakeMode;
  }
  
  //-----------------------------------------------------------------------------
  /**
   *	@brief  Fakeレートの取得
   */
  //-----------------------------------------------------------------------------
  u32 DateTime::GetFakeRate( void )
  {
    return m_Rate;
  }
  
#endif


  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   *
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( s32 year, s32 month, s32 day, s32 hour, s32 minute, s32 second, s32 millisecond ) : m_Date( year, month, day, hour, minute, second, millisecond )
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   *
   *	@param	second    2000/01/01/00:00からの経過秒数
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( s64 second ) : m_Date()
  {
    this->SetDateTimeToSecond( second );
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  DateTime::DateTime( void ) : m_Date()
  {
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  DateTime::~DateTime()
  {
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  現在日時の取得
   *
   *	@param	p_date    格納先
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetNow( DateTime* p_date )
  {
    GFL_ASSERT( m_Initialize );

    if( p_date ){
#if GFL_DEBUG
      if( m_FakeMode != DateTime::FAKE_NONE ){
        p_date->m_Date = m_FakeDate;
      }else{
        p_date->m_Date = m_CoreDate;
      }
#else
      p_date->m_Date = m_CoreDate;
#endif
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  情報のコピー
   *
   *	@param	cp_src    ソース
   *	@param	p_dst     格納先
   */
  //-----------------------------------------------------------------------------
  void DateTime::Copy( const DateTime* cp_src, DateTime* p_dst )
  {
    if( (p_dst != NULL) && (cp_src != NULL) ){
      p_dst->m_Date = cp_src->m_Date;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  最小日時の取得
   *
   *	@param	p_date    格納先
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetMin( DateTime* p_date )
  {
    p_date->m_Date = nn::fnd::DateTime::MIN_DATETIME;
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  最大日時の取得
   *
   *	@param	p_date    格納先
   */
  //-----------------------------------------------------------------------------
  void DateTime::GetMax( DateTime* p_date )
  {
    p_date->m_Date = nn::fnd::DateTime::MAX_DATETIME;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  年の設定
   *
   *	@param	s32 year 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetYear( s32 year )
  {
    m_Date = m_Date.ReplaceYear( year );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  月の設定
   *
   *	@param	s32 month 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMonth( s32 month )
  {
    m_Date = m_Date.ReplaceMonth( month );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  日の設定
   *
   *	@param	s32 day 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetDay( s32 day )
  {
    m_Date = m_Date.ReplaceDay( day );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  時間の設定
   *
   *	@param	s32 hour 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetHour( s32 hour )
  {
    m_Date = m_Date.ReplaceHour( hour );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  分の設定
   *
   *	@param	s32 minute 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMinute( s32 minute )
  {
    m_Date = m_Date.ReplaceMinute( minute );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  秒の設定
   *
   *	@param	s32 second 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetSecond( s32 second )
  {
    m_Date = m_Date.ReplaceSecond( second );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ミリ秒の設定
   *
   *	@param	millisecond 
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetMilliSecond( s32 millisecond )
  {
    m_Date = m_Date.ReplaceMilliSecond( millisecond );
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  年の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetYear( void ) const
  {
    return m_Date.GetYear();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  月の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMonth( void ) const
  {
    return m_Date.GetMonth();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  曜日の取得
   */
  //-----------------------------------------------------------------------------
  nn::fnd::Week DateTime::GetWeek( void ) const
  {
    return m_Date.GetWeek();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  日の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetDay( void ) const
  {
    return m_Date.GetDay();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  時間の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetHour( void ) const
  {
    return m_Date.GetHour();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  分の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMinute( void ) const
  {
    return m_Date.GetMinute();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  秒の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetSecond( void ) const
  {
    return m_Date.GetSecond();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ミリ秒の取得
   */
  //-----------------------------------------------------------------------------
  s32 DateTime::GetMilliSecond( void ) const
  {
    return m_Date.GetMilliSecond();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  パラメータまとめて取得
   */
  //----------------------------------------------------------------------------
  nn::fnd::DateTimeParameters DateTime::GetParameters( void ) const
  {
    return m_Date.GetParameters();
  }
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  日時を2000/1/1/00:00からの経過日数に変換する。
   *	@return　経過日数
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToDay( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetDays();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  日時を2000/1/1/00:00からの経過時間に変換する。
   *	@return　経過秒数
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToSecond( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetSeconds();
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  日時を2000/1/1/00:00からの経過時間に変換する。
   *	@return　経過ミリ秒
   */
  //-----------------------------------------------------------------------------
  s64 DateTime::GetDateTimeToMilliSecond( void ) const
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = m_Date - date2000;
    return time.GetMilliSeconds();
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  2000/1/1/00:00からの経過秒数から、日時を設定する
   *  @param  second  秒数
   */
  //-----------------------------------------------------------------------------
  void DateTime::SetDateTimeToSecond( s64 second )
  {
    nn::fnd::DateTime date2000;
    nn::fnd::TimeSpan time = nn::fnd::TimeSpan::FromSeconds( second );

    m_Date = date2000 + time;
  }


  //----------------------------------------------------------------------------
  /**
   *	@brief  正常な日時か判断する。
   *
   *	@retval true    正常
   *	@retval false   おかしい
   */
  //-----------------------------------------------------------------------------
  bool DateTime::IsValidDate( void ) const
  {
    return nn::fnd::DateTime::IsValidParameters( 
        m_Date.GetYear(),
        m_Date.GetMonth(),
        m_Date.GetDay(),
        m_Date.GetHour(),
        m_Date.GetMinute(),
        m_Date.GetSecond(),
        m_Date.GetMilliSecond() );
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  うるう年か判断する。
   *
   *	@retval true    うるう年
   *	@retval false   ちがう
   */
  //-----------------------------------------------------------------------------
  bool DateTime::IsLeapYear( void ) const
  {
    return nn::fnd::DateTime::IsLeapYear( m_Date.GetYear() );
  }


} // namespace std
} //namespace gfl

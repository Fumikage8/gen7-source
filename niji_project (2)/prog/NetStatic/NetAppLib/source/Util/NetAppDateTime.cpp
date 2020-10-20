// ============================================================================
/*
 * @file NetAppDateTime.h
 * @brief 
 * @date 2016.05.09
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppDateTime.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


NetAppDateTime::NetAppDateTime() :
  m_Year( 0 ),
  m_Month( 0 ),
  m_Day( 0 ),
  m_Hour( 0 ),
  m_Minute( 0 )
{
}


NetAppDateTime::NetAppDateTime( const NetAppDateTime& dateTime ) :
  m_Year( dateTime.m_Year ),
  m_Month( dateTime.m_Month ),
  m_Day( dateTime.m_Day ),
  m_Hour( dateTime.m_Hour ),
  m_Minute( dateTime.m_Minute )
{
}


NetAppDateTime::NetAppDateTime( u16 year, u8 month, u8 day, u8 hour, u8 minute ) :
  m_Year( year ),
  m_Month( month ),
  m_Day( day ),
  m_Hour( hour ),
  m_Minute( minute )
{
}


NetAppDateTime::~NetAppDateTime()
{
}


NetAppDateTime& NetAppDateTime::operator = ( const NetAppDateTime& dateTime )
{
  m_Year = dateTime.m_Year;
  m_Month = dateTime.m_Month;
  m_Day = dateTime.m_Day;
  m_Hour = dateTime.m_Hour;
  m_Minute = dateTime.m_Minute;
  return *this;
}


u64 NetAppDateTime::GetTime()
{
  u64 time = 0;

  const u64 YEAR_CONVERT   = 100000000;
  const u64 MONTH_CONVERT  =   1000000;
  const u64 DAY_CONVERT    =     10000;
  const u64 HOUR_CONVERT   =       100;
  const u64 MINUTE_CONVERT =         1;

  u64 year   = static_cast<u64>( m_Year )   * YEAR_CONVERT;
  u64 month  = static_cast<u64>( m_Month )  * MONTH_CONVERT;
  u64 day    = static_cast<u64>( m_Day )    * DAY_CONVERT;
  u64 hour   = static_cast<u64>( m_Hour )   * HOUR_CONVERT;
  u64 minute = static_cast<u64>( m_Minute ) * MINUTE_CONVERT;

  time = year + month + day + hour + minute;

  return time;
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

// ============================================================================
/*
 * @file NetAppDateTime.h
 * @brief 
 * @date 2016.05.09
 * @author endo_akira
 * @note Coverityの警告削除用に用意したクラスです。
 *       現状、必要最低限の機能しかないので、状況に応じて随時追加するかもしれません。
 */
// ============================================================================
#if !defined( NETAPP_TIME_CONVERT_UTILITY_H_INCLUDE )
#define NETAPP_TIME_CONVERT_UTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class NetAppDateTime
{

public:

  NetAppDateTime();
  NetAppDateTime( const NetAppDateTime& dateTime );
  NetAppDateTime( u16 year, u8 month, u8 day, u8 hour, u8 minute );
  virtual~NetAppDateTime();

  NetAppDateTime& operator = ( const NetAppDateTime& dateTime );

  u64 GetTime();

private:

  u16   m_Year;
  u8    m_Month;
  u8    m_Day;
  u8    m_Hour;
  u8    m_Minute;

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_TIME_CONVERT_UTILITY_H_INCLUDE

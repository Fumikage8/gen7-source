// ============================================================================
/*
 * @file JoinFestaGlobalAttractionWork.h
 * @brief グローバルアトラクションアプリのワーククラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTION_WORK_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTION_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionDefine.h"
#include "NetStatic/NetAppLib/include/Util/NetAppDateTime.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaGlobalAttractionWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionWork );

private:

  typedef struct
  {
    u16     crc;
    u16     padding;
    u32     version;
    u32     fileSize;
    u32     reserve;
  } Header;

public:


  typedef struct
  {
    u32       successRecord;
    u16       myRecord;
    u16       successReward;
    u16       failureReward;
    u8        ID;
    u8        startDateTimeYear;
    u8        startDateTimeMonth;
    u8        startDateTimeDay;
    u8        startDateTimeHour;
    u8        startDateTimeMinute;
    u8        endDateTimeYear;
    u8        endDateTimeMonth;
    u8        endDateTimeDay;
    u8        endDateTimeHour;
    u8        endDateTimeMinute;
    u8        enableData;
    u8        padding[18];
  } GlobalAttractionInfo;


  static const u32 DATA_MAX = 12;

private:

  static const u32  APP_HEAP_SIZE             = NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_APP_DEVICE_HEAP_SIZE;

public:
  JoinFestaGlobalAttractionWork();
  virtual~JoinFestaGlobalAttractionWork();


  void SetDownloadData( const u8* pDownloadData );

  void SetCurrentDateTime( const NetAppLib::Util::NetAppDateTime& dateTime );

  bool IsOpenGlobalAttraction();
  bool IsOpenGlobalAttraction( GlobalAttractionInfo* pGlobalAttractionInfo );

  GlobalAttractionInfo* GetOpenGlobalAttractionInfo();

  GlobalAttractionInfo* GetDownloadGlobalAttractionInfoList();
  u32 GetDownloadGlobalAttractionInfoListSize();

  u32 GetAttractionNameMassageID( GlobalAttractionInfo* pGlobalAttractionInfo );
  u32 GetAttractionDescriptionMassageID( GlobalAttractionInfo* pGlobalAttractionInfo );

  int GetOpenGlobalAttractionIndex();

  int IDToIndex( GlobalAttractionInfo* pGlobalAttractionInfo );

  bool IsOpenGlobalAttractionHistory();

private:

  bool isOpenGlobalAttraction( GlobalAttractionInfo* pGlobalAttractionInfo );

private:

  Header                            m_Header;
  GlobalAttractionInfo              m_aGlobalAttractionInfo[ DATA_MAX ];
  NetAppLib::Util::NetAppDateTime   m_CurrentDateTime;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONWORK_H_INCLUDE

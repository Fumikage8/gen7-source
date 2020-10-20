// ============================================================================
/*
 * @file JoinFestaGlobalAttractionWork.cpp
 * @brief グローバルアトラクションアプリのワーククラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <util/include/gfl2_std_string.h>
#include <niji_conv_header/message/msg_jf_attraction.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


JoinFestaGlobalAttractionWork::JoinFestaGlobalAttractionWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_Header(),
  m_aGlobalAttractionInfo(),
  m_CurrentDateTime()
{
}


JoinFestaGlobalAttractionWork::~JoinFestaGlobalAttractionWork()
{
}


void JoinFestaGlobalAttractionWork::SetDownloadData( const u8* pDownloadData )
{
  gfl2::std::MemCopy( &pDownloadData[0], &m_Header, sizeof( Header ) );
  gfl2::std::MemCopy( &pDownloadData[ sizeof( Header ) ], &m_aGlobalAttractionInfo[0], sizeof( m_aGlobalAttractionInfo ) );

  GFL_PRINT( "crc      = %d\n", m_Header.crc );
  GFL_PRINT( "padding  = %d\n", m_Header.padding );
  GFL_PRINT( "version  = %d\n", m_Header.version );
  GFL_PRINT( "fileSize = %d\n", m_Header.fileSize );
  GFL_PRINT( "reserve  = %d\n", m_Header.reserve );

  // @fix GFNMCat[5599] カンスト値をセーブデータに設定
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    for( u32 i = 0; i < DATA_MAX; ++i )
    {
      NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pGlobalAttractionSaveData = pJoinFestaDataSave->GetGlobalAttractionData( static_cast<u8>(i) );

      if( pGlobalAttractionSaveData )
      {
        NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData copyData;
        
        gfl2::std::MemCopy( pGlobalAttractionSaveData, &copyData, sizeof( NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData ) );

        copyData.localRecordMax = m_aGlobalAttractionInfo[i].myRecord;
        
        // @fix GFNMCat[5605] グローバルアトラクションIDを設定
        copyData.globalAttractionID = m_aGlobalAttractionInfo[i].ID;
        
        // @fix GFNMCat[5606] 終了年月日を設定
        copyData.endYear    = m_aGlobalAttractionInfo[i].endDateTimeYear;
        copyData.endMonth   = m_aGlobalAttractionInfo[i].endDateTimeMonth;
        copyData.endDay     = m_aGlobalAttractionInfo[i].endDateTimeDay;
        copyData.endHour    = m_aGlobalAttractionInfo[i].endDateTimeHour;
        copyData.endMinitue = m_aGlobalAttractionInfo[i].endDateTimeMinute;

        pJoinFestaDataSave->SetGlobalAttractionData( static_cast<u8>(i), &copyData );
      }
    }
  }

}


void JoinFestaGlobalAttractionWork::SetCurrentDateTime( const NetAppLib::Util::NetAppDateTime& dateTime )
{
  m_CurrentDateTime = dateTime;
}


bool JoinFestaGlobalAttractionWork::IsOpenGlobalAttraction()
{
  bool bIsOpenGlobalAttraction = false;

  if( GetOpenGlobalAttractionIndex() >= 0 )
  {
    bIsOpenGlobalAttraction = true;
  }

  return bIsOpenGlobalAttraction;
}


bool JoinFestaGlobalAttractionWork::IsOpenGlobalAttraction( GlobalAttractionInfo* pGlobalAttractionInfo )
{
  return isOpenGlobalAttraction( pGlobalAttractionInfo );
}


JoinFestaGlobalAttractionWork::GlobalAttractionInfo* JoinFestaGlobalAttractionWork::GetOpenGlobalAttractionInfo()
{
  JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo = NULL;

  int index = GetOpenGlobalAttractionIndex();

  if( 0 <= index && index < DATA_MAX )
  {
    pGlobalAttractionInfo = &m_aGlobalAttractionInfo[ index ];
  }

  return pGlobalAttractionInfo;
}


int JoinFestaGlobalAttractionWork::GetOpenGlobalAttractionIndex()
{
  int index = -1;

  for( u32 i = 0; i < DATA_MAX; ++i )
  {
    if( isOpenGlobalAttraction( &m_aGlobalAttractionInfo[i] ) )
    {
      index = static_cast<int>(i);

      break;
    }
  }

  return index;
}


int JoinFestaGlobalAttractionWork::IDToIndex( GlobalAttractionInfo* pGlobalAttractionInfo )
{
  int index = -1;

  if( pGlobalAttractionInfo )
  {
    // @fix GFNMCat[5605] IDの値が連番ではなく、IDの値がバラバラに振られる仕様になったので、その対応
    for( u32 i = 0; i < DATA_MAX; ++i )
    {
      if( m_aGlobalAttractionInfo[i].ID == pGlobalAttractionInfo->ID )
      {
        index = static_cast<int>(i);

        break;
      }
    }
  }

  return index;
}


bool JoinFestaGlobalAttractionWork::isOpenGlobalAttraction( GlobalAttractionInfo* pGlobalAttractionInfo )
{
  bool isOpen = false;

  if( pGlobalAttractionInfo->enableData > 0 )
  {
    const u16 value2000 = 2000;

    u16 startDateTimeYear = value2000 + static_cast<u16>( pGlobalAttractionInfo->startDateTimeYear );
    u16 endDateTimeYear = value2000 + static_cast<u16>( pGlobalAttractionInfo->endDateTimeYear );

    NetAppLib::Util::NetAppDateTime startDateTime(
      startDateTimeYear,
      pGlobalAttractionInfo->startDateTimeMonth,
      pGlobalAttractionInfo->startDateTimeDay,
      pGlobalAttractionInfo->startDateTimeHour,
      pGlobalAttractionInfo->startDateTimeMinute );

    NetAppLib::Util::NetAppDateTime endDateTime(
      endDateTimeYear,
      pGlobalAttractionInfo->endDateTimeMonth,
      pGlobalAttractionInfo->endDateTimeDay,
      pGlobalAttractionInfo->endDateTimeHour,
      pGlobalAttractionInfo->endDateTimeMinute );

    u64 startTime = startDateTime.GetTime();

    u64 currentTime = m_CurrentDateTime.GetTime();

    u64 endTime = endDateTime.GetTime();

//    GFL_PRINT( "startTime = %llu, currentTime = %llu, endTime = %llu\n", startTime, currentTime, endTime );

    if( startTime <= currentTime && currentTime <= endTime )
    {
      isOpen = true;
    }
  }

  return isOpen;
}


JoinFestaGlobalAttractionWork::GlobalAttractionInfo* JoinFestaGlobalAttractionWork::GetDownloadGlobalAttractionInfoList()
{
  return &m_aGlobalAttractionInfo[0];
}


u32 JoinFestaGlobalAttractionWork::GetDownloadGlobalAttractionInfoListSize()
{
  u32 listSize = 0;

  for( u32 i = 0; i < DATA_MAX; ++i )
  {
    if( m_aGlobalAttractionInfo[i].enableData > 0 )
    {
      ++listSize;
    }
  }

  return listSize;
}


bool JoinFestaGlobalAttractionWork::IsOpenGlobalAttractionHistory()
{
  bool bIsOpenGlobalAttractionHistory = false;

  for( u32 i = 0; i < DATA_MAX; ++i )
  {
    if( m_aGlobalAttractionInfo[i].enableData > 0 )
    {
      bIsOpenGlobalAttractionHistory = true;
    }
  }

  return bIsOpenGlobalAttractionHistory;
}


u32 JoinFestaGlobalAttractionWork::GetAttractionNameMassageID( GlobalAttractionInfo* pGlobalAttractionInfo )
{
  u32 messageID = jf_g_attraction_name_01;

  switch( pGlobalAttractionInfo->ID )
  {
    // @fix MMCat[446]グローバルアトラクションIDがmomijiだと13～24になる。
    case 13: messageID = jf_g_attraction_name_01; break;
    case 14: messageID = jf_g_attraction_name_02; break;
    case 15: messageID = jf_g_attraction_name_03; break;
    case 16: messageID = jf_g_attraction_name_04; break;
    case 17: messageID = jf_g_attraction_name_05; break;
    case 18: messageID = jf_g_attraction_name_06; break;
    case 19: messageID = jf_g_attraction_name_07; break;
    case 20: messageID = jf_g_attraction_name_08; break;
    case 21: messageID = jf_g_attraction_name_09; break;
    case 22: messageID = jf_g_attraction_name_10; break;
    case 23: messageID = jf_g_attraction_name_11; break;
    case 24: messageID = jf_g_attraction_name_12; break;
  }

  return messageID;
}


u32 JoinFestaGlobalAttractionWork::GetAttractionDescriptionMassageID( GlobalAttractionInfo* pGlobalAttractionInfo )
{
  u32 messageID = jf_g_attraction_exp_01;

  // @fix MMCat[446]グローバルアトラクションIDがmomijiだと13～24になる。
  switch( pGlobalAttractionInfo->ID )
  {
    case 13: messageID = jf_g_attraction_exp_01; break;
    case 14: messageID = jf_g_attraction_exp_02; break;
    case 15: messageID = jf_g_attraction_exp_03; break;
    case 16: messageID = jf_g_attraction_exp_04; break;
    case 17: messageID = jf_g_attraction_exp_05; break;
    case 18: messageID = jf_g_attraction_exp_06; break;
    case 19: messageID = jf_g_attraction_exp_07; break;
    case 20: messageID = jf_g_attraction_exp_08; break;
    case 21: messageID = jf_g_attraction_exp_09; break;
    case 22: messageID = jf_g_attraction_exp_10; break;
    case 23: messageID = jf_g_attraction_exp_11; break;
    case 24: messageID = jf_g_attraction_exp_12; break;
  }

  return messageID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

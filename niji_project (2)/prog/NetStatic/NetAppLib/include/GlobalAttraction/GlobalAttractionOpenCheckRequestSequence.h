#if defined(GF_PLATFORM_CTR)
// ============================================================================
/*
 * @file GlobalAttractionOpenCheckRequestSequence.h
 * @brief 
 * @date 2016.01.19
 */
// ============================================================================
#if !defined( GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_SEQUENCE_H_INCLUDE )
#define GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_SEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetStatic/NetAppLib/include/GlobalAttraction/GlobalAttractionOpenCheckResponseListener.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

#if defined( GF_PLATFORM_CTR )

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>

#endif // defined( GF_PLATFORM_CTR )

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(GlobalAttraction)

class GlobalAttractionOpenCheckRequestSequence :
#if defined( GF_PLATFORM_CTR )
  public gflnet2::boss::BossEventListener,
#endif // defined( GF_PLATFORM_CTR )
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GlobalAttractionOpenCheckRequestSequence );

public:

  GlobalAttractionOpenCheckRequestSequence();
  virtual~GlobalAttractionOpenCheckRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetAppLib::System::ApplicationWorkBase* pWork ){ m_pWork = pWork; }
  virtual void RemoveListener(){ m_pGlobalAttractionOpenCheckResponseListener = NULL; }
  void SetListener( GlobalAttractionOpenCheckResponseListener* pGlobalAttractionOpenCheckResponseListener ){ m_pGlobalAttractionOpenCheckResponseListener = pGlobalAttractionOpenCheckResponseListener; }

#if defined( GF_PLATFORM_CTR )

private:

  bool checkCrc( const size_t bodySize , const u8* pBody );

  // BossEventListener
  virtual void OnBossNewArrivalData( const u32 serialId , const bool isFinalData );
  virtual void OnBossNewArrivalDataEmpty( void );
  virtual bool OnBossReadData( const nn::boss::NsDataHeaderInfo* pHeader , const size_t bodySize , const u8* pBody );
  virtual void OnBossRecvFileList( const char* pTaskName , const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum );
  virtual void OnBossRecvFileListFailed( const char* pTaskName , const nn::Result& result );
  virtual void OnBossError( const nn::Result& result );
  virtual void OnBossTaskError( const char* pTaskName , const u32 errorCode );

  void initialize();
  void terminate();

  bool isOpenGlobalAttraction( const u8* pDownloadData, u32& index );
  bool isEntry( u32 index );

#endif // defined( GF_PLATFORM_CTR )

private:

  typedef struct
  {
    u16     crc;
    u16     padding;
    u32     version;
    u32     fileSize;
    u32     reserve;
  } Header;

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

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_START_DOWNLOAD_FILE,
    SEQUENCE_WAIT_DOWNLOAD_FILE,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_TERMINATE
  };

  enum CancelSequence
  {
    CANCEL_SEQUENCE_START,
    CANCEL_SEQUENCE_WAIT,
    CANCEL_SEQUENCE_NOTIFICATION_CANCEL,
    CANCEL_SEQUENCE_TERMINATE
  };

  NetAppLib::System::ApplicationWorkBase*         m_pWork;
  GlobalAttractionOpenCheckResponseListener*      m_pGlobalAttractionOpenCheckResponseListener;
  u32                                             m_CancelSequenceNo;
  u32                                             m_SerialID;
  bool                                            m_bIsGlobalAttractionOpenNotEntry;

};


GFL_NAMESPACE_END(GlobalAttraction)
GFL_NAMESPACE_END(NetAppLib)

#endif // GLOBAL_ATTRACTION_OPEN_CHECK_REQUEST_SEQUENCE_H_INCLUDE
#endif
// ============================================================================
/*
 * @file RegulationDLSearchDataRequestSequence.h
 * @brief レギュレーションデータを探す通信リクエストです。
 * @date 2015.06.30
 */
// ============================================================================
#if !defined( REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE )
#define REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/RegulationSelect/source/RequestSequence/RegulationDLSearchDataResponseListener.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"

#if defined( GF_PLATFORM_CTR )

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>

#endif // defined( GF_PLATFORM_CTR )


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(RequestSequence)

class RegulationDLSearchDataRequestSequence :
#if defined( GF_PLATFORM_CTR )
  public gflnet2::boss::BossEventListener,
#endif // defined( GF_PLATFORM_CTR )
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDLSearchDataRequestSequence );

public:

  RegulationDLSearchDataRequestSequence();
  virtual~RegulationDLSearchDataRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork ){ m_pRegulationSelectWork = pRegulationSelectWork; }
  virtual void RemoveListener(){ m_pRegulationDLSearchDataResponseListener = NULL; }
  void SetListener( RegulationDLSearchDataResponseListener* pRegulationDLSearchDataResponseListener ){ m_pRegulationDLSearchDataResponseListener = pRegulationDLSearchDataResponseListener; }

#if defined( GF_PLATFORM_CTR )

private:

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

#endif // defined( GF_PLATFORM_CTR )

private:

  enum Sequence
  {
    SEQUENCE_INITIALIZE,
    SEQUENCE_START_DOWNLOAD_FILE_LIST,
    SEQUENCE_WAIT_DOWNLOAD_FILE_LIST,
    SEQUENCE_NOTIFICATION_SUCCESS,
    SEQUENCE_NOTIFICATION_ERROR,
    SEQUENCE_NOTIFICATION_ERROR2,
    SEQUENCE_TERMINATE
  };

  enum CancelSequence
  {
    CANCEL_SEQUENCE_START,
    CANCEL_SEQUENCE_WAIT,
    CANCEL_SEQUENCE_NOTIFICATION_CANCEL,
    CANCEL_SEQUENCE_TERMINATE
  };


  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*    m_pRegulationSelectWork;
  RegulationDLSearchDataResponseListener*                               m_pRegulationDLSearchDataResponseListener;
  u32                                                                   m_CancelSequenceNo;
  u32                                                                   m_SerialID;

  u8                                                                    m_WorkBuffer[ Regulation::REGULATION_MAX_DATA_SIZE ];

};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE

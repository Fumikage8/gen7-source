// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDownloadRequestSequence.h
 * @brief レギュレーションデータを探す通信リクエストです。
 * @date 2016.02.29
 */
// ============================================================================
#if !defined( REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE )
#define REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionDownloadResponseListener.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"

#if defined( GF_PLATFORM_CTR )

#include <gflnet2/include/boss/gflnet2_BossEventListener.h>

#endif // defined( GF_PLATFORM_CTR )


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(RequestSequence)

class JoinFestaGlobalAttractionDownloadRequestSequence :
#if defined( GF_PLATFORM_CTR )
  public gflnet2::boss::BossEventListener,
#endif // defined( GF_PLATFORM_CTR )
  public gflnet2::ServerClient::RequestSequenceBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionDownloadRequestSequence );

public:

  JoinFestaGlobalAttractionDownloadRequestSequence();
  virtual~JoinFestaGlobalAttractionDownloadRequestSequence();

  // gflnet2::ServerClient::RequestSequenceBase
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update();
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel();
  void SetRequestParam( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork ){ m_pJoinFestaGlobalAttractionWork = pJoinFestaGlobalAttractionWork; }
  virtual void RemoveListener(){ m_pJoinFestaGlobalAttractionDownloadResponseListener = NULL; }
  void SetListener( JoinFestaGlobalAttractionDownloadResponseListener* pJoinFestaGlobalAttractionDownloadResponseListener ){ m_pJoinFestaGlobalAttractionDownloadResponseListener = pJoinFestaGlobalAttractionDownloadResponseListener; }

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

#endif // defined( GF_PLATFORM_CTR )

private:

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


  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*        m_pJoinFestaGlobalAttractionWork;
  JoinFestaGlobalAttractionDownloadResponseListener*                               m_pJoinFestaGlobalAttractionDownloadResponseListener;
  u32                                                                              m_CancelSequenceNo;
  u32                                                                              m_SerialID;


};


GFL_NAMESPACE_END(RequestSequence)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONDLSEARCHDATAREQUESTSEQUENCE_H_INCLUDE

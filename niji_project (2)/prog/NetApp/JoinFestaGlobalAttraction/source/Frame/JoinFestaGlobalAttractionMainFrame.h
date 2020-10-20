// ============================================================================
/*
 * @file JoinFestaGlobalAttractionMainFrame.h
 * @brief グローバルアトラクションアプリの○○プロセスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONMAIN_FRAME_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONMAIN_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaGlobalAttraction/source/Frame/JoinFestaGlobalAttractionFrameBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionMessageMenuLowerViewListener.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionDetailLowerViewListener.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionSelectListLowerViewListener.h"
#include "NetApp/JoinFestaGlobalAttraction/source/RequestSequence/JoinFestaGlobalAttractionDownloadResponseListener.h"
#include "NetStatic/NetAppLib/include/GameSync/GameSyncSaveDataSyncResponseListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaGlobalAttractionMainFrame :
  public NetApp::JoinFestaGlobalAttraction::Frame::JoinFestaGlobalAttractionFrameBase,
  public NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener,
  public NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerViewListener,
  public NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener,
  public NetApp::JoinFestaGlobalAttraction::RequestSequence::JoinFestaGlobalAttractionDownloadResponseListener,
  public NetAppLib::GameSync::GameSyncSaveDataSyncResponseListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionMainFrame );
public:
  JoinFestaGlobalAttractionMainFrame(
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork,
    NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionViewSystemAccessor* pJoinFestaGlobalAttractionViewSystemAccessor );
  virtual~JoinFestaGlobalAttractionMainFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  // JoinFestaGlobalAttractionDownloadResponseListener
  virtual void OnDownloadSuccess();
  virtual void OnDownloadError();
  virtual void OnDownloadCancel();

  // GameSyncSaveDataSyncResponseListener
  virtual void OnSaveDataSyncSuccess();
  virtual void OnSaveDataSyncError( u32 errorCode );

  void gameSyncRequest();
  void checkNotGetReward();
  bool isRegistEntry( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );
  void entryComfirmGlobalAttraction( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );
  void registCompleteGlobalAttraction( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );
  void notGetRewardMessage();
  void setupMainScreen();
  void setupDetailScreen( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );

private:

  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionMessageMenuLowerViewListener      m_JoinFestaGlobalAttractionMessageMenuLowerViewListener;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerViewListener           m_JoinFestaGlobalAttractionDetailLowerViewListener;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener       m_JoinFestaGlobalAttractionSelectListLowerViewListener;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo*     m_pEntryComfirmGlobalAttractionInfo;
  bool                                                                                                m_bDownloadError;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONMAIN_FRAME_H_INCLUDE

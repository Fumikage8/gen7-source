// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptWork.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のワーククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListSelectForScriptWork::JoinFestaPlayerListSelectForScriptWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_eBootMode( NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_DEFAULT ),
  m_JoinFestaPlayerListManager( GetDeviceHeap() ),
  m_pSelectPlayer( NULL ),
  m_pVipActivePlayerData( NULL ),
  m_pGuestActivePlayerData( NULL ),
  m_JoinFestaRecordListManager( GetDeviceHeap() ),
  m_SelectRecord(),
  m_JoinFestaSurveyListManager( GetDeviceHeap() ),
  m_ListSize( 0 ),
  m_bIsSelectCancel( true ),
  m_bShowVip( false ),
  /*  BFC */
  m_pBFCModeInfo(NULL),
  m_pCommonIconData(NULL)
{
}


JoinFestaPlayerListSelectForScriptWork::~JoinFestaPlayerListSelectForScriptWork()
{
}


void JoinFestaPlayerListSelectForScriptWork::ResetActivePlayerData()
{
  m_pVipActivePlayerData = NULL;
  m_pGuestActivePlayerData = NULL;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)

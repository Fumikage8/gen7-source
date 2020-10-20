// ============================================================================
/*
 * @file JoinFestaPlayerListShowWork.cpp
 * @brief 閲覧アプリのワーククラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


JoinFestaPlayerListShowWork::JoinFestaPlayerListShowWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_JoinFestaPlayerListManager( GetDeviceHeap() ),
  m_SelectPlayer(),
  m_JoinFestaRecordListManager( GetDeviceHeap() ),
  m_SelectRecord(),
  m_JoinFestaSurveyListManager( GetDeviceHeap() ),
  m_pVipActivePlayerData( NULL ),
  m_pGuestActivePlayerData( NULL ),
  m_pBlackActivePlayerData( NULL ),
  m_eShowType( NetApp::JoinFestaPlayerListShow::SHOW_TYPE_GUEST ),
  m_bDescriptionOpenAnimeFlag( true ),
  m_bUpdateNotificationFlag( true ),
  m_pAppParam(NULL)
{
}


JoinFestaPlayerListShowWork::~JoinFestaPlayerListShowWork()
{
}


void JoinFestaPlayerListShowWork::ResetActivePlayerData()
{
  m_pVipActivePlayerData = NULL;
  m_pGuestActivePlayerData = NULL;
  m_pBlackActivePlayerData = NULL;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file JoinFestaAttractionListSelectWork.cpp
 * @brief アトラクション開催参加アプリのワーククラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


JoinFestaAttractionListSelectWork::JoinFestaAttractionListSelectWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_JoinFestaAttractionListManager( GetDeviceHeap() ),
  m_pSelectAttraction( NULL ),
  m_pDescriptionAttraction( NULL ),
  m_pOpenActiveAttractionData( NULL ),
  m_pEntryActiveAttractionData( NULL ),
  m_eDescriptionBootFrameID( NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN )
{
}


JoinFestaAttractionListSelectWork::~JoinFestaAttractionListSelectWork()
{
}


void JoinFestaAttractionListSelectWork::ResetActiveAttractionData()
{
  m_pOpenActiveAttractionData = NULL;
  m_pEntryActiveAttractionData = NULL;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

// ============================================================================
/*
 * @file RegulationSelectFrameFactory.cpp
 * @brief レギュレーション選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectFrameFactory.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameID.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectLoadFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationSelectMainFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationDLLoadFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationDLMainFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationDLSelectFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationDLDeleteFrame.h"
#include "NetApp/RegulationSelect/source/Frame/RegulationDetailFrame.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


RegulationSelectFrameFactory::RegulationSelectFrameFactory( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects* pRegulationSelectViewObjects ) :
  m_pRegulationSelectWork( pRegulationSelectWork ),
  m_pRegulationSelectViewObjects( pRegulationSelectViewObjects )
{
}


RegulationSelectFrameFactory::~RegulationSelectFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* RegulationSelectFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_LOAD:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationSelectLoadFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationSelectMainFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_LOAD:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationDLLoadFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_MAIN:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationDLMainFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_SELECT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationDLSelectFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_DELETE:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationDLDeleteFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;

    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::Frame::RegulationDetailFrame( m_pRegulationSelectWork, m_pRegulationSelectViewObjects );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

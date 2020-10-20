// ============================================================================
/*
 * @file RegulationSelectWork.cpp
 * @brief レギュレーション選択アプリのワーククラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


RegulationSelectWork::RegulationSelectWork() :
  ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE ),
  m_pRootUpperView( NULL ),
  m_pRootLowerView( NULL ),
  m_eBootMode( NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P ),
  m_eBattleRule( Regulation::BATTLE_RULE_SINGLE ),
  m_eDetailBootFrame( NetApp::RegulationSelect::DETAIL_BOOT_FRAME_SELECT_MAIN ),
  m_PresetList(),
  m_DownloadList(),
  m_DownloadedList(),
  m_pSelectRegulationDrawInfo( NULL ),
  m_pActiveSelectRegulationDrawInfo( NULL ),
  m_pActiveDownloadRegulationDrawInfo( NULL ),
  m_pActiveDeleteRegulationDrawInfo( NULL ),
  m_bFatalErrorFlag( false ),
  m_bSingleNormalClearFlag( false ),
  m_bDoubleNormalClearFlag( false ),
  m_bMultiNormalClearFlag( false ),
  m_bBattleRoyalNormalClearFlag( false ),
  m_bBattleRoyalSuperClearFlag( false ),
  m_bBattleRoyalHyperClearFlag( false ),
  m_bQRPartyFlag( false )
{
  app::util::Heap* pAppHeap = GetAppHeap();
  m_PresetList.CreateBuffer( pAppHeap->GetDeviceHeap(), NetApp::RegulationSelect::PRESET_LIST_MAX );
  m_DownloadList.CreateBuffer( pAppHeap->GetDeviceHeap(), NetApp::RegulationSelect::DOWNLOAD_LIST_MAX );
  m_DownloadedList.CreateBuffer( pAppHeap->GetDeviceHeap(), NetApp::RegulationSelect::DOWNLOADED_LIST_MAX );
}


RegulationSelectWork::~RegulationSelectWork()
{
  ClearList( &m_PresetList );
  ClearList( &m_DownloadList );
  ClearList( &m_DownloadedList );
}


void RegulationSelectWork::ClearList( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList )
{
  if( pRegulationList )
  {
    for( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>::Iterator it = pRegulationList->Begin(); it != pRegulationList->End(); ++it )
    {
      GFL_SAFE_DELETE( *it );
    }
    
    pRegulationList->Erase();
  }
}


void RegulationSelectWork::AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const Regulation::PRESET presetRegulation, NetAppLib::UI::RegulationDrawInfo::Rank rank )
{
  if( pRegulationList )
  {
    app::util::Heap* pAppHeap = GetAppHeap();
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::RegulationDrawInfo( pAppHeap->GetDeviceHeap(), presetRegulation, rank );
    pRegulationList->PushBack( pRegulationDrawInfo );
  }
}


#if PM_DEBUG
void RegulationSelectWork::AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const Regulation::PRESET presetRegulation, NetAppLib::UI::RegulationDrawInfo::Rank rank, bool preset )
{
  if( pRegulationList )
  {
    app::util::Heap* pAppHeap = GetAppHeap();
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::RegulationDrawInfo( pAppHeap->GetDeviceHeap(), presetRegulation, rank, preset );
    pRegulationList->PushBack( pRegulationDrawInfo );
  }
}
#endif // PM_DEBUG


void RegulationSelectWork::AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const void* pBuffer, const u32 size )
{
  if( pRegulationList )
  {
    app::util::Heap* pAppHeap = GetAppHeap();
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::RegulationDrawInfo( pAppHeap->GetDeviceHeap(), pBuffer, size );
    pRegulationList->PushBack( pRegulationDrawInfo );
  }
}


void RegulationSelectWork::AddRegulation( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, const void* pBuffer, const u32 size, int fileIndex )
{
  if( pRegulationList )
  {
    app::util::Heap* pAppHeap = GetAppHeap();
    NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::RegulationDrawInfo( pAppHeap->GetDeviceHeap(), pBuffer, size, fileIndex );
    pRegulationList->PushBack( pRegulationDrawInfo );
  }
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

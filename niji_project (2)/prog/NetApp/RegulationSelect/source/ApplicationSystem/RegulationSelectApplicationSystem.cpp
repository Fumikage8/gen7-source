// ============================================================================
/*
 * @file RegulationSelectApplicationSystem.cpp
 * @brief レギュレーション選択アプリのアプリケーションシステムです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectApplicationSystem.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectViewObjects.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectBuilder.h"
#include "NetApp/RegulationSelect/source/RegulationSelectResourceID.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


#include <arc_def_index/arc_def.h>

#include <arc_index/message.gaix>
#include <arc_index/RegulationSelect.gaix>
#include <arc_index/MenuWindow.gaix>
#include <arc_index/MenuCursor.gaix>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


RegulationSelectApplicationSystem::RegulationSelectApplicationSystem() :
  m_pRegulationSelectWork( NULL ),
  m_pRegulationSelectViewObjects( NULL ),
  m_pRootUpperView( NULL ),
  m_pRootLowerView( NULL )
{
}


RegulationSelectApplicationSystem::~RegulationSelectApplicationSystem()
{
  if( m_pRegulationSelectViewObjects )
  {
    m_pRegulationSelectViewObjects->RemoveFromSuperViewAll();
  }

  m_pRootLowerView->RemoveAllSubView();
  m_pRootUpperView->RemoveAllSubView();

  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );

  GFL_SAFE_DELETE( m_pRegulationSelectViewObjects );
}


void RegulationSelectApplicationSystem::Build( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork, NetApp::RegulationSelect::ApplicationSystem::RegulationSelectBuilder* pApplicationSystemBuilder )
{
  m_pRegulationSelectWork = pRegulationSelectWork;

  app::util::Heap* pAppHeap = m_pRegulationSelectWork->GetAppHeap();

  m_pRegulationSelectViewObjects = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::RegulationSelect::ApplicationSystem::RegulationSelectViewObjects();
  pApplicationSystemBuilder->SetDrawManager( m_pRegulationSelectViewObjects );

  m_pRootUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );
  m_pRootLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );

  m_pRegulationSelectWork->SetRootUpperView( m_pRootUpperView );
  m_pRegulationSelectWork->SetRootLowerView( m_pRootLowerView );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void RegulationSelectApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_LAYOUT, ARCID_REGULATION_SELECT, GARC_RegulationSelect_RegulationSelect_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_MESSAGE_REGULATION, print::GetMessageArcId(), GARC_message_regulation_DAT, false, true );

  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pPresetList = m_pRegulationSelectWork->GetPresetList();
  m_pRegulationSelectWork->ClearList( pPresetList );


  switch( m_pRegulationSelectWork->GetBootMode() )
  {
    case NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P:
    {
      Regulation::BATTLE_RULE eBattleRule = m_pRegulationSelectWork->GetBattleRule();

      switch( eBattleRule ){
      case Regulation::BATTLE_RULE_SINGLE:       //! シングル
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_SINGLE_NORMAL,    NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_SINGLE_FLAT,      NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_SINGLE_UNLIMITED, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        break;
      case Regulation::BATTLE_RULE_DOUBLE:       //! ダブル
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_DOUBLE_NORMAL,    NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_DOUBLE_FLAT,      NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_DOUBLE_UNLIMITED, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        break;
      case Regulation::BATTLE_RULE_ROYAL:        //! ロイヤル
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_NORMAL,    NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_FLAT,      NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_UNLIMITED, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        break;
      case Regulation::BATTLE_RULE_MULTI:        //! マルチ
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_MULTI_NORMAL,    NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_MULTI_FLAT,      NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_MULTI_UNLIMITED, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
        break;
      }
    }
    break;

    case NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_CENTER:
    {
      m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_NORMAL_S, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
      m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_NORMAL_D, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
      m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_NORMAL_M, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );
  
      if( m_pRegulationSelectWork->GetSingleNormalClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_SUPER_S, NetAppLib::UI::RegulationDrawInfo::RANK_SUPER );
      }

      if( m_pRegulationSelectWork->GetDoubleNormalClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_SUPER_D, NetAppLib::UI::RegulationDrawInfo::RANK_SUPER );
      }

      if( m_pRegulationSelectWork->GetMultiNormalClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_BATTLE_TREE_SUPER_M, NetAppLib::UI::RegulationDrawInfo::RANK_SUPER );
      }
    }
    break;

    case NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_ROYAL:
    {
      m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_FACILITY_NORMAL, NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL );

      if( m_pRegulationSelectWork->GetBattleRoyalNormalClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_FACILITY_SUPER,  NetAppLib::UI::RegulationDrawInfo::RANK_SUPER );
      }

      if( m_pRegulationSelectWork->GetBattleRoyalSuperClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_FACILITY_HYPER,  NetAppLib::UI::RegulationDrawInfo::RANK_HYPER );
      }

      if( m_pRegulationSelectWork->GetBattleRoyalHyperClearFlag() )
      {
        m_pRegulationSelectWork->AddRegulation( pPresetList, Regulation::PRESET_ROYAL_FACILITY_MASTER, NetAppLib::UI::RegulationDrawInfo::RANK_MASTER );
      }
    }
    break;
  }
}


void RegulationSelectApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_regulation_DAT, m_pResourceManager->GetResourceBuffer(  NetApp::RegulationSelect::REGULATIONSELECT_RESOURCE_ID_MESSAGE_REGULATION ) );
  m_pRegulationSelectWork->SetMessageUtility( m_pMessageUtility );
}


void RegulationSelectApplicationSystem::CreateView()
{
  m_pRegulationSelectViewObjects->CreateDraw( m_pRegulationSelectWork, *m_pNetAppMessageMenu );
}


void RegulationSelectApplicationSystem::UpdateView()
{
  m_pRootUpperView->UpdateTree();
  m_pRootLowerView->UpdateTree();
}


void RegulationSelectApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}


bool RegulationSelectApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pRegulationSelectViewObjects )
  {
    if( m_pRegulationSelectViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  return bExit;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

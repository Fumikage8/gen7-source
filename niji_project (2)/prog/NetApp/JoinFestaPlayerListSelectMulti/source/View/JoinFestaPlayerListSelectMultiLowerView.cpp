// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiLowerView.cpp
 * @brief プレイヤー複数選択アプリの下画面を表示するクラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectMulti/source/View/JoinFestaPlayerListSelectMultiLowerView.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/JoinFestaPlayerListSelectMultiResourceID.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(View)

JoinFestaPlayerListSelectMultiLowerView::JoinFestaPlayerListSelectMultiLowerView( NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork ) :
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase( pJoinFestaPlayerListSelectMultiWork, NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_LAYOUT, NetApp::JoinFestaPlayerListSelectMulti::JOINFESTAPLAYERLISTSELECTMULTI_RESOURCE_ID_MENU_CURSOR ),
  m_pJoinFestaPlayerListSelectMultiWork( pJoinFestaPlayerListSelectMultiWork )
{
}


JoinFestaPlayerListSelectMultiLowerView::~JoinFestaPlayerListSelectMultiLowerView()
{
}


void JoinFestaPlayerListSelectMultiLowerView::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
  {
    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData = getJoinFestaPlayerData( pos );

    if( pJoinFestaPlayerData )
    {
      bool bOnOff = false;

      if( pJoinFestaPlayerData->IsSelect() )
      {
        pJoinFestaPlayerData->SetSelect( false );
        bOnOff = true;
      }

      if( !bOnOff )
      {
        NetAppLib::JoinFestaUI::JoinFestPlayerDataList& selectPlayerList = m_pJoinFestaPlayerListSelectMultiWork->GetSelectPlayerList();
        if( selectPlayerList.Size() < selectPlayerList.MaxSize() )
        {
          if( pJoinFestaPlayerData->IsSelect() )
          {
            pJoinFestaPlayerData->SetSelect( false );
          }
          else
          {
            if( pJoinFestaPlayerData->IsOnline() )
            {
              pJoinFestaPlayerData->SetSelect( true );
            }
          }
          bOnOff = true;
        }
      }

      if( bOnOff )
      {
        if( m_pPaneListView )
        {
          app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
          if( pPaneList )
          {
            u32 cursorPos = 0;
            pPaneList->GetCursorData( &cursorPos, NULL );
            u32 paneIndex = pPaneList->GetPosPaneIndex( cursorPos );
            if( paneIndex < LIST_ITEM_PANE_MAX )
            {
              m_pMyPaneListData[ paneIndex ].pCheckMark->SetVisible( pJoinFestaPlayerData->IsSelect() );
            }
          }
        }
      }
    }
  }

  JoinFestaPlayerListSelectLowerViewBase::PaneListItemSelectListener( pos );
}


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

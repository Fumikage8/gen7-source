//==============================================================================
/**
 * @file   BattleSpotApplicationSystem.cpp
 * @date   2015/10/02 Fri. 12:32:51
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./BattleSpotWork.h"
#include  "./BattleSpotApplicationSystem.h"
#include  "../BattleSpotResource.h"
#include  "../Views/BattleSpotViewContainer.h"
#include  "GameSys/include/GameData.h"
#if PM_DEBUG
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <debug/include/gfl2_DebugWinSystem.h>
// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "NetStatic/NetLib/include/Pgl/PGLConnector.h"
#endif

namespace NetApp      {
namespace BattleSpot  {


#if PM_DEBUG
static void BattleSpotClearCacheUpdateFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "NetLib::NijiNetworkSystem::SetClearCacheFlag( true );\n" );
    NetLib::NijiNetworkSystem::SetClearCacheFlag( true );
  }
}

static void BattleSpotClearCupWorkUpdateFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "BattleSpotClearCupWorkUpdateFunc\n" );
    
    BattleSpotWork* work = (BattleSpotWork*)userWork;

    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
    u8 lockIndex = pGameData->GetBattleSpotData()->GetInternetLockTeamIndex();
    if( lockIndex != 0xFF )
    {
      pGameData->GetBOX()->SetTeamLock( lockIndex , false );
    }
    lockIndex = pGameData->GetBattleSpotData()->GetFriendLockTeamIndex();
    if( lockIndex != 0xFF )
    {
      pGameData->GetBOX()->SetTeamLock( lockIndex , false );
    }

    work->m_pBattleSpotSave->SetFriendEntryWork( 0 );
    work->m_pBattleSpotSave->SetInternetEntryWork( 0 );
    work->m_pBattleSpotSave->SetFriendLockTeamIndex( 0xFF );
    work->m_pBattleSpotSave->SetInternetLockTeamIndex( 0xFF );

    work->m_pBattleSpotSave->SetInternetMatchCount( 0 );
    work->m_pBattleSpotSave->SetFriendMatchCount( 0 );
  }
}



static void BattleSpotClearRatingPIDUpdateFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "BattleSpotClearRatingPIDUpdateFunc\n" );

    BattleSpotWork* work = (BattleSpotWork*)userWork;
    Savedata::BattleSpotData::ENTRY_COMMUNITY com;
    std::memset( &com , 0 , sizeof(com) );

    for( u32 index = 0 ; index < 5 ; ++index )
    {
      com.community_id = work->m_pBattleSpotSave->GetEntryCommunityId( (Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE)index );
      work->m_pBattleSpotSave->SetEntryCommunity( com , (Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE)index );
    }

    work->m_pBattleSpotSave->SetFriendMatchPID(0);
    
  }
}

static void BattleSpotClearRatingCommunityIDUpdateFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "BattleSpotClearRatingPIDUpdateFunc\n" );

    BattleSpotWork* work = (BattleSpotWork*)userWork;
    Savedata::BattleSpotData::ENTRY_COMMUNITY com;
    std::memset( &com , 0 , sizeof(com) );

    for( u32 index = 0 ; index < 5 ; ++index )
    {
      com.principalId = work->m_pBattleSpotSave->GetEntryCommunityPid( (Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE)index );
      work->m_pBattleSpotSave->SetEntryCommunity( com , (Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE)index );
    }

    work->m_pBattleSpotSave->SetFriendMatchCommunityID(0);
  }
}

#endif // #if PM_DEBUG

// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::ApplicationSystem(BattleSpotWork* pWork)
  : NetAppLib::System::ApplicationSystemBase()
  , m_pWork(pWork)
  , m_pViewContainer(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
ApplicationSystem::~ApplicationSystem()
{
  GFL_SAFE_DELETE(m_pViewContainer);

#if PM_DEBUG
  gfl2::debug::DebugWin_RemoveGroupByName( L"BattleSpot" );
#endif // #if PM_DEBUG
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void ApplicationSystem::Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder )
{
#if PM_DEBUG
  //::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  //gfl2::str::MsgData* pMsgData = devWork->GetMessageData();
  gfl2::debug::DebugWinGroup* pGroup = gfl2::debug::DebugWin_AddGroup( m_pWork->GetDeviceHeap(), L"BattleSpot" );  
  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, m_pWork->GetDeviceHeap(), L"ClearCache", NULL, BattleSpotClearCacheUpdateFunc );    // "ClearCache"

  gfl2::debug::DebugWin_AddItemValueU32(pGroup, m_pWork->GetDeviceHeap(), L"ForcedLongRegionName", &(m_pWork->m_IsDebugForcedRegionName), 0, 1);

  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, m_pWork->GetDeviceHeap(), L"ClearCupWork", m_pWork, BattleSpotClearCupWorkUpdateFunc );

  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, m_pWork->GetDeviceHeap(), L"ClearRegistPID", m_pWork, BattleSpotClearRatingPIDUpdateFunc );    

  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, m_pWork->GetDeviceHeap(), L"ClearRegistCommunityID", m_pWork, BattleSpotClearRatingCommunityIDUpdateFunc );

  gfl2::debug::DebugWin_AddItemValueU8(pGroup, m_pWork->GetDeviceHeap(), L"InternetCupBattleCount", &m_pWork->m_pBattleSpotSave->m_BattleSpotSaveData.net_match_count, 0, 255,1);
  gfl2::debug::DebugWin_AddItemValueU8(pGroup, m_pWork->GetDeviceHeap(), L"FriendCupBattleCount", &m_pWork->m_pBattleSpotSave->m_BattleSpotSaveData.friend_match_count, 0, 255,1);
#ifdef  GF_PLATFORM_CTR
  gfl2::debug::DebugWin_AddItemValueU32(pGroup, m_pWork->GetDeviceHeap(), L"DebugBattleType", NetLib::Pgl::PGLConnector::GetDebugBattleType(), 0, 1 , 1);
#endif
  gfl2::debug::DebugWin_AddItemValueBool(pGroup, m_pWork->GetDeviceHeap(), L"ForceWinReport", &m_pWork->m_pAppParam->m_IsDebugForcedWinReport );
  
#endif // #if PM_DEBUG

  /*  super  */
  NetAppLib::System::ApplicationSystemBase::Build(pApplicationSystemBuilder);

  m_pWork->m_pTrainerIconUtility  = m_pNetAppTrainerIconUtility;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::LoadResourceRequest()
{
  NetAppLib::System::ResourceManager*   pResourceManager  = m_pWork->GetResourceManager();


  /*
    resourceID
  
    void AddLoadRequest(
          NetAppLib::System::ResourceID resourceID,
          gfl2::fs::ArcFile::ARCID                    loadArcID,
          gfl2::fs::ArcFile::ARCDATID                 loadDataID,
          bool                                        compressed,
          bool                                        useDeviceHeap
    );
  */
  pResourceManager->AddLoadRequest(RESOURCE_Message,                print::GetMessageArcId(), BATTLESPOT_MESSAGE_ARCDAT,                  false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_Message_BGMList,        print::GetMessageArcId(), BATTLESPOT_MESSAGE_BGMLIST_ARCDAT,          false,  true);
  pResourceManager->AddLoadRequest(RESOURCE_BG,                     ARCID_BATTLE_SPOT,        GARC_BattleSpot_Live_BGEmit_000_bptcl_COMP, true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_Layout,                 ARCID_BATTLE_SPOT,        GARC_BattleSpot_BattleSpot_applyt_COMP,     true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonWindow,     ARCID_MENU_WINDOW,        GARC_MenuWindow_MenuWindow_applyt_COMP,     true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutMenuWindow2,      ARCID_MENU_WINDOW2,       GARC_MenuWindow2_MenuWindow2_applyt_COMP,   true,   true);
  pResourceManager->AddLoadRequest(RESOURCE_LayoutCommonMenuCursor, ARCID_MENU_CURSOR,        GARC_MenuCursor_MenuCursor_applyt_COMP,     true,   true);

  /*  font  */
  pResourceManager->FontLoadRequest(NetAppLib::System::ResourceManager::FONT_NUMBER);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateMessageData()
{
  m_pWork->GetMessageUtility()->LoadFromMemory(BATTLESPOT_MESSAGE_ARCDAT,         m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message));
  m_pWork->GetMessageUtility()->LoadFromMemory(BATTLESPOT_MESSAGE_BGMLIST_ARCDAT, m_pWork->GetResourceManager()->GetResourceBuffer(RESOURCE_Message_BGMList));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  ApplicationSystem::CreateView()
{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();
  
  m_pViewContainer          = GFL_NEW(pHeapBase) BattleSpotViewContainer(m_pWork);
  m_pWork->m_pViewContainer = m_pViewContainer;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ApplicationSystem::UpdateView()
{
  m_pViewContainer->Update();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void ApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pViewContainer->Draw(displayNo);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool ApplicationSystem::IsExitView()
{
  bool  isStable  = true;

  /*  views  */
  if(m_pViewContainer->IsDrawingAny())
  {
    isStable  = false;
  }

  return isStable;
}

} /*  namespace BattleSpot  */
} /*  namespace NetApp      */


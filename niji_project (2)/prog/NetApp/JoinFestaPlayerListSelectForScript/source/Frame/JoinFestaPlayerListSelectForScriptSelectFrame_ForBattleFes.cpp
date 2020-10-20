//==============================================================================
/**
 * @file   JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes.cpp
 * @date   2016/12/06 Tue. 16:52:20
 * @author muto_yu
 * @brief  BattleFes用のTrainerSelect [0,2]のPlayerDataを返却する
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameResult.h>

#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListForBattleFesUpperView.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h>
#include <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h>

#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h>

#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

#include <niji_conv_header/message/msg_jf_playerlist.h>

#include "./JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes.h"


namespace NetApp {
namespace JoinFestaPlayerListSelectForScript {
namespace Frame {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes
namespace FILELOCAL {
  const u32   POKEICON_SYSHEAP_BYTES  = 0x00080000;
  const u32   POKEICON_DEVHEAP_BYTES  = 0x00180000;

  enum StartupState
  {
    STARTUP_Initial,
    STARTUP_LoadingPokeIcons,
    STARTUP_End,
    
    STARTUP_
  };
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes(
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork* pJoinFestaPlayerListSelectForScriptWork,
  NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListSelectForScript::Frame::JoinFestaPlayerListSelectForScriptFrameBase( pJoinFestaPlayerListSelectForScriptWork, pJoinFestaPlayerListSelectForScriptViewSystemAccessor ),
  m_JoinFestaPlayerListSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_bWaitForStatusEvent(false),
  m_IconHeap(),
  m_StartupState(FILELOCAL::STARTUP_Initial),
  m_PokeparaItems(0)
{
  gfl2::heap::HeapBase*   pHeapBase = m_pJoinFestaPlayerListSelectForScriptWork->GetDeviceHeap();
  
  m_IconHeap.LocalHeapCreate( pHeapBase, pHeapBase, FILELOCAL::POKEICON_SYSHEAP_BYTES, FILELOCAL::POKEICON_DEVHEAP_BYTES);

  for(u32 index = 0; index < GFL_NELEMS(m_pCoreParams); ++index)
  {
    m_pCoreParams[index]  = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::~JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes()
{
  ReleasePokepara();
  m_IconHeap.Delete();
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
//------------------------------------------------------------------
/**
  *  @brief    startup
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::startup()
{
  switch(m_StartupState)
  {
    case FILELOCAL::STARTUP_Initial:
    {
      NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaPlayerListManager();

      u32 listSize = 0;

      if( m_pJoinFestaPlayerListSelectForScriptWork->GetBootMode() == NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_LIMIT )
      {
        listSize = m_pJoinFestaPlayerListSelectForScriptWork->GetListSize();
      }

      joinFestaPlayerListManager.UpdateListMode4And5( true, listSize );

      m_pJoinFestaPlayerListSelectForScriptViewObjects->InputDisableAll();
      m_pJoinFestaPlayerListSelectForScriptViewList->RemoveAllView();

      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );
      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleDecideButton( true );
      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleMenuCursor( true );

      {
        m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleCheckBox( m_pJoinFestaPlayerListSelectForScriptWork->GetBootMode() == BOOT_MODE_BattleFesCircle );
        m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetPassiveDecideButton();
      }

      /*  add upper/lower  */
      m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGUpperView() );
      m_pJoinFestaPlayerListSelectForScriptViewList->AddUpperView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListForBattleFesUpperView() );
      m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListBGLowerView() );
      m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerView( m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase() );

      /*  BFC用Pokeparaを生成可能であるかどうかでActive状態を変更し、リストをソート */
      {
        NetAppLib::JoinFestaUI::JoinFestPlayerDataList* const listTable[] =
        {
          &(joinFestaPlayerListManager.GetVipList()),
          &(joinFestaPlayerListManager.GetGuestList())
        };

        for(u32 index = 0; index < GFL_NELEMS(listTable); ++index)
        {
          NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itrEnd  = listTable[index]->End();
          
          for(NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itr =  listTable[index]->Begin(); itr != itrEnd; ++itr)
          {
            const pml::pokepara::CoreParam* pCoreParam          = CreatePokeparaFromPersonalData(*itr);
            const bool                      bAvailablePokepara  = (pCoreParam != NULL);
            const bool                      bRecentPartner      = BattleFesTool::IsRecentPartner(*itr);
            const bool                      bAvailablePartner   = (bAvailablePokepara && bRecentPartner);
            
            itr->BFC_SetPokepara(pCoreParam);
            itr->BFC_SetRecentPartner(bRecentPartner);
            itr->SetPassive(!bAvailablePartner);
          }
        }
        
        joinFestaPlayerListManager.SortList(true);
        m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->ShowBFCGrade(true);    /*  グレード表示有効  */
        m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetupPokeIcon(this, &m_IconHeap);
      }


      m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( NULL );

      m_StartupState = FILELOCAL::STARTUP_LoadingPokeIcons;
    }
      break;


    case  FILELOCAL::STARTUP_LoadingPokeIcons:
    {
      if(!m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->IsPokeIconLoading())
      {
        setupList();
        m_StartupState = FILELOCAL::STARTUP_End;
      }
    }
      break;
      
    case  FILELOCAL::STARTUP_End:
    default:
      /*  nop  */
      break;
  }


  return (m_StartupState == FILELOCAL::STARTUP_End);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::cleanup()
{
  return m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->CleanupPokeIcon();
}


//------------------------------------------------------------------
/**
  *  @brief    setupList
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaPlayerListManager();

  bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();

  if( bShowVip )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& vipList = joinFestaPlayerListManager.GetVipList();

    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &vipList, jf_playerlist_cap_13, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData = m_pJoinFestaPlayerListSelectForScriptWork->GetVipActivePlayerData();

    if( pVipActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin();
      pVipActivePlayerData = ( vipList.Size() > 0 ) ? &(*it) : NULL;
    }
//    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListForBattleFesUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    setActivePlayerData( pVipActivePlayerData );
  }
  else
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& guestList = joinFestaPlayerListManager.GetGuestList();

    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &guestList, jf_playerlist_cap_12, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData = m_pJoinFestaPlayerListSelectForScriptWork->GetGuestActivePlayerData();

    if( pGuestActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin();
      pGuestActivePlayerData = ( guestList.Size() > 0 ) ? &(*it) : NULL;
    }
//    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListForBattleFesUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    setActivePlayerData( pGuestActivePlayerData );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    setActivePlayerData
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData )
{
  bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();

  if( bShowVip )
  {
    m_pJoinFestaPlayerListSelectForScriptWork->SetVipActivePlayerData( pActivePlayerData );
  }
  else
  {
    m_pJoinFestaPlayerListSelectForScriptWork->SetGuestActivePlayerData( pActivePlayerData );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::setListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetListener( &m_JoinFestaPlayerListSelectLowerViewListener );
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetInputEnabled( true );

  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::removeListener()
{
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RemoveListener();
  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();
}


//------------------------------------------------------------------
/**
  *  @brief    更新
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::updateSequence()
{
  /*
    status復帰待ち
  */
  if(m_bWaitForStatusEvent)
  {
    if(!m_pJoinFestaPlayerListSelectForScriptWork->GetBFCModeInfo()->IsStatusTargetAvailable())
    {
      m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();   /*  messageViewをはずす  */
      m_bWaitForStatusEvent = false;
    }
    
    if(m_bWaitForStatusEvent) return;
  }


  /*
    UI
  */
  {
    NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerView*  pMessageMenuView  = m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListMessageMenuLowerView();

    u32  inputEventID         = m_JoinFestaPlayerListSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bPushLButton         = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton         = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectPlayer        = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PLAYER ) ? true : false;
    bool bSelectPassivePlayer = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PASSIVE_PLAYER ) ? true : false;
    bool bTouchDecideButton   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_DECIDE_BUTTON ) ? true : false;
    bool bIsUpdateUpperView   = m_JoinFestaPlayerListSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
//      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
      m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListForBattleFesUpperView()->SetJoinFestaPlayerData(m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer());

      /**
        @fix  MMcat[525]   【フェスサークル】バトルエージェントの仲間選択時に特定操作を行うと、上画面の表示とカーソル位置が合わなくなる
          上画面更新処理を追加
      */
      setActivePlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
    }

    if( bTouchBackButton )
    {
      /*  back  */
      m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( NULL );

      m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( true );
      m_pJoinFestaPlayerListSelectForScriptWork->GetBFCModeInfo()->Clear();
      
      exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_BACK );
    }
    else
    if( bPushRButton || bPushLButton )
    {
      /*  L/R  */
      bool bShowVip = m_pJoinFestaPlayerListSelectForScriptWork->GetShowVip();
      bShowVip = ( bShowVip ) ? false : true;
      m_pJoinFestaPlayerListSelectForScriptWork->SetShowVip( bShowVip );
      setupList();
    }
    else
    if( bSelectPlayer )
    {
      /*  選択  */
      NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

      if( pSelectPlayer )
      {
        BFCModeInfo*  pInfo = m_pJoinFestaPlayerListSelectForScriptWork->GetBFCModeInfo();

        /*  どうしますか  */
        m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( pMessageMenuView );
        
        /*  規定の人数を選択している状態の場合、追加する選択肢は表示させない  */
        if(!pInfo->IsFull() || pSelectPlayer->IsSelect())
        {
          pMessageMenuView->SetMessageMenuBlackFilter(
            jf_playerlist_win_08,
            pSelectPlayer->GetName(),
            pSelectPlayer->IsSelect() ? jf_playerlist_sel_15 : jf_playerlist_sel_08, /*  選ぶ/はずす     */
            jf_playerlist_sel_14, /*  ポケモンをみる  */
            jf_playerlist_sel_05  /*  やめる          */
          );
        }
        else
        {
          pMessageMenuView->SetMessageMenuBlackFilter(
            jf_playerlist_win_08,
            pSelectPlayer->GetName(),
            jf_playerlist_sel_14, /*  ポケモンをみる  */
            jf_playerlist_sel_05  /*  やめる          */
          );
        }
      }
    }
    else if(bSelectPassivePlayer)
    {
      /*  selected passive player  */
      NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

      if( pSelectPlayer )
      {
        /*  pokeparaがあるのにpassiveである == 最近来ていない  */
        if(pSelectPlayer->BFC_GetPokeparaAvailability())
        {
          const u32 messageID =  jf_playerlist_win_40 /* 最近来ていない */;

          m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( pMessageMenuView );
          pMessageMenuView->SetStreamMessageBlackFilter(messageID);
        }
      }
    }
    else if(bTouchDecideButton)
    {
      /*  決定  */
      m_pJoinFestaPlayerListSelectForScriptViewList->AddLowerViewInputEnable( pMessageMenuView );
      pMessageMenuView->SetMessageYesNoBlackFilter(jf_playerlist_win_38);   /*  決定しますか？  */
    }

    m_JoinFestaPlayerListSelectLowerViewListener.ResetInputEvent();
  }

  /*
    message
  */
  {
    u32 messageID         = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetMessageID();
    u32 selectMenuID      = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetSelectMenuID();
    u32 inputEventID      = m_JoinFestaPlayerListMessageMenuLowerViewListener.GetInputEventID();
    bool bTouchScreen     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN ) ? true : false;
    bool bTouchYesButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_YES_BUTTON ) ? true : false;
    bool bTouchNoButton   = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_NO_BUTTON ) ? true : false;
    bool bSelectMenu      = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener::INPUT_EVENT_ID_SELECT_MENU ) ? true : false;

    switch( messageID )
    {
      // ---------------------------------------------------------------------------------------------
      case jf_playerlist_win_08:
      {
        if( bSelectMenu )
        {
          switch( selectMenuID )
          {
            /*  えらぶ/外す  */
            case jf_playerlist_sel_08:
            case jf_playerlist_sel_15:
            {
              NetAppLib::JoinFestaUI::JoinFestaPlayerData*  pPlayerData = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();
              bool  isEnd = true;
              
              m_pJoinFestaPlayerListSelectForScriptWork->SetSelectPlayer( pPlayerData );

              /*  for BFC  */
              if(m_pJoinFestaPlayerListSelectForScriptWork->GetBootMode() == BOOT_MODE_BattleFesCircle)
              {
                BFCModeInfo*  pInfo = m_pJoinFestaPlayerListSelectForScriptWork->GetBFCModeInfo();
                /*  チェック状態  */
                {
                  const bool isSelected = BFCModeInfo::IsValidIndex(pInfo->ToggleInsert(*pPlayerData));
                  
                  pPlayerData->SetSelect(isSelected);
                  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RefreshCheckBox();
                  
                  /*  決定ボタン  */
                  if( pInfo->dataNum )  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetActiveDecideButton();
                  else                  m_pJoinFestaPlayerListSelectForScriptViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetPassiveDecideButton();
                }
                
                /*  規定の人数を選択し、即終了するならtrue  */
                isEnd = false;
//                isEnd = pInfo->IsFull();
              }

              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();

              if(isEnd)
              {
                m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( false );

                exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_SELECT_PLAYER );
              }
            }
            break;

            /*  ポケモンをみる  */
            case  jf_playerlist_sel_14:
            {
              m_pJoinFestaPlayerListSelectForScriptWork->GetBFCModeInfo()->SetStatusTarget(*(m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer()));
              m_bWaitForStatusEvent = true;  /**<  復帰待ち  */
              /*  ここではRemoveLowerViewせず、StatusEventからの復帰を待ってから行う  */
//              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
            }
            break;
            
            /*  やめる  */
            case jf_playerlist_sel_05:
            {
              m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
            }
            break;
          }
        }
      }
      break;

      case  jf_playerlist_win_39: /*  まだ遊んでいない  */
      case  jf_playerlist_win_40: /*  最近来ていない    */
      {
        if(bTouchScreen)
        {
          m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
        }
      }
      break;
      
      /*  決定しますか？  */
      case  jf_playerlist_win_38:
      {
        if(bTouchYesButton)
        {
          /*  yes: 終了  */
          m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
          m_pJoinFestaPlayerListSelectForScriptWork->SelectCancel( false );
          exitFrame( NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_SELECT_PLAYER );
        }
        else if(bTouchNoButton)
        {
          /*  no: 継続  */
          m_pJoinFestaPlayerListSelectForScriptViewList->RemoveLowerView();
        }
      }
    }



    m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::setScriptSelectedFlag()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

  if( pSelectPlayer )
  {
    if( pSelectPlayer->IsVip() )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pVipList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_VIP );
      
      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pVipList->Begin(); it != pVipList->End(); ++it )
      {
        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pSelectPlayer->GetJoinFestaFriendKey(), (*it)->GetJoinFestaFriendKey() ) )
        {
          (*it)->SetScriptListSelected( true );
          
          break;
        }
      }
    }
    else
    if( pSelectPlayer->IsGuest() )
    {
      NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pGuestList = pJoinFestaPersonalDataManager->GetPersonalDataList( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );

      for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator it = pGuestList->Begin(); it != pGuestList->End(); ++it )
      {
        if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( pSelectPlayer->GetJoinFestaFriendKey(), (*it)->GetJoinFestaFriendKey() ) )
        {
          (*it)->SetScriptListSelected( true );
          
          break;
        }
      }
    }
  }

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
const pml::pokepara::CoreParam* JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::CreatePokeparaFromPersonalData(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData)
{
  const pml::pokepara::CoreParam* pCoreParam  = BattleFesTool::CreateBattleTreePokemon(m_pJoinFestaPlayerListSelectForScriptWork->GetDeviceHeap(), rJFPersonalData);
  
  if(!RegisterPokepara(pCoreParam))
  {
    GFL_SAFE_DELETE(pCoreParam);
  }
  
  return pCoreParam;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
const pml::pokepara::CoreParam* JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::OnRequestPokepara(NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase* pCaller, const NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::ListType listType, const u32 dataIndex)
{
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList*     pList           = NULL;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& rJFPListManager = m_pJoinFestaPlayerListSelectForScriptWork->GetJoinFestaPlayerListManager();
  
  
  switch(listType)
  {
    case  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST:  pList = &rJFPListManager.GetGuestList();  break;
    case  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP:    pList = &rJFPListManager.GetVipList();    break;
    case  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_BLACK:  pList = &rJFPListManager.GetBlackList();  break;

    default:
      GFL_ASSERT(false);
      break;
  }
  if(!pList)  return NULL;
  

  {
    NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itrEnd  = pList->End();
    u32                                               index   = 0;

    for(NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itr = pList->Begin(); itr != itrEnd; ++itr)
    {
      if(index != dataIndex)
      {
        ++index;
        continue;
      }

      return itr->BFC_GetPokepara();
    }
  }


  return NULL;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::RegisterPokepara(const pml::pokepara::CoreParam* pPokepara)
{
  const bool  bValidIndex = m_PokeparaItems < GFL_NELEMS(m_pCoreParams);

  GFL_ASSERT(bValidIndex);
  if(bValidIndex)
  {
    m_pCoreParams[m_PokeparaItems++]  = pPokepara;
  }

  return bValidIndex;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes::ReleasePokepara(void)
{
  for(u32 index = 0; index < GFL_NELEMS(m_pCoreParams); ++index)
  {
    GFL_SAFE_DELETE(m_pCoreParams[index]);
  }
}





}  /*  namespace Frame  */
}  /*  namespace JoinFestaPlayerListSelectForScript  */
}  /*  namespace NetApp  */



// ============================================================================
/*
 * @file JoinFestaPlayerListShowSelectFrame.cpp
 * @brief 閲覧アプリのSDカード読み込みプロセスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h>

#include "./JoinFestaPlayerListShowSelectFrame.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListPlayerInfoUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerView.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"

#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <niji_conv_header/message/msg_jf_playerlist.h>
#include <GameSys/include/GameData.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)

static const u32 PUSH_UPDATE_BUTTON_MESSAGE_ID    = jf_playerlist_win_02;


JoinFestaPlayerListShowSelectFrame::JoinFestaPlayerListShowSelectFrame(
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor ) :
  NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase( pJoinFestaPlayerListShowWork, pJoinFestaPlayerListShowViewSystemAccessor ),
  m_JoinFestaPlayerListSelectLowerViewListener(),
  m_JoinFestaPlayerListMessageMenuLowerViewListener(),
  m_bUpdateButtonVisible( true ),
  m_bWaitForPhotoViewEvent( false ),
  m_bNeedToRemoveLowerView( false ),
  m_bRequestedSlideShow( false )
{
}


JoinFestaPlayerListShowSelectFrame::~JoinFestaPlayerListShowSelectFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaPlayerListShowSelectFrame::startup()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();

  /*  リスト更新  */
  joinFestaPlayerListManager.UpdateListMode3( true );

  m_pJoinFestaPlayerListShowViewObjects->InputDisableAll();
  m_pJoinFestaPlayerListShowViewList->RemoveAllView();

  m_bUpdateButtonVisible = NetLib::NijiNetworkSystem::IsWLANSwitchEnable();
  setVisibleUpdateButton( m_bUpdateButtonVisible );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleDecideButton( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleMenuCursor( true );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleCheckBox( false );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleInfoButton( true );
  {
    const bool isPhotoViewAvailable = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->IsMenuIconEnable(Savedata::FieldMenu::ICON_ID_ALBUM);
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetPhotoEnable(isPhotoViewAvailable, isPhotoViewAvailable);
  }
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleSlideshowButtonIfAvailable(joinFestaPlayerListManager.ContainsAnyPhoto());

  m_pJoinFestaPlayerListShowViewList->AddUpperView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGUpperView() );
  m_pJoinFestaPlayerListShowViewList->AddUpperView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView() );

  m_pJoinFestaPlayerListShowViewList->AddLowerView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListBGLowerView() );
  m_pJoinFestaPlayerListShowViewList->AddLowerView( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase() );

  setupList();

  return true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::setupList()
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();

  NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();

  if( eShowType == SHOW_TYPE_VIP )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& vipList = joinFestaPlayerListManager.GetVipList();
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &vipList, jf_playerlist_cap_13, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pVipActivePlayerData = m_pJoinFestaPlayerListShowWork->GetVipActivePlayerData();

    if( pVipActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = vipList.Begin();
      pVipActivePlayerData = ( vipList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pVipActivePlayerData );
    setActivePlayerData( pVipActivePlayerData );
  }
  else
  if( eShowType == SHOW_TYPE_GUEST )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& guestList = joinFestaPlayerListManager.GetGuestList();
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &guestList, jf_playerlist_cap_12, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pGuestActivePlayerData = m_pJoinFestaPlayerListShowWork->GetGuestActivePlayerData();

    if( pGuestActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = guestList.Begin();
      pGuestActivePlayerData = ( guestList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pGuestActivePlayerData );
    setActivePlayerData( pGuestActivePlayerData );
  }
  else
  if( eShowType == SHOW_TYPE_BLACK )
  {
    NetAppLib::JoinFestaUI::JoinFestPlayerDataList& blackList = joinFestaPlayerListManager.GetBlackList();
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetList( &blackList, jf_playerlist_cap_60, NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_BLACK );

    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pBlackActivePlayerData = m_pJoinFestaPlayerListShowWork->GetBlackActivePlayerData();

    if( pBlackActivePlayerData == NULL )
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = blackList.Begin();
      pBlackActivePlayerData = ( blackList.Size() > 0 ) ? &(*it) : NULL;
    }
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( pBlackActivePlayerData );
    setActivePlayerData( pBlackActivePlayerData );
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData )
{
  NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();

  if( eShowType == SHOW_TYPE_VIP )
  {
    m_pJoinFestaPlayerListShowWork->SetVipActivePlayerData( pActivePlayerData );
  }
  else
  if( eShowType == SHOW_TYPE_GUEST )
  {
    m_pJoinFestaPlayerListShowWork->SetGuestActivePlayerData( pActivePlayerData );
  }
  else
  if( eShowType == SHOW_TYPE_BLACK )
  {
    m_pJoinFestaPlayerListShowWork->SetBlackActivePlayerData( pActivePlayerData );
  }
}

//------------------------------------------------------------------
/**
*  @brief    
*/
//------------------------------------------------------------------
const NetAppLib::JoinFestaUI::JoinFestaPlayerData*  JoinFestaPlayerListShowSelectFrame::getActivePlayerData(void) const
{
  const NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();
  NetAppLib::JoinFestaUI::JoinFestaPlayerData*    pJFPData  = NULL;

  if(eShowType == SHOW_TYPE_VIP)
  {
    pJFPData = m_pJoinFestaPlayerListShowWork->GetVipActivePlayerData();
  }
  else if(eShowType == SHOW_TYPE_GUEST)
  {
    pJFPData = m_pJoinFestaPlayerListShowWork->GetGuestActivePlayerData();
  }
  else if(eShowType == SHOW_TYPE_BLACK)
  {
    pJFPData = m_pJoinFestaPlayerListShowWork->GetBlackActivePlayerData();
  }

  return pJFPData;
}



//------------------------------------------------------------------
/**
  *  @brief    写真呼び出し
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::callPhotoView(void)
{
  APP_PARAM*  pAppParam = m_pJoinFestaPlayerListShowWork->GetAppParam();

  pAppParam->RegisterSlideShowTarget(*getActivePlayerData());
  
  m_bWaitForPhotoViewEvent              = true; /**<  復帰待ち  */
}


//------------------------------------------------------------------
/**
  *  @brief    スライドショー呼び出し
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::callPhotoSlideShow(void)
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& rJFPlayerListManager  = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList* pLists[]  =
  {
    &(rJFPlayerListManager.GetGuestList()),
    &(rJFPlayerListManager.GetVipList()),
  };
  APP_PARAM*  pAppParam = m_pJoinFestaPlayerListShowWork->GetAppParam();

  for(u32 index = 0; index < GFL_NELEMS(pLists); ++index)
  {
    NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList        itr     = pLists[index]->Begin();
    const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itrEnd  = pLists[index]->End();

    while(itr != itrEnd)
    {
      if(itr->FS_IsPhotoDataAvailable())
      {
        pAppParam->RegisterSlideShowTarget(*itr);
        m_bWaitForPhotoViewEvent              = true; /**<  復帰待ち  */
      }
      ++itr;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::setListener()
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetListener( &m_JoinFestaPlayerListSelectLowerViewListener );
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetInputEnabled( true );

  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetListener( &m_JoinFestaPlayerListMessageMenuLowerViewListener );

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RegistPersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListShowSelectFrame::removeListener()
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RemoveListener();
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->RemoveListener();

#if defined( GF_PLATFORM_CTR )
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPacketManager )->RemovePersonalListener( this );
#endif // defined( GF_PLATFORM_CTR )
}


void JoinFestaPlayerListShowSelectFrame::OnWLANSwitchDisable()
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::applyPhotoNewFlag(void)
{
  APP_PARAM*                                              pAppParam = m_pJoinFestaPlayerListShowWork->GetAppParam();
  if(pAppParam->IsEmptySlideShowTarget()) return;


  NetAppLib::JoinFestaUI::JoinFestPlayerDataList*         pJFPDList = pAppParam->pPhotoOwnerList;
  NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList        itr       = pJFPDList->Begin();
  const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  itrEnd    = pJFPDList->End();

  NetAppLib::JoinFestaUI::JoinFestPlayerDataList* pDstJFPDLists[] =
  {
    &(m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager().GetGuestList()),
    &(m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager().GetVipList()),
  };

  for(; itr != itrEnd; ++itr)
  {
    App::FinderStudio::Static::PhotoData  photoData;

    if(!itr->GetPacketPhotoData(&photoData)) continue;
    if(itr->IsNewPhoto()) continue;
    
    for(u32 index = 0; index < GFL_NELEMS(pDstJFPDLists); ++index)
    {
      NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList        dstItr      = pDstJFPDLists[index]->Begin();
      const NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList  dstItrEnd   = pDstJFPDLists[index]->End();
      bool                                                    isEnd       = false;
      
      for(; dstItr != dstItrEnd; ++dstItr)
      {
        if(NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey(itr->GetJoinFestaFriendKey(), dstItr->GetJoinFestaFriendKey()))
        {
          dstItr->SetNewPhotoOff();
          dstItr->FS_SetPhotoDataStatus_Available();    /*  statusキャッシュも変更する  */
          isEnd = true;
          break;
        }
      }
      
      if(isEnd) break;
    }
  }

  /*  更新ボタンを押したときにNewStatusが巻き戻らないよう、ここでPersonalDataManagerへも反映させる  */
  m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager().ApplyPhotoFlagToPersonalDataManager();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListShowSelectFrame::updateSequence()
{
  /*
    PhotoView復帰待ち
  */
  if(m_bWaitForPhotoViewEvent)
  {
    const APP_PARAM* const  pAppParam = m_pJoinFestaPlayerListShowWork->GetAppParam();
    
    
    if(pAppParam->IsEmptySlideShowTarget())
    {
      if(m_bNeedToRemoveLowerView)  m_pJoinFestaPlayerListShowViewList->RemoveLowerView();

      m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->RefreshPhotoIconAll();
      m_bNeedToRemoveLowerView  = false;
      m_bWaitForPhotoViewEvent  = false;
      /**
        @fix    MMcat[342]   【フェスサークル】「しゃしんをみる」「スライドショー」のボタンアイコンタッチ直後にタッチ操作を受け付けている
                nijiにあったボタン類はその場でFrameが終了するか、ModalViewが上に追加されるため問題はなかったが、写真系はFadeをはさんで遷移し、且つ戻ってくるため明示的に入力の排他が必要
      */
      LockLowerListView(false);   /*  unlock  */
    }
    else
    {
      applyPhotoNewFlag();
    }
    
    if(m_bWaitForPhotoViewEvent)  return;
  }


  /*
    UI
  */
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
    u32 inputEventID            = m_JoinFestaPlayerListSelectLowerViewListener.GetInputEventID();
    bool bTouchBackButton       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON ) ? true : false;
    bool bTouchUpdateButton     = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_UPDATE_BUTTON ) ? true : false;
    bool bPushLButton           = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_L_BUTTON ) ? true : false;
    bool bPushRButton           = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_PUSH_R_BUTTON ) ? true : false;
    bool bSelectPlayer          = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener::INPUT_EVENT_ID_SELECT_PLAYER ) ? true : false;
    bool bTouchInfoButton       = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_INFO_BUTTON ) ? true : false;
    bool bTouchPhotoViewButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_PHOTOVIEW_BUTTON );
    bool bTouchSlideShowButton  = ( inputEventID == NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBaseListener::INPUT_EVENT_ID_TOUCH_SLIDESHOW_BUTTON );
    bool bIsUpdateUpperView     = m_JoinFestaPlayerListSelectLowerViewListener.IsUpdateUpperView();

    if( bIsUpdateUpperView )
    {
      m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->SetJoinFestaPlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
      setActivePlayerData( m_JoinFestaPlayerListSelectLowerViewListener.GetActivePlayer() );
    }

    if( bTouchBackButton )
    {
      exitFrame( NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_BACK );
    }
    else
    if( bTouchUpdateButton )
    {
      /*  更新ボタン押下  */
      if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() )
      {
        m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( PUSH_UPDATE_BUTTON_MESSAGE_ID );
      }
      else
      {
        m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_31 );
      }
    }
    else
    if( bPushLButton )
    {
      NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();

      switch( eShowType )
      {
        case SHOW_TYPE_GUEST: eShowType = SHOW_TYPE_BLACK; break;
        case SHOW_TYPE_VIP:   eShowType = SHOW_TYPE_GUEST; break;
        case SHOW_TYPE_BLACK: eShowType = SHOW_TYPE_VIP;   break;
      }

      m_pJoinFestaPlayerListShowWork->SetShowType( eShowType );

      setupList();
      setVisibleUpdateButton( m_bUpdateButtonVisible );
    }
    else
    if( bPushRButton )
    {
      NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();

      switch( eShowType )
      {
        case SHOW_TYPE_GUEST: eShowType = SHOW_TYPE_VIP;   break;
        case SHOW_TYPE_VIP:   eShowType = SHOW_TYPE_BLACK; break;
        case SHOW_TYPE_BLACK: eShowType = SHOW_TYPE_GUEST; break;
      }

      m_pJoinFestaPlayerListShowWork->SetShowType( eShowType );

      setupList();
      setVisibleUpdateButton( m_bUpdateButtonVisible );
    }
    else
    if( bSelectPlayer )
    {
      NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

      if( pSelectPlayer )
      {
        NetAppLib::JoinFestaUI::JoinFestaPlayerData& selectPlayer = m_pJoinFestaPlayerListShowWork->GetSelectPlayer();
        selectPlayer = *pSelectPlayer;

        if( pSelectPlayer->IsVip() )
        {
          m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_08, pSelectPlayer->GetName(), jf_playerlist_sel_03, jf_playerlist_sel_04, jf_playerlist_sel_05 );
        }
        else
        if( pSelectPlayer->IsGuest() )
        {
          exitFrame( NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_SELECT_PLAYER );
        }
        else
        if( pSelectPlayer->IsBlack() )
        {
          const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData& blackCoreData = pSelectPlayer->GetBlackPlayerCoreData();
          m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageMenuBlackFilter( jf_playerlist_win_08, blackCoreData.name, jf_playerlist_sel_13, jf_playerlist_sel_05 );
        }
      }
    }
    else
    if( bTouchInfoButton )
    {
      m_pJoinFestaPlayerListShowWork->SetDescriptionOpenAnimeFlag( true );
      exitFrame( NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_TO_DESCRIPTION );
    }
    else
    if(bTouchPhotoViewButton)
    {
      if(pJoinFestaDataSave->GetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelect())
      {
        /**
          @fix    MMcat[342]   【フェスサークル】「しゃしんをみる」「スライドショー」のボタンアイコンタッチ直後にタッチ操作を受け付けている
                  nijiにあったボタン類はその場でFrameが終了するか、ModalViewが上に追加されるため問題はなかったが、写真系はFadeをはさんで遷移し、且つ戻ってくるため明示的に入力の排他が必要
        */
        LockLowerListView(true);
        callPhotoView();
      }
      else
      {
        /*  UGC confirm  */
        m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( msg_playerlist_guideline_00 );
        pJoinFestaDataSave->SetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelect(true);
      }
    }
    else
    if(bTouchSlideShowButton)
    {
      if(pJoinFestaDataSave->GetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelectSlideShow())
      {
        /**
          @fix    MMcat[342]   【フェスサークル】「しゃしんをみる」「スライドショー」のボタンアイコンタッチ直後にタッチ操作を受け付けている
                  nijiにあったボタン類はその場でFrameが終了するか、ModalViewが上に追加されるため問題はなかったが、写真系はFadeをはさんで遷移し、且つ戻ってくるため明示的に入力の排他が必要
        */
        LockLowerListView(true);
        callPhotoSlideShow();
      }
      else
      {
        /*  UGC confirm  */
        m_bRequestedSlideShow = true;
        m_pJoinFestaPlayerListShowViewList->AddLowerViewInputEnable( m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView() );
        m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( msg_playerlist_guideline_00 );
        pJoinFestaDataSave->SetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelectSlideShow(true);
      }
    }

    m_JoinFestaPlayerListSelectLowerViewListener.ResetInputEvent();
  }

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
      /*  更新メッセージ  */
      case PUSH_UPDATE_BUTTON_MESSAGE_ID:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListPlayerInfoUpperView()->ResetCurrentJoinFestaFriendKey();

          setVisibleUpdateButton( false );

          m_pJoinFestaPlayerListShowWork->SetUpdateNotificationFlag( false );

          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();

          /*  リスト更新  */
          joinFestaPlayerListManager.UpdateListMode3( false );

          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->ResetPaneListCursorInfo();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->UpdateList();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleSlideshowButtonIfAvailable(joinFestaPlayerListManager.ContainsAnyPhoto());
          m_pJoinFestaPlayerListShowWork->ResetActivePlayerData();

          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      case jf_playerlist_win_08:
      {
        if( bSelectMenu )
        {
          switch( selectMenuID )
          {
            case jf_playerlist_sel_03:
            {
              m_pJoinFestaPlayerListShowViewList->RemoveLowerView();

              exitFrame( NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_SELECT_PLAYER );
            }
            break;

            case jf_playerlist_sel_04:
            {
              NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

              m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetMessageYesNoBlackFilter( jf_playerlist_win_09, pSelectPlayer->GetName() );
            }
            break;
            
            case jf_playerlist_sel_05:
            {
              m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
            }
            break;
            
            case jf_playerlist_sel_13:
            {
              m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageYesNoBlackFilter( jf_playerlist_win_11 );
            }
            break;
          }
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      /*  VIPから外しますか?  */
      case jf_playerlist_win_09:
      {
        if( bTouchYesButton )
        {
          Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
          NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

          // @fix NMCat[3593]：ゲストNG設定なのでゲスト0人のはずなのに、ゲストに降格されると状態として不整合になり、PacketManagerのIsRejectで消される。
          if( pJoinFestaDataSave->IsRejectGuest() )
          {
            // ゲストNGの場合は、降格できないので削除する
            GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->RemoveListAndSave( pSelectPlayer->GetJoinFestaFriendKey() );
          }
          else
          {
            GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ChangePersonalRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST, pSelectPlayer->GetJoinFestaFriendKey() );
          }

          // @fix GFNMCat[1633]：フィールドにもいた場合、そちらの関係性も更新
          {
            NetAppLib::JoinFesta::JoinFestaPersonalData* pFieldPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetFieldPersonalData( pSelectPlayer->GetJoinFestaFriendKey() );
            if( pFieldPersonal )
            {
              pFieldPersonal->SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
            }

            NetAppLib::JoinFesta::JoinFestaPersonalData& lastP2pPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetLastP2pPersonal();
            if( lastP2pPersonal.IsEnable() )
            {
              lastP2pPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
            }

            NetAppLib::JoinFesta::JoinFestaPersonalData& recruiterPersonal = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetRecruiterPersonal();
            if( recruiterPersonal.IsEnable() )
            {
              recruiterPersonal.SetRelation( NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_GUEST );
            }
          }

          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_10, pSelectPlayer->GetName() );
        }
        else
        if( bTouchNoButton )
        {
          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      /*  VIPからの削除  */
      case jf_playerlist_win_10:
      {
        if( bTouchScreen )
        {
          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();

          /*  リスト更新  */
          joinFestaPlayerListManager.UpdateListMode3( false );

          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->ResetPaneListCursorInfo();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->UpdateList();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleSlideshowButtonIfAvailable(joinFestaPlayerListManager.ContainsAnyPhoto());
          m_pJoinFestaPlayerListShowWork->ResetActivePlayerData();

          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      case jf_playerlist_win_11:
      {
        if( bTouchYesButton )
        {
          NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_JoinFestaPlayerListSelectLowerViewListener.GetSelectPlayer();

          Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

          pJoinFestaDataSave->DeleteBlackList( pSelectPlayer->GetBlackPlayerCoreData().GetJoinFestaFriendKey() );

          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListMessageMenuLowerView()->SetStreamMessageBlackFilter( jf_playerlist_win_12 );
        }
        else
        if( bTouchNoButton )
        {
          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      /*  おことわりからの削除  */
      case jf_playerlist_win_12:
      {
        if( bTouchScreen )
        {
          NetAppLib::JoinFestaUI::JoinFestaPlayerListManager& joinFestaPlayerListManager = m_pJoinFestaPlayerListShowWork->GetJoinFestaPlayerListManager();

          /*  リスト更新  */
          joinFestaPlayerListManager.UpdateListMode3( false );

          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->ResetPaneListCursorInfo();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->UpdateList();
          m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleSlideshowButtonIfAvailable(joinFestaPlayerListManager.ContainsAnyPhoto());
          m_pJoinFestaPlayerListShowWork->ResetActivePlayerData();

          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;

      // ---------------------------------------------------------------------------------------------
      case jf_playerlist_win_31:
      {
        if( bTouchScreen )
        {
          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
        }
      }
      break;


      // ---------------------------------------------------------------------------------------------
      /*  UGC guideline message  */
      case  msg_playerlist_guideline_00:
      {
        if(bTouchYesButton)
        {
          if(m_bRequestedSlideShow) callPhotoSlideShow();
          else                      callPhotoView();
          /*  removeLowerViewはEvent復帰時に行う  */
          m_bNeedToRemoveLowerView  = true;
          m_bRequestedSlideShow     = false;
        }
        else  if( bTouchNoButton )
        {
          m_pJoinFestaPlayerListShowViewList->RemoveLowerView();
          m_bRequestedSlideShow     = false;
        }
      }
      break;

    }

    m_JoinFestaPlayerListMessageMenuLowerViewListener.ResetInputEvent();
  }
}


void JoinFestaPlayerListShowSelectFrame::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListShowWork->SetUpdateNotificationFlag( true );

  setVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListShowSelectFrame::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  m_pJoinFestaPlayerListShowWork->SetUpdateNotificationFlag( true );

  setVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListShowSelectFrame::OnDeleteJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  m_pJoinFestaPlayerListShowWork->SetUpdateNotificationFlag( true );

  setVisibleUpdateButton( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() );
}


void JoinFestaPlayerListShowSelectFrame::setVisibleUpdateButton( bool bVisible )
{
  NetApp::JoinFestaPlayerListShow::ShowType eShowType = m_pJoinFestaPlayerListShowWork->GetShowType();

  if( eShowType == SHOW_TYPE_BLACK )
  {
    m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( false );
  }
  else
  {
    if( m_pJoinFestaPlayerListShowWork->GetUpdateNotificationFlag() )
    {
      m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetVisibleUpdateButton( bVisible );
    }
    m_bUpdateButtonVisible = bVisible;
  }
}


//------------------------------------------------------------------
/**
    @fix    MMcat[342]   【フェスサークル】「しゃしんをみる」「スライドショー」のボタンアイコンタッチ直後にタッチ操作を受け付けている
            nijiにあったボタン類はその場でFrameが終了するか、ModalViewが上に追加されるため問題はなかったが、写真系はFadeをはさんで遷移し、且つ戻ってくるため明示的に入力の排他が必要
    @brief  入力排他
  */
//------------------------------------------------------------------
void  JoinFestaPlayerListShowSelectFrame::LockLowerListView(const bool bLock)
{
  m_pJoinFestaPlayerListShowViewObjects->GetJoinFestaPlayerListSelectLowerViewBase()->SetInputEnabled(!bLock);
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

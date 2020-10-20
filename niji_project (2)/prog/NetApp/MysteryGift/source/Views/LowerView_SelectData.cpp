
//==============================================================================
/**
 * @file   LowerView_SelectData.cpp
 * @date   2015/07/31 Fri. 18:31:28
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../ApplicationSystem/GiftWork.h"

#include  "./LowerView_SelectData.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  
  {
    MSGSTATE_None,

    MSGSTATE_AlbumCleanupConfirm,
 
    /*  exit系  */
    MSGSTATE_Exit_1stConfirm,
    MSGSTATE_Exit_RemainConfirm,
    
    MSGSTATE_
  };
}





namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_SelectData::LowerView_SelectData(GiftWork* pWork)
  : LowerView_SelectionScrollListBase(pWork)
  , m_pHandler(NULL)
  , m_SelectedIndex(0)
  , m_Remains(0)
  , m_pMessageMenu(NULL)
  , m_MessageState(MSGSTATE_None)
{
  gfl2::heap::HeapBase*   pHeapBase = pWork->GetDeviceHeap();
  
  
  m_pMessageMenu  = GFL_NEW(pHeapBase)  GiftMessageMenu(pWork, this);
  AddSubView(m_pMessageMenu);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
LowerView_SelectData::~LowerView_SelectData()
{
  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();

  GFL_SAFE_DELETE(m_pMessageMenu);
}



/*
  super
*/
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    基底からの終了通知 
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::OnExit(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_ReceiveCancelConfirm);   /*  受け取るのをやめますか  */
  m_MessageState = MSGSTATE_Exit_1stConfirm;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    カーソル移動通知
  */
//------------------------------------------------------------------
void  LowerView_SelectData::OnCursorMove(const u32 listIndex)
{
  m_SelectedIndex = listIndex;
  Event(IEventHandler::EVENT_FocusItemChange);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::OnUpdateItem(gfl2::lyt::LytTextBox* pTextbox, const u32 listIndex)
{
  GetG2DUtil()->SetTextBoxPaneString( pTextbox, m_pWork->m_pAppParam->GetDeliveryEventName(listIndex) );
}

//------------------------------------------------------------------
/**
  *  @func     OnSelectedItem
  *  @brief    項目選択イベント
  *  @param    itemIndex  [in]  選択された項目のIndex（0-origin）
  */
//------------------------------------------------------------------
void  LowerView_SelectData::OnSelectedItem(const u32 listIndex)
{
  m_SelectedIndex = listIndex;
  Event(IEventHandler::EVENT_SelectedItem);
}
/*
  end super
*/


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
      if(m_MessageState == MSGSTATE_AlbumCleanupConfirm)
      {
        m_MessageState  = MSGSTATE_None;
        Event(IEventHandler::EVENT_AlbumCleanup);   /*  アルバム整理  */
      }
      else if(m_MessageState == MSGSTATE_Exit_1stConfirm)
      {
        Event(IEventHandler::EVENT_OnCheckRemains);   /**<  残件数確認  */

        /*  残りがあれば再確認、なければここで終了  */
        if(m_Remains)
        {
          HideMessage();
          m_pMessageMenu->Show(GiftMessageMenu::MODE_RemainCancelConfirm);    /*  まだ残っているけどやめますか  */
          m_MessageState = MSGSTATE_Exit_RemainConfirm;
        }
        else
        {
        m_MessageState  = MSGSTATE_None;
        Event(IEventHandler::EVENT_Exit);
        }
      }
      else if(m_MessageState == MSGSTATE_Exit_RemainConfirm)
      {
        /*  終了  */
        m_MessageState  = MSGSTATE_None;
        Event(IEventHandler::EVENT_Exit);
      }
      break;

    case  EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
      if(m_MessageState == MSGSTATE_AlbumCleanupConfirm)
      {
        Event(IEventHandler::EVENT_Exit);
      }
      else if(m_MessageState == MSGSTATE_Exit_1stConfirm)
      {
        /*  nop */
      }
      else if(m_MessageState == MSGSTATE_Exit_RemainConfirm)
      {
        /*  nop */
      }
      m_MessageState  = MSGSTATE_None;
      break;


    case  EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
    case  EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::Event(const IEventHandler::EventCode eventCode)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_SelectData_OnEvent(eventCode);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_AlreadyReceived(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_NG_AlreadyReceived);   /**<  すでにうけとっているのでNG          */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_OnceAtDayInfo(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_CanReceiveOnceAtDay);    /**<  一日一回受け取れます  */
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_OnceAtDay(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_NG_OnceAtDay);  /**<  一日一回なのでNG                    */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_OnceAtDay_HistoryFull(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_NG_OnceAtDay_HistoryFull);   /**<  一日一回系履歴領域Full              */
}


//------------------------------------------------------------------
/**
  *  @brief    終了しています
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_Finalizing(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_Finalizing);   /**<  終了しています                      */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_AlbumFull(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumFull);  /**<  いっぱいです    */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_AlbumCleanupConfirm(void)
{
  HideMessage();
  m_MessageState  = MSGSTATE_AlbumCleanupConfirm;
  m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumCleanupConfirm);   /**<  整理しますか？  */
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_SelectData::ShowMessage_Saving(void)
{
  HideMessage();
  m_pMessageMenu->Show(GiftMessageMenu::MODE_Saving);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_SelectData::HideMessage(void)
{
  m_pMessageMenu->HideMessage();
}



} /*  namespace Gift    */
} /*  namespace NetApp  */

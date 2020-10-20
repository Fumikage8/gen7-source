#if !defined(LOWERVIEW_SELECTDATA_H_INCLUDED)
#define LOWERVIEW_SELECTDATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_SelectData.h
 * @date   2015/07/31 Fri. 18:26:51
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./Base/LowerView_SelectionScrollListBase.h"
#include  "./GiftMessageMenu.h"

namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.23 Tue.
  */
//==============================================================================
class LowerView_SelectData
  : public    LowerView_SelectionScrollListBase
  , protected GiftMessageMenu::IEventHandler
  , protected GiftMessageMenu::IValueProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_SelectData );
public:

  /*
  */
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_Exit,

      EVENT_MessageFinish,


      EVENT_FocusItemChange,    /**<  選択中の項目が変更された      */
      EVENT_SelectedItem,       /**<  項目が決定された              */

      EVENT_OnCheckRemains,     /**<  受信可能なデータがまだあるか  */

      EVENT_AlbumCleanup,       /**<  アルバム削除モードへ          */


      EVENT_
    };

    virtual void  LowerView_SelectData_OnEvent(const EventCode )=0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  LowerView_SelectData(GiftWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual ~LowerView_SelectData();



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  u32 SelectedIndex(void) const   {return m_SelectedIndex;}




  void  SetRemains(const u32 remains) {m_Remains  = remains;}


  /*  message  */
  void  ShowMessage_AlreadyReceived(void);
  void  ShowMessage_OnceAtDayInfo(void);
  void  ShowMessage_OnceAtDay(void);
  void  ShowMessage_OnceAtDay_HistoryFull(void);
  void  ShowMessage_Finalizing(void);           /**<  終了しています  */

  void  ShowMessage_AlbumFull(void);            /**<  いっぱいです    */
  void  ShowMessage_AlbumCleanupConfirm(void);  /**<  整理しますか？  */


  void  ShowMessage_CommitSaveError(const AuthenticationHelper::CommitSaveResult commitSaveResult)  {m_pMessageMenu->ShowAuthError(commitSaveResult);}



  void  ShowMessage_Saving(void);
  void  HideMessage(void);



  /*
    UIView
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
//  virtual void  Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
//  virtual void  Draw(void);
  /*
    end UIView
  */



protected:

  /*
    super
  */
  virtual void  OnCursorMove(const u32 listIndex);                                    /**<  カーソル移動通知  */
  virtual void  OnUpdateItem(gfl2::lyt::LytTextBox* pTextbox, const u32 listIndex);   /**<  項目更新委譲      */
  //------------------------------------------------------------------
  /**
    *  @func     OnSelectedItem
    *  @brief    項目選択イベント
    *  @param    itemIndex  [in]  選択された項目のIndex（0-origin）
    */
  //------------------------------------------------------------------
  virtual void  OnSelectedItem(const u32 listIndex);
  //------------------------------------------------------------------
  /**
    *  @func     OnExit
    *  @brief    基底からの終了通知
    */
  //------------------------------------------------------------------
  virtual void  OnExit(void);
  /*
    end super
  */

  /*  GiftMessageMenu::IEventHandler  */
  virtual void  MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode eventCode);

  /*  GiftMessageMenu::IValueProvider  */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)     {return 0;}   /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender) {return 0;}   /**<  受信Progress  */

  void  Event(const IEventHandler::EventCode eventCode);







  IEventHandler*    m_pHandler;
  u32               m_SelectedIndex;
  u32               m_Remains;

  GiftMessageMenu*  m_pMessageMenu;
  u32               m_MessageState;
};





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LOWERVIEW_SELECTDATA_H_INCLUDED)  */


#if !defined(LOWERVIEW_ALBUM_H_INCLUDED)
#define LOWERVIEW_ALBUM_H_INCLUDED
#pragma once
//==============================================================================
/**
  *  @file   LowerView_Album.h
  *  @brief  ふしぎなおくりもの 下画面 カード選択
  *  @date   2015.05.20 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <niji_conv_header/poke_lib/monsno_def.h>
#include  "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"


#include  "./GiftMessageMenu.h"
#include  "./LowerView_InfoWindow.h"

namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
class AlbumCardView;

//==============================================================================
/**
  *  @class  LowerView_Album
  *  @brief  Draw: 項目選択 下画面
  *  @date   2015.05.20 Wed.
  */
//==============================================================================
class LowerView_Album
  : public    NetAppLib::UI::NetAppCursorView
  , protected GiftMessageMenu::IValueProvider
  , protected GiftMessageMenu::IEventHandler

{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_Album );

public:
  class IEventHandler
  {
  public:
    virtual ~IEventHandler(){}

    enum  EventCode
    {
      EVENT_Exit,
      /*  message  */
      EVENT_MessageFinish,
      
      /*  selection  */
      EVENT_MoveCursor,     /**<  カーソル移動      */
      EVENT_SelectedItem,   /**<  カーソルでの決定  */

      /*  operation  */
      EVENT_FlipCard,       /**<  表/裏をみる、裏返すボタン  */
      EVENT_SwapCard,       /**<  入れ替えの実行             */
      EVENT_DeleteCard,     /**<  カード削除                */

      /*  cleanupCancelConfirm  */
      EVENT_CleanupCancel,

      /*  anim  */
      EVENT_OnSwapAnimAEnd,
      EVENT_OnSwapAnimBEnd,
      EVENT_OnDeleteAnimEnd,
      EVENT_OnPushCard,
      EVENT_OnPopCard,


      EVENT_
    };
  
  
    virtual void  LowerView_Album_OnEvent(LowerView_Album* pSender, const EventCode event) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)    {m_pHandler = pHandler;}


  //==============================================================================
  /**
    *  @brief  
    *  @date   2015.05.22 Fri.
    */
  //==============================================================================
  class ICardAlbumDataProvider
  {
  public:
    struct  CardInfo
    {
      enum  GiftType
      {
        GIFTTYPE_Pokemon,
        GIFTTYPE_Item,
        GIFTTYPE_Others,
        
        GIFTTYPE_
      };

      GiftType  giftType;

      enum  Rarity
      {
        RARITY_Bronze,
        RARITY_Silver,
        RARITY_Gold,
        
        RARITY_
      };
      Rarity    rarity;

      pml::pokepara::CoreParam*   pPokeParaCoreParam;   /*  一時利用  */

      u16       itemNo;


      u16       year;
      u8        month;
      u8        day;
      bool      isReceived;


      void  Clear(void);
    };
  
    virtual u32   AlbumDataProvider_DataNum(void) const = 0;
    virtual bool  AlbumDataProvider_GetCardInfo(CardInfo& rInfo, const u32 listIndex) = 0;
    virtual bool  AlbumDataProvider_DeleteQuery(const u32 listIndex) = 0;
    
    virtual ~ICardAlbumDataProvider(){}
  };
  void  SetDataProvider(ICardAlbumDataProvider* pDataProvider)
        {
          m_pAlbumDataProvider = pDataProvider;
          InvalidateView();
        }


  void  SetDeleteMode(const bool isDeleteMode)  {m_IsDeleteMode = isDeleteMode;}
  void  SetCardFace(const bool isFront);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  u32 GetSelectedIndex(void) const    {return m_SelectedIndex;}
  u32 GetPushedIndex(void) const      {return m_StackCardIndex;}


  //------------------------------------------------------------------
  /**
    *  @func     InvalidateView
    *  @brief    Viewを無効化し、次回更新時にDataProviderからのDataによってViewを更新する
    */
  //------------------------------------------------------------------
  void  InvalidateView(void);



  void  SetProgress(const u32 progress)   {m_Progress = progress;}


  GiftMessageMenu*  GetMessageMenu(void)  {return m_pMessageMenu;}
  void              ShowMessage_CardDeleted(const bool isNoHide);
  void              ShowMessage_Saving(void);
  void              ShowMessage_Cleanup(void)               {m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumCleanup);}
  void              ShowMessage_CleanupCancelConfirm(void)  {m_pMessageMenu->Show(GiftMessageMenu::MODE_AlbumCleanupExitConfirm);}
  void              ShowMessage_NoCard(void)                {m_pMessageMenu->Show(GiftMessageMenu::MODE_Album_NoCard);}
  void              HideMessage(void)                       {m_pMessageMenu->HideMessage();}


  void  Reset(void);

  /*
    cardAnim
  */
  void  StartCardSwapAnimA(const u32 targetCardIndex1, const u32 targetCardIndex2);
  void  StartCardSwapAnimB(const u32 targetCardIndex1, const u32 targetCardIndex2);
  void  StartCardDeleteAnim(const u32 targetCardIndex);

  void  SetCardSwapEnable(const bool isEnable)  {m_pMessageMenu->SetCardSwapEnable(isEnable);}



  /*
    UIView
  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
  /*
    end UIView
  */


  /*
    UIInputListener
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult
    OnLayoutPaneEvent( const u32 painId );
  /*
    end UIInputListener
  */



  LowerView_Album(GiftWork* pWork);
  virtual ~LowerView_Album();




  //------------------------------------------------------------------
  /**
    *  @func     RefreshView
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  RefreshView(const bool isSkipResetAnime = false);


protected:
  /** 表示モード  */
  enum  ViewMode
  {
    VIEWMODE_CardSelect,      /**<  カード選択      */

    VIEWMODE_
  };

  /**
    TextBox
  */
  enum  TextBoxID
  {
    TEXTBOX_Title,          /**<  タイトルバー                */
    TEXTBOX_CurrentPage,    /**<  現在のページ                */
    TEXTBOX_PageSeparator,  /**<  ページ/ページ数間セパレータ */
    TEXTBOX_PageNum,        /**<  ページ数                    */

    TEXTBOX_MAX
  };


  /**
    CardID
  */
  enum  CardID 
  {
    CARD_00,
    CARD_01,
    CARD_02,
    CARD_03,

    CARD_NUM
  };

  /*  type  */
  enum  CardType
  {
    TYPE_Silver,
    TYPE_Bank,
    TYPE_Gold,

    TYPE_MAX
  };

  //------------------------------------------------------------------
  /**
    *  @func     SetupViewMode
    *  @brief    モード切り替え
    */
  //------------------------------------------------------------------
  void  SetViewMode(const ViewMode viewMode);

  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  void  SetText(const TextBoxID textBoxID, const u32 messageID);


  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  void  SetPageText(const u32 page, const u32 pageMax);


  //------------------------------------------------------------------
  /**
    *  @func     SetCardText
    *  @brief    カードのテキストを設定する
    */
  //------------------------------------------------------------------
  void  SetCardText(const CardID cardID, const u32 index, const u32 year, const u32 month, const u32 date);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetCardPokeIcon(const CardID cardID, const pml::pokepara::PokemonParam& rPokemonParam);

  //------------------------------------------------------------------
  /**
    *  @func     SetupCardIcon
    */
  //------------------------------------------------------------------
  void  SetupCardIcon(AlbumCardView* pCardView, const ICardAlbumDataProvider::CardInfo& rCardInfo);


  //------------------------------------------------------------------
  /**
    *  @func     TransitionPage
    *  @brief    ページ遷移
    */
  //------------------------------------------------------------------
  bool  TransitionPage(const bool isForward);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Event(const IEventHandler::EventCode event);

  void  OnSelectedCard(void);
  bool  IsValidIndex(const u32 index) const                 {return (index < m_DataNum);}
  bool  IsValidPage(const u32 page) const;
  u32   PageOffsetToDataIndex(const u32 pageOffset) const   {return ((m_Page * m_PageItems) + pageOffset);}
  u32   DataIndexToPageOffset(const u32 dataIndex) const    {return (dataIndex % m_PageItems);}
  bool  IsDataIndexInCurrentPage(const u32 dataIndex) const {return (IsValidIndex(dataIndex) && (((m_Page * m_PageItems) <= dataIndex) && (dataIndex < (m_Page * m_PageItems + m_PageItems))));}

  bool  IsCardPushed(void) const                            {return m_IsCardPushed;}
  void  PushCard(void);
  void  PopCard(void);

  void  Flip(void);
  void  OnTryCardDelete(void);

  bool  IsEnableSwap(void) const;

  /*  class GiftMessageMenu::IValueProvider */
  virtual u32   ValueProvider_AmountSpace(GiftMessageMenu* pSender)     {return 0;}             /**<  あとn件       */
  virtual u32   ValueProvider_ReceiveProgress(GiftMessageMenu* pSender) {return m_Progress;}    /**<  受信Progress  */
  virtual void  MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event);




  /*  implement app::tool::CursorController  */
  virtual void  CursorController_OnEvent(app::tool::CursorController* pSender, const app::tool::CursorController::IEventHandler::EventCode eventCode);


  /*
    members
  */
  class AlbumCursorController;

  GiftWork*                 m_pWork;
  gfl2::lyt::LytTextBox*    m_pTextBoxes[TEXTBOX_MAX];
  AlbumCardView*            m_pCardViews[CARD_NUM];
  GiftMessageMenu*          m_pMessageMenu;
  LowerView_InfoWindow*     m_pInfoWindow;
  gfl2::lyt::LytPane*       m_pCursorPosPanes[CARD_NUM];
  gfl2::lyt::LytPane*       m_pPane_Null_PageNum;
  AlbumCursorController*    m_pAlbumCursorController;

  IEventHandler*            m_pHandler;
  ICardAlbumDataProvider*   m_pAlbumDataProvider;
  u32                       m_DataNum;      /**<  データ総数                      */
  u32                       m_Page;         /**<  現在のページ                    */
  u32                       m_PageItems;    /**<  ページあたりの項目数            */
  u32                       m_SelectedIndex;
  u32                       m_StackCardIndex; /**<  入れ替え元  */
  u32                       m_Progress;     /**<  受信進捗                        */

  bool                      m_IsViewDirty;  /**<  表示内容を更新する必要があるか  */
  bool                      m_IsDeleteMode; 
  bool                      m_bCardFaceIsFront; /**<  表面を表示中  */
  bool                      m_IsCardPushed;

  /*  cardanim  */
  /*  swap  */
  bool                      m_bPlayingSwapAnimA;
  bool                      m_bPlayingSwapAnimB;
  u32                       m_SwapAnim_ID1;
  u32                       m_SwapAnim_ID2;
  /*  delete  */
  bool                      m_bPlayingDeleteAnim;
  u32                       m_DeleteAnim_ID;


};


} /*  namespace MysteryGift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LOWERVIEW_ALBUM_H_INCLUDED)  */


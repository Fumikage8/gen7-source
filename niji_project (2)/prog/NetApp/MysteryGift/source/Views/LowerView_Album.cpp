
//==============================================================================
/**
  *  @file   MysteryGiftDraw_CardSelectLower.cpp
  *  @brief  ふしぎなおくりもの 下画面 カード選択
  *  @date   2015.05.20 Wed.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "../ApplicationSystem/GiftWork.h"

#include  "../Common/ButtonHelper.h"
#include  "../GiftResource.h"
#include  "./Base/GiftAlbumCardView.h"
#include  "./LowerView_Album.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================
/*
  local
*/
namespace
{

  /** layout work  */
  enum  {
    LAYOUTWORK_CardSelectLow,

    LAYOUTWORK_MAX
  };

  /**
    ButtonID
  */
  enum  {
    BUTTON_Card00,    /**<  カード  */
    BUTTON_Card01,    /**<  カード  */
    BUTTON_Card02,    /**<  カード  */
    BUTTON_Card03,    /**<  カード  */

    BUTTON_Backward,  /**<  逆方向  */
    BUTTON_Forward,   /**<  順方向  */

    BUTTON_FlipCard,  /**<  カード裏返す  */
    BUTTON_Back,     /**<  戻る  */

    BUTTON_MAX
  };



  const ButtonHelperInfo buttonHelperInfo[]  = {

    ButtonHelperInfo(
      BUTTON_Card00,
      PANENAME_GIFT_CRD_LOW_000_PANE_CARD_00,
      PANENAME_GIFT_BTN_LOW_000_PANE_BOUND_00,
      true
    ),
    ButtonHelperInfo(
      BUTTON_Card01,
      PANENAME_GIFT_CRD_LOW_000_PANE_CARD_01,
      PANENAME_GIFT_BTN_LOW_000_PANE_BOUND_00,
      true
    ),
    ButtonHelperInfo(
      BUTTON_Card02,
      PANENAME_GIFT_CRD_LOW_000_PANE_CARD_02,
      PANENAME_GIFT_BTN_LOW_000_PANE_BOUND_00,
      true
    ),
    ButtonHelperInfo(
      BUTTON_Card03,
      PANENAME_GIFT_CRD_LOW_000_PANE_CARD_03,
      PANENAME_GIFT_BTN_LOW_000_PANE_BOUND_00,
      true
    ),

    ButtonHelperInfo(
      BUTTON_Backward,  /**<  逆方向  */
      PANENAME_GIFT_CRD_LOW_000_PANE_CURSOR_L,
      PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
      true,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_TOUCH,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_CANSEL,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_TOUCH_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_PASSIVE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_L_ACTIVE
    ),
    ButtonHelperInfo(
      BUTTON_Forward,  /**<  順方向  */
      PANENAME_GIFT_CRD_LOW_000_PANE_CURSOR_R,
      PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
      true,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_TOUCH,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_CANSEL,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_TOUCH_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_PASSIVE,
      LA_GIFT_GIFT_CRD_LOW_000__CURSOR_R_ACTIVE
    ),

    /*  カードflip  */
    ButtonHelperInfo(
      BUTTON_FlipCard,
      PANENAME_GIFT_CRD_LOW_000_PANE_CHANGEBUTTON,
      PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
      true,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_TOUCH,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_CANSEL,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_TOUCH_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_PASSIVE,
      LA_GIFT_GIFT_CRD_LOW_000__CHANGEBUTTON_ACTIVE
    ),
    /*  終了  */
    ButtonHelperInfo(
      BUTTON_Back,
      PANENAME_GIFT_CRD_LOW_000_PANE_BACKBUTTON,
      PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
      true,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_TOUCH,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_CANSEL,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_TOUCH_RELEASE,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_PASSIVE,
      LA_GIFT_GIFT_CRD_LOW_000__BACKBUTTON_ACTIVE
    ),
  };


  /*
  */
  inline u32  GetPageNum(const u32 dataNum, const u32 pageItems)
  {
    return ((dataNum + (pageItems -1)) / pageItems);
  }


  static const struct
  {
    u32 anim_Gold;
    u32 anim_Silver;
    u32 anim_Bronze;
    /*  stateanim  */
    u32 anim_Pushed;    /*  入れ替え元:loop */
    u32 anim_SwapA;
    u32 anim_SwapB;
    u32 anim_Reset;
    u32 anim_Delete;
  }cardAnimTable[]  =
  {
    {
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_TYPE_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_TYPE_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_TYPE_02,
      /*  stateanim  */
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_ANI_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_ANI_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_ANI_02,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_ANI_03,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_00_CARD_ANI_04,
    },
    {
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_TYPE_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_TYPE_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_TYPE_02,
      /*  stateanim  */
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_ANI_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_ANI_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_ANI_02,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_ANI_03,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_01_CARD_ANI_04,
    },
    {
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_TYPE_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_TYPE_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_TYPE_02,
      /*  stateanim  */
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_ANI_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_ANI_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_ANI_02,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_ANI_03,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_02_CARD_ANI_04,
    },
    {
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_TYPE_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_TYPE_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_TYPE_02,
      /*  stateanim  */
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_ANI_00,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_ANI_01,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_ANI_02,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_ANI_03,
      LA_GIFT_GIFT_CRD_LOW_000__CARD_03_CARD_ANI_04,
    },
  };
  
  
  
  static const u32  CARD_ROWS = 2;
  static const u32  CARD_COLS = 2;

  u32   PageIndexToColumn(const u32 pageIndex)
        {
          return (pageIndex % CARD_COLS);
        }

  u32   PageIndexToRow(const u32 pageIndex)
        {
          return (pageIndex / CARD_ROWS);
        }

  void  PageIndexToLocation(const u32 pageIndex, u32& rColumn, u32& rRow)
        {
          rColumn = PageIndexToColumn(pageIndex);
          rRow    = PageIndexToRow(pageIndex);
        }

  u32   LocationToPageIndex(const u32 column, const u32 row)
        {
          return (row * CARD_ROWS + column);
        }


} /*  namespace (noname)  */



// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/03/17 Thu. 12:22:58
  */
//==============================================================================
class LowerView_Album::AlbumCursorController
  : public  app::tool::CursorController
{
  GFL_FORBID_COPY_AND_ASSIGN(AlbumCursorController);
public:

  AlbumCursorController(app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager, LowerView_Album* pOwner)
    : CursorController(pHeap, pRenderingManager)
    , m_pOwner(pOwner)
  {}

  virtual ~AlbumCursorController()  {}

  void  Reset(void) {MoveTo(0);}


protected:
  /*  override  */
  virtual app::ui::UIInputListener::ListenerResult  OnUp(   const bool isTrigger = false);
  virtual app::ui::UIInputListener::ListenerResult  OnDown( const bool isTrigger = false);
  virtual app::ui::UIInputListener::ListenerResult  OnLeft( const bool isTrigger = false);
  virtual app::ui::UIInputListener::ListenerResult  OnRight(const bool isTrigger = false);

  LowerView_Album*  m_pOwner;
};



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Album::AlbumCursorController::OnUp(   const bool isTrigger)
{
  u32 column, row;

  PageIndexToLocation(m_SelectedIndex, column, row);

  /*  上端ではtrigger時のみ  */
  if(row || isTrigger)
  {
    for(u32 num = 0; num < (CARD_ROWS-1); ++num)
    {
      row += (CARD_ROWS-1);
      row %= CARD_ROWS;
      {
        const u32 tempIndex = LocationToPageIndex(column, row);
        if(tempIndex < m_SelectedIndexMax)
        {
          MoveTo(tempIndex);
          Sound::PlaySE(SEQ_SE_SELECT1);
          break;
        }
      }
    }
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Album::AlbumCursorController::OnDown( const bool isTrigger)
{
  u32 column, row;

  PageIndexToLocation(m_SelectedIndex, column, row);

  /*  下端ではtrigger時のみ  */
  if((row != (CARD_ROWS-1)) || isTrigger)
  {
    for(u32 num = 0; num < (CARD_ROWS-1); ++num)
    {
      ++row;
      row %= CARD_ROWS;
      {
        const u32 tempIndex = LocationToPageIndex(column, row);
        if(tempIndex < m_SelectedIndexMax)
        {
          MoveTo(tempIndex);
          Sound::PlaySE(SEQ_SE_SELECT1);
          break;
        }
      }
    }
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Album::AlbumCursorController::OnLeft( const bool isTrigger)
{
  u32 column, row;

  PageIndexToLocation(m_SelectedIndex, column, row);


  if(column)
  {
    MoveTo(LocationToPageIndex(--column, row));
    Sound::PlaySE(SEQ_SE_SELECT1);
  }
  else
  {
    /*  左端ならページ戻り、カーソルを右端へ  */
    if((m_pOwner->m_Page) || (isTrigger))   /*  ページのループはTrigger時のみ  */
    {
      if(m_pOwner->TransitionPage(false))
      {
        u32   tempIndex = 0;

        m_pOwner->RefreshView();
        column    = CARD_COLS-1;
        tempIndex = LocationToPageIndex(column, row);
        tempIndex = (tempIndex < m_SelectedIndexMax) ? tempIndex : (m_SelectedIndexMax-1);    /*  補正  */

        MoveTo(tempIndex);
        Sound::PlaySE(SEQ_SE_SELECT1);
      }
      else
      {
        /*  ページ内WrapAroundを試す  */
        for(u32 tempColumn = (CARD_ROWS-1); column < tempColumn; --tempColumn)   /*  右端から順に  */
        {
          const u32 tempIndex = LocationToPageIndex(tempColumn, row);
          
          if(tempIndex < m_SelectedIndexMax)
          {
            MoveTo(tempIndex);
            Sound::PlaySE(SEQ_SE_SELECT1);
            break;
          }
        }
      }
    }
  }
  
  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Album::AlbumCursorController::OnRight(const bool isTrigger)
{
  u32 column, row;
  u32 tempIndex;

  PageIndexToLocation(m_SelectedIndex, column, row);

  /*  ページ内移動を試す  */
  if(column < (CARD_COLS-1))
  {
    tempIndex = LocationToPageIndex(column+1, row);
    if(tempIndex < m_SelectedIndexMax)
    {
      MoveTo(tempIndex);
      Sound::PlaySE(SEQ_SE_SELECT1);
      return app::ui::UIInputListener::DISABLE_ACCESS;
    }
  }


  /*  右端ならページ送り、カーソルを左端へ  */
  if((m_pOwner->m_Page != (GetPageNum(m_pOwner->m_DataNum, m_pOwner->m_PageItems)-1)) || (isTrigger))    /*  ページのループはTrigger時のみ  */
  {
    if(m_pOwner->TransitionPage(true))
    {
      tempIndex = 0;

      m_pOwner->RefreshView();
      column    = 0;
      tempIndex = LocationToPageIndex(column, row);
      /*  補正  */
      if(m_SelectedIndexMax <= tempIndex)
      {
        u32   tempColumn, tempRow;
        PageIndexToLocation(m_SelectedIndexMax-1, column, row);
        tempIndex = LocationToPageIndex(0, row);;
      }

      MoveTo(tempIndex);
      Sound::PlaySE(SEQ_SE_SELECT1);
    }
    else
    {
      /*  ページ内WrapAroundを試す  */
      for(u32 tempColumn = 0; tempColumn < column; ++tempColumn)
      {
        const u32 tempIndex = LocationToPageIndex(tempColumn, row);
        
        if(tempIndex < m_SelectedIndexMax)
        {
          MoveTo(tempIndex);
          Sound::PlaySE(SEQ_SE_SELECT1);
          break;
        }
      }
    }
  }

  return app::ui::UIInputListener::DISABLE_ACCESS;
}




//------------------------------------------------------------------
/**
  *  @func     LowerView_Album
  *  @brief    ctor
  */
//------------------------------------------------------------------
LowerView_Album::LowerView_Album(GiftWork*  pWork)
  : NetAppLib::UI::NetAppCursorView( pWork )
  , m_pWork(pWork)
  , m_pMessageMenu(NULL)
  , m_pInfoWindow(NULL)
  , m_pPane_Null_PageNum(NULL)
  , m_pAlbumCursorController(NULL)
  , m_pHandler(NULL)
  , m_pAlbumDataProvider(NULL)
  , m_DataNum(0)
  , m_Page(0)
  , m_PageItems(CARD_NUM)
  , m_SelectedIndex(0)
  , m_StackCardIndex(0)
  , m_Progress(0)
  , m_IsViewDirty(true)
  , m_IsDeleteMode(false)
  , m_bCardFaceIsFront(true)
  , m_IsCardPushed(false)
  /*  swap  */
  , m_bPlayingSwapAnimA(false)
  , m_bPlayingSwapAnimB(false)
  , m_SwapAnim_ID1(UINT_MAX)
  , m_SwapAnim_ID2(UINT_MAX)
  /*  delete  */
  , m_bPlayingDeleteAnim(false)
  , m_DeleteAnim_ID(UINT_MAX)
{
  app::util::Heap*      pHeap     = m_pWork->GetAppHeap();
  gfl2::heap::HeapBase* pHeapBase = pHeap->GetDeviceHeap();

  /*  setupLayoutSystem  */
  {
    NetAppLib::System::ResourceManager* pResourceManager  = m_pWork->GetResourceManager();
    void*                               pLayoutBuffer     = pResourceManager->GetResourceBuffer(RESOURCE_Layout);

    const app::util::G2DUtil::LytResData  lytResDataTable[] = {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[LAYOUTWORK_MAX] =
    {
      {
        0,                                    //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_GIFT_GIFT_CRD_LOW_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_GIFT_GIFT_SLC_LOW_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                        //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                 //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_LOWER,      //!< ディスプレイ
        true                                  //!< 表示設定
      },
    };


    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pHeap, false );

    Create2D(
      pHeap,                            /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      LAYOUTWORK_MAX,                   /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル用素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル用素数    */
      m_pWork->GetMessageData(),        /*  gfl2::str::MsgData *messageData   */
      m_pWork->GetWordSet()             /*  print::WordSet *wordset,          */
    );

    app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(LAYOUTWORK_CardSelectLow), &(GetG2DUtil()->GetLayoutResourceID( 0 )));

    /*  pane取得  */
    {
      m_pTextBoxes[TEXTBOX_Title]         = helper.Clear().GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_00);  /**<  タイトルバー                */
      m_pTextBoxes[TEXTBOX_CurrentPage]   = helper.Clear().GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_01);  /**<  現在のページ                */
      m_pTextBoxes[TEXTBOX_PageSeparator] = helper.Clear().GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_03);  /**<  ページ/ページ数間セパレータ */
      m_pTextBoxes[TEXTBOX_PageNum]       = helper.Clear().GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_02);  /**<  ページ数                    */

      m_pPane_Null_PageNum                = helper.Clear().GetPane(PANENAME_GIFT_CRD_LOW_000_PANE_NULL_PAGENUM);
    }

    /*  button  */
    CreateButton(
      pHeap,
      this,
      LAYOUTWORK_CardSelectLow,
      0,
      buttonHelperInfo,
      GFL_NELEMS(buttonHelperInfo)
      );
    SetInputListener( this );
    {
      app::tool::ButtonManager* pButtonManager  = GetButtonManager();
      
      for(u32 index = 0; index < BUTTON_MAX; ++index)
      {
        pButtonManager->SetButtonSelectSE(index, SEID_DECIDE);
      }
      pButtonManager->SetButtonSelectSE(BUTTON_Back, SEID_CANCEL);
    }



    /*  cursor  */
    {
      m_pAlbumCursorController  = GFL_NEW(pHeapBase)  AlbumCursorController(pHeap, m_pWork->GetAppRenderingManager(), this);
      SetCursorController(m_pAlbumCursorController);
      CreateCursor(RESOURCE_LayoutCommonMenuCursor, 0, 0);

      m_pCursorPosPanes[CARD_00]  = helper.Clear().Push(PANENAME_GIFT_CRD_LOW_000_PANE_CARD_00).GetPane(PANENAME_GIFT_BTN_LOW_000_PANE_NULL_CUR);
      m_pCursorPosPanes[CARD_01]  = helper.Clear().Push(PANENAME_GIFT_CRD_LOW_000_PANE_CARD_01).GetPane(PANENAME_GIFT_BTN_LOW_000_PANE_NULL_CUR);
      m_pCursorPosPanes[CARD_02]  = helper.Clear().Push(PANENAME_GIFT_CRD_LOW_000_PANE_CARD_02).GetPane(PANENAME_GIFT_BTN_LOW_000_PANE_NULL_CUR);
      m_pCursorPosPanes[CARD_03]  = helper.Clear().Push(PANENAME_GIFT_CRD_LOW_000_PANE_CARD_03).GetPane(PANENAME_GIFT_BTN_LOW_000_PANE_NULL_CUR);
    }

    RegisterShortcut_L(BUTTON_Backward);
    RegisterShortcut_R(BUTTON_Forward);
    RegisterShortcut_B(BUTTON_Back);
    RegisterShortcut_X(BUTTON_FlipCard);
  }




  /*  cardview  */
  m_pCardViews[CARD_00]  = GFL_NEW(pHeapBase) AlbumCardView(m_pWork, pHeap, this, LAYOUTWORK_CardSelectLow, 0, PANENAME_GIFT_CRD_LOW_000_PANE_CARD_00);
  m_pCardViews[CARD_01]  = GFL_NEW(pHeapBase) AlbumCardView(m_pWork, pHeap, this, LAYOUTWORK_CardSelectLow, 0, PANENAME_GIFT_CRD_LOW_000_PANE_CARD_01);
  m_pCardViews[CARD_02]  = GFL_NEW(pHeapBase) AlbumCardView(m_pWork, pHeap, this, LAYOUTWORK_CardSelectLow, 0, PANENAME_GIFT_CRD_LOW_000_PANE_CARD_02);
  m_pCardViews[CARD_03]  = GFL_NEW(pHeapBase) AlbumCardView(m_pWork, pHeap, this, LAYOUTWORK_CardSelectLow, 0, PANENAME_GIFT_CRD_LOW_000_PANE_CARD_03);

  /*  subview  */
  {
    /*
      this
        +- info
            +- message
    */

    /*  info  */
    m_pInfoWindow = GFL_NEW(pHeapBase) LowerView_InfoWindow(m_pWork);
    AddSubView(m_pInfoWindow);

    /*  messagemenu  */
    m_pMessageMenu  = GFL_NEW(pHeapBase) GiftMessageMenu(m_pWork, this);
    m_pInfoWindow->AddSubView(m_pMessageMenu);
  }



  /*  表示設定  */
  SetText(TEXTBOX_Title,          msg_mystery_cap_27);
  SetText(TEXTBOX_PageSeparator,  msg_mystery_cap_13);

}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
LowerView_Album::~LowerView_Album()
{
  for(int num = 0; num < GFL_NELEMS(m_pCardViews); ++num)
  {
    GFL_SAFE_DELETE(m_pCardViews[num]);
  }

  if(m_pInfoWindow)   m_pInfoWindow->RemoveFromSuperView();
  if(m_pMessageMenu)  m_pMessageMenu->RemoveFromSuperView();


  GFL_SAFE_DELETE(m_pInfoWindow);
  GFL_SAFE_DELETE(m_pMessageMenu);
  GFL_SAFE_DELETE(m_pAlbumCursorController);
  ReleaseCursor();
  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @func     InvalidateView
  *  @brief    Viewを無効化し、次回更新時にDataProviderからのDataによってViewを更新する
  */
//------------------------------------------------------------------
void  LowerView_Album::InvalidateView(void)
{
  m_IsViewDirty = true;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::ShowMessage_CardDeleted(const bool isNoHide)
{
  m_pMessageMenu->Show(isNoHide ? GiftMessageMenu::MODE_Album_DeletedCard_NoHide : GiftMessageMenu::MODE_Album_DeletedCard);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Album::ShowMessage_Saving(void)
{
  m_pMessageMenu->Show(GiftMessageMenu::MODE_Album_Saving);
}



//--------------------------------------------------------------------------------------------
/**
* @brief  更新処理
*         子ウインドウについても自動的にコールする(子→親の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_Album::Update(void)
{
  app::util::G2DUtil*   pG2DUtil      = GetG2DUtil();

  NetAppLib::UI::NetAppCursorView::Update();    /*  super  */


  RefreshView();

  for(int num = 0; num < GFL_NELEMS(m_pCardViews); ++num)
  {
    m_pCardViews[num]->Update();
  }

  /*  特殊アニメ  */
  /*  swap  */
  if(m_bPlayingSwapAnimA)
  {
    const bool  isAnim1End  = (m_SwapAnim_ID1 == UINT_MAX) || (pG2DUtil->IsAnimeEnd(0, m_SwapAnim_ID1));
    const bool  isAnim2End  = (m_SwapAnim_ID2 == UINT_MAX) || (pG2DUtil->IsAnimeEnd(0, m_SwapAnim_ID2));

    if(isAnim1End && isAnim2End)
    {
      m_SwapAnim_ID1      = UINT_MAX;
      m_SwapAnim_ID2      = UINT_MAX;
      m_bPlayingSwapAnimA = false;
      Event(IEventHandler::EVENT_OnSwapAnimAEnd);
    }
  }
  else if(m_bPlayingSwapAnimB)
  {
    const bool  isAnim1End  = (m_SwapAnim_ID1 == UINT_MAX) || (pG2DUtil->IsAnimeEnd(0, m_SwapAnim_ID1));
    const bool  isAnim2End  = (m_SwapAnim_ID2 == UINT_MAX) || (pG2DUtil->IsAnimeEnd(0, m_SwapAnim_ID2));

    if(isAnim1End && isAnim2End)
    {
      m_SwapAnim_ID1      = UINT_MAX;
      m_SwapAnim_ID2      = UINT_MAX;
      m_bPlayingSwapAnimB = false;
      Event(IEventHandler::EVENT_OnSwapAnimBEnd);
    }
  }
  else if(m_bPlayingDeleteAnim)
  {
    const bool  isAnimEnd  = (m_DeleteAnim_ID == UINT_MAX) || (pG2DUtil->IsAnimeEnd(0, m_DeleteAnim_ID));

    if(isAnimEnd)
    {
      m_bPlayingDeleteAnim  = false;
      m_DeleteAnim_ID       = UINT_MAX;
      Event(IEventHandler::EVENT_OnDeleteAnimEnd);
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief  描画処理
*         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  LowerView_Album::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_CardSelectLow);

  NetAppCursorView::Draw(displayNo);    /*  super  */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  LowerView_Album::IsDrawing(void) const
{
  const bool  isDrawingBase   = NetAppLib::UI::NetAppCursorView::IsDrawing();
  bool        isEndCardViews  = true;

  for(u32 index = 0; index < GFL_NELEMS(m_pCardViews); ++index)
  {
    isEndCardViews  &= m_pCardViews[index]->EndFunc();
  }

  return  (isDrawingBase && (!isEndCardViews));
}


//--------------------------------------------------------------------------------------------
/**
  * @brief ペインイベントの検知
  */
//--------------------------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult  LowerView_Album::OnLayoutPaneEvent( const u32 paneID )
{
  app::ui::UIInputListener::ListenerResult  result  = app::ui::UIInputListener::ENABLE_ACCESS;


  GFL_PRINT("button[%02d] event\n", paneID);
  switch(paneID)
  {
    case  BUTTON_Card00:    /**<  カード  */
      m_SelectedIndex = m_Page * m_PageItems + 0;
      OnSelectedCard();
      Event(IEventHandler::EVENT_SelectedItem);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Card01:    /**<  カード  */
      m_SelectedIndex = m_Page * m_PageItems + 1;
      OnSelectedCard();
      Event(IEventHandler::EVENT_SelectedItem);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Card02:    /**<  カード  */
      m_SelectedIndex = m_Page * m_PageItems + 2;
      OnSelectedCard();
      Event(IEventHandler::EVENT_SelectedItem);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Card03:    /**<  カード  */
      m_SelectedIndex = m_Page * m_PageItems + 3;
      OnSelectedCard();
      Event(IEventHandler::EVENT_SelectedItem);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Backward:  /**<  逆方向  */
      TransitionPage(false);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    case  BUTTON_Forward:   /**<  順方向  */
      TransitionPage(true);
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;


    case  BUTTON_FlipCard:
      Flip();
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;


    /*
      exit
    */
    case  BUTTON_Back:  /**<  戻る  */
      /*  入れ替えモード時は入れ替え処理のキャンセル  */
      if(IsCardPushed())
      {
        PopCard();
        m_pMessageMenu->HideMessage();
      }
      else
      {
        Event(IEventHandler::EVENT_Exit);
      }
      result  = app::ui::UIInputListener::DISABLE_ACCESS;
      break;

    default:
      break;
  }


  return  result;
}


//------------------------------------------------------------------
/**
  *  @func     SetCardText
  *  @brief    カードのテキストを設定する
  */
//------------------------------------------------------------------
void  LowerView_Album::SetCardText(const CardID cardID, const u32 index, const u32 year, const u32 month, const u32 day)
{
  m_pCardViews[cardID]->SetCardText(index, year, month, day);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::SetCardPokeIcon(const CardID cardID, const pml::pokepara::PokemonParam& rPokemonParam)
{
//  m_pCardViews[cardID]->SetPokeIcon(rPokemonParam);
}


//------------------------------------------------------------------
/**
  *  @func     RefreshView
  *  @brief    表示内容の更新（カレントページ）
  */
//------------------------------------------------------------------
bool  LowerView_Album::RefreshView(const bool isSkipResetAnime)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();


  if(!m_IsViewDirty)        return false;
  if(!m_pAlbumDataProvider) return false;



  /*  データ数更新  */
  m_DataNum = m_pAlbumDataProvider->AlbumDataProvider_DataNum();

  /*  ページ番号表示  */
  m_pPane_Null_PageNum->SetVisible((m_DataNum != 0));


  /*  必要に応じてpage戻し  */
  while(m_Page && (!IsValidPage(m_Page)))
  {
    TransitionPage(false);
  }

  u32                               itemIndex  = m_Page * m_PageItems;
  ICardAlbumDataProvider::CardInfo  cardInfo;
  u32                               cards = 0;


  /*  cards  */
  {
    for(u32 num = 0; num < m_PageItems; ++num, ++itemIndex)  {
      m_pAlbumDataProvider->AlbumDataProvider_GetCardInfo(cardInfo, itemIndex);

      if(itemIndex < m_DataNum)
      {
        SetupCardIcon(m_pCardViews[num], cardInfo);
        m_pCardViews[num]->SetCardText(itemIndex+1, cardInfo.year, cardInfo.month, cardInfo.day);
        m_pCardViews[num]->SetStampVisibility(cardInfo.isReceived);
        m_pCardViews[num]->SetVisibility(true);
//        m_pCardViews[num]->SetPushState(IsCardPushed() && (itemIndex == GetPushedIndex()));

        if(num < GFL_NELEMS(cardAnimTable))
        {
          /*  色  */
          if(cardInfo.rarity  == ICardAlbumDataProvider::CardInfo::RARITY_Gold)         pG2DUtil->StartAnime(0, cardAnimTable[num].anim_Gold);
          else if(cardInfo.rarity  == ICardAlbumDataProvider::CardInfo::RARITY_Silver)  pG2DUtil->StartAnime(0, cardAnimTable[num].anim_Silver);
          else                                                                          pG2DUtil->StartAnime(0, cardAnimTable[num].anim_Bronze);
          
          /*  選択状態  */
          if(IsCardPushed() && (itemIndex == GetPushedIndex()))
          {
            const u32 pushedAnimID  = cardAnimTable[num].anim_Pushed;
            
            if(pG2DUtil->IsAnimeEnd(0, pushedAnimID))
            {
              pG2DUtil->StartAnime(0, pushedAnimID);
            }
          }
          else
          {
            pG2DUtil->StopAnime(0, cardAnimTable[num].anim_Pushed);
            if(!isSkipResetAnime)
            {
              pG2DUtil->StartAnime(0, cardAnimTable[num].anim_Reset);
            }
          }
        }

        ++cards;
      }
      else
      {
        m_pCardViews[num]->SetVisibility(false);
      }
    }
  }

  /*  page  */
  {
    const u32 pageNum = GetPageNum(m_DataNum, m_PageItems);
    
    SetPageText(m_Page+1, pageNum);

    if(1 < pageNum)
    {
      GetButtonManager()->SetButtonVisible(BUTTON_Forward);
      GetButtonManager()->SetButtonVisible(BUTTON_Backward);
    }
    else
    {
      GetButtonManager()->SetButtonInvisible(BUTTON_Forward);
      GetButtonManager()->SetButtonInvisible(BUTTON_Backward);
    }
  }

  /*  cursor設定  */
  {
    const u32 buttonIDs[] = {
                BUTTON_Card00,    /**<  カード  */
                BUTTON_Card01,    /**<  カード  */
                BUTTON_Card02,    /**<  カード  */
                BUTTON_Card03,    /**<  カード  */
              };

    GFL_ASSERT(cards <= GFL_NELEMS(buttonIDs));
    
    SetCusorPosPanes(m_pCursorPosPanes, buttonIDs, cards);
  }

  m_IsViewDirty = false;
  pG2DUtil->Update2D();

  return true;
}


//------------------------------------------------------------------
/**
  *  @func     SetupCardIcon
  */
//------------------------------------------------------------------
void  LowerView_Album::SetupCardIcon(AlbumCardView* pCardView, const ICardAlbumDataProvider::CardInfo& rCardInfo)
{
  switch(rCardInfo.giftType)
  {
    case  ICardAlbumDataProvider::CardInfo::GIFTTYPE_Pokemon:
      pCardView->SetPokeIcon(rCardInfo.pPokeParaCoreParam);
      break;

    case  ICardAlbumDataProvider::CardInfo::GIFTTYPE_Item:
      pCardView->SetItemIcon(rCardInfo.itemNo);
      break;

    case  ICardAlbumDataProvider::CardInfo::GIFTTYPE_Others:    /*  through  */
    default:
      pCardView->UnsetIcon();
      break;
  }
}


//------------------------------------------------------------------
/**
  *  @func     TransitionPage
  *  @brief    ページ遷移
  */
//------------------------------------------------------------------
bool  LowerView_Album::TransitionPage(const bool isForward)
{
  const u32 pageNum     = GetPageNum(m_DataNum, m_PageItems);
  const u32 oldPage     = m_Page;
  bool      bTransited  = false;

  GFL_ASSERT(pageNum > 0);

  if(isForward)
  {
    /*  順方向  */
    
    
//    if((m_Page+1) < pageNum)
    {
      ++m_Page;
      m_Page  %= pageNum;
    }
  }
  else
  {
    /*  逆方向  */

//    if((m_Page > 0))
    {
      m_Page += (pageNum-1);
      m_Page  %= pageNum;
    }
  }


  bTransited  = (oldPage != m_Page);
  if(bTransited)
  {
    InvalidateView();
  }

  return bTransited;
}



//------------------------------------------------------------------
/**
  *  @func     SetupViewMode
  *  @brief    モード切り替え
  */
//------------------------------------------------------------------
void  LowerView_Album::SetViewMode(const ViewMode viewMode) {
//  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();

  switch(viewMode)  {
    case  VIEWMODE_CardSelect:    /**<  カード選択  */
//      pG2DUtil->StartAnime( LAYOUTWORK_SelectionLow, LA_GIFT_GIFT_SLC_LOW_000_BUTTON_PATERN_LIST);
      break;

    default:
      GFL_PRINT("%s:[warning]:invalid viewmode (%d)\n", __FUNCTION__, static_cast<u32>(viewMode));
      break;
  };
}


//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  LowerView_Album::SetText(const TextBoxID textBoxID, const u32 messageID)
{
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[textBoxID], messageID);
}


//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  LowerView_Album::SetPageText(const u32 page, const u32 pageMax)
{
  app::util::G2DUtil*   pG2DUtil      = GetG2DUtil();
  gfl2::lyt::LytWk*     pLayout       = pG2DUtil->GetLayoutWork(LAYOUTWORK_CardSelectLow);
  const u32             digits        = 2;

  SetTextboxPaneNumber(pLayout->GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_01), msg_mystery_cap_14, page,    digits);
  SetTextboxPaneNumber(pLayout->GetTextBoxPane(PANENAME_GIFT_CRD_LOW_000_PANE_TEXTBOX_02), msg_mystery_cap_12, pageMax, digits);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::Event(const IEventHandler::EventCode event)
{
  if(m_pHandler)
  {
    m_pHandler->LowerView_Album_OnEvent(this, event);
  }
}

//------------------------------------------------------------------
/**
  *  @brief    MessageMenuView::IEventHandler
  */
//------------------------------------------------------------------
void  LowerView_Album::MessageMenuView_OnEvent(const GiftMessageMenu::IEventHandler::EventCode event)
{
  switch(event)
  {
    case  GiftMessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
      Event(IEventHandler::EVENT_MessageFinish);
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
    {
      const u32 selectedIndex = m_pMessageMenu->GetSelectedItemIndex();

      if(selectedIndex == 0)       {Flip();}                                          /*  おもて/うらをみる  */
      else if(selectedIndex == 1)  {
        OnTryCardDelete();
      }
      else if(selectedIndex == 2)
      {
        if(IsEnableSwap())
        {
          /*  入れ替えモード         */
          PushCard();
          m_StackCardIndex = m_SelectedIndex;
        }
        else
        {
          /*  戻る               */
          /*  nop  */
        }
      }
      else if(selectedIndex == 3)  {/*  nop  */}   /*  戻る               */
    }
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
    {
      if(m_pMessageMenu->GetMode() == GiftMessageMenu::MODE_Album_DeleteCardYN)
      {
        Event(IEventHandler::EVENT_DeleteCard);
      }
      else if(m_pMessageMenu->GetMode() == GiftMessageMenu::MODE_AlbumCleanupExitConfirm)   /*  アルバム整理の拒否  */
      {
        Event(IEventHandler::EVENT_CleanupCancel);
      }
    }
      break;

    case  GiftMessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
      if(m_pMessageMenu->GetMode() == GiftMessageMenu::MODE_Album_DeleteCardYN)
      {
        /*  nop  */
      }
      else if(m_pMessageMenu->GetMode() == GiftMessageMenu::MODE_AlbumCleanupExitConfirm)   /*  アルバム整理の拒否  */
      {
        /*  nop */
      }
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
void  LowerView_Album::OnTryCardDelete(void)
{
  /*  カードを捨てる     */
  if(m_pAlbumDataProvider->AlbumDataProvider_DeleteQuery(GetSelectedIndex()))
  {
    m_pMessageMenu->Show(GiftMessageMenu::MODE_Album_DeleteCardYN);       /*  捨てても良いですか  */
  }
  else
  {
    m_pMessageMenu->Show(GiftMessageMenu::MODE_Album_CannotDeleteCard);   /*  受け取っていないので捨てられない  */
  }
}

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::OnSelectedCard(void)
{
  if(m_IsDeleteMode)
  {
    /*  削除モード  */
    OnTryCardDelete();
  }
  else if(IsCardPushed())
  {
    /*
      カード入れ替えモード時は、SwapEvent
    */
    if(GetSelectedIndex() != GetPushedIndex())
    {
      Event(IEventHandler::EVENT_SwapCard);
    }

    PopCard();
  }
  else
  {
    /*  通常モード  */
    m_pMessageMenu->SetCardSwapEnable(IsEnableSwap());
    m_pMessageMenu->Show(m_bCardFaceIsFront ? GiftMessageMenu::MODE_Album_ChooseOperation_BackFace : GiftMessageMenu::MODE_Album_ChooseOperation_FrontFace);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  LowerView_Album::IsEnableSwap(void) const
{
  return (2 <= m_DataNum);
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Album::SetCardFace(const bool isFront)
{
  m_bCardFaceIsFront  = isFront;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  LowerView_Album::IsValidPage(const u32 page) const
{
  return  (page < GetPageNum(m_DataNum, m_PageItems));
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::Reset(void)
{
  m_DataNum           = 0;
  m_Page              = 0;
  m_SelectedIndex     = 0;
  m_StackCardIndex    = 0;
  m_Progress          = 0;
  m_IsViewDirty       = true;
  m_IsDeleteMode      = false;
  m_bCardFaceIsFront  = true;

  m_pAlbumCursorController->Reset();
  RefreshView();
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::ICardAlbumDataProvider::CardInfo::Clear(void)
{
  giftType            = GIFTTYPE_Others;
  rarity              = RARITY_Bronze;

  pPokeParaCoreParam  = NULL;
  itemNo              = ITEM_DUMMY_DATA;

  year                = 2000;
  month               = 1;
  day                 = 1;
  isReceived          = false;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Album::CursorController_OnEvent(app::tool::CursorController* pSender, const app::tool::CursorController::IEventHandler::EventCode eventCode)
{
  /*  super  */
  NetAppLib::UI::NetAppCursorView::CursorController_OnEvent(pSender, eventCode);

  /*  カーソルが移動した    */
  if(eventCode == app::tool::CursorController::IEventHandler::EVENT_OnMove)
  {
    m_SelectedIndex = m_Page * m_PageItems + m_pAlbumCursorController->GetSelectedIndex();
    Event(IEventHandler::EVENT_MoveCursor);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::Flip(void)
{
  Event(IEventHandler::EVENT_FlipCard);
  m_bCardFaceIsFront  = !m_bCardFaceIsFront;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  LowerView_Album::PushCard(void)
{
  m_IsCardPushed    = true;
  m_StackCardIndex  = m_SelectedIndex;
  InvalidateView();
  Event(IEventHandler::EVENT_OnPushCard);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::PopCard(void)
{
  m_IsCardPushed = false;
  InvalidateView();
  Event(IEventHandler::EVENT_OnPopCard);
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::StartCardSwapAnimA(const u32 targetCardIndex1, const u32 targetCardIndex2)
{
  m_bPlayingSwapAnimA = true;
  m_SwapAnim_ID1      = UINT_MAX;
  m_SwapAnim_ID2      = UINT_MAX;

  if(IsValidIndex(targetCardIndex1))
  {
    if(IsDataIndexInCurrentPage(targetCardIndex1))
    {
      m_SwapAnim_ID1  = cardAnimTable[DataIndexToPageOffset(targetCardIndex1)].anim_SwapA;
      GetG2DUtil()->StartAnime(0, m_SwapAnim_ID1);
    }
  }

  if(IsValidIndex(targetCardIndex2))
  {
    if(IsDataIndexInCurrentPage(targetCardIndex2))
    {
      m_SwapAnim_ID2  = cardAnimTable[DataIndexToPageOffset(targetCardIndex2)].anim_SwapA;
      GetG2DUtil()->StartAnime(0, m_SwapAnim_ID2);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::StartCardSwapAnimB(const u32 targetCardIndex1, const u32 targetCardIndex2)
{
  m_bPlayingSwapAnimB = true;
  m_SwapAnim_ID1      = UINT_MAX;
  m_SwapAnim_ID2      = UINT_MAX;

  if(IsValidIndex(targetCardIndex1))
  {
    if(IsDataIndexInCurrentPage(targetCardIndex1))
    {
      m_SwapAnim_ID1  = cardAnimTable[DataIndexToPageOffset(targetCardIndex1)].anim_SwapB;
      GetG2DUtil()->StartAnime(0, m_SwapAnim_ID1);
    }
  }

  if(IsValidIndex(targetCardIndex2))
  {
    if(IsDataIndexInCurrentPage(targetCardIndex2))
    {
      m_SwapAnim_ID2  = cardAnimTable[DataIndexToPageOffset(targetCardIndex2)].anim_SwapB;
      GetG2DUtil()->StartAnime(0, m_SwapAnim_ID2);
    }
  }

  Sound::PlaySE(SEQ_SE_PAGE2);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  LowerView_Album::StartCardDeleteAnim(const u32 targetCardIndex)
{
  m_bPlayingDeleteAnim = true;
  m_DeleteAnim_ID      = UINT_MAX;

  if(IsValidIndex(targetCardIndex))
  {
    if(IsDataIndexInCurrentPage(targetCardIndex))
    {
      m_DeleteAnim_ID  = cardAnimTable[DataIndexToPageOffset(targetCardIndex)].anim_Delete;
      GetG2DUtil()->StartAnime(0, m_DeleteAnim_ID);
    }
  }

  Sound::PlaySE(SEQ_SE_PF_DELETE);
}



} /*  namespace Gift    */
} /*  namespace NetApp  */


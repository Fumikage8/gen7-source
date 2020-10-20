//==============================================================================
/**
 * @file   GiftAlbumCardView.cpp
 * @date   2015.05.25 Mon.
 * @date   2015/08/03 Mon. 18:58:11
 * @author muto_yu
 *  @brief  ふしぎなおくりもの アルバム カード表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../ApplicationSystem/GiftWork.h"

#include  "../../GiftResource.h"
#include  "./GiftAlbumCardView.h"



namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     AlbumCardView
  *  @brief    ctor
  */
//------------------------------------------------------------------
AlbumCardView::AlbumCardView(
  GiftWork*  pWork,
  app::util::Heap*          pHeap,
  app::ui::UIView*          pUIView,
  u32                       workID,
  u32                       resourceID,
  u32                       partsID
)
  : m_pWork(pWork)
  , m_pUIView(pUIView)
  , m_pG2DUtil(m_pUIView->GetG2DUtil())
  , m_pLayoutWork(m_pG2DUtil->GetLayoutWork(workID))
  , m_rMultiResourceID(m_pG2DUtil->GetLayoutResourceID(resourceID))
  , m_pParts(m_pLayoutWork->GetPartsPane(partsID))
  , m_pPokeIconTool(NULL)
  , m_pItemIconTool(NULL)
  , m_PokeIconBufID(0)
  , m_ItemIconBufID(0)

{
  /*  text  */
  m_pTextBox[TEXTBOX_Index]     = m_pLayoutWork->GetTextBoxPane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_TEXTBOX_00,    &m_rMultiResourceID );
  m_pTextBox[TEXTBOX_Date]      = m_pLayoutWork->GetTextBoxPane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_TEXTBOX_01,    &m_rMultiResourceID );

  /*  picture  */
  m_pPicture[PICTURE_Stamp]     = m_pLayoutWork->GetPicturePane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_STAMPICON,     &m_rMultiResourceID );
  m_pPicture[PICTURE_PokeIcon]  = m_pLayoutWork->GetPicturePane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_PICON,         &m_rMultiResourceID );
  m_pPicture[PICTURE_ItemIcon]  = m_pLayoutWork->GetPicturePane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_ITEMICON,      &m_rMultiResourceID );
  m_pPicture[PICTURE_BoxIcon]   = m_pLayoutWork->GetPicturePane(m_pParts, PANENAME_GIFT_BTN_LOW_000_PANE_PRESENT_BOX,   &m_rMultiResourceID );

  /*  pokeIcon  */
  {
    app::tool::PokeIconToolEx::PokeIconToolParam  pokeIconToolParam;

    pokeIconToolParam.pUIView   = m_pUIView;
    pokeIconToolParam.pHeap     = pHeap;
    pokeIconToolParam.layout_id = workID;
    pokeIconToolParam.res_id    = resourceID;
    pokeIconToolParam.iconMax   = 2;
    pokeIconToolParam.sync      = true;

    m_pPokeIconTool = GFL_NEW( pHeap->GetDeviceHeap() ) app::tool::PokeIconToolEx( pokeIconToolParam);
  }

  /*  ItemIcon  */
  {
    app::tool::ItemIconToolEx::ItemIconToolParam  itemIconToolParam;
    
    itemIconToolParam.pUIView   = m_pUIView;
    itemIconToolParam.pHeap     = pHeap;
    itemIconToolParam.layout_id = workID;
    itemIconToolParam.res_id    = resourceID;
    itemIconToolParam.iconMax   = 2;
    itemIconToolParam.sync      = true;

    m_pItemIconTool = GFL_NEW( pHeap->GetDeviceHeap() ) app::tool::ItemIconToolEx( itemIconToolParam);
  }
  

}


//------------------------------------------------------------------
/**
  *  @func     ~AlbumCardView
  *  @brief    dtor
  */
//------------------------------------------------------------------
AlbumCardView::~AlbumCardView(void)
{
  GFL_SAFE_DELETE(m_pItemIconTool);
  GFL_SAFE_DELETE(m_pPokeIconTool);
}


//------------------------------------------------------------------
/**
  *  @brief    下位モジュールを終了させる
  */
//------------------------------------------------------------------
bool  AlbumCardView::EndFunc(void)
{
  return (m_pPokeIconTool->EndFunc() && m_pItemIconTool->EndFunc());
}



//------------------------------------------------------------------
/**
  *  @func     Update
  *  @brief    状態更新
  */
//------------------------------------------------------------------
void  AlbumCardView::Update(void)
{
  m_pPokeIconTool->Update();
  m_pItemIconTool->Update();
}


//------------------------------------------------------------------
/**
  *  @func     SetCardText
  *  @brief    カードに表示するためのテキストを設定
  */
//------------------------------------------------------------------
void  AlbumCardView::SetCardText(const u32 index, const u32 year, const u32 month, const u32 day)
{
  /*  id  */
  {
    const u32 digits  = 2;
    
    m_pG2DUtil->SetTextBoxPaneNumber(m_pTextBox[TEXTBOX_Index], msg_mystery_cap_14, index, digits, 0, print::NUM_DISP_ZERO);
  }
  /*  date  */
  {
    /**
      @fix  MMcat[401]   ポケモンのふしぎカードがある状態でカードアルバムを選択すると、とまる
        3つの数値タグを含むメッセージデータに対しSetTextBoxPaneNumberを3回に分けてコールしていたため、タグ1と2の展開時には
        前回セットされた文字列が使用されていた
        場合によってはバッファ長が足りないとAssertが報告されていた
        SetTextBoxPaneStringExpand前に各タグに対しRegisterを行うよう修正
    */

    const u32                   digits    = 2;
    const print::NumberDispType dispType  = print::NUM_DISP_ZERO;
    const print::NumberCodeType codeType  = print::NUM_CODE_HANKAKU;

    m_pG2DUtil->SetRegisterNumber(0, year%100, digits, dispType, codeType);
    m_pG2DUtil->SetRegisterNumber(1, month%13, digits, dispType, codeType);
    m_pG2DUtil->SetRegisterNumber(2, day%32,   digits, dispType, codeType);
    m_pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBox[TEXTBOX_Date], msg_mystery_cap_11);
  }

}


//------------------------------------------------------------------
/**
  *  @func     SetVisibility
  *  @brief    カードの表示状態の設定
  */
//------------------------------------------------------------------
void  AlbumCardView::SetVisibility(const bool isVisible)
{
  m_pParts->SetVisible(isVisible);
}


//------------------------------------------------------------------
/**
  *  @func     SetStampVisibility
  *  @brief    受け取り済みスタンプの表示設定
  */
//------------------------------------------------------------------
void  AlbumCardView::SetStampVisibility(const bool isVisible)
{
  m_pPicture[PICTURE_Stamp]->SetVisible(isVisible);
}


//------------------------------------------------------------------
/**
  *  @func     SetPokeIcon
  *  @brief    アイコン表示するポケモンをMonsNoで指定する
  *  @note     アイコンの生成に使用するMokemonParamの各種パラメータは、PokemonParamクラスの実装に依存する
  */
//------------------------------------------------------------------
void  AlbumCardView::SetPokeIcon(const pml::pokepara::CoreParam* pCoreParam)
{
  m_PokeIconBufID = !m_PokeIconBufID;
  m_pPokeIconTool->SetPokeIcon(m_PokeIconBufID, pCoreParam, m_pPicture[PICTURE_PokeIcon]);

  SetIconMode(ICONMODE_Pokemon);
}

//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  AlbumCardView::SetItemIcon(const u16 itemNo)
{
  m_ItemIconBufID = !m_ItemIconBufID;
  m_pItemIconTool->SetItemIcon(m_ItemIconBufID, itemNo, m_pPicture[PICTURE_ItemIcon] );

  SetIconMode(ICONMODE_Item);
}

//------------------------------------------------------------------
/**
  *  @func     UnsetIcon
  *  @brief    ポケモンアイコン/アイテムアイコンの表示を解除
  */
//------------------------------------------------------------------
void  AlbumCardView::UnsetIcon(void)
{
  SetIconMode(ICONMODE_None);
}

//------------------------------------------------------------------
/**
  *  @func     SetIconMode
  *  @brief    アイコンの表示モードを設定
  */
//------------------------------------------------------------------
void  AlbumCardView::SetIconMode(const IconMode iconMode)
{
  m_pPicture[PICTURE_PokeIcon]->SetVisible(iconMode == ICONMODE_Pokemon);
  m_pPicture[PICTURE_ItemIcon]->SetVisible(iconMode == ICONMODE_Item);
  m_pPicture[PICTURE_BoxIcon]->SetVisible(iconMode  == ICONMODE_None);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  AlbumCardView::SetPushState(const bool isPushed)
{
  m_pParts->SetAlpha(isPushed ? 160 : 0xff);
}


} /*  namespace Gift    */
} /*  namespace NetApp  */

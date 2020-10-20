
#if !defined(GIFTALBUMCARDVIEW_H_INCLUDED)
#define GIFTALBUMCARDVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftAlbumCardView.h
 * @date   2015/08/03 Mon. 18:56:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <niji_conv_header/poke_lib/monsno_def.h>


#include  "AppLib/include/Ui/UIView.h"
#include  "AppLib/include/tool/app_tool_PokeIcon.h"
#include  "AppLib/include/tool/AppToolItemIcon.h"



namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  AlbumCardView
  *  @brief  ふしぎなおくりもの アルバム カード表示
  *  @date   2015.05.22 Fri.
  */
//==============================================================================
class AlbumCardView
{
public:
  /*  各種TextBox  */
  enum  TextBoxID
  {
    TEXTBOX_Index,      /**<  カード番号  */
    TEXTBOX_Date,       /**<  受信年月日  */
    
    TEXTBOX_MAX
  };

  /*  各種PicturePane  */
  enum  PictureID
  {
    PICTURE_Stamp,      /**<  受信済みスタンプ            */
    PICTURE_PokeIcon,   /**<  ポケモンアイコン            */
    PICTURE_ItemIcon,   /**<  アイテムアイコン            */
    PICTURE_BoxIcon,    /**<  プレゼントボックスアイコン  */
    
    PICTURE_MAX
  };


  //------------------------------------------------------------------
  /**
    *  @func     AlbumCardView
    *  @brief    ctor
    *  @param    pHeap      [in]  アイコン表示などに使用するapp::util::Heap
    *  @param    pUIView    [in]  カード表示のためのUIView
    *  @param    workID     [in]  カード表示のためのLytWkID
    *  @param    resourceID [in]  カード表示のためのLytWk内ResourceID
    *  @param    partsID    [in]  カードを構成する「部品」のID
    */
  //------------------------------------------------------------------
  AlbumCardView(
    GiftWork*         m_pWork,
    app::util::Heap*  pHeap,
    app::ui::UIView*  pUIView,
    u32               workID,
    u32               resourceID,
    u32               partsID
  );
  
  
  //------------------------------------------------------------------
  /**
    *  @func     ~AlbumCardView
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~AlbumCardView(void);




  //------------------------------------------------------------------
  /**
    *  @brief    下位モジュールを終了させる
    *  @return   全て終了したらtrue
    */
  //------------------------------------------------------------------
  bool  EndFunc(void);



  //------------------------------------------------------------------
  /**
    *  @func     Update
    *  @brief    状態更新
    *  @note     PokeIconのUpdateなどのため、定期的にコールする
    */
  //------------------------------------------------------------------
  void  Update(void);


  //------------------------------------------------------------------
  /**
    *  @func     SetCardText
    *  @brief    カードに表示するためのテキストを設定
    *  @param    index  [in]  カード番号（アルバム内の通し番号）
    *  @param    year   [in]  受信年
    *  @param    month  [in]  受信月
    *  @param    day    [in]  受信日
    */
  //------------------------------------------------------------------
  void  SetCardText(const u32 index, const u32 year, const u32 month, const u32 day);


  //------------------------------------------------------------------
  /**
    *  @func     SetVisibility
    *  @brief    カードの表示状態の設定
    */
  //------------------------------------------------------------------
  void  SetVisibility(const bool isVisible);

  //------------------------------------------------------------------
  /**
    *  @func     SetStampVisibility
    *  @brief    受け取り済みスタンプの表示設定
    */
  //------------------------------------------------------------------
  void  SetStampVisibility(const bool isVisible);


  //------------------------------------------------------------------
  /**
    *  @func     SetPokeIcon
    *  @brief    アイコン表示するポケモンをMonsNoで指定する
    *  @note     アイコンの生成に使用するMokemonParamの各種パラメータは、MokemonParamクラスの実装に依存する
    */
  //------------------------------------------------------------------
  void  SetPokeIcon(const pml::pokepara::CoreParam* pCoreParam);

  //------------------------------------------------------------------
  /**
    *  @func     SetItemIcon
    *  @brief    アイコン表示するアイテム指定する
    */
  //------------------------------------------------------------------
  void  SetItemIcon(const u16 itemNo);

  //------------------------------------------------------------------
  /**
    *  @func     UnsetIcon
    *  @brief    ポケモンアイコン/アイテムアイコンの表示を解除
    */
  //------------------------------------------------------------------
  void  UnsetIcon(void);


  void  SetPushState(const bool isPushed);

private:
  /** アイコン表示モード  */
  enum  IconMode
  {
    ICONMODE_None,      /**<  なし      */
    ICONMODE_Pokemon,   /**<  ポケモン  */
    ICONMODE_Item,      /**<  アイテム  */
    
    ICONMODE_
  };


  //------------------------------------------------------------------
  /**
    *  @func     SetIconMode
    *  @brief    アイコンの表示モードを設定
    */
  //------------------------------------------------------------------
  void  SetIconMode(const IconMode iconMode);


  /*
    members
  */
  GiftWork*                     m_pWork;
  app::ui::UIView*              m_pUIView;                /**<  表示に使用するUIView                          */
  app::util::G2DUtil*           m_pG2DUtil;               /**<  表示に使用するG2DUtil、m_pUIViewより取得する  */
  gfl2::lyt::LytWk*             m_pLayoutWork;            /**<  表示に使用するLytWk、m_pUIViewより取得する    */
  gfl2::lyt::LytMultiResID&     m_rMultiResourceID;       /**<  表示に使用する、m_pLayoutWork内resourceID     */
  gfl2::lyt::LytParts*          m_pParts;                 /**<  カードを構成する「部品」                      */

  gfl2::lyt::LytTextBox*        m_pTextBox[TEXTBOX_MAX];  /**<  各種TextBoxPane                               */
  gfl2::lyt::LytPicture*        m_pPicture[PICTURE_MAX];  /**<  各種PicturePane                               */

  app::tool::PokeIconToolEx*    m_pPokeIconTool;          /**<  ポケモンアイコン表示ツール                    */
  app::tool::ItemIconToolEx*    m_pItemIconTool;          /**<  アイテムアイコン表示ツール                    */
  u32                           m_PokeIconBufID;
  u32                           m_ItemIconBufID;

};





} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTALBUMCARDVIEW_H_INCLUDED)  */

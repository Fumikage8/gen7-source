
#if !defined(UPPERVIEW_CARD_H_INCLUDED)
#define UPPERVIEW_CARD_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Card.h
 * @date   2015.05.18 Mon.
 * @author muto_yu
 * @brief  ふしぎなおくりもの カード閲覧 上画面描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <str/include/gfl2_StrSys.h>

#include  "AppLib/include/Util/EffectUtil.h"

#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"

#include  "../GiftResource.h"

/*
  forward declare
*/


namespace NetApp  {
namespace Gift    {
  class GiftWork;

// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  UpperView_Card
  *  @brief  Draw: カード閲覧 上画面
  *  @date   2015.05.18 Mon.
  */
//==============================================================================
class UpperView_Card
  : public NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Card );

public:
  /*
    カード種別
  */
  enum  CardType
  {
    CARDTYPE_Silver,      /**<  銀のカード          */
    CARDTYPE_PokeBank,    /**<  ポケバンクのカード  */
    CARDTYPE_Gold,        /**<  金のカード          */

    CARDTYPE_
  };

  //------------------------------------------------------------------
  /**
    *  @func     UpperView_Card
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  UpperView_Card(GiftWork* pWork);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    デストラクタ
  */
  //--------------------------------------------------------------------------------------------
  virtual ~UpperView_Card();


  void  UpdateCardInfo(const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* pGiftData);


  /*
    UIView
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  /*
    end UIView
  */


#if PM_DEBUG
  void  DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo);
#endif



  enum  MessageMode
  {
    MESSAGEMODE_Exit,                 /**<  終わる              */
    MESSAGEMODE_Swap,                 /**<  入れ替えるカードを選んで  */
//    MESSAGEMODE_Information,          /**<  詳しく見る          */
//    MESSAGEMODE_Information_Exit,     /**<  詳しく見る/終わる   */
    
    MESSAGEMODE_
  };
  void  ShowMessage(const MessageMode messageMode);
  void  HideMessage(void);



  void  SetCardFace(const bool isFront, const bool isImmediate = false);


  void  HidePokeModel(void);
  bool  FinallizePokeModel(void);

  //------------------------------------------------------------------
  /**
    *  @func     Flip
    *  @brief    カードを裏返す
    *  @param    toMessageFace  [in]  （表側から）説明側へ裏返すか ※falseで逆方向
    */
  //------------------------------------------------------------------
  void  Flip(const bool toMessageFace, const bool isImmediate);



protected:
  /*
    textbox
  */
  enum  TextBoxID
  {
    TEXTBOX_Title,

    TEXTBOX_Message_Message,       /**<  一行メッセージ  */

    TEXTBOX_CardTitle,             /**<  カードタイトル  */
    TEXTBOX_CardDescription,       /**<  カード説明      */
    TEXTBOX_CardReceiveDateLabel,  /**<  受信日項目名    */
    TEXTBOX_CardReceiveDate,       /**<  受信日          */
    TEXTBOX_CardMessage,           /**<  裏面メッセージ  */
    
    
    TEXTBOX_MAX
  };

  /*
    paneID
  */
  enum  PaneID
  {
    PANE_Message,
    PANE_GiftBoxIcon,
    PANE_Null_Card,
    PANE_PokeFrame,
    
    PANE_MAX
  };


  //------------------------------------------------------------------
  /**
    *  @func     SetText
    *  @brief    TextBoxのTextを設定
    */
  //------------------------------------------------------------------
  void  SetText(const TextBoxID textBoxID, const u32 messageID);
  void  SetText(const TextBoxID textBoxID, const gfl2::str::STRCODE* pStrCode);
  void  SetCardText(const TextBoxID textBoxID, const u32 messageID);



  //------------------------------------------------------------------
  /**
    *  @func     SetCardReceiveDate
    *  @brief    Cardの受信日を設定
    */
  //------------------------------------------------------------------
  void  SetCardReceiveDate(const u16 year, const u8 month, const u8 day);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  RegisterDate(
          const u32 year,
          const u32 month,
          const u32 day
        );

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  gfl2::lyt::LytTextBox*  GetTextBox(const TextBoxID textBoxID);


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  UpdatePokemodelVisibility(void);


  void  ShowEffect(const bool bShow);


  /*
    members
  */
  GiftWork*               m_pWork;
  gfl2::lyt::LytTextBox*  m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*     m_pPanes[PANE_MAX];
  bool                    m_IsFrontFace;
  
  /*  pokemodel  */
  NetAppLib::Util::NetAppPokeModelUtility*      m_pPokeModelUtil;
  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE  m_DataType;

  /*  effect  */
  app::util::EffectUtil                                       m_EffectUtil;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;
  poke_3d::model::BaseCamera*                                 m_pBaseCamera;

};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_CARD_H_INCLUDED)  */


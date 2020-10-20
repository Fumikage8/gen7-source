//======================================================================
/**
 * @file    BoxSearchUpperMessage.cpp
 * @date    2015/07/27 18:18:08
 * @author  fukushima_yuya
 * @brief   ボックス検索：上画面メッセージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchUpperMessage.h"

#include <App/BoxSearch/include/BoxSearchAppParam.h>
#include <App/BoxSearch/source/BoxSearchDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/app/box/res2d/srcupp_pane.h>
#include <niji_conv_header/message/msg_box.h>
#include <niji_conv_header/message/msg_tokuseiinfo.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   マクロ
//------------------------------------------------------------------------------
#if !defined(VISIBLE_PANE)
#define VISIBLE_PANE( pane ) m_pBase->SetVisiblePane( pane, true );
#endif

#if !defined(INVISIBLE_PANE)
#define INVISIBLE_PANE( pane ) m_pBase->SetVisiblePane( pane, false );
#endif


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
UpperMsg::UpperMsg( DrawerBase* base, gfl2::str::MsgData* msgData_Tokusei )
  : m_pBase(base)
  , m_pTokuseiInfo(msgData_Tokusei)
  , m_pMsgWndPane(NULL)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void UpperMsg::Initialize( void )
{
  // ペインのセット
  m_pMsgWndPane = m_pBase->GetPane( PANENAME_BOX_SRC_UPP_000_PANE_NULL_DESC );
  m_pGuideText  = m_pBase->GetTextBox( PANENAME_BOX_SRC_UPP_000_PANE_TEXTBOX_00 );
  m_pNameText   = m_pBase->GetTextBox( PANENAME_BOX_SRC_UPP_000_PANE_TEXTBOX_01 );
  m_pDescText   = m_pBase->GetTextBox( PANENAME_BOX_SRC_UPP_000_PANE_TEXTBOX_23X2 );
  m_pType1Text  = m_pBase->GetTextBox( PANENAME_BOX_SRC_UPP_000_PANE_TEXTBOX_21X1_00 );
  m_pType2Text  = m_pBase->GetTextBox( PANENAME_BOX_SRC_UPP_000_PANE_TEXTBOX_21X1_01 );

  // 初期表示設定
  SetMessage_ItemList();
}



//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(検索項目リスト用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_ItemList( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    view->SetTextboxPaneMessage( m_pDescText, msg_box_search_03_01 );
  }

  // 表示切替
  {
    VISIBLE_PANE( m_pMsgWndPane );
    INVISIBLE_PANE( m_pGuideText );
    INVISIBLE_PANE( m_pNameText );
    VISIBLE_PANE( m_pDescText );
    INVISIBLE_PANE( m_pType1Text );
    INVISIBLE_PANE( m_pType2Text );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(頭文字選択用)
 *
 * @param   listID    リストの種類
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Initial( u32 listID )
{
  u32 msgID = 0;

  switch( listID )
  {
  case SRCID_POKE_NAME:
    msgID = msg_box_search_04_01;
    break;
  case SRCID_WAZA_NAME:
    msgID = msg_box_search_04_05;
    break;
  case SRCID_TOKUSEI:
    msgID = msg_box_search_04_08;
    break;
  default:
    GFL_ASSERT(0);
    msgID = msg_box_search_04_01;
  }

  // ガイドオンリー
  SetGuideOnly( msgID );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(ポケモン名用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_PokeName( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_02 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(タイプ用)
 *
 * @param   listID  リストの種類
 * @param   type1   タイプ１
 * @param   type2   タイプ２
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Type( u32 listID, u8 type1, u8 type2 )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    // ガイド
    if( listID == SRCID_TYPE_1 )
    {
      view->SetTextboxPaneMessage( m_pGuideText, msg_box_search_04_03 );
    }
    else {
      view->SetTextboxPaneMessage( m_pGuideText, msg_box_search_04_04 );
    }

    // タイプ１
    u32 type1MsgID = GetTypeMsg( type1 );
    {
      view->SetTextboxPaneMessage( m_pType1Text, type1MsgID );
    }
    // タイプ２
    u32 type2MsgID = GetTypeMsg( type2 );
    {
      view->SetTextboxPaneMessage( m_pType2Text, type2MsgID );
    }
  }

  // 表示切替
  {
    VISIBLE_PANE( m_pMsgWndPane );
    VISIBLE_PANE( m_pGuideText );
    INVISIBLE_PANE( m_pNameText );
    INVISIBLE_PANE( m_pDescText );
    VISIBLE_PANE( m_pType1Text );
    VISIBLE_PANE( m_pType2Text );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タイプ表示用のメッセージIDを取得
 *
 * @param   type    タイプ
 *
 * @return  "(type == POKETYPE_NULL) = [-]"
 * @return  "(type != POKETYPE_NULL) = タイプ名"
 */
//------------------------------------------------------------------------------
u32 UpperMsg::GetTypeMsg( u8 type )
{
  u32 msgID = msg_box_search_02_00;

  if( type != POKETYPE_NULL )
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    wordSet->RegisterPokeTypeName( 0, type );

    msgID = msg_box_search_02_04;
  }

  return msgID;
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(技名用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_WazaName( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_06 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(技マシン用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_WazaMachine( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_07 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(特性用)
 *
 * @param   tokusei   特性No
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Tokusei( TokuseiNo tokusei )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    // ガイドメッセージ
    view->SetTextboxPaneMessage( m_pGuideText, msg_box_search_04_09 );  // @fix NMCat[2155]

    // 特性名
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterTokuseiName( 0, tokusei );
    }
    view->SetTextboxPaneMessage( m_pNameText, msg_box_search_02_12 );

    // 特性の詳細
    app::util::G2DUtil* g2d = m_pBase->GetG2D();
    {
      g2d->SetMessageData( m_pTokuseiInfo, wordSet );

      view->SetTextboxPaneMessage( m_pDescText, (TOKUSEIINFO_000 + tokusei) );

      g2d->SetMessageData( m_pBase->GetMsgData(), wordSet );
    }
  }

  // 表示切替
  {
    VISIBLE_PANE( m_pMsgWndPane );
    VISIBLE_PANE( m_pGuideText );
    VISIBLE_PANE( m_pNameText );
    VISIBLE_PANE( m_pDescText );
    INVISIBLE_PANE( m_pType1Text );
    INVISIBLE_PANE( m_pType2Text );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(性格用)
 *
 * @param   seikaku   性格ID
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Seikaku( pml::pokepara::Seikaku seikaku )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    // ガイドメッセージ
    view->SetTextboxPaneMessage( m_pGuideText, msg_box_search_04_10 );

    // 性格名
    u32 nameMsgID = PARAM_NONE;

    if( seikaku != BOX_SEARCH_SEIKAKU_NULL )
    {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterSeikakuName( 0, seikaku );
      }
      nameMsgID = msg_box_search_05_05;
    }
    view->SetTextboxPaneMessage( m_pNameText, nameMsgID );

    // 性格の詳細
    u32 descMsgID = PARAM_NONE;

    if( seikaku != BOX_SEARCH_SEIKAKU_NULL )
    {
      descMsgID = static_cast<u32>( seikaku + msg_box_search_08_01 );
    }
    view->SetTextboxPaneMessage( m_pDescText, descMsgID );
  }

  // 表示切替
  {
    VISIBLE_PANE( m_pMsgWndPane );
    VISIBLE_PANE( m_pGuideText );
    VISIBLE_PANE( m_pNameText );
    VISIBLE_PANE( m_pDescText );
    INVISIBLE_PANE( m_pType1Text );
    INVISIBLE_PANE( m_pType2Text );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(性別用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Gender( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_11 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(もちもの用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_Item( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_12 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(ボックスマーク用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_BoxMark( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_13 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(チーム登録用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_TeamEntry( void )
{
  GFL_ASSERT(0);
  //!< 仕様削除20160222
  //// ガイドオンリー
  //SetGuideOnly( msg_box_search_04_14 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの設定(チーム名用)
 */
//------------------------------------------------------------------------------
void UpperMsg::SetMessage_TeamName( void )
{
  // ガイドオンリー
  SetGuideOnly( msg_box_search_04_15 );
}


//------------------------------------------------------------------------------
/**
 * @brief   ガイドオンリーメッセージ
 *
 * @param   msgID   メッセージ
 */
//------------------------------------------------------------------------------
void UpperMsg::SetGuideOnly( u32 msgID )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    view->SetTextboxPaneMessage( m_pGuideText, msgID );
  }

  // 表示切替
  {
    INVISIBLE_PANE( m_pMsgWndPane );
    VISIBLE_PANE( m_pGuideText );
    INVISIBLE_PANE( m_pNameText );
    INVISIBLE_PANE( m_pDescText );
    INVISIBLE_PANE( m_pType1Text );
    INVISIBLE_PANE( m_pType2Text );
  }
}

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

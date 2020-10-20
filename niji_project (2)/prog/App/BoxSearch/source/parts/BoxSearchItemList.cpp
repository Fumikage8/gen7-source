//======================================================================
/**
 * @file    BoxSearchItemList.cpp
 * @date    2016/02/01 17:08:15
 * @author  fukushima_yuya
 * @brief   ボックス検索：検索項目リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchItemList.h"

#include <App/BoxSearch/include/BoxSearchAppParam.h>
#include <App/BoxSearch/source/BoxSearchConst.h>
#include <App/BoxSearch/source/BoxSearchDrawerBase.h>

#include <AppLib/include/Ui/UIView.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/BoxSave.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/box/res2d/srclow_pane.h>
#include <niji_conv_header/app/box/res2d/srclow_anim_list.h>
#include <niji_conv_header/message/msg_box.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  static const u32 ITEM_LIST_BTN_MAX = 10;    //!< 検索項目リストのボタン最大数
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ItemList::ItemList( CreateParam& param )
  : ListBase( param.base, param.cursor )
  , m_pAppParam(param.appParam)
  , m_ListData()
  , m_pBoxSave(param.boxSave)
  , m_pListener(param.listener)
  , m_pContentsText(NULL)
  , m_pBoxMarkPane(NULL)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void ItemList::Initialize( void )
{
  m_ListData.max = SRCID_NUM;

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    // 検索内容テキストボックス
    m_pContentsText = GFL_NEW_ARRAY(heap) gfl2::lyt::LytTextBox*[ITEM_LIST_BTN_MAX];
    // ボックスマーク表示切替ペイン
    m_pBoxMarkPane  = GFL_NEW_ARRAY(heap) gfl2::lyt::LytPane*[ITEM_LIST_BTN_MAX];
  }

  // リストの初期化
  InitializeList();
  // リストの生成
  CreateList();
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
void ItemList::Terminate( void )
{
  DeleteList();

  if( m_pPaneListView )
  {
    m_pPaneListView->RemoveFromSuperView();   // @fix GFNMCat[4752]

    GFL_DELETE( m_pPaneListView );
  }

  GFL_SAFE_DELETE_ARRAY( m_pPaneData );
  GFL_SAFE_DELETE_ARRAY( m_pBoxMarkPane );
  GFL_SAFE_DELETE_ARRAY( m_pContentsText );
}



//------------------------------------------------------------------------------
/**
 * @brief   リストの初期化
 */
//------------------------------------------------------------------------------
void ItemList::InitializeList( void )
{
  //! @brief  検索項目リストパーツインデックス
  static const u32 c_ItemListBtnIndex[] = {
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_00,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_01,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_02,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_03,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_04,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_05,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_06,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_07,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_08,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_L_09,
  };
  //! @brief  検索項目リストボタンアニメ
  static const u32 c_ItemListBtnAnimIndex[] = {
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_00_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_01_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_02_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_03_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_04_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_05_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_06_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_07_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_08_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_09_TOUCH,
  };

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    // ペインデータの初期化
    m_pPaneData = GFL_NEW_ARRAY(heap) app::tool::PaneList::PANE_DATA[ITEM_LIST_BTN_MAX];
    {
      GFL_ASSERT( m_pPaneData );

      for( u32 i=0; i<ITEM_LIST_BTN_MAX; ++i )
      {
        gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_ItemListBtnIndex[i] );

        app::tool::PaneList::PANE_DATA* data = &m_pPaneData[i];
        {
          data->base        = parts;
          data->text        = m_pBase->GetTextBox( PANENAME_BOX_BTN_LOW_001_PANE_TEXTBOX_00, parts );
          data->bound       = m_pBase->GetBounding( PANENAME_BOX_BTN_LOW_001_PANE_BOUND_00, parts );
          data->cursor_pos  = m_pBase->GetPane( PANENAME_BOX_BTN_LOW_001_PANE_NULL_CUR, parts );

          const u32 animID = c_ItemListBtnAnimIndex[i];
          {
            data->holdAnime    = animID + BTN_ANIM_TOUCH;
            data->releaseAnime = animID + BTN_ANIM_RELEASE;
            data->cancelAnime  = animID + BTN_ANIM_CANCEL;
            data->selectAnime  = animID + BTN_ANIM_TOUCH_RELEASE;
          }
        }

        m_pContentsText[i] = m_pBase->GetTextBox( PANENAME_BOX_BTN_LOW_001_PANE_TEXTBOX_01, parts );
        m_pBoxMarkPane[i]  = m_pBase->GetPane( PANENAME_BOX_BTN_LOW_001_PANE_NULL_MARKS, parts );
      }
    }

    // PaneListViewの生成
    m_pPaneListView = GFL_NEW(heap) app::tool::PaneListView( m_pBase->GetHeap(), true );  // @fix GFMMCat[24]: 対処
    {
      GFL_ASSERT( m_pPaneListView );

      m_pPaneListView->SetListener( this );

      // 入力を無効に
      SetInputEnable( false );

      // サブビューへの追加
      m_pBase->GetUIView()->AddSubView( m_pPaneListView );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの生成
 */
//------------------------------------------------------------------------------
void ItemList::CreateList( void )
{
  GFL_ASSERT( m_pPaneListView );

  app::tool::PaneList::SETUP_PARAM list_setup;
  {
    // リストパラメータのセットアップ
    SetListParam( &list_setup );
  }

  // スクロール値の取得
  u32 scrollVal = this->GetScrollValue( list_setup );

  // スクロールバーのセットアップパラメータ
  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup = this->GetDefaultScrollBarParam();
  {
    app::util::ScrollBar::SETUP_PARAM* param = &scroll_bar_setup;
    {
      param->max          = scrollVal;
      param->parts_index  = PANENAME_BOX_SRC_LOW_000_PANE_SCROLL_L;
      param->mark_index   = PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER;
      param->top_index    = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP;
      param->bottom_index = PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM;
    }
  }

  // PaneListViewのセットアップ
  m_pPaneListView->CreatePaneList( &list_setup );
  m_pPaneListView->CreateScrollBar( &scroll_bar_setup );

  // PaneListのセットアップ
  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, true, false );
    paneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, app::tool::PaneList::SE_NONE );

    // タッチエリアの設定
    SetTouchArea( &list_setup );
  }

  // 入力を有効に
  this->SetInputEnable( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定
 *
 * @param   prm   [out]リストパラメータ
 */
//------------------------------------------------------------------------------
void ItemList::SetListParam( app::tool::PaneList::SETUP_PARAM* prm )
{
  prm->heap           = m_pBase->GetDeviceHeap();;
  prm->pLytWk         = m_pBase->GetLytWk();
  prm->paneList       = m_pPaneData;
  prm->paneListMax    = ITEM_LIST_BTN_MAX;
  prm->valMax         = SRCID_NUM;
  prm->listPosMax     = 7;
  prm->listPos        = m_ListData.curPos;
  prm->listScroll     = m_ListData.scroll;
  prm->defPY          = 100;
  prm->defSX          = 282;
  prm->defSY          = 28;
  prm->writeOffsetY   = -1;
  prm->listStartPane  = prm->pLytWk->GetPane( PANENAME_BOX_SRC_LOW_000_PANE_NULL_01 );
  prm->cbFunc         = this;
  prm->cbWork         = NULL;
  prm->uiDevMan       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  prm->menuCursor     = m_pCursor->GetMenuCursor();
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチエリアの設定
 *
 * @param   prm   [in]リストパラメータ
 */
//------------------------------------------------------------------------------
void ItemList::SetTouchArea( const app::tool::PaneList::SETUP_PARAM* prm )
{
  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    gfl2::math::VEC3 pos = m_pPaneData[0].bound->GetTranslate();
    u32 posMax = prm->listPosMax;
    if( prm->valMax < posMax )
    {
      posMax = prm->valMax;
    }

    f32 lx = 0.0f - (prm->defSX / 2.0f);
    f32 rx = lx + prm->defSX;
    f32 uy = prm->defPY + pos.y + (prm->defSY / 2.0f);
    f32 dy = uy - (prm->defSY * posMax);

    // タッチエリアの変更 @fix NMCat[733]
    paneList->MakeTouchArea( lx, rx, uy, dy );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定
 *
 * @param   pane
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    // 項目名メッセージID
    static const s32 c_ItemTitle[] = {
      msg_box_search_02_01,
      msg_box_search_02_03,
      msg_box_search_02_05,
      msg_box_search_02_07,
      msg_box_search_02_09,
      msg_box_search_02_11,
      msg_box_search_02_13,
      msg_box_search_02_15,
      msg_box_search_02_19,
      msg_box_search_02_21,
      //msg_box_search_02_24,  //!< 仕様削除20160222
      msg_box_search_02_27,
    };
    view->SetTextboxPaneMessage( pane[pane_index].text, c_ItemTitle[pos] );
  }

  bool boxMark = (pos != SRCID_MARK);
  {
    m_pBase->SetVisiblePane( m_pContentsText[pane_index], boxMark );
    m_pBase->SetVisiblePane( m_pBoxMarkPane[pane_index], !boxMark );
  }

  void (ItemList::*UPDATE_LIST[SRCID_NUM])( u32 ) = {
    &ItemList::UpdateList_PokeName,
    &ItemList::UpdateList_Type1,
    &ItemList::UpdateList_Type2,
    &ItemList::UpdateList_WazaName,
    &ItemList::UpdateList_WazaMachine,
    &ItemList::UpdateList_Tokusei,
    &ItemList::UpdateList_Seikaku,
    &ItemList::UpdateList_Gender,
    &ItemList::UpdateList_Item,
    &ItemList::UpdateList_BoxMark,
    //&ItemList::UpdateList_TeamEntry,  //!< 仕様削除20160222
    &ItemList::UpdateList_TeamName,
  };
  (this->*UPDATE_LIST[pos])( pane_index );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(ポケモン名)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_PokeName( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    MonsNo monsNo = m_pAppParam->monsNo;

    if( monsNo == MONSNO_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterPokeMonsNameNo( 0, monsNo );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_02 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(タイプ１)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Type1( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    pml::PokeType type = m_pAppParam->type1;

    if( type == POKETYPE_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterPokeTypeName( 0, type );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_04 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(タイプ２)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Type2( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    pml::PokeType type = m_pAppParam->type2;

    if( type == POKETYPE_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterPokeTypeName( 0, type );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_04 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(技名)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_WazaName( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    WazaNo wazaNo = m_pAppParam->wazaNo;

    if( wazaNo == WAZANO_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterWazaName( 0, wazaNo );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_08 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(技マシン)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_WazaMachine( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    u8 index = m_pAppParam->wazaMachine;

    if( index == CONDITIONAL_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        WazaNo wazaNo = static_cast<WazaNo>(item::ITEM_GetWazaMashineWaza( index - 1 ));
        wordSet->RegisterWazaName( 0, wazaNo );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_08 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(特性)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Tokusei( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    TokuseiNo tokusei = m_pAppParam->tokusei;

    if( tokusei == TOKUSEI_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterTokuseiName( 0, tokusei );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_12 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(性格)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Seikaku( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    pml::pokepara::Seikaku seikaku = m_pAppParam->seikaku;

    if( seikaku == BOX_SEARCH_SEIKAKU_NULL )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterSeikakuName( 0, seikaku );
      }
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], msg_box_search_02_14 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(性別)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Gender( u32 pane_index )
{
  // 性別条件メッセージID
  static const s32 c_GenderMsgID[] = {
    msg_box_search_02_16,
    msg_box_search_02_17,
    msg_box_search_02_18,
    PARAM_NONE,
  };

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    pml::Sex sex = m_pAppParam->sex;

    view->SetTextboxPaneMessage( m_pContentsText[pane_index], c_GenderMsgID[sex] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(もちもの)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_Item( u32 pane_index )
{
  // 道具所持条件メッセージID
  static const s32 c_ItemMsgID[] = {
    PARAM_NONE,
    msg_box_search_02_22,
    msg_box_search_02_23,
  };

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    u32 flag = m_pAppParam->item;

    view->SetTextboxPaneMessage( m_pContentsText[pane_index], c_ItemMsgID[flag] );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(ボックスマーク)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_BoxMark( u32 pane_index )
{
  // ボックスマークアニメ
  static const u32 c_BoxMarkAnim[ITEM_LIST_BTN_MAX] = {
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_00_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_01_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_02_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_03_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_04_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_05_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_06_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_07_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_08_MARK_COLOR_00,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_L_09_MARK_COLOR_00,
  };

  for( u32 i=0; i<pml::pokepara::BOX_MARK_NUM; ++i )
  {
    u32 anmID = c_BoxMarkAnim[pane_index] + i;
    u32 color = m_pAppParam->mark.markColor[i];

    m_pBase->StartAnim( anmID, false );
    m_pBase->SetAnimFrame( anmID, static_cast<f32>(color) );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(チーム登録)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_TeamEntry( u32 pane_index )
{
  GFL_ASSERT(0);
  //!< 仕様削除20160222
  //// チーム登録条件メッセージID
  //static const s32 c_EntryMsgID[] = {
  //  PARAM_NONE,
  //  msg_box_search_02_25,
  //  msg_box_search_02_26,
  //};

  //app::ui::UIView* view = m_pBase->GetUIView();
  //{
  //  u32 flag = m_pAppParam->entry;

  //  view->SetTextboxPaneMessage( m_pContentsText[pane_index], c_EntryMsgID[flag] );
  //}
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(チーム名)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ItemList::UpdateList_TeamName( u32 pane_index )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    u32 teamNo = m_pAppParam->teamNo;

    if( teamNo == 0 )
    {
      view->SetTextboxPaneMessage( m_pContentsText[pane_index], PARAM_NONE );
    }
    else {
      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        g2d->SetTextBoxPaneString( m_pContentsText[pane_index], m_pBoxSave->GetTeamName( teamNo - 1 ) );
      }
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   検索条件のリセット
 */
//------------------------------------------------------------------------------
void ItemList::ResetSearchParam( void )
{
  // パラメータのリセット
  ResetParameter();
  // パラメータのリセット
  ResetPaneList();
}

//------------------------------------------------------------------------------
/**
 * @brief   パラメータのリセット
 */
//------------------------------------------------------------------------------
void ItemList::ResetParameter( void )
{
  APP_PARAM prm;
  {
    m_pAppParam->monsNo       = prm.monsNo;
    m_pAppParam->wazaNo       = prm.wazaNo;
    m_pAppParam->type1        = prm.type1;
    m_pAppParam->type2        = prm.type2;
    m_pAppParam->item         = prm.item;
    m_pAppParam->wazaMachine  = prm.wazaMachine;
    m_pAppParam->sex          = prm.sex;
    m_pAppParam->mark         = prm.mark;
    m_pAppParam->seikaku      = prm.seikaku;
    m_pAppParam->tokusei      = prm.tokusei;
    //m_pAppParam->entry        = prm.entry;  //!< 仕様削除20160222
    m_pAppParam->teamNo       = prm.teamNo;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストのリセット
 */
//------------------------------------------------------------------------------
void ItemList::ResetPaneList( void )
{
  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    // カーソルデータの取得
    ListData* ld = &m_ListData;
    {
      paneList->GetCursorData( &ld->curPos, &ld->scroll );
    }

    app::tool::PaneList::SETUP_PARAM list_setup;
    {
      // リストパラメータのセット
      SetListParam( &list_setup );

      // 補正
      AdjustValue( &list_setup, m_ListData.curPos, static_cast<u32>(m_ListData.scroll) );
      {
        m_ListData.curPos = list_setup.listPos;
        m_ListData.scroll = list_setup.listScroll;
      }

      // リストを再設定
      m_pPaneListView->ResetPaneList( &list_setup );
    }

    // 再設定
    paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, true, false );
    paneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, app::tool::PaneList::SE_NONE );

    // タッチエリアの設定
    SetTouchArea( &list_setup );

    // 入力を有効
    SetInputEnable( true );  // @fix NMCat[1459]
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   補正
 */
//------------------------------------------------------------------------------
void ItemList::AdjustValue( app::tool::PaneList::SETUP_PARAM* param, u32 cursorPos, u32 scrollVal )
{
  // スクロール補正
  u32 scrollMax = 0;
  {
    if( param->valMax >= param->listPosMax )
    {
      scrollMax = param->valMax - param->listPosMax;
    }
    if( scrollVal > scrollMax )
    {
      scrollVal = scrollMax;
    }
    param->listScroll = static_cast<f32>(scrollVal);
  }
  // カーソル位置補正
  u32 posMax = 0;
  {
    if( scrollMax != 0 )
    {
      posMax = param->listPosMax - 1;
    }
    else if( param->valMax != 0 )
    {
      posMax = param->valMax - 1;
    }
    if( cursorPos > posMax )
    {
      cursorPos = posMax;
    }
    param->listPos = cursorPos;
  }
}





//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画するコールバック関数
 */
//------------------------------------------------------------------------------
void ItemList::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // リストの更新
  UpdateList( pane, pane_index, pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリスト決定時に呼ばれるリスナー
 */
//------------------------------------------------------------------------------
void ItemList::PaneListItemSelectListener( u32 pos )
{
  m_ListData.index = pos;

  if( m_pPaneListView )
  {
    app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
    {
      // カーソルデータの取得
      paneList->GetCursorData( &m_ListData.curPos, &m_ListData.scroll );
    }
  }

#ifdef DEBUG_ONLY_FOR_fukushima_yuya
  static const char* c_ItemListName[SRCID_NUM] = {
    "PokeName", "Type1", "Type2", "WazaName", "WazaMachine",
    "Tokusei", "Seikaku", "Sex", "Item", "Mark", "TeamName",
  };

  FUKUSHIMA_PRINT( "PaneListItemSelectListener\n" );
  FUKUSHIMA_PRINT( " ListData={ " );
  FUKUSHIMA_PRINT( "itemName=[%s]", c_ItemListName[m_ListData.index] );
  FUKUSHIMA_PRINT( "curPos=[%d]", m_ListData.curPos );
  FUKUSHIMA_PRINT( "scroll=[%.1f]", m_ListData.scroll );
  FUKUSHIMA_PRINT( "}\n" );
#endif

  // 検索内容リストの表示
  if( m_pListener )
  {
    m_pListener->OpenContentsList( pos );
  }
}

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

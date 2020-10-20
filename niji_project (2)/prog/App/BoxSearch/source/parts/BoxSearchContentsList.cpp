//======================================================================
/**
 * @file    BoxSearchContentsList.cpp
 * @date    2016/02/01 17:08:44
 * @author  fukushima_yuya
 * @brief   ボックス検索：検索内容リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchContentsList.h"
#include "BoxSearchSortData.h"
#include "BoxSearchPokeIcon.h"

#include <App/BoxSearch/include/BoxSearchAppParam.h>
#include <App/BoxSearch/source/BoxSearchConst.h>
#include <App/BoxSearch/source/BoxSearchDrawerBase.h>
#include <App/BoxSearch/source/BoxSearchDrawListener.h>

#include <GameSys/include/GameData.h>
#include <Savedata/include/BoxSave.h>
#include <Savedata/include/MyItemSave.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/box/res2d/srclow_pane.h>
#include <niji_conv_header/app/box/res2d/srclow_anim_list.h>
#include <niji_conv_header/message/msg_box.h>
#include <niji_conv_header/message/msg_initial.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  static const u32 CONT_LIST_BTN_MAX    = 10;    //!< 検索内容リストのボタン最大数
  static const u32 LIST_INDEX_HAIHUN    = 0;
  static const u32 LIST_INDEX_EISUJI    = 1;

#include "BoxSearchInitialList.cdat"
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ContentsList::ContentsList( CreateParam& param )
  : ListBase( param.base, param.cursor )
  , m_pAppParam(param.appParam)
  , m_pBoxSave(param.boxSave)
  , m_pMsgDataInitial(param.msgData_Initial)
  , m_pListener(param.listener)
  , m_uiListener(param.drawListener)
  , m_pGameMan(GFL_SINGLETON_INSTANCE(GameSys::GameManager))
  , m_pGameData(m_pGameMan->GetGameData())
  , m_pSortData(NULL)   // @fix cov_ctr[11495]
  , m_pPokeIcon(NULL)   // @fix cov_ctr[11495]
  , m_ListID(SRCID_NUM)
  , m_Hierarchy(HIE_CONSONANT)
  , m_Language(System::GetLang())
  , m_pListBtnEnable(NULL)
  , m_HaveWazaMachineNum(0)
  , m_pHaveWazaMachine(NULL)
  , m_pBtnBaseParts(NULL)
  , m_pPokeNameText(NULL)
  , m_pPokeIconPane(NULL)
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void ContentsList::Initialize( void )
{
  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    // ボタンの部品ペイン
    m_pBtnBaseParts = GFL_NEW_ARRAY(heap) gfl2::lyt::LytParts*[CONT_LIST_BTN_MAX];
    // 検索内容テキストボックス
    m_pPokeNameText = GFL_NEW_ARRAY(heap) gfl2::lyt::LytTextBox*[CONT_LIST_BTN_MAX];
    // ポケモンアイコンペイン
    m_pPokeIconPane = GFL_NEW_ARRAY(heap) gfl2::lyt::LytPane*[CONT_LIST_BTN_MAX];
  }

  // ソートデータの初期化
  InitializeSortData();
  // ポケモンアイコンの初期化
  InitializePokeIcon();
  // 技マシン情報の初期化
  InitializeWazaMachine();

  // リストの初期化
  InitializeList();
}

//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
void ContentsList::Terminate( void )
{
  DeleteList();

  if( m_pPaneListView )
  {
    m_pPaneListView->RemoveFromSuperView();   // @fix GFNMCat[4752]

    GFL_DELETE( m_pPaneListView );
  }
  
  GFL_SAFE_DELETE_ARRAY( m_pPaneData );

  GFL_SAFE_DELETE_ARRAY( m_pHaveWazaMachine );
  
  GFL_SAFE_DELETE( m_pPokeIcon );

  GFL_SAFE_DELETE( m_pSortData );

  GFL_SAFE_DELETE_ARRAY( m_pPokeIconPane );
  
  GFL_SAFE_DELETE_ARRAY( m_pPokeNameText );
  
  GFL_SAFE_DELETE_ARRAY( m_pBtnBaseParts );
}


//------------------------------------------------------------------------------
/**
 * @brief   ソートデータの初期化
 */
//------------------------------------------------------------------------------
void ContentsList::InitializeSortData( void )
{
  Savedata::ZukanData* zd = m_pGameData->GetZukanData();

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    m_pSortData = GFL_NEW(heap) SortData( heap, zd, m_Language );
    {
      GFL_ASSERT( m_pSortData );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンの初期化
 */
//------------------------------------------------------------------------------
void ContentsList::InitializePokeIcon( void )
{
  Savedata::ZukanData* zd = m_pGameData->GetZukanData();

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    m_pPokeIcon = GFL_NEW(heap) PokeIcon( m_pBase, m_pSortData, zd, CONT_LIST_BTN_MAX );
    {
      GFL_ASSERT( m_pPokeIcon );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   技マシン情報の初期化
 */
//------------------------------------------------------------------------------
void ContentsList::InitializeWazaMachine( void )
{
  m_HaveWazaMachineNum = 0;

  //u32 wazaMachineMax = item::ITEM_GetNormalWazaMashineMax() + item::ITEM_GetHidenMashineMax();
  u32 wazaMachineMax = item::ITEM_GetNormalWazaMashineMax(); //@fix GFNMCat[1150]
  {
    gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
    {
      m_pHaveWazaMachine = GFL_NEW_ARRAY(heap) u16[wazaMachineMax];
    }
  }

  u32 index = 0;

  for( u32 i=0; i<wazaMachineMax; ++i )
  {
    u16 itemNo = item::ITEM_GetWazaMashineItemNo( i );

    if( itemNo != ITEM_DUMMY_DATA )
    {
      m_pHaveWazaMachine[index] = itemNo;
      m_HaveWazaMachineNum++;
      index++;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void ContentsList::Update( void )
{
  if( m_pPokeIcon )
  {
    m_pPokeIcon->Update();
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   リストの初期化
 */
//------------------------------------------------------------------------------
void ContentsList::InitializeList( void )
{
  //! @brief  検索内容リストパーツインデックス
  static const u32 c_ContentsListBtnIndex[] = {
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_00,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_01,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_02,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_03,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_04,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_05,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_06,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_07,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_08,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_09,
  };
  //! @brief  検索項目リストボタンアニメ
  static const u32 c_ContentsListBtnAnimIndex[] = {
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_00_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_01_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_02_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_03_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_04_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_05_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_06_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_07_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_08_TOUCH,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_09_TOUCH,
  };

  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    // ペインデータの初期化
    m_pPaneData = GFL_NEW_ARRAY(heap) app::tool::PaneList::PANE_DATA[CONT_LIST_BTN_MAX];
    {
      GFL_ASSERT( m_pPaneData );

      for( u32 i=0; i<CONT_LIST_BTN_MAX; ++i )
      {
        gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_ContentsListBtnIndex[i] );
        m_pBtnBaseParts[i] = parts;

        app::tool::PaneList::PANE_DATA* data = &m_pPaneData[i];
        {
          data->base        = parts;
          data->text        = m_pBase->GetTextBox( PANENAME_BOX_BTN_LOW_000_PANE_TEXTBOX_00, parts );
          data->bound       = m_pBase->GetBounding( PANENAME_BOX_BTN_LOW_000_PANE_BOUND_00, parts );
          data->cursor_pos  = m_pBase->GetPane( PANENAME_BOX_BTN_LOW_000_PANE_NULL_CUR, parts );

          const u32 animID = c_ContentsListBtnAnimIndex[i];
          {
            data->holdAnime    = animID + BTN_ANIM_TOUCH;
            data->releaseAnime = animID + BTN_ANIM_RELEASE;
            data->cancelAnime  = animID + BTN_ANIM_CANCEL;
            data->selectAnime  = animID + BTN_ANIM_TOUCH_RELEASE;
          }
        }

        m_pPokeNameText[i] = m_pBase->GetTextBox( PANENAME_BOX_BTN_LOW_000_PANE_TEXTBOX_01, parts );
        m_pPokeIconPane[i] = m_pBase->GetPane( PANENAME_BOX_BTN_LOW_000_PANE_ICON_POKE, parts );
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
void ContentsList::CreateList( void )
{
  GFL_ASSERT( m_pPaneListView );

  // ソートデータの初期化
  if( m_Hierarchy == HIE_VOWEL )
  {
    SetSortDataItemMax();
  }

  // リストの最大数
  u32 listMax = GetListMax();
  {
    GFL_ASSERT( listMax > 0 );
  }

  // 入力許可フラグの生成
  CreateListBtnEnable( listMax );

  app::tool::PaneList::SETUP_PARAM list_setup;
  {
    // リストパラメータのセットアップ
    SetListParam( &list_setup, listMax );
  }

  // スクロール値の取得
  u32 scrollVal = this->GetScrollValue( list_setup );

  // スクロールバーのセットアップパラメータ
  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup = this->GetDefaultScrollBarParam();
  {
    app::util::ScrollBar::SETUP_PARAM* param = &scroll_bar_setup;
    {
      param->max          = scrollVal;
      param->parts_index  = PANENAME_BOX_SRC_LOW_000_PANE_SCROLL_S;
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
    paneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

    { // タッチエリアの変更 @fix NMCat[733]
      gfl2::math::VEC3 pos = m_pPaneData[0].bound->GetTranslate();
      u32 posMax = list_setup.listPosMax;
      if( list_setup.valMax < posMax )
      {
        posMax = list_setup.valMax;
      }

      f32 lx = 62.0f - (list_setup.defSX / 2.0f);
      f32 rx = lx + list_setup.defSX;
      f32 uy = list_setup.defPY + pos.y + (list_setup.defSY / 2.0f);
      f32 dy = uy - (list_setup.defSY * posMax);

      paneList->MakeTouchArea( lx, rx, uy, dy );
    }

    // ビープ音の設定
    for( u32 i=0; i<CONT_LIST_BTN_MAX && i<listMax; ++i )
    {
      if( m_pListBtnEnable[i] == false )
      {
        if( i < (CONT_LIST_BTN_MAX - 1) )
        {
          paneList->SetButtonSEPaneIndex( i+1, SEQ_SE_BEEP );
        }
      }
    }
  }

  // 入力を有効に
  this->SetInputEnable( true );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの破棄
 */
//------------------------------------------------------------------------------
void ContentsList::DeleteList( void )
{
  // 入力許可フラグの破棄
  GFL_SAFE_DELETE_ARRAY( m_pListBtnEnable );

  if( m_pPaneListView )
  {
    if( m_pPaneListView->GetPaneList() )
    {
      // スクロールバーの破棄
      m_pPaneListView->DeleteScrollBar();

      // ペインリストの破棄
      m_pPaneListView->DeletePaneList();
    }

    // 入力を無効に
    this->SetInputEnable( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定
 *
 * @param   prm       [out]リストパラメータ
 * @param   listMax   [in]リストの最大数
 */
//------------------------------------------------------------------------------
void ContentsList::SetListParam( app::tool::PaneList::SETUP_PARAM* prm, u32 listMax )
{
  prm->heap           = m_pBase->GetDeviceHeap();;
  prm->pLytWk         = m_pBase->GetLytWk();
  prm->paneList       = m_pPaneData;
  prm->paneListMax    = CONT_LIST_BTN_MAX;
  prm->valMax         = listMax;
  prm->listPosMax     = 8;
  prm->listPos        = m_ListData[m_Hierarchy].curPos;
  prm->listScroll     = m_ListData[m_Hierarchy].scroll;
  prm->defPY          = 100;
  prm->defSX          = 182;
  prm->defSY          = 28;
  prm->writeOffsetY   = -1;
  prm->listStartPane  = prm->pLytWk->GetPane( PANENAME_BOX_SRC_LOW_000_PANE_NULL_00 );
  prm->cbFunc         = this;
  prm->cbWork         = NULL;
  prm->uiDevMan       = m_pGameMan->GetUiDeviceManager();
  prm->menuCursor     = m_pCursor->GetMenuCursor();
}


//------------------------------------------------------------------------------
/**
 * @brief   次の階層へ
 */
//------------------------------------------------------------------------------
void ContentsList::NextHierarchy( void )
{
  // 次の階層へ移動できないので、検索項目リストへ移動
  if( !IsNextHierarchy() )
  {
    // 条件の決定
    SetSearchParam();

    // ソートデータの破棄
    DeleteSortDataContents();

    if( m_pListener )
    {
      m_pListener->OpenItemList();  //!< 検索項目リストへ
    }
  }
  else {
    // リストの破棄
    DeleteList();

    // スキップ処理(日本語で英数字を選択)
    if( SkipHierarchy() )
    {
      m_Hierarchy++;
    }

    m_Hierarchy++;

    // リストデータの初期化
    m_ListData[m_Hierarchy] = ListData();

    // ソートデータの生成
    CreateSortDataContents();

    // リストの生成
    CreateList();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   前の階層へ
 */
//------------------------------------------------------------------------------
void ContentsList::PrevHierarchy( void )
{
  if( m_Hierarchy == GetFirstHierarchy() )
  {
    if( m_pListener )
    {
      m_pListener->OpenItemList();  //!< 検索項目リストへ
    }
  }
  else {
    // ソートデータの破棄
    DeleteSortDataContents();

    // リストの破棄
    DeleteList();

    // リストデータの初期化
    m_ListData[m_Hierarchy] = ListData(); //@fix GFNMCat[1151]

    // スキップ処理(日本語で英数字を選択)
    if( SkipHierarchy() )
    {
      m_Hierarchy--;
    }

    m_Hierarchy--;

    // リストの生成
    CreateList();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   最初の階層を取得
 *
 * @return  階層
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetFirstHierarchy( void )
{
  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
  case SRCID_WAZA_NAME:
  case SRCID_TOKUSEI:
    {
      if( m_Language == POKEMON_LANG_JAPAN )
      {
        return HIE_CONSONANT;    //!< 子音リストへ
      }
      else {
        return HIE_VOWEL;        //!< 母音リストへ
      }
    } // no break;

  default:
    {
      return HIE_CONTENTS;       //!< 内容リストへ
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   次の階層へ移動できるか？
 *
 * @return  "true  = 移動可"
 * @return  "false = 移動不可"
 */
//------------------------------------------------------------------------------
bool ContentsList::IsNextHierarchy( void )
{
  // 最下層であるため移動不可
  if( m_Hierarchy == HIE_CONTENTS ) return false;

  if( m_ListID == SRCID_POKE_NAME || m_ListID == SRCID_WAZA_NAME || m_ListID == SRCID_TOKUSEI )
  {
    u32 index = m_ListData[HIE_CONSONANT].index;

    if( m_Language != POKEMON_LANG_JAPAN )
    {
      index = m_ListData[HIE_VOWEL].index;
    }

    if( index == LIST_INDEX_HAIHUN )
    {
      // [－]を選択したので移動不可
      return false;
    }
  }

  // 次の階層へ移動可
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   階層のスキップ
 *
 * @note    日本語の英数字選択がされた時のみ
 *
 * @return  "true  = スキップあり"
 * @return  "false = スキップなし"
 */
//------------------------------------------------------------------------------
bool ContentsList::SkipHierarchy( void )
{
  if( m_Language != POKEMON_LANG_JAPAN ) return false;   //!< 日本語
  if( m_ListData[HIE_CONSONANT].index != LIST_INDEX_EISUJI ) return false;   //!< 英数字  
  //if( m_Hierarchy != HIE_CONSONANT )      return false;   //!< 子音リスト

  // 上記の条件をクリアした時は母音リストをスキップする
  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの入力有効フラグの生成
 *
 * @param   listMax   項目最大数
 */
//------------------------------------------------------------------------------
void ContentsList::CreateListBtnEnable( u32 listMax )
{
  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    m_pListBtnEnable = GFL_NEW_LOW_ARRAY(heap) bool[listMax];
    {
      GFL_ASSERT( m_pListBtnEnable );

      for( u32 i=0; i<listMax; ++i )
      {
        m_pListBtnEnable[i] = true;   //!< 最初はすべて有効に
      }
    }
  }

  // ボタン入力設定
  if( m_Hierarchy == HIE_CONSONANT )
  {
    SetListBtnEnable_Consonant( listMax );
  }
  else if( m_Hierarchy == HIE_VOWEL )
  {
    SetListBtnEnable_Vowel( listMax );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタン入力設定
 *
 * @param   listMax   項目最大数
 */
//------------------------------------------------------------------------------
void ContentsList::SetListBtnEnable_Consonant( u32 listMax )
{
  // 日本語のみ
  if( m_Language == POKEMON_LANG_JAPAN )
  {
    if( m_ListID == SRCID_POKE_NAME )
    {
      // ポケモン名と特性名は英数字はないので、
      // 英数字を入力無効に
      m_pListBtnEnable[1] = false;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタン入力設定
 *
 * @param   listMax   項目最大数
 */
//------------------------------------------------------------------------------
void ContentsList::SetListBtnEnable_Vowel( u32 listMax )
{
  if( m_Language == POKEMON_LANG_JAPAN )
  {
    // 内容リストが空の場合は入力を無効にしておく
    for( u32 i=0; i<listMax; ++i )
    {
      m_pListBtnEnable[i] = (GetListMax_VowelNum( i ) > 0);
    }
  }
  else {
    m_pListBtnEnable[0] = true;

    for( u32 i=1; i<listMax; ++i )
    {
      m_pListBtnEnable[i] = (GetListMax_VowelNum( i ) > 0);
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   リストの最大数の取得
 *
 * @return  リスト数
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetListMax( void )
{
  u32 (ContentsList::*GET_LIST_MAX_HIERARCHY[HIE_NUM])( void ) = {
    &ContentsList::GetListMax_Consonant,
    &ContentsList::GetListMax_Vowel,
    &ContentsList::GetListMax_Contents,
  };
  
  return (this->*GET_LIST_MAX_HIERARCHY[m_Hierarchy])();
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの最大数の取得(子音リスト)
 *
 * @return  リスト数
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetListMax_Consonant( void )
{
  GFL_ASSERT( m_Language == POKEMON_LANG_JAPAN );   //!< 子音リストは日本語しか使用しない

  // －,英数字,あ,か,さ,た,な,は,ま,や,ら,わ
  return GFL_NELEMS( c_ConsonantInitial );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの最大数の取得(母音リスト)
 *
 * @return  リスト数
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetListMax_Vowel( void )
{
  switch( m_Language )
  {
  case POKEMON_LANG_JAPAN:  //!< 日本語
    {
      u32 consonant = m_ListData[HIE_CONSONANT].index - 1;    //!< - と 英数字 分減らす

      return c_VowelInitialNum_Kana[consonant];
    } // no break;

  case POKEMON_LANG_KOREA:  //!< 韓国語
    {
      return GFL_NELEMS( c_VowelInitial_Hangul );
    } // no break;

  case POKEMON_LANG_CHINA:  //!< 簡体字
    {
      return GFL_NELEMS( c_VowelInitial_Simplified );
    } // no break;

  case POKEMON_LANG_TAIWAN:  //!< 繁体字
    {
      return GFL_NELEMS( c_VowelInitial_Traditonal );
    } // no break;

  default:  //!< それ以外の言語
    {
      return GFL_NELEMS( c_VowelInitial_Alphabet );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの最大数の取得(内容リスト)
 *
 * @return  リスト数
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetListMax_Contents( void )
{
  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
    {
      return GetListMax_VowelNum( m_ListData[HIE_VOWEL].index );
    } // no break;

  case SRCID_TOKUSEI:
    {
      return m_pSortData->GetTokuseiCount();    // @fix NMCat[2926]
    }

  case SRCID_WAZA_NAME:
    {
      return m_pSortData->GetWazaCount();
    } // no break;

  case SRCID_TYPE_1:
  case SRCID_TYPE_2:
    {
      return  POKETYPE_MAX + 1;   //!< 全タイプ(18種類)＋非選択分
    } // no break;

  case SRCID_WAZAMACHINE:
    {
      return m_HaveWazaMachineNum + 1;    //!< 技マシン＋非選択分
    } // no break;

  case SRCID_SEIKAKU:
    {
      return pml::pokepara::SEIKAKU_NUM + 1;
    } // no break;

  case SRCID_GENDER:
    {
      return pml::SEX_NUM + 1;
    } // no break;

  //case SRCID_ENTRY:  //!< 仕様削除20160222
  case SRCID_ITEM:
    {
      return CONDITIONAL_NUM;
    } // no break;

  case SRCID_MARK:
    {
      return pml::pokepara::BOX_MARK_NUM;
    } // no break;

  case SRCID_TEAM:
    {
      return 7;    //!< 6チーム + 非選択分
    } // no break;
  }

  GFL_ASSERT(0);
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   リストの最大数の取得(内容リスト数)
 *
 * @return  リスト数
 */
//------------------------------------------------------------------------------
u32 ContentsList::GetListMax_VowelNum( u32 index )
{
  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
    return m_pSortData->GetPokemonSortVowelNum( index );

  case SRCID_WAZA_NAME:
    return m_pSortData->GetWazaSortVowelNum( index );

  case SRCID_TOKUSEI:
    return m_pSortData->GetTokuseiSortVowelNum( index );
  }

  GFL_ASSERT(0);
  return 0;
}



//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定
 *
 * @param   pane
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // ボタン有効状態のセットアップ解除
  UnsetupList_BtnEnable( pane, pane_index, pos );

  {
    // 通常テキストボックス(ボックスマークとポケモン名以外で表示)
    m_pBase->SetVisiblePane( pane[pane_index].text, true );
    // ポケモン名専用テキストボックス
    m_pBase->SetVisiblePane( m_pPokeNameText[pane_index], false );
    // ポケモンアイコン
    m_pBase->SetVisiblePane( m_pPokeIconPane[pane_index], false );
    // ボックスマークペイン
    gfl2::lyt::LytPane* markPane = GetBoxMarkPane( pane, pane_index, pos );
    if( markPane ) m_pBase->SetVisiblePane( markPane, false );
  }

  void (ContentsList::*UPDATE_LIST_HIERARCHY[HIE_NUM])( const app::tool::PaneList::PANE_DATA*, u32, u32 ) = {
    &ContentsList::UpdateList_Consonant,
    &ContentsList::UpdateList_Vowel,
    &ContentsList::UpdateList_Contents,
  };
  (this->*UPDATE_LIST_HIERARCHY[m_Hierarchy])( pane, pane_index, pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(子音リスト)
 *
 * @param   pane
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Consonant( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  GFL_ASSERT( m_Language == POKEMON_LANG_JAPAN );

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, c_ConsonantInitial[0] );
    }
    else {
      // ボタン有効状態のセットアップ
      SetupList_BtnEnable( pane, pane_index, pos );

      SetInitial( pane[pane_index].text, c_ConsonantInitial[pos] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(母音リスト)
 *
 * @param   pane
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Vowel( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  switch( m_Language )
  {
  case POKEMON_LANG_JAPAN:
    {
      u32 consonant = m_ListData[HIE_CONSONANT].index - 2;    //!< - と 英数字 分減らす
      const u32* initial = c_VowelInitial_Kana[consonant];

      // ボタン有効状態のセットアップ
      SetupList_BtnEnable( pane, pane_index, pos );

      SetInitial( pane[pane_index].text, initial[pos] );
    } break;

  default:
    {
      const u32* initial = GetVowelList();

      if( pos == LIST_INDEX_HAIHUN )
      {
        view->SetTextboxPaneMessage( pane[pane_index].text, initial[0] );
      }
      else {
        // ボタン有効状態のセットアップ
        SetupList_BtnEnable( pane, pane_index, pos );

        SetInitial( pane[pane_index].text, initial[pos] );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(内容リスト)
 *
 * @param   pane
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Contents( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  void (ContentsList::*UPDATE_LIST[SRCID_NUM])( const app::tool::PaneList::PANE_DATA*, u32, u32 ) = {
    &ContentsList::UpdateList_PokeName,
    &ContentsList::UpdateList_Type,
    &ContentsList::UpdateList_Type,
    &ContentsList::UpdateList_WazaName,
    &ContentsList::UpdateList_WazaMachine,
    &ContentsList::UpdateList_Tokusei,
    &ContentsList::UpdateList_Seikaku,
    &ContentsList::UpdateList_Gender,
    &ContentsList::UpdateList_Item,
    &ContentsList::UpdateList_BoxMark,
    //&ContentsList::UpdateList_TeamEntry,  //!< 仕様削除20160222
    &ContentsList::UpdateList_TeamName,
  };
  (this->*UPDATE_LIST[m_ListID])( pane, pane_index, pos );
}


//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(ポケモン名)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_PokeName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // ソートデータから取得
  MonsNo monsNo = m_pSortData->GetPokemonSortItemData( pos );

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterPokeMonsNameNo( 0, monsNo );
    }
    view->SetTextboxPaneMessage( m_pPokeNameText[pane_index], msg_box_search_02_02 );
  }

  // ポケモンアイコンの変更
  m_pPokeIcon->SetPokeIcon( pane_index, monsNo );

  // ペインの表示切替
  m_pBase->SetVisiblePane( pane[pane_index].text, false );
  m_pBase->SetVisiblePane( m_pPokeNameText[pane_index], true );
  m_pBase->SetVisiblePane( m_pPokeIconPane[pane_index], true );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(タイプ)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Type( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      pml::PokeType type = static_cast<pml::PokeType>(pos - 1);

      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterPokeTypeName( 0, type );
      }
      view->SetTextboxPaneMessage( pane[pane_index].text, msg_box_search_02_04 );
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
void ContentsList::UpdateList_WazaName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // ソートデータから取得
  WazaNo wazaNo = m_pSortData->GetWazaNo( pos );

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterWazaName( 0, wazaNo );
    }
    view->SetTextboxPaneMessage( pane[pane_index].text, msg_box_search_02_08 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(技マシン)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_WazaMachine( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      WazaNo wazaNo = static_cast<WazaNo>(item::ITEM_GetWazaNo( m_pHaveWazaMachine[pos - 1] ));

      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterWazaName( 0, wazaNo );
      }
      view->SetTextboxPaneMessage( pane[pane_index].text, msg_box_search_02_08 );
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
void ContentsList::UpdateList_Tokusei( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // ソートデータから取得
  TokuseiNo tokuseiNo = m_pSortData->GetTokuseiNo( pos );

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    print::WordSet* wordSet = m_pBase->GetWordSet();
    {
      wordSet->RegisterTokuseiName( 0, tokuseiNo );
    }
    view->SetTextboxPaneMessage( pane[pane_index].text, msg_box_search_02_08 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(性格)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Seikaku( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      pml::pokepara::Seikaku seikaku = static_cast<pml::pokepara::Seikaku>(pos - 1);

      print::WordSet* wordSet = m_pBase->GetWordSet();
      {
        wordSet->RegisterSeikakuName( 0, seikaku );
      }
      view->SetTextboxPaneMessage( pane[pane_index].text, msg_box_search_02_14 );
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
void ContentsList::UpdateList_Gender( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 c_GenderMsgID[] = {
    msg_box_search_02_16,
    msg_box_search_02_17,
    msg_box_search_02_18,
  };

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      view->SetTextboxPaneMessage( pane[pane_index].text, c_GenderMsgID[pos-1] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(もちもの)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_Item( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 c_ItemMsgID[] = {
    msg_box_search_02_22,
    msg_box_search_02_23,
  };

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      view->SetTextboxPaneMessage( pane[pane_index].text, c_ItemMsgID[pos-1] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(ボックスマーク)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_BoxMark( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // ボックスマークアニメ
  static const u32 c_BoxMarkAnim[] = {
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_01_MARK_COLOR,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_02_MARK_COLOR,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_03_MARK_COLOR,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_04_MARK_COLOR,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_05_MARK_COLOR,
    LA_SRCLOW_BOX_SRC_LOW_000__LIST_S_06_MARK_COLOR,
  };

  u32 anmID = c_BoxMarkAnim[pos];
  u32 color = m_pAppParam->mark.markColor[pos];

  // アニメの開始
  m_pBase->StartAnim( anmID, false );
  m_pBase->SetAnimFrame( anmID, static_cast<f32>(color) );

  gfl2::lyt::LytPane* markPane = GetBoxMarkPane( pane, pane_index, pos );

  // ペインの表示切替
  m_pBase->SetVisiblePane( markPane, true );
  m_pBase->SetVisiblePane( pane[pane_index].text, false );
  m_pBase->SetVisiblePane( m_pPokeNameText[pane_index], false );
  m_pBase->SetVisiblePane( m_pPokeIconPane[pane_index], false );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(チーム登録)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_TeamEntry( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 c_EntryMsgID[] = {
    msg_box_search_02_25,
    msg_box_search_02_26,
  };

  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      view->SetTextboxPaneMessage( pane[pane_index].text, c_EntryMsgID[pos-1] );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータの設定(チーム名)
 *
 * @param   pane_index
 */
//------------------------------------------------------------------------------
void ContentsList::UpdateList_TeamName( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::ui::UIView* view = m_pBase->GetUIView();
  {
    if( pos == LIST_INDEX_HAIHUN )
    {
      view->SetTextboxPaneMessage( pane[pane_index].text, PARAM_NONE );
    }
    else {
      app::util::G2DUtil* g2d = m_pBase->GetG2D();
      {
        g2d->SetTextBoxPaneString(
          pane[pane_index].text,
          m_pBoxSave->GetTeamName( static_cast<u32>(pos - 1) ) );
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタン有効状態のセットアップ
 */
//------------------------------------------------------------------------------
void ContentsList::SetupList_BtnEnable( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  nw::ut::Color8 color;
  {
    if( m_pListBtnEnable[pos] )
    {
      color = nw::ut::Color8::BLACK;
    }
    else {
      color = nw::ut::Color8::GRAY;
    }

    // テキストカラーの変更
    pane[pane_index].text->SetTextColor( color, color );
  }

  u32 se;
  {
    if( m_pListBtnEnable[pos] )
    {
      se = SEQ_SE_DECIDE1;
    }
    else {
      se = SEQ_SE_BEEP;
    }

    app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
    {
      if( paneList )
      {
        // ボタンの選択音の変更
        paneList->SetButtonSEPaneIndex( pane_index, se );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタン有効状態のセットアップ解除
 */
//------------------------------------------------------------------------------
void ContentsList::UnsetupList_BtnEnable( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  nw::ut::Color8 color = nw::ut::Color8::BLACK;
  {
    // テキストカラーの変更
    pane[pane_index].text->SetTextColor( color, color );
  }

  u32 se = SEQ_SE_DECIDE1;
  {
    app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
    {
      if( paneList )
      {
        // ボタンの選択音の変更
        paneList->SetButtonSEPaneIndex( pane_index, se );
      }
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ソートデータ項目数の設定
 */
//------------------------------------------------------------------------------
void ContentsList::SetSortDataItemMax( void )
{
  u32 consonant = 0;
  u32 vowelMax  = 0;

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    consonant = m_ListData[HIE_CONSONANT].index - 1;    //!< - 分減らす
    vowelMax  = c_VowelInitialNum_Kana[consonant];
  }
  else {
    consonant = m_ListData[HIE_CONSONANT].index;
    vowelMax  = GetListMax_Vowel();
  }

  FUKUSHIMA_PRINT( "SetSortDataItemMax\n" );
  FUKUSHIMA_PRINT( " consonant[%d], vowelMax[%d]\n", consonant, vowelMax );

  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
    m_pSortData->InitPokemonSortItemMax( consonant, vowelMax );
    break;
  case SRCID_TOKUSEI:
    m_pSortData->InitTokuseiSortItemMax( consonant, vowelMax );
    break;
  case SRCID_WAZA_NAME:
    m_pSortData->InitWazaSortItemMax( consonant, vowelMax );
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ソートデータの生成
 */
//------------------------------------------------------------------------------
void ContentsList::CreateSortDataContents( void )
{
  if( m_Hierarchy != HIE_CONTENTS ) return;

  u32 consonant = m_ListData[HIE_CONSONANT].index;
  u32 vowel     = m_ListData[HIE_VOWEL].index;

  if( m_Language == POKEMON_LANG_JAPAN )
  {
    consonant -= 1;   //!< [-]分減らす
  }
  else {
    vowel -= 1;       //!< [-]分減らす
  }

  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
    {
      m_pSortData->SetInitialPokemonSortData( consonant, vowel );
    } break;

  case SRCID_WAZA_NAME:
    {
      m_pSortData->CreateWazaSortData( consonant, vowel );
    } break;

  case SRCID_TOKUSEI:
    {
      m_pSortData->CreateTokuseiSortData( consonant, vowel );
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ソートデータの破棄
 */
//------------------------------------------------------------------------------
void ContentsList::DeleteSortDataContents( void )
{
  if( m_Hierarchy != HIE_CONTENTS ) return;

  switch( m_ListID )
  {
  case SRCID_POKE_NAME:
    {
      // ポケモン名は頭文字の変更をしているため、
      // 破棄するものは無い
    } break;

  case SRCID_WAZA_NAME:
    {
      m_pSortData->DeleteWazaSortData();
    } break;

  case SRCID_TOKUSEI:
    {
      m_pSortData->DeleteTokuseiSortData();
    } break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   言語ごとの頭文字リストの取得
 *
 * @return  頭文字リスト
 */
//------------------------------------------------------------------------------
const u32* ContentsList::GetVowelList( void ) const
{
  switch( m_Language )
  {
  case POKEMON_LANG_JAPAN:
    {
      u32 consonant = m_ListData[HIE_CONSONANT].index - 2;    //!< - と 英数字 分減らす
      return c_VowelInitial_Kana[consonant];
    }
  case POKEMON_LANG_KOREA:
    return c_VowelInitial_Hangul;
  case POKEMON_LANG_CHINA:
    return c_VowelInitial_Simplified;
  case POKEMON_LANG_TAIWAN:
    return c_VowelInitial_Traditonal;
  default:
    return c_VowelInitial_Alphabet;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   頭文字の文字列を取得
 *
 * @param   textBox   テキストボックス
 * @param   msgID     メッセージID
 */
//------------------------------------------------------------------------------
void ContentsList::SetInitial( gfl2::lyt::LytTextBox* textBox, u32 msgID )
{
  //app::ui::UIView* view = m_pBase->GetUIView();
  //{
  //  gfl2::str::StrBuf* initial = m_pBase->GetG2D()->GetTempStrBuf(0);
  //  {
  //    m_pMsgDataInitial->GetString( msgID, *initial );
  //  }
  //  view->SetTextboxPaneString( textBox, initial );
  //}

  app::util::G2DUtil* g2d = m_pBase->GetG2D();
  {
    g2d->SetMessageData( m_pMsgDataInitial, m_pBase->GetWordSet() );

    g2d->SetTextBoxPaneString( textBox, msgID );

    g2d->SetMessageData( m_pBase->GetMsgData(), m_pBase->GetWordSet() );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボックスマークペインの取得
 *
 * @param   pane        ペインデータ
 * @param   pane_index  ペインインデックス
 * @param   pos         データインデックス
 *
 * @return  gfl2::lyt::LytPane*
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytPane* ContentsList::GetBoxMarkPane( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  if( pos < pml::pokepara::BOX_MARK_NUM )
  {
    // 検索条件リストのボックスマークペイン
    static const u32 c_BoxMarkPane[pml::pokepara::BOX_MARK_NUM] = {
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_00,
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_01,
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_02,
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_03,
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_04,
      PANENAME_BOX_BTN_LOW_000_PANE_MARKICON_05,
    };

    return m_pBase->GetPane( c_BoxMarkPane[pos], m_pBtnBaseParts[pane_index] );
  }

  // 範囲外のためNULLを返す
  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam( void )
{
  u32 value = m_ListData[HIE_CONTENTS].index;

  void (ContentsList::*SET_SEARCH_PARAM[SRCID_NUM])( u32 ) = {
    &ContentsList::SetSearchParam_PokeName,     //!<  0:ポケモン名
    &ContentsList::SetSearchParam_Type1,        //!<  1:タイプ１
    &ContentsList::SetSearchParam_Type2,        //!<  2:タイプ２
    &ContentsList::SetSearchParam_WazaName,     //!<  3:技名
    &ContentsList::SetSearchParam_WazaMachine,  //!<  4:技マシン
    &ContentsList::SetSearchParam_Tokusei,      //!<  5:特性
    &ContentsList::SetSearchParam_Siekaku,      //!<  6:性格
    &ContentsList::SetSearchParam_Gender,       //!<  7:性別
    &ContentsList::SetSearchParam_Item,         //!<  8:持ち物
    &ContentsList::SetSearchParam_Mark,         //!<  9:マーク
    //&ContentsList::SetSearchParam_TeamEntry,    //!< 10:チーム登録  //!< 仕様削除20160222
    &ContentsList::SetSearchParam_TeamName,     //!< 11:チーム名
  };

  (this->*SET_SEARCH_PARAM[ m_ListID ])( value );
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_PokeName( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->monsNo = MONSNO_NULL;

    if( m_Hierarchy == HIE_CONTENTS )
    {
      m_pAppParam->monsNo = m_pSortData->GetPokemonSortItemData( val );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Type1( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->type1 = POKETYPE_NULL;

    if( val > 0 )
    {
      m_pAppParam->type1 = static_cast<u8>( val - 1 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Type2( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->type2 = POKETYPE_NULL;

    if( val > 0 )
    {
      m_pAppParam->type2 = static_cast<u8>( val - 1 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_WazaName( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->wazaNo = WAZANO_NULL;

    if( m_Hierarchy == HIE_CONTENTS )
    {
      m_pAppParam->wazaNo = m_pSortData->GetWazaNo( val );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_WazaMachine( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->wazaMachine = static_cast<u8>( val );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Tokusei( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->tokusei = TOKUSEI_NULL;

    if( m_Hierarchy == HIE_CONTENTS )
    {
      m_pAppParam->tokusei = m_pSortData->GetTokuseiNo( val );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Siekaku( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->seikaku = BOX_SEARCH_SEIKAKU_NULL;

    if( val > 0 )
    {
      m_pAppParam->seikaku = static_cast<pml::pokepara::Seikaku>( val - 1 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Gender( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->sex = BOX_SEARCH_GENDER_NULL;

    if( val > 0 )
    {
      m_pAppParam->sex = static_cast<pml::Sex>( val - 1 );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Item( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->item = static_cast<u8>( val );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_Mark( u32 val )
{
  u32 markColor = m_pAppParam->mark.markColor[val];
  {
    markColor = (markColor + 1) % pml::pokepara::BOX_MARK_COLOR_NUM;

    m_pAppParam->mark.markColor[val] = static_cast<pml::pokepara::BoxMarkColor>( markColor );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_TeamEntry( u32 val )
{
  GFL_ASSERT(0);
  //!< 仕様削除20160222
  //if( m_pAppParam )
  //{
  //  m_pAppParam->entry = static_cast<u8>( val );
  //}
}

//------------------------------------------------------------------------------
/**
 * @brief   検索条件の更新
 *
 * @param   val   値
 */
//------------------------------------------------------------------------------
void ContentsList::SetSearchParam_TeamName( u32 val )
{
  if( m_pAppParam )
  {
    m_pAppParam->teamNo = static_cast<u8>( val );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage( u32 pos )
{
  if( m_uiListener == NULL ) return;

  void (ContentsList::*SET_UPPER_MESSAGE[HIE_NUM])( u32 ) = {
    &ContentsList::SetUpperMessage_Consonant,
    &ContentsList::SetUpperMessage_Vowel,
    &ContentsList::SetUpperMessage_Contents,
  };

  (this->*SET_UPPER_MESSAGE[m_Hierarchy])( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Consonant( u32 pos )
{
  if( m_Language != POKEMON_LANG_JAPAN ) return;

  // 上画面メッセージのセット(頭文字選択用)
  m_uiListener->SetUpperMessage_Initial( m_ListID );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Vowel( u32 pos )
{
  // 上画面メッセージのセット(頭文字選択用)
  m_uiListener->SetUpperMessage_Initial( m_ListID );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Contents( u32 pos )
{
  void (ContentsList::*SET_UPPER_MESSAGE_CONTENTS[SRCID_NUM])( u32 ) = {
    &ContentsList::SetUpperMessage_PokeName,
    &ContentsList::SetUpperMessage_Type1,
    &ContentsList::SetUpperMessage_Type2,
    &ContentsList::SetUpperMessage_WazaName,
    &ContentsList::SetUpperMessage_WazaMachine,
    &ContentsList::SetUpperMessage_Tokusei,
    &ContentsList::SetUpperMessage_Seikaku,
    &ContentsList::SetUpperMessage_Gender,
    &ContentsList::SetUpperMessage_Item,
    &ContentsList::SetUpperMessage_Mark,
    //&ContentsList::SetUpperMessage_TeamEntry,  //!< 仕様削除20160222
    &ContentsList::SetUpperMessage_TeamName,
  };

  (this->*SET_UPPER_MESSAGE_CONTENTS[m_ListID])( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_PokeName( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_POKE_NAME );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Type1( u32 pos )
{
  u8 type = POKETYPE_NULL;

  if( pos > 0 )
  {
    type = static_cast<u8>(pos - 1);
  }

  m_uiListener->SetUpperMessage_Type( SRCID_TYPE_1, type, m_pAppParam->type2 );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Type2( u32 pos )
{
  u8 type = POKETYPE_NULL;

  if( pos > 0 )
  {
    type = static_cast<u8>(pos - 1);
  }

  m_uiListener->SetUpperMessage_Type( SRCID_TYPE_2, m_pAppParam->type1, type );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_WazaName( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_WAZA_NAME );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_WazaMachine( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_WAZAMACHINE );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Tokusei( u32 pos )
{
  TokuseiNo tokuseiNo = m_pSortData->GetTokuseiNo( pos );
  {
    m_uiListener->SetUpperMessage_Tokusei( tokuseiNo );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Seikaku( u32 pos )
{
  pml::pokepara::Seikaku seikaku = BOX_SEARCH_SEIKAKU_NULL;

  if( pos > 0 )
  {
    seikaku = static_cast<pml::pokepara::Seikaku>(pos - 1);
  }

  m_uiListener->SetUpperMessage_Seikaku( seikaku );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Gender( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_GENDER );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Item( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_ITEM );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_Mark( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_MARK );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_TeamEntry( u32 pos )
{
  GFL_ASSERT(0);
  //!< 仕様削除20160222
  //GFL_UNUSED( pos );

  //m_uiListener->SetUpperMessage_Guide( SRCID_ENTRY );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面メッセージのセット
 *
 * @param   pos   リストインデックス
 */
//------------------------------------------------------------------------------
void ContentsList::SetUpperMessage_TeamName( u32 pos )
{
  GFL_UNUSED( pos );

  m_uiListener->SetUpperMessage_Guide( SRCID_TEAM );
}



//------------------------------------------------------------------------------
/**
 * @brief   リスト項目IDの設定
 *
 * @param   srcID   リスト項目ID
 */
//------------------------------------------------------------------------------
void ContentsList::SetItemListID( u32 srcID )
{
  m_ListID = srcID;

  // リストデータの初期化
  for( u32 i=0; i<HIE_NUM; ++i )
  {
    m_ListData[i] = ListData();
  }

  // 最初の階層を取得
  m_Hierarchy = GetFirstHierarchy();
}






//==============================================================================
/**
 *      ↓　以下がリスナー処理　↓
 */
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   カーソル移動時のコールバック
 */
//------------------------------------------------------------------------------
void ContentsList::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  // 上画面メッセージのセット
  SetUpperMessage( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   リストの項目を描画するコールバック関数
 */
//------------------------------------------------------------------------------
void ContentsList::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  // リストの更新
  UpdateList( pane, pane_index, pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリスト決定時に呼ばれるリスナー
 */
//------------------------------------------------------------------------------
void ContentsList::PaneListItemSelectListener( u32 pos )
{
  if( m_pListBtnEnable[pos] )
  {
    // リストデータの更新
    ListData* listData = &m_ListData[m_Hierarchy];
    {
      listData->index = pos;

      app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
      {
        paneList->GetCursorData( &listData->curPos, &listData->scroll );
      }
    }

    if( m_Hierarchy == HIE_CONTENTS && m_ListID == SRCID_MARK )
    {
      // 条件の決定
      SetSearchParam();
      // 表示を更新
      UpdateList_BoxMark( m_pPaneData, pos+1, pos );

      return; //!< 次の階層へは行かないのでここで抜ける
    }

    // 次の階層へ
    NextHierarchy();
  }
}

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

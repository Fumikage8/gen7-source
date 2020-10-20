#if PM_DEBUG
//=============================================================================
/**
 * @file    ListLarge.cpp
 * @brief   Newペインリスト(大)サンプル
 * @author  Hiroyuki Nakamura
 * @date    2015.07.30
 */
//=============================================================================

// module
#include "ListLarge.h"
// niji
#include "Sound/include/Sound.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// resource
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll.h"
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll_pane.h"
#include "niji_conv_header/app/test/nakahiro_test/h_scroll/HScroll_anim_list.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Nakahiro)

static const u32 BasePaneTable[] =
{
  PANENAME_LOWER_PANE_BG_201,
  PANENAME_LOWER_PANE_BG_202,
  PANENAME_LOWER_PANE_BG_203,
};

static const u32 IconPaneTable[] =
{
  PANENAME_LOWER_PANE_ITEM_201,
  PANENAME_LOWER_PANE_ITEM_202,
  PANENAME_LOWER_PANE_ITEM_203,
};

static const u32 TextPaneTable[] =
{
  PANENAME_LOWER_PANE_TEXT_201,
  PANENAME_LOWER_PANE_TEXT_202,
  PANENAME_LOWER_PANE_TEXT_203,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
ListLarge::ListLarge( app::util::Heap * heap, app::util::G2DUtil * g2d, app::tool::ItemIcon * icon, app::tool::MenuCursor * cursor )
  : m_heap( heap )
  , m_g2dUtil( g2d )
  , m_itemIcon( icon )
  , m_menuCursor( cursor )
{
  InitListData();

  for( u32 i=0; i<PANE_MAX; i++ )
  {
    m_itemIconReq[i]  = ITEM_DUMMY_DATA;
    m_itemIconLoad[i] = ITEM_DUMMY_DATA;
    m_itemIconSeq[i]  = 0;
  }
  CreatePaneList();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ListLarge::~ListLarge()
{
  DeletePaneList();
}

void ListLarge::InitListData(void)
{
  for( u32 i=0; i<LIST_DATA_MAX; i++ )
  {
    m_listData[i] = i;
  }
}

void ListLarge::CreatePaneList(void)
{
  m_lytwk = m_g2dUtil->GetLayoutWork( 0 );

  App::Tool::NewPaneList::SETUP_PARAM param =
  {
    App::Tool::NewPaneList::TYPE_H,

    m_lytwk,
    m_listPaneData,
    PANE_MAX,

    LIST_DATA_MAX,
    0,
    0,
    0.0f,

    0.0f,
    -80.0f,

    320.0f,

    0,
    0,

    m_menuCursor,
  };

  for( u32 i=0; i<PANE_MAX; i++ )
  {
    m_listPaneData[i].base        = param.lytwk->GetPane( BasePaneTable[i] );
    m_listPaneData[i].bound       = m_listPaneData[i].base;
    m_listPaneData[i].cursor_pos  = m_listPaneData[i].base;
    m_listPaneData[i].anm_touch   = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_release = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_cancel  = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_select  = app::tool::ButtonManager::ANIMATION_NULL;
  }

  m_pNewPaneList = GFL_NEW(m_heap->GetSystemHeap()) App::Tool::NewPaneList( m_heap, &param, this );
  m_pNewPaneList->SetupTouchArea( 160, 240, 0, 320 );
  m_pNewPaneList->SetupInsertPane( param.lytwk->GetPane(PANENAME_LOWER_PANE_INSERT) );
  m_pNewPaneList->SetupNoLoop();
  m_pNewPaneList->SetupInputMode( true, true, true );

/*
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );
  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    m_heap->GetSystemHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_HORIZONTAL,
    LIST_DATA_MAX-7,

    m_lytwk,
    res_id,
    PANENAME_LOWER_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL,
    NULL
  };
  m_pNewPaneList->CreateScrollBar( &scroll_bar_setup );
*/

  m_pNewPaneList->Initialize();
}

void ListLarge::DeletePaneList(void)
{
  if( m_pNewPaneList != NULL )
  {
    m_pNewPaneList->Terminate();
//    m_pNewPaneList->DeleteScrollBar();
    GFL_SAFE_DELETE( m_pNewPaneList );
  }
}

void ListLarge::PaneListListener_Write( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  GFL_ASSERT( pane_index < PANE_MAX );
  GFL_ASSERT( pos < LIST_DATA_MAX );

  m_itemIconReq[pane_index] = m_listData[pos] + 1;

  {
    char str[32];
#if defined(GF_PLATFORM_CTR)
    sprintf( str, "%d", m_listData[pos] );
#elif defined(GF_PLATFORM_WIN32)
    sprintf_s( str, 32, "%d", m_listData[pos] );
#endif
    m_g2dUtil->SetTextboxPaneString( 0, TextPaneTable[pane_index], str );
  }
}

void ListLarge::PaneListListener_MoveCursor( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

void ListLarge::PaneListListener_Decide( u32 pos )
{
  GFL_PRINT( "★Decide -large[%d]\n", pos );
}

void ListLarge::PaneListListener_StartItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

void ListLarge::PaneListListener_EndItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos1, u32 pos2 )
{
  GFL_ASSERT( pos1 < LIST_DATA_MAX );
  GFL_ASSERT( pos2 < LIST_DATA_MAX );

  if( pos1 < pos2 )
  {
    u8 tmp = m_listData[pos1];
    for( s32 i=pos1; i<pos2; i++ )
    {
      m_listData[i] = m_listData[i+1];
    }
    m_listData[pos2] = tmp;
  }
  else if( pos1 > pos2 )
  {
    u8 tmp = m_listData[pos1];
    for( s32 i=pos1; i>pos2; i++ )
    {
      m_listData[i] = m_listData[i-1];
    }
    m_listData[pos2] = tmp;
  }
}

App::Tool::NewPaneList * ListLarge::GetNewPaneList(void)
{
  return m_pNewPaneList;
}

app::tool::MenuCursor * ListLarge::GetMenuCursor(void)
{
  return m_menuCursor;
}


void ListLarge::UpdateItemIcon(void)
{
  for( u32 i=0; i<PANE_MAX; i++ )
  {
    if( m_itemIconReq[i] == ITEM_DUMMY_DATA )
    {
      continue;
    }

    switch( m_itemIconSeq[i] )
    {
    case 0:
      m_itemIcon->LoadRequest( i+14, m_itemIconReq[i] );
      m_itemIconLoad[i] = m_itemIconReq[i];
      m_itemIconSeq[i] = 1;
      /* FALL THROUGH */
    case 1:
      if( m_itemIcon->IsLoadFinished(i+14) == false )
      {
        break;
      }
      // 新しいリクエストが発行されていないか
      if( m_itemIconLoad[i] == m_itemIconReq[i] )
      {
        m_itemIcon->ReplaceReadTexture( i+14, m_lytwk->GetPicturePane(IconPaneTable[i]) );
        m_itemIconReq[i] = ITEM_DUMMY_DATA;
      }
      m_itemIconSeq[i] = 0;
      break;
    }
  }
}


GFL_NAMESPACE_END(Nakahiro)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG

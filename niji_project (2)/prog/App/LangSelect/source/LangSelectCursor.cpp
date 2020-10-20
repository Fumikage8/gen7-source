//======================================================================
/**
 * @file LangSelectCursor.cpp
 * @date 2015/07/24 11:08:12
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectCursor.h"
#include "App/LangSelect/source/LangSelectDrawerBase.h"

#include <AppLib/include/Util/app_util_2d.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/lang_select/lang_select_pane.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
LangSelectCursor::LangSelectCursor( DrawerBase* pBase, app::util::AppRenderingManager* pAppRenderingManager )
  : m_pBase( pBase )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pMenuCursor( NULL )
  , m_pCursorBuffer( NULL )
  , m_Seq( SEQ_LOAD )
  , m_DeleteSeq( 0 )
  , m_Index( 0 )
  , m_LangIndex( 0 )
  , m_DispMode( DISP_MODE_LANG_SELECT )
{
  app::util::Heap* heap = m_pBase->GetHeap();
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // メニューカーソルの生成
  m_pMenuCursor = GFL_NEW(devHeap) app::tool::MenuCursor( heap, &m_pCursorBuffer );

  // NULLカーソルのセット
  static const u32 c_CursorPosIndex[BTN_ID_NUM][2] = {
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_00, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_01, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_02, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_03, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_04, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_05, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_06, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_07, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_08, PANENAME_LANGSELECT_BTN_UPP_000_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_00, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_YES_02, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
    { PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_NO_01, PANENAME_LANGSELECT_BTN_UPP_001_PANE_NULL_CUR },
  };

  for( u32 i=0; i<BTN_ID_NUM; ++i )
  {
    const u32* indexList = c_CursorPosIndex[i];

    gfl2::lyt::LytParts* btnParts = m_pBase->GetParts( indexList[0] );
    m_pCursorPos[i] = m_pBase->GetPane( indexList[1], btnParts );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
LangSelectCursor::~LangSelectCursor( void )
{
  GFL_DELETE m_pMenuCursor;
}


//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LangSelectCursor::Update( void )
{
  if( m_pMenuCursor == NULL ) return;

  m_pMenuCursor->Update();

  switch( m_Seq )
  {
  case SEQ_LOAD:
    {
      if( !m_pMenuCursor->LoadResource() ) break;

      StartMenuCursor();  //! メニューカーソルの開始

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_DELETE:
    {
      if( m_DeleteSeq == 0 )
      {
        m_DeleteSeq++;
      }
      else if( m_DeleteSeq == 1 )
      {
        m_pMenuCursor->DeleteLayoutWork();
        m_pMenuCursor->DeleteResource();

        m_DeleteSeq++;
      }
      else {
        if( !m_pMenuCursor->IsModuleFree() ) break;

        m_Seq = SEQ_END;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LangSelectCursor::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pMenuCursor == NULL ) return;
  if( m_Seq != SEQ_MAIN ) return;

  m_pMenuCursor->Draw( m_pAppRenderingManager, no, gfl2::lyt::DISPLAY_UPPER );
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 * @return  true == 完了
 */
//------------------------------------------------------------------------------
bool LangSelectCursor::IsReady( void )
{
  return m_Seq == SEQ_MAIN;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了可能か？
 * @return  true == 終了、false == 否
 */
//------------------------------------------------------------------------------
bool LangSelectCursor::IsEnd( void )
{
  return m_Seq == SEQ_END;
}


//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの開始
 */
//------------------------------------------------------------------------------
void LangSelectCursor::StartMenuCursor( void )
{
  app::util::G2DUtil* pG2D = m_pBase->GetG2D();

  // メニューカーソルの作成
  m_pMenuCursor->CreateLayoutWork(
    pG2D->GetLayoutSystem(),
    pG2D->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_UPPER ),
    pG2D->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );

  // カーソルを非表示
  SetVisible( false, app::tool::MenuCursor::CURSOR_ALL );

  // アニメの開始
  m_pMenuCursor->StartAnime();
}

//------------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの終了
 */
//------------------------------------------------------------------------------
void LangSelectCursor::EndMenuCursor( void )
{
  SetVisible( false, app::tool::MenuCursor::CURSOR_ALL );

  m_DeleteSeq = 0;
  m_Seq = SEQ_DELETE;
}


//------------------------------------------------------------------------------
/**
 * @brief   画面モードのセット
 * @param   dispMode    画面モード
 */
//------------------------------------------------------------------------------
void LangSelectCursor::SetDispMode( DispMode dispMode )
{
  if( dispMode == DISP_MODE_LANG_SELECT )
  {
    m_Index = m_LangIndex;
  }
  else if( dispMode == DISP_MODE_CONFIRM )
  {
    m_LangIndex = m_Index;
    m_Index = BTN_ID_CONFIRM_YES;
  }
  else {
    m_LangIndex = m_Index;
    m_Index = BTN_ID_CONFIRM_JPN_KANA;
  }

  // カーソル位置の変更
  SetCursorPos( m_Index );

  m_DispMode = dispMode;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置の設定
 */
//------------------------------------------------------------------------------
void LangSelectCursor::SetCursorPos( u32 index, bool playSE )
{
  if( m_pMenuCursor == NULL ) return;
  if( index >= BTN_ID_NUM ) return;

  gfl2::lyt::LytWk* lytwk = m_pBase->GetLytWk();

  m_pMenuCursor->PutNonVisible( lytwk, m_pCursorPos[index] );

  if( m_Index != index )
  {
    if( playSE )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );  //!< カーソル移動音
    }

    m_Index = index;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置の取得
 */
//------------------------------------------------------------------------------
u32 LangSelectCursor::GetCursorPos( void )
{
  return m_Index;
}

//------------------------------------------------------------------------------
/**
 * @brief   キー入力
 */
//------------------------------------------------------------------------------
void LangSelectCursor::InputKey( gfl2::ui::Button* pButton )
{
  u32 index = m_Index;

  if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
  {
    if( m_DispMode == DISP_MODE_CONFIRM )
    {
      if( index == BTN_ID_CONFIRM_YES ) index = BTN_ID_CONFIRM_NO;
      else index = BTN_ID_CONFIRM_YES;
    }
    else if( m_DispMode == DISP_MODE_CONFIRM_JPN )
    {
      if( index == BTN_ID_CONFIRM_JPN_KANA )
      {
        index = BTN_ID_CONFIRM_JPN_NO;
      }
      else {
        index--;
      }
    }

    // カーソル位置の変更
    SetCursorPos( index );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
  {
    if( m_DispMode == DISP_MODE_CONFIRM )
    {
      if( index == BTN_ID_CONFIRM_YES ) index = BTN_ID_CONFIRM_NO;
      else index = BTN_ID_CONFIRM_YES;
    }
    else if( m_DispMode == DISP_MODE_CONFIRM_JPN )
    {
      if( index == BTN_ID_CONFIRM_JPN_NO )
      {
        index = BTN_ID_CONFIRM_JPN_KANA;
      }
      else {
        index++;
      }
    }

    // カーソル位置の変更
    SetCursorPos( index );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示切替
 * @param   visible   表示フラグ
 * @param   id        カーソルID(デフォルト = CURSOR_LEFT)
 */
//------------------------------------------------------------------------------
void LangSelectCursor::SetVisible( bool visible, u32 id )
{
  m_pMenuCursor->SetVisible( visible, id );
}

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

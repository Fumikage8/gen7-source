//============================================================================================
/**
 * @file		app_tool_MenuCursor.cpp
 * @brief		汎用メニューカーソル
 * @author	Hiroyuki Nakamura
 * @date		12.05.29
 */
//============================================================================================
#include "AppLib/include/tool/app_tool_MenuCursor.h"

#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/MenuCursor.gaix"

#include "niji_conv_header/app/common_cursor/MenuCursor.h"
#include "niji_conv_header/app/common_cursor/MenuCursor_pane.h"
#include "niji_conv_header/app/common_cursor/MenuCursor_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

const f32 MenuCursor::S_WIDTH_MIN = 48.0f;    //!< 小カーソルの最小幅（テクスチャサイズ：24x2）
const f32 MenuCursor::L_WIDTH_MIN = 64.0f;    //!< 大カーソルの最小幅（テクスチャサイズ：32x2）

//! カーソルデータ
const MenuCursor::CURSOR_DATA MenuCursor::CursorData[ MenuCursor::CURSOR_SIZE_MAX ] =
{
  {
    LYTRES_MENUCURSOR_COMMONCURSOR_SML_LOW_000_BFLYT,
    PANENAME_COMMONCURSOR_SML_LOW_000_PANE_CUR_MAIN,
    {
      PANENAME_COMMONCURSOR_SML_LOW_000_PANE_COMMON_CURSOR_L,
      PANENAME_COMMONCURSOR_SML_LOW_000_PANE_COMMON_CURSOR_R,
    },
    MenuCursor::S_WIDTH_MIN,
  },

  {
    LYTRES_MENUCURSOR_COMMONCURSOR_LRG_LOW_000_BFLYT,
    PANENAME_COMMONCURSOR_LRG_LOW_000_PANE_CUR_MAIN,
    {
      PANENAME_COMMONCURSOR_LRG_LOW_000_PANE_COMMON_CURSOR_L,
      PANENAME_COMMONCURSOR_LRG_LOW_000_PANE_COMMON_CURSOR_R,
    },
    MenuCursor::L_WIDTH_MIN,
  },

  {
    LYTRES_MENUCURSOR_COMMONCURSOR_BTL_LOW_000_BFLYT,
    PANENAME_COMMONCURSOR_BTL_LOW_000_PANE_CUR_MAIN,
    {
      PANENAME_COMMONCURSOR_BTL_LOW_000_PANE_COMMON_CURSOR_L,
      PANENAME_COMMONCURSOR_BTL_LOW_000_PANE_COMMON_CURSOR_R,
    },
    MenuCursor::L_WIDTH_MIN
  },
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 * @param   buf   リソースの確保場所
 */
//-----------------------------------------------------------------------------
MenuCursor::MenuCursor( app::util::Heap * heap, void ** buf )
  : m_heap( heap )
  , m_pLytResBuff( buf )
  , m_lytsys( NULL )
  , m_pLytWk( NULL )
  , m_subSequence( 0 )
  , m_isCreate( false )
  , m_isModuleFree( true )
  , m_isDrawEnable( true )
{
  m_cursorData = &CursorData[SIZE_SMALL];
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MenuCursor::~MenuCursor()
{
  GFL_ASSERT_STOP( IsModuleFree() != false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   アップデート
 */
//-----------------------------------------------------------------------------
void MenuCursor::Update(void)
{
  if( m_pLytWk != NULL )
  {
    m_pLytWk->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   描画
 *
 * @param   render_manager  描画マネージャ
 * @param   disp            描画先ディスプレイ
 */
//-----------------------------------------------------------------------------
void MenuCursor::Draw( app::util::AppRenderingManager * render_manager, gfl2::gfx::CtrDisplayNo displayNo, gfl2::lyt::DisplayType disp, u8 order, u32 layerNo )
{
  if( m_pLytWk == NULL )
  {
    return;
  }

  if( m_isDrawEnable == false )
  {
    return;
  }

  if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
  {
    if( disp == gfl2::lyt::DISPLAY_UPPER )
    {
      render_manager->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pLytWk, layerNo, order );
    }
  }
  else if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    if( disp == gfl2::lyt::DISPLAY_LOWER )
    {
      render_manager->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pLytWk, layerNo, order );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    IsModuleFree
 * @brief   クラスを破棄できるか
 * @date    2015.03.25
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MenuCursor::IsModuleFree(void)
{
  return m_isModuleFree;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadResourceSync
 * @brief   リスース読み込み（同期版）
 * @date    2015.03.25
 */
//-----------------------------------------------------------------------------
void MenuCursor::LoadResourceSync(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_MENU_CURSOR, m_heap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_MENU_CURSOR,
    GARC_MenuCursor_MenuCursor_applyt_COMP,
    m_pLytResBuff,
    m_heap->GetDeviceHeap(),
    true,
    nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
  app::util::FileAccessor::FileCloseSync( ARCID_MENU_CURSOR );
  m_isCreate = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadResource
 * @brief   リスース読み込み（非同期版）
 * @date    2015.03.25
 *
 * @retval  true  = 読み込み中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuCursor::LoadResource(void)
{
  if( m_isCreate != false )
  {
    return true;
  }

  switch( m_subSequence )
  {
  case 0:
    m_isModuleFree = false;
    app::util::FileAccessor::FileOpen( ARCID_MENU_CURSOR, m_heap->GetSystemHeap() );
    m_subSequence++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( ARCID_MENU_CURSOR ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_MENU_CURSOR,
      GARC_MenuCursor_MenuCursor_applyt_COMP,
      m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSequence++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_MENU_CURSOR, m_heap->GetSystemHeap() );
    m_subSequence++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( ARCID_MENU_CURSOR ) == false )
    {
      break;
    }
    m_isModuleFree = true;
    m_isCreate = true;
    m_subSequence = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteResource
 * @brief   リスース削除
 * @date    2015.03.25
 */
//-----------------------------------------------------------------------------
void MenuCursor::DeleteResource(void)
{
  if( m_isCreate != false && *m_pLytResBuff != NULL )
  {
    GflHeapFreeMemory( *m_pLytResBuff );
    m_pLytResBuff = NULL;
    m_isCreate = false;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateLayoutWork
 * @brief   レイアウトワーク作成
 * @date    2015.03.25
 *
 * @param   lytsys      レイアウトシステム
 * @param   setup       セットアップデータ
 * @param   accessor    applytファイルアクセサ
 * @param   size        カーソルサイズ ( 大小 )
 * @param   auto_draw   オート描画フラグ ( true = 有効 )
 */
//-----------------------------------------------------------------------------
void MenuCursor::CreateLayoutWork( gfl2::lyt::LytSys * lytsys, const gfl2::lyt::LytWkSetUp * setup, app::util::AppLytAccessor * accessor, CursorSize size, bool auto_draw )
{
  m_lytsys     = lytsys;
  m_cursorData = &CursorData[size];

  accessor->SetAppLytBuf( *m_pLytResBuff );

  m_resID = lytsys->InitMultiResource( m_heap->GetDeviceHeap(), 1 );
  m_resID = lytsys->LoadLytRes( &m_resID, m_heap->GetDeviceAllocator(), accessor->GetLytDat(), false, 0 );

  m_pLytWk = m_lytsys->CreateLytWk(
              &m_resID,
              m_cursorData->bflyt,
              accessor->GetAnimeTable(m_cursorData->bflyt),
              accessor->GetAnimeMax(m_cursorData->bflyt),
              accessor->GetAnimeMax(m_cursorData->bflyt),
              setup );
  m_pLytWk->SetAutoDrawFlag( auto_draw );
  m_pLytWk->SetInvisiblePaneCalculateMtx( false );
  SetVisible( false );
  m_isDrawEnable = true;  // @note レイアウト破棄後に再生成されて使用する場合があるため
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteLayoutWork
 * @brief   レイアウトワーク削除
 * @date    2015.03.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MenuCursor::DeleteLayoutWork(void)
{
  if( m_pLytWk != NULL )
  {
    m_isDrawEnable = false;
    if( m_pLytWk->IsDrawing() != false )  // @note MenuCursorのインスタンスに対してこれと同じ判定を行い、falseが返って来たらDeleteLayoutWorkでその場で直ちに破棄する、というのをやっている箇所が外部にあるので、破棄できるか否かの判定を変更する際は全員に連絡するようにして下さい。
    {
      return false;
    }

    m_lytsys->RemoveLytWk( m_pLytWk );
    m_pLytWk = NULL;

    if( m_resID.IsEnable() != false ){
       m_lytsys->ReleaseLytRes( &m_resID );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク取得
 *
 * @return  レイアウトワーク
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytWk * MenuCursor::GetLayoutWork(void)
{
  return m_pLytWk;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPane
 * @brief   カーソルペイン取得
 * @date    2015.03.25
 *
 * @return  カーソルのペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPane * MenuCursor::GetPane(void)
{
  return m_pLytWk->GetPane( m_cursorData->base );
}

//-----------------------------------------------------------------------------
/**
 * @brief   アニメ開始
 */
//-----------------------------------------------------------------------------
void MenuCursor::StartAnime(void)
{
  _StartAnime( PLAY_ANIME_ID_IDLE, true );
}

//-----------------------------------------------------------------------------
/**
 * @brief   座標設定
 *
 * @param   pos   座標
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetPos( gfl2::math::VEC3 * pos )
{
  if( m_pLytWk == NULL )
  {
    return;
  }
  GetPane()->SetTranslate( *pos );
}

//-----------------------------------------------------------------------------
/**
 * @brief   座標設定（ペイン指定）
 *
 * @param   pane  基準となるペイン
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetPos( gfl2::lyt::LytPane * pane )
{
  gfl2::math::VEC3 pos = pane->GetTranslate();
  SetPos( &pos );
}

//-----------------------------------------------------------------------------
/**
 * @brief   サイズ設定
 *
 * @param   w   幅
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetSize( f32 w )
{
  if( m_pLytWk == NULL )
  {
    return;
  }

  if( w < m_cursorData->min_width )
  {
    w = m_cursorData->min_width;
  }

  nw::lyt::Size	size = GetPane()->GetSize();
  size.width = w;
  GetPane()->SetSize( size );
}

//-----------------------------------------------------------------------------
/**
 * @brief   サイズ設定（ペイン指定）
 *
 * @param   pane  基準となるペイン
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetSize( gfl2::lyt::LytPane * pane )
{
  nw::lyt::Size	size = pane->GetSize();
  SetSize( size.width );
}

//-----------------------------------------------------------------------------
/**
 * @brief   表示切り替え
 *
 * @param   flg   true = 表示, false = 非表示
 * @param   id    カーソルID
 *
 * @li  id = CURSOR_LEFT  : 左カーソル
 * @li  id = CURSOR_RIGHT : 右カーソル
 * @li  id = CURSOR_ALL   : 全体
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetVisible( bool flg, u32 id )
{
  if( m_pLytWk == NULL )
  {
    return;
  }

  if( id & CURSOR_LEFT )
  {
    m_pLytWk->GetPane(m_cursorData->cursor[ID_LEFT])->SetVisible( flg );
  }
#if 0
  if( id & CURSOR_RIGHT )
  {
    m_pLytWk->GetPane(m_cursorData->cursor[ID_RIGHT])->SetVisible( flg );
  }
#endif
  if( id == CURSOR_ALL )
  {
    GetPane()->SetVisible( flg );
  }
  else
  {
    if( flg != false )
    {
      GetPane()->SetVisible( true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソルが表示されているか
 *
 * @param   id    カーソルID
 *
 * @retval  true  = 表示されている
 * @retval  false = それ以外
 *
 * @li  id = CURSOR_LEFT  : 左カーソル
 * @li  id = CURSOR_RIGHT : 右カーソル
 * @li  id = CURSOR_ALL   : 全体
 */
//-----------------------------------------------------------------------------
bool MenuCursor::IsVisible( u32 id )
{
  if( GetPane()->IsVisible() == false )
  {
    return false;
  }
  if( id & CURSOR_LEFT )
  {
    if( m_pLytWk->GetPane(m_cursorData->cursor[ID_LEFT])->IsVisible() == false )
    {
      return false;
    }
  }
#if 0
  if( id & CURSOR_RIGHT )
  {
    if( m_pLytWk->GetPane(m_cursorData->cursor[ID_RIGHT])->IsVisible() == false )
    {
      return false;
    }
  }
#endif
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソル配置
 *
 * @param   lytwk   基準ペインのレイアウトワーク
 * @param   base    基準ペイン
 */
//-----------------------------------------------------------------------------
void MenuCursor::Put( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * base )
{
  PutNonVisible( lytwk, base );
  SetVisible( true );
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソル配置  (内部のvisibleがない版)
 *
 * @param   lytwk   基準ペインのレイアウトワーク
 * @param   base    基準ペイン
 */
//-----------------------------------------------------------------------------
void MenuCursor::PutNonVisible( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * base )
{
  gfl2::lyt::LytPane * basePane = base;
  gfl2::lyt::LytPane * rootPane = lytwk->GetLayoutCore()->GetRootPane();
  gfl2::math::VEC3	pos = basePane->GetTranslate();

  SetSize( basePane );

  for ever
  {
    gfl2::lyt::LytPane * parentPane = basePane->GetParent();
    // 親がいない or 親がルートペイン
    if( parentPane == NULL || parentPane == rootPane )
    {
      break;
    }
    pos += parentPane->GetTranslate();
    basePane = parentPane;
  }

  //  同期ペインの回転とスケール値をカーソルに設定
  {
    gfl2::lyt::LytPane* pPane = GetPane();
    GFL_ASSERT( pPane );

    pPane->SetRotate( base->GetRotate() );
    pPane->SetScale( base->GetScale() );
  }

  SetPos( &pos );
}

//-----------------------------------------------------------------------------
/**
 * @brief   オート描画フラグ設定
 *
 * @param   flg   描画フラグ
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetAutoDrawFlag( bool flg )
{
  if( m_pLytWk != NULL )
  {
    m_pLytWk->SetAutoDrawFlag( flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   表示プライオリティ設定
 *
 * @param   priority  プライオリティ
 *
 * @li	オート描画時のみ有効
 */
//-----------------------------------------------------------------------------
void MenuCursor::SetPriority( u32 priority )
{
  if( m_pLytWk != NULL )
  {
    m_pLytWk->SetPriority( priority );
  }
}

//-----------------------------------------------------------------------------
/**
  * @func    _StartAnime
  * @brief   アニメ再生
  * @date    2015.06.02
  *
  * @param   animeIdx    : 再生アニメidx指定
  * @parma   bAutoUnbind : アニメのバインドを自動で外すか
  */
//-----------------------------------------------------------------------------
void MenuCursor::_StartAnime( const u32 animeIdx, const bool bAutoUnbind )
{
  GFL_ASSERT( m_pLytWk );

//  m_pLytWk->SetAutoAnimeSpeed( 0, 1.0f );
  if( m_pLytWk->IsBindAnime( animeIdx ) == false )
  {
    m_pLytWk->BindAnime( animeIdx, bAutoUnbind );
  }

  m_pLytWk->SetAutoAnime( animeIdx, true );
  m_pLytWk->SetAnimeFrame( animeIdx, 0.0f );
}

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

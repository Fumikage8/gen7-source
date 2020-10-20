//======================================================================
/**
 * @file    FinderStudioViewBase.cpp
 * @date    2016/11/18 11:19:07
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioViewUtil.h>

#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_KeyRepeatController.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )


//////////////////////////////////////////////////////////////////////////
///
/// @brief    ViewBaseクラス
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
ViewBase::ViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf/* = NULL*/ )
  : app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
  , m_pHeap( heap )
  , m_pAppRenderingManager( renderMan )
  , m_pMenuCursor( NULL )
  , m_pCommonIcon( NULL )
  , m_pKeyController( NULL )
  , m_pUtil( NULL )
  , m_pCursorBuf( cursorBuf )
  , m_Result( 0 )
  , m_CursorPos( 0 )
  , m_bDrawEnable( false )
{
  ;
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    継承関数群
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   初期化
 */
void ViewBase::Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  GFL_UNUSED( lytDatBuff );
  GFL_UNUSED( msgData );
  GFL_UNUSED( wordSet );
}


/**
 * @brief   レイアウトシステム 初期化・破棄
 */
void ViewBase::InitializeSystem( void )
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );
}

void ViewBase::TerminateSystem( void )
{
  Delete2D();
  DeleteGraphicsSystem();
}


/**
 * @brief   更新
 */
void ViewBase::Update( void )
{
  _Update();
}

void ViewBase::_Update( void )
{
  app::ui::UIView::Update();

  if( m_pMenuCursor ) m_pMenuCursor->Update();
}

/**
 * @brief   描画
 */
void ViewBase::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_bDrawEnable )
  {
    _Draw( no );
  }
}

void ViewBase::_Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* g2d = this->GetG2DUtil();

  if( g2d ) g2d->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
  if( m_pMenuCursor ) m_pMenuCursor->Draw( m_pAppRenderingManager, no, gfl2::lyt::DISPLAY_LOWER );
}


/**
 * @brief   表示：Viewを表示
 */
void ViewBase::Show( void )
{
  _Show();
}

void ViewBase::_Show( void )
{
  this->SetVisible( true );
  this->SetInputEnabled( true );

  m_bDrawEnable = true;
}

/**
 * @brief   非表示：Viewを非表示
 */
void ViewBase::Hide( void )
{
  _Hide();
}

void ViewBase::_Hide( void )
{
  InvisibleCursor();
  this->SetVisible( false );
  this->SetInputEnabled( false );

  m_bDrawEnable = false;
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    カーソル
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void ViewBase::CreateMenuCursor( void )
{
  m_pMenuCursor = GFL_NEW( m_pHeap->GetSystemHeap() ) app::tool::MenuCursor( m_pHeap, &m_pCursorBuf );

  app::util::G2DUtil* g2d = m_pUtil->GetG2D();
  
  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );

  m_pMenuCursor->StartAnime();
}

/**
 * @brief   破棄
 */
void ViewBase::DeleteMenuCursor( void )
{
  m_pMenuCursor->DeleteLayoutWork();
  GFL_DELETE( m_pMenuCursor );
}

/**
 * @brief   カーソルの配置
 *
 * @param   pane    配置先ペイン
 * @param   visible true なら配置時に表示、false なら配置時に表示変更を行わない
 */
void ViewBase::PutCursor( gfl2::lyt::LytPane* pane, bool visible/* = true*/ )
{
  if( m_pMenuCursor )
  {
    gfl2::lyt::LytWk* lytwk = m_pUtil->GetLytWk();

    if( visible )
    {
      m_pMenuCursor->Put( lytwk, pane );
    }
    else {
      m_pMenuCursor->PutNonVisible( lytwk, pane );
    }
  }
}

/**
 * @brief   カーソルを表示
 */
void ViewBase::VisibleCursor( void )
{
  if( m_pMenuCursor )
  {
    m_pMenuCursor->SetVisible( true );
  }
}

/**
 * @brief   カーソルを非表示
 */
void ViewBase::InvisibleCursor( void )
{
  if( m_pMenuCursor )
  {
    m_pMenuCursor->SetVisible( false );
  }
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief     ViewUtilクラス
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void ViewBase::CreateViewUtil( u32 lytID )
{
  m_pUtil = GFL_NEW( m_pHeap->GetSystemHeap() ) ViewUtil( this, lytID );
}

/**
 * @brief   破棄
 */
void ViewBase::DeleteViewUtil( void )
{
  GFL_SAFE_DELETE( m_pUtil );
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    View設定
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ボタンアニメ中のキー入力設定
 */
void ViewBase::SetUIInputExclusiveKeyByButtonAnimation( void )
{
  if( m_pUtil )
  {
    m_pUtil->SetPrevInputExclusiveKeyByBtnAnim();
    m_pUtil->SetInputExclusiveKeyByBtnAnim( true );
  }
}

/**
 * @brief   ボタンアニメ中のキー入力設定解除
 */
void ViewBase::ResetUIInputExclusiveKeyByButtonAnimation( void )
{
  if( m_pUtil )
  {
    m_pUtil->ResetInputExclusiveKeyByBtnAnim();
  }
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    キーリピートコントローラ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void ViewBase::CreateKeyRepeatController( u32 posMaxY, u32 posMaxX )
{
  if( posMaxY > 0 ) posMaxY--;
  if( posMaxX > 0 ) posMaxX--;

  m_pKeyController = GFL_NEW( m_pHeap->GetSystemHeap() ) app::util::KeyRepeatController( posMaxY, posMaxX );
}

/**
 * @brief   破棄
 */
void ViewBase::DeleteKeyRepeatController( void )
{
  GFL_SAFE_DELETE( m_pKeyController );
}



//////////////////////////////////////////////////////////////////////////
///
/// @brief    ButtonInfoクラス
///
//////////////////////////////////////////////////////////////////////////

/// @brief    デフォルト：ButtonInfo構造体
app::ui::ButtonInfoEx ButtonInfo::m_DefBtnInfo = {
  0, NULL, NULL,
  BTN_ANIM_TOUCH,
  BTN_ANIM_RELEASE,
  BTN_ANIM_CANCEL,
  BTN_ANIM_TOUCH_RELEASE,
  BTN_ANIM_ACTIVE,
  BTN_ANIM_PASSIVE,
};

/**
 * @brief   ButtonInfoのセット
 */
void ButtonInfo::SetupBtnInfo( app::ui::ButtonInfoEx* info, u32 id, u32 touchAnim )
{
  *info = m_DefBtnInfo;

  info->button_id = id;

  if( touchAnim != app::tool::ButtonManager::ANIMATION_NULL )
  {
    info->touch_anime_index = touchAnim + BTN_ANIM_TOUCH;
    info->release_anime_index = touchAnim + BTN_ANIM_RELEASE;
    info->cancel_anime_index = touchAnim + BTN_ANIM_CANCEL;
    info->key_select_anime_index = touchAnim + BTN_ANIM_TOUCH_RELEASE;
    info->active_anime_index = touchAnim + BTN_ANIM_ACTIVE;
    info->passive_anime_index = touchAnim + BTN_ANIM_PASSIVE;
  }
  else {
    info->touch_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
    info->release_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
    info->cancel_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
    info->key_select_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
    info->active_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
    info->passive_anime_index = app::tool::ButtonManager::ANIMATION_NULL;
  }
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

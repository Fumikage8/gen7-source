//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_ScrollPaneManager.cpp
 *	@brief  スクロールペインマネージャー
 *	@author	Toru=Nagihashi
 *	@date		2012.06.28
 *	
 *
 *	//覚え書き カーソルと座標の検知はいらないのでは？
 *	      タッチしたスクロールペインそれ自体がインデックスを持っているので
 *	      タッチ→タッチしたものがそのままカーソル？
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "AppLib/include/Tool/app_tool_TouchPanelOperator.h"
#include "AppLib/include/Tool/app_tool_ScrollPaneManager.h"
#include "AppLib/include/Tool/app_tool_ScrollValue.h"

#include "niji_conv_header/sound/SoundMiddleID.h"

#include <Sound/include/Sound.h>
#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  
//-----------------------------------------------------------------------
// 定数
//-----------------------------------------------------------------------
/**
 * @brief 連続押しでのカーソル
 */
const int ScrollPaneManager::KEY_HOLD_SPEED[ KEY_HOLD_SPEED_LEN ] =
{
  8, 6, 3, 1,
};

//-----------------------------------------------------------------------
// 生成・破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param desc  設定構造体
 */
ScrollPaneManager::ScrollPaneManager( const Description & desc ) :
  UIView( desc.heap ),
  m_desc( desc ),
  m_state_seq(0),
  m_scroll( 0 ),
  m_scroll_target( 0 ),
  m_scroll_add( 0 ),
  m_inertia( 0 ),
  m_scroll_trans(),
  m_next_scroll_cursor_pos( 0 ),
  m_cursor_pos( 0 ),
  m_key_hold_count( 0 ),
  m_key_hold_speed_index( 0 ),
  m_is_drag(false),
  m_is_req_cursor_pos(0),
  m_is_finish_key_move(0),
  m_is_update_enable(true),
  m_is_input_key_enable(true),
  m_on_button_action(NULL),
  m_on_key_action(NULL),
  m_scroll_dir(0)
{
  this->CreateButtonManager();
  this->CreateButton();
  this->CreateScrollPane();
  this->CalcCursorScroll( m_desc.default_cursor_pos );
  this->NextState( &ScrollPaneManager::StateInputWait );
  this->SetInputListener( this );

  m_draw_area.SetOriginAndSize( 
   m_desc.pane_draw_area_left,
   m_desc.pane_draw_area_top,
   m_desc.pane_draw_area_right - m_desc.pane_draw_area_left,
   m_desc.pane_draw_area_down - m_desc.pane_draw_area_top );
  
  m_input_area.SetOriginAndSize( 
   m_desc.pane_start_x,
   m_desc.pane_start_y,
   (m_desc.pane_input_limit_col-1) * m_desc.pane_width,
   -((m_desc.pane_input_limit_row-1) * m_desc.pane_height) );

  if( m_desc.list_data_num == 0 )
  {
    if( m_desc.menu_cursor )
    {
      m_desc.menu_cursor->SetVisible( false );
    }
  }
}

/**
 * @brief デストラクタ
 */
ScrollPaneManager::~ScrollPaneManager()
{
  this->DeleteScrollPane();
} 
/**
 * @brief セルリスト  更新
 */
void ScrollPaneManager::Update( void )
{
  if( m_desc.list_data_num == 0 )
  {
    return;
  }
  
  if( !m_is_update_enable )
  {
    return;
  }

  //ここでUpdateResponseが周り、入力検知
  this->UIView::Update();
    
  //各状態の処理
  if( !this->IsButtonAnimation() && this->IsInputEnabled() )
  {
    this->UpdateState();
  }
 
  
  m_scroll = gfl2::math::Clamp( m_scroll, static_cast<s16>(0), this->GetScrollMax() );

  //ペインと擬似ペインの連結処理
  this->UpdateScrollPane();

  //カーソル位置更新
  this->UpdateCursorPos();
  

  //入力をクリア
  m_on_button_action = NULL;
  m_on_key_action = NULL;
}



//-----------------------------------------------------------------------
// 操作
//-----------------------------------------------------------------------
/**
 * @brief   タッチパッド、ボタン、アナログキーの入力検出有効設定
 *
 * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
 *
 * @return  none
 */
void ScrollPaneManager::SetInputEnabled( bool isEnable )
{
  this->app::ui::UIResponder::SetInputEnabled( isEnable );
  
  if( m_desc.menu_cursor )
  {
    m_desc.menu_cursor->SetVisible( isEnable );
  }
}
//-----------------------------------------------------------------------
/**
 * @brief   ビュー自体の入力可能状態を設定する
 *
 * @param   isEnable falseで入力を検出した状態
 */
void ScrollPaneManager::SetViewInputEnabled( bool isEnable )
{
  this->m_isInputEnable = isEnable;
}

/**
 * @brief   アナログキーの入力検出有効設定
 *
 * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
 *
 * @return  none
 */
void ScrollPaneManager::SetInputKeyEnabled( bool isEnable )
{
  m_is_input_key_enable = isEnable;
}

/**
 * @brief スクロール現在値の取得
 *
 * @return スクロール現在値の取得
 * 
 * @note ScrollBarにセットするための値です。
 */
s16 ScrollPaneManager::GetScrollValue( void ) const
{
  return m_scroll;
}

/**
 * @brief スクロール値の設定
 *
 * @param scroll  0〜GetScrollMaxまでの値
 * 
 * @note  スクロールバーをドラッグした時などに設定してください
 */
#if 0
void ScrollPaneManager::SetScrollValue( s16 scroll )
{
  s16 next = gfl::math::Clamp( scroll, static_cast<s16>(0), this->GetScrollMax() );
 
  if( next/m_desc.pane_height != m_scroll/m_desc.pane_height )
  {
    xy_snd::PlaySE(SEQ_SE_SELECT1);
  }
  m_scroll  = next;

  this->NextState( &ScrollPaneManager::StateAutoScroll );
}
#endif
/**
 * @brief リストすべてを呼ぶ
 *
 * @param param ForEachUpdate関数に渡る値
 */
void ScrollPaneManager::ForEach( int param )
{

  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    u32 use_pane  = 0;
    if( it->IsUsedPane() )
    {
      use_pane = it->GetUsedPaneID();
    }
    m_desc.listener->ForEachUpdate( param, it->IsUsedPane(), use_pane, it->GetDataID() );
  }
}

/**
 * @brief 入力検知フラグセット
 */
void ScrollPaneManager::SetButtonInputEnable( u32 dataID, bool is_enable )
{
  for(PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it)
  {
    ScrollPane* scroll_pane = &(*it);
    if( scroll_pane->GetDataID() == dataID )
    {
      scroll_pane->SetInputEnable( is_enable );
      break;
    }
  }
}

/**
 * @brief じょうたいを取得
 *
 * @return State
 */
ScrollPaneManager::State ScrollPaneManager::GetState( void ) const
{
  if( this->IsCompareState(&ScrollPaneManager::StateDecideAnime) )
  {
    return STATE_DECIDE_ANIME;
  }
  else if( this->IsCompareState(&ScrollPaneManager::StateInputWait) )
  {
    return STATE_INPUT_WAIT;
  }
  else if( this->IsCompareState(&ScrollPaneManager::StateKeyScroll) )
  {
    return STATE_KEY_SCROLL;
  }
  else if( this->IsCompareState(&ScrollPaneManager::StateDragScroll) )
  {
    return STATE_DRAG_SCROLL;
  }
  else if( this->IsCompareState(&ScrollPaneManager::StateAutoScroll) )
  {
    return STATE_AUTO_SCROLL;
  }
  else if( this->IsCompareState(&ScrollPaneManager::StateRequestScroll) )
  {
    return STATE_REQ_SCROLL;
  }
  
  GFL_ASSERT(0);
  return STATE_INPUT_WAIT;
}


/**
 * @brief メニューカーソルの表示
 *
 * @param is_visible true表示 false非表示
 */
void ScrollPaneManager::SetVisibleMenuCursor( bool is_visible )
{
  if( is_visible )
  {
    m_is_req_cursor_pos = true;
  }
  else
  {
    m_desc.menu_cursor->SetVisible( false );
  }
}

/**
 * @brief スクロール値の設定リクエスト
 *
 * @param scroll  0〜GetScrollMaxまでの値
 * 
 * @note  スクロールバーをドラッグした時などに設定してください
 */
void ScrollPaneManager::RequestScroll( s16 scroll )
{
  
#if 0
  scroll = scroll / m_desc.pane_height * m_desc.pane_height;
  m_scroll_target  = gfl::math::Clamp( scroll, static_cast<s16>(0), this->GetScrollMax() );
  
  m_scroll_add = (m_scroll_target - m_scroll) / 4;
  this->NextState( &ScrollPaneManager::StateRequestScroll );
#else
  s16 next = gfl2::math::Clamp( scroll, static_cast<s16>(0), this->GetScrollMax() );
 
  if( next != m_scroll )
  {
    if( next/m_desc.pane_height != m_scroll/m_desc.pane_height )
    {
      s16 value = next - m_scroll;
      if( ( m_scroll_dir == 0 
            || (m_scroll_dir > 0 && 0 < value )
            || (m_scroll_dir < 0 && 0 > value )
          )  )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      m_scroll_dir  = value;
    }
    m_scroll  = next;
    
    this->NextState( &ScrollPaneManager::StateAutoScroll );
  }
#endif
}

//-----------------------------------------------------------------------
// 継承の実装
//-----------------------------------------------------------------------
/**
 * @brief 押されたボタン
 *
 * @param buttonID  ボタンID
 */
::app::ui::UIInputListener::ListenerResult ScrollPaneManager::OnLayoutPaneEvent( const u32 painId )
{
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    if( it->IsUsedPane() )
    {
      if( it->GetUsedPaneID() == painId ) 
      {
        
        m_is_drag = false;
        this->UpdateGridScroll( true );

        //カーソル表示
        s16 cursor = this->GetLocalPos(it->GetDataID());
        this->SetCursor( this->GetCol( cursor ), this->GetRow( cursor ) );
        
        m_desc.listener->Decide( it->GetUsedPaneID(), it->GetDataID() );
        
        this->NextState( &ScrollPaneManager::StateInputWait );
        return DISABLE_ACCESS;
      }
    }
  }
  
  return ENABLE_ACCESS;
}
/**
 * @brief  キー入力の検知
 *         ※タッチパネルの入力が優先される
 *
 * @param  pKey    十字キー
 * @param  pStick  アナログスティック
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
::app::ui::UIInputListener::ListenerResult ScrollPaneManager::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_is_input_key_enable )
  {
    m_on_button_action = pButton;
    m_on_key_action    = pKey;
  }

  return ENABLE_ACCESS;
}
/**
 * @brief  タッチパネルイベントの検知
 *
 * @param  pTouchPanel タッチパネル
 * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
 *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
::app::ui::UIInputListener::ListenerResult ScrollPaneManager::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( !this->IsInputEnabled() || this->GetState() == STATE_DECIDE_ANIME )
  {
    return ENABLE_ACCESS;
  }

  // タッチされた（かつリリースイベントを取得していない）
  if( isTouch && !pTouchPanel->IsTouchRelease() )
  {
    // 初めてタッチ
    if( pTouchPanel->IsTouchTrigger() )
    {
      if( this->IsTouchArea( pTouchPanel->GetX(), pTouchPanel->GetY() ) )
      {
        u32 pane;
        if( this->GetTriggerPane( pTouchPanel->GetX(), pTouchPanel->GetY(), &pane ) )
        {
          this->OnLayoutPaneTouchTriggerEventMine( pane );
        }

        m_is_drag = true;
        this->Drag( 0, 0, true );
        return DISABLE_ACCESS;
      }
    }
    // ドラッグ
    else if( m_is_drag )
    {
      this->Drag( pTouchPanel->GetMoveX(), -pTouchPanel->GetMoveY(), false );
      return DISABLE_ACCESS;
    }
  }
  // リリースされた
  else if( m_is_drag )
  {
    this->NextState( &ScrollPaneManager::StateAutoScroll );
    m_is_drag = false;
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

void ScrollPaneManager::OnLayoutPaneStartingAnimeEvent( const u32 paneId )
{  
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    if( it->IsUsedPane() )
    {
      if( it->GetUsedPaneID() == paneId ) 
      {
        m_desc.listener->DecideBeforeAnime( it->GetUsedPaneID(), it->GetDataID() );
        this->NextState( &ScrollPaneManager::StateDecideAnime );
      }
    }
  }
}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param  painId   通知を受けるペインＩＤ
  *
  * @return none
  * 
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  */
  //--------------------------------------------------------------------------------------------
void ScrollPaneManager::OnLayoutPaneTouchTriggerEventMine( const u32 paneId )
{
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    if( it->IsUsedPane() )
    {
      if( it->GetUsedPaneID() == paneId ) 
      {
        if( m_button_manager->IsButtonInputEnable(paneId) == false )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }

        m_is_drag = false;
        this->UpdateGridScroll( true );

        //カーソル表示
        s16 cursor = this->GetLocalPos(it->GetDataID());
        this->SetCursor( this->GetCol( cursor ), this->GetRow( cursor ) );
        return;
      }
    }
  }
}

//-----------------------------------------------------------------------
// PRIVATE 生成・破棄
//-----------------------------------------------------------------------
/**
 * @brief ボタンマネージャー作成
 */
void ScrollPaneManager::CreateButtonManager( void )
{
  //UIViewのCreateButtonManagerをつかわない
  m_button_manager  = GFL_NEW(m_desc.heap->GetSystemHeap()) app::tool::ButtonManager( m_desc.heap->GetSystemHeap(), NULL, m_desc.pane_table_num );
}
/**
 * @brief ボタン作成
 */
void ScrollPaneManager::CreateButton( void )
{
  for( u32 i=0; i< m_desc.pane_table_num; ++i )
  {
    PaneInfo* info = m_desc.pane_table;

    m_button_manager->AddButton( m_desc.heap->GetSystemHeap(), i, m_desc.layout_work,
        info[i].root_pane,
        info[i].bounding_pane,
        info[i].hold_anime_index,
        info[i].release_anime_index,
        info[i].cancel_anime_index,
        info[i].select_anime_index,
        app::tool::ButtonManager::ANIMATION_NULL,
        app::tool::ButtonManager::ANIMATION_NULL,
        this );
    
    m_button_manager->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
  }
  
  m_button_manager->SetButtonMode( app::tool::ButtonManager::BUTTON_MODE_SCROLL );
}

/**
 * @brief スクロールペイン作成
 */
void ScrollPaneManager::CreateScrollPane( void )
{
  m_use_pane_flag = static_cast<u8*>(GflHeapAllocMemory( m_desc.heap->GetSystemHeap(), sizeof(u8)*m_desc.pane_table_num ));
  gfl2::std::MemClear( m_use_pane_flag, sizeof(u8) *m_desc.pane_table_num  );

  m_scroll_pane = GFL_NEW_ARRAY( m_desc.heap->GetSystemHeap() ) ScrollPane[ m_desc.list_data_num ];

  //一旦全部のペインを消す
  for( u32 i = 0; i < m_desc.pane_table_num; ++i  )
  {
    PaneInfo* info = m_desc.pane_table;

    info[i].root_pane->SetVisible( false );
  }

  //追加
  for( int i = 0; i < m_desc.list_data_num; ++i)
  {
    this->AddActivePane( i );
  }
  
  GFL_PRINT( "Size %d %d\n", m_active_list.GetSize(), m_desc.list_data_num );
}

/**
 * @brief スクロールペイン破棄
 */
void ScrollPaneManager::DeleteScrollPane( void )
{
  
  m_active_list.clear();

  GFL_DELETE_ARRAY m_scroll_pane;
  GflHeapFreeMemory( m_use_pane_flag );
}

//-----------------------------------------------------------------------
// PRIVATE 状態関係
//-----------------------------------------------------------------------
/**
 * @brief 状態の更新処理
 */
void ScrollPaneManager::UpdateState( void )
{
  //各状態の処理
  if( m_state != NULL )
  {
    (this->*m_state)();
  }
}
/**
 * @brief 入力待機状態
 */
void ScrollPaneManager::StateInputWait( void )
{
  gfl2::ui::Button *button = this->GetButton();
  gfl2::ui::VectorDevice *key = this->GetKey();
  
  m_scroll_dir  = 0;
  if( button == NULL || key == NULL )
  {
    this->ResetKeyHoldCound();
    return;
  }
  
  //十字キー押しっぱなしチェック(押しっぱなしだとスクロール速度が上がる)
  if( key->IsHold( gfl2::ui::BUTTON_UP|gfl2::ui::BUTTON_DOWN ) )
  {
    this->IncKeyHoldCount();
  }

  //上下キーを押した時
  if( key->IsRepeat(gfl2::ui::BUTTON_UP) )
  {
    s16 next_col = this->GetCursorCol();
    s16 next_row = this->GetCursorRow()-1;
    //カーソル移動ができる
    if( this->CanMoveCursor( next_col, next_row ) )
    {
      //カーソル移動領域内
      if( this->IsMoveLimitCursor( next_col, next_row ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetCursor( next_col, next_row );
      }
      else  //領域外なのでスクロール
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetupScroll( 0, -1 );
        this->NextState( &ScrollPaneManager::StateKeyScroll );
      }
    }
  }
  else if( key->IsRepeat(gfl2::ui::BUTTON_DOWN) )
  {
    //↓にいくときは階段状になっている場合に
    //きちんと一番下にいくようにする
    
    s16 next_col = this->GetCursorCol();
    s16 next_row = this->GetCursorRow()+1;
    
    
    
    //カーソル移動ができる
    if( this->CanMoveCursor( next_col, next_row ) )
    {
      //カーソル移動領域内
      if( this->IsMoveLimitCursor( next_col, next_row ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetCursor( next_col, next_row );
      }
      else  //領域外なのでスクロール
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetupScroll( 0, 1 );
        this->NextState( &ScrollPaneManager::StateKeyScroll );
      }
    }
    else if( this->GetRow( this->GetGrobalCursor() ) < this->GetRow( m_desc.list_data_num-1 ) )
    {
      //行けなくても、もし後ろに行があれば最後尾にいく
      GFL_PRINT( "nowRow%d lastRow%d cursorRow%d\n", this->GetRow( this->GetGrobalCursor() ), this->GetRow( m_desc.list_data_num ) );
      
      //最後尾までスクロールしていれば、カーソルが変わるだけ
      //していなかったらスクロールする
      if( this->GetScrollMax() == m_scroll )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
        
        s16 last_cursor = this->GetLocalPos( m_desc.list_data_num-1 );
        this->SetCursor( this->GetCol( last_cursor ), this->GetRow( last_cursor ) );
      }
      else  //領域外なのでスクロール
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetupScrollLast();
        this->NextState( &ScrollPaneManager::StateKeyScroll );
      }
    }
  }
  //左右キーを押した時
  else if( key->IsRepeat(gfl2::ui::BUTTON_LEFT) )
  {
    s16 next_col = this->GetCursorCol()-1;
    s16 next_row = this->GetCursorRow();
    //カーソル移動ができる
    if( this->CanMoveCursor( next_col, next_row ) )
    {
      //カーソルが行内にいるならカーソル移動
      if( this->IsMoveLimitCursor( next_col, next_row ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetCursor( next_col, next_row );
      }
      else  //カーソルが行外にいったのでスクロールしてカーソル位置移動
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetupScroll( -1, 0 );
        this->NextState( &ScrollPaneManager::StateKeyScroll );
      }
    }
  }
  else if( key->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
  {
    s16 next_col = this->GetCursorCol()+1;
    s16 next_row = this->GetCursorRow();
    //カーソル移動ができる
    if( this->CanMoveCursor( next_col, next_row ) )
    {
      //カーソルが行内にいるならカーソル移動
      if( this->IsMoveLimitCursor( next_col, next_row ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetCursor( next_col, next_row );
      }
      else  //カーソルが行外にいったのでスクロールしてカーソル位置移動
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );

        this->SetupScroll( 1, 0 );
        this->NextState( &ScrollPaneManager::StateKeyScroll );
      }
    }
  }
  else if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
    {
      if( it->GetDataID() == static_cast<u32>(this->GetGrobalCursor()) && it->IsUsedPane()
          && it->GetInputEnable() )
      {
        m_desc.listener->DecideBeforeAnime( it->GetUsedPaneID(), it->GetDataID() );
        m_button_manager->StartSelectedAct( it->GetUsedPaneID() );
        this->NextState( &ScrollPaneManager::StateDecideAnime );
        break;
      }
    }
  }
}

/**
 * @brief ボタンでのスクロール状態
 */
void ScrollPaneManager::StateKeyScroll( void )
{
  enum
  {
    SEQ_START,
    SEQ_WAIT,
  };

  switch( this->GetStateSeq() )
  {
  case SEQ_START:
    this->StartScroll();
    this->IncStateSeq();
    // fall
    
  case SEQ_WAIT:
    this->UpdateScroll();
    if( this->IsEndScroll() )
    {     
      this->SetCursor( this->GetCol(m_next_scroll_cursor_pos), this->GetRow(m_next_scroll_cursor_pos) );
      this->NextState( &ScrollPaneManager::StateInputWait );
    }
    break;
  }
}
/**
 * @brief ドラッグによるスクロール状態
 */
void ScrollPaneManager::StateDragScroll( void )
{
  //Drag関数の中で動作をおこなっている
}
/**
 * @brief オートスクロール状態
 */
void ScrollPaneManager::StateAutoScroll( void )
{ 
  this->UpdateDragScroll();
  
  if( this->IsOnGrid() )
  {
    GFL_PRINT( "スクロール終わった？\n" );
    m_is_req_cursor_pos = true;

    this->NextState( &ScrollPaneManager::StateInputWait );
  }
  
  //何か入力した
  gfl2::ui::Button *button = this->GetButton();
  gfl2::ui::VectorDevice *key = this->GetKey();
  if( ( button && button->GetTrigger() ) || ( key && key->GetTrigger() ) )
  {
     if( m_desc.menu_cursor )
    {
      m_desc.menu_cursor->SetVisible( true );
    }

    this->UpdateGridScroll( true );
    this->NextState( &ScrollPaneManager::StateInputWait );
  }
}

/**
 * @brief RequestScrollを使った時の動作
 */
void ScrollPaneManager::StateRequestScroll( void )
{ 
  if( m_scroll > m_scroll_target )
  {
    m_scroll  += m_scroll_add;
    if( m_scroll <= m_scroll_target )
    {
      m_scroll  = m_scroll_target;
    }
  }
  else if( m_scroll < m_scroll_target )
  {
    m_scroll  += m_scroll_add;
    if( m_scroll >= m_scroll_target )
    {
      m_scroll  = m_scroll_target;
    }
  }

  if( this->IsOnGrid() && m_scroll_target == m_scroll )
  {
    GFL_PRINT( "スクロール終わった？\n" );
    m_is_req_cursor_pos = true;

    this->NextState( &ScrollPaneManager::StateInputWait );
  }
  
  //何か入力した
  gfl2::ui::Button *button = this->GetButton();
  gfl2::ui::VectorDevice *key = this->GetKey();
  if( ( button && button->GetTrigger() ) || ( key && key->GetTrigger() ) )
  {
    if( m_desc.menu_cursor )
    {
      m_desc.menu_cursor->SetVisible( true );
    }

    this->UpdateGridScroll( true );
    this->NextState( &ScrollPaneManager::StateInputWait );
  }
}

/**
 * @brief 決定アニメ待ち
 */
void ScrollPaneManager::StateDecideAnime( void )
{
}

//シーケンス制御
void ScrollPaneManager::NextState( StateFunction state )
{
  m_state_seq = 0;
  m_state  = state;
}
//シーケンスが指定のものと同じかどうか
bool ScrollPaneManager::IsCompareState( StateFunction state ) const
{
  return m_state == state;
}
//状態内シーケンス制御
void ScrollPaneManager::IncStateSeq( void )
{
  ++m_state_seq;
}
u32 ScrollPaneManager::GetStateSeq( void ) const
{
  return m_state_seq;
}


//-----------------------------------------------------------------------
// PRIVATE スクロールペインの管理
//-----------------------------------------------------------------------
/**
 * @brief ペインを設定
 *
 * @param dataID  データID
 */
void ScrollPaneManager::AddActivePane( u32 dataID )
{
  for( s32 i = 0; i < m_desc.list_data_num; ++i )
  {
    ScrollPane* scroll_pane = &m_scroll_pane[i];

    // 未使用のキャラクターペインを再利用する
    if( scroll_pane->IsEnable() ){
      continue;
    }

    // 作成
    scroll_pane->Create( dataID, m_desc.pane_width, m_desc.pane_height );
    m_active_list.push_back( scroll_pane );
    
    //座標設定
    this->SetScrollPanePosition();
    return;
  }
  
  GFL_ASSERT(0);
}

/**
 * @brief  スクロールペインの座標の設定
 */
void ScrollPaneManager::SetScrollPanePosition( void )
{
  int cnt = 0;
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    ScrollPanePosition target = GetPositionByPos( cnt );
    it->SetPosition( target );
    ++cnt;
  }
}


/**
 * @brief スクロールペインの更新処理
 */
void ScrollPaneManager::UpdateScrollPane( void )
{
  for(PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it)
  {
    ScrollPane* scroll_pane = &(*it);
    //座標によってペインを割り当て・解除する
    this->RemoveUsingCharaIcon( scroll_pane );
  }

  for(PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it)
  {
    ScrollPane* scroll_pane = &(*it);

    //座標によってペインを割り当て・解除する
    this->AssignUsingCharaIcon( scroll_pane );
    
    //入力範囲内ならばボタン入力可、
    this->UpdateEnableButton( scroll_pane );
    
    // アイコン座標の更新
    this->UpdateImagePanePosition( scroll_pane );
  }
}
/**
 * @brief  キャラクターペインの座標値によって、使用するイメージペインを更新する
 *
 * @param  scroll_pane 操作対象キャラクターペイン
 */
void ScrollPaneManager::AssignUsingCharaIcon( ScrollPane* scroll_pane )
{
  if( !scroll_pane->IsUsedPane() )
  {
    // 画面内のキャラクターペインにイメージペインを割り当て

    if( scroll_pane->IsDisplayIn( 0, m_scroll, m_draw_area ) ){
      AssignImagePane( scroll_pane );
    }
  }
}

/**
 * @brief  キャラクターペインの座標値によって、使用するイメージペインを更新する
 *
 * @param  scroll_pane 操作対象キャラクターペイン
 */
void ScrollPaneManager::RemoveUsingCharaIcon( ScrollPane* scroll_pane )
{
  if( scroll_pane->IsUsedPane() )
  {
    // 画面外のキャラクターペインが使用しているイメージペインを解放

    if( !scroll_pane->IsDisplayIn( 0, m_scroll, m_draw_area ) )
    {
      RemoveImagePane( scroll_pane );
      scroll_pane->FreePane();
    }
  }
}

/**
 * @brief ボタンの入力可否設定
 *
 * @param charaPane スクロールペイン
 */
void ScrollPaneManager::UpdateEnableButton( ScrollPane* scroll_pane )
{
  if( scroll_pane->IsUsedPane() )
  {
    //モード別の処理  mode
    bool is_enable  = true;
    switch( m_desc.mode )
    {
    case MODE_NORMAL:
      is_enable = true;
      break;
    case MODE_DOUBLE_TOUCH:
      is_enable = false;
      if( this->IsOnGrid() )
      {
        if( scroll_pane->GetDataID() == static_cast<u32>(this->GetGrobalCursor()) )
        {
          is_enable = true;
        }
      }
      break;
    }

    //入力範囲内
    if( scroll_pane->IsDisplayIn( 0, m_scroll, m_input_area )
        && scroll_pane->GetInputEnable()
        && is_enable )
    {
      m_button_manager->SetButtonInputEnable( scroll_pane->GetUsedPaneID(), true );
    }
    else
    {
      //範囲外
      m_button_manager->SetButtonInputEnable( scroll_pane->GetUsedPaneID(), false );
    }
  }
}

/**
 * @brief  キャラクターアイコンの表示座標設定
 *
 * @param  scroll_pane 操作対象キャラクターペイン
 *
 */
void ScrollPaneManager::UpdateImagePanePosition( ScrollPane* scroll_pane )
{
  if( scroll_pane->IsUsedPane() )
  {
    const u32            paneIdx  = scroll_pane->GetUsedPaneID();
    u32 dataID         = scroll_pane->GetDataID();
    const ScrollPanePosition&    charPos  = scroll_pane->GetPosition();
    f32                  baseX    = gfl2::math::S16ToF32(charPos.GetX());
    f32                  baseY    = gfl2::math::S16ToF32(charPos.GetY())+m_scroll;
    gfl2::math::VEC3      imgPanePos;

    imgPanePos.x = baseX;
    imgPanePos.y = baseY;
    imgPanePos.z = 0.0f;
      
    switch( scroll_pane->IsLoading() )
    {
    case ScrollPane::LOAD_STATE_LOADING:
      if( m_desc.listener->IsLoadedTexture( paneIdx, dataID ) )
      {
        // ローディング終了処理
        scroll_pane->FinishLoading();
        m_desc.listener->WriteTexture( paneIdx, dataID, true );
      }
      break;
      
    case ScrollPane::LOAD_STATE_NONE:
      if( m_desc.listener->StartLoadTexture( paneIdx, dataID ) )
      {
        scroll_pane->StartLoading();
      }
      break;
    }

    PaneInfo* info = m_desc.pane_table;
    info[paneIdx].root_pane->SetTranslate( imgPanePos );
  }
}

/**
 * @brief  キャラクターペインに対してイメージペインを割り当てる.
 *
 * @param  scroll_pane 割り当て対象となるキャラクターペイン
 *
 * @return 割り当てが行われたならtrueを返却する
 */
bool ScrollPaneManager::AssignImagePane( ScrollPane* scroll_pane )
{
  for( u32 i = 0; i < m_desc.pane_table_num; ++i )
  {
    if( !m_use_pane_flag[i] )
    {
      m_use_pane_flag[i] = true;
      scroll_pane->UsePaneID( i );

      // テクスチャー差し替え
      ReplacePlayerIconTexture( scroll_pane );
      return true;
    }
  }
  return false;
}

/**
 * @brief  確保されているイメージペインを解除
 *
 * @param  scroll_pane 解除対象となるキャラペイン情報
 */
void ScrollPaneManager::RemoveImagePane( ScrollPane* scroll_pane )
{
  // イメージペインを非表示
  {
    if( scroll_pane->IsUsedPane() )
    {
      u32 imagePaneIdx = scroll_pane->GetUsedPaneID();

      PaneInfo* info = m_desc.pane_table;
      info[imagePaneIdx].root_pane->SetVisible( false );

      m_use_pane_flag[imagePaneIdx] = false;
    }
  }
}


/**
 * @brief  キャラクターペインのプレイヤーアイコンテクスチャを更新する
 *
 * @param  scroll_pane 生成対象キャラクターペイン
 */
void ScrollPaneManager::ReplacePlayerIconTexture( ScrollPane* scroll_pane )
{
  if( scroll_pane->IsUsedPane() )
  {
    u32 imagePaneIdx   = scroll_pane->GetUsedPaneID();
    u32 dataID         = scroll_pane->GetDataID();

    PaneInfo* info = m_desc.pane_table;
    info[imagePaneIdx].root_pane->SetVisible( true );

//    this->SetPaneAlpha( paneInf.root_pane, scroll_pane->GetAlpha() );
    
    //非同期かつ読み込んでいたor非同期ではない→テクスチャ変更。
    //読み込んでいない→ダミーテクスチャにする
    if( !m_desc.listener->IsAsyncWrite()
        || m_desc.listener->IsLoadedTexture( imagePaneIdx, dataID ) )
    {
      //読み込んでいたので書き込み
      m_desc.listener->WriteTexture( imagePaneIdx, dataID, true );
    }
    else
    {
      //読み込み開始     
      bool ret = m_desc.listener->StartLoadTexture( imagePaneIdx, dataID );
      m_desc.listener->WriteTexture( imagePaneIdx, dataID, false );

      // ローディング状態に設定
      if( ret )
      {
        scroll_pane->StartLoading();
      }
    }
  }
}
//-----------------------------------------------------------------------
// PRIVATE スクロール関係
//-----------------------------------------------------------------------
/**
 * @brief スクロール最大値
 *
 * @return  スクロール最大値
 */
s16 ScrollPaneManager::GetScrollMax( void ) const
{
  if( this->IsScrollable() )
  {
    return m_desc.pane_height * ( this->GetRow(m_active_list.size()-1) - (m_desc.pane_input_limit_row-1));
  }
  else
  {
    return 0;
  }
}

/**
 * @brief スクロール可能かどうか
 *
 * @return true可能 false不可能
 */
bool ScrollPaneManager::IsScrollable( void ) const
{
  s32 distance = gfl2::math::Abs( m_desc.pane_draw_area_down - m_desc.pane_draw_area_top );
  
  // GFL_PRINT( "d%d ad%d at%d h%d num%d\n", distance, m_desc.pane_draw_area_down, m_desc.pane_draw_area_top, m_desc.pane_height, m_desc.list_data_num  );
  return this->GetRow(m_desc.list_data_num-1) >= ( distance / m_desc.pane_height ) - 1;
}


/**
 * @brief 更新フラグ
 *
 * @param is_enable
 */
void ScrollPaneManager::SetUpdateEnableFlag( bool is_enable )
{
  m_is_update_enable  = is_enable;
}

/**
 * @brief ボタンのホールド状況をリセットする
 */
void ScrollPaneManager::ResetHoldState( void )
{
  m_button_manager->ResetHoldState();
}

/**
 * @brief カーソル位置設定
 *
 * @param next_col 列
 * @param next_row 行
 */
void ScrollPaneManager::SetCursor( s16 next_col, s16 next_row )
{
  s16 next_pos  = this->GetPos( next_col, next_row );
  // if( m_cursor_pos != next_pos )
  {
    m_cursor_pos  = next_pos;
    GFL_PRINT( "SetCursor %d\n",m_cursor_pos );

    m_is_req_cursor_pos = true;
  }
  
}


/**
 * @brief カーソル位置の更新
 */
void ScrollPaneManager::UpdateCursorPos( void )
{
  if( m_is_req_cursor_pos && ( this->IsOnGrid() ) )
  {   
    m_is_req_cursor_pos = false;
    
    //もし画面外にいったペインをタッチしていたら、枠内に戻す
    if( m_cursor_pos < 0 )
    {
      m_cursor_pos  += m_desc.col;
    }
    s16 cursor_limit = this->GetPos( m_desc.pane_input_limit_col-1, m_desc.pane_input_limit_row-1 );
    if( m_cursor_pos > cursor_limit )
    {
      m_cursor_pos  -= m_desc.col;
    }
    
    ScrollPane *scroll_pane = this->GetScrollPaneByPos( this->GetGrobalCursor() );
    
    if( !scroll_pane )
    {
      m_cursor_pos  -= m_desc.col;
      scroll_pane = this->GetScrollPaneByPos( this->GetGrobalCursor() );
      
    }
    
    if( scroll_pane )
    {
      if( scroll_pane->IsUsedPane() )
      {
        m_desc.listener->OnCursor( scroll_pane->GetUsedPaneID(), scroll_pane->GetDataID() );
        m_is_finish_key_move  = true;

        if( m_desc.menu_cursor )
        {
          PaneInfo* info = m_desc.pane_table;
          
          if( m_desc.put_cursor_visible )
          {
            m_desc.menu_cursor->Put( m_desc.layout_work, info[scroll_pane->GetUsedPaneID()].cursor_pane );
          }
          else {
            m_desc.menu_cursor->PutNonVisible( m_desc.layout_work, info[scroll_pane->GetUsedPaneID()].cursor_pane );
          }
        }
      }
    }
  }
  
  if( m_is_finish_key_move && ( this->IsOnGrid() ) )
  {
    ScrollPane *scroll_pane = this->GetScrollPaneByPos( this->GetGrobalCursor() );
    if( scroll_pane == NULL )
    {
      GFL_ASSERT_STOP( 0 ); //@fix
      return;
    }
    
    gfl2::ui::Button *button = this->GetButton();
    gfl2::ui::VectorDevice *key = this->GetKey();
    if( ( button == NULL || button->GetHold() == 0 ) && ( key == NULL || key->GetHold() == 0 ) )
    {
      m_is_finish_key_move  = false;
      m_desc.listener->OnFinishKeyMove( scroll_pane->GetUsedPaneID(), scroll_pane->GetDataID() );
    }
    
  }
}

/**
 * @brief カーソル移動ができるかどうか
 *
 * @param next_col 列
 * @param next_row 行
 *
 * @return true カーソル移動可  カーソル移動  不可能
 */
bool ScrollPaneManager::CanMoveCursor( s16 next_col, s16 next_row ) const
{
  s16 next_cursor_pos = this->GetPos( next_col, next_row );
  s16 next_groval_cursor = this->GetGrobalPos( next_cursor_pos );

  return 0 <= next_groval_cursor && next_groval_cursor < m_desc.list_data_num;
}
/**
 * @brief カーソルの駆動範囲内か
 *
 * @param next_col 列
 * @param next_row 行
 *
 * @return true 駆動範囲内  false範囲外（スクロールをすべき）
 */
bool ScrollPaneManager::IsMoveLimitCursor( s16 next_col, s16 next_row ) const
{
  GFL_ASSERT( this->CanMoveCursor( next_col, next_row ) );  //@checkプログラム的に見たいだけ
  
  s16 next_cursor = this->GetPos( next_col, next_row );
  s16 cursor_limit = this->GetPos( m_desc.pane_input_limit_col-1, m_desc.pane_input_limit_row-1 );
  
  if( 0 <= next_cursor && next_cursor <= cursor_limit )
  {
    return true; 
  }
  
  return false;
}
/**
 * @brief スクロールの設定
 *
 * @param move_col  行移動量→だと↓に１
 * @param move_row  列移動量↓に１
 */
void ScrollPaneManager::SetupScroll( s16 move_col, s16 move_row )
{
  s16 move  = 0;
  if( move_row == 0 )
  {
    if( move_col != 0 )
    {
      move = move_col;
      //左右に移動したときは目的カーソルの位置が変わる
      m_next_scroll_cursor_pos  = m_cursor_pos + -move_col * ( m_desc.col - 1 );
      
      //左右スクロール時は一旦消す
      if( m_desc.menu_cursor )
      {
        m_desc.menu_cursor->SetVisible( false );
      }

    }
  }
  else
  {
    move  = move_row;
    //上下に移動したときは目的カーソルのいちは変わらない
    m_next_scroll_cursor_pos  = m_cursor_pos;
  }


  s16 add_value = move * m_desc.pane_height;

  f32 start = gfl2::math::S16ToF32(m_scroll);
  f32 end = gfl2::math::S16ToF32(m_scroll + add_value);
  m_scroll_trans.Restart(start, end , KEY_HOLD_SPEED[ m_key_hold_speed_index ] );
}

/**
 * @brief 最後のセルへスクロールする
 */
void ScrollPaneManager::SetupScrollLast( void )
{
  //一旦消す
  if( m_desc.menu_cursor )
  {
    m_desc.menu_cursor->SetVisible( false );
  }
  
  m_next_scroll_cursor_pos  = (m_desc.list_data_num-1) - (m_scroll / m_desc.pane_height) * m_desc.col;

  f32 start = gfl2::math::S16ToF32(m_scroll);
  f32 end = gfl2::math::S16ToF32(m_scroll + m_desc.pane_height);
  m_scroll_trans.Restart(start, end , KEY_HOLD_SPEED[ m_key_hold_speed_index ] );
}

/**
 * @brief スクロール開始
 */
void ScrollPaneManager::StartScroll( void )
{
  //実際はSetupでやっている
}
/**
 * @brief スクロール更新
 */
void ScrollPaneManager::UpdateScroll( void )
{
  f32 now = m_scroll_trans.Update();
  m_desc.listener->OnScroll( now - m_scroll );

  m_scroll = static_cast<s16>( now );
}
/**
 * @brief スクロール終了チェック
 *
 * @return true終了 false続行
 */
bool ScrollPaneManager::IsEndScroll( void ) const
{
  return m_scroll_trans.IsEnd();
}

/**
 * @brief 現在のカーソルの列を取得（X)
 *
 * @return 列
 */
s16 ScrollPaneManager::GetCursorCol( void ) const
{
  return this->GetCol( m_cursor_pos );
}
/**
 * @brief 現在のカーソルの行を取得（Y）
 *
 * @return 行
 */
s16 ScrollPaneManager::GetCursorRow( void ) const
{
  return this->GetRow( m_cursor_pos );
}

/**
 * @brief カーソル絶対位置を取得する
 *
 * @return カーソル
 */
s16 ScrollPaneManager::GetGrobalCursor( void ) const
{
  GFL_ASSERT( this->IsOnGrid() );//@checkプログラム的に見たいだけ
  return this->GetGrobalPos( m_cursor_pos );
}

/**
 * @brief スクロールの長さにより慣性の倍率を求める
 *
 * @return 倍率
 */
f32 ScrollPaneManager::GetIneriaRate( void ) const
{
  f32 rate = 1.0f;
  if( m_desc.framerate == GameSys::FRAMEMODE_60 )
  {
    rate = 2.0f;
  }

  if( m_desc.list_data_num < 10 )
  {
    return 1.0f * rate;
  }
  else if( m_desc.list_data_num < 50 )
  {
    return 1.5f * rate;
  }
  else if( m_desc.list_data_num < 100 )
  {
    return 2.0f * rate;
  }
  else
  {
    return 3.0f * rate;
  }
}

/**
 * @brief ドラッグのスクロール処理
 */
void ScrollPaneManager::UpdateDragScroll( void )
{
  // 慣性スクロール
  if( m_inertia != 0 )
  {
    if( this->GetScrollMax() == m_scroll || m_scroll == 0 ){
      m_inertia = 0;  // 目標地点に到達
    }
    m_desc.listener->OnScroll( m_inertia );

    this->AddScrollValue( m_inertia, true );
    m_inertia   = app::tool::ReduceScrollValue( m_inertia );
  }

  // スクロールバー表示状態解除
  this->UpdateGridScroll( false );

  // スクロール制限
  m_scroll = gfl2::math::Clamp( m_scroll, static_cast<s16>(0), this->GetScrollMax() );
}

/**
 * @brief グリッド処理
 * @param  is_force true強制的にグリッドの上に乗せる  false徐々に載る
 */
void ScrollPaneManager::UpdateGridScroll( bool is_force )
{

  const s16 gridSpeed = 6;  //grid speed
  if( gridSpeed >= gfl2::math::Abs(m_inertia) || is_force )
  {
    m_inertia   = 0;
    
    s16 gridComplement = gfl2::math::Abs(m_scroll % m_desc.pane_height);
    if( gridComplement != 0 )
    {
      //スクロールは↑に移動し、m_scrollの値が大きいほど↑にいく
      if( gridComplement > (m_desc.pane_height>>1) )
      {
        //余りが、高さの半分より大きいということは↑に行くのが近いので、足しにいく
        
        //高さの半分より大きいので、足しに行く
        //ただし、埋めるのは余り〜高さまで
        gridComplement  =  m_desc.pane_height - gridComplement;
        if( is_force )
        {
          this->AddScrollValue( gridComplement, !is_force );

          GFL_ASSERT_MSG( (m_scroll % m_desc.pane_height) == 0, "scr%d comp%d he%d\n", m_scroll, gridComplement, m_desc.pane_height );//@checkプログラム的に見たいだけ
        }
        else
        {
          s16 value = ((gridComplement < gridSpeed)?1:gridSpeed);
          this->AddScrollValue( value, !is_force );
        }
      }
      else
      {
        //高さの半分より小さいので、引きに行く
        //埋めるのは０〜余りまで
        if( is_force )
        {
          this->AddScrollValue( -gridComplement, !is_force );
          GFL_ASSERT_MSG( (m_scroll % m_desc.pane_height) == 0, "scr%d comp%d he%d\n", m_scroll, gridComplement, m_desc.pane_height );//@checkプログラム的に見たいだけ
        }
        else
        {
          s16 value = ((gridComplement < gridSpeed)?1:gridSpeed);
          
          this->AddScrollValue( -value, !is_force );
        }
      }
      GFL_PRINT( "Grid scr%d comp%d he%d\n", m_scroll, gridComplement, m_desc.pane_height );
    }
  }
}


/**
 * @brief グリッドの上にのっているか
 *
 * @return true乗っている false載っていない
 */
bool ScrollPaneManager::IsOnGrid( void ) const
{
  return (m_scroll % m_desc.pane_height) == 0 && m_inertia == 0 && m_is_drag == false;
  //m_scroll_trans.IsEnd()はスクロール中しかみないし、
  //スクロール中は必ずリクエストされてから動くので監視の必要なし
}

/**
 * @brief カーソル、スクロール位置を計算
 *
 * @param global_cursor グローバルカーソル位置
 */
void ScrollPaneManager::CalcCursorScroll( s16 global_cursor )
{
  m_scroll  = this->GetRow(global_cursor) * m_desc.pane_height;
  m_scroll = gfl2::math::Clamp( m_scroll, static_cast<s16>(0), this->GetScrollMax() );
  s16 local_cursor = this->GetLocalPos( global_cursor );
  this->SetCursor( this->GetCol( local_cursor ), this->GetRow( local_cursor ) );
}

/**
 * @brief スクロール値の更新
 *
 * @param value 加算値
 * @param play_se trueでSE再生
 */
void ScrollPaneManager::AddScrollValue( s16 value, bool play_se )
{
  s16 next  = m_scroll + value;
  
  if( next/m_desc.pane_height != m_scroll/m_desc.pane_height )
  {
    if( play_se && value != 0
        &&
        ( m_scroll_dir == 0 
          || (m_scroll_dir > 0 && 0 < value )
          || (m_scroll_dir < 0 && 0 > value )
          )  )
    {
      Sound::PlaySE(SEQ_SE_SELECT1);
    }
    m_scroll_dir  = value;
  }

  m_scroll  = next;
}

//-----------------------------------------------------------------------
// PRIVATE 入力関係
//-----------------------------------------------------------------------
/**
 * @brief ボタン取得
 *
 * @return  ボタン
 */
gfl2::ui::Button * ScrollPaneManager::GetButton(void) const
{
  return m_on_button_action;//m_desc.ui_device_manager->GetButton( gfl::ui::CTR_DeviceManager::BUTTON_STICK_EMU );
}
/**
 * @brief キー取得
 *
 * @return  キー
 */
gfl2::ui::VectorDevice * ScrollPaneManager::GetKey(void) const
{
  return m_on_key_action;
}
/**
 * @brief キー押し続け処理
 */
void ScrollPaneManager::IncKeyHoldCount( void )
{
  s16 add_value = 1;
  
  if( m_desc.framerate == GameSys::FRAMEMODE_30 )
  {
    add_value = 2;
  }

  m_key_hold_count  += add_value;
  if( m_key_hold_count > KEY_HOLD_SPEED_CHANGE_CNT )
  {
    m_key_hold_count  = 0;
    if( ++m_key_hold_speed_index >= KEY_HOLD_SPEED_LEN )
    {
      m_key_hold_speed_index  = KEY_HOLD_SPEED_LEN-1;
    }
  }
}
/**
 * @brief キー押し続け処理
 */
void ScrollPaneManager::ResetKeyHoldCound( void )
{
  m_key_hold_count  = 0;
  m_key_hold_speed_index  = 0;
}

/**
 * @brief ドラッグ処理
 *
 * @param moveX 移動値
 * @param moveY 移動値
 * @param isTouchTrigger  トリガー
 */
void ScrollPaneManager::Drag( s16 moveX, s16 moveY, bool isTouchTrigger )
{
  m_desc.listener->OnScroll( moveY );

  this->AddScrollValue( moveY, true );

  m_inertia   = gfl2::math::F32ToS16( gfl2::math::S16ToF32(moveY) * this->GetIneriaRate() );
  this->NextState( &ScrollPaneManager::StateDragScroll );
  

  //スクロール時は一旦消す
  if( m_desc.menu_cursor )
  {
    m_desc.menu_cursor->SetVisible( false );
  }
}

/**
 * @brief タッチ範囲内か計算
 *
 * @param touch_x タッチX座標
 * @param touch_y タッチY座標
 *
 * @return  trueタッチ範囲内  false外
 */
bool ScrollPaneManager::IsTouchArea( s16 touch_x, s16 touch_y ) const
{	
  // タッチ座標からディスプレイ座標への変換
  s16 x =  touch_x - (DISPLAY_LOWER_WIDTH>>1);
  s16 y = (DISPLAY_LOWER_HEIGHT>>1) - touch_y;

  //s16 touch_area_x  = m_desc.pane_start_x - m_desc.pane_width/2;
  //s16 touch_area_y  = m_desc.pane_start_y + m_desc.pane_height/2;
  s16 touch_area_x = static_cast<s16>( m_desc.pane_input_start_x );
  s16 touch_area_y = static_cast<s16>( m_desc.pane_input_start_y );
  s16 touch_area_w  = (m_desc.pane_input_limit_col-1) * m_desc.pane_width + m_desc.pane_width;
  s16 touch_area_h  = (m_desc.pane_input_limit_row-1) * m_desc.pane_height + m_desc.pane_height;
  
  // GFL_PRINT( "touch x%d y%d w%d h%d px%dpy%d\n", touch_area_x, touch_area_y, touch_area_w, touch_area_h, x, y );

  return ( touch_area_x <= x && x <= (touch_area_x + touch_area_w)
    && ((touch_area_y - touch_area_h) <= y) && (y <= touch_area_y ) );
}

//ペイン関係

/**
 * @brief ペイン位置設定
 *
 * @param index ペインインデックス
 * @param pos 位置
 */
void ScrollPaneManager::SetPanePos( gfl2::lyt::LytPaneIndex index, const gfl2::math::VEC3 & pos )
{
	gfl2::lyt::LytPane * pane = m_desc.layout_work->GetPane( index );
	if( pane ){
		pane->SetTranslate( pos );
	}
}

/**
 * @brief ペインの表示設定
 *
 * @param index ペインインデックス
 * @param is_visible  true表示  false非表示
 */
void ScrollPaneManager::SetPaneVisible( gfl2::lyt::LytPaneIndex index, bool is_visible )
{
	gfl2::lyt::LytPane * pane = m_desc.layout_work->GetPane( index );
	if( pane ){
		pane->SetVisible( is_visible );
	}
}
/**
 * @brief ペインの表示取得
 *
 * @param index ペインインデックス
 *
 * @return  true 表示中 false非表示
 */
bool ScrollPaneManager::GetPaneVisible( gfl2::lyt::LytPaneIndex index ) const
{
	gfl2::lyt::LytPane * pane = m_desc.layout_work->GetPane( index );
	if( pane ){
		return pane->IsVisible();
	}
	return false;
}

/**
 * @brief 位置からペインIDを取得する
 *
 * @param pos 位置
 *
 * @return ペインID
 * 
 * @note  ペインが止まっていないと使えない(座標をもとに検索するため)
 */
u32 ScrollPaneManager::GetPaneIDByPos( s16 pos )
{
  GFL_ASSERT( this->IsOnGrid() );//@checkプログラム的に見たいだけ
  ScrollPanePosition target = this->GetPositionByPos( pos );
  
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    if( it->GetPosition() == target )
    {
      GFL_ASSERT( it->IsUsedPane() );//@checkプログラム的に見たいだけ
      return it->GetUsedPaneID();
    }
  }

  GFL_ASSERT(0);
  return 0;
}


/**
 * @brief 指定位置からスクロールペインを探す
 *
 * @param pos 位置（GrobalPos）
 *
 * @return スクロールペイン
 */
ScrollPane * ScrollPaneManager::GetScrollPaneByPos( s16 pos )
{
  for( PaneList::iterator it = m_active_list.begin(); it != m_active_list.end(); ++it )
  {
    if( it->GetDataID() == static_cast<u32>(pos) )
    {
      return &(*it);
    }
  }
  return NULL;
}


/**
 * @brief 位置から座標を取得する
 *
 * @param pos 位置
 *
 * @return  座標
 */
ScrollPanePosition ScrollPaneManager::GetPositionByPos( s16 pos ) const
{
  GFL_ASSERT( this->IsOnGrid() ); //@checkプログラム的に見たいだけ
  
  ScrollPanePosition  target;
  target.SetX( m_desc.pane_start_x + this->GetCol( pos ) * m_desc.pane_width );
  target.SetY( m_desc.pane_start_y - this->GetRow( pos ) * m_desc.pane_height );
  
  return target;
}

/**
 * @brief 位置から行への変換(X)
 *
 * @param pos 位置
 *
 * @return 行
 */
s16 ScrollPaneManager::GetCol( s16 pos ) const
{
  return pos % m_desc.col;
}
/**
 * @brief 位置から列への変換(Y)
 *
 * @param pos 位置
 *
 * @return 列
 */
s16 ScrollPaneManager::GetRow( s16 pos ) const
{
  return pos / m_desc.col;
}
/**
 * @brief 行と列から位置への変換
 *
 * @param col 行
 * @param row 列
 *
 * @return 位置
 */
s16 ScrollPaneManager::GetPos( s16 col, s16 row ) const
{
  return col + row * m_desc.col;
}
/**
 * @brief グローバルカーソル位置を取得
 *
 * @param pos カーソル位置
 *
 * @return グローバルカーソルいち
 */
s16 ScrollPaneManager::GetGrobalPos( s16 pos ) const
{
  GFL_ASSERT( this->IsOnGrid() );//@checkプログラム的に見たいだけ
  return pos + (m_scroll / m_desc.pane_height) * m_desc.col;
}

/**
 * @brief ローカルカーソル位置を取得
 *
 * @param pos カーソル位置
 *
 * @return  ローカルカーソル位置
 */
s16 ScrollPaneManager::GetLocalPos( s16 pos ) const
{
  GFL_ASSERT( this->IsOnGrid() );//@checkプログラム的に見たいだけ
  return pos - (m_scroll / m_desc.pane_height)*m_desc.col;
}


//-----------------------------------------------------------------------
// PRIVATE 変換系
//-----------------------------------------------------------------------
/**
 * @brief ヒットしたペインを取得する
 *
 * @param x X座標
 * @param y Y座標
 * @param pane  受け取り
 *
 * @return trueだとヒットした falseだとしていない
 */
bool ScrollPaneManager::GetTriggerPane( s16 x, s16 y, u32 * pane )
{
  for( u32 i = 0; i < m_desc.pane_table_num; ++i )
  {
    PaneInfo* info = m_desc.pane_table;
    
    if( m_desc.layout_work->GetHitPane( x, y, info[i].bounding_pane ) )
    {
      *pane = i;
      return true;
    }
  }
  
  return false;
}

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

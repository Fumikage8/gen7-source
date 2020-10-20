//=============================================================================
/**
 * @file    AppToolNewPaneList.cpp
 * @brief   ペインリスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================

// module
#include "AppLib/include/Tool/AppToolNewPaneList.h"
#include "NewPaneListPartsH.h"
//#include "NewPaneListPartsV.h"
// niji
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/AppToolSlideScroll.h"
#include "AppLib/include/Tool/app_tool_ScrollValue.h"
#include "Sound/include/Sound.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//! メインシーケンス
enum
{
  MAINSEQ_WAIT = 0,
  MAINSEQ_DRAG,
  MAINSEQ_RELEASE,
  MAINSEQ_SLIDE_PLUS,
  MAINSEQ_SLIDE_MINUS,
  MAINSEQ_KEY_SCROLL_PLUS,
  MAINSEQ_KEY_SCROLL_MINUS,
  MAINSEQ_KEY_SKIP_PLUS,
  MAINSEQ_KEY_SKIP_MINUS,
  MAINSEQ_EX_SCROLL_PLUS,
  MAINSEQ_EX_SCROLL_MINUS,
  MAINSEQ_KEY_ITEM_CHANGE_PLUS,
  MAINSEQ_KEY_ITEM_CHANGE_MINUS,
  MAINSEQ_KEY_ITEM_CHANGE_SKIP_PLUS,
  MAINSEQ_KEY_ITEM_CHANGE_SKIP_MINUS,
};

//! デフォルトスクロールパラメータ
const NewPaneList::ScrollParam NewPaneList::DefaultScrollParam[NewPaneList::SCROLL_PARAM_MAX] =
{
  { 0.5f, 0.04f },
  { 0.6f, 0.03f },
  { 0.7f, 0.02f },
  { 0.8f, 0.01f },
};

//! キースクロールフレームテーブル
const u8 NewPaneList::KeyScrollFrame[NewPaneList::KEY_SCROLL_COUNT_MAX] = { 4, 3, 2, 1 };


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap      ヒープ
 * @param setup     セットアップパラメータ
 * @param listener  リスナー
 */
//-----------------------------------------------------------------------------
NewPaneList::NewPaneList( app::util::Heap * heap, const SETUP_PARAM * setup, ListListener * listener )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_param( *setup )
  , m_listListener( listener )
  , m_insertPane( NULL )
  , m_scrollMax( 0.0f )
  , m_plusCursorPos( 0 )
  , m_minusCursorPos( 0 )
  , m_touchAreaUp( 0 )
  , m_touchAreaDown( DISPLAY_LOWER_HEIGHT )
  , m_touchAreaLeft( 0 )
  , m_touchAreaRight( DISPLAY_LOWER_WIDTH )
  , m_isLoop( true )
  , m_isInputKey( true )
  , m_isInputTouch( true )
  , m_isDecide( true )
  , m_skipMode( SKIP_MODE_KEY )
  , m_effectMode( EFFECT_MODE_NONE )
  , m_pPaneLowTable( NULL )
  , m_pPartsBase( NULL )
  , m_pSlideScroll( NULL )
  , m_pScrollValue( NULL )
  , m_dragOffset( 0 )
  , m_dragStartScroll( 0.0f )
  , m_slideValue( 0.0f )
  , m_slideRest( 0.0f )
  , m_slideStop( false )
  , m_slideStopX( 0 )
  , m_slideStopY( 0 )
  , m_keyScrollValue( 0.0f )
  , m_keyScrollCount( 0 )
  , m_keyScrollFrame( 0 )
  , m_keyScrollMoveCount( 0 )
  , m_buttonTouchFlag( false )
  , m_isExScrollRepeat( false )
  , m_pScrollBar( NULL )
  , m_mainSeq( MAINSEQ_WAIT )
  , m_subSeq( 0 )
{
  for( u32 i=0; i<SCROLL_PARAM_MAX; i++ )
  {
    m_scrollParam[i].speed = 0.0f;
    m_scrollParam[i].decel = 0.0f;
  };
  SetupSE( SEQ_SE_DECIDE1, SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
NewPaneList::~NewPaneList()
{
  Terminate();
}

//-----------------------------------------------------------------------------
/**
 * @func  Initialize
 * @brief 初期化
 * @date  2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::Initialize(void)
{
  CreatePaneLowTable();

  SetScrollMax();
  SetCursorMoveParam();

  CreateButton();

  if( m_param.list_type == TYPE_H )
  {
    m_pPartsBase = GFL_NEW(m_heap->GetSystemHeap()) NewPaneListPartsH(
                                                      &m_param,
                                                      m_listListener,
                                                      app::ui::UIResponder::GetButtonManager(),
                                                      m_insertPane,
                                                      m_pPaneLowTable,
                                                      m_scrollMax,
                                                      m_effectMode );
    m_pSlideScroll = GFL_NEW(m_heap->GetSystemHeap()) SlideScroll( SlideScroll::MODE_H );
  }
  else
  {
#if 0 // @note 未実装
    m_pPartsBase = GFL_NEW(m_heap->GetSystemHeap()) NewPaneListPartsV(
                                                      &m_param,
                                                      m_listListener,
                                                      app::ui::UIResponder::GetButtonManager(),
                                                      m_insertPane,
                                                      m_pPaneLowTable,
                                                      m_scrollMax,
                                                      m_effectMode );
    m_pSlideScroll = GFL_NEW(m_heap->GetSystemHeap()) SlideScroll( SlideScroll::MODE_V );
#else
    GFL_ASSERT( 0 );
    m_pPartsBase = GFL_NEW(m_heap->GetSystemHeap()) NewPaneListPartsH(
                                                      &m_param,
                                                      m_listListener,
                                                      app::ui::UIResponder::GetButtonManager(),
                                                      m_insertPane,
                                                      m_pPaneLowTable,
                                                      m_scrollMax,
                                                      m_effectMode );
    m_pSlideScroll = GFL_NEW(m_heap->GetSystemHeap()) SlideScroll( SlideScroll::MODE_H );
#endif
  }
  m_pSlideScroll->SetTouchAreaDisp( m_touchAreaUp, m_touchAreaDown, m_touchAreaLeft, m_touchAreaRight );

  SetupScrollParam( DefaultScrollParam );

  m_pPartsBase->InitListPut();
  MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
  SetInputListener( this );

  m_pPartsBase->CreateChangeLowTable( m_heap );
}

//-----------------------------------------------------------------------------
/**
 * @func  Terminate
 * @brief 破棄
 * @date  2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::Terminate(void)
{
  DeleteScrollValue();  // @fix NMcat[3806]: 通信で強制終了された場合の対処
  GFL_SAFE_DELETE( m_pSlideScroll );
  GFL_SAFE_DELETE( m_pPartsBase );
  DeletePaneLowTable();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePaneLowTable
 * @brief   ペインの並び順テーブル生成
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::CreatePaneLowTable(void)
{
  m_pPaneLowTable = GFL_NEW_ARRAY(m_heap->GetSystemHeap()) u8[m_param.pane_data_max];
  for( u32 i=0; i<m_param.pane_data_max; i++ )
  {
    m_pPaneLowTable[i] = i;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePaneLowTable
 * @brief   ペインの並び順テーブル削除
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::DeletePaneLowTable(void)
{
  GFL_SAFE_DELETE_ARRAY( m_pPaneLowTable );
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupInsertPane
 * @brief ペインの表示優先変更の基準となるペインを設定
 * @date  2015.07.22
 *
 * @param pane  基準ペイン
 *
 * @note
 *  デフォルト：NULL
 *
 *  ペインが被らない場合は設定しなくてよい
 *  入れ替えを有効にする場合は設定すること
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupInsertPane( gfl2::lyt::LytPane * pane )
{
  m_insertPane = pane;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupTouchArea
 * @brief タッチ範囲を設定
 * @date  2015.07.22
 *
 * @param up    上Y座標
 * @param down  下Y座標
 * @param left  左X座要
 * @param right 右X座標
 *
 * @note
 *  デフォルト：画面全体
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupTouchArea( s16 up, s16 down, s16 left, s16 right )
{
  m_touchAreaUp    = up;
  m_touchAreaDown  = down;
  m_touchAreaLeft  = left;
  m_touchAreaRight = right;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsTouchArea
 * @brief   タッチ有効範囲チェック
 * @date    2015.07.28
 *
 * @param   x   X座標
 * @param   y   Y座標
 *
 * @retval  true  = 範囲内
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneList::IsTouchArea( u16 x, u16 y ) const
{
  if( x >= m_touchAreaLeft && x <= m_touchAreaRight && y >= m_touchAreaUp && y <= m_touchAreaDown )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupSE
 * @brief SEを設定
 * @date  2015.07.30
 *
 * @param decide  決定時のSE
 * @param move    カーソル移動時のSE
 *
 * @note
 *  デフォルト：
 *    decide = SEQ_SE_DECIDE1
 *      move = SEQ_SE_SELECT1
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupSE( u32 decide, u32 move )
{
  m_se[SE_ID_DECIDE] = decide;
  m_se[SE_ID_CURSOR_MOVE] = move;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupScrollParam
 * @brief スライド用のスクロールパラメータを設定
 * @date  2015.07.27
 *
 * @param param   スクロールパラメータ
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupScrollParam( const NewPaneList::ScrollParam * param )
{
  for( u32 i=0; i<SCROLL_PARAM_MAX; i++ )
  {
    m_scrollParam[i] = param[i];
  }
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupNoLoop
 * @brief リストのループ処理を無効化
 * @date  2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupNoLoop(void)
{
  m_isLoop = false;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupSkipMode
 * @brief リストのスキップ処理を設定
 * @date  2015.07.30
 *
 * @param mode  スキップモード
 *
 * @note
 *  デフォルト：十字キー
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupSkipMode( SkipMode mode )
{
  m_skipMode = mode;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupEffectMode
 * @brief エフェクトモードを設定
 * @date  2015.07.31
 *
 * @param mode  エフェクトモード
 *
 * @note
 *  デフォルト：エフェクトなし
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupEffectMode( EffectMode mode )
{
  m_effectMode = mode;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetupInputMode
 * @brief 入力処理の有効/無効切り替え
 * @date  2015.07.30
 *
 * @param is_key      キー
 * @param is_touch    タッチ
 * @param is_decide   決定
 *
 * @note
 *  デフォルト：全て有効
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetupInputMode( bool is_key, bool is_touch, bool is_decide )
{
  m_isInputKey   = is_key;
  m_isInputTouch = is_touch;
  m_isDecide     = is_decide;
}

//-----------------------------------------------------------------------------
/**
 * @func  IsModuleAction
 * @brief ペインリストが動作中か
 * @date  2015.07.29
 *
 * @retval  true  = 動作中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneList::IsModuleAction(void)
{
  if( m_mainSeq == MAINSEQ_WAIT )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursorPos
 * @brief   画面上のカーソル位置を取得
 * @date    2015.07.30
 *
 * @return  カーソル位置
 */
//-----------------------------------------------------------------------------
u32 NewPaneList::GetCursorPos(void)
{
  return m_param.list_pos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetScroll
 * @brief   スクロール値を取得
 * @date    2015.07.30
 *
 * @return  スクロール値
 */
//-----------------------------------------------------------------------------
f32 NewPaneList::GetScroll(void)
{
  return m_param.list_scroll;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListPos
 * @brief   リスト上のカーソル位置を取得
 * @date    2015.07.30
 *
 * @return  カーソル位置 ( GetCursorPos()+GetScroll() )
 */
//-----------------------------------------------------------------------------
u32 NewPaneList::GetListPos(void)
{
  return ( m_param.list_pos + static_cast<u32>(m_param.list_scroll) );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetScrollMax
 * @brief   スクロール最大値を設定
 * @date    2015.07.27
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetScrollMax(void)
{
  m_scrollMax = static_cast<f32>( m_param.list_max - m_param.list_pos_max - 1 );
  if( m_scrollMax < 0.0f )
  {
    m_scrollMax = 0.0f;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCursorMoveParam
 * @brief   リストがスクロールするカーソル位置を設定
 * @date    2015.07.29
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetCursorMoveParam(void)
{
  m_minusCursorPos = m_param.list_pos_max / 2;
  m_plusCursorPos  = m_minusCursorPos + ( m_param.list_pos_max & 1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.07.24
 */
//-----------------------------------------------------------------------------
void NewPaneList::CreateButton(void)
{
  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[m_param.pane_data_max];

  for( u32 i=0; i<m_param.pane_data_max; i++ )
  {
    info[i].button_id              = i;
    info[i].picture_pane           = m_param.pane_data[i].base;
    info[i].bound_pane             = m_param.pane_data[i].bound;
    info[i].touch_anime_index      = m_param.pane_data[i].anm_touch;
    info[i].release_anime_index    = m_param.pane_data[i].anm_release;
    info[i].cancel_anime_index     = m_param.pane_data[i].anm_cancel;
    info[i].key_select_anime_index = m_param.pane_data[i].anm_select;
    info[i].active_anime_index     = app::tool::ButtonManager::ANIMATION_NULL;
    info[i].passive_anime_index    = app::tool::ButtonManager::ANIMATION_NULL;
  }

  app::ui::UIResponder::CreateButtonManager( m_heap, m_param.lytwk, info, m_param.pane_data_max );

  GFL_DELETE_ARRAY info;

  if( m_isDecide != false )
  {
    app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
    for( u32 i=0; i<m_param.pane_data_max; i++ )
    {
      man->SetButtonSelectSE( i, m_se[SE_ID_DECIDE] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateScrollValue
 * @brief   スクロール値管理クラス生成
 * @date    2015.07.27
 *
 * @param   level   速度レベル
 * @param   mv      スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::CreateScrollValue( u32 level, s32 mv )
{
  app::tool::ScrollValue::Settings  dat;
  f32 speed;

  dat.init_value = m_param.list_scroll; // 初期値[value]
  dat.min_value  = 0.0f;                // 最小値[value]
  dat.max_value  = m_scrollMax;         // 最大値[value]

  if( level >= SCROLL_PARAM_MAX )
  {
    // 減速度[value/frame*frame]
    dat.default_decel = m_scrollParam[SCROLL_PARAM_MAX-1].decel;
    // 初速度
    speed = m_scrollParam[SCROLL_PARAM_MAX-1].speed;
  }
  else
  {
    // 減速度[value/frame*frame]
    dat.default_decel = m_scrollParam[level].decel;
    // 初速度
    speed = m_scrollParam[level].speed;
  }

  dat.overrun_enable = false;   // 限界値突破を許すかどうか
  dat.overrun_frame  = 0;       // 限界値突破時の強制スクロールのフレーム数

  m_pScrollValue = GFL_NEW_LOW(m_heap->GetSystemHeap()) app::tool::ScrollValue( dat );

  if( mv < 0 )
  {
    m_pScrollValue->StartScroll( -speed, 1.0f );
  }
  else
  {
    m_pScrollValue->StartScroll( speed, 1.0f );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteScrollValue
 * @brief   スクロール値管理クラス削除
 * @date    2015.07.27
 */
//-----------------------------------------------------------------------------
void NewPaneList::DeleteScrollValue(void)
{
  GFL_SAFE_DELETE( m_pScrollValue );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartSlide
 * @brief   スライド動作開始
 * @date    2015.07.28
 *
 * @param   mv  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::StartSlide( s32 mv )
{
  m_param.list_scroll = m_pPartsBase->InitListPutBase( 0 );
  SetDragPos();
  CreateScrollValue( m_pSlideScroll->GetSlideLevel(), mv );
  m_slideValue = m_param.list_scroll;
  m_slideRest  = 0.0f;
  m_slideStop  = false;

  m_pPartsBase->SetVisibleCursor( false );
}


//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.07.24
 *
 * @param   displayNo   描画ディスプレイ
 */
//-----------------------------------------------------------------------------
void NewPaneList::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.07.24
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update(void)
{
  app::ui::UIView::Update();

  switch( m_mainSeq )
  {
  case MAINSEQ_WAIT:
    break;

  case MAINSEQ_DRAG:
    Update_Drag();
    break;

  case MAINSEQ_RELEASE:
    Update_Release();
    break;

  case MAINSEQ_SLIDE_PLUS:
    Update_Slide( 1 );
    break;
  case MAINSEQ_SLIDE_MINUS:
    Update_Slide( -1 );
    break;

  case MAINSEQ_KEY_SCROLL_PLUS:
    Update_KeyScrollPlus();
    break;
  case MAINSEQ_KEY_SCROLL_MINUS:
    Update_KeyScrollMinus();
    break;

  case MAINSEQ_KEY_SKIP_PLUS:
    Update_KeySkip( 1 );
    break;
  case MAINSEQ_KEY_SKIP_MINUS:
    Update_KeySkip( -1 );
    break;

  case MAINSEQ_EX_SCROLL_PLUS:
    Update_ExScrollPlus();
    break;
  case MAINSEQ_EX_SCROLL_MINUS:
    Update_ExScrollMinus();
    break;

  case MAINSEQ_KEY_ITEM_CHANGE_PLUS:
    Update_KeyItemChangePlus();
    break;
  case MAINSEQ_KEY_ITEM_CHANGE_MINUS:
    Update_KeyItemChangeMinus();
    break;
  case MAINSEQ_KEY_ITEM_CHANGE_SKIP_PLUS:
    Update_KeyItemChangeSkip( 1 );
    break;
  case MAINSEQ_KEY_ITEM_CHANGE_SKIP_MINUS:
    Update_KeyItemChangeSkip( -1 );
    break;
  }

  UpdateScrollBar();
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_Drag
 * @brief   アップデート：ドラッグ処理
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_Drag(void)
{
  u32 tmp  = static_cast<u32>( m_param.list_scroll );
  s32 offs = m_pSlideScroll->GetDragOffset();
  m_param.list_scroll = m_pPartsBase->MoveList( offs-m_dragOffset );
  m_dragOffset = offs;
  if( tmp != static_cast<u32>(m_param.list_scroll) )
  {
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_Release
 * @brief   アップデート：リリース処理（タッチリリース）
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_Release(void)
{
  u32 tmp = static_cast<u32>( m_param.list_scroll );
//  GFL_PRINT( "★RELEASE[1] %f\n", m_param.list_scroll );
  m_param.list_scroll = m_pPartsBase->InitListPutBase( 0 );
  m_pPartsBase->ChangeButtonEnable();
//  GFL_PRINT( "★RELEASE[2] %f\n", m_param.list_scroll );
  if( SetDragPos() != false )
  {
    MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
  }
  else
  {
    // 位置が変わらない場合はリスナーを呼ばない
    m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
  }
  if( tmp != static_cast<u32>(m_param.list_scroll) )
  {
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
  }
  app::ui::UIResponder::GetButtonManager()->SetInputEnable( true );
  ChangeUpdate( MAINSEQ_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_Slide
 * @brief   アップデート：スライド処理
 * @date    2015.07.29
 *
 * @param   mv  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_Slide( s32 mv )
{
  // @fix NMcat[3806]: 通信で強制終了された場合の対処
  if( m_pScrollValue == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( m_slideStop != false && m_pScrollValue->HaveStopRequest() == false )
  {
    m_pScrollValue->StopRequest( 1 );
  }

  f32 tmp = m_pScrollValue->GetCurrentValue();
  m_pScrollValue->UpdateScroll();
  f32 val = m_pScrollValue->GetCurrentValue();
  if( val == tmp )
  {
    DeleteScrollValue();
    m_param.list_scroll = m_pPartsBase->InitListPutBase( mv );
    m_pPartsBase->ChangeButtonEnable();
    if( m_slideStop != false )
    {
      for( u32 i=0; i<m_param.pane_data_max; i++ )
      {
        if( m_param.pane_data[i].base->IsVisible() != false &&
            m_param.lytwk->GetHitPane( m_slideStopX, m_slideStopY, m_param.pane_data[i].bound ) != false )
        {
          u32 list_pos = m_pPartsBase->GetPosPaneLow( i );
          if( list_pos <= m_param.list_pos_max )
          {
            m_param.list_pos = list_pos;
          }
          break;
        }
      }
      m_pSlideScroll->SetExTouchFlag();
    }
    MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
    app::ui::UIResponder::GetButtonManager()->SetInputEnable( true );
    ChangeUpdate( MAINSEQ_WAIT );
  }
  else
  {
    f32 move = ( m_slideValue - val ) * m_param.space + m_slideRest;
    if( gfl2::math::FAbs(move) >= 1.0f )
    {
      u32 tmp_scroll = static_cast<u32>( m_param.list_scroll );
      m_param.list_scroll = m_pPartsBase->MoveList( static_cast<s32>(move) );
      m_slideValue = val;
      m_slideRest  = move - static_cast<f32>( static_cast<s32>(move) );
      if( tmp_scroll != static_cast<u32>(m_param.list_scroll) )
      {
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeyScrollPlus
 * @brief   アップデート：キー入力でのスクロール処理（＋方向）
 * @date    2015.07.29
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeyScrollPlus(void)
{
  switch( m_subSeq )
  {
  case 0:
    // スクロールしない
    if( m_scrollMax == 0 )
    {
      u32 list_pos = m_param.list_pos + 1;
      if( list_pos >= m_param.list_max )
      {
        list_pos = 0;
      }
      if( list_pos == m_param.list_pos )
      {
        EndKeyScroll();
        break;
      }
      MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
      m_subSeq = 2;
    }
    // カーソル位置がスクロール可能位置
    else if( m_param.list_pos >= m_plusCursorPos )
    {
      if( m_param.list_scroll == m_scrollMax )
      {
        if( m_param.list_pos < m_param.list_pos_max )
        {
          u32 list_pos = m_param.list_pos + 1;
          MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
          Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
          m_subSeq = 2;
        }
        else
        {
          KeyScrollLoop( 0, 0.0f );
          EndKeyScroll();
        }
        break;
      }
      else
      {
        m_keyScrollValue = 0.0f;
        m_keyScrollMoveCount = 0;
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 1;
      }
    }
    // カーソル位置がスクロール不可能位置
    else
    {
      if( m_param.list_pos < m_param.list_pos_max )
      {
        u32 list_pos = m_param.list_pos + 1;
        MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 2;
      }
      else
      {
        EndKeyScroll();
      }
      break;
    }
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_param.list_scroll = m_pPartsBase->MoveList( -static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      m_param.list_scroll = m_pPartsBase->InitListPutBase( 1 );
      CountupKeyScroll();
      MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
      m_subSeq = 2;
    }
    break;

  case 2:
    if( m_pPartsBase->IsKeyHold(NewPaneListPartsBase::INPUT_PLUS) == false )
    {
      EndKeyScroll();
    }
    else
    {
      if( m_pPartsBase->CheckKeyInput() == NewPaneListPartsBase::INPUT_PLUS )
      {
        m_subSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeyScrollMinus
 * @brief   アップデート：キー入力でのスクロール処理（－方向）
 * @date    2015.07.29
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeyScrollMinus(void)
{
  switch( m_subSeq )
  {
  case 0:
    // スクロールしない
    if( m_scrollMax == 0 )
    {
      s32 list_pos = static_cast<s32>(m_param.list_pos) - 1;
      if( list_pos < 0 )
      {
        list_pos = m_param.list_max - 1;
      }
      if( list_pos == m_param.list_pos )
      {
        EndKeyScroll();
        break;
      }
      MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
      m_subSeq = 2;
    }
    // カーソル位置がスクロール可能位置
    else if( m_param.list_pos <= m_minusCursorPos )
    {
      if( m_param.list_scroll == 0 )
      {
        if( m_param.list_pos != 0 )
        {
          u32 list_pos = m_param.list_pos - 1;
          MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
          Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
          m_subSeq = 2;
        }
        else
        {
          KeyScrollLoop( m_param.list_pos_max, m_scrollMax );
          EndKeyScroll();
        }
        break;
      }
      else
      {
        m_keyScrollValue = 0.0f;
        m_keyScrollMoveCount = 0;
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 1;
      }
    }
    // カーソル位置がスクロール不可能位置
    else
    {
      if( m_param.list_pos != 0 )
      {
        u32 list_pos = m_param.list_pos - 1;
        MoveCursor( list_pos, m_pPartsBase->GetPosPaneIndex(list_pos), true );
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 2;
      }
      else
      {
        EndKeyScroll();
      }
      break;
    }
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_param.list_scroll = m_pPartsBase->MoveList( static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      m_param.list_scroll = m_pPartsBase->InitListPutBase( -1 );
      CountupKeyScroll();
      MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
      m_subSeq = 2;
    }
    break;

  case 2:
    if( m_pPartsBase->IsKeyHold(NewPaneListPartsBase::INPUT_MINUS) == false )
    {
      EndKeyScroll();
    }
    else
    {
      if( m_pPartsBase->CheckKeyInput() == NewPaneListPartsBase::INPUT_MINUS )
      {
        m_subSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeySkip
 * @brief   アップデート：キー入力でのページスキップ処理
 * @date    2015.07.30
 *
 * @param   mv  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeySkip( s32 mv )
{
  s32 offs = 0;
  u32 pos  = m_param.list_pos;
  s32 max  = gfl2::math::Abs( ( m_param.list_pos_max + 1 ) * static_cast<s32>(m_param.space) );

  if( mv > 0 )
  {
    if( m_scrollMax == 0 )
    {
      if( m_param.list_max == 0 )
      {
        pos = 0;
      }
      else
      {
        pos = m_param.list_max - 1;
      }
    }
    else if( m_param.list_scroll < m_scrollMax )
    {
      offs = -static_cast<s32>( ( m_scrollMax - m_param.list_scroll ) * m_param.space );
      if( gfl2::math::Abs(offs) > max )
      {
        offs = -max;
      }
    }
    else if( pos != m_param.list_pos_max )
    {
      pos = m_param.list_pos_max;
    }
  }
  else
  {
    if( m_param.list_scroll > 0.0f )
    {
      offs = static_cast<s32>( m_scrollMax * m_param.space );
      if( gfl2::math::Abs(offs) > max )
      {
        offs = max;
      }
    }
    else if( pos != 0 )
    {
      pos = 0;
    }
  }

  {
    bool is_move = false;
    if( offs != 0 )
    {
      m_param.list_scroll = m_pPartsBase->MoveList( offs );
      MoveCursor( pos, m_pPartsBase->GetPosPaneIndex(pos), true );
      is_move = true;
    }
    else
    {
      if( pos != m_param.list_pos )
      {
        MoveCursor( pos, m_pPartsBase->GetPosPaneIndex(pos), true );
        is_move = true;
      }
    }
    if( is_move != false )
    {
      m_pPartsBase->ChangeButtonEnable();
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
    }
  }

  ChangeUpdate( MAINSEQ_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_ExScrollPlus
 * @brief   アップデート：外部スクロール処理（＋方向）
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_ExScrollPlus(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_param.list_scroll == m_scrollMax )
    {
      EndKeyScroll();
      break;
    }
    m_keyScrollValue = 0.0f;
    m_keyScrollMoveCount = 0;
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
    m_subSeq = 1;
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_param.list_scroll = m_pPartsBase->MoveList( -static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      m_param.list_scroll = m_pPartsBase->InitListPutBase( 1 );
      CountupKeyScroll();
      MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
      if( m_isExScrollRepeat == false )
      {
        EndKeyScroll();
      }
      m_isExScrollRepeat = false;
      m_subSeq = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_ExScrollMinus
 * @brief   アップデート：外部スクロール処理（－方向）
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_ExScrollMinus(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_param.list_scroll == 0 )
    {
      EndKeyScroll();
      break;
    }
    m_keyScrollValue = 0.0f;
    m_keyScrollMoveCount = 0;
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
    m_subSeq = 1;
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_param.list_scroll = m_pPartsBase->MoveList( static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      m_param.list_scroll = m_pPartsBase->InitListPutBase( -1 );
      CountupKeyScroll();
      MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
      if( m_isExScrollRepeat == false )
      {
        EndKeyScroll();
      }
      m_isExScrollRepeat = false;
      m_subSeq = 0;
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeyItemChangePlus
 * @brief   アップデート：キー入力での入れ替え処理（＋方向）
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeyItemChangePlus(void)
{
  switch( m_subSeq )
  {
  case 0:
    // スクロールしない
    if( m_scrollMax == 0 )
    {
      // ペインの入れ替え
      u32 list_pos = m_param.list_pos + 1;
      if( list_pos >= m_param.list_max )
      {
        list_pos = 0;
      }
      if( list_pos == m_param.list_pos )
      {
        EndKeyScroll();
        break;
      }
      m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
      m_param.list_pos = list_pos;
      m_pPartsBase->SwapItemChangePane( 1 );
      m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
      m_subSeq = 2;
    }
    // カーソル位置がスクロール可能位置
    else if( m_param.list_pos >= m_plusCursorPos )
    {
      if( m_param.list_scroll >= m_scrollMax )
      {
        if( m_param.list_pos < m_param.list_pos_max )
        {
          // ペインの入れ替え
          u32 list_pos = m_param.list_pos + 1;
          m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
          m_param.list_pos = list_pos;
          m_pPartsBase->SwapItemChangePane( 1 );
          m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
          Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
          m_subSeq = 2;
        }
        else
        {
          // ループ
          KeyScrollLoopChange( 0, 0.0f, 1 );
          EndKeyScroll();
        }
        break;
      }
      else
      {
        // スクロール
        m_pPartsBase->StartItemChangeScroll( 1 );
        m_keyScrollValue = 0.0f;
        m_keyScrollMoveCount = 0;
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 1;
      }
    }
    // カーソル位置がスクロール不可能位置
    else
    {
      if( m_param.list_pos < m_param.list_pos_max )
      {
        // ペインの入れ替え
        u32 list_pos = m_param.list_pos + 1;
        m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
        m_param.list_pos = list_pos;
        m_pPartsBase->SwapItemChangePane( 1 );
        m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 2;
      }
      else
      {
        EndKeyScroll();
      }
      break;
    }
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_pPartsBase->MoveChangeList( -static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      CountupKeyScroll();
      m_param.list_scroll++;
      m_pPartsBase->AdjustChangeList();
      m_pPartsBase->EndItemChangeScroll();
//      MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
      m_subSeq = 2;
    }
    break;

  case 2:
    if( m_pPartsBase->IsKeyHold(NewPaneListPartsBase::INPUT_PLUS) == false )
    {
      EndKeyScroll();
    }
    else
    {
      if( m_pPartsBase->CheckKeyInput() == NewPaneListPartsBase::INPUT_PLUS )
      {
        m_subSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeyItemChangeMinus
 * @brief   アップデート：キー入力での入れ替え処理（－方向）
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeyItemChangeMinus(void)
{
  switch( m_subSeq )
  {
  case 0:
    // スクロールしない
    if( m_scrollMax == 0 )
    {
      // ペインの入れ替え
      s32 list_pos = static_cast<s32>(m_param.list_pos) - 1;
      if( list_pos < 0 )
      {
        list_pos = m_param.list_max - 1;
      }
      if( list_pos == m_param.list_pos )
      {
        EndKeyScroll();
        break;
      }
      m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
      m_param.list_pos = list_pos;
      m_pPartsBase->SwapItemChangePane( -1 );
      m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
      m_subSeq = 2;
    }
    // カーソル位置がスクロール可能位置
    else if( m_param.list_pos <= m_minusCursorPos )
    {
      if( m_param.list_scroll <= 0 )
      {
        if( m_param.list_pos != 0 )
        {
          // ペインの入れ替え
          u32 list_pos = m_param.list_pos - 1;
          m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
          m_param.list_pos = list_pos;
          m_pPartsBase->SwapItemChangePane( -1 );
          m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
          Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
          m_subSeq = 2;
        }
        else
        {
          // ループ
          KeyScrollLoopChange( m_param.list_pos_max, m_scrollMax, -1 );
          EndKeyScroll();
        }
        break;
      }
      else
      {
        // スクロール
        m_pPartsBase->StartItemChangeScroll( -1 );
        m_keyScrollValue = 0.0f;
        m_keyScrollMoveCount = 0;
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 1;
      }
    }
    // カーソル位置がスクロール不可能位置
    else
    {
      if( m_param.list_pos != 0 )
      {
        // ペインの入れ替え
        u32 list_pos = m_param.list_pos - 1;
        m_pPartsBase->SwapChangeLowTable( m_param.list_pos, list_pos );
        m_param.list_pos = list_pos;
        m_pPartsBase->SwapItemChangePane( -1 );
        m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(list_pos), true );
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        m_subSeq = 2;
      }
      else
      {
        EndKeyScroll();
      }
      break;
    }
    /* FALL THROUGH */
  case 1:
    m_keyScrollValue += ( m_param.space / static_cast<f32>( KeyScrollFrame[m_keyScrollCount] ) );
    m_pPartsBase->MoveChangeList( static_cast<s32>(m_keyScrollValue) );
    UpdateKeyScrollValue();
    m_keyScrollMoveCount++;
    if( m_keyScrollMoveCount == KeyScrollFrame[m_keyScrollCount] )
    {
      CountupKeyScroll();
      m_param.list_scroll--;
      m_pPartsBase->AdjustChangeList();
      m_pPartsBase->EndItemChangeScroll();
      m_subSeq = 2;
    }
    break;

  case 2:
    if( m_pPartsBase->IsKeyHold(NewPaneListPartsBase::INPUT_MINUS) == false )
    {
      EndKeyScroll();
    }
    else
    {
      if( m_pPartsBase->CheckKeyInput() == NewPaneListPartsBase::INPUT_MINUS )
      {
        m_subSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_KeyItemChangeSkip
 * @brief   アップデート：キー入力での入れ替えページスキップ処理
 * @date    2015.08.03
 *
 * @param   mv  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::Update_KeyItemChangeSkip( s32 mv )
{
  f32 scroll = m_param.list_scroll;
  u32 pos = m_param.list_pos;

  if( mv > 0 )
  {
    if( m_scrollMax == 0 )
    {
      if( m_param.list_max == 0 )
      {
        pos = 0;
      }
      else
      {
        pos = m_param.list_max - 1;
      }
    }
    else if( scroll < m_scrollMax )
    {
//      scroll += static_cast<f32>( m_param.list_pos_max + 1 );
      scroll += static_cast<f32>( m_param.list_pos_max );
      if( scroll > m_scrollMax )
      {
        scroll = m_scrollMax;
      }
    }
    else if( pos != m_param.list_pos_max )
    {
      pos = m_param.list_pos_max;
    }
  }
  else
  {
    if( scroll > 0.0f )
    {
//      scroll -= static_cast<f32>( m_param.list_pos_max + 1 );
      scroll -= static_cast<f32>( m_param.list_pos_max );
      if( scroll < 0.0f )
      {
        scroll = 0.0f;
      }
    }
    else if( pos != 0 )
    {
      pos = 0;
    }
  }

  {
    bool is_move = false;
    if( scroll != m_param.list_scroll )
    {
      m_pPartsBase->SkipChangeLowTable( static_cast<s32>(scroll-m_param.list_scroll), mv );
      m_pPartsBase->InitListPutItemChangeSkip();
      m_param.list_scroll = scroll;
      is_move = true;
    }
    else
    {
      if( pos != m_param.list_pos )
      {
        m_pPartsBase->SwapChangeLowTable( m_param.list_pos, pos );
        m_pPartsBase->SwapItemChangePane( m_param.list_pos, pos );
        m_param.list_pos = pos;
        m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
        is_move = true;
      }
    }
    if( is_move != false )
    {
      m_pPartsBase->ChangeButtonEnable();
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
    }
  }

  ChangeUpdate( MAINSEQ_WAIT );

/*
  s32 offs = 0;
  u32 pos  = m_param.list_pos;
  s32 max  = gfl2::math::Abs( ( m_param.list_pos_max + 1 ) * static_cast<s32>(m_param.space) );

  if( mv > 0 )
  {
    if( m_scrollMax == 0 )
    {
      if( m_param.list_max == 0 )
      {
        pos = 0;
      }
      else
      {
        pos = m_param.list_max - 1;
      }
    }
    else if( m_param.list_scroll < m_scrollMax )
    {
      offs = -static_cast<s32>( ( m_scrollMax - m_param.list_scroll ) * m_param.space );
      if( gfl2::math::Abs(offs) > max )
      {
        offs = -max;
      }
    }
    else if( pos != m_param.list_pos_max )
    {
      pos = m_param.list_pos_max;
    }
  }
  else
  {
    if( m_param.list_scroll > 0.0f )
    {
      offs = static_cast<s32>( m_scrollMax * m_param.space );
      if( gfl2::math::Abs(offs) > max )
      {
        offs = max;
      }
    }
    else if( pos != 0 )
    {
      pos = 0;
    }
  }

  {
    bool is_move = false;
    if( offs != 0 )
    {
      m_param.list_scroll = m_pPartsBase->MoveList( offs );
      MoveCursor( pos, m_pPartsBase->GetPosPaneIndex(pos), true );
      is_move = true;
    }
    else
    {
      if( pos != m_param.list_pos )
      {
        MoveCursor( pos, m_pPartsBase->GetPosPaneIndex(pos), true );
        is_move = true;
      }
    }
    if( is_move != false )
    {
      m_pPartsBase->ChangeButtonEnable();
      Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
    }
  }

  ChangeUpdate( MAINSEQ_WAIT );
*/
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangeUpdate
 * @brief   アップデートシーケンス切り替え
 * @date    2015.07.29
 *
 * @param   main  メインシーケンス
 * @param   sub   サブシーケンス
 */
//-----------------------------------------------------------------------------
void NewPaneList::ChangeUpdate( u32 main, u32 sub )
{
  m_mainSeq = main;
  m_subSeq  = sub;
}

//-----------------------------------------------------------------------------
/**
 * @func    CountupKeyScroll
 * @brief   キー入力スクロールのカウントアップ処理
 * @date    2015.07.29
 *
 * @retval  true  = カウントアップ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneList::CountupKeyScroll(void)
{
  m_keyScrollFrame++;
//  if( m_keyScrollFrame >= KeyScrollFrame[m_keyScrollCount] )
  if( m_keyScrollFrame >= KEY_SCROLL_COUNT_MAX )
  {
    if( m_keyScrollCount < ( KEY_SCROLL_COUNT_MAX - 1 ) )
    {
      m_keyScrollCount++;
    }
    m_keyScrollFrame = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndKeyScroll
 * @brief   キー入力スクロール終了
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneList::EndKeyScroll(void)
{
  m_pPartsBase->SetDeviceKey( NULL );
  m_pPartsBase->ChangeButtonEnable();
  app::ui::UIResponder::GetButtonManager()->SetInputEnable( true );
  ChangeUpdate( MAINSEQ_WAIT );
}

//-----------------------------------------------------------------------------
/**
 * @func    KeyScrollLoop
 * @brief   キー入力のループ処理
 * @date    2015.07.30
 *
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 */
//-----------------------------------------------------------------------------
void NewPaneList::KeyScrollLoop( u32 pos, f32 scroll )
{
  if( m_isLoop != false )
  {
    m_param.list_pos    = pos;
    m_param.list_scroll = scroll;
    m_pPartsBase->InitListPut();
    MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2015.07.29
 *
 * @param   pos           カーソル位置
 * @param   pane_index    配置先のペインインデックス
 * @param   is_cursor_on  true = カーソル表示
 */
//-----------------------------------------------------------------------------
void NewPaneList::MoveCursor( u32 pos, u32 pane_index, bool is_cursor_on )
{
  m_param.list_pos = pos;

  m_listListener->PaneListListener_MoveCursor(
    m_param.pane_data, pane_index, m_param.list_pos+static_cast<s32>(m_param.list_scroll) );

  m_pPartsBase->PutCursor( pane_index, is_cursor_on );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDragPos
 * @brief   ドラッグ位置を設定
 * @date    2015.07.29
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool NewPaneList::SetDragPos(void)
{
  s32 start_list_pos = m_param.list_pos + static_cast<s32>( m_dragStartScroll );
  s32 pos = start_list_pos - static_cast<s32>( m_param.list_scroll );
  if( pos >= 0 && pos <= static_cast<s32>(m_param.list_pos_max) )
  {
    m_param.list_pos = pos;
  }

  if( start_list_pos == ( static_cast<s32>(m_param.list_pos)+static_cast<s32>(m_param.list_scroll) ) )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func  SetExScroll
 * @brief 外部スクロール開始リクエスト
 * @date  2015.07.31
 *
 * @param mode  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetExScroll( ScrollMode mode )
{
  if( m_mainSeq == MAINSEQ_WAIT )
  {
    app::ui::UIResponder::GetButtonManager()->SetInputEnable( false );
    m_keyScrollCount = 0;
    m_keyScrollFrame = 0;
    if( mode == SCROLL_MODE_PLUS )
    {
      ChangeUpdate( MAINSEQ_EX_SCROLL_PLUS );
    }
    else
    {
      ChangeUpdate( MAINSEQ_EX_SCROLL_MINUS );
    }
  }
  else if( m_mainSeq == MAINSEQ_EX_SCROLL_PLUS || m_mainSeq == MAINSEQ_EX_SCROLL_MINUS )
  {
    m_isExScrollRepeat = true;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func  StopExScroll
 * @brief 外部スクロール停止リクエスト
 * @date  2015.07.31
 *
 * @note  SetExScroll()のリクエストを無効化
 */
//-----------------------------------------------------------------------------
void NewPaneList::StopExScroll(void)
{
  m_isExScrollRepeat = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    KeyScrollLoopChange
 * @brief   入れ替え時のキー入力のループ処理
 * @date    2015.08.01
 *
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 * @param   mv      スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneList::KeyScrollLoopChange( u32 pos, f32 scroll, s32 mv )
{
  if( m_isLoop != false )
  {
    m_param.list_pos    = pos;
    m_param.list_scroll = scroll;
//    m_pPartsBase->InitListPut();
//    MoveCursor( m_param.list_pos, m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
    m_pPartsBase->InitListPutItemChangeLoop();
    m_pPartsBase->PutCursor( m_pPartsBase->GetPosPaneIndex(m_param.list_pos), true );
    m_pPartsBase->LoopChangeLowTable( mv );
    Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func  SetItemChange
 * @brief いれかえ設定
 * @date  2015.08.04
 *
 * @param param   設定パラメータ
 */
//-----------------------------------------------------------------------------
void NewPaneList::SetItemChange( ItemChangeParam param )
{
  switch( param )
  {
  case ITEM_CHANGE_START:
  case ITEM_CHANGE_DECIDE:
    m_pPartsBase->SetItemChange();
    break;
  case ITEM_CHANGE_CANCEL:
    if( m_pPartsBase->IsItemChange() == false )
    {
      GFL_ASSERT( 0 );
      break;
    }
    m_pPartsBase->CancelItemChange();
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func  IsItemChange
 * @brief いれかえモードか
 * @date  2015.08.04
 *
 * @retval  true  = いれかえモード
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneList::IsItemChange(void)
{
  return m_pPartsBase->IsItemChange();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateScrollBar
 * @brief   スクロールバーを生成
 * @date    2016.02.02
 *
 * @param   setup   セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void NewPaneList::CreateScrollBar( const app::util::ScrollBar::SETUP_PARAM * setup )
{
  if( m_pScrollBar != NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_pScrollBar = GFL_NEW(m_heap->GetSystemHeap()) app::util::ScrollBar( setup );
  m_pScrollBar->SetRequest( app::util::ScrollBar::REQ_BAR_ON );
  ResetScrollBar( setup->max );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteScrollBar
 * @brief   スクロールバーを削除
 * @date    2016.02.02
 */
//-----------------------------------------------------------------------------
void NewPaneList::DeleteScrollBar(void)
{
  GFL_SAFE_DELETE( m_pScrollBar );
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetScrollBar
 * @brief   スクロールバーを再設定
 * @date    2016.02.02
 *
 * @param   max   最大スクロール値
 */
//-----------------------------------------------------------------------------
void NewPaneList::ResetScrollBar( u32 max )
{
  if( m_pScrollBar != NULL )
  {
    m_pScrollBar->ChangeParam( max );
    if( max == 0 )
    {
      m_pScrollBar->SetDrawEnable( false );
    }
    else
    {
      m_pScrollBar->SetDrawEnable( true );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateScrollBar
 * @brief   スクロールバーの更新処理
 * @date    2016.02.02
 */
//-----------------------------------------------------------------------------
void NewPaneList::UpdateScrollBar(void)
{
  if( m_pScrollBar == NULL )
  {
    return;
  }
  if( m_pScrollBar->IsVisible() == false )
  {
    return;
  }
  m_pScrollBar->Draw( GetScroll() );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateKeyScrollValue
 * @brief   キースクロール用のワーク更新
 * @date    2016.02.06
 */
//-----------------------------------------------------------------------------
void NewPaneList::UpdateKeyScrollValue(void)
{
  f32 f = m_keyScrollValue - static_cast<f32>( static_cast<s32>(m_keyScrollValue) );
  if( f < 0.5f )
  {
    m_keyScrollValue = 1.0f;
  }
  else
  {
    m_keyScrollValue = f;
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.07.22
 *
 * @param   touch_panel タッチパネル
 * @param   is_touch    true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult NewPaneList::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  // タッチ入力無効
  if( m_isInputTouch == false )
  {
    return ENABLE_ACCESS;
  }

  if( m_buttonTouchFlag == true && is_touch == false )
  {
    m_pPartsBase->SetVisibleCursor( true );
    m_buttonTouchFlag = false;
  }

  if( m_mainSeq == MAINSEQ_SLIDE_PLUS || m_mainSeq == MAINSEQ_SLIDE_MINUS )
  {
    if( touch_panel->IsTouchTrigger() != false )
    {
      touch_panel->GetXY( &m_slideStopX, &m_slideStopY );
      if( IsTouchArea( m_slideStopX, m_slideStopY ) != false )
      {
        m_slideStop = true;
      }
      return MYSUBVIEW_ONLY_ACCESS;
    }
  }
  else if( m_mainSeq == MAINSEQ_WAIT || m_mainSeq == MAINSEQ_DRAG )
  {
    // スクロール可能な場合のみ
    if( m_scrollMax != 0 )
    {
      SlideScroll::Result result = m_pSlideScroll->UpdateSlide( touch_panel, is_touch );
      if( result == SlideScroll::RESULT_DRAG_START )
      {
        m_pPartsBase->SetVisibleCursor( false );
        m_buttonTouchFlag = false;

        app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();
        man->ResetHoldState();
        man->SetInputEnable( false );
        m_dragOffset = 0;
        m_dragStartScroll = m_param.list_scroll;
        m_pPartsBase->SetBasePane();
        ChangeUpdate( MAINSEQ_DRAG );
        return MYSUBVIEW_ONLY_ACCESS;
      }
      else if( result == SlideScroll::RESULT_DRAG )
      {
        ChangeUpdate( MAINSEQ_DRAG );
        return MYSUBVIEW_ONLY_ACCESS;
      }
      else if( result == SlideScroll::RESULT_RELEASE )
      {
        ChangeUpdate( MAINSEQ_RELEASE );
        return MYSUBVIEW_ONLY_ACCESS;
      }
      else if( result == SlideScroll::RESULT_SLIDE_PLUS )
      {
        StartSlide( 1 );
        ChangeUpdate( MAINSEQ_SLIDE_PLUS );
        return MYSUBVIEW_ONLY_ACCESS;
      }
      else if( result == SlideScroll::RESULT_SLIDE_MINUS )
      {
        StartSlide( -1 );
        ChangeUpdate( MAINSEQ_SLIDE_MINUS );
        return MYSUBVIEW_ONLY_ACCESS;
      }
    }
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.07.22
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult NewPaneList::OnLayoutPaneEvent( const u32 button_id )
{
//  MoveCursor( m_pPartsBase->GetPosPaneLow(button_id), button_id, true );
  m_pPartsBase->SetVisibleCursor( true );
  m_buttonTouchFlag = false;

  if( m_isDecide != false )
  {
    m_listListener->PaneListListener_Decide( m_pPartsBase->GetPosPaneLow(button_id)+static_cast<s32>(m_param.list_scroll) );
  }
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneLongPressEvent
 * @brief   ペイン長押しイベントの検知
 * @date    2015.07.22
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult NewPaneList::OnLayoutPaneLongPressEvent( const u32 button_id )
{
  m_listListener->PaneListListener_PaneLongPress(
    m_param.pane_data, button_id, m_param.list_pos+static_cast<s32>(m_param.list_scroll) );

  m_buttonTouchFlag = false;

  return MYSUBVIEW_ONLY_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneTouchTriggerEvent
 * @brief   ペインタッチ時を検知
 * @date    2015.07.22
 *
 * @param   button_id   選択されたボタンID
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
void NewPaneList::OnLayoutPaneTouchTriggerEvent( const u32 button_id )
{
  // タッチ入力無効
  if( m_isInputTouch == false )
  {
    return;
  }

#if 1   // カーソル移動する？
  MoveCursor( m_pPartsBase->GetPosPaneLow(button_id), button_id, false );
#else
  m_pPartsBase->SetVisibleCursor( false );
#endif
  m_buttonTouchFlag = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.07.22
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult NewPaneList::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
#if PM_DEBUG
#if 0
  // 外部スクロールテスト
  if( button->IsHold(gfl2::ui::BUTTON_X) )
  {
    SetExScroll(SCROLL_MODE_PLUS);
    return DISABLE_ACCESS;
  }
  if( button->IsHold(gfl2::ui::BUTTON_Y) )
  {
    SetExScroll(SCROLL_MODE_MINUS);
    return DISABLE_ACCESS;
  }
#endif
#endif


  // キー入力無効
  if( m_isInputKey == false )
  {
    return ENABLE_ACCESS;
  }

  if( m_mainSeq == MAINSEQ_WAIT )
  {
    NewPaneListPartsBase::InputID input_id = m_pPartsBase->CheckKeyInput( key );

    if( input_id == NewPaneListPartsBase::INPUT_PLUS )
    {
      app::ui::UIResponder::GetButtonManager()->SetInputEnable( false );
      m_pPartsBase->SetDeviceKey( key );
      m_keyScrollCount = 0;
      m_keyScrollFrame = 0;
      if( m_pPartsBase->IsItemChange() == false )
      {
        ChangeUpdate( MAINSEQ_KEY_SCROLL_PLUS );
      }
      else
      {
        ChangeUpdate( MAINSEQ_KEY_ITEM_CHANGE_PLUS );
      }
      return MYSUBVIEW_ONLY_ACCESS;
    }
    else if( input_id == NewPaneListPartsBase::INPUT_MINUS )
    {
      app::ui::UIResponder::GetButtonManager()->SetInputEnable( false );
      m_pPartsBase->SetDeviceKey( key );
      m_keyScrollCount = 0;
      m_keyScrollFrame = 0;
      if( m_pPartsBase->IsItemChange() == false )
      {
        ChangeUpdate( MAINSEQ_KEY_SCROLL_MINUS );
      }
      else
      {
        ChangeUpdate( MAINSEQ_KEY_ITEM_CHANGE_MINUS );
      }
      return MYSUBVIEW_ONLY_ACCESS;
    }

    if( m_skipMode == SKIP_MODE_KEY )
    {
      input_id = m_pPartsBase->CheckKeyInputSkip( key );
    }
    else if( m_skipMode == SKIP_MODE_BUTTON )
    {
      input_id = m_pPartsBase->CheckKeyInputSkip( button );
    }
    else
    {
      input_id = NewPaneListPartsBase::INPUT_NONE;
    }

    if( input_id == NewPaneListPartsBase::INPUT_PLUS )
    {
      if( !( m_param.list_scroll == m_scrollMax && m_param.list_pos == m_param.list_pos_max ) )
      {
        if( m_pPartsBase->IsItemChange() == false )
        {
          ChangeUpdate( MAINSEQ_KEY_SKIP_PLUS );
        }
        else
        {
          ChangeUpdate( MAINSEQ_KEY_ITEM_CHANGE_SKIP_PLUS );
        }
      }
      return MYSUBVIEW_ONLY_ACCESS;
    }
    else if( input_id == NewPaneListPartsBase::INPUT_MINUS )
    {
      if( !( m_param.list_scroll == 0.0f && m_param.list_pos == 0 ) )
      {
        if( m_pPartsBase->IsItemChange() == false )
        {
          ChangeUpdate( MAINSEQ_KEY_SKIP_MINUS );
        }
        else
        {
          ChangeUpdate( MAINSEQ_KEY_ITEM_CHANGE_SKIP_MINUS );
        }
      }
      return MYSUBVIEW_ONLY_ACCESS;
    }

    // 決定
    if( m_isDecide != false )
    {
      if( button->IsTrigger(gfl2::ui::BUTTON_A) )
      {
        // 通常
        if( m_pPartsBase->IsItemChange() == false )
        {
          app::ui::UIResponder::GetButtonManager()->StartSelectedAct( m_pPartsBase->GetPosPaneIndex(m_param.list_pos) );
          return MYSUBVIEW_ONLY_ACCESS;
        }
      }
    }

/*
    // いれかえ
    if( m_itemChangeButtonID != ITEM_CHANGE_NONE )
    {
      if( button->IsTrigger(m_itemChangeButtonID) )
      {
        m_pPartsBase->SetItemChange();
        Sound::PlaySE( m_se[SE_ID_CURSOR_MOVE] );
        return MYSUBVIEW_ONLY_ACCESS;
      }

      if( button->IsTrigger(gfl2::ui::BUTTON_B) )
      {
        if( m_pPartsBase->IsItemChange() != false )
        {
          m_pPartsBase->CancelItemChange();
          return MYSUBVIEW_ONLY_ACCESS;
        }
      }
    }
*/

  }


#if PM_DEBUG
#if 0
  // スクロールテスト
  if( button->IsTrigger(gfl2::ui::BUTTON_X) )
  {
    m_pPartsBase->SetBasePane();
    return DISABLE_ACCESS;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_param.list_scroll = m_pPartsBase->InitListPutBase( 0 );
    return DISABLE_ACCESS;
  }
  if( button->IsHold(gfl2::ui::BUTTON_L) )
  {
    m_param.list_scroll = m_pPartsBase->MoveList( -1 );
    return DISABLE_ACCESS;
  }
  if( button->IsHold(gfl2::ui::BUTTON_R) )
  {
    m_param.list_scroll = m_pPartsBase->MoveList( 1 );
    return DISABLE_ACCESS;
  }
#endif
#endif


  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

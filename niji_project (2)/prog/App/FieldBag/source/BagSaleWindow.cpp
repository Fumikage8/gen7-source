//=============================================================================
/**
 * @file    BagSaleWindow.cpp
 * @brief   バッグ画面：道具売却ウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.07.16
 */
//=============================================================================

// module
#include "App/FieldBag/include/BagSaleWindow.h"
// niji
#include "Savedata/include/MiscSave.h"
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/common_bag/CommonBag.h"
#include "niji_conv_header/app/common_bag/CommonBag_pane.h"
#include "niji_conv_header/app/common_bag/CommonBag_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 * @param   g2d   G2DUtil
 */
//-----------------------------------------------------------------------------
BagSaleWindow::BagSaleWindow( app::util::Heap * heap, app::util::G2DUtil * g2d )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_g2dUtil( g2d )
  , m_key( NULL )
  , m_item( NULL )
  , m_gold( 0 )
  , m_num( 0 )
  , m_result( RESULT_UPDATE )
  , m_isPlus( false )
  , m_isMinus( false )
  , m_isBeforeTouch( false )
  , m_isBeforeKey( false )
{
  Init();
  SetInputListener( NULL );
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagSaleWindow::~BagSaleWindow()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetResult
 * @brief   処理状況を取得
 * @date    2015.07.16
 *
 * @return  処理状況
 */
//-----------------------------------------------------------------------------
BagSaleWindow::Result BagSaleWindow::GetResult(void)
{
  return m_result;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新処理
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::Update(void)
{
  app::ui::UIView::Update();

  // キー操作の矢印ON/OFFチェック
  if( m_isPlus != false )
  {
    if( !m_key->IsHold(gfl2::ui::DIR_UP|gfl2::ui::DIR_RIGHT) )
    {
      SetArrowAnime( 1, false );
      SetInputKey( false );
    }
  }
  if( m_isMinus != false )
  {
    if( !m_key->IsHold(gfl2::ui::DIR_DOWN|gfl2::ui::DIR_LEFT) )
    {
      SetArrowAnime( -1, false );
      SetInputKey( false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画処理
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期処理
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::Init(void)
{
  m_g2dUtil->SetTextBoxPaneString( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTCOUNTER_00, msg_bag_01_15 );
  {
    gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork(0);
    gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );
    gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane(
                                    lytwk->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_SELL ),
                                    PANENAME_BAG_BTN_LOW_000_PANE_TEXTBOX_00,
                                    &res_id );
    m_g2dUtil->SetTextBoxPaneString( text, msg_bag_03_30 );
  }
  CreateButton();

  m_key = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU);
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.07.17
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::CreateButton(void)
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[BUTTON_ID_MAX];

  CreateNormalButton( &info[BUTTON_ID_PLUS] );
  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, BUTTON_ID_MAX );

  m_arrowBound[BUTTON_ID_PLUS]  = info[BUTTON_ID_PLUS].bound_pane;
  m_arrowBound[BUTTON_ID_MINUS] = info[BUTTON_ID_MINUS].bound_pane;

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  man->SetButtonBindKey( BUTTON_ID_DECIDE, gfl2::ui::BUTTON_A, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BUTTON_ID_CANCEL, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  man->SetButtonSelectSE( BUTTON_ID_DECIDE, SEQ_SE_DECIDE1 );
  man->SetButtonSelectSE( BUTTON_ID_CANCEL, SEQ_SE_CANCEL1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetButtonInfoEx
 * @brief   ボタン情報の生成
 * @date    2015.07.17
 *
 * @param   info      生成場所
 * @param   parts_tbl 部品ペインテーブル
 * @param   info_tbl  デフォルト情報テーブル
 * @param   bound     境界ペインテーブル
 * @param   max       生成数
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, const gfl2::lyt::LytPaneIndex * bound, u32 max )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  for( u32 i=0; i<max; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound[i], &res_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateNormalButton
 * @brief   通常のボタン情報生成
 * @date    2015.07.17
 *
 * @param   info  生成場所
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::CreateNormalButton( app::ui::ButtonInfoEx * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_PLUS,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_MINUS,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_SELL,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_BACK,
  };
  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_BAG_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_BAG_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_BAG_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoEx info_tbl[] =
  {
    {
      BUTTON_ID_PLUS, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_TOUCH,
//      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_CANSEL,
//      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_TOUCH_RELEASE,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_PASSIVE,
    },
    {
      BUTTON_ID_MINUS, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_TOUCH,
//      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_CANSEL,
//      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_TOUCH_RELEASE,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_PASSIVE,
    },
    {
      BUTTON_ID_DECIDE, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SELL_PASSIVE,
    },
    {
      BUTTON_ID_CANCEL, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_PASSIVE,
    },
  };

  SetButtonInfoEx( info, parts_tbl, info_tbl, bound_tbl, GFL_NELEMS(parts_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   処理開始
 * @date    2015.07.16
 *
 * @param   item  道具
 * @param   gold  売値
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::Start( const Savedata::MyItem::ITEM_ST * item, u32 gold )
{
  m_item   = item;
  m_gold   = gold;
  m_num    = 1;
  m_result = RESULT_UPDATE;

  UpdateInfo();

  m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_COUNTER_IN );
  m_g2dUtil->AppendChild(
    0,
    PANENAME_BAG_MIN_LOW_000_PANE_ALL,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT,
    app::util::G2DUtil::APPEND_CHILD_MODE_INSERT,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_BACK );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartOnce
 * @brief   所持数１の場合の開始処理
 * @date    2016.04.07
 *
 * @param   item  道具
 * @param   gold  売値
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::StartOnce( const Savedata::MyItem::ITEM_ST * item, u32 gold )
{
  m_item   = item;
  m_gold   = gold;
  m_num    = 1;
  m_result = RESULT_DECIDE;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始チェック
 * @date    2015.07.16
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagSaleWindow::IsStart(void)
{
  if( m_g2dUtil->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000_COUNTER_IN ) == false )
  {
    return false;
  }
  SetInputListener( this );
  SetInputEnabled( true );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   処理終了
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::End(void)
{
  m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_COUNTER_OUT );
  m_g2dUtil->AppendChild(
    0,
    PANENAME_BAG_MIN_LOW_000_PANE_ALL,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_BACK,
    app::util::G2DUtil::APPEND_CHILD_MODE_INSERT,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT );
  SetInputListener( NULL );
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEnd
 * @brief   終了チェック
 * @date    2015.07.16
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagSaleWindow::IsEnd(void)
{
  return m_g2dUtil->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000_COUNTER_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItem
 * @brief   道具取得
 * @date    2015.07.16
 *
 * @return  道具
 */
//-----------------------------------------------------------------------------
const Savedata::MyItem::ITEM_ST * BagSaleWindow::GetItem(void)
{
  return m_item;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGold
 * @brief   売値取得
 * @date    2015.07.16
 *
 * @return  売値
 */
//-----------------------------------------------------------------------------
u32 BagSaleWindow::GetGold(void)
{
  return m_gold;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetNum
 * @brief   個数取得
 * @date    2015.07.16
 *
 * @return  個数
 */
//-----------------------------------------------------------------------------
u32 BagSaleWindow::GetNum(void)
{
  return m_num;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateInfo
 * @brief   情報更新
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::UpdateInfo(void)
{
  m_g2dUtil->SetTextBoxPaneNumber( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTCOUNTER_01, msg_bag_01_17, m_num, 3, 0, print::NUM_DISP_ZERO );
  m_g2dUtil->SetTextBoxPaneNumber( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTCOUNTER_02, msg_bag_01_27, m_gold*m_num, Savedata::Misc::SELL_PRICE_KETA, 0 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetArrowAnime
 * @brief   矢印アニメセット（キー操作用）
 * @date    2015.07.17
 *
 * @param   mv    個数の増減方向
 * @param   flg   true = ON, false = OFF
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::SetArrowAnime( int mv, bool flg )
{
  if( ( mv > 0 && m_isPlus == flg ) ||
      ( mv < 0 && m_isMinus == flg ) )
  {
    return;
  }

  if( flg != false )
  {
    if( mv > 0 )
    {
      m_g2dUtil->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_CANSEL );
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_TOUCH );
      m_isPlus = flg;
    }
    else
    {
      m_g2dUtil->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_CANSEL );
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_TOUCH );
      m_isMinus = flg;
    }
  }
  else
  {
    if( mv > 0 )
    {
      m_g2dUtil->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_TOUCH );
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_PLUS_CANSEL );
      m_isPlus = flg;
    }
    else
    {
      m_g2dUtil->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_TOUCH );
      m_g2dUtil->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_MINUS_CANSEL );
      m_isMinus = flg;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetValue
 * @brief   個数を変更する
 * @date    2015.07.17
 *
 * @param   plus    増減値
 * @param   is_trg  true = トリガー入力
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::SetValue( int plus, bool is_trg )
{
  int tmp = static_cast<int>( m_num );
  int val = tmp + plus;

  if( val > static_cast<int>(m_item->num) )
  {
    if( is_trg != false && tmp == static_cast<int>(m_item->num) )
    {
      val = 1;
    }
    else
    {
      val = static_cast<int>( m_item->num );
    }
  }
  else if( val <= 0 )
  {
    if( is_trg != false && tmp == 1 )
    {
      val = static_cast<int>( m_item->num );
    }
    else
    {
      val = 1;
    }
  }

  if( val == tmp )
  {
    return;
  }
  m_num = static_cast<u32>( val );
  UpdateInfo();
  Sound::PlaySE( SEQ_SE_SELECT1 );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsArrowHit
 * @brief   矢印上をタッチしているか
 * @date    2015.07.17
 *
 * @param   id  ボタンID
 *
 * @retval  true  = タッチしている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagSaleWindow::IsArrowHit( ButtonID id )
{
  if( app::ui::UIResponder::GetButtonManager()->GetHoldingButtonId() != id )
  {
    return false;
  }
  return m_g2dUtil->IsPaneTouchHold( 0, m_arrowBound[id] );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputKey
 * @brief   キー入力の状態を設定
 * @date    2016.03.25
 *
 * @param   flg   true = キー入力, false = それ以外
 */
//-----------------------------------------------------------------------------
void BagSaleWindow::SetInputKey( bool flg )
{
  app::ui::UIResponder::GetButtonManager()->SetInputEnable( !flg );
  m_isBeforeKey = flg;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.07.16
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
app::ui::UIInputListener::ListenerResult BagSaleWindow::OnLayoutPaneEvent( const u32 button_id )
{
  switch( button_id )
  {
  case BUTTON_ID_DECIDE:
    m_result = RESULT_DECIDE;
    break;
  case BUTTON_ID_CANCEL:
    m_result = RESULT_CANCEL;
    break;
  }
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.07.17
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
app::ui::UIInputListener::ListenerResult BagSaleWindow::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  if( m_isBeforeKey != false )
  {
    return ENABLE_ACCESS;
  }

  app::tool::ButtonManager * man = app::ui::UIResponder::GetButtonManager();

  if( man->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_HOLDING )
  {
    if( touch_panel->IsTouchRepeat() != false )
    {
      if( IsArrowHit( BUTTON_ID_PLUS ) != false )
      {
        SetValue( 1, !m_isBeforeTouch );
      }
      else if( IsArrowHit( BUTTON_ID_MINUS ) != false )
      {
        SetValue( -1, !m_isBeforeTouch );
      }
    }
  }

  m_isBeforeTouch = is_touch;

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.07.16
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
app::ui::UIInputListener::ListenerResult BagSaleWindow::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_isBeforeTouch != false )
  {
    return ENABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_UP) )
  {
    SetValue( 1, true );
    SetArrowAnime( 1, true );
    SetArrowAnime( -1, false );
    SetInputKey( true );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
  {
    SetValue( -1, true );
    SetArrowAnime( -1, true );
    SetArrowAnime( 1, false );
    SetInputKey( true );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_LEFT) )
  {
    SetValue( -10, true );
    SetArrowAnime( -1, true );
    SetArrowAnime( 1, false );
    SetInputKey( true );
    return DISABLE_ACCESS;
  }
  if( key->IsTrigger(gfl2::ui::DIR_RIGHT) )
  {
    SetValue( 10, true );
    SetArrowAnime( 1, true );
    SetArrowAnime( -1, false );
    SetInputKey( true );
    return DISABLE_ACCESS;
  }

  if( m_isBeforeKey != false )
  {
    if( key->IsRepeat(gfl2::ui::DIR_UP) )
    {
      SetValue( 1, false );
      SetArrowAnime( 1, true );
      SetArrowAnime( -1, false );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      SetValue( -1, false );
      SetArrowAnime( -1, true );
      SetArrowAnime( 1, false );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
    {
      SetValue( -10, false );
      SetArrowAnime( -1, true );
      SetArrowAnime( 1, false );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
    {
      SetValue( 10, false );
      SetArrowAnime( 1, true );
      SetArrowAnime( -1, false );
      return DISABLE_ACCESS;
    }

    if( key->IsHold(gfl2::ui::DIR_UP|gfl2::ui::DIR_DOWN|gfl2::ui::DIR_LEFT|gfl2::ui::DIR_RIGHT) )
    {
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

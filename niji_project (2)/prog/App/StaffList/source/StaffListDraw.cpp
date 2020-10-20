//=============================================================================
/**
 * @file    StaffListDraw.cpp
 * @brief   momiji追加分のスタッフロール画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================

// module
#include "StaffListDraw.h"
// resource
#include "niji_conv_header/app/stafflist/StaffList.h"
#include "niji_conv_header/app/stafflist/StaffList_pane.h"
#include "niji_conv_header/app/stafflist/StaffList_anim_list.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

//! 開始時の背景アルファ加算値
const f32 StaffListDraw::START_ALPHA_BG_CALC_VALUE = static_cast<f32>(StaffListDraw::BG_ALPHA) / static_cast<f32>(StaffListDraw::START_ALPHA_FRAME_BG);
//! 開始時の文字列アルファ加算値
const f32 StaffListDraw::START_ALPHA_STR_CALC_VALUE = static_cast<f32>(StaffListDraw::STR_ALPHA) / static_cast<f32>(StaffListDraw::START_ALPHA_FRAME_STR);
//! 終了時の背景アルファ加算値
const f32 StaffListDraw::END_ALPHA_BG_CALC_VALUE = static_cast<f32>(StaffListDraw::BG_ALPHA) / static_cast<f32>(StaffListDraw::END_ALPHA_FRAME);
//! 終了時の文字列アルファ加算値
const f32 StaffListDraw::END_ALPHA_STR_CALC_VALUE = static_cast<f32>(StaffListDraw::STR_ALPHA) / static_cast<f32>(StaffListDraw::END_ALPHA_FRAME);

//! テキストボックスペインテーブル
const gfl2::lyt::LytPaneIndex StaffListDraw::TextPaneTable[StaffListDraw::TEXT_PANE_MAX] =
{
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_00,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_01,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_02,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_03,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_04,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_05,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_06,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_07,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_08,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_09,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_10,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_11,
  // ここから二重化用
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_12,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_13,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_14,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_15,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_20,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_21,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_22,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_23,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_24,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_25,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_26,
  PANENAME_STAFFLIST_TXT_UPP_000_PANE_TEXTBOX_27,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 */
//-----------------------------------------------------------------------------
StaffListDraw::StaffListDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_bgSeq( 0 )
  , m_bgCount( 0 )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
#if PM_DEBUG
  , m_listener( NULL )
#endif // PM_DEBUG
{
  InitLabelWork();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
StaffListDraw::~StaffListDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListDraw::Update(void)
{
  app::ui::UIView::Update();
  if( m_isDrawEnable != false )
  {
    UpdateMain();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2017.03.02
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void StaffListDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( s32 i=0; i<LYTID_MAX; i++ )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, i );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2017.03.02
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::Init( void * res_buf, void * msg_buf )
{
  Initialize2D( res_buf, msg_buf );
  SetInputListener( this );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2017.03.02
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2017.03.02
 *
 * @param   res_buf   レイアウトリソースバッファ
 * @param   msg_buf   メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void StaffListDraw::Initialize2D( void * res_buf, void * msg_buf )
{
  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_STAFFLIST_STAFFLIST_TXT_UPP_000_BFLYT,
      0,
      res_buf,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  InitPane();
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPane
 * @brief   ペインの状態初期設定
 * @date    2017.03.03
 */
//-----------------------------------------------------------------------------
void StaffListDraw::InitPane(void)
{
  SetBgPaneAlpha( 0.0f );
  for( u32 i=0; i<TEXT_PANE_MAX; i++ )
  {
    SetTextPaneAlpha( i, 0.0f );
    PutTextPane( i, DISPLAY_UPPER_WIDTH, DISPLAY_UPPER_HEIGHT );  // 画面外へ
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetBgPaneAlpha
 * @brief   背景ペインにアルファを設定
 * @date    2017.03.03
 *
 * @param   alpha   アルファ値
 */
//-----------------------------------------------------------------------------
void StaffListDraw::SetBgPaneAlpha( f32 alpha )
{
  GetG2DUtil()->SetPaneAlpha( LYTID_UPPER, PANENAME_STAFFLIST_TXT_UPP_000_PANE_BG_00, static_cast<u8>(alpha) );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetBgPaneAlpha
 * @brief   背景ペインのアルファを取得
 * @date    2017.03.17
 *
 * @return  アルファ値
 */
//-----------------------------------------------------------------------------
u32 StaffListDraw::GetBgPaneAlpha(void)
{
  return GetG2DUtil()->GetPaneAlpha( LYTID_UPPER, PANENAME_STAFFLIST_TXT_UPP_000_PANE_BG_00 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutBgPane
 * @brief   背景ペインを配置
 * @date    2017.03.03
 *
 * @param   mode  配置モード
 */
//-----------------------------------------------------------------------------
void StaffListDraw::PutBgPane( BgPutMode mode )
{
  static const f32 tbl[][4] =
  {
    // x, y, width, height
    { -60.0f, 0.0f, 280.0f, 240.0f },   // 左
    {  60.0f, 0.0f, 280.0f, 240.0f },   // 右
    {   0.0f, 0.0f, 400.0f, 100.0f },   // 中央
  };

  if( mode >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    mode = BG_PUT_LEFT;
  }

  gfl2::math::VEC3 pos;
  pos.x = tbl[mode][0];
  pos.y = tbl[mode][1];
  pos.z = 0.0f;

  nw::lyt::Size size;
  size.width  = tbl[mode][2];
  size.height = tbl[mode][3];

  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetPanePos( LYTID_UPPER, PANENAME_STAFFLIST_TXT_UPP_000_PANE_BG_00, &pos );
  g2d->SetSize( LYTID_UPPER, PANENAME_STAFFLIST_TXT_UPP_000_PANE_BG_00, size );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTextPaneAlpha
 * @brief   テキストペインにアルファを設定
 * @date    2017.03.03
 *
 * @param   index   インデックス
 * @param   alpha   アルファ値
 */
//-----------------------------------------------------------------------------
void StaffListDraw::SetTextPaneAlpha( u32 index, f32 alpha )
{
  GetG2DUtil()->SetPaneAlpha( LYTID_UPPER, TextPaneTable[index], static_cast<u8>(alpha) );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutTextPane
 * @brief   デキストペインを配置
 * @date    2017.03.03
 *
 * @param   index   インデックス
 * @param   x       X座標
 * @param   y       Y座標
 */
//-----------------------------------------------------------------------------
void StaffListDraw::PutTextPane( u32 index, f32 x, f32 y )
{
  gfl2::math::VEC3 pos( x, y, 0.0f );
  GetG2DUtil()->SetPanePos( LYTID_UPPER, TextPaneTable[index], &pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetString
 * @brief   文字列書き込み
 * @date    2017.03.03
 *
 * @param   index   インデックス
 * @param   label   テキストラベル
 * @param   mode    配置モード
 */
//-----------------------------------------------------------------------------
void StaffListDraw::SetString( u32 index, u32 label, TextPutMode mode )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->GetString( str, label );
  g2d->SetTextBoxPaneString( LYTID_UPPER, TextPaneTable[index], str );
  
  gfl2::lyt::LytTextBox * text_box = g2d->GetLayoutWork(LYTID_UPPER)->GetTextBoxPane( TextPaneTable[index] );
  text_box->SetTextPositionH( static_cast<nw::lyt::HorizontalPosition>(mode) );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsVanishBg
 * @brief   背景が非表示か
 * @date    2017.03.03
 *
 * @retval  true  = 非表示
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::IsVanishBg(void)
{
  if( m_bgSeq == BG_SEQ_WAIT )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListData
 * @brief   リストデータをセット
 * @date    2017.03.03
 *
 * @param   no    データ番号
 * @param   data  リストデータ
 * @param   max   リストデータ数
 */
//-----------------------------------------------------------------------------
void StaffListDraw::SetListData( u32 no, const LIST_ITEM_DATA * data, u32 max )
{
  for( u32 i=0; i<max; i++ )
  {
    if( data[i].index == no )
    {
      u32 index = SetLabelWork( &data[i] );
      if( index != LABEL_WORK_NONE )
      {
        SetTextPaneAlpha( index, 0.0f );
        PutTextPane( index, static_cast<f32>(data[i].x), static_cast<f32>(data[i].y) );
        SetString( index, data[i].label, static_cast<TextPutMode>(data[i].put) );
        // 二重化
        if( data[i].is_double != 0 )
        {
          u32 double_index = SetDoubleLabelWork( index );
          if( double_index != LABEL_WORK_NONE )
          {
            SetTextPaneAlpha( double_index, 0.0f );
            PutTextPane( double_index, static_cast<f32>(data[i].x), static_cast<f32>(data[i].y) );
            SetString( double_index, data[i].label, static_cast<TextPutMode>(data[i].put) );
          }
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitLabelWork
 * @brief   ラベルワークを初期化
 * @date    2017.03.03
 */
//-----------------------------------------------------------------------------
void StaffListDraw::InitLabelWork(void)
{
  gfl2::std::MemClear( m_labelWork, sizeof(LABEL_WORK)*TEXT_PANE_MAX );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetLabelWork
 * @brief   ラベルワークをセット
 * @date    2017.03.03
 *
 * @param   data  リストデータ
 *
 * @retval  LABEL_WORK_NONE  = 失敗
 * @retval  LABEL_WORK_NONE != セットしたリストデータのインデックス
 */
//-----------------------------------------------------------------------------
u32 StaffListDraw::SetLabelWork( const LIST_ITEM_DATA * data )
{
  for( u32 i=0; i<TEXT_PANE_DOUBLE_INDEX; i++ )
  {
    if( m_labelWork[i].seq == LABEL_SEQ_NONE )
    {
      m_labelWork[i].seq          = LABEL_SEQ_IN;
      m_labelWork[i].count        = 0;
      m_labelWork[i].frame        = data->frame;
      m_labelWork[i].is_end       = data->is_end;
      m_labelWork[i].is_vanish_bg = data->is_vanish_bg;
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return LABEL_WORK_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDoubleLabelWork
 * @brief   二重化用ラベルワークをセット
 * @date    2017.04.07
 *
 * @param   index   二重化元のワークインデックス
 *
 * @retval  LABEL_WORK_NONE  = 失敗
 * @retval  LABEL_WORK_NONE != セットしたリストデータのインデックス
 */
//-----------------------------------------------------------------------------
u32 StaffListDraw::SetDoubleLabelWork( u32 index )
{
  for( u32 i=TEXT_PANE_DOUBLE_INDEX; i<TEXT_PANE_MAX; i++ )
  {
    if( m_labelWork[i].seq == LABEL_SEQ_NONE )
    {
      m_labelWork[i] = m_labelWork[index];
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return LABEL_WORK_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsActiveLabel
 * @brief   動作中のラベルデータがあるか
 * @date    2017.03.03
 *
 * @retval  true  = 有
 * @retval  false = 無
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::IsActiveLabel(void)
{
  for( u32 i=0; i<TEXT_PANE_MAX; i++ )
  {
    if( m_labelWork[i].seq == LABEL_SEQ_IN ||
        m_labelWork[i].seq == LABEL_SEQ_WAIT ||
        m_labelWork[i].seq == LABEL_SEQ_OUT )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEndLabel
 * @brief   終了を発行しているラベルがあるか
 * @date    2017.03.03
 *
 * @retval  true  = 有
 * @retval  false = 無
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::IsEndLabel(void)
{
  for( u32 i=0; i<TEXT_PANE_MAX; i++ )
  {
    if( m_labelWork[i].seq == LABEL_SEQ_END )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsVanishBgLabel
 * @brief   背景を非表示にしているラベルデータがあるか
 * @date    2017.03.17
 *
 * @retval  true  = 有
 * @retval  false = 無
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::IsVanishBgLabel(void)
{
  for( u32 i=0; i<TEXT_PANE_MAX; i++ )
  {
    if( m_labelWork[i].is_vanish_bg != false )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsSetLabel
 * @brief   設定されているラベルデータがあるか
 * @date    2017.03.17
 *
 * @retval  true  = 有
 * @retval  false = 無
 */
//-----------------------------------------------------------------------------
bool StaffListDraw::IsSetLabel(void)
{
  for( u32 i=0; i<TEXT_PANE_MAX; i++ )
  {
    if( m_labelWork[i].seq != LABEL_SEQ_NONE )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateLabel
 * @brief   ラベル更新処理
 * @date    2017.03.03
 *
 * @param   index   ラベルワークインデックス
 */
//-----------------------------------------------------------------------------
void StaffListDraw::UpdateLabel( u32 index )
{
  LABEL_WORK * wk = &m_labelWork[index];

  switch( wk->seq )
  {
  case LABEL_SEQ_NONE:
    break;

  case LABEL_SEQ_IN:
    if( wk->count != START_ALPHA_FRAME_STR )
    {
      SetTextPaneAlpha( index, START_ALPHA_STR_CALC_VALUE*static_cast<f32>(wk->count) );
      wk->count++;
      break;
    }
    SetTextPaneAlpha( index, static_cast<f32>(STR_ALPHA) );
    wk->count = 0;
    wk->seq = LABEL_SEQ_WAIT;
    /* FALL THROUGH */
  case LABEL_SEQ_WAIT:
    if( wk->count != wk->frame )
    {
      wk->count++;
      break;
    }
    wk->count = 0;
    if( wk->is_end != 0 )
    {
      wk->seq = LABEL_SEQ_END;
      break;
    }
    wk->seq = LABEL_SEQ_OUT;
    /* FALL THROUGH */
  case LABEL_SEQ_OUT:
    if( wk->count != END_ALPHA_FRAME )
    {
      SetTextPaneAlpha(
        index,
        static_cast<f32>(STR_ALPHA)-END_ALPHA_STR_CALC_VALUE*static_cast<f32>(wk->count) );
      wk->count++;
      break;
    }
    SetTextPaneAlpha( index, 0.0f );
    PutTextPane( index, DISPLAY_UPPER_WIDTH, DISPLAY_UPPER_HEIGHT );  // 画面外へ
    gfl2::std::MemClear( wk, sizeof(LABEL_WORK) );
    break;

  case LABEL_SEQ_END:
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMain
 * @brief   全体の更新処理
 * @date    2017.03.03
 */
//-----------------------------------------------------------------------------
void StaffListDraw::UpdateMain(void)
{
  switch( m_bgSeq )
  {
  case BG_SEQ_WAIT:
    if( IsActiveLabel() == false )
    {
      break;
    }
    m_bgCount = 0;
    m_bgSeq = BG_SEQ_IN;
    /* FALL THROUGH */
  case BG_SEQ_IN:
    if( IsVanishBgLabel() == false )
    {
      if( m_bgCount != START_ALPHA_FRAME_BG )
      {
        SetBgPaneAlpha( START_ALPHA_BG_CALC_VALUE*static_cast<f32>(m_bgCount) );
        m_bgCount++;
        break;
      }
      SetBgPaneAlpha( static_cast<f32>(BG_ALPHA) );
      m_bgCount = 0;
    }
    m_bgSeq = BG_SEQ_MAIN;
    /* FALL THROUGH */
  case BG_SEQ_MAIN:
    for( u32 i=0; i<TEXT_PANE_MAX; i++ )
    {
      UpdateLabel( i );
    }
    if( IsEndLabel() != false )
    {
      m_bgSeq = BG_SEQ_OUT;
    }
    else if( IsSetLabel() == false )
    {
      if( GetBgPaneAlpha() == 0 )
      {
        m_bgSeq = BG_SEQ_WAIT;
      }
    }
    break;

  case BG_SEQ_OUT:
    if( m_bgCount != END_ALPHA_FRAME )
    {
      SetBgPaneAlpha(
        static_cast<f32>(BG_ALPHA)-END_ALPHA_BG_CALC_VALUE*static_cast<f32>(m_bgCount) );
      for( u32 i=0; i<TEXT_PANE_MAX; i++ )
      {
        if( m_labelWork[i].seq != LABEL_SEQ_NONE )
        {
          SetTextPaneAlpha(
            i, static_cast<f32>(STR_ALPHA)-END_ALPHA_STR_CALC_VALUE*static_cast<f32>(m_bgCount) );
        }
      }
      m_bgCount++;
      break;
    }
    InitLabelWork();
    InitPane();
    m_bgCount = 0;
    m_bgSeq = BG_SEQ_WAIT;
    break;
  }
}


#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ（デバッグ用）
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   listener    リスナー
 */
//-----------------------------------------------------------------------------
StaffListDraw::StaffListDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, Listener * listener )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
  , m_listener( listener )
{
  InitLabelWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2017.03.02
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
app::ui::UIInputListener::ListenerResult StaffListDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    m_listener->OnAction( Listener::ACTION_ID_REQ_0 );
    return DISABLE_ACCESS;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_X) )
  {
    m_listener->OnAction( Listener::ACTION_ID_REQ_1 );
    return DISABLE_ACCESS;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_listener->OnAction( Listener::ACTION_ID_REQ_2 );
    return DISABLE_ACCESS;
  }
  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    m_listener->OnAction( Listener::ACTION_ID_END );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}
#endif // PM_DEBUG


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)

//======================================================================
/**
 * @file    NumberInputDraw.cpp
 * @date    2015/11/24 17:02:34
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：グラフィック関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "NumberInputDraw.h"

// arc
#include <arc_index/message.gaix>
#include <niji_conv_header/app/number_input/number_input.h>
#include <niji_conv_header/app/number_input/number_input_pane.h>
#include <niji_conv_header/app/number_input/number_input_anim_list.h>
#include <niji_conv_header/message/msg_number_input.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap        ヒープ
 * @param   man         ファイルマネージャ
 * @param   renderMan   レンダリングマネージャ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
NumberInputDraw::NumberInputDraw( app::util::Heap* heap, gfl2::fs::AsyncFileManager* man, app::util::AppRenderingManager* renderMan )
  : m_pHeap( heap )
  , m_pAsyncFileManager( man )
  , m_pAppRenderingManager( renderMan )
  , m_EndFlag( false )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   pAppLytBuff   レイアウトバッファ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::Initialize( void * pAppLytBuff )
{
  Create2D( pAppLytBuff );
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 *
 * @param   none
 *
 * @return  "true  = 完了"
 * @return  "false = 処理中"
 */
//------------------------------------------------------------------------------
bool NumberInputDraw::Finalize( void )
{
  m_EndFlag = true;

  if( !Delete2D() )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   アップデート
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::Update( void )
{
  if( m_EndFlag )
  {
    return;
  }

  m_pG2D->Update2D();
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 *
 * @param   no    描画ディスプレイ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_EndFlag )
  {
    return;
  }

  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LYTID_UPPER );
  m_pG2D->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, LYTID_LOWER );
}

//------------------------------------------------------------------------------
/**
 * @brief   ファイル読み込みチェック
 *
 * @param   none
 *
 * @return  "true  = 読み込み中"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool NumberInputDraw::CheckFileRead( void )
{
  return false;
}


//------------------------------------------------------------------------------
/**
 * @brief   レイアウトワークの取得
 *
 * @param   id    レイアウトID
 *
 * @return  レイアウトワーク
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytWk* NumberInputDraw::GetLayoutWork( u32 id )
{
  return m_pG2D->GetLayoutWork( id );
}

//------------------------------------------------------------------------------
/**
 * @brief   レイアウトリソースIDの取得
 *
 * @param   id    レイアウトID
 *
 * @return  レイアウトリソースID
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytMultiResID NumberInputDraw::GetLayoutResourceID( void )
{
  return m_pG2D->GetLayoutResourceID( 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   モード別初期化
 *
 * @param   full    true = 数値+アルファベット, false = 数値のみ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::InitMode( bool full )
{
  if( full )
  {
    m_pG2D->StartAnime( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW_TYPE_02 );
  }
  else {
    GFL_ASSERT(0);    //!< 仕様上ここにはこないはず
    m_pG2D->StartAnime( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW_TYPE_01 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モード初期化アニメ再生チェック
 *
 * @param   none
 *
 * @return  "true  = 再生中"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool NumberInputDraw::CheckModeAnime( void )
{
  if( m_pG2D->IsAnimeEnd( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW_TYPE_01 ) &&
    m_pG2D->IsAnimeEnd( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW_TYPE_02 ) )
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力されたデータを配置
 *
 * @param   pos   データ位置
 * @param   val   入力されたデータ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::PutInputCode( u32 pos, s32 val )
{
  if( val == INPUT_DEFAULT_CODE )
  {
    m_pG2D->SetTextBoxPaneString( m_pInputText[pos], msg_number_input_none );
  }
  else {
    m_pG2D->SetTextBoxPaneString( m_pInputText[pos], msg_number_button_101 + val );
  }

  FUKUSHIMA_PRINT( "%s : pos[%d] val[%d]\n", __FUNCTION__, pos, val );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力文字コード取得
 *
 * @param   pos   キーボード位置
 *
 * @return  文字コード
 */
//------------------------------------------------------------------------------
gfl2::str::STRCODE NumberInputDraw::GetInputStrCode( u32 pos )
{
  gfl2::str::StrBuf* strBuf = m_pG2D->GetTempStrBuf( 0 );

  m_pG2D->GetString( strBuf, msg_number_button_101 + pos );

  const gfl2::str::STRCODE * code = strBuf->GetPtr();

  return code[0];
}

//------------------------------------------------------------------------------
/**
 * @brief   文字コードからキーボードの位置を取得
 *
 * @param   code    文字コード
 *
 * @return  キーボード位置
 */
//------------------------------------------------------------------------------
u32 NumberInputDraw::GetStrCodePos( gfl2::str::STRCODE code )
{
  gfl2::str::StrBuf* strBuf = m_pG2D->GetTempStrBuf( 0 );

  for( u32 i=0; i<CODE_BUTTON_MAX; ++i )
  {
    m_pG2D->GetString( strBuf, msg_number_button_101 + i );
    const gfl2::str::STRCODE * ptr = strBuf->GetPtr();
    if( ptr[0] == code )
    {
      return i;
    }
  }

  GFL_ASSERT(0);
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置
 *
 * @param   posPane     基準位置のペイン
 * @param   sizePane    基準サイズのペイン
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::PutCursor( gfl2::lyt::LytPane* posPane, gfl2::lyt::LytPane* sizePane )
{
  GFL_ASSERT( posPane );
  GFL_ASSERT( sizePane );

  nw::lyt::Size size = sizePane->GetSize();
  {
    size.width  += 20;
    size.height += 20;

    m_pCursorWindow->SetSize( size );
  }

  gfl2::math::VEC3 trans = posPane->GetTranslate();
  {
    m_pCursorParts->SetTranslate( trans );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力位置カーソル配置
 *
 * @param   pos   入力位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::PutInputCursor( u32 pos )
{
  gfl2::math::VEC3 trans = m_pG2D->GetPanePos( m_pInputParts[pos] );

  f32 x = trans.x;

  trans = m_pG2D->GetPanePos( m_pInputMark );

  trans.x = x;  //!< 入力位置の更新

  m_pInputMark->SetTranslate( trans );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力位置カーソル表示切り替え
 *
 * @param   flg   true = 表示
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::SetVisibleInputCursor( bool flg )
{
  m_pG2D->SetPaneVisible( m_pInputMark, flg );
}

//------------------------------------------------------------------------------
/**
 * @brief   キーボード選択アニメ開始
 *
 * @param   id    キーボードID
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::StartKeyBoardSelectAnime( u32 id )
{
  static const u32 select_anim[] = {
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_00_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_01_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_02_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_03_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_04_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_05_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_06_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_07_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_08_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_09_TOUCH_RELEASE,

    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_10_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_11_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_12_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_13_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_14_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_15_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_16_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_17_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_18_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_19_TOUCH_RELEASE,

    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_20_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_21_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_22_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_23_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_24_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_25_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_26_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_27_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_28_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_29_TOUCH_RELEASE,

    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_30_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_31_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_32_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_33_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_34_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_35_TOUCH_RELEASE,

    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__BUTTON1_00_TOUCH_RELEASE,
    LA_NUMBER_INPUT_NUMBER_INPUT_LOW__BUTTON1_01_TOUCH_RELEASE,
  };

  FUKUSHIMA_PRINT( "%s : id[%d] animID[%d]\n", __FUNCTION__, id, select_anim[id] );

  m_pG2D->StartAnime( LYTID_LOWER, select_anim[id] );
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面文字表示
 *
 * @param   mode    表示モード
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::PutUpperString( u32 mode )
{
  static const u32 tbl[] = {
    msg_number_01_01,
    msg_number_01_02,
  };

  gfl2::lyt::LytWk* lytwk = GetLayoutWork( LYTID_UPPER );
  gfl2::lyt::LytMultiResID resID = GetLayoutResourceID();

  gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( PANENAME_NUMBER_INPUT_UPP_PANE_MESSAGE );
  gfl2::lyt::LytTextBox* textbox = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00, &resID );

  m_pG2D->SetTextBoxPaneString( textbox, tbl[mode] );

  m_pG2D->SetPaneVisible( textbox, true );
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   pAppLytBuff     レイアウトバッファ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::Create2D( void * pAppLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  // G2DUtilの生成
  m_pG2D = GFL_NEW(devHeap) app::util::G2DUtil( m_pHeap );

  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) }
  };

  // レイアウトデータ
  const app::util::G2DUtil::LytwkData layoutData[] = {
    { // 上画面
      0,
      LYTRES_NUMBER_INPUT_NUMBER_INPUT_UPP_BFLYT,
      LA_NUMBER_INPUT_NUMBER_INPUT_UPP___NUM,
      pAppLytBuff,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true,
    },
    { // 下画面
      0,
      LYTRES_NUMBER_INPUT_NUMBER_INPUT_LOW_BFLYT,
      LA_NUMBER_INPUT_NUMBER_INPUT_LOW___NUM,
      pAppLytBuff,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true,
    },
  };

  m_pG2D->CreateAppLytAccessor( m_pHeap );

  m_pG2D->CreateLayoutSystem( m_pHeap );
  m_pG2D->CreateLayoutEnv( m_pHeap, false );

  m_pG2D->CreateLayoutWork( resTbl, GFL_NELEMS(resTbl), GFL_NELEMS(layoutData), m_pHeap, gfl2::lyt::MEMORY_AREA_FCRAM );
  
  m_pG2D->AttachLayoutResourceReference( resTbl, GFL_NELEMS(resTbl) );
  
  m_pG2D->SetLayoutWork( 0, layoutData, GFL_NELEMS(layoutData), m_pHeap->GetSystemAllocator(), m_pHeap->GetDeviceAllocator(), false, false );
  
  m_pG2D->CreateMessageData( GARC_message_number_input_DAT, gfl2::str::MsgData::LOAD_FULL, m_pHeap );

  // ペインのセットアップ
  SetupPane();
  // コードの初期化
  InitInputCode();
  // キーボード上の文字表示
  PutKeyBoard();

  m_pG2D->StartAnime( LYTID_UPPER, LA_NUMBER_INPUT_NUMBER_INPUT_UPP__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );
  m_pG2D->StartAnime( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__CURSOR_00_CURSOL );
  m_pG2D->StartAnime( LYTID_LOWER, LA_NUMBER_INPUT_NUMBER_INPUT_LOW_INPUT_MARK );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連削除
 *
 * @param   none
 *
 * @return  "true  = 完了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool NumberInputDraw::Delete2D( void )
{
  if( m_pG2D == NULL )
  {
    return true;
  }

  if( m_pG2D->IsDrawing() )
  {
    return false;
  }

  m_pG2D->DeleteMessageData();
  m_pG2D->DeleteLayoutWork();
  m_pG2D->DeleteLayoutEnv();
  m_pG2D->DeleteLayoutSystem();
  m_pG2D->DeleteAppLytAccessor();

  GFL_DELETE  m_pG2D;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   キーボード上に文字を配置
 * 
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::PutKeyBoard( void )
{
  enum {
    PANE_KIND_PARTS = 0,      //!< パーツ
    PANE_KIND_PANE,           //!< パーツ内ペイン
    PANE_KIND_NUM,
  };

  static const u32 KEYBOARD_MAX = 10 + 26 + 2;   //!< 数字(10) + アルファベット(26) + 消す + 終了

  static const gfl2::lyt::LytPaneIndex tbl[KEYBOARD_MAX][PANE_KIND_NUM] = {
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_00, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_01, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_02, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_03, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_04, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_05, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_06, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_07, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_08, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_09, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },

    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_10, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_11, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_12, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_13, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_14, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_15, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_16, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_17, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_18, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_19, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },

    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_20, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_21, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_22, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_23, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_24, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_25, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_26, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_27, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_28, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_29, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },

    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_30, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_31, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_32, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_33, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_34, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_WORD_35, PANENAME_STRINPUT_BTN_LOW_000_PANE_MOJI_00 },

    { PANENAME_NUMBER_INPUT_LOW_PANE_BUTTON1_00, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00 },
    { PANENAME_NUMBER_INPUT_LOW_PANE_BUTTON1_01, PANENAME_STRINPUT_BTN_LOW_002_PANE_TEXTBOX_00 },
  };

  gfl2::lyt::LytWk* lytwk = GetLayoutWork( LYTID_LOWER );
  gfl2::lyt::LytMultiResID resID = GetLayoutResourceID();

  for( u32 i=0; i<GFL_NELEMS(tbl); ++i )
  {
    gfl2::lyt::LytPaneIndex partsIndex = tbl[i][PANE_KIND_PARTS];
    gfl2::lyt::LytPaneIndex paneIndex  = tbl[i][PANE_KIND_PANE];

    gfl2::lyt::LytParts* parts = lytwk->GetPartsPane( partsIndex );
    gfl2::lyt::LytTextBox* textbox = lytwk->GetTextBoxPane( parts, paneIndex, &resID );

    m_pG2D->SetTextBoxPaneString( textbox, msg_number_button_101 + i );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   入力データ表示を初期化
 * 
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::InitInputCode( void )
{
  for( u32 i=0;i<INPUT_PART_CODE_MAX * INPUT_PART_MAX; ++i )
  {
    PutInputCode( i, INPUT_DEFAULT_CODE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインのセットアップ
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void NumberInputDraw::SetupPane( void )
{
  gfl2::lyt::LytMultiResID resID = GetLayoutResourceID();

  { // 上画面
    gfl2::lyt::LytWk* lytwk = GetLayoutWork( LYTID_UPPER );
    
  }

  { // 下画面
    gfl2::lyt::LytWk* lytwk = GetLayoutWork( LYTID_LOWER );
    
    m_pInputMark = lytwk->GetPane( PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_MARK );

    const gfl2::lyt::LytPaneIndex inputPaneTbl[] = {
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_00,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_01,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_02,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_03,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_04,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_05,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_06,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_07,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_08,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_09,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_10,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_11,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_12,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_13,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_14,
      PANENAME_NUMBER_INPUT_LOW_PANE_INPUT_15,
    };

    for( u32 i=0; i<GFL_NELEMS(inputPaneTbl); ++i )
    {
      m_pInputParts[i] = lytwk->GetPartsPane( inputPaneTbl[i] );
      m_pInputText[i]  = lytwk->GetTextBoxPane( m_pInputParts[i], PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT_00, &resID );
    }

    m_pCursorParts  = lytwk->GetPartsPane( PANENAME_NUMBER_INPUT_LOW_PANE_CURSOR_00 );
    m_pCursorWindow = lytwk->GetWindowPane( m_pCursorParts, PANENAME_STRINPUT_CUR_LOW_000_PANE_WINDOW_00, &resID );
  }
}

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

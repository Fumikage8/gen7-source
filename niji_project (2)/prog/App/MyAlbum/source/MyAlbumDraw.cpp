//=============================================================================
/**
 * @file    MyAlbumDraw.cpp
 * @brief   マイアルバム画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================

// module
#include "App/MyAlbum/include/MyAlbumAppParam.h"
#include "MyAlbumDraw.h"
#include "MyAlbumPhotoData.h"
// momiji
#include "GameSys/include/GameData.h"
#include "Sound/include/Sound.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "AppLib/include/Tool/app_tool_TimeIcon.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/Sort/StringMonsName.h"
#include "App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h"
// resource
#include "niji_conv_header/app/studio_album/MyAlbum.h"
#include "niji_conv_header/app/studio_album/MyAlbum_pane.h"
#include "niji_conv_header/app/studio_album/MyAlbum_anim_list.h"
#include "niji_conv_header/message/msg_fs_album.h"
#include "niji_conv_header/message/msg_fs_photo.h"
#include "niji_conv_header/message/msg_initial.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

const f32 MyAlbumDraw::TOP_LIST_PX = 2.0f;    //!< ソートトップリストの表示X座標
const f32 MyAlbumDraw::TOP_LIST_PY = 87.0f;   //!< ソートトップリストの表示Y座標
const f32 MyAlbumDraw::TOP_LIST_SX = 290.0f;  //!< ソートトップリストのXサイズ
const f32 MyAlbumDraw::TOP_LIST_SY = 30.0f;   //!< ソートトップリストのYサイズ

//! ソートトップリストの部品テーブル
const gfl2::lyt::LytPaneIndex MyAlbumDraw::ListTopPartsTable[MyAlbumDraw::TOP_LIST_PANE_MAX] =
{
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_00,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_01,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_02,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_03,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_04,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_05,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_06,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_07,
  PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BUTTON_08,
};

const f32 MyAlbumDraw::CORE_LIST_PX = 0.0f;    //!< ソート項目リストの表示X座標
const f32 MyAlbumDraw::CORE_LIST_PY = 87.0f;   //!< ソート項目リストの表示Y座標
const f32 MyAlbumDraw::CORE_LIST_SX = 264.0f;  //!< ソート項目リストのXサイズ
const f32 MyAlbumDraw::CORE_LIST_SY = 30.0f;   //!< ソート項目リストのYサイズ

//! ソート項目リストの部品テーブル
const gfl2::lyt::LytPaneIndex MyAlbumDraw::ListCorePartsTable[MyAlbumDraw::CORE_LIST_PANE_MAX] =
{
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_00,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_01,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_02,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_03,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_04,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_05,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_06,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_07,
  PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BUTTON_08,
};

//! 子音テーブル
const u32 MyAlbumDraw::Initial_Consonant[MyAlbumDraw::CORE_LIST_CONSONANT_MAX] =
{
  initial_00,   // ダミー
  initial_00,   // ア
  initial_05,   // カ
  initial_10,   // サ
  initial_15,   // タ
  initial_20,   // ナ
  initial_25,   // ハ
  initial_30,   // マ
  initial_35,   // ヤ
  initial_38,   // ラ
  initial_43,   // ワ
};

//! 母音データ
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_A[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_00, 1 },  // ア
  { initial_01, 2 },  // イ
  { initial_02, 3 },  // ウ
  { initial_03, 4 },  // エ
  { initial_04, 5 },  // オ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_KA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_05, 6 },  // カ
  { initial_06, 7 },  // キ
  { initial_07, 8 },  // ク
  { initial_08, 9 },  // ケ
  { initial_09, 10 }, // コ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_SA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_10, 11 },  // サ
  { initial_11, 12 },  // シ
  { initial_12, 13 },  // ス
  { initial_13, 14 },  // セ
  { initial_14, 15 },  // ソ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_TA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_15, 16 },  // タ
  { initial_16, 17 },  // チ
  { initial_17, 18 },  // ツ
  { initial_18, 19 },  // テ
  { initial_19, 20 },  // ト
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_NA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_20, 21 },  // ナ
  { initial_21, 22 },  // ニ
  { initial_22, 23 },  // ヌ
  { initial_23, 24 },  // ネ
  { initial_24, 25 },  // ノ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_HA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_25, 26 },  // ハ
  { initial_26, 27 },  // ヒ
  { initial_27, 28 },  // フ
  { initial_28, 29 },  // ヘ
  { initial_29, 30 },  // ホ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_MA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_30, 31 },  // マ
  { initial_31, 32 },  // ミ
  { initial_32, 33 },  // ム
  { initial_33, 34 },  // メ
  { initial_34, 35 },  // モ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_YA[MyAlbumDraw::VOWEL_DATA_YA_MAX] =
{
  { initial_35, 36 },  // ヤ
  { initial_36, 37 },  // ユ
  { initial_37, 38 },  // ヨ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_RA[MyAlbumDraw::VOWEL_DATA_MAX] =
{
  { initial_38, 39 },  // ラ
  { initial_39, 40 },  // リ
  { initial_40, 41 },  // ル
  { initial_41, 42 },  // レ
  { initial_42, 43 },  // ロ
};
const MyAlbumDraw::VOWEL_DATA MyAlbumDraw::VowelData_WA[MyAlbumDraw::VOWEL_DATA_WA_MAX] =
{
  { initial_43, 44 },  // ワ
};

//! 母音データテーブル
const MyAlbumDraw::VOWEL_DATA * MyAlbumDraw::VowelDataTable[MyAlbumDraw::CORE_LIST_CONSONANT_MAX] =
{
  MyAlbumDraw::VowelData_A,
  MyAlbumDraw::VowelData_KA,
  MyAlbumDraw::VowelData_SA,
  MyAlbumDraw::VowelData_TA,
  MyAlbumDraw::VowelData_NA,
  MyAlbumDraw::VowelData_HA,
  MyAlbumDraw::VowelData_MA,
  MyAlbumDraw::VowelData_YA,
  MyAlbumDraw::VowelData_RA,
  MyAlbumDraw::VowelData_WA,
};

//! 母音データテーブルサイズ
const u32 MyAlbumDraw::VowelDataTableLen[MyAlbumDraw::CORE_LIST_CONSONANT_MAX] =
{
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_YA_MAX,
  MyAlbumDraw::VOWEL_DATA_MAX,
  MyAlbumDraw::VOWEL_DATA_WA_MAX,
};

//! アルファベット頭文字テーブル
const u32 MyAlbumDraw::Initial_Alphabet[MyAlbumDraw::INITIAL_LIST_ALPHABET_MAX] =
{
  initial_00,   // ダミー
  initial_00,
  initial_01,
  initial_02,
  initial_03,
  initial_04,
  initial_05,
  initial_06,
  initial_07,
  initial_08,
  initial_09,
  initial_10,
  initial_11,
  initial_12,
  initial_13,
  initial_14,
  initial_15,
  initial_16,
  initial_17,
  initial_18,
  initial_19,
  initial_20,
  initial_21,
  initial_22,
  initial_23,
  initial_24,
  initial_25,
};

//! ハングル頭文字テーブル
const u32 MyAlbumDraw::Initial_Koria[MyAlbumDraw::INITIAL_LIST_KORIA_MAX] =
{
  initial_00,   // ダミー
  initial_00,
  initial_01,
  initial_02,
  initial_03,
  initial_04,
  initial_05,
  initial_06,
  initial_07,
  initial_08,
  initial_09,
  initial_10,
  initial_11,
  initial_12,
  initial_13,
};

//! 簡体字頭文字テーブル
const u32 MyAlbumDraw::Initial_China[MyAlbumDraw::INITIAL_LIST_CHINA_MAX] =
{
  initial_00,   // ダミー
  initial_00,
  initial_01,
  initial_02,
  initial_03,
  initial_04,
  initial_05,
  initial_06,
  initial_07,
  initial_08,
  initial_09,
  initial_10,
  initial_11,
  initial_12,
  initial_13,
  initial_14,
  initial_15,
  initial_16,
  initial_17,
  initial_18,
  initial_19,
  initial_20,
  initial_21,
  initial_22,
};

//! 繁体字頭文字テーブル
const u32 MyAlbumDraw::Initial_Taiwan[MyAlbumDraw::INITIAL_LIST_TAIWAN_MAX] =
{
  initial_00,   // ダミー
  initial_00,
  initial_01,
  initial_02,
  initial_03,
  initial_04,
  initial_05,
  initial_06,
  initial_07,
  initial_08,
  initial_09,
  initial_10,
  initial_11,
  initial_12,
  initial_13,
  initial_14,
  initial_15,
  initial_16,
  initial_17,
  initial_18,
  initial_19,
  initial_20,
  initial_21,
  initial_22,
  initial_23,
  initial_24,
  initial_25,
  initial_26,
  initial_27,
  initial_28,
  initial_29,
  initial_30,
  initial_31,
  initial_32,
  initial_33,
  initial_34,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param       外部設定パラメータ
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャ
 * @param   listener    リスナー
 */
//-----------------------------------------------------------------------------
MyAlbumDraw::MyAlbumDraw( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man, Listener * listener )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_appParam( param )
  , m_heap( heap )
  , m_renderingManager( render_man )
  , m_pMsgData( NULL )
  , m_pBgMsgData( NULL )
  , m_pInitialMsgData( NULL )
  , m_pWordSet( NULL )
  , m_menuCursor( NULL )
  , m_listener( listener )
  , m_dispMode( DISP_MODE_SELECT )
  , m_photoData( NULL )
  , m_cursorPos( 0 )
  , m_page( 0 )
  , m_pageMax( 0 )
  , m_photoDeleteCount( 0 )
  , m_listTop( NULL )
  , m_listCore( NULL )
  , m_pListTopData( NULL )
  , m_pListCoreData( NULL )
  , m_listType( LIST_TYPE_TOP )
  , m_vowelData( NULL )
  , m_vowelDataLen( 0 )
  , m_sort( NULL )
  , m_sortInitialMax( 0 )
  , m_langInitialTable( NULL )
  , m_langInitialMax( 0 )
  , m_subSeq( 0 )
  , m_isDrawEnable( true )
{
  SetLangInitial();
  InitButtonView();
  InitPokemonSortData();
  InitPaneListPos();
  InitDeletePhotoBuffer();
  InitPhotoLoadFlag();
  SetInputEnabled( false );
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
MyAlbumDraw::~MyAlbumDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::GetGameData(void)
{
/*
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::Update(void)
{
  app::ui::UIView::Update();
  UpdateMenuCursor();
  if( m_isDrawEnable != false )
  {
    UpdateThumbnail();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2017.01.25
 *
 * @param   displayNo   描画面
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    app::util::G2DUtil * g2d = GetG2DUtil();
    for( s32 i=0; i<LYTID_MAX; i++ )
    {
      g2d->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, i );
    }
    DrawMenuCursor( displayNo );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2017.01.25
 *
 * @param   res_buf         レイアウトリソースバッファ
 * @param   msg_buf         メッセージリソースバッファ
 * @param   msg_bg_buf      背景リスト項目用メッセージリソースバッファ
 * @param   msg_initial_buf イニシャル用メッセージリソースバッファ
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::Init( void * res_buf, void * msg_buf, void * msg_bg_buf, void * msg_initial_buf )
{
  switch( m_subSeq )
  {
  case 0:
    CreateNumFont();
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( SetNumFont() == false )
    {
      break;
    }
    Initialize2D( res_buf, msg_buf, msg_bg_buf, msg_initial_buf );
    SetInputListener( this );
    m_subSeq = 0;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   初期データセットアップ
 * @date    2017.02.16
 *
 * @param   photo_data  写真データ
 * @param   cursor      メニューカーソル
 * @param   pos         カーソル位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::Setup( MyAlbumPhotoData * photo_data, app::tool::MenuCursor * cursor, u32 pos )
{
  m_photoData = photo_data;
  CreatePageData();
  InitCursorPos( pos );
  CreateButton();
  CreateMenuCursorLayout( cursor );
  if( m_pageMax == 0 )
  {
    SetPhotoNone();
  }
  else
  {
    PutPhotoMax();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2017.01.25
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  Terminate2D();
  DeleteNumFont();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateNumFont
 * @brief   NUMフォント生成
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateNumFont(void)
{
  print::SystemFont_LoadNumFont( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap() );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNumFont
 * @brief   NUMフォントをセット
 * @date    2017.02.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::SetNumFont(void)
{
  if( print::SystemFont_IsLoadNumFont() == false )
  {
    return false;
  }
  print::SystemFont_SetLoadNumFont( m_heap->GetSystemHeap() );
  print::SystemFont_SetLytSystemNumFont();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteNumFont
 * @brief   NUMフォント削除
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::DeleteNumFont(void)
{
  print::SystemFont_ResetNumFontInfo();
  print::SystemFont_DeleteNumFont();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2017.01.25
 *
 * @param   res_buf         レイアウトリソースバッファ
 * @param   msg_buf         メッセージリソースバッファ
 * @param   msg_bg_buf      背景リスト項目用メッセージリソースバッファ
 * @param   msg_initial_buf イニシャル用メッセージリソースバッファ
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::Initialize2D( void * res_buf, void * msg_buf, void * msg_bg_buf, void * msg_initial_buf )
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
      LYTRES_MYALBUM_STUDIO_ALBUM_UP_00_BFLYT,
      LA_MYALBUM_STUDIO_ALBUM_UP_00___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_UPPER,
      true
    },
    {
      0,
      LYTRES_MYALBUM_STUDIO_ALBUM_LOW_BG_00_BFLYT,
      LA_MYALBUM_STUDIO_ALBUM_LOW_BG_00___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
    {
      0,
      LYTRES_MYALBUM_STUDIO_ALBUM_LOW_00_BFLYT,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
    {
      0,
      LYTRES_MYALBUM_STUDIO_ALBUM_LIST_LOW_01_BFLYT,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
    {
      0,
      LYTRES_MYALBUM_STUDIO_ALBUM_LIST_LOW_00_BFLYT,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00___NUM,
      res_buf,
      false,
      app::util::G2DUtil::SETUP_LOWER,
      false
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  m_pBgMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_bg_buf, m_heap->GetDeviceHeap() );
  m_pInitialMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_initial_buf, m_heap->GetDeviceHeap() );

//  SetWordSetLoader( wset_loader );

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

  WriteDefaultString();
  StartTimerIconAnime();

  // 背景アニメ
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StartAnime( LYTID_UPPER_BG, LA_MYALBUM_STUDIO_ALBUM_UP_00_BG_LOOP_01 );
  g2d->StartAnime( LYTID_LOWER_BG, LA_MYALBUM_STUDIO_ALBUM_LOW_BG_00__BG01_BG_LOOP_01 );
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::Terminate2D(void)
{
  Delete2D();
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
  GFL_SAFE_DELETE( m_pBgMsgData );
  GFL_SAFE_DELETE( m_pInitialMsgData );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMsgData
 * @brief   メッセージデータ取得
 * @date    2017.01.27
 *
 * @return  メッセージデータ
 */
//-----------------------------------------------------------------------------
gfl2::str::MsgData * MyAlbumDraw::GetMsgData(void)
{
  return m_pMsgData;
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::Start(void)
{
  ChangeDispMode( DISP_MODE_SELECT );
//  PutCursor( 0 );
  ChangePage( 0, false );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsStart
 * @brief   開始処理が終了したかをチェック
 * @date    2017.01.25
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsStart(void)
{
  SetInputEnabled( true );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultSrring
 * @brief   デフォルト文字列描画
 * @date    2017.01.25
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteDefaultString(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  {
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_PHOTO );
    // けす
    SetTextboxPaneMessage(
      lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_STUDIO_ALBUM_LOW_00_PANE_DELETEBUTTON), PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &res_id ),
      fs_album_cap_05 );
    // "/"
    SetTextboxPaneMessage( lytwk->GetTextBoxPane(PANENAME_STUDIO_ALBUM_LOW_00_PANE_PNO_CENTER), fs_album_cap_04 );
  }

  {
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_SEARCH_TYPE );
    // リセット
    SetTextboxPaneMessage(
      lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_RESETBTN), PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &res_id ),
      fs_album_cap_32 );
    // さがす
    SetTextboxPaneMessage(
      lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_SEARCHBTN), PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &res_id ),
      fs_album_cap_52 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPhotoMax
 * @brief   写真枚数表示
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::PutPhotoMax(void)
{
  GetG2DUtil()->SetTextBoxPaneNumber( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_PNO_01, fs_album_cap_03, m_photoData->GetDataCount(), 3 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPhotoNumber
 * @brief   カーソル位置の写真番号を表示
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::PutPhotoNumber(void)
{
  GetG2DUtil()->SetTextBoxPaneNumber( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_PNO_00, fs_album_cap_03, GetCursorPosIndex()+1, 3 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoNumber
 * @brief   写真番号表示切り替え
 * @date    2017.02.06
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoNumber( bool flg )
{
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_PHOTONO, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoTitle
 * @brief   タイトル表示切り替え
 * @date    2017.02.06
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoTitle( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->SetPaneVisible( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_HEADER_TEXT, flg );
  g2d->SetPaneVisible( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_DAY, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2017.01.27
 *
 * @param   menu_cursor   メニューカーソルクラス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateMenuCursorLayout( app::tool::MenuCursor * menu_cursor )
{
  m_menuCursor = menu_cursor;

  app::util::G2DUtil * g2d = GetG2DUtil();

  m_menuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
//  m_menuCursor->SetVisible( true );
  m_menuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2017.01.27
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::DeleteMenuCursorLayout(void)
{
  return m_menuCursor->DeleteLayoutWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMenuCursor
 * @brief   メニューカーソル更新
 * @date    2017.01.27
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::UpdateMenuCursor(void)
{
  if( m_menuCursor != NULL )
  {
    m_menuCursor->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawMenuCursor
 * @brief   メニューカーソル描画
 * @date    2017.01.27
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::DrawMenuCursor( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_menuCursor != NULL )
  {
    m_menuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitButtonView
 * @brief   ボタン表示管理初期化
 * @date    2017.02.28
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitButtonView(void)
{
  static const ButtonID tbl[] =
  {
    BTNID_FAVORITE,   // お気に入り
    BTNID_SHARE,      // 共有
    BTNID_SEARCH,     // 検索
    BTNID_DELETE,     // 削除
  };

  GFL_ASSERT_STOP( BUTTON_VIEW_MAX == GFL_NELEMS(tbl) );

  for( u32 i=0; i<BUTTON_VIEW_MAX; i++ )
  {
    m_buttonView[i].id = tbl[i];
    m_buttonView[i].is_view_active = true;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsButtonViewActive
 * @brief   ボタン表示がアクティブか
 * @date    2017.02.28
 *
 * @param   id  ボタンID
 *
 * @retval  true  = アクティブ
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsButtonViewActive( ButtonID id )
{
  for( u32 i=0; i<BUTTON_VIEW_MAX; i++ )
  {
    if( m_buttonView[i].id == id )
    {
      return m_buttonView[i].is_view_active;
    }
  }
  GFL_ASSERT( 0 );  // 管理外
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetButtonViewActive
 * @brief   ボタン表示状態を設定
 * @date    2017.02.28
 *
 * @param   id    ボタンID
 * @param   flg   true = アクティブ
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetButtonViewActive( ButtonID id, bool flg )
{
  for( u32 i=0; i<BUTTON_VIEW_MAX; i++ )
  {
    if( m_buttonView[i].id == id )
    {
      m_buttonView[i].is_view_active = flg;
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2017.01.26
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateButton(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoLytWk * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoLytWk[BTNID_MAX];
  CreateBaseButton( &info[BTNID_SEL_PHOTO_00] );
  CreateSearchTypeButton( &info[BTNID_SEARCH_TYPE_RETURN] );
  CreateSearchCoreButton( &info[BTNID_SEARCH_CORE_RETURN] );
  CreateButtonManager( m_heap, info, BTNID_MAX );
  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man = GetButtonManager();
  // キーバインド
  man->SetButtonBindKey( BTNID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_SEARCH, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_DELETE, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_PAGE_LEFT, gfl2::ui::BUTTON_L, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_PAGE_RIGHT, gfl2::ui::BUTTON_R, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_DEL_MULTI, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  man->SetButtonBindKey( BTNID_SEARCH_TYPE_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_SEARCH_TYPE_SEARCH, gfl2::ui::BUTTON_Y, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonBindKey( BTNID_SEARCH_TYPE_RESET, gfl2::ui::BUTTON_X, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  man->SetButtonBindKey( BTNID_SEARCH_CORE_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );

  // SE
  { // 決定
    static const ButtonID tbl[] = 
    {
      BTNID_SEL_PHOTO_00,
      BTNID_SEL_PHOTO_01,
      BTNID_SEL_PHOTO_02,
      BTNID_SEL_PHOTO_03,
      BTNID_SEL_PHOTO_04,
      BTNID_SEL_PHOTO_05,
      BTNID_FAVORITE,
      BTNID_SHARE,
      BTNID_INFO,
      BTNID_SEARCH,
      BTNID_DELETE,
      BTNID_DELETE,
      BTNID_DEL_MULTI,
      BTNID_SEARCH_TYPE_RESET,
      BTNID_SEARCH_TYPE_SEARCH,
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonSelectSE( tbl[i], SEQ_SE_DECIDE1 );
    }
  }
  { // 移動
    static const ButtonID tbl[] = 
    {
      BTNID_MV_PHOTO_00,
      BTNID_MV_PHOTO_01,
      BTNID_MV_PHOTO_02,
      BTNID_MV_PHOTO_03,
      BTNID_MV_PHOTO_04,
      BTNID_MV_PHOTO_05,
      BTNID_PAGE_LEFT,
      BTNID_PAGE_RIGHT,
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonSelectSE( tbl[i], SEQ_SE_SELECT1 );
    }
  }
/*
  { // ページ
    static const ButtonID tbl[] = 
    {
      BTNID_PAGE_LEFT,
      BTNID_PAGE_RIGHT,
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonSelectSE( tbl[i], SEQ_SE_PAGE3 );
    }
  }
*/
  { // キャンセル
    static const ButtonID tbl[] = 
    {
      BTNID_RETURN,
      BTNID_SEARCH_TYPE_RETURN,
      BTNID_SEARCH_CORE_RETURN,
    };
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonSelectSE( tbl[i], SEQ_SE_CANCEL1 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetButtonInfoEx
 * @brief   ボタン情報生成
 * @date    2017.01.26
 *
 * @param   info        生成場所
 * @param   info_tbl    デフォルト情報テーブル
 * @param   parts_tbl   部品ペインテーブル
 * @param   bound_tbl   境界ペインテーブル
 * @param   max         生成数
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetButtonInfoEx(
      u32 lyt_id,
      app::ui::ButtonInfoLytWk * info,
      const app::ui::ButtonInfoLytWk * info_tbl, 
      const gfl2::lyt::LytPaneIndex * parts_tbl,
      const gfl2::lyt::LytPaneIndex * bound_tbl,
      u32 max )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( lyt_id );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<max; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = info_tbl[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, bound_tbl[i], &res_id );
    info[i].lyt_wk       = lytwk;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateBaseButton
 * @brief   写真選択画面のボタン生成
 * @date    2017.01.26
 *
 * @param   info  生成場所
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateBaseButton( app::ui::ButtonInfoLytWk * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    // 写真（決定用）
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_05,
    // 写真（カーソル移動用）
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_05,
    // 矢印
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PAGECURSOR_L,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PAGECURSOR_R,
    // 上部
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_BTN_HEART,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_BTN_UP,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_INFOMATION,
    // 下部
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_BACKBUTTON,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_SEARCHBUTTON,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_DUSTBUTTON,
    // 消す（複数モード）
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_DELETEBUTTON,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    // 写真（決定用）
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    // 写真（カーソル移動用）
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_BOUND_00,
    // 矢印
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_CUR_LOW_002_PANE_BOUND_00,
    // 上部
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00,
    // 下部
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_BOX_BTN_LOW_003_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_022_PANE_BOUND_00,
    // 消す（複数モード）
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    { // 写真（決定用）
      BTNID_SEL_PHOTO_00, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_PASSIVE,
    },
    {
      BTNID_SEL_PHOTO_01, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_PASSIVE,
    },
    {
      BTNID_SEL_PHOTO_02, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_PASSIVE,
    },
    {
      BTNID_SEL_PHOTO_03, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_PASSIVE,
    },
    {
      BTNID_SEL_PHOTO_04, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_PASSIVE,
    },
    {
      BTNID_SEL_PHOTO_05, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_PASSIVE,
    },
    { // 写真（カーソル移動用）
      BTNID_MV_PHOTO_00, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTNID_MV_PHOTO_01, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTNID_MV_PHOTO_02, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTNID_MV_PHOTO_03, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTNID_MV_PHOTO_04, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    {
      BTNID_MV_PHOTO_05, NULL, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // ページ切り替え・左
      BTNID_PAGE_LEFT, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_L_PASSIVE,
    },
    { // ページ切り替え・右
      BTNID_PAGE_RIGHT, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__PAGECURSOR_R_PASSIVE,
    },
    { // お気に入り
      BTNID_FAVORITE, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_HEART_PASSIVE,
    },
    { // 共有
      BTNID_SHARE, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BTN_UP_PASSIVE,
    },
    { // InfoWindow起動
      BTNID_INFO, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__INFOMATION_PASSIVE,
    },
    { // 戻る
      BTNID_RETURN, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__BACKBUTTON_PASSIVE,
    },
    { // 検索
      BTNID_SEARCH, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_PASSIVE,
    },
    { // 削除
      BTNID_DELETE, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DUSTBUTTON_PASSIVE,
    },
    { // 消す（複数モード）
      BTNID_DEL_MULTI, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LOW_00__DELETEBUTTON_PASSIVE,
    },
  };

  SetButtonInfoEx( LYTID_LOWER_PHOTO, info, info_tbl, parts_tbl, bound_tbl, GFL_NELEMS(info_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateSearchTypeButton
 * @brief   検索画面のボタン生成
 * @date    2017.01.26
 *
 * @param   info  生成場所
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateSearchTypeButton( app::ui::ButtonInfoLytWk * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_BACKBTN,
    PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_RESETBTN,
    PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_SEARCHBTN,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
    PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BTNID_SEARCH_TYPE_RETURN, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BACKBTN_PASSIVE,
    },
    {
      BTNID_SEARCH_TYPE_RESET, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__RESETBTN_PASSIVE,
    },
    {
      BTNID_SEARCH_TYPE_SEARCH, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__SEARCHBTN_PASSIVE,
    },
  };

  SetButtonInfoEx( LYTID_LOWER_SEARCH_TYPE, info, info_tbl, parts_tbl, bound_tbl, GFL_NELEMS(info_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateSearchCoreButton
 * @brief   検索条件画面のボタン生成
 * @date    2017.01.26
 *
 * @param   info  生成場所
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreateSearchCoreButton( app::ui::ButtonInfoLytWk * info )
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_BACKBTN,
  };

  static const gfl2::lyt::LytPaneIndex bound_tbl[] =
  {
    PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00,
  };

  static const app::ui::ButtonInfoLytWk info_tbl[] =
  {
    {
      BTNID_SEARCH_CORE_RETURN, NULL, NULL, NULL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_TOUCH_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_ACTIVE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BACKBTN_PASSIVE,
    },
  };

  SetButtonInfoEx( LYTID_LOWER_SEARCH_CORE, info, info_tbl, parts_tbl, bound_tbl, GFL_NELEMS(info_tbl) );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDispMode
 * @brief   画面モード取得
 * @date    2017.01.31
 *
 * @return  画面モード
 */
//-----------------------------------------------------------------------------
MyAlbumDraw::DispMode MyAlbumDraw::GetDispMode(void)
{
  return m_dispMode;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeDispMode
 * @brief   画面モード切り替え
 * @date    2017.01.26
 *
 * @param   mode  画面モード
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::ChangeDispMode( DispMode mode )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  app::tool::ButtonManager * man = GetButtonManager();

  m_dispMode = mode;

  // レイアウト非表示
  for( u32 i=LYTID_LOWER_PHOTO; i<=LYTID_LOWER_SEARCH_CORE; i++ )
  {
    g2d->SetLayoutDrawEnable( i, false );
  }

  // 全ボタンパッシブ
  for( u32 i=0; i<BTNID_MAX; i++ )
  {
    man->SetButtonPassive( i, false );
  }

  // 上画面リスト説明ウィンドウOFF
  SetVisibleListInfoWindow( false );

  // 個別
  if( mode == DISP_MODE_SELECT || mode == DISP_MODE_DELETE )
  {
    g2d->SetLayoutDrawEnable( LYTID_LOWER_PHOTO, true );
    if( mode == DISP_MODE_SELECT )
    {
      {
        static const ButtonID tbl[] =
        {
          BTNID_PAGE_LEFT,
          BTNID_PAGE_RIGHT,
          BTNID_INFO,
          BTNID_RETURN,
        };
        for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
        {
          man->SetButtonActive( tbl[i], false );
        }
      }
      {
        static const ButtonID tbl[] =
        {
          BTNID_FAVORITE,
          BTNID_SHARE,
          BTNID_DELETE,
        };
        if( m_pageMax == 0 )
        {
          for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
          {
            if( IsButtonViewActive(tbl[i]) != false )
            {
              man->SetButtonActive( tbl[i], false );  // @note データなしの場合でも一旦アクティブにする（パッシブアニメが動かないため）
              man->SetButtonPassive( tbl[i], true );
              SetButtonViewActive( tbl[i], false );
            }
          }
        }
        else
        {
          for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
          {
            if( IsButtonViewActive(tbl[i]) == false )
            {
              man->SetButtonActiveAnimeFrameLast( tbl[i] );  // @fix MMCat[654]: 最終フレームからアニメを再生
              SetButtonViewActive( tbl[i], true );
            }
            else
            {
              man->SetButtonActive( tbl[i], false );
            }
          }
        }
      }
      // 検索ボタン
      if( m_pageMax == 0 && m_appParam->is_sort_mode == false )
      {
        if( IsButtonViewActive(BTNID_SEARCH) != false )
        {
          man->SetButtonActive( BTNID_SEARCH, false );  // @note データなしの場合でも一旦アクティブにする（パッシブアニメが動かないため）
          man->SetButtonPassive( BTNID_SEARCH, true );
          SetButtonViewActive( BTNID_SEARCH, false );
        }
      }
      else
      {
        if( IsButtonViewActive(BTNID_SEARCH) == false )
        {
          man->SetButtonActiveAnimeFrameLast( BTNID_SEARCH );  // @fix MMCat[654]: 最終フレームからアニメを再生
          SetButtonViewActive( BTNID_SEARCH, true );
        }
        else
        {
          man->SetButtonActive( BTNID_SEARCH, false );
        }
      }
      if( m_appParam->is_sort_mode == false )
      {
        g2d->StopAnime( LYTID_LOWER_PHOTO, LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_SEARCH_ON );
        g2d->StartAnime( LYTID_LOWER_PHOTO, LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_SEARCH_OFF );
      }
      else
      {
        g2d->StopAnime( LYTID_LOWER_PHOTO, LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_SEARCH_OFF );
        g2d->StartAnime( LYTID_LOWER_PHOTO, LA_MYALBUM_STUDIO_ALBUM_LOW_00__SEARCHBUTTON_SEARCH_ON );
      }

      man->SetButtonVisible( BTNID_SEARCH );
      man->SetButtonVisible( BTNID_DELETE );
      man->SetButtonInvisible( BTNID_DEL_MULTI );
    }
    else
    {
      for( u32 i=BTNID_PAGE_LEFT; i<=BTNID_RETURN; i++ )
      {
        man->SetButtonActive( i, false );
      }
      man->SetButtonActive( BTNID_DEL_MULTI, false );
      man->SetButtonInvisible( BTNID_SEARCH );
      man->SetButtonInvisible( BTNID_DELETE );
      man->SetButtonVisible( BTNID_DEL_MULTI );
      g2d->SetPaneVisible( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_DELETEBUTTON, true );  // エディタ上で非表示になっているため
    }
    for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
    {
      if( m_cursorPos == i )
      {
        man->SetButtonActive( BTNID_SEL_PHOTO_00+i, false );
      }
      else
      {
        man->SetButtonActive( BTNID_MV_PHOTO_00+i, false );
      }
    }
    SetVisiblePhoto();
    m_menuCursor->SetVisible( PutCursor(m_cursorPos) );
  }
  else if( mode == DISP_MODE_SEARCH_TYPE )
  {
    g2d->SetLayoutDrawEnable( LYTID_LOWER_SEARCH_TYPE, true );
    for( u32 i=BTNID_SEARCH_TYPE_RETURN; i<=BTNID_SEARCH_TYPE_SEARCH; i++ )
    {
      man->SetButtonActive( i, false );
    }
    SetVisibleListInfoWindow( true );
  }
  else if( mode == DISP_MODE_SEARCH_CORE )
  {
    g2d->SetLayoutDrawEnable( LYTID_LOWER_SEARCH_CORE, true );
    man->SetButtonActive( BTNID_SEARCH_CORE_RETURN, false );
    SetVisibleListInfoWindow( true );
  }
  else if( mode == DISP_MODE_ERROR )
  {
    m_menuCursor->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePhotoData
 * @brief   写真データ削除
 * @date    2017.02.15
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumDraw::DeletePhotoData(void)
{
  switch( m_subSeq )
  {
  case 0:
    {
      u32 max = m_photoData->GetDataCount();
      for( u32 i=m_photoDeleteCount; i<max; i++ )
      {
        if( IsDeletePhotoBit( i ) != false )
        {
          break;
        }
        m_photoDeleteCount++;
      }
      if( m_photoDeleteCount >= max )
      {
        InitDeletePhotoBuffer();
        m_photoData->InitDataRow( &m_appParam->sort_data );
        CreatePageData();
        ChangePage( 0, false );
        PutPhotoMax();
        if( PutCursor( m_cursorPos ) == false )
        {
          s32 pos = GetEnableDataPos( m_page*PHOTO_PUT_MAX+m_cursorPos );
          if( pos != -1 )
          {
            PutCursor( pos % PHOTO_PUT_MAX );
          }
          else
          {
            SetPhotoNone();
          }
        }
        m_listener->OnAction( Listener::ACTION_ID_CURSOR_MOVE );
        m_photoDeleteCount = 0;
        m_subSeq = 0;
        if( m_photoData->IsDataBroken() != false )
        {
          return MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN;
        }
        return MyAlbumPhotoData::SDC_ACCESS_RESULT_OK;
      }
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    {
      MyAlbumPhotoData::SDCardAccessResult result = m_photoData->DeleteData( m_photoDeleteCount );
      if( result == MyAlbumPhotoData::SDC_ACCESS_RESULT_OK || result == MyAlbumPhotoData::SDC_ACCESS_RESULT_NG_BROKEN )
      {
        m_photoDeleteCount++;
        m_subSeq = 0;
      }
      else
      {
        return result;
      }
    }
    break;
  }

  return MyAlbumPhotoData::SDC_ACCESS_RESULT_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitDeletePhotoBuffer
 * @brief   写真データ削除ビット初期化
 * @date    2017.01.30
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitDeletePhotoBuffer(void)
{
  for( u32 i=0; i<PHOTO_DELTE_BUF_SIZE; i++ )
  {
    m_photoDeleteBit[i] = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDeletePhotoBit
 * @brief   写真データ削除ビット設定
 * @date    2017.01.30
 *
 * @param   index   写真のインデックス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetDeletePhotoBit( u32 index )
{
  m_photoDeleteBit[index/8] ^= (1<<(index%8));
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDeletePhotoBit
 * @brief   写真データ削除ビットチェック
 * @date    2017.01.30
 *
 * @param   index   写真のインデックス
 *
 * @retval  true  = ON
 * @retval  false = OFF
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsDeletePhotoBit( u32 index )
{
  if( ( m_photoDeleteBit[index/8] & (1<<(index%8)) ) != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckSelectDeletePhoto
 * @brief   写真が削除できるか
 * @date    2017.01.31
 *
 * @return  結果
 */
//-----------------------------------------------------------------------------
MyAlbumDraw::DeleteResult MyAlbumDraw::CheckSelectDeletePhoto(void)
{
  bool is_select = false;

  for( u32 i=0; i<m_photoData->GetDataCount(); i++ )
  {
    if( IsDeletePhotoBit(i) != false )
    {
      is_select = true;
      if( m_photoData->IsFavorite(i) != false )
      {
        return DEL_RESULT_NG_FAVORITE;
      }
      if( m_photoData->IsShare(i) != false )
      {
        return DEL_RESULT_NG_SHARE;
      }
    }
  }

  if( is_select == false )
  {
    return DEL_RESULT_NG_SELECT;
  }

  return DEL_RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePageData
 * @brief   ページ関連のデータを生成
 * @date    2017.01.26
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CreatePageData(void)
{
  u32 max = m_photoData->GetDataCount();

  m_pageMax = max / PHOTO_PUT_MAX;
  if( ( max % PHOTO_PUT_MAX ) != 0 )
  {
    m_pageMax++;
  }

  if( m_page != 0 && m_page >= m_pageMax )
  {
    if( m_pageMax == 0 )
    {
      m_page = 0;
    }
    else
    {
      m_page = m_pageMax - 1;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitCursorPos
 * @brief   初期カーソル位置を設定
 * @date    2017.02.15
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitCursorPos( u32 pos )
{
  if( pos < m_photoData->GetDataCount() )
  {
    u32 page = pos / PHOTO_PUT_MAX;
    if( page < m_pageMax )
    {
      m_page = page;
      m_cursorPos = pos % PHOTO_PUT_MAX;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursorPos
 * @brief   カーソル位置を取得
 * @date    2017.01.31
 *
 * @return  カーソル位置
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetCursorPos(void)
{
  return m_cursorPos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetCursorPosIndex
 * @brief   カーソル位置の写真のインデックスを取得
 * @date    2017.01.30
 *
 * @return  カーソル位置のインデックス
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetCursorPosIndex(void)
{
  return ( m_page * PHOTO_PUT_MAX + m_cursorPos );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutCursor
 * @brief   カーソル配置
 * @date    2017.01.26
 *
 * @param   pos   配置位置
 *
 * @retval  true  = 指定位置にデータあり
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::PutCursor( u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  u32 index = m_page * PHOTO_PUT_MAX + pos;

  if( index >= m_photoData->GetDataCount() )
  {
    return false;
  }

  { // タイトル
    g2d->SetRegisterMonsNameNo( 0, m_photoData->GetMonsNo(index) );
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_ALBUM_NO, fs_album_cap_01 );
    SetVisiblePhotoTitle( true );
  }

  { // 日付
    u32 y=0, m=0, d=0;
    m_photoData->GetDate( index, &y, &m, &d );
    g2d->SetRegisterNumber( 0, y, 4, print::NUM_DISP_ZERO );
    g2d->SetRegisterNumber( 1, m, 2, print::NUM_DISP_ZERO );
    g2d->SetRegisterNumber( 2, d, 2, print::NUM_DISP_ZERO );
    g2d->SetTextBoxPaneStringExpand( LYTID_LOWER_PHOTO, PANENAME_STUDIO_ALBUM_LOW_00_PANE_ALBUM_DAY, fs_album_cap_02 );
  }

  { // カーソル配置
    static const gfl2::lyt::LytPaneIndex tbl[] =
    {
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_00,
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_01,
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_02,
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_03,
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_04,
      PANENAME_STUDIO_ALBUM_LOW_00_PANE_NULL_CUR_05,
    };
    gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_PHOTO );
    m_menuCursor->Put( lytwk, lytwk->GetPane(tbl[pos]) );
  }

  { // ボタン切り替え
    app::tool::ButtonManager * man = GetButtonManager();
    // 移動前のカーソル位置
    man->SetButtonPassive( BTNID_SEL_PHOTO_00+m_cursorPos, false );
    man->SetButtonActive( BTNID_MV_PHOTO_00+m_cursorPos, false );
    // 移動後のカーソル位置
    man->SetButtonPassive( BTNID_MV_PHOTO_00+pos, false );
    man->SetButtonActive( BTNID_SEL_PHOTO_00+pos, false );
  }

  m_cursorPos = pos;

  PutPhotoNumber();
  SetVisiblePhotoNumber( true );

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveCursor
 * @brief   カーソル移動
 * @date    2017.01.26
 *
 * @param   mx  X移動方向
 * @param   my  Y移動方向
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::MoveCursor( s32 mx, s32 my )
{
  s32 px = m_cursorPos % PHOTO_PUT_WIDTH;
  s32 py = m_cursorPos / PHOTO_PUT_WIDTH;

  if( mx < 0 )
  {
    if( px == 0 )
    {
      if( ChangePage( -1, false ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return;
    }
  }
  else if( mx > 0 )
  {
    // @fix MMcat[341]: 右に写真がない場合のページ切り替え
    u32 count = m_photoData->GetDataCount();
    if( px == (PHOTO_PUT_WIDTH-1) ||
        ( count != 0 && GetCursorPosIndex() == (count-1) ) )
    {
      if( ChangePage( 1, false ) != false )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
      return;
    }
  }

  px += mx;
  py += my;

  if( px < 0 )
  {
    px = 0;
  }
  else if( px >= PHOTO_PUT_WIDTH )
  {
    px = PHOTO_PUT_WIDTH - 1;
  }

  if( py < 0 )
  {
    py = 0;
  }
  else if( py >= PHOTO_PUT_HEIGHT )
  {
    py = PHOTO_PUT_HEIGHT - 1;
  }

  u32 mv_pos = py * PHOTO_PUT_WIDTH + px;

  if( mv_pos == m_cursorPos )
  {
    return;
  }

  if( PutCursor( mv_pos ) != false )
  {
    m_listener->OnAction( Listener::ACTION_ID_CURSOR_MOVE );
    Sound::PlaySE( SEQ_SE_SELECT1 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    指定位置より前の存在するデータ位置を取得
 * @brief   カーソル移動
 * @date    2017.01.27
 *
 * @param   pos   検索開始位置
 *
 * @retval  -1  = データなし
 * @retval  -1 != データ位置
 */
//-----------------------------------------------------------------------------
s32 MyAlbumDraw::GetEnableDataPos( u32 pos )
{
  u32 max = m_photoData->GetDataCount();

  for( s32 i=static_cast<s32>(pos); i>=0; i-- )
  {
    if( i < max )
    {
      return i;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePage
 * @brief   ページ切り替え
 * @date    2017.01.26
 *
 * @param   mv        切り替え方向
 * @param   is_touch  true = タッチ操作
 *
 * @retval  true  = 切り替えた
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::ChangePage( s32 mv, bool is_touch )
{
  // 初期設定
  if( mv == 0 )
  {
    SetVisiblePageArrow();
    SetVisiblePhoto();
    SetVisiblePhotoMark();
    return false;
  }

  s32 page = m_page + mv;
  s32 pos  = m_cursorPos;

  if( m_pageMax == 0 )
  {
    page = 0;
  }
  else if( page < 0 )
  {
    page = m_pageMax - 1;
  }
  else if( page >= m_pageMax )
  {
    page = 0;
  }
  
  if( page == m_page )
  {
    return false;
  }

  m_page = page;
    
  if( is_touch == false )
  {
    if( mv < 0 )
    {
      pos += (PHOTO_PUT_WIDTH-1);
    }
    else
    {
      // @fix MMCat[341]: カーソル位置を左に固定
      pos = pos / PHOTO_PUT_WIDTH * PHOTO_PUT_WIDTH;
      if( (m_page*PHOTO_PUT_MAX+pos) >= m_photoData->GetDataCount() )
      {
        pos = 0;
      }
    }
  }

  if( PutCursor( pos ) == false )
  {
    pos = GetEnableDataPos( m_page*PHOTO_PUT_MAX+pos );
    if( pos != -1 )
    {
      PutCursor( pos % PHOTO_PUT_MAX );
    }
  }
  SetVisiblePageArrow();
  SetVisiblePhoto();
  SetVisiblePhotoMark();
  m_listener->OnAction( Listener::ACTION_ID_CURSOR_MOVE );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePageArrow
 * @brief   ページ矢印表示切り替え
 * @date    2017.01.26
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePageArrow(void)
{
  app::tool::ButtonManager * man = GetButtonManager();

  if( m_pageMax > 1 )
  {
    man->SetButtonVisible( BTNID_PAGE_LEFT );
    man->SetButtonVisible( BTNID_PAGE_RIGHT );
  }
  else
  {
    man->SetButtonInvisible( BTNID_PAGE_LEFT );
    man->SetButtonInvisible( BTNID_PAGE_RIGHT );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhoto
 * @brief   写真ペイン表示切り替え
 * @date    2017.01.27
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhoto(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  app::tool::ButtonManager * man = GetButtonManager();

  u32 max = m_photoData->GetDataCount();
  u32 index = m_page * PHOTO_PUT_MAX;

  for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
  {
    if( index >= max )
    {
      man->SetButtonInvisible( BTNID_SEL_PHOTO_00+i );
      man->SetButtonInvisible( BTNID_MV_PHOTO_00+i );
      SetVisiblePhotoSelectMark( i, false );
    }
    else
    {
      man->SetButtonVisible( BTNID_SEL_PHOTO_00+i );
      man->SetButtonVisible( BTNID_MV_PHOTO_00+i );
      SetVisiblePhotoSelectMark( i, IsDeletePhotoBit(index) );

      LoadThumbnail( index, i );
    }
    index++;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetThumnailPane
 * @brief   サムネイルのペインを取得
 * @date    2017.02.10
 *
 * @param   index   インデックス
 *
 * @return  ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPicture * MyAlbumDraw::GetThumnailPane( u32 index )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_PARTS_05,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_PHOTO );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  return lytwk->GetPicturePane( lytwk->GetPartsPane(tbl[index]), PANENAME_STUDIOPARTS_BUTTON_09_2_PANE_PHOTOSAMNE, &res_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetThumbnail
 * @brief   写真ペインにサムネイルをセット
 * @date    2017.02.10
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetThumbnail( u32 index )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::lyt::LytPicture * picture = GetThumnailPane( index );

  // 仮想アドレスから物理アドレスへ変換
  uptr addr = nn::gx::GetPhysicalAddr( reinterpret_cast<uptr>(m_photoData->GetThumbnailBuffer(index)) );

  // テクスチャ生成
  static const nw::lyt::TexSize tex_size( THUMBNAIL_TEX_WIDTH, THUMBNAIL_TEX_HEIGHT );
  static const nw::lyt::TexSize real_size( THUMBNAIL_BUF_WIDTH, THUMBNAIL_BUF_HEIGHT );
  gfl2::lyt::LytResTextureInfo tex_info;
  tex_info.Set( 0, addr, tex_size, real_size, nw::lyt::TEXFORMAT_RGBA8 );

  // テクスチャ差し替え
  gfl2::lyt::LytMaterial * material = picture->GetMaterial( 0 );
  static const nw::lyt::res::TexSRT tex_srt =
  {
    nw::lyt::res::Vec2( 0.0f, 0.0f ),
    0.0f,
    nw::lyt::res::Vec2( 1.0f, 1.0f ),
  };
  material->SetTexSRT( 0, tex_srt );
  material->SetTexMap( 0, &tex_info );
#endif  // GF_PLATFORM_CTR
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadThumbnail
 * @brief   サムネイル読み込み
 * @date    2017.02.10
 *
 * @param   photo_index   写真データインデックス
 * @param   put_index     配置位置インデックス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::LoadThumbnail( u32 photo_index, u32 put_index )
{
  if( m_photoData->LoadThumbnailRequest( photo_index, put_index ) != false )
  {
    GetThumnailPane(put_index)->SetVisible( false );
    if( m_photoLoadFlag[put_index] == 0 )
    {
      SetTimerIcon( put_index, true );
      m_photoLoadFlag[put_index] = 1;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateThumbnail
 * @brief   サムネイル更新（個別）
 * @date    2017.02.10
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::UpdateThumbnail( u32 index )
{
  if( m_photoLoadFlag[index] != 0 )
  {
    if( m_photoData->IsLoadThumbnail(index) != false )
    {
      SetThumbnail( index );
      SetTimerIcon( index, false );
      GetThumnailPane(index)->SetVisible( true );
      m_photoLoadFlag[index] = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateThumbnail
 * @brief   サムネイル更新（全体）
 * @date    2017.02.10
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::UpdateThumbnail(void)
{
  for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
  {
    UpdateThumbnail( i );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPhotoLoadFlag
 * @brief   サムネイル読み込みフラグ初期化
 * @date    2017.02.10
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitPhotoLoadFlag(void)
{
  for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
  {
    m_photoLoadFlag[i] = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPhotoFavorite
 * @brief   お気に入りに登録されているか
 * @date    2017.01.30
 *
 * @retval  true  = されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsPhotoFavorite(void)
{
  return m_photoData->IsFavorite( GetCursorPosIndex() );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPhotoFavoriteFlag
 * @brief   お気に入り登録を変更
 * @date    2017.01.30
 *
 * @return  表示するメッセージ
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::SetPhotoFavoriteFlag(void)
{
  u32 index = GetCursorPosIndex();
  u32 msg = fs_album_win_16;

  if( m_photoData->IsFavorite(index) == false )
  {
    m_photoData->SetFavorite( index, true );
    SetVisiblePhotoFavoriteMark( m_cursorPos, true );
  }
  else
  {
    m_photoData->SetFavorite( index, false );
    SetVisiblePhotoFavoriteMark( m_cursorPos, false );
    msg = fs_album_win_17;
  }

  return msg;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPhotoShareOther
 * @brief   共有されている他の写真があるか
 * @date    2017.02.06
 *
 * @retval  true  = ある
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsPhotoShareOther(void)
{
  u32 pos = GetCursorPosIndex();

  if( pos >= m_photoData->GetDataCount() )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  u32 index = m_photoData->GetIndex( pos );

  for( u32 i=0; i<m_photoData->GetDataCountOrigin(); i++ )
  {
    if( m_photoData->IsShareOrigin(i) != false )
    {
      if( m_photoData->GetIndexOrigin(i) != index )
      {
        return true;
      }
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPhotoShare
 * @brief   共有されているか
 * @date    2017.02.06
 *
 * @retval  true  = されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumDraw::IsPhotoShare(void)
{
  return m_photoData->IsShare( GetCursorPosIndex() );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPhotoShareFlag
 * @brief   共有状態を変更
 * @date    2017.02.06
 *
 * @return  表示するメッセージ
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::SetPhotoShareFlag(void)
{
  u32 index = GetCursorPosIndex();
  u32 msg = fs_album_cap_50;

  if( m_photoData->IsShare(index) == false )
  {
    m_photoData->SetShare( index, true );
  }
  else
  {
    m_photoData->SetShare( index, false );
    msg = fs_album_cap_51;
  }

  // マーク表示切り替え
  u32 max = m_photoData->GetDataCount();
  index = m_page * PHOTO_PUT_MAX;
  for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
  {
    if( index >= max )
    {
      SetVisiblePhotoShareMark( i, false );
    }
    else
    {
      SetVisiblePhotoShareMark( i, m_photoData->IsShare(index) );
    }
    index++;
  }

  return msg;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoMark
 * @brief   写真マーク表示設定
 * @date    2017.01.30
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoMark(void)
{
  u32 index = m_page * PHOTO_PUT_MAX;
  u32 max = m_photoData->GetDataCount();

  for( u32 i=0; i<PHOTO_PUT_MAX; i++ )
  {
    if( index >= max )
    {
      SetVisiblePhotoFavoriteMark( i, false );
      SetVisiblePhotoShareMark( i, false );
      SetVisiblePhotoEditMark( i, false );
    }
    else
    {
      SetVisiblePhotoFavoriteMark( i, m_photoData->IsFavorite(index) );
      SetVisiblePhotoShareMark( i, m_photoData->IsShare(index) );
      SetVisiblePhotoEditMark( i, m_photoData->IsDeco(index) );
    }
    index++;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoFavoriteMark
 * @brief   お気に入り登録マークの表示を変更
 * @date    2017.01.30
 *
 * @param   pos   写真の表示位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoFavoriteMark( u32 pos, bool flg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_H_05,
  };
  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER_PHOTO, tbl[pos], flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoShareMark
 * @brief   共有マークの表示を変更
 * @date    2017.01.30
 *
 * @param   pos   写真の表示位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoShareMark( u32 pos, bool flg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_U_05,
  };
  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER_PHOTO, tbl[pos], flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoEditMark
 * @brief   編集済みマークの表示を変更
 * @date    2017.03.10
 *
 * @param   pos   写真の表示位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoEditMark( u32 pos, bool flg )
{
  static const gfl2::lyt::LytPaneIndex tbl[] =
  {
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_00,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_01,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_02,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_03,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_04,
    PANENAME_STUDIO_ALBUM_LOW_00_PANE_ICON_D_05,
  };
  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }
  GetG2DUtil()->SetPaneVisible( LYTID_LOWER_PHOTO, tbl[pos], flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisiblePhotoSelectMark
 * @brief   選択マークの表示を変更
 * @date    2017.01.31
 *
 * @param   pos   写真の表示位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisiblePhotoSelectMark( u32 pos, bool flg )
{
  static const u32 on_tbl[] =
  {
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_CHECK_ON,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_CHECK_ON,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_CHECK_ON,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_CHECK_ON,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_CHECK_ON,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_CHECK_ON,
  };
  static const u32 off_tbl[] =
  {
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00_CHECK_OFF,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01_CHECK_OFF,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02_CHECK_OFF,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03_CHECK_OFF,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04_CHECK_OFF,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05_CHECK_OFF,
  };
  if( pos >= GFL_NELEMS(on_tbl) || pos >= GFL_NELEMS(off_tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  const u32 * anm_start = on_tbl;
  const u32 * anm_stop  = off_tbl;

  if( flg == false )
  {
    anm_start = off_tbl;
    anm_stop  = on_tbl;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StartAnime( LYTID_LOWER_PHOTO, anm_start[pos] );
  g2d->StopAnime( LYTID_LOWER_PHOTO, anm_stop[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPhotoNone
 * @brief   写真なしの場合の表示
 * @date    2017.02.06
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetPhotoNone(void)
{
  // カーソル非表示
  m_menuCursor->SetVisible( false );
  // 番号非表示
  SetVisiblePhotoNumber( false );
  // タイトル非表示
  SetVisiblePhotoTitle( false );
  { // お気に入り/共有/検索/削除ボタンパッシブ
    static const ButtonID tbl[] =
    {
      BTNID_FAVORITE,
      BTNID_SHARE,
      BTNID_DELETE,
    };
    app::tool::ButtonManager * man = GetButtonManager();
    for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
    {
      man->SetButtonActive( tbl[i], false );  // @fix GFMMCat[708]: 一旦アクティブにする（パッシブアニメが動かないため）
      man->SetButtonPassive( tbl[i] );
      SetButtonViewActive( tbl[i], false );   // @fix MMCat[437]: ボタンの状態を保存
    }
    // @fix MMCat[635]: 検索ボタンは非検索モードのときのみパッシブ
    if( m_appParam->is_sort_mode == false )
    {
      man->SetButtonActive( BTNID_SEARCH, false );  // 一旦アクティブにする（パッシブアニメが動かないため）
      man->SetButtonPassive( BTNID_SEARCH );
      SetButtonViewActive( BTNID_SEARCH, false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleUpperBg
 * @brief   上画面の背景レイアウト表示切り替え
 * @date    2017.02.07
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisibleUpperBg( bool flg )
{
  GetG2DUtil()->SetLayoutDrawEnable( LYTID_UPPER_BG, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartTimerIconAnime
 * @brief   タイマーアイコンアニメ開始
 * @date    2017.02.10
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::StartTimerIconAnime(void)
{
  static const u32 tbl[] =
  {
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00__TIMER_KEEP,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01__TIMER_KEEP,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02__TIMER_KEEP,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03__TIMER_KEEP,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04__TIMER_KEEP,
    LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05__TIMER_KEEP,
  };

  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_LOWER_PHOTO );

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    App::Tool::TimeIcon::StartAnime( lytwk, tbl[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetTimerIcon
 * @brief   タイマーアイコンアニメ表示切り替え
 * @date    2017.02.10
 *
 * @param   index   インデックス
 * @param   flg     true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetTimerIcon( u32 index, bool flg )
{
  static const u32 tbl[][2] =
  {
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_00__TIMER_OUT },
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_01__TIMER_OUT },
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_02__TIMER_OUT },
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_03__TIMER_OUT },
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_04__TIMER_OUT },
    { LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05__TIMER_IN, LA_MYALBUM_STUDIO_ALBUM_LOW_00__PARTS_05__TIMER_OUT },
  };

  u32 start = tbl[index][0];
  u32 stop  = tbl[index][1];

  if( flg == false )
  {
    start = tbl[index][1];
    stop  = tbl[index][0];
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StopAnime( LYTID_LOWER_PHOTO, stop );
  g2d->StartAnime( LYTID_LOWER_PHOTO, start );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetInfoPos
 * @brief   インフォボタンの座標を取得
 * @date    2017.02.17
 *
 * @param   vec   座標格納場所
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::GetInfoPos( gfl2::math::Vector3 * vec )
{
  *vec = GetButtonManager()->GetButtonPosByGfVec3( BTNID_INFO );
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2017.01.25
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
app::ui::UIInputListener::ListenerResult MyAlbumDraw::OnLayoutPaneEvent( const u32 button_id )
{
  switch( button_id )
  {
  case BTNID_SEL_PHOTO_00:
  case BTNID_SEL_PHOTO_01:
  case BTNID_SEL_PHOTO_02:
  case BTNID_SEL_PHOTO_03:
  case BTNID_SEL_PHOTO_04:
  case BTNID_SEL_PHOTO_05:
    if( m_dispMode == DISP_MODE_SELECT )
    {
      m_listener->OnAction( Listener::ACTION_ID_PHOTO );
    }
    else
    {
      m_listener->OnAction( Listener::ACTION_ID_SELECT_DELETE );
    }
    break;

  case BTNID_MV_PHOTO_00:
  case BTNID_MV_PHOTO_01:
  case BTNID_MV_PHOTO_02:
  case BTNID_MV_PHOTO_03:
  case BTNID_MV_PHOTO_04:
  case BTNID_MV_PHOTO_05:
    PutCursor( button_id-BTNID_MV_PHOTO_00 );
    m_listener->OnAction( Listener::ACTION_ID_CURSOR_MOVE );
    break;

  case BTNID_PAGE_LEFT:
    ChangePage( -1, true );
    break;
  case BTNID_PAGE_RIGHT:
    ChangePage( 1, true );
    break;

  case BTNID_FAVORITE:
    m_listener->OnAction( Listener::ACTION_ID_FAVORITE );
    break;

  case BTNID_SHARE:
    m_listener->OnAction( Listener::ACTION_ID_SHARE );
    break;

  case BTNID_INFO:
    m_listener->OnAction( Listener::ACTION_ID_INFO );
    break;

  case BTNID_RETURN:
    if( m_dispMode == DISP_MODE_SELECT )
    {
      m_listener->OnAction( Listener::ACTION_ID_RETURN );
    }
    else
    {
      m_listener->OnAction( Listener::ACTION_ID_END_DELETE );
    }
    break;

  case BTNID_SEARCH:
    m_listener->OnAction( Listener::ACTION_ID_SEARCH );
    break;

  case BTNID_DELETE:
    m_listener->OnAction( Listener::ACTION_ID_DELETE );
    break;

  case BTNID_DEL_MULTI:
    m_listener->OnAction( Listener::ACTION_ID_DECIDE_DELETE );
    break;

  case BTNID_SEARCH_TYPE_RETURN:
    m_listener->OnAction( Listener::ACTION_ID_SEARCH_TYPE_CANCEL );
    break;

  case BTNID_SEARCH_TYPE_RESET:
    m_listener->OnAction( Listener::ACTION_ID_SEARCH_TYPE_RESET );
    break;

  case BTNID_SEARCH_TYPE_SEARCH:
    m_listener->OnAction( Listener::ACTION_ID_SEARCH_TYPE_DECIDE );
    break;

  case BTNID_SEARCH_CORE_RETURN:
    m_listener->OnAction( Listener::ACTION_ID_SEARCH_CORE_CANCEL );
    break;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2017.01.25
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
app::ui::UIInputListener::ListenerResult MyAlbumDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_dispMode == DISP_MODE_SELECT || m_dispMode == DISP_MODE_DELETE )
  {
    if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      GetButtonManager()->StartSelectedAct( BTNID_SEL_PHOTO_00+m_cursorPos );
      return DISABLE_ACCESS;
    }

    if( key->IsRepeat(gfl2::ui::DIR_UP) )
    {
      MoveCursor( 0, -1 );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      MoveCursor( 0, 1 );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
    {
      MoveCursor( -1, 0 );
      return DISABLE_ACCESS;
    }
    if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
    {
      MoveCursor( 1, 0 );
      return DISABLE_ACCESS;
    }
  }

  return ENABLE_ACCESS;
}


//-----------------------------------------------------------------------------
/**
 * @func    InitPaneListPos
 * @brief   ペインリスト位置データ初期化
 * @date    2017.02.21
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitPaneListPos(void)
{
  for( u32 i=0; i<LIST_TYPE_MAX; i++ )
  {
    m_listPos[i] = 0;
    m_listScroll[i] = 0.0f;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListType
 * @brief   ペインリストタイプを設定
 * @date    2017.02.21
 *
 * @param   type  リストタイプ
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetListType( ListType type )
{
  { // カーソル位置退避
    app::tool::PaneListView * view = m_listCore;
    if( m_listType == LIST_TYPE_TOP )
    {
      view = m_listTop;
    }
    if( view != NULL )
    {
      view->GetPaneList()->GetCursorData( &m_listPos[m_listType], &m_listScroll[m_listType] );
    }
  }

  m_listType = type;

  // 上画面メッセージ変更
  SetListInfoMessage( m_listType );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListType
 * @brief   ペインリストタイプを取得
 * @date    2017.02.21
 *
 * @return  リストタイプ
 */
//-----------------------------------------------------------------------------
MyAlbumDraw::ListType MyAlbumDraw::GetListType(void)
{
  return m_listType;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListTop
 * @brief   ソートトップリスト初期化
 * @date    2017.02.20
 *
 * @param   view    リストビュークラス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitListTop( app::tool::PaneListView * view )
{
  static const u32 anm[TOP_LIST_PANE_MAX][4] =
  {
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_00_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_00_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_00_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_00_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_01_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_01_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_01_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_01_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_02_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_02_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_02_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_02_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_03_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_03_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_03_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_03_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_04_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_04_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_04_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_04_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_05_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_05_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_05_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_05_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_06_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_06_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_06_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_06_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_07_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_07_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_07_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_07_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_08_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_08_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_08_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_01__BUTTON_08_TOUCH_RELEASE,
    },
  };

  m_listTop = view;
  m_pListTopData = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::tool::PaneList::PANE_DATA[TOP_LIST_PANE_MAX];

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_SEARCH_TYPE );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<TOP_LIST_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListTopPartsTable[i] );
    m_pListTopData[i].base       = parts;
    m_pListTopData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_STUDIOPARTS_BUTTON_12_PANE_TEXTBOX_00, &res_id );
    m_pListTopData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_STUDIOPARTS_BUTTON_12_PANE_BOUND_00, &res_id );
    m_pListTopData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_STUDIOPARTS_BUTTON_12_PANE_NULL_CUR, &res_id );

    m_pListTopData[i].holdAnime    = anm[i][0];
    m_pListTopData[i].releaseAnime = anm[i][1];
    m_pListTopData[i].cancelAnime  = anm[i][2];
    m_pListTopData[i].selectAnime  = anm[i][3];
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetListTopParam( &list_setup, 0, 0.0f );

  u32 scroll_bar_max;
  if( list_setup.valMax <= list_setup.listPosMax )
  {
    scroll_bar_max = 0;
  }
  else
  {
    scroll_bar_max = list_setup.valMax - list_setup.listPosMax;
  }

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    m_heap->GetSystemHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    scroll_bar_max,

    lytwk,
    res_id,
    PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL
  };
  
  m_listTop->CreatePaneList( &list_setup );
  m_listTop->CreateScrollBar( &scroll_bar_setup );
  SetPaneListConfig(
    m_listTop->GetPaneList(),
    m_pListTopData,
    &list_setup,
    TOP_LIST_PX, TOP_LIST_PY, TOP_LIST_SX, TOP_LIST_SY );

  AddSubView( m_listTop );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndListTop
 * @brief   ソートトップリスト終了
 * @date    2017.02.20
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::EndListTop(void)
{
  if( m_listTop != NULL )
  {
    m_listTop->DeleteScrollBar();
    m_listTop->DeletePaneList();
    m_listTop->RemoveFromSuperView();
    m_listTop = NULL;
  }
  GFL_SAFE_DELETE_ARRAY( m_pListTopData );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListTopParam
 * @brief   ソートトップのペインリスト初期化パラメータ設定
 * @date    2017.02.21
 *
 * @param   param   パラメータ生成場所
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetListTopParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, f32 scroll )
{
  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_LOWER_SEARCH_TYPE );

  param->heap = m_heap->GetSystemHeap();    // ヒープ
  param->pLytWk = lytwk;                    // レイアウトワーク
  param->paneList = m_pListTopData;       // ペインデータ
  param->paneListMax = TOP_LIST_PANE_MAX; // ペインデータ数

  param->valMax = TOP_LIST_ITEM_MAX;            // 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax = TOP_LIST_CURSOR_MOVE_MAX; // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos = pos;         // 初期カーソル位置 ( 0 ? listPosMax-1 の範囲 )
  param->listScroll = scroll;   // 初期スクロール位置 ( 0 ? valMax-listPosMax-1 の範囲 )

  param->defPY = TOP_LIST_PY;  // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX = TOP_LIST_SX;  // ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = TOP_LIST_SY;  // ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;     // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = lytwk->GetPane( PANENAME_STUDIO_ALBUM_LIST_LOW_01_PANE_NULL_BTN );  // 項目基準位置のペイン

  param->cbFunc = this;   // コールバック関数
  param->cbWork = NULL;   // コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  param->menuCursor = m_menuCursor;   // カーソル
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetTopList
 * @brief   ソートトップリスト再生成
 * @date    2017.02.21
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::ResetTopList(void)
{
  SetListType( LIST_TYPE_TOP );

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetListTopParam( &list_setup, m_listPos[LIST_TYPE_TOP], m_listScroll[LIST_TYPE_TOP] );
  m_listTop->ResetPaneList( &list_setup );
  SetPaneListConfig(
    m_listTop->GetPaneList(),
    m_pListTopData,
    &list_setup,
    TOP_LIST_PX, TOP_LIST_PY, TOP_LIST_SX, TOP_LIST_SY );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListCore
 * @brief   ソート項目リスト初期化
 * @date    2017.02.21
 *
 * @param   view    リストビュークラス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitListCore( app::tool::PaneListView * view )
{
  static const u32 anm[CORE_LIST_PANE_MAX][4] =
  {
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_00_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_00_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_00_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_00_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_01_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_01_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_01_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_01_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_02_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_02_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_02_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_02_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_03_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_03_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_03_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_03_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_04_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_04_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_04_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_04_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_05_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_05_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_05_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_05_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_06_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_06_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_06_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_06_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_07_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_07_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_07_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_07_TOUCH_RELEASE,
    },
    {
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_08_TOUCH,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_08_RELEASE,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_08_CANSEL,
      LA_MYALBUM_STUDIO_ALBUM_LIST_LOW_00__BUTTON_08_TOUCH_RELEASE,
    },
  };

  m_listCore = view;
  m_pListCoreData = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::tool::PaneList::PANE_DATA[CORE_LIST_PANE_MAX];

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_SEARCH_CORE );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<CORE_LIST_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListCorePartsTable[i] );
    m_pListCoreData[i].base       = parts;
    m_pListCoreData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_STUDIOPARTS_BUTTON_02_PANE_TEXTBOX_00, &res_id );
    m_pListCoreData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_STUDIOPARTS_BUTTON_02_PANE_BOUND_00, &res_id );
    m_pListCoreData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_STUDIOPARTS_BUTTON_02_PANE_NULL_CUR, &res_id );

    m_pListCoreData[i].holdAnime    = anm[i][0];
    m_pListCoreData[i].releaseAnime = anm[i][1];
    m_pListCoreData[i].cancelAnime  = anm[i][2];
    m_pListCoreData[i].selectAnime  = anm[i][3];
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetListCoreParam( &list_setup, 0, 0.0f, CORE_LIST_LOW_MAX );  // 初期値は適当

  u32 scroll_bar_max;
  if( list_setup.valMax <= list_setup.listPosMax )
  {
    scroll_bar_max = 0;
  }
  else
  {
    scroll_bar_max = list_setup.valMax - list_setup.listPosMax;
  }

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    m_heap->GetSystemHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    scroll_bar_max,

    lytwk,
    res_id,
    PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL
  };
  
  m_listCore->CreatePaneList( &list_setup );
  m_listCore->CreateScrollBar( &scroll_bar_setup );
  SetPaneListConfig(
    m_listCore->GetPaneList(),
    m_pListCoreData,
    &list_setup,
    CORE_LIST_PX, CORE_LIST_PY, CORE_LIST_SX, CORE_LIST_SY );

  AddSubView( m_listCore );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndListCore
 * @brief   ソート項目リスト終了
 * @date    2017.02.21
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::EndListCore(void)
{
  if( m_listCore != NULL )
  {
    m_listCore->DeleteScrollBar();
    m_listCore->DeletePaneList();
    m_listCore->RemoveFromSuperView();
    m_listCore = NULL;
  }
  GFL_SAFE_DELETE_ARRAY( m_pListCoreData );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListCoreParam
 * @brief   ソート項目のペインリスト初期化パラメータ設定
 * @date    2017.02.21
 *
 * @param   param   パラメータ生成場所
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 * @param   max     項目数
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetListCoreParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, f32 scroll, u32 max )
{
  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( LYTID_LOWER_SEARCH_CORE );

  param->heap = m_heap->GetSystemHeap();    // ヒープ
  param->pLytWk = lytwk;                    // レイアウトワーク
  param->paneList = m_pListCoreData;        // ペインデータ
  param->paneListMax = CORE_LIST_PANE_MAX;  // ペインデータ数

  param->valMax = max;      // 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax = CORE_LIST_CURSOR_MOVE_MAX;  // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos = pos;                           // 初期カーソル位置 ( 0 ? listPosMax-1 の範囲 )
  param->listScroll = scroll;                     // 初期スクロール位置 ( 0 ? valMax-listPosMax-1 の範囲 )

  param->defPY = CORE_LIST_PY;  // ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX = CORE_LIST_SX;  // ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = CORE_LIST_SY;  // ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;     // 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = lytwk->GetPane( PANENAME_STUDIO_ALBUM_LIST_LOW_00_PANE_ALL );  // 項目基準位置のペイン

  param->cbFunc = this;   // コールバック関数
  param->cbWork = NULL;   // コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  param->menuCursor = m_menuCursor;   // カーソル
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListConfig
 * @brief   ペインリストの設定変更
 * @date    2017.02.20
 *
 * @param   list    ペインリスト
 * @param   setup   ペインリストのセットアップパラメータ
 * @param   px      項目配置X座標
 * @param   py      項目配置Y座標
 * @param   sx      項目Xサイズ
 * @param   sy      項目Yサイズ
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetPaneListConfig(
      app::tool::PaneList * list,
      app::tool::PaneList::PANE_DATA * data,
      app::tool::PaneList::SETUP_PARAM * setup,
      f32 px, f32 py, f32 sx, f32 sy )
{
  // SE
  list->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = data[0].bound->GetTranslate();
    u32 pos_max = setup->listPosMax;
    if( setup->valMax < pos_max )
    {
      pos_max = setup->valMax;
    }
    f32 lx = px - sx / 2.0f;
    f32 rx = lx + sx;
    f32 uy = py + pos.y + sy / 2.0f;
    f32 dy = uy - sy * pos_max;
    list->MakeTouchArea( lx, rx, uy, dy );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetCoreList
 * @brief   ソート項目リスト再生成
 * @date    2017.02.21
 *
 * @param   type      リストタイプ
 * @param   max       項目数
 * @param   pos_init  true = カーソル位置を初期化
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::ResetCoreList( ListType type, u32 max, bool pos_init )
{
  SetListType( type );

  if( pos_init != false )
  {
    m_listPos[m_listType] = 0;
    m_listScroll[m_listType] = 0.0f;
  }
  else
  {
    // スクロール値補正
    u32 scroll_max = 0;
    if( max >= CORE_LIST_CURSOR_MOVE_MAX )
    {
      scroll_max = max - CORE_LIST_CURSOR_MOVE_MAX;
    }
    if( static_cast<u32>(m_listScroll[m_listType]) > scroll_max )
    {
      m_listScroll[m_listType] = static_cast<f32>(scroll_max);
    }
    // カーソル位置補正
    u32 pos_max = 0;
    if( scroll_max != 0 )
    {
      pos_max = CORE_LIST_CURSOR_MOVE_MAX - 1;
    }
    else if( max != 0 )
    {
      pos_max = max - 1;
    }
    if( m_listPos[m_listType] > pos_max )
    {
      m_listPos[m_listType] = pos_max;
    }
  }

  { // 母音リストの設定を戻す
    nw::ut::Color8 color = nw::ut::Color8::BLACK;
    for( u32 i=0; i<CORE_LIST_PANE_MAX; i++ )
    {
      m_pListCoreData[i].text->SetTextColor( color, color );
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  SetListCoreParam( &list_setup, m_listPos[m_listType], m_listScroll[m_listType], max );
  m_listCore->ResetPaneList( &list_setup );
  SetPaneListConfig(
    m_listCore->GetPaneList(),
    m_pListCoreData,
    &list_setup,
    CORE_LIST_PX, CORE_LIST_PY, CORE_LIST_SX, CORE_LIST_SY );

  {
    app::tool::PaneList * list = m_listCore->GetPaneList();

    // 母音リストのSE設定
    if( m_listType == LIST_TYPE_VOWEL )
    {
      for( u32 i=0; i<m_vowelDataLen; i++ )
      {
        if( GetPokemonInitialDataCount(i) == 0 )
        {
          int index = list->GetPosPaneIndexEz( i );
          if( index == -1 )
          {
            GFL_ASSERT( 0 );
          }
          else
          {
            list->SetButtonSEPaneIndex( index, SEQ_SE_BEEP );
          }
        }
      }
    }

    // ポケモン頭文字
    if( m_listType == LIST_TYPE_CONSONANT && System::GetLang() != POKEMON_LANG_JAPAN )
    {
      // リスト範囲外の配置を考慮して±2の位置を調べる
      int item = static_cast<int>(m_listScroll[m_listType]) - 2;
      for( int i=-2; i<static_cast<int>(CORE_LIST_PANE_MAX)+2; i++ )
      {
        if( item >= 0 && item < max )
        {
          if( GetPokemonInitialDataCount(item) == 0 )
          {
            int index = list->GetPosPaneIndexEz( i );
            if( index != -1 )
            {
              list->SetButtonSEPaneIndex( index, SEQ_SE_BEEP );
            }
          }
        }
        item++;
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorMove
 * @brief   カーソル移動時のコールバック関数
 * @date    2017.02.20
 *
 * @param   work        外部から指定されたワーク
 * @param   cur_pane    ペインデータ
 * @param   pane_index  カーソル位置のペインデータインデックス
 * @param   pos         カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
//  PutListWazaInfo( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ListWrite
 * @brief		リストの項目を描画するコールバック関数
 * @date    2017.02.20
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  switch( m_listType )
  {
  case LIST_TYPE_TOP:         // トップ
    WriteListTopItem( pane_index, pos );
    break;
  case LIST_TYPE_LOW:         // 並び
    WriteListLowItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_CONSONANT:   // ポケモン：子音
    WriteListConsonantItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_VOWEL:       // ポケモン：母音
    WriteListVowelItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_MONS_NAME:   // ポケモン名
    WriteListMonsNameItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_FAVORITE:    // お気に入り
    WriteListFavoriteItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_DECO:        // 飾りつけ
    WriteListDecoItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_PHOTO:       // 撮り方
    WriteListPhotoItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_BG:          // 背景
    WriteListBgItem( pane, pane_index, pos );
    break;
  case LIST_TYPE_SHARE:       // 共有
    WriteListShareItem( pane, pane_index, pos );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListTopItem
 * @brief		ソートトップリスト項目描画
 * @date    2017.02.20
 *
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListTopItem( u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_LOWER_SEARCH_TYPE );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListTopPartsTable[pane_index] );
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  static const u32 title_tbl[] =
  {
    fs_album_cap_07,  // ならび
    fs_album_cap_08,  // ポケモン
    fs_album_cap_10,  // おきにいり
    fs_album_cap_09,  // かざりつけ
    fs_album_cap_11,  // とりかた
    fs_album_cap_12,  // はいけい
    fs_album_cap_13,  // きょうゆう
  };

  if( pos >= TOP_LIST_ITEM_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_STUDIOPARTS_BUTTON_12_PANE_TEXTBOX_00,&res_id), title_tbl[pos] );

  switch( pos )
  {
  case 0:   // ならび
    {
      static const u32 tbl[] =
      {
        fs_album_cap_15,  // あたらしいじゅん
        fs_album_cap_14,  // ふるいじゅん
      };
      g2d->GetString( str, tbl[m_appParam->sort_data.low] );
    }
    break;

  case 1:   // ポケモン
    if( m_appParam->sort_data.mons_no == 0 )
    {
      g2d->GetString( str, fs_album_cap_17 );
    }
    else
    {
      g2d->SetRegisterMonsNameNo( 0, static_cast<MonsNo>(m_appParam->sort_data.mons_no) );
      g2d->GetStringExpand( str, fs_album_cap_18 );
    }
    break;

  case 2:   // おきにいり
    {
      static const u32 tbl[] =
      {
        fs_album_cap_17,  // ー
        fs_album_cap_20,  // している
        fs_album_cap_19,  // していない
      };
      g2d->GetString( str, tbl[m_appParam->sort_data.favorite] );
    }
    break;

  case 3:   // かざりつけ
    {
      static const u32 tbl[] =
      {
        fs_album_cap_17,  // ー
        fs_album_cap_20,  // している
        fs_album_cap_19,  // していない
      };
      g2d->GetString( str, tbl[m_appParam->sort_data.deco] );
    }
    break;

  case 4:   // とりかた
    {
      static const u32 tbl[] =
      {
        fs_album_cap_17,  // ー
        fs_album_cap_21,  // ポケモンだけ
        fs_album_cap_22,  // ポケモンといっしょ
      };
      g2d->GetString( str, tbl[m_appParam->sort_data.photo] );
    }
    break;

  case 5:   // はいけい
    GetBgListItemString( str, m_appParam->sort_data.bg_no );
    break;

  case 6:   // きょうゆう
    {
      static const u32 tbl[] =
      {
        fs_album_cap_17,  // ー
        fs_album_cap_20,  // している
        fs_album_cap_19,  // していない
      };
      g2d->GetString( str, tbl[m_appParam->sort_data.share] );
    }
    break;
  }

  g2d->SetTextBoxPaneString( lytwk->GetTextBoxPane(parts,PANENAME_STUDIOPARTS_BUTTON_12_PANE_TEXTBOX_01,&res_id), str );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListLowItem
 * @brief		ソート並びリスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListLowItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 tbl[] =
  {
    fs_album_cap_15,  // あたらしいじゅん
    fs_album_cap_14,  // ふるいじゅん
  };

  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetTextBoxPaneString( pane[pane_index].text, tbl[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListConsonantItem
 * @brief		ソート子音リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListConsonantItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  if( pos == 0 )
  {
    if( System::GetLang() != POKEMON_LANG_JAPAN )
    {
      nw::ut::Color8 color = nw::ut::Color8::BLACK;
      pane[pane_index].text->SetTextColor( color, color );
    }
    g2d->GetString( str, fs_album_cap_17 );
  }
  else
  {
    if( System::GetLang() != POKEMON_LANG_JAPAN )
    {
      nw::ut::Color8 color = nw::ut::Color8::BLACK;
      u32 se = SEQ_SE_DECIDE1;
      if( GetPokemonInitialDataCount(pos) == 0 )
      {
        color = nw::ut::Color8::GRAY;
        se = SEQ_SE_BEEP;
      }
      pane[pane_index].text->SetTextColor( color, color );
      app::tool::PaneList * list = m_listCore->GetPaneList();
      if( list != NULL )
      {
        list->SetButtonSEPaneIndex( pane_index, se );
      }
    }

    m_pInitialMsgData->GetString( m_langInitialTable[pos], *str );
  }

  g2d->SetTextBoxPaneString( pane[pane_index].text, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListVowelItem
 * @brief		ソート母音リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListVowelItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  if( m_vowelData == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( m_sort == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  nw::ut::Color8 color = nw::ut::Color8::BLACK;
  if( GetPokemonInitialDataCount(pos) == 0 )
  {
    color = nw::ut::Color8::GRAY;
  }
  pane[pane_index].text->SetTextColor( color, color );

  m_pInitialMsgData->GetString( m_vowelData[pos].msg, *str );
  g2d->SetTextBoxPaneString( pane[pane_index].text, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListMonsNameItem
 * @brief		ソートポケモン名リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListMonsNameItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->SetRegisterMonsNameNo( 0, m_sortInitial[pos] );
  g2d->SetTextBoxPaneStringExpand( pane[pane_index].text, fs_album_cap_18 );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListFavoriteItem
 * @brief		ソートお気に入りリスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListFavoriteItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 tbl[] =
  {
    fs_album_cap_17,  // ー
    fs_album_cap_20,  // している
    fs_album_cap_19,  // していない
  };

  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetTextBoxPaneString( pane[pane_index].text, tbl[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListDecoItem
 * @brief		ソート飾りつけリスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListDecoItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 tbl[] =
  {
    fs_album_cap_17,  // ー
    fs_album_cap_20,  // している
    fs_album_cap_19,  // していない
  };

  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetTextBoxPaneString( pane[pane_index].text, tbl[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListPhotoItem
 * @brief		ソート撮り方リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListPhotoItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 tbl[] =
  {
    fs_album_cap_17,  // ー
    fs_album_cap_21,  // ポケモンだけ
    fs_album_cap_22,  // ポケモンといっしょ
  };

  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetTextBoxPaneString( pane[pane_index].text, tbl[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListBgItem
 * @brief		ソート背景リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListBgItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  GetBgListItemString( str, pos );
  g2d->SetTextBoxPaneString( pane[pane_index].text, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteListShareItem
 * @brief		ソート共有リスト項目描画
 * @date    2017.02.21
 *
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::WriteListShareItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  static const u32 tbl[] =
  {
    fs_album_cap_17,  // ー
    fs_album_cap_20,  // している
    fs_album_cap_19,  // していない
  };

  if( pos >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  GetG2DUtil()->SetTextBoxPaneString( pane[pane_index].text, tbl[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorOff
 * @brief		カーソルを非表示にするコールバック関数
 * @date    2017.02.20
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	カーソル位置のペインデータインデックス
 * @param		pos					カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ItemChange
 * @brief		入れ替え用コールバック関数
 * @date    2017.02.20
 *
 * @param		work	外部から指定されたワーク
 * @param		pos1	入れ替え位置１
 * @param		pos2	入れ替え位置２
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetBgListItemString
 * @brief   背景名取得
 * @date    2017.02.21
 *
 * @param   buf   文字列格納場所
 * @param   val   背景値
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::GetBgListItemString( gfl2::str::StrBuf * buf, u32 val )
{
  if( val == 0 )
  {
    GetG2DUtil()->GetString( buf, fs_album_cap_17  );
  }
  else
  {
    // @fix MMCat[325]: スタジオデータから並びを取得
    App::FinderStudio::BGDataAccessor::UIData data = App::FinderStudio::BGDataAccessor::GetUIData( val-1 );
    m_pBgMsgData->GetString( data.msgID, *buf );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectVowelData
 * @brief   選択された母音データをセット
 * @date    2017.02.22
 *
 * @param   pos   選択位置 ( 0オリジン )
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetSelectVowelData( u32 pos )
{
  m_vowelData = VowelDataTable[pos];
  m_vowelDataLen = VowelDataTableLen[pos];
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectVowelData
 * @brief   母音データ数を取得
 * @date    2017.02.22
 *
 * @return  データ数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetVowelListMax(void)
{
  return m_vowelDataLen;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPokemonSortData
 * @brief   ポケモンソートデータ初期化
 * @date    2017.02.22
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::InitPokemonSortData(void)
{
  for( u32 i=0; i<POKE_SORT_DATA_MAX; i++ )
  {
    m_sortAll[i] = MONSNO_NULL;
    m_sortInitial[i] = MONSNO_NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPokemonSortData
 * @brief   ポケモンソートデータをセット
 * @date    2017.02.22
 *
 * @param   sort  ソートデータクラス
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetPokemonSortData( const app::sort::StringMonsName * sort )
{
  u32 count = 0;

  m_sort = sort;

  for( u32 i=0; i<POKE_SORT_DATA_MAX; i++ )
  {
    MonsNo mons = static_cast<MonsNo>( m_sort->GetSortOrder( i ) );
    if( m_photoData->CheckMonsNoData( mons ) != false )
    {
      m_sortAll[count] = mons;
      count++;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePokemonInitialData
 * @brief   ポケモン頭文字ソートデータをセット
 * @date    2017.02.22
 *
 * @param   pos   リスト位置
 *
 * @return  データ数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::CreatePokemonInitialData( u32 pos )
{
  m_sortInitialMax = 0;

  if( m_sort == NULL )
  {
    GFL_ASSERT( 0 );
    return 0;
  }

  u32 sort = pos;
  if( System::GetLang() == POKEMON_LANG_JAPAN )
  {
    if( m_vowelData == NULL )
    {
      GFL_ASSERT( 0 );
      return 0;
    }
    sort = m_vowelData[pos].sort;
  }

  for( u32 i=0; i<POKE_SORT_DATA_MAX; i++ )
  {
    if( m_sortAll[i] == MONSNO_NULL )
    {
      break;
    }
    if( m_sort->GetInitialIndex(m_sortAll[i]) == sort )
    {
      m_sortInitial[m_sortInitialMax] = m_sortAll[i];
      m_sortInitialMax++;
    }
  }

  return m_sortInitialMax;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPokemonInitialDataCount
 * @brief   ポケモン頭文字ソートデータ数を取得
 * @date    2017.02.22
 *
 * @param   pos   リスト位置
 *
 * @return  データ数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetPokemonInitialDataCount( u32 pos )
{
  if( m_sort == NULL )
  {
    GFL_ASSERT( 0 );
    return 0;
  }

  u32 sort = pos;
  if( System::GetLang() == POKEMON_LANG_JAPAN )
  {
    if( m_vowelData == NULL )
    {
      GFL_ASSERT( 0 );
      return 0;
    }
    sort = m_vowelData[pos].sort;
  }

  u32 count = 0;

  for( u32 i=0; i<POKE_SORT_DATA_MAX; i++ )
  {
    if( m_sortAll[i] == MONSNO_NULL )
    {
      break;
    }
    if( m_sort->GetInitialIndex(m_sortAll[i]) == sort )
    {
      count++;
    }
  }

  return count;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPokemonInitialData
 * @brief   ポケモン頭文字ソートデータを取得
 * @date    2017.02.22
 *
 * @param   pos   リスト位置
 *
 * @return  ポケモン番号
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetPokemonInitialData( u32 pos )
{
  return m_sortInitial[pos];
}


//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleListInfoWindow
 * @brief   リスト説明ウィンドウ表示切り替え
 * @date    2017.02.23
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetVisibleListInfoWindow( bool flg )
{
  GetG2DUtil()->SetPaneVisible( LYTID_UPPER_BG, PANENAME_STUDIO_ALBUM_UP_00_PANE_MESSAGE_L2, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListInfoMessage
 * @brief   リスト説明メッセージセット
 * @date    2017.02.23
 *
 * @param   type  リストタイプ
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetListInfoMessage( ListType type )
{
  static const u32 tbl[] =
  {
    fs_album_cap_06,    //!< トップ
    fs_album_cap_33,    //!< 並び
    fs_album_cap_34,    //!< ポケモン：子音
    fs_album_cap_34,    //!< ポケモン：母音
    fs_album_cap_35,    //!< ポケモン名
    fs_album_cap_37,    //!< お気に入り
    fs_album_cap_36,    //!< 飾りつけ
    fs_album_cap_38,    //!< 撮り方
    fs_album_cap_39,    //!< 背景
    fs_album_cap_40,    //!< 共有
  };

  if( type >= GFL_NELEMS(tbl) )
  {
    GFL_ASSERT( 0 );
    type = LIST_TYPE_TOP;
  }

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( LYTID_UPPER_BG );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytTextBox * pane = lytwk->GetTextBoxPane( lytwk->GetPartsPane(PANENAME_STUDIO_ALBUM_UP_00_PANE_MESSAGE_L2), PANENAME_COMMON_MSG_UPP_000_PANE_TEXTBOX_00, &res_id );

  g2d->SetTextBoxPaneString( pane, tbl[type] );
  pane->SetVisible( true );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartSort
 * @brief   絞込み開始
 * @date    2017.02.27
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::StartSort(void)
{
  m_page = 0;
  m_cursorPos = 0;
  m_photoData->CreateSortData( &m_appParam->sort_data );
  CreatePageData();
  ChangePage( 0, false );
  PutPhotoMax();
  if( PutCursor( m_cursorPos ) == false )
  {
    SetPhotoNone();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetLangInitial
 * @brief   言語別頭文字設定
 * @date    2017.02.28
 */
//-----------------------------------------------------------------------------
void MyAlbumDraw::SetLangInitial(void)
{
  u32 lang = System::GetLang();

  // 日本語
  if( lang == POKEMON_LANG_JAPAN )
  {
    m_langInitialTable = Initial_Consonant;
    m_langInitialMax = CORE_LIST_CONSONANT_MAX;
  }
  // ハングル
  else if( lang == POKEMON_LANG_KOREA )
  {
    m_langInitialTable = Initial_Koria;
    m_langInitialMax = INITIAL_LIST_KORIA_MAX;
  }
  // 簡体字
  else if( lang == POKEMON_LANG_CHINA )
  {
    m_langInitialTable = Initial_China;
    m_langInitialMax = INITIAL_LIST_CHINA_MAX;
  }
  // 繁体字
  else if( lang == POKEMON_LANG_TAIWAN )
  {
    m_langInitialTable = Initial_Taiwan;
    m_langInitialMax = INITIAL_LIST_TAIWAN_MAX;
  }
  // アルファベット
  else
  {
    m_langInitialTable = Initial_Alphabet;
    m_langInitialMax = INITIAL_LIST_ALPHABET_MAX;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetLangListMax
 * @brief   言語別頭文字数取得
 * @date    2017.02.28
 *
 * @return  頭文字数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumDraw::GetLangListMax(void)
{
  return m_langInitialMax;
}


//-----------------------------------------------------------------------------
/**
 * @func    GetExpandMessage
 * @brief   メッセージを展開して取得
 * @date    2017.04.14
 *
 * @param   msg   メッセージID
 * @param   val   Register[0]に入れる値
 * @param   keta  valの桁
 * 
 * @return  文字列バッファ
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * MyAlbumDraw::GetExpandMessage( u32 msg, u32 val, u32 keta )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->SetRegisterNumber( 0, val, keta );
  g2d->GetStringExpand( str, msg );
  return str;
}


GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)

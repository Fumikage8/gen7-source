#if PM_DEBUG
//======================================================================
/**
 * @file   ncl_font_viewer.cpp
 * @brief	 フォントの全文字表示デバッグモード
 * @author ichibayashi_hironori@Nintendo
 * @date   2013/05/07
 *
 * @note    sangoから移植
 * @author  Hiroyuki Nakamura
 * @date    2016/06/13
 */
//======================================================================

// 有泉様の ari_msg_test.cpp をコピーして改変

// 
// ・文字に対応する正しい字形が表示されていない
// ・システムフォントとバトルフォントとで字形が異なる
// といった問題を確認するのに使用する。
//
// 表示対象の文字は、あらかじめ ncl_font_view.cdat に定義してある。
// これを「文字セット」と呼ぶことにする。文字セットは2種類定義する:
//   UserInputCode: ユーザー入力可能な文字
//   DefinedCode  : kujiraで使用する全文字、kujira_for_ms.xllt の文字
//
// 1ページを 16 [文字/行] * 6 [行] = 96文字とし、
// 「文字セット」に含まれる文字を、頭からページ単位に区切って表示する。
//
// 操作方法:
//   上/下  前/次のページ
//   Y      文字セット切り替え
//   A      自動ページ送り（押している間、次のページに移動しつづける）
//   START  終了
//
// おまけで、現在表示している文字コードの範囲と、ページ番号/ページ数を表示する。
//

#include "Debug/FontViewer/include/ncl_font_viewer.h"

#include "System/include/HeapDefine.h"
#include "Print/include/WordSet.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/debug_message.gaix"
#include "arc_index/DebugFontViewer.gaix"
#include "niji_conv_header/message/debug/msg_d_ncl.h"
#include "niji_conv_header/app/debug/debug_font_viewer/DebugFontViewer.h"
#include "niji_conv_header/app/debug/debug_font_viewer/DebugFontViewer_pane.h"
#include "niji_conv_header/app/debug/debug_font_viewer/DebugFontViewer_anim_list.h"


GFL_NAMESPACE_BEGIN(Debug)

const int VIEWBODY_LINESIZE   = 16;
const int VIEWBODY_LINECOUNT  = 6;
const int VIEWBODY_INDEXCOUNT = (VIEWBODY_LINESIZE*VIEWBODY_LINECOUNT);  // ペインに表示する文字数
// 上下ボタンで文字コードが変化する量
const int CODEMOVE_COUNT      = (VIEWBODY_LINESIZE*VIEWBODY_LINECOUNT);

#include "ncl_char_set.cdat"

static const u32 MANAGER_HEAP_SIZE = 0x1000;          //!< マネージャヒープサイズ
static const u32 LOCAL_HEAP_SIZE        = 0x080000;   //!< ローカルヒープサイズ
static const u32 LOCAL_DEVICE_HEAP_SIZE = 0x800000;   //!< ローカルデバイスヒープサイズ


FontViewer::FontViewer( void )
  : GameSys::GameProc()
  , mpManagerHeap( NULL )
  , mHeapMem( NULL )
  , mDeviceMem( NULL )
  , mHeapAllocater( NULL )
  , mDeviceAllocater( NULL )
  , mpRenderingManager( NULL )
  , mpG2DUtil( NULL )
  , mpLytResBuff( NULL )
  , mDeviceManager( NULL )
  , mIsUpdateMessage( false )
  , mMsgData( NULL )
  , mStrBuf( NULL )
  , mExpandBuf( NULL )
  , mWordSet( NULL )
  , m_IndexMode( INDEXMODE_DEFINED )
  , m_IndexBegin( 0 )
  , m_IndexEnd( 0 )
{
}

FontViewer::~FontViewer()
{
}

/*
gamesystem::FrameMode FontViewer::GetFrameMode( void ) const
{
  return gamesystem::FRAMEMODE_60;
}
*/


//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result FontViewer::InitFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  // ヒープ
  gfl2::heap::HeapBase * heap     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase * dev_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  mpManagerHeap = GFL_CREATE_LOCAL_HEAP( dev_heap, MANAGER_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  mHeap.LocalHeapCreate( heap, dev_heap, LOCAL_HEAP_SIZE, LOCAL_DEVICE_HEAP_SIZE );
  mHeapMem   = mHeap.GetSystemHeap();
  mDeviceMem = mHeap.GetDeviceHeap();
  mHeapAllocater   = mHeap.GetSystemAllocator();
  mDeviceAllocater = mHeap.GetDeviceAllocator();

  // UI
  mDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

  // 描画マネージャ
  mpRenderingManager = GFL_NEW(mpManagerHeap) app::util::AppRenderingManager();
  app::util::AppRenderingManager::Description desc;
  for( int i=0; i<app::util::AppRenderingManager::RenderPlaceType::NUM; i++ )
  {
    desc.renderDesc[i].renderOrder     = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[i].pokeModelNumMax = 0;
    desc.renderDesc[i].modelNumMax     = 0;
  }
  mpRenderingManager->SyncCreate(
    desc,
    &mHeap,
    NULL,
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() );

  // レイアウトリソース
  app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_FONT_VIEWER, mHeapMem );
  app::util::FileAccessor::FileLoadSync(
    ARCID_DEBUG_FONT_VIEWER,
    GARC_DebugFontViewer_DebugFontViewer_applyt_COMP,
    &mpLytResBuff,
    mDeviceMem,
    true,
    128 );
  app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_FONT_VIEWER );

  // G2DUtil
  mpG2DUtil = GFL_NEW(mHeapMem) app::util::G2DUtil( &mHeap );

  // レイアウト
  mpG2DUtil->CreateLayoutSystem( &mHeap );
  mpG2DUtil->CreateAppLytAccessor( &mHeap );
  mpG2DUtil->CreateLayoutEnv( &mHeap, false );
  {
    // リソースデータ
    const app::util::G2DUtil::LytResData res_tbl[] =
    {
      { mpLytResBuff, 0, app::util::G2DUtil::ATTACH_ROOT },
    };
    // レイアウトデータ
	  const app::util::G2DUtil::LytwkData data_tbl[] =
    {
      {
        0,
        LYTRES_DEBUGFONTVIEWER_FONT_VIEWER_UPPER_BFLYT,
        LA_DEBUGFONTVIEWER_FONT_VIEWER_UPPER___NUM,
        mpLytResBuff,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
      {
        0,
        LYTRES_DEBUGFONTVIEWER_FONT_VIEWER_LOWER_BFLYT,
        LA_DEBUGFONTVIEWER_FONT_VIEWER_LOWER___NUM,
        mpLytResBuff,
        false,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };
    mpG2DUtil->CreateLayoutWork( res_tbl, GFL_NELEMS(res_tbl), LYTID_MAX, &mHeap );
    mpG2DUtil->AttachLayoutResourceReference( res_tbl, GFL_NELEMS(res_tbl) );
    mpG2DUtil->SetLayoutWork( 0, data_tbl, GFL_NELEMS(data_tbl), mHeapAllocater, mDeviceAllocater );
  }

  // テキスト
  mWordSet = GFL_NEW(mHeapMem) print::WordSet(mHeapMem);
  mMsgData = GFL_NEW(mHeapMem) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_ncl_DAT, mHeapMem );
  mStrBuf = GFL_NEW(mHeapMem) gfl2::str::StrBuf( 2048, mHeapMem );
  mExpandBuf = GFL_NEW(mHeapMem) gfl2::str::StrBuf( 2048, mHeapMem );
  mpG2DUtil->SetMessageData( mMsgData, mWordSet );

  // 操作説明
  mpG2DUtil->SetTextBoxPaneString( LYTID_LOWER, PANENAME_FONT_VIEWER_LOWER_PANE_INSTRUCTION, FONT_VIEWER_INSTRUCTION );

  setIndexMode(INDEXMODE_DEFINED);
  setIndex(0);
  updateText();

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result FontViewer::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  switch( GetSubSeq() )
  {
  case 0:
    if( mpG2DUtil->IsDrawing() != false )
    {
      break;
    }
    // テキスト
    GFL_DELETE mStrBuf;
    GFL_DELETE mExpandBuf;
    GFL_DELETE mMsgData;
    GFL_DELETE mWordSet;
    // レイアウト
    mpG2DUtil->DeleteLayoutWork();
    mpG2DUtil->DeleteLayoutEnv();
    mpG2DUtil->DeleteAppLytAccessor();
    mpG2DUtil->DeleteLayoutSystem();
    // G2DUtil
    GFL_DELETE mpG2DUtil;
    // レイアウトリソース
    GflHeapFreeMemory( mpLytResBuff );
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( mpRenderingManager->CanDestroy() == false )
    {
      break;
    }
    // 描画マネージャ
    GFL_DELETE( mpRenderingManager );
    // ヒープ
    mHeap.Delete();
    GFL_DELETE_HEAP( mpManagerHeap );
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

//--------------------------------------------------------------------------
//  現在のモードの文字インデックスをUnicodeに変換
//--------------------------------------------------------------------------
gfl2::str::STRCODE FontViewer::indexToCode(int index)
{
  switch (m_IndexMode)
  {
  case INDEXMODE_DEFINED:
    GFL_ASSERT(index < GFL_NELEMS(DefinedCode));
    return DefinedCode[index];
  case INDEXMODE_USERINPUT:
    GFL_ASSERT(index < GFL_NELEMS(UserInputCode));
    return UserInputCode[index];
  default:
    GFL_ASSERT(0);
  }
  return 0;
}

//--------------------------------------------------------------------------
//  現在のモードの文字インデックス数を得る
//--------------------------------------------------------------------------
int FontViewer::getIndexCount()
{
  switch (m_IndexMode)
  {
  case INDEXMODE_DEFINED:
    return GFL_NELEMS(DefinedCode);
  case INDEXMODE_USERINPUT:
    return GFL_NELEMS(UserInputCode);
  default:
    GFL_ASSERT(0);
  }
  return 0;
}

//--------------------------------------------------------------------------
//  文字インデックスモードの取得
//--------------------------------------------------------------------------
FontViewer::IndexMode FontViewer::getIndexMode()
{
  return m_IndexMode;
}

//--------------------------------------------------------------------------
//  文字インデックスモードの変更
//--------------------------------------------------------------------------
void FontViewer::setIndexMode(IndexMode new_mode)
{
  GFL_ASSERT(0 <= new_mode && new_mode <= INDEXMODE_MAX);

  m_IndexMode = new_mode;
}

//--------------------------------------------------------------------------
//  表示する文字インデックスをnew_beginからにする
//--------------------------------------------------------------------------
void FontViewer::setIndex(int new_begin)
{
  const int index_count = getIndexCount();
  int new_end;

  // 終了インデックスの計算
  new_end = new_begin + VIEWBODY_INDEXCOUNT;
  if (new_end > index_count)
  {
    new_end = index_count;
  }

  m_IndexBegin = new_begin;
  m_IndexEnd = new_end;
}

//--------------------------------------------------------------------------
//  表示する文字インデックスをdiffだけずらす
//--------------------------------------------------------------------------
void FontViewer::moveIndex(int diff)
{
  const int index_count = getIndexCount();
  int new_begin = m_IndexBegin + diff;

  if (new_begin < 0)
  {
    // 最後のページ
    new_begin = ((index_count-1) / VIEWBODY_INDEXCOUNT) * VIEWBODY_INDEXCOUNT;
  }
  else if (new_begin >= index_count)
  {
    // 最初のページ
    new_begin = 0;
  }
  GFL_ASSERT((0 <= new_begin) && (new_begin < index_count));

  setIndex(new_begin);
}

void FontViewer::updateText()
{
  // 選択中のコードの文字列
  {
    mStrBuf->Clear();
    for (int index = m_IndexBegin; index < m_IndexEnd; ++index)
    {
      mStrBuf->AddCode(indexToCode(index));
      if ((index % 16) == 15)
      {
        mStrBuf->AddCode(gfl2::str::CR_CODE);
      }
    }
    mpG2DUtil->SetTextBoxPaneString( LYTID_UPPER, PANENAME_FONT_VIEWER_UPPER_PANE_SYSFONT, mStrBuf );
    mpG2DUtil->SetTextBoxPaneString( LYTID_UPPER, PANENAME_FONT_VIEWER_UPPER_PANE_BATTLEFONT, mStrBuf );
  }

  // 文字セット
  mpG2DUtil->SetTextBoxPaneString( LYTID_LOWER, PANENAME_FONT_VIEWER_LOWER_PANE_INDEXMODE, FONT_VIEWER_INDEXMODE_00+getIndexMode() );

  // コード
  mpG2DUtil->SetRegisterNumber( 0, indexToCode(m_IndexBegin), 5 );
  mpG2DUtil->SetRegisterNumber( 1, indexToCode(m_IndexEnd-1), 5 );
  mpG2DUtil->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_FONT_VIEWER_LOWER_PANE_CODERANGE, FONT_VIEWER_CODERANGE );

  // ページ番号
  mpG2DUtil->SetRegisterNumber( 0, m_IndexBegin/VIEWBODY_INDEXCOUNT+1, 3 );
  mpG2DUtil->SetRegisterNumber( 1, getIndexCount()/VIEWBODY_INDEXCOUNT+1, 3 );
  mpG2DUtil->SetTextBoxPaneStringExpand( LYTID_LOWER, PANENAME_FONT_VIEWER_LOWER_PANE_PAGE, FONT_VIEWER_PAGE );
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメインを行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result FontViewer::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::ui::Button *uiButton = mDeviceManager->GetButton(0);

  bool isUpdateRequested = false;

  // 表示文字、戻る
  if( uiButton->IsTrigger( gfl2::ui::DIR_UP ) )
  {
    moveIndex(-CODEMOVE_COUNT);
    isUpdateRequested = true;
  }
  // 表示文字、進む
  if( uiButton->IsTrigger( gfl2::ui::DIR_DOWN ) || uiButton->IsHold( gfl2::ui::BUTTON_A ))
  {
    moveIndex(CODEMOVE_COUNT);
    isUpdateRequested = true;
  }

  // 文字セット切り替え
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_Y) )
  {
    switch (getIndexMode())
    {
    case INDEXMODE_DEFINED:
      setIndexMode(INDEXMODE_USERINPUT);
      break;
    case INDEXMODE_USERINPUT:
    default:
      setIndexMode(INDEXMODE_DEFINED);
      break;
    }
    setIndex(0);
    isUpdateRequested = true;
  }

  if (isUpdateRequested)
  {
    updateText();
  }

  mpG2DUtil->Update2D();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    return gfl2::proc::RES_FINISH;
  }

  return gfl2::proc::RES_CONTINUE;
}

void FontViewer::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
  mpG2DUtil->AddDrawableLytWkForOneFrame( mpRenderingManager, displayNo, LYTID_UPPER );
  mpG2DUtil->AddDrawableLytWkForOneFrame( mpRenderingManager, displayNo, LYTID_LOWER );
  mpRenderingManager->Draw( displayNo );
}

void FontViewer::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  mpRenderingManager->PreDraw();
}


GFL_NAMESPACE_END(Debug)

#endif // PM_DEBUG

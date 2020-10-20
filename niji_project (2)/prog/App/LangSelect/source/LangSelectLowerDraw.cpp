//======================================================================
/**
 * @file    LangSelectLowerDraw.cpp
 * @date    2015/07/23 15:37:17
 * @author  fukushima_yuya
 * @brief   言語選択：メイン画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectLowerDraw.h"
#include "App/LangSelect/source/LangSelectDrawerBase.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <Print/include/WordSet.h>
#include <Print/include/SystemFont.h>
#include <sound/include/Sound.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <niji_conv_header/app/lang_select/lang_select.h>
#include <niji_conv_header/app/lang_select/lang_select_pane.h>
#include <niji_conv_header/app/lang_select/lang_select_anim_list.h>
#include <niji_conv_header/message/msg_language_select.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
LangSelectLowerDraw::LangSelectLowerDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager )
  : app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRenderingManager )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pAppParam( NULL )
  , m_pDrawerBase( NULL )
  , m_pMsgParts( NULL )
  , m_pTextBox( NULL )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam       外部設定パラメータ
 * @param   pAppLytBuff     レイアウトバイナリ
 * @param   msgData         メッセージデータ
 * @param   wordSet         ワードセット
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet )
{
  m_pAppParam = pAppParam;

  m_pMsgData = msgData;
  m_pWordSet = wordSet;

  m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( pAppLytBuff );
  InitializeText();

  SetInputListener( this );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool LangSelectLowerDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::Update( void )
{
  app::ui::UIView::Update();

  // シーケンス処理
  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ステータス画面を開く
      LangSelectOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim( true ) ) break;
      
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
 
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//-----------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 */
//-----------------------------------------------------------------------------
bool LangSelectLowerDraw::IsReady( void )
{
  return m_Seq == SEQ_MAIN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool LangSelectLowerDraw::IsExit( void )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のオープン
 */
//-----------------------------------------------------------------------------
void LangSelectLowerDraw::LangSelectOpen( void )
{
  if( m_pFadeManager ) m_pFadeManager->RequestIn( gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, gfl2::Fade::FADE_DEFAULT_SYNC );

  m_Seq = SEQ_IN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面のクローズ
 */
//-----------------------------------------------------------------------------
void LangSelectLowerDraw::LangSelectClose( void )
{
  m_Seq = SEQ_OUT;
}

//-----------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//-----------------------------------------------------------------------------
bool LangSelectLowerDraw::IsEndInOutAnim( bool in )
{
  if( m_pFadeManager )
  {
    return m_pFadeManager->IsEnd( gfl2::Fade::DISP_LOWER );
  }

  return true;
}



//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::Initialize2D( void* pAppLytBuff )
{
  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();

  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_LANG_SELECT_LANGSELECT_MIN_LOW_BFLYT,
      LA_LANG_SELECT_LANGSELECT_MIN_LOW___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    m_pMsgData,
    m_pWordSet );

  this->SetInputEnabled( false );

  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, m_pWordSet, m_pMsgData );
    GFL_ASSERT( m_pDrawerBase );

    // メッセージボックスのパーツ
    m_pMsgParts = m_pDrawerBase->GetParts( PANENAME_LANGSELECT_MIN_LOW_PANE_MESSAGE );
    m_pDrawerBase->SetVisiblePane( m_pMsgParts, true );
    // メッセージボックスのテキストボックス
    m_pTextBox = m_pDrawerBase->GetTextBox( PANENAME_COMMON_MSG_LOW_006_PANE_TEXTBOX_00, m_pMsgParts );
    m_pDrawerBase->SetVisiblePane( m_pTextBox, false );

    // 背景アニメの再生
    m_pDrawerBase->StartAnim( LA_LANG_SELECT_LANGSELECT_MIN_LOW__BG_00_BG_LOOP_00 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   初期テキスト表示
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::InitializeText( void )
{

}

//------------------------------------------------------------------------------
/**
 * @brief   セーブデータの状態メッセージの表示
 * @param   status    SAVE_STATUS
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetSaveDataStatusMessage( SAVE_STATUS status )
{
  static const s32 c_SaveDataStatusMessageID[] = {
    -1,
    savedata_check_guide_01,
    language_select_error_01,
    language_select_error_04,
    language_select_error_05,
    savedata_check_guide_01,
  };

  // メッセージの表示
  SetMessage( c_SaveDataStatusMessageID[status] );
}

//------------------------------------------------------------------------------
/**
 * @brief   拡張セーブデータの状態メッセージの表示
 * @param   status    EX_SAVE_STATUS
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetExSaveDataStatusMessage( EX_SAVE_STATUS status )
{
  static const s32 c_ExSaveDataStatusMessageID[] = {
    -1,
    savedata_check_guide_01,
    language_select_error_02,
    language_select_error_03,
  };

  s32 msgID = c_ExSaveDataStatusMessageID[status];

  // メッセージの表示
  SetMessage( msgID );
}

//------------------------------------------------------------------------------
/**
 * @brief   拡張セーブデータのエラーメッセージ：基本
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetExSaveDataErrorMessage_Basic( void )
{
  // メッセージの設定
  SetMessage( language_select_error_03 );
}

//------------------------------------------------------------------------------
/**
 * @brief   拡張セーブデータのエラーメッセージ：容量不足
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetExSaveDataErrorMessage_Shortage( void )
{
  // メッセージの設定
  SetMessage( language_select_error_06 );
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージの表示
 * @param   msgID   メッセージID
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetMessage( s32 msgID )
{
  bool vis = (msgID != -1);

  if( vis )
  {
    // メッセージのセット
    this->SetTextboxPaneMessage( m_pTextBox, msgID );
  }

  // 表示切替
  m_pDrawerBase->SetVisiblePane( m_pTextBox, vis );
}

//------------------------------------------------------------------------------
/**
 * @brief   言語選択メッセージの表示
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetLangSelectMessage( void )
{
  SetMessage( language_select_guide_01 );
}

//------------------------------------------------------------------------------
/**
 * @brief   言語決定確認メッセージの表示
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetLangDecideConfirmMessage( void )
{
  SetMessage( language_select_guide_02 );
}

//------------------------------------------------------------------------------
/**
 * @brief   言語決定確認メッセージの表示(日本語版)
 */
//------------------------------------------------------------------------------
void LangSelectLowerDraw::SetLangDecideJpnConfirmMessage( void )
{
  SetMessage( language_select_guide_03 );
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ関連のデータを再設定
 * @param   msg_data  メッセージデータ
 * @param   word_set  単語セット
 */
//------------------------------------------------------------------
void LangSelectLowerDraw::ResetMessage( gfl2::str::MsgData * msg_data, print::WordSet * word_set )
{
  m_pMsgData = msg_data;
  m_pWordSet = word_set;
  GetG2DUtil()->SetMessageData( m_pMsgData, m_pWordSet );
}


GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

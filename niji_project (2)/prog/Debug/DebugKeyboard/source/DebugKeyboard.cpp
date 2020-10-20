
#if PM_DEBUG

//=============================================================================================
/**
 * @file  DebugKeyboard.cpp
 * @brief ポケモンXY デバッグキーボード
 * @author  taya
 *
 * @date  2011.11.14
 */
//=============================================================================================
#include <System/include/GflUse.h>
#include <System/include/PokemonVersion.h>
#include <System/include/PokemonLanguage.h>
#include <System/include/nijiAllocator.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include <Print/include/PrintSystem.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/debug_message.gaix>
#include <arc_index/debug_keyboard.gaix>
#include <niji_conv_header/app/debug/debug_keyboard/res2d/debug_keyboard.h>
#include <niji_conv_header/app/debug/debug_keyboard/res2d/debug_keyboard_anim_list.h>
#include <niji_conv_header/app/debug/debug_keyboard/res2d/debug_keyboard_pane.h>

#include <NetStatic/NetAppLib/include/Util/NetAppLatinumStringTool.h>
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>

#include <System/include/Timer/gfl2_PerformanceCounter.h>

namespace debug {

//------------------------------------------------------
// ctor / dtor
//------------------------------------------------------
KeyboardProc::KeyboardProc( void ) :
  m_sysHeapBase(NULL),
  m_devHeapBase(NULL),
  m_devHeap(NULL),
  m_lytSys(NULL),
  m_wordComp(NULL),
  m_wordCompDataID(-1),
  m_prevChangeColorButton( BTN_NULL ),
  m_bExpandSearch(false),
  m_colorNormal(255,255,255,255),
  m_colorSelect(69,87,251,255),
  m_default_inputmode(INPUT_HIRAGANA),
  m_default_hankaku(false)
{
}

KeyboardProc::~KeyboardProc()
{
}
/// msgData から補間用の単語を探す
void  KeyboardProc::SetSearchWords( gfl2::str::MsgData* msgData, int* index, bool expand_search )
{
  if( (m_wordCompDataID==-1) && (m_wordComp==NULL) )
  {
    m_wordComp = msgData;
    m_wordIndexOut = index;
    m_bExpandSearch = expand_search;
  }
}
/// ARCID_MESSAGEの中から、指定 dataID のファイルを探し、補間用の単語データにする
void  KeyboardProc::SetSearchWords( u32 dataID, int* index, bool expand_search )
{
  if( (m_wordCompDataID==-1) && (m_wordComp==NULL) )
  {
    m_wordCompDataID = dataID;
    m_wordIndexOut = index;
    m_bExpandSearch = expand_search;
  }
}

void  KeyboardProc::SetDefaultInputMode(InputMode mode, bool hankaku)
{
  if( mode == INPUT_HIRAGANA || mode == INPUT_KATAKANA )
  {
    if( System::GetLang() != POKEMON_LANG_JAPAN )
      {
        // ひらがな・カタカナが許されるのは日本語版のみ
        mode = INPUT_ALPHA;
        hankaku  = true;
      }
    else
    {
      hankaku = false;
    }
  }
  m_default_inputmode = mode;
  m_default_hankaku = hankaku;
}

gfl2::proc::Result KeyboardProc::InitFunc( gfl2::proc::Manager* pManager )
{
  if( (m_sysHeapBase != NULL)
  &&  (m_devHeapBase != NULL)
  ){
    static const u32 DEVICE_HEAP_SIZE = 0x100000;
    m_devHeap  = GFL_CREATE_LOCAL_HEAP_NAME( m_devHeapBase, DEVICE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "HEAPID_DEBUG_KBD_DEV" );

    m_uiMan    = mpGameProcManager->GetGameManager()->GetUiDeviceManager();
    m_uiTp     = m_uiMan->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
    m_uiButton = m_uiMan->GetButton( 0 );
    m_uiKey    = m_uiMan->GetCrossKey( 0 );

/*
    m_uiButton = m_uiMan->GetButton( 0 );
    m_uiKey    = m_uiMan->GetCrossKey( 0 );
    m_strBuf    = GFL_NEW(m_devHeap) gfl2::str::StrBuf( 512, m_devHeap );
*/


    m_strBufTmp     = GFL_NEW(m_devHeap) gfl2::str::StrBuf( m_strBuf->GetSize()+4, m_devHeap );
    m_strBufToUpper = GFL_NEW(m_devHeap) gfl2::str::StrBuf( m_strBuf->GetSize()+4, m_devHeap );
    m_strBufWork1 = GFL_NEW(m_devHeap) gfl2::str::StrBuf( m_strMaxLen+4, m_devHeap );
    m_strBufWork2 = GFL_NEW(m_devHeap) gfl2::str::StrBuf( m_strMaxLen+4, m_devHeap );

    u32 lang = System::GetLang();

    if( (m_wordComp == NULL) && (m_wordCompDataID >= 0) )
    {
      /**
       * CBBTS435: デバッグキーボードは韓国語入力ができないので、言語コードが韓国語の場合はひらがなメッセージで探索を行う
       */
//      if( lang == POKEMON_LANG_KOREA ){
//        m_wordComp = GFL_NEW(m_devHeap) gfl2::str::MsgData( print::GetMessageArcId(System::MSGLANGID_KANA), m_wordCompDataID, m_devHeap, gfl2::str::MsgData::LOAD_FULL );
      // NContactMCat[212]: 韓国/中国/台湾では数字の文字列テーブルを使用する
      if( lang == POKEMON_LANG_KOREA || lang == POKEMON_LANG_CHINA || lang == POKEMON_LANG_TAIWAN ){
        m_wordComp = GFL_NEW(m_devHeap) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_numbertbl_DAT, m_devHeap, gfl2::str::MsgData::LOAD_FULL );
      }else{
        m_wordComp = GFL_NEW(m_devHeap) gfl2::str::MsgData( print::GetMessageArcId(), m_wordCompDataID, m_devHeap, gfl2::str::MsgData::LOAD_FULL );
      }
    }


    this->createAllocator();
    this->createRenderingPipeLine();
    this->init2D();
    this->setupFlickPanes();

    m_state = STATE_FREE;
    m_seq = 0;
    m_bWordDisped = false;
    m_hitButton = BTN_NULL;
    m_releasedButton = BTN_NULL;
    m_isDrawEnable = true;


    m_bHankaku = m_default_hankaku;
    if( lang != POKEMON_LANG_JAPAN && lang != POKEMON_LANG_KOREA && lang != POKEMON_LANG_CHINA && lang != POKEMON_LANG_TAIWAN )
    {
      m_default_inputmode = INPUT_ALPHA;
      m_bHankaku  = true;
    }

    changeInputMode( m_default_inputmode );
    renewZenHanCaption();

    this->setupSearchWordPanes();
    this->updateString( true );
  }
  else
  {
    GFL_ASSERT(0);
  }

  return gfl2::proc::RES_FINISH;
}
gfl2::proc::Result KeyboardProc::EndFunc( gfl2::proc::Manager* pManager )
{
  // 描画中は解放しない
  if( ( m_lytWk != NULL ) &&
      ( m_lytWk->IsDrawing() ) ) {
    return gfl2::proc::RES_CONTINUE;
  }

  if( m_strBufWork1 ){ GFL_DELETE m_strBufWork1; }
  if( m_strBufWork2 ){ GFL_DELETE m_strBufWork2; }
  if( m_strBufTmp ){ GFL_DELETE m_strBufTmp; }
  if( m_strBufToUpper ){ GFL_DELETE m_strBufToUpper; }

  if( m_wordComp )
  {
    // 自前ヒープに確保した場合のみ自分で削除
    if( m_wordCompDataID >= 0 ){
      GFL_DELETE m_wordComp;
    }
  }

  this->quit2D();
  this->deleteRenderingPipeLine();
  this->deleteAllocator();

  if( m_devHeap ){
    gfl2::heap::Manager::DeleteHeap( m_devHeap );
  }

  return gfl2::proc::RES_FINISH;
}

/**
 * @brief アロケータを生成する
 */
void KeyboardProc::createAllocator( void )
{
  m_sysAllocator   = GFL_NEW(m_devHeap)  gfl2::heap::NwAllocator(m_devHeap);
  m_devAllocator   = GFL_NEW(m_devHeap)  gfl2::heap::NwAllocator(m_devHeap);
}

/**
 * @brief レンダリングパイプラインを生成する
 */
void KeyboardProc::createRenderingPipeLine( void )
{
  m_renderingPipeLine = GFL_NEW(m_devHeap) AppRenderingPipeLine( m_devHeap, 512, 256 );
}

/**
 * @brief レンダリングパイプラインを削除する
 */
void KeyboardProc::deleteRenderingPipeLine( void )
{
  GFL_SAFE_DELETE( m_renderingPipeLine );
}

/**
 *  @brief アロケータを削除する
 */
void KeyboardProc::deleteAllocator( void )
{
  if( m_devAllocator ){
    GFL_DELETE  m_devAllocator;
    m_devAllocator = NULL;
  }
  if( m_sysAllocator ){
    GFL_DELETE  m_sysAllocator;
    m_sysAllocator = NULL;
  }
}

/**
 *  2Dリソース初期化
 */
void KeyboardProc::init2D( void )
{
  // レイアウトシステム生成
  {
    const gfl2::lyt::LytSysSetUp SetUp = {
	    GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
      10, // lyt_resource_max
      10  // lyt_wk_max
    };

    // gfl2::lyt::LytSys::SetDefaultFont( SystemFont_GetFont(), print::LAYOUT_DEFAULT_FONT ); 不要になったためコメントアウト tomoya  

    if( m_lytSys == NULL ){
      m_lytSys = GFL_NEW(m_devHeap) gfl2::lyt::LytSys( &SetUp, m_sysAllocator, m_devAllocator, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );
      m_bLytSysCreateSelf = true;
    }else{
      m_bLytSysCreateSelf = false;
    }

    {
      static const gfl2::lyt::LytWkSetUp LAYOUT_WORK_SETUP_DATA = 
      {
        gfl2::math::Vector3( 0, -290, 0 ),  // pos
        gfl2::math::Vector2( 1, 1 ),        // scale
        gfl2::math::Vector3( 0, 0, 0 ),     // rotate
        0,                              // priority
        0,
      };

      static const gfl2::lyt::LytArcIndex animTable[] = {
        LYTRES_DEBUG_KEYBOARD_DEBUG_KEYBOARD_BFLAN,
      };

      app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_KEYBOARD, m_devHeap );
      app::util::FileAccessor::FileLoadSync( ARCID_DEBUG_KEYBOARD, GARC_debug_keyboard_debug_keyboard_APPLYT, &m_lytDataBuffer, m_devHeap, false, 128 );
      app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_KEYBOARD );

      app::util::AppLytAccessor* accessor = GFL_NEW_LOW( m_devHeap ) app::util::AppLytAccessor();
      accessor->SetAppLytBuf( m_lytDataBuffer );

      m_lytResID = m_lytSys->LoadLytRes( NULL, m_devAllocator, accessor->GetLytDat() );
      m_lytWk  = m_lytSys->CreateLytWk( &m_lytResID, LYTRES_DEBUG_KEYBOARD_DEBUG_KEYBOARD_BFLYT, animTable, GFL_NELEMS(animTable), GFL_NELEMS(animTable), &LAYOUT_WORK_SETUP_DATA );
      m_lytWk->SetAutoDrawFlag( true );
      m_lytWk->SetAutoAnime( 0, true );

      m_cursorPane = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_CURSOR );
      m_stringPane = m_lytWk->GetTextBoxPane( PANENAME_DEBUG_KEYBOARD_PANE_INPUTSTR );

      m_btnMaskPane[ BTNCOLOR_LIGHT ]  = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_MASK_LIGHT );
      m_btnMaskPane[ BTNCOLOR_MIDDLE ] = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_MASK_MID );
      m_btnMaskPane[ BTNCOLOR_DARK ]   = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_MASK_DARK );

      GFL_SAFE_DELETE( accessor );
    }
  }


}
/**
 *  2Dリソース破棄
 */
void KeyboardProc::quit2D( void )
{
  if( m_lytSys )
  {
    if( m_lytWk )
    {
      m_lytSys->RemoveLytWk( m_lytWk );
      m_lytWk = NULL;
    }

    m_lytSys->ReleaseLytRes( &m_lytResID );

    if( m_bLytSysCreateSelf ){
      GFL_DELETE  m_lytSys;
    }
    m_lytSys = NULL;
  }
  GflHeapSafeFreeMemory( m_lytDataBuffer );
}
//========================================================================================
/**
 *  Update
 */
//========================================================================================
gfl2::proc::Result KeyboardProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  m_lytSys->UpdateAll();

  checkTouchWord();

  switch( m_state ){
  case STATE_FREE:
    if( proc_Free() ){
      m_state = STATE_EXIT;
    }
    break;

  case STATE_1ST_TOUCH:
    proc_1st_touch();
    break;

  case STATE_NORMAL_RELEASE:
    proc_normal_release();
    break;

  case STATE_FLICK_LOW:
    proc_flick_low();
    break;

  case STATE_FLICK_HIGH:
    proc_flick_high();
    break;

  case STATE_BACKSPACE:
    proc_back_space();
    break;

  case STATE_KANA_FUNC:
  case STATE_ALPHA_FUNC:
    proc_input_func();
    break;

  case STATE_EXIT:
    m_isDrawEnable = false;
    if( m_lytWk->IsDrawing() ) {
      break;
    }
    return gfl2::proc::RES_FINISH;
  }

  if( m_renderingPipeLine && m_isDrawEnable ) {
    m_renderingPipeLine->Update();
    m_renderingPipeLine->GetLayoutRenderPath()->ClearDrawableLytWk();
    m_renderingPipeLine->GetLayoutRenderPath()->AddDrawableLytWk( m_lytWk, gfl2::lyt::DISPLAY_LOWER );
  }

  return gfl2::proc::RES_CONTINUE;
}
void KeyboardProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if( ( displayNo != gfl2::gfx::CtrDisplayNo::DOWN ) ||
     !( m_isDrawEnable ) ) {
    return;
  }

  //if( m_graphicSys )
  {
    this->drawLyt();
  }
}
/**
 *  Layout system 描画
 */
void KeyboardProc::drawLyt( void )
{
#if 0
  gfl::grp::RenderTarget* upperFrameBuffer = m_graphicSys->GetCurrentFrameBuffer( gfl::grp::DISPLAY_UPPER );
  gfl::grp::RenderTarget* rightFrameBuffer = m_graphicSys->GetCurrentFrameBuffer( gfl::grp::DISPLAY_UPPER_RIGHT );
  gfl::grp::RenderTarget* lowerFrameBuffer = m_graphicSys->GetCurrentFrameBuffer( gfl::grp::DISPLAY_LOWER );

#if 1
  m_lytSys->DrawAll( m_graphicSys, gfl::grp::DISPLAY_UPPER, upperFrameBuffer, rightFrameBuffer );
  m_lytSys->DrawAll( m_graphicSys, gfl::grp::DISPLAY_LOWER, lowerFrameBuffer );
#else
//  m_lytWk->Draw( m_graphicSys, gfl::grp::DISPLAY_UPPER,       upperFrameBuffer );
//  m_lytWk->Draw( m_graphicSys, gfl::grp::DISPLAY_UPPER_RIGHT, rightFrameBuffer );
  m_lytWk->Draw( m_graphicSys, gfl::grp::DISPLAY_LOWER, lowerFrameBuffer );
#endif
  if( m_renderingPipeLine == NULL ) {
      return;
    }
#endif

  gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::DOWN ];
  gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
  gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
  u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
  u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
  m_renderingPipeLine->SetRenderTarget( pBackBuffer );
  m_renderingPipeLine->SetDepthStencil( pDepthBuffer );
  m_renderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
  m_renderingPipeLine->Execute();
}

/**
 *  state 切り替え
 */
void KeyboardProc::change_state( State next_state )
{
  m_state = next_state;
  m_seq = 0;
  m_stateTick = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();
}
// 切り替え後の経過時間リセット
void KeyboardProc::resetStateTick( void )
{
  m_stateTick = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();
}
// 切り替え後の経過時間を取得
u64 KeyboardProc::getStateMilliSec( void )
{
  u64 begin = m_stateTick;
  u64 cur   = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();;
  return ( cur - begin );
}


/**
 *  proc - free
 */
bool KeyboardProc::proc_Free( void )
{
  ButtonID btnID;

  if( m_bConnectFree ){
    btnID = m_hitButton;
    m_bConnectFree = false;
  } else {
    btnID = checkTouchButton();
  }

/*
  if( btnID != BTN_NULL ){
    GFL_PRINT( "ボタン触ったでござる -> btnID=%d\n", btnID );
  }
*/

  switch( btnID ){
  case   BTN_MODE_NUMBER:   changeInputMode( INPUT_NUMBER );   changeButtonAllFreeColor(); break;
  case   BTN_MODE_ALPHA:    changeInputMode( INPUT_ALPHA );    changeButtonAllFreeColor(); break;
  case   BTN_MODE_HIRAGANA: changeInputMode( INPUT_HIRAGANA ); changeButtonAllFreeColor(); break;
  case   BTN_MODE_KATAKANA: changeInputMode( INPUT_KATAKANA ); changeButtonAllFreeColor(); break;

  case   BTN_BACKSPACE:
    changeButtonAllFreeColor();
    change_state( STATE_BACKSPACE );
    break;

  case   BTN_SPACE:
    changeButtonAllFreeColor();
    putSpace();
    break;

  case   BTN_OK:
    if( m_wordIndexOut )
    {
      *m_wordIndexOut = getSearchWordIndex();
    }
    return true;

  case BTN_LET9:
    if( (m_inputMode == INPUT_HIRAGANA)
    ||  (m_inputMode == INPUT_KATAKANA)
    ){
      change_state( STATE_KANA_FUNC );
    }
    else if( m_inputMode == INPUT_ALPHA )
    {
      change_state( STATE_ALPHA_FUNC );
    }
    break;

  default:
    if( (btnID >= BTN_LET0) && (btnID <= BTN_LET11) )
    {
      m_hitButton = btnID;
      m_hitContCount = 0;
      m_touchX = m_uiTp->GetX();
      m_touchY = m_uiTp->GetY();
      change_state( STATE_1ST_TOUCH );
    }
    break;
  }
  if( m_strBuf->GetLength() > 0 &&
      m_wordComp )
  {
    if( m_wordStartIdx > 0 ){
      if( m_uiButton->IsRepeat(gfl2::ui::BUTTON_LEFT) || m_uiButton->IsRepeat(gfl2::ui::BUTTON_L)){
        m_wordStartIdx--;
        dispSearchWords();
        //GFL_PRINT("SetOffset- %d\n",m_wordStartIdx);
      }
    }
    if( m_uiButton->IsRepeat(gfl2::ui::BUTTON_RIGHT) || m_uiButton->IsRepeat(gfl2::ui::BUTTON_R)){
      m_wordStartIdx++;
      dispSearchWords();
      //GFL_PRINT("SetOffset+ %d\n",m_wordStartIdx);
    }
  }

  return false;
}
/**
 *  proc - BACK SPACE
 */
void KeyboardProc::proc_back_space( void )
{
  switch( m_seq ){
  case 0:
    changeButtonColor( BTN_BACKSPACE, BTNCOLOR_LIGHT );
    backSpace();
    ++m_seq;
    break;
  case 1:
    if( checkTouchButton(true) == BTN_BACKSPACE )
    {
      if( getStateMilliSec() > 500 )
      {
        backSpace();
        resetStateTick();
      }
    }
    else{
      changeButtonColor( BTN_BACKSPACE, BTNCOLOR_DARK );
      change_state( STATE_FREE );
    }
  }
}
/**
 *  proc - 濁点半濁点や、小文字切り替えなどの便利機能ボタン
 */
void  KeyboardProc::proc_input_func( void )
{
  switch( m_seq ){
  case 0:
      {
        wchar_t  end_code = getEndCode();
        if( end_code == gfl2::str::EOM_CODE ){
          change_state( STATE_FREE );
          return;
        }
        wchar_t next_code = getFuncNextCode( end_code );
        if( next_code == end_code ){
          change_state( STATE_FREE );
          return;
        }
        changeButtonColor( BTN_LET9, BTNCOLOR_DARK );
        changeChar( next_code );
        ++m_seq;
      }
    break;
  case 1:
    if( checkTouchButton() != BTN_LET9 )
    {
      changeButtonColor( BTN_LET9, BTNCOLOR_LIGHT );
      change_state( STATE_FREE );
    }
  }

}

/**
 *  proc - 1st_touch
 */
void KeyboardProc::proc_1st_touch( void )
{
  if( m_uiTp->IsTouch() )
  {
    switch( m_seq ){
    case 0:
      changeButtonColor( m_hitButton, BTNCOLOR_DARK );
      ++m_seq;
      break;
    case 1:
      {
        ButtonID  btnID = checkTouchButton(true);

        if( btnID == m_hitButton )
        {
          if( getStateMilliSec() > 600 )
          {
            changeButtonColor( m_hitButton, BTNCOLOR_LIGHT );
            change_state( STATE_FLICK_LOW );
          }
        }
        else
        {
          changeButtonColor( m_hitButton, BTNCOLOR_LIGHT );
          m_touchX = m_uiTp->GetX();
          m_touchY = m_uiTp->GetY();
          change_state( STATE_FLICK_HIGH );
        }
      }
      break;
    }
  }
  else
  {
    change_state( STATE_NORMAL_RELEASE );
  }
}
/**
 *  proc - 1st_touch から普通にリリース（ケータイ打ちへ）
 */
void KeyboardProc::proc_normal_release( void )
{
  enum {
    SEQ_START = 0,
    SEQ_CHECK_ROOT,
    SEQ_HOLD,
    SEQ_AFTER_CHANGE,
  };
  switch( m_seq ){
  // １文字目入力 > 色を中間色に変更
  case SEQ_START:
    putChar( getLetterCode(m_hitButton, m_hitContCount) );
    changeButtonColor( m_hitButton, BTNCOLOR_MIDDLE );
    m_seq = SEQ_CHECK_ROOT;
    break;

  // 判定ルート
  case SEQ_CHECK_ROOT:
    {
      ButtonID  btnID = checkTouchButton();

      // 中間色の間にボタンが押された
      if( btnID != BTN_NULL )
      {
        // 同じボタン > ホールド状態
        if( btnID == m_hitButton ){
          changeButtonColor( m_hitButton, BTNCOLOR_DARK );
          resetStateTick();
          m_seq = SEQ_HOLD;
        }
        // 違うボタン > 1st touch へ
        else{
          m_hitButton = btnID;
          m_hitContCount = 0;
          m_touchX = m_uiTp->GetX();
          m_touchY = m_uiTp->GetY();
          m_bConnectFree = true;
          change_state( STATE_FREE );
        }
      }
      // ボタンが押されず一定時間が経過 > ボタン色を元に戻して free 状態へ
      else
      {
        if( m_uiKey->IsTrigger(gfl2::ui::DIR_DOWN)
        ||  m_uiButton->IsTrigger(gfl2::ui::BUTTON_B)
        ||  (getStateMilliSec() > 1200)
        ){
          //changeButtonColor( m_hitButton, BTNCOLOR_LIGHT );
          changeButtonAllFreeColor();
          change_state( STATE_FREE );
        }
      }
    }
    break;

  // ボタンホールド状態
  case SEQ_HOLD:
    if( m_uiTp->IsTouch() )
    {
      ButtonID  btnID = checkTouchButton(true);

      // 同じボタンホールドし続け >> 一定期間継続なら低速フリックへ
      if( btnID == m_hitButton )
      {
        if( getStateMilliSec() > 600 ){
          changeButtonColor( m_hitButton, BTNCOLOR_LIGHT );
          change_state( STATE_FLICK_LOW );
        }
      }
      // ボタンから外れたがホールドし続け >> 高速フリックへ
      else{
        changeButtonColor( m_hitButton, BTNCOLOR_LIGHT );
        m_touchX = m_uiTp->GetX();
        m_touchY = m_uiTp->GetY();
        change_state( STATE_FLICK_HIGH );
      }
    }
    // リリースされた > 文字切り替え後、判定ルートへ
    else
    {
      changeButtonColor( m_hitButton, BTNCOLOR_MIDDLE );
      m_hitContCount = getNextLetterIdx( m_hitButton, m_hitContCount );
      changeChar( getLetterCode(m_hitButton, m_hitContCount) );
      resetStateTick();
      m_seq = SEQ_CHECK_ROOT;
    }
    break;
  }
}
/**
 *  proc - 1st_touch から一定時間押し（低速モードのフリックへ）
 */
void KeyboardProc::proc_flick_low( void )
{
  enum {
    SEQ_START = 0,
    SEQ_CHECK_ROOT,
    SEQ_HOLD,
    SEQ_AFTER_CHANGE,
  };

  switch( m_seq ){
  case SEQ_START:
    m_hitContCount = 0;
    switchLowFlickWin( true );
    drawFlickButtonFull( m_hitButton );
    m_seq = SEQ_CHECK_ROOT;
    break;

  case SEQ_CHECK_ROOT:
    if( m_uiTp->IsTouch() )
    {
       int x = m_uiTp->GetX();
       int y = m_uiTp->GetY();

       m_hitContCount = checkTouchFlickButtonIdx( x, y );
       updateFlickButtonSelect( m_hitContCount );
    }
    else
    {
      if( m_hitContCount < FLICK_LET_MAX ){
        putChar( getFlickLetterCode(m_hitButton, m_hitContCount) );
      }
      hideFlickButton();
      switchLowFlickWin( false );
      m_hitContCount = 0;
      change_state( STATE_FREE );
    }
  }

}
/**
 *  proc - 1st_touch から一定時間押し（低速モードのフリックへ）
 */
void KeyboardProc::proc_flick_high( void )
{
  enum {
    SEQ_START = 0,
    SEQ_CHECK_ROOT,
    SEQ_HOLD,
    SEQ_AFTER_CHANGE,
  };

  switch( m_seq ){
  case SEQ_START:
    m_hitContCount = checkFlickDirection( m_hitButton, m_touchX, m_touchY );
    drawFlickButtonSingle( m_hitButton, m_hitContCount );
    m_seq = SEQ_CHECK_ROOT;
    break;
  case SEQ_CHECK_ROOT:
    if( m_uiTp->IsTouch() )
    {
      m_touchX = m_uiTp->GetX();
      m_touchY = m_uiTp->GetY();
      m_hitContCount = checkFlickDirection( m_hitButton, m_touchX, m_touchY );
      drawFlickButtonSingle( m_hitButton, m_hitContCount );
    }
    else
    {
      if( m_hitContCount < FLICK_LET_MAX ){
        putChar( getFlickLetterCode(m_hitButton, m_hitContCount) );
      }
      hideFlickButton();
      m_hitContCount = 0;
      change_state( STATE_FREE );
    }
    break;
  }
}
/**
 *  タッチボタンID、現在タッチ位置から、フリックではじいた方向を返す
 */
u32 KeyboardProc::checkFlickDirection( ButtonID btnID, int tpX, int tpY )
{
  if( checkTouchButtonByPos( tpX, tpY ) == btnID ){
    return 0;
  }
  else{

    int direction;

    int diff_x = tpX - (BtnPosTable[btnID].x + (BtnPosTable[btnID].width / 2));
    int diff_y = tpY - (BtnPosTable[btnID].y + (BtnPosTable[btnID].height / 2));

    int abs_x = (diff_x > 0)? diff_x : -diff_x;
    int abs_y = (diff_y > 0)? diff_y : -diff_y;

    if( abs_x > abs_y )
    {
      direction = (diff_x < 0)? 1 : 3;
    }
    else
    {
      direction = (diff_y < 0)? 2 : 4;
    }

    // （念のため）文字コードが割り当てられていないIndexになってしまったら0を返す
    if( getFlickLetterCode(btnID, direction) == L'\0' ){
      direction = 0;
    }

    #if 0
    GFL_PRINT( "btn(%d,%d) tp(%d,%d)  diffX=%d, diffY=%d  dir=%d\n",
          BtnPosTable[btnID].x + (BtnPosTable[btnID].width / 2),
          BtnPosTable[btnID].y + (BtnPosTable[btnID].height / 2),
          tpX, tpY, direction );
    #endif

    return direction;
  }
}

/**
 *  フリックボタン関連ペインＩＤ
 */
const u16  KeyboardProc::FlickPaneIdx[FLICK_LET_MAX][3] = {
  { PANENAME_DEBUG_KEYBOARD_PANE_FLICK_C, PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SC, PANENAME_DEBUG_KEYBOARD_PANE_FLIC_LET_C },
  { PANENAME_DEBUG_KEYBOARD_PANE_FLICK_L, PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SL, PANENAME_DEBUG_KEYBOARD_PANE_FLIC_LET_L },
  { PANENAME_DEBUG_KEYBOARD_PANE_FLICK_U, PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SU, PANENAME_DEBUG_KEYBOARD_PANE_FLIC_LET_U },
  { PANENAME_DEBUG_KEYBOARD_PANE_FLICK_R, PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SR, PANENAME_DEBUG_KEYBOARD_PANE_FLIC_LET_R },
  { PANENAME_DEBUG_KEYBOARD_PANE_FLICK_D, PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SD, PANENAME_DEBUG_KEYBOARD_PANE_FLIC_LET_D },
};

/**
 *  フリック入力時に表示するペインをメンバー保存しておく
 */
void KeyboardProc::setupFlickPanes( void )
{
  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    m_flickBtnPane[i]    = m_lytWk->GetPane( FlickPaneIdx[i][0] );
    m_flickSelBtnPane[i] = m_lytWk->GetPane( FlickPaneIdx[i][1] );
    m_flickLetPane[i]    = m_lytWk->GetTextBoxPane( FlickPaneIdx[i][2] );
  }
}

/**
 *  低速フリックモード用にフリックボタン表示
 */
void KeyboardProc::drawFlickButtonFull( ButtonID btnID )
{
  gfl2::math::VEC3  pos[ FLICK_LET_MAX ];
  tpPosToLytPos( &BtnPosTable[btnID], &pos[0] );

  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    calcFlickBtnPos( i, pos[0], &pos[i] );

    wchar_t  code = getFlickLetterCode( btnID, i );
    if( code != L'\0' )
    {
      wchar_t str[2];
      str[0] = code;
      str[1] = L'\0';

      m_flickBtnPane[i]->SetTranslate( pos[i] );
      m_flickBtnPane[i]->SetVisible( true );

      m_flickLetPane[i]->SetTranslate( pos[i] );
      m_flickLetPane[i]->SetString( str );
      m_flickLetPane[i]->SetVisible( true );
    }
    else
    {
      m_flickBtnPane[i]->SetVisible( false );
      m_flickLetPane[i]->SetVisible( false );
    }
  }

  updateFlickButtonSelect( 0 );
}
/**
 * 高速フリックモード用にフリックボタン単一表示
 */
void KeyboardProc::drawFlickButtonSingle( ButtonID btnID, u32 idx )
{
  gfl2::math::VEC3  pos[ 2 ];
  tpPosToLytPos( &BtnPosTable[btnID], &pos[0] );

  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    if( i == idx )
    {
      wchar_t str[2];

      calcFlickBtnPos( i, pos[0], &pos[1] );
      str[0] = getFlickLetterCode( btnID, i );
      str[1] = L'\0';

      m_flickSelBtnPane[i]->SetTranslate( pos[1] );
      m_flickSelBtnPane[i]->SetVisible( true );

      m_flickLetPane[i]->SetTextColor( m_colorNormal, m_colorNormal );
      m_flickLetPane[i]->SetString( str );
      m_flickLetPane[i]->SetTranslate( pos[1] );
      m_flickLetPane[i]->SetVisible( true );
    }
    else
    {
      m_flickSelBtnPane[i]->SetVisible( false );
      m_flickBtnPane[i]->SetVisible( false );
      m_flickLetPane[i]->SetVisible( false );
    }
  }
}
/**
 *  フリックボタンを全て非表示
 */
void KeyboardProc::hideFlickButton( void )
{
  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    m_flickBtnPane[i]->SetVisible( false );
    m_flickSelBtnPane[i]->SetVisible( false );
    m_flickLetPane[i]->SetVisible( false );
  }
}
/**
 * フリックボタンの１つを選択状態に
 */
void KeyboardProc::updateFlickButtonSelect( u32 letIdx )
{
  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    if( i == letIdx )
    {
      m_flickSelBtnPane[i]->SetTranslate( m_flickBtnPane[i]->GetTranslate() );
      m_flickSelBtnPane[i]->SetVisible( true );
      m_flickLetPane[i]->SetTextColor( m_colorNormal, m_colorNormal );
    }
    else
    {
      m_flickSelBtnPane[i]->SetVisible( false );
      m_flickLetPane[i]->SetTextColor( m_colorSelect, m_colorSelect );
    }
  }
}
/**
 *  タッチしているフリックボタンのIndexを返す
 */
u32  KeyboardProc::checkTouchFlickButtonIdx( int x, int y )
{
  for(u32 i=0; i<FLICK_LET_MAX; ++i)
  {
    if( m_lytWk->GetHitPane(x, y, FlickPaneIdx[i][0]) ){
      return i;
    }
    if( m_lytWk->GetHitPane(x, y, FlickPaneIdx[i][1]) ){
      return i;
    }
  }
  return FLICK_LET_MAX;
}

void KeyboardProc::calcFlickBtnPos( u32 idx, const gfl2::math::VEC3& centerPos, gfl2::math::VEC3* dst )
{
  gfl2::math::VEC3 pos = centerPos;
  switch( idx ){
  case 1: pos.x -= 50; break;
  case 2: pos.y += 44; break;
  case 3: pos.x += 51; break;
  case 4: pos.y -= 45; break;
  }
  *dst = pos;
}

/**
 *  低速フリックモードのマスクウィンドウ表示／非表示
 */
void KeyboardProc::switchLowFlickWin( bool bVisible )
{
  gfl2::lyt::LytPane* pane = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_FLICK_SHADOW );
  if( pane )
  {
    pane->SetVisible( bVisible );
  }
}

/**
 *  文字ボタンの色変更
 */
void KeyboardProc::changeButtonColor( ButtonID btnID, ButtonColor colorID )
{
  if( (btnID != m_prevChangeColorButton) && (m_prevChangeColorButton != BTN_NULL) )
  {
    changeButtonColorCore( m_prevChangeColorButton, BtnPosTable[m_prevChangeColorButton].baseColor );
  }

  changeButtonColorCore( btnID, colorID );

  m_prevChangeColorButton = btnID;
}
/**
 *  全ボタンの文字色を初期状態へ
 */
void KeyboardProc::changeButtonAllFreeColor( void )
{
  for(int i=BTN_LET0; i<BTN_MAX; ++i)
  {
    changeButtonColorCore( (ButtonID)i, BtnPosTable[i].baseColor );
  }
  resetButtonColorMask();
}
/**
 *  ボタン色変更ペインを全て非表示
 */
void KeyboardProc::resetButtonColorMask( void )
{
  for(uint i=0; i<GFL_NELEMS(m_btnMaskPane); ++i){
    m_btnMaskPane[i]->SetVisible( false );
  }
}
/**
 *  文字ボタンの色変更（コア部分）
 */
void KeyboardProc::changeButtonColorCore( ButtonID btnID, ButtonColor colorID )
{
  // マスク色替え
  if( BtnPosTable[btnID].baseColor == colorID )
  {
    resetButtonColorMask();
  }
  else
  {
    for(uint i=0; i<GFL_NELEMS(m_btnMaskPane); ++i)
    {
      if( (ButtonColor)i == colorID )
      {
        gfl2::math::VEC3  pos;
        nw::lyt::Size    size;

        tpPosToLytPos( &BtnPosTable[btnID], &pos );
        size.width  = BtnPosTable[btnID].width;
        size.height = BtnPosTable[btnID].height;

        m_btnMaskPane[i]->SetTranslate( pos );
        m_btnMaskPane[i]->SetSize( size );
        m_btnMaskPane[i]->SetVisible( true );
      }
      else
      {
        m_btnMaskPane[i]->SetVisible( false );
      }
    }
  }

  // 文字色替え
  gfl2::lyt::LytTextBox* texPane = m_lytWk->GetTextBoxPane( ButtonStrPaneID[btnID] );
  if( colorID == BTNCOLOR_LIGHT ){
    texPane->SetTextColor( m_colorSelect, m_colorSelect );
  }else{
    texPane->SetTextColor( m_colorNormal, m_colorNormal );
  }
}
/**
 *  ボタンに割り当てられた文字Indexの、次の値を返す
 */
uint KeyboardProc::getNextLetterIdx( ButtonID btnID, u32 currentIdx )
{
  const BTN_PARAM* bp = getButtonParamTable( m_inputMode );
  int  letIdx = btnID - BTN_LET0;

  if( currentIdx >= BTN_PER_LET ){
    currentIdx %= BTN_PER_LET;
  }

  for ever
  {
    ++currentIdx;
    if(currentIdx >= BTN_PER_LET){
      return 0;
    }
    if( bp[letIdx].letter[ currentIdx ] != L'\0' ){
      return currentIdx;
    }
  }
}

/**
 *  ボタンID＆連続タッチ回数に応じて、入力される文字コードを返す
 */
wchar_t KeyboardProc::getLetterCode( ButtonID btnID, u32 contCount )
{
  const BTN_PARAM* bp = getButtonParamTable( m_inputMode );
  int  idx = btnID - BTN_LET0;

  if( contCount >= BTN_PER_LET ){
    contCount %= BTN_PER_LET;
  }

  u32 c = 0;
  while( contCount )
  {
    if( bp[idx].letter[c] != L'\0' ){
      --contCount;
    }
    if( ++c >= BTN_PER_LET ){
      c = 0;
      break;
    }
  }

  wchar_t ret =  bp[idx].letter[ c ];

  GFL_PRINT("btnIdx=%d, leterIdx=%d, result=%04x\n", idx, contCount, ret);

  return ret;
}
/**
 *  ボタンID＆連続タッチ回数に応じて、入力される文字コードを返す
 */
wchar_t KeyboardProc::getFlickLetterCode( ButtonID btnID, u32 letIdx )
{
  const BTN_PARAM* bp = getButtonParamTable( m_inputMode );
  int  idx = btnID - BTN_LET0;

  if( letIdx >= FLICK_LET_MAX ){
    letIdx %= FLICK_LET_MAX;
  }

  wchar_t ret =  bp[idx].flick_letter[ letIdx ];
  return ret;
}

/**
 *
 */
const KeyboardProc::BTN_TP_POS  KeyboardProc::BtnPosTable[] = {
  {   0,   0,  0,  0, BTNCOLOR_DARK },
  {  25,  64, 60, 44, BTNCOLOR_DARK },
  {  25, 108, 60, 44, BTNCOLOR_DARK },
  {  25, 152, 60, 44, BTNCOLOR_DARK },
  {  25, 196, 60, 44, BTNCOLOR_DARK },
  { 235,  64, 60, 44, BTNCOLOR_DARK },
  { 235, 108, 60, 44, BTNCOLOR_DARK },
  { 235, 152, 60, 88, BTNCOLOR_DARK },

  {  85,  64, 50, 44, BTNCOLOR_LIGHT },
  { 135,  64, 50, 44, BTNCOLOR_LIGHT },
  { 185,  64, 50, 44, BTNCOLOR_LIGHT },
  {  85, 108, 50, 44, BTNCOLOR_LIGHT },
  { 135, 108, 50, 44, BTNCOLOR_LIGHT },
  { 185, 108, 50, 44, BTNCOLOR_LIGHT },
  {  85, 152, 50, 44, BTNCOLOR_LIGHT },
  { 135, 152, 50, 44, BTNCOLOR_LIGHT },
  { 185, 152, 50, 44, BTNCOLOR_LIGHT },
  {  85, 196, 50, 44, BTNCOLOR_LIGHT },
  { 135, 196, 50, 44, BTNCOLOR_LIGHT },
  { 185, 196, 50, 44, BTNCOLOR_LIGHT },
};

/**
 *  触っているボタンIDを取得
 */
KeyboardProc::ButtonID  KeyboardProc::checkTouchButton( bool bCheckHold )
{
  bool isTouch = (bCheckHold)? m_uiTp->IsTouch() : m_uiTp->IsTouchTrigger();

  if( isTouch )
  {
    int x = m_uiTp->GetX();
    int y = m_uiTp->GetY();

    return checkTouchButtonByPos( x, y );

  }
  return BTN_NULL;
}

KeyboardProc::ButtonID KeyboardProc::checkTouchButtonByPos( int x, int y )
{
  for(int i=1; i<GFL_NELEMS(BtnPosTable); ++i)
  {
    if( (x >= (BtnPosTable[i].x)) && (y >= (BtnPosTable[i].y))
    &&  (x < (BtnPosTable[i].x + BtnPosTable[i].width))
    &&  (y < (BtnPosTable[i].y + BtnPosTable[i].height))
    ){
      return (ButtonID)( i );
    }
  }
  return (ButtonID)0;
}

const u16 KeyboardProc::CaptionPaneID[] = {
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_123,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_ABC,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_HIRA,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_KATA
};

/**
 *  入力モード切替
 */
void KeyboardProc::changeInputMode( InputMode next_mode )
{
  gfl2::lyt::LytTextBox* texPane;

  if( m_inputMode != next_mode )
  {
    InputMode  prev_mode = m_inputMode;
    m_inputMode = next_mode;

    gfl2::lyt::LytPane* pane = m_lytWk->GetPane( PANENAME_DEBUG_KEYBOARD_PANE_SEL_MODE );
    gfl2::math::VEC3  pos;
    tpPosToLytPos( &BtnPosTable[next_mode+1], &pos );
    pane->SetTranslate( pos );

    texPane = m_lytWk->GetTextBoxPane( CaptionPaneID[prev_mode] );
    texPane->SetTextColor( m_colorNormal, m_colorNormal );

    texPane = m_lytWk->GetTextBoxPane( CaptionPaneID[next_mode] );
    texPane->SetTextColor( m_colorSelect, m_colorSelect );
  }
  else
  {
    if( (next_mode == INPUT_NUMBER) || (next_mode == INPUT_ALPHA) ){
      m_bHankaku = !(m_bHankaku);
      renewZenHanCaption();
    }
    else{
      return;
    }
  }

  // 文字ボタン書き換え
  const BTN_PARAM* bp = getButtonParamTable( next_mode );
  for(u32 i=0; i<BTN_LET_MAX; ++i)
  {
    texPane = m_lytWk->GetTextBoxPane( bp->paneID );
    const wchar_t* string = (bp->caption[0] != L'\0')?  bp->caption : bp->letter;
    setButtonCaption( texPane, string, (i==0) );
    ++bp;
  }
}
/**
 *  全角・半角の切り替えに対応（モードのキャプション部分）
 */
void KeyboardProc::renewZenHanCaption( void )
{
  gfl2::lyt::LytTextBox* paneNum   = m_lytWk->GetTextBoxPane( CaptionPaneID[INPUT_NUMBER] );
  gfl2::lyt::LytTextBox* paneAlpha = m_lytWk->GetTextBoxPane( CaptionPaneID[INPUT_ALPHA] );

  if( m_bHankaku )
  {
    setButtonCaption( paneNum,   L">123<" );
    setButtonCaption( paneAlpha, L">ABC<" );
  }
  else
  {
    setButtonCaption( paneNum,   L"１２３" );
    setButtonCaption( paneAlpha, L"ＡＢＣ" );
  }
}
/**
 *  タッチパネルボタン座標データ -> レイアウト座標データへ変換
 */
void KeyboardProc::tpPosToLytPos( const BTN_TP_POS* tpPos, gfl2::math::VEC3* pos )
{
  int x = (tpPos->x - 160) + (tpPos->width / 2);
  int y = (120 - tpPos->y) - (tpPos->height / 2);

  pos->x = x;
  pos->y = y;
  pos->z = 0;
}
/**
 *  ボタン文字列書き換え（長すぎて納まらない文字列が来た場合は、適宜、縮小率を調整する）
 */
void KeyboardProc::setButtonCaption( gfl2::lyt::LytTextBox* texPane, const wchar_t* string, bool debugFlag )
{
  nw::lyt::Size paneSize = texPane->GetSize();
  const nw::font::Font* font = texPane->GetFont();

  const wchar_t* s = string;
  int strWidth = 0;
  while( *s != L'\0' ){
    strWidth += font->GetCharWidth( *s++ );
  }

  f32 ratio = (f32)(paneSize.width) / (f32)strWidth;
  if( ratio > 0.68f ){
    ratio = 0.68f;
  }
  f32 nextFontWidth = 24.0f * ratio;

  nw::lyt::Size  fontSize = texPane->GetFontSize();

  fontSize.width = nextFontWidth;
  texPane->SetFontSize( fontSize );
  texPane->SetString( string );
}


//==========================================================================================
// 文字列補間
//==========================================================================================
const u16 KeyboardProc::WordBoxPaneID[DISP_SEARCH_WORD_MAX][2] = {
  { PANENAME_DEBUG_KEYBOARD_PANE_WORDBG00, PANENAME_DEBUG_KEYBOARD_PANE_WORDBOX00 },
  { PANENAME_DEBUG_KEYBOARD_PANE_WORDBG01, PANENAME_DEBUG_KEYBOARD_PANE_WORDBOX01 },
  { PANENAME_DEBUG_KEYBOARD_PANE_WORDBG02, PANENAME_DEBUG_KEYBOARD_PANE_WORDBOX02 },
  { PANENAME_DEBUG_KEYBOARD_PANE_WORDBG03, PANENAME_DEBUG_KEYBOARD_PANE_WORDBOX03 },
  { PANENAME_DEBUG_KEYBOARD_PANE_WORDBG04, PANENAME_DEBUG_KEYBOARD_PANE_WORDBOX04 },
};

void KeyboardProc::setupSearchWordPanes( void )
{
  for(u32 i=0; i<DISP_SEARCH_WORD_MAX; ++i)
  {
    m_wordBGPane[ i ]   = m_lytWk->GetPane( WordBoxPaneID[i][0] );
    m_wordTextPane[ i ] = m_lytWk->GetTextBoxPane( WordBoxPaneID[i][1] );
  }
}

int KeyboardProc::GetMessageDatCount(void)
{
  int max = m_wordComp->GetStrCount();
  if(m_wordSearchMax==0){
    return max;
  }
  
  GFL_ASSERT_MSG( max>=m_wordSearchMax, "searchMaxがメッセージデータよりも大きい%d<%d\n", // @check
                  max, m_wordSearchMax );

  return m_wordSearchMax;
}


void KeyboardProc::dispSearchWords( void )
{
  if( m_wordComp == NULL ){
    return;
  }

  m_bWordDisped = true;

  u32 wordMax = GetMessageDatCount();//m_wordComp->GetStrCount();
  u32 hitCount = 0;
  int dispX = -154; // 画面左端からちょっとだけ間を開けたとこ。
  s32 skipCount = 0;

  for(u32 i=0; i<wordMax; ++i)
  {
    m_wordComp->GetString( i, *m_strBufTmp );
    /**
      * CBBTS 435:ウムラウト文字が入力できないので、アルファベット入力のみで
      *           ウムラウト文字と同じ音のアルファベットに置き換えて検索してほしい
      */
    if( System::GetLang() != POKEMON_LANG_JAPAN && System::GetLang() != POKEMON_LANG_KOREA ){
      NetAppLib::Util::NetAppLatinumStringTool::ToUpperNoAccent( m_strBufTmp, m_strBufToUpper ); // 大文字化＆ウムラウト外し
    }else{
      m_strBufToUpper->SetStr( *m_strBufTmp );
    }
    bool find = false;
    if( !m_bExpandSearch ){ find = isMatchComplete(m_strBuf, m_strBufToUpper); }
    else{ find = isMatchExpand(m_strBuf, m_strBufToUpper); }
    if( find )
    {
      if( skipCount++ < m_wordStartIdx ){
        continue;
      }
      dispX = setWordPane( m_strBufTmp, hitCount++, dispX );
      if( hitCount >= DISP_SEARCH_WORD_MAX ){
        break;
      }
    }
  }

  while( hitCount < DISP_SEARCH_WORD_MAX ){
    hideWordPane( hitCount++ );
  }
}

bool KeyboardProc::isMatchExpand( const gfl2::str::StrBuf* input, const gfl2::str::StrBuf* target )
{
  to_Katakana( input, m_strBufWork1 );
  to_Katakana( target, m_strBufWork2 );

  const wchar_t* pInput = m_strBufWork1->GetPtr();
  const wchar_t* pTarget = m_strBufWork2->GetPtr();
  if( *pInput == gfl2::str::EOM_CODE ){
    return false;
  }
  while( *pInput != gfl2::str::EOM_CODE )
  {
    bool find = false;
    while( *pTarget != gfl2::str::EOM_CODE )
    {
      if( *pInput == *pTarget )
      {
        find = true;
        break;
      }
      ++pTarget;
    }
    if( !find ){return false;}

    while( *pInput != gfl2::str::EOM_CODE )
    {
      if( *pInput != *pTarget )
      {
        find = false;
        break;
      }
      ++pInput;
      ++pTarget;
    }
    if( find ){return true;}
  }
  return true;
}

bool KeyboardProc::isMatchComplete( const gfl2::str::StrBuf* input, const gfl2::str::StrBuf* target )
{
  to_Katakana( input, m_strBufWork1 );
  to_Katakana( target, m_strBufWork2 );

  const wchar_t* pInput = m_strBufWork1->GetPtr();
  const wchar_t* pTarget = m_strBufWork2->GetPtr();
  if( *pInput == gfl2::str::EOM_CODE ){
    return false;
  }
  while( *pInput != gfl2::str::EOM_CODE )
  {
    if( *pInput != *pTarget ){
      return false;
    }
    ++pInput;
    ++pTarget;
  }
  return true;
}
// from にひらがながあれば、カタカナに変換して to にコピー
void KeyboardProc::to_Katakana( const gfl2::str::StrBuf* from, gfl2::str::StrBuf* to )
{
  to->Clear();
  const wchar_t* sp = from->GetPtr();
  while( *sp != gfl2::str::EOM_CODE )
  {
    wchar_t code = *sp;
    if( (code >= 0x3041) && (code <= 0x3093) )
    {
      code = 0x30a1 + (code - 0x3041);
    }
    to->AddCode( code );
    ++sp;
  }
}

// 文字幅計算
int KeyboardProc::calcWordWidth( const gfl2::str::StrBuf* word, gfl2::lyt::LytTextBox* pane )
{
  const nw::font::Font* font = pane->GetFont();
  const wchar_t* sp = word->GetPtr();
  f32 width = 0.0f;
  f32 tmp;
  while( *sp != gfl2::str::EOM_CODE )
  {
    tmp = font->GetCharWidth( *sp++ );
  //  tmp *= 0.64f;
    width += tmp;
  }
  return (gfl2::math::FCeil( width ) + 4);
}

int KeyboardProc::setWordPane( const gfl2::str::StrBuf* word, u32 paneIdx, int dispX )
{
  if( paneIdx < DISP_SEARCH_WORD_MAX )
  {
    gfl2::lyt::LytTextBox*   paneTex = m_wordTextPane[ paneIdx ];
    gfl2::lyt::LytPane*      paneBg  = m_wordBGPane[ paneIdx ];

    int  tex_width = calcWordWidth( word, paneTex );
    int  bg_width = tex_width + 16;

    nw::lyt::Size    size = paneBg->GetSize();
    size.width  = bg_width;
    paneBg->SetSize( size );

    size = paneTex->GetSize();
    size.width = tex_width;
    paneTex->SetSize( size );

    dispX += ( bg_width / 2 );

    gfl2::math::VEC3  pos = paneBg->GetTranslate();
    pos.x = dispX;
    paneBg->SetTranslate( pos );

    pos = paneTex->GetTranslate();
    pos.x = dispX;
    paneTex->SetTranslate( pos );

    paneTex->SetString( word->GetPtr() );

    paneBg->SetVisible( true );
    paneTex->SetVisible( true );

    dispX += ( (bg_width / 2) + 4 );
  }

  return dispX;
}
/**
 *  特定の補間候補ウィンドウ表示をオフ
 */
void KeyboardProc::hideWordPane( u32 paneIdx )
{
  if( paneIdx < DISP_SEARCH_WORD_MAX )
  {
    m_wordBGPane[paneIdx]->SetVisible( false );
    m_wordTextPane[paneIdx]->SetVisible( false );
  }
}
/**
 *  全ての補間候補ウィンドウ表示をオフ
 */
void KeyboardProc::hideAllSearchWords( void )
{
  for(u32 i=0; i<DISP_SEARCH_WORD_MAX; ++i)
  {
    hideWordPane( i );
  }
}
/**
 *  表示されている補間候補ウィンドウにタッチされたら文字列を更新する
 */
void KeyboardProc::checkTouchWord( void )
{
  if( !m_bWordDisped ){
    return;
  }
  if( m_uiTp->IsTouchTrigger() )
  {
    int x = m_uiTp->GetX();
    int y = m_uiTp->GetY();

    for(u32 i=0; i<DISP_SEARCH_WORD_MAX; ++i)
    {
      if( m_wordTextPane[ i ]->IsVisible() )
      {
        if( m_lytWk->GetHitPane( x, y, WordBoxPaneID[i][1] ) )
        {
          const wchar_t* sp = m_wordTextPane[ i ]->GetStringBuffer();
          m_strBuf->SetStr( sp );
          updateString( true );
          hideAllSearchWords();
          break;
        }
      }
    }
  }
}
/**
 *  入力文字ペインをタッチされたか判定
 */
bool KeyboardProc::checkTouchStrPane( void )
{
  if( m_uiTp->IsTouchTrigger() )
  {
    int x = m_uiTp->GetX();
    int y = m_uiTp->GetY();

    return m_lytWk->GetHitPane( x, y, PANENAME_DEBUG_KEYBOARD_PANE_INPUTSTR );
  }
  return false;
}

/**
 *  入力されている文字が、補間候補の１つであればそのIndexを返す
 */
int KeyboardProc::getSearchWordIndex( void )
{
  if( m_wordComp )
  {
    u32 word_max = GetMessageDatCount();//m_wordComp->GetStrCount();
    for(u32 i=0; i<word_max; ++i)
    {
      m_wordComp->GetString( i, *m_strBufTmp );
      if(NetAppLib::Util::NetAppLatinumStringTool::CompareBufferNoCaseNoAccent(m_strBuf, m_strBufTmp)){
        return i;
      }
    }
  }
  return MATCH_NONE;
}
//==========================================================================================

/**
 *  入力文字列を更新
 */
void  KeyboardProc::updateString( bool bFirstTime )
{
  m_stringPane->SetString( m_strBuf->GetPtr() );
  nw::ut::Rect  rect = m_stringPane->GetTextDrawRect();

  gfl2::math::VEC3 pos(0, 0, 0);
  pos.x = rect.right + 2;
  m_cursorPane->SetTranslate( pos );
  
  //候補スタートIndexを初期化
  m_wordStartIdx = 0;

  if( !bFirstTime ){
    dispSearchWords();
  }
}

/**
 *  文字を末尾に追加
 */
void KeyboardProc::putChar( wchar_t  charCode )
{
  if( m_strBuf->GetLength() < m_strMaxLen )
  {
    m_strBuf->AddCode( charCode );
    updateString();
  }
}
/**
 *  末尾の文字を切り替え
 */
void KeyboardProc::changeChar( wchar_t  charCode )
{
  backSpace( false );
  m_strBuf->AddCode( charCode );
  updateString();
}
/**
 *  スペース１文字追加
 */
void KeyboardProc::putSpace( void )
{
  if( m_inputMode == INPUT_ALPHA ){
    putChar( L' ' );
  }else{
    putChar( L'　' );
  }
}
/**
 *  １文字削除
 */
void KeyboardProc::backSpace( bool bUpdate )
{
  const wchar_t* sp = m_strBuf->GetPtr();
  u32 len = m_strBuf->GetLength();
  if( len ){
    --len;
  }
  m_strBufTmp->Clear();
  for(u32 i=0; i<len; ++i){
    m_strBufTmp->AddCode( *sp++ );
  }
  m_strBuf->SetStr( *m_strBufTmp );

  if( bUpdate ){
    updateString();
  }
}
/**
 *  入力されている最後の１文字を返す
 */
wchar_t KeyboardProc::getEndCode( void )
{
  const wchar_t* sp = m_strBuf->GetPtr();
  if( *sp == gfl2::str::EOM_CODE){
    return gfl2::str::EOM_CODE;
  }
  else{
    wchar_t result = *sp++;
    while( *sp != gfl2::str::EOM_CODE )
    {
      result = *sp++;
    }
    return result;
  }

}
/**
 *  濁点、半濁点や小文字／大文字切り替えの候補文字を返す
 */
wchar_t KeyboardProc::getFuncNextCode( wchar_t code )
{
  static const wchar_t  tbl_h01[] = { L'あ', L'ぁ', L'\0' };
  static const wchar_t  tbl_h02[] = { L'い', L'ぃ', L'\0' };
  static const wchar_t  tbl_h03[] = { L'う', L'ぅ', L'\0' };
  static const wchar_t  tbl_h04[] = { L'え', L'ぇ', L'\0' };
  static const wchar_t  tbl_h05[] = { L'お', L'ぉ', L'\0' };
  static const wchar_t  tbl_h06[] = { L'か', L'が', L'\0' };
  static const wchar_t  tbl_h07[] = { L'き', L'ぎ', L'\0' };
  static const wchar_t  tbl_h08[] = { L'く', L'ぐ', L'\0' };
  static const wchar_t  tbl_h09[] = { L'け', L'げ', L'\0' };
  static const wchar_t  tbl_h10[] = { L'こ', L'ご', L'\0' };
  static const wchar_t  tbl_h11[] = { L'さ', L'ざ', L'\0' };
  static const wchar_t  tbl_h12[] = { L'し', L'じ', L'\0' };
  static const wchar_t  tbl_h13[] = { L'す', L'ず', L'\0' };
  static const wchar_t  tbl_h14[] = { L'せ', L'ぜ', L'\0' };
  static const wchar_t  tbl_h15[] = { L'そ', L'ぞ', L'\0' };
  static const wchar_t  tbl_h16[] = { L'た', L'だ', L'\0' };
  static const wchar_t  tbl_h17[] = { L'ち', L'ぢ', L'\0' };
  static const wchar_t  tbl_h18[] = { L'つ', L'っ', L'づ', L'\0' };
  static const wchar_t  tbl_h19[] = { L'て', L'で', L'\0' };
  static const wchar_t  tbl_h20[] = { L'と', L'ど', L'\0' };
  static const wchar_t  tbl_h21[] = { L'は', L'ば', L'ぱ', L'\0' };
  static const wchar_t  tbl_h22[] = { L'ひ', L'び', L'ぴ', L'\0' };
  static const wchar_t  tbl_h23[] = { L'ふ', L'ぶ', L'ぷ', L'\0' };
  static const wchar_t  tbl_h24[] = { L'へ', L'べ', L'ぺ', L'\0' };
  static const wchar_t  tbl_h25[] = { L'ほ', L'ぼ', L'ぽ', L'\0' };
  static const wchar_t  tbl_h26[] = { L'や', L'ゃ', L'\0' };
  static const wchar_t  tbl_h27[] = { L'ゆ', L'ゅ', L'\0' };
  static const wchar_t  tbl_h28[] = { L'よ', L'ょ', L'\0' };

  static const wchar_t  tbl_k01[] = { L'ア', L'ァ', L'\0' };
  static const wchar_t  tbl_k02[] = { L'イ', L'ィ', L'\0' };
  static const wchar_t  tbl_k03[] = { L'ウ', L'ゥ', L'ヴ', L'\0' };
  static const wchar_t  tbl_k04[] = { L'エ', L'ェ', L'\0' };
  static const wchar_t  tbl_k05[] = { L'オ', L'ォ', L'\0' };
  static const wchar_t  tbl_k06[] = { L'カ', L'ガ', L'\0' };
  static const wchar_t  tbl_k07[] = { L'キ', L'ギ', L'\0' };
  static const wchar_t  tbl_k08[] = { L'ク', L'グ', L'\0' };
  static const wchar_t  tbl_k09[] = { L'ケ', L'ゲ', L'\0' };
  static const wchar_t  tbl_k10[] = { L'コ', L'ゴ', L'\0' };
  static const wchar_t  tbl_k11[] = { L'サ', L'ザ', L'\0' };
  static const wchar_t  tbl_k12[] = { L'シ', L'ジ', L'\0' };
  static const wchar_t  tbl_k13[] = { L'ス', L'ズ', L'\0' };
  static const wchar_t  tbl_k14[] = { L'セ', L'ゼ', L'\0' };
  static const wchar_t  tbl_k15[] = { L'ソ', L'ゾ', L'\0' };
  static const wchar_t  tbl_k16[] = { L'タ', L'ダ', L'\0' };
  static const wchar_t  tbl_k17[] = { L'チ', L'ヂ', L'\0' };
  static const wchar_t  tbl_k18[] = { L'ツ', L'ッ', L'ヅ', L'\0' };
  static const wchar_t  tbl_k19[] = { L'テ', L'デ', L'\0' };
  static const wchar_t  tbl_k20[] = { L'ト', L'ド', L'\0' };
  static const wchar_t  tbl_k21[] = { L'ハ', L'バ', L'パ', L'\0' };
  static const wchar_t  tbl_k22[] = { L'ヒ', L'ビ', L'ピ', L'\0' };
  static const wchar_t  tbl_k23[] = { L'フ', L'ブ', L'プ', L'\0' };
  static const wchar_t  tbl_k24[] = { L'ヘ', L'ベ', L'ペ', L'\0' };
  static const wchar_t  tbl_k25[] = { L'ホ', L'ボ', L'ポ', L'\0' };
  static const wchar_t  tbl_k26[] = { L'ヤ', L'ャ', L'\0' };
  static const wchar_t  tbl_k27[] = { L'ユ', L'ュ', L'\0' };
  static const wchar_t  tbl_k28[] = { L'ヨ', L'ョ', L'\0' };

  static const wchar_t* tbl_hira[] = {
    tbl_h01,  tbl_h02,  tbl_h03,  tbl_h04,  tbl_h05,  tbl_h06,  tbl_h07,  tbl_h08,  tbl_h09,  tbl_h10,  tbl_h11,  tbl_h12,  tbl_h13,  tbl_h14,
    tbl_h15,  tbl_h16,  tbl_h17,  tbl_h18,  tbl_h19,  tbl_h20,  tbl_h21,  tbl_h22,  tbl_h23,  tbl_h24,  tbl_h25,  tbl_h26,  tbl_h27,  tbl_h28
  };
  static const wchar_t* tbl_kata[] = {
    tbl_k01,  tbl_k02,  tbl_k03,  tbl_k04,  tbl_k05,  tbl_k06,  tbl_k07,  tbl_k08,  tbl_k09,  tbl_k10,  tbl_k11,  tbl_k12,  tbl_k13,  tbl_k14,
    tbl_k15,  tbl_k16,  tbl_k17,  tbl_k18,  tbl_k19,  tbl_k20,  tbl_k21,  tbl_k22,  tbl_k23,  tbl_k24,  tbl_k25,  tbl_k26,  tbl_k27,  tbl_k28
  };

  const wchar_t** tblAdrs = NULL;
  u32 tblElems = 0;

  switch( m_inputMode ){
  case INPUT_HIRAGANA:
      tblAdrs = tbl_hira;
      tblElems = GFL_NELEMS(tbl_hira);
      break;
  case INPUT_KATAKANA:
      tblAdrs = tbl_kata;
      tblElems = GFL_NELEMS(tbl_kata);
      break;
  case INPUT_ALPHA:
      if( (code >= L'A') && (code <= L'Z') ){
        code = L'a' + (code - L'A');
        return code;
      }
      if( (code >= L'a') && (code <= L'z') ){
        code = L'A' + (code - L'a');
        return code;
      }
      if( (code >= L'Ａ') && (code <= L'Ｚ') ){
        code = L'ａ' + (code - L'Ａ');
        return code;
      }
      if( (code >= L'ａ') && (code <= L'ｚ') ){
        code = L'Ａ' + (code - L'ａ');
        return code;
      }

      /* fallthru */
  default:
    return code;
  }

  if( tblAdrs )
  {
    for(u32 t=0; t<tblElems; ++t)
    {
      u32 i=0;
      while( tblAdrs[t][i] != L'\0' )
      {
        if( tblAdrs[t][i] == code )
        {
          ++i;
          if( tblAdrs[t][i] != L'\0' ){
            return tblAdrs[t][i];
          }
          return tblAdrs[t][0];
        }
        ++i;
      }
    }
  }

  return code;


}

//==========================================================================================
//==========================================================================================


/**
 *  ボタンパラメータ（数字入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamNumber[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'１', L'\0',L'\0',L'\0',L'\0',  L'\0'  }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'１', L'\0',L'\0',L'\0',L'\0',  L'\0'  },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'２', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'２', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'３', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'３', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'４', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'４', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'５', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'５', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'６', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'６', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'７', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'７', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'８', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'８', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'９', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'９', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'０', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'０', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  }
};
/**
 *  ボタンパラメータ（数字入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamNumber_Half[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'1', L'\0',L'\0',L'\0',L'\0',  L'\0'  }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'1', L'\0',L'\0',L'\0',L'\0',  L'\0'  },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'2', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'2', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'3', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'3', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'4', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'4', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'5', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'5', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'6', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'6', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'7', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'7', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'8', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'8', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'9', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'9', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  }
};
/**
 *  ボタンパラメータ（アルファベット大文字入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamAlphaL[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'＠', L'＃',L'／',L'＆',L'＿',  L'\0'}, { L'@',L'#',L'/',L'&',L'_',L'\0',L'\0',L'\0', },
    { L'＠', L'＃',L'／',L'＆',L'＿',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'Ａ', L'Ｂ',L'Ｃ',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ａ', L'Ｂ',L'Ｃ',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'Ｄ', L'Ｅ',L'Ｆ',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｄ', L'Ｅ',L'Ｆ',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'Ｇ', L'Ｈ',L'Ｉ',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｇ', L'Ｈ',L'Ｉ',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'Ｊ', L'Ｋ',L'Ｌ',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｊ', L'Ｋ',L'Ｌ',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'Ｍ', L'Ｎ',L'Ｏ',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｍ', L'Ｎ',L'Ｏ',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'Ｐ', L'Ｑ',L'Ｒ',L'Ｓ',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｐ', L'Ｑ',L'Ｒ',L'Ｓ',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'Ｔ', L'Ｕ',L'Ｖ',L'\0',L'\0', L'\0' }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｔ', L'Ｕ',L'Ｖ',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'Ｗ', L'Ｘ',L'Ｙ',L'Ｚ',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'Ｗ', L'Ｘ',L'Ｙ',L'Ｚ',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'a',L'/',L'A',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'’', L'”',L'（',L'）',L'：',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'’', L'”',L'（',L'）',L'：',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'ー',L'．', L'，',L'？',L'！', L'\0' }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ー',L'．', L'，',L'？',L'！', L'\0' },
  }
};
/**
 *  ボタンパラメータ（アルファベット大文字入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamAlphaL_Half[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'@', L'#',L'/',L'&',L'_',  L'\0'}, { L'@',L'#',L'/',L'&',L'_',L'\0',L'\0',L'\0', },
    { L'@', L'#',L'/',L'&',L'_',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'A', L'B',L'C',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'A', L'B',L'C',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'D', L'E',L'F',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'D', L'E',L'F',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'G', L'H',L'I',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'G', L'H',L'I',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'J', L'K',L'L',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'J', L'K',L'L',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'M', L'N',L'O',L'\0',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'M', L'N',L'O',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'P', L'Q',L'R',L'S',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'P', L'Q',L'R',L'S',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'T', L'U',L'V',L'\0',L'\0', L'\0' }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'T', L'U',L'V',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'W', L'X',L'Y',L'Z',L'\0',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'W', L'X',L'Y',L'Z',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'a',L'/',L'A',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'\'', L'"',L'(',L')',L'：',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\'', L'"',L'(',L')',L'：',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'-', L'.', L',',L'?',L'!', L'\0' }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'-', L'.', L',',L'?',L'!', L'\0' },
  }
};
/**
 *  ボタンパラメータ（アルファベット小文字入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamAlphaS[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'@', L'#',L'/',L'&',L'_',  L'\0'},       { L'@',L'#',L'/',L'&',L'_',L'\0',L'\0',L'\0', },
    { L'@', L'#',L'/',L'&',L'_',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'a', L'b',L'c',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'a', L'b',L'c',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'd', L'e',L'f',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'd', L'e',L'f',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'g', L'h',L'i',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'g', L'h',L'i',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'j', L'k',L'l',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'j', L'k',L'l',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'm', L'n',L'o',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'm', L'n',L'o',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'p', L'q',L'r',L's',L'\0', L'\0' },      { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'p', L'q',L'r',L's',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L't', L'u',L'v',L'\0',L'\0', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L't', L'u',L'v',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'w', L'x',L'y',L'z',L'\0', L'\0' },      { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'w', L'x',L'y',L'z',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0', L'\0' },  { L'a',L'/',L'A',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'\'', L'"',L'(',L')',L':', L'\0' },     { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'\'', L'"',L'(',L')',L':', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'-', L'.', L',',L'?',L'!', L'\0' },      { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'-', L'.', L',',L'?',L'!', L'\0' },
  }
};
/**
 *  ボタンパラメータ（ひらがな入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamHira[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'あ', L'い',L'う',L'え',L'お', L'\0' }, { L'あ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'あ', L'い',L'う',L'え',L'お', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'か', L'き',L'く',L'け',L'こ', L'\0' }, { L'か',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'か', L'き',L'く',L'け',L'こ', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'さ', L'し',L'す',L'せ',L'そ', L'\0' }, { L'さ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'さ', L'し',L'す',L'せ',L'そ', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'た', L'ち',L'つ',L'て',L'と', L'\0' }, { L'た',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'た', L'ち',L'つ',L'て',L'と', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'な', L'に',L'ぬ',L'ね',L'の', L'\0' }, { L'な',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'な', L'に',L'ぬ',L'ね',L'の', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'は', L'ひ',L'ふ',L'へ',L'ほ', L'\0' }, { L'は',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'は', L'ひ',L'ふ',L'へ',L'ほ', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'ま', L'み',L'む',L'め',L'も', L'\0' }, { L'ま',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ま', L'み',L'む',L'め',L'も', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'や', L'ゆ',L'よ',L'\0',L'\0', L'\0' }, { L'や',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'や', L'\0',L'ゆ',L'\0',L'よ', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'ら', L'り',L'る',L'れ',L'ろ', L'\0' }, { L'ら',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ら', L'り',L'る',L'れ',L'ろ', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0', L'\0' }, { L'"', L'　', L'°',L'\n',L'小',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'わ', L'を',L'ん',L'ー',L'\0', L'\0' }, { L'わ',L'ー',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'わ', L'を',L'ん',L'ー',L'\0', L'\0' },
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'♂', L'♀',L'？',L'！',L'・', L'\0' }, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'♂', L'♀',L'？',L'！',L'・', L'\0' },
  }
};
/**
 *  ボタンパラメータ（カタカナ入力）
 */
const KeyboardProc::BTN_PARAM  KeyboardProc::ButtonParamKata[] = {
  {
    PANENAME_DEBUG_KEYBOARD_PANE_LET0,
    { L'ア', L'イ',L'ウ',L'エ',L'オ',  L'\0'}, { L'ア',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ア', L'イ',L'ウ',L'エ',L'オ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET1,
    { L'カ', L'キ',L'ク',L'ケ',L'コ',  L'\0'}, { L'カ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'カ', L'キ',L'ク',L'ケ',L'コ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET2,
    { L'サ', L'シ',L'ス',L'セ',L'ソ',  L'\0'}, { L'サ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'サ', L'シ',L'ス',L'セ',L'ソ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET3,
    { L'タ', L'チ',L'ツ',L'テ',L'ト',  L'\0'}, { L'タ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'タ', L'チ',L'ツ',L'テ',L'ト',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET4,
    { L'ナ', L'ニ',L'ヌ',L'ネ',L'ノ',  L'\0'}, { L'ナ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ナ', L'ニ',L'ヌ',L'ネ',L'ノ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET5,
    { L'ハ', L'ヒ',L'フ',L'ヘ',L'ホ',  L'\0'}, { L'ハ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ハ', L'ヒ',L'フ',L'ヘ',L'ホ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET6,
    { L'マ', L'ミ',L'ム',L'メ',L'モ',  L'\0'}, { L'マ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'マ', L'ミ',L'ム',L'メ',L'モ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET7,
    { L'ヤ', L'ユ',L'ヨ',L'\0',L'\0',  L'\0'}, { L'ヤ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ヤ', L'\0',L'ユ',L'\0',L'ヨ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET8,
    { L'ラ', L'リ',L'ル',L'レ',L'ロ',  L'\0'}, { L'ラ',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ラ', L'リ',L'ル',L'レ',L'ロ',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET9,
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'}, { L'"', L'　', L'°',L'\n',L'小',L'\0',L'\0',L'\0', },
    { L'\0', L'\0',L'\0',L'\0',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET10,
    { L'ワ', L'ヲ',L'ン',L'ー',L'\0',  L'\0'}, { L'ワ',L'ー',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'ワ', L'ヲ',L'ン',L'ー',L'\0',  L'\0'},
  },{
    PANENAME_DEBUG_KEYBOARD_PANE_LET11,
    { L'♂', L'♀',L'？',L'！',L'・',  L'\0'}, { L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0',L'\0', },
    { L'♂', L'♀',L'？',L'！',L'・',  L'\0'},
  }
};

/**
 *  InputMode に対応したボタン・文字列パラメータを取得
 */
const KeyboardProc::BTN_PARAM* KeyboardProc::getButtonParamTable( InputMode mode )
{
  switch( mode ){
  case INPUT_HIRAGANA:    return ButtonParamHira;
  case INPUT_KATAKANA:    return ButtonParamKata;

  case INPUT_NUMBER:
    return (m_bHankaku)? ButtonParamNumber_Half : ButtonParamNumber;

  case INPUT_ALPHA:
    return (m_bHankaku)? ButtonParamAlphaL_Half : ButtonParamAlphaL;

  default:
    GFL_ASSERT(0);
    return ButtonParamHira;
  }
}

const u16 KeyboardProc::ButtonStrPaneID[] = {
  0,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_123,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_ABC,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_HIRA,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_KATA,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_BS,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_SPACE,
  PANENAME_DEBUG_KEYBOARD_PANE_CAP_OK,
  PANENAME_DEBUG_KEYBOARD_PANE_LET0,
  PANENAME_DEBUG_KEYBOARD_PANE_LET1,
  PANENAME_DEBUG_KEYBOARD_PANE_LET2,
  PANENAME_DEBUG_KEYBOARD_PANE_LET3,
  PANENAME_DEBUG_KEYBOARD_PANE_LET4,
  PANENAME_DEBUG_KEYBOARD_PANE_LET5,
  PANENAME_DEBUG_KEYBOARD_PANE_LET6,
  PANENAME_DEBUG_KEYBOARD_PANE_LET7,
  PANENAME_DEBUG_KEYBOARD_PANE_LET8,
  PANENAME_DEBUG_KEYBOARD_PANE_LET9,
  PANENAME_DEBUG_KEYBOARD_PANE_LET10,
  PANENAME_DEBUG_KEYBOARD_PANE_LET11,
};



} // end of namespace

#endif  // #if PM_DEBUG

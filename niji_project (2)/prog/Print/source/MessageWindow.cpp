//=============================================================================
/**
 * @file    MessageWindow.cpp
 * @brief   文字送り対応メッセージウィンドウ
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.05.20
 *
 * @li  sangoのxy_msgWin.cppを移植
 */
//=============================================================================

#include "Print/include/PrintSystem.h"
#include "Print/include/MessageWindow.h"
#include "System/include/PokemonVersion.h"
#include "Sound/include/Sound.h"
//#include "savedata/sv_config.h" //langId
//#include <debug/debugwin.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( print )

inline u16 FP16_One( void )
{
  return 1000;
}

inline u16 FP16_From( f32 value )
{
  value *= 1000.0f;
  u16 result = (u16) value ;

  return result;
}

inline f32 FP16_To( u16 value )
{
  f32 result = (f32)value;
  result /= 1000.0f;
  return result;
}

Savedata::ConfigSave::MSG_SPEED MessageWindow::mDefaultMsgSpeed = Savedata::ConfigSave::MSG_NORMAL;

#if PM_DEBUG
u32  MessageWindow::mDebugWaitFrame = 0;
u32  MessageWindow::mDebugPutCount  = 1;
bool MessageWindow::mDebugUseFlag   = false;
#endif

/**
 * コンストラクタ
 */
MessageWindow::MessageWindow(
  const gfl2::ui::DeviceManager * uiDevice,
  gfl2::lyt::LytWk * lytWk,
  u16 numLines,
  int waitFrames,
  gfl2::heap::HeapBase* pHeap,
  u32 procCharCount )
  : gfl2::str::StrWin( lytWk, numLines, waitFrames, pHeap, NULL, procCharCount )
  , m_uiDevice( uiDevice )
  , m_bFirstPaneAdded( false )
  , m_font(NULL)
  , m_paneFontWidth(0.0f)
  , m_pane_num( 0 )
  , m_bSuspendKey( false )
  , m_bCommAutoFlag( false )
  , m_bAutoFlagOnce( false )
  , m_enableCtrlKey( true )
  , m_enableCtrlTouch( true )
  , m_enalbeMsgBoost( true )
  , m_waitCnt( 0 )
  , m_msgCbClass( NULL )
  , m_isWaitCallback( false )
{
  SetTagProcessor( &m_tagProc );
  SetLineFadeOutFrames( XY_MSG_FADE_FRAMES );
  SetLineScrollFrames( XY_MSG_SCROLL_FRAMES );
  m_tagProc.SetMsgWin(this);
  m_LineBreakSE=0;
}

MessageWindow::MessageWindow(
  const gfl2::ui::DeviceManager * uiDevice,
  gfl2::lyt::LytWk * lytWk,
  u16 numLines,
  gfl2::heap::HeapBase* pHeap ,
  bool is60Fps )
  : gfl2::str::StrWin( lytWk, numLines, 1, pHeap, NULL, 1 )
  , m_uiDevice( uiDevice )
  , m_bFirstPaneAdded( false )
  , m_font(NULL)
  , m_paneFontWidth(0.0f)
  , m_pane_num( 0 )
  , m_bSuspendKey( false )
  , m_bCommAutoFlag( false )
  , m_bAutoFlagOnce( false )
  , m_enableCtrlKey( true )
  , m_enableCtrlTouch( true )
  , m_enalbeMsgBoost( true )
  , m_msgCbClass( NULL )
  , m_isWaitCallback( false )
{
  SetMessageSpeed( mDefaultMsgSpeed , is60Fps );
  SetTagProcessor( &m_tagProc );
  SetLineFadeOutFrames( XY_MSG_FADE_FRAMES );
  SetLineScrollFrames( XY_MSG_SCROLL_FRAMES );
  m_tagProc.SetMsgWin(this);
}

/**
 * デストラクタ
 */
MessageWindow::~MessageWindow()
{
}

//--------------------------------------------------------------------------------------
/**
 * @brief  表示メイン処理（１回呼び出すごとに処理を更新します）
 *
 * @retval   StrWin::Result   結果コード
 */
//--------------------------------------------------------------------------------------
gfl2::str::StrWin::Result MessageWindow::Proc( void )
{
  if( m_msgCbClass != NULL )
  {
    if( m_isWaitCallback )
    {
      const bool ret = m_msgCbClass->UpdateMsgCallback(m_callbackArg);
      if( ret )
      {
        return gfl2::str::StrWin::RES_CONTINUE;
      }
      else
      {
        m_isWaitCallback = false;
      }
    }
  }

  if( m_waitCnt == 0 )
  {
    gfl2::ui::Button * button = m_uiDevice->GetButton( 0 );
    //Aボタンブースト
    #if 1
    if( m_bCommAutoFlag == false &&
        button->IsHold(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) &&
        m_enableCtrlKey && m_enalbeMsgBoost )
    {
      if( m_waitCnt == 0 && m_isWaitCallback == false )
      {
        // ２回呼ぶのに近い処理内容となるよう一時的にパラメータ書き換え
        u32 waitFrame, numPutChar;
        gfl2::str::StrWin::GetMessageSpeedParams( &waitFrame, &numPutChar );
        if( waitFrame > 0 )
        {
          --waitFrame;
        }
        else
        {
          numPutChar *= 2;
        }
        gfl2::str::StrWin::SetMessageSpeedTemporary( waitFrame, numPutChar );
      }
    }
    #endif

    const gfl2::str::StrWin::Result ret = gfl2::str::StrWin::Proc();
    
    //描画枚にProccesが呼ばれ、最初から描画されるので、
    //ここで色変更フラグはリセットしておく
    m_tagProc.m_bColorChanged = false;
    
    return ret;
  }
  else
  {
    m_waitCnt--;
    if(!m_is60Fps)
    {
      if(m_waitCnt>0)
      {
        m_waitCnt--;
      }
    }
    return gfl2::str::StrWin::RES_CONTINUE;
  }
}


/* -------------------------------------------------------------------------*/
/**
 * @brief ペインの追加
 *
 * @param paneIdx 追加するペインインデックス
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::AddLinePaneIdx( u32 paneIdx )
{
  gfl2::str::StrWin::AddLinePaneIdx( paneIdx );

  // 初めてペイン追加された時、その幅を取得しておく
  if( !m_bFirstPaneAdded )
  {
    // 幅データなどを更新する
    RefreshFirstPane();
  }
  // 追加数を管理
  ++m_pane_num;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief ペインの追加
 *
 * @param paneIdx 追加するペインインデックス
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::AddLinePaneIdxPtr( gfl2::lyt::LytTextBox * pTextBoxPain )
{
  gfl2::str::StrWin::AddLinePaneIdxPtr( pTextBoxPain );

  // 初めてペイン追加された時、その幅を取得しておく
  if( !m_bFirstPaneAdded )
  {
    // 幅データなどを更新する
    RefreshFirstPane();
  }
  // 追加数を管理
  ++m_pane_num;
}



/* -------------------------------------------------------------------------*/
/**
 * @brief 最初に登録したペイン情報の更新
 * @note  AddLinePaneIdx後にデータを書き換えを行いたい場合に使用してください
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::RefreshFirstPane( void )
{
  if( m_paneWork[0].pPane )
  {
    m_font = m_paneWork[0].pPane->GetFont();
    {
      nw::lyt::Size size = m_paneWork[0].pPane->GetFontSize();
      m_paneFontWidth     = size.width;
      m_basePaneFontWidth = size.width;
    }

    {
      nw::lyt::Size size = m_paneWork[0].pPane->GetSize();
      nw::ut::Color8 colorTop    = m_paneWork[0].pPane->GetTextColor( nw::lyt::TEXTCOLOR_TOP );
      nw::ut::Color8 colorBottom = m_paneWork[0].pPane->GetTextColor( nw::lyt::TEXTCOLOR_BOTTOM );

      m_tagProc.SetWindowWidth( static_cast<u32>(size.width) );
      m_tagProc.SetDefaultColor( colorTop, colorBottom );
    }

    m_bFirstPaneAdded = true;

    const nw::lyt::res::ExtUserData *extData = m_paneWork[0].pPane->FindExtUserDataByName("g_fw");
    
    if( extData == NULL )
    {
      extData = m_paneWork[0].pPane->FindExtUserDataByName("g2d_fw");
      if( extData )
      {
        GFL_PRINT("古いg2d_fw定義です。コンバートしなおしてください。\n");
      }
    }
    
    if( extData )
    {
      m_paneFontWidth = extData->GetFloatArray()[0];
      m_basePaneFontWidth = extData->GetFloatArray()[0];
    }
    //      GFL_PRINT("Pane font WIDTH = %5.2f ,  DefWidth=%d\n", m_paneFontWidth, m_font->GetWidth() );
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 位置マトリックスを再初期化する
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::RefreshInitialMtx( void )
{
  for( int i = 0; i < m_pane_num; i++ )
  {
    if( m_paneWork[0].pPane != NULL )
    {
			m_paneWork[i].initialMtx = gfl2::math::ConvertNwVec3ToGfVec3( m_paneWork[0].pPane->GetTranslate() );
      m_paneWork[i].mtx = m_paneWork[i].initialMtx;
    }
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 文字列をセットする
 *
 * @param str    セットする文字バッファ
 * @param langId 表示言語ID(幅縮小のため)
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetString( const gfl2::str::StrBuf& str )
{
  SetString( str, GetMessageLangId() );
}

void MessageWindow::SetString( const gfl2::str::StrBuf& str , System::MSGLANGID langId )
{
  m_tagProc.ResetParam();
  gfl2::str::StrWin::SetString( str );

  // デフォルト処理後、領域幅固定タグをストリームウィンドウ用に置換する
  const gfl2::str::STRCODE* sp = (m_pStrBuf->GetPtr());
  
  //タグ単位でメッセージを縮小する仕組み。
  //kujiraではMessageWindow全体を見て縮小するので不要。
  //動かすにはwordsetを修正する必要あり。
  while( *sp != gfl2::str::EOM_CODE )
  {
    if( *sp == gfl2::str::TAG_START_CODE )
    {
      if( (GetTagGroup(sp) == TAGGROUP_GENERAL_CTRL) &&
          (GetTagIndex(sp) == TAGIDX_GENERAL_FIX_WIDTH_START) )
      {
        f32 areaLimitWidth = m_tagProc.CalcAreaLimitWidth( m_paneFontWidth, GetTagParam(sp, 0) );

        const gfl2::str::STRCODE* next_sp = gfl2::str::SkipTagCode( sp );
        f32 strWidth = m_tagProc.CalcAreaStrWidth( next_sp, m_font, m_paneFontWidth );

        u16  ratio = FP16_One();

        if( strWidth > areaLimitWidth )
        {
          f32 r = areaLimitWidth / strWidth;
          ratio = FP16_From( r );
//          GFL_PRINT( "文字幅:%d, エリア幅：%d, ratio=%5.2f->%d\n", (int)strWidth, (int)areaLimitWidth, r, ratio );
        }

        gfl2::str::STRCODE* r_sp = (gfl2::str::STRCODE*)(sp + 2);
        r_sp[0] = CreateTagCode( TAGGROUP_GENERAL_CTRL, TAGIDX_GENERAL_FIX_WIDTH_DIRECT );
        r_sp[1] = ratio;
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else
    {
      ++sp;
    }
  }

  if( CheckEnableShortStr( m_paneWork[0].pPane,langId ) )
  {
    nw::lyt::Size size = m_paneWork[0].pPane->GetFontSize();
    m_paneFontWidth = m_basePaneFontWidth * (PRINT_EFIGS_SCALE);
    size.width = m_paneFontWidth;

    for( int i=0;i<m_pane_num;i++ )
    {
      m_paneWork[i].pPane->SetFontSize(size);
    }
  }
  else
  {
    nw::lyt::Size size = m_paneWork[0].pPane->GetFontSize();
    m_paneFontWidth = m_basePaneFontWidth;
    size.width = m_paneFontWidth;
    for( int i=0;i<m_pane_num;i++ )
    {
      m_paneWork[i].pPane->SetFontSize(size);
    }
  }

  //ウィンドウをはみ出るようなら縮小する
  const int strLen = GetStringWidthMax();
  const f32 winWidth = m_paneWork[0].pPane->GetSize().width;
  if( strLen > winWidth )
  {
    GFL_PRINT("len[%d]win[%f]\n",strLen,winWidth);
    GFL_PRINT("表示領域をはみ出ました！！！\n");

    nw::lyt::Size size = m_paneWork[0].pPane->GetFontSize();
    m_paneFontWidth = m_paneFontWidth * (winWidth/strLen);
    size.width = m_paneFontWidth;
    //はみ出ることがあったので少しサバを読む
    size.width -= 0.05f;
    for( int i=0;i<m_pane_num;i++ )
    {
      m_paneWork[i].pPane->SetFontSize(size);
#if PM_DEBUG
#if 0 // @note デバッグ処理未実装
      if( GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMsgWidthCheck )
      {
        m_paneWork[i].pPane->SetTextColor(nw::lyt::TEXTCOLOR_TOP    ,nw::ut::Color8::RED );
        m_paneWork[i].pPane->SetTextColor(nw::lyt::TEXTCOLOR_BOTTOM ,nw::ut::Color8::BLUE );
      }
#endif
#endif
    }
  }
#if PM_DEBUG
#if 0 // @note デバッグ処理未実装
  if( GFL_SINGLETON_INSTANCE(xy_debug::DebugWinWork)->mMsgSystemCheck )
  {
    for( int i=0;i<m_pane_num;i++ )
    {
      m_paneWork[i].pPane->SetTextColor(nw::lyt::TEXTCOLOR_TOP    ,nw::ut::Color8::YELLOW );
      m_paneWork[i].pPane->SetTextColor(nw::lyt::TEXTCOLOR_BOTTOM ,nw::ut::Color8::GREEN );
    }
  }
#endif
#endif
}

/* -------------------------------------------------------------------------*/
/**
 * @brief   各ラインの文字幅合計を算出後、最大値を返す
 *
 * @return  文字幅
 */
/* -------------------------------------------------------------------------*/
int MessageWindow::GetStringWidthMax( void )
{
  return print::GetStringWidth( m_pStrBuf , m_font, m_paneFontWidth );
/*
  // デフォルト処理後、領域幅固定タグをストリームウィンドウ用に置換する
  const gfl::str::STRCODE* sp = (m_pStrBuf->GetPtr());
  bool  bCalcMode = false;
  f32 str_width_max = 0;
  f32 str_width = 0;
  s32 line = 0;

  while( *sp != gfl::str::EOM_CODE )
  {
    // 幅リセット
    if( *sp == gfl::str::CR_CODE )
    {
      // 最大文字幅をチェック
      if( str_width_max < str_width )
      {
        str_width_max = str_width;
      }

      str_width = 0;
      ++sp;
    }
    // 通常文字
    else if( *sp != gfl::str::TAG_START_CODE )
    {
      // 幅を計算後文字を進める
      f32 w = m_tagProc.CalcCharWidth( sp, m_font, m_paneFontWidth );
      //str_width += gfl::math::FCeil(w);
      str_width += w;
      ++sp;
    }
    // タグ
    else
    {
      // タグをスキップし、幅を計算

      TagGroup  tagGroup = GetTagGroup( sp );
      u32       tagIdx   = GetTagIndex( sp );
      //改行タグをチェック
      if( tagGroup == TAGGROUP_STREAM_CTRL &&
         (tagIdx == TAGIDX_STREAM_LINE_FEED || tagIdx == TAGIDX_STREAM_PAGE_CLEAR ) )
      {
        if( str_width_max < str_width )
        {
          str_width_max = str_width;
        }
        str_width = 0;
      }

      sp = gfl::str::SkipTagCode( sp );
      //ここで文字幅をたすと、連続でタグが入っているとまずい。
    }
  }

  // 最終行をチェック
  if( str_width_max < str_width )
  {
    str_width_max = str_width;
  }

  return static_cast<int>(gfl::math::FCeil(str_width_max));
*/
}

/* -------------------------------------------------------------------------*/
/**
 * @brief フォントの高さを取得する
 *
 * @return フォントの高さ
 */
/* -------------------------------------------------------------------------*/
int MessageWindow::GetFontHeight( void )
{
  if( m_paneWork[0].pPane )
  {
    return static_cast<int>( m_paneWork[0].pPane->GetFontSize().height );
  }
  GFL_ASSERT_MSG(0,"フォント高さを取得できません\n"); //@check 対処済み
  return 0;
}

/**
 *  改行リリース判定
 */
bool MessageWindow::IsPauseRelease( void )
{
  const USER_INPUT_TYPE ret = checkUserInput();
  if( ret != USER_INPUT_NO )
  {
    if( ret == USER_INPUT_UI )      //自動送りの時はSEを鳴らさない
    {
      if(m_LineBreakSE!=0)
      {
        Sound::PlaySE( m_LineBreakSE );
      }
      else
      {
        Sound::PlaySE( SEQ_SE_MESSAGE );
      }
    }
    return true;
  }
  return false;
}
/**
 *  表示終了後フィニッシュ判定
 */
bool MessageWindow::IsFinishInput( void )
{
  const USER_INPUT_TYPE ret = checkUserInput();
  if( ret != USER_INPUT_NO )
  {
    return true;
  }
  return false;
}
/**
 *  色変更を行ったかどうか判定
 */
bool MessageWindow::IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom )
{
  bool result = m_tagProc.IsColorChanged( top, bottom );
  return result;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief キー入力可否設定
 * @note  trueで一時キー入力無効
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetSuspendKey( bool flg )
{
  m_bSuspendKey = flg;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 通信時自動キー送り(+メッセージブースト無効)
 * @note  trueで自動送り
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetMessageAutoFlg( bool flg )
{
  m_bCommAutoFlag = flg;
}

//--------------------------------------------------------------------------
/**
 *	@brief  自動キー送りを次の1度だけ行う
 */
//--------------------------------------------------------------------------
void MessageWindow::SetMessageAutoOnce( void )
{
  m_bAutoFlagOnce = true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief メッセージ速度の設定(config版)
 *        デフォルト値は変更しません。
 *
 * @param  Savedata::ConfigSave::MSG_SPEED spd メッセージ速度
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd , bool is60Fps )
{
  m_is60Fps = is60Fps;

  bool isFast = false;
  
  //日本語、韓国語
  static const int frameWaitArr[Savedata::ConfigSave::MSG_SPEED_MAX] = { 1,0,0,0 };
  static const u32 putCountArr[Savedata::ConfigSave::MSG_SPEED_MAX]  = { 1,1,2,255 };

  static const int frameWaitArr60[Savedata::ConfigSave::MSG_SPEED_MAX] = { 2,1,0,0 };
  static const u32 putCountArr60[Savedata::ConfigSave::MSG_SPEED_MAX]  = { 1,1,1,255 };

  //そのほかの言語の場合
  static const u32 putCountArr_Fast[Savedata::ConfigSave::MSG_SPEED_MAX]   = { 2,2,4,255 };
  static const u32 putCountArr60_Fast[Savedata::ConfigSave::MSG_SPEED_MAX] = { 2,2,2,255 };

#if PM_DEBUG
  if( mDebugUseFlag )
  {
    gfl2::str::StrWin::SetMessageSpeed( mDebugWaitFrame, mDebugPutCount );
    return;  //!< 以降の処理をスキップ
  }
#endif

  switch(System::GetLang()){
  case POKEMON_LANG_JAPAN:
  case POKEMON_LANG_KOREA:
  case POKEMON_LANG_TAIWAN:
  case POKEMON_LANG_CHINA:
    break;
  case POKEMON_LANG_ENGLISH:
  case POKEMON_LANG_FRANCE:
  case POKEMON_LANG_ITALY:
  case POKEMON_LANG_GERMANY:
  case POKEMON_LANG_SPAIN:
    isFast=true;
    break;
  default:
    GFL_ASSERT(0);//GFNMCat5479  言語が増えたら速度を設定すること
    break;
  }
  
  if( is60Fps ){
    if(!isFast){
      gfl2::str::StrWin::SetMessageSpeed( frameWaitArr60[spd] , putCountArr60[spd] );
    }
    else{
      gfl2::str::StrWin::SetMessageSpeed( frameWaitArr60[spd] , putCountArr60_Fast[spd] );
    }
  }
  else{
    if(!isFast){
      gfl2::str::StrWin::SetMessageSpeed( frameWaitArr[spd] , putCountArr[spd] );
    }
    else{
      gfl2::str::StrWin::SetMessageSpeed( frameWaitArr[spd] , putCountArr_Fast[spd] );
    }
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief デフォルトメッセージ速度の設定
 *
 * @param  savedata::Config::MSG_SPEED spd メッセージ速度
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetDefaultMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd )
{
  mDefaultMsgSpeed = spd;
}

#if PM_DEBUG
void MessageWindow::SetDebugMessageSpeedUseFlag( bool flag )
{
  mDebugUseFlag = flag;
}
bool MessageWindow::GetDebugMessageSpeedUseFlag( void )
{
  return mDebugUseFlag;
}
void MessageWindow::SetDebugMessageSpeed( u32 waitFrame, u32 putCount )
{
  mDebugWaitFrame = waitFrame;
  mDebugPutCount  = putCount;
}
#endif

//-----------------------------------------------------------------------
/**
 *  @brief      行更新時に呼ばれる
 */
//-----------------------------------------------------------------------
void MessageWindow::RefreshLine(void)
{
  m_tagProc.ResetParam();
}

//-----------------------------------------------------------------------
/**
 * TagProcessor が処理したタグによっては
 * 続けてすぐに次の文字を処理してほしくない場合がある。
 * そのような場合、この関数を継承してタグコードをチェックし、
 * 続けて次の文字を処理させたくない（１度タグ処理だけを行うフレームを設けたい）場合に
 * true を返すようにする。
 *
 * @param   tagInfo   タグ情報
 *
 * @retval  bool    次の文字をすぐに処理しない場合 true を返す
 */
//-----------------------------------------------------------------------
bool MessageWindow::CheckTagProcBreak( const TagInfo& tagInfo )
{
  TagGroup tagGroup = TagCodeToGroup( tagInfo.code );
  u8       tagIdx   = TagCodeToIndex( tagInfo.code );

  if( tagGroup == TAGGROUP_STREAM_CTRL )
  {
    if( tagIdx == TAGIDX_STREAM_WAIT_ONE ){
      SetWaitCnt(tagInfo.params[0]);
      return true;
    }
    else   ///< コールバック引数変更(単発
    if( tagIdx == TAGIDX_STREAM_CALLBACK_ONE )
    {
      if( m_msgCbClass != NULL )
      {
        const bool ret = m_msgCbClass->UpdateMsgCallback( tagInfo.params[0] ) ;
        if( ret )
        {
          m_isWaitCallback = true;
          m_callbackArg = tagInfo.params[0];
        }
        return ret;
      }
      return true;
    }
  }

  return false;
}


/**
 *  ユーザー入力判定（改行リリース／表示後フィニッシュ共通）
 */
MessageWindow::USER_INPUT_TYPE MessageWindow::checkUserInput( void )
{
  if( m_bSuspendKey )
  {
    return USER_INPUT_NO;
  }

  if( m_bCommAutoFlag )
  {
    return USER_INPUT_AUTO;
  }

  if( m_bAutoFlagOnce )
  {
    m_bAutoFlagOnce = false; // 1度だけ有効なのでリセット
    return USER_INPUT_AUTO;
  }

  gfl2::ui::Button * button = m_uiDevice->GetButton(0);

  if( button->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) && m_enableCtrlKey )
  {
    return USER_INPUT_UI;
  }

  if( m_enableCtrlTouch )
  {
    gfl2::ui::TouchPanel * tp = m_uiDevice->GetTouchPanel(0);
    if( tp->IsTouchTrigger() )
    {
      return USER_INPUT_UI;
    }
  }

  return USER_INPUT_NO;
}

void MessageWindow::CallMessgaeCallback(u16 arg)
{
  if( m_msgCbClass )
  {
    const bool ret = m_msgCbClass->UpdateMsgCallback(arg);
    if( ret )
    {
      m_isWaitCallback = true;
      m_callbackArg = arg;
    }
  }
}




void MessageWindow::SetWaitCnt( int cnt )
{
  m_waitCnt = cnt;
}


//  文字列がウィンドウの幅に収まるかチェックする
bool MessageWindow::CheckStringWidth( const gfl2::str::StrBuf& str )
{

  gfl2::str::StrWin::SetString( str );

  const int strLen = GetStringWidthMax();
  const f32 winWidth = m_paneWork[0].pPane->GetSize().width;

  //  GFL_PRINT(">>>>> strLen[%d] : window[%f]\n",strLen,winWidth);

  if( strLen > winWidth )
    {
      return false;
    }

 return true;
}

//==============================================================================================
//==============================================================================================
nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::Process( u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext )
{
  const gfl2::str::STRCODE* sp  = pContext->str - 1;

  // ペインサイズを超える文字描画が行われると、なぜかタグ開始コードを伴わずに呼ばれることがある。原因不明。とりあえず回避。
  //  ↑crコードとかスペースとかは普通に来ることがある
  if( *sp != gfl2::str::TAG_START_CODE )
  {
//    GFL_PRINT("[TagProcess] PainSize Over: code=%04x,   str:", code );
//    for(u32 i=0; i<8; ++i){
//      GFL_PRINT("%04x, ", sp[i]);
//    }
//    GFL_PRINT("\n");
    return nw::font::TagProcessorBase<gfl2::str::STRCODE>::Process( code, pContext );
  }

  Operation retType = OPERATION_DEFAULT;

  TagGroup  tagGroup = GetTagGroup( sp );
  u32       tagIdx   = GetTagIndex( sp );

//  GFL_PRINT("[TagProcess] code=%04x, TagGroup,Idx=(0x%02x : 0x%02x)\n", code, tagGroup, tagIdx);

  switch( tagGroup ){
  case TAGGROUP_GENERAL_CTRL:
    retType = process_GeneralCtrl( pContext, tagIdx, sp );
    break;
  case TAGGROUP_STREAM_CTRL:
    retType = process_StreamCtrl( pContext, tagIdx, sp );
    break;
  case TAGGROUP_SYSTEM:
    switch( tagIdx )
    {
      case TAGIDX_SYSTEML_CHANGE_COLOR:
      {
        u16  colorIdx = GetTagParam( sp, 0 );
        if( colorIdx == 0 )
        {
          resetTextColor(pContext->writer);
        }
        else
        {
          static const nw::ut::Color8 colorTable[] = {
            nw::ut::Color8(   0,   0,   0, 255 ),   // 黒(上でリセット処理になってる)
            nw::ut::Color8( 255,   0,   0, 255 ),   // 赤
            nw::ut::Color8(   0, 140, 255, 255 ),   // 青
            nw::ut::Color8(   0, 192,   0, 255 ),   // 緑
          };
          if( colorIdx >= GFL_NELEMS(colorTable) )
          {
            GFL_ASSERT( 0 );
          }
          else
          {
            setTextColor( pContext->writer , colorTable[colorIdx] );
          }
        }
      }
      break;
    }
    //個々で戻り値を変える場合CalcRectも対応すること
    retType = OPERATION_NO_CHAR_SPACE;
    break;
  default:
    GFL_PRINT("OtherTagGroup[%d][%d]\n",tagGroup,tagIdx);
    break;
  }

  if( m_topStrPos < pContext->str )
  {
    m_topStrPos = pContext->str;
  }

  pContext->str = gfl2::str::SkipTagCode( sp );

  return retType;
}
/**
 *  汎用コントロール処理
 */
nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::process_GeneralCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp )
{
  switch( tagIdx ){
  case TAGIDX_GENERAL_CHANGE_COLOR:
    {
      static const nw::ut::Color8 colorTable[] = {
        nw::ut::Color8( 255,   0,   0, 255 ),   // 赤
        nw::ut::Color8(   0, 192,   0, 255 ),   // 緑
        nw::ut::Color8(   0, 140, 255, 255 ),   // 青
        nw::ut::Color8( 255, 128,   0, 255 ),   // オレンジ
        nw::ut::Color8( 255, 128, 255, 255 ),   // ピンク
        nw::ut::Color8( 140,   0, 255, 255 ),   // 紫
        nw::ut::Color8(  80, 200, 228, 255 ),   // 水色
        nw::ut::Color8( 255, 230,  80, 255 ),   // 急所用黄
        nw::ut::Color8( 100, 180, 255, 255 ),   // コンテスト用青
        nw::ut::Color8( 255, 255,  80, 255 ),   // [momiji] StaffList用役職カラー
      };

      u16  colorIdx = GetTagParam( sp, 0 );
//      GFL_PRINT("colorIdx=%d..\n", colorIdx);
      if( colorIdx < GFL_NELEMS(colorTable) )
      {
        setTextColor( pContext->writer , colorTable[colorIdx] );
      }
    }
    break;

  case TAGIDX_GENERAL_BACK_COLOR:
    resetTextColor(pContext->writer);
    break;

  case TAGIDX_GENERAL_X_CENTERING:
    {
      if( m_windowWidth != 0 )
      {
        sp = gfl2::str::SkipTagCode( sp );

        u32 strWidth = static_cast<u32>( calcStrWidth( pContext, sp ) );

        f32 xpos = static_cast<f32>( (m_windowWidth-strWidth)/2 );
        pContext->writer->SetCursorX( xpos );
      }
      else
      {
        GFL_PRINT("Window幅が0になっているのでセンタリングタグは使えません！\n");
      }
    }
    break;

  case TAGIDX_GENERAL_X_FIT_RIGHT:       ///< 文字描画Ｘ座標 右寄せ
    {
      if( m_windowWidth != 0 )
      {

        f32 margin = (f32)(GetTagParam( sp,0 ));
        f32 strWidth = calcStrWidth( pContext, sp );
        // @fix GFNMCat[3592]: kujiraでの-1.5fの調整をするとぼやけるので対処
//        f32 xpos = (m_windowWidth - strWidth - margin - 1.5f);//誤差が出るので調整
        f32 xpos = (m_windowWidth - strWidth - margin);
        //GFL_PRINT("[%f][%d][%f][%f][%f]\n",strWidth ,m_windowWidth,xpos,margin,pContext->writer->GetCursorX());
        pContext->writer->SetCursorX( xpos );
      }
      else
      {
        GFL_PRINT("Window幅が0になっているので右寄せタグは使えません！\n");
      }
    }
    break;

  case TAGIDX_GENERAL_X_ADD:
    {
      f32  x = pContext->writer->GetCursorX() + (f32)(GetTagParam(sp,0));
      pContext->writer->SetCursorX( x );
    }
    break;

  case TAGIDX_GENERAL_X_MOVE:        ///< 文字描画Ｘ座標 指示
    {
      f32  x = (f32)( GetTagParam(sp,0) );
      pContext->writer->SetCursorX( x );
    }
    break;

  case TAGIDX_GENERAL_FIX_WIDTH_START:
    {
      u16 r_value = this->calcStrRatioValue( sp, pContext );

      if( r_value != FP16_One() )
      {
        m_defaultScale = pContext->writer->GetScaleH();

        f32  ratio = FP16_To( r_value );
        f32  tmpScale = m_defaultScale * ratio;

//        GFL_PRINT("[TagProc] FixWidthStart Reverse ratio = %5.2f, scale->%5.2f\n", ratio, tmpScale );
        pContext->writer->SetScale( tmpScale, pContext->writer->GetScaleV() );
      }
    }
    break;

  case TAGIDX_GENERAL_FIX_WIDTH_DIRECT:
    {
       m_defaultScale = pContext->writer->GetScaleH();

       f32  ratio = FP16_To( GetTagParam(sp, 0) );
       f32  tmpScale = m_defaultScale * ratio;

//       GFL_PRINT("[TagProc] FixWidthDirect Reverse ratio = %5.2f, scale->%5.2f\n", ratio, tmpScale );
       pContext->writer->SetScale( tmpScale, pContext->writer->GetScaleV() );
    }
    break;

  case TAGIDX_GENERAL_FIX_WIDTH_END:
    //
    if( m_defaultScale != 0.0f )
    {
      pContext->writer->SetScale( m_defaultScale, pContext->writer->GetScaleV() );
    }
    break;
  case TAGIDX_GENERAL_NOTUSED_MESSAGE:    // コンバータで未使用メッセージに仕込んであるタグ
#if PM_DEBUG
    {
      uint messageId = GetTagParam( sp, 0 );
      GFL_ASSERT_MSG(0, "未使用メッセージ(%d)が表示されました", messageId);//@check 
    }
#endif
    break;
  default:
    GFL_PRINT("OtherTagCode[%d]\n",tagIdx);
    break;
  }
  //個々で戻り値を変える場合CalcRectも対応すること
  return OPERATION_NO_CHAR_SPACE;
}

nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::process_StreamCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp )
{
  switch( tagIdx ){
  case TAGIDX_STREAM_LINE_FEED:      ///< ページ切り替え(行送り
    //外で処理されてここには来ていない。
    GFL_ASSERT_MSG(0,"ページ切り替え(行送り タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_PAGE_CLEAR:     ///< ページ切り替え(クリア
    //外で処理されてここには来ていない。
    GFL_ASSERT_MSG(0,"ページ切り替え(クリア タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_WAIT_ONE:       ///< ウェイト変更(単発）
    if( m_msgWin )
    {
      if( m_topStrPos < pContext->str )
      {
        //タグチェック時に入れる。
        //u16 cnt = GetTagParam( sp, 0 );
        //m_msgWin->SetWaitCnt(cnt);
        //GFL_PRINT("[%8x][%8x]\n",m_topStrPos,pContext->str);
      }
    }
    break;
  case TAGIDX_STREAM_WAIT_CONT:      ///< ウェイト変更(継続）
    GFL_ASSERT_MSG(0,"ウェイト変更(継続 タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_WAIT_RESET:     ///< ウェイトリセット
    //GFL_ASSERT_MSG(0,"ウェイトリセット タグは未実装です。");//@check コメント これは以前使われていたタグ。ウェイト継続は使われていないので本来は要らない。
    break;
  case TAGIDX_STREAM_CALLBACK_ONE:   ///< コールバック引数変更(単発
    if( m_msgWin )
    {
      if( m_topStrPos < pContext->str )
      {
        //タグチェック時に入れる。
        //u16 arg = GetTagParam( sp, 0 );
        //m_msgWin->CallMessgaeCallback(arg);
      }
    }
    break;
  case TAGIDX_STREAM_CALLBACK_CONT:  ///< コールバック引数変更(継続
    GFL_ASSERT_MSG(0,"コールバック引数変更(継続 タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_CALLBACK_RESET: ///< コールバック引数リセット
    GFL_ASSERT_MSG(0,"コールバック引数リセット タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_CLEAR_WIN:      ///< 描画領域クリア
    GFL_ASSERT_MSG(0,"描画領域クリア タグは未実装です。");//@check 作成時に気づく
    break;
  case TAGIDX_STREAM_CTRL_SPEED:     ///< 速度コントロール
    GFL_ASSERT_MSG(0,"速度コントロール タグは未実装です。");//@check 作成時に気づく
    break;
  }
  //個々で戻り値を変える場合CalcRectも対応すること
  return OPERATION_DEFAULT;

}

nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
    FontTagProc::CalcRect( nw::ut::Rect * pRect, u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext )
{
  const gfl2::str::STRCODE* sp  = pContext->str - 1;

  // ペインサイズを超える文字描画が行われると、なぜかタグ開始コードを伴わずに呼ばれることがある。原因不明。とりあえず回避。
  //  ↑crコードとかスペースとかは普通に来ることがある
  if( *sp != gfl2::str::TAG_START_CODE )
  {
//    GFL_PRINT("[TagProcess] PainSize Over: code=%04x,   str:", code );
//    for(u32 i=0; i<8; ++i){
//      GFL_PRINT("%04x, ", sp[i]);
//    }
//    GFL_PRINT("\n");
    return nw::font::TagProcessorBase<gfl2::str::STRCODE>::CalcRect( pRect,code, pContext );
  }

  TagGroup  tagGroup = GetTagGroup( sp );

  switch( tagGroup )
  {
  case TAGGROUP_GENERAL_CTRL:
  case TAGGROUP_STREAM_CTRL:
  case TAGGROUP_SYSTEM:
    pContext->str = gfl2::str::SkipTagCode( sp );
    return OPERATION_NO_CHAR_SPACE;
  }
  
  pContext->str = gfl2::str::SkipTagCode( sp );
  return OPERATION_DEFAULT;
}


u16 FontTagProc::calcStrRatioValue( const gfl2::str::STRCODE* sp, nw::font::PrintContext<gfl2::str::STRCODE>* pContext )
{
  f32 areaWidth = this->CalcAreaLimitWidth( pContext->writer->GetFontWidth(), GetTagParam(sp, 0) );

  sp = gfl2::str::SkipTagCode( sp );

  f32 strWidth = this->CalcAreaStrWidth( sp, pContext->writer->GetFont(), pContext->writer->GetFontWidth() );

  if( strWidth > areaWidth )
  {
    f32 ratio = areaWidth / strWidth;
    return FP16_From( ratio );
  }
  return FP16_One();
}

//
f32 FontTagProc::CalcAreaLimitWidth( f32 curFontWidth, u16 maxCharValue )
{
  f32  areaWidth = (f32)((u32)(maxCharValue * curFontWidth) >> 8);

//  GFL_PRINT("[CalcArea] fontW=%5.2f, areaChars=%04x, areaWidth=%5.2f\n", curFontWidth, maxCharValue, areaWidth );
  return areaWidth;
}
//
f32 FontTagProc::CalcAreaStrWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth )
{
  int width = 0;

//  GFL_PRINT("[CalcWidth] ..\n");

  while( *sp != gfl2::str::EOM_CODE )
  {
    if( *sp == gfl2::str::TAG_START_CODE )
    {
      if( (GetTagGroup(sp) == TAGGROUP_GENERAL_CTRL) && (GetTagIndex(sp) == TAGIDX_GENERAL_FIX_WIDTH_END) )
      {
        break;
      }

      sp = gfl2::str::SkipTagCode( sp );

    }
    else
    {
      width += font->GetCharWidth( *sp );
//      GFL_PRINT("  code=%04x, width=%d\n", *sp, font->GetCharWidth( *sp ) );
      ++sp;
    }
  }

  f32  ratio = curFontWidth / (f32)(font->GetWidth());
  f32  f_width = (f32)width * ratio;

//  GFL_PRINT("[CalcWidth] srcWidth=%d, fontRatio=%5.2f, resultWidth=%5.2f\n", width, ratio, f_width);

  return f_width;
}

void FontTagProc::setTextColor( nw::font::TextWriterBase<gfl2::str::STRCODE>* pWriter  , const nw::ut::Color8 &changeCol )
{
  if( m_bColorChanged )
  {
    GFL_ASSERT_MSG(0,"色変更タグが入れ子になっています。\n");//@check 対処済み
    return;
  }
  
  m_defaultColorTop    = pWriter->GetGradationStartColor();
  m_defaultColorBottom = pWriter->GetGradationEndColor();
  m_currentColorTop    = changeCol;
  m_currentColorBottom = changeCol;
  /*
  GFL_PRINT( " prev  r:%3d, g:%3d, b:%3d -> r:%3d, g:%3d, r:%3d\n", m_defaultColorTop.r, m_defaultColorTop.g, m_defaultColorTop.b,
    m_defaultColorBottom.r, m_defaultColorBottom.g, m_defaultColorBottom.b );
  GFL_PRINT( " next  r:%3d, g:%3d, b:%3d \n", m_currentColorTop.r, m_currentColorTop.g, m_currentColorTop.b );
  */
  pWriter->SetTextColor( changeCol );
  m_bColorChanged = true;
  //GFL_PRINT("[STRWIN] TexColor Change !!!\n");
}

void FontTagProc::resetTextColor(nw::font::TextWriterBase<gfl2::str::STRCODE>* pWriter )
{
  if( m_bColorChanged )
  {
    pWriter->SetTextColor( m_defaultColorTop, m_defaultColorBottom );
    m_bColorChanged = false;
    //GFL_PRINT("[STRWIN] TexColor Reset    !!!\n");
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 文字の幅を取得する
 *
 * @param sp            取得したい文字をさしているポインタ
 * @param font          フォント
 * @param curFontWidth  フォント幅
 *
 * @return 文字幅
 */
/* -------------------------------------------------------------------------*/
f32 FontTagProc::CalcCharWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth )
{
  int width = font->GetCharWidth( *sp );
  f32  ratio = curFontWidth / static_cast<f32>(font->GetWidth());
  f32  f_width = (f32)width * ratio;

  return f_width;
}

//メッセージをセットする前に1回呼んで内部パラメータリセット
void FontTagProc::ResetParam(void)
{
  m_topStrPos = 0;
  m_bColorChanged = false;
}




//--------------------------------------------------------
/**
 *  文字列描画サイズ（幅）を計算
 */
//--------------------------------------------------------
f32 FontTagProc::calcStrWidth( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, const gfl2::str::STRCODE* sp )
{
  u32 len = gfl2::str::LineLen( sp, 0 );
  f32 width = pContext->writer->CalcStringWidth( sp, len );
  return width;
}
//==============================================================================================
/**
 *  タグ処理により描画色が変えられているかどうか判定
 *
 *  @param[out]   top       描画色が変えられている場合、グラデーション開始色が入る
 *  @param[out]   top       描画色が変えられている場合、グラデーション終端色が入る
 *
 *  @retval   bool      描画色が変えられている場合は true
 */
//==============================================================================================
bool FontTagProc::IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom ) const
{
  if( m_bColorChanged )
  {
    *top    = m_currentColorTop;
    *bottom = m_currentColorBottom;
    return true;
  }
  return false;
}


GFL_NAMESPACE_END( print )

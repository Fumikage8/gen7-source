//===========================================================================
/**
 * @file    gfl2_StrWin.cpp
 * @brief   GAMEFREAK Library  String Print System ( 文字表示ウィンドウ )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#include <str/include/gfl2_StrWin.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

//=============================================================================================
/**
 * コンストラクタ
 *
 * @param[in]   lytWk       表示に使用するレイアウトワークポインタ
 * @param[in]   numLines    表示行数
 * @param[in]   waitFrames  １文字出力ごとのwaitフレーム値
 * @param[in]   pHeap       ヒープハンドル
 * @param[in]   pTagProc    タグプロセッサ（不要ならNULLでも可）
 * @param[in]   pTagProc    タグプロセッサ（不要ならNULLでも可）
 */
//=============================================================================================
StrWin::StrWin( gfl2::lyt::LytWk* lytWk, u8 numLines, u32 waitFrames, gfl2::heap::HeapBase* pHeap, TagProcBase* pTagProc, u32 putCharCount )
  : m_paneWork(NULL)
  , m_pStrBuf(NULL)
  , m_pTagProc(pTagProc)
  , m_pHeap(pHeap)
  , m_pLytWk(lytWk)
  , m_finishType(FINISH_NONE)
  , m_lineFadeFrames(DEFAULT_FADE_FRAMES)
  , m_lineScrollFrames(DEFAULT_SCROLL_FRAMES)
  , m_numDispLines(numLines)
  , m_numStrLines(0)
  , m_numPanes(0)
  , m_currentReadLine(0)
  , m_currentDispLine(0)
  , m_waitFrame(waitFrames)
  , m_countFrame(0)
  , m_targetPaneIdx(0)
  , m_hidePaneIdx(0)
  , m_lineFeedSeq(0)
  , m_finishFrame(0)
  , m_finishFrameCount(0)
  , m_putCharCountPerFrame(putCharCount)
  , m_seq(SEQ_DONE)
  , m_readIndex(0)
  , m_readLineIndex(0)
  , m_strLength(0)
  , m_bReadLineDone(false)
  , m_bReadTag(false)
  , m_bForceQuit(false)
  , m_procMode(PROC_MODE_NORMAL)
  , m_bAvirable(false)
  , m_bPaneSetup(false)
{
//  debug::SetPrintFlag( STRWIN_PRINT_FLAG );

  if( m_numDispLines>0 && m_putCharCountPerFrame>0 )
  {
    m_paneWork = static_cast<PANE_WORK*>GflHeapAllocMemory( pHeap, sizeof(PANE_WORK)*numLines );
    if( m_paneWork )
    {
      for( u32 i=0; i<numLines; i++ )
      {
        m_paneWork[i].pPane = NULL;
      }
      m_waitFrameOrg = m_waitFrame;
      m_putCharCountPerFrameOrg = m_putCharCountPerFrame;
      m_bAvirable = true;
      return;
    }
  }
  else
  {
    GFL_ASSERT(0);  // numLines > 0 な必要がある
  }
}
//=============================================================================================
/**
 * デストラクタ
 */
//=============================================================================================
StrWin::~StrWin()
{
  if( m_pStrBuf )
  {
    GFL_DELETE( m_pStrBuf );
  }
  if( m_paneWork )
  {
    GflHeapFreeMemory( m_paneWork );
  }
}


//=============================================================================================
/**
 * @brief 文字表示に使うペインのIndexを追加。表示行数と同数だけ追加する必要がある。
 *
 * @param[in]   paneIdx   ペインIndex
 */
//=============================================================================================
void StrWin::AddLinePaneIdx( u32 paneIdx )
{
  GFL_ASSERT( m_bAvirable );
  AddLinePaneIdxWithStrMinLen( paneIdx, LINE_BUFFSER_SIZE_MIN );
}


//=============================================================================================
/**
 * @brief 文字表示に使うペインのIndexを追加。表示行数と同数だけ追加する必要がある。
 *
 * @param[in]   paneIdx   ペインIndex
 */
//=============================================================================================
void StrWin::AddLinePaneIdxPtr( gfl2::lyt::LytTextBox * pTextBoxPain )
{
  GFL_ASSERT( m_bAvirable );
  AddLinePaneIdxWithStrMinLenPtr( pTextBoxPain, LINE_BUFFSER_SIZE_MIN );
}



//=============================================================================================
/**
 * @brief 文字表示に使うペインのIndexを追加。表示行数と同数だけ追加する必要がある。
 *
 * @param[in]   paneIdx   ペインIndex
 */
//=============================================================================================
void StrWin::baseAddLinePaneStrMinLenPtr( gfl2::lyt::LytTextBox * pTextBoxPain, u32 strMinLen )
{
  GFL_ASSERT( m_bAvirable );

  if( m_bAvirable )
  {
    if( m_numPanes < m_numDispLines )
    {
      m_paneWork[m_numPanes].pPane = pTextBoxPain;
      if( m_paneWork[m_numPanes].pPane != NULL )
      {
        if( m_paneWork[m_numPanes].pPane->GetStringBufferLength() < strMinLen )
        {
          m_pLytWk->AllocTextBoxPaneStringBuffer( pTextBoxPain, strMinLen );
        }
        if( m_pTagProc != NULL )
        {
          m_paneWork[m_numPanes].pPane->SetTagProcessor( m_pTagProc );
        }
        m_paneWork[m_numPanes].initialMtx = gfl2::math::ConvertNwVec3ToGfVec3( m_paneWork[m_numPanes].pPane->GetTranslate() );
        m_paneWork[m_numPanes].mtx = m_paneWork[m_numPanes].initialMtx;
        m_paneWork[m_numPanes].myPosIdx = m_numPanes;
        m_paneWork[m_numPanes].timer = 0;
        if( ++m_numPanes == m_numDispLines )
        {
          m_bPaneSetup = true;
        }
      }
      else
      {
        GFL_ASSERT(0);  // ペインが見つからない
      }
    }
    else
    {
      GFL_ASSERT(0);  // これ以上ペインを登録できない
    }
  }
}



//=============================================================================================
/**
 * @brief 文字表示に使うペインのIndexを追加。表示行数と同数だけ追加する必要がある。
 *
 * @param[in]   paneIdx   ペインIndex
 */
//=============================================================================================
void StrWin::AddLinePaneIdxWithStrMinLen( u32 paneIdx, u32 strMinLen )
{

  baseAddLinePaneStrMinLenPtr( m_pLytWk->GetTextBoxPane( paneIdx ),strMinLen );
}


//=============================================================================================
/**
 * @brief 文字表示に使うペインのIndexを追加。表示行数と同数だけ追加する必要がある。
 *
 * @param[in]   gfl2::lyt::LytTextBox*  pTextBoxPain
 */
//=============================================================================================
void StrWin::AddLinePaneIdxWithStrMinLenPtr( gfl2::lyt::LytTextBox * pTextBoxPain, u32 strMinLen )
{
  baseAddLinePaneStrMinLenPtr(pTextBoxPain,strMinLen);
}


//=============================================================================================
/**
 *  @brief  表示する文字列を設定する
 *
 * @param[in]   str   表示する文字列
 *                  （この関数を呼んだ後はクラス内部で内容を保持するので、
 *                    元の文字列は破棄しても構いません）
 */
//=============================================================================================
void StrWin::SetString( const StrBuf& str )
{
  GFL_PRINT_EX(STRWIN_PRINT_FLAG, "[STRWIN] paneSetup = %d\n", m_bPaneSetup );

  GFL_ASSERT( m_bPaneSetup );

  if( m_bPaneSetup )
  {
//    GFL_PRINT("**** StrWin Set Strings Below ****\n");
//    PrintCode( str.GetPtr() );

    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] set string...\n");

    m_strLength = str.GetLength();
    m_readIndex = 0;
    m_readLineIndex = 0;
    m_bReadLineDone = false;

    u32 buf_size = m_strLength + 1;
    if( buf_size < STR_BUFFER_SIZE_MIN )
    {
      buf_size = STR_BUFFER_SIZE_MIN;
    }

    if( (m_pStrBuf != NULL) && (m_pStrBuf->GetSize() < buf_size) )
    {
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] remove buffer\n");
      GFL_DELETE( m_pStrBuf );
      m_pStrBuf = NULL;
    }

    if( m_pStrBuf == NULL )
    {
      m_pStrBuf = GFL_NEW(m_pHeap) StrBuf( buf_size, m_pHeap );
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] create new buffer\n");
    }

    if( m_pStrBuf )
    {
      m_pStrBuf->SetStr( str );
      m_numStrLines = CountLines( m_pStrBuf->GetPtr() );
    }
    else
    {
      GFL_ASSERT(0);
      m_numStrLines = 1;
    }

    m_currentReadLine  = 0;
    m_finishFrameCount = 0;

    setNextSequence( SEQ_PUT );
    setupReader();
    this->Clear();
  }
}


//=============================================================================================
/**
 * @brief    表示を最後まで行う
 */
//=============================================================================================
void StrWin::ProcAtOnce( void )
{
  GFL_ASSERT( m_bPaneSetup );

  m_procMode = PROC_MODE_AT_ONCE;

  Result res = this->Proc();
  while( (res != RES_DONE) && (res != RES_FINISH) )
  {
    res = this->Proc();
  }

  m_procMode = PROC_MODE_NORMAL;
}

//=============================================================================================
/**
 * @brief  １ページ分の表示処理を行う
 */
//=============================================================================================
StrWin::Result StrWin::ProcForPage( void )
{
  GFL_ASSERT( m_bPaneSetup );

  m_procMode = PROC_MODE_FOR_PAGE;

  Result res = this->Proc();
  while( (res != RES_DONE) && (res != RES_FINISH) && (res != RES_PAUSE) )
  {
    res = this->Proc();
  }

  m_procMode = PROC_MODE_NORMAL;

  return res;
}


//=============================================================================================
/**
 * @brief  表示メイン処理（１回呼び出すごとに処理を更新します）
 *
 * @retval   StrWin::Result   結果コード（gfl_StrWin.h 参照）
 */
//=============================================================================================
StrWin::Result StrWin::Proc( void )
{
  GFL_ASSERT( m_bPaneSetup );

  Result     result;
  SubResult  subResult;
  int  proc_count = 0;
  bool bDone = false;

  do
  {
    result = this->procCore( &subResult );

    switch( subResult ){
    case SUB_RES_FORCE_QUIT:
      bDone = true;
      break;

    case SUB_RES_FORCE_RECALL:
      break;

    case SUB_RES_DEFAULT:
      if( result != RES_CONTINUE )
      {
        bDone = true;
        break;
      }
      if( ++proc_count >= m_putCharCountPerFrame )
      {
        bDone = true;
        break;
      }
      break;
    }

  }while(!bDone);

  if( m_putState == PUT_PASSED_PROC )
  {
    m_putState = PUT_PASSED_FRAME;
  }

  m_waitFrame = m_waitFrameOrg;
  m_putCharCountPerFrame = m_putCharCountPerFrameOrg;

  return result;
}

//-----------------------------------------------------------------------------
/**
 *  Proc の メイン処理部分
 */
//-----------------------------------------------------------------------------
StrWin::Result StrWin::procCore( SubResult* subResult )
{
  *subResult = SUB_RES_DEFAULT;

  switch( m_seq ){
  case SEQ_DONE:
    return procSub_Done( subResult );

  case SEQ_FINISH:
    return RES_FINISH;

  case SEQ_PAUSE:
    return procSub_Pause( subResult );

  case SEQ_LINE_FEED:
    return procSub_LineFeed( subResult );

  case SEQ_CLEAR:
    return procSub_Clear( subResult );

  case SEQ_PUT:
    return procSub_Put( subResult );

  // 1フレ複数文字出力の時に改行を挟む場合、タグプロセッサが動作するのを待つ必要がある
  case SEQ_TAGPROC_FOR_NEWLINE:
    return procSub_TagProcForNewLine( subResult );

  default:
    GFL_ASSERT(0);
  }

  return RES_CONTINUE;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_DONE
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_Done( SubResult* subResult )
{
  if( m_procMode == PROC_MODE_NORMAL )
  {
    if( m_finishType != FINISH_NONE )
    {
      if( checkFinish() )
      {
        setNextSequence( SEQ_FINISH );
        GFL_PRINT_EX( STRWIN_PRINT_FLAG, "Win Print Finish!\n");
        return RES_FINISH;
      }
    }
  }
  return RES_DONE;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_PAUSE
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_Pause( SubResult* subResult )
{
  if( m_procMode != PROC_MODE_FOR_PAGE )
  {
    if( m_procMode == PROC_MODE_AT_ONCE || checkReleasePause() )
    {
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Pause Released  ");
      if( m_currentReadLine >= m_numStrLines )
      {
        GFL_PRINT_EX( STRWIN_PRINT_FLAG, " ->Done\n");
        m_currentDispLine = 0;
        m_targetPaneIdx   = 0;
        checkNextLineColor();
        RefreshLine();
        setNextSequence( SEQ_DONE );
        return RES_DONE;
      }
      else
      {
        GFL_PRINT_EX( STRWIN_PRINT_FLAG, " ->Restart\n");
        this->Restart();
      }
    }
  }
  return RES_PAUSE;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_LINE_FEED
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_LineFeed( SubResult* subResult )
{
  if( progLineFeed() )
  {
    setupReader();
    *subResult = SUB_RES_FORCE_RECALL;
    setNextSequence( SEQ_PUT );
    return RES_CONTINUE;
  }

  *subResult = SUB_RES_FORCE_QUIT;
  return RES_CONTINUE;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_CLEAR
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_Clear( SubResult* subResult )
{
  if( m_currentReadLine >= m_numStrLines )
  {
    if( this->isReadEnd() )
    {
      setNextSequence( SEQ_DONE );
      return RES_DONE;
    }
  }
  *subResult = SUB_RES_FORCE_RECALL;
  setNextSequence( SEQ_PUT );
  return RES_CONTINUE;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_PUT
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_Put( SubResult* subResult )
{
  if( m_putState == PUT_READY )
  {
    m_putState = PUT_PASSED_PROC;
  }

  if( m_bForceQuit )
  {
    setNextSequence( SEQ_FINISH );
    return RES_FINISH;
  }

  if( ++m_countFrame <= m_waitFrame )
  {
    return RES_CONTINUE;
  }

  m_countFrame = 0;

  int  length;
  const gfl2::str::STRCODE* sp = this->readNext( &length );

  // 通常文字コード読み込み
  if( !this->getLatestTagCode(&m_prevTag) )
  {
    m_paneWork[m_targetPaneIdx].pPane->SetString( sp, 0, length );

    if( this->isReadEnd() )
    {
      *subResult = SUB_RES_FORCE_QUIT;
      setNextSequence( SEQ_TAGPROC_FOR_NEWLINE );
    }
    return RES_CONTINUE;
  }

  // TAG開始コード読み込み
  GFL_PRINT_EX(
    STRWIN_PRINT_FLAG,
    "[STRWIN] PrevTagCode = %04x ... LF=%04x,PC=%04x\n",
    m_prevTag.code, TAGCODE_LINEFEED, TAGCODE_PAGECLEAR );

  if( (m_prevTag.code==TAGCODE_LINEFEED) || (m_prevTag.code==TAGCODE_PAGECLEAR) )
  {
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Check Pause Tag..%04x, nextLine=%d\n", m_prevTag.code, m_currentReadLine);
    ++m_currentReadLine;
    ++m_currentDispLine;
    *subResult = SUB_RES_FORCE_QUIT;
    setNextSequence( SEQ_PAUSE );
    return RES_CONTINUE;
  }

  m_paneWork[m_targetPaneIdx].pPane->SetString( sp, 0, length );
  m_bTagProcBreak = CheckTagProcBreak( m_prevTag );

  if( this->isReadEnd() )
  {
    *subResult = SUB_RES_FORCE_QUIT;
    setNextSequence( SEQ_TAGPROC_FOR_NEWLINE );
    return RES_CONTINUE;
  }
  else
  {
    if( m_bTagProcBreak )
    {
      *subResult = SUB_RES_FORCE_QUIT;
      return RES_CONTINUE;
    }
    *subResult = SUB_RES_FORCE_RECALL;
    return RES_CONTINUE;
  }
}

/**
 * メインシーケンスを変更
 * @param nextSeq
 */
void StrWin::setNextSequence( Sequence nextSeq )
{
  m_seq = nextSeq;
  if( m_seq == SEQ_PUT )
  {
    m_putState = PUT_READY;
  }
  else
  {
    m_putState = PUT_PASSED_PROC;
  }
}
/**
 * SEQ_PUT処理開始後、最初の1frameを終了したかどうか
 * @return
 */
bool StrWin::isPassed1stFrame( void ) const
{
  return ( m_putState == PUT_PASSED_FRAME );
}

/**
 * 改行待ちのユーザー入力があったかどうかチェック
 */
bool StrWin::checkReleasePause( void )
{
  if( !isPassed1stFrame() )
  {
    return false;
  }
  return IsPauseRelease();
}



// 書き出すべき文字列先頭ポインタと文字列サイズを取得
const gfl2::str::STRCODE* StrWin::readNext( int* strLen )
{
  const gfl2::str::STRCODE* sp_start = gfl2::str::GetLinePtr( m_pStrBuf->GetPtr(), m_readLineIndex );
  const gfl2::str::STRCODE* sp = sp_start;
  int length = 0;

  if( sp )
  {
    sp += m_readIndex;

    if( *sp != gfl2::str::TAG_START_CODE )
    {
      m_bReadTag = false;
      if( (*sp != gfl2::str::EOM_CODE) && (*sp != gfl2::str::CR_CODE) ){
        ++sp;
      }
    }
    else
    {
      m_bReadTag = true;
      sp = readTag( sp, &m_readTag );
    }

    length = ((uptr)sp - (uptr)sp_start) / sizeof(gfl2::str::STRCODE);

#if GFL_DEBUG_PRINT
		{
      const gfl2::str::STRCODE* sp_head = m_pStrBuf->GetPtr();
      GFL_PRINT_EX(
        STRWIN_PRINT_FLAG,
        "[STRWIN] sp_head=%p(%04x), sp_start=%p(%04x), code=%04x, readIndex=%d, readLength=%d, lineNo=%d\n",
        sp_head, *sp_head, sp_start, *sp_start, *sp, m_readIndex, length, m_readLineIndex );
    }
#endif
    switch( *sp ){
    case gfl2::str::EOM_CODE:
      m_bReadLineDone = true;
      break;

    case gfl2::str::CR_CODE:
      ++m_readLineIndex;
      m_readIndex = 0;
      m_bReadLineDone = true;
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] LineFeed, nextLineIndex=%d\n", m_readLineIndex );
      break;

    default:
      m_readIndex = length;
      m_bReadLineDone = false;
      break;
    }
  }

  *strLen = length;
  return sp_start;
}

/**
 *  タグ開始コードを指す文字列ポインタから、タグ情報を読み込む
 *
 *  @param[in]    sp      読み込み元文字列のポインタ
 *  @param[out]   dst     読み込みパラメータ格納先
 *
 *  @return    const gfl::str::STRCODE*   読み込んだタグ情報を飛ばした先のポインタ
 */
const gfl2::str::STRCODE* StrWin::readTag( const gfl2::str::STRCODE* sp, TagInfo* dst )
{
  dst->code = gfl2::str::GetTagCode( sp );
  dst->paramCount = gfl2::str::GetTagParamCount( sp );
  if( dst->paramCount > GFL_NELEMS(dst->params) )
  {
    GFL_ASSERT_MSG( 0, "too much tag params (%d) : code=%04x\n", dst->paramCount, dst->code );
    dst->paramCount = GFL_NELEMS(dst->params);
  }
  for(u32 i=0; i<dst->paramCount; ++i)
  {
    dst->params[i] = gfl2::str::GetTagParam( sp, i );
  }

  return gfl2::str::SkipTagCode( sp );
}

// 最後に読み込んだタグコード取得
bool StrWin::getLatestTagCode( TagInfo* dst ) const
{
  if( m_bReadTag )
  {
    *dst = m_readTag;
    return true;
  }
  return false;
}

//-------------------------------------------------
/**
 *  サブ処理：SEQ_TAGPROC_FOR_NEWLINE
 */
//-------------------------------------------------
StrWin::Result StrWin::procSub_TagProcForNewLine( SubResult* subResult )
{
  Sequence nextSeq = proc_checkNextLine();
  setNextSequence( nextSeq );
  switch( nextSeq ){
  case SEQ_DONE:
    return RES_DONE;
  case SEQ_PAUSE:
    return RES_PAUSE;
  case SEQ_PUT:
    return RES_CONTINUE;
  default:
    return RES_CONTINUE;
  }
}
// １行分文字列読み取り終わり後の処理
StrWin::Sequence StrWin::proc_checkNextLine( void )
{
  ++m_currentReadLine;
  ++m_currentDispLine;
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Reader End %d/%d \n", m_currentReadLine, m_numStrLines );
  if( m_currentReadLine >= m_numStrLines )
  {
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "Win Print Done A\n");//
    return SEQ_DONE;
  }
  else if( m_currentDispLine >= m_numDispLines )
  {
    m_prevTag.Initialize( TAGCODE_LINEFEED );
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "Start LineFeed\n");
    return SEQ_PAUSE;
  }
  else
  {
    setupReader();
    ++m_targetPaneIdx;
    checkNextLineColor();
    RefreshLine();
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "Start Next Line : pane=%d\n", m_targetPaneIdx );
    return SEQ_PUT;
  }
}


//--------------------------------------------------------------------------------
/**
 *  TagProccessorによって描画色の変更があるかどうかを判定、あればPaneに反映させる
 */
//--------------------------------------------------------------------------------
void StrWin::checkNextLineColor( void )
{
  nw::ut::Color8 topColor, bottomColor;
  if( IsColorChanged(&topColor, &bottomColor) )
  {
    m_paneWork[m_targetPaneIdx].pPane->SetTextColor( topColor, bottomColor );
  }
}

//=============================================================================================
/**
 * @brief      ポーズ解除して次の行を表示する
 */
//=============================================================================================
void StrWin::Restart( void )
{
  return this->Restart( false );
}

//-----------------------------------------------------------------
/**
 * ポーズ解除後、ウィンドウ内容を全クリアする必要があるか判定
 *
 * @return  全クリアの必要ありならtrue / １行送りなら false
 */
//-----------------------------------------------------------------
bool StrWin::isPauseTypeClear( void ) const
{
  if( (m_prevTag.code == TAGCODE_PAGECLEAR) ||
      (m_currentDispLine < m_numDispLines) )  // 最下行まで表示していない場合FEEDさせるとおかしな挙動になるのでCLEAR扱い
  {
    return true;
  }
  return false;
}

//=============================================================================================
/**
 * @brief      ポーズ解除して次の行を表示する
 *
 * @param[in]  bClear   trueなら 次行の表示前にウィンドウ内容を全クリアする
 */
//=============================================================================================
void StrWin::Restart( bool bClear )
{
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Call Restart\n");

  if( m_seq == SEQ_PAUSE )
  {
    if( bClear || isPauseTypeClear() )
    {
      for(u32 i=0; i<m_numDispLines; ++i)
      {
        m_paneWork[i].mtx = m_paneWork[i].initialMtx;
        m_paneWork[i].myPosIdx = i;
        m_paneWork[i].pPane->SetString( L"" );
        m_paneWork[i].pPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(m_paneWork[i].initialMtx) );
      }
      m_currentDispLine = 0;
      m_targetPaneIdx   = 0;
      checkNextLineColor();
      RefreshLine();

      setupReader();
      setNextSequence( SEQ_CLEAR );
    }
    else
    {
      startLineFeed();
      setNextSequence( SEQ_LINE_FEED );
    }
  }
  else
  {
    GFL_ASSERT(0);  // ポーズ中以外に呼び出された
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief      Windowのクリア
 */
//-----------------------------------------------------------------------------------------
void StrWin::Clear( void )
{
  for(u32 i=0; i<m_numDispLines; ++i)
  {
    m_paneWork[i].mtx = m_paneWork[ i ].initialMtx;
    m_paneWork[i].myPosIdx = i;
    m_paneWork[i].pPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(m_paneWork[i].initialMtx) );
    m_paneWork[i].pPane->SetString( GFLSTR("") );
  }
  m_currentDispLine = 0;
  m_targetPaneIdx   = 0;
  checkNextLineColor();
  RefreshLine();
}

//-----------------------------------------------------------------------------------------
/**
 * @brief      フィニッシュ待ちモード設定
 *
 * @param[in]  type     フィニッシュ待ちモード
 * @param[in]  frames   待ちフレーム数（type == FINISH_TIMER || FINISH_HYB の場合のみ有効）
 */
//-----------------------------------------------------------------------------------------
void StrWin::SetFinishMode( FinishType type, u8 frames )
{
  if( type < NUM_FINISH_TYPES )
  {
    m_finishType = type;
    m_finishFrame = frames;
    m_finishFrameCount = 0;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief      表示強制終了し、Proc() が できるだけ早く RES_FINISH を返すようになる
 */
//---------------------------------------------------------------------------------
void StrWin::ForceQuit( void )
{
  m_bForceQuit = true;
}

//----------------------------------------------------------------------
/**
 * @brief      メッセージ処理速度変更
 *             描画途中の変更はサポート外です。
 *
 * @param[in]   waitFrames    １文字出力ごとのwaitフレーム値
 * @param[in]   putCharCount  １度に出力する文字数
 */
//----------------------------------------------------------------------
void StrWin::SetMessageSpeed( u32 waitFrames, u32 putCharCount )
{
  m_waitFrameOrg = m_waitFrame = waitFrames;
  m_putCharCountPerFrameOrg = m_putCharCountPerFrame = putCharCount;
}

//----------------------------------------------------------------------
/**
 * @brief      メッセージ処理速度を一時的に変更（ボタン押し中のみ速度アップなどに利用）
 *
 * @param[in]   waitFrames    １文字出力ごとのwaitフレーム値
 * @param[in]   putCharCount  １度に出力する文字数
 */
//----------------------------------------------------------------------
void StrWin::SetMessageSpeedTemporary( u32 waitFrames , u32 putCharCount )
{
  m_waitFrame = waitFrames;
  m_putCharCountPerFrame = putCharCount;
}

//----------------------------------------------------------------------
/**
 * @brief      メッセージ処理速度パラメータを取得
 * @param[out]  waitFrames    １文字出力ごとの wait フレーム数
 * @param[out]  putCharCount  １度に出力する文字数
 */
//----------------------------------------------------------------------
void StrWin::GetMessageSpeedParams( u32* waitFrames, u32* putCharCount ) const
{
  *waitFrames   = m_waitFrame;
  *putCharCount = m_putCharCountPerFrame;
}


//-----------------------------------------------------------------------
/**
 * @brief   改行時、最上行のフェードアウトに要するフレーム数を設定する
 *        （何もしなければ DEFAULT_FADE_FRAMES が設定されています）
 *
 * @param[in]   frames    フレーム数
 */
//-----------------------------------------------------------------------
void StrWin::SetLineFadeOutFrames( u16 frames )
{
  m_lineFadeFrames = frames;
}

//-----------------------------------------------------------------------
/**
 * @brief   改行時、各行のスクロールに要するフレーム数を設定する
 *        （何もしなければ DEFAULT_SCROLL_FRAMES が設定されています）
 *
 * @param[in]   frames    フレーム数
 */
//-----------------------------------------------------------------------
void StrWin::SetLineScrollFrames( u16 frames )
{
  m_lineScrollFrames = frames;
}

//-----------------------------------------------------------------------
/**
 * ポーズ解除後の改ページタイプ判定
 * @return  改ページタイプ
 */
//-----------------------------------------------------------------------
StrWin::PauseType StrWin::GetPauseType( void ) const
{
  if( m_seq != SEQ_PAUSE )
  {
    return PAUSETYPE_NONE;
  }
  if( m_procMode == PROC_MODE_AT_ONCE )
  {
    return PAUSETYPE_NONE;
  }
  if( isPauseTypeClear() )
  {
    return PAUSETYPE_CLEAR;
  }
  return PAUSETYPE_FEED;
}


//-----------------------------------------------------------------------
/**
 * @brief  テキストボックスペインのアルファをリセット(不透明に)する
 * @attention  バグ修正のために用意した関数なので使用禁止。
 */
//-----------------------------------------------------------------------
void StrWin::ResetTextBoxPaneAlpha(u8 index)
{
  // @fix NMCat[3868] テキストの一部が表示されなくなることがある
  //      →外部からアルファを元に戻せるようにする。
  //        [改善案] テキストボックスペインの操作を外部からいっさいできないようにして(この関数は削除して)、本クラスのClear関数内でアルファを元に戻すほうがよい。

  // 本クラスのClear関数が呼ばれるときの条件と合わせてある。

  GFL_ASSERT( m_bPaneSetup );
  if( m_bPaneSetup )
  {
    if( m_paneWork )
    {
      if( index < m_numDispLines )
      {
        m_paneWork[index].pPane->SetAlpha(255);
      }
    }
  }
}


//--------------------------------------------------------------------------------------
/**
 *  書き出す行の文字列を m_reader に設定する
 *  （m_pStrBuf, m_currentReadLine を参照）
 */
//--------------------------------------------------------------------------------------
void StrWin::setupReader( void )
{
  const STRCODE* sp = m_pStrBuf->GetPtr();
  sp = GetLinePtr( sp, m_currentReadLine );
  if( sp )
  {
#if GFL_DEBUG_PRINT
    u32 line_len = LineLen( sp, m_currentReadLine );
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Set Reader lineNo=%d,.. len=%d 1stCode=%04x \n", m_currentReadLine, line_len, *sp );
#endif
//    PrintCode( sp );
//    m_reader.SetString( sp, line_len, m_pHeap );
  }
  else{
    if( this->isReadEnd() )
    {
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] setupReader Fail, End\n");
    }
    else
    {
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] setupReader Fail, not END \n");
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 * 設定された文字列を読み終わったかどうか判定
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool StrWin::isReadEnd( void ) const
{
  return m_bReadLineDone;
}


//--------------------------------------------------------------------------------------
/**
 *  改行処理を開始
 */
//--------------------------------------------------------------------------------------
void StrWin::startLineFeed( void )
{
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Start Line Feed : currentDispLine=%d\n", m_currentDispLine );
  ++m_targetPaneIdx;
  if( m_targetPaneIdx >= m_numDispLines )
  {
    m_targetPaneIdx = 0;
  }
  checkNextLineColor();
  RefreshLine();

  m_lineFeedSeq = 0;
}

//--------------------------------------------------------------------------------------
/**
 * 改行処理を更新
 *
 * @retval  bool    改行処理が終了していたら true
 */
//--------------------------------------------------------------------------------------
bool StrWin::progLineFeed( void )
{
  PANE_WORK* work = &m_paneWork[m_targetPaneIdx];

  switch( m_lineFeedSeq ){
  case 0:   // 先頭行のフェードアウトを開始
    work->fadeVal.StartLinear( 255, 0, m_lineFadeFrames );
    ++m_lineFeedSeq;

    /* fallthru */

  case 1:   // 先頭行フェード進行
    {
      f32 alpha = work->fadeVal.Progress();
      work->pPane->SetAlpha( static_cast<u8>(alpha) );
      if( work->fadeVal.IsEnd() )
      {
        ++m_lineFeedSeq;
      }
    }
    break;

  case 2:   // 各行スクロール開始
    for(u32 i=0; i<m_numDispLines; ++i)
    {
      // 先頭以外の行はスクロール処理
      if( m_paneWork[i].myPosIdx )
      {
        m_paneWork[i].myPosIdx--;
        m_paneWork[i].movePos.StartLinear( m_paneWork[i].mtx, m_paneWork[m_paneWork[i].myPosIdx].initialMtx, m_lineScrollFrames );
      }
      // 先頭行は空文字設定し、次の配置行へ
      else
      {
        m_hidePaneIdx = i;
        m_paneWork[i].myPosIdx = m_numDispLines - 1;
        m_paneWork[i].mtx = m_paneWork[m_paneWork[i].myPosIdx].initialMtx;
        m_paneWork[i].pPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(m_paneWork[i].mtx) );
        m_paneWork[i].pPane->SetString( L"" );
      }
    }
    ++m_lineFeedSeq;

    /* fallthru */

  case 3: // 各行スクロール進行
    {
      u32 end_count = 0;
      for(u32 i=0; i<m_numDispLines; ++i)
      {
        if( i != m_hidePaneIdx )
        {
          m_paneWork[i].mtx = m_paneWork[i].movePos.Progress();
          m_paneWork[i].pPane->SetTranslate( gfl2::math::ConvertGfVec3ToNwVec3(m_paneWork[i].mtx) );
          if( m_paneWork[i].movePos.IsEnd() )
          {
            ++end_count;
          }
        }
      }

      u32 btm_index = m_numDispLines - 1;
      if( end_count >= btm_index )
      {
        m_paneWork[m_hidePaneIdx].pPane->SetAlpha( 255 );
        m_lineFeedSeq++;
        return true;
      }
    }
    break;

  default:
    return true;

  }
  return false;
}

//--------------------------------------------------------------------------------------
/**
 * フィニッシュ待ち判定
 *
 * @retval  bool    フィニッシュしていれば true
 */
//--------------------------------------------------------------------------------------
bool StrWin::checkFinish( void )
{
  switch( m_finishType ){
  case FINISH_NONE:
    return true;

  case FINISH_TIMER:
    if( checkFinishFrame() )
    {
      return true;
    }
    return false;

  case FINISH_USER:
    if( checkFinishInput() )
    {
      return true;
    }
    return false;

  case FINISH_HYB:
    if( checkFinishInput() )
    {
      return true;
    }
    if( checkFinishFrame() )
    {
      return true;
    }
    return false;

  default:
    GFL_ASSERT_STOP(0);
    return true;
  }
}
//--------------------------------------------------------------
/**
 *  フィニッシュ待ち判定 - ユーザ入力
 */
//--------------------------------------------------------------
bool StrWin::checkFinishInput( void )
{
  if( !isPassed1stFrame() )
  {
    return false;
  }
  return IsFinishInput();
}
//--------------------------------------------------------------
/**
 *  フィニッシュ待ち判定 - フレームカウント
 */
//--------------------------------------------------------------
bool StrWin::checkFinishFrame( void )
{
  if( m_finishFrameCount > m_finishFrame )
  {
    return true;
  }

  if( ++m_finishFrameCount > m_finishFrame )
  {
      return true;
  }

  return false;
}

void FadeF32::StartLinear( f32 start, f32 end, u32 steps )
{
  m_stepMax = steps + 1;
  m_value = start;
  m_endValue = end;
  m_stepValue = (end - start) / (static_cast<f32>(m_stepMax));

  m_step = 0;
  m_bEnd = false;
}

f32 FadeF32::Progress( void )
{
  if( m_step < m_stepMax )
  {
    if( ++m_step < m_stepMax )
    {
      m_value += m_stepValue;
      return m_value;
    }
  }
  m_bEnd = true;
  return m_endValue;
}

void FadeVEC3::StartLinear( const gfl2::math::Vector3& start, const gfl2::math::Vector3& end, u32 steps )
{
  m_stepMax  = steps + 1;
  m_value    = start;
  m_endValue = end;
  m_stepValue.x = (end.x - start.x) / (static_cast<f32>(m_stepMax));
  m_stepValue.y = (end.y - start.y) / (static_cast<f32>(m_stepMax));
  m_stepValue.z = (end.z - start.z) / (static_cast<f32>(m_stepMax));

  m_step = 0;
  m_bEnd = false;
}

const gfl2::math::Vector3& FadeVEC3::Progress( void )
{
  if( m_step < m_stepMax )
  {
    if( ++m_step < m_stepMax )
    {
      m_value += m_stepValue;
      return m_value;
    }
  }
  m_bEnd = true;
  return m_endValue;
}


GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

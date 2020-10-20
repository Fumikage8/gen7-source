// ����ŐV��

//===========================================================================
/**
 * @file    gfl_StrWin.cpp
 * @brief   GAMEFREAK Library  String Print System ( �����\���E�B���h�E )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#include <gfl_Common.h>

#include <str/gfl_StrSys.h>
#include <str/gfl_StrWin.h>



namespace gfl {
namespace str {



//=============================================================================================
/**
 * �R���X�g���N�^
 *
 * @param[in]   lytWk       �\���Ɏg�p���郌�C�A�E�g���[�N�|�C���^
 * @param[in]   numLines    �\���s��
 * @param[in]   waitFrames  �P�����o�͂��Ƃ�wait�t���[���l
 * @param[in]   pHeap       �q�[�v�n���h��
 * @param[in]   pTagProc    �^�O�v���Z�b�T�i�s�v�Ȃ�NULL�ł��j
 * @param[in]   pTagProc    �^�O�v���Z�b�T�i�s�v�Ȃ�NULL�ł��j
 */
//=============================================================================================
StrWin::StrWin( gfl::grp::g2d::LytWk* lytWk, u16 numLines, u32 waitFrames, gfl::heap::HeapBase* pHeap, TagProcBase* pTagProc, u32 putCharCount )
 :  m_paneWork(NULL), m_pStrBuf(NULL),
     m_pTagProc(pTagProc), m_pHeap(pHeap), m_pLytWk(lytWk),
    m_finishType(FINISH_NONE),
    m_lineFadeFrames(DEFAULT_FADE_FRAMES), m_lineScrollFrames(DEFAULT_SCROLL_FRAMES),
    m_numDispLines(numLines), m_numStrLines(0), m_numPanes(0), m_currentReadLine(0), m_currentDispLine(0),
    m_waitFrame(waitFrames), m_countFrame(0), m_targetPaneIdx(0), m_hidePaneIdx(0), m_lineFeedSeq(0),
    m_finishFrame(0), m_finishFrameCount(0), m_putCharCountPerFrame(putCharCount), m_seq(SEQ_DONE),
    m_readIndex(0), m_readLineIndex(0), m_strLength(0), m_bReadLineDone(false), m_bReadTag(false),
    m_bForceQuit(false), m_procMode(PROC_MODE_NORMAL),
    m_bAvirable(false), m_bPaneSetup(false)
{
//  debug::SetPrintFlag( STRWIN_PRINT_FLAG );

  if( m_numDispLines>0 && m_putCharCountPerFrame>0 )
  {
    m_paneWork = reinterpret_cast<PANE_WORK*>GflHeapAllocMemory( pHeap, sizeof(PANE_WORK)*numLines );
    if( m_paneWork )
    {
      for( u32 i=0; i<numLines; ++i )
      {
        m_paneWork[ i ].pPane = NULL;
      }

      m_waitFrameOrg = m_waitFrame;
      m_putCharCountPerFrameOrg = m_putCharCountPerFrame;
      m_bAvirable = true;
      return;
    }
  }
  else
  {
    GFL_ASSERT(0);  // numLines > 0 �ȕK�v������
  }
}
//=============================================================================================
/**
 * �f�X�g���N�^
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
 * @brief �����\���Ɏg���y�C����Index��ǉ��B�\���s���Ɠ��������ǉ�����K�v������B
 *
 * @param[in]   paneIdx   �y�C��Index
 */
//=============================================================================================
void
StrWin::AddLinePaneIdx( u32 paneIdx )
{
  GFL_ASSERT( m_bAvirable );

  AddLinePaneIdxWithStrMinLen( paneIdx, LINE_BUFFSER_SIZE_MIN );
}
//=============================================================================================
/**
 * @brief �����\���Ɏg���y�C����Index��ǉ��B�\���s���Ɠ��������ǉ�����K�v������B
 *
 * @param[in]   paneIdx   �y�C��Index
 */
//=============================================================================================
void
StrWin::AddLinePaneIdxWithStrMinLen( u32 paneIdx, u32 strMinLen )
{
  GFL_ASSERT( m_bAvirable );

  if( m_bAvirable )
  {
    if( m_numPanes < m_numDispLines )
    {
      m_paneWork[ m_numPanes ].pPane = m_pLytWk->GetTextBoxPane( paneIdx );
      if( m_paneWork[ m_numPanes ].pPane != NULL )
      {
        if( m_paneWork[ m_numPanes ].pPane->GetStringBufferLength() < strMinLen )
        {
          m_pLytWk->AllocTextBoxPaneStringBuffer( paneIdx, strMinLen );
        }

        if( m_pTagProc != NULL )
        {
          m_paneWork[ m_numPanes ].pPane->SetTagProcessor( m_pTagProc );
        }

        m_paneWork[ m_numPanes ].initialMtx = m_paneWork[ m_numPanes ].pPane->GetTranslate();
        m_paneWork[ m_numPanes ].mtx = m_paneWork[ m_numPanes ].initialMtx;
        m_paneWork[ m_numPanes ].myPosIdx = m_numPanes;
        m_paneWork[ m_numPanes ].timer = 0;
        if( ++m_numPanes == m_numDispLines )
        {
          m_bPaneSetup = true;
        }
      }
      else
      {
        GFL_ASSERT_MSG(0, "Illegal PaneIdx:%d", paneIdx);  // �������y�C��Index���n����Ă��Ȃ�
      }
    }
    else
    {
      GFL_ASSERT(0);  // ����ȏ�y�C����o�^�ł��Ȃ�
    }
  }

}

//=============================================================================================
/**
 *  @brief  �\�����镶�����ݒ肷��
 *
 * @param[in]   str   �\�����镶����
 *                  �i���̊֐����Ă񂾌�̓N���X�����œ��e��ێ�����̂ŁA
 *                    ���̕�����͔j�����Ă��\���܂���j
 */
//=============================================================================================
void
StrWin::SetString( const StrBuf& str )
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

    if( (m_pStrBuf != NULL)
    &&  (m_pStrBuf->GetSize() < buf_size)
    ){
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
    else{
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
 * @brief    �\�����Ō�܂ōs��
 */
//=============================================================================================
void
StrWin::ProcAtOnce( void )
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
 * @brief  �P�y�[�W���̕\���������s��
 */
//=============================================================================================
StrWin::Result
StrWin::ProcForPage( void )
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
 * @brief  �\�����C�������i�P��Ăяo�����Ƃɏ������X�V���܂��j
 *
 * @retval   StrWin::Result   ���ʃR�[�h�igfl_StrWin.h �Q�Ɓj
 */
//=============================================================================================
StrWin::Result
StrWin::Proc( void )
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
      if( ++proc_count >= m_putCharCountPerFrame ){
        bDone = true;
        break;
      }
      break;
    }

  }while(!bDone);

  if( m_putState == PUT_PASSED_PROC ){
    m_putState = PUT_PASSED_FRAME;
  }

  m_waitFrame = m_waitFrameOrg;
  m_putCharCountPerFrame = m_putCharCountPerFrameOrg;

  return result;
}
//-----------------------------------------------------------------------------
/**
 *  Proc �� ���C����������
 */
//-----------------------------------------------------------------------------
StrWin::Result
StrWin::procCore( SubResult* subResult )
{
  *subResult = SUB_RES_DEFAULT;

  switch( m_seq ){
  case   SEQ_DONE:
    return procSub_Done( subResult );

  case SEQ_FINISH:
    return RES_FINISH;

  case   SEQ_PAUSE:
    return procSub_Pause( subResult );

  case   SEQ_LINE_FEED:
    return procSub_LineFeed( subResult );

  case   SEQ_CLEAR:
    return procSub_Clear( subResult );

  case   SEQ_PUT:
    return procSub_Put( subResult );

  // 1�t�����������o�͂̎��ɉ��s�����ޏꍇ�A�^�O�v���Z�b�T�����삷��̂�҂K�v������
  case SEQ_TAGPROC_FOR_NEWLINE:
    return procSub_TagProcForNewLine( subResult );

  default:
    GFL_ASSERT(0);
  }

  return RES_CONTINUE;
}
//-------------------------------------------------
/**
 *  �T�u�����FSEQ_DONE
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_Done( SubResult* subResult )
{
  if( m_procMode == PROC_MODE_NORMAL )
  {
    if( m_finishType != FINISH_NONE )
    {
      if( checkFinish() ){
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
 *  �T�u�����FSEQ_PAUSE
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_Pause( SubResult* subResult )
{
  if( m_procMode != PROC_MODE_FOR_PAGE )
  {
    if( m_procMode == PROC_MODE_AT_ONCE || checkReleasePause() )
    {
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Pause Released  ");
      if( m_currentReadLine >= m_numStrLines ){
        GFL_PRINT_EX( STRWIN_PRINT_FLAG, " ->Done\n");
        m_currentDispLine = 0;
        m_targetPaneIdx   = 0;
        checkNextLineColor();
        RefreshLine();
        setNextSequence( SEQ_DONE );
        return RES_DONE;
      }
      else{
        GFL_PRINT_EX( STRWIN_PRINT_FLAG, " ->Restart\n");
        this->Restart();
      }
    }
  }
  return RES_PAUSE;
}
//-------------------------------------------------
/**
 *  �T�u�����FSEQ_LINE_FEED
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_LineFeed( SubResult* subResult )
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
 *  �T�u�����FSEQ_CLEAR
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_Clear( SubResult* subResult )
{
  if( m_currentReadLine >= m_numStrLines )
  {
    if( this->isReadEnd() ){
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
 *  �T�u�����FSEQ_PUT
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_Put( SubResult* subResult )
{
  if( m_putState == PUT_READY ){
    m_putState = PUT_PASSED_PROC;
  }

  if( m_bForceQuit ){
    setNextSequence( SEQ_FINISH );
    return RES_FINISH;
  }

  if( ++m_countFrame <= m_waitFrame )
  {
    return RES_CONTINUE;
  }

  m_countFrame = 0;

  int  length;
  const gfl::str::STRCODE* sp = this->readNext( &length );

  // �ʏ핶���R�[�h�ǂݍ���
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

  // TAG�J�n�R�[�h�ǂݍ���
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] PrevTagCode = %04x ... LF=%04x,PC=%04x\n",
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
    if( m_bTagProcBreak ){
      *subResult = SUB_RES_FORCE_QUIT;
      return RES_CONTINUE;
    }
    *subResult = SUB_RES_FORCE_RECALL;
    return RES_CONTINUE;
  }
}

/**
 * ���C���V�[�P���X��ύX
 * @param nextSeq
 */
void StrWin::setNextSequence( Sequence nextSeq )
{
  m_seq = nextSeq;
  if( m_seq == SEQ_PUT ){
    m_putState = PUT_READY;
  }else{
    m_putState = PUT_PASSED_PROC;
  }
}
/**
 * SEQ_PUT�����J�n��A�ŏ���1frame���I���������ǂ���
 * @return
 */
bool StrWin::isPassed1stFrame( void ) const
{
  return m_putState == PUT_PASSED_FRAME;
}

/**
 * ���s�҂��̃��[�U�[���͂����������ǂ����`�F�b�N
 */
bool  StrWin::checkReleasePause( void )
{
  if( !isPassed1stFrame() ){
    return false;
  }
  return IsPauseRelease();
}



// �����o���ׂ�������擪�|�C���^�ƕ�����T�C�Y���擾
const gfl::str::STRCODE* StrWin::readNext( int* strLen )
{
  const gfl::str::STRCODE* sp_start = gfl::str::GetLinePtr( m_pStrBuf->GetPtr(), m_readLineIndex );
  const gfl::str::STRCODE* sp = sp_start;
  int   length = 0;

  if( sp )
  {
    sp += m_readIndex;

    if( *sp != gfl::str::TAG_START_CODE )
    {
      m_bReadTag = false;
      if( (*sp != gfl::str::EOM_CODE) && (*sp != gfl::str::CR_CODE) ){
        ++sp;
      }
    }
    else
    {
      m_bReadTag = true;
      sp = readTag( sp, &m_readTag );
    }

    length = ((uptr)sp - (uptr)sp_start) / sizeof(gfl::str::STRCODE);

    {
      const gfl::str::STRCODE* sp_head = m_pStrBuf->GetPtr();
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] sp_head=%p(%04x), sp_start=%p(%04x), code=%04x, readIndex=%d, readLength=%d, lineNo=%d\n",
            sp_head, *sp_head, sp_start, *sp_start, *sp, m_readIndex, length, m_readLineIndex );
    }

    switch( *sp ){
    case gfl::str::EOM_CODE:
      m_bReadLineDone = true;
      break;

    case gfl::str::CR_CODE:
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
 *  �^�O�J�n�R�[�h���w��������|�C���^����A�^�O����ǂݍ���
 *
 *  @param[in]    sp      �ǂݍ��݌�������̃|�C���^
 *  @param[out]   dst     �ǂݍ��݃p�����[�^�i�[��
 *
 *  @return    const gfl::str::STRCODE*   �ǂݍ��񂾃^�O�����΂�����̃|�C���^
 */
const gfl::str::STRCODE* StrWin::readTag( const gfl::str::STRCODE* sp, TagInfo* dst )
{
  dst->code = gfl::str::GetTagCode( sp );
  dst->paramCount = gfl::str::GetTagParamCount( sp );
  if( dst->paramCount > GFL_NELEMS(dst->params) ){
    GFL_ASSERT_MSG( 0, "too much tag params (%d) : code=%04x\n", dst->paramCount, dst->code );
    dst->paramCount = GFL_NELEMS(dst->params);
  }
  for(u32 i=0; i<dst->paramCount; ++i)
  {
    dst->params[i] = gfl::str::GetTagParam( sp, i );
  }

  return gfl::str::SkipTagCode( sp );
}

// �Ō�ɓǂݍ��񂾃^�O�R�[�h�擾
bool StrWin::getLatestTagCode( TagInfo* dst ) const
{
  if( m_bReadTag ){
    *dst = m_readTag;
    return true;
  }
  return false;
}

//-------------------------------------------------
/**
 *  �T�u�����FSEQ_TAGPROC_FOR_NEWLINE
 */
//-------------------------------------------------
StrWin::Result
StrWin::procSub_TagProcForNewLine( SubResult* subResult )
{
  Sequence nextSeq = proc_checkNextLine();
  setNextSequence( nextSeq );
  switch( nextSeq ){
  case SEQ_DONE:  return RES_DONE;
  case SEQ_PAUSE: return RES_PAUSE;
  case SEQ_PUT:   return RES_CONTINUE;
  default:
    return RES_CONTINUE;
  }
}
// �P�s��������ǂݎ��I����̏���
StrWin::Sequence
StrWin::proc_checkNextLine( void )
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
 *  TagProccessor�ɂ���ĕ`��F�̕ύX�����邩�ǂ����𔻒�A�����Pane�ɔ��f������
 */
//--------------------------------------------------------------------------------
void
StrWin::checkNextLineColor( void )
{
  nw::ut::Color8 topColor, bottomColor;
  if( IsColorChanged(&topColor, &bottomColor) )
  {
    m_paneWork[m_targetPaneIdx].pPane->SetTextColor( topColor, bottomColor );
  }
}
//=============================================================================================
/**
 * @brief      �|�[�Y�������Ď��̍s��\������
 */
//=============================================================================================
void
StrWin::Restart( void )
{
  return this->Restart( false );
}
//-----------------------------------------------------------------
/**
 * �|�[�Y������A�E�B���h�E���e��S�N���A����K�v�����邩����
 *
 * @return  �S�N���A�̕K�v����Ȃ�true / �P�s����Ȃ� false
 */
//-----------------------------------------------------------------
bool StrWin::isPauseTypeClear( void ) const
{
  if( (m_prevTag.code == TAGCODE_PAGECLEAR)
  ||  (m_currentDispLine < m_numDispLines)  // �ŉ��s�܂ŕ\�����Ă��Ȃ��ꍇFEED������Ƃ������ȋ����ɂȂ�̂�CLEAR����
  ){
    return true;
  }
  return false;
}

//=============================================================================================
/**
 * @brief      �|�[�Y�������Ď��̍s��\������
 *
 * @param[in]  bClear   true�Ȃ� ���s�̕\���O�ɃE�B���h�E���e��S�N���A����
 */
//=============================================================================================
void
StrWin::Restart( bool bClear )
{
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Call Restart\n");

  if( m_seq == SEQ_PAUSE )
  {
    if( bClear
    ||  isPauseTypeClear()
    ){
      for(u32 i=0; i<m_numDispLines; ++i)
      {
        m_paneWork[ i ].mtx = m_paneWork[ i ].initialMtx;
        m_paneWork[ i ].myPosIdx = i;
        m_paneWork[ i ].pPane->SetString( L"" );
        m_paneWork[ i ].pPane->SetTranslate( m_paneWork[ i ].initialMtx );
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
    GFL_ASSERT(0);  // �|�[�Y���ȊO�ɌĂяo���ꂽ
  }
}

//-----------------------------------------------------------------------------------------
/**
 * @brief      Window�̃N���A
 */
//-----------------------------------------------------------------------------------------
void  StrWin::Clear( void )
{
  for(u32 i=0; i<m_numDispLines; ++i)
  {
    m_paneWork[ i ].mtx = m_paneWork[ i ].initialMtx;
    m_paneWork[ i ].myPosIdx = i;
    m_paneWork[ i ].pPane->SetTranslate( m_paneWork[ i ].initialMtx );
    m_paneWork[ i ].pPane->SetString( GFLSTR("") );
  }
  m_currentDispLine = 0;
  m_targetPaneIdx   = 0;
  checkNextLineColor();
  RefreshLine();
}

//-----------------------------------------------------------------------------------------
/**
 * @brief      �t�B�j�b�V���҂����[�h�ݒ�
 *
 * @param[in]  type     �t�B�j�b�V���҂����[�h
 * @param[in]  frames   �҂��t���[�����itype == FINISH_TIMER || FINISH_HYB �̏ꍇ�̂ݗL���j
 */
//-----------------------------------------------------------------------------------------
void  StrWin::SetFinishMode( FinishType type, u8 frames )
{
  if( type < NUM_FINISH_TYPES )
  {
    m_finishType = type;
    m_finishFrame = frames;
    m_finishFrameCount = 0;
  }
  else{
    GFL_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief      �\�������I�����AProc() �� �ł��邾������ RES_FINISH ��Ԃ��悤�ɂȂ�
 */
//---------------------------------------------------------------------------------
void  StrWin::ForceQuit( void )
{
  m_bForceQuit = true;
}

//----------------------------------------------------------------------
/**
 * @brief      ���b�Z�[�W�������x�ύX
 *             �`��r���̕ύX�̓T�|�[�g�O�ł��B
 *
 * @param[in]   waitFrames    �P�����o�͂��Ƃ�wait�t���[���l
 * @param[in]   putCharCount  �P�x�ɏo�͂��镶����
 */
//----------------------------------------------------------------------
void  StrWin::SetMessageSpeed( u32 waitFrames, u32 putCharCount )
{
  m_waitFrameOrg = m_waitFrame = waitFrames;
  m_putCharCountPerFrameOrg = m_putCharCountPerFrame = putCharCount;
}
//----------------------------------------------------------------------
/**
 * @brief      ���b�Z�[�W�������x���ꎞ�I�ɕύX�i�{�^���������̂ݑ��x�A�b�v�Ȃǂɗ��p�j
 *
 * @param[in]   waitFrames    �P�����o�͂��Ƃ�wait�t���[���l
 * @param[in]   putCharCount  �P�x�ɏo�͂��镶����
 */
//----------------------------------------------------------------------
void  StrWin::SetMessageSpeedTemporary( u32 waitFrames , u32 putCharCount )
{
  m_waitFrame = waitFrames;
  m_putCharCountPerFrame = putCharCount;
}
//----------------------------------------------------------------------
/**
 * @brief      ���b�Z�[�W�������x�p�����[�^���擾
 * @param[out]  waitFrames    �P�����o�͂��Ƃ� wait �t���[����
 * @param[out]  putCharCount  �P�x�ɏo�͂��镶����
 */
//----------------------------------------------------------------------
void  StrWin::GetMessageSpeedParams( u32* waitFrames, u32* putCharCount ) const
{
  *waitFrames   = m_waitFrame;
  *putCharCount = m_putCharCountPerFrame;
}


//-----------------------------------------------------------------------
/**
 * @brief   ���s���A�ŏ�s�̃t�F�[�h�A�E�g�ɗv����t���[������ݒ肷��
 *        �i�������Ȃ���� DEFAULT_FADE_FRAMES ���ݒ肳��Ă��܂��j
 *
 * @param[in]   frames    �t���[����
 */
//-----------------------------------------------------------------------
void  StrWin::SetLineFadeOutFrames( u16 frames )
{
  m_lineFadeFrames = frames;
}
//-----------------------------------------------------------------------
/**
 * @brief   ���s���A�e�s�̃X�N���[���ɗv����t���[������ݒ肷��
 *        �i�������Ȃ���� DEFAULT_SCROLL_FRAMES ���ݒ肳��Ă��܂��j
 *
 * @param[in]   frames    �t���[����
 */
//-----------------------------------------------------------------------
void  StrWin::SetLineScrollFrames( u16 frames )
{
  m_lineScrollFrames = frames;
}
//-----------------------------------------------------------------------
/**
 * �|�[�Y������̉��y�[�W�^�C�v����
 * @return  ���y�[�W�^�C�v
 */
//-----------------------------------------------------------------------
StrWin::PauseType  StrWin::GetPauseType( void ) const
{
  if( m_seq != SEQ_PAUSE ){
    return PAUSETYPE_NONE;
  }
  if( m_procMode == PROC_MODE_AT_ONCE ){
    return PAUSETYPE_NONE;
  }
  if( isPauseTypeClear() ){
    return PAUSETYPE_CLEAR;
  }
  return PAUSETYPE_FEED;
}




//--------------------------------------------------------------------------------------
/**
 *  �����o���s�̕������ m_reader �ɐݒ肷��
 *  �im_pStrBuf, m_currentReadLine ���Q�Ɓj
 */
//--------------------------------------------------------------------------------------
void
StrWin::setupReader( void )
{
  const STRCODE* sp = m_pStrBuf->GetPtr();
  u32 line_len = LineLen( sp, m_currentReadLine );
  sp = GetLinePtr( sp, m_currentReadLine );
  if( sp )
  {
    GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Set Reader lineNo=%d,.. len=%d 1stCode=%04x \n", m_currentReadLine, line_len, *sp );
//    PrintCode( sp );
//    m_reader.SetString( sp, line_len, m_pHeap );
  }
  else{
    if( this->isReadEnd() ){
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] setupReader Fail, End\n");
    }else{
      GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] setupReader Fail, not END \n");
    }
  }
}

//--------------------------------------------------------------------------------------
/**
 * �ݒ肳�ꂽ�������ǂݏI��������ǂ�������
 *
 * @retval  bool
 */
//--------------------------------------------------------------------------------------
bool
StrWin::isReadEnd( void ) const
{
  return m_bReadLineDone;
}


//--------------------------------------------------------------------------------------
/**
 *  ���s�������J�n
 */
//--------------------------------------------------------------------------------------
void
StrWin::startLineFeed( void )
{
  GFL_PRINT_EX( STRWIN_PRINT_FLAG, "[STRWIN] Start Line Feed : currentDispLine=%d\n", m_currentDispLine );
  ++m_targetPaneIdx;
  if( m_targetPaneIdx >= m_numDispLines ){
    m_targetPaneIdx = 0;
  }
  checkNextLineColor();
  RefreshLine();

  m_lineFeedSeq = 0;
}

//--------------------------------------------------------------------------------------
/**
 * ���s�������X�V
 *
 * @retval  bool    ���s�������I�����Ă����� true
 */
//--------------------------------------------------------------------------------------
bool StrWin::progLineFeed( void )
{
  PANE_WORK* work = &m_paneWork[ m_targetPaneIdx ];

  switch( m_lineFeedSeq ){
  case 0:   // �擪�s�̃t�F�[�h�A�E�g���J�n
    work->fadeVal.StartLinear( 255, 0, m_lineFadeFrames );
    ++m_lineFeedSeq;

    /* fallthru */

  case 1:   // �擪�s�t�F�[�h�i�s
    {
      f32 alpha = work->fadeVal.Progress();
      work->pPane->SetAlpha( static_cast<u8>(alpha) );
      if( work->fadeVal.IsEnd() )
      {
        ++m_lineFeedSeq;
      }
    }
    break;

  case 2:   // �e�s�X�N���[���J�n
    for(u32 i=0; i<m_numDispLines; ++i)
    {
      // �擪�ȊO�̍s�̓X�N���[������
      if( m_paneWork[i].myPosIdx )
      {
        m_paneWork[i].myPosIdx--;
        m_paneWork[i].movePos.StartLinear( m_paneWork[i].mtx, m_paneWork[m_paneWork[i].myPosIdx].initialMtx, m_lineScrollFrames );
      }
      // �擪�s�͋󕶎��ݒ肵�A���̔z�u�s��
      else
      {
        m_hidePaneIdx = i;
        m_paneWork[i].myPosIdx = m_numDispLines - 1;
        m_paneWork[i].mtx = m_paneWork[m_paneWork[i].myPosIdx].initialMtx;
        m_paneWork[i].pPane->SetTranslate( m_paneWork[i].mtx );
        m_paneWork[i].pPane->SetString( L"" );
      }
    }
    ++m_lineFeedSeq;

    /* fallthru */

  case 3: // �e�s�X�N���[���i�s
    {
      u32 end_count = 0;
      for(u32 i=0; i<m_numDispLines; ++i)
      {
        if( i != m_hidePaneIdx )
        {
          m_paneWork[i].mtx = m_paneWork[i].movePos.Progress();
          m_paneWork[i].pPane->SetTranslate( m_paneWork[i].mtx );
          if( m_paneWork[i].movePos.IsEnd() ){
            ++end_count;
          }
        }
      }

      u32 btm_index = m_numDispLines - 1;
      if( end_count >= btm_index )
      {
        m_paneWork[ m_hidePaneIdx ].pPane->SetAlpha( 255 );
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
 * �t�B�j�b�V���҂�����
 *
 * @retval  bool    �t�B�j�b�V�����Ă���� true
 */
//--------------------------------------------------------------------------------------
bool StrWin::checkFinish( void )
{
  switch( m_finishType ){
  case FINISH_NONE:
    return true;

  case FINISH_TIMER:
    if( checkFinishFrame() ){
      return true;
    }
    return false;

  case FINISH_USER:
    if( checkFinishInput() ){
      return true;
    }
    return false;

  case FINISH_HYB:
    if( checkFinishInput() ){
      return true;
    }
    if( checkFinishFrame() ){
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
 *  �t�B�j�b�V���҂����� - ���[�U����
 */
//--------------------------------------------------------------
bool StrWin::checkFinishInput( void )
{
  if( !isPassed1stFrame() ){
    return false;
  }
  return IsFinishInput();
}
//--------------------------------------------------------------
/**
 *  �t�B�j�b�V���҂����� - �t���[���J�E���g
 */
//--------------------------------------------------------------
bool StrWin::checkFinishFrame( void )
{
  if( m_finishFrameCount > m_finishFrame )
  {
    return true;
  }

  if( ++m_finishFrameCount > m_finishFrame ){
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

void FadeVEC3::StartLinear( const gfl::math::VEC3& start, const gfl::math::VEC3& end, u32 steps )
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

const gfl::math::VEC3& FadeVEC3::Progress( void )
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






} /* end of namespace str */
} /* end of namespace gfl */

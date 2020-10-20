//=============================================================================
/**
 * @file    MessageWindow.cpp
 * @brief   ��������Ή����b�Z�[�W�E�B���h�E
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.05.20
 *
 * @li  sango��xy_msgWin.cpp���ڐA
 */
//=============================================================================

//#include "sound/sound.h"

#include "../include/PrintSystem.h"
#include "../include/MessageWindow.h"
#include "../../System/include/PokemonVersion.h"
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


//savedata::Config::MSG_SPEED MessageWindow::mDefaultMsgSpeed = savedata::Config::MSG_NORMAL;
/**
 * �R���X�g���N�^
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
#if 0 // @todo savedata�҂�
  SetMessageSpeed( mDefaultMsgSpeed , is60Fps );
#else
  SetMessageSpeed( 0, is60Fps );
#endif
  SetTagProcessor( &m_tagProc );
  SetLineFadeOutFrames( XY_MSG_FADE_FRAMES );
  SetLineScrollFrames( XY_MSG_SCROLL_FRAMES );
  m_tagProc.SetMsgWin(this);
}

/**
 * �f�X�g���N�^
 */
MessageWindow::~MessageWindow()
{
}

//--------------------------------------------------------------------------------------
/**
 * @brief  �\�����C�������i�P��Ăяo�����Ƃɏ������X�V���܂��j
 *
 * @retval   StrWin::Result   ���ʃR�[�h
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
    //A�{�^���u�[�X�g
    #if 1
    if( m_bCommAutoFlag == false &&
        button->IsHold(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) &&
        m_enableCtrlKey && m_enalbeMsgBoost )
    {
      if( m_waitCnt == 0 && m_isWaitCallback == false )
      {
        // �Q��ĂԂ̂ɋ߂��������e�ƂȂ�悤�ꎞ�I�Ƀp�����[�^��������
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
    
    //�`�文��Procces���Ă΂�A�ŏ�����`�悳���̂ŁA
    //�����ŐF�ύX�t���O�̓��Z�b�g���Ă���
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
 * @brief �y�C���̒ǉ�
 *
 * @param paneIdx �ǉ�����y�C���C���f�b�N�X
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::AddLinePaneIdx( u32 paneIdx )
{
  gfl2::str::StrWin::AddLinePaneIdx( paneIdx );

  // ���߂ăy�C���ǉ����ꂽ���A���̕����擾���Ă���
  if( !m_bFirstPaneAdded )
  {
    // ���f�[�^�Ȃǂ��X�V����
    RefreshFirstPane();
  }
  // �ǉ������Ǘ�
  ++m_pane_num;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief �y�C���̒ǉ�
 *
 * @param paneIdx �ǉ�����y�C���C���f�b�N�X
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::AddLinePaneIdxPtr( gfl2::lyt::LytTextBox * pTextBoxPain )
{
  gfl2::str::StrWin::AddLinePaneIdxPtr( pTextBoxPain );

  // ���߂ăy�C���ǉ����ꂽ���A���̕����擾���Ă���
  if( !m_bFirstPaneAdded )
  {
    // ���f�[�^�Ȃǂ��X�V����
    RefreshFirstPane();
  }
  // �ǉ������Ǘ�
  ++m_pane_num;
}



/* -------------------------------------------------------------------------*/
/**
 * @brief �ŏ��ɓo�^�����y�C�����̍X�V
 * @note  AddLinePaneIdx��Ƀf�[�^�������������s�������ꍇ�Ɏg�p���Ă�������
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
        GFL_PRINT("�Â�g2d_fw��`�ł��B�R���o�[�g���Ȃ����Ă��������B\n");
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
 * @brief �ʒu�}�g���b�N�X���ď���������
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
 * @brief ��������Z�b�g����
 *
 * @param str    �Z�b�g���镶���o�b�t�@
 * @param langId �\������ID(���k���̂���)
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

  // �f�t�H���g������A�̈敝�Œ�^�O���X�g���[���E�B���h�E�p�ɒu������
  const gfl2::str::STRCODE* sp = (m_pStrBuf->GetPtr());
  bool  bCalcMode = false;
  
  //�^�O�P�ʂŃ��b�Z�[�W���k������d�g�݁B
  //kujira�ł�MessageWindow�S�̂����ďk������̂ŕs�v�B
  //�������ɂ�wordset���C������K�v����B
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
//          GFL_PRINT( "������:%d, �G���A���F%d, ratio=%5.2f->%d\n", (int)strWidth, (int)areaLimitWidth, r, ratio );
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

  //�E�B���h�E���͂ݏo��悤�Ȃ�k������
  const int strLen = GetStringWidthMax();
  const f32 winWidth = m_paneWork[0].pPane->GetSize().width;
  if( strLen > winWidth )
  {
    GFL_PRINT("len[%d]win[%f]\n",strLen,winWidth);
    GFL_PRINT("�\���̈���͂ݏo�܂����I�I�I\n");

    nw::lyt::Size size = m_paneWork[0].pPane->GetFontSize();
    m_paneFontWidth = m_paneFontWidth * (winWidth/strLen);
    size.width = m_paneFontWidth;
    //�͂ݏo�邱�Ƃ��������̂ŏ����T�o��ǂ�
    size.width -= 0.05f;
    for( int i=0;i<m_pane_num;i++ )
    {
      m_paneWork[i].pPane->SetFontSize(size);
#if PM_DEBUG
#if 0 // @todo �f�o�b�O����������
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
#if 0 // @todo �f�o�b�O����������
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
 * @brief   �e���C���̕��������v���Z�o��A�ő�l��Ԃ�
 *
 * @return  ������
 */
/* -------------------------------------------------------------------------*/
int MessageWindow::GetStringWidthMax( void )
{
  return print::GetStringWidth( m_pStrBuf , m_font, m_paneFontWidth );
/*
  // �f�t�H���g������A�̈敝�Œ�^�O���X�g���[���E�B���h�E�p�ɒu������
  const gfl::str::STRCODE* sp = (m_pStrBuf->GetPtr());
  bool  bCalcMode = false;
  f32 str_width_max = 0;
  f32 str_width = 0;
  s32 line = 0;

  while( *sp != gfl::str::EOM_CODE )
  {
    // �����Z�b�g
    if( *sp == gfl::str::CR_CODE )
    {
      // �ő啶�������`�F�b�N
      if( str_width_max < str_width )
      {
        str_width_max = str_width;
      }

      str_width = 0;
      ++sp;
    }
    // �ʏ핶��
    else if( *sp != gfl::str::TAG_START_CODE )
    {
      // �����v�Z�㕶����i�߂�
      f32 w = m_tagProc.CalcCharWidth( sp, m_font, m_paneFontWidth );
      //str_width += gfl::math::FCeil(w);
      str_width += w;
      ++sp;
    }
    // �^�O
    else
    {
      // �^�O���X�L�b�v���A�����v�Z

      TagGroup  tagGroup = GetTagGroup( sp );
      u32       tagIdx   = GetTagIndex( sp );
      //���s�^�O���`�F�b�N
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
      //�����ŕ������������ƁA�A���Ń^�O�������Ă���Ƃ܂����B
    }
  }

  // �ŏI�s���`�F�b�N
  if( str_width_max < str_width )
  {
    str_width_max = str_width;
  }

  return static_cast<int>(gfl::math::FCeil(str_width_max));
*/
}

/* -------------------------------------------------------------------------*/
/**
 * @brief �t�H���g�̍������擾����
 *
 * @return �t�H���g�̍���
 */
/* -------------------------------------------------------------------------*/
int MessageWindow::GetFontHeight( void )
{
  if( m_paneWork[0].pPane )
  {
    return static_cast<int>( m_paneWork[0].pPane->GetFontSize().height );
  }
  GFL_ASSERT_MSG(0,"�t�H���g�������擾�ł��܂���\n"); //@check �Ώ��ς�
  return 0;
}

/**
 *  ���s�����[�X����
 */
bool MessageWindow::IsPauseRelease( void )
{
  const USER_INPUT_TYPE ret = checkUserInput();
  if( ret != USER_INPUT_NO )
  {
    if( ret == USER_INPUT_UI )
    {
      //��������̎���SE��炳�Ȃ�
#if 0 // @todo SE�����҂�
      xy_snd::PlaySE( SEQ_SE_MESSAGE );
#endif
    }
    return true;
  }
  return false;
}
/**
 *  �\���I����t�B�j�b�V������
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
 *  �F�ύX���s�������ǂ�������
 */
bool MessageWindow::IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom )
{
  bool result = m_tagProc.IsColorChanged( top, bottom );
  return result;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief �L�[���͉ېݒ�
 * @note  true�ňꎞ�L�[���͖���
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetSuspendKey( bool flg )
{
  m_bSuspendKey = flg;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief �ʐM�������L�[����(+���b�Z�[�W�u�[�X�g����)
 * @note  true�Ŏ�������
 */
/* -------------------------------------------------------------------------*/
void MessageWindow::SetMessageAutoFlg( bool flg )
{
  m_bCommAutoFlag = flg;
}

//--------------------------------------------------------------------------
/**
 *	@brief  �����L�[���������1�x�����s��
 */
//--------------------------------------------------------------------------
void MessageWindow::SetMessageAutoOnce( void )
{
  m_bAutoFlagOnce = true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ���b�Z�[�W���x�̐ݒ�(config��)
 *        �f�t�H���g�l�͕ύX���܂���B
 *
 * @param  savedata::Config::MSG_SPEED spd ���b�Z�[�W���x
 */
/* -------------------------------------------------------------------------*/
#if 0 // @todo savedata�҂�
void MessageWindow::SetMessageSpeed( savedata::Config::MSG_SPEED spd , bool is60Fps )
#else
void MessageWindow::SetMessageSpeed( int spd , bool is60Fps )
#endif
{
  m_is60Fps = is60Fps;

  bool isFast = true;
  
#if 0
  //���{��A�؍���
  static const int frameWaitArr[savedata::Config::MSG_SPEED_MAX] = { 1,0,0,0 };
  static const u32 putCountArr[savedata::Config::MSG_SPEED_MAX]  = { 1,1,2,255 };

  static const int frameWaitArr60[savedata::Config::MSG_SPEED_MAX] = { 2,1,0,0 };
  static const u32 putCountArr60[savedata::Config::MSG_SPEED_MAX]  = { 1,1,1,255 };

  //���̂ق��̌���̏ꍇ
  static const u32 putCountArr_Fast[savedata::Config::MSG_SPEED_MAX]  = { 2,2,4,255 };
  static const u32 putCountArr60_Fast[savedata::Config::MSG_SPEED_MAX]  = { 2,2,2,255 };
#else
  //���{��A�؍���
  static const int frameWaitArr[] = { 1,0,0,0 };
  static const u32 putCountArr[]  = { 1,1,2,255 };

  static const int frameWaitArr60[] = { 2,1,0,0 };
  static const u32 putCountArr60[]  = { 1,1,1,255 };

  //���̂ق��̌���̏ꍇ
  static const u32 putCountArr_Fast[]  = { 2,2,4,255 };
  static const u32 putCountArr60_Fast[]  = { 2,2,2,255 };
#endif

//  const int msgSpd = static_cast<int>(spd);

  if(( System::GetLang() == POKEMON_LANG_JAPAN) || (System::GetLang() == POKEMON_LANG_KOREA)){
    isFast=false;
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
 * @brief �f�t�H���g���b�Z�[�W���x�̐ݒ�
 *
 * @param  savedata::Config::MSG_SPEED spd ���b�Z�[�W���x
 */
/* -------------------------------------------------------------------------*/
#if 0 // @todo savedata�҂�
void MessageWindow::SetDefaultMessageSpeed( savedata::Config::MSG_SPEED spd )
{
  mDefaultMsgSpeed = spd;
}
#endif

//-----------------------------------------------------------------------
/**
 *  @brief      �s�X�V���ɌĂ΂��
 */
//-----------------------------------------------------------------------
void MessageWindow::RefreshLine(void)
{
  m_tagProc.ResetParam();
}

//-----------------------------------------------------------------------
/**
 * TagProcessor �����������^�O�ɂ���Ă�
 * �����Ă����Ɏ��̕������������Ăق����Ȃ��ꍇ������B
 * ���̂悤�ȏꍇ�A���̊֐����p�����ă^�O�R�[�h���`�F�b�N���A
 * �����Ď��̕������������������Ȃ��i�P�x�^�O�����������s���t���[����݂������j�ꍇ��
 * true ��Ԃ��悤�ɂ���B
 *
 * @param   tagInfo   �^�O���
 *
 * @retval  bool    ���̕����������ɏ������Ȃ��ꍇ true ��Ԃ�
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
    else   ///< �R�[���o�b�N�����ύX(�P��
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
 *  ���[�U�[���͔���i���s�����[�X�^�\����t�B�j�b�V�����ʁj
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
    m_bAutoFlagOnce = false; // 1�x�����L���Ȃ̂Ń��Z�b�g
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


//==============================================================================================
//==============================================================================================
nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::Process( u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext )
{
  const gfl2::str::STRCODE* sp  = pContext->str - 1;

  // �y�C���T�C�Y�𒴂��镶���`�悪�s����ƁA�Ȃ����^�O�J�n�R�[�h�𔺂킸�ɌĂ΂�邱�Ƃ�����B�����s���B�Ƃ肠��������B
  //  ��cr�R�[�h�Ƃ��X�y�[�X�Ƃ��͕��ʂɗ��邱�Ƃ�����
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
            nw::ut::Color8(   0,   0,   0, 255 ),   // ��(��Ń��Z�b�g�����ɂȂ��Ă�)
            nw::ut::Color8( 255,   0,   0, 255 ),   // ��
            nw::ut::Color8(   0, 140, 255, 255 ),   // ��
            nw::ut::Color8(   0, 192,   0, 255 ),   // ��
          };
          setTextColor( pContext->writer , colorTable[colorIdx] );
        }
      }
      break;
    }
    //�X�Ŗ߂�l��ς���ꍇCalcRect���Ή����邱��
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
 *  �ėp�R���g���[������
 */
nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::process_GeneralCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp )
{
  switch( tagIdx ){
  case TAGIDX_GENERAL_CHANGE_COLOR:
    {
      static const nw::ut::Color8 colorTable[] = {
        nw::ut::Color8( 255,   0,   0, 255 ),   // ��
        nw::ut::Color8(   0, 192,   0, 255 ),   // ��
        nw::ut::Color8(   0, 140, 255, 255 ),   // ��
        nw::ut::Color8( 255, 128,   0, 255 ),   // �I�����W
        nw::ut::Color8( 255, 128, 255, 255 ),   // �s���N
        nw::ut::Color8( 140,   0, 255, 255 ),   // ��
        nw::ut::Color8(  80, 200, 228, 255 ),   // ���F
        nw::ut::Color8( 255, 230,  80, 255 ),   // �}���p��
        nw::ut::Color8( 100, 180, 255, 255 ),   // �R���e�X�g�p��
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
        GFL_PRINT("Window����0�ɂȂ��Ă���̂ŃZ���^�����O�^�O�͎g���܂���I\n");
      }
    }
    break;

  case TAGIDX_GENERAL_X_FIT_RIGHT:       ///< �����`��w���W �E��
    {
      if( m_windowWidth != 0 )
      {

        f32 margin = (f32)(GetTagParam( sp,0 ));
        f32 strWidth = calcStrWidth( pContext, sp );
        f32 xpos = (m_windowWidth - strWidth - margin - 1.5f);//�덷���o��̂Œ���
        //GFL_PRINT("[%f][%d][%f][%f][%f]\n",strWidth ,m_windowWidth,xpos,margin,pContext->writer->GetCursorX());
        pContext->writer->SetCursorX( xpos );
      }
      else
      {
        GFL_PRINT("Window����0�ɂȂ��Ă���̂ŉE�񂹃^�O�͎g���܂���I\n");
      }
    }
    break;

  case TAGIDX_GENERAL_X_ADD:
    {
      f32  x = pContext->writer->GetCursorX() + (f32)(GetTagParam(sp,0));
      pContext->writer->SetCursorX( x );
    }
    break;

  case TAGIDX_GENERAL_X_MOVE:        ///< �����`��w���W �w��
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
  case TAGIDX_GENERAL_NOTUSED_MESSAGE:    // �R���o�[�^�Ŗ��g�p���b�Z�[�W�Ɏd����ł���^�O
#if PM_DEBUG
    {
      uint messageId = GetTagParam( sp, 0 );
      GFL_ASSERT_MSG(0, "���g�p���b�Z�[�W(%d)���\������܂���", messageId);//@check 
    }
#endif
    break;
  default:
    GFL_PRINT("OtherTagCode[%d]\n",tagIdx);
    break;
  }
  //�X�Ŗ߂�l��ς���ꍇCalcRect���Ή����邱��
  return OPERATION_NO_CHAR_SPACE;
}

nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
FontTagProc::process_StreamCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp )
{
  switch( tagIdx ){
  case TAGIDX_STREAM_LINE_FEED:      ///< �y�[�W�؂�ւ�(�s����
    //�O�ŏ�������Ă����ɂ͗��Ă��Ȃ��B
    GFL_ASSERT_MSG(0,"�y�[�W�؂�ւ�(�s���� �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_PAGE_CLEAR:     ///< �y�[�W�؂�ւ�(�N���A
    //�O�ŏ�������Ă����ɂ͗��Ă��Ȃ��B
    GFL_ASSERT_MSG(0,"�y�[�W�؂�ւ�(�N���A �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_WAIT_ONE:       ///< �E�F�C�g�ύX(�P���j
    if( m_msgWin )
    {
      if( m_topStrPos < pContext->str )
      {
        //�^�O�`�F�b�N���ɓ����B
        //u16 cnt = GetTagParam( sp, 0 );
        //m_msgWin->SetWaitCnt(cnt);
        //GFL_PRINT("[%8x][%8x]\n",m_topStrPos,pContext->str);
      }
    }
    break;
  case TAGIDX_STREAM_WAIT_CONT:      ///< �E�F�C�g�ύX(�p���j
    GFL_ASSERT_MSG(0,"�E�F�C�g�ύX(�p�� �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_WAIT_RESET:     ///< �E�F�C�g���Z�b�g
    //GFL_ASSERT_MSG(0,"�E�F�C�g���Z�b�g �^�O�͖������ł��B");//@check �R�����g ����͈ȑO�g���Ă����^�O�B�E�F�C�g�p���͎g���Ă��Ȃ��̂Ŗ{���͗v��Ȃ��B
    break;
  case TAGIDX_STREAM_CALLBACK_ONE:   ///< �R�[���o�b�N�����ύX(�P��
    if( m_msgWin )
    {
      if( m_topStrPos < pContext->str )
      {
        //�^�O�`�F�b�N���ɓ����B
        //u16 arg = GetTagParam( sp, 0 );
        //m_msgWin->CallMessgaeCallback(arg);
      }
    }
    break;
  case TAGIDX_STREAM_CALLBACK_CONT:  ///< �R�[���o�b�N�����ύX(�p��
    GFL_ASSERT_MSG(0,"�R�[���o�b�N�����ύX(�p�� �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_CALLBACK_RESET: ///< �R�[���o�b�N�������Z�b�g
    GFL_ASSERT_MSG(0,"�R�[���o�b�N�������Z�b�g �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_CLEAR_WIN:      ///< �`��̈�N���A
    GFL_ASSERT_MSG(0,"�`��̈�N���A �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  case TAGIDX_STREAM_CTRL_SPEED:     ///< ���x�R���g���[��
    GFL_ASSERT_MSG(0,"���x�R���g���[�� �^�O�͖������ł��B");//@check �쐬���ɋC�Â�
    break;
  }
  //�X�Ŗ߂�l��ς���ꍇCalcRect���Ή����邱��
  return OPERATION_DEFAULT;

}

nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
    FontTagProc::CalcRect( nw::ut::Rect * pRect, u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext )
{
  const gfl2::str::STRCODE* sp  = pContext->str - 1;

  // �y�C���T�C�Y�𒴂��镶���`�悪�s����ƁA�Ȃ����^�O�J�n�R�[�h�𔺂킸�ɌĂ΂�邱�Ƃ�����B�����s���B�Ƃ肠��������B
  //  ��cr�R�[�h�Ƃ��X�y�[�X�Ƃ��͕��ʂɗ��邱�Ƃ�����
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
  u32       tagIdx   = GetTagIndex( sp );

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
    GFL_ASSERT_MSG(0,"�F�ύX�^�O������q�ɂȂ��Ă��܂��B\n");//@check �Ώ��ς�
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
 * @brief �����̕����擾����
 *
 * @param sp            �擾�����������������Ă���|�C���^
 * @param font          �t�H���g
 * @param curFontWidth  �t�H���g��
 *
 * @return ������
 */
/* -------------------------------------------------------------------------*/
f32 FontTagProc::CalcCharWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth )
{
  int width = font->GetCharWidth( *sp );
  f32  ratio = curFontWidth / static_cast<f32>(font->GetWidth());
  f32  f_width = (f32)width * ratio;

  return f_width;
}

//���b�Z�[�W���Z�b�g����O��1��Ă�œ����p�����[�^���Z�b�g
void FontTagProc::ResetParam(void)
{
  m_topStrPos = 0;
  m_bColorChanged = false;
}




//--------------------------------------------------------
/**
 *  ������`��T�C�Y�i���j���v�Z
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
 *  �^�O�����ɂ��`��F���ς����Ă��邩�ǂ�������
 *
 *  @param[out]   top       �`��F���ς����Ă���ꍇ�A�O���f�[�V�����J�n�F������
 *  @param[out]   top       �`��F���ς����Ă���ꍇ�A�O���f�[�V�����I�[�F������
 *
 *  @retval   bool      �`��F���ς����Ă���ꍇ�� true
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

#ifndef __GFL_STRWIN_H__
#define __GFL_STRWIN_H__
//===========================================================================
/**
 * @file    gfl_StrWin.h
 * @brief   GAMEFREAK Library String System ( �����\���E�B���h�E )
 * @author  GAMEFREAK inc.
 * @date    2010.11.12
 */
//===========================================================================
#pragma once

#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <grp/g2d/gfl_LytSys.h>

#include <str/gfl_StrSys.h>
#include <str/gfl_StrBufReader.h>
#include <str/gfl_TagProc.h>


namespace gfl {
namespace str {

template <typename  Type>
struct FadeBase {
public:
  bool  IsEnd( void ) const { return m_bEnd; }

protected:
  Type    m_value;
  Type    m_endValue;
  u32     m_stepMax;
  u32     m_step;
  bool    m_bEnd;
};

typedef struct FadeBase<f32>              FadeBaseF32;
typedef struct FadeBase<gfl::math::VEC3>  FadeBaseVEC3;


struct FadeF32 : public FadeBaseF32 {
public:
  void StartLinear( f32 start, f32 end, u32 steps );
  f32  Progress( void );

private:
  f32    m_stepValue;
};

struct FadeVEC3 : public FadeBaseVEC3 {
public:
  void StartLinear( const gfl::math::VEC3& start, const gfl::math::VEC3& end, u32 steps );
  const gfl::math::VEC3& Progress( void );

private:
  gfl::math::VEC3    m_stepValue;
};

/*
template <typename  Type>
struct FadeSign : public FadeBase {
public:
  Setup( Type start, Type end, int steps, f32 ratio = 1.0f );
}
*/

/**
 *  ������E�B���h�E���N���X
 */
class StrWin {

  GFL_FORBID_COPY_AND_ASSIGN( StrWin );


public:

  /**
   *  ���ʃR�[�h
   */
  enum Result {
    RES_DONE,       ///< �\���������Ō�܂ŏI������
    RES_CONTINUE,   ///< �\��������
    RES_PAUSE,      ///< �|�[�Y��A�w���҂�
    RES_LINE_FEED,  ///< �|�[�Y�����̏�����
    RES_FINISH,     ///< �\�������̏I����A�ݒ肳�ꂽ�t�B�j�b�V���҂��܂ŏI��
  };

  /**
   *  �t�B�j�b�V���҂��^�C�v
   */
  enum FinishType{
    FINISH_NONE,    ///< �t�B�j�b�V���҂��Ȃ��i�f�t�H���g����j
    FINISH_USER,    ///< ���炩�̃��[�U�[���͂������������
    FINISH_TIMER,   ///< ��莞�ԁi�Ăяo���t���[�����j�o�߂��������
    FINISH_HYB,     ///< FINISH_USER & FINISH_TIMER = �ǂ��炩�̏����ŏ���
    
    NUM_FINISH_TYPES,
  };

  /**
   *  ���y�[�W�^�C�v�i���y�[�W�w���҂����A����\�j
   */
  enum PauseType {
    PAUSETYPE_NONE = 0, ///< ���y�[�W�w���҂����ł͂Ȃ�
    PAUSETYPE_CLEAR,    ///< ���y�[�W�w����A�E�B���h�E���e���N���A����
    PAUSETYPE_FEED,     ///< ���y�[�W�w����A�e�L�X�g���P�s����
  };


  //=============================================================================================
  /**
   * �R���X�g���N�^
   *
   * @param[in]   lytWk         �\���Ɏg�p���郌�C�A�E�g���[�N�|�C���^
   * @param[in]   numLines      �\���s��
   * @param[in]   waitFrames    �P�����o�͂��Ƃ�wait�t���[���l
   * @param[in]   pHeap         �q�[�v�n���h��
   * @param[in]   pTagProc      �^�O�v���Z�b�T�i�s�v�Ȃ�NULL�ł��j
   * @param[in]   putCharCount  �P�x�ɏo�͂��镶����
   */
  //=============================================================================================
  StrWin( gfl::grp::g2d::LytWk* lytWk, u16 numLines, u32 waitFrames, gfl::heap::HeapBase* pHeap,
      TagProcBase* pTagProc = NULL, u32 putCharCount=1 );
  virtual ~StrWin();


  //----------------------------------------------------------------------
  /**
   * @brief   ������\���p�y�C��Index��ݒ�i�\���s�����A�Ăяo�����Ɓj
   *
   * @param[in]   paneIdx     �y�C��Index
   *
   */
  //----------------------------------------------------------------------
  virtual void     AddLinePaneIdx( u32 paneIdx );

  //----------------------------------------------------------------------
  /**
   * @brief   ������\���p�y�C��Index���Œᕶ�����Ɠ����ɐݒ�i�\���s�����A�Ăяo�����Ɓj
   *
   * @param[in]   paneIdx     �y�C��Index
   * @param[in]   strMinLen   �Œᕶ����
   *
   */
  //----------------------------------------------------------------------
  virtual void     AddLinePaneIdxWithStrMinLen( u32 paneIdx, u32 strMinLen );

  //----------------------------------------------------------------------
  /**
   * @brief   ��������Z�b�g����
   *
   * @param[in]   str      �\�����镶����
   *                     �i���̊֐����Ă񂾌�̓N���X�����œ��e��ێ�����̂ŁA
   *                      ���̃C���X�^���X�͔j�����Ă��\���܂���j
   *
   */
  //----------------------------------------------------------------------
  virtual void     SetString( const StrBuf& str );

  //--------------------------------------------------------------------------------------
  /**
   * @brief  �\�����C�������i�P��Ăяo�����Ƃɏ������X�V���܂��j
   *
   * @retval   StrWin::Result   ���ʃR�[�h
   */
  //--------------------------------------------------------------------------------------
  virtual Result   Proc( void );

  //--------------------------------------------------------------------------------------
  /**
   * @brief    �\�����Ō�܂ōs��
   */
  //--------------------------------------------------------------------------------------
  void     ProcAtOnce( void );

  //--------------------------------------------------------------------------------------
  /**
   * @brief    �P�y�[�W���̕\���������s��
   */
  //--------------------------------------------------------------------------------------
  Result     ProcForPage( void );

  //----------------------------------------------------------------------
  /**
   * @brief      �|�[�Y�������Ď��̍s��\������
   */
  //----------------------------------------------------------------------
  void  Restart( void );

  //----------------------------------------------------------------------
  /**
   * @brief      �|�[�Y�������Ď��̍s��\������
   *
   * @param[in]  bClear   true�Ȃ� ���s�̕\���O�ɃE�B���h�E���e��S�N���A����
   */
  //----------------------------------------------------------------------
  void  Restart( bool bClear );

  //----------------------------------------------------------------------
  /**
   * @brief      �\��������̃N���A
   */
  //----------------------------------------------------------------------
  void  Clear( void );

  //----------------------------------------------------------------------
  /**
   * @brief      �t�B�j�b�V���҂����[�h�ݒ�
   *
   * @param[in]  type     �t�B�j�b�V���҂����[�h
   * @param[in]  frames   �҂��t���[�����itype == FINISH_TIMER || FINISH_HYB �̏ꍇ�̂ݗL���j
   */
  //----------------------------------------------------------------------
  void  SetFinishMode( FinishType type, u8 frames );

  //----------------------------------------------------------------------
  /**
   * @brief      �\�������I�����AProc() �� �ł��邾������ RES_FINISH ��Ԃ��悤�ɂȂ�
   */
  //----------------------------------------------------------------------
  void  ForceQuit( void );

  //----------------------------------------------------------------------
  /**
   * @brief      ���b�Z�[�W�������x�ύX
   *
   * @param[in]   waitFrames    �P�����o�͂��Ƃ�wait�t���[����
   * @param[in]   putCharCount  �P�x�ɏo�͂��镶����
   */
  //----------------------------------------------------------------------
  void  SetMessageSpeed( u32 waitFrames , u32 putCharCount=1 );

  //----------------------------------------------------------------------
  /**
   * @brief      ���b�Z�[�W�������x���ꎞ�I�ɕύX�i�{�^���������̂ݑ��x�A�b�v�Ȃǂɗ��p�j
   *
   * @param[in]   waitFrames    �P�����o�͂��Ƃ�wait�t���[����
   * @param[in]   putCharCount  �P�x�ɏo�͂��镶����
   */
  //----------------------------------------------------------------------
  void  SetMessageSpeedTemporary( u32 waitFrames , u32 putCharCount=1 );

  //----------------------------------------------------------------------
  /**
   * @brief      ���b�Z�[�W�������x�p�����[�^���擾
   * @param[out]  waitFrames    �P�����o�͂��Ƃ� wait �t���[����
   * @param[out]  putCharCount  �P�x�ɏo�͂��镶����
   */
  //----------------------------------------------------------------------
  void  GetMessageSpeedParams( u32* waitFrames, u32* putCharCount ) const;

  //-----------------------------------------------------------------------
  /**
   * @brief   ���s���A�ŏ�s�̃t�F�[�h�A�E�g�ɗv����t���[������ݒ肷��
   *        �i�������Ȃ���� DEFAULT_FADE_FRAMES ���ݒ肳��Ă��܂��j
   *
   * @param[in]   frames    �t���[����
   */
  //-----------------------------------------------------------------------
  void   SetLineFadeOutFrames( u16 frames );

  //-----------------------------------------------------------------------
  /**
   * @brief   ���s���A�e�s�̃X�N���[���ɗv����t���[������ݒ肷��
   *        �i�������Ȃ���� DEFAULT_SCROLL_FRAMES ���ݒ肳��Ă��܂��j
   *
   * @param[in]   frames    �t���[����
   */
  //-----------------------------------------------------------------------
  void   SetLineScrollFrames( u16 frames );

  //-----------------------------------------------------------------------
  /**
   * �|�[�Y������̉��y�[�W�^�C�v����
   * @return  ���y�[�W�^�C�v
   */
  //-----------------------------------------------------------------------
  PauseType  GetPauseType( void ) const;

protected:
  //-----------------------------------------------------------------------
  /**
   *  @brief    �s����Ȃǂ̈ꎞ��~�����`�F�b�N
   *            ����~�����̏�����C�ӂɔ��肵�����ꍇ�ɃI�[�o�[���C�h���Ă��������B
   *
   *  @retval   bool    true ��Ԃ������~��������
   */
  //-----------------------------------------------------------------------
  virtual bool  IsPauseRelease( void ){ return false; }

  //-----------------------------------------------------------------------
  /**
   *  @brief    �t�B�j�b�V���҂��`�F�b�N
   *            ���t�B�j�b�V���҂��̏�����C�ӂɔ��肵�����ꍇ�ɃI�[�o�[���C�h���Ă��������B
   *
   *  @retval   bool    true ��Ԃ�����t�B�j�b�V���iProc() �����ʃR�[�h RES_FINISH ��Ԃ��悤�ɂȂ�j
   */
  //-----------------------------------------------------------------------
  virtual bool  IsFinishInput( void ){ return true; }

  //-----------------------------------------------------------------------
  /**
   * @brief     �J���[�ύX�L���`�F�b�N
   *            ���P�s���̕�����`�揈�����I��������ɌĂяo����܂��B
   *               �^�O�����Ȃǂɂ��y�C���̕`��F��ς��Ă���ꍇ�ɃI�[�o�[���C�h���Ă��������B
   *               ���̍s�i�y�C���j�֕`����A�w�肳�ꂽ�F�ŊJ�n���܂��B
   *
   * @param[out]   top      �O���f�[�V�����J�n�F
   * @param[out]   bottom   �O���f�[�V�����I���F �i�O���f�[�V�����s�v�̏ꍇ�Atop �Ɠ����l�ɂ��Ă��������j
   *
   * @retval    bool    �F�ύX���s���Ă���ꍇ�� top, bottom �𐳂����ݒ�̏�Atrue ��Ԃ��Ă��������B
   */
  //-----------------------------------------------------------------------
  virtual bool  IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom )
  {
    GFL_UNUSED(top);
    GFL_UNUSED(bottom);
    return false;
  }

  //-----------------------------------------------------------------------
  /**
   *  @brief      TagProcessor �ݒ�
   *
   *  @param[in]  pTagProc    �^�O�v���Z�b�T
   *
   */
  //-----------------------------------------------------------------------
  void   SetTagProcessor( TagProcBase* pTagProc )
  {
    m_pTagProc = pTagProc;
  }

  //-----------------------------------------------------------------------
  /**
   *  @brief      �s�X�V���ɌĂ΂��
   *
   */
  //-----------------------------------------------------------------------
  virtual void   RefreshLine(void){};


private:
  //----------------------------------------------------------------------
  // �f�o�b�O Print �L��
  //----------------------------------------------------------------------
  enum {
    STRWIN_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL1)
  };


  enum {
    STR_BUFFER_SIZE_MIN   = 512,   ///< �Œ���A���ꂾ���̕��������i�[���邾���̃o�b�t�@���m�ۂ���
    LINE_BUFFSER_SIZE_MIN =  40,   ///< �P�s�\���p�Ɏw�肳�ꂽ�y�C�������ꂾ���̃T�C�Y�̃o�b�t�@�������Ă��Ȃ���Ίg������

    DEFAULT_FADE_FRAMES   = 6,    ///< �\���擪�s���t�F�[�h�A�E�g����̂ɗv����t���[�����̃f�t�H���g�l
    DEFAULT_SCROLL_FRAMES = 4,    ///< ���s�X�N���[���ɗv����t���[�����̃f�t�H���g�l

    TAGCODE_LINEFEED  = 0xbe00,
    TAGCODE_PAGECLEAR = 0xbe01,
  };

  typedef enum {
    SEQ_DONE,
    SEQ_FINISH,
    SEQ_PAUSE,
    SEQ_LINE_FEED,
    SEQ_PUT,
    SEQ_CLEAR,
    SEQ_TAGPROC_FOR_NEWLINE,
  }Sequence;

  enum ProcMode {

    PROC_MODE_NORMAL = 0,
    PROC_MODE_AT_ONCE,
    PROC_MODE_FOR_PAGE,
  };

  /**
   *
   */
  typedef struct {
    gfl::grp::g2d::LytTextBox*   pPane;
    gfl::math::VEC3              initialMtx;
    gfl::math::VEC3              mtx;
    FadeF32                      fadeVal;
    FadeVEC3                     movePos;
    u16                          timer;
    u8                           myPosIdx;

  }PANE_WORK;


protected:
  PANE_WORK*              m_paneWork;
  StrBuf*                 m_pStrBuf;

  /**
   *  �ǂݍ��񂾃^�O���
   */
  class TagInfo {
    public:
    STRCODE    code;          ///< �^�O�p�����[�^
    u32        paramCount;    ///< �����̐�
    u16        params[ gfl::str::NUM_TAG_PARAM_MAX ];  ///< �����̓��e

    // ctor
    TagInfo()
    {
      Initialize( EOM_CODE );
    }
    // dtor
    ~TagInfo(){};

    // Initialize
    void Initialize( STRCODE  tagCode )
    {
      code = tagCode;
      paramCount = 0;
      for(u32 i=0; i<GFL_NELEMS(params); ++i){
        params[i] = 0;
      }
    }
  };


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
  virtual bool   CheckTagProcBreak( const TagInfo&  tagInfo ){ return false; };


private:
  /**
   *  �����񏈗����ʁi�����p�j
   */
  enum  SubResult {
    SUB_RES_DEFAULT,        ///< �f�t�H���g�����i�������C���N�������g���P�t���o�͕������ɒB���Ă�����I���j
    SUB_RES_FORCE_QUIT,     ///< ���������B�ł������I�ɏI������i�X�N���[���҂��A�y�[�W�҂��Ȃǁj
    SUB_RES_FORCE_RECALL,   ///< �����I�ɍăR�[������i�^�O�����Ȃǂŕs�v�ȂP�t����҂��Ȃ��悤�Ɂj
  };

  /**
   *�@�V�K������E�V�K�s�̏o�́iSEQ_PUT�j��
   */
  enum  PutState {
    PUT_READY = 0,    ///< �o�͏�����
    PUT_PASSED_PROC,  ///< �P�x�ł� SEQ_PUT �v���Z�X��ʉ߂����^���̃v���Z�X�ɐ؂�ւ���������
    PUT_PASSED_FRAME, ///< PUT_PASSED_PROC �̏�ԂŃt���[���������I����
  };

  TagProcBase*            m_pTagProc;
  gfl::heap::HeapBase*    m_pHeap;
  gfl::grp::g2d::LytWk*   m_pLytWk;
  FinishType              m_finishType;
  u16                     m_lineFadeFrames;
  u16                     m_lineScrollFrames;
  u8                      m_numDispLines;
  u8                      m_numStrLines;
  u8                      m_numPanes;
  u8                      m_currentReadLine;
  u8                      m_currentDispLine;
  u8                      m_waitFrame;
  u8                      m_countFrame;
  u8                      m_targetPaneIdx;
  u8                      m_hidePaneIdx;
  u8                      m_lineFeedSeq;
  u8                      m_finishFrame;
  u8                      m_finishFrameCount;
  u8                      m_putCharCountPerFrame;
  u8                      m_waitFrameOrg;
  u8                      m_putCharCountPerFrameOrg;
  Sequence                m_seq;
  int                     m_readIndex;
  int                     m_readLineIndex;
  int                     m_strLength;
  bool                    m_bReadLineDone;
  bool                    m_bReadTag;
  bool                    m_bForceQuit;
  ProcMode                m_procMode;
  TagInfo                 m_readTag;
  TagInfo                 m_prevTag;

  bool                    m_bAvirable;
  bool                    m_bPaneSetup;
  bool                    m_bAtOnceMode;
  bool                    m_bTagProcBreak;
  PutState                m_putState;

  void    setupReader( void );
  bool    isReadEnd( void ) const;
  const gfl::str::STRCODE*  readNext( int* strLen );
  bool    getLatestTagCode( TagInfo* dst ) const;

  void    startLineFeed( void );
  bool    progLineFeed( void );
  bool    checkFinish( void );
  bool    checkFinishInput( void );
  bool    checkFinishFrame( void );
  void    checkNextLineColor( void );
  Sequence    proc_checkNextLine( void );
  bool    isPauseTypeClear( void ) const;
  bool    checkReleasePause( void );
  void    setNextSequence( Sequence nextSeq );
  bool    isPassed1stFrame( void ) const;


  Result  procCore( SubResult* subResult );
  Result  procSub_Done( SubResult* subResult );
  Result  procSub_Pause( SubResult* subResult );
  Result  procSub_LineFeed( SubResult* subResult );
  Result  procSub_Clear( SubResult* subResult );
  Result  procSub_Put( SubResult* subResult );
  Result  procSub_TagProcForNewLine( SubResult* subResult );

  static const gfl::str::STRCODE* readTag( const gfl::str::STRCODE* sp, TagInfo* dst );
};



} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_STRWIN_H__

//=============================================================================
/**
 * @file    MessageWindow.h
 * @brief   ��������Ή����b�Z�[�W�E�B���h�E
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.05.20
 *
 * @li  sango��xy_msgWin.h���ڐA
 */
//=============================================================================
#if !defined( MESSAGEWINDOW_H_INCLUDED )
#define MESSAGEWINDOW_H_INCLUDED
#pragma once

#include <ui/include/gfl2_UI.h>
#include <str/include/gfl2_Str.h>

#include "../../System/include/PokemonLanguage.h"

//#include <gfl_Common.h>
//#include <str/include/gfl2_Str.h>
//#include <gfl_UI.h>
//#include <Print/include/PrintSystem.h>
//#include "arc_def.h"

//#include "savedata/sv_config.h"


GFL_NAMESPACE_BEGIN( print )

  class MsgCallback
  {
  public:
    /**
     *  �^�O�̃R�[���o�b�N���Ă΂��ƌĂ΂�܂��B
     *  �߂�l��ture�̌��胁�b�Z�[�W�̍X�V���~�߂܂��B
     *
     *  @param[in]   arg    ���b�Z�[�W�Ŏw�肳�ꂽ����������܂��B
     *
     *  @retval   bool      true�̊ԃ��b�Z�[�W�̍X�V�͎~�܂�܂��B
     */
    virtual bool UpdateMsgCallback( u16 arg ) = 0;
  };


  class MessageWindow;

  //----------------------------------------------------------------------------
  /**
   *  �^�O�v���Z�b�T�N���X
   */
  //----------------------------------------------------------------------------
  class FontTagProc : public nw::font::TagProcessorBase<gfl2::str::STRCODE> {
  public:
    FontTagProc()
      : m_defaultColorTop(0,0,0,255)
      , m_defaultColorBottom(0,0,0,255)
      , m_defaultScale(0.0f)
      , m_bColorChanged(false)
      , m_msgWin(NULL)
    {};

    ~FontTagProc(){};

    friend class MessageWindow;

    /**
     *  �^�O�����ɂ��`��F���ς����Ă��邩�ǂ�������
     *
     *  @param[out]   top       �`��F���ς����Ă���ꍇ�A�O���f�[�V�����J�n�F������
     *  @param[out]   top       �`��F���ς����Ă���ꍇ�A�O���f�[�V�����I�[�F������
     *
     *  @retval   bool      �`��F���ς����Ă���ꍇ�� true
     */
    bool IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom ) const;


    nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
        Process( u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext );
    nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
        CalcRect( nw::ut::Rect * pRect, u16 code, nw::font::PrintContext<gfl2::str::STRCODE>* pContext );

    void SetWindowWidth( u32 width ){ m_windowWidth = width; }
    void SetDefaultColor( const nw::ut::Color8& top, const nw::ut::Color8& bottom ){
      m_defaultColorTop    = top;
      m_defaultColorBottom = bottom;
    }

    //���b�Z�[�W���Z�b�g����O��1��Ă�œ����p�����[�^���Z�b�g
    void ResetParam(void);
    void SetMsgWin( MessageWindow *MessageWindow ){m_msgWin = MessageWindow;}

    f32 CalcAreaLimitWidth( f32 curFontWidth, u16 width_value );
    f32 CalcAreaStrWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth );

  private:
    nw::ut::Color8     m_defaultColorTop;
    nw::ut::Color8     m_defaultColorBottom;
    nw::ut::Color8     m_currentColorTop;
    nw::ut::Color8     m_currentColorBottom;
    u32                m_windowWidth;
    f32                m_defaultScale;
    bool               m_bColorChanged;

    MessageWindow             *m_msgWin;
    const gfl2::str::STRCODE      *m_topStrPos;  //�P���^�O����p

    nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
      process_GeneralCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp );
    nw::font::TagProcessorBase<gfl2::str::STRCODE>::Operation
      process_StreamCtrl( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, u32 tagIdx, const gfl2::str::STRCODE* sp );

    f32 calcStrWidth( nw::font::PrintContext<gfl2::str::STRCODE>* pContext, const gfl2::str::STRCODE* sp );
    u16 calcStrRatioValue( const gfl2::str::STRCODE* sp, nw::font::PrintContext<gfl2::str::STRCODE>* pContext );


    void setTextColor( nw::font::TextWriterBase<gfl2::str::STRCODE>* pWriter , const nw::ut::Color8 &changeCol );
    void resetTextColor(nw::font::TextWriterBase<gfl2::str::STRCODE>* pWriter );

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
    f32 CalcCharWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth );

  };

  //----------------------------------------------------------------------------
  /**
   *  ������E�B���h�E�N���X
   */
  //----------------------------------------------------------------------------
  class MessageWindow : public gfl2::str::StrWin {
    GFL_FORBID_COPY_AND_ASSIGN(MessageWindow);

  public:
    MessageWindow(
      const gfl2::ui::DeviceManager * uiDevice,
      gfl2::lyt::LytWk * lytWk,
      u16 numLines,
      int waitFrames,
      gfl2::heap::HeapBase * pHeap,
      u32 procCharCount=1 );

    MessageWindow(
      const gfl2::ui::DeviceManager * uiDevice,
      gfl2::lyt::LytWk * lytWk,
      u16 numLines,
      gfl2::heap::HeapBase* pHeap ,
      bool is60Fps = false );

    ~MessageWindow();

    //--------------------------------------------------------------------------------------
    /**
     * @brief  �\�����C�������i�P��Ăяo�����Ƃɏ������X�V���܂��j
     *
     * @retval   StrWin::Result   ���ʃR�[�h
     */
    //--------------------------------------------------------------------------------------
    virtual gfl2::str::StrWin::Result Proc( void );

    /**
     *  �L�[�҂�����ŁA�^�b�`�p�l���̃g���K���͂��L���ƌ��Ȃ����ǂ�����؂�ւ��i�f�t�H���g�� true = ���Ȃ��j
     */
    void SetTPReleaseMode( bool flag );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �y�C���̒ǉ�
     *
     * @param paneIdx �ǉ�����y�C���C���f�b�N�X
     */
    /* -------------------------------------------------------------------------*/
    virtual void AddLinePaneIdx( u32 paneIdx );
    virtual void AddLinePaneIdxPtr( gfl2::lyt::LytTextBox * pTextBoxPain );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ��������Z�b�g����
     *
     * @param str    �Z�b�g���镶���o�b�t�@
     * @param langId �\������ID(���k���̂���)sv_config���̌���ID������
     */
    /* -------------------------------------------------------------------------*/
    virtual void SetString( const gfl2::str::StrBuf& str );
    void SetString( const gfl2::str::StrBuf& str , System::MSGLANGID langId );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief   �e���C���̕��������v���Z�o��A�ő�l��Ԃ�
     *
     * @return  ������
     *
     * @note    SetString��ɌĂяo���Ă�������
     */
    /* -------------------------------------------------------------------------*/
    int GetStringWidthMax( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �t�H���g�̍������擾����
     *
     * @return �t�H���g�̍���
     */
    /* -------------------------------------------------------------------------*/
    int GetFontHeight( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �ŏ��ɓo�^�����y�C�����̍X�V
     * @note  AddLinePaneIdx��Ƀf�[�^�������������s�������ꍇ�Ɏg�p���ĉ�����
     */
    /* -------------------------------------------------------------------------*/
    void RefreshFirstPane( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �ʒu�}�g���b�N�X���ď���������
     */
    /* -------------------------------------------------------------------------*/
    void RefreshInitialMtx( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �L�[���͉ېݒ�
     * @note  true�ňꎞ�L�[���͖���
     */
    /* -------------------------------------------------------------------------*/
    void SetSuspendKey( bool flg );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief �����L�[����(+���b�Z�[�W�u�[�X�g����)
     *        �ʐM���Ȃǂɗ��p���Ă��������B
     * @note  true�Ŏ�������
     */
    /* -------------------------------------------------------------------------*/
    void SetMessageAutoFlg( bool flg );

    //--------------------------------------------------------------------------
  /**
   *	@brief  �����L�[���������1�x�����s��
   */
  //--------------------------------------------------------------------------
    void SetMessageAutoOnce( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ���b�Z�[�W���x�̐ݒ�(config��)
     *        �f�t�H���g�l�͕ύX���܂���B
     *
     * @param  savedata::Config::MSG_SPEED spd ���b�Z�[�W���x
     */
    /* -------------------------------------------------------------------------*/
#if 0 // @todo savedata�҂�
    void SetMessageSpeed( savedata::Config::MSG_SPEED spd , bool is60Fps = false );
#else
    void SetMessageSpeed( int spd , bool is60Fps = false );
#endif

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ���b�Z�[�W�R�[���o�b�N�̐ݒ�
     *
     * @param  savedata::Config::MSG_SPEED spd ���b�Z�[�W���x
     */
    /* -------------------------------------------------------------------------*/
    void SetMessageCallback( MsgCallback *msgCb ){m_msgCbClass = msgCb;}

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ����̗L�������̐ݒ�
     *
     * @param  bool enableKey   �L�[����̗L������
     * @param  bool enableTouch �^�b�`����̗L������
     */
    /* -------------------------------------------------------------------------*/
    void SetEnableCtrl( bool enableKey , bool enableTouch ){m_enableCtrlTouch = enableTouch;m_enableCtrlKey = enableKey;}

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ���b�Z�[�W�u�[�X�g�̗L�������̐ݒ�
     *
     * @param  bool isEnable   �L�������̐ݒ�
     */
    /* -------------------------------------------------------------------------*/
    void SetEnableMessageBoost( bool isEnable ){m_enalbeMsgBoost = isEnable;}

    //-----------------------------------------------------------------------
    /**
     *  @brief      �s�X�V���ɌĂ΂��
     */
    //-----------------------------------------------------------------------
    virtual void RefreshLine(void);

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
    virtual bool CheckTagProcBreak( const TagInfo& tagInfo );

    //TagProc�p
    void SetWaitCnt( int cnt );
    void CallMessgaeCallback(u16 arg);

  protected:
    virtual bool IsPauseRelease( void );
    virtual bool IsFinishInput( void );
    virtual bool IsColorChanged( nw::ut::Color8* top, nw::ut::Color8* bottom );

  private:
    const gfl2::ui::DeviceManager * m_uiDevice;
    FontTagProc                     m_tagProc;
    bool                            m_bFirstPaneAdded;
    const nw::font::Font*           m_font;
    f32                             m_paneFontWidth;
    f32                             m_basePaneFontWidth;
    int                             m_pane_num;
    bool                            m_bSuspendKey;
    bool                            m_bCommAutoFlag;
    bool                            m_bAutoFlagOnce;

    bool  m_enableCtrlKey;
    bool  m_enableCtrlTouch;
    bool  m_enalbeMsgBoost;

    //TagProc�p
    int   m_waitCnt;
    MsgCallback *m_msgCbClass;
    bool  m_isWaitCallback;
    u16   m_callbackArg;

    u16   m_is60Fps;
    
    typedef enum
    {
      USER_INPUT_NO,    //���͖�
      USER_INPUT_UI,    //���[�U�[�̓���
      USER_INPUT_AUTO,  //����

      USER_INPUT_MAX,
    }USER_INPUT_TYPE;
    
    USER_INPUT_TYPE    checkUserInput( void );


  public:
    /* -------------------------------------------------------------------------*/
    /**
     * @brief �f�t�H���g���b�Z�[�W���x�̐ݒ�
     *
     * @param  savedata::Config::MSG_SPEED spd ���b�Z�[�W���x
     */
    /* -------------------------------------------------------------------------*/
#if 0 // @todo savedata�҂�
    static void SetDefaultMessageSpeed( savedata::Config::MSG_SPEED spd );
  private:
    static savedata::Config::MSG_SPEED mDefaultMsgSpeed;
#endif

    enum {
      XY_MSG_FADE_FRAMES = 0,
      XY_MSG_SCROLL_FRAMES = 1,
    };
  };

GFL_NAMESPACE_END( print )

#endif  // MESSAGEWINDOW_H_INCLUDED

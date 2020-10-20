#if !defined( __DEBUGKEYBOARD_H__ )
#define __DEBUGKEYBOARD_H__
//=============================================================================================
/**
 * @file  DebugKeyboard.h
 * @brief ポケモンXY デバッグキーボード
 * @author  taya
 *
 * @date  2011.11.14
 */
//=============================================================================================
#pragma once

#if PM_DEBUG


#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <str/include/gfl2_Str.h>
#include <GameSys/include/GameProc.h>
#include <System/include/nijiAllocator.h>
#include <System/include/AppRenderingPipeline.h>

namespace debug {

class KeyboardProc : public GameSys::GameProc {
  GFL_FORBID_COPY_AND_ASSIGN( KeyboardProc );

public:
  KeyboardProc( void );
  ~KeyboardProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  /**
   *  公開定数
   */
  enum {
    MATCH_NONE = -1,
  };
	

  /**
   *  入力文字種
   */
  enum InputMode {
    INPUT_NUMBER,
    INPUT_ALPHA,
    INPUT_HIRAGANA,
    INPUT_KATAKANA,
  };

  //=============================================================================================
  /**
   * 最低限必要なパラメータのセットアップ
   *
   * ! Proc 作成直後にまずこれを呼んでください
   * ! 文字列補間が必要な場合は、続けて SetSearchWords() を呼んでください。
   *
   * @param[in]   heapSys         システムヒープハンドル
   * @param[in]   heapDev         デバイスヒープハンドル
   * @param[io]   pStr            入力文字列を受け取るバッファ（初期文字列としても利用します）
   * @param[in]   strMaxLen (=16) 入力可能な最大文字数
   * @param[in]   searchMax (=0)  メッセージデータの探索最大数（0はメッセージすべて、0以外は探索最大)
   *
   */
  //=============================================================================================
  inline void  SetupParam( gfl2::heap::HeapBase* heapSys,  gfl2::heap::HeapBase* heapDev, gfl2::str::StrBuf* pStr, u32 strMaxLen=16, u32 searchMax = 0 )
  {
    m_sysHeapBase = heapSys;
    m_devHeapBase = heapDev;
    m_strBuf = pStr;
    m_strMaxLen = strMaxLen;
    m_wordSearchMax = searchMax;
  }

  //=============================================================================================
  /**
   *  補間用文字列を設定
   *
   *  ※ 補間用文字列のうちどれかと完全一致する状態で入力画面を終えた時、index にはその文字Indexが入ります。（'フシギダネ"なら 1 です）
   *     全ての補間用文字列と一致しない状態で入力画面を終えた場合 MATCH_NONE(-1) が入ります。
   *     expand_searchにtrueをセットすることで、部分一致でも候補に上げられるようになります
   */
  //=============================================================================================
  void  SetSearchWords( gfl2::str::MsgData* msgData, int* index, bool expand_search = false );   /// msgData から補間用の単語を探す
  void  SetSearchWords( u32 dataID, int* index, bool expand_search = false );                   /// ARCID_MESSAGEの中から、指定 dataID のファイルを探し、補間用の単語データにする

  //=============================================================================================
  /**
   * 初期の入力文字種を指定
   *
   * @note    デフォルトの初期入力文字種はひらがなになります。
   * @param   mode    InputModeを指定します
   * @param   hankaku falseなら全角、trueなら半角モードになります
   *                  ただし、ひらがな・カタカナを指定した場合は強制的に全角になります
   */
  //=============================================================================================
  void SetDefaultInputMode(InputMode mode, bool hankaku = true);

  //=============================================================================================
  /**
   * 既に作られているレイアウトシステムを使いたい場合はこれを呼んでセットしてください
   *
   * @param   lytSys
   */
  //=============================================================================================
  void  SetLayoutSystem( gfl2::lyt::LytSys* lytSys )
  {
    m_lytSys = lytSys;
  }


private:

  /**
    * 定数
    */
  enum {
    DISP_SEARCH_WORD_MAX = 5,   ///< 表示する補間候補の最大数
  };

  /**
   *  状態
   */
  enum State {
    STATE_FREE = 0,
    STATE_1ST_TOUCH,
    STATE_NORMAL_RELEASE,
    STATE_FLICK_LOW,
    STATE_FLICK_HIGH,
    STATE_BACKSPACE,
    STATE_KANA_FUNC,
    STATE_ALPHA_FUNC,
    STATE_EXIT,
  };

  /**
   *  ボタンID
   */
  enum ButtonID {
    BTN_NULL = 0,
    BTN_MODE_NUMBER,
    BTN_MODE_ALPHA,
    BTN_MODE_HIRAGANA,
    BTN_MODE_KATAKANA,
    BTN_BACKSPACE,
    BTN_SPACE,
    BTN_OK,
    BTN_LET0,
    BTN_LET1,
    BTN_LET2,
    BTN_LET3,
    BTN_LET4,
    BTN_LET5,
    BTN_LET6,
    BTN_LET7,
    BTN_LET8,
    BTN_LET9,
    BTN_LET10,
    BTN_LET11,
    BTN_MAX,
  };

  enum ButtonColor {
    BTNCOLOR_LIGHT,
    BTNCOLOR_MIDDLE,
    BTNCOLOR_DARK,

    BTNCOLOR_MAX,
  };

  /**
   *  定数
   */
  enum {
    BTN_PER_LET     = 5,    ///< ひとつのボタンに割り当てられている文字数最大
    BTN_CAPTION_LEN = 8,     ///< キャプション文字数最大
    BTN_LET_MAX     = 12,    ///< 文字ボタンの総数
    FLICK_LET_MAX   = 5,
  };

  //----------------------------------------------------------
  // メンバ変数
  //----------------------------------------------------------
  gfl2::heap::HeapBase*        m_sysHeapBase;
  gfl2::heap::HeapBase*        m_devHeapBase;
  gfl2::heap::HeapBase*        m_devHeap;
#if 0
  gfl::grp::GraphicsSystem*   m_graphicSys;
  gfl2::heap::NwAllocator*     m_sysAllocator;
  gfl2::heap::NwAllocator*     m_devAllocator;
#endif
  gfl2::str::StrBuf*           m_strBuf;
  gfl2::str::StrBuf*           m_strBufTmp;
  gfl2::str::StrBuf*           m_strBufToUpper;
  gfl2::str::StrBuf*           m_strBufWork1;
  gfl2::str::StrBuf*           m_strBufWork2;
  u32                         m_strMaxLen;
  bool                        m_bHankaku;
  bool                        m_bLytSysCreateSelf;

  AppRenderingPipeLine*        m_renderingPipeLine;
  gfl2::heap::NwAllocator*     m_sysAllocator;
  gfl2::heap::NwAllocator*     m_devAllocator;
  gfl2::lyt::LytSys*           m_lytSys;
  gfl2::lyt::LytWk*            m_lytWk;
  gfl2::lyt::LytMultiResID     m_lytResID;
  void*                        m_lytDataBuffer;
  bool                         m_isDrawEnable;

#if 0
  gfl2::lyt::LytSys*      m_lytSys;
  gfl2::lyt::LytWk*       m_lytWk;
#endif
  gfl2::lyt::LytPane*     m_cursorPane;
  gfl2::lyt::LytTextBox*  m_stringPane;
#if 0
  u32                         m_lytResID;
#endif

  gfl2::str::MsgData*          m_wordComp;
  int                         m_wordCompDataID;
  int                         *m_wordIndexOut;
  int                         m_wordSearchMax;  ///< メッセージDATの何番までを探索するか
  int                         m_wordStartIdx;

  gfl2::ui::DeviceManager*     m_uiMan;
  gfl2::ui::TouchPanel*        m_uiTp;
  gfl2::ui::VectorDevice*      m_uiKey;
  gfl2::ui::Button*            m_uiButton;
  State                       m_state;
  int                         m_seq;
  InputMode                   m_inputMode;
  ButtonID                    m_hitButton;
  ButtonID                    m_releasedButton;
  ButtonID                    m_prevChangeColorButton;
  int                         m_touchX;
  int                         m_touchY;
  u64                         m_stateTick;
  int                         m_hitContCount;
  bool                        m_bConnectFree;
  bool                        m_bWordDisped;

  bool                        m_bExpandSearch;

  nw::ut::Color8              m_colorNormal;
  nw::ut::Color8              m_colorSelect;
  gfl2::lyt::LytPane*     m_btnMaskPane[ BTNCOLOR_MAX ];

  gfl2::lyt::LytPane*     m_wordBGPane[ DISP_SEARCH_WORD_MAX ];
  gfl2::lyt::LytTextBox*  m_wordTextPane[ DISP_SEARCH_WORD_MAX ];

  gfl2::lyt::LytPane*     m_flickBtnPane[ FLICK_LET_MAX ];
  gfl2::lyt::LytPane*     m_flickSelBtnPane[ FLICK_LET_MAX ];
  gfl2::lyt::LytTextBox*  m_flickLetPane[ FLICK_LET_MAX ];

  InputMode m_default_inputmode;
  bool m_default_hankaku;


  /**
   *  文字ボタンステータス
   */
  typedef struct {
    u32         paneID;
    wchar_t     letter[ BTN_PER_LET + 1 ];
    wchar_t     caption[ BTN_CAPTION_LEN ];
    wchar_t     flick_letter[ FLICK_LET_MAX + 1 ];
  }BTN_PARAM;

  static const BTN_PARAM ButtonParamNumber[];
  static const BTN_PARAM ButtonParamAlphaL[];
  static const BTN_PARAM ButtonParamAlphaS[];
  static const BTN_PARAM ButtonParamHira[];
  static const BTN_PARAM ButtonParamKata[];
  static const BTN_PARAM ButtonParamNumber_Half[];
  static const BTN_PARAM ButtonParamAlphaL_Half[];

  static const BTN_PARAM* ButtonParamTable[];
  static const u16       ButtonStrPaneID[];
  static const u16       WordBoxPaneID[DISP_SEARCH_WORD_MAX][2];
  static const u16       FlickPaneIdx[FLICK_LET_MAX][3];
  static const u16       CaptionPaneID[];


  /**
   *
   */
  typedef struct {
      s16  x;
      s16  y;
      s16  width;
      s16  height;
      ButtonColor  baseColor;
  }BTN_TP_POS;
  static const BTN_TP_POS  BtnPosTable[];

  static const u16  FlickLetPaneIdx[];



  // メンバ関数
  void createAllocator( void );
  void deleteAllocator( void );
  void init2D( void );
  void quit2D( void );
  void createRenderingPipeLine( void );
  void deleteRenderingPipeLine( void );
  void drawLyt( void );

  bool proc_Free( void );
  void proc_1st_touch( void );
  void proc_normal_release( void );
  void proc_flick_low( void );
  void proc_flick_high( void );
  void proc_back_space( void );
  void proc_input_func( void );

  void change_state( State next_state );
  u64  getStateMilliSec( void );
  void resetStateTick( void );


  ButtonID  checkTouchButton( bool bCheckHold = false );
  ButtonID  checkTouchButtonByPos( int x, int y );
  void      changeInputMode( InputMode next_mode );
  void      tpPosToLytPos( const BTN_TP_POS* tpPos, gfl2::math::VEC3* pos );
  void      setButtonCaption( gfl2::lyt::LytTextBox* texPane, const wchar_t* string, bool debugFlag=false );
  void      switchLowFlickWin( bool bVisible );
  void      changeButtonColor( ButtonID btnID, ButtonColor colorID );
  void      changeButtonColorCore( ButtonID btnID, ButtonColor colorID );
  void      changeButtonAllFreeColor( void );
  void      resetButtonColorMask( void );
  void      updateString( bool bFirstTime=false );
  void      putChar( wchar_t charCode );
  void      changeChar( wchar_t  charCode );
  void      putSpace( void );
  void      backSpace( bool bUpdate=true );
  void      renewZenHanCaption( void );
  u32       checkFlickDirection( ButtonID btnID, int tpX, int tpY );


  void      setupFlickPanes( void );
  void      drawFlickButtonFull( ButtonID btnID );
  void      drawFlickButtonSingle( ButtonID btnID, uint idx );
  void      hideFlickButton( void );
  void      updateFlickButtonSelect( u32 letIdx );
  u32       checkTouchFlickButtonIdx( int x, int y );
  void      calcFlickBtnPos( u32 idx, const gfl2::math::VEC3& centerPos, gfl2::math::VEC3* dst );


  void      setupSearchWordPanes( void );
  void      dispSearchWords( void );
  bool      isMatchComplete( const gfl2::str::StrBuf* input, const gfl2::str::StrBuf* target );
  bool      isMatchExpand( const gfl2::str::StrBuf* input, const gfl2::str::StrBuf* target );
  int       setWordPane( const gfl2::str::StrBuf* word, u32 paneIdx, int dispX );
  void      hideWordPane( u32 paneIdx );
  void      to_Katakana( const gfl2::str::StrBuf* from, gfl2::str::StrBuf* to );
  int       calcWordWidth( const gfl2::str::StrBuf* word, gfl2::lyt::LytTextBox* pane );
  void      checkTouchWord( void );
  bool      checkTouchStrPane( void );
  void      hideAllSearchWords( void );
  int       getSearchWordIndex( void );
  int       GetMessageDatCount(void);


  uint      getNextLetterIdx( ButtonID btnID, u32 contCount );
  wchar_t   getLetterCode( ButtonID btnID, u32 contCount );
  wchar_t   getFlickLetterCode( ButtonID btnID, u32 contCount );
  wchar_t   getEndCode( void );
  wchar_t   getFuncNextCode( wchar_t code );
  const  BTN_PARAM*  getButtonParamTable( InputMode mode );





};  // class KeyboardProc

} // end of namespace

#endif // #if PM_DEBUG

#endif // __DEBUGKEYBOARD_H__

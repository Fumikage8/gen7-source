//=============================================================================
/**
 * @file    MessageWindow.h
 * @brief   文字送り対応メッセージウィンドウ
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.05.20
 *
 * @li  sangoのxy_msgWin.hを移植
 */
//=============================================================================
#if !defined( MESSAGEWINDOW_H_INCLUDED )
#define MESSAGEWINDOW_H_INCLUDED
#pragma once

#include <ui/include/gfl2_UI.h>
#include <str/include/gfl2_Str.h>

#include <System/include/PokemonLanguage.h>

//#include <gfl_Common.h>
//#include <str/include/gfl2_Str.h>
//#include <gfl_UI.h>
//#include <Print/include/PrintSystem.h>
//#include "arc_def.h"

//#include "savedata/sv_config.h"

#include "Savedata/include/ConfigSave.h"

GFL_NAMESPACE_BEGIN( print )

  class MsgCallback
  {
  public:
    /**
     *  タグのコールバックが呼ばれると呼ばれます。
     *  戻り値がtureの限りメッセージの更新を止めます。
     *
     *  @param[in]   arg    メッセージで指定された引数が入ります。
     *
     *  @retval   bool      trueの間メッセージの更新は止まります。
     */
    virtual bool UpdateMsgCallback( u16 arg ) = 0;
  };


  class MessageWindow;

  //----------------------------------------------------------------------------
  /**
   *  タグプロセッサクラス
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
     *  タグ処理により描画色が変えられているかどうか判定
     *
     *  @param[out]   top       描画色が変えられている場合、グラデーション開始色が入る
     *  @param[out]   top       描画色が変えられている場合、グラデーション終端色が入る
     *
     *  @retval   bool      描画色が変えられている場合は true
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

    //メッセージをセットする前に1回呼んで内部パラメータリセット
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
    const gfl2::str::STRCODE      *m_topStrPos;  //単発タグ制御用

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
     * @brief 文字の幅を取得する
     *
     * @param sp            取得したい文字をさしているポインタ
     * @param font          フォント
     * @param curFontWidth  フォント幅
     *
     * @return 文字幅
     */
    /* -------------------------------------------------------------------------*/
    f32 CalcCharWidth( const gfl2::str::STRCODE* sp, const nw::font::Font* font, f32 curFontWidth );

  };

  //----------------------------------------------------------------------------
  /**
   *  文字列ウィンドウクラス
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
     * @brief  表示メイン処理（１回呼び出すごとに処理を更新します）
     *
     * @retval   StrWin::Result   結果コード
     */
    //--------------------------------------------------------------------------------------
    virtual gfl2::str::StrWin::Result Proc( void );

    /**
     *  キー待ち判定で、タッチパネルのトリガ入力も有効と見なすかどうかを切り替え（デフォルトは true = 見なす）
     */
    void SetTPReleaseMode( bool flag );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief ペインの追加
     *
     * @param paneIdx 追加するペインインデックス
     */
    /* -------------------------------------------------------------------------*/
    virtual void AddLinePaneIdx( u32 paneIdx );
    virtual void AddLinePaneIdxPtr( gfl2::lyt::LytTextBox * pTextBoxPain );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief 文字列をセットする
     *
     * @param str    セットする文字バッファ
     * @param langId 表示言語ID(幅縮小のため)sv_config内の言語IDを入れる
     */
    /* -------------------------------------------------------------------------*/
    virtual void SetString( const gfl2::str::StrBuf& str );
    void SetString( const gfl2::str::StrBuf& str , System::MSGLANGID langId );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief   各ラインの文字幅合計を算出後、最大値を返す
     *
     * @return  文字幅
     *
     * @note    SetString後に呼び出してください
     */
    /* -------------------------------------------------------------------------*/
    int GetStringWidthMax( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief フォントの高さを取得する
     *
     * @return フォントの高さ
     */
    /* -------------------------------------------------------------------------*/
    int GetFontHeight( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief 最初に登録したペイン情報の更新
     * @note  AddLinePaneIdx後にデータを書き換えを行いたい場合に使用して下し亜
     */
    /* -------------------------------------------------------------------------*/
    void RefreshFirstPane( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief 位置マトリックスを再初期化する
     */
    /* -------------------------------------------------------------------------*/
    void RefreshInitialMtx( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief キー入力可否設定
     * @note  trueで一時キー入力無効
     */
    /* -------------------------------------------------------------------------*/
    void SetSuspendKey( bool flg );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief 自動キー送り(+メッセージブースト無効)
     *        通信時などに利用してください。
     * @note  trueで自動送り
     */
    /* -------------------------------------------------------------------------*/
    void SetMessageAutoFlg( bool flg );

    //--------------------------------------------------------------------------
  /**
   *	@brief  自動キー送りを次の1度だけ行う
   */
  //--------------------------------------------------------------------------
    void SetMessageAutoOnce( void );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief メッセージ速度の設定(config版)
     *        デフォルト値は変更しません。
     *
     * @param  Savedata::ConfigSave::MSG_SPEED spd メッセージ速度
     */
    /* -------------------------------------------------------------------------*/
    void SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd , bool is60Fps = false );

    /* -------------------------------------------------------------------------*/
    /**
     * @brief メッセージコールバックの設定
     *
     * @param  savedata::Config::MSG_SPEED spd メッセージ速度
     */
    /* -------------------------------------------------------------------------*/
    void SetMessageCallback( MsgCallback *msgCb ){m_msgCbClass = msgCb;}

    /* -------------------------------------------------------------------------*/
    /**
     * @brief 操作の有効無効の設定
     *
     * @param  bool enableKey   キー操作の有効無効
     * @param  bool enableTouch タッチ操作の有効無効
     */
    /* -------------------------------------------------------------------------*/
    void SetEnableCtrl( bool enableKey , bool enableTouch ){m_enableCtrlTouch = enableTouch;m_enableCtrlKey = enableKey;}

    /* -------------------------------------------------------------------------*/
    /**
     * @brief メッセージブーストの有効無効の設定
     *
     * @param  bool isEnable   有効無効の設定
     */
    /* -------------------------------------------------------------------------*/
    void SetEnableMessageBoost( bool isEnable ){m_enalbeMsgBoost = isEnable;}

    //-----------------------------------------------------------------------
    /**
     *  @brief      行更新時に呼ばれる
     */
    //-----------------------------------------------------------------------
    virtual void RefreshLine(void);

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
    virtual bool CheckTagProcBreak( const TagInfo& tagInfo );

    //TagProc用
    void SetWaitCnt( int cnt );
    void CallMessgaeCallback(u16 arg);

    //-----------------------------------------------------------------------
    /**
     *  @brief    改行のＳＥを交換する
     */
    //-----------------------------------------------------------------------
    inline void ChangeLineBreakSE(u32 soundID){ m_LineBreakSE=soundID; };


    //-----------------------------------------------------------------------
    /**
     *  @brief    文字列がウィンドウの幅に収まるかチェックする
     */
    //-----------------------------------------------------------------------
    bool CheckStringWidth( const gfl2::str::StrBuf& str );




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
    u32 m_LineBreakSE;

    //TagProc用
    int   m_waitCnt;
    MsgCallback *m_msgCbClass;
    bool  m_isWaitCallback;
    u16   m_callbackArg;

    u16   m_is60Fps;
    
    typedef enum
    {
      USER_INPUT_NO,    //入力無
      USER_INPUT_UI,    //ユーザーの入力
      USER_INPUT_AUTO,  //自動

      USER_INPUT_MAX,
    }USER_INPUT_TYPE;
    
    USER_INPUT_TYPE    checkUserInput( void );


  public:
    /* -------------------------------------------------------------------------*/
    /**
     * @brief デフォルトメッセージ速度の設定
     *
     * @param  savedata::Config::MSG_SPEED spd メッセージ速度
     */
    /* -------------------------------------------------------------------------*/
    static void SetDefaultMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd );

#if PM_DEBUG
    static void SetDebugMessageSpeedUseFlag( bool flag );
    static bool GetDebugMessageSpeedUseFlag( void );
    static void SetDebugMessageSpeed( u32 waitFrame, u32 putCount );
#endif

  private:
    static Savedata::ConfigSave::MSG_SPEED mDefaultMsgSpeed;

#if PM_DEBUG
    static bool mDebugUseFlag;
    static u32  mDebugWaitFrame;
    static u32  mDebugPutCount;
#endif

    enum {
      XY_MSG_FADE_FRAMES = 0,
      XY_MSG_SCROLL_FRAMES = 1,
    };
  };

GFL_NAMESPACE_END( print )

#endif  // MESSAGEWINDOW_H_INCLUDED

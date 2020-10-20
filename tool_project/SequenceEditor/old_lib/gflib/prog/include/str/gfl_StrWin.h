#ifndef __GFL_STRWIN_H__
#define __GFL_STRWIN_H__
//===========================================================================
/**
 * @file    gfl_StrWin.h
 * @brief   GAMEFREAK Library String System ( 文字表示ウィンドウ )
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
 *  文字列ウィンドウ基底クラス
 */
class StrWin {

  GFL_FORBID_COPY_AND_ASSIGN( StrWin );


public:

  /**
   *  結果コード
   */
  enum Result {
    RES_DONE,       ///< 表示処理が最後まで終了した
    RES_CONTINUE,   ///< 表示処理中
    RES_PAUSE,      ///< ポーズ後、指示待ち
    RES_LINE_FEED,  ///< ポーズ解除の処理中
    RES_FINISH,     ///< 表示処理の終了後、設定されたフィニッシュ待ちまで終了
  };

  /**
   *  フィニッシュ待ちタイプ
   */
  enum FinishType{
    FINISH_NONE,    ///< フィニッシュ待ちなし（デフォルト動作）
    FINISH_USER,    ///< 何らかのユーザー入力があったら消滅
    FINISH_TIMER,   ///< 一定時間（呼び出しフレーム数）経過したら消滅
    FINISH_HYB,     ///< FINISH_USER & FINISH_TIMER = どちらかの条件で消滅
    
    NUM_FINISH_TYPES,
  };

  /**
   *  改ページタイプ（改ページ指示待ち中、判定可能）
   */
  enum PauseType {
    PAUSETYPE_NONE = 0, ///< 改ページ指示待ち中ではない
    PAUSETYPE_CLEAR,    ///< 改ページ指示後、ウィンドウ内容をクリアする
    PAUSETYPE_FEED,     ///< 改ページ指示後、テキストを１行送る
  };


  //=============================================================================================
  /**
   * コンストラクタ
   *
   * @param[in]   lytWk         表示に使用するレイアウトワークポインタ
   * @param[in]   numLines      表示行数
   * @param[in]   waitFrames    １文字出力ごとのwaitフレーム値
   * @param[in]   pHeap         ヒープハンドル
   * @param[in]   pTagProc      タグプロセッサ（不要ならNULLでも可）
   * @param[in]   putCharCount  １度に出力する文字数
   */
  //=============================================================================================
  StrWin( gfl::grp::g2d::LytWk* lytWk, u16 numLines, u32 waitFrames, gfl::heap::HeapBase* pHeap,
      TagProcBase* pTagProc = NULL, u32 putCharCount=1 );
  virtual ~StrWin();


  //----------------------------------------------------------------------
  /**
   * @brief   文字列表示用ペインIndexを設定（表示行数分、呼び出すこと）
   *
   * @param[in]   paneIdx     ペインIndex
   *
   */
  //----------------------------------------------------------------------
  virtual void     AddLinePaneIdx( u32 paneIdx );

  //----------------------------------------------------------------------
  /**
   * @brief   文字列表示用ペインIndexを最低文字数と同時に設定（表示行数分、呼び出すこと）
   *
   * @param[in]   paneIdx     ペインIndex
   * @param[in]   strMinLen   最低文字数
   *
   */
  //----------------------------------------------------------------------
  virtual void     AddLinePaneIdxWithStrMinLen( u32 paneIdx, u32 strMinLen );

  //----------------------------------------------------------------------
  /**
   * @brief   文字列をセットする
   *
   * @param[in]   str      表示する文字列
   *                     （この関数を呼んだ後はクラス内部で内容を保持するので、
   *                      元のインスタンスは破棄しても構いません）
   *
   */
  //----------------------------------------------------------------------
  virtual void     SetString( const StrBuf& str );

  //--------------------------------------------------------------------------------------
  /**
   * @brief  表示メイン処理（１回呼び出すごとに処理を更新します）
   *
   * @retval   StrWin::Result   結果コード
   */
  //--------------------------------------------------------------------------------------
  virtual Result   Proc( void );

  //--------------------------------------------------------------------------------------
  /**
   * @brief    表示を最後まで行う
   */
  //--------------------------------------------------------------------------------------
  void     ProcAtOnce( void );

  //--------------------------------------------------------------------------------------
  /**
   * @brief    １ページ分の表示処理を行う
   */
  //--------------------------------------------------------------------------------------
  Result     ProcForPage( void );

  //----------------------------------------------------------------------
  /**
   * @brief      ポーズ解除して次の行を表示する
   */
  //----------------------------------------------------------------------
  void  Restart( void );

  //----------------------------------------------------------------------
  /**
   * @brief      ポーズ解除して次の行を表示する
   *
   * @param[in]  bClear   trueなら 次行の表示前にウィンドウ内容を全クリアする
   */
  //----------------------------------------------------------------------
  void  Restart( bool bClear );

  //----------------------------------------------------------------------
  /**
   * @brief      表示文字列のクリア
   */
  //----------------------------------------------------------------------
  void  Clear( void );

  //----------------------------------------------------------------------
  /**
   * @brief      フィニッシュ待ちモード設定
   *
   * @param[in]  type     フィニッシュ待ちモード
   * @param[in]  frames   待ちフレーム数（type == FINISH_TIMER || FINISH_HYB の場合のみ有効）
   */
  //----------------------------------------------------------------------
  void  SetFinishMode( FinishType type, u8 frames );

  //----------------------------------------------------------------------
  /**
   * @brief      表示強制終了し、Proc() が できるだけ早く RES_FINISH を返すようになる
   */
  //----------------------------------------------------------------------
  void  ForceQuit( void );

  //----------------------------------------------------------------------
  /**
   * @brief      メッセージ処理速度変更
   *
   * @param[in]   waitFrames    １文字出力ごとのwaitフレーム数
   * @param[in]   putCharCount  １度に出力する文字数
   */
  //----------------------------------------------------------------------
  void  SetMessageSpeed( u32 waitFrames , u32 putCharCount=1 );

  //----------------------------------------------------------------------
  /**
   * @brief      メッセージ処理速度を一時的に変更（ボタン押し中のみ速度アップなどに利用）
   *
   * @param[in]   waitFrames    １文字出力ごとのwaitフレーム数
   * @param[in]   putCharCount  １度に出力する文字数
   */
  //----------------------------------------------------------------------
  void  SetMessageSpeedTemporary( u32 waitFrames , u32 putCharCount=1 );

  //----------------------------------------------------------------------
  /**
   * @brief      メッセージ処理速度パラメータを取得
   * @param[out]  waitFrames    １文字出力ごとの wait フレーム数
   * @param[out]  putCharCount  １度に出力する文字数
   */
  //----------------------------------------------------------------------
  void  GetMessageSpeedParams( u32* waitFrames, u32* putCharCount ) const;

  //-----------------------------------------------------------------------
  /**
   * @brief   改行時、最上行のフェードアウトに要するフレーム数を設定する
   *        （何もしなければ DEFAULT_FADE_FRAMES が設定されています）
   *
   * @param[in]   frames    フレーム数
   */
  //-----------------------------------------------------------------------
  void   SetLineFadeOutFrames( u16 frames );

  //-----------------------------------------------------------------------
  /**
   * @brief   改行時、各行のスクロールに要するフレーム数を設定する
   *        （何もしなければ DEFAULT_SCROLL_FRAMES が設定されています）
   *
   * @param[in]   frames    フレーム数
   */
  //-----------------------------------------------------------------------
  void   SetLineScrollFrames( u16 frames );

  //-----------------------------------------------------------------------
  /**
   * ポーズ解除後の改ページタイプ判定
   * @return  改ページタイプ
   */
  //-----------------------------------------------------------------------
  PauseType  GetPauseType( void ) const;

protected:
  //-----------------------------------------------------------------------
  /**
   *  @brief    行送りなどの一時停止解除チェック
   *            ※停止解除の条件を任意に判定したい場合にオーバーライドしてください。
   *
   *  @retval   bool    true を返したら停止解除する
   */
  //-----------------------------------------------------------------------
  virtual bool  IsPauseRelease( void ){ return false; }

  //-----------------------------------------------------------------------
  /**
   *  @brief    フィニッシュ待ちチェック
   *            ※フィニッシュ待ちの条件を任意に判定したい場合にオーバーライドしてください。
   *
   *  @retval   bool    true を返したらフィニッシュ（Proc() が結果コード RES_FINISH を返すようになる）
   */
  //-----------------------------------------------------------------------
  virtual bool  IsFinishInput( void ){ return true; }

  //-----------------------------------------------------------------------
  /**
   * @brief     カラー変更有無チェック
   *            ※１行分の文字列描画処理が終わった時に呼び出されます。
   *               タグ処理などによりペインの描画色を変えている場合にオーバーライドしてください。
   *               次の行（ペイン）へ描画を、指定された色で開始します。
   *
   * @param[out]   top      グラデーション開始色
   * @param[out]   bottom   グラデーション終了色 （グラデーション不要の場合、top と同じ値にしてください）
   *
   * @retval    bool    色変更を行っている場合に top, bottom を正しく設定の上、true を返してください。
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
   *  @brief      TagProcessor 設定
   *
   *  @param[in]  pTagProc    タグプロセッサ
   *
   */
  //-----------------------------------------------------------------------
  void   SetTagProcessor( TagProcBase* pTagProc )
  {
    m_pTagProc = pTagProc;
  }

  //-----------------------------------------------------------------------
  /**
   *  @brief      行更新時に呼ばれる
   *
   */
  //-----------------------------------------------------------------------
  virtual void   RefreshLine(void){};


private:
  //----------------------------------------------------------------------
  // デバッグ Print 有効
  //----------------------------------------------------------------------
  enum {
    STRWIN_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL1)
  };


  enum {
    STR_BUFFER_SIZE_MIN   = 512,   ///< 最低限、これだけの文字数を格納するだけのバッファを確保する
    LINE_BUFFSER_SIZE_MIN =  40,   ///< １行表示用に指定されたペインがこれだけのサイズのバッファを持っていなければ拡張する

    DEFAULT_FADE_FRAMES   = 6,    ///< 表示先頭行がフェードアウトするのに要するフレーム数のデフォルト値
    DEFAULT_SCROLL_FRAMES = 4,    ///< 改行スクロールに要するフレーム数のデフォルト値

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
   *  読み込んだタグ情報
   */
  class TagInfo {
    public:
    STRCODE    code;          ///< タグパラメータ
    u32        paramCount;    ///< 引数の数
    u16        params[ gfl::str::NUM_TAG_PARAM_MAX ];  ///< 引数の内容

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
  virtual bool   CheckTagProcBreak( const TagInfo&  tagInfo ){ return false; };


private:
  /**
   *  文字列処理結果（内部用）
   */
  enum  SubResult {
    SUB_RES_DEFAULT,        ///< デフォルト処理（文字数インクリメント→１フレ出力文字数に達していたら終了）
    SUB_RES_FORCE_QUIT,     ///< 文字数未達でも強制的に終了する（スクロール待ち、ページ待ちなど）
    SUB_RES_FORCE_RECALL,   ///< 強制的に再コールする（タグ処理などで不要な１フレを待たないように）
  };

  /**
   *　新規文字列・新規行の出力（SEQ_PUT）状況
   */
  enum  PutState {
    PUT_READY = 0,    ///< 出力準備中
    PUT_PASSED_PROC,  ///< １度でも SEQ_PUT プロセスを通過した／他のプロセスに切り替えがあった
    PUT_PASSED_FRAME, ///< PUT_PASSED_PROC の状態でフレーム処理を終えた
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

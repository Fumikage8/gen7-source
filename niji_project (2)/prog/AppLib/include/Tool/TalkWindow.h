//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		TalkWindow.h
 *	@brief  会話用ウィンドウのクラス
 *	@author	Toru=Nagihashi => k.ohno
 *	@date		2012.06.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_TOOL_TALKWINDOW_H__)
#define __APP_TOOL_TALKWINDOW_H__

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <print/include/MessageWindow.h>
#include <print/include/WordSet.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>



#include <AppLib/include/tool/TalkWindowType.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)


//==================================================================
//
//   @brief  会話用ウィンドウのクラス
//   @attention  UIViewからできています。  kujira,sangoとは扱いが異なるので注意してください。
//
//   基本メソッド
//
//   Setupでレイアウトをセットします
//   InitMsgData でメッセージデータをセットアップします
//   InitTrainerMsgData でトレーナー名データをセットアップします 必要ないと分かっている時にはいりません。
//   CallMsgWin で表示します
//   CloseMsgWin  で非表示にします
//   End で開放します
//
//   GetStatusをみながらウインドウの状態を確認してください
//
//==================================================================
class TalkWindow : public app::ui::UIView, public app::ui::UIInputListener, public print::MsgCallback
{
  GFL_FORBID_COPY_AND_ASSIGN(TalkWindow);
public:

  static const int TEMP_STRBUF_LENGTH = 200;


  static const int SIZE_TALKWINDOW_APP = 0x2000;        /// 
  static const int SIZE_TALKWINDOW_APPDEV = 0xc0000-0x10000;    ///GFNMCat[3637] タイマーアイコン追加分で増えた0x1000のEVENT_DEVICE消費量増加を、ここで相殺する（計算した余剰が50000あるので0x10000は減らしても問題ないと大野さん達と確認済み）

  
  /**
   * @brief シーケンス
   */
  enum WIN_STATUS
  {
    WIN_STATUS_SETUP_WAIT,          ///< 初期化待ち
    WIN_STATUS_SUSPEND,             ///< 待機中
    WIN_STATUS_WIN_FRAME_IN,        ///< フレームイン
    WIN_STATUS_WIN_FRAME_IN_WAIT,   ///< フレームイン待ち
    WIN_STATUS_WIN_UPDATE,          ///< 更新中
    WIN_STATUS_WIN_FRAME_OUT_WAIT,  ///< フレームアウト待ち
    WIN_STATUS_WIN_EXIT,            ///< 終了処理
    WIN_STATUS_WIN_END,
  };
  
  enum WIN_TAIL_SIDE
  {
    WIN_TAIL_SIDE_UP,
    WIN_TAIL_SIDE_DOWN,
    WIN_TAIL_SIDE_LEFT,
    WIN_TAIL_SIDE_RIGHT,
    WIN_TAIL_SIDE_NONE,
  };

  static const int CLOSE_TIMER_NONE          = -1;   ///< タイマー機能を無効にするときの値
  
public:
  //-----------------------------------------------------------------------
  //  生成・破棄
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param heap  ヒープ
   * @param rendering_manager  アプリケーションレンダリングマネージャー
   */
  TalkWindow(app::util::Heap *pheap, app::util::AppRenderingManager * rendering_manager);

  /**
   * @brief デストラクタ
   */
  virtual ~TalkWindow();
  
  /**
   * @brief 初期化
   *
   * @param device_manager  デバイスマネージャ
   */
 // void Initialize( const gfl::ui::DeviceManager* device_manager );

  //-----------------------------------------------------------------------
  //  セットアップ・解放
  //-----------------------------------------------------------------------


public:	
	/**
   * @brief レイアウトワーク作成  レイアウトデータを外部から受け取ってセットアップします
   *
   * @comment　メッセージバッファ長は流し込むメッセージ長にあわせて設定する事
   *           標準ではTEMP_STRBUF_LENGTH=200なので、日本語66文字までが目安
   *          （メッセージ長はタグ・改行を含むと純粋な可読文字数以上に膨らむため注意）
   */
  void Setup( void* pAppLytBuff, u32 buflen = TEMP_STRBUF_LENGTH );
  /**
   * @brief レイアウトワーク作成   この関数でファイル読み込みを行いクローズします
   */
  void Setup( gfl2::fs::AsyncFileManager* pAsyncFileMan, u32 buflen = TEMP_STRBUF_LENGTH );

  /**
   * @brief レイアウトワーク破棄
   * 
   * @note  LytResIDを内部で読み込んでいた場合それの破棄も行います。（外部の場合は破棄しません）
   */
  void End( void );

  /**
   * @brief 更新
   */
  virtual void Update( void );



  /**
   * @brief メッセージウィンドウ呼び出し 表示開始
   * 
   * @note  設定してあるStrBufやメッセージタイプで実行します
   *        文字列が設定されていないとアサートです
   *
   */
  void CallMsgWin( void );
  
  /**
   * @brief メッセージウィンドウを閉じる
   */
  virtual void CloseMsgWin( void );
  /**
   * @brief メッセージウィンドウを強制閉じる
   */
  void CloseMsgWinReadily( void );
  
  /**
   * @brief 状態を取得
   *
   * @return 状態
   */
  WIN_STATUS GetStatus( void ) const;
  /**
   * @brief StrWinの更新状態を取得
   *
   * @return 更新状態
   */
  gfl2::str::StrWin::Result GetStrWinResult( void ) const;

  /**
   * @brief ポーズ解除して次の行を表示する
   */
  void Restart(void);
  
  /**
   * @brief ウィンドウが自動で閉じるタイマーの設定
   *
   * @param time  フレーム  CLOSE_TIMER_NONEだと無効
   */
  void SetTimer( s32 time );
  /**
   * @brief ウィンドウが自動で閉じる設定になっているか
   *
   * @return  trueなっている  falseなっていない
   */
  bool IsSetTimer(void ) const;
  
  /**
   * @brief ウィンドウタイプを設定
   *
   * @param window_type ウィンドウタイプ
   */
  void SetWindowType( TalkWindowType window_type );
  /**
   * @brief ウィンドウカラーを設定
   *
   * @param color ウィンドウカラー
   */
  void SetWindowColor( WIN_COLOR_TYPE color );


  /**
   * @brief  メッセージのセット
   * @param  pBuff  メッセージバッファ
   */
  void SetMessage( const gfl2::str::StrBuf* pBuff );
  /**
   * @brief メッセージのセット
   * @param  msgid  メッセージのヘッダーのラベル
   */
  void SetMessage(  u32 msgid, gfl2::str::StrBuf* pName = NULL );

  /**
   * @brief メッセージデータの初期化  ファイル読み込みます＆こちらで廃棄します
   * @param  arcid   メッセージのARCID  基本printクラスからとってくる事
   * @param  gaixno  arcのヘッダーのラベル
   */
  void InitMsgData( u32 arcid, u32 gaixno );
  /**
   * @brief トレーナー名前ファイルの初期化  ファイル読み込みますファイル読み込みます＆こちらで廃棄します
   */
  void InitTrainerMsgData( void );
  /**
   * @brief メッセージデータの初期化
   * @param  pMsgData   読み込み済みのメッセージデータ
   */
  void InitMsgData( gfl2::str::MsgData* pMsgData );
  /**
   * @brief トレーナー名前ファイルの初期化
   * @param  pMsgData   読み込み済みのメッセージデータ
   */
  void InitTrainerMsgData( gfl2::str::MsgData* pTrNameMsgData );
  /**
   * @brief  ワードセットの登録解除
   * @param  pwordset   ワードセット
   */
  void SetScriptWordSet( print::WordSet* pwordset ){ mpScriptWordSet = pwordset; }
  /**
   * @brief ウィンドウ位置を設定
   *
   * @param vec 位置
   */
  void SetWindowPos( const gfl2::math::Vector3 *vec );
  /**
   * @brief ウィンドウサイズを変更する（可変ウィンドウのみに対応）
   *
   * @param size  ウィンドウサイズ
   */
  //void SetWindowSize( const gfl2::math::Vector2 *size );

  /**
   * @brief オート描画フラグをセット
   *
   * @param is_auto true オート描画ON（デフォルト）  false OFF
   */
  virtual void SetAutoDrawFlag( bool is_auto );
  
  /**
   * @brief 描画
   * 
   * @param graphics  グラフィック
   * @param drawtype  描画先
   * @note SetAutoDrawFlagにfalseを設定した場合呼んでください
   */
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  void Draw( gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo );

  void SetSuspendKey( bool flag );
  void SetEnableCtrl( bool enableKey , bool enableTouch );
  void SetEnableMessageBoost( bool isEnable );
  void SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd );

  void SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame=0 );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief  キー入力の検知
   *         ※タッチパネルの入力が優先される
   *
   * @param  pButton  ボタン ( A,B,X,Y,L,R... )
   * @param  pKey     十字キー
   * @param  pStick   アナログスティック
   *
   * @return 同フレーム内での他入力イベントを許可するならtrue、
   *         他のイベントを排除するならfalseを返却すること。
   */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージカーソル表示非表示
   *
   * @param   bUser 自分でやる場合 true
   * @param   flg  onoff
   */
  //--------------------------------------------------------------------------------------------
  void SetMsgCursorUserVisible(bool bUser, bool flg);
  
  //----------------------------------------------------------------------------------
  /**
   * @brief タイマーアイコン表示切り替え
   *
   * @param flg   true = 表示, false = 非表示
   */
  //----------------------------------------------------------------------------------
  void SetVisibleTimerIcon( bool flg );

  //----------------------------------------------------------------------------------
  /**
   * @brief タイマーアイコンの非表示アニメが終了したか
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //----------------------------------------------------------------------------------
  bool IsTimerIconOutAnimeEnd(void);


private:
  void fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);
  void fileReadBlock( app::util::FileAccessor::RequestData* pfrs, gfl2::fs::AsyncFileManager* pAsyncFileMan );

  void _setWindowType( void );


  void Initialize2D( void );

  virtual void UpdateState( void );
  virtual void UpdateResource( void );

  void CreateMsgWin( const gfl2::str::StrBuf *string, u32 line_num );


  bool UpdateTimer(void);
  
  
  gfl2::math::Vector3 CreateHeadDirVecL( gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h );
  gfl2::math::Vector3 CreateHeadDirVecD( gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h );
  gfl2::math::Vector3 CreateHeadDirVecU( gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h );
  gfl2::math::Vector3 CreateHeadDirVecR( gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h );

  void initMsg( u32 buflen );
  void endMsg( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMsgCallback
   * @brief   メッセージ表示コールバック関数
   * @date    2016.01.26
   *
   * @param   arg   コールバックID
   *
   * @retval  true  = 継続
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  virtual bool UpdateMsgCallback( u16 arg );
  void setMsgCallBackSE( u32 se );
  void setMsgCallBackME( u32 me );

  void dispFieldName(void );
  void dispFieldName( gfl2::str::StrBuf* pName );
  bool GetTrainerNo( const gfl2::str::StrBuf* buf, int* args );

  
protected:
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  app::util::Heap * m_pAppHeap;                         //!< ヒープ
  void* m_pArcReadBuff;

  WIN_STATUS m_state;
  bool m_res_id_create;

  gfl2::str::MsgData* mpMsgData;          ///< このウインドウで表示するメッセージデータ 一本管理

  gfl2::str::MsgData* mpTrNameMsgData;       ///< トレーナー名メッセージデータ

  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf2;
  gfl2::str::StrBuf*   mpTrainerStrBuf;


  print::WordSet*  mpScriptWordSet;  ///< スクリプトの方からもらうワードセット

  TalkWindowType m_WindowType;  //ウインドウタイプ

  gfl2::str::StrWin::Result  m_strwin_result;

  s32   m_timer;                                ///< タイマー値
  bool  m_auto_close;
  bool m_bDraw;             ///< レイアウトデータを自前で読み込んだかどうか
  bool m_bMsgData;          ///< メッセージデータを自前で読み込んだかどうか
  bool m_bTrainerMsgData;   ///< トレーナーデータを自前で読み込んだかどうか

  gfl2::str::StrWin::Result m_endResult;    ///< 終了を識別する値

  u32  m_msgCallBackWork;  //メッセージコールバックで用いる4byteワーク
};
  

GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif		// __APP_TOOL_TALKWINDOW_H__

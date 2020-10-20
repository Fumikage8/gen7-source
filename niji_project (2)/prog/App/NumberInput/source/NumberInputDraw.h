#if !defined( __NUMBER_INPUT_DRAW_H_INCLUDED__ )
#define __NUMBER_INPUT_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputDraw.h
 * @date    2015/11/24 16:20:12
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：グラフィック関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "NumberInputDef.h"

#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_heap.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( NumberInput )

//------------------------------------------------------------------------------
// @brief   Drawクラス
//------------------------------------------------------------------------------
class NumberInputDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( NumberInputDraw ); //!< コピーコンストラクタ＋代入禁止

public:
  enum {
    LYTID_UPPER = 0,    //!< 上画面
    LYTID_LOWER,        //!< 下画面
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   man         ファイルマネージャ
   * @param   renderMan   レンダリングマネージャ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  NumberInputDraw( app::util::Heap* heap, gfl2::fs::AsyncFileManager* man, app::util::AppRenderingManager* renderMan );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~NumberInputDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   pAppLytBuff     レイアウトバッファ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void Initialize( void * pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   *
   * @param   none
   *
   * @return  "true  = 完了"
   * @return  "false = 処理中"
   */
  //------------------------------------------------------------------
  bool Finalize( void );

  //------------------------------------------------------------------
  /**
   * @brief   アップデート
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   *
   * @param   no    描画ディスプレイ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   ファイル読み込みチェック
   *
   * @param   none
   *
   * @return  "true  = 読み込み中"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CheckFileRead( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   レイアウトワークの取得
   *
   * @param   id    レイアウトID
   *
   * @return  レイアウトワーク
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytWk* GetLayoutWork( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースIDの取得
   *
   * @param   none
   *
   * @return  レイアウトリソースID
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytMultiResID GetLayoutResourceID( void );

  //------------------------------------------------------------------
  /**
   * @brief   モード別初期化
   *
   * @param   full    true = 数値+アルファベット, false = 数値のみ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void InitMode( bool full );

  //------------------------------------------------------------------
  /**
   * @brief   モード初期化アニメ再生チェック
   *
   * @param   none
   *
   * @return  "true  = 再生中"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CheckModeAnime( void );

  //------------------------------------------------------------------
  /**
   * @brief   入力されたデータを配置
   *
   * @param   pos   データ位置
   * @param   val   入力されたデータ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void PutInputCode( u32 pos, s32 val );

  //------------------------------------------------------------------
  /**
   * @brief   入力文字コード取得
   *
   * @param   pos   キーボード位置
   *
   * @return  文字コード
   */
  //------------------------------------------------------------------
  gfl2::str::STRCODE GetInputStrCode( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   文字コードからキーボードの位置を取得
   *
   * @param   code    文字コード
   *
   * @return  キーボード位置
   */
  //------------------------------------------------------------------
  u32 GetStrCodePos( gfl2::str::STRCODE code );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置
   *
   * @param   posPane     基準位置のペイン
   * @param   sizePane    基準サイズのペイン
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void PutCursor( gfl2::lyt::LytPane* posPane, gfl2::lyt::LytPane* sizePane );

  //------------------------------------------------------------------
  /**
   * @brief   入力位置カーソル配置
   *
   * @param   pos   入力位置
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void PutInputCursor( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   入力位置カーソル表示切り替え
   *
   * @param   flg   true = 表示
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void SetVisibleInputCursor( bool flg );

  //------------------------------------------------------------------
  /**
   * @brief   キーボード選択アニメ開始
   *
   * @param   id    キーボードID
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void StartKeyBoardSelectAnime( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   上画面文字表示
   *
   * @param   mode    表示モード
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void PutUpperString( u32 mode );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   pAppLytBuff     レイアウトバッファ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void Create2D( void * pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   2D関連削除
   *
   * @param   none
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool Delete2D( void );

  //------------------------------------------------------------------
  /**
   * @brief   キーボード上に文字を配置
   * 
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void PutKeyBoard( void );

  //------------------------------------------------------------------
  /**
   * @brief   入力データ表示を初期化
   * 
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void InitInputCode( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインのセットアップ
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void SetupPane( void );

private:
  app::util::Heap *                 m_pHeap;                    //!< ヒープ

  gfl2::fs::AsyncFileManager*       m_pAsyncFileManager;        //!< ファイルマネージャ

  app::util::AppRenderingManager*   m_pAppRenderingManager;     //!< レンダリングマネージャ

  app::util::G2DUtil*               m_pG2D;                     //!< 2D関連

  bool                              m_EndFlag;                  //!< 終了がリクエストされたか

private:
  gfl2::lyt::LytParts*              m_pInputParts[INPUT_MAX];

  gfl2::lyt::LytTextBox*            m_pInputText[INPUT_MAX];

  gfl2::lyt::LytPane*               m_pInputMark;

  gfl2::lyt::LytParts*              m_pCursorParts;

  gfl2::lyt::LytWindow*             m_pCursorWindow;
};

GFL_NAMESPACE_END( NumberInput )
GFL_NAMESPACE_END( App )

#endif // __NUMBER_INPUT_DRAW_H_INCLUDED__
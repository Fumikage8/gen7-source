#ifndef NIJI_PROJECT_APP_STRINPUT_UI_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_UI_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputUi.h
 * @brief   UI関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_ui.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputArea.h"

#include <AppLib/include/tool/app_tool_ButtonManager.h>
#include <AppLib/include/tool/app_tool_CursorManager.h>
#include <AppLib/include/Util/app_util_heap.h>

// arc
#include <niji_conv_header/app/strinput/strinput_pane.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//#define  STRINPUT_USE_UI_GYROSCOPE      // コメントを外すとジャイロセンサーが作成されます
//#define  STRINPUT_USE_UI_ACCELEROMETER  // コメントを外すと加速度センサーが作成されます

//------------------------------------------------------------------------------
/**
 * @brief  UI
 */
//------------------------------------------------------------------------------
class UI
{
public:
  //! @brief  入力結果
  enum {
    RET_NONE = 0,           //!< 動作なし
    RET_CURSOR_MOVE,        //!< カーソル移動
    RET_CURSOR_MOVE_TOUCH,  //!< カーソル移動（タッチ操作）
    RET_SCROLL_TOUCH,       //!< スクロールボタンをタッチ中
    RET_L_SCROLL_KEY,       //!< 左スクロール（キー操作）
    RET_L_SCROLL,           //!< 左スクロール
    RET_R_SCROLL_KEY,       //!< 右スクロール（キー操作）
    RET_R_SCROLL,           //!< 右スクロール
    RET_END_JUMP,           //!< 終了にジャンプ
    RET_CHG_HIRAGANA,       //!<「かな」選択
    RET_CHG_KATAKANA,       //!<「カナ」選択
    RET_CHG_ABC,            //!< アルファベットボタン選択
    RET_CHG_MARK,           //!< マークボタン選択
    RET_CHG_INCREMENT,      //!< モード切替インクリメント
    RET_DELETE_KEY,         //!< １文字削除（キー操作）
    RET_DELETE,             //!< １文字削除（タッチ操作）
    RET_END,                //!< 終了ボタン選択
    RET_INPUT,              //!< 入力エリアをタッチ
    RET_INPUT_TOUCH,        //!< ※未使用
    RET_SPACE,              //!< スペースボタン選択
    RET_SHIFT,              //!< シフトボタン選択
    RET_CHG_CODE,           //!< 文字変換（Xボタン操作）
    RET_DECIDE,             //!< 文字入力（Aボタン操作）
    RET_ICON,               //!< アイコンタッチ

    RET_KEYBOARD            //!< 以降、選択した文字ボタン
  };

  //! @brief  ボタンID
  enum {
    BTNID_HIRAGANA = 0x10000000,  //!< かな
    BTNID_KATAKANA,               //!< カナ
    BTNID_ABC,                    //!< ABC
    BTNID_MARK,                   //!< 記号
    BTNID_DELETE,                 //!< 消す
    BTNID_END,                    //!< 終わり
    BTNID_SPACE,                  //!< SPACE
    BTNID_SHIFT,                  //!< SHIFT
    BTNID_L,                      //!< 左矢印
    BTNID_R,                      //!< 右矢印
    BTNID_ICON,                   //!< アイコン

    // ローカライズ用
    BTNID_USA_CAPITAL = BTNID_HIRAGANA,   //!< 英語用：大文字
    BTNID_USA_SMALL = BTNID_KATAKANA,     //!< 英語用：小文字
    BTNID_KOR_HANGL = BTNID_HIRAGANA      //!< 韓国語用：ローマ時風入力
  };

  //! @brief  ボタン取得モード
  enum {
    TP_GET_MODE_HOLD = 0,   //!< タッチ中のボタン
    TP_GET_MODE_RELEASE     //!< 選択したボタン
  };

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンデータ
   */
  //------------------------------------------------------------------------------
  typedef struct {
    gfl2::lyt::LytPaneIndex bound;  //!< 当たり判定用ペイン
    gfl2::lyt::LytPaneIndex parts;  //!< ボタン部品ペイン
    u32  id;                        //!< ボタンID
    u32  anmOffs;                   //!< アニメオフセット
  }BUTTON_DATA;

  //------------------------------------------------------------------------------
  /**
   * @brief   UIデータ
   */
  //------------------------------------------------------------------------------
  typedef struct {
    const BUTTON_DATA * button;                          //!< ボタンデータ
    u32  button_size;                                    //!< ボタンデータサイズ  
    const app::tool::CursorManager::MOVE_DATA * cursor;  //!< カーソル移動データ
    u32  cursor_size;                                    //!< カーソル移動データサイズ
    u32  anmNo;                                          //!< アニメ番号
  }UI_DATA;


  //------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   man     UIデバイスマネージャ
   * @param   heap    ヒープ
   */
  //------------------------------------------------------------------------------
  UI( gfl2::ui::DeviceManager * man, app::util::Heap * heap );

  //------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------------------
  ~UI( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   UIデータ作成（ボタン＋カーソル）
   *
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   * @param   mode    処理モード
   * @param   lang    言語ID
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void CreateUiData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, u32 mode, u8 lang );

  //------------------------------------------------------------------------------
  /**
   * @brief   UIデータ削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void DeleteUiData( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   矢印ボタン設定
   *
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void SetArrowButtonData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID );

  //------------------------------------------------------------------------------
  /**
   * @brief   アイコンボタン設定
   *
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void SetIconButtonData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンの選択通知タイミングを設定
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void SetButtonNotifyTimming( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   入力エリア作成
   *
   * @param   heap    ヒープ
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   * @param   str     入力文字列
   * @param   siz     入力文字数
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void CreateInputArea( app::util::Heap * heap, gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, const gfl2::str::STRCODE * str, u32 siz );

  //------------------------------------------------------------------------------
  /**
   * @brief   入力エリア削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void DeleteInputArea( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   入力エリア取得
   *
   * @param   start  入力開始位置格納場所
   * @param   end    入力終了位置格納場所
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void GetInputAreaPos( u16 * start, u16 * end );

  //------------------------------------------------------------------------------
  /**
   * @brief   入力エリアサイズ取得
   *
   * @param   none
   *
   * @return  入力エリアサイズ
   */
  //------------------------------------------------------------------------------
  u32 GetInputAreaSize( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   メイン処理
   *
   * @param   none
   *
   * @return  入力結果
   */
  //------------------------------------------------------------------------------
  u32 Main( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソル位置のボタンに対応した入力結果を取得
   *
   * @param   none
   *
   * @return  入力結果
   */
  //------------------------------------------------------------------------------
  u32 GetDecideID( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソル位置取得
   *
   * @param   none
   *
   * @return  カーソル位置
   */
  //------------------------------------------------------------------------------
  u32 GetCursorPos( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソル位置設定
   *
   * @param   pos    カーソル位置
   *
   * @retval  "true = 位置が移動した"
   * @retval  "false = それ以外"
   */
  //------------------------------------------------------------------------------
  bool SetCursorPos( u32 pos );

  //------------------------------------------------------------------------------
  /**
   * @brief   指定位置のカーソルID取得
   *
   * @param   pos    カーソル位置
   *
   * @return  カーソルID
   */
  //------------------------------------------------------------------------------
  u32 GetCursorID( u32 pos );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンIDから位置を取得
   *
   * @param   id  ボタンID
   *
   * @return  ボタンデータインデックス
   */
  //------------------------------------------------------------------------------
  u32 GetButtonPosID( u32 id );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソル位置からアニメを取得
   *
   * @param   none
   *
   * @return  アニメオフセット
   */
  //------------------------------------------------------------------------------
  u32 GetCursorPosAnmOffset( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンIDからアニメを取得
   *
   * @param   id  ボタンID1
   *
   * @return  アニメオフセット
   */
  //------------------------------------------------------------------------------
  u32 GetButtonPosIDAnmOffset( u32 id );

  //------------------------------------------------------------------------------
  /**
   * @brief   タッチしているボタンのIDを取得
   *
   * @param   mode  取得モード
   *
   * @return  ボタンID
   */
  //------------------------------------------------------------------------------
  u32 GetTouchID( u32 mode );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンの当たり判定ペインを取得（カーソル位置指定）
   *
   * @param   pos    カーソル位置
   *
   * @return  当たり判定ペイン
   */
  //------------------------------------------------------------------------------
  void GetButtonBoundingPane( u32 pos, gfl2::lyt::LytPaneIndex * pPartsIdx, gfl2::lyt::LytPaneIndex * pPaneIdx );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンの当たり判定ペインを取得（ID指定）
   *
   * @param   id  ボタンID（ = カーソルID）
   *
   * @return  当たり判定ペイン
   */
  //------------------------------------------------------------------------------
  void GetButtonIDBoundingPane( u32 id, gfl2::lyt::LytPaneIndex * pPartsIdx, gfl2::lyt::LytPaneIndex * pPaneIdx );


private:
  //------------------------------------------------------------------------------
  /**
   * @brief   UI関連作成
   *
   * @param   man   UIデバイスマネージャ
   * @param   haap  ヒープ
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void CreateUI( gfl2::ui::DeviceManager * man, app::util::Heap * heap );

  //------------------------------------------------------------------------------
  /**
   * @brief   UI関連削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void DeleteUI( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソルマネージャ作成
   *
   * @param   dat   カーソルデータ
   * @param   size  データ数
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void CreateCursorManager( const app::tool::CursorManager::MOVE_DATA * dat, u32 size );

  //------------------------------------------------------------------------------
  /**
   * @brief   カーソルマネージャ削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void DeleteCursorManager( void );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャ作成
   *
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   * @param   dat     ボタンデータ
   * @param   anm_no  アニメ番号
   * @param   anm_array   先頭アニメ番号配列
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void CreateButtonManager( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, const UI::BUTTON_DATA * dat, int size, u32 anm_no, const u32 * anm_array );

  //------------------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャ削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------------------
  void DeleteButtonManager( void );

  //------------------------------------------------------------------------------
  /**GetButtonBoundingPane
   * @brief   各ボタンに対応した入力結果を取得
   *
   * @param   id  ボタンID
   *
   * @return  入力結果
   */
  //------------------------------------------------------------------------------
  u32 GetUpdateReturn( u32 id );


private:
  gfl2::ui::DeviceManager * pUiDeviceManager;    //!< 入力デバイス管理オブジェクト
  gfl2::ui::VectorDevice * pKey;                 //!< 十字キー
  gfl2::ui::VectorDevice * pStick;               //!< アナログスティック
  gfl2::ui::Button * pButton;                    //!< ボタン
  gfl2::ui::TouchPanel * pTouchPanel;            //!< タッチパネル
#ifdef  STRINPUT_USE_UI_GYROSCOPE
  gfl2::ui::Gyroscope * pGyroscope;              //!< ジャイロ
#endif  // STRINPUT_USE_UI_GYROSCOPE
#ifdef  STRINPUT_USE_UI_ACCELEROMETER
  gfl2::ui::Accelerometer * pAccelerometer;      //!< 加速度センサー
#endif  // STRINPUT_USE_UI_ACCELEROMETER

  gfl2::heap::HeapBase * pHeap;  //!< ヒープ

  app::tool::CursorManager * pCursorManager;            //!< カーソル移動マネージャ
  app::tool::CursorManager::MOVE_KEY cursorRecord[128]; //!< 移動記録用バッファ ※未使用

  app::tool::ButtonManager * pButtonManager;  //!< ボタンマネージャ

  const UI_DATA * pUiData;    //!< UIデータ

  InputArea * pInputArea;     //!< 入力エリア
};



GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_UI_H_INCLUDED

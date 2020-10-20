#ifndef NIJI_PROJECT_APP_STRINPUT_DATA_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_DATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputData.h
 * @brief   キーボードデータ
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_data.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputDef.h"

#include <AppLib/include/Util/app_util_heap.h>

// gflib2
#include <str/include/gfl2_Str.h>


GFL_WARNING_PUSH                  //!< #pragma設定を退避
GFL_WARNING_WIN32_IGNORE(4200)    //!< C4200を抑制

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )

//------------------------------------------------------------------------------
/**
 * @brief  キーボード
 */
//------------------------------------------------------------------------------
class KeyMap
{
  GFL_FORBID_COPY_AND_ASSIGN( KeyMap );

public:
  //! @brief  変換データ
  enum {
    CHG_DATA_DAKUTEN = 0,   //!< 濁点
    CHG_DATA_HANDAKUTEN,    //!< 半濁点
    CHG_DATA_QWERTY,        //!< ローマ字入力
    CHG_DATA_MAX            //!< 変換データ最大値定義
  };

  //! @brief  キーデータ
  enum {
    KEY_DATA_50K_HIRAGANA = 0,  //!< 50音：かな
    KEY_DATA_50K_KATAKANA,      //!< 50音：カナ
    KEY_DATA_50K_ABC,           //!< 50音：ABC
    KEY_DATA_50K_MARK,          //!< 50音：記号

    KEY_DATA_KEYBOARD_NORMAL,   //!< キーボード：通常
    KEY_DATA_KEYBOARD_SHIFT,    //!< キーボード：SHIFT

    KEY_DATA_KEYBOARD_HANGUL_N, //!< キーボード：ハングル（通常）
    KEY_DATA_KEYBOARD_HANGUL_S, //!< キーボード：ハングル（SHIFT）

    KEY_DATA_MAX    //!< キーデータ最大値定義
  };

  static const u32 CHANGE_CODE_LEN = 3;      //!< 変換コードの文字数 ※namein_change_conv.pl内のNAMEIN_STRCHANGE_CODE_LENと同じにすること
  static const gfl2::str::STRCODE CHANGE_END_CODE = L'　';  //!< 変換コード文字列のNULLコード
  static const gfl2::str::STRCODE CHANGE_END_CODE_LOCALIZE = L' ';  //!< 変換コード文字列のNULLコード
  static const gfl2::str::STRCODE CHANGE_REST_NULL_CODE = L'　';    //!< ローマ時変換の残す文字列のNULLコード

  static const u32 CHANGE_LOOP_CODE_MAX = 3;  //!< 循環変換のコード数

private:
  //----------------------------------------------------------------------------
  /**
   * @brief   キーデータ
   */
  //----------------------------------------------------------------------------
  typedef struct {
    u16  w;                       //!< 横幅
    u16  h;                       //!< 縦幅
    gfl2::str::STRCODE  code[0];  //!< 文字コード
  }KEY_DATA;

  //----------------------------------------------------------------------------
  /**
   * @brief   キーマップデータ
   */
  //----------------------------------------------------------------------------
  typedef struct {
    void * key[KEY_DATA_MAX];   //!< キーデータ
  }KEYMAP_DATA;

  //----------------------------------------------------------------------------
  /**
   * @brief   変換コード
   */
  //----------------------------------------------------------------------------
  typedef struct {
    gfl2::str::STRCODE input[CHANGE_CODE_LEN];    //!< 入力データ
    gfl2::str::STRCODE change[CHANGE_CODE_LEN];   //!< 変換データ
    gfl2::str::STRCODE shift[CHANGE_CODE_LEN];    //!< シフトデータ
    gfl2::str::STRCODE rest[CHANGE_CODE_LEN];     //!< 余剰データ
  }CHANGE_CODE;

  //----------------------------------------------------------------------------
  /**
   * @brief   変換データ
   */
  //----------------------------------------------------------------------------
  typedef struct {
    u16  num;               //!< データ数
    CHANGE_CODE word[0];    //!< 変換コード
  }CHANGE_DATA;

  //----------------------------------------------------------------------------
  /**
   * @brief   循環データ
   */
  //----------------------------------------------------------------------------
  typedef struct {
    u16  num;                     //!< データ数
    gfl2::str::STRCODE code[0];   //!< 文字コード
  }LOOP_CODE_DATA;


public:
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //----------------------------------------------------------------------------
  KeyMap( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ~KeyMap( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   キーコード取得
   *
   * @param   mode    入力モード
   * @param   type    入力タイプ
   * @param   shift   シフトが押されているか
   * @param   id      入力位置
   *
   * @return  キーコード
   */
  //----------------------------------------------------------------------------
  gfl2::str::STRCODE & GetKeyCode( u32 mode, u32 type, u32 shift, u32 id );

  //----------------------------------------------------------------------------
  /**
   * @brief   文字変換
   *
   * @param   mode    変換データモード
   * @param   in      入力文字列
   * @param   out     変換後の文字列
   *
   * @retval  "true = 変換した"
   * @retval  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool GetChangeCode( u32 mode, const gfl2::str::STRCODE * in, gfl2::str::STRCODE * out );

  //----------------------------------------------------------------------------
  /**
   * @brief   ローマ字変換
   *
   * @param   type    入力タイプ
   * @param   in      入力文字列
   * @param   out     変換後の文字列
   * @param   rest    余剰データの格納場所
   *
   * @retval  "true = 変換した"
   * @retval  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool GetChangeRomajiCode( u32 type, const gfl2::str::STRCODE * in, gfl2::str::STRCODE * out, gfl2::str::STRCODE * rest );

  //----------------------------------------------------------------------------
  /**
   * @brief   循環コード変換
   *
   * @param   in      入力文字列
   * @param   out     変換後の文字列
   *
   * @retval  "true = 変換した"
   * @retval  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool GetLoopCode( const gfl2::str::STRCODE in, gfl2::str::STRCODE * out );

  //----------------------------------------------------------------------------
  /**
   * @brief   終端コードを変換終了コードに置き換え
   *
   * @param   str    文字列
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void MakeCheckCode( gfl2::str::STRCODE * str );


private:
  //----------------------------------------------------------------------------
  /**
   * @brief   変換コード比較
   *
   * @param   c1  文字列１
   * @param   c2  文字列２
   *
   * @retval  "true = 同じ"
   * @retval  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckCode( const gfl2::str::STRCODE * c1, const gfl2::str::STRCODE * c2 );

  //----------------------------------------------------------------------------
  /**
   * @brief   データ作成
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateData( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   データ削除
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteData( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   キーマップデータ作成
   *
   * @param   af    アークファイル
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateKeyMap( gfl2::fs::ArcFile * af, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   キーマップデータ削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteKeyMap( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   変換データ作成
   *
   * @param   af    アークファイル
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateChangeCode( gfl2::fs::ArcFile * af, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   変換データ削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteChangeCode( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   循環データ作成
   *
   * @param   af    アークファイル
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateLoopCode( gfl2::fs::ArcFile * af, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   循環データ削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteLoopCode( void );


public:
  //----------------------------------------------------------------------------
  /**
   * @brief   ファイルの読み込み
   *
   * @param   arcID   ARCファイルID
   * @param   heap    ヒープ
   * @param   dataIdx データインデックス
   * @param   fileIdx ファイルインデックス
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void LoadData( gfl2::fs::ArcFile::ARCID arcID, gfl2::heap::HeapBase* heap, u32 dataIdx, u32 fileIdx );

  //----------------------------------------------------------------------------
  /**
   * @brief   ファイルの読み込み終了チェック
   *
   * @param   dataIdx データインデックス
   * @param   fileIdx ファイルインデックス
   *
   * @return  "true = 読み込み終了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool IsLoadData( u32 dataIdx, u32 fileIdx );

  //----------------------------------------------------------------------------
  /**
   * @brief   全てのファイルを読み込み終えたか
   *
   * @param   dataIdx データインデックス
   * @param   fileIdx ファイルインデックス
   *
   * @return  "true = 全て完了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool IsDataLoadFinished( u32* dataIdx, u32* fileIdx );


private:
  KEYMAP_DATA  dat;                   //!< キーマップデータ
  void * changeData[CHG_DATA_MAX];    //!< 変換データ

  void * loopData;    //!< 循環データ
};


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

GFL_WARNING_POP    //!< #pragma設定を回復

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_DATA_H_INCLUDED

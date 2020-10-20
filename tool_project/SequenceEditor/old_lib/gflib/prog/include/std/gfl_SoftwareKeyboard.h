//======================================================================
/**
 * @file  gfl_SoftwareKeyboard
 * @brief ソフトウェアキーボード
 * @author  ariizumi
 * @data
 */
//======================================================================

#ifndef __GFL_SOFTWARE_KEYBOARD_H__
#define __GFL_SOFTWARE_KEYBOARD_H__

#pragma once

#include <nn/swkbd.h>

#include <gfl_Heap.h>

namespace gfl
{
namespace std
{

//ユーザー辞書の型
typedef nn::swkbd::UserWord UserWord;


/*!
    @brief  キーボードのタイプ列挙体
*/
enum KeyboardType
{
    KEYBOARD_TYPE_FULL    = nn::swkbd::KEYBOARD_TYPE_FULL,             //!< フルキーボード
    KEYBOARD_TYPE_QWERTY  = nn::swkbd::KEYBOARD_TYPE_QWERTY,               //!< QWERTYキーボード
    KEYBOARD_TYPE_TENKEY  = nn::swkbd::KEYBOARD_TYPE_TENKEY,               //!< テンキーボード
    KEYBOARD_TYPE_MAX     = nn::swkbd::KEYBOARD_TYPE_MAX,

    KEYBOARD_TYPE_MAX_BIT = nn::swkbd::KEYBOARD_TYPE_MAX_BIT
};

/*!
    @brief  下帯ボタンのタイプ列挙体
*/
enum BottomButtonType
{
    BOTTOM_BUTTON_TYPE_1BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_1BUTTON,     //!< 1ボタン
    BOTTOM_BUTTON_TYPE_2BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_2BUTTON,         //!< 2ボタン
    BOTTOM_BUTTON_TYPE_3BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_3BUTTON,         //!< 3ボタン
    BOTTOM_BUTTON_TYPE_MAX     = nn::swkbd::BOTTOM_BUTTON_TYPE_MAX,

    BOTTOM_BUTTON_TYPE_MAX_BIT = nn::swkbd::BOTTOM_BUTTON_TYPE_MAX_BIT
};

/*!
    @brief  下帯ボタンのインデックス列挙体
*/
enum BottomButton
{
    BOTTOM_BUTTON_0   = nn::swkbd::BOTTOM_BUTTON_0,
    BOTTOM_BUTTON_1   = nn::swkbd::BOTTOM_BUTTON_1,
    BOTTOM_BUTTON_2   = nn::swkbd::BOTTOM_BUTTON_2,
    BOTTOM_BUTTON_MAX = nn::swkbd::BOTTOM_BUTTON_MAX,

    BOTTOM_BUTTON_1BUTTON           = nn::swkbd::BOTTOM_BUTTON_1BUTTON,         //!< 1ボタンのボタン
    BOTTOM_BUTTON_2BUTTON_LEFT      = nn::swkbd::BOTTOM_BUTTON_2BUTTON_LEFT,    //!< 2ボタンの左ボタン
    BOTTOM_BUTTON_2BUTTON_RIGHT     = nn::swkbd::BOTTOM_BUTTON_2BUTTON_RIGHT,   //!< 2ボタンの右ボタン
    BOTTOM_BUTTON_3BUTTON_LEFT      = nn::swkbd::BOTTOM_BUTTON_3BUTTON_LEFT,    //!< 3ボタンの左ボタン
    BOTTOM_BUTTON_3BUTTON_MIDDLE    = nn::swkbd::BOTTOM_BUTTON_3BUTTON_MIDDLE,  //!< 3ボタンの中ボタン
    BOTTOM_BUTTON_3BUTTON_RIGHT     = nn::swkbd::BOTTOM_BUTTON_3BUTTON_RIGHT,   //!< 3ボタンの右ボタン

    BOTTOM_BUTTON_MAX_BIT           = nn::swkbd::BOTTOM_BUTTON_MAX_BIT
};

/*!
    @brief  テンキーの拡張キーのインデックス列挙体
*/
enum TenkeyExtKey
{
    TENKEY_EXT_KEY_LEFT  = nn::swkbd::TENKEY_EXT_KEY_LEFT,            //!< 「0」キーの左隣のキー
    TENKEY_EXT_KEY_RIGHT = nn::swkbd::TENKEY_EXT_KEY_RIGHT,               //!< 「0」キーの右隣のキー
    TENKEY_EXT_KEY_MAX   = nn::swkbd::TENKEY_EXT_KEY_MAX,

    TENKEY_EXT_KEY_MAX_BIT = nn::swkbd::TENKEY_EXT_KEY_MAX_BIT
};

/*!
    @brief  入力確定可能条件列挙体
*/
enum FinishableCondition
{
    //!< 常に確定可能
    FINISHABLE_CONDITION_NONE     = nn::swkbd::FINISHABLE_CONDITION_NONE,

    //!< 1文字以上の入力
    FINISHABLE_CONDITION_ANY      = nn::swkbd::FINISHABLE_CONDITION_ANY,

    //!< 空白記号（全角・半角）以外の1文字以上の入力
    FINISHABLE_CONDITION_NON_SPACE = nn::swkbd::FINISHABLE_CONDITION_NON_SPACE,

    //!< 空白記号（全角・半角）以外の1文字以上の入力、または無入力
    FINISHABLE_CONDITION_EXCEPT_SP = nn::swkbd::FINISHABLE_CONDITION_EXCEPT_SPACE_ONLY,

    //!< 入力可能な最大文字数以上の入力
    FINISHABLE_CONDITION_FULL     = nn::swkbd::FINISHABLE_CONDITION_FULL,

    FINISHABLE_CONDITION_MAX      = nn::swkbd::FINISHABLE_CONDITION_MAX,

    FINISHABLE_CONDITION_MAX_BIT  = nn::swkbd::FINISHABLE_CONDITION_MAX_BIT
};

/*!
    @brief  入力文字の隠蔽モード
*/
enum MaskMode
{
    //!< 隠蔽しない
    MASK_MODE_DISABLE = nn::swkbd::MASK_MODE_DISABLE,

    //!< 入力後に即座に隠蔽する
    MASK_MODE_IMMEDIATE = nn::swkbd::MASK_MODE_IMMEDIATE,

    //!< 入力から1秒待つか、次の文字が入力されたら隠蔽する
    MASK_MODE_WAIT_1SEC = nn::swkbd::MASK_MODE_WAIT_1SEC,

    MASK_MODE_MAX = nn::swkbd::MASK_MODE_MAX,

    MASK_MODE_MAX_BIT = nn::swkbd::MASK_MODE_MAX_BIT
};

/*!
    @brief  保護者による使用制限一時解除モード
*/
enum ParentalControlsMode
{
    //!< 保護者による使用制限の一時解除を行わない
    PARENTAL_CONTROLS_MODE_DISABLE = nn::swkbd::PARENTAL_CONTROLS_MODE_DISABLE,

    //!< 保護者による使用制限の一時解除を行う
    PARENTAL_CONTROLS_MODE_ENABLE = nn::swkbd::PARENTAL_CONTROLS_MODE_ENABLE,

    // 本体設定用（使用禁止）
    PARENTAL_CONTROLS_MODE_SYSTEM_SETTINGS = nn::swkbd::PARENTAL_CONTROLS_MODE_SYSTEM_SETTINGS,
    PARENTAL_CONTROLS_MODE_MAX = nn::swkbd::PARENTAL_CONTROLS_MODE_MAX,

    PARENTAL_CONTROLS_MODE_MAX_BIT = nn::swkbd::PARENTAL_CONTROLS_MODE_MAX_BIT
};

/*!
    @brief  上画面の表示方法を示すフラグ
*/
enum UpperScreenFlag
{
    //※bit管理

    //!< アプリケーションの画像をそのまま表示します。
    UPPER_SCREEN_NORMAL = nn::swkbd::UPPER_SCREEN_NORMAL,

    //!< アプリケーションの画像を暗くして表示します。
    UPPER_SCREEN_DARK = nn::swkbd::UPPER_SCREEN_DARK,

    UPPER_SCREEN_MAX_BIT = nn::swkbd::UPPER_SCREEN_MAX_BIT
};

/*!
    @brief  テキストチェックの種類を示すフラグ
*/
enum TextCheckFlag
{
    //!< テキストチェックを行わない
    TEXT_CHECK_DISABLE      = nn::swkbd::TEXT_CHECK_DISABLE,// = 0x0,

    //!< 数字の入力数を制限する
    TEXT_CHECK_NUMBER       = nn::swkbd::TEXT_CHECK_NUMBER,// = 0x1,

    //!< アットマークの入力を禁止する
    TEXT_CHECK_AT_MARK      = nn::swkbd::TEXT_CHECK_AT_MARK,// = 0x2,

    //!< パーセント記号の入力を禁止する
    TEXT_CHECK_PERCENT_SIGN = nn::swkbd::TEXT_CHECK_PERCENT_SIGN,// = 0x4,

    //!< バックスラッシュの入力を禁止する
    TEXT_CHECK_BACKSLASH    = nn::swkbd::TEXT_CHECK_BACKSLASH,// = 0x8,

    //!< スクリーン上に表示される文字列として問題のある語句の入力を禁止する
    TEXT_CHECK_BANNED_WORDS = nn::swkbd::TEXT_CHECK_BANNED_WORDS,// = 0x10,

    //!< アプリケーションよるテキストチェックを行う
    TEXT_CHECK_APP          = nn::swkbd::TEXT_CHECK_APP,// = 0x20,

    TEXT_CHECK_MAX_BIT      = nn::swkbd::TEXT_CHECK_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  拡張出力データの種類を示すフラグ
*/
enum ExtOutputDataFlag
{
    //!< 拡張出力データなし
    EXT_OUTPUT_DATA_NONE = nn::swkbd::EXT_OUTPUT_DATA_NONE,// = 0x0,

    //!< 最終動作状態データ
    EXT_OUTPUT_DATA_STATUS = nn::swkbd::EXT_OUTPUT_DATA_STATUS,// = 0x1,

    //!< 予測変換学習データ
    EXT_OUTPUT_DATA_LEARNING = nn::swkbd::EXT_OUTPUT_DATA_LEARNING,// = 0x2,

    EXT_OUTPUT_DATA_MAX_BIT = nn::swkbd::EXT_OUTPUT_DATA_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  アプリケーションによるテキストチェック結果
*/
enum AppTextCheckResult
{
    //!< 問題なし。そのままキーボード終了。
    APP_TEXT_CHECK_RESULT_OK = nn::swkbd::APP_TEXT_CHECK_RESULT_OK,// = 0,

    //!< 問題あり。ダイアログを表示してキーボード終了。
    APP_TEXT_CHECK_RESULT_NG_FINISH = nn::swkbd::APP_TEXT_CHECK_RESULT_NG_FINISH,

    //!< 問題あり。ダイアログを表示してキーボード続行。
    APP_TEXT_CHECK_RESULT_NG_CONTINUE = nn::swkbd::APP_TEXT_CHECK_RESULT_NG_CONTINUE,

    APP_TEXT_CHECK_RESULT_MAX = nn::swkbd::APP_TEXT_CHECK_RESULT_MAX,

    APP_TEXT_CHECK_RESULT_MAX_BIT = nn::swkbd::APP_TEXT_CHECK_RESULT_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  リターンコード

            正常に終了した場合は 0 以上、異常終了した場合は負値となります。
*/
enum ReturnCode
{
    //!< 不明
    RETURN_CODE_UNKNOWN = nn::swkbd::RETURN_CODE_UNKNOWN,// = -1,

    //!< コンフィグの値が不正
    RETURN_CODE_INVALID_CONFIG = nn::swkbd::RETURN_CODE_INVALID_CONFIG,// = -2,

    //!< メモリ不足による異常終了
    RETURN_CODE_OUT_OF_MEMORY = nn::swkbd::RETURN_CODE_OUT_OF_MEMORY,// = -3,

    //!< バージョン番号不一致
    //RETURN_CODE_INVALID_VERSION                 = -4,

    //!< 1ボタンタイプの下帯ボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_1BUTTON = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_1BUTTON,// = 0,

    //!< 2ボタンタイプの下帯ボタンの内、左のボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_2BUTTON_LEFT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_2BUTTON_LEFT,// = 1,

    //!< 2ボタンタイプの下帯ボタンの内、右のボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_2BUTTON_RIGHT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_2BUTTON_RIGHT,// = 2,

    //!< 3ボタンタイプの下帯ボタンの内、左のボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_LEFT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_LEFT,// = 3,

    //!< 3ボタンタイプの下帯ボタンの内、中央のボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_MIDDLE = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_MIDDLE,// = 4,

    //!< 3ボタンタイプの下帯ボタンの内、右のボタンが押された
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_RIGHT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_RIGHT,// = 5,

    //!< HOME ボタンが押された
    RETURN_CODE_HOME_BUTTON = nn::swkbd::RETURN_CODE_HOME_BUTTON,// = 10,

    //!< ソフトウェアリセットとなる組み合わせのボタンが押された
    RETURN_CODE_SOFTWARE_RESET = nn::swkbd::RETURN_CODE_SOFTWARE_RESET,// = 11,

    //!< 電源ボタンが押された
    RETURN_CODE_POWER_BUTTON = nn::swkbd::RETURN_CODE_POWER_BUTTON,// = 12,

    //!< 保護者による使用制限の一時解除に成功した
    RETURN_CODE_PARENTAL_CONTROLS_SUCCESS = nn::swkbd::RETURN_CODE_PARENTAL_CONTROLS_SUCCESS,// = 20,

    //!< 保護者による使用制限の一時解除に失敗した
    RETURN_CODE_PARENTAL_CONTROLS_FAILURE = nn::swkbd::RETURN_CODE_PARENTAL_CONTROLS_FAILURE,// = 21,

    //!< アプリケーションによるテキストチェックの結果がNGだった
    RETURN_CODE_APP_TEXT_CHECK_NG = nn::swkbd::RETURN_CODE_APP_TEXT_CHECK_NG,// = 30,

    RETURN_CODE_MAX_BIT = nn::swkbd::RETURN_CODE_MAX_BIT,// = (s32)(1u << 31)
};


class SoftwareKeyboard
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(SoftwareKeyboard);

public:

  //----------------------------------------------------------------------------
  /*
   *  @brief  コンストラクタ
   *
   *  @param  heap::HeapBase *heap 作業用ヒープ ※デバイスメモリ禁止！
   *  @param  const u32 strLen 入力文字列のサイズ
   */
  //-----------------------------------------------------------------------------
  SoftwareKeyboard( gfl::heap::HeapBase *heap , const u32 strLen );

  ~SoftwareKeyboard();

  //----------------------------------------------------------------------------
  /*
   *  @brief  ソフトウェアキーボード起動
   */
  //-----------------------------------------------------------------------------
  AppletWakeupState BootKeyboard( void );

  //----------------------------------------------------------------------------
  /*
   *  @brief  リターンコードの取得
   */
  //-----------------------------------------------------------------------------
  ReturnCode GetReturnCode(void);

  //-----------------------------------------------------------------------------
  //各種設定
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /*
   *  @brief  キーボードタイプ設定
   */
  //-----------------------------------------------------------------------------
  void SetKeyboardType( const KeyboardType kbType );
  //----------------------------------------------------------------------------
  /*
   *  @brief  下帯ボタンのタイプ設定
   */
  //-----------------------------------------------------------------------------
  void SetBottomButtonType( const BottomButtonType buttonType );
  //----------------------------------------------------------------------------
  /*
   *  @brief  下帯ボタンのテキスト設定
   */
  //-----------------------------------------------------------------------------
  void SetBottomButtonText( const BottomButton buttonPos , const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  テンキーの拡張ボタンのテキスト設定
   */
  //-----------------------------------------------------------------------------
  void SetTenkeyExtText( const TenkeyExtKey buttonPos , const wchar_t text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  入力確定可能条件設定
   */
  //-----------------------------------------------------------------------------
  void SetFinishableCondition( const FinishableCondition con );
  //----------------------------------------------------------------------------
  /*
   *  @brief  入力文字の隠蔽モード設定
   */
  //-----------------------------------------------------------------------------
  void SetMaskMode( const MaskMode mode );
  //----------------------------------------------------------------------------
  /*
   *  @brief  保護者による使用制限一時解除モード設定
   */
  //-----------------------------------------------------------------------------
  void SetParentalControlsMode( const ParentalControlsMode mode );
  //----------------------------------------------------------------------------
  /*
   *  @brief  上画面の表示方法フラグの設定
   *
   *  @param  const u32 flg UpperScreenFlagの論理和
   */
  //-----------------------------------------------------------------------------
  void SetUpperScreenFlag( const u32 flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  テキストチェックの種類フラグの設定
   *
   *  @param  const u32 flg TextCheckFlagの論理和
   */
  //-----------------------------------------------------------------------------
  void SetTextCheckFlag( const u32 flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  数字の最大入力可能数
   *          SetTextCheckFlagでTEXT_CHECK_NUMBER をセットする必要あり
   *
   *  @param  const u16 num 文字数
   */
  //-----------------------------------------------------------------------------
  void SetNumbersMax( const u16 num );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ガイドテキストの設定
   */
  //-----------------------------------------------------------------------------
  void SetGuideText( const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  予測変換の設定
   */
  //-----------------------------------------------------------------------------
  void SetEnablePrediction( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  改行の設定
   */
  //-----------------------------------------------------------------------------
  void SetEnableLinefeed( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ホームボタン終了可能か
   *          ※HOMEボタン時はSetTextCheckFlagのチェックが無効になります
   */
  //-----------------------------------------------------------------------------
  void SetEnableHomeButton( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ソフトウェアリセットが可能か？
   */
  //-----------------------------------------------------------------------------
  void SetEnableSoftwareReset( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  等幅表示の設定
   */
  //-----------------------------------------------------------------------------
  void SetEnableFixedWidthMode( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  初期テキストの設定
   *
   *  @param  const wchar_t *text 初期文字列(NULL終端)
   *                              初期文字列をなくしたい場合NULLを渡してください。
   */
  //-----------------------------------------------------------------------------
  void SetDefaultText( const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ユーザー辞書の設定
   */
  //-----------------------------------------------------------------------------
  void SetUserWord( const UserWord *userWord , const u32 wordNum );



  //-----------------------------------------------------------------------------
  /**
   * @brief 入力した文字列を取得する
   */
  //-----------------------------------------------------------------------------
  const wchar_t* GetInputString( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief 入力した文字列の長さを取得する
   */
  //-----------------------------------------------------------------------------
  u16 GetInputStringLength( void ) const;

  /** 動作状態データのサイズ定義 */
  enum {
    STATUS_DATA_SIZE = nn::swkbd::CTR::STATUS_DATA_SIZE,
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief  動作状態データの先頭アドレスを取得する
   *
   * @return 動作状態データ先頭アドレス
   *
   * @note
   * 動作状態データのサイズは STATUS_DATA_SIZE で定義されています。
   * ソフトウェアキーボードの動作終了後、この関数で取得したデータを保存しておき、
   * 次回 BootKeyboard の前に SetStatusData に引数として引き渡すことで、
   * 動作状態（入力文字種の指定など）を引き継いでキーボードを起動できます。
   */
  //-----------------------------------------------------------------------------
  const void* GetStatusData( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief 動作状態データをセットする
   * @param statusData    動作状態データ
   */
  //-----------------------------------------------------------------------------
  void SetStatusData( const void* statusData );

private:

  gfl::heap::HeapBase *mHeap;
  u16            mInputStrLen;
  wchar_t*       mInputStr;
  wchar_t*       mDefaultStr;

  const UserWord *mUserWord;

  nn::swkbd::Parameter mKbdParam; // キーボードの設定構造体と、画面キャプチャ情報が入った構造体

  u8             mStatusData[ STATUS_DATA_SIZE ];   ///< 動作後の状態設定データを保存しておく
  const void*    mInitialStatusData;                ///< 動作開始前の状態設定データを指定する

};



} //namespace std
} //namespace gfl


#endif //__GFL_SOFTWARE_KEYBOARD_H__

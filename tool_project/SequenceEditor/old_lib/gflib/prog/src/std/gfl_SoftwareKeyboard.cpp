//======================================================================
/**
 * @file  gfl_SoftwareKeyboard
 * @brief ソフトウェアキーボード
 * @author  ariizumi
 * @data
 */
//======================================================================

#include <std/gfl_SoftwareKeyboard.h>

#include "core/gfl_Dummy.h"

namespace gfl
{
namespace std
{

SoftwareKeyboard::SoftwareKeyboard( heap::HeapBase *heap , const u32 strLen )
  :mHeap(heap)
  ,mUserWord(NULL)
  ,mInitialStatusData(NULL)
{
  mInputStrLen = 0;
  mInputStr   = static_cast<wchar_t*>(GflHeapAllocMemory( mHeap , sizeof(wchar_t)*(strLen+1) ));
  mDefaultStr = NULL;

  nn::swkbd::InitializeConfig( &mKbdParam.config );
  mKbdParam.config.textLengthMax = strLen;
}

SoftwareKeyboard::~SoftwareKeyboard()
{
  GflHeapFreeMemory( mInputStr );

  if( mDefaultStr )
  {
    GflHeapFreeMemory( mDefaultStr );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ソフトウェアキーボード起動
 *
 */
//-----------------------------------------------------------------------------
AppletWakeupState SoftwareKeyboard::BootKeyboard( void )
{
  // 共有メモリ領域サイズ計算
  s32  memSize  = nn::swkbd::GetSharedMemorySize( &mKbdParam.config, NULL, NULL );
  void *swkBuf  = NULL;

  // 必要な共有メモリサイズが０の場合はメモリを確保しない
  if( memSize > 0 )
  {
    swkBuf = GflHeapAllocMemoryAlign4096(mHeap , memSize);
  }

  //------------------------
  //SDK4.0からコマンドリストを完了させることが必須になりました！
  //------------------------
  nngxWaitCmdlistDone();
  //------------------------

  // キーボード呼び出し
  mKbdParam.config.inputTextLength = 0;
  mKbdParam.config.extOutputDataFlag |= EXT_OUTPUT_DATA_STATUS;
  AppletWakeupState wstate;
  bool ret = nn::swkbd::StartKeyboardApplet( &wstate,
                                  &mKbdParam,
                                  swkBuf,   // 4096バイト整合されている必要があります。デバイスメモリは指定できません
                                  memSize,  // 4096の倍数である必要があります
                                  mDefaultStr,
                                  mUserWord,
                                  mInitialStatusData );
  if( swkBuf != NULL )
  {
    // 入力された文字列をコピー
    mInputStrLen = mKbdParam.config.inputTextLength;
    gfl::std::MemCopy( swkBuf, mInputStr, mKbdParam.config.inputTextLength * sizeof(wchar_t) );

    // 文字列終端コードをセット
    mInputStr[mInputStrLen] = '\0';

    // 動作状態データを保存しておく
    void* statusDataPtr = static_cast<u8*>(swkBuf) + mKbdParam.config.statusData;
    gfl::std::MemCopy( statusDataPtr, mStatusData, sizeof(mStatusData) );

    GflHeapFreeMemory(swkBuf);
  }

  //状況に応じて終了対応。メインループのapplet処理が拾ってくれるかも。
  GFL_PRINT("Swkbd return[%d]\n",wstate);
  return wstate;
}

//----------------------------------------------------------------------------
/*
 *  @brief  リターンコードの取得
 */
//-----------------------------------------------------------------------------
ReturnCode SoftwareKeyboard::GetReturnCode(void)
{
  return static_cast<ReturnCode>(mKbdParam.config.returnCode);
}


//----------------------------------------------------------------------------
/*
 *  @brief  キーボードタイプ設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetKeyboardType( const KeyboardType kbType )
{
  mKbdParam.config.keyboardType = static_cast<nn::swkbd::KeyboardType>(kbType);
}

//----------------------------------------------------------------------------
/*
 *  @brief  下帯ボタンのタイプ設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetBottomButtonType( const BottomButtonType buttonType )
{
  mKbdParam.config.bottomButtonType = static_cast<nn::swkbd::BottomButtonType>(buttonType);;
}


//----------------------------------------------------------------------------
/*
 *  @brief  下帯ボタンのテキスト設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetBottomButtonText( const BottomButton buttonPos , const wchar_t *text )
{
  gfl::str::StrCopy( mKbdParam.config.bottomButtonText[buttonPos] , text );
}


//----------------------------------------------------------------------------
/*
 *  @brief  テンキーの拡張ボタンのテキスト設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetTenkeyExtText( const TenkeyExtKey buttonPos , const wchar_t text )
{
  mKbdParam.config.tenkeyExtKeyChar[buttonPos] = text;
}

//----------------------------------------------------------------------------
/*
 *  @brief  入力確定可能条件設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetFinishableCondition( const FinishableCondition con )
{
  mKbdParam.config.finishableCondition = static_cast<nn::swkbd::FinishableCondition>(con);
}

//----------------------------------------------------------------------------
/*
 *  @brief  入力文字の隠蔽モード設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetMaskMode( const MaskMode mode )
{
  mKbdParam.config.maskMode = static_cast<nn::swkbd::MaskMode>(mode);
}

//----------------------------------------------------------------------------
/*
 *  @brief  保護者による使用制限一時解除モード設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetParentalControlsMode( const ParentalControlsMode mode )
{
  mKbdParam.config.parentalControlsMode = static_cast<nn::swkbd::ParentalControlsMode>(mode);
}

//----------------------------------------------------------------------------
/*
 *  @brief  上画面の表示方法フラグの設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetUpperScreenFlag( const u32 flg )
{
  mKbdParam.config.upperScreenFlag = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  テキストチェックの種類フラグの設定
 *
 *  @param  const u32 flg TextCheckFlagの論理和
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetTextCheckFlag( const u32 flg )
{
  mKbdParam.config.textCheckFlag = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  数字の最大入力可能数
 *          SetTextCheckFlagでTEXT_CHECK_NUMBER をセットする必要あり
 *
 *  @param  const u16 num 文字数
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetNumbersMax( const u16 num )
{
  mKbdParam.config.numNumbersMax = num;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ガイドテキストの設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetGuideText( const wchar_t *text )
{
  gfl::str::StrCopy( mKbdParam.config.guideText , text );
}

//----------------------------------------------------------------------------
/*
 *  @brief  予測変換の設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnablePrediction( const bool flg )
{
  mKbdParam.config.prediction = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  改行の設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableLinefeed( const bool flg )
{
  mKbdParam.config.lineFeed = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  等幅表示の設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableFixedWidthMode( const bool flg )
{
  mKbdParam.config.fixedWidthMode = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ホームボタン終了可能か
 *          ※HOMEボタン時はSetTextCheckFlagのチェックが無効になります
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableHomeButton( const bool flg )
{
  mKbdParam.config.homeButton = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ソフトウェアリセットが可能か？
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableSoftwareReset( const bool flg )
{
  mKbdParam.config.softwareReset = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  初期テキストの設定
 *
 *  @param  const wchar_t *text 初期文字列(NULL終端)
 *                              初期文字列をなくしたい場合NULLを渡してください。
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetDefaultText( const wchar_t *text )
{
  if( text )
  {
    if( mDefaultStr == NULL )
    {
      mDefaultStr = static_cast<wchar_t*>(GflHeapAllocMemory( mHeap , sizeof(wchar_t)*(mKbdParam.config.textLengthMax+1) ));
    }
    gfl::str::StrCopy( mDefaultStr , text );
  }
  else
  {
    if( mDefaultStr )
    {
      GflHeapFreeMemory( mDefaultStr );
    }
    mDefaultStr = NULL;
  }
}

//----------------------------------------------------------------------------
/*
 *  @brief  ユーザー辞書の設定
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetUserWord( const UserWord *userWord , const u32 wordNum )
{
  mUserWord = userWord;
  mKbdParam.config.numUserWords = wordNum;
}




//-----------------------------------------------------------------------------
/**
 * @brief 入力した文字列を取得する
 */
//-----------------------------------------------------------------------------
const wchar_t* SoftwareKeyboard::GetInputString( void ) const
{
  return mInputStr;
}

//-----------------------------------------------------------------------------
/**
 * @brief 入力した文字列の長さを取得する
 */
//-----------------------------------------------------------------------------
u16 SoftwareKeyboard::GetInputStringLength( void ) const
{
  return mInputStrLen;
}

//-----------------------------------------------------------------------------
/**
 * 動作状態データの先頭アドレスを取得する
 *
 * @return 動作状態データ先頭アドレス
 */
//-----------------------------------------------------------------------------
const void* SoftwareKeyboard::GetStatusData( void )
{
  return mStatusData;
}
//-----------------------------------------------------------------------------
/**
 * @brief 動作状態データをセットする
 * @param statusData    動作状態データ
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetStatusData( const void* statusData )
{
  mInitialStatusData = statusData;
}


} //namespace std
} //namespace gfl



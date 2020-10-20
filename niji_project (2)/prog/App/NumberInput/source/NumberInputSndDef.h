#if !defined( __NUMBER_INPUT_SND_DEF_H_INCLUDED__ )
#define __NUMBER_INPUT_SND_DEF_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputSndDef.h
 * @date    2015/11/24 20:25:02
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：サウンド定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <sound/include/Sound.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
#define NUMINPUT_SE_CURSOR_MOVE   ( SEQ_SE_SELECT1 )    //!< カーソル移動音
#define NUMINPUT_SE_DECIDE        ( SEQ_SE_DECIDE1 )    //!< Ａボタン
#define NUMINPUT_SE_DELETE        ( SEQ_SE_CANCEL1 )    //!< 「けす」
#define NUMINPUT_SE_COMPLETE      ( SEQ_SE_COMPLETE1 )  //!< 「おわり」
#define NUMINPUT_SE_ERROR         ( SEQ_SE_BEEP )       //!< エラー音

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)

#endif // __NUMBER_INPUT_SND_DEF_H_INCLUDED__

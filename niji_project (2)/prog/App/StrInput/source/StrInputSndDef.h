#ifndef NIJI_PROJECT_APP_STRINPUT_SND_DEF_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_SND_DEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputSndDef.h
 * @brief   SE定義
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_snd_def.hから移植
 */
//==============================================================================

// niji
#include <Sound/include/Sound.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


#define  STR_INPUT_SE_CURSOR_MOVE    ( SEQ_SE_SELECT1 )    //!< カーソルの移動音
#define  STR_INPUT_SE_PAGE_CHANGE    ( SEQ_SE_PAGE3 )      //!< ページ切り替え
#define  STR_INPUT_SE_DECIDE          ( SEQ_SE_DECIDE1 )    //!< 決定音
#define  STR_INPUT_SE_ERROR          ( SEQ_SE_BEEP )        //!< エラー音
#define  STR_INPUT_SE_CNACEL          ( SEQ_SE_CANCEL1 )    //!< 文字を消す、Ｂボタン（戻るマークのタッチリリース）
#define  STR_INPUT_SE_CLOSE          ( SEQ_SE_CLOSE1 )      //!< Ｘボタン（閉じるマークのタッチリリース）
#define  STR_INPUT_SE_COMPLETE        ( SEQ_SE_COMPLETE1 )  //!< 「おわり」で、入力した文字を決定したとき


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_SND_DEF_H_INCLUDED

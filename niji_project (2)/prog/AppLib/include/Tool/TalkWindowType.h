//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		TalkWindowType.h
 *	@brief  ウィンドウタイプ定義
 *	@author	Masayuki Onoue  => k.ohno
 *	@date		2012.05.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __APP_TOOL_TALKWINDOW_TYPE_H__ )
#define __APP_TOOL_TALKWINDOW_TYPE_H__

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

enum TalkWindowType
{
  WIN_TYPE_SYS,       // システムウィンドウ(強制2行)
  WIN_TYPE_TALK,      // 会話(強制2行)    ある
  WIN_TYPE_GIZA,      // ギザ             ある

  WIN_TYPE_SYS_DOWN,       // システムウィンドウ(強制2行) 下
  WIN_TYPE_TALK_DOWN,      // 会話(強制2行)    ある       下
  WIN_TYPE_GIZA_DOWN,      // ギザ             ある       下

  WIN_TYPE_DIARY,     //日記
  WIN_TYPE_MONEY,     //おかね

  WIN_TYPE_TELOP,     //テロップ

  WIN_TYPE_GAIN_BOARD,      //おとくなけいじばん
  WIN_TYPE_NARUHODO_BOARD,  //なるほどけいじばん
  WIN_TYPE_NORMAL_BOARD,    //ノーマル掲示板

  WIN_TYPE_CREDIT,    // 字幕用

  WIN_TYPE_JOIN_FESTA, // ジョインフェスタ用
  WIN_TYPE_SYS_AT_ONCE,// システムウィンドウ（一括表示）

  WIN_TYPE_TELOP_WHITE,  //  テロップ（白背景、黒文字用）
  WIN_TYPE_BATTLE_FES, // バトルフェス用
  WIN_TYPE_TELOP_CLEAR, // RR団専用テロップ（透明背景、白文字）

  WIN_TYPE_MAX,       // 最大値
};


  /// カラー定数
enum WIN_COLOR_TYPE{
  WIN_COLOR_TYPE_DEFAULT = 0,
  WIN_COLOR_TYPE_HERO,
  WIN_COLOR_TYPE_FRIEND1,
  WIN_COLOR_TYPE_FRIEND2,
  WIN_COLOR_TYPE_FRIEND3,
  WIN_COLOR_TYPE_HAKASE,
  WIN_COLOR_TYPE_CHAMPION,
  WIN_COLOR_TYPE_MYSTERYM,
  WIN_COLOR_TYPE_FLARE_BOSS,
  WIN_COLOR_TYPE_HEROINE,
  WIN_COLOR_TYPE_MAX,
};


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif		// __APP_TOOL_TALKWINDOW_TYPE_H__

#ifndef __H_HEAP_DEFINE_H__
#define __H_HEAP_DEFINE_H__
/**
* @file    HeapDefine.h
* @brief   nijiアプリケーション ヒープID定義
* @author  Miyuki Iwasawa 
* @date    2015.01.31
*/

#include <heap/include/gfl2_Heap.h>

//HEAPID定義
enum
{
  HEAPID_START = gfl2::heap::HEAPID_USER_START-1,
  //マクロ展開を使っていたので上のような書き方をしていた。sangoのmain.h参照
  HEAPID_DEVICE,
  HEAPID_RESIDENT,
  HEAPID_RESIDENT_DEVICE,
  HEAPID_APP,
  HEAPID_APP_DEVICE,
  HEAPID_PROCCELL,
  HEAPID_FILEREAD,
  HEAPID_AUTO_STACK,
  HEAPID_DLL_LOAD,
  HEAPID_EVENT,
  HEAPID_EVENT_DEVICE,

  HEAPID_DEBUG,
  HEAPID_DEBUG_DEVICE,

  HEAPID_SYSTEM_FONT,
  HEAPID_SYSTEM_FONT_DEVICE,

  HEAPID_SOUND,
  HEAPID_SOUND_DEVICE,

  HEAPID_DEBUG_LAUNCHER_SYSTEM,
  HEAPID_DEBUG_LAUNCHER,
  HEAPID_DEBUG_LAUNCHER_DEVICE,

  HEAPID_DEBUGWIN, //デバッグウィンドウ
  HEAPID_DEBUGWIN_DEVICE, //デバッグウィンドウ
  HEAPID_DEBUG_BATTLE, // デバッグバトル
  HEAPID_DEBUG_BATTLE_DEVICE, // デバッグバトル

  HEAPID_DEBUG_FS_GASC,                   // 親ヒープはHEAPID_DEBUG。ArcSrcファイルを読み込む際に使うメモリ。
  HEAPID_DEBUG_FS_LOG,                    // 親ヒープはHEAPID_DEBUG。fsのデバッグ用のログを取るのに使うメモリ。
  HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE,  // 親ヒープはHEAPID_DEBUG_DEVICE。CTRのHIOを初期化するのに使うメモリ。
  HEAPID_DEBUG_FS_HIO,                    // 親ヒープはHEAPID_DEBUG。CTRのHIOが有効のときに使うメモリ。
  HEAPID_DEBUG_FS_CALLER,                 // 親ヒープはHEAPID_DEBUG。リクエスト発行者の管理に使うメモリ。
  HEAPID_DEBUG_FS_DIRECT_SDMC_DEVICE,     // 親ヒープはHEAPID_DEBUG。開発中専用デバイスDirectSdmcに使うメモリ。

  HEAPID_FIELD,
  HEAPID_FIELD_DEVICE,
  HEAPID_FIELD_SUBSCREEN,
  HEAPID_FIELD_SUBSCREEN_DEVICE,
  HEAPID_FIELD_SCRIPT,
  HEAPID_FIELD_SCRIPT_FILEREAD,
  HEAPID_FIELD_SCRIPT_WORK,
  HEAPID_FIELD_STATIC_MODEL_DEVICE,

  HEAPID_NETWORK_RESIDENT,
  HEAPID_NETWORK_DEVICE,

  HEAPID_ERROR_APPLET_SYSTEM,
  HEAPID_ERROR_APPLET_MESSAGE,

  HEAPID_STRINPUT_SYS,
  HEAPID_STRINPUT_APP,
  HEAPID_STRINPUT_DEVICE,
  HEAPID_STRINPUT_UI,

  // @note WIN32版限定機能で使用する常駐ヒープ、WIN32とCTRでIDを一致させるためHEAPIDを追加する際はこの定義より上に追加してください 150520 ikuta_junya
#if defined(GF_PLATFORM_WIN32)
  HEAPID_WIN32_WORK,
#endif

  HEAPID_CHILD_MAX //終端
};

//niji全体で使用さえるローカルヒープの数 数は暫定
static const int HEAPID_LOCAL_MAX = 128;


#endif //__H_HEAP_DEFINE_H__


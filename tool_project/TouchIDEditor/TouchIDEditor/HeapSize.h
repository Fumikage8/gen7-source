#ifndef __H_HEAP_SIZE_H__
#define __H_HEAP_SIZE_H__
//===================================================================
/**
 * @file    HeapSize.h
 * @brief   ヒープサイズ指定
 * @author  Miyuki Iwasawa
 * @date    15.01.31
 */
//===================================================================
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(System)

class HeapSize
{
private:

//---------------------------------------------------
//  最適化無効時にDLLに割り振るメモリサイズ
//---------------------------------------------------
#if PM_ENABLE_OPTIMIZE
  #if PM_DEBUG_PRINT
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x4000;
  #else //#if PM_DEBUG_PRINT
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x0;
  #endif //#if PM_DEBUG_PRINT
#else //#if PM_ENABLE_OPTIMIZE
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x60000;
#endif  //#if PM_ENABLE_OPTIMIZE

public:

/**
 *  nijiProject 16MB増加の内訳
 */
static const u32 APP_DEVICE_HEAP                    = 0x500000;     // アプリケーションデバイスが使用
static const u32 CHINA_FONT_DEVICE_HEAP             = 0x380000;    // 中国語フォント 0x380000byeを予定
static const u32 BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP  = 0x300000; // バトル遷移高速化  0x300000byteを予定
//static const u32 DEVICE_MEMORY_BUFFER               = 0x480000;      // バッファとしておいておくもの。
static const u32 DEVICE_MEMORY_BUFFER               = 0x080000;      // バッファとしておいておくもの。 @todo デバッグメニューにメモリを多く必要なのでバッファを減らしている。
static const u32 DEBUG_HEAP_USE_BUFFER              = 0x400000;   // デバッグヒープとして使用する領域

// 追加した16MB
static const u32 TOTAL_ADD_MEMORY_NIJI_16MB         = APP_DEVICE_HEAP + CHINA_FONT_DEVICE_HEAP + BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP + DEVICE_MEMORY_BUFFER + DEBUG_HEAP_USE_BUFFER;

// HEAPID_DEVICEから追加確保する
static const u32 ADD_DEVICE_TOTAL_ADD_MEMORY_NIJI   = TOTAL_ADD_MEMORY_NIJI_16MB  - DEBUG_HEAP_USE_BUFFER/*- DEVICE_MEMORY_BUFFER*/;  // @note いざとなったら、DEVICE_MEMORY_BUFFERをプログラムメモリに


//---------------------------------------------------
//  デバッグメモリ　DEBUG_HEAP
//  DEBUG_PROGRAMはDEBUG,DEVELOP構成時にプログラム領域に割り当てるサイズ
//  大きくしすぎると、ロム起動時に動かなくなる事に気づかなくなるので注意
//---------------------------------------------------
#if 1

  static const u32 DEBUG_PROGRAM_SIZE   = 0x0200000;
#if PM_ENABLE_OPTIMIZE
  #if PM_DEBUG_PRINT
  static const u32 DEBUG_PROGRAM         = DEBUG_PROGRAM_SIZE; // デバッグコード用に取っておくプログラム領域(Develop/Debug)
  #else //#if PM_DEBUG_PRINT
  static const u32 DEBUG_PROGRAM         = 0x0;       // デバッグコード用に取っておくプログラム領域(Release or Final)
  #endif //#if PM_DEBUG_PRINT
#else //#if PM_ENABLE_OPTIMIZE
  static const u32 DEBUG_PROGRAM         = DEBUG_PROGRAM_SIZE; // デバッグコード用に取っておくプログラム領域(Develop/Debug)
#endif  //#if PM_ENABLE_OPTIMIZE

  static const u32 DEBUG_HEAP            = 0x0100000 + DEBUG_HEAP_USE_BUFFER; // 商品の実機でも動作するデバッグ用のヒープメモリ。 Release Finalでは、デバッグコード用に確保していたヒープを使用

  static const size_t DEBUG_DEBUGGER_178M   = 0x7200000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //64M
  static const size_t DEBUG_DEBUGGER_124M   = 0x3C00000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //64M
  static const size_t DEBUG_DEBUGGER_96M    = 0x2000000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //32M
  static const size_t DEBUG_DEBUGGER_80M    = 0x1000000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //16M
  static const u32 DEBUG_FS_GASC                   = 0x0040000;  // HEAPID_DEBUG_FS_GASC                 
  static const u32 DEBUG_FS_LOG                    = 0x0001000;  // HEAPID_DEBUG_FS_LOG                  
  static const u32 DEBUG_FS_HIO_INITIALIZE_DEVICE  = 0x0014200;  // HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE
  static const u32 DEBUG_FS_HIO                    = 0x0008000;  // HEAPID_DEBUG_FS_HIO                  
  static const u32 DEBUG_FS_CALLER                 = 0x0005000;  // HEAPID_DEBUG_FS_CALLER
  static const u32 DEBUG_FS_DIRECT_SDMC_DEVICE     = 0x0000400;  // HEAPID_DEBUG_FS_DIRECT_SDMC_DEVICE
  static const u32 DEBUG_FS_MATCH_CHECK            = 0x0001B00;  // HEAPID_DEBUG_FS_MATCH_CHECK  // 1個84バイト(MatchCheckReqElem36バイト+gfl2::heapヒープ管理領域32バイト+nn::fndヒープ管理領域16バイト)

  static const u32 DEBUG_DYNAMIC_HEADER_DEVICE     = 0x0300000;  // HEAPID_DEBUG_DYNAMIC_HEADER_DEVICE

#if defined(GF_PLATFORM_WIN32)
  static const u32 DEBUG_DEVICE        = 0x2000000;
#endif

#else // PM_DEBUG
  static const u32 DEBUG_HEAP            = 0; 
#endif // PM_DEBUG
//---------------------------------------------------
//---------------------------------------------------


//---------------------------------------------------
//  メインヒープとデバイス HEAP_MEMORY,DEVICE_MEMORY
//  @todo CTRは一旦sangoサイズ
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 WIN32_WORK_MEMORY = 1024*1024*64 * 3;// WIN32版限定機能で使用する常駐ヒープ 150520 ikuta_junya
#endif

/**
 *  @note HEAP_MEMORY のサイズ　
 *    1.NETWORK_RESIDENT サイズ
 *    2.マイクバッファ分のヒープを確保
 *    3.RoModuleのcrsとcrrのバッファ  crs 300KB crr 300KB  と考える。 余裕を見て1MB取っておく。
 *    4.RoModuleのcro 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY
 */
static const u32 HEAP_MEMORY      = 0x14F000 + 0x1000 + 0x100000 + 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY; // 
static const u32 DEVICE_MEMORY    = 0x3470000+ ADD_DEVICE_TOTAL_ADD_MEMORY_NIJI + DEBUG_HEAP;

//---------------------------------------------------
//  常駐メモリ RESIDENT  
//  parent::HEAPID_SYSTEM
//  確保されるものはNETWORK_RESIDENTのみ
//---------------------------------------------------
static const u32 RESIDENT         = 0x14F000; // 0x5ED000 - 0x14F000 = 0x49E000; 

//---------------------------------------------------
//  常駐メモリ RESIDENT_DEVICE
//  parent::HEAPID_DEVICE
//---------------------------------------------------
static const u32 RESIDENT_DEVICE  = 0x0EB8000 - 0x20000 - 0x10000 + 0x40000 + 0x49E000 + CHINA_FONT_DEVICE_HEAP;

//---------------------------------------------------
//  常駐メモリ DLL_LOAD
//  parent::HEAPID_SYSTEM
//  DLLの読み込みに使用
//---------------------------------------------------
static const u32 DLL_LOAD         = 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY;

//---------------------------------------------------
//  常駐メモリ　APP
//  parent::HEAPID_SYSTEM
//  アプリケーションの親ヒープ APP
//  マイク用バッファの生成にのみ使用
//---------------------------------------------------
static const u32 APP              = 0x1000; // 0x504000 - 0x1000 = 0x503000;

//---------------------------------------------------
//  常駐メモリ　APP_DEVICE
//  parent::HEAPID_DEVICE
//  アプリケーションの親ヒープ
//---------------------------------------------------
static const u32 APP_DEVICE       = 0x1C60000 + APP_DEVICE_HEAP + 0x440000;

//---------------------------------------------------
//  個々の常駐メモリサイズ
//---------------------------------------------------

//---------------------------------------------------
//  常駐メモリ　PROCCELL
//  parent::HEAPID_RESIDENT_DEVICE
//  プロック中で使うべきではない
//  ProcBaseProcessやGameEventクラスの生成時に利用　
//---------------------------------------------------
static const u32 PROCCELL         = 1024*20;    ///< HEAPID_PROCCELLのサイズ

//---------------------------------------------------
//  イベント　EVENT_DEVICE
//  parent::HEAPID_RESIDENT_DEVIDE
//---------------------------------------------------
static const u32 EVENT_DEVICE    = 0x3B2000;   ///< イベント中に使用するデバイス

//---------------------------------------------------
//  現在未使用
//---------------------------------------------------
//static const u32 APP_CONTROL     = 0x1400;     ///< イベントとイベントをまたいで保持されるメモリ

//---------------------------------------------------
//  常駐メモリ　FILE_READ
//  parent::HEAPID_RESIDENT_DEVIDE
//---------------------------------------------------
static const u32 FILE_READ       = 0x200000;   ///< ファイルの非同期読み込みの作業ワーク

//---------------------------------------------------
//  現在未使用
//---------------------------------------------------
//static const u32 AUTO_STACK      = 0x1E000;    ///< AutoStackManager用メモリ。4096の倍数であること

//-----------------------------------------------------------------------------
//  SYSTEMFONTに使用するメモリ
//  HEAPID_SYSTEM_FONT, HEAPID_SYSTEM_FONT_DEVICE
//  @todo 3DS版はsangoと同じ。
//-----------------------------------------------------------------------------
static const u32 SYSTEM_FONT_SIZE        = 0x600;   //ARC随時読みの保証..
static const u32 SYSTEM_FONT_DEVICE_SIZE = 0x150d00 + 0x8000+ 0x3800 + 0x40000 + 0x150000 + CHINA_FONT_DEVICE_HEAP;  //bffntになって増えた分0x40000

//---------------------------------------------------
//  フィールドアプリメモリ　HEAP_FIELD
//---------------------------------------------------

// ・sango
// HEAP_FIELD_DEVICE_SIZE = 24 * 1024 * 1024 - 4 * 1024 * 1024 // ( フィールド全体(下画面は別) - フィールドレジデント)
static const u32 HEAP_FIELD_SIZE          = 0x100000;
static const u32 HEAP_FIELD_DEVICE_SIZE   = 20 * 1024 * 1024 + APP_DEVICE_HEAP + (0x180000) + 0x300000;    // 20MB(sango) + 5MB + 1.5MB（下画面） + RESIDENT(4M中3M)

//---------------------------------------------------
//  フィールドアプリ下画面メニューメモリ　HEAP_FIELD_SUBSCREEN
//---------------------------------------------------
// @note このヒープ親はAPP,APP_DEVICE
static const u32 HEAP_FIELD_SUBSCREEN_SIZE          = 100 * 1024;           // 100k
static const u32 HEAP_FIELD_SUBSCREEN_DEVICE_SIZE   = 1.5 * 1024 * 1024;    // 1.5MB

//---------------------------------------------------
//  フィールドスクリプトメモリ　FIELD_SCRIPT_HEAP
//---------------------------------------------------
static const u32 HEAP_FIELD_SCRIPT_SIZE = 0x5d000;
static const u32 HEAP_FIELD_SCRIPT_FILEREAD_SIZE = 0x13A00;
static const u32 HEAP_FIELD_SCRIPT_WORK_SIZE = 0x4000 + 0x10000; // + IEMessage, AIMessage

//---------------------------------------------------
//  サウンドメモリ　SOUND_HEAP
//---------------------------------------------------
static const u32 HEAP_SOUND = 0x10000;
static const u32 HEAP_SOUND_DEVICE = 0x183800;

//-----------------------------------------------------------------------------
/**
  *      通信に使用するメモリ
  */
//-----------------------------------------------------------------------------
  static const u32 NETWORK_RESIDENT = 0x142420;
  static const u32 NETWORK_DEVICE = 0x200000;

  static const u32 ERROR_APPLET_SYSTEM = 0x10000;   /// ERROR/EULA APPLETメッセージ
  static const u32 ERROR_APPLET_MESSAGE = 0x3C00;   /// ERROR/EULA APPLETメッセージDEV

};

GFL_NAMESPACE_END(System)

#endif  //__H_HEAP_SIZE_H__

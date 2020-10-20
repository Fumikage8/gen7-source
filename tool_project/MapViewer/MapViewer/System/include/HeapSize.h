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
#if defined(GF_PLATFORM_WIN32)
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY   = 0x0;
#elif defined(GF_PLATFORM_CTR)
  #if PM_ENABLE_OPTIMIZE
    #if PM_DEBUG_PRINT
    static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x4000;
    #else //#if PM_DEBUG_PRINT
    static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x0;
    #endif //#if PM_DEBUG_PRINT
  #else //#if PM_ENABLE_OPTIMIZE
    static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x60000;
  #endif  //#if PM_ENABLE_OPTIMIZE
#endif  //defined(GF_PLATFORM_CTR)

public:

//---------------------------------------------------
//  メインヒープとデバイス HEAP_MEMORY,DEVICE_MEMORY
//  @todo CTRは一旦sangoサイズ
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 HEAP_MEMORY      = 0xA00000+0x2000000;  //1024*1024*10+1024*1024*32;
  //static const u32 DEVICE_MEMORY    = 1024*1024*64;
  static const u32 DEVICE_MEMORY    = 1024*1024*64 * 3;// 150317 tomoya 
  static const u32 WIN32_WORK_MEMORY = 1024*1024*64 * 3;// WIN32版限定機能で使用する常駐ヒープ 150520 ikuta_junya
#elif defined(GF_PLATFORM_CTR)
  static const u32 DEBUG_APP_DEVICE_HEAP = 0x0500000; // アプリケーションデバイスが使用

  static const u32 HEAP_MEMORY      = 0x0cc0000 + 0x30000 + FOR_DISABLE_OPTIMIZE_MEMORY;
  static const u32 DEVICE_MEMORY    = 0x02b78000 - 0x30000 + 0x40000 + DEBUG_APP_DEVICE_HEAP;
#endif  //defined(GF_PLATFORM_CTR)

//---------------------------------------------------
//  常駐メモリ RESIDENT
//  parent::HEAPID_SYSTEM
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 RESIDENT         = 0x800000; //1024*1024*8;
  static const u32 RESIDENT_DEVICE  = 0x1000000;  //1024*1024*16
  static const u32 DLL_LOAD         = 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY;  //Winでは不要だけど・・・
#elif defined(GF_PLATFORM_CTR)
  static const u32 RESIDENT         = 0x5ED000; //0x05c0000 + 0x4000 + 0x19000 + 0x10000;
  static const u32 RESIDENT_DEVICE  = 0x0EB8000 - 0x20000 - 0x10000 + 0x40000;
  static const u32 DLL_LOAD         = 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY;
#endif  //defined(GF_PLATFORM_CTR)

//---------------------------------------------------
//  アプリケーションの親ヒープ APP
//  parent::HEAPID_SYSTEM
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 APP              = 0x1000000;  //1024*1024*16;
  //static const u32 APP_DEVICE       = 0x3000000;  //1024*1024*46;
  static const u32 APP_DEVICE       = 0x3000000 * 3;  //1024*1024*46 150317 tomoya ;
#elif defined(GF_PLATFORM_CTR)
  static const u32 APP              = 0x504000;
  static const u32 APP_DEVICE       = 0x1C60000 + DEBUG_APP_DEVICE_HEAP;
#endif  //defined(GF_PLATFORM_CTR)

//---------------------------------------------------
//  個々の常駐メモリサイズ
//---------------------------------------------------

//---------------------------------------------------
//  ProcBaseProcessやGameEventクラスの生成時に利用　PROCCELL
//  parent::HEAPID_RESIDENT,HEAPID_RESIDENT_DEVIDE
//  ブロック中で使うべきではない
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 PROCCELL         = 1024*20;    ///< HEAPID_PROCCELLのサイズ
#elif defined(GF_PLATFORM_CTR)
  static const u32 PROCCELL         = 1024*20;    ///< HEAPID_PROCCELLのサイズ
#endif  //defined(GF_PLATFORM_CTR)

//---------------------------------------------------
//  イベント　EVENT
//  parent::HEAPID_RESIDENT,HEAPID_RESIDENT_DEVIDE
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 EVENT           = 0x20000;    ///< イベント中に使用する
  static const u32 EVENT_DEVICE    = 0x392000;   ///< イベント中に使用するデバイス
  static const u32 APP_CONTROL     = 0x1400;     ///< イベントとイベントをまたいで保持されるメモリ
  static const u32 FILE_READ       = 0x200000;   ///< ファイルの非同期読み込みの作業ワーク
  static const u32 AUTO_STACK      = 0x1E000;    ///< AutoStackManager用メモリ。4096の倍数であること

#elif defined(GF_PLATFORM_CTR)
  static const u32 EVENT           = 0x20000;    ///< イベント中に使用する
  static const u32 EVENT_DEVICE    = 0x392000;   ///< イベント中に使用するデバイス
  static const u32 APP_CONTROL     = 0x1400;     ///< イベントとイベントをまたいで保持されるメモリ
  static const u32 FILE_READ       = 0x200000;   ///< ファイルの非同期読み込みの作業ワーク
  static const u32 AUTO_STACK      = 0x1E000;    ///< AutoStackManager用メモリ。4096の倍数であること
#endif  //defined(GF_PLATFORM_CTR)

//-----------------------------------------------------------------------------
//  SYSTEMFONTに使用するメモリ
//  HEAPID_SYSTEM_FONT, HEAPID_SYSTEM_FONT_DEVICE
//  @todo 3DS版はsangoと同じ。
//-----------------------------------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  static const u32 SYSTEM_FONT_SIZE        = 0x600;   //ARC随時読みの保証..
  static const u32 SYSTEM_FONT_DEVICE_SIZE = 0x150d00 + 0x8000+ 0x3800 + 0x40000;  //bffntになって増えた分0x40000
#elif defined(GF_PLATFORM_WIN32)
  static const u32 SYSTEM_FONT_SIZE        = 0x600;   //ARC随時読みの保証..
  static const u32 SYSTEM_FONT_DEVICE_SIZE = 0x300000;
#endif

//---------------------------------------------------
//  フィールドアプリメモリ　HEAP_FIELD
//---------------------------------------------------

// ・sango
// HEAP_FIELD_DEVICE_SIZE = 24 * 1024 * 1024 - 4 * 1024 * 1024 // ( フィールド全体(下画面は別) - フィールドレジデント)

#if defined(GF_PLATFORM_CTR)
  static const u32 HEAP_FIELD_SIZE          = 0x100000;
  static const u32 HEAP_FIELD_DEVICE_SIZE   = 20 * 1024 * 1024 + DEBUG_APP_DEVICE_HEAP;    // 20MB(sango) + 5MB
#elif defined(GF_PLATFORM_WIN32)
  static const u32 HEAP_FIELD_SIZE          = 0x400000;
  static const u32 HEAP_FIELD_DEVICE_SIZE   = 0x2A00000 * 3;    // 親のサイズ0x3000000 150317 tomoya 
#endif

//---------------------------------------------------
//  フィールドアプリ下画面メニューメモリ　HEAP_FIELD_SUBSCREEN
//---------------------------------------------------
// @note このヒープ親はAPP,APP_DEVICE
#if defined(GF_PLATFORM_CTR)
  static const u32 HEAP_FIELD_SUBSCREEN_SIZE          = 100 * 1024;           // 100k
  static const u32 HEAP_FIELD_SUBSCREEN_DEVICE_SIZE   = 1.5 * 1024 * 1024;    // 1.5MB
#elif defined(GF_PLATFORM_WIN32)
  static const u32 HEAP_FIELD_SUBSCREEN_SIZE          = 100 * 1024 * 3;          // 100k(CTR) * 3
  static const u32 HEAP_FIELD_SUBSCREEN_DEVICE_SIZE   = 1.5 * 1024 * 1024 * 3;   // 1.5MB(CTR) * 3
#endif

  //---------------------------------------------------
//  フィールドスクリプトメモリ　FIELD_SCRIPT_HEAP
//---------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  static const u32 HEAP_FIELD_SCRIPT_SIZE = 0x5d000;
  static const u32 HEAP_FIELD_SCRIPT_FILEREAD_SIZE = 0x13A00;
  static const u32 HEAP_FIELD_SCRIPT_WORK_SIZE = 0x4000 + 0x10000; // + IEMessage, AIMessage
#elif defined(GF_PLATFORM_WIN32)
  static const u32 HEAP_FIELD_SCRIPT_SIZE = 0x5d000;
  static const u32 HEAP_FIELD_SCRIPT_FILEREAD_SIZE = 0x13A00;
  static const u32 HEAP_FIELD_SCRIPT_WORK_SIZE = 0x4000 + 0x10000; // + IEMessage, AIMessage
#endif

//---------------------------------------------------
//  サウンドメモリ　SOUND_HEAP
//---------------------------------------------------
#if defined(GF_PLATFORM_CTR)
  static const u32 HEAP_SOUND = 0x10000;
  static const u32 HEAP_SOUND_DEVICE = 0x183800;
#elif defined(GF_PLATFORM_WIN32)
  static const u32 HEAP_SOUND = 0x10000*2;
  static const u32 HEAP_SOUND_DEVICE = 0x183800*2;
#endif

//---------------------------------------------------
//  デバッグメモリ　DEBUG_HEAP
//  DEBUG_PROGRAMはDEBUG,DEVELOP構成時にプログラム領域に割り当てるサイズ
//  大きくしすぎると、ロム起動時に動かなくなる事に気づかなくなるので注意
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 DEBUG_HEAP          = 0x0100000;
  static const u32 DEBUG_PROGRAM       = 0x0200000;
  static const u32 DEBUG_DEVICE        = 0x2000000 - DEBUG_HEAP - DEBUG_PROGRAM;
#elif defined(GF_PLATFORM_CTR)
  static const u32 DEBUG_HEAP            = 0x0100000;
  static const u32 DEBUG_PROGRAM         = 0x0200000;
  static const u32 DEBUG_DEVICE_124M   = 0x3C00000 - DEBUG_HEAP - DEBUG_PROGRAM - DEBUG_APP_DEVICE_HEAP;  //64M
  static const u32 DEBUG_DEVICE_96M    = 0x2000000 - DEBUG_HEAP - DEBUG_PROGRAM - DEBUG_APP_DEVICE_HEAP;  //32M
  static const u32 DEBUG_DEVICE_80M    = 0x1000000 - DEBUG_HEAP - DEBUG_PROGRAM - DEBUG_APP_DEVICE_HEAP;  //16M
#endif  //defined(GF_PLATFORM_CTR)
  static const u32 DEBUG_FS_GASC                   = 0x0040000;  // HEAPID_DEBUG_FS_GASC                 
  static const u32 DEBUG_FS_LOG                    = 0x0001000;  // HEAPID_DEBUG_FS_LOG                  
  static const u32 DEBUG_FS_HIO_INITIALIZE_DEVICE  = 0x0014200;  // HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE
  static const u32 DEBUG_FS_HIO                    = 0x0008000;  // HEAPID_DEBUG_FS_HIO                  
  static const u32 DEBUG_FS_CALLER                 = 0x0005000;  // HEAPID_DEBUG_FS_CALLER
  static const u32 DEBUG_FS_DIRECT_SDMC_DEVICE     = 0x0000400;  // HEAPID_DEBUG_FS_DIRECT_SDMC_DEVICE


//-----------------------------------------------------------------------------
/**
  *      通信に使用するメモリ
  */
//-----------------------------------------------------------------------------
  static const u32 NETWORK_RESIDENT = 0x142420;
  static const u32 NETWORK_DEVICE = 0x400000;

  static const u32 ERROR_APPLET_SYSTEM = 0x10000;   /// ERROR/EULA APPLETメッセージ
  static const u32 ERROR_APPLET_MESSAGE = 0x3C00;   /// ERROR/EULA APPLETメッセージDEV

};

GFL_NAMESPACE_END(System)

#endif  //__H_HEAP_SIZE_H__

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
#if defined(GF_CONFIG_DEBUG)
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x100000;
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG = 0xB000;
#elif defined(GF_CONFIG_DEVELOP)
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x30000;
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG = 0x1000;
#elif defined(GF_CONFIG_RELEASE)
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x0;
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG = 0x0;
#elif defined(GF_CONFIG_FINAL)
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY = 0x0;
  static const u32 FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG = 0x0;
#endif

public:

/**
 *  nijiProject 16MB増加の内訳
 */
static const u32 APP_DEVICE_HEAP                    = 0x500000;     // アプリケーションデバイスが使用
static const u32 CHINA_FONT_DEVICE_HEAP             = 0x380000;    // 中国語フォント 0x380000byeを予定
static const u32 BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP  = 0x300000; // バトル遷移高速化  0x300000byteを予定
static const u32 SOUND_DEVICE_MEMORY_ADD            = 180 * 1024; // サウンドのメモリ追加分
static const u32 DEVICE_MEMORY_BUFFER               = 0x300000 - SOUND_DEVICE_MEMORY_ADD;      // バッファとしておいておくもの。 @note デバッグメニューにメモリを多く必要なのでバッファを減らしている。
static const u32 DEBUG_HEAP_USE_BUFFER              = 0x180000;   // デバッグヒープとして使用する領域

// 追加した16MB
static const u32 TOTAL_ADD_MEMORY_NIJI_16MB         = APP_DEVICE_HEAP + CHINA_FONT_DEVICE_HEAP + BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP + DEVICE_MEMORY_BUFFER + DEBUG_HEAP_USE_BUFFER + SOUND_DEVICE_MEMORY_ADD;

//===================================================================================
// @note いざとなったら、DEVICE_MEMORY_BUFFERをプログラムメモリに
// STATIC_PROGRAM_DATA_SIZE + DLL_PROGRAM_ADD_SIZE よりもDEVICE_MEMORY_BUFFERのサイズよりも小さい必要がある。

// 常駐プログラムのヒープに使用するサイズ
static const u32 STATIC_PROGRAM_DATA_SIZE = 0x10000;  
                                                      
// DLLヒープに加算するサイズ
static const u32 DLL_PROGRAM_ADD_SIZE = 0x14000+0x4000;

// FONTヒープからプログラム領域に持っていくメモリサイズ
static const u32 FONT_TO_PROGRAM_MEM_SIZE = 0x140000;

//===================================================================================

// HEAPID_DEVICEから追加確保する
static const u32 ADD_DEVICE_TOTAL_ADD_MEMORY_NIJI = TOTAL_ADD_MEMORY_NIJI_16MB - DEBUG_HEAP_USE_BUFFER - STATIC_PROGRAM_DATA_SIZE - DLL_PROGRAM_ADD_SIZE - FONT_TO_PROGRAM_MEM_SIZE;


//---------------------------------------------------
//  デバッグメモリ　DEBUG_HEAP
//  DEBUG_PROGRAMはDEBUG,DEVELOP構成時にプログラム領域に割り当てるサイズ
//  大きくしすぎると、ロム起動時に動かなくなる事に気づかなくなるので注意
//---------------------------------------------------
#if PM_DEBUG

#if defined(GF_CONFIG_DEBUG)
  static const u32 DEBUG_PROGRAM         = 0x0200000 + FOR_DISABLE_OPTIMIZE_MEMORY + FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG; // デバッグコード用に取っておくプログラム領域(Debug)
#elif defined(GF_CONFIG_DEVELOP)
  static const u32 DEBUG_PROGRAM         = 0x040000 + FOR_DISABLE_OPTIMIZE_MEMORY + FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG;  // デバッグコード用に取っておくプログラム領域(Develop)
#elif defined(GF_CONFIG_RELEASE)
  static const u32 DEBUG_PROGRAM         = 0x0;       // デバッグコード用に取っておくプログラム領域(Release)
  //@note プロファイラで動作させるときは下記のコメントアウトを有効にする。
  //static const u32 DEBUG_PROGRAM         = 0x0200000 + FOR_DISABLE_OPTIMIZE_MEMORY + FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG;       // デバッグコード用に取っておくプログラム領域(プロファイリング時の設定)
#elif defined(GF_CONFIG_FINAL)
  static const u32 DEBUG_PROGRAM         = 0x0;       // デバッグコード用に取っておくプログラム領域(Final)
#endif

  static const u32 DEBUG_HEAP            = 0x0100000 + DEBUG_HEAP_USE_BUFFER; // 商品の実機でも動作するデバッグ用のヒープメモリ。 Release Finalでは、デバッグコード用に確保していたヒープを使用

  static const size_t DEBUG_DEBUGGER_178M   = 0x7200000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //64M
  static const size_t DEBUG_DEBUGGER_124M   = 0x3C00000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //64M
  static const size_t DEBUG_DEBUGGER_96M    = 0x2000000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //32M
  static const size_t DEBUG_DEBUGGER_80M    = 0x1000000 - DEBUG_PROGRAM - TOTAL_ADD_MEMORY_NIJI_16MB;  //16M
  static const u32 DEBUG_FS_GASC                   = 0x0200000;  // HEAPID_DEBUG_FS_GASC                 
  static const u32 DEBUG_FS_LOG                    = 0x000C000;  // HEAPID_DEBUG_FS_LOG                  
  static const u32 DEBUG_FS_HIO_INITIALIZE_DEVICE  = 0x0014200;  // HEAPID_DEBUG_FS_HIO_INITIALIZE_DEVICE  // これだけは必ずデバイスメモリで。
  static const u32 DEBUG_FS_HIO                    = 0x0008000;  // HEAPID_DEBUG_FS_HIO                  
  static const u32 DEBUG_FS_CALLER                 = 0x0002B00;  // HEAPID_DEBUG_FS_CALLER
  static const u32 DEBUG_FS_DIRECT_SDMC            = 0x0003300;  // HEAPID_DEBUG_FS_DIRECT_SDMC
  static const u32 DEBUG_FS_MATCH_CHECK            = 0x0001B00;  // HEAPID_DEBUG_FS_MATCH_CHECK  // 1個84バイト(MatchCheckReqElem36バイト+gfl2::heapヒープ管理領域32バイト+nn::fndヒープ管理領域16バイト)
  static const u32 DEBUG_FS_INTERVAL_CHECK         = 0x0001900;  // HEAPID_DEBUG_FS_INTERVAL_CHECK  // 1個64バイト(IntervalCheckInfo16バイト+gfl2::heapヒープ管理領域32バイト+nn::fndヒープ管理領域16バイト)

  static const u32 DEBUG_DYNAMIC_HEADER_DEVICE     = 0x0300000;  // HEAPID_DEBUG_DYNAMIC_HEADER_DEVICE

#if defined(GF_PLATFORM_WIN32)
  static const u32 DEBUG_DEVICE        = 0x2000000;
#endif


  static const u32 DEBUG_DLL_LOAD   = 0x3C000 + FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG;  // デバッグプログラムDLL読み込み領域

#else // PM_DEBUG
  static const u32 DEBUG_HEAP            = 0; 
#endif // PM_DEBUG
//---------------------------------------------------
//---------------------------------------------------


//---------------------------------------------------
//  メインヒープとデバイス HEAP_MEMORY,DEVICE_MEMORY
//---------------------------------------------------
#if defined(GF_PLATFORM_WIN32)
  static const u32 WIN32_WORK_MEMORY = 1024*1024*64 * 3;// WIN32版限定機能で使用する常駐ヒープ 150520 ikuta_junya
#endif


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
static const u32 RESIDENT_DEVICE = 0x0EB8000 - 0x20000 - 0x10000 + 0xA0000 + 0x49E000 + 0xC0000 + CHINA_FONT_DEVICE_HEAP + BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP + SOUND_DEVICE_MEMORY_ADD - FONT_TO_PROGRAM_MEM_SIZE;

//---------------------------------------------------
//  常駐メモリ DLL_LOAD
//  parent::HEAPID_SYSTEM
//  DLLの読み込みに使用
//---------------------------------------------------
static const u32 DLL_LOAD         = 0x01b8000 + FOR_DISABLE_OPTIMIZE_MEMORY + DLL_PROGRAM_ADD_SIZE;

/**
 *  @note HEAP_MEMORY のサイズ　
 *    1.NETWORK_RESIDENT サイズ
 *    2.マイクバッファ分のヒープを確保
 *    3.RoModuleのcrsとcrrのバッファ  crs 300KB crr 300KB  と考える。 余裕を見て1MB取っておく。
 *    4.RoModuleのcro 0x01b8000+FOR_DISABLE_OPTIMIZE_MEMORY
 */
static const u32 HEAP_MEMORY      = RESIDENT + 0x2000 + 0x100000 + DLL_LOAD + FOR_DISABLE_OPTIMIZE_MEMORY_DEBUG; // 
static const u32 DEVICE_MEMORY = 0x3470000 + ADD_DEVICE_TOTAL_ADD_MEMORY_NIJI + DEBUG_HEAP;

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
static const u32 APP_DEVICE       = 0x1C60000 + APP_DEVICE_HEAP + 0x440000 + 0xC0000;

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
static const u32 EVENT_DEVICE    = 0x3B2000 + 0xA0000;   ///< イベント中に使用するデバイス
static const u32 EVENT_HEAP    = 0x20000;   ///< GameEvent生成用

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
//  AutoStackManager
//---------------------------------------------------
static const u32 HEAPSIZE_AUTO_STACK      = 0x1E000;    ///< AutoStackManager用メモリ。4096の倍数であること
  
  
//-----------------------------------------------------------------------------
//  SYSTEMFONTに使用するメモリ
//  HEAPID_SYSTEM_FONT, HEAPID_SYSTEM_FONT_DEVICE
//-----------------------------------------------------------------------------
static const u32 SYSTEM_FONT_SIZE        = 0x600;   //ARC随時読みの保証..

// momijiで0x30000減らした
// →2017/04/13　更に0x100000 減らした(定義はFONT_TO_PROGRAM_MEM_SIZE)
static const u32 SYSTEM_FONT_DEVICE_SIZE = 0x150d00 + 0x8000 + 0x3800 + 0x40000 + 0x150000 + CHINA_FONT_DEVICE_HEAP - FONT_TO_PROGRAM_MEM_SIZE;  //bffntになって増えた分0x40000

//---------------------------------------------------
//  フィールドアプリメモリ　HEAP_FIELD
//---------------------------------------------------
// AREA用に取っておくヒープサイズ
// ※下記の４つの定数を変更した場合、R:\home\momiji\convert\field_convert\world\area\omakeの変更も忘れずに
static const u32 AREA_DATA_SIZE_AND_BUFFER_MAX      = 0xB80000; 
static const u32 HEAP_FIELD_BATTLE_MODEL_HEAP_SIZE  = 0x480000;
static const u32 HEAP_FIELD_JOINFESTA_HEAP_SIZE     = 0x920000;
static const u32 HEAP_FIELD_MIDDLE_HEAP_SIZE        = 0x200000;

// ・sango
// HEAP_FIELD_DEVICE_SIZE = 24 * 1024 * 1024 - 4 * 1024 * 1024 // ( フィールド全体(下画面は別) - フィールドレジデント)
static const u32 HEAP_FIELD_SIZE          = 0x100000;
static const u32 HEAP_FIELD_DEVICE_SIZE   = 20 * 1024 * 1024 + APP_DEVICE_HEAP + (0x180000) + 0x300000 + 0x500000 - AREA_DATA_SIZE_AND_BUFFER_MAX + 0x40000;    // 20MB(sango) + 5MB + 1.5MB（下画面） + RESIDENT(4M中3M)
//↑momijiで0x40000追加（主要な使用目的は配置人物を増やすため）

//---------------------------------------------------
//  フィールドアプリ下画面メニューメモリ　HEAP_FIELD_SUBSCREEN
//---------------------------------------------------
// @note このヒープ親はAPP,APP_DEVICE
static const u32 HEAP_FIELD_SUBSCREEN_SIZE          = 100 * 1024;           // 100k
static const u32 HEAP_FIELD_SUBSCREEN_DEVICE_SIZE   = (1.5 * 1024 * 1024) + 0x20000 + 0x90000;    // 1.5MB + momiji(0x90000)

//---------------------------------------------------
//  フィールドスクリプトメモリ　FIELD_SCRIPT_HEAP
//---------------------------------------------------
static const u32 HEAP_FIELD_SCRIPT_SIZE = 0x5d000;
static const u32 HEAP_FIELD_SCRIPT_FILEREAD_SIZE = 0x14000;
static const u32 HEAP_FIELD_SCRIPT_WORK_SIZE = 0x10000;

//---------------------------------------------------
//  サウンドメモリ　SOUND_HEAP
//---------------------------------------------------
static const u32 HEAP_SOUND = 0x10000;
static const u32 HEAP_SOUND_DEVICE = 0x195000 + SOUND_DEVICE_MEMORY_ADD;

//---------------------------------------------------
//  バトル遷移用ヒープ　HEAPID_BATTLE_TRANSITION(3MB)
//---------------------------------------------------
static const u32 HEAP_SIZE_BATTLE_TRANSITION = BATTLE_SCENE_CHANGE_OPTIMIZE_HEAP;

//-----------------------------------------------------------------------------
/**
  *      通信に使用するメモリ
  */
//-----------------------------------------------------------------------------
static const u32 NETWORK_RESIDENT = 0x142420;
static const u32 NETWORK_DEVICE = 0x200000;

static const u32 ERROR_APPLET_SYSTEM = 0x10000;   /// ERROR/EULA APPLETメッセージ
static const u32 ERROR_APPLET_MESSAGE = 0x3C00;   /// ERROR/EULA APPLETメッセージDEV

static const u32 JOIN_FESTA_PERSONAL = 0x14A00;   //< JoinFestaパーソナルデータ確保用（常駐） JoinFestaPersonalData×132

//-----------------------------------------------------------------------------
/**
  *      セーブデータクラス用
  */
//-----------------------------------------------------------------------------
static const u32 SAVE_DATA_HEAP_SIZE = 512 * 1024; // 512KByte

};

GFL_NAMESPACE_END(System)

#endif  //__H_HEAP_SIZE_H__

//===================================================================
/**
 * @file    CTRApplication.cpp
 * @brief   CTRアプリケーション メインフレームワーク
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

#if defined(GF_PLATFORM_CTR)

#include <wchar.h>
#include <nn/applet.h>

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <NetStatic/NetLib/include/Error/NijiNetworkErreulaApplet.h>
#include <base/include/gfl2_Singleton.h>
#include <ro/include/gfl2_RoManager.h>
#include <System/include/PokemonVersion.h>

#include "System/include/HeapSize.h"

namespace Main{

#if PM_DEBUG
// 例外キャッチハンドラの多重割り込みを防ぐためのクリティカルオブジェクト
static nn::os::CriticalSection s_criticalSection;
static void nijiExceptionHandler( nn::os::ARM::ExceptionInfo *pei, nn::os::ARM::ExceptionContext *pec);

#endif  //PM_DEBUG

//=======================================================
/*
  @brief  起動モードに応じたメモリサイズ取得
 */
//=======================================================
static void SetStartupMemorySize( size_t device_memory_size, size_t heap_memory_size )
{
#if NN_PLATFORM_HAS_MMU
#pragma push
#pragma diag_suppress 1361
#pragma push
#pragma diag_suppress 177
  //size_tはunsignd intなので0以下チェックできない。なのでs32にした
  const s32 assingment   = nn::os::GetAppMemorySize();
  const s32 currentUsing = nn::os::GetUsingMemorySize();
  const s32 programsize  = nn::os::GetCodeRegionSize();

  const s32 available    = assingment - currentUsing;
  const s32 freeSize     = available - static_cast<s32>(device_memory_size + heap_memory_size);

  GFL_RELEASE_PRINT("AllMemSize           [0x%08x]\n", assingment);
  GFL_RELEASE_PRINT("├UseMemSize         [0x%08x]\n", currentUsing);
  GFL_RELEASE_PRINT("│└CodeMemSize      [0x%08x]\n", programsize);
  GFL_RELEASE_PRINT("└AvailableMemSize   [0x%08x]\n", available);
  GFL_RELEASE_PRINT("  ├AcquireHeapSize  [0x%08x]\n", device_memory_size + heap_memory_size );
  GFL_RELEASE_PRINT("  │ ├DeviceMemSize [0x%08x]\n", device_memory_size);
  GFL_RELEASE_PRINT("  │ └HeapMemSize   [0x%08x]\n", heap_memory_size);
  GFL_RELEASE_PRINT("  └FreeSize         [0x%08x]\n", freeSize );
#pragma pop
  if ( freeSize < 0 )
  { // 足りないパターン
    GFL_RELEASE_PRINT( "!!!! ProgramSizeOver [0x%08x] !!!!\n", -freeSize );
    GFL_ASSERT_STOP( freeSize >= 0 );
    while(1){}  // このタイミングではまだAssertコールバックが未指定なので停止しない。無限ループで止める
  }

#if PM_DEBUG
  if( freeSize < System::HeapSize::DEBUG_PROGRAM )
  {
    GFL_RELEASE_PRINT( "!!!! ProgramSize Warning [0x%08x]/[0x%08x] !!!!\n", freeSize, System::HeapSize::DEBUG_PROGRAM );
  }
#endif

  // ビルドにより、メモリ増強している場所を使用しているかチェック
#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
  if( freeSize < System::HeapSize::DEBUG_PROGRAM )
  {
    GFL_RELEASE_PRINT( "!!!! ProgramSize Warning [0x%08x]/[0x%08x] !!!!\n", freeSize, System::HeapSize::DEBUG_PROGRAM );
    GFL_ASSERT(freeSize >= System::HeapSize::DEBUG_PROGRAM);
  }
  GFL_RELEASE_PRINT( "STATIC_PROGRAM_DATA_SIZE + DLL_PROGRAM_ADD_SIZE[%d] < DEVICE_MEMORY_BUFFER[%d]\n", System::HeapSize::STATIC_PROGRAM_DATA_SIZE + System::HeapSize::DLL_PROGRAM_ADD_SIZE, System::HeapSize::DEVICE_MEMORY_BUFFER );

  if( freeSize < 0x10000 )
  {
    GFL_ASSERT(freeSize >= 0x10000);
    while(1){}  // 無限ループ
  }
#endif

  {
    GFL_ASSERT_STOP( heap_memory_size % 4096 == 0 );
    GFL_ASSERT_STOP( device_memory_size % 4096 == 0 );
    nn::Result result;
    result = nn::os::SetHeapSize( heap_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
    result = nn::os::SetDeviceMemorySize( device_memory_size );
    NN_UTIL_PANIC_IF_FAILED( result );
  }

#pragma pop
#endif  // if NN_PLATFORM_HAS_MMU
}

//=======================================================
/*
  @brief  mainの前に動く初期化関数の中身
 */
//=======================================================
void StartupApplication(void)
{
  size_t device_memory_size = System::HeapSize::DEVICE_MEMORY;
  size_t heap_memory_size = System::HeapSize::HEAP_MEMORY;

#if PM_DEBUG

  const size_t assingment = nn::os::GetAppMemorySize();
  switch(assingment){
  case 0xB200000: //Snake dev1=178M
    device_memory_size += System::HeapSize::DEBUG_DEBUGGER_178M;
    GFL_PRINT("SystemMode Snake dev1/dev2( enable HOME menu & Applets )\n");
    GFL_PRINT("Memory178Mb UseDebugMem114MB\n");
    break;
    break;
  case 0x7C00000: //Snake dev2=124M
    device_memory_size += System::HeapSize::DEBUG_DEBUGGER_124M;
    GFL_PRINT("SystemMode Snake dev1/dev2( enable HOME menu & Applets )\n");
    GFL_PRINT("Memory124Mb UseDebugMem60MB\n");
    break;
  case 0x6000000: //CTR dev1=96M
    device_memory_size += System::HeapSize::DEBUG_DEBUGGER_96M;
    GFL_PRINT("SystemMode CTR dev1( disable HOME menu & Applets )\n");
    GFL_PRINT("Memory96Mb UseDebugMem32MB\n");
    break;

  // @note 中国語対応により、CTR dev2はRelease or Final でしか動作しなくなる。
  case 0x5000000: //CTR dev2
    GFL_PRINT("SystemMode CTR dev2( enable HOME menu & Applets )\n");
    GFL_PRINT("Memory80Mb UseDebugMem16MB\n");
    device_memory_size += System::HeapSize::DEBUG_DEBUGGER_80M;
    break;
  case 0x4000000: //Snake/CTR prod=64M
    GFL_PRINT("SystemMode Snake/CTR prod( enable HOME menu & Applets & Browser )\n");
    GFL_PRINT("Memory64Mb UseDebugMem 0MB\n");
    break;
  default:
    GFL_PRINT("SystemMode ???\n");
    GFL_ASSERT_STOP(0);
    break;
  }
#endif //PM_DEBUG

  //メモリ領域初期化
  SetStartupMemorySize( device_memory_size, heap_memory_size );
}

//=======================================================
/*
  @brief ハード初期化関数
 */
//=======================================================
void InitializeApplication(void)
{
  //本体設定関数の初期化
  nn::cfg::Initialize();

  // PTMの初期化
  nn::ptm::CTR::Initialize();

#if PM_DEBUG

  // 例外発生時のクリティカルセクション用オブジェクトの初期化
  s_criticalSection.Initialize();

  // 例外発生キャッチハンドラ登録
  nn::os::ARM::SetUserExceptionHandler(
     nijiExceptionHandler,
     nn::os::ARM::HANDLER_STACK_USE_THREAD_STACK,
     nn::os::ARM::EXCEPTION_BUFFER_USE_THREAD_STACK
  );
#endif
}

//=======================================================
/*
  @brief ハード終了関数
 */
//=======================================================
void TerminateApplication(void)
{
  //アプリケーションクローズ
//  nn::applet::PrepareToCloseApplication();
//  nn::applet::CloseApplication();

#if PM_DEBUG
  // 例外発生時のクリティカルセクション用オブジェクトの終了
  s_criticalSection.Finalize();
#endif

  // PTMの終了処理
  nn::ptm::CTR::Finalize();

  //本体設定関数の終了処理
  nn::cfg::Finalize();
}

#if PM_DEBUG
#define DEBUG_STR_MAX ( 448 )
#define DEBUG_TMP_MAX ( 64 )
//------------------------------------------------------------------
/**
 * @brief	例外発生時のハンドラ
 */
//------------------------------------------------------------------
static void nijiExceptionHandler( nn::os::ARM::ExceptionInfo *pei, nn::os::ARM::ExceptionContext *pec)
{
  nn::os::CriticalSection::ScopedLock lock(s_criticalSection);    // 排他制御

  static wchar_t string[DEBUG_STR_MAX];
  static wchar_t temp[DEBUG_TMP_MAX];
  int type = pei->type.Get();
  u32 *sp  = (u32*)(pec->r[13]);

  // 発生した例外で別々のプリントを出す
  if(type==nn::os::ARM::EXCEPTION_TYPE_PABT||type==nn::os::ARM::EXCEPTION_TYPE_DABT){
    swprintf(string, DEBUG_STR_MAX, L"[HangUp! type=%d version=%d]\n fsr=%x, far=%x cpsr=%x PC=%08x SP=%08x\nR0-2:%08x %08x %08x\nR3-5:%08x %08x %08x\nR6-8:%08x %08x %08x\nR9-A %08x %08x", 
                       pei->type.Get(), GET_VERSION(),
                       pei->fsr, pei->far,
                       pec->cpsr, pec->r[15], pec->r[13], 
                       pec->r[0], pec->r[1], pec->r[2], 
                       pec->r[3], pec->r[4], pec->r[5], 
                       pec->r[6], pec->r[7], pec->r[8], 
                       pec->r[9], pec->r[10] ); 
  }else{
    swprintf(string, DEBUG_STR_MAX, L"type=%x, fpexc=%x fpinst=%x fpinst2=%x cpsr=%x PC=%08x SP=%08x r0=%08x r1=%08x r2=%08x r3=%08x r4=%08x r5=%08x r6=%08x r7=%08x r8=%08x r9=%08x r10=%08x", 
                       pei->type.Get(), pei->fpexc, pei->fpinst, pei->fpinst2,
                       pec->cpsr, pec->r[15],  pec->r[13],
                       pec->r[0],pec->r[1],  pec->r[2], pec->r[3], pec->r[4], pec->r[5], 
                       pec->r[6], pec->r[7], pec->r[8], pec->r[9], pec->r[10]);
  }
  NetLib::Error::NijiNetworkErreulaApplet::ErrorDisp(string);

  // スタックを表示する
  swprintf(string, DEBUG_STR_MAX, L"[stack]\n PC=%08x SP=%08x\n%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x", 
                        pec->r[15], pec->r[13], 
                        sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7], 
                        sp[8], sp[9], sp[10], sp[11], sp[12], sp[13], sp[14], sp[15]); 
  NetLib::Error::NijiNetworkErreulaApplet::ErrorDisp(string);

  
  // DLL情報の表示
  swprintf(string, DEBUG_STR_MAX, L"[dll]\n PC=%08x \n", pec->r[15] );
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  u32 registNum;
  nn::ro::RegionInfo info;
  nn::ro::Module** modules = roMgr->DebugGetModules( &registNum );
  for(int i=0;i<registNum;i++){
    modules[i]->GetRegionInfo( &info );
    swprintf(temp, DEBUG_TMP_MAX, L"%s:%08x ", modules[i]->GetName(), (u32)info.codeBegin);
    wcscat( string, temp );
  }
  NetLib::Error::NijiNetworkErreulaApplet::ErrorDisp(string);
}
#endif

} //namespace Main

#endif //defined(GF_PLATFORM_CTR)

// ============================================================================
/*
 * @file gflnet2_ICecSystem.cpp
 * @brief すれ違い通信のシステムクラスです。
 *        すれ違い通信の初期化処理や、終了処理などを行います。
 *        3DS本体のすれ違い通信ボックスを操作するインターフェースクラスを取得してから各種操作を行います。
 * @date 2015.02.13
 */
// ============================================================================
#include <gflnet2/include/cec/gflnet2_ICecSystem.h>

#if defined( GF_PLATFORM_CTR )
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecSystem.h>
#include <nn/cec.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <gflnet2/source/cec/win32/gflnet2_WIN32_CecSystem.h>
#endif

#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)


// --------------------------------------------------------------------------
/**
  * @brief  3DS版かWin32版のすれ違い通信のシステムクラスを作成します。
  * @note   作成する関数はありますが、削除する関数はありません。
  *         削除する場合はアプリ側で delete する事になります。
  * @param  pHeap すれ違い通信のシステムクラスを作成する為のヒープです。
  * @return すれ違い通信のシステムクラス
  */
// --------------------------------------------------------------------------
ICecSystem* ICecSystem::CreateCecSystem( gfl2::heap::HeapBase* pHeap )
{
  ICecSystem* pCecSystem = NULL;

#if defined( GF_PLATFORM_CTR )
  pCecSystem = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecSystem();
#elif defined( GF_PLATFORM_WIN32 )
  pCecSystem = GFL_NEW( pHeap ) gflnet2::cec::win32::WIN32_CecSystem();
#endif

  return pCecSystem;
}


#if GFL_DEBUG
// --------------------------------------------------------------------------
/**
 * @brief  すれ違い通信のすれ違い状態をリセットします。
 *         リセットする事によって８時間待たなくてもすれ違い通信行われるようになります。
 *         この機能は3DS本体のデバッグモードを有効にしないと機能しません。
 */
// --------------------------------------------------------------------------
void ICecSystem::ResetCecInfo()
{
#if GF_PLATFORM_CTR
  nn::Result result = nn::cec::CecControl::StartScanning( true );
  if( result.IsSuccess() )
  {
    GFL_PRINT( "StartScanning success\n" );
  }
  else
  {
    GFL_PRINT( "StartScanning failure\n" );
  }
#endif // GF_PLATFORM_CTR
}
#endif // GFL_DEBUG


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

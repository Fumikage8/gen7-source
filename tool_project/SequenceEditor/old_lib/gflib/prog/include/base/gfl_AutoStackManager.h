//=============================================================================
/**
 * @file   gfl_AutoStackManager.h
 * @brief  StackMemoryAutoStackManager 実装クラス
 * @author suzuki_hisanao
 * @date   2012.6.18
 */
//=============================================================================
#ifndef __GFL_AUTO_STACK_MANAGER_H__
#define __GFL_AUTO_STACK_MANAGER_H__
#pragma once

#include <nn.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>

namespace gfl {
namespace base {

/**
* @brief StackMemoryAutoStackManager 実装クラス<br>
*        <br>
*        主に<br>
*        <br>
*        　　nn::os::Thread::StartUsingAutoStack()<br>
*        　　nn::os::Thread::TryStartUsingAutoStack()<br>
*        <br>
*        コール時に使用されるスタックエリアを確保するためのスタックマネージャ。<br>
*        <br>
*        なお、nn::nex::Core::SetThreadMode()の引数に<br>
*        <br>
*        　　nn::nex::ThreadModeUnsafeTransportBuffer<br>
*        　　nn::nex::ThreadModeInternal<br>
*        <br>
*        等を指定した場合には、内部スレッドにて本マネージャ経由でスタックエリアが確保される。<br>
*        確保されるスタックエリアは4096バイトアライメントで確保される。<br>
*        また、スタック破壊時には、スタック外エリアへのアドレス参照が行われた旨のAssertが発行される。<br>
*/
class AutoStackManager : public nn::os::StackMemoryAutoStackManager
{
  GFL_FORBID_COPY_AND_ASSIGN( AutoStackManager );

public:
  /***************************************************************************
   * コンストラクタ
   * @param heap スタック領域として使用される専用ヒープエリア。<br>
   *             Nexで使用することを考慮し、デバイスヒープを指定してはならない。<br>
   *             また、本メモリを他のモジュールが使用してはならない。
   *
   * @return none
   ***************************************************************************/
  AutoStackManager( gfl::heap::HeapBase* heap );

  /***************************************************************************
   * デストラクタ
   * @return none
   ***************************************************************************/
  virtual ~AutoStackManager( void );


public:
  /***************************************************************************
   * @brief スタック用のメモリを確保する
   *
   * @param stackSize 確保すべきスタックのサイズ
   *                  サイズが4096の倍数でない場合、自動的に4096の倍数分確保される
   *
   * @return 確保したスタック用メモリの最後尾へのポインタ
   ***************************************************************************/
  virtual void* Construct( size_t stackSize );

  /***************************************************************************
   * @brief スタック用のメモリを解放する
   *
   * @param pStackBottom 解放すべきスタック。
   * @param isError エラーに伴う解放か否かを表す
   *
   * @return none
   ***************************************************************************/
  virtual void Destruct( void * pStackBottom, bool isError );

private:
  static const u32                STACK_SIZE = 0x1000;      // デストラクタ用ワークサイズ
  //
  gfl::heap::FndAllocator*        m_allocator;              // スタック用アロケータ
  nn::os::StackBuffer<STACK_SIZE> m_stackForDestruct;       // デストラクタ用ワークエリア
};



} // namespace base
} // namespace gfl



#endif // __GFL_AUTO_STACK_MANAGER_H__

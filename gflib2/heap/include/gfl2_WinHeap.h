/**
* @file    gfl2_WinHeap.h
* @brief   Windowsでの開発時に使用するヒープクラス
* @author  ikuta_junya
* @date    2014.07.15
*/

#ifndef __GFL2_WIN_HEAP__
#define __GFL2_WIN_HEAP__
#pragma once

#ifdef  GF_PLATFORM_WIN32

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "gfl2_heap_base.h"

#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えない
#include <thread/include/gfl2_mutex.h>
#endif


GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( heap );

/**
* @brief  Windowsでの開発時に使用するヒープクラス
*/
class WinHeap : public GflHeapBase
{
public:

  /**
  * @brief   コンストラクタ
  */
  WinHeap(void);

  /**
  * @brief   デストラクタ
  */
  virtual ~WinHeap(void);

  /**
  *  @brief   ヒープの作成
  *           friendでgfl::heap::Managerからのみ呼び出し
  *
  *  @param   pBuf     確保用バッファ
  *  @param   size    バッファサイズ
  *  @param   param   ユーザーパラメーター
  *
  *  @return  無し
  */
  virtual void CreateHeap( void *pBuf, const int size, void *param );

  /**
  *  @brief   ヒープの削除
  *           friendでgfl::heap::Managerからのみ呼び出し
  *
  *  @return  無し
  */
  virtual void DeleteHeap( void );

  //
  // 実際のメモリ確保・開放処理
  // 

  /**
  * @brief   メモリ確保処理
  *
  * @param   size   確保サイズ
  * @param   align  メモリアライメントサイズ
  *
  * @return  確保したアドレス
  * @retval  NULL 確保に失敗
  */
  virtual void* AllocAlign( const int size, const int align );

  /**
  * @brief   メモリ開放処理
  * @note    確保元が同じヒープである必要があります
  *
  * @param   pBuf  開放対象アドレス
  *
  * @return  確保したアドレス
  * @retval  NULL 確保に失敗
  */
  virtual void Free( void* pBuf );

  /**
  * @brief   デフォルトで使用するアライメントサイズを取得
  *
  * @return  アライメントサイズ
  */
  virtual int GetDefaultAlign( void );

  /**
  *  @brief  メモリ状況の表示
  *
  *  @return 無し
  */
  virtual void Dump(void);

  /**
  *  @brief  メモリ状況の表示
  *          独自で用意したMemoryHeaderを元にダンプします。　指定したIDの情報のみダンプする版
  *
  *  @param  id   SetDebugUserID でしていていたID
  *
  *  @return 無し
  */
  virtual void DumpId( u8 id );

  /**
  *  @brief  メモリ状況の表示
  *          ライブラリなどで用意された関数を使いダンプします
  *
  *  @param  無し
  */
  virtual void SystemDump(void);

  /**
  *  @brief   ヒープのサイズを取得します
  *
  *  @return  サイズ
  */
  virtual int GetTotalSize(void);

  /**
  *  @brief   ヒープの合計空きサイズを取得します
  *
  *  @return  サイズ
  */
  virtual int GetTotalFreeSize(void);

  /**
  *  @brief   ヒープで確保できる最大のサイズを取得します
  *
  *  @return  サイズ
  */
  virtual int GetTotalAllocatableSize(void);

  /**
  *  @brief  確保された領域のサイズを取得(HeapManagerにもあります)
  *
  *  @param  mem  調べるメモリ
  *
  *  @return メモリサイズ(-1は失敗)
  */
  virtual int GetMemorySize( const void *mem );

   /**
   *  @brief  渡されたメモリが管理しているメモリかチェックする
   *  @note   CheckMemoryBlockでは偶然の一致を検出できない
   *  @note   検索するため、重い
   *
   *  @param  pMemory 調べるメモリ
   *
   *  @return 管理しているメモリならtrue、管理外ならfalseを返す
   */
  virtual b32 IsManaged( const void *pMemory ) const;

   /**
   *  @brief  渡されたメモリが管理しているメモリかチェックする
   *  @note   初期確保サイズの範囲チェックのみを行う
   *
   *  @param  pMemory 調べるメモリ
   *
   *  @return 管理しているメモリならtrue、管理外ならfalseを返す
   */
  virtual b32 IsManagedFast( const void *pMemory ) const;

  /**
  *  @brief  すべてのメモリを解放します
  *
  *  @return 成功したらtrue、失敗したらfalse
  */
  virtual bool FreeAll(void);

private:

  void* m_pSpace;
  int   m_Size;

#if !defined(__cplusplus_cli) // C++/CLIはネイティブスレッドを使えないらしいのでかこった
   // スレッドによる同時処理を防ぐ
  gfl2::thread::Mutex mAllocLock;
#endif
  
};

GFL_NAMESPACE_END( heap );
GFL_NAMESPACE_END( gfl2 );

#endif //GF_PLATFORM_WIN32

#endif __GFL2_WIN_HEAP__
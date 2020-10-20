//============================================================================================
/**
 * @file    app_util_heap.h
 * @brief    ヒープ関連
 * @author  Hiroyuki Nakamura
 * @date    11.10.04
 */
//============================================================================================
#if !defined( __APP_UTIL_HEAP_H__ )
#define __APP_UTIL_HEAP_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <util/include/gfl2_GLHeapAllocator.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  app::util
 * @brief      アプリケーションユーティリティ処理
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace util {

//--------------------------------------------------------------------------------------------
/**
 * @brief  ヒープ
 */
//--------------------------------------------------------------------------------------------
class Heap {
  GFL_FORBID_COPY_AND_ASSIGN(Heap);

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
  Heap(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  virtual ~Heap();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    作成
   *
   * @param   heap          ヒープメモリ確保先となるヒープベース
   * @param   devHeap       デバイスメモリ確保先となるヒープベース
   * @param    heapID        ヒープID
   * @param    heapSize      ヒープサイズ
   * @param    devHeapID      デバイスヒープID
   * @param    devHeapSize    デバイスヒープサイズ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void Create( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, const gfl2::heap::HEAPID heapID, const u32 heapSize, const gfl2::heap::HEAPID devHeapID, const u32 devHeapSize );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief    作成 ローカルALLOCATE版
   *
   * @param   heap          ヒープメモリ確保先となるヒープベース
   * @param   devHeap       デバイスメモリ確保先となるヒープベース
   * @param    heapSize      ヒープサイズ
   * @param    devHeapSize    デバイスヒープサイズ
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void LocalHeapCreate( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, const u32 heapSize, const u32 devHeapSize );

  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    作成（外部ヒープセット版）
   *
   * @param    heap                ヒープ
   * @param    devHeap             デバイスピープ
   * @param    heapAllocator       ヒープアロケーター
   * @param    devHeapAllocator    デバイスピープアロケーター
   * @param    glHeapAllocator     GLヒープアロケータ
   * @param    devGLHeapAllocator  デバイスGLヒープアロケータ
   *
   * @retrun  none
   */
  //--------------------------------------------------------------------------------------------
  void Create(
      gfl2::heap::HeapBase        * heap,
      gfl2::heap::HeapBase        * devHeap,
      gfl2::heap::NwAllocator     * heapAllocator,
      gfl2::heap::NwAllocator     * devHeapAllocator,
      gfl2::util::GLHeapAllocator * glHeapAllocator,
      gfl2::util::GLHeapAllocator * devGLHeapAllocator
  );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    削除
   *
   * @param    none
   *
   * @return  none
   */
  //--------------------------------------------------------------------------------------------
  void Delete(void);


  //--------------------------------------------------------------------------------------------
  /**
   * @brief    setter
   * @retval  ファイル読み込みよう一時ヒープ
   */
  //--------------------------------------------------------------------------------------------
  inline void SetFileReadHeap(gfl2::heap::HeapBase * pHeap){ m_pFileReadHeap = pHeap; }

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  システムヒープ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::heap::HeapBase * GetSystemHeap(void) const { return m_pHeap; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  デバイスヒープ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::heap::HeapBase * GetDeviceHeap(void) const { return m_pDevHeap; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  システムアロケータ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::heap::NwAllocator * GetSystemAllocator(void) const { return m_pHeapAllocator; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  デバイスアロケータ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::heap::NwAllocator * GetDeviceAllocator(void) const { return m_pDevHeapAllocator; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  GLヒープアロケータ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::util::GLHeapAllocator * GetGLHeapAllocator(void) const { return m_pGLHeapAllocator; }
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  デバイスGLヒープアロケータ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::util::GLHeapAllocator * GetDevGLHeapAllocator(void) const { return m_pDevGLHeapAllocator; }


  //--------------------------------------------------------------------------------------------
  /**
   * @brief    getter
   * @retval  ファイル読み込みよう一時ヒープ
   */
  //--------------------------------------------------------------------------------------------
  inline gfl2::heap::HeapBase * GetFileReadHeap(void) const { return m_pFileReadHeap; }


  
private:
  gfl2::heap::HeapBase        * m_pHeap;                //!< ヒープ
  gfl2::heap::HeapBase        * m_pDevHeap;             //!< デバイスヒープ
  gfl2::heap::HeapBase        * m_pFileReadHeap;        //!< ファイル読み込み用一時ヒープ
  gfl2::heap::NwAllocator     * m_pHeapAllocator;       //!< ヒープアロケータ
  gfl2::heap::NwAllocator     * m_pDevHeapAllocator;    //!< デバイスヒープアロケータ
  gfl2::util::GLHeapAllocator * m_pGLHeapAllocator;     //!< GLヒープアロケータ
  gfl2::util::GLHeapAllocator * m_pDevGLHeapAllocator;  //!< デバイスGLヒープアロケータ

  bool  m_is_create;  //!< 生成フラグ ( true = 生成した )
};


}    // util
}    // app

#endif // __APP_UTIL_HEAP_H__

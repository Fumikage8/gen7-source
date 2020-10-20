//============================================================================================
/**
 * @file    app_util_heap.cpp
 * @brief    ヒープ関連
 * @author  Hiroyuki Nakamura
 * @date    11.10.04
 *
 * namespace  app::util::
 */
//============================================================================================
#include <AppLib/include/Util/app_util_heap.h>

#include <System/include/HeapDefine.h>

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
 * @brief    コンストラクタ
 */
//--------------------------------------------------------------------------------------------
Heap::Heap(void) :
  m_pHeap(NULL),
  m_pDevHeap(NULL),
  m_pFileReadHeap(NULL),
  m_pHeapAllocator(NULL),
  m_pDevHeapAllocator(NULL),
  m_pGLHeapAllocator(NULL),
  m_pDevGLHeapAllocator(NULL),
  m_is_create(false)
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//--------------------------------------------------------------------------------------------
Heap::~Heap()
{
  Delete();
}


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
void Heap::Create( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, const gfl2::heap::HEAPID heapID, const u32 heapSize, const gfl2::heap::HEAPID devHeapID, const u32 devHeapSize )
{
  Delete();

  m_pHeap    = GFL_CREATE_HEAP( heap,    heapID,    heapSize,    gfl2::heap::HEAP_TYPE_EXP, false );
  m_pDevHeap = GFL_CREATE_HEAP( devHeap, devHeapID, devHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );

  m_pHeapAllocator    = GFL_NEW( m_pHeap    ) gfl2::heap::NwAllocator( m_pHeap );
  m_pDevHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::heap::NwAllocator( m_pDevHeap );

  m_pGLHeapAllocator    = GFL_NEW( m_pHeap    ) gfl2::util::GLHeapAllocator( m_pHeap );
  m_pDevGLHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::util::GLHeapAllocator( m_pDevHeap );

  m_is_create = true;
}

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
void Heap::LocalHeapCreate( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, const u32 heapSize, const u32 devHeapSize )
{

  Delete();

  m_pHeap    = GFL_CREATE_LOCAL_HEAP( heap,    heapSize,    gfl2::heap::HEAP_TYPE_EXP, false );
  m_pDevHeap = GFL_CREATE_LOCAL_HEAP( devHeap, devHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );

  m_pHeapAllocator    = GFL_NEW( m_pHeap    ) gfl2::heap::NwAllocator( m_pHeap );
  m_pDevHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::heap::NwAllocator( m_pDevHeap );

  m_pGLHeapAllocator    = GFL_NEW( m_pHeap    ) gfl2::util::GLHeapAllocator( m_pHeap );
  m_pDevGLHeapAllocator = GFL_NEW( m_pDevHeap ) gfl2::util::GLHeapAllocator( m_pDevHeap );

  m_is_create = true;

}


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
void Heap::Create(
    gfl2::heap::HeapBase        * heap,
    gfl2::heap::HeapBase        * devHeap,
    gfl2::heap::NwAllocator     * heapAllocator,
    gfl2::heap::NwAllocator     * devHeapAllocator,
    gfl2::util::GLHeapAllocator * glHeapAllocator,
    gfl2::util::GLHeapAllocator * devGLHeapAllocator
)
{  
  m_pHeap               = heap;
  m_pDevHeap            = devHeap;
  m_pHeapAllocator      = heapAllocator;
  m_pDevHeapAllocator   = devHeapAllocator;
  m_pGLHeapAllocator    = glHeapAllocator;
  m_pDevGLHeapAllocator = devGLHeapAllocator;
  
  //is_createはたてない！
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    削除
 *
 * @param    none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void Heap::Delete(void)
{
  if( m_is_create )
  {
    GFL_SAFE_DELETE(m_pDevGLHeapAllocator);
    GFL_SAFE_DELETE(m_pGLHeapAllocator);
    GFL_SAFE_DELETE(m_pDevHeapAllocator);
    GFL_SAFE_DELETE(m_pHeapAllocator);
    gfl2::heap::Manager::DeleteHeap(m_pDevHeap);  m_pDevHeap = NULL;
    gfl2::heap::Manager::DeleteHeap(m_pHeap);     m_pHeap    = NULL;
    m_is_create = false;
  }
  else
  {
    m_pDevGLHeapAllocator = NULL;
    m_pGLHeapAllocator    = NULL;
    m_pDevHeapAllocator   = NULL;
    m_pHeapAllocator      = NULL;
    m_pDevHeap            = NULL;
    m_pHeap               = NULL;
  }
  m_pFileReadHeap = NULL;  //これは外部渡しなのでけすだけ
}



}    // util
}    // app

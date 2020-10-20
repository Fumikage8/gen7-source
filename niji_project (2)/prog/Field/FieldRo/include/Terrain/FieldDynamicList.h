// ============================================================================
/*
 * @file FieldDynamicList.h
 * @brief ListNodeを外部で確保するタイプの双方向連結のリストクラス
 * @note gfl2::Listは初期化時にバッファを作成する型式だったので実装、高橋L了承済み
 * @note 基本挙動はgfl2::Listに準拠
 * @date 2015.06.24
 */
// ============================================================================
#if !defined( __FIELDDYNAMICLIST_H_INCLUDED )
#define __FIELDDYNAMICLIST_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field )

// ノード
template<class Type>
struct ListNode
{
  ListNode<Type>*   pPrev;
  ListNode<Type>*   pNext;
  Type              data;
  bool              bUse;

  ListNode() :
    pPrev( NULL ),
    pNext( NULL ),
    bUse( false )
  {
  }
};

// イテレーター
template<class Type>
class ListIterator
{
public:

  ListIterator() :
    m_pNode( NULL )
  {
  }

  ListIterator( ListNode<Type>* pNode ) :
    m_pNode( pNode )
  {
  }

  ListIterator& operator ++ ()
  {
    m_pNode = m_pNode->pNext;
    return *this;
  }

  ListIterator& operator -- ()
  {
    m_pNode = m_pNode->pPrev;
    return *this;
  }

  ListIterator operator ++ (int)
  {
    ListIterator temp = *this;
    ++*this;
    return temp;
  }

  ListIterator operator -- (int)
  {
    ListIterator temp = *this;
    --*this;
    return temp;
  }

  Type* operator -> ()
  {
    return &m_pNode->data;
  }

  Type& operator * () const
  {
    return m_pNode->data;
  }

  bool operator == ( const ListIterator& it )
  {
    return m_pNode == it.m_pNode;
  }

  bool operator != ( const ListIterator& it )
  {
    return !( m_pNode == it.m_pNode );
  }

  ListNode<Type>*     m_pNode;

};


template<class Type>
class DynamicList
{
public:

  typedef ListIterator< Type >    Iterator;

private:

  typedef ListNode< Type >        _ListNode;

public:


  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを作成します。
   * @param  pHeap バッファを確保する為のヒープです
   * @param  bufferSize リストの要素数を指定します
   */
  // --------------------------------------------------------------------------
  DynamicList( gfl2::heap::HeapBase* pHeap ) :
    m_pRootNode( NULL ),
    m_NodeCount( 0 )
  {
    m_pRootNode = GFL_NEW( pHeap ) _ListNode();
    m_pRootNode->pPrev = m_pRootNode;
    m_pRootNode->pNext = m_pRootNode;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを削除します。
   */
  // --------------------------------------------------------------------------
  ~DynamicList()
  {
    Erase();
    GFL_SAFE_DELETE( m_pRootNode );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  リストの要素数を返します。
   * @return リストの要素数
   */
  // --------------------------------------------------------------------------
  u32 Size() const
  {
    return m_NodeCount;
  }

#if 0
  // --------------------------------------------------------------------------
  /**
   * @brief  リストの要素数の最大値を返します。
   * @return リストの要素数の最大値
   */
  // --------------------------------------------------------------------------
  u32 MaxSize() const
  {
    return m_BufferSize;
  }
#endif

  // --------------------------------------------------------------------------
  /**
   * @brief  先頭を指すイテレータを返します。
   * @return 先頭を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Begin()
  {
    return Iterator( m_pRootNode->pNext );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  末尾を指すイテレータを返します。
   * @return 末尾を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator End()
  {
    return Iterator( m_pRootNode );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  リストの先頭に要素を追加します。
   * @param  data 追加する要素
   * @note   要素を追加する時にコンストラクタは呼び出されません。
   */
  // --------------------------------------------------------------------------
  void PushFront( const Type& data, gfl2::heap::HeapBase* pHeap )
  {
    Insert( Begin(), data );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  リストの末尾に要素を追加します。
   * @param  data 追加する要素
   * @note   要素を追加する時にコンストラクタは呼び出されません。
   */
  // --------------------------------------------------------------------------
  void PushBack( const Type& data, gfl2::heap::HeapBase* pHeap )
  {
    Insert( End(), data, pHeap );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  先頭要素を削除します。
   */
  // --------------------------------------------------------------------------
  void PopFront()
  {
    Erase( Begin() );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  末尾要素を削除します。
   */
  // --------------------------------------------------------------------------
  void PopBack()
  {
    Erase( --End() );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  要素をリストに挿入します。
   * @param  it 挿入位置を指すイテレータ
   * @param  data 挿入するデータ
   * @return 挿入された要素を指すイテレータを返します
   * @note   要素を挿入する時にコンストラクタは呼び出されません。
   */
  // --------------------------------------------------------------------------
  Iterator Insert( Iterator it, const Type& data, gfl2::heap::HeapBase* pHeap )
  {
    _ListNode* pAddNode = GFL_NEW( pHeap ) _ListNode();
    // GFL_ASSERT_STOP_MSG( pAddNode, "Error !! : insert failed. buffer over !!" );

    pAddNode->bUse = true;
    pAddNode->data = data;
    pAddNode->pNext = it.m_pNode;
    pAddNode->pPrev = it.m_pNode->pPrev;

    it.m_pNode->pPrev->pNext = pAddNode;
    it.m_pNode->pPrev = pAddNode;

    ++m_NodeCount;

    return Iterator( pAddNode );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  要素を削除します。
   * @param  it 削除する要素を指すイテレータ
   * @return 削除した要素の直後の位置を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Erase( Iterator it )
  {
    if( it == End() )
    {
      return End();
    }

    _ListNode* pDeleteNode = it.m_pNode;

    _ListNode* pTemp = pDeleteNode->pNext;

    pDeleteNode->pPrev->pNext = pDeleteNode->pNext;
    pDeleteNode->pNext->pPrev = pDeleteNode->pPrev;

    pDeleteNode->bUse = false;
    pDeleteNode->pPrev = NULL;
    pDeleteNode->pNext = NULL;
    GFL_SAFE_DELETE( pDeleteNode );

    --m_NodeCount;

    return Iterator( pTemp );
  }


  // --------------------------------------------------------------------------
  /**
   * @brief  要素を全て削除します。
   * @return 末尾を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Erase()
  {
    Iterator it = Begin();
    while( it != End() )
    {
      it = Erase( it );
    }

    m_NodeCount = 0;
    return it;
  }


  // --------------------------------------------------------------------------
  /**
   * @brief  dataと等しい全ての要素を削除します。
   * @param  data 削除したい要素
   *
   * @param  削除した数
   */
  // --------------------------------------------------------------------------
  u32 Remove( const Type& data )
  {
    u32 removeCount( 0 );

    for( Iterator first = Begin(), last = End(); first != last; )
    {
      Iterator next = first;
      ++next;
      if( *first == data )
      {
        Erase( first );
        removeCount++;
      }
      first = next;
    }

    return removeCount;
  }


private:


private:
  _ListNode*      m_pRootNode;
  u32             m_NodeCount;

};


GFL_NAMESPACE_END( Field )

#endif // __FIELDDYNAMICLIST_H_INCLUDED

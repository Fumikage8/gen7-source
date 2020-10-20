// ============================================================================
/*
 * @file gfl2_List.h
 * @brief 双方向連結のリストクラスです。
 * @date 2015.02.26
 */
// ============================================================================
#if !defined( GFL2_LIST_H_INCLUDED )
#define GFL2_LIST_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_ListNode.h>
#include <util/include/gfl2_ListIterator.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)


template<class Type>
class List
{
public:

  typedef ListIterator< Type >    Iterator;

private:

  typedef ListNode< Type >        _ListNode;

public:

  List() :
    m_pRootNode( NULL ),
    m_pBuffer( NULL ),
    m_BufferSize( 0 ),
    m_EmptyIndex( 0 ),
    m_NodeCount( 0 )
  {
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを作成します。
   * @param  pHeap バッファを確保する為のヒープです
   * @param  bufferSize リストの要素数を指定します
   */
  // --------------------------------------------------------------------------
  List( gfl2::heap::HeapBase* pHeap, u32 bufferSize ) :
    m_pRootNode( NULL ),
    m_pBuffer( NULL ),
    m_BufferSize( 0 ),
    m_EmptyIndex( 0 ),
    m_NodeCount( 0 )
  {
    CreateBuffer( pHeap, bufferSize );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを削除します。
   */
  // --------------------------------------------------------------------------
  ~List()
  {
    Clear();
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを作成します。
   * @param  pHeap バッファを確保する為のヒープです
   * @param  bufferSize リストの要素数を指定します
   * @note   内部バッファを作成した時に要素のコンストラクタが呼び出されます
   */
  // --------------------------------------------------------------------------
  void CreateBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize )
  {
    GFL_ASSERT_MSG( !m_pRootNode, "Error !! : Please clear buffer." );

    m_pRootNode = GFL_NEW( pHeap ) _ListNode();
    m_pBuffer = GFL_NEW_ARRAY( pHeap ) _ListNode[ bufferSize ];
    m_BufferSize = bufferSize;

    m_pRootNode->pPrev = m_pRootNode;
    m_pRootNode->pNext = m_pRootNode;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを削除します。
   */
  // --------------------------------------------------------------------------
  void Clear()
  {
    GFL_SAFE_DELETE_ARRAY( m_pBuffer );
    GFL_SAFE_DELETE( m_pRootNode );
    m_BufferSize = 0;
    m_EmptyIndex = 0;
    m_NodeCount = 0;
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
  void PushFront( const Type& data )
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
  void PushBack( const Type& data )
  {
    Insert( End(), data );
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
  Iterator Insert( Iterator it, const Type& data )
  {
    _ListNode* pAddNode = getEmptyNode();
    GFL_ASSERT_MSG( pAddNode, "Error !! : insert failed. buffer over !!" );

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

    --m_NodeCount;

    return Iterator( pTemp );
  }


  // --------------------------------------------------------------------------
  /**
   * @brief  要素を全て削除します。
   * @note   内部のバッファを削除しないで要素を全て削除します。
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
    return it;
  }


  // --------------------------------------------------------------------------
  /**
   * @brief  dataと等しい全ての要素を削除します。
   * @param  data 削除したい要素
   */
  // --------------------------------------------------------------------------
  void Remove( const Type& data )
  {
    for( Iterator first = Begin(), last = End(); first != last; )
    {
      Iterator next = first;
      ++next;
      if( *first == data )
      {
        Erase( first );
      }
      first = next;
    }
  }


private:

  _ListNode* getEmptyNode()
  {
    u32 emptyIndex = m_EmptyIndex;
    for( u32 i = 0; i < m_BufferSize; ++i )
    {
      if( !m_pBuffer[ emptyIndex ].bUse )
      {
        m_EmptyIndex = emptyIndex + 1;
        m_EmptyIndex = ( m_EmptyIndex >= m_BufferSize ) ? 0 : m_EmptyIndex;
        return &m_pBuffer[ emptyIndex ];
      }
      ++emptyIndex;
      emptyIndex = ( emptyIndex >= m_BufferSize ) ? 0 : emptyIndex;
    }
    return NULL;
  }

private:
  _ListNode*      m_pRootNode;
  _ListNode*      m_pBuffer;
  u32             m_BufferSize;
  u32             m_EmptyIndex;
  u32             m_NodeCount;

};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_LIST_H_INCLUDED

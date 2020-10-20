// ============================================================================
/*
 * @file gfl2_Map.h
 * @brief ツリー構造の連想配列クラスです。
 * @date 2015.02.24
 */
// ============================================================================
#if !defined( GFL2_MAP_H_INCLUDED )
#define GFL2_MAP_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_Pair.h>
#include <util/include/gfl2_TreeNode.h>
#include <util/include/gfl2_TreeIterator.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)


template<class Key, class Value>
class Map
{
public:

  typedef TreeIterator< Pair < Key, Value > >   Iterator;

private:

  typedef TreeNode < Pair < Key, Value > >      PairTreeNode;

public:

  Map() :
    m_pHeader( NULL ),
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
   * @param  bufferSize マップの要素数を指定します
   */
  // --------------------------------------------------------------------------
  Map( gfl2::heap::HeapBase* pHeap, u32 bufferSize ) :
    m_pHeader( NULL ),
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
  ~Map()
  {
    Clear();
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを作成します。
   * @param  pHeap バッファを確保する為のヒープです
   * @param  bufferSize マップの要素数を指定します
   * @note   内部バッファを作成した時に要素のコンストラクタが呼び出されます。
   */
  // --------------------------------------------------------------------------
  void CreateBuffer( gfl2::heap::HeapBase* pHeap, u32 bufferSize )
  {
    m_pHeader = GFL_NEW( pHeap ) PairTreeNode();
    m_pBuffer = GFL_NEW_ARRAY( pHeap ) PairTreeNode[ bufferSize ];
    m_BufferSize = bufferSize;

    m_pHeader->pLeft = m_pHeader;
    m_pHeader->pRight = m_pHeader;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  内部バッファを削除します。
   */
  // --------------------------------------------------------------------------
  void Clear()
  {
    GFL_SAFE_DELETE_ARRAY( m_pBuffer );
    GFL_SAFE_DELETE( m_pHeader );
    m_pRootNode = NULL;
    m_BufferSize = 0;
    m_EmptyIndex = 0;
    m_NodeCount = 0;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  マップの要素数を返します。
   * @return マップの要素数
   */
  // --------------------------------------------------------------------------
  u32 Size() const
  {
    return m_NodeCount;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  マップの要素数の最大値を返します。
   * @return マップの要素数の最大値
   */
  // --------------------------------------------------------------------------
  u32 MaxSize() const
  {
    return m_BufferSize;
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  要素をマップに挿入します。
   * @param  pair 挿入するデータ
   * @note   要素を挿入する時にコンストラクタは呼び出されません。
   */
  // --------------------------------------------------------------------------
  void Insert( const Pair<Key, Value>& pair )
  {
    Iterator it = Find( pair );
    if( it != End() )
    {
      GFL_WARNING( "Warning : insert failed. found the same key !! key = %d\n", pair.first );
      return;
    }

    PairTreeNode* pAddNode = getEmptyNode();
    if( pAddNode == NULL )
    {
      GFL_WARNING( "Warning : insert failed. buffer over !!\n" );
      return;
    }

    pAddNode->bUse = true;
    pAddNode->pair = pair;
    pAddNode->pLeft = NULL;
    pAddNode->pRight = NULL;
    pAddNode->pParent = NULL;

    if( m_pRootNode == NULL )
    {
      m_pRootNode = pAddNode;
      m_pRootNode->pParent = m_pHeader;
      ++m_NodeCount;

      m_pHeader->pParent = m_pHeader;
      m_pHeader->pLeft = pAddNode;
      m_pHeader->pRight = pAddNode;

      return;
    }

    PairTreeNode* pTemp1 = m_pRootNode;
    PairTreeNode* pTemp2 = NULL;

    while( pTemp1 != NULL )
    {
      pTemp2 = pTemp1;
      if( pTemp1->pair.first > pAddNode->pair.first )
      {
        pTemp1 = pTemp1->pLeft;
      }
      else
      {
        pTemp1 = pTemp1->pRight;
      }
    }

    pAddNode->pParent = pTemp2;
    if( pTemp2->pair.first > pAddNode->pair.first )
    {
      pTemp2->pLeft = pAddNode;
    }
    else
    {
      pTemp2->pRight = pAddNode;
    }
    ++m_NodeCount;

    if( pAddNode->pair.first < m_pHeader->pLeft->pair.first )
    {
      m_pHeader->pLeft = pAddNode;
    }
    else
    if( pAddNode->pair.first > m_pHeader->pRight->pair.first )
    {
      m_pHeader->pRight = pAddNode;
    }

  }

  // --------------------------------------------------------------------------
  /**
   * @brief  先頭を指すイテレータを返します。
   * @return 先頭を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Begin()
  {
    if( m_pRootNode == NULL )
    {
      return Iterator( m_pHeader );
    }
    return Iterator( getLeftNode( m_pRootNode ) );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  末尾を指すイテレータを返します。
   * @return 末尾を指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator End()
  {
    return Iterator( m_pHeader );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  要素のキーが指すイテレータを返します。
   * @param  pair 要素
   * @return 要素が指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Find( const Pair<Key, Value>& pair )
  {
    return Find( pair.first );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  キーが指すイテレータを返します。
   * @param  key キー
   * @return keyが指すイテレータ
   */
  // --------------------------------------------------------------------------
  Iterator Find( Key key )
  {
    PairTreeNode* pTemp = m_pRootNode;
    while( pTemp != NULL )
    {
      if( pTemp->pair.first > key )
      {
        pTemp = pTemp->pLeft;
      }
      else
      if( pTemp->pair.first < key )
      {
        pTemp = pTemp->pRight;
      }
      else
      {
        return Iterator( pTemp );
      }
    }
    return Iterator( m_pHeader );
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  キーが指す要素を削除します。
   * @param  key キー
   */
  // --------------------------------------------------------------------------
  void Erase( Key key )
  {
    Iterator it = Find( key );
    if( it != End() )
    {
      Erase( it );
    }
  }

  // --------------------------------------------------------------------------
  /**
   * @brief  イテレータが指す要素を削除します。
   * @param  it イテレータ
   */
  // --------------------------------------------------------------------------
  void Erase( Iterator it )
  {
    PairTreeNode* pDeleteNode = it.m_pNode;

    PairTreeNode* pMinimumNode = NULL;
    PairTreeNode* pMaximumNode = NULL;
    if( pDeleteNode == m_pHeader->pLeft )
    {
      Iterator temp = it;
      ++temp;
      if( temp.m_pNode != pDeleteNode )
      {
        pMinimumNode = temp.m_pNode;
      }
    }
    else
    if( pDeleteNode == m_pHeader->pRight )
    {
      Iterator temp = it;
      --temp;
      if( temp.m_pNode != pDeleteNode )
      {
        pMaximumNode = temp.m_pNode;
      }
    }

    if( pDeleteNode->pLeft == NULL )
    {
      replace( pDeleteNode, pDeleteNode->pRight );
      resetNodeParameter( pDeleteNode );
    }
    else
    if( pDeleteNode->pRight == NULL )
    {
      replace( pDeleteNode, pDeleteNode->pLeft );
      resetNodeParameter( pDeleteNode );
    }
    else
    {
      PairTreeNode* pReplaceNode = getLeftNode( pDeleteNode->pRight );
      pDeleteNode->pair = pReplaceNode->pair;
      replace( pReplaceNode, pReplaceNode->pRight );
      resetNodeParameter( pReplaceNode );
    }
    --m_NodeCount;

    if( pMinimumNode != NULL )
    {
      m_pHeader->pLeft = pMinimumNode;
    }
    else
    if( pMaximumNode != NULL )
    {
      m_pHeader->pRight = pMaximumNode;
    }
  }

private:

  PairTreeNode* getEmptyNode()
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

  PairTreeNode* getLeftNode( PairTreeNode* pSearchTopNode )
  {
    PairTreeNode* pLeftNode = pSearchTopNode;
    while( pLeftNode->pLeft != NULL )
    {
      pLeftNode = pLeftNode->pLeft;
    }
    return pLeftNode;
  }

  void replace( PairTreeNode* pDeleteNode, PairTreeNode* pReplaceNode )
  {
    PairTreeNode* pDeleteNodeParent = pDeleteNode->pParent;
    
    if( pReplaceNode != NULL )
    {
      pReplaceNode->pParent = pDeleteNodeParent;
    }

    if( pDeleteNode == m_pRootNode )
    {
      m_pRootNode = pReplaceNode;
    }
    else
    if( pDeleteNodeParent->pLeft == pDeleteNode )
    {
      pDeleteNodeParent->pLeft = pReplaceNode;
    }
    else
    {
      pDeleteNodeParent->pRight = pReplaceNode;
    }
  }

  void resetNodeParameter( PairTreeNode* pDeleteNode )
  {
    pDeleteNode->pParent = NULL;
    pDeleteNode->pLeft = NULL;
    pDeleteNode->pRight = NULL;
    pDeleteNode->bUse = false;
 }

private:

  PairTreeNode*     m_pHeader;
  PairTreeNode*     m_pRootNode;
  PairTreeNode*     m_pBuffer;
  u32               m_BufferSize;
  u32               m_EmptyIndex;
  u32               m_NodeCount;


};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_MAP_H_INCLUDED

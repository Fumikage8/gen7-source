// ============================================================================
/*
 * @file gfl2_TreeIterator.h
 * @brief ２分木構造のイテレータです。
 * @date 2015.02.24
 */
// ============================================================================
#if !defined( GFL2_TREEITERATOR_H_INCLUDED )
#define GFL2_TREEITERATOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_TreeNode.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

template<class Type>
class TreeIterator
{
public:

  TreeIterator() :
    m_pNode( NULL )
  {
  }

  TreeIterator( TreeNode<Type>* pNode ) :
    m_pNode( pNode )
  {
  }

  TreeIterator& operator ++ ()
  {
    if( m_pNode->pRight )
    {
      m_pNode = m_pNode->pRight;
      while( m_pNode->pLeft )
      {
        m_pNode = m_pNode->pLeft;
      }
    }
    else
    {
      TreeNode<Type>* pTemp = m_pNode->pParent;
      while( m_pNode == pTemp->pRight )
      {
        m_pNode = pTemp;
        pTemp = pTemp->pParent;
      }
      if( m_pNode->pRight != pTemp )
      {
        m_pNode = pTemp;
      }
    }
    return *this;
  }

  TreeIterator& operator -- ()
  {
    if( m_pNode->pParent->pParent == m_pNode )
    {
      m_pNode = m_pNode->pRight;
    }
    else
    if( m_pNode->pLeft )
    {
      TreeNode<Type>* pTemp = m_pNode->pLeft;
      while( pTemp->pRight )
      {
        pTemp = pTemp->pRight;
      }
        m_pNode = pTemp;
    }
    else
    {
      TreeNode<Type>* pTemp = m_pNode->pParent;
      while( m_pNode == pTemp->pLeft )
      {
        m_pNode = pTemp;
        pTemp = pTemp->pParent;
      }
      m_pNode = pTemp;
    }
    return *this;
  }

  TreeIterator operator ++ (int)
  {
    TreeIterator temp = *this;
    ++*this;
    return temp;
  }

  TreeIterator operator -- (int)
  {
    TreeIterator temp = *this;
    --*this;
    return temp;
  }

  Type* operator -> ()
  {
    return &m_pNode->pair;
  }

  bool operator == ( const TreeIterator& it )
  {
    return m_pNode == it.m_pNode;
  }

  bool operator != ( const TreeIterator& it )
  {
    return !( m_pNode == it.m_pNode );
  }

  TreeNode<Type>*     m_pNode;

};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_TREEITERATOR_H_INCLUDED

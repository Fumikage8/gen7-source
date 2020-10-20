// ============================================================================
/*
 * @file gfl2_ListIterator.h
 * @brief 双方向リストのイテレータです。
 * @date 2015.02.26
 */
// ============================================================================
#if !defined( GFL2_LISTITERATOR_H_INCLUDED )
#define GFL2_LISTITERATOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_ListNode.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

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


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_LISTITERATOR_H_INCLUDED

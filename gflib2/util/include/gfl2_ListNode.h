// ============================================================================
/*
 * @file gfl2_ListNode.h
 * @brief 双方向リストのノードです。
 * @date 2015.02.26
 */
// ============================================================================
#if !defined( GFL2_LISTNODE_H_INCLUDED )
#define GFL2_LISTNODE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

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
    data(),
    bUse( false )
  {
  }
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_LISTNODE_H_INCLUDED

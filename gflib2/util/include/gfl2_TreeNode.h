// ============================================================================
/*
 * @file gfl2_TreeNode.h
 * @brief ２分木構造のノードです。
 * @date 2015.02.24
 */
// ============================================================================
#if !defined( GFL2_TREENODE_H_INCLUDED )
#define GFL2_TREENODE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)

template<class Type>
struct TreeNode
{
  TreeNode<Type>*   pLeft;
  TreeNode<Type>*   pRight;
  TreeNode<Type>*   pParent;
  Type              pair;
  bool              bUse;

  TreeNode() :
    pLeft( NULL ),
    pRight( NULL ),
    pParent( NULL ),
    bUse( false )
  {
  }
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_TREENODE_H_INCLUDED

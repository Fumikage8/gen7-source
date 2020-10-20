#if !defined(GFL2_FSLIST_H_INCLUDED)
#define GFL2_FSLIST_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_FsList.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   リスト
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// fsのインクルード
#include <fs/include/gfl2_FsToolDefine.h>
#include <fs/include/gfl2_FsDefine.h>
#include <fs/include/gfl2_FsResult.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)

class ElemBase;

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


//==============================================================================
/**
 @class     List
 @brief     リスト
 */
//==============================================================================
class List
{
  GFL_FORBID_COPY_AND_ASSIGN(List);

public:
  List(void);
  virtual ~List();

  ElemBase* GetHead(void) const;
  ElemBase* GetTail(void) const;
  ElemBase* GetRemoveHead(void);
  
  void Add(ElemBase* elem);
  void Remove(const ElemBase* elem);

private:
  ElemBase* m_head;
  ElemBase* m_tail;
};
 

GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_FSLIST_H_INCLUDED

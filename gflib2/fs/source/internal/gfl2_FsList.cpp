//==============================================================================
/**
 @file    gfl2_FsList.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   リスト
 */
//==============================================================================


// fsのインクルード
#include <fs/include/internal/gfl2_FsElemBase.h>
#include "gfl2_FsList.h"


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)


List::List(void)
  : m_head(NULL),
    m_tail(NULL)
{}
List::~List()
{}

ElemBase* List::GetHead(void) const { return m_head; }
ElemBase* List::GetTail(void) const { return m_tail; }

ElemBase* List::GetRemoveHead(void)
{
  ElemBase* elem = m_head;
  this->Remove(elem);
  return elem;
}

void List::Add(ElemBase* elem)
{
  // 末尾から先頭へ、優先度を比較しながら進んでいく。
  // 同じ優先度のときは後ろ(即ち末尾側)に追加する。即ち、先に追加されているものが優先される。
  ElemBase* prev = m_tail;
  while(prev != NULL)  // 先頭に到達したらNULLになるので抜ける
  {
    if(elem->GetPrio() < prev->GetPrio())
    {
      prev = prev->GetPrev();
    }
    else
    {
      break;  // 追加場所が決定したので抜ける
    }
  }

  // 追加する
  ElemBase* next = NULL;
  if(prev) next = prev->GetNext();
  else     next = m_head;

  elem->SetPrev(prev);
  elem->SetNext(next);

  if(prev) prev->SetNext(elem);
  else     m_head = elem;

  if(next) next->SetPrev(elem);
  else     m_tail = elem;
}
void List::Remove(const ElemBase* elem)
{
  if(elem)
  {
    ElemBase* prev = elem->GetPrev();
    ElemBase* next = elem->GetNext();
    
    if(prev) prev->SetNext(next);
    else     m_head = next;
 
    if(next) next->SetPrev(prev);
    else     m_tail = prev;
  }
}


GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


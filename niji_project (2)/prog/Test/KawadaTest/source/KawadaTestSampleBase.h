#if !defined(NIJI_PROJECT_TEST_KAWADATESTSAMPLEBASE_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTSAMPLEBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestSampleBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestSampleBase
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestSampleBase);

public:
  KawadaTestSampleBase(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
    : m_enableToEnd(false),
      m_gameManager(NULL),
      m_uniMem(NULL),
      m_heapMem(NULL),
      m_devMem(NULL)
  {
    // this->initialize(gameManager, parentHeapMem, parentDevMem); を呼ぶ
  }
  virtual ~KawadaTestSampleBase()
  {
    // this->terminate(); を呼ぶ
  }

  //! @brief  終了できるか
  //! @return  終了できるときtrueを返す
  virtual bool DoesEnableToEnd(void) const
  {
    return m_enableToEnd;
  }
  
  virtual void Update(void) = 0;
  virtual void PreDraw(void) = 0;
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo) = 0;

protected:
  virtual void initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem) = 0;
  //! @note  m_enableToEndがtrueのときに呼んで下さい。
  virtual void terminate(void) = 0;

protected:
  bool                   m_enableToEnd;  // 終了できるときtrue
  GameSys::GameManager*  m_gameManager;
  gfl2::heap::HeapBase*  m_uniMem;   // heapMemでもdevMemでもどちらでもいいメモリ。ただのポインタ。
  gfl2::heap::HeapBase*  m_heapMem;  // heapMemでなければならないところに使うメモリ。              
  gfl2::heap::HeapBase*  m_devMem;   // devMemでなければならないところに使うメモリ。               
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTSAMPLEBASE_H_INCLUDED


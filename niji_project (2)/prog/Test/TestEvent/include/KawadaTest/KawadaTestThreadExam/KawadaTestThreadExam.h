#if !defined(NIJI_PROJECT_TEST_KAWADATEST_KAWADATESTTHREADEXAM_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATEST_KAWADATESTTHREADEXAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestThreadExam.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_ScopedLock.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>




// 前方宣言
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)
  class KawadaTestCountThread;
GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestThreadExam
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestThreadExam
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestThreadExam);

public:
  KawadaTestThreadExam(gfl2::heap::HeapBase* heap);
  virtual ~KawadaTestThreadExam();

  void Update(void);

  void IncreaseTestCount(s32 val);
  void DecreaseTestCount(s32 val);

private:
  s32                             m_testCount;
  gfl2::thread::CriticalSection*  m_testCountCriticalSection;  // m_testCountの同期オブジェクト
  KawadaTestCountThread*          m_testCountThread;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATEST_KAWADATESTTHREADEXAM_H_INCLUDED


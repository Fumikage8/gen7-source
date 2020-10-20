#if !defined(NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLE_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestLayoutSample.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
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

// KawadaTestのインクルード
#include "../KawadaTestSampleBase.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestLayoutSample
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestLayoutSample : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestLayoutSample);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestLayoutSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestLayoutSample();

  virtual void Update(void);
  virtual void PreDraw(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:
  virtual void initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual void terminate(void);
  
  ////////////////////////////////////////////////////////////////////////////////
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLE_H_INCLUDED


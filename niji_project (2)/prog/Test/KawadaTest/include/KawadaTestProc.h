#if !defined(NIJI_PROJECT_TEST_KAWADATESTPROC_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTPROC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestProc.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.22
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>

// KawadaTestのインクルード
#include <Test/KawadaTest/include/KawadaTestProcParam.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


// 前方宣言
class KawadaTestSampleBase;


//==============================================================================
/**
 @class     KawadaTestProc
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestProc);

public:
  //! @param[in]  param  このKawadaTestProcコンストラクタではparamの値のコピーを行うので、
  //!                    呼び出し元はparamを破棄して大丈夫です。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  KawadaTestProc(KawadaTestProcParam* param);
  virtual ~KawadaTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

public:
  KawadaTestSampleBase* GetSample(void) const { return m_sample; }

private:
  KawadaTestProcParam     m_param;

  gfl2::heap::HeapBase*   m_uniMem;   // heapMemでもdevMemでもどちらでもいいメモリ。ただのポインタ。
  gfl2::heap::HeapBase*   m_heapMem;  // heapMemでなければならないところに使うメモリ。              
  gfl2::heap::HeapBase*   m_devMem;   // devMemでなければならないところに使うメモリ。               

  KawadaTestSampleBase*   m_sample;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTPROC_H_INCLUDED


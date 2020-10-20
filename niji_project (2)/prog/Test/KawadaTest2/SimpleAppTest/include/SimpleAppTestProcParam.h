#if PM_DEBUG


#if !defined(NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROCPARAM_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROCPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    SimpleAppTestProcParam.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのProcに渡す引数
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
/**
 @class     SimpleAppTestProcParam
 @brief     シンプルなアプリのProcに渡す引数
 */
//==============================================================================
class SimpleAppTestProcParam
{
public:
  s32 dummy;  //!< [in]  ダミー

public:
  SimpleAppTestProcParam(void)
    : dummy(0)
  {}
};


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // NIJI_PROJECT_TEST_KAWADATEST2_SIMPLEAPPTEST_SIMPLEAPPTESTPROCPARAM_H_INCLUDED


#endif  // PM_DEBUG


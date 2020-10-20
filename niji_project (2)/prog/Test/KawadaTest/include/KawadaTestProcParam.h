#if !defined(NIJI_PROJECT_TEST_KAWADATESTPROCPARAM_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTPROCPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestProcParam.h
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


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestProcParam
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestProcParam
{
public:
  enum SampleId
  {
    // 外部
    SAMPLE_ID_MODEL               ,
    SAMPLE_ID_LAYOUT              ,
    SAMPLE_ID_RADER_CHART         ,
    SAMPLE_ID_ZUKAN               ,
    SAMPLE_ID_ZUKAN_REGISTER      ,
    SAMPLE_ID_POKEMODEL           ,
    SAMPLE_ID_INFO_WINDOW         ,
    SAMPLE_ID_KIRA_CARD           ,

    // 内部
    SAMPLE_ID_LAYOUT_MAIN         ,
    SAMPLE_ID_POKEMODEL_MAIN      ,
    SAMPLE_ID_INFO_WINDOW_MAIN    ,
    SAMPLE_ID_KIRA_CARD_MAIN      ,
    
    SAMPLE_ID_NUM                 ,

    SAMPLE_ID_DEFAULT          = 0,
  };

public:
  SampleId sampleId;  //!< [in]  サンプルの番号

public:
  KawadaTestProcParam(void)
    : sampleId(SAMPLE_ID_DEFAULT)
  {}
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTPROCPARAM_H_INCLUDED


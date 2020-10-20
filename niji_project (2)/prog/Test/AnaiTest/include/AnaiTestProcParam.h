#if !defined(NIJI_PROJECT_TEST_ANAITESTPROCPARAM_H_INCLUDED)
#define NIJI_PROJECT_TEST_ANAITESTPROCPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    AnaiTestProcParam.h
 @author  anai_shigeo@gamefreak.co.jp
 @date    2015.07.24
 @brief   AnaiTest
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
GFL_NAMESPACE_BEGIN(AnaiTest)


//==============================================================================
/**
 @class     AnaiTestProcParam
 @brief     AnaiTest
 */
//==============================================================================



/// 外部設定パラメータ（初期化データ・終了時のデータなど）
struct APP_PARAM {
	// [ in ]
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー
  app::util::AppRenderingManager* mpAppRenderingManager;   //レンダリングマネージャー
  app::util::Heap* mpAppHeap;  //appheap
  //applib::frame::Manager* mpAppFrameManager;
  System::Skybox::Skybox* mpSkybox;
  //app::ui::UIView* mpUIView;
	// [ out ]
};







class AnaiTestProcParam
{
public:
  enum SampleId
  {
    // 外部
    SAMPLE_ID_BGVIEW1             ,

    // 内部
    SAMPLE_ID_BGVIEW2             ,
    
    SAMPLE_ID_NUM                 ,

    SAMPLE_ID_DEFAULT          = 0,
  };

public:
  SampleId sampleId;  //!< [in]  サンプルの番号

public:
  AnaiTestProcParam(void)
    : sampleId(SAMPLE_ID_DEFAULT)
  {}
};


GFL_NAMESPACE_END(AnaiTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_ANAITESTPROCPARAM_H_INCLUDED


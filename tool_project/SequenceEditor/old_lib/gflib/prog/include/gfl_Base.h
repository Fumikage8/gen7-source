#ifndef __GFL_BASE_H__
#define __GFL_BASE_H__
//=============================================================================
/**
 * @file	 gfl_Base.h
 * @brief	 ベースになるGFLクラスライブラリの集約ヘッダー
 * @date	 2010.10.18
 */
//=============================================================================
#pragma once

#ifdef __cplusplus 

#include <base/gfl_ProcBaseProcess.h>  //PROCを作るのに継承して使ってほしいベースクラス
#include <base/gfl_ProcManager.h>      //PROCマネージャー
#include <base/gfl_Singleton.h>
#include <base/gfl_Bit.h>
#include <base/gfl_Thread.h>
#include <base/gfl_ThreadManager.h>
#include <base/gfl_TaskSystem.h>
#include <base/gfl_BinLinkerAccessor.h>
#include <base/gfl_Queue.h>
#include <base/gfl_SimpleContainer.h>
#include <base/gfl_AutoStackManager.h>
#include <base/gfl_KeyContainer.h>
#include <base/gfl_Key2Container.h>
#include <base/gfl_FixedList.h>
#include <base/gfl_FixedListSentinel.h>
#include <base/gfl_Applet.h>

#endif // __cplusplus 

#endif // __GFL_BASE_H__

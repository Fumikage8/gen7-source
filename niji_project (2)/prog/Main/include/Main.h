#ifndef __H_MAIN_H__
#define __H_MAIN_H__
//===================================================================
/**
 * @file    Main.h
 * @brief   nijiプロジェクト アプリケーションメイン
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================
#pragma once


namespace Main{

//--------------------------------------------
/*
  HEAPID_RESIDENT, HEAPID_RESIDENT_DEVICEの初期化
  ソフトリセットで消すために、Systemとは初期化タイミングをずらす

  GflUseモジュール初期化のタイミングで呼び出す
*/
//--------------------------------------------
extern void ResidentHeapCreate(void);
extern void ResidentHeapFree(void);

} //namespace Main

#endif //__H_MAIN_H__

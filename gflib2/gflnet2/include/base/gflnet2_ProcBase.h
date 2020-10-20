#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_PROCBASE_H__
#define __GFLNET_PROCBASE_H__
//=============================================================================
/**
 * @file	 gflnet_ProcBase.h
 * @brief	 通信のPROCの基底クラス  INIT ENDはなし速度重視
 * @author k.ohno
 * @date	 2011.4.20
 */
//=============================================================================
#pragma once

#include <nn.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <proc/include/gfl2_ProcBase.h>

#include "gflnet2/include/scan/gflnet2_Manager.h"

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::proc
* @brief     通信で使うproc群 直接使う必要は無い
*/
GFL_NAMESPACE_BEGIN(proc)


class ProcBase {
  GFL_FORBID_COPY_AND_ASSIGN(ProcBase);

protected:
  int m_seq;					      ///< プロセス関数の動作シーケンス

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------
  ProcBase(void) : m_seq(0){};
  virtual ~ProcBase(){};

  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を返す
   * @return	シーケンス番号
   */
  //------------------------------------------------------------------
  inline int GetSubSeq(void) const { return m_seq; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を更新する
   * @return	シーケンス番号
   */
  //------------------------------------------------------------------
  inline void SetSubSeq(int seq){  m_seq = seq; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）をリセットする
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void ResetSubSeq(void){  m_seq = 0; };
  //------------------------------------------------------------------
  /**
   * @brief	  プロセスの過程（現在の状態）を１つ先に進める
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void AddSubSeq(void){  m_seq++; };


  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメインを行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gflnet2::Manager* pManager) = 0;

};
GFL_NAMESPACE_END(proc)
GFL_NAMESPACE_END(gflnet2)

#endif //__GFLNET_PROCBASE_H__
#endif

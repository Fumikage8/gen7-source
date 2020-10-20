#ifndef __GFL_EVENT_ADAPTER_H__
#define __GFL_EVENT_ADAPTER_H__
//=============================================================================
/**
 * @file	 gfl_event_adapter.h
 * @brief	 完了したことを伝えるクラス 継承して機種依存コードを書くこと
 * @author k.ohno
 * @date	 2014.5.13
 */
//=============================================================================
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(thread)



class EventAdapter{
  GFL_FORBID_COPY_AND_ASSIGN(EventAdapter);

public:
  //-----------------------------------------------------------------------
  /**
   * @brief コンストラクタ デストラクタ
   */
  //-----------------------------------------------------------------------
  EventAdapter(void){};
  virtual ~EventAdapter(void){};
  //-----------------------------------------------------------------------
  /**
   * @brief フラグをセットします
   */
  //-----------------------------------------------------------------------
  virtual void Signal(void)=0;
  //-----------------------------------------------------------------------
  /**
   * @brief フラグがセットされるまで待ちます。  デッドロックなので使用には注意が必要です。
   */
  //-----------------------------------------------------------------------
  virtual bool Wait(void)=0;
  //-----------------------------------------------------------------------
  /**
   * @brief フラグがセットされているかを確認します
   */
  //-----------------------------------------------------------------------
  virtual bool TryWait(void)=0;
  //-----------------------------------------------------------------------
  /**
   * @brief フラグをリセットします
   */
  //-----------------------------------------------------------------------
  virtual void Clear(void)=0;


};





GFL_NAMESPACE_END(thread)
GFL_NAMESPACE_END(gfl2)

#endif//__GFL_EVENT_ADAPTER_H__

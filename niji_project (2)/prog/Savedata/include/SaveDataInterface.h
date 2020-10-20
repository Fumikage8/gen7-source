#ifndef __SAVEDATAINTERFACE_H__
#define __SAVEDATAINTERFACE_H__
//===================================================================
/**
 * @file    SaveDataInterface.h
 * @brief   個々のセーブデータの基底クラス
 * @author  k.ohno(nijiへ移植 Ariizumi)
 * @date    11.12.3
 */
//===================================================================
#pragma once

#include "macro/include/gfl2_macros.h"
#include "heap/include/gfl2_Heap.h"

GFL_NAMESPACE_BEGIN(Savedata)

class SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(SaveDataInterface); //コピーコンストラクタ＋代入禁止


  
  
public:
  SaveDataInterface(void){};
  virtual ~SaveDataInterface(){};

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void* GetData(void) = 0;
  //-----------------------------------------------------------------------------
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  //-----------------------------------------------------------------------------
  virtual size_t GetDataSize(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データサイズ
   *  @param    heap
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap ) = 0;
};


GFL_NAMESPACE_END(Savedata)

#endif// __SAVEDATAINTERFACE_H__

#ifndef __GFL_READPCFILEMANAGER_H__
#define __GFL_READPCFILEMANAGER_H__
//===========================================================================
/**
 * @file    gfl_ReadPCFileManager.h
 * @brief   GAMEFREAK Library PCファイルを読み込むために必要な初期化と解放のクラス
 * @author  k.ohno
 * @date    2010.11.18
 */
//===========================================================================
#pragma once

#if NN_SWITCH_ENABLE_HOST_IO

#include <nw/io/io_HioFileStream.h>
#include <gfl_Heap.h>

namespace gfl {
namespace fs {

class ReadPCFileManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ReadPCFileManager);

private:
  static void* deviceMemory;
  
public:

  //----------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------
  
  ReadPCFileManager(void){};

  //----------------------------------------------------------------------
  /**
   * @brief   HIOの開始 デバイスメモリから使用するメモリの確保が行われる
   * @param[in]   heapId  デバイスメモリをさしているIDを入れなければいけない
   */
  //----------------------------------------------------------------------
  static void Initialize(const gfl::heap::HEAPID heapId);

  //----------------------------------------------------------------------
  /**
   * @brief   HIOの開放 デバイスメモリの開放
   */
  //----------------------------------------------------------------------
  static void Finalize(void);
};


}  //fs
}  //gfl

#endif //GFL_DEBUG
#endif // __GFL_READPCFILEMANAGER_H__

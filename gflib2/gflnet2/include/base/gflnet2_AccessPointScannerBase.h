#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_APSCANNERBASE_H__
#define __GFLNET_APSCANNERBASE_H__
//=============================================================================
/**
 * @file	 gflnet_AccessPointScannerBase.h
 * @brief	 アクセスポイント検出インターフェース
 * @author	 kousaka Shin
 * @date	 2012.9.14
 */
//=============================================================================

#pragma once

#include <gflnet2/include/base/gflnet2_AccessPointHistoryBase.h>
#include <gflnet2/include/base/gflnet2_Base.h>


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


class AccessPointScannerBase {
  //GFL_FORBID_COPY_AND_ASSIGN(AccessPointScannerBase);

public:

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   アクセスポイントスキャン
  *
  * @param[in]  pScanBuffer スキャン用バッファ
  * @param[in]  scanBufferSize スキャン用バッファサイズ
  * @param[in]  pAccessPointHistory スキャン結果保存クラス
    *
  * @return  実行結果
  */
  //--------------------------------------------------------------------------------------------
  virtual nn::Result   ScanAccessPoint( u8* pScanBuffer , size_t scanBufferSize , AccessPointHistoryBase* pAccessPointHistory ) = 0;

//--------------------------------------------------------------------------------------------
/**
* @brief   アクセスポイントスキャン(AC版)
*
* @param[in]  pScanBuffer スキャン用バッファ
* @param[in]  scanBufferSize スキャン用バッファサイズ
* @param[in]  pAccessPointHistory スキャン結果保存クラス
  *
* @return  実行結果
*/
//--------------------------------------------------------------------------------------------
virtual nn::Result   ScanAccessPointAC( void* pScanBuffer , size_t scanBufferSize , AccessPointHistoryBase* pAccessPointHistory , gflnet2::Base* pNetBase ) = 0;


};

GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)

#endif //__GFLNET_APSCANNERBASE_H__
#endif

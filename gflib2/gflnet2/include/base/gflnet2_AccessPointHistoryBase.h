#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_APHISTORYBASE_H__
#define __GFLNET_APHISTORYBASE_H__
//=============================================================================
/**
 * @file	 gflnet_AccessPointHistoryBase.h
 * @brief	 アクセスポイント履歴インターフェース
 * @author	 kousaka Shin
 * @date	 2012.9.14
 */
//=============================================================================
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(base)


class AccessPointHistoryBase {
  //GFL_FORBID_COPY_AND_ASSIGN(AccessPointHistoryBase);

public:

	//=============================================================================================
	/**
	*  アクセスポイントのスキャン結果をチェック
	*/
	//=============================================================================================
	virtual void CheckIsKnownAccessPoint( const char* pSSID , const char* pMacString ) = 0;

	//=============================================================================================
	/**
	*  アクセスポイントのタイプをリセット
	*/
	//=============================================================================================
	virtual void ClearAccessPointType( void ) = 0;

};

GFL_NAMESPACE_END(base)
GFL_NAMESPACE_END(gflnet2)

#endif //__GFLNET_APHISTORYBASE_H__
#endif

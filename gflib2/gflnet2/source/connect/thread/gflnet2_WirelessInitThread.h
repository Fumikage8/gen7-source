#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_WIRLESSINITTHREAD_H__
#define __GFLNET_WIRLESSINITTHREAD_H__

#pragma once
//=============================================================================
/**
 * @file	 gfl_NetWirelessInitThread.h
 * @brief	 無線通信 初期管理クラス
 * @author k.ohno
 * @date	 2011.1.13
 */
//=============================================================================



#include <gflnet2/include/base/gflnet2_NetThread.h>
#include <gflnet2/include/base/gflnet2_Base.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gflnet2)



class NetWirlessInitThread : public gflnet2::base::NetThread {
  GFL_FORBID_COPY_AND_ASSIGN( NetWirlessInitThread ); // コピーコンストラクタ, 代入演算子を禁止
private:
  gflnet2::Base* mpNetBase;  //無線通信の元クラス

public:


  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  NetWirlessInitThread( gflnet2::Base* pnetbase );

  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~NetWirlessInitThread(){};



private:


  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Function( void );







};

GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_WIRLESSINITTHREAD_H__

#endif

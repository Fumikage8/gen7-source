#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_IRAUTOMATCHTHREAD_H__
#define __GFLNET_IRAUTOMATCHTHREAD_H__

#pragma once
//=============================================================================
/**
 * @file	 gflnet_IrAutoMatchThread.h
 * @brief	 赤外線 自動マッチング
 * @author k.ohno
 * @date	 2012.4.12
 */
//=============================================================================


#include <gflnet2/include/base/gflnet2_NetThread.h>
#include <gflnet2/include/base/gflnet2_Base.h>
#include <gflnet2/include/base/gflnet2_TransferedIDReceiver.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)



class IrAutoMatchThread : public gflnet2::base::NetThread {
  GFL_FORBID_COPY_AND_ASSIGN( IrAutoMatchThread ); // コピーコンストラクタ, 代入演算子を禁止
private:
  gflnet2::Base* mpNetBase;  //赤外線通信の元クラス
  gflnet2::base::TransferedIDReceiver* mpTransferedIDReceiver;

  bool bSuccess;
  
public:

  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  IrAutoMatchThread( gflnet2::Base* pnetbase );
  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~IrAutoMatchThread(){};

private:
  //----------------------------------------------------------------------------
  /**
   * @brief 開始処理
   * ※スレッドの開始直後に呼ばれる関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Initialize( void ){};

  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Main( void );
  //----------------------------------------------------------------------------
  /**
   * @brief 終了処理
   * ※スレッドの終了直前に呼ばれる関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Finalize( void ){};

  //----------------------------------------------------------------------------
  /**
   * @brief スレッド終了時の処理
   * ※Kill() が呼ばれた直後に実行される処理です。
   */
  //----------------------------------------------------------------------------
  virtual void OnKill( void ){};

  //----------------------------------------------------------------------------
  /**
   * @brief マッチングしてIDをやり取りする
   */
  //----------------------------------------------------------------------------
  nn::Result ConfirmID(void);

  bool Yield( void );

  
public:
  void SetTransferedIDReceiver(gflnet2::base::TransferedIDReceiver* pTr){mpTransferedIDReceiver=pTr;};

  bool GetSuccess(void){ return bSuccess; };
  
};

GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_IRAUTOMATCHTHREAD_H__

#endif

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliverySystem.h
 * @brief  ビーコン配信システムクラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_SYSTEM_H__
#define __GFLNET_DELIVERY_SYSTEM_H__

#include <nn.h>


#include <gflnet2/include/base/gflnet2_InitParam.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
/**
* @namespace NetAppLib::delivery
* @brief     ビーコン配信関連
*/
GFL_NAMESPACE_BEGIN(Delivery)

//--------------------------------------------------------------------------
class DeliveryRecvListener;
//--------------------------------------------------------------------------


/***************************************************************************!
 * @brief  ビーコン配信システムクラス
 ***************************************************************************/
class DeliverySystem
{
  GFL_FORBID_COPY_AND_ASSIGN(DeliverySystem); //コピーコンストラクタ＋代入禁止


public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  DeliverySystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~DeliverySystem();
protected:
  //------------------------------------------------------------------
  /**
  * @brief  DeliverySystemの初期化
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  bool InitializeBase();

  //------------------------------------------------------------------
  /**
  * @brief  DeliverySystemの終了処理
  *
  * @return 実行結果
  */
  //------------------------------------------------------------------
  void FinalizeBase( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  virtual void Update( DeliveryRecvListener* pRecvListener ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  リスナーを削除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void ){};

  //------------------------------------------------------------------
  /**
   * @brief  終了準備
   */
  //------------------------------------------------------------------
  virtual void PrepareFinalize( void ){};

  //------------------------------------------------------------------
  /**
   * @brief  停止確認
   */
  //------------------------------------------------------------------
  virtual bool IsCompretePrepareFinalize( void ){ return true; };



public:

  //------------------------------------------------------------------
  /**
  * @brief  初期化状態を調査
  *
  * @retval true： 初期済み
  * @retval false: 未初期化・または初期化中
  */
  //------------------------------------------------------------------
  inline bool IsInitialized( void ) const;

protected:
  bool                 	m_isInit;            	//!< 初期済みフラグ
  gfl2::heap::HeapBase* 	m_pHeap;				//!< ヒープ
  gflnet2::InitParam* m_pInitParam;  ///< 通信初期化パラメータ
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_SYSTEM_H__

#endif

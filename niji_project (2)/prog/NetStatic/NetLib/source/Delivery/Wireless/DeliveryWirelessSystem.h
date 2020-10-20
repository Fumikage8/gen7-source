#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessSystem.h
 * @brief  ビーコン配信システムクラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================
#ifndef __GFLNET_DELIVERY_WIRELESS_SYSTEM_H__
#define __GFLNET_DELIVERY_WIRELESS_SYSTEM_H__


#include <NetStatic/NetLib/source/Delivery/DeliverySystem.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>


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
class DeliveryWirelessBeaconBinder;
class DeliveryWirelessBeaconSplitter;
class DeliveryRecvListener;
//--------------------------------------------------------------------------


/***************************************************************************!
 * @brief  ビーコン配信システムクラス
 ***************************************************************************/
class DeliveryWirelessSystem : public NetLib::Delivery::DeliverySystem
{
	GFL_FORBID_COPY_AND_ASSIGN(DeliveryWirelessSystem); //コピーコンストラクタ＋代入禁止


public:
	//------------------------------------------------------------------
	/**
	* @brief   コンストラクタ
	*/
	//------------------------------------------------------------------
	DeliveryWirelessSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam );

	//------------------------------------------------------------------
	/**
	* @brief   デストラクタ
	*/
	//------------------------------------------------------------------
	virtual ~DeliveryWirelessSystem();

	//------------------------------------------------------------------
	/**
	* @brief  DeliveryWirelessSystemの初期化
	*
	* @param  [in]  bDoSigning  送信時：署名を施す  受信時：署名を検証する
	* @return 実行結果
	*/
	//------------------------------------------------------------------
	bool Initialize(
	        const u16     deliveryID  = 0
	      , const void*   pSendData   = 0
	      , const size_t  dataSize    = 0
        , const bool    bDoSigning  = false
	     );

	//------------------------------------------------------------------
	/**
	* @brief  DeliveryWirelessSystemの終了処理
	*
	* @return 実行結果
	*/
	//------------------------------------------------------------------
	void Finalize( void );

	//------------------------------------------------------------------
	/**
	* @brief  更新処理。
	*
	* @note   メインループ毎にポーリングすること
	*/
	//------------------------------------------------------------------
	virtual void Update( DeliveryRecvListener* pRecvListener );

public:
	
	//------------------------------------------------------------------
	/**
	* @brief  ビーコンマネージャを取得
	*
	* @return ビーコンマネージャ
	*/
	//------------------------------------------------------------------
	gflnet2::base::BeaconManager* GetBeaconManager( void );
	
	//------------------------------------------------------------------
	/**
	* @brief  ビーコン分割クラスを取得
	*
	* @return ビーコン分割クラス
	*/
	//------------------------------------------------------------------
	DeliveryWirelessBeaconSplitter* GetBeaconSplitter( void );
	
private:
	
	DeliveryWirelessBeaconBinder* m_pBeaconBinder;	//!< データ結合クラス
	DeliveryWirelessBeaconSplitter* m_pBeaconSplitter;//!<データ分割クラス
	gflnet2::base::BeaconManager*	m_pBeaconManager;//!< データ受信機
	gflnet2::base::BeaconSender*	m_pBeaconSender;//!< ビーコン送信機
};

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)

#endif // __GFLNET_DELIVERY_WIRELESS_SYSTEM_H__

#endif

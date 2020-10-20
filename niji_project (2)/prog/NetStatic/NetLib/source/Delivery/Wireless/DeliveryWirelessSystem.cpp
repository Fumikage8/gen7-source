#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryWirelessSystem.cpp
 * @brief  ビーコン配信システムクラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessSystem.h>
#include <NetStatic/NetLib/include/Delivery/DeliveryRecvListener.h>

#include "NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconBinder.h"
#include "NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessBeaconSplitter.h"
#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Delivery)


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessSystem::DeliveryWirelessSystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam )
: DeliverySystem( pHeap , pInitParam )
, m_pBeaconBinder( NULL )
, m_pBeaconSplitter( NULL )
, m_pBeaconManager( NULL )
, m_pBeaconSender( NULL )
{
  //OHNO_PRINT("@P \n");
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryWirelessSystem::~DeliveryWirelessSystem()
{
 	Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryWirelessSystemの初期化
 * @result 	実行結果
 */
//------------------------------------------------------------------
bool DeliveryWirelessSystem::Initialize(
        const u16 deliveryID
      , const void* pSendData
      , const size_t dataSize
      /*  sign  */
      , const bool  bDoSigning    /*  送信時：署名を施す  受信時：署名を検証する  */
     )
{
	GFL_PRINT("DeliveryWirelessSystem::Initialize\n");
	
	if( !m_isInit )
	{//初期化
		if( pSendData && dataSize > 0 )
		{//送信モード
      
			m_pBeaconSplitter = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessBeaconSplitter( m_pHeap , m_pInitParam , deliveryID , pSendData , dataSize, bDoSigning);
		}
		else
		{//受信モード
			m_pBeaconBinder = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessBeaconBinder( m_pHeap , m_pInitParam, bDoSigning);
		}
		
		m_pBeaconManager = GFL_NEW_LOW( m_pHeap ) gflnet2::base::BeaconManager();
		m_pBeaconSender = GFL_NEW_LOW( m_pHeap ) gflnet2::wireless::WirelessBeaconSender();
		
		m_pBeaconManager->SetInitParam( m_pInitParam );
		m_pBeaconManager->SetBeaconSender( m_pBeaconSender );
		m_pBeaconManager->SetBeaconListener( m_pBeaconBinder );
		
		m_pBeaconManager->CreateBeaconData();//ヘッダを作成
	}
	
	if( !m_pBeaconManager || !m_pBeaconSender )
	{
		Finalize();
		return false;
	}
	
	DeliverySystem::InitializeBase();
	
	return true;
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryWirelessSystemの終了処理
 * @result 実行結果
 */
//------------------------------------------------------------------
void DeliveryWirelessSystem::Finalize( void )
{
	GFL_PRINT("DeliveryWirelessSystem::Finalize\n");
	
	//if( m_isInit )
	{
		GFL_SAFE_DELETE( m_pBeaconBinder );
		GFL_SAFE_DELETE( m_pBeaconSplitter );
		GFL_SAFE_DELETE( m_pBeaconManager );
		GFL_SAFE_DELETE( m_pBeaconSender );
	}
	
	DeliverySystem::FinalizeBase();
}

//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void DeliveryWirelessSystem::Update( DeliveryRecvListener* pRecvListener )
{
	if( m_pBeaconBinder )
	{
		bool isFinish = m_pBeaconBinder->Update( pRecvListener );
	}
	
	if( m_pBeaconManager )
	{
		m_pBeaconManager->MainCheckRecvBeacon();
	}
}

//------------------------------------------------------------------
/**
* @brief  ビーコンマネージャを取得
*
* @return ビーコンマネージャ
*/
//------------------------------------------------------------------
gflnet2::base::BeaconManager* DeliveryWirelessSystem::GetBeaconManager( void )
{
	return m_pBeaconManager;
}

//------------------------------------------------------------------
/**
* @brief  ビーコン分割クラスを取得
*
* @return ビーコン分割クラス
*/
//------------------------------------------------------------------
DeliveryWirelessBeaconSplitter* DeliveryWirelessSystem::GetBeaconSplitter( void )
{
	return m_pBeaconSplitter;
}


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif

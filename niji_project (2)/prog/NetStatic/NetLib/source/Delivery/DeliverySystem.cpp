#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliverySystem.cpp
 * @brief  ビーコン配信システムクラス
 * @author Shin Kosaka
 * @date   2012.7.24
 */
//=============================================================================

#include <NetStatic/NetLib/source/Delivery/DeliverySystem.h>


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
DeliverySystem::DeliverySystem( gfl2::heap::HeapBase* pHeap , gflnet2::InitParam* pInitParam )
: m_isInit( false )
, m_pHeap( pHeap )
, m_pInitParam( pInitParam )
{
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliverySystem::~DeliverySystem()
{
 	FinalizeBase();
}

//------------------------------------------------------------------
/**
 * @brief  DeliverySystemの初期化
 * @param	type	配信タイプ（無線、Wifi、赤外線）
 * @result 	実行結果
 */
//------------------------------------------------------------------
bool DeliverySystem::InitializeBase( )
{
	if( !m_isInit )
	{//初期化
		m_isInit = true;
	}
	
	return true;
}


//------------------------------------------------------------------
/**
 * @brief  DeliverySystemの終了処理
 * @result 実行結果
 */
//------------------------------------------------------------------
void DeliverySystem::FinalizeBase( void )
{
	if( m_isInit )
	{
		m_isInit = false;
		m_pHeap = NULL;
		m_pInitParam = NULL;
	}
}


GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif

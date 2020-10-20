// ============================================================================
/*
 * @file gflnet2_CTR_CecSystem.cpp
 * @brief ctr版すれ違い通信行う為のクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecSystem.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecAllocator.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecController.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecCoreMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecSendMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecRecvMessageBox.h>
#include <nn/cec.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


CTR_CecSystem::CTR_CecSystem() :
  m_pCecAllocator( NULL ),
  m_pCecController( NULL ),
  m_pCecCoreMessageBox( NULL ),
  m_pCecSendMessageBox( NULL ),
  m_pCecRecvMessageBox( NULL )
{
}


CTR_CecSystem::~CTR_CecSystem()
{
}


// --------------------------------------------------------------------------
/**
  * @brief  すれ違い通信ライブラリの初期化を行います。
  * @param  pHeap すれ違い通信ライブラリ内で使用するヒープです。
  * @param  pInitParam すれ違い通信ライブラリの初期化パラメータです。
  */
// --------------------------------------------------------------------------
void CTR_CecSystem::Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam )
{
  m_pCecAllocator = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecAllocator( pHeap );

  nn::Result result = nn::cec::Initialize( *m_pCecAllocator );
  if( result.IsFailure() )
  {
    GFL_PRINT( "Error : cec initialize failed[0x%x].\n", result.GetPrintableBits() );
  }

  m_pCecController = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecController( pHeap );
  m_pCecCoreMessageBox = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecCoreMessageBox( m_pCecController );
  m_pCecSendMessageBox = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecSendMessageBox( m_pCecController );
  m_pCecRecvMessageBox = GFL_NEW( pHeap ) gflnet2::cec::ctr::CTR_CecRecvMessageBox( m_pCecController );

  m_pCecController->Initialize( pInitParam, m_pCecAllocator );
  m_pCecSendMessageBox->Initialize( pHeap, pInitParam );
  m_pCecRecvMessageBox->Initialize( pHeap, pInitParam );
}


// --------------------------------------------------------------------------
/**
  * @brief  すれ違い通信ライブラリの終了処理を行います。
  */
// --------------------------------------------------------------------------
void CTR_CecSystem::Terminate()
{
  GFL_SAFE_DELETE( m_pCecRecvMessageBox );
  GFL_SAFE_DELETE( m_pCecSendMessageBox );
  GFL_SAFE_DELETE( m_pCecCoreMessageBox );
  GFL_SAFE_DELETE( m_pCecController );

  nn::cec::Finalize();

  GFL_SAFE_DELETE( m_pCecAllocator );
}


// --------------------------------------------------------------------------
/**
  * @brief  すれ違い通信ライブラリの更新処理を行います。
  *         主に非同期で実行された処理のスレッドを監視してたりします。
  */
// --------------------------------------------------------------------------
void CTR_CecSystem::Update()
{
  m_pCecController->Update();
}


// --------------------------------------------------------------------------
/**
  * @brief  3DS本体のメッセージボックスを作成したり削除したりするクラスを取得します。
  * @return 3DS本体のメッセージボックスを作成したり削除したりするクラス
  */
// --------------------------------------------------------------------------
ICecCoreMessageBox* CTR_CecSystem::GetCecCoreMessageBox()
{
  return m_pCecCoreMessageBox;
}


// --------------------------------------------------------------------------
/**
  * @brief  送信ボックスに対して操作を行うクラスを取得します。
  * @return 送信ボックスに対して操作を行うクラス
  */
// --------------------------------------------------------------------------
ICecSendMessageBox* CTR_CecSystem::GetCecSendMessageBox()
{
  return m_pCecSendMessageBox;
}


// --------------------------------------------------------------------------
/**
  * @brief  受信ボックスに対して操作を行うクラスを取得します。
  * @return 受信ボックスに対して操作を行うクラス
  */
// --------------------------------------------------------------------------
ICecRecvMessageBox* CTR_CecSystem::GetCecRecvMessageBox()
{
  return m_pCecRecvMessageBox;
}


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif

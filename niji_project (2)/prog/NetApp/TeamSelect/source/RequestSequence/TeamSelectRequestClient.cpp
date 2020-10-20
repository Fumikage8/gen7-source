// ============================================================================
/*
 * @file TeamSelectRequestClient.cpp
 * @brief TeamSelectアプリで必要な通信リクエストを投げる為のクラスです。
 * @date 2015.06.23
 */
// ============================================================================
#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectRequestClient.h"
#include "NetApp/TeamSelect/source/RequestSequence/TeamSelectValidationRequestSequence.h"
#include <gflnet2/include/ServerClient/gflnet2_ServerClientRequestManager.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)


//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ/デストラクタ
 */
//--------------------------------------------------------------------------------------------
TeamSelectRequestClient::TeamSelectRequestClient( gfl2::heap::HeapBase* pHeap )
: m_handle(0)
, m_pHeap(pHeap)
{
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Initialize( pHeap );
  }
}
TeamSelectRequestClient::~TeamSelectRequestClient()
{
  gflnet2::ServerClient::ServerClientRequestManager::RemoveListener( m_handle );
  if( GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager) ) 
  {
    GFL_SINGLETON_INSTANCE(gflnet2::ServerClient::ServerClientRequestManager)->Finalize();
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   レギュレーションチェックと不正チェックをまとめて行う非同期処理の実行をリクエストする
 * @param   pListener 結果を受け取るリスナー
 * @param   requestParam リクエストパラメータ
 */
//--------------------------------------------------------------------------------------------
void TeamSelectRequestClient::CheckRegulationAndValidationRequest( TeamSelectResponseListener* pListener , CHECK_REQUEST_PARAM& requestParam )
{
  requestParam.pHeap = m_pHeap;
  m_handle = gflnet2::ServerClient::ServerClientRequestManager::AddRequest< TeamSelectValidationRequestSequence, TeamSelectResponseListener , CHECK_REQUEST_PARAM >( pListener, requestParam );
}



GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gflnet_Manager.cpp
 * @brief	 通信管理クラス
 * @author k.ohno
 * @date	 2011.1.6
 */
//=============================================================================


#include "gflnet2/include/scan/gflnet2_Ir.h"
//#include "gflnet2/include/scan/gflnet2_WifiBase.h"

#include "gflnet2/source/scan/proc/gflnet2_WirelessScanProc.h"
#include "gflnet2/source/scan/proc/gflnet2_IrScanProc.h"

#include "gflnet2/include/beacon/gflnet2_BeaconManager.h"

#include "gflnet2/include/base/gflnet2_AccessPointScannerBase.h"
#include <gflnet2/include/scan/gflnet2_Wireless.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


//------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------
Manager::Manager(gfl2::heap::HeapBase * pBase)
: gfl2::proc::Manager(pBase)
, pNetBase(NULL) ///< 基底クラスを保存しておく変数 今のところ一個だけしか起動できないようにしてある
, mpNetW(NULL)
, mpIr(NULL)
, mpBeaconManager(NULL)
, mpBaseProc(NULL)
, mpBeaconSender(NULL)
, mpNetStateListener(NULL) //状態遷移を伝えるリスナー
, mpTransferedIDReceiver(NULL)
, mpAccessPointScanner(NULL)
, mpAccessPointHistory(NULL)
, bEndSignal(false)
{
}

//------------------------------------------------------------------
/**
 * @brief	  デストラクタ
 */
//------------------------------------------------------------------

Manager::~Manager()
{
  //OHNO_PRINT("Manager::~Manager()\n");
}

//------------------------------------------------------------------
/**
 * @brief	  通信初期化パラメータに対応した通信クラスを作成する
 * @param   pIParam   通信初期化パラメータ
 * @param   pBase  ヒープクラス
 */
//------------------------------------------------------------------
void Manager::CreateNetwork(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix,
                            gflnet2::base::BeaconManager* pBeaconManager,
                            gflnet2::base::AccessPointScannerBase* pAccessPointScanner,
                            gflnet2::base::AccessPointHistoryBase* pAccessPointHistory)
{
  gflnet2::Manager* pNetMan = this;

  if(pBeaconManager){
    mpBeaconManager = pBeaconManager;
  }
  if(pAccessPointScanner){
    mpAccessPointScanner = pAccessPointScanner;
  }
  if(pAccessPointHistory){
    mpAccessPointHistory = pAccessPointHistory;
  }
  
  //OHNO_PRINT("Scan::Manager::CreateNetwork::%d\n",pIParam->type);
  
  switch(pIParam->type){

  case gflnet2::TYPE_P2P_WIFI:
    GFL_ASSERT(0); //@fix
    break;
  case gflnet2::TYPE_P2P_WIRELESS:
    {
      gflnet2::NetWirelessScanProc* pProc;
      Wireless* pNetW;

      mpNetW = GFL_NEW(pIParam->pHeapBase) Wireless(pIParam,pIParamFix);
      pNetW=mpNetW;
      pNetBase = mpNetW;
      mpBeaconManager->SetInitParam( pNetW->GetInitParam() );
  //    mpBeaconSender = GFL_NEW(pIParam->pHeapBase) gflnet2::wireless::WirelessBeaconSender();
  //    mpBeaconManager->SetBeaconSender( mpBeaconSender );
      pNetBase->SetBeaconManager(mpBeaconManager);

      pProc = GFLIB_PROC_CALL<gflnet2::Manager, gflnet2::NetWirelessScanProc>( pNetMan);
      pProc->SetThread(gflnet2::NET_THREAD_INITIALIZE, mpNetW);
      pProc->SetNetManager(this);

      pProc->SetAPAcc(mpAccessPointScanner,mpAccessPointHistory);
      mpBaseProc=pProc;

    }
    break;
  case gflnet2::TYPE_P2P_IR:
    {
      gflnet2::IrScanProc* pProc;

      mpIr = GFL_NEW(pIParam->pHeapBase) Ir(pIParam,pIParamFix);
      pNetBase = mpIr;
      mpBeaconManager->SetInitParam( pNetBase->GetInitParam() );
      pProc = GFLIB_PROC_CALL<gflnet2::Manager, gflnet2::IrScanProc>( pNetMan );
      pProc->SetTransferedIDReceiver(mpTransferedIDReceiver);
      pProc->SetThread(gflnet2::NET_THREAD_INITIALIZE, mpIr);
      pProc->SetManagerListener(mpNetStateListener);

      pNetBase->SetBeaconManager(mpBeaconManager);
      mpBaseProc=pProc;
    }

    break;

  default:
    GFL_ASSERT(0); //@fix
    break;
  }


}



gfl2::proc::MainStatus Manager::MainProcess(void)
{
  if(pNetBase && bEndSignal){
    EndingNetwork();
  }
  return Main();
}


//------------------------------------------------------------------
/**
 * @brief	  通信終了をPROCに伝達
 */
//------------------------------------------------------------------


void Manager::Finalize(void)
{
  bEndSignal=true;
}




//------------------------------------------------------------------
/**
 * @brief	  通信終了をマネージャーに通知する
 */
//------------------------------------------------------------------


bool Manager::EndingNetwork(void)
{
  gflnet2::BaseProcess* pNBP=mpBaseProc;

  if(pNBP){
    if(!IsProcEnd()){
      //OHNO_PRINT("KillThreadReq\n");
      pNBP->ReqEnd();
      return true;
    }
  }
  return false;
}



//---------------------------------------------------------------------------------
/**
 * @brief   通信切断完了確認
 */
//---------------------------------------------------------------------------------
bool Manager::IsProcEnd(void)
{
  if(GetBaseProcess()){
    return false;
  }
  if(GetNextBaseProcess()){
    return false;
  }
  if(IsMainHandle()){
    return false;
  }
  return true;
}



//------------------------------------------------------------------
/**
 * @brief	  通信終了をマネージャーに通知する
 * @return  完了したらtrue 完了していない場合false
 */
//------------------------------------------------------------------


void Manager::DeleteNetwork(void)
{
  //OHNO_PRINT("Manager::DeleteNetwork\n");
  gflnet2::BaseProcess* pNBP=mpBaseProc;
  if(pNBP){
    if(!IsProcEnd()){
      pNBP->SetThread(gflnet2::NET_THREAD_END, NULL);
    }
  }
  if(mpBeaconManager){
    mpBeaconManager->SetInitParam( NULL );
    mpBeaconManager = NULL;
  }
  if(mpBeaconSender){
    GFL_DELETE(mpBeaconSender);
    mpBeaconSender=NULL;
  }
  if(mpNetW){
    GFL_DELETE(mpNetW);
  }
  else{
    GFL_DELETE(mpIr);
  }
  pNetBase = NULL;
  mpBaseProc=NULL;
  //return true;
}





GFL_NAMESPACE_END(gflnet2)

#endif

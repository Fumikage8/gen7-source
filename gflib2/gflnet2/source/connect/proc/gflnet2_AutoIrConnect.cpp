#ifdef  GF_PLATFORM_CTR
//================================================================================
/**
 * @file   AutoIrConnect.cpp
 * @brief  赤外線で通信する
 *			xyよりgflnetへ移植(2012/08/20 shin kosaka)
 * @author k.ohno
 * @date   2012.4.19
 */
//================================================================================
#include <gflnet2/include/connect/gflnet2_AutoIrConnect.h>

#include <gflnet2/source/dev/gflnet2_PiaAdapter.h>
#include <gflnet2/source/dev/gflnet2_IrAdapter.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(connect)

//================================================================================
/**
 *    AutoIrConnect
 */
//================================================================================
//------------------------------------------------------------------
/**
 * @brief   コンストラクタ  通信自動接続を開始します
 */
//------------------------------------------------------------------
AutoIrConnect::AutoIrConnect( const gflnet2::InitParameter& irInitParam , const gflnet2::InitParameterFixed& irInitParamFixed , gflnet2::base::BeaconManager* pBeaconManager )
: mpManagerListener(NULL)
, mTransferedIDReceiver()
, mpP2pManager(NULL)
, mpNetGame(NULL)     //ベース用netgame
, mNetMng(NULL)
, m_irInitParam(irInitParam)
, m_irInitParamFixed(irInitParamFixed)
, m_pBeaconManager( pBeaconManager )
, mStep(_START)
, mFinishTimer(0)
, mbEnd(false)
, mbConnected(false)
, mbManagerEnd(false)
, mbShutdownCall(false)
{
	KOUSAKA_PRINT("gflnet2::AutoIrConnect::AutoIrConnect \n");
	//OHNO_PRINT("gflnet2::AutoIrConnect::AutoIrConnect \n");
}


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
AutoIrConnect::~AutoIrConnect()
{
	GFL_SAFE_DELETE( mNetMng );//一応
	GFL_SAFE_DELETE( mpNetGame );
	GFL_SAFE_DELETE( mpP2pManager );
	
	GFL_SAFE_DELETE( mpManagerListener );
	
	m_pBeaconManager = NULL;
}




//------------------------------------------------------------------
/**
 * @brief   通信処理を勝手に進めます
 */
//------------------------------------------------------------------

void AutoIrConnect::Update( void )
{
	if(mpP2pManager){
		mpP2pManager->NetMain();
	}
#if GFL_DEBUG
  static int debugno=-1;
  if(debugno != mStep){
    //OHNO_PRINT("AutoIrConnect::Update %d\n",mStep);
    debugno = mStep;
  }
#endif
  switch(mStep){
  case _START:
    KOUSAKA_PRINT("gflnet2::AutoIrConnect:Update step=_START\n");
    InitializeInternal();
    mStep++;
//    break;
  case _LOOP_MATCH:
    mStep++;
//    _MngMain(_CONNECT_P2P);
//    break;
  case _CONNECT_P2P:
    {
      KOUSAKA_PRINT("AutoIrConnect:Update step=_CONNECT_P2P\n");
      InitP2pInternal();
      mStep++;
    }
//    break;
  case _CONNECT:
      KOUSAKA_PRINT("gflnet2::AutoIrConnect:Update step=_CONNECT\n");
    if(mpP2pManager){
      //mpP2pManager->ConnectIR(mTransferedIDReceiver.IsMaster(), mTransferedIDReceiver.Get());
    }
    mStep++;
//    break;
	case _CONNECTED:
    if(mpP2pManager){
      //if(mpP2pManager->IsConnectEnd())
      {
        KOUSAKA_PRINT("gflnet2::AutoIrConnect:Update step=_CONNECTED\n");
        mpNetGame->Initialize();
        //mpP2pManager->MatchingIR();
     //   mbConnected = true;
        mStep++;
      }
    }
    break;
	case _CONNECTED2:
    if(mpNetGame->IsInitialize()){
      mbConnected = true;
      mStep++;
    }
    break;
	case _CONNECTED3:




    //送信可能状態
	break;

	case _SHUTDOWN:
    if(mpP2pManager){
      //mpP2pManager->PreShutdown();
    }
    mFinishTimer = FINISH_DOWN_TIMER;
    //maErrTimeout.Start();
    mStep++;
    break;
  case _SHUTDOWN2:
    if(mFinishTimer>0){
      mFinishTimer--;
      break;
    }
    if(mpP2pManager){
      //mpP2pManager->Shutdown();
    }
    mStep++;
    break;
  case _SHUTDOWN_WAIT:
    endP2p();
    break;
  case _END:
    //maErrTimeout.Stop();
    break;
	}

}


//------------------------------------------------------------------
/**
 * @brief   終了時に呼ぶ処理
 */
//------------------------------------------------------------------

void AutoIrConnect::endP2p( void )
{
  if(mpP2pManager) {
    //if(mpP2pManager->IsShutdownEnd())  
    {
      //mpP2pManager->Exit();
      if( mpNetGame ){
        mpNetGame->Finalize();
      }
      mbEnd = true;
    }
  }
  else{
    mbEnd = true;
  }
  if(mbEnd){
    //maErrTimeout.Stop();
    mStep++;
  }
}




//------------------------------------------------------------------
/**
 * @brief   通信を終了したい場合によびます  updateは isEnd=trueになるまで呼んで下さい
 */
//------------------------------------------------------------------
bool AutoIrConnect::Finalize( void )
{
  if(!mbShutdownCall){
    mbShutdownCall=true;
    mStep = _SHUTDOWN;
  }
	return true;
}

//------------------------------------------------------------------
/**
 * @brief   接続したかどうかを返します
 * @return  true 接続
 */
//------------------------------------------------------------------
bool AutoIrConnect::isConnect(void)
{
  return mbConnected;
}

//------------------------------------------------------------------
/**
 * @brief   通信が終了したかどうかを返します
 * @return  true 終了 デストラクタ呼んでいい
 */
//------------------------------------------------------------------
bool AutoIrConnect::IsFinalized(void)
{
	return mbEnd;
}

//------------------------------------------------------------------
/**
 * @brief		データ送信
 * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
 * @param  size      送信サイズ
 * @return bool      送る準備ができたらtrue だめならfalse
 */
//------------------------------------------------------------------
bool AutoIrConnect::SendData( void* pSendData, u32 size )
{
	return mpNetGame->SendData( (char*)pSendData, size );
}

//------------------------------------------------------------------
/**
 * @brief		データ受信
 * @param  id        送ってきた人の通信ID
 * @param  pRecvData 受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @return bool      データがあればtrue なければfalse
 */
//------------------------------------------------------------------
bool AutoIrConnect::RecvData( int id, char* pRecvData, int size )
{
	return mpNetGame->RecvData( id, pRecvData, size );
}

//------------------------------------------------------------------
/**
 * @brief   データ受信
 * @param  id        送ってきた人の通信ID
 * @param  pRecvData 受け取るバッファ
 * @param  size      受信バッファサイズ これ以上は受け取りません
 * @param  pSize     受け取ったサイズを返す
 * @return bool      データがあればtrue なければfalse
 */
//------------------------------------------------------------------
bool AutoIrConnect::RecvData( int id, char* pRecvData, int size, u16* pSize )
{
  return mpNetGame->RecvData( id, pRecvData, size, pSize );
}

//--------------------------------------------------------------
/**
 * @brief   初期化
 */
//--------------------------------------------------------------
bool AutoIrConnect::InitializeInternal()
{
#if 0
  mNetMng = GFL_NEW( m_irInitParam.pHeapBase ) gflnet2::Manager( m_irInitParam.pHeapBase );
	mNetMng->SetTransferedIDReceiver( &mTransferedIDReceiver );

	mpManagerListener = GFL_NEW( m_irInitParam.pHeapBase ) ManagerListenerConnect( this );

	mNetMng->SetManagerListener( mpManagerListener );
	mNetMng->CreateNetwork( &m_irInitParam ,
	                     &m_irInitParamFixed ,
	                     m_pBeaconManager );
#else
  SetManagerEnd();

#endif
  
	KOUSAKA_PRINT("gflnet2::AutoIrConnect::InitializeInternal \n");
	return true;
}

//------------------------------------------------------------------
/**
 * @brief   P2P初期化
 */
//------------------------------------------------------------------
void AutoIrConnect::InitP2pInternal( void )
{
  //mpP2pManager = GFL_NEW( m_irInitParam.pHeapBase ) gflnet2::p2p::P2pManager( m_irInitParam.pHeapBase,  m_irInitParam.pHeapDev, m_irInitParam.pHeapBase );
  //mpP2pManager->Initialize( &m_irInitParam,
   //                         &m_irInitParamFixed,
    //                        m_pBeaconManager );

  //gflnet2::p2p::GameInitialize init;
  //init.max = m_irInitParam.LimitNum;
  //KOUSAKA_PRINT("接続開始人数 %d\n", init.max);
  //init.pHeapSystem = m_irInitParam.pHeapBase;  //通信で使うHEAPID
  //mpNetGame = GFL_NEW( m_irInitParam.pHeapBase ) gflnet2::p2p::NetGame( &init , mpP2pManager->GetInitParam() );
}


//------------------------------------------------------------------
/**
 * @brief  NetMng更新
 */
//------------------------------------------------------------------
void AutoIrConnect::MngMainInternal(int mode)
{
#if 0
  if(gfl2::proc::MAINSTATUS_NULL == mNetMng->Main()){
    mNetMng->DeleteNetwork();
    GFL_SAFE_DELETE( mNetMng );
    mNetMng = NULL;
    if(mode){
      mStep = mode;
    }
  }
#else
      mStep = mode;
#endif
}


bool AutoIrConnect::IsMachineMaster( void )
{
  return gflnet2::dev::IrAdapter::GetInstance()->IsMaster();
}


//================================================================================
/**
 *    ManagerListenerConnect
 */
//================================================================================
//------------------------------------------------------------------
/**
 * @brief  コンストラクタ
 */
//------------------------------------------------------------------
ManagerListenerConnect::ManagerListenerConnect(AutoIrConnect* pAutoIrConnect)
: mpAutoIrConnect( pAutoIrConnect )
{
}


//------------------------------------------------------------------
/**
 * @brief  作業終了
 */
//------------------------------------------------------------------
void ManagerListenerConnect::OnSpecificProcessingEnd( void )
{
	KOUSAKA_PRINT("gflnet2::OnSpecificProcessingEnd\n");
	if( mpAutoIrConnect )
	{
		mpAutoIrConnect->SetManagerEnd();
	}
}

//------------------------------------------------------------------
/**
 * @brief  作業中の取り決めた状態を返す
 */
//------------------------------------------------------------------
void ManagerListenerConnect::OnUpdateState( int num )
{
  //特に実装なし
}

GFL_NAMESPACE_END(connect)
GFL_NAMESPACE_END(gflnet2)
#endif

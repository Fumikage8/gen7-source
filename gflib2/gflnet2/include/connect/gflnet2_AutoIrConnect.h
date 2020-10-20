#ifdef  GF_PLATFORM_CTR
//================================================================================
/**
 * @file   AutoIrConnect.h
 * @brief  赤外線でつないだあと無線に切り替える
*			xyよりgflnetへ移植(2012/08/20 shin kosaka)
 * @author k.ohno
 * @date   2012.4.12
 */
//================================================================================
#ifndef __GFLNET_AUTOIRCONNECT_H__
#define __GFLNET_AUTOIRCONNECT_H__

#include <gflnet2/include/base/gflnet2_InitParam.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>
#include <gflnet2/include/base/gflnet2_TransferedIDReceiver.h>
#include <gflnet2/include/p2p/gflnet2_P2pManager.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#include <gflnet2/include/scan/gflnet2_Manager.h>
#include <gflnet2/include/base/gflnet2_NetStateListener.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
/**
* @namespace gflnet2::connect
* @brief     赤外線の接続 送受信を行う
*/
GFL_NAMESPACE_BEGIN(connect)



class ManagerListenerConnect;


//================================================================================
/**
 *    AutoIrConnect
 */
//================================================================================
class AutoIrConnect
{
GFL_FORBID_COPY_AND_ASSIGN( AutoIrConnect );

private:
  
	static const int AUTOCONNECT_MAX = 2;
  static const int FINISH_DOWN_TIMER = 60;

	enum
	{
		_START = 0,
    _LOOP_MATCH,
    _CONNECT_P2P,
		_CONNECT,
		_CONNECTED,
		_CONNECTED2,
		_CONNECTED3,
    _SHUTDOWN,
    _SHUTDOWN2,
    _SHUTDOWN_WAIT,
    _END,
	};


public:
	enum
	{
		AUTOMATCH_BOTH = 0,
		AUTOMATCH_PARENT,
		AUTOMATCH_CHILD,
	};



public:

	//------------------------------------------------------------------
	/**
	* @brief   コンストラクタ  通信自動接続を開始します
	*/
	//------------------------------------------------------------------
	AutoIrConnect( const gflnet2::InitParameter& irInitParam , const gflnet2::InitParameterFixed& irInitParamFixed , gflnet2::base::BeaconManager* pBeaconManager );
	~AutoIrConnect();

	//------------------------------------------------------------------
	/**
	* @brief   通信処理を勝手に進めます
	*/
	//------------------------------------------------------------------
	void Update( void );
	//------------------------------------------------------------------
	/**
	* @brief   通信を終了したい場合によびます  updateは isEnd=trueになるまで呼んで下さい
	          NetGameクラスは解放しておいて下さい
	*/
	//------------------------------------------------------------------
	bool Finalize( void );
	//------------------------------------------------------------------
	/**
	* @brief   接続したかどうかを返します
	          これがTRUEなら通信可能です
	          NetGameクラスを作成して送受信を開始できます
	* @return  true 接続
	*/
	//------------------------------------------------------------------
	bool isConnect(void);
	//------------------------------------------------------------------
	/**
	* @brief   通信が終了したかどうかを返します
	* @return  true 終了 デストラクタ呼んでいい
	*/
	//------------------------------------------------------------------
	bool IsFinalized(void);

	//マネージャからの終了通知設定
	inline void SetManagerEnd(void){   mbManagerEnd=true; }

	//------------------------------------------------------------------
	/**
	* @brief		データ送信
	 * @param  sendData  送るデータ  UDS_PACKET_PAYLOAD_MAX_SIZE を超えると、関数の実行は失敗します。
	 * @param  size      送信サイズ
	 * @return bool      送る準備ができたらtrue だめならfalse
	*/
	//------------------------------------------------------------------
	bool SendData( void* pSendData, u32 size );
	//------------------------------------------------------------------
	/**
	* @brief		データ受信
	 * @param  id        送ってきた人の通信ID
	 * @param  pRecvData 受け取るバッファ
	 * @param  size      受信バッファサイズ これ以上は受け取りません
	 * @return bool      データがあればtrue なければfalse
	*/
	//------------------------------------------------------------------
	bool RecvData( int id, char* pRecvData, int size );
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
  bool RecvData( int id, char* pRecvData, int size, u16* pSize );

  //------------------------------------------------------------------
  /**
   * @brief  親かどうか
   * @return bool      true=親
   */
  //------------------------------------------------------------------
  bool IsMachineMaster(void);


  
private:
	//初期化
	bool InitializeInternal();
	//P2P初期化
	void InitP2pInternal(void );
	//マネージャ更新
	void MngMainInternal(int mode);
	//P2P終了
  void endP2p( void );

private:

	ManagerListenerConnect* mpManagerListener;
	gflnet2::base::TransferedIDReceiver mTransferedIDReceiver;
	gflnet2::p2p::P2pManager* mpP2pManager;
	gflnet2::p2p::NetGame*  mpNetGame;     //ベース用netgame
	gflnet2::Manager* mNetMng;
	gflnet2::InitParameter m_irInitParam;
	gflnet2::InitParameterFixed m_irInitParamFixed;
	gflnet2::base::BeaconManager*	m_pBeaconManager;//!< ビーコン受信機
  //gflnet2::err::ErrTimeout maErrTimeout;
	int mStep;
  int mFinishTimer;
	bool mbEnd;
	bool mbConnected;
	bool mbManagerEnd;
  bool mbShutdownCall;
};


//================================================================================
/**
 *    ManagerListenerConnect
 */
//================================================================================
class ManagerListenerConnect : public gflnet2::NetStateListener
{
  GFL_FORBID_COPY_AND_ASSIGN( ManagerListenerConnect );

private:
  AutoIrConnect* mpAutoIrConnect;


public:
  ManagerListenerConnect(AutoIrConnect* pAutoIrConnect);
  virtual ~ManagerListenerConnect(){};
  
  //------------------------------------------------------------------
  /**
   * @brief  作業終了
   */
  //------------------------------------------------------------------
  virtual void OnSpecificProcessingEnd( void );

  //------------------------------------------------------------------
  /**
   * @brief  作業中の取り決めた状態を返す
   */
  //------------------------------------------------------------------
  virtual void OnUpdateState( int num );
};


GFL_NAMESPACE_END(connect)
GFL_NAMESPACE_END(gflnet2)
#endif //__GFLNET_AUTOIRCONNECT_H__
#endif

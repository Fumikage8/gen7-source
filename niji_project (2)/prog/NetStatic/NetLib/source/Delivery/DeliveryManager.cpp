#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_DeliveryManager.cpp
 * @brief  ビーコン配信マネージャクラス
 * @author Shin Kosaka
 * @date   2012.7.23
 */
//=============================================================================

#include <NetStatic/NetLib/include/Delivery/DeliveryManager.h>
#include <NetStatic/NetLib/include/Delivery/DeliverySendListener.h>


#include "NetStatic/NetLib/source/Delivery/Wireless/DeliveryWirelessSystem.h"


#include "NetStatic/NetLib/source/Delivery/Wifi/DeliveryWifiSystem.h"
#include "NetStatic/NetLib/source/Delivery/IR/DeliveryIRSystem.h"



#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(NetLib::Delivery::DeliveryManager)
template class gfl2::base::SingletonAccessor<NetLib::Delivery::DeliveryManager>;
#endif


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
DeliveryManager::DeliveryManager( void )
: m_isInit( false )
, m_pSendThread( NULL )
, m_pRecvThread( NULL )
, m_deliveryType( DELIVERY_TYPE_wireless )
, m_pHeap( NULL )
, m_pSendListener( NULL )		//!< 送信イベントリスナー
, m_pRecvListener( NULL )		//!< 受信イベントリスナー
, m_pDeliverySystem( NULL )
, m_InitParam()
//, m_InitParameter()//!<アプリ固有通信初期化パラメータ（InitParamに入ってるが、個別でも必要）
//, m_InitParameterFixed()//!<タイトル固有通信初期化パラメータ（InitParamに入ってるが、個別でも必要）
, m_pFilter(NULL)
, m_SerialCode(0)
, m_bDoUDSSigning(false)
, m_bDoUDSVerifySign(false)
{
  std::memset( &m_InitParameter , 0 , sizeof(m_InitParameter) );
  std::memset( &m_InitParameterFixed , 0 , sizeof(m_InitParameterFixed) );
  std::memset( &m_attributes , 0 , sizeof(m_attributes) );

  std::memset( m_attribute1 , 0 , sizeof(m_attribute1) );
  std::memset( m_attribute2 , 0 , sizeof(m_attribute2) );
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
DeliveryManager::~DeliveryManager()
{
   Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryManagerの初期化
 * @param	type	配信タイプ（無線、Wifi、赤外線）
 * @result 	実行結果
 */
//------------------------------------------------------------------
nn::Result DeliveryManager::Initialize( const gflnet2::InitParameter* pIParam,
                    const gflnet2::InitParameterFixed* pIParamFix,
                    const DELIVERY_TYPE type )
{
  nn::Result ret = nn::ResultSuccess();

  if( !m_isInit )
  {//初期化

    GFL_PRINT("DeliveryManager::Initialize\n");

    m_isInit = true;
    m_deliveryType = type;
    m_pHeap = pIParam->pHeapBase;

    m_InitParameter = *pIParam;
    m_InitParameterFixed = *pIParamFix;
    m_InitParam.Initialize(pIParam,pIParamFix);

    std::memset( &m_attributes , 0 , sizeof(m_attributes) );
  }

  return ret;
}


//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void DeliveryManager::Update( void )
{
  if( m_pDeliverySystem )
  {
    m_pDeliverySystem->Update( m_pRecvListener );
  }
}

//------------------------------------------------------------------
/**
 * @brief  DeliveryManagerの終了処理
       スレッドが起動中だと停止までブロックされます
       事前にPrepareFinalizeしてIsCompretePrepareFinalizeからtrueが返るのを待ってください
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result DeliveryManager::Finalize( void )
{
  nn::Result ret = nn::ResultSuccess();

  if( m_isInit )
  {
    GFL_PRINT("DeliveryManager::Finalize\n");

    m_isInit = false;

    if( m_pDeliverySystem )
    {
      DeleteAsyncInternal();
      GFL_SAFE_DELETE( m_pDeliverySystem );
    }

    m_pSendListener = NULL;		//!< 送信イベントリスナー
    m_pRecvListener = NULL;		//!< 受信イベントリスナー
    m_pHeap = NULL;
    m_pFilter = NULL;

    /*  serial  */
    m_SerialCode  = 0;
    std::memset( m_attribute1 , 0 , sizeof(m_attribute1) );
    std::memset( m_attribute2 , 0 , sizeof(m_attribute2) );
  }

  return ret;
}

//------------------------------------------------------------------
/**
 * @brief   検索属性の設定（Wifi用）
 */
//------------------------------------------------------------------
void DeliveryManager::SetAttribute( const gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES* pAttributes )
{
  if( !m_isInit || m_deliveryType != DELIVERY_TYPE_wifi || pAttributes == NULL )
    return;

  std::memcpy( &m_attributes , pAttributes , sizeof(m_attributes) );
}

//------------------------------------------------------------------
/**
 * @brief   検索属性の削除（Wifi用）
 */
//------------------------------------------------------------------
void DeliveryManager::ClearAttribute( s32 attributeIndex )
{
  if( !m_isInit || m_deliveryType != DELIVERY_TYPE_wifi )
  {
    return;
  }

  if( attributeIndex < 0 || attributeIndex > ATTRIBUTE_INDEX_MAX )
  {
    return;
  }

  switch( attributeIndex )
  {
    case 0:
    {
      m_attributes.pAttribute1 = NULL;
    }
    break;
    case 1:
    {
      m_attributes.pAttribute2 = NULL;
    }
    break;
    case 2:
    {
      m_attributes.pAttribute3 = NULL;
    }
    break;
    default:
    break;
  }

  GFL_PRINT("DeliveryManager::ClearAttribute : index[%d]\n",attributeIndex );
}

//------------------------------------------------------------------
/**
 * @brief   検索用フィルターの設定（Wifi用）
 */
//------------------------------------------------------------------
void DeliveryManager::SetFileListFilter( DeliveryFileListFilter* pFilter )
{
  if( !m_isInit || m_deliveryType != DELIVERY_TYPE_wifi || pFilter == NULL )
    return;

  m_pFilter = pFilter;
}

//------------------------------------------------------------------
/**
 * @brief   送信開始
 */
//------------------------------------------------------------------
bool DeliveryManager::StartSend( const int threadPriority , const u16 deliveryID , const void* pSendData , const size_t dataSize )
{
  if( !m_isInit )
  {
    return false;
  }

  if( m_pDeliverySystem )
  {
    return true;
  }

  switch( m_deliveryType )//Factory形式にした方がいいかも
  {
  case DELIVERY_TYPE_wireless:		//無線
    {
      GFL_PRINT("DeliveryManager::StartSend:Wireless\n");

      //m_pDeliverySystem = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessSystem( m_pHeap , &m_InitParam );
      //reinterpret_cast<DeliveryWirelessSystem*>( m_pDeliverySystem )->Initialize( deliveryID , pSendData , dataSize );
      //m_pThread = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessSendThread( m_pHeap , THREAD_STACK_SIZE ,reinterpret_cast<DeliveryWirelessSystem*>( m_pDeliverySystem ));

      DeliveryWirelessSystem* pSystem = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessSystem( m_pHeap , &m_InitParam );
      pSystem->Initialize(deliveryID, pSendData, dataSize, m_bDoUDSSigning);
      m_pSendThread = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessSendThread( m_pHeap , THREAD_STACK_SIZE , pSystem );

      m_pDeliverySystem = pSystem;
    }
  break;
  case DELIVERY_TYPE_wifi:			//Wifi
    {
      GFL_PRINT("DeliveryManager::StartSend: error Wifi is not surported\n");//Wifiは送信機能無し
      GFL_ASSERT(0);  //@fix
    }
  break;
  case DELIVERY_TYPE_ir:				//赤外線
    {
      GFL_PRINT("DeliveryManager::StartSend:IR\n");

      DeliveryIRSystem* pSystem = GFL_NEW_LOW( m_pHeap ) DeliveryIRSystem( m_pHeap , &m_InitParam , &m_InitParameter , &m_InitParameterFixed );
      if( !pSystem->Initialize( threadPriority , deliveryID , pSendData , dataSize ) )
      {
        GFL_SAFE_DELETE( pSystem );
      }

      m_pDeliverySystem = pSystem;
    }
  break;
  }

  if( !m_pDeliverySystem )
  {
    Finalize();
    return false;
  }

  if( m_pSendThread )
  {
    m_pSendThread->Start();
  }

  return true;
}

//------------------------------------------------------------------
/**
 * @brief   受信開始
 */
//------------------------------------------------------------------
bool DeliveryManager::StartRecv( int threadPriority , bool isWifiConnected )
{
  if( !m_isInit )
  {
    return false;
  }

  if( m_pDeliverySystem )
  {
    return true;
  }

  switch( m_deliveryType )
  {
  case DELIVERY_TYPE_wireless:		//無線
    {
      GFL_PRINT("DeliveryManager::StartRecv:Wireless\n");
      SetFileListFilter( NULL );
      DeliveryWirelessSystem* pSystem = GFL_NEW_LOW( m_pHeap ) DeliveryWirelessSystem( m_pHeap , &m_InitParam );

      if( !pSystem->Initialize(0, NULL, 0, m_bDoUDSVerifySign) )
      {//失敗
        GFL_SAFE_DELETE( pSystem );
      }
      else
      {
        m_pRecvThread = GFL_NEW_LOW(m_pHeap) DeliveryWirelessRecvThread( m_pHeap , THREAD_STACK_SIZE , pSystem );
      }

      m_pDeliverySystem = pSystem;
    }
  break;
  case DELIVERY_TYPE_wifi:			//Wifi
    {
      GFL_PRINT("DeliveryManager::StartRecv:Wifi\n");

      {//Wifi設定
        ClearAttribute(0);
        ClearAttribute(1);
        ClearAttribute(2);

        gflnet2::boss::BossManager::BOSS_SEARCH_ATTRIBUTES attr;

        NetAppLib::Util::BossUtility::BOSS_APP_TYPE type;
        if( m_SerialCode )
        {//シリアル
          std::snprintf(m_attribute2, sizeof(m_attribute2), "%u", m_SerialCode);
          attr.pAttribute2 = m_attribute2;

          type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_giftSerial;
        }
        else
        {//シリアル以外
          NetAppLib::Util::BossUtility::CreateXYBossAttribute3( NetAppLib::Util::BossUtility::BOSS_APP_TYPE_gift , m_attribute2 );
          attr.pAttribute2 = m_attribute2;

          type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_gift;
        }

        //BOSSattribute1作成
        char attribute1[NetAppLib::Util::BossUtility::ATTRIBUTE_BUFFER_SIZE];
        NetAppLib::Util::BossUtility::CreateXYBossAttribute1( type , m_attribute1 );
        attr.pAttribute1 = m_attribute1;

        GFL_PRINT("WonderGiftRecv::_CreateAttribute : [%s]\n",attr.pAttribute1 );

        SetAttribute( &attr );
        SetFileListFilter( this );
      }


      DeliveryWifiSystem* pSystem = GFL_NEW_LOW( m_pHeap ) DeliveryWifiSystem( m_pHeap , &m_InitParam , &m_attributes , m_pFilter , isWifiConnected );
      if( !pSystem->Initialize( m_pRecvListener , threadPriority ) )
      {
        GFL_SAFE_DELETE( pSystem );
      }

      m_pDeliverySystem = pSystem;
    }
  break;
  case DELIVERY_TYPE_ir:				//赤外線
    {
      GFL_PRINT("DeliveryManager::StartRecv:IR\n");
      SetFileListFilter( NULL );
      DeliveryIRSystem* pSystem = GFL_NEW_LOW( m_pHeap ) DeliveryIRSystem( m_pHeap , &m_InitParam , &m_InitParameter , &m_InitParameterFixed );
      if( !pSystem->Initialize( threadPriority ) )
      {
        GFL_SAFE_DELETE( pSystem );
      }

      m_pDeliverySystem = pSystem;
    }
  break;
  }



  if( !m_pDeliverySystem )
  {
    GFL_PRINT("DeliveryManager::StartRecv:error m_pDeliverySystem is NULL\n");
    Finalize();
    return false;
  }

  if( m_pRecvThread )
  {
    m_pRecvThread->Start();
  }

  return true;
}

//------------------------------------------------------------------
/**
  * @brief   シリアルコード設定
  */
//------------------------------------------------------------------
void DeliveryManager::SetSerialCode( const u32 serialCode )
{
  m_SerialCode = serialCode;
}

//------------------------------------------------------------------
/**
 * @brief  終了準備
 */
//------------------------------------------------------------------
void DeliveryManager::PrepareFinalize( void )
{
  CancelAsyncInternal();
}

//------------------------------------------------------------------
/**
 * @brief  停止確認
 */
//------------------------------------------------------------------
bool DeliveryManager::IsCompretePrepareFinalize( void )
{
  return !CheckAsyncInternal();
}


//------------------------------------------------------------------
/**
 * @brief   送信リスナー設定
 */
//------------------------------------------------------------------
void DeliveryManager::SetSendListener( DeliverySendListener* pSendListener )
{
  m_pSendListener = pSendListener;
}

//------------------------------------------------------------------
/**
 * @brief   送信リスナー解除
 */
//------------------------------------------------------------------
void DeliveryManager::RemoveSendListener()
{
  m_pSendListener = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   受信リスナー設定
 */
//------------------------------------------------------------------
void DeliveryManager::SetRecvListener( DeliveryRecvListener* pRecvListener )
{
  m_pRecvListener = pRecvListener;
}

//------------------------------------------------------------------
/**
 * @brief   受信リスナー解除
 */
//------------------------------------------------------------------
void DeliveryManager::RemoveRecvListener()
{
  if( m_pDeliverySystem )
  {
    m_pDeliverySystem->RemoveListener();
  }
  m_pRecvListener = NULL;
}


//------------------------------------------------------------------
/**
 * @brief  初期化状態を調査
 * @result true： 初期済み
 *         false: 未初期化・または初期化中
 */
//------------------------------------------------------------------
bool DeliveryManager::IsInitialized( void ) const
{
  if( !m_isInit )
  {
    return false;
  }
  return m_isInit;
}

//------------------------------------------------------------------
/**
 * @brief  初期化パラメータの取得
 */
//------------------------------------------------------------------
gflnet2::InitParam* DeliveryManager::GetInitParam( void )
{
  return &m_InitParam;
}



//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信が実行中かどうかを調べる
 * @retval true   実行中
 * @retval false  実行していない
 */
//-------------------------------------------------------------------------------
bool DeliveryManager::CheckAsyncInternal( void ) const
{
  switch( m_deliveryType )
  {
  case DELIVERY_TYPE_wireless:		//無線
    {
      if( m_pRecvThread )
      {
        if( m_pRecvThread->IsAlive() )
        {
          return true;
        }
      }
      if( m_pSendThread )
      {
        if( m_pSendThread->IsAlive() )
        {
          return true;
        }
      }
      return false;
    }
  //break;
  case DELIVERY_TYPE_wifi:			//Wifi
    {
      if( m_pDeliverySystem )
      {
        return !m_pDeliverySystem->IsCompretePrepareFinalize();
      }
      else
      {
        return false;
      }
    }
  //break;
  case DELIVERY_TYPE_ir:				//赤外線
    {
      if( m_pDeliverySystem )
      {
        return !m_pDeliverySystem->IsCompretePrepareFinalize();
      }
      else
      {
        return false;
      }
    }
  //break;
  }

  return false;
}

//-------------------------------------------------------------------------------
/**
 * @brief 実行中の非同期通信をキャンセルする
 *
 */
//-------------------------------------------------------------------------------
void DeliveryManager::CancelAsyncInternal( void )
{
  if( CheckAsyncInternal() )
  {
    switch( m_deliveryType )
    {
    case DELIVERY_TYPE_wireless:		//無線
      {
        if( m_pRecvThread )
        {
          m_pRecvThread->Kill();
        }
        if( m_pSendThread )
        {
          m_pSendThread->Kill();
        }
      }
    break;
    case DELIVERY_TYPE_wifi:			//Wifi
      {
        if( m_pDeliverySystem )
        {
          m_pDeliverySystem->PrepareFinalize();
        }
      }
    break;
    case DELIVERY_TYPE_ir:				//赤外線
      {
        if( m_pDeliverySystem )
        {
          m_pDeliverySystem->PrepareFinalize();
        }
      }
    break;
    }
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief スレッドの終了待ちを行います。
 */
//-------------------------------------------------------------------------------
void DeliveryManager::WaitAsyncFinishedInternal( void )
{
  while( CheckAsyncInternal() )
  {
    if( m_pDeliverySystem )
    {
      m_pDeliverySystem->PrepareFinalize();
      m_pDeliverySystem->Update(NULL);
    }
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief 非同期処理スレッドを破棄する
 */
//-------------------------------------------------------------------------------
void DeliveryManager::DeleteAsyncInternal( void )
{
  CancelAsyncInternal();
  WaitAsyncFinishedInternal();

  GFL_SAFE_DELETE( m_pRecvThread );
  GFL_SAFE_DELETE( m_pSendThread );
}


//niji追加分

//------------------------------------------------------------------
/**
* @brief 受け取るファイルを決定する
*/
//------------------------------------------------------------------
s32 DeliveryManager::CheckFileList( const nn::boss::NsaList::NsaInformation* pFileList , const u32 fileNum )
{ 
  GFL_PRINT( "WonderGiftRecv::CheckFileList:fileNum[%d]\n" ,fileNum );

  if( fileNum >= MAX_FILE_NUM || fileNum == 0 )
  {//ファイル数オーバーorファイル無し
    return -1;
  }

  return 0;

  /*
  std::memset( m_downloadFileList , 0 , sizeof(m_downloadFileList) );
  s32 fileIndex = -1;
  s32 enableFileNum = 0;
  if( m_networkType != NETWORK_TYPE_WifiSerial )
  {//Romバージョンでフィルタリング

    const u32 VERSION_LIST[] =
    {
      VERSION_X,
      VERSION_Y,
      VERSION_CTR_SAPPHIRE,
      VERSION_CTR_RUBY,
    };

    int versionIndex = 1;
    for( u32 index = 0 ; index < GFL_NELEMS(VERSION_LIST) ; ++index )
    {
      if( VERSION_LIST[index] == PM_VERSION )
      {
        versionIndex = index+1;
        break;
      }
    }

    u8 romVersion = versionIndex;
    u32 bit = 1 << ( romVersion - 1 );

    for( u32 i = 0 ; i < fileNum ; ++i )
    {
      char attribute2[netapp::util::BossUtility::ATTRIBUTE_BUFFER_SIZE];
      if( sizeof(attribute2) == sizeof(pFileList[i].attribute2) )
      {
        std::memcpy( attribute2 , pFileList[i].attribute2 , sizeof(attribute2) );
      }
      else
      {
        GFL_ASSERT( 0 );//@fix
      }

      char *stopString;
      u32 romMask = std::strtol( attribute2, &stopString, ATTRIBUTE2_NUM_BASE );
      GFL_PRINT("WonderGiftRecv::CheckFileList : file[%d]attribute[%s]mask[%u]\n",i,attribute2,romMask);

      u32 flag = bit & romMask;
      if( flag != 0 )
      {//ダウンロード可能
        GFL_PRINT("WonderGiftRecv::CheckFileList : file found[%d]\n",i);
        m_downloadFileList[enableFileNum] = i;
        enableFileNum++;
      }
    }
  }
  else
  {
    enableFileNum = fileNum;
    for( u32 index = 0 ; index < fileNum ; ++index )
    {
      m_downloadFileList[index] = index;
    }
  }

  if( enableFileNum == 1 )
  {//１つだけ
    return 0;
  }

  //ランダム配信
  for( u32 i = 0 ; i < 65535 ; ++i )
  {
    s32 rand = xy_system::GflUse::GetPokemonCreateRand( enableFileNum );
    for( u32 index = 0 ; index < enableFileNum ; ++index )
    {
      GFL_PRINT("WonderGiftRecv::CheckFileList : random param [%d][%d]\n",rand, m_downloadFileList[index] );
      if( m_downloadFileList[index] == rand )
      {//ランダムに選んだインデックスがヒット
        GFL_PRINT("WonderGiftRecv::CheckFileList : random index decide[%d]\n",rand);
        fileIndex = rand;
        break;
      }
    }

    if( fileIndex >= 0 )
    {
      break;
    }
  }

  return fileIndex;
  */
}

//------------------------------------------------------------------
/**
* @brief シリアルIDの範囲取得（最大）
*/
//------------------------------------------------------------------
u32 DeliveryManager::GetSerialIdMax( void )
{ 
  NetAppLib::Util::BossUtility::BOSS_APP_TYPE type;
  if( m_SerialCode )
  {//シリアル
    type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_giftSerial;
  }
  else
  {//シリアル以外
    type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_gift;
  }

  return NetAppLib::Util::BossUtility::GetSerialIdMax( type );
}

//------------------------------------------------------------------
/**
* @brief シリアルIDの範囲取得（最少）
*/
//------------------------------------------------------------------
u32 DeliveryManager::GetSerialIdMin( void )
{ 
  NetAppLib::Util::BossUtility::BOSS_APP_TYPE type;
  if( m_SerialCode )
  {//シリアル
    type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_giftSerial;
  }
  else
  {//シリアル以外
    type = NetAppLib::Util::BossUtility::BOSS_APP_TYPE_gift;
  }

  return NetAppLib::Util::BossUtility::GetSerialIdMin( type );
}

GFL_NAMESPACE_END(Delivery)
GFL_NAMESPACE_END(NetLib)
#endif

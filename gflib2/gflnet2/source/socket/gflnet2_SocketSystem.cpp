#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_SocketSystem.h
 *
 * @brief  nn::socketのラッパークラス。Socketライブラリを管理する
 *         NEXライブラリを使用する場合は、事前に本SocketSystemを初期化すること。
 *         なお、事前にfs, acライブラリを初期化しておく必要がある。
 *
 * @author h.suzuki
 */
//=============================================================================
#include <gflnet2/include/socket/gflnet2_SocketSystem.h>
#include <nn/socket/socket_Result.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(socket)


//------------------------------------------------------------------
/**
 * メンバ変数定義
 */
//------------------------------------------------------------------
void*  SocketSystem::m_socketBuffer   = NULL;  // Socketが内部で使用するメモリ
bool   SocketSystem::m_isInit         = false; // Socket初期化フラグ


//------------------------------------------------------------------
/**
 * @brief  コンストラクタ＆デストラクタ(隠蔽化)
 */
//------------------------------------------------------------------
SocketSystem::SocketSystem(){}
SocketSystem::~SocketSystem(){}


//------------------------------------------------------------------
/**
 * @brief  SocketSystemの初期化
 *         事前にfs, acを初期化する必要があります！
 *
 * @param  heap ソケット用に割り当てるヒープメモリエリア
 *              ※デバイスメモリは指定不可能！
 */
//------------------------------------------------------------------
nn::Result SocketSystem::Initialize( gfl2::heap::HeapBase* heap )
{
  nn::Result result = nn::ResultSuccess();

  if( !m_isInit )
  {
    u32 alocatedSize;    // 実際に確保されたメモリサイズ
    u32 needBufferSize = nn::socket::GetRequiredMemorySize(SOCKET_BUFFER_SIZE, MAX_SESSION);

    // NEX用に必要メモリサイズに予備分のサイズを加算
    needBufferSize += SUB_MEMORY_SIZE;

    if( !AllocateWorkBuffer( heap, needBufferSize, &alocatedSize )  ){
      return nn::socket::ResultOutOfMemory();
    }
    result = nn::socket::Initialize( NN_ANY_TO_UPTR(m_socketBuffer), alocatedSize, SOCKET_BUFFER_SIZE, MAX_SESSION );

    GFL_PRINT( "nn::socket::Initialize(%d) [%u byte need][%u byte allocated]\n", result.IsSuccess(), needBufferSize, alocatedSize );

    if( result.IsSuccess() ){
      m_isInit = true;
    }
  }
  return result;
}


//------------------------------------------------------------------
/**
 * @brief 内部ワークメモリの確保
 *
 * @param  heap ソケット用に割り当てるヒープメモリエリア
 *              ※デバイスメモリは指定不可能！
 *
 * @param  needBufferSize Socketが最低限必要とするヒープサイズ
 * @param  alocateSize    確保されたワークエリアサイズ
 *
 * @return 確保に成功または既に確保済みであればtrue、
 *         確保に失敗した場合はfalseを返却
 */
//------------------------------------------------------------------
bool SocketSystem::AllocateWorkBuffer( gfl2::heap::HeapBase* heap, u32 needBufferSize, u32* alocateSize )
{
  if( m_socketBuffer == NULL )
  {
    *alocateSize = (TOTAL_BUFFER_SIZE >= needBufferSize) ? TOTAL_BUFFER_SIZE : needBufferSize;

    m_socketBuffer  = GflHeapAllocMemoryAlign4096Lower( heap, (*alocateSize) );
  }
  return m_socketBuffer != NULL;
}


//------------------------------------------------------------------
/**
 * @brief ワークメモリの解放
 */
//------------------------------------------------------------------
void SocketSystem::FreeWorkBuffer( void )
{
  if( m_socketBuffer != NULL )
  {
    GflHeapFreeMemory( m_socketBuffer );
    m_socketBuffer = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief  SocketSystemの終了処理
 */
//------------------------------------------------------------------
nn::Result SocketSystem::Finalize( void )
{
  nn::Result result = nn::ResultSuccess();

  if( m_isInit )
  {
    result = nn::socket::Finalize();

    FreeWorkBuffer();
    m_isInit = false;
  }
  return result;
}


//------------------------------------------------------------------
/**
 * @brief  内部情報のダンプ（デバッグ用）
 */
//------------------------------------------------------------------
void SocketSystem::DumpInfo( void )
{
  s32 ret;
  nn::socket::InAddr addr, netmask;
  ret = nn::socket::GetPrimaryAddress(reinterpret_cast<u8*>(&addr), reinterpret_cast<u8*>(&netmask));

  GFL_PRINT("[SocketSystem::DumpInfo]----------------------\n" );
  if( ret >= 0 )
  {
    GFL_PRINT("host    : %s\n", GetAddressString(addr));
    GFL_PRINT("netmask : %s\n", GetAddressString(netmask));
  }
  else{
    GFL_PRINT("nn::socket::GetPrimaryAddress() : failed\n" );
  }

  nn::socket::InAddr dns1, dns2;
  ret = nn::socket::GetResolver(reinterpret_cast<u8*>(&dns1), reinterpret_cast<u8*>(&dns2));

  if( ret >= 0 )
  {
    GFL_PRINT("dns1    : %s\n", GetAddressString(dns1));
    GFL_PRINT("dns2    : %s\n", GetAddressString(dns2));
  }
  else{
    GFL_PRINT("nn::socket::GetResolver() : failed\n" );
  }

  nn::socket::InAddr gateway;
  ret = nn::socket::GetDefaultGateway(reinterpret_cast<u8*>(&gateway));

  if( ret >= 0 )
  {
    GFL_PRINT("gateway : %s\n", GetAddressString(gateway));
  }
  else{
    GFL_PRINT("nn::socket::GetDefaultGateway() : failed\n" );
  }
  GFL_PRINT("-------------------------------------------------\n" );
}


//------------------------------------------------------------------
/**
 * @brief  エラー内容をコンソールに出力
 */
//------------------------------------------------------------------
void SocketSystem::PrintError( nn::Result &result )
{
  if( result == nn::socket::ResultFailedToInitializeInterface() )
  {
    GFL_PRINT("nn::socket failed  / ResultFailedToInitializeInterface! \n"  );
  }
  else if( result == nn::socket::ResultFailedToInitializeSocketCore() )
  {
    GFL_PRINT("nn::socket failed  / ResultFailedToInitializeSocketCore! \n"  );
  }
  else if( result == nn::socket::ResultTooManyRequests() )
  {
    GFL_PRINT("nn::socket failed  / ResultTooManyRequests! \n"  );
  }
  else if( result == nn::socket::ResultAlreadyInitialized() )
  {
    GFL_PRINT("nn::socket failed  / ResultAlreadyInitialized! \n"  );
  }
  else if( result == nn::socket::ResultNotInitialized() )
  {
    GFL_PRINT("nn::socket failed  / ResultNotInitialized! \n"  );
  }
  else if( result == nn::socket::ResultOutOfMemory() )
  {
    GFL_PRINT("nn::socket failed  / ResultOutOfMemory! \n"  );
  }
  else if( result == nn::socket::ResultPermissionDenied() )
  {
    GFL_PRINT("nn::socket failed  / ResultPermissionDenied! \n"  );
  }
  else if( result == nn::socket::ResultUnkownConfiguration() )
  {
    GFL_PRINT("nn::socket failed  / ResultUnkownConfiguration! \n"  );
  }
  else if( result == nn::socket::ResultInvalidSize() )
  {
    GFL_PRINT("nn::socket failed  / ResultInvalidSize! \n"  );
  }
  else if( result == nn::socket::ResultUnkownClient() )
  {
    GFL_PRINT("nn::socket failed  / ResultUnkownClient! \n"  );
  }
  else if( result == nn::socket::ResultBadDescriptor() )
  {
    GFL_PRINT("nn::socket failed  / ResultBadDescriptor! \n"  );
  }
  else if( result == nn::socket::ResultNetworkResetted() )
  {
    GFL_PRINT("nn::socket failed  / ResultNetworkResetted! \n"  );
  }
  else if( result == nn::socket::ResultRequestSessionFull() )
  {
    GFL_PRINT("nn::socket failed  / ResultRequestSessionFull! \n"  );
  }
  else if( result == nn::socket::ResultToBeDefined() )
  {
    GFL_PRINT("nn::socket failed  / ResultToBeDefined! \n"  );
  }
  else if( result == nn::socket::ResultTooManyProcesses() )
  {
    GFL_PRINT("nn::socket failed  / ResultTooManyProcesses! \n"  );
  }
  else if( result == nn::socket::ResultAlreadyRegistered() )
  {
    GFL_PRINT("nn::socket failed  / ResultAlreadyRegistered! \n"  );
  }
  else if( result == nn::socket::ResultTooManySockets() )
  {
    GFL_PRINT("nn::socket failed  / ResultTooManySockets! \n"  );
  }
  else if( result == nn::socket::ResultMismatchVersion() )
  {
    GFL_PRINT("nn::socket failed  / ResultMismatchVersion! \n"  );
  }
  else if( result == nn::socket::ResultAddressCollision() )
  {
    GFL_PRINT("nn::socket failed  / ResultAddressCollision! \n"  );
  }
  else if( result == nn::socket::ResultTimeOut() )
  {
    GFL_PRINT("nn::socket failed  / ResultTimeOut! \n"  );
  }
  else if( result == nn::socket::ResultOutOfSystemResource() )
  {
    GFL_PRINT("nn::socket failed  / ResultOutOfSystemResource! \n"  );
  }
  else if( result == nn::socket::ResultInvalidCoreState() )
  {
    GFL_PRINT("nn::socket failed  / ResultInvalidCoreState! \n"  );
  }
  else if( result == nn::socket::ResultAborted() )
  {
    GFL_PRINT("nn::socket failed  / ResultAborted! \n"  );
  }
  else
  {
    GFL_PRINT("nn::socket failed  / unknown! \n"  );
  }
}

//------------------------------------------------------------------
/**
 * @brief  IPアドレスを文字列に変換
 *
 * @param  addr 文字列変換対象となるIPアドレス
 *
 * @return アドレス情報文字列
 */
//------------------------------------------------------------------
const char* SocketSystem::GetAddressString(nn::socket::InAddr addr)
{
  static char buffer[32];
  u8* p = reinterpret_cast<u8*>(&addr);
  std::snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
  return buffer;
}

//------------------------------------------------------------------
/**
 * @brief  DNSを取得
 *
 * @param  dns1 プライマリDNS
 * @param  dns2 セカンダリDNS
 *
 */
//------------------------------------------------------------------
void SocketSystem::GetDNS(nn::socket::InAddr* dns1 , nn::socket::InAddr* dns2 )
{
  s32 ret = nn::socket::GetResolver(reinterpret_cast<u8*>(dns1), reinterpret_cast<u8*>(dns2));

  if( ret < 0 )
  {
    GFL_PRINT("nn::socket::GetResolver() : failed[0x%x]\n",ret );
  }
}

GFL_NAMESPACE_END(socket)
GFL_NAMESPACE_END(gflnet2)

#endif

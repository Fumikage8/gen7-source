//==============================================================================
/**
 * @file   GiftWork.cpp
 * @date   2015/07/31 Fri. 15:50:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <print/include/PrintSystem.h>
#include  <str/include/gfl2_MsgData.h>

#include  <NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h>

#include  "./GiftWork.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE  = 0x00400000;
  static const u32  DEVHEAP_SIZE  = 0x00a00000 + 0x00400000;

  static const u32  MessageUtil_DataNum     =  2;   /**<  同時に扱うMessageDataの数  */
  static const u32  MessageUtil_WordNum     = 12;   /**<  wordsetの単語数            */
  static const u32  MessageUtil_WordBufLen  = 64;   /**<  wordsetの最大長            */

} /*  namespace  */



/*
*/
namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     Gift
  *  @brief    ctor
  */
//------------------------------------------------------------------
GiftWork::GiftWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pPokeModelUtil(NULL)
  , m_pAppParam(NULL)
  , m_pAppFrameManager(NULL)
  , m_pAuthHelper(NULL)
  , m_IsReceiveViaSerialAuth(false)
  , m_bCleanupMode(false)
  , m_bNoHandlingNetworkError(false)
  /*    */
  , m_MessageUtility(GetAppHeap()->GetDeviceHeap(), MessageUtil_DataNum, MessageUtil_WordNum, MessageUtil_WordBufLen)
  , m_pGiftViewContainer(NULL)
  /*  internal fatal  */
  , m_IsInternalFatalErrorOccured(false)
#if PM_DEBUG
  , m_pGiftDebug(NULL)
#endif
{
}


//------------------------------------------------------------------
/**
  *  @func     Gift
  *  @brief    ctor
  */
//------------------------------------------------------------------
GiftWork::~GiftWork()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @return   社内fatalか（SDKfatalとは違い、タイトルアプリに制御が戻り、専用のFatalEventで停止させる。）
  */
//------------------------------------------------------------------
bool  GiftWork::NetworkErrorHandling(void)
{
  if(m_bNoHandlingNetworkError)
  {
    return false;
  }



  // エラー処理対応
  // memo:このEventは全ての通信アプリ起動の親イベントになっている。
  //      対処すべきエラーは、
  //      ・P2P切断  → 強制P2P切断をしてP2Pを終わらせる必要がある
  //      ・WiFi切断 → 強制WiFi切断をしてWiFiを切断する必要がある
  //    
  //      P2P切断は、P2Pを接続するEventが各自呼んでいるはずなので、ここでは処理しない
  //      WiFi切断はここで処理する
  //
  NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

  if( pErrorManager->IsFatalError() )
  {
    /*  ここではフラグを立てるのみとし、GiftEventまで戻ってFatalEventへ遷移する  */
    m_IsInternalFatalErrorOccured = true;
    GFL_PRINT("%s: FatalErrorOccured\n", __FUNCTION__);
  }
  else if( pErrorManager->IsError() )
  {
    // 共通通信エラー対処
    pErrorManager->CommonNetworkErrorHandling();
    GFL_PRINT("%s: CommonNetworkErrorOccured\n", __FUNCTION__);
  }
  else
  {
    /*  nop  */
    GFL_PRINT("%s: no error\n", __FUNCTION__);
  }

  // 通信エラークリア
  pErrorManager->Clear();

  return IsInternalNetworkFatalErrorOccured();
}


} /*  namespace Gift    */
} /*  namespace NetApp  */

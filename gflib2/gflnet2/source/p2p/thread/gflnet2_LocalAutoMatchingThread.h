#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_LOCAL_AUTO_MATCHING_THREAD_H__
#define __GFLNET_LOCAL_AUTO_MATCHING_THREAD_H__

#pragma once
//=============================================================================================
/**
 * @file	 gflnet2_LocalAutoMatchingThread.h
 * @brief	 ローカルオートマッチングスレッド
 */
//=============================================================================================
#include <pia/local.h>

#include <gflnet2/include/base/gflnet2_NetThread.h>
#include <gflnet2/include/base/gflnet2_Yield.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

class P2pManager;

class LocalAutoMatchingThread : public gflnet2::base::NetThread 
{
  GFL_FORBID_COPY_AND_ASSIGN( LocalAutoMatchingThread ); // コピーコンストラクタ, 代入演算子を禁止

public:
  enum
  {
    ID_AUTOMATCH = 1,
    ID_START_GAME = 2
  };

private:
  enum
  {
    MATCH_CYCLE_MIN = 1,
    MATCH_CYCLE_MAX = 2,
    MATCH_CYCLE_BASE_TIME = 1500,//ms
    PARENT_BASE_TIME = 2000,//ms
    SLEEP_RAND = 4,
    RAND_TIME = 500,//ms
    PARENT_MATCH_TIMEOUT = 10000,//ms
    FIRST_TIME_SEARCH_WAIT = 500,//ms
    AUTO_MATCH_CHANNEL = 11,
  };

  enum
  {
    FIRST_TIME_PATTERN_search = 0,
    FIRST_TIME_PATTERN_create,
    FIRST_TIME_PATTERN_searchAfterWait,
    FIRST_TIME_PATTERN_MAX,
  };


  P2pManager* mpP2pManager;
  u8 m_maxEntry;
  bool m_isConnectSuccess;
  bool m_isMaster;
  u8 m_matchType;
  

  //u8 mTempBuff[nn::uds::NET_DESC_APPDATA_SIZE_MAX];  //一時的に読み込むバッファ

  
public:


  //------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //------------------------------------------------------------------------------------
  LocalAutoMatchingThread( gfl2::heap::HeapBase* pHeap , u8 maxEntry );

  //------------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //------------------------------------------------------------------------------------
  virtual ~LocalAutoMatchingThread();

public:

  //------------------------------------------------------------------------------------
  /**
   * @brief 接続したか
   */
  //------------------------------------------------------------------------------------
  bool GetIsConnectSuccess( void ) const { return m_isConnectSuccess; };

  //------------------------------------------------------------------------------------
  /**
   * @brief 親か
   */
  //------------------------------------------------------------------------------------
  bool IsMaster( void ) const { return m_isMaster; };

private:

  //----------------------------------------------------------------------------
  /**
   * @brief メイン処理
   * ※スレッド関数のループ内で呼ばれ続ける関数です。
   */
  //----------------------------------------------------------------------------
  virtual void Function( void );

  //------------------------------------------------------------------
  /**
   * @brief	  ギャザリング参加
   */
  //------------------------------------------------------------------
  bool ParticipateGatheringInternal();

  //------------------------------------------------------------------
  /**
   * @brief	  ギャザリング検索
   */
  //------------------------------------------------------------------
  bool UpdateGatheringListInternal( u32 findCount );

  //---------------------------------------------------------------
  /**
   * @brief	  ギャザリング参加
   */
  //------------------------------------------------------------------
  bool Participate(nn::pia::local::UdsGatheringHolder* pGathering);
public:
  void SetP2pManager(P2pManager* pP2pManager);

};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_LOCAL_AUTO_MATCHING_THREAD_H__
#endif

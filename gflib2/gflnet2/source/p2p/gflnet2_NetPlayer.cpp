#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file	 gfl_NetPlayer.cpp
 * @brief	 通信プレーヤー情報を管理するクラス
 * @author k.ohno
 * @date	 2011.9.27
 */
//=============================================================================

#include <gflnet2/include/p2p/gflnet2_NetPlayer.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)


//コンストラクタ
NetPlayer::NetPlayer(void)
: mTransferedID(0)
{
}

//デストラクタ
NetPlayer::~NetPlayer()
{
}


//------------------------------------------------------------------
/**
 * @brief	  transferedID記録
 * @param   transferedID
 */
//------------------------------------------------------------------
void NetPlayer::SetTransferedID(const bit64 transferedID)
{
  mTransferedID = transferedID;
}

//------------------------------------------------------------------
/**
 * @brief	  transferedIDを得る
 * @return  transferedID
 */
//------------------------------------------------------------------
bit64 NetPlayer::GetTransferedID(void)
{
  return mTransferedID;
}


GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif

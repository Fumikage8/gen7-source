#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_P2PSTATICMANAGER_H__
#define __GFLNET_P2PSTATICMANAGER_H__
//=============================================================================
/**
 * @file	 gflnet_P2pStaticManager.h
 * @brief	 p2p通信 ポインタ管理
 * @author k.ohno
 * @date	 2011.9.21
 */
//=============================================================================
#pragma once

#include <nex.h>
#include <nn.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN( p2p )



class StaticManager{
  GFL_FORBID_COPY_AND_ASSIGN(StaticManager); //コピーコンストラクタ＋代入禁止
private:
  static nn::nex::NetZ* mpNetZ;
  static bool s_isRegisterd;
  
public:
  //---------------------------------------------------------------------------------
  /**
   * @brief    nn::nex::NetZを作成 保存しておく
   * @return   NetZポインタ
   */
  //---------------------------------------------------------------------------------
  static nn::nex::NetZ* CreateNetZ(void);
  //---------------------------------------------------------------------------------
  /**
   * @brief  nn::nex::NetZを消す
   */
  //---------------------------------------------------------------------------------
  static void DestroyNetZ(void);


  static inline nn::nex::NetZ* GetNetZ(void){ return mpNetZ; }
  
  static inline void SetRegisterdMigration(void){ s_isRegisterd = true; }

  static bool InitializeSetting(void);
  static bool DispatchJob(void);
  
};



GFL_NAMESPACE_END( p2p )
GFL_NAMESPACE_END( gflnet2 )


#endif// __GFLNET_P2PSTATICMANAGER_H__

#endif

#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_GLOBALACCESS_H__
#define __GFLNET_GLOBALACCESS_H__

//=============================================================================
/**
 * @file	 gflnet_GlobalAccess.h
 * @brief	 通信にてどうしても静的管理の方で解決しないと難しい物の為のクラス
           影響範囲を考えながら実装する事
           名前のセンスが悪いのは使わないようにして欲しいため
 * @author k.ohno
 * @date	 2012.1.27
 */
//=============================================================================
#pragma once

#include <base/include/gfl2_Singleton.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)


class GlobalAccess : public gfl2::base::Singleton<GlobalAccess>{
  GFL_FORBID_COPY_AND_ASSIGN(GlobalAccess);

private:
  u32 mPauseBIT;   // ビーコンサイクル通信の一時停止
#if GFL_DEBUG
  bool mbConnectWifi;
#endif
public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ＆デストラクタ
   */
  //------------------------------------------------------------------
  GlobalAccess(void);
  virtual ~GlobalAccess();

public:

  //------------------------------------------------------------------
  /**
   * @brief	  pause状態を返す
   * @return  pauseしている時はtrue
   */
  //------------------------------------------------------------------
  inline bool IsPause(void){ return (mPauseBIT!=0); };
  //------------------------------------------------------------------
  /**
   * @brief	  BIT制御でpause状態にする
   * @param   BIT
   */
  //------------------------------------------------------------------
  inline void SetPauseBit(u32 pauseBit){ mPauseBIT = (mPauseBIT | pauseBit); };

  //------------------------------------------------------------------
  /**
   * @brief	  BIT制御でresetpause状態にする
   * @param   BIT
   */
  //------------------------------------------------------------------
  inline void ResetPauseBit(u32 pauseBit){ mPauseBIT = (mPauseBIT & (~pauseBit)); };
  

#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief	  wifi状態を返す
   */
  //------------------------------------------------------------------
  inline bool IsConnectWifi(void){ return mbConnectWifi; };
  //------------------------------------------------------------------
  /**
   * @brief	  wifi状態を返す
   */
  //------------------------------------------------------------------
  inline void SetConnectWifi(bool bConnectWifi){
    //OHNO_PRINT("SetConnectWifi \n");
    mbConnectWifi = bConnectWifi;
  };
#endif //GFL_DEBUG

  
};

GFL_NAMESPACE_END(gflnet2)


#endif// __GFLNET_GLOBALACCESS_H__
#endif

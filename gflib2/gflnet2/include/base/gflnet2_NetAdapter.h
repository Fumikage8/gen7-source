#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_ADAPTER_H__
#define __GFLNET_ADAPTER_H__
//=============================================================================
/**
 * @file	 gflnet_NetAdapter.h
 * @brief	 各通信のアダプター
 * @author k.ohno
 * @date	 2012.4.15
 */
//=============================================================================
#pragma once


GFL_NAMESPACE_BEGIN( gflnet2 )


class NetAdapter {
  GFL_FORBID_COPY_AND_ASSIGN(NetAdapter);

protected:
  InitParam mInitParam;  ///< 通信初期化パラメータ

public:
  NetAdapter(const gflnet2::InitParameter* pIParam, const gflnet2::InitParameterFixed* pIParamFix)
    {      mInitParam.Initialize(pIParam,pIParamFix);  };
  virtual ~NetAdapter(){};

  virtual bool Initialize(gfl2::heap::HeapBase* pHeapSys) = 0;   ///初期化

  virtual bool Connect(void) = 0;   ///< APにつながる方

  virtual bool Scan(void) = 0;

  virtual bool SetBeacon(const u8* pData, const u32 size) = 0;

  virtual void* GetBeacon(const int index, const u32 size) = 0;
  
  virtual bool CreateMaster(void) = 0;

  virtual bool Matching(int TransferedID) = 0;

  virtual bool Update(void) = 0;

  virtual bool SendData(void* pData,const u32 size) = 0;

//  virtual void* RecvData(int id) = 0;

  virtual void* RecvDataFunc(int id, size_t* pSize) = 0;

  virtual bool Disconnect(void) = 0;  ///<切断

  virtual bool Finalize(void) = 0;  ///< 解放＋終了処理

  virtual bool IsMaster(void) = 0;

  virtual bool IsMatched(void) = 0;  ///< お互いが接続済み

  virtual bool IsConnected(void) = 0;  ///< APとかに接続
  
};




GFL_NAMESPACE_END( gflnet2 )

#endif// __GFLNET_ADAPTER_H__
#endif

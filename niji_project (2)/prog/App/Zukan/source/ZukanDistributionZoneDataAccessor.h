#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONZONEDATAACCESSOR_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONZONEDATAACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDistributionZoneDataAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.17
 @brief   図鑑の分布図のゾーンリストのアクセッサー
 @note    zukan_distribution_zone_data_bin_maker.rbが生成したzukan_distribution_zone_data.binの中身にアクセスする
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanDistributionZoneDataAccessor
 @brief     図鑑の分布図のゾーンリストのアクセッサー
 */
//==============================================================================
class ZukanDistributionZoneDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDistributionZoneDataAccessor);

public:
  static const u32 ZONE_DATA_NUM_MAX = 640;
  
  // 座標は何倍された値か
  static const f32 POS_SCALL_VAL;  // = 100.0f;

  //! @brief  ゾーンデータ
  class ZoneData
  {
  public:
    u32  islandNo;
    s32  intX;  // 本来の値をPOS_SCALL_VAL倍して、小数点以下は切り捨てて整数にしてある
    s32  intZ;  // ので、使用するときはPOS_SCALL_VALで割ってから使って下さい。
    ZoneData(void)
      : islandNo(0),
        intX(0),
        intZ(0)
    {}
  };

public:
  static void ZoneDataBitFlagSetOn(u32 zoneDataBitFlag[], u32 zoneDataNo);  // APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATIONで宣言したものを渡すこと。
  static bool ZoneDataBitFlagIsOn(const u32 zoneDataBitFlag[], u32 zoneDataNo);  // APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATIONで宣言したものを渡すこと。

public:
  ZukanDistributionZoneDataAccessor(void)
    : m_buf(NULL),
      m_zoneDataNum(0),
      m_zoneDataBinArray(NULL)
  {}
  virtual ~ZukanDistributionZoneDataAccessor()
  {}

  //! @brief  アクセッサーが必要とするデータを設定する
  //! @param[in]  buf  zukan_distribution_zone_data.binを読み込んだバッファ
  void SetupData(const void* buf);

  //! @brief  ゾーンデータを取得する
  //! @param[in]  zoneDataNo  ゾーンデータ番号(0開始)
  //! @param[out] zoneData    ここに渡された構造体に値を書き込んで返す
  void GetZoneData(u32 zoneDataNo, ZoneData* zoneData) const;

  //! @brief  ゾーンデータの個数を得る
  u32 GetZoneDataNum(void) const { return m_zoneDataNum; }

private:
  struct ZoneDataBin
  {
    u32 islandNo;
    s32 intX;
    s32 intZ;
  };

private:
  // 外部から受け取るポインタ
  const void* m_buf;

  // アクセスしやすくするため
  u32 m_zoneDataNum;                      // m_buf内にあるゾーンデータの個数  // 0<=zoneDataNo<m_zoneDataNum
  const ZoneDataBin* m_zoneDataBinArray;  // ゾーンデータ配列(要素数はm_zoneDataNum)
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#define APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATION(varName) u32 varName[ZukanDistributionZoneDataAccessor::ZONE_DATA_NUM_MAX/32] = {0}


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONZONEDATAACCESSOR_H_INCLUDED


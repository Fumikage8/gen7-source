#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOSITIONDATAACCESSOR_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOSITIONDATAACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDistributionPositionDataAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の分布図の配置IDのリストのアクセッサー
 @note    zukan_distribution_position_data_converter.rbが生成したzukan_distribution_position_data.binの中身にアクセスする
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
 @class     ZukanDistributionPositionDataAccessor
 @brief     図鑑の分布図の配置IDのリストのアクセッサー
 */
//==============================================================================
class ZukanDistributionPositionDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDistributionPositionDataAccessor);

public:
  // position_dataに出力されている配置IDの所属するzoneDataNoが不正値
  static const u32 POSITION_PLACE_ID_ZONE_DATA_NO_INVALID = 0x3FFF;

  // 座標は何倍された値か
  static const f32 POS_SCALL_VAL;  // = 100.0f;

  // エンカウントタイプ  // zukan_distribution_position_data_converter.rbと値を一致させておくこと
  enum EncountType
  {
    ENCOUNT_TYPE_KUSA      =  0,
    ENCOUNT_TYPE_SEA       =  1,
    ENCOUNT_TYPE_WATER     =  2,
    ENCOUNT_TYPE_DESERT    =  3,
    ENCOUNT_TYPE_FISHING   =  4,
    ENCOUNT_TYPE_FLOOR     =  5,
    ENCOUNT_TYPE_FLOWER    =  6,
    ENCOUNT_TYPE_KINOMI    =  7,
    ENCOUNT_TYPE_GECKUSA   =  8,
    ENCOUNT_TYPE_GECSAND   =  9,
    ENCOUNT_TYPE_GECTUCHI  = 10,
    ENCOUNT_TYPE_GECWATER  = 11,
    ENCOUNT_TYPE_GEEKOSOKU = 12,
    ENCOUNT_TYPE_GEPKAGE   = 13,
    ENCOUNT_TYPE_GEPKUSA   = 14,
    ENCOUNT_TYPE_GEPSAND   = 15,
    ENCOUNT_TYPE_GEPTUCHI  = 16,
    ENCOUNT_TYPE_GESPLANT  = 17,
    ENCOUNT_TYPE_GESWOOD   = 18,
    ENCOUNT_TYPE_GETEST    = 19,
    ENCOUNT_TYPE_NUM       = 20,
  };

  //! @brief  座標データ
  struct PosData
  {
    s32 intX;  // 本来の値をPOS_SCALL_VAL倍して、小数点以下は切り捨てて整数にしてある
    s32 intZ;  // ので、使用するときはPOS_SCALL_VALで割ってから使って下さい。
  };

  //! @brief  配置IDのデータ  // 配置IDのデータって言い方、何か変だけどいいや。
  class PlaceIdData
  {
  public:
    u32  islandNo;
    u32  zoneDataNo;  // POSITION_PLACE_ID_ZONE_DATA_NO_INVALIDのとき無効値
    u32  encountType;  // EncountTypeの値
    u32  posDataNum;         // 0個のとき無効
    const PosData* posData;  // posDataNumが0のときNULL
    PlaceIdData(void)
      : islandNo(0),
        zoneDataNo(POSITION_PLACE_ID_ZONE_DATA_NO_INVALID),
        encountType(ENCOUNT_TYPE_KUSA),  // @fix cov_ctr[11326]: 未初期化を初期化
        posDataNum(0),
        posData(NULL)
    {}
  };

public:
  ZukanDistributionPositionDataAccessor(void)
    : m_buf(NULL),
      m_placeIdDataNum(0),
      m_startByte(NULL)
  {}
  virtual ~ZukanDistributionPositionDataAccessor()
  {}

  //! @brief  アクセッサーが必要とするデータを設定する
  //! @param[in]  buf  zukan_distribution_position_data.binを読み込んだバッファ
  void SetupData(const void* buf);

  //! @brief  配置IDのデータを取得する
  //! @param[in]  placeId      配置ID(0開始)
  //! @param[out] placeIdData  ここに渡された構造体に値を書き込んで返す
  void GetPlaceIdData(u32 placeId, PlaceIdData* placeIdData) const;

  //! @brief  配置IDのデータの個数を得る
  u32 GetPlaceIdDataNum(void) const { return m_placeIdDataNum; }

private:
  struct PlaceIdDataHeader
  {
    u32 islandNo    : 3;
    u32 posDataNum  : 4;
    u32 zoneDataNo  :14;
    u32 encountType : 5;
    u32 dummy       : 6;
    s32 posDataTopIntX;
  };

private:
  // 外部から受け取るポインタ
  const void* m_buf;

  // アクセスしやすくするため
  u32 m_placeIdDataNum;                       // m_buf内にある配置IDのデータの個数  // 0<=placeId<m_placeIdDataNum
  const u32* m_startByte;                     // 開始バイト位置 m_startByte[placeId]
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOSITIONDATAACCESSOR_H_INCLUDED


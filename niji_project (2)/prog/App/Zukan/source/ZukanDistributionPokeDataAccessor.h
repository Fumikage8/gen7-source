#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOKEDATAACCESSOR_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOKEDATAACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDistributionPokeDataAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の分布図のポケモンごとの分布データのアクセッサー
 @note    zukan_distribution_encount_data_converter.rbが生成したzukan_distribution_poke_data.binの中身にアクセスする
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// pmlibのインクルード
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanDistributionPokeDataAccessor
 @brief     図鑑の分布図のポケモンごとの分布データのアクセッサー
 */
//==============================================================================
class ZukanDistributionPokeDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDistributionPokeDataAccessor);

public:
  //! 出現の仕方の種類  // zukan_distribution_encount_data_converter.rbと値を一致させておくこと
  enum Appearance
  {
    APPEARANCE_DAY_REGULAR      =  0,  // 昼通常
    APPEARANCE_DAY_ENTER        =  1,  // 昼乱入 
    APPEARANCE_DAY_RAIN_ENTER   =  2,  // 昼雨乱入
    APPEARANCE_DAY_SNOW_ENTER   =  3,  // 昼雪乱入
    APPEARANCE_DAY_SAND_ENTER   =  4,  // 昼砂嵐乱入
    APPEARANCE_NIGHT_REGULAR    =  5,  // 夜通常
    APPEARANCE_NIGHT_ENTER      =  6,  // 夜乱入
    APPEARANCE_NIGHT_RAIN_ENTER =  7,  // 夜雨乱入
    APPEARANCE_NIGHT_SNOW_ENTER =  8,  // 夜雪乱入
    APPEARANCE_NIGHT_SAND_ENTER =  9,  // 夜砂嵐乱入
    APPEARANCE_NUM              = 10,
  };

public:
  //! @brief  配置IDのデータ  // 配置IDのデータって言い方、何か変だけどいいや。
  struct PlaceIdData
  {
    u16 placeId;
    u16 appearanceBitFlag;  // 出現の仕方で該当する箇所のビットを立てておく(0ビット目APPEARANCE_DAY_REGULAR; ...; 9ビット目APPEARANCE_NIGHT_SAND_ENTER;)
  };
  //! @brief  ポケモンの分布データ
  class PokeDistributionData
  {
  public:
    u16 placeIdDataNum;  // placeIdDataの個数
    u16 bitFlag;  // 使っていない
    const PlaceIdData* placeIdData;  // placeIdDataNumが0のときNULL
    PokeDistributionData(void)
      : placeIdDataNum(0),
        bitFlag(0),
        placeIdData(NULL)
    {}
  };

public:
  ZukanDistributionPokeDataAccessor(void)
    : m_buf(NULL),
      m_monsNo_end(0),
      m_startByte(NULL)
  {}
  virtual ~ZukanDistributionPokeDataAccessor()
  {}

  //! @brief  アクセッサーが必要とするデータを設定する
  //! @param[in]  buf  zukan_distribution_poke_data.binを読み込んだバッファ
  void SetupData(const void* buf);

  //! @brief  ポケモンの分布データを取得する
  //! @param[out] pokeDistributionData  ここに渡された構造体に値を書き込んで返す
  void GetPokeDistributionData(MonsNo monsNo, PokeDistributionData* pokeDistributionData) const;

  //! @brief  昼に出現するか(夜に出現するかは判定していない)
  static bool IsAppearanceDay(const PlaceIdData* placeIdData);
  //! @brief  夜に出現するか(昼に出現するかは判定していない)
  static bool IsAppearanceNight(const PlaceIdData* placeIdData);
  // 乱入
  static bool IsAppearanceDayEnter(const PlaceIdData* placeIdData);
  static bool IsAppearanceNightEnter(const PlaceIdData* placeIdData);


private:
  struct PokeDistributionDataHeader
  {
    u16 placeIdDataNum;
    u16 bitFlag;
    u16 placeIdDataTopPlaceId;
    u16 padding;
  };

private:
  // 外部から受け取るポインタ
  const void* m_buf;

  // アクセスしやすくするため
  u32 m_monsNo_end;        // m_buf内の全国図鑑番号の最大値  // 1<=monsNo<=m_monsNo_end
  const u32* m_startByte;  // 開始バイト位置 m_startByte[monsNo]  // monsNoに0は入れちゃダメ 
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDISTRIBUTIONPOKEDATAACCESSOR_H_INCLUDED


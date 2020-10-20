//======================================================================
/**
* @file NamiSamplingData.h
* @date 2016/09/12 16:39:59
* @author fang_yicheng
* @brief 波のサンプリングデータ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __NAMI_SAMPLINGDATA_H_INCLUDED__
#define __NAMI_SAMPLINGDATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

class NamiSamplingData
{
  GFL_FORBID_COPY_AND_ASSIGN(NamiSamplingData);

public:
  // 定数
  enum
  {
    ROW_SIZE = 20,
    COL_SIZE = 40,
  };

  static u32 m_rowSize;
  static u32 m_colSize;

public:

  NamiSamplingData();
  virtual ~NamiSamplingData();

  static f32 GetData(u32 row1, u32 col1, u32 row2, u32 col2, f32 coefficient, f32 interpolation);
  static f32 GetDataMax(f32 coefficient);

  static f32 GetUpZoneThreshold();

  static gfl2::math::Vector3 CalcNormal(f32 x, f32 y, f32 coefficient);

private:

};

GFL_NAMESPACE_END(Nami)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __NAMI_SAMPLINGDATA_H_INCLUDED__
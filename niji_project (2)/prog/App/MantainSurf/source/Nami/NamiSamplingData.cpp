//======================================================================
/**
* @file NamiSamplingData.cpp
* @date 2016/09/12 16:40:17
* @author fang_yicheng
* @brief 波のサンプリングデータ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "NamiSamplingData.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

u32 NamiSamplingData::m_colSize = NamiSamplingData::COL_SIZE;
u32 NamiSamplingData::m_rowSize = NamiSamplingData::ROW_SIZE;

NamiSamplingData::NamiSamplingData()
{
}

NamiSamplingData::~NamiSamplingData()
{
}

// 前半(上り)=放物線
// 後半(下り)=正弦関数
f32 NamiSamplingData::GetData(u32 row1, u32 col1, u32 row2, u32 col2, f32 coefficient, f32 interpolation)
{
  if(row1 > m_rowSize || col1 > m_colSize || row2 > m_rowSize || col2 > m_colSize)
  {
    return 0.0f;
  }
  //coefficient = gfl2::math::Clamp(coefficient, 0.0f, 1.0f);
  interpolation = gfl2::math::Clamp(interpolation, 0.0f, 1.0f);

  f32 startVal, endVal;
  if(col1 <= NamiSamplingData::m_colSize / 2 && col2 <= NamiSamplingData::m_colSize / 2)
  {
    // 波の前半(放物線)に居る
    startVal = coefficient * col1 * col1;
    endVal = coefficient * col2 * col2;
  }
  else
  {
    // 波の後半(正弦関数)に居る
    f32 top = GetDataMax(coefficient);  // 頂点の高さ
    startVal = top * (gfl2::math::FSin(PI2 / m_colSize * col1 - PI / 2.0f) + 1.0f) / 2.0f;
    endVal = top * (gfl2::math::FSin(PI2 / m_colSize * col2 - PI / 2.0f) + 1.0f) / 2.0f;
  }
  return gfl2::math::Lerp(startVal, endVal, interpolation);
}

/*
f32 NamiSamplingData::GetData(u32 row1, u32 col1, u32 row2, u32 col2, f32 coefficient, f32 interpolation)
{
if(row1 >= m_rowSize || col1 >= m_colSize || row2 >= m_rowSize || col2 >= m_colSize)
{
return 0.0f;
}
coefficient = gfl2::math::Clamp(coefficient, 0.0f, 1.0f);
interpolation = gfl2::math::Clamp(interpolation, 0.0f, 1.0f);

f32 startVal = coefficient * col1 * col1;
f32 endVal   = coefficient * col2 * col2;
return gfl2::math::Lerp(startVal, endVal, interpolation);
}
*/

f32 NamiSamplingData::GetDataMax(f32 coefficient)
{
  return GetData(m_rowSize, m_colSize / 2, m_rowSize, m_colSize / 2, coefficient, 1.0f);
}

gfl2::math::Vector3 NamiSamplingData::CalcNormal(f32 x, f32 y, f32 coefficient)
{
  if(x == 0.0f || y == 0.0f || coefficient == 0.0f)
  {
    return gfl2::math::Vector3(0.0f, 1.0f, 0.0f);
  }

  gfl2::math::Vector3 normal(0.0f, 1.0f, 0.0f);
  f32 m; // 法線の傾き
  if(x <= m_colSize / 2.0f)
  {
    // 波の前半(放物線)に居る
    m = 2.0f * coefficient * x; // 接線の傾き
  }
  else
  {
    // 波の後半(正弦関数)に居る
    m = gfl2::math::FSin(PI2 / m_colSize * x); // 接線の傾き
    f32 coef = 1.0f; //GetDataMax(coefficient);
    m *= coef;
  }

  if(m != 0.0f)
  {
    m = -1.0f / m; // 法線の傾き
    gfl2::math::Vector3 p0(x - y / m, 0.0f, 0.0f);  // 法線がX軸との交点の座標
    normal = gfl2::math::Vector3(x, y, 0) - p0;
    normal.x = -normal.x;
    normal.NormalizeRet();
  }
  return normal;
}

f32 NamiSamplingData::GetUpZoneThreshold()
{
  return m_colSize / 2.0f;
}

GFL_NAMESPACE_END(Nami)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

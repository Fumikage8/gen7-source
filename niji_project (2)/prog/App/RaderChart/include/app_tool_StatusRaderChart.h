#if !defined(NIJI_PROJECT_APPLIB_APP_TOOL_STATUSRADERCHART_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_APP_TOOL_STATUSRADERCHART_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_tool_StatusRaderChart.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   ステータスのレーダーチャート
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

// nijiのインクルード
#include <App/RaderChart/include/app_tool_RaderChart.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class     StatusRaderChart
 @brief     ステータスのレーダーチャート
 */
//==============================================================================
class StatusRaderChart : public RaderChart
{
  GFL_FORBID_COPY_AND_ASSIGN(StatusRaderChart);

public:
  // レーダーチャートの透過率
  static const f32 RADAR_CHART_ALPHA;
  // レーダーチャートの色(能力値)
  static const gfl2::math::Vector4 RADAR_CHART_NORMAL_PARAM_COLOR;
  // レーダーチャート内側の色
  static const gfl2::math::Vector4 RADAR_CHART_INSIDE_COLOR;
  // レーダーチャート外側の色
  static const gfl2::math::Vector4 RADAR_CHART_OUTSIDE_COLOR;
  // レーダーチャート外側の色(努力値MAX)
  static const gfl2::math::Vector4 RADAR_CHART_OUTSIDE_COLOR_DORYOKU_MAX;

public:
  //! @brief  モード
  enum StatusMode
  {
    STATUS_MODE_PRO,  //!< 種族値と努力値を表示するモード
    STATUS_MODE_AMA,  //!< 能力値を表示するモード 
  };
  //! @brief  図
  enum StatusFigure
  {
    STATUS_FIGURE_PRO_INSIDE,   //!< 種族値
    STATUS_FIGURE_PRO_OUTSIDE,  //!< 努力値
    STATUS_FIGURE_AMA,          //!< 能力値
    STATUS_FIGURE_NUM,
  };

private:
  ////////////////////////
  // 種族値＆努力値
  ////////////////////////
  static const u32 VAL_INSIDE_MAX;      // 種族値(この値までは成り得る)
  static const f32 VAL_INSIDE_ZERO;      // 0のときどれだけ大きいところに描くかc
  static const u32 VAL_OUTSIDE_MAX;      // 努力値(この値までは成り得る)
  static const f32 VAL_OUTSIDE_TIMES;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)/2=VAL_INSIDE_MAXとなるような値(VAL_INSIDE_MAXのときに半分までグラフが来るような値)
  static const u32 VAL_OUTSIDE_ZERO;      // 0のときどれだけ大きいところに描くかb
  static const f32 VAL_TOTAL;  // 合計
      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
  static const f32 VAL_TOTAL_MAX;
      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。


  ////////////////////////
  // 能力値
  ////////////////////////
  static const f32 VAL_AMA_DISP_ZERO;  // 表示上の最小値。0のときどれだけ大きいところに描くか。
  static const u32 VAL_AMA_DISP_MAX;   // 表示上の最大値。この値以上なら振り切る。

 
public:
  StatusRaderChart(void)
    : RaderChart(),
      m_statusMode(STATUS_MODE_PRO)
  {
    for(u32 i=0; i<STATUS_FIGURE_NUM; ++i)
    {
      m_statusColor[i] = gfl2::math::Vector4(0,0,0,0);
      for(u32 j=0; j<ITEM_NUM; ++j)
      {
        m_statusVal[i][j] = 0;
      }
    }
  }
  virtual ~StatusRaderChart()
  {}

private:
  virtual void SetRadius(f32 insideRadius, f32 outsideRadius)
  {
    GFL_ASSERT(0);  // 使用禁止。SetStatusRadiusを使って下さい。開発中に気付かせるためのASSERT。
  }
public:
  virtual void SetColor(const gfl2::math::Vector4& insideColor, const gfl2::math::Vector4& outsideColor)
  {
    GFL_ASSERT(0);  // 使用禁止。SetStatusColorを使って下さい。開発中に気付かせるためのASSERT。
  }
private:
  virtual void SetVal(Figure fig, u32 item, f32 val)
  {
    GFL_ASSERT(0);  // 使用禁止。SetStatusValを使って下さい。開発中に気付かせるためのASSERT。
  }

public:
  //! @brief  モードを設定する
  void SetStatusMode(StatusMode mode);

  //! @brief  半径を設定する
  //! @param[in] outsideRadius  外側チャートの半径。レイアウト座標で指定する。
  void SetStatusRadius(f32 outsideRadius);
 
  //! @brief  色を設定する
  //! @param[in] proInsideColor   内側チャート(種族値)の色。(x=r,y=g,z=b,w=a)。0.0fから1.0fで指定する。
  //! @param[in] proOutsideColor  外側チャート(努力値)の色。(x=r,y=g,z=b,w=a)。0.0fから1.0fで指定する。
  //! @param[in] amaColor         能力値チャートの色。(x=r,y=g,z=b,w=a)。0.0fから1.0fで指定する。
  void SetStatusColor(const gfl2::math::Vector4& proInsideColor, const gfl2::math::Vector4& proOutsideColor, const gfl2::math::Vector4& amaColor);

  //! @brief  値を設定する
  //! @param[in] figure     内側チャート(種族値)か外側チャート(努力値)か、能力値チャートか。
  //! @param[in] item       項目番号。0<=val<ITEM_NUM。
  //! @param[in] statusVal  種族値か努力値か能力値をそのまま渡せばよい。
  void SetStatusVal(StatusFigure fig, u32 item, u32 statusVal);

private:
  void updateStatusVal(StatusFigure fig, u32 item, u32 statusVal);

protected:
  virtual f32 GetAdjustVal(const f32 val[FIGURE_NUM][ITEM_NUM], u32 fig, u32 item) const;

private:
  StatusMode m_statusMode;
  gfl2::math::Vector4 m_statusColor[STATUS_FIGURE_NUM];
  u32 m_statusVal[STATUS_FIGURE_NUM][ITEM_NUM];
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_APP_TOOL_STATUSRADERCHART_H_INCLUDED


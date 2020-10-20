#if !defined(NIJI_PROJECT_APPLIB_APP_TOOL_BOXRADERCHART_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_APP_TOOL_BOXRADERCHART_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_tool_BoxRaderChart.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   ボックスのレーダーチャート
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
 @class     BoxRaderChart
 @brief     ボックスのレーダーチャート
 */
//==============================================================================
class BoxRaderChart : public RaderChart
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxRaderChart);

private:
  static const u32 VAL_INSIDE_MAX;        // 種族値(この値までは成り得る)
  static const u32 VAL_INSIDE_ZERO;        // 0のときどれだけ大きいところに描くかc
  static const u32 VAL_OUTSIDE_MAX;        // 個体値(この値までは成り得る)
  static const f32 VAL_OUTSIDE_TIMES;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)/2=VAL_INSIDE_MAXとなるような値(VAL_INSIDE_MAXのときに半分までグラフが来るような値)
  static const u32 VAL_OUTSIDE_ZERO;        // 0のときどれだけ大きいところに描くかb
  static const u32 VAL_TOTAL;  // 合計
      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
  static const u32 VAL_TOTAL_MAX;
      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。

public:
  BoxRaderChart(void)
    : RaderChart()
  {
    for(u32 i=0; i<FIGURE_NUM; ++i)
    {
      for(u32 j=0; j<ITEM_NUM; ++j)
      {
        m_boxVal[i][j] = 0;
      }
    }
  }
  virtual ~BoxRaderChart()
  {}

private:
  virtual void SetRadius(f32 insideRadius, f32 outsideRadius)
  {
    GFL_ASSERT(0);  // 使用禁止。SetBoxRadiusを使って下さい。開発中に気付かせるためのASSERT。
  }
  virtual void SetVal(Figure fig, u32 item, f32 val)
  {
    GFL_ASSERT(0);  // 使用禁止。SetBoxValを使って下さい。開発中に気付かせるためのASSERT。
  }

public:
  //! @brief  半径を設定する
  //! @param[in] outsideRadius  外側チャートの半径。レイアウト座標で指定する。
  void SetBoxRadius(f32 outsideRadius);
 
  //! @brief  値を設定する
  //! @param[in] figure  内側チャート(種族値)か外側チャート(個体値)か。
  //! @param[in] item    項目番号。0<=val<ITEM_NUM。
  //! @param[in] boxVal  種族値か個体値をそのまま渡せばよい。
  void SetBoxVal(Figure fig, u32 item, u32 boxVal);

private:
  void updateBoxVal(Figure fig, u32 item, u32 boxVal);

private:
  u32 m_boxVal[FIGURE_NUM][ITEM_NUM];
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_APP_TOOL_BOXRADERCHART_H_INCLUDED


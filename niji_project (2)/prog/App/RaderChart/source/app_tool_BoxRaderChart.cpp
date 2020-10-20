//==============================================================================
/**
 @file    app_tool_BoxRaderChart.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   ボックスのレーダーチャート
 */
//==============================================================================


// nijiのインクルード
#include <App/RaderChart/include/app_tool_BoxRaderChart.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


const u32 BoxRaderChart::VAL_INSIDE_MAX    = 255;        // 種族値(この値までは成り得る)
//const u32 BoxRaderChart::VAL_INSIDE_ZERO   =  50;        // 0のときどれだけ大きいところに描くかc  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
//const u32 BoxRaderChart::VAL_OUTSIDE_MAX   =  31;        // 個体値(この値までは成り得る)
//const f32 BoxRaderChart::VAL_OUTSIDE_TIMES =  11.9355f;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)/2=VAL_INSIDE_MAXとなるような値(VAL_INSIDE_MAXのときに半分までグラフが来るような値)
//const u32 BoxRaderChart::VAL_OUTSIDE_ZERO  =  25;        // 0のときどれだけ大きいところに描くかb
//const u32 BoxRaderChart::VAL_TOTAL = VAL_INSIDE_MAX + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;  // 合計
      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
//const u32 BoxRaderChart::VAL_TOTAL_MAX = 140 + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;
      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。
      // 140+50+31*11.9355+25 = 585.0005;
      // ゲタの割合 50/585.0005 = 0.08547;

// ↓
// ↓ 個体値だけの表示でよくなったので下記のように変更↓
// ↓

//const u32 BoxRaderChart::VAL_OUTSIDE_MAX   =    31;      // 個体値(この値までは成り得る)
//const u32 BoxRaderChart::VAL_OUTSIDE_ZERO  =  1.83;      // 0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
//const u32 BoxRaderChart::VAL_TOTAL = VAL_OUTSIDE_MAX + VAL_OUTSIDE_ZERO;  // 合計
      // ゲタの割合 1.83/31 = 0.059;  // rader_chart.mbのモデル形状が崩れない最小の半径は0.059である(最大の半径を1としたとき)。よって、ゲタの割合は0.059以上でなければならない。

// ↓
// ↓ 小さ過ぎたので下記のように変更↓
// ↓

//const u32 BoxRaderChart::VAL_OUTSIDE_MAX   =    31;      // 個体値(この値までは成り得る)
//const u32 BoxRaderChart::VAL_OUTSIDE_ZERO  =   3.1;      // 0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
//const u32 BoxRaderChart::VAL_TOTAL = VAL_OUTSIDE_MAX + VAL_OUTSIDE_ZERO;  // 合計
      // ゲタの割合 3.1/31 = 0.1;

// ↓
// ↓ @fix NMCat[832] グラフの数値が最大の時に外枠に達していない
// ↓ 上方向と下方向に31、斜め横方向4つは0、というグラフにしたとき、上方向と下方向が枠の外周まで届かなかったので、ゲタを大きくした。
// ↓ また、中心座標の指定を(x.5, y)(x,yは整数)で指定してもらうようにした。枠の絵に描かれている縦線の位置に合うし、上下方向に伸ばしたときにドットがきれいに描画されるから。
// ↓ 横は0.5ずらさなくても枠の絵に合う。真横ではなく斜めに伸ばしているのでドットの描かれ方も気にしなくていい。
// ↓

const u32 BoxRaderChart::VAL_OUTSIDE_MAX   =    31;      // 個体値(この値までは成り得る)
const u32 BoxRaderChart::VAL_OUTSIDE_ZERO  =  5.45;      // 0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
const u32 BoxRaderChart::VAL_TOTAL = VAL_OUTSIDE_MAX + VAL_OUTSIDE_ZERO;  // 合計
      // 【誤】ゲタの割合 5.45/31 = 0.1758;  ←ゲタの割合の計算が間違っている。正しくは下行の通り。
      // 【正】ゲタの割合 5.45/(5.45+31)=0.14952;



void BoxRaderChart::SetBoxRadius(f32 outsideRadius)
{
  RaderChart::SetRadius(outsideRadius, outsideRadius);  // 親の関数を呼び、値を設定する
}

void BoxRaderChart::SetBoxVal(Figure fig, u32 item, u32 boxVal)
{ 
  u32 valU32 = 0;
  
  if(item < ITEM_NUM)
  {
    if(fig == FIGURE_INSIDE)
    {
      // 種族値
      if(boxVal > VAL_INSIDE_MAX)
      {
        GFL_ASSERT(0);  // 範囲外。開発中に気付かせるためのASSERT。
        valU32 = VAL_INSIDE_MAX;
      }
      else
      {
        valU32 = boxVal;
      }
      this->updateBoxVal(fig, item, valU32);
    }
    else if(fig == FIGURE_OUTSIDE)
    {
      // 個体値
      if(boxVal > VAL_OUTSIDE_MAX)
      {
        GFL_ASSERT(0);  // 範囲外。開発中に気付かせるためのASSERT。
        valU32 = VAL_OUTSIDE_MAX;
      }
      else
      {
        valU32 = boxVal;
      }
      this->updateBoxVal(fig, item, valU32);
    }
    else
    {
      GFL_ASSERT(0);  // figが範囲外。開発中に気付かせるためのASSERT。
    }
  }
  else
  {
    GFL_ASSERT(0);  // itemが範囲外。開発中に気付かせるためのASSERT。
  }
}

void BoxRaderChart::updateBoxVal(Figure fig, u32 item, u32 boxVal)
{
  // fig, item, boxValは適切な値になってやって来るので、ここではチェック不要。
  m_boxVal[fig][item] = boxVal;
  
  //u32 valU32Inside  = m_boxVal[FIGURE_INSIDE][item];  @fix GFCHECK[8230]  ワーニング対処  ←使っていないのでコメントアウト。
  u32 valU32Outside = m_boxVal[FIGURE_OUTSIDE][item];
  
  f32 valF32 = 0.0f; 
 
  /*
  valF32 = (static_cast<f32>(valU32Inside + VAL_INSIDE_ZERO))/(static_cast<f32>(VAL_TOTAL_MAX));
  RaderChart::SetVal(FIGURE_INSIDE, item, valF32);  // 親の関数を呼び、値を設定する
  
  if(valU32Outside == 0)
  {
    valF32 = (static_cast<f32>(valU32Inside + VAL_INSIDE_ZERO + valU32Outside*VAL_OUTSIDE_TIMES))/(static_cast<f32>(VAL_TOTAL_MAX));  // 外側が0のときは外側のゲタははかせない
  }
  else
  {
    valF32 = (static_cast<f32>(valU32Inside + VAL_INSIDE_ZERO + valU32Outside*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO))/(static_cast<f32>(VAL_TOTAL_MAX));
  }
  RaderChart::SetVal(FIGURE_OUTSIDE, item, valF32);  // 親の関数を呼び、値を設定する
  */
 
  // 内側
  RaderChart::SetVal(FIGURE_INSIDE, item, 0);  // 親の関数を呼び、値を設定する
  // 外側
  valF32 = (static_cast<f32>(valU32Outside + VAL_OUTSIDE_ZERO))/(static_cast<f32>(VAL_TOTAL));
  RaderChart::SetVal(FIGURE_OUTSIDE, item, valF32);  // 親の関数を呼び、値を設定する
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


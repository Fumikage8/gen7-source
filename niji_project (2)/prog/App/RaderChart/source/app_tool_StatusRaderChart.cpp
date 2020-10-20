//==============================================================================
/**
 @file    app_tool_StatusRaderChart.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   ステータスのレーダーチャート
 */
//==============================================================================


// nijiのインクルード
#include <App/RaderChart/include/app_tool_StatusRaderChart.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


// レーダーチャートの透過率
const f32 StatusRaderChart::RADAR_CHART_ALPHA = 0.75f;
// レーダーチャートの色(能力値)
const gfl2::math::Vector4 StatusRaderChart::RADAR_CHART_NORMAL_PARAM_COLOR = gfl2::math::Vector4( 0.0f, (70.0f / 255.0f), 1.0f, RADAR_CHART_ALPHA );
// レーダーチャート内側の色   @note 内側は透過させない
const gfl2::math::Vector4 StatusRaderChart::RADAR_CHART_INSIDE_COLOR = gfl2::math::Vector4( 1.0f, (240.0f / 255.0f), 0.0f, 1.0f );
// レーダーチャート外側の色
const gfl2::math::Vector4 StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR = gfl2::math::Vector4( 1.0f, (155.0f / 255.0f), 0.0f, RADAR_CHART_ALPHA );
// レーダーチャート外側の色(努力値MAX)
const gfl2::math::Vector4 StatusRaderChart::RADAR_CHART_OUTSIDE_COLOR_DORYOKU_MAX = gfl2::math::Vector4( 0.0f, (200.0f / 255.0f), 1.0f, RADAR_CHART_ALPHA );



////////////////////////
// 種族値＆努力値
////////////////////////
//const u32 StatusRaderChart::VAL_INSIDE_MAX    = 255;      // 種族値(この値までは成り得る)
//const u32 StatusRaderChart::VAL_INSIDE_ZERO   =  50;      // 0のときどれだけ大きいところに描くかc  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
//const u32 StatusRaderChart::VAL_OUTSIDE_MAX   = 252;      // 努力値(この値までは成り得る)
//const f32 StatusRaderChart::VAL_OUTSIDE_TIMES =   0.71f;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)*(4/5)=VAL_INSIDE_MAXとなるような値(VAL_INSIDE_MAXのときに4/5までグラフが来るような値)
//const u32 StatusRaderChart::VAL_OUTSIDE_ZERO  =  25;      // 0のときどれだけ大きいところに描くかb
//const u32 StatusRaderChart::VAL_TOTAL = VAL_INSIDE_MAX + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;  // 合計
//      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
//const u32 StatusRaderChart::VAL_TOTAL_MAX = 140 + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;
//      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。
//      // 140+50+252*0.71+25 = 393.92;
//      // ゲタの割合 50/393.92 = 0.12693;  ←能力値のゲタと割合を合わせておく

// ↓
// ↓ 値を調整、ゲタの割合を合わせる必要もなくなった↓
// ↓

//const u32 StatusRaderChart::VAL_INSIDE_MAX    = 255;      // 種族値(この値までは成り得る)
//const u32 StatusRaderChart::VAL_INSIDE_ZERO   =  28.1;    // 0のときどれだけ大きいところに描くかc  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
//const u32 StatusRaderChart::VAL_OUTSIDE_MAX   = 252;      // 努力値(この値までは成り得る)
//const f32 StatusRaderChart::VAL_OUTSIDE_TIMES =   1.11f;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)*(1/3)=VAL_INSIDE_MAXとなるような値(VAL_INSIDE_MAXのときに1/3までグラフが来るような値)
//const u32 StatusRaderChart::VAL_OUTSIDE_ZERO  =  28.1;    // 0のときどれだけ大きいところに描くかb
//const u32 StatusRaderChart::VAL_TOTAL = VAL_INSIDE_MAX + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;  // 合計
//      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
//const u32 StatusRaderChart::VAL_TOTAL_MAX = 140 + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;
//      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。
//      // 140+28.1+252*1.11+28.1 = 475.92;
//      // ゲタの割合 28.1/475.92 = 0.059;  // rader_chart.mbのモデル形状が崩れない最小の半径は0.059である(最大の半径を1としたとき)。よって、ゲタの割合は0.059以上でなければならない。

// ↓
// ↓ 小さ過ぎたので下記のように変更↓あと、上の1/3は255ではなく140になっていたので間違っていた？いやそうしないと絵が描けないので正しい。
// ↓

/*
////////NMCat[2019]のためにここをコメントアウトし下記のように修正////////
const u32 StatusRaderChart::VAL_INSIDE_MAX    = 255;       // 種族値(この値までは成り得る)
const u32 StatusRaderChart::VAL_INSIDE_ZERO   =  50;       // 0のときどれだけ大きいところに描くかc  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
const u32 StatusRaderChart::VAL_OUTSIDE_MAX   = 252;       // 努力値(この値までは成り得る)
const f32 StatusRaderChart::VAL_OUTSIDE_TIMES =   1.111f;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)*(1/3)=140となるような値(140のときに1/3までグラフが来るような値)(140はINSIDEの値)
const u32 StatusRaderChart::VAL_OUTSIDE_ZERO  =  30;       // 0のときどれだけ大きいところに描くかb
const u32 StatusRaderChart::VAL_TOTAL = VAL_INSIDE_MAX + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;  // 合計
      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
const u32 StatusRaderChart::VAL_TOTAL_MAX = 140 + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;
      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。
      // 140+50+252*1.111+30 = 499.972;
      // ゲタの割合 50/499.972 = 0.1;  // rader_chart.mbのモデル形状が崩れない最小の半径は0.059である(最大の半径を1としたとき)。よって、ゲタの割合は0.059以上でなければならない。
*/

// ↓
// ↓ @fix NMCat[2019] ステータス画面の「種族値+努力値」のグラフが、隣接する能力によって見え方が異なってしまう
// ↓ app_tool_BoxRaderChart.cppに施した「NMCat[832] グラフの数値が最大の時に外枠に達していない」に対する修正と同様のことを行う。ゲタを大きくするのは必須で、必要に応じて中心座標を(x.5, y)にしたり、半径を大きくしたり。
// ↓

const u32 StatusRaderChart::VAL_INSIDE_MAX    = 255;       // 種族値(この値までは成り得る)
const f32 StatusRaderChart::VAL_INSIDE_ZERO   =  79.108f;  // 0のときどれだけ大きいところに描くかc  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
const u32 StatusRaderChart::VAL_OUTSIDE_MAX   = 252;       // 努力値(この値までは成り得る)
const f32 StatusRaderChart::VAL_OUTSIDE_TIMES =   1.111f;  // 何倍するかa  // aは(140+VAL_OUTSIDE_MAX*a)*(1/3)=140となるような値(140のときに1/3までグラフが来るような値)(140はINSIDEの値)
const u32 StatusRaderChart::VAL_OUTSIDE_ZERO  =  30;       // 0のときどれだけ大きいところに描くかb
const f32 StatusRaderChart::VAL_TOTAL = VAL_INSIDE_MAX + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;  // 合計
      // 種族値 + 種族値0のときもcのところに描く + 努力値*a + 努力値0のときも種族値よりb大きいところに描く
const f32 StatusRaderChart::VAL_TOTAL_MAX = 140 + VAL_INSIDE_ZERO + VAL_OUTSIDE_MAX*VAL_OUTSIDE_TIMES + VAL_OUTSIDE_ZERO;
      // 合計がこの値以上なら振り切る。VAL_INSIDE_MAXのところを140にした。
      // 140+79.108+252*1.111+30 = 529.08;
      // ゲタの割合 79.108/529.08 = 0.14952;  // rader_chart.mbのモデル形状が崩れない最小の半径は0.059である(最大の半径を1としたとき)。よって、ゲタの割合は0.059以上でなければならない。



////////////////////////
// 能力値
////////////////////////
//const u32 StatusRaderChart::VAL_AMA_DISP_ZERO =  58;  // 表示上の最小値。0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
////////NMCat[2019]のためにここをコメントアウトし下記のように修正////////const u32 StatusRaderChart::VAL_AMA_DISP_MAX  = 400;  // 表示上の最大値。この値以上なら振り切る。
      // ゲタの割合 58/(58+400) = 0.12664;  ←種族値＆努力値のゲタと割合を合わせておく
// ↓
// ↓ VAL_AMA_DISP_ZEROを変更した理由↓
// rader_chart.mbのモデル形状が崩れない最小の半径は0.059である(最大の半径を1としたとき)。よって、ゲタの割合は0.059以上でなければならない。
// ↓
// 能力値の最低値は5である。現在設定されているエルミートの値においては0～5のときの5増加は、約0.028の割合だけ増えることになる。
// ゲタを13にすれば、ゲタの割合が 13/(13+400) = 0.031 となり、0.028と合わせて0.059となる。
// ↓ これらを踏まえて↓
// ↓
////////NMCat[2019]のためにここをコメントアウトし下記のように修正////////const u32 StatusRaderChart::VAL_AMA_DISP_ZERO =  13;  // 表示上の最小値。0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。

// ↓
// ↓ @fix NMCat[2019] ステータス画面の「種族値+努力値」のグラフが、隣接する能力によって見え方が異なってしまう
// ↓ app_tool_BoxRaderChart.cppに施した「NMCat[832] グラフの数値が最大の時に外枠に達していない」に対する修正と同様のことを行う。ゲタを大きくするのは必須で、必要に応じて中心座標を(x.5, y)にしたり、半径を大きくしたり。
// ↓

const u32 StatusRaderChart::VAL_AMA_DISP_MAX  = 400;      // 表示上の最大値。この値以上なら振り切る。
const f32 StatusRaderChart::VAL_AMA_DISP_ZERO =  59.61f;  // 表示上の最小値。0のときどれだけ大きいところに描くか。  // 【注意】頂点に幅を持たせているので、頂点同士が重ならないところまでしか小さくできないようにしておくこと。
      // きれいに見えるゲタの割合はNMCat[832]より5.45/(5.45+31)=0.14952;
      // 能力値の最低値は5。しかし性格補正で4になり得る。さらにヌケニンはHP能力値が1になる。
      // だから、「最低値と言ってもいい能力値4」のときに「きれいに見えるゲタの割合」になり、能力値1のときに「rader_chart.mbのモデル形状が崩れない最小の半径0.059」以上となるようにしておくのがよい。
      // 現在設定されているエルミートの値で4を変換すると9.110324になる(9.110324は「表示上の最大値400」のときの計算結果)。
      // 能力値4のとき(エルミート計算結果9.110324)のグラフの値は(9.110324+59.61)/(59.61+400)=0.14952;(グラフの値は0.0～1.0におさめたもの)となり、「きれいに見えるゲタの割合」となる。
      // 能力値1のとき(エルミート計算結果2.301779)のグラフの値は(2.301779+59.61)/(59.61+400)=0.13471;(グラフの値は0.0～1.0におさめたもの)となり、「モデル形状が崩れない最小の半径」以上となる。



void StatusRaderChart::SetStatusMode(StatusMode mode)
{
  m_statusMode = mode;
 
  // 更新
  if(m_statusMode == STATUS_MODE_PRO)
  {
    for(u32 item=0; item<ITEM_NUM; ++item)
    {
      this->updateStatusVal(STATUS_FIGURE_PRO_INSIDE, item, m_statusVal[STATUS_FIGURE_PRO_INSIDE][item]);
      this->updateStatusVal(STATUS_FIGURE_PRO_OUTSIDE, item, m_statusVal[STATUS_FIGURE_PRO_OUTSIDE][item]);
    }
  }
  else  // (m_statusMode == STATUS_MODE_AMA)
  {
    for(u32 item=0; item<ITEM_NUM; ++item)
    {
      this->updateStatusVal(STATUS_FIGURE_AMA, item, m_statusVal[STATUS_FIGURE_AMA][item]);
    }
  }
  this->SetStatusColor( m_statusColor[STATUS_FIGURE_PRO_INSIDE], m_statusColor[STATUS_FIGURE_PRO_OUTSIDE], m_statusColor[STATUS_FIGURE_AMA] );
}

void StatusRaderChart::SetStatusRadius(f32 outsideRadius)
{
  RaderChart::SetRadius(outsideRadius, outsideRadius);  // 親の関数を呼び、値を設定する
}

void StatusRaderChart::SetStatusColor(const gfl2::math::Vector4& proInsideColor, const gfl2::math::Vector4& proOutsideColor, const gfl2::math::Vector4& amaColor)
{
  m_statusColor[STATUS_FIGURE_PRO_INSIDE ] = proInsideColor;
  m_statusColor[STATUS_FIGURE_PRO_OUTSIDE] = proOutsideColor;
  m_statusColor[STATUS_FIGURE_AMA        ] = amaColor;
  
  // 更新
  if(m_statusMode == STATUS_MODE_PRO)
  {
    RaderChart::SetColor(m_statusColor[STATUS_FIGURE_PRO_INSIDE ], m_statusColor[STATUS_FIGURE_PRO_OUTSIDE]);  // 親の関数を呼び、値を設定する
  }
  else  // (m_statusMode == STATUS_MODE_AMA)
  {
    RaderChart::SetColor(m_statusColor[STATUS_FIGURE_AMA        ], m_statusColor[STATUS_FIGURE_AMA        ]);  // 親の関数を呼び、値を設定する
  }
}

void StatusRaderChart::SetStatusVal(StatusFigure fig, u32 item, u32 statusVal)
{
  u32 valU32 = 0;
 
  if(item < ITEM_NUM)
  {
    switch(fig)
    {
    case STATUS_FIGURE_PRO_INSIDE:
      {
        // 種族値
        if(statusVal > VAL_INSIDE_MAX)
        {
          GFL_ASSERT(0);  // 範囲外。開発中に気付かせるためのASSERT。
          valU32 = VAL_INSIDE_MAX;
        }
        else
        {
          valU32 = statusVal;
        }
        this->updateStatusVal(fig, item, valU32);
        break;
      }
    case STATUS_FIGURE_PRO_OUTSIDE:
      {
        // 努力値
        if(statusVal > VAL_OUTSIDE_MAX)
        {
          GFL_ASSERT(0);  // 範囲外。開発中に気付かせるためのASSERT。
          valU32 = VAL_OUTSIDE_MAX;
        }
        else
        {
          valU32 = statusVal;
        }
        this->updateStatusVal(fig, item, valU32);
        break;
      }
    case STATUS_FIGURE_AMA:
      {
        // 能力値
        valU32 = statusVal;  // HPとその他で値が異なるが同じように扱うため、範囲チェックは行わない。
        this->updateStatusVal(fig, item, valU32);
        break;
      }
    default:
      {
        GFL_ASSERT(0);  // figが範囲外。開発中に気付かせるためのASSERT。
        break;
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // itemが範囲外。開発中に気付かせるためのASSERT。
  }
}

void StatusRaderChart::updateStatusVal(StatusFigure fig, u32 item, u32 statusVal)
{
  // fig, item, statusValは適切な値になってやって来るので、ここではチェック不要。
  m_statusVal[fig][item] = statusVal;
 
  if(m_statusMode == STATUS_MODE_PRO)
  {
    u32 valU32Inside  = m_statusVal[STATUS_FIGURE_PRO_INSIDE][item];
    u32 valU32Outside = m_statusVal[STATUS_FIGURE_PRO_OUTSIDE][item];
    
    f32 valF32 = 0.0f; 
    
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
  }
  else  // (m_statusMode == STATUS_MODE_AMA)
  {
    // 内側
    {
      RaderChart::SetVal(FIGURE_INSIDE, item, 0);  // 親の関数を呼び、値を設定する
    }
    // 外側
    {
      static const f32 HERMITE_POINT_FRAME_VALUE_MIN =   0.0f;  // frameとvalueのMIN値
      static const f32 HERMITE_POINT_FRAME_VALUE_MAX = 100.0f;  // frameとvalueのMAX値
      static const RaderChart::HermitePoint HERMITE_POINT_ARRAY[] =
      {
        { HERMITE_POINT_FRAME_VALUE_MIN, HERMITE_POINT_FRAME_VALUE_MIN, 0          , 2.309923f },
        {                            40,                         60.0f, 1.186237f  , 1.186237f },
        {                            70,                         90.0f, 0.782187f  , 0.782187f },
        { HERMITE_POINT_FRAME_VALUE_MAX, HERMITE_POINT_FRAME_VALUE_MAX, 0.036548f, 0           },
      };
      static const u32 HERMITE_POINT_NUM = sizeof(HERMITE_POINT_ARRAY)/sizeof(HERMITE_POINT_ARRAY[0]);
    
      // m_statusVal[STATUS_FIGURE_AMA][item] を (HERMITE_POINT_FRAME_VALUE_MIN～HERMITE_POINT_FRAME_VALUE_MAX) の範囲内の値に変換する
      f32 valForHermite = m_statusVal[STATUS_FIGURE_AMA][item] / (static_cast<f32>(VAL_AMA_DISP_MAX)) * (HERMITE_POINT_FRAME_VALUE_MAX-HERMITE_POINT_FRAME_VALUE_MIN);
      // それをエルミートで補間する
      f32 hermitePointValue = RaderChart::GetHermiteValue( valForHermite, HERMITE_POINT_ARRAY, HERMITE_POINT_NUM );
      // それを元の範囲内(0～VAL_AMA_DISP_MAX)の値に戻す
      f32 valAfterHermite = hermitePointValue / (HERMITE_POINT_FRAME_VALUE_MAX-HERMITE_POINT_FRAME_VALUE_MIN) * (static_cast<f32>(VAL_AMA_DISP_MAX));

      // ゲタを考慮し(0～1)の範囲内の値にする
      f32 valF32 = (valAfterHermite + VAL_AMA_DISP_ZERO) / (static_cast<f32>(VAL_AMA_DISP_ZERO+VAL_AMA_DISP_MAX));
     
      // 値を設定する
      RaderChart::SetVal(FIGURE_OUTSIDE, item, valF32);  // 親の関数を呼び、値を設定する
    }
  }
}


// @fix NMCat[2019] ステータス画面の「種族値+努力値」のグラフが、隣接する能力によって見え方が異なってしまう
// →ポリゴンを細く描画しなければならないときに、１項目ごとに半径を調整してきれいに見えるようにした。
f32 StatusRaderChart::GetAdjustVal(const f32 val[FIGURE_NUM][ITEM_NUM], u32 fig, u32 item) const
{
  // グラフ値を調整する。(種族値や努力値、能力値を調整するのではなく、最終的にグラフ描画に使う値を調整する。だから、種族値や努力値、能力値に関係なくグラフをきれいに見せられる。)

  static const f32 MIN_VAL_PRO = 0.14951994f;  // 種族値0＋努力値0のときのグラフ値
  static const f32 DIFF_MIN_VAL_PRO = 0.4f;  // 差がこれ以下のときは調整しない
  static const f32 DIFF_MAX_VAL_PRO = 1.0f - MIN_VAL_PRO;  // 「種族値0＋努力値0のときのグラフ値」と「最大のグラフ値(ある一定値以上になるとグラフ値は変わらない)」の差
  static const f32 ADJUST_VAL_PRO[ITEM_NUM] =
  {
    1.045f,//1.036f,
    1.036f,
    1.036f,
    1.036f,
    1.053f,//1.028f,//1.053f,
    1.036f,
  };

  static const f32 MIN_VAL_AMA = 0.13470504f;  // 能力値1のときのグラフ値
  static const f32 DIFF_MIN_VAL_AMA = 0.4f;  // 差がこれ以下のときは調整しない
  static const f32 DIFF_MAX_VAL_AMA = 1.0f - MIN_VAL_AMA;  // 「能力値1のグラフ値」と「能力値最大のグラフ値(ある一定値以上になるとグラフ値は変わらない)」の差
  static const f32 ADJUST_VAL_AMA[ITEM_NUM] =
  {
    1.050f,
    1.050f,
    1.059f,
    1.050f,
    1.059f,
    1.059f,
  };

  f32 ret = 1.0f;

  if(m_statusMode == STATUS_MODE_PRO)
  {
    // 内側、外側共通
    f32 currVal  = val[fig][item];
    f32 leftVal  = val[fig][(item+ITEM_NUM-1)%ITEM_NUM];
    f32 rightVal = val[fig][(item+1)%ITEM_NUM];
    f32 bigVal = leftVal;
    if(bigVal < rightVal) bigVal = rightVal;

    if(currVal > bigVal)  // 「左右の大きいほう」より自身のほうが大きいとき、ポリゴンが細く描画される
    {
      f32 diffVal = currVal - bigVal;  // 差が大きいほどポリゴンが細く描画されるので、差が大きいほど調整値が大きくなる
      if( diffVal > DIFF_MAX_VAL_PRO ) diffVal = DIFF_MAX_VAL_PRO;  // 0<=diffVal<(DIFF_MAX_VAL_PRO)
   
      if( diffVal > DIFF_MIN_VAL_PRO )
      {
        ret = (ADJUST_VAL_PRO[item]-1.0f)/(DIFF_MAX_VAL_PRO-DIFF_MIN_VAL_PRO) * (diffVal-DIFF_MIN_VAL_PRO) + 1.0f;
      }
    }
  }
  else  // (m_statusMode == STATUS_MODE_AMA)
  {
    // 外側だけ
    if(fig == FIGURE_OUTSIDE)
    {
      f32 currVal  = val[fig][item];
      f32 leftVal  = val[fig][(item+ITEM_NUM-1)%ITEM_NUM];
      f32 rightVal = val[fig][(item+1)%ITEM_NUM];
      f32 bigVal = leftVal;
      if(bigVal < rightVal) bigVal = rightVal;

      if(currVal > bigVal)  // 「左右の大きいほう」より自身のほうが大きいとき、ポリゴンが細く描画される
      {
        f32 diffVal = currVal - bigVal;  // 差が大きいほどポリゴンが細く描画されるので、差が大きいほど調整値が大きくなる
        if( diffVal > DIFF_MAX_VAL_AMA ) diffVal = DIFF_MAX_VAL_AMA;  // 0<=diffVal<(DIFF_MAX_VAL_AMA)
       
        if( diffVal > DIFF_MIN_VAL_AMA )
        {
          ret = (ADJUST_VAL_AMA[item]-1.0f)/(DIFF_MAX_VAL_AMA-DIFF_MIN_VAL_AMA) * (diffVal-DIFF_MIN_VAL_AMA) + 1.0f;
        }
      }
    }
  }

  //////// ↓サイズ調整テスト↓ ////////
  /*
  static s32 debugVal = 0;
  if(debugVal == 0)
  {
    return ret;
  }
  else
  {
    return 1.0f;
  }
  */
  //////// ↑サイズ調整テスト↑ ////////
  return ret;
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


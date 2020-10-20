//======================================================================
/**
 * @file BillBoardFade.cpp
 * @date 2015/11/09 16:06:11
 * @author saito_nozomu
 * @brief ビルボードのフェード管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "Field/FieldRo/include/Weather/BillBoardFade.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

/**
 * @brief 初期化
 * @param pModel モデルへのポインタ
 * @param alpha  α値
 */
void BillBoardFade::Initialize(poke_3d::model::BaseModel *pModel, f32 alpha)
{
  m_pModel = pModel;
  m_State = IDLE;
  m_Alpha = alpha;
  SetAlpha(m_Alpha);
}

/**
 * @brief 更新処理
 */
void BillBoardFade::Update(void)
{
  switch(m_State)
  {
  case IDLE:
    break;
  case FADE_OUT:
    m_Alpha -= 0.1f;
    if (m_Alpha < 0)
    {
      m_Alpha = 0;
      m_State = IDLE;
      NOZOMU_PRINT("FADEOUT_END\n");
    }
    SetAlpha(m_Alpha);
    break;
  case FADE_IN:
     m_Alpha += 0.1f;
    if (m_Alpha > 1)
    {
      m_Alpha = 1;
      m_State = IDLE;
      NOZOMU_PRINT("FADEOUT_IN\n");
    }
    SetAlpha(m_Alpha);
    break;
  }
}

/**
 * @brief 表示状態
 * @param visible true で表示
 */
void BillBoardFade::SetVisible(bool visible)
{
  f32 alpha = 0;
  if (visible) alpha = 1;
  m_Alpha = alpha;
  SetAlpha(alpha);
}

/**
 * @brief コンスタントカラーのα値セット
 * @param alpha α値
 */
void BillBoardFade::SetAlpha(f32 alpha)
{
  gfl2::math::Vector4 col = m_pModel->GetMaterialColorConstant(0,0);
  col.w = alpha;
  m_pModel->SetMaterialColorConstant(0,0,col);
  if(alpha > 0)
  {
    m_pModel->SetVisible(true);
  }
  else
  {
    m_pModel->SetVisible(false);
  }
}

/**
 * @brief 表示状態取得
 * @retrun bool trueで表示
 */
bool BillBoardFade::IsVisible(void)
{
  return m_pModel->IsVisible();
}

    
GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
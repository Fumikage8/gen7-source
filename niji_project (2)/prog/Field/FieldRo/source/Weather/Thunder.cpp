//======================================================================
/**
 * @file Thunder.cpp
 * @date 2015/10/27 12:05:35
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
// system
#include "System/include/GflUse.h"

#include "Field/FieldRo/include/Weather/Thunder.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

#define FLASH_ALPHA (0.5)

#define FLASH_BASE_MARGINE (120)
#define FLASH_MARGINE (60)
#define FLASH_SHORT_MARGINE (5)
#define SE_BASE_MARGINE (60)
#define SE_MARGINE (60)

/**
 * @brief 初期化
 * @param pModel モデルへのポインタ
 */
void Thunder::Initialize(poke_3d::model::BaseModel *pModel)
{
  m_pModel = pModel;
  m_Alpha = 0;
  m_NextAlpha = 0;
  m_FlashMargine = FLASH_BASE_MARGINE + System::GflUse::GetPublicRand( FLASH_MARGINE );
  m_SEMargine = SE_BASE_MARGINE + System::GflUse::GetPublicRand( SE_MARGINE );
  m_ShortFlash = false;
  SetAlpha(m_Alpha);
  m_Active = false;
}

/**
 * @brief 更新関数
 */
void Thunder::Update(void)
{
  //フラッシュセット
  Flash();
  //フラッシュ更新
  FlashFade();
}

/**
 * @brief フラッシュ処理のアクティブ状態の制御
 * @param active true　でアクティブ
 */
void Thunder::SetActive(bool active)
{
  m_Active = active;
  m_pModel->SetVisible(m_Active);
}

/**
 * @brief フラッシュ設定
 */
void Thunder::Flash(void)
{
  if (m_Active)
  {
    ModelFlash();
    SoundUpdate();
  }
}

/**
 * @brief モデルのフラッシュ処理
 */
void Thunder::ModelFlash(void)
{
  if (m_FlashMargine <= 0)
  {
    //モデルのα値をセット
    if (m_NextAlpha > 0)
    {
      m_Alpha = m_NextAlpha;
      m_NextAlpha = 0;
    }
    else
    {
      m_Alpha = FLASH_ALPHA + (f32)( System::GflUse::GetPublicRand(20) / 100.0) ;
    }
    //次のフラッシュ間隔を決定
    if ( (!m_ShortFlash) && System::GflUse::GetPublicRand( 100 ) >= 80 )
    {
      //２割で間隔の短いフラッシュ
      m_FlashMargine = System::GflUse::GetPublicRand( FLASH_SHORT_MARGINE ) + 1;
      m_NextAlpha = m_Alpha;
      m_ShortFlash = true;
    }
    else
    {
      //それ以外は長め
      m_FlashMargine = FLASH_BASE_MARGINE + System::GflUse::GetPublicRand( FLASH_MARGINE );
      m_ShortFlash = false;
    }

    //フラッシュのＳＥ（トラック操作）
    Sound::ChangeSETrackVariable( SEQ_SE_FLD_AMB_TENKOU_RAIU, 3, 0, 1 );   //SEのトラック変数を変更する
  }
  else
  {
    if(m_FlashMargine > 0) m_FlashMargine--;
  }
}

/**
 * @brief 雷の音制御
 */
void Thunder::SoundUpdate(void)
{
  if (m_SEMargine <= 0)
  {
    m_SEMargine = SE_BASE_MARGINE + System::GflUse::GetPublicRand( SE_MARGINE );
    //フラッシュのＳＥ（トラック操作）
    Sound::ChangeSETrackVariable( SEQ_SE_FLD_AMB_TENKOU_RAIU, 2, 0, 1 );   //SEのトラック変数を変更する
  }
  else
  {
    if(m_SEMargine > 0) m_SEMargine--;
  }
}

/**
 * @brief フラッシュ更新関数
 */
void Thunder::FlashFade(void)
{
  if ( m_Alpha > 0 )
  {
    m_Alpha -= 0.1;
    if (m_Alpha < 0)
    {
      m_Alpha = 0;
      if (!m_Active)
      {
        m_pModel->SetVisible(false);
      }
    }
    SetAlpha(m_Alpha);
  }
}

/**
 * @brief コンスタントカラーのα値セット
 * @param alpha α値
 */
void Thunder::SetAlpha(f32 alpha)
{
  gfl2::math::Vector4 col = m_pModel->GetMaterialColorConstant(0,0);
  col.w = alpha;
  m_pModel->SetMaterialColorConstant(0,0,col);
}

/**
 * @brief 表示状態取得
 * @retrun bool trueで表示
 */
bool Thunder::IsVisible(void)
{
  return m_pModel->IsVisible();
}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
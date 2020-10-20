/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectQuestion.cpp
 *  @brief  フィールドエフェクト：「？」
 *  @author miyachi_soichi
 *  @date   2015.06.10
 */

#include "Field/FieldRo/include/Effect/content/FieldEffectQuestion.h"
#include "Sound/include/Sound.h"
#include "niji_conv_header/field/effect/question_icon.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectQuestion::EffectQuestion( void )
  :EffectBallonIcon()
{
  m_nSeID = SEQ_SE_FLD_215;
}

/**
 *  @brief  デストラクタ
 */
EffectQuestion::~EffectQuestion( void )
{
}

/**
 *  @brief  初期化
 */
void EffectQuestion::Initialize( EffectBallonIcon::SetupData &setupData )
{
  EffectBallonIcon::Initialize( setupData );
}

/**
 *  @brief  破棄
 */
bool EffectQuestion::Terminate( void )
{
  return EffectBallonIcon::Terminate();
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectQuestion::Delete( void )
{
  EffectBallonIcon::Delete();
}

// リソースID取得
u32 EffectQuestion::GetModelDataID( void )
{
  return BL_IDX_QUESTION_ICON_MAP_CTR_GFBMDLP;
}
u32 EffectQuestion::GetMotionDataID( void )
{
  return BL_IDX_QUESTION_ICON_MAP_GFBMOT;
}

/**
 *  @brief  更新
 */
void EffectQuestion::updateNormal( void )
{
  EffectBallonIcon::updateNormal();
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

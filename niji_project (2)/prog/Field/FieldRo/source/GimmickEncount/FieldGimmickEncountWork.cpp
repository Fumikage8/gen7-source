//======================================================================
/**
 * @file FieldGimmickEncountWork.cpp
 * @date 2015/09/04 16:59:34
 * @author saita_kazuki
 * @brief ギミックエンカウント共通アクセスワーク
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountWork.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

// @note 自機の移動速度が変更されたらここも合わせて変更する
// 自機の移動速度は定義がなくモーションに入っている。が、モーションを再生しないと取れないため
const f32 Work::PLAYER_MOVE_SPEED[Work::PLAYER_MOVE_SPEED_MAX] = 
{
  10.0f,
  20.666f,
};

/**
 * @brief コンストラクタ
 */
Work::Work()
  : m_playerPosition( gfl2::math::Vector3::GetZero())
{
}

/**
 * @brief デストラクタ
 */
Work::~Work()
{
}

/**
 * @brief 初期化
 * @param desc 初期化情報
 */
void Work::Initialize()
{
}

/**
 * @brief 破棄
 */
void Work::Terminate()
{
  m_playerPosition  = gfl2::math::Vector3::GetZero();
}

/**
 * @brief 更新
 * @param desc 更新情報
 */
void Work::Update( const UpdateDescription& desc)
{
  m_playerPosition = desc.playerPosition;
}

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )


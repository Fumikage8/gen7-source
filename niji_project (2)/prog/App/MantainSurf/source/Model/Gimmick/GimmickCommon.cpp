//======================================================================
/**
 * @file GimmickCommon.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 通用型障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "GimmickCommon.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32 GimmickCommon::COLLIDER_RADIUS = 40.0f;

GimmickCommon::GimmickCommon()
{
  m_type                            = TYPE_COMMON;
  //m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_001_normal_BIN;
  //m_SetupParam.m_modelResourceId    = BL_IDX_GMK_001_NORMAL_GMK_001_NORMAL_CTR_GFBMDLP;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = true;
}

GimmickCommon::~GimmickCommon()
{
}

void GimmickCommon::CreateCollider()
{
  AddSphereCollider(COLLIDER_RADIUS);
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

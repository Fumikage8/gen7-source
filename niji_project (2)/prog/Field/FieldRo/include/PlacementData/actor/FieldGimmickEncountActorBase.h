//======================================================================
/**
 * @file FieldGimmickEncountActorBase.h
 * @date 2015/08/20 11:12:06
 * @author saita_kazuki
 * @brief ギミックエンカウントアクター共通インターフェース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_ACTOR_BASE_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_ACTOR_BASE_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldGimmickEncountAccessor.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"

namespace Field { namespace GimmickEncount {
  class Manager;
  class LocalWork;
}}

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @class ギミックエンカウントアクター共通インターフェース
 */
class IFieldGimmickEncountActorBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IFieldGimmickEncountActorBase );

public:

  IFieldGimmickEncountActorBase(){}

  /**
   *  @brief ギミックエンカウント配置情報の取得
   */
  virtual const FieldGimmickEncountAccessor::Data* GetGimmickEncountData() const = 0;

  /**
   *  @brief ギミックエンカウントマネージャーの取得
   *  @return ギミックエンカウントマネージャー
   */
  virtual GimmickEncount::Manager* GetGimmickEncountManager() const = 0;

  /**
   *  @brief イベント通知
   */
  virtual void Notify( GimmickEncount::NotifyState notify) = 0;

  /**
   *  @brief ギミックエンカウントローカルワーク取得
   */
  virtual GimmickEncount::LocalWork* GetLocalWork() const = 0;

  /**
   *  @brief 更新
   */
  virtual void Update() {}
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_GIMMICK_ENCOUNT_ACTOR_BASE_INCLUDED__
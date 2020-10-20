//======================================================================
/**
 * @file FieldGimmickEncountManager.h
 * @date 2015/09/04 16:59:09
 * @author saita_kazuki
 * @brief ギミックエンカウントマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_MANAGER_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountWork.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLocalWork.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"

namespace Field {
  class EventWork;
  class World;
  class Area;
  class FieldResident;
  class IFieldGimmickEncountActorBase;
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @class ギミックエンカウントマネージャー
 */
class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN( Manager );

public:

  static const u32 GIMMICK_ENCOUNT_MAX = 16;

  struct Description
  {
    Description()
      : pEventWork( NULL)
      , pWorld( NULL)
      , pArea( NULL)
      , pResident( NULL)
    {}

    EventWork*      pEventWork;
    World*          pWorld;
    Area*           pArea;
    FieldResident*  pResident;
  };

public:

  /**
   * @brief コンストラクタ
   */
  Manager();

  /**
   * @brief デストラクタ
   */
  virtual ~Manager();

  /**
   * @brief 初期化
   * @param workDesc ワークの初期化情報
   */
  void Initialize( const Description& desc);

  /**
   * @brief 破棄
   */
  void Terminate();

  /**
   * @brief イベントチェック前更新
   */
  void EventCheckPreUpdate();

  /**
   * @brief イベントチェック後更新
   */
  void EventCheckPostUpdate();

  /**
   * @brief 更新
   * @param workDesc ワークの更新情報
   */
  void Update( const Work::UpdateDescription& workDesc);

  /**
   * @brief 共通アクセスワーク取得
   */
  const Work& GetWork() const { return m_work; }

  /**
   * @brief 空いているローカルワーク取得
   */
  LocalWork* GetEmptyLocalWork();

  /**
   * @brief ゾーンのギミックフラグ抽選
   * @param zoneID ゾーンID
   */
  void LotteryZoneGimmickFlag( ZoneID zoneID);

  /**
   * @brief このクラスに対してアクターを登録
   */
  void RegistActor( IFieldGimmickEncountActorBase* pActor);

  /**
   * @brief このクラスに対してアクターを削除
   */
  void UnRegistActor( IFieldGimmickEncountActorBase* pActor);

private:

  /**
   * @brief ギミックフラグ抽選
   * @param pZoneGimmickEncountPlacementData そのゾーンのギミックエンカウント配置データ
   * @param pGimmickEncountData ギミックエンカウントテーブルデータ
   */
  void LotteryGimmickFlag( EventWork* pEventWork, void* pZoneGimmickEncountPlacementData, void* pGimmickEncountData);

  /**
   * @brief SEの更新
   */
  void UpdateSE();

private:

  Work                              m_work;
  LocalWork                         m_localWork[ GIMMICK_ENCOUNT_MAX ];

  Description                       m_desc;

  IFieldGimmickEncountActorBase*    m_pActor[ GIMMICK_ENCOUNT_MAX ];
};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_MANAGER_H_INCLUDED__


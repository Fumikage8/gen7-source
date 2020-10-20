//======================================================================
/**
 * @file FieldGimmickEncountLocalWork.h
 * @date 2015/09/04 16:59:09
 * @author saita_kazuki
 * @brief ギミックエンカウント各々がアクセスできるワーク
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_LOCAL_WORK_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_LOCAL_WORK_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector3.h>

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"


namespace Field {
  class IFieldGimmickEncountActorBase;
}



GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @class ギミックエンカウント各々がアクセスできるワーク
 */
class LocalWork
{
  GFL_FORBID_COPY_AND_ASSIGN( LocalWork );

public:

  /**
   * @brief 初期化情報
   */
  struct InitializeDescription
  {
    void*                           pTypesData;       ///< タイプごとデータ
    GimmickType                     gimmickType;      ///< ギミック型
    u16                             gimmickFlagNo;    ///< ギミックフラグNo
    gfl2::math::Vector3             firstPosition;    ///< 初期位置
    IFieldGimmickEncountActorBase*  pActor;           ///< アクター
  };

  /**
   * @brief 更新情報
   */
  struct UpdateDescription
  {
    gfl2::math::Vector3 nowPosition;    ///< 現在位置
  };

public:

  /**
   * @brief コンストラクタ
   */
  LocalWork();

  /**
   * @brief デストラクタ
   */
  virtual ~LocalWork();

  /**
   * @brief 初期化
   * @param desc 初期化情報
   */
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief 更新
   * @param desc 更新情報
   */
  void Update( const UpdateDescription& desc);

  /**
   * @brief クリア
   */
  void Clear();

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
   *  @brief イベント通知
   */
  void Notify( NotifyState notify);

  /**
   *  @brief SEパラメーターセット
   */
  void SetSEParameter( bool isPlaySE, u32 nowSEID);

  /**
   *  @brief SEを鳴らすか
   */
  bool IsPlaySE( u32* pOutSEID) const ;

  /**
   * @brief getter
   */
  bool IsActive() const { return m_isActive; }
  NotifyState GetNotifyState() const { return m_notifyState; }
  NotifyState GetNotifyStateOld() const { return m_notifyStateOld; }
  void* GetTypesData() const { return m_pTypesData; }
  GimmickType GetGimmickType() const { return m_gimmickType; }
  u16 GetGimmickFlagNo() const { return m_gimmickFlagNo; }
  const gfl2::math::Vector3& GetFirstPosition() const { return m_firstPosition; }
  const gfl2::math::Vector3& GetNowPosition() const { return m_nowPosition; }
  const IFieldGimmickEncountActorBase* GetActor() const { return m_pActor; }

private:

  bool                            m_isActive;
  NotifyState                     m_notifyState;
  NotifyState                     m_notifyStateOld;
  void*                           m_pTypesData;
  GimmickType                     m_gimmickType;
  u16                             m_gimmickFlagNo;
  gfl2::math::Vector3             m_firstPosition;
  gfl2::math::Vector3             m_nowPosition;
  IFieldGimmickEncountActorBase*  m_pActor;
  u32                             m_nowSEID;
  bool                            m_isPlaySE;
};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_LOCAL_WORK_H_INCLUDED__


/**
 *  GAME FREAK inc.
 *
 *  @file   FieldPosEventActor.h
 *  @brief  配置イベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.27
 */

#if !defined(__FIELD_POSEVENT_ACTOR_H__)
#define __FIELD_POSEVENT_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldPosEventAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldPosEventActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldPosEventActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldPosEventActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldPosEventActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_PLACEMENT; }
  
  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  専用データ設定
   */
  virtual void SetData( const FieldPosEventAccessor::PosData *pData );

  /**
   *  @brief  専用データ取得
   */
  virtual const FieldPosEventAccessor::PosData *GetData( void ) const { return m_pData; }


  /**
   *  @brief  データポジション取得
   */
  virtual gfl2::math::Vector3 GetDataPosition() const {
    return gfl2::math::Vector3(m_pData->position[0],m_pData->position[1],m_pData->position[2]);
  }

  /**
   *  @brief  データクォータニオン取得
   */
  virtual gfl2::math::Quaternion GetDataQuaternion() const {
    return gfl2::math::Quaternion(m_pData->quaternion[0],m_pData->quaternion[1],m_pData->quaternion[2],m_pData->quaternion[3]);
  }


protected:
  // 専用データ
  const FieldPosEventAccessor::PosData *m_pData;

}; // class FieldPosEventActor

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_POSEVENT_ACTOR_H__)

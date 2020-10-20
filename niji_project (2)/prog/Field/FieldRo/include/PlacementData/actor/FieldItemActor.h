//======================================================================
/**
 * @file FieldItemActor.h
 * @date 2015/09/04 16:01:09
 * @author ikawa_hiroki
 * @brief 配置アイテム用アクター
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined(__FIELDITEMACTOR_H__)
#define __FIELDITEMACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldItemAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldItemActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldItemActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldItemActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldItemActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_ITEM; }

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
  void SetData( const FieldItemAccessor::FieldItemData *data );

  /**
   *  @brief  専用データ取得
   */
  const FieldItemAccessor::FieldItemData *GetData( void ) const { return m_pData; }

  /**
   *  @brief  イベントID取得
   */
  virtual u32 GetDataEventID() const { return m_pData->eventID; }

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
  const FieldItemAccessor::FieldItemData *m_pData;

}; // class FieldItemActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELDITEMACTOR_H__)

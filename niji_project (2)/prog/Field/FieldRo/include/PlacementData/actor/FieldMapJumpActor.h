/**
 *  GAME FREAK inc.
 *
 *  @file   FieldMapJumpActor.h
 *  @brief  マップジャンプ用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.28
 */

#if !defined(__FIELD_MAP_JUMP_ACTOR_H__)
#define __FIELD_MAP_JUMP_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMapJumpAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldMapJumpActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldMapJumpActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldMapJumpActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldMapJumpActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_MAPJUMP; }
  
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
  void SetData( const FieldMapJumpAccessor::MapJumpData *pData );

  /**
   *  @brief  専用データ取得
   */
  const FieldMapJumpAccessor::MapJumpData *GetData( void ) const { return m_pData; }

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

  /**
  *  @brief  カメラフォーカス停止チェック
  */
  virtual bool IsCameraFocusStop( void ) const { return false; }
  

protected:
  // 専用データ
  const FieldMapJumpAccessor::MapJumpData *m_pData;

}; // class FieldMapJumpActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_MAP_JUMP_ACTOR_H__)

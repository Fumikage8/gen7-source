/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBgEventActor.h
 *  @brief  Bgイベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.06.01
 */

#if !defined(__FIELD_BG_EVENT_ACTOR_H__)
#define __FIELD_BG_EVENT_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldBgEventAccessor.h"

GFL_NAMESPACE_BEGIN(Field);

class FieldBgEventActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldBgEventActor );

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldBgEventActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldBgEventActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_BG; }

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
  void SetData( const FieldBgEventAccessor::BgEventData *data );

  /**
   *  @brief  専用データ取得
   */
  const FieldBgEventAccessor::BgEventData *GetData( void ) const { return m_pData; }


  /**
   *  @brief  アクションボタンの注視をクリアするフラグ  BgEventはクリアしない。
   */
  virtual bool IsActionButtonInterestClear( void ) const;

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
   *  @brief  フラグワークの値セット
   */
  virtual bool SetEventFlagWork( u16 value );


protected:
  // 専用データ
  const FieldBgEventAccessor::BgEventData *m_pData;

}; // class FieldBgEventActor : public IFieldActorBase

GFL_NAMESPACE_END(Field);

#endif // !defined(__FIELD_BG_EVENT_ACTOR_H__)

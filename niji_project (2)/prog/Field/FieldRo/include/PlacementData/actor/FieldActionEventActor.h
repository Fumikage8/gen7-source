/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActionEventActor.h
 *  @brief  アクションイベント用アクター
 *  @author miyachi_soichi
 *  @date   2015.05.27
 */

#if !defined(__FIELD_ACTION_EVENT_ACTOR_H__)
#define __FIELD_ACTION_EVENT_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldActionEventAccessor.h"

GFL_NAMESPACE_BEGIN( Field )

// 話しかけアクター
class FieldActionEventTalkActor;

class FieldActionEventActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldActionEventActor );

public:

  struct Description
  {
    BaseCollisionScene                                    *pCollisionScene;
    BaseCollisionScene                                    *pCollisionSceneTalk;
    System::nijiAllocator           *pAllocator;
    gfl2::heap::HeapBase            *pHeap;
    FieldCollisionAccessor          *colAccessor;
    gfl2::math::Vector3             vPos;
    const FieldActionEventAccessor::ActionEventData * pData;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldActionEventActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldActionEventActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_ACTION; }

  /**
   *  @brief  初期化
   */
  void Initialize( const Description& description );

  /**
   *  @brief  破棄
   */
  virtual void Terminate( void );

  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  追加
   */
  virtual void RegistActor( void );

  /**
   *  @brief  削除
   */
  virtual void UnregistActor( void );

  /**
   *  @brief  専用データの取得
   */
  const FieldActionEventAccessor::ActionEventData *GetData( void ) const { return m_pData; }

  /**
   *  @brief  専用アクセサー初期化
   */
  void InitializeAccessor( const void* pData);


  /**
   *  @brief  判定の共通処理
   */
  static bool _IsInvalid( const FieldActionEventAccessor::ActionEventData *pData );

  static bool IsTalkEventMake( const FieldActionEventAccessor::ActionEventData *pData );
  static u32 GetTalkEventScrID( const FieldActionEventAccessor::ActionEventData *pData );

protected:
  // 専用データ
  const FieldActionEventAccessor::ActionEventData  *m_pData;
  FieldActionEventAccessor                          m_Accessor;

  // 話しかけコリジョンの情報
  FieldActionEventTalkActor  * m_pTalkActor;

}; // class FieldActionEventActor


// 話しかけ範囲用
class FieldActionEventTalkActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldActionEventTalkActor );

public:

  /** 
   *  @brief コンストラクタ
   */
  FieldActionEventTalkActor(void);

  /** 
   *  @brief デストラクタ
   */
  virtual ~FieldActionEventTalkActor(void);

  /**
   *  @brief  初期化
   */
  void Initialize( const FieldActionEventActor::Description& description );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_ACTION_TALK; }

  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

protected:
  // 専用データ
  const FieldActionEventAccessor::ActionEventData  *m_pData;
};

GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_ACTION_EVENT_ACTOR_H__)

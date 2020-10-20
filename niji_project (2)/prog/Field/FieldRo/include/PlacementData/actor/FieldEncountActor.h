/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEncountActor.h
 *  @brief  エンカウント用アクター
 *  @author saita_kazuki
 *  @date   2015.05.31
 */

#if !defined(__FIELD_ENCOUNT_ACTOR_H__)
#define __FIELD_ENCOUNT_ACTOR_H__
#pragma once

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldEncountAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldEncountActor : public IFieldActorBase
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldEncountActor );

public:
  /**
   *  初期化用構造体
   */
  struct EncountSetupData
  {
    System::nijiAllocator*                            pAllocator;
    gfl2::heap::HeapBase*                             pHeap;
    BaseCollisionScene*                               pCollisionScene;
    FieldEncountAccessor*                             pAccessor;
    const FieldEncountAccessor::EncountData*          pEncountData;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldEncountActor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldEncountActor( void );

  /**
   *  @brief  Encount作成
   */
  void Initialize( EncountSetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual void Terminate( void );
  
  /**
   *  @brief  追加
   */
  virtual void RegistActor( void );

  /**
   *  @brief  削除
   */
  virtual void UnregistActor( void );

  /**
   *  @brief  種別取得
   */
  virtual FieldActorType GetType( void ) const { return FIELD_ACTOR_TYPE_ENCOUNT; }
  
  /**
   *  @brief  起動
   */
  virtual bool Kick( const KickRequest &req, bool* p_outIsEventCreate );

  /**
   *  @brief  判定
   */
  virtual bool IsInvalid( void );

  /**
   *  @brief  専用データ取得
   */
  const FieldEncountAccessor::EncountData *GetData( void ) const { return m_pData; }

  /**
   *  @brief エンカウントできるか
   *  @param attr チェックするアトリビュート
   *  @retval true エンカウント可
   *  @retval false エンカウント不可
   */
  bool IsEncount( u32 attr);

protected:

  /**
   *  @brief  対象アトリビュートビットフィールド取得
   */
  u32 GetTargetAttributeBits( u32 attr);

  /**
   *  @brief  対象アトリビュートビットフィールド作成
   */
  u32 MakeTargetAttributeBits( u32 attr);

protected:
  // 専用データ
  const FieldEncountAccessor::EncountData*  m_pData;

}; // class FieldEncountActor : public IFieldActorBase

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_ENCOUNT_ACTOR_H__)

//======================================================================
/**
 * @file FieldDebugEffect.h
 * @date 2015/08/28 12:48:01
 * @author miyachi_soichi
 * @brief デバッグ：エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUG_EFFECT_H_INCLUDED__
#define __FIELDDEBUG_EFFECT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugEffect
 * @brief クラスの説明
 */
class DebugEffect
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugEffect);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugEffect( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugEffect( void );

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, Field::Effect::EffectManager *pEffectManager, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  /**
   *  @brief  再生中のエフェクトが指定と異なる場合
   */
  void CheckCurrentEffect( u32 type, bool bCheckAnim );
  /**
   *  @brief  エフェクト発行リクエスト
   */
  void RequestPlayEffect( u32 type );

  Field::Effect::IEffectBase *GetEffect( void ){ return m_pEffect; }
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }
  inline  gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }
  u32 GetEffectType( void ) { return m_eEffectType; }

  
  void ClearDynamicLoadControl( void );
  void UnloadAllDynamicLoadControl( void );

private:

  void loadDynaicLoadResource( Effect::Type type );
  void unloadDynamicLoadResource( Effect::Type type );

private:



  gfl2::heap::HeapBase          *m_pHeap;
  Field::Effect::EffectManager  *m_pEffectManager;
  Field::Effect::IEffectBase    *m_pEffect;
  u32                           m_eEffectType;
  gfl2::str::MsgData*           m_pMsgData;

  b32                           m_IsLoad[ Effect::EFFECT_TYPE_MAX ];

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_EFFECT_H_INCLUDED__

#endif // PM_DEBUG

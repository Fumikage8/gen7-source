//======================================================================
/**
 * @file FieldDebugTrialModel.h
 * @date 2015/09/08 14:50:08
 * @author miyachi_soichi
 * @brief デバッグ：TrialModel
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUG_TRIALMODEL_H_INCLUDED__
#define __FIELDDEBUG_TRIALMODEL_H_INCLUDED__
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
GFL_NAMESPACE_BEGIN(TrialModel)
class FieldTrialModel;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

/**
 * @class DebugTrialModel
 * @brief クラスの説明
 */
class DebugTrialModel
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugTrialModel);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugTrialModel( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugTrialModel( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  Field::TrialModel::TrialModelFactory *GetFactory( void ) const { return m_pFactory; }
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

public:
  enum State
  {
    STATE_NONE,
    STATE_INIT,
    STATE_READY,
    STATE_TERM,
  };
  State m_eState;
  enum MotionPackState
  {
    MS_NONE,
    MS_OPNE_WAIT,
    MS_OPEN,
    MS_LOAD_WAIT,
    MS_LOAD,
    MS_CLOSE_WAIT,
  };
  MotionPackState m_eMotionState;
  void *m_pResource;
  Field::TrialModel::FieldTrialModel         *m_pModel;
  gfl2::gfx::IGLAllocator *m_pAllocator;
  s32 m_nAnim;

private:
  gfl2::heap::HeapBase                  *m_pHeap;
  Field::TrialModel::TrialModelFactory  *m_pFactory;
  gfl2::str::MsgData*                   m_pMsgData;

};

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_TRIALMODEL_H_INCLUDED__

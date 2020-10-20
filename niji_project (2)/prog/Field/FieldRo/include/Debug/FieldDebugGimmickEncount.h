//======================================================================
/**
 * @file FieldDebugGimmickEncount.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：GimmickEncount
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUG_GIMMICK_ENCOUNT_H_INCLUDED__
#define __FIELDDEBUG_GIMMICK_ENCOUNT_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugGimmickEncount
 * @brief クラスの説明
 */
class DebugGimmickEncount
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugGimmickEncount);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugGimmickEncount( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugGimmickEncount( void );
  
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

  //! ヒープの取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }


  static void UpdateEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item );
  static wchar_t* DrawEaseFunc( void* userWork, gfl2::debug::DebugWinItem* item );

  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }


public:

  gfl2::heap::HeapBase*   m_pHeap;
  gfl2::str::MsgData*     m_pMsgData;    

  static bool             m_isSeEnable;
  static f32              m_playSeLength;
  static f32              m_volumeChangeLengthRate;
  static f32              m_changeVolume;

  static bool             m_isDebugMotionBlurEnable;
  static u32              m_motionBlurBlendType;
  static u32              m_motionBlurAnimeType;

  static bool             m_isDebugMotionBlurBlendInterpEnable;
  static u32              m_motionBlurBlendInterpType;
  static f32              m_motionBlurBlendEnd;
  static s32              m_motionBlurBlendFrame;

  static bool                 m_isDebugMotionBlurColorInterpEnable;
  static u32                  m_motionBlurColorInterpType;
  static gfl2::math::Vector3  m_motionBlurColorEnd;
  static s32                  m_motionBlurColorFrame;

  static bool             m_isEncountEffectEnable;
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_GIMMICK_ENCOUNT_H_INCLUDED__

#endif // PM_DEBUG

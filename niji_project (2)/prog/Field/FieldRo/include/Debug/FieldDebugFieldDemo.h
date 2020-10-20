//======================================================================
/**
 * @file FieldDebugFieldDemo.h
 * @date 2015/11/12 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：FieldDemo フィールドデモイベント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUG_FIELD_DEMO_H_INCLUDED__
#define __FIELDDEBUG_FIELD_DEMO_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
}
namespace Field {
  class Fieldmap;
}
namespace gfl2 {
  namespace renderingengine { namespace scenegraph {
    namespace resource {
      class TextureResouceNode;
    }
  }}
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


/**
 * @class DebugFieldDemo
 * @brief デバッグ：FieldDemo フィールドデモイベント
 */
class DebugFieldDemo
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugFieldDemo);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugFieldDemo( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugFieldDemo( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase* pHeap,
    gfl2::str::MsgData* pMsgData,
    Fieldmap* pFieldmap
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

public:

  gfl2::heap::HeapBase*                                     m_pHeap;
  gfl2::str::MsgData*                                       m_pMsgData;
  Fieldmap*                                                 m_pFieldmap;
  s32                                                       m_demoID;
  u32                                                       m_option;
  u32                                                       m_unUseOption;
  DemoLib::Sequence::SequenceRenderingPipeLine::UseOption   m_renderUseOption;
  bool                                                      m_isForceOneStep;
  s32                                                       m_textureIndex;
  gfl2::renderingengine::scenegraph::DagNode*               m_pTextureNode;


  // Control
  s32       m_controlOption;
  s32       m_controlUnUseOption;
  s32       m_controlRenderUseOption;

};


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_FIELD_DEMO_H_INCLUDED__

#endif // PM_DEBUG


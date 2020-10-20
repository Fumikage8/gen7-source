//======================================================================
/**
 * @file FieldDebugStaticModel.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：StaticModel 配置モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __FIELDDEBUG_STATIC_MODEL_H_INCLUDED__
#define __FIELDDEBUG_STATIC_MODEL_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include <niji_conv_header/field/static_model/static_model_id.h>

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
  namespace fs {
    class AsyncFileManager;
  }
}

namespace Field {
  class Fieldmap;
  class MyRenderingPipeLine;
  namespace StaticModel {
    class StaticModel;
    class StaticModelManager;
  }
}

namespace System {
  class nijiAllocator;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 * @class DebugStaticModel
 * @brief クラスの説明
 */
class DebugStaticModel
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugStaticModel);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugStaticModel( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugStaticModel( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase *pHeap,
    gfl2::fs::AsyncFileManager* pAsyncFileReadManager,
    Fieldmap* pFieldmap,
    MyRenderingPipeLine* pRenderingPipeLine,
    gfl2::str::MsgData* pMsgData
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  /**
   *  @brief  配置モデル作成
   */
  void CreateStaticModel();

  /**
   *  @brief  配置モデル動的作成
   */
  void CreateStaticModelDynamic();

  /**
   *  @brief  配置モデルワーク出力
   */
  void DumpStaticModelWorks();

  /**
   *  @brief  配置モデルワーク表示ON/OFF
   */
  void SetVisible( u32 index, bool isVisible);

  /**
   *  @brief  配置モデルワーク表示ON/OFF取得
   */
  bool IsVisible( u32 index);

  /**
   *  @brief  全配置モデルワーク表示ON/OFF
   */
  void SetVisibleAll( bool isVisible);

  /**
   *  @brief  DebugGroupが生成されたときに呼ぶ初期化
   */
  void InitializeCreateDebugFunc();

  /**
   *  @brief  DebugGroupが破棄されたときに呼ぶ破棄
   */
  void TerminateDestroyDebugFunc();

  /**
   *  @brief  リセット
   */
  void ResetVisible();

  //! ヒープの取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  //! Fieldmapの取得
  Fieldmap* GetFieldmap() const { return m_pFieldmap; };

public:

  gfl2::heap::HeapBase*             m_pHeap;
  System::nijiAllocator*            m_pAllocator;
  Fieldmap*                         m_pFieldmap;
  MyRenderingPipeLine*              m_pRenderingPipeLine;
  gfl2::str::MsgData*               m_pMsgData;
  gfl2::fs::AsyncFileManager*       m_pAsyncFileReadManager;
  StaticModel::StaticModelManager*  m_pStaticModelManager;
  StaticModel::StaticModel*         m_pStaticModel[ StaticModel::StaticModelManager::STATIC_MODEL_MAX ];
  void*                             m_pStaticModelPackData[ StaticModel::StaticModelManager::STATIC_MODEL_MAX ];

  s32                               m_staticModelID;
  gfl2::math::Vector3               m_pos;
  gfl2::math::Quaternion            m_rot;

  s32                               m_controlStaticModelID;
  bool                              m_isVisibleAll;
  bool                              m_isVisibleDefault[ StaticModel::StaticModelManager::STATIC_MODEL_MAX ];

private:

  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize);


  void** GetModelPackDataBuffer( u32* pOutWorkIndex);
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_STATIC_MODEL_H_INCLUDED__

#endif // PM_DEBUG

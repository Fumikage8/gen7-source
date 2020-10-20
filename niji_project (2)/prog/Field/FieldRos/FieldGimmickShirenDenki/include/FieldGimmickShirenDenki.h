//=================================================================================================
/**
*  @file FieldGimmickShirenDenki.h
*  @brief フィールドギミック 電気試練
*  @author ikuta_junya
*  @date 2016.02.17
*/
//=================================================================================================

#if !defined __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__
#define __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__
#pragma once

// niji
#include <model/include/gfl2_BaseModel.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include <niji_reference_files/script/FieldPawnTypes.h>
#include "Field/FieldRo/include/Collision/StaticActor.h"

// reference
//#include <niji_reference_files/script/FieldPawnTypes.h>

GFL_NAMESPACE_BEGIN( Field )


class FieldGimmickShirenDenkiCameraCallBack;

/**
 *  @class 電気試練ギミック
 */
class FieldGimmickShirenDenki : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickShirenDenki );

public:

  /**
   *  @brief コンストラクタ
   *  @param pHeap ヒープ
   *  @param pFieldmap フィールドマップ
   */
  FieldGimmickShirenDenki( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap );

  /**
   *  @brief デストラクタ
   */
  virtual ~FieldGimmickShirenDenki();

  /**
   *  @brief  生成
   */
  virtual void Create(void);
  virtual void UpdateCreate(void);
  virtual bool IsCreated(void);

  /**
 *  @brief  Ready中1度だけ呼び出し
 */
  virtual void Ready(void);

  /**
   *  @biref 破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief 更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void );


  poke_3d::model::BaseModel* GetModel(s32 index){ return m_pModel; };

private:

  /**
   *  @brief ギミックの状態
   */
  enum State
  {
    STATE_NONE,       ///< なし(動作していない)
    STATE_INIT,       ///< 初期化中
    STATE_RUNNIG,     ///< 動作中
    STATE_END,        ///< 終了

    STATE_MAX
  };

private:

  s32                                           m_InitializeState;
  s32                                           m_FinalizeState;

  // 外部オブジェクト
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;
  gfl2::fs::AsyncFileManager*                   m_pFileReadManager;

  // 内部オブジェクト
  System::nijiAllocator*                                          m_pAllocator;
  FieldGimmickShirenDenkiCameraCallBack*                          m_pCameraCallBack;

  // モデル
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pModelResource;
  poke_3d::model::BaseModel*                                      m_pModel;
  StaticActor*                                                    m_pModelActor;

  // アニメ
  gfl2::animation::AnimationPackList                              m_AnimPackList;
  
  // ロードリソース
  void*                                                           m_pResourceBuff;
  gfl2::fs::BinLinkerAccessor                                     m_BinLinker;
};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_SHIRENDENKI_H_INCLUDED__
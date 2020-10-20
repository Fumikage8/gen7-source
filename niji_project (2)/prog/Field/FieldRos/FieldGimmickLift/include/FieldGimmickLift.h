//=================================================================================================
/**
*  @file FieldGimmickLift.h
*  @brief フィールドギミック 昇降機
*  @author saita_kazuki
*  @date 2016.02.17
*/
//=================================================================================================

#if !defined __FIELD_GIMMICK_LIFT_H_INCLUDED__
#define __FIELD_GIMMICK_LIFT_H_INCLUDED__
#pragma once

// field
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

// reference
#include <niji_reference_files/script/FieldPawnTypes.h>


// 前方参照
namespace StaticModel {
  class StaticModel;
}
namespace MoveModel {
  class FieldMoveModel;
}


GFL_NAMESPACE_BEGIN( Field )

/**
 *  @class 昇降機ギミック
 */
class FieldGimmickLift : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickLift );

public:

  static const u32 REGIST_MOVE_MODEL_MAX = 5;   ///< 登録可能な動作モデル数

  /**
   *  @brief 登録パラメーター
   *  @note 登録後も保持する必要はない
   */
  struct RegistParam
  {
    GimmickLiftMode   mode;                                       ///< 動作モード
    u32               staticModelEventID;                         ///< 昇降機配置モデルのイベントID
    u32               moveModelEventID[ REGIST_MOVE_MODEL_MAX ];  ///< 登録する動作モデルのイベントID

    // コンストラクタ
    RegistParam()
      : mode( GIMMICK_LIFT_MODE_Y)
      , staticModelEventID( FIELD_EVENTID_NULL)
    {
      for( u32 i = 0; i < REGIST_MOVE_MODEL_MAX; ++i)
      {
        moveModelEventID[i] = FIELD_EVENTID_NULL;
      }
    }
  };

public:

  /**
   *  @brief コンストラクタ
   *  @param pHeap ヒープ
   *  @param pFieldmap フィールドマップ
   *  @param zoneId ゾーンID
   *  @param pPlacementDataManager 配置情報管理クラス
   */
  FieldGimmickLift( gfl2::heap::HeapBase* pHeap, Fieldmap *pFieldmap, u32 zoneId, PlacementDataManager* pPlacementDataManager );

  /**
   *  @brief デストラクタ
   */
  virtual ~FieldGimmickLift();

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

  /**
   *  @brief 登録
   *  @param param 登録パラメーター
   *  @note この関数を呼んだ後からギミックが動作します
   */
  void Regist( const RegistParam& param);

  /**
   *  @brief 登録解除
   */
  void UnRegist();

private:

  /**
   *  @brief ギミックの状態
   */
  enum State
  {
    STATE_NONE,       ///< なし(動作していない)
    STATE_RUNNIG,     ///< 動作中
    STATE_END,        ///< 終了

    STATE_MAX
  };

private:

  void Clear();

  void TraverseRegistMoveModel();

  gfl2::math::Vector3 GetStaticModelJointWorldPosition( StaticModel::StaticModel* pStaticModel, s32 jointIndex);
  poke_3d::model::BaseModel* GetBaseModelByStaticModel( StaticModel::StaticModel* pStaticModel);

  void ConvertModeSetPosition( gfl2::math::Vector3* pOutPos, const gfl2::math::Vector3& srcPos, poke_3d::model::BaseModel* pModel, const gfl2::math::Vector3& offset);

  void UpdateStateNone();
  void UpdateStateRunning();
  void UpdateStateEnd();

private:

  gfl2::heap::HeapBase*             m_pHeap;                                          ///< ヒープ
  PlacementDataManager*             m_pPlacementDataManager;                          ///< 配置情報管理クラス
  u32                               m_zoneID;                                         ///< 現在のゾーンID
  State                             m_state;                                          ///< ギミックの状態
  GimmickLiftMode                   m_mode;                                           ///< ギミックの動作モード
  StaticModel::StaticModel*         m_pStaticModel;                                   ///< 配置モデル
  s32                               m_staticModelJointIndex;                          ///< 配置モデルのジョイント番号
  MoveModel::FieldMoveModel*        m_pMoveModel[ REGIST_MOVE_MODEL_MAX ];            ///< 登録動作モデル
  BaseCollisionScene*               m_pCollisionSceneStash[ REGIST_MOVE_MODEL_MAX ];  ///< 登録動作モデルの地面コリジョンコンテナ退避用ワーク
  gfl2::math::Vector3               m_jointOffset[ REGIST_MOVE_MODEL_MAX ];           ///< ジョイントとのオフセット

};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_LIFT_H_INCLUDED__

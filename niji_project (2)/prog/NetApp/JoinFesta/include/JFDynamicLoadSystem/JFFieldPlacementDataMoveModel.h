//==============================================================================
/**
 * @file	JFFieldPlacementDataMoveModel.h
 * @brief	ジョインフェスタの動的動作モデル配置情報
 * @author	ichiraku_katsuhiko
 * @date	2015/06/03 18:32:46
 */
// =============================================================================
#if !defined( __JFFIELD_PLACEMENT_DATA_MOVEMODEL_H__ )
#define __JFFIELD_PLACEMENT_DATA_MOVEMODEL_H__
#pragma once

#include <model/include/gfl2_DressUpParam.h>
// 
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

class JFFieldPlacementDataMoveModel : public Field::FieldMoveModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( JFFieldPlacementDataMoveModel );

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  JFFieldPlacementDataMoveModel();
  // デストラクタ
  virtual ~JFFieldPlacementDataMoveModel(){};


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  /**
   *  @brief 話しかけコリジョン情報の取得
   */
  const virtual void * GetTalkCollisionData( const MoveModelData & data ) const;

  /**
   *  @brief 衝突用コリジョン情報の取得
   */
  virtual const void * GetCollisionData( const MoveModelData & data ) const;

  /**
   *  @brief 着せ替え用パラメータの取得
   *
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
   */
  virtual poke_3d::model::DressUpParam * GetDressupParam() const;
  
  /**
   *  @brief 着せ替え用リソースマネージャの取得
   *
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。現状ジョインフェスタしかないはず。 ichiraku
   */
  virtual poke_3d::model::DressUpModelResourceManager * GetDressUpModelResourceManager() const;
  
  /**
   *  @brief 着せ替えパラメータの設定
   *  @param  pDressUpParam    着せ替えパラメータ
   */
  void SetDressUpParam( poke_3d::model::DressUpParam* pDressUpParam );

  /**
   *  @brief 着せ替えリソースマネージャの設定
   *  @param  pDressUpModelResourceManager    着せ替えパラメータ
   */
  void SetDressUpModelResourceManager( poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager );

  /**
   *  @brief 施設店員の話しかけコリジョンオフセット
   *  @param  facilitiesRotationRadY  店員の回転Y
   *  @param  talkCollisionDistance   話しかけコリジョンオフセットずらす距離（中心から）
   */
  void SetFacilitiesTalkCollisionOffset( f32 facilitiesRotationRadY, f32 talkCollisionDistance );

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:
  u32 m_type[7]; // CollisionDataHeaderのデータ
  poke_3d::model::DressUpParam*                 m_pDressUpParam;                 //! 着せ替えパラメータ
  poke_3d::model::DressUpModelResourceManager*  m_pDressUpModelResourceManager;  //! 着せ替えリソースマネージャ

  // 施設店員の話しかけコリジョンオフセット用
  f32   m_facilitiesRotationRadY;     //! 店員の回転Y
  f32   m_talkCollisionDistance;      //! 話しかけコリジョンオフセットずらす距離（中心から）
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JFFIELD_PLACEMENT_DATA_MOVEMODEL_H__

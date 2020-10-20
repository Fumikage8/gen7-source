//==============================================================================
/**
 * @file	BattleInstPlacementDataMoveModel.h
 * @brief	バトル施設の動的動作モデル配置情報
 * @author	onoue_masayuki
 * @date	2016/01/27
 */
// =============================================================================
#if !defined( __BATTLEINSTPLACEMENTDATAMOVEMODEL_H__ )
#define __BATTLEINSTPLACEMENTDATAMOVEMODEL_H__
#pragma once

#include <model/include/gfl2_DressUpParam.h>
// 
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(BattleInst)

class BattleInstPlacementDataMoveModel : public Field::FieldMoveModelAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( BattleInstPlacementDataMoveModel );

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  BattleInstPlacementDataMoveModel();
  // デストラクタ
  virtual ~BattleInstPlacementDataMoveModel(){};


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
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。
   */
  virtual poke_3d::model::DressUpParam * GetDressupParam() const;
  
  /**
   *  @brief 着せ替え用リソースマネージャの取得
   *
   *  @note  動作モデルに着せ替えを適用させる場合に使用する。
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


  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:
  u32 m_type[7]; // CollisionDataHeaderのデータ
  poke_3d::model::DressUpParam*                 m_pDressUpParam;                 //! 着せ替えパラメータ
  poke_3d::model::DressUpModelResourceManager*  m_pDressUpModelResourceManager;  //! 着せ替えリソースマネージャ
};

GFL_NAMESPACE_END(BattleInst)

#endif // __BATTLEINSTPLACEMENTDATAMOVEMODEL_H__

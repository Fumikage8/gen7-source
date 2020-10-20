//==============================================================================
/**
 * @file	BattleFesModel.h
 * @brief	バトルフェスモデル  ※BattleInstから移植
 * @author	munakata_kai
 * @date	2016/11/26
 */
// =============================================================================

#ifndef __BATTLEFESMODEL_H__
#define __BATTLEFESMODEL_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <system/include/Date/gfl2_Date.h>
#include <model/include/gfl2_DressUpParam.h>
#include <model/include/gfl2_CharaModelFactory.h> // for DressUpModelResourceManager

// system
#include "System/include/nijiAllocator.h"

// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"

#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesPlacementDataMoveModel.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include "Field/FieldStatic/include/BattleFes/BattleFes.h"


GFL_NAMESPACE_BEGIN( BattleFes )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class BattleFesModel
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleFesModel);

public:
  // コンストラクタ
  BattleFesModel( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~BattleFesModel();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ
  */
  //-----------------------------------------------------------------------------
  void Setup(const MoveModelData* p_data);

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ完了チェック
  *  @retval  true:完了
  *  @retval  false:未完了
  */
  //-----------------------------------------------------------------------------
  bool IsSetupEnd() const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ完了チェック
  *  @param   isCheckHitPlayer    主人公との衝突判定をするかどうか(trueでする）
  *  @retval  true:完了
  *  @retval  false:未完了
  */
  //-----------------------------------------------------------------------------
  bool SetupUpdate( const bool isCheckHitPlayer );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルのセットアップ
  */
  //-----------------------------------------------------------------------------
  void SetupModel(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief   ロード完了待ち
  */
  //-----------------------------------------------------------------------------
  bool IsLoaded(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId);

  //-----------------------------------------------------------------------------
  /**
  *  @BRIEF   ロード開始
  */
  //-----------------------------------------------------------------------------
  void LoadStart(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager, const u32 characterId);

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデル破棄
  *  @return  trueで破棄完了
  */
  //-----------------------------------------------------------------------------
  bool Delete();

private:
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルデータセットアップ
  *  @param   [in]index           インデックス値 
  *  @param   [in]pos             表示位置
  *  @param   [in]sex             性別
  */
  //-----------------------------------------------------------------------------
  void setupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const u32 characterId );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルデータセットアップ
  *  @param   [in]pos             表示位置
  *  @param   [in]sex             性別
  */
  //-----------------------------------------------------------------------------
  void setupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const Savedata::MyStatus* pMyStatus );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   基本パラメータ設定
  */
  //-----------------------------------------------------------------------------
  void setupMoveModelDataDefault(const gfl2::math::Quaternion rot, const gfl2::math::Vector3 &pos, const u32 index);

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルアクターセットアップ
  */
  //-----------------------------------------------------------------------------
  void setupMoveModelActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公と衝突しているかチェック
  *  @retun   trueで衝突している
  */
  //-----------------------------------------------------------------------------
  bool isHitPlayer() const;

  Field::FieldMoveModelActor*                           mp_moveModelActor;
  Field::FieldMoveModelAccessor::MoveModelData          m_moveModelDataArray;           //! 動作モデル配置情報
  Field::MoveModel::FieldMoveModel*                     m_pRequestCharaModel;           //! 表示リクエスト中の動作モデル
  BattleFesPlacementDataMoveModel                       m_fieldPlacementDataMoveModel;  //! 動作モデル配置情報
  poke_3d::model::DressUpModelResourceManager           m_dressUpModelResourceManager;  //! 着せ替えリソースマネージャ
  System::nijiAllocator*                                m_pAllocator;                   //! アロケーター
  gfl2::heap::HeapBase*                                 m_pLocalHeapForModelResource;   //! モデルリソース用ローカルヒープ
  Field::MoveModel::ApplicationDataForNPC*              mp_CharaData;
  u8                                                    m_setupSeq;                     //! セットアップ時のシーケンス制御用
  u8                                                    m_deleteSeq;                    //! 削除時のシーケンス制御用
  u32                                                   m_subobjectId;                  // 0なら無効値
  const MoveModelData*                                  m_cpRequestData;
  bool                                                  m_IsSetuped;
  bool                                                  m_IsLoad;
  poke_3d::model::DressUpParam                          m_dressUpParam;                 //! ドレスアップパラメータ
};

GFL_NAMESPACE_END( BattleFes )

#endif // __BATTLEFESMODEL_H__

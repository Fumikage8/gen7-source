//==============================================================================
/**
 * @file	BattleInstModel.h
 * @brief	バトル施設モデル
 * @author	onoue_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#ifndef __BATTLEINSTMODEL_H__
#define __BATTLEINSTMODEL_H__
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

#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstPlacementDataMoveModel.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"


GFL_NAMESPACE_BEGIN(BattleInst)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class BattleInstModel
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleInstModel);

public:
  // コンストラクタ
  BattleInstModel( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~BattleInstModel();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ
  */
  //-----------------------------------------------------------------------------
  void Setup( BattleInst::MoveModelData* p_data );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ完了チェック
  *  @retval  true:完了
  *  @retval  false:未完了
  */
  //-----------------------------------------------------------------------------
  bool IsSetupEnd();

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルデータを更新する
  *  @retun   trueで衝突している
  */
  //-----------------------------------------------------------------------------
  void UpdateModelData( BattleInst::MoveModelData* p_data );

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
  void SetupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const u32 characterId );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルデータセットアップ
  *  @param   [in]pos             表示位置
  *  @param   [in]sex             性別
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelData( const u32 index, const gfl2::math::Vector3 pos, const gfl2::math::Quaternion rot, const Savedata::MyStatus* pMyStatus );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   基本パラメータ設定
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelDataDefault(const gfl2::math::Quaternion rot, const gfl2::math::Vector3 &pos, const u32 index);

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルアクターセットアップ
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公と衝突しているかチェック
  *  @retun   trueで衝突している
  */
  //-----------------------------------------------------------------------------
  bool IsHitPlayer() const;

private:
  Field::FieldMoveModelActor*                           mp_moveModelActor;
  Field::FieldMoveModelAccessor::MoveModelData          m_moveModelDataArray;           //! 動作モデル配置情報
  Field::MoveModel::FieldMoveModel*                     m_pRequestCharaModel;           //! 表示リクエスト中の動作モデル
  BattleInstPlacementDataMoveModel                      m_fieldPlacementDataMoveModel;  //! 動作モデル配置情報
  poke_3d::model::DressUpModelResourceManager           m_dressUpModelResourceManager;  //! 着せ替えリソースマネージャ
  System::nijiAllocator*                                m_pAllocator;                   //! アロケーター
  gfl2::heap::HeapBase*                                 mp_Heap;
  Field::MoveModel::ApplicationDataForNPC*              mp_CharaData;
  u8                                                    m_setupSeq;       //! セットアップ時のシーケンス制御用
  u8                                                    m_deleteSeq;      //! 削除時のシーケンス制御用
  u32 m_subobjectId; // 0なら無効値
  BattleInst::MoveModelData* m_pRequestData;
  bool m_IsSetuped;
  bool m_IsLoad;
  poke_3d::model::DressUpParam                          m_dressUpParam; //! ドレスアップパラメータ
};

GFL_NAMESPACE_END(BattleInst)

#endif // __BATTLEINSTMODEL_H__

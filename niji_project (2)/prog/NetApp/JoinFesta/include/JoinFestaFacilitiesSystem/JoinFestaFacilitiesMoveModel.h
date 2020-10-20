//==============================================================================
/**
 * @file	JoinFestaFacilitiesMoveModel.h
 * @brief	ジョインフェスタ施設の店員の動作モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/11/14 15:59:28
 */
// =============================================================================

#ifndef __JOINFESTA_FACILITIES_MOVE_MODEL_H__
#define __JOINFESTA_FACILITIES_MOVE_MODEL_H__
#pragma once

#include <macro/include/gfl2_macros.h>

// system
#include "System/include/nijiAllocator.h"

// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldMoveModelAccessor.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFFieldPlacementDataMoveModel.h"


// 前方宣言


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaFacilitiesMoveModel
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesMoveModel);

public:
  // コンストラクタ
  JoinFestaFacilitiesMoveModel( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~JoinFestaFacilitiesMoveModel();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ
  *  @param   [in]facilitiesIndex   施設インデックス
  *  @param   [in]facilitiesType    施設種類
  *  @param   [in]characterId       キャラクターID
  */
  //-----------------------------------------------------------------------------
  void Setup( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaScript::FacilitiesType facilitiesType, const u32 characterId  );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ完了チェック
  *  @retval  true:完了
  *  @retval  false:未完了
  */
  //-----------------------------------------------------------------------------
  bool IsSetup();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデル破棄（破棄完了が返った後でも呼んでもOK）
  *  @retval  true:破棄完了
  *  @retval  false:未完了
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
  *  @brief   動作モデルアクターセットアップ
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelActor();

private:
  //
  Field::FieldMoveModelActor                            m_moveModelActor;
  Field::FieldMoveModelAccessor::MoveModelData          m_moveModelDataArray;           //! 動作モデル配置情報
  Field::MoveModel::FieldMoveModel*                     m_pRequestCharaModel;           //! 表示リクエスト中の動作モデル
  JFFieldPlacementDataMoveModel                         m_fieldPlacementDataMoveModel;  //! 動作モデル配置情報

  u8                                                    m_setupSeq;       //! セットアップ時のシーケンス制御用
  u8                                                    m_deleteSeq;      //! 削除時のシーケンス制御用
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JOINFESTA_FACILITIES_MOVE_MODEL_H__

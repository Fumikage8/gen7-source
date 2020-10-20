//==============================================================================
/**
 * @file	BattleFesModelManager.h
 * @brief	バトルフェスモデルマネージャー  ※BattleInstから移植
 * @author	munakata_kai
 * @date	2016/11/26
 */
// =============================================================================

#ifndef __BATTLEFESMODELMANAGER_H__
#define __BATTLEFESMODELMANAGER_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesModel.h"
#include "Field/FieldStatic/include/BattleFes/BattleFes.h"

GFL_NAMESPACE_BEGIN( BattleFes )

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class BattleFesModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleFesModelManager);

public:
  // コンストラクタ
  BattleFesModelManager( gfl2::heap::HeapBase* pHeap );
  // デストラクタ
  virtual ~BattleFesModelManager();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   *  @brief 表示リクエスト
   *  @param MoveModelData              表示データ
   */
  //-----------------------------------------------------------------------------
  void Request(MoveModelData* p_data);

  //-----------------------------------------------------------------------------
  /**
  *  @brief 表示リクエスト(再読み込み)
  *  @param [in]pJFPersonalData  ジョインフェスタパーソナルデータ
  *  @param [in]jfPointIndex     JFPointのインデックス値
  *  @param [in]pos  表示位置
  */
  //-----------------------------------------------------------------------------
  void RequestReload(MoveModelData* p_data);

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルがセットアップ済みか確認
  *  @param modelIndex  モデルのインデックス
  *  @return trueで完了済み
  */
  //-----------------------------------------------------------------------------
  bool IsSetupEnd(const int modelIndex ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief 表示リクエストの対応処理完了したか
   *  @param   isCheckHitPlayer    主人公との衝突判定をするかどうか(trueでする）
   *  @return trueで完了
   */
  //-----------------------------------------------------------------------------
  bool IsRequestEnd(const bool isCheckHitPlayer);

  //-----------------------------------------------------------------------------
  /**
   *  @brief モデル削除
   *  @param index     インデックス
   *  @note  IsDeleteEndで削除完了確認する
   */
  //-----------------------------------------------------------------------------
  void RequestDelete(const u32 index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief モデル削除の対応処理完了したか
   *  @return trueで完了
   */
  //-----------------------------------------------------------------------------
  bool Delete();

  //-----------------------------------------------------------------------------
  /**
   *  @brief 破棄
   *  @retval true:破棄完了
   *  @retval false:破棄中
   */
  //-----------------------------------------------------------------------------
  bool Terminate();

private:
  BattleFesModel*               m_pRequestModel[BattleFesDefine::BFES_MODEL_DISP_MAX];       //! モデル
  u32                           m_requestModelIndex;                                         //! 表示リクエスト中のインデックス
  u32                           m_deleteModelIndex;                                          //! 削除リクエスト中のインデックス
};

GFL_NAMESPACE_END( BattleFes )

#endif // __BATTLEFESMODELMANAGER_H__

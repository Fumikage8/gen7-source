//==============================================================================
/**
 * @file	BattleInstModelManager.h
 * @brief	バトル施設モデルマネージャー
 * @author	onoeu_masayuki
 * @date	2016/01/27
 */
// =============================================================================

#ifndef __BATTLEINSTMODELMANAGER_H__
#define __BATTLEINSTMODELMANAGER_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/BattleInst/BattleInstModel.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

GFL_NAMESPACE_BEGIN(BattleInst)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class BattleInstModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleInstModelManager);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  BattleInstModelManager( gfl2::heap::HeapBase* pHeap,u32 useModelCount );
  // デストラクタ
  virtual ~BattleInstModelManager();


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
  void Request( BattleInst::MoveModelData* p_data );

  //-----------------------------------------------------------------------------
  /**
  *  @brief 表示リクエスト(再読み込み)
  *  @param [in]pJFPersonalData  ジョインフェスタパーソナルデータ
  *  @param [in]jfPointIndex     JFPointのインデックス値
  *  @param [in]pos  表示位置
  */
  //-----------------------------------------------------------------------------
  void RequestReload( BattleInst::MoveModelData* p_data );

  //-----------------------------------------------------------------------------
  /**
  *  @brief モデルがセットアップ済みか確認
  *  @param modelIndex  モデルのインデックス
  *  @return trueで完了済み
  */
  //-----------------------------------------------------------------------------
  bool IsSetupEnd( int modelIndex );

  //-----------------------------------------------------------------------------
  /**
   *  @brief 表示リクエストの対応処理完了したか
   *  @param   isCheckHitPlayer    主人公との衝突判定をするかどうか(trueでする）
   *  @return trueで完了
   */
  //-----------------------------------------------------------------------------
  bool IsRequestEnd( const bool isCheckHitPlayer );

  //-----------------------------------------------------------------------------
  /**
   *  @brief モデル削除
   *  @param index     インデックス
   *  @note  IsDeleteEndで削除完了確認する
   */
  //-----------------------------------------------------------------------------
  void Delete( const u32 index );

  //-----------------------------------------------------------------------------
  /**
   *  @brief モデル削除の対応処理完了したか
   *  @return trueで完了
   */
  //-----------------------------------------------------------------------------
  bool IsDeleteEnd();

  //-----------------------------------------------------------------------------
  /**
   *  @brief 破棄
   *  @retval true:破棄完了
   *  @retval false:破棄中
   */
  //-----------------------------------------------------------------------------
  bool Terminate();


  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルデータを更新する
  */
  //-----------------------------------------------------------------------------
  void UpdateModelData( BattleInst::MoveModelData* p_data );

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================

private:

  //! 表示する動作モデル最大数
  static const u32  MODEL_MAX = BattleInstDefine::BINST_MODEL_DISP_MAX;

  gfl2::heap::HeapBase*         m_pHeap;
  BattleInstModel*              m_pRequestModel[ MODEL_MAX ];       //! モデル
  gfl2::heap::HeapBase*         m_pLocalHeapForModel[ MODEL_MAX ];  //! モデル用ローカルヒープ
  //
  u32                           m_requestModelIndex;                //! 表示リクエスト中のインデックス
  u32                           m_deleteModelIndex;                 //! 削除リクエスト中のインデックス
  u32                           m_useModelCount;  //! 実際に表示されるモデル数
};

GFL_NAMESPACE_END(BattleInst)

#endif // __BATTLEINSTMODELMANAGER_H__

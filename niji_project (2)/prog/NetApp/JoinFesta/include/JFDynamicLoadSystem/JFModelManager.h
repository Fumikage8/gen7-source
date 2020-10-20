//==============================================================================
/**
 * @file	JFModelManager.h
 * @brief	ジョインフェスタモデルマネージャー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#ifndef __JF_MODEL_MANAGER_H__
#define __JF_MODEL_MANAGER_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>

#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"

// 前方宣言
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}
namespace NetApp{
  namespace JoinFesta{
    class JoinFestaEffectSystem; // エフェクトシステム
  }
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JFModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(JFModelManager);
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
  JFModelManager( gfl2::heap::HeapBase* pHeap, JoinFestaEffectSystem* pJoinFestaEffectSystem );
  // デストラクタ
  virtual ~JFModelManager();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   *  @brief 表示リクエスト
   *  @param [in]pJFPersonalData  ジョインフェスタパーソナルデータ
   *  @param [in]jfPointIndex     JFPointのインデックス値
   *  @param [in]pos              表示位置
   */
  //-----------------------------------------------------------------------------
  void Request( NetAppLib::JoinFesta::JoinFestaPersonalData* pJFPersonalData, const u32 jfPointIndex, const gfl2::math::Vector3 pos );

  //-----------------------------------------------------------------------------
  /**
   *  @brief 表示リクエストの対応処理完了したか
   *  @param   JfModelSetupMode    セットアップモード
   *  @return trueで完了
   */
  //-----------------------------------------------------------------------------
  bool IsRequestEnd( const JfModelSetupMode setupMode );

  //-----------------------------------------------------------------------------
  /**
   *  @brief モデル削除
   *  @param [in]jfPointIndex     JFPointのインデックス値
   *  @note  IsDeleteEndで削除完了確認する
   */
  //-----------------------------------------------------------------------------
  void Delete( const u32 jfPointIndex );

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

  //------------------------------------------------------------------
  /**
   *  @brief   トラバース直前の更新（処理負荷対策用）
   */
  //------------------------------------------------------------------
  void TraverseBefore();

  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
  *  @brief   JFパーソナルデータ取得
  *  @param   eventId 動作モデルのイベントID
  */
  //-----------------------------------------------------------------------------
  NetAppLib::JoinFesta::JoinFestaPersonalData* GetJoinFestaPersonalData( u32 eventId );

  //-----------------------------------------------------------------------------
  /**
   *  @brief   World座標の取得
   *  @param   eventId 動作モデルのイベントID
   */
  //-----------------------------------------------------------------------------
  const gfl2::math::Vector3* GetWorldPosition( u32 eventId );

  //-----------------------------------------------------------------------------
  /**
   *  @brief   WorldMatrixの取得
   *  @param   eventId 動作モデルのイベントID
   */
  //-----------------------------------------------------------------------------
  const gfl2::math::Matrix34* GetWorldMatrix( u32 eventId );


  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
   *  @brief   JFモデル取得
   *  @param   eventId 動作モデルのイベントID
   */
  //-----------------------------------------------------------------------------
  JFModel* GetJFModel( u32 eventId );

private:

  //! 表示する動作モデル最大数
  static const u32  MODEL_MAX = JoinFestaDefine::JFMODEL_DISP_MAX;

  //! 処理負荷対策用のトラバース更新を何フレームに1回やるか
  static const u32 TRAVERSE_UPDATE_FRAME = 3;

  gfl2::heap::HeapBase*         m_pHeap;
  JFModel*                      m_pRequestJFModel[ MODEL_MAX ];       //! JFモデル
  gfl2::heap::HeapBase*         m_pLocalHeapForJFModel[ MODEL_MAX ];  //! JFモデル用ローカルヒープ
  gfl2::heap::HeapBase*         m_pLocalHeapForDummyActor[ MODEL_MAX ];  //! 衝突判定用用ローカルヒープ
  //
  u32                           m_requestJfModelIndex;                //! 表示リクエスト中のインデックス
  u32                           m_deleteJfModelIndex;                 //! 削除リクエスト中のインデックス
  u8                            m_traverseResetCount;
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JF_MODEL_MANAGER_H__

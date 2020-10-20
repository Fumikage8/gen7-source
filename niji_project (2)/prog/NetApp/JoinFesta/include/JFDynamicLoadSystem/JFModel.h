//==============================================================================
/**
 * @file	JFModel.h
 * @brief	ジョインフェスタモデル
 * @author	ichiraku_katsuhiko
 * @date	2015/05/28 15:59:28
 */
// =============================================================================

#ifndef __JF_MODEL_H__
#define __JF_MODEL_H__
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

#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFFieldPlacementDataMoveModel.h"

#include "NetApp/JoinFesta/include/JoinFestaEffectSystem/JoinFestaEffectSystem.h"


// 前方宣言
namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 *  @enum JfModelSetupMode
 *        JF動作モデルのセットアップモード
 */
enum JfModelSetupMode{
  JF_MODEL_SETUP_NORMAL,    //! 通常時：エフェクト有り/主人公との当たり判定有り
  JF_MODEL_SETUP_NON_EFFECT,//! エフェクトなし:エフェクト無し/主人公との当たり判定有り　動作モデルの表示はする
  JF_MODEL_SETUP_END,       //! 終了時用:エフェクト無し/主人公との当たり判定無し　動作モデルの表示はなし
};


class JFModel : JoinFestaEffectSystem::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(JFModel);

public:
  // コンストラクタ
  JFModel( gfl2::heap::HeapBase* pDressUpHeap, gfl2::heap::HeapBase* pDummyActorHeap, JoinFestaEffectSystem* pJoinFestaEffectSystem );
  // デストラクタ
  virtual ~JFModel();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ
  *  @param   [in]pJFPersonalData ジョインフェスタパーソナルデータ
 *   @param   [in]jfPointIndex    JFPointのインデックス値
  *  @param   [in]pos             表示位置
  */
  //-----------------------------------------------------------------------------
  void Setup( NetAppLib::JoinFesta::JoinFestaPersonalData* pJFPersonalData, const u32 jfPointIndex, const gfl2::math::Vector3 pos );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデルセットアップ完了チェック
  *  @param   JfModelSetupMode    セットアップモード
  *  @retval  true:完了
  *  @retval  false:未完了
  */
  //-----------------------------------------------------------------------------
  bool IsSetup( const JfModelSetupMode mode );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   モデル破棄
  *  @return  trueで破棄完了
  */
  //-----------------------------------------------------------------------------
  bool Delete();

  //-----------------------------------------------------------------------------
  /**
   *  @brief   トラバース更新チェックをする必要があるかどうか
   *  @retval   true:チェック必要
   *  @retval   false:チェック不必要
   */
  //-----------------------------------------------------------------------------
  bool IsTraverseCheckNeed();

  //-----------------------------------------------------------------------------
  /**
   *  @brief   強制でトラバース更新をするかどうか
   *  @retval   true:する
   *  @retval   false:しない
   */
  //-----------------------------------------------------------------------------
  bool IsForceTraverseUpdate();

  //-----------------------------------------------------------------------------
  /**
   *  @brief   トラーバス更新の設定
   *  @param   isTraverseUpdate true:トラバース更新処理する false:トラバース更新処理しない
   */
  //-----------------------------------------------------------------------------
  void SetTraverseUpdate( const bool isTraverseUpdate );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   JFパーソナルデータ取得
  *  @return  NetAppLib::JoinFesta::JoinFestaPersonalData
  */
  //-----------------------------------------------------------------------------
  NetAppLib::JoinFesta::JoinFestaPersonalData* GetJoinFestaPersonalData(){ return m_pJFPersonalData;  }

  //-----------------------------------------------------------------------------
  /**
  *  @brief   JFポイントのインデックス値取得
  *  @return  JFPoint::INDEX_NULLの場合はモデル非表示状態
  */
  //-----------------------------------------------------------------------------
  u32 GetJFPointIndex() const{ return m_jfPointIndex; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief   World座標の取得
  */
  //-----------------------------------------------------------------------------
  const gfl2::math::Vector3* GetWorldPosition();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   WorldMatrixの取得
  */
  //-----------------------------------------------------------------------------
  const gfl2::math::Matrix34* GetWorldMatrix();

  //-----------------------------------------------------------------------------
  /**
   *  @brief   出現エフェクトのイベントハンドラ
   */
  //-----------------------------------------------------------------------------
  virtual void JoinFestaEffectSystem_OnEvent( JoinFestaEffectSystem* pCaller, const JoinFestaEffectSystem::IEventHandler::EventID eventID );
  
private:
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルデータセットアップ
  *  @param   [in]jfPointIndex    JFPointのインデックス値
  *  @param   [in]pos             表示位置
  *  @param   [in]sex             性別
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelData( const u32 jfPointIndex, const gfl2::math::Vector3 pos, const u32 sex );

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデルアクターセットアップ
  */
  //-----------------------------------------------------------------------------
  void SetupMoveModelActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   動作モデル表示
  */
  //-----------------------------------------------------------------------------
  void MoveModelVisibleOn();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公との衝突判定するダミーアクターを追加
  */
  //-----------------------------------------------------------------------------
  void RegistDummyStaticActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公との衝突判定するダミーアクターを削除
  */
  //-----------------------------------------------------------------------------
  void UnregistDummyStaticActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公との衝突判定するダミーアクターのコリジョンシステムにデータ適用
  */
  //-----------------------------------------------------------------------------
  void ApplyMoveModelToActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief   主人公と衝突しているかチェック
  *  @retun   trueで衝突している
  */
  //-----------------------------------------------------------------------------
  bool IsHitPlayer() const;

private:

  /*
   * @enum SETUP_SEQ
   *  更新シーケンス
   */
  enum SETUP_SEQ{
    UPDATE_SEQ_SAFE_POS_CHECK,    //!< 配置しても安全な場所かチェック
    UPDATE_SEQ_DRESSUP_LOAD,      //!< 着せ替えパーツの読み込み開始
    UPDATE_SEQ_DRESSUP_LOAD_WAIT, //!< 着せ替えパーツの読み込み完了待ち
    UPDATE_SEQ_DRESSUP_SETUP,     //!< 動作モデルセットアップ
    UPDATE_SEQ_DRESSUP_SETUP_WAIT,//!< 動作モデルセットアップ完了待ち
    UPDATE_SEQ_DRESSUP_EFFECT_WAIT,//!< 出現エフェクト終了待ち
    UPDATE_SEQ_END,               //!< 終了
  };


private:
  JoinFestaEffectSystem*                                m_pJoinFestaEffectSystem;       //! エフェクトシステム
  //
  NetAppLib::JoinFesta::JoinFestaPersonalData*          m_pJFPersonalData;              //! JFパーソナルデータ
  //
  Field::FieldMoveModelActor                            m_moveModelActor;
  Field::FieldMoveModelAccessor::MoveModelData          m_moveModelDataArray;           //! 動作モデル配置情報
  Field::MoveModel::FieldMoveModel*                     m_pRequestCharaModel;           //! 表示リクエスト中の動作モデル
  JFFieldPlacementDataMoveModel                         m_fieldPlacementDataMoveModel;  //! 動作モデル配置情報
  poke_3d::model::DressUpParam                          m_dressUpParam;                 //! ドレスアップパラメータ
  poke_3d::model::DressUpModelResourceManager           m_dressUpModelResourceManager;  //! 着せ替えリソースマネージャ
  System::nijiAllocator*                                m_pDressUpAllocator;            //! 着せ替えアロケーター
  gfl2::heap::HeapBase*                                 m_pDummyActorHeap;              //! 衝突判定用ヒープ
  System::nijiAllocator*                                m_pDummyActorAllocator;         //! 衝突判定用アロケーター

  StaticActor*                                          m_pStaticActor;   //! モデル表示までの壁として

  u32                                                   m_jfPointIndex;   //! 表示されているポイントのインデックス
  SETUP_SEQ                                             m_setupSeq;       //! セットアップ時のシーケンス制御用
  u8                                                    m_deleteSeq;      //! 削除時のシーケンス制御用
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JF_MODEL_H__

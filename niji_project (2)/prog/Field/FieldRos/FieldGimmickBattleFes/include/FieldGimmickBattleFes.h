//==============================================================================
/**
 * @file	FieldGimmickBattleFes.h
 * @brief	フィールドギミック：バトルフェス  ※BattleInstから移植
 * @author	munakata_kai
 * @date	2016/11/26
 */
// =============================================================================

#if !defined(__FIELDGIMMICKBATTLEFES_H__)
#define __FIELDGIMMICKBATTLEFES_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "AppLib/include/Util/app_util_heap.h"
#include <ro/include/gfl2_RoManager.h> // Ro

// FieldGimmick
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"

#include <gfx/include/gfl2_Texture.h>

GFL_NAMESPACE_BEGIN( BattleFes )
  class BattleFesModelManager;
  class BattleFesMsgResourceManager;
GFL_NAMESPACE_END( BattleFes )

GFL_NAMESPACE_BEGIN( Field )

/// バトルフェスギミック
class FieldGimmickBattleFes : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickBattleFes );

public:
  // コンストラクタ
  FieldGimmickBattleFes(gfl2::heap::HeapBase* pHeap, Fieldmap *pMap);
  // デストラクタ
  virtual ~FieldGimmickBattleFes();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  /**
  *  @brief  生成
  */
  virtual void Create();
  virtual void UpdateCreate();
  virtual bool IsCreated();
  /**
  *  @biref  破棄更新
  *  @return 破棄が完了したかどうか
  *
  *  破棄が完了するまでマイフレーム呼出がなされる関数
  */
  virtual bool Finalize();

  /**
  *  @brief  破棄
  */
  virtual void Delete();

  /**
  *  @brief  Ready中1度だけ呼び出し
  */
  virtual void Ready();

  /**
  *  @brief  更新
  *  Fieldmap::Updateにて呼出
  *  キャラクタ前更新よりも手前で呼び出される
  */
  virtual void UpdateFirst();
  /**
  *  @brief  更新
  *  Fieldmap::Updateにて呼出
  *  UpdateNormalが終了してから呼び出される
  */
  virtual void UpdateLast();
  /**
  *  @brief  更新
  *  Fieldmap::UpdateNormalにて呼び出される
  */
  virtual void CalculateAfter();

  /**
  *  @brief  特殊ステップチェック
  */
  virtual bool MoveRequestCheck(const EventCheckRequest *cpReq);

  /**
  *  @brief  イベント起動チェック
  */
  virtual bool EventRequestCheck(const EventCheckRequest *cpReq);

  virtual void TransferTexture(gfl2::gfx::CtrDisplayNo displayNo);
  
  //=============================================================================
  //! 動作モデル動的ロードシステム取得
  BattleFes::BattleFesModelManager* GetModelManager() const { return m_pModelManager; }
  //! リソースマネージャ取得
  BattleFes::BattleFesMsgResourceManager* GetBattleFesMsgResourceManager() const { return m_pResourceManager; }

  /**
  *  @biref  ギミックをオープンする
  *  @return 初期化処理が不要な場合にtrue
  */
  bool OpenGimmick();

  /**
  *  @biref  ギミックをクローズする
  *  @return 完了でtrue
  */
  bool CloseGimmick();

private:
  gfl2::heap::HeapBase*                     m_pFieldDeviceHeap;
  BattleFes::BattleFesModelManager*         m_pModelManager;        ///< バトルフェスモデルマネージャー
  BattleFes::BattleFesMsgResourceManager*   m_pResourceManager;     ///< バトルフェスメッセージリソースマネージャー
  gfl2::ro::Module*                         m_pJoinFestaRoModule;   ///< ジョインフェスタDLL 解放の為に保存
  gfl2::ro::Module*                         m_pKisekaeDataRoModule; ///< きせかえDLL 解放の為に保存（バトルフェスには不要だがジョインフェスタDLLに必要なため）
  app::util::Heap                           m_ResourceManagerHeap;  ///< リソースマネージャ用Heap（参照Create）

  /**
   * @enum INITIALIZE_SEQ
   *  初期化シーケンス
   */
  enum INITIALIZE_SEQ{
    INITIALIZE_SEQ_START,                   //! 初期化最初

    INITIALIZE_SEQ_CHARA_LOAD_1,            //! キャラクターリソースロード
    INITIALIZE_SEQ_CHARA_WAIT_1,            //! キャラクターリソースロード待ち
    INITIALIZE_SEQ_CHARA_LOAD_2,            //! キャラクターリソースロード
    INITIALIZE_SEQ_CHARA_WAIT_2,            //! キャラクターリソースロード待ち

    INITIALIZE_SEQ_END,                     //! 初期化完了
  };


  /**
   * @enum FINALIZE_SEQ
   *  終了処理シーケンス
   */
  enum FINALIZE_SEQ{
    FINALIZE_SEQ_TERMINATE,             //! 終了処理
    FINALIZE_SEQ_END,                   //! 終了処理完了
  };

  enum GimmickStatus{
    Closed,
    Setup,
    Open,
    Closing,
  };

  INITIALIZE_SEQ  m_initializeSeq;
  FINALIZE_SEQ    m_finalizeSeq;

  GimmickStatus m_status;

  /**
  *  @biref  ステータスをセットする
  */
  void setStatus(GimmickStatus status);
};

GFL_NAMESPACE_END( Field )

#endif // __FIELDGIMMICKBATTLEFES_H__

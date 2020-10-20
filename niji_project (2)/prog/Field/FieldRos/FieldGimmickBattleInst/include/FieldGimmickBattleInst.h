//==============================================================================
/**
 * @file	FieldGimmickBattleInst.h
 * @brief	フィールドギミック：バトル施設
 * @author	onoue_masayuki
 * @date	2015/01/28
 */
// =============================================================================

#if !defined(__FIELDGIMMICKBATTLEINST_H__)
#define __FIELDGIMMICKBATTLEINST_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "AppLib/include/Util/app_util_heap.h"

// FieldGimmick
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"

//-----------------------------------------------------------------------------
namespace gfl2{ namespace debug{  class DebugWinGroup;  }}

namespace BattleInst{
  class BattleInstModelManager;
}
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN( Field )

/// バトル施設用ギミック
class FieldGimmickBattleInst : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickBattleInst );

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  FieldGimmickBattleInst( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap, u32 zoneId );
  // デストラクタ
  virtual ~FieldGimmickBattleInst();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================

  /**
   *  @brief  生成
   */
  virtual void Create( void );
  virtual void UpdateCreate( void );
  virtual bool IsCreated( void );
  /**
   *  @biref  破棄更新
   *  @return 破棄が完了したかどうか
   *
   *  破棄が完了するまでマイフレーム呼出がなされる関数
   */
  virtual bool Finalize( void );

  /**
   *  @brief  破棄
   */
  virtual void Delete( void );

  /**
   *  @brief  Ready中1度だけ呼び出し
   */
  virtual void Ready( void );
  
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  キャラクタ前更新よりも手前で呼び出される
   */
  virtual void UpdateFirst( void );
  /**
   *  @brief  更新
   *  Fieldmap::Updateにて呼出
   *  UpdateNormalが終了してから呼び出される
   */
  virtual void UpdateLast( void );
  /**
   *  @brief  更新
   *  Fieldmap::UpdateNormalにて呼び出される
   */
  virtual void CalculateAfter( void );

  /**
   *  @brief  特殊ステップチェック
   */
  virtual bool MoveRequestCheck( const EventCheckRequest *cpReq );

  /**
   *  @brief  イベント起動チェック
   */
  virtual bool EventRequestCheck( const EventCheckRequest *cpReq );
  
  //=============================================================================
  //! 動作モデル動的ロードシステム取得
  BattleInst::BattleInstModelManager* GetModelManager() const { return m_pModelManager; }

  /**
  *  @biref  ギミックをオープンする
  *  @return 初期化処理が不要な場合にtrue
  */
  bool OpenGimmick( void );

  /**
  *  @biref  ギミックをクローズする
  *  @return 完了でTreu
  */
  bool CloseGimmick( void );

private:
  gfl2::heap::HeapBase*                     m_pFieldDeviceHeap;
  BattleInst::BattleInstModelManager*       m_pModelManager; ///< バトル施設モデルマネージャー

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

    INITIALIZE_SEQ_RECOVER_START,           //! モデル復帰
    INITIALIZE_SEQ_RECOVER_LOAD_WAIT,       //! モデル復帰待ち

    INITIALIZE_SEQ_END,                     //! 初期化完了
  };


  /**
   * @enum FINALIZE_SEQ
   *  初期化シーケンス
   */
  enum FINALIZE_SEQ{
    FINALIZE_SEQ_SAVE_TRANSFORM,        //! キャラクターリソースロード待ち
    FINALIZE_SEQ_TERMINATE,             //! 初期化完了
    FINALIZE_SEQ_END,                   //! 初期化完了
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
  u32 m_zoneID;


private:

  /**
  *  @biref  ステータスをセットする
  */
  void SetStatus(GimmickStatus status);

  bool IsExistBattleInst();
};

GFL_NAMESPACE_END( Field )

#endif // __FIELDGIMMICKBATTLEINST_H__

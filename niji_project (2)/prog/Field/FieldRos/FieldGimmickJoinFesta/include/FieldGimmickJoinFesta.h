//==============================================================================
/**
 * @file	FieldGimmickJoinFesta.h
 * @brief	フィールドギミック：ジョインフェスタ
 * @author	ichiraku_katsuhiko
 * @date	2015/06/09 13:09:29
 */
// =============================================================================

#if !defined(__FIELD_GIMMICK_JOINFESTA_H__)
#define __FIELD_GIMMICK_JOINFESTA_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "AppLib/include/Util/app_util_heap.h"

// FieldGimmick
#include "Field/FieldRo/include/Gimmick/FieldGimmick.h"

// JoinFestaDefine
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"


//-----------------------------------------------------------------------------
namespace gfl2{ namespace debug{  class DebugWinGroup;  }}

namespace NetApp{
  namespace JoinFesta{
    class JFDynamicLoadSystem;
    class JoinFestaEffectSystem;
    class JoinFestaResourceManager;
    class JoinFestaFacilitiesSystem;
    class JoinFestaDebug;
    class JoinFestaFacilitiesSystemDebug;
  }
}
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN( Field )

/// ジョインフェスタ用ギミック
class FieldGimmickJoinFesta : public FieldGimmick
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickJoinFesta );

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  FieldGimmickJoinFesta( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap );
  // デストラクタ
  virtual ~FieldGimmickJoinFesta();

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
   *  トラバース直前に呼び出される
   */
  virtual void TraverseBefore( void );
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
  NetApp::JoinFesta::JFDynamicLoadSystem* GetJFDynamicLoadSystem() const { return m_pJFDynamicLoadSystem; }
  //! 施設システム取得
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* GetJoinFestaFacilitiesSystem() const { return m_pFacilitiesSystem; }
  //! エフェクトシステム取得
  NetApp::JoinFesta::JoinFestaEffectSystem* GetJoinFestaEffectSystem() const { return m_pEffectSystem; }
  //! リソースマネージャ取得
  NetApp::JoinFesta::JoinFestaResourceManager* GetJoinFestaResourceManager() const { return m_pResourceManager; }


#if PM_DEBUG
  /** JoinFesta関連デバッグの最上位グループを取得    */
  gfl2::debug::DebugWinGroup* GetDebugGroup(void) const {return m_pJoinFestaDebugGroup;}
#endif

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   Updateで初期化
   * @return  初期化完了でtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool InitializeInUpdate();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   キャラクターリソースのセットアップ（ロード）
   * @return  セットアップ完了でtrue
   */
  /* ----------------------------------------------------------------------------------------- */
  bool CharaModelResourceSetup();

private:
  gfl2::heap::HeapBase*                         m_pFieldDeviceHeap;
  NetApp::JoinFesta::JFDynamicLoadSystem*       m_pJFDynamicLoadSystem; ///< ジョインフェスタ動的ロードシステム
  NetApp::JoinFesta::JoinFestaEffectSystem*     m_pEffectSystem;        ///< ジョインフェスタエフェクトシステム
  NetApp::JoinFesta::JoinFestaResourceManager*  m_pResourceManager;     ///< ジョインフェスタリソースマネージャ
  NetApp::JoinFesta::JoinFestaFacilitiesSystem* m_pFacilitiesSystem;  ///< ジョインフェスタ施設システム
  gfl2::ro::Module*                             m_pJoinFestaRoModule;   ///< ジョインフェスタDLL 解放の為に保存
  gfl2::ro::Module*                             m_pKisekaeDataRoModule; ///<  ジョインフェスタで使うきせかえデータのDLL 解放の為に保存
  app::util::Heap                               m_ResourceManagerHeap;  ///< リソースマネージャ用Heap（参照Create）

  /**
   * @enum INITIALIZE_SEQ
   *  初期化シーケンス
   */
  enum INITIALIZE_SEQ{
    INITIALIZE_SEQ_START,                   //! 初期化最初

    INITIALIZE_SEQ_RESOURCE_LOAD,           //! リソース読み込み待ち
    INITIALIZE_SEQ_MODULE_INIT_1,           //! 各システム初期化開始
    INITIALIZE_SEQ_MODULE_WAIT_1,           //! 各システム初期化待ち
    INITIALIZE_SEQ_MODULE_INIT_2,           //! 各システム初期化開始
    INITIALIZE_SEQ_MODULE_WAIT_2,           //! 各システム初期化待ち

    INITIALIZE_SEQ_END,                     //! 初期化完了
  };

  INITIALIZE_SEQ  m_initializeSeq;

  /**
   * @enum UPDATE_INITIALIZE_SEQ
   *  更新での初期化シーケンス
   */
  enum UPDATE_INITIALIZE_SEQ{
    UPDATE_INITIALIZE_SEQ_START,                   //! 初期化最初
    UPDATE_INITIALIZE_SEQ_MODULE_INIT_1,           //! 各システム初期化開始
    UPDATE_INITIALIZE_SEQ_MODULE_WAIT_1,           //! 各システム初期化待ち
    UPDATE_INITIALIZE_SEQ_END,                     //! 初期化完了
  };

  UPDATE_INITIALIZE_SEQ  m_updateInitializeSeq;

  /**
   * @enum CHARA_SETUP_SEQ
   *   キャラクターモデルリソースセットアップシーケンス
   */
  enum CHARA_SETUP_SEQ{
    CHARA_SETUP_SEQ_LOAD_1,            //! キャラクターリソースロード
    CHARA_SETUP_SEQ_WAIT_1,            //! キャラクターリソースロード待ち
    CHARA_SETUP_SEQ_LOAD_2,            //! キャラクターリソースロード
    CHARA_SETUP_SEQ_WAIT_2,            //! キャラクターリソースロード待ち
    CHARA_SETUP_SEQ_END,               //! 初期化完了
  };
  CHARA_SETUP_SEQ m_charSetupSeq;


#if PM_DEBUG
  gfl2::debug::DebugWinGroup*                   m_pJoinFestaDebugGroup; ///<  JoinFesta関連デバッグの最上位グループ
#endif

#ifdef JF_DEBUG_MENU
private:
  void DebugMenu_Init();
  void DebugMenu_Exit();
  NetApp::JoinFesta::JoinFestaDebug*  m_pDebugMenu;
  NetApp::JoinFesta::JoinFestaFacilitiesSystemDebug*  m_pFacilitiesDebug;
#endif // JF_DEBUG_MENU

};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_JOINFESTA_H__

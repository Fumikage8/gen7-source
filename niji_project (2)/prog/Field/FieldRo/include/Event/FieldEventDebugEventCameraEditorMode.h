//======================================================================
/**
* @file FieldEventDebugEventCameraEditorMode.h
* @brief キャラエディターの動作モード
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#if !defined( __FIELD_EVENT_DEBUG_EVENT_CAMERA_EDITOR_MODE_H_INCLUDED__)
#define __FIELD_EVENT_DEBUG_EVENT_CAMERA_EDITOR_MODE_H_INCLUDED__
#pragma once

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// field script
#include "FieldScript/include/ScriptDebug.h"
#include "FieldScript/include/ScriptWork.h"

// field camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeEventScript.h"

// 前方参照
namespace Field {
  class Fieldmap;
  namespace MoveModel {
    class FieldMoveModel;
  }
  namespace FieldScript {
    class FieldScriptSystem;
  }
}

namespace Field {
  class DebugEventCameraEditorModeBase;
  class DebugEventCameraEditorModeManager;

  namespace Camera {
    namespace Controller {
      class ControllerBase;
      class ControllerTypeAdjustPosition;
      class ControllerTypeEventScript;
    }
  }
}


GFL_NAMESPACE_BEGIN( Field )


//------------------------------------------------------------------------------
/**
 * @class DebugEventCameraEditorCommonWork
 * @brief 動作モード共通ワーク
 */
//------------------------------------------------------------------------------
class DebugEventCameraEditorCommonWork
{
public:

  struct FrameWork
  {
    FrameWork()
      : pos( gfl2::math::Vector3::GetZero())
      , target( gfl2::math::Vector3::GetZero())
      , fovy( 0.0f)
      , bank( 0.0f)
    {
      rotation = gfl2::math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f);
    }

    gfl2::math::Vector3     pos;
    gfl2::math::Vector3     target;
    gfl2::math::Quaternion  rotation;
    f32                     fovy;
    f32                     bank;
  };

  struct ScriptCameraControllerParam
  {
    Field::Camera::Controller::ControllerTypeEventScript::TARGET_PARAM        moveTargetStart;
    Field::Camera::Controller::ControllerTypeEventScript::TARGET_PARAM        moveTargetEnd;
    Field::Camera::Controller::ControllerTypeEventScript::ActionState::Value  actionState;
    f32                                                                       bank;
    u32                                                                       moveType;
    u32                                                                       moveEndFrame;
    u32                                                                       moveNowFrame;
    gfl2::math::Easing::EaseFunc                                              easingFunc;
  };

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugEventCameraEditorCommonWork();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugEventCameraEditorCommonWork();

public:

  gfl2::math::Vector3           defaultPos;
  gfl2::math::Vector3           defaultTarget;
  gfl2::math::Quaternion        defaultRotation;
                               
  FrameWork                     firstFrameWork;
  FrameWork                     lastFrameWork;
                               
  s32                           easingType;
  u32                           frame;
  bool                          isLoop;

  ScriptCameraControllerParam   scriptCameraControllerParam;

  bool                          isTalkWindowVisibleDefault[ FIELDTALKWINDOW_MAX ];    ///< 会話ウインドウ表示状態
};


//------------------------------------------------------------------------------
/**
 * @class DebugEventCameraEditorModeManager
 * @brief 動作モード管理
 */
//------------------------------------------------------------------------------
class DebugEventCameraEditorModeManager
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugEventCameraEditorModeManager );

public:

  /**
   * @brief 状態
   */ 
  enum State
  {
    STATE_NONE,       ///< なし
    STATE_CONTINUE,   ///< 継続中
    STATE_END,        ///< 終了
  };

  /**
   * @brief 動作モード
   */ 
  enum Mode
  {
    MODE_EDIT,        ///< リスト表示項目操作
    MODE_FRAMING,     ///< フレーミング操作

    MODE_MAX,
  };

  /**
   * @brief 初期化情報
   */
  struct InitializeDescription
  {
    gfl2::heap::HeapBase*             pHeap;
    gfl2::str::MsgData*               pMsgData;
    Fieldmap*                         pFieldmap;
    FieldScript::FieldScriptSystem*   pScriptSystem;
  };

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugEventCameraEditorModeManager();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugEventCameraEditorModeManager();

  /**
   * @brief 初期化
   * @param desc 初期化設定
   */ 
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief 破棄
   */ 
  void Terminate();

  /**
   * @brief 更新
   * @return 更新後の状態
   */ 
  State Update( gfl2::util::DrawUtilText* pDrawText );

  /**
   * @brief セットアップ
   */ 
  void Setup( Mode mode, gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief モード取得
   * @param mode モード
   * @return 指定したモード
   */ 
  DebugEventCameraEditorModeBase* GetMode( Mode mode) const ;

  /**
   * @brief 共通ワーク取得
   * @return 共通ワーク(書き換え可)
   */ 
  DebugEventCameraEditorCommonWork* GetCommonWork();

  /**
   * @brief 会話ウインドウの表示状態を保存
   */ 
  void SaveTalkWindowVisible();

  /**
   * @brief 会話ウインドウの表示状態変更
   */ 
  void SetTalkWindowVisible( bool isVisible);

  /**
   * @brief 会話ウインドウの表示状態を保存した状態にする
   */ 
  void ResetTalkWindowVisible();

  /**
   * @brief フィールドマップ取得
   * @return フィールドマップ
   */ 
  Fieldmap* GetFieldmap() const { return m_pFieldmap; }

  /**
   * @brief スクリプトシステム取得
   * @return スクリプトシステム
   */ 
  FieldScript::FieldScriptSystem* GetScriptSystem() const { return m_pScriptSystem; }


  static s32 m_IsDrawTextInfo;          // true 情報表示を表示する

private:

  DebugEventCameraEditorModeBase*       m_pModeTable[ MODE_MAX ];

  gfl2::heap::HeapBase*                 m_pHeap;
  gfl2::debug::DebugWinGroup*           m_pDebugGroup;
  gfl2::str::MsgData*                   m_pMsgData;
  Fieldmap*                             m_pFieldmap;
  FieldScript::FieldScriptSystem*       m_pScriptSystem;
  State                                 m_state;
  DebugEventCameraEditorModeBase*       m_pCurrentMode;
  DebugEventCameraEditorCommonWork      m_commonWork;

};


//------------------------------------------------------------------------------
/**
 * @class DebugEventCameraEditorModeBase
 * @brief 動作モード基底
 */
//------------------------------------------------------------------------------
class DebugEventCameraEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugEventCameraEditorModeBase );

public:

  /**
   * @brief 状態
   */ 
  enum State
  {
    STATE_NONE,       ///< なし
    STATE_CONTINUE,   ///< 継続中
    STATE_END,        ///< 終了
  };

  /**
   * @brief 初期化情報
   */
  struct InitializeDescription
  {
    gfl2::heap::HeapBase*               pHeap;
    gfl2::debug::DebugWinGroup*         pDebugGroup;
    gfl2::str::MsgData*                 pMsgData;
    Fieldmap*                           pFieldmap;
    DebugEventCameraEditorModeManager*  pManager;
  };

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugEventCameraEditorModeBase();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugEventCameraEditorModeBase();

  /**
   * @brief 初期化
   * @param desc 初期化設定
   */ 
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief 破棄
   */ 
  virtual void Terminate() = 0 ;

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup) = 0 ;

  /**
   * @brief クリーン
   */ 
  virtual void Clean() = 0 ;

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual State Update( DebugEventCameraEditorModeManager* pManager) = 0 ;

  /**
  * @brief 情報表示 
  */ 
  virtual void DrawTextInfo( gfl2::util::DrawUtilText* pDrawText ){}

  /**
   * @brief モード管理インスタンス取得
   */
  DebugEventCameraEditorModeManager* GetModeManager() { return m_pManager; }

protected:

  virtual void initialize() = 0 ;


protected:

  gfl2::heap::HeapBase*                 m_pHeap;
  gfl2::debug::DebugWinGroup*           m_pDebugGroup;
  gfl2::str::MsgData*                   m_pMsgData;
  Fieldmap*                             m_pFieldmap;
  DebugEventCameraEditorModeManager*    m_pManager;
};


//------------------------------------------------------------------------------
/**
 * @class DebugEventCameraEditorModeEdit
 * @brief モデルリスト表示動作モード
 */
//------------------------------------------------------------------------------
class DebugEventCameraEditorModeEdit : public DebugEventCameraEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugEventCameraEditorModeEdit );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugEventCameraEditorModeEdit();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugEventCameraEditorModeEdit();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugEventCameraEditorModeBase::State Update( DebugEventCameraEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

  /**
   * @brief ログ出力
   */
  void PrintFirstFrame();

  /**
   * @brief ログ出力
   */
  void PrintMotion();

  /**
   * @brief カメラモーション再生
   */
  void PlayCameraMotion();

  /**
   * @brief カメラモーション停止
   */
  void StopCameraMotion();

  /**
   * @brief スクリプトカメラコントローラー取得
   */
  Field::Camera::Controller::ControllerTypeEventScript* GetEventScriptCameraController() const { return m_pCameraControllerEventScript; }

protected:

  virtual void initialize();

protected:

  Field::Camera::Controller::ControllerTypeEventScript*           m_pCameraControllerEventScript;                         ///< スクリプト用カメラ機能
  bool                                                            m_isMoving;                                             ///< 再生中か
  DebugEventCameraEditorCommonWork::ScriptCameraControllerParam   m_ScriptCameraControllerParam_PlayBefore;               ///< 再生前のスクリプトカメラコントローラーパラメーター

};


//------------------------------------------------------------------------------
/**
 * @class DebugEventCameraEditorModeFraming
 * @brief 配置操作動作モード
 */
//------------------------------------------------------------------------------
class DebugEventCameraEditorModeFraming : public DebugEventCameraEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugEventCameraEditorModeFraming );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugEventCameraEditorModeFraming();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugEventCameraEditorModeFraming();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugEventCameraEditorModeBase::State Update( DebugEventCameraEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

  /**
  * @brief 情報表示 
  */ 
  virtual void DrawTextInfo( gfl2::util::DrawUtilText* pDrawText );

protected:

  virtual void initialize();

protected:

  Field::Camera::Controller::ControllerTypeAdjustPosition*    m_pCameraControllerAdjustPosition;  ///< 位置調整用カメラ機能
  Field::Camera::Controller::ControllerTypeEventScript*       m_pCameraControllerEventScript;     ///< スクリプト用カメラ機能

  bool                                                        m_isMoving;
};

GFL_NAMESPACE_END( Field )

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // __FIELD_EVENT_DEBUG_EVENT_CAMERA_EDITOR_MODE_H_INCLUDED__
#endif // PM_DEBUG

//======================================================================
/**
* @file FieldEventDebugCharaLayoutEditorMode.h
* @brief キャラエディターの動作モード
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#if !defined( __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_MODE_H_INCLUDED__)
#define __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_MODE_H_INCLUDED__
#pragma once

#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

#if defined(GF_ENABLE_DEBUG_EDITOR)

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <str/include/gfl2_MsgData.h>

// field script
#include "FieldScript/include/ScriptDebug.h"
#include "FieldScript/include/ScriptWork.h"

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
  class DebugCharaLayoutEditorModeBase;
  class DebugCharaLayoutEditorModeManager;

  namespace Camera {
    namespace Controller {
      class ControllerBase;
      class ControllerTypeDebugTopView;
    }
  }
}


GFL_NAMESPACE_BEGIN( Field )


/**
 * @brief デバッグウインドウ更新用キャラワーク
 */
struct CharaWork
{
  DebugCharaLayoutEditorModeManager*  pManager;
  DebugCharaLayoutEditorModeBase*     pSelf;
  u32                                 index;
  u32                                 walkStep;
  u32                                 runStep;
  gfl2::math::Vector3                 defaultPos;
  gfl2::math::Quaternion              defaultRot;
  u32                                 defaultEyeIndex;
  u32                                 defaultMouthIndex;
  u32                                 motionIndex;
  u32                                 dynamicMotionIndex;
  u32                                 dynamicMotionStartIndex;
  u32                                 dynamicMotionEndIndex;
  u32                                 dynamicMotionCount;
  u32                                 eyeIndex;
  u32                                 mouthIndex;
};

//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorCommonWork
 * @brief 動作モード共通ワーク
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorCommonWork
{
public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorCommonWork();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorCommonWork();

public:

  int                                           m_editCharaNum;
  const Field::FieldScript::DebugChrEditList*   m_pEditCharaList;

  u32                                           m_selectCharaIndex;

  CharaWork*                                    m_pCharaWork;

};

//------------------------------------------------------------------------------
/**
 * @class DebugMotionMessageManager
 * @brief モーションのメッセージ関連の管理クラス
 */
//------------------------------------------------------------------------------
class DebugMotionMessageManager
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugMotionMessageManager );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugMotionMessageManager(
    gfl2::heap::HeapBase* pHeap,
    gfl2::str::MsgData*   pMsgDataMotion,
    gfl2::str::MsgData*   pMsgDataDynamicMotion,
    void*                 pDynamicMotionMsgIDData
    );

  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugMotionMessageManager();

  /**
   * @brief 常駐モーションの文字列取得
   */ 
  const gfl2::str::STRCODE* GetMotionString( u32 motionID);

  /**
   * @brief 非常駐モーションの文字列取得
   */
  const gfl2::str::STRCODE* GetDynamicMotionString( u32 packID, u32 motionID);

  /**
   * @brief 目モーションの文字列取得
   */ 
  const gfl2::str::STRCODE* GetEyeMotionString( u32 motionID);

  /**
   * @brief 口モーションの文字列取得
   */ 
  const gfl2::str::STRCODE* GetMouthMotionString( u32 motionID);

private:

  static const u32 INVALID_START_MSG_ID = 0xFFFFFFFF;

  struct DynamicMotionMsgIDDataCore
  {
    u32     packID;
    u16     msgIDOffset;
    u8      motionNum;
    u8      padding[1];
  };

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  struct DynamicMotionMsgIDData
  {
    u32                          num;
    DynamicMotionMsgIDDataCore   core[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

private:

  u32 GetDynamicMotionStartMsgID( u32 packID) const ;

private:

  gfl2::heap::HeapBase*                 m_pHeap;
  gfl2::str::MsgData*                   m_pMsgDataMotion;
  gfl2::str::MsgData*                   m_pMsgDataDynamicMotion;
  DynamicMotionMsgIDData*               m_pDynamicMotionMsgIDData;

  gfl2::str::StrBuf*                    m_pStrBuf;
};

//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModeManager
 * @brief 動作モード管理
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModeManager
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModeManager );

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
    MODE_DISP_MODEL_LIST,     ///< モデルリスト表示
    MODE_PLACEMENT_EDIT,      ///< 配置操作
    MODE_PLAY_MOTION,         ///< 常駐モーション再生
    MODE_PLAY_DYNAMIC_MOTION, ///< 非常駐モーション再生

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
    gfl2::str::MsgData*               pMsgDataMotion;
    gfl2::str::MsgData*               pMsgDataDynamicMotion;
    void*                             pDynamicMotionMsgIDData;
  };

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModeManager();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModeManager();

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
  State Update();

  /**
   * @brief セットアップ
   */ 
  void Setup( Mode mode, gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief モード取得
   * @param mode モード
   * @return 指定したモード
   */ 
  DebugCharaLayoutEditorModeBase* GetMode( Mode mode) const ;

  /**
   * @brief 共通ワーク取得
   * @return 共通ワーク(書き換え可)
   */ 
  DebugCharaLayoutEditorCommonWork* GetCommonWork();

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

  /**
   * @brief モーションメッセージ管理クラス取得
   */
  DebugMotionMessageManager* GetMotionMessageManager() const { return  m_pMotionMessageManager; }

private:

  DebugCharaLayoutEditorModeBase*       m_pModeTable[ MODE_MAX ];

  gfl2::heap::HeapBase*                 m_pHeap;
  gfl2::debug::DebugWinGroup*           m_pDebugGroup;
  gfl2::str::MsgData*                   m_pMsgData;
  Fieldmap*                             m_pFieldmap;
  FieldScript::FieldScriptSystem*       m_pScriptSystem;
  State                                 m_state;
  DebugCharaLayoutEditorModeBase*       m_pCurrentMode;
  DebugCharaLayoutEditorCommonWork      m_commonWork;
  DebugMotionMessageManager*            m_pMotionMessageManager;
};


//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModeBase
 * @brief 動作モード基底
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModeBase );

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
    DebugCharaLayoutEditorModeManager*  pManager;
  };

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModeBase();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModeBase();

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
  virtual State Update( DebugCharaLayoutEditorModeManager* pManager) = 0 ;

  /**
   * @brief モード管理インスタンス取得
   */
  DebugCharaLayoutEditorModeManager* GetModeManager() { return m_pManager; }

protected:

  virtual void initialize() = 0 ;


protected:

  gfl2::heap::HeapBase*                 m_pHeap;
  gfl2::debug::DebugWinGroup*           m_pDebugGroup;
  gfl2::str::MsgData*                   m_pMsgData;
  Fieldmap*                             m_pFieldmap;
  DebugCharaLayoutEditorModeManager*    m_pManager;
  bool                                  m_isUpdate;
};


//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModeDispModelList
 * @brief モデルリスト表示動作モード
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModeDispModelList : public DebugCharaLayoutEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModeDispModelList );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModeDispModelList();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModeDispModelList();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugCharaLayoutEditorModeBase::State Update( DebugCharaLayoutEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

protected:

  virtual void initialize();

protected:

};


//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModePlacementEdit
 * @brief 配置操作動作モード
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModePlacementEdit : public DebugCharaLayoutEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModePlacementEdit );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModePlacementEdit();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModePlacementEdit();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugCharaLayoutEditorModeBase::State Update( DebugCharaLayoutEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

protected:

  virtual void initialize();


  void Print( const char* name, const gfl2::math::Vector3& pos, const gfl2::math::Quaternion& rot);

protected:

  MoveModel::FieldMoveModel*    m_pControlMoveModel;
  f32                           m_addPos;
  f32                           m_addRot;
  bool                          m_isTalkWindowVisibleDefault[ FIELDTALKWINDOW_MAX ];
  bool                          m_isTopViewMode;

  Camera::Controller::ControllerBase*               m_pSwapCameraController;
  Camera::Controller::ControllerTypeDebugTopView*   m_pTopViewCameraController;
  gfl2::math::Vector3                               m_cameraPos;
  gfl2::math::Vector3                               m_cameraTarget;
  gfl2::math::Quaternion                            m_cameraRot;

};


//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModePlayMotion
 * @brief モーション再生動作モード
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModePlayMotion : public DebugCharaLayoutEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModePlayMotion );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModePlayMotion();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModePlayMotion();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugCharaLayoutEditorModeBase::State Update( DebugCharaLayoutEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

protected:

  virtual void initialize();

  void PrintMotion( const char* name, u32 motionIndex);
  void PrintFace( const char* name, u32 eyeIndex, u32 mouthIndex);
  void PrintWalk( const char* name, u32 walkStep);
  void PrintRun( const char* name, u32 runStep);

protected:

  MoveModel::FieldMoveModel*    m_pControlMoveModel;

};

//------------------------------------------------------------------------------
/**
 * @class DebugCharaLayoutEditorModePlayDynamicMotion
 * @brief 非常駐モーション再生動作モード
 */
//------------------------------------------------------------------------------
class DebugCharaLayoutEditorModePlayDynamicMotion : public DebugCharaLayoutEditorModeBase
{
  /**
   * @brief コピー禁止クラスにする
   */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCharaLayoutEditorModePlayDynamicMotion );

public:

  /**
   * @brief コンストラクタ
   */ 
  DebugCharaLayoutEditorModePlayDynamicMotion();
  
  /**
   * @brief デストラクタ
   */ 
  virtual ~DebugCharaLayoutEditorModePlayDynamicMotion();

  /**
   * @brief 破棄
   */ 
  virtual void Terminate();

  /**
   * @brief 更新
   * @return 更新後のモードの状態
   */ 
  virtual DebugCharaLayoutEditorModeBase::State Update( DebugCharaLayoutEditorModeManager* pManager);

  /**
   * @brief セットアップ
   */ 
  virtual void Setup( gfl2::debug::DebugWinGroup* pDebugGroup);

  /**
   * @brief クリーン
   */ 
  virtual void Clean();

protected:

  virtual void initialize();

  void PrintMotion( const char* name, u32 motionIndex, s32 packID, u32 startIndex);
  void PrintFace( const char* name, u32 eyeIndex, u32 mouthIndex);

public:

  MoveModel::FieldMoveModel*    m_pControlMoveModel;

};


GFL_NAMESPACE_END( Field )

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // __FIELD_EVENT_DEBUG_CHARA_LAYOUT_EDITOR_MODE_H_INCLUDED__
#endif // PM_DEBUG

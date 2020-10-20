//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventFieldmapControl.h
 *  @brief  フィールドマップ管理
 *  @author tomoya takahashi
 *  @date   2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_FIELDMAP_CONTROL_H__ )
#define __EVENT_FIELDMAP_CONTROL_H__
#pragma once

#include <Fade/include/gfl2_FadeManager.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"

#include "Field/FieldRo/include/Fieldmap.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
typedef bool (*FLD_MAPCTRL_CALLBACK_FUNC)( void* p_work, GameSys::GameManager* p_gman );

//ブラックフェード
bool FldMapCtrlCallback_FadeInSetupDefault
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeInSetupDefaultUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupDefault
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupDefaultUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupDefaultLowerOnly
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeIsEndDefault
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeIsEndLowerOnly
  ( void* p_work, GameSys::GameManager* p_gman );

//イベント用ブラックフェード
bool FldMapCtrlCallback_FadeInSetupBlackFade
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupBlackFade
  ( void* p_work, GameSys::GameManager* p_gman );

bool FldMapCtrlCallback_FadeInSetupBlackFadeUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupBlackFadeUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman );

// キャプチャフェードアウト
bool FldMapCtrlCallback_FadeOutSetupCaptureBlackFade
  ( void* p_work, GameSys::GameManager* p_gman );


//エントランスイン・アウト
bool FldMapCtrlCallback_FadeInSetupEntranceIn
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeInSetupEntranceOut
  ( void* p_work, GameSys::GameManager* p_gman );

//カスタムフェード
bool FldMapCtrlCallback_FadeInSetupCustom
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeOutSetupCustom
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_FadeIsEndCustom
  ( void* p_work, GameSys::GameManager* p_gman );

// ロトムフェードイン
bool FldMapCtrlCallback_RotomFadeInSetup
  ( void* p_work, GameSys::GameManager* p_gman );
bool FldMapCtrlCallback_RotomFadeIsEnd
  ( void* p_work, GameSys::GameManager* p_gman );

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

struct FieldmapControlFadeParam{
  gfl2::Fade::DISP      disp;
  gfl2::Fade::FADE_TYPE type;
  gfl2::math::Vector4   startCol;
  gfl2::math::Vector4   endCol;
  u32                   sync;
};

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

/**
 *  @brief  マップチェンジ　データ更新クラス
 *  @class UpdateMapData
 */
class UpdateMapData
{
  GFL_FORBID_COPY_AND_ASSIGN(UpdateMapData); //コピーコンストラクタ＋代入禁止
public:

  enum Mode
  {
    MODE_MAPJUMP,                   /// マップジャンプ
    MODE_CONTINUE_ZONECHANGE,       ///< コンティニュー  ゾーン変動
    MODE_CONTINUE_NO_ZONECHANGE,    ///< コンティニュー  ゾーン変動なし
    MODE_JFCLOSE,                   ///< JFCLOSE
    MODE_JFOPEN,                    ///< JFOPEN

    MODE_MAX,
  };

  // @fix GFNMCat[702] 「あなぬけのヒモ」の遷移先が陸地でも、水上ライドが解除されない
  enum PcRideControlType
  {
    PC_RIDECONTROL_CLEAR,       ///< 必ずクリア
    PC_RIDECONTROL_CONNECT,     ///< コネクトタイプ
    PC_RIDECONTROL_ESCAPE,      ///< 穴抜けモード
    PC_RIDECONTROL_CONTINUE,    ///< 続きからモード

    PC_RIDECONTROL_MAX  ///< 最大値
  };

  struct Desc
  {
    GameSys::GameManager* p_gman;
    Field::Location location;
    b8              bUpdateEvTime;
    Mode            mode;
    PcRideControlType pc_ride_control;  // @fix GFNMCat[702]
  };

public:
  UpdateMapData();
  ~UpdateMapData();

  /**
   *  @brief 更新パラメータ設定
   */
  void Setup( const Desc & desc );

  /**
   *  @brief 更新
   */
  bool Update( void );


  /**
   *  @brief PlacementDataManagerの初期化共通処理
   */
  static void InitializePlacementDataManager( GameSys::GameManager* p_gman, const Field::Location& location );

  /**
   *  @brief GimmickEncountManagerの初期化共通処理
   */
  static void InitializeGimmickEncountManager( GameSys::GameManager* p_gman, const Field::Location& location );


  /**
   *  @brief 現状の状況からプレイヤーRide状態を設定
   */
  void ControlPlayerRide( GameSys::GameManager* p_gman, const Field::Location& location, PcRideControlType pcRideControl );

  void  SetupGameData( bool is_continue_and_not_zonechange = false );


  // エリアのモード取得
  static Area::Mode GetAreaMode( const GameSys::GameData* cp_gdata, Field::World* pWorld, AreaID areaId );

private:
  void OldDeleteJump(void);
  void OldDeleteContinue(void);
  void OldDeleteMoveModel(b32 is_force, int start = MoveModel::FIELD_MOVE_MODEL_PLAYER, b32 is_assert=false);

  bool isContinueMode() const;
  bool IsZoneChange() const;
  void JoinFestaControl() ;

private:
  Desc m_Data;
  u32 m_Seq;
};

/**
 *  @brief  マップデータ　アプリからリカバークラス
 *  @class RecoverMapData
 */
class RecoverMapData
{
  GFL_FORBID_COPY_AND_ASSIGN(RecoverMapData); //コピーコンストラクタ＋代入禁止

public:
  struct Desc
  {
    GameSys::GameManager* p_gman;
    Field::Location location;
  };

public:
  RecoverMapData();
  ~RecoverMapData();

  /**
   *  @brief 更新パラメータ設定
   */
  void Setup( const Desc & desc );

  /**
   *  @brief 更新
   */
  bool Update( void );

private:
  Desc m_Data;
  u32 m_Seq;
};




/**
 *  @class フィールドマップ生成
 */
class EventFieldmapOpen : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventFieldmapOpen); //コピーコンストラクタ＋代入禁止

#if PM_DEBUG
public:
  static int m_s_DebugCheckVramClear;
#endif

public:
  EventFieldmapOpen( gfl2::heap::HeapBase* heap );
  virtual ~EventFieldmapOpen();


  /** 
   *  @brief  FieldmapDescを設定
   */
  inline void SetDesc( const FieldmapDesc& desc ) { m_Desc = desc; }
  void SetDesc( FieldmapOpenMode openMode, const Location& playerLocation) { m_Desc.openMode = openMode; m_Desc.playerLocation = playerLocation; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  プレイヤーの位置情報を設定 FieldmapOpenModeを指定しない初期化は不正なので、ロケーションのみセットできてしまう本関数を削除 151114 by iwasawa
   */
  //-----------------------------------------------------------------------------
  //void SetPlayerLocation( const Location& playerLocation) { m_Desc.playerLocation = playerLocation; }

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
  * @brief   フェードコールバック設定
  */
  //--------------------------------------------------------------
  virtual void SetFadeCallBack
  (FLD_MAPCTRL_CALLBACK_FUNC fadeProc, FLD_MAPCTRL_CALLBACK_FUNC fadeIsEndProc, void * fadeWork);
  

  //----------------------------------------------------------------------------
  /**
   *  @brief  フェードコールバックをデフォルトに戻す。（デフォルト＝ブラックフェード）
   */
  //-----------------------------------------------------------------------------
  virtual void SetFadeCallBackDefault(void);
  virtual void SetFadeCallBackDefaultUpperOnly(void);


  //--------------------------------------------------------------
  /**
  * @brief   カスタムフェード設定
  */
  //--------------------------------------------------------------
  virtual void SetFadeCallBackCustom
    (gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, u32 sync = gfl2::Fade::FADE_DEFAULT_SYNC);

  //--------------------------------------------------------------
  /**
  * @brief   ジョインフェスタ専用フェード設定（ロトムフェード）
  */
  //--------------------------------------------------------------
  virtual void SetFadeCallBackJfOpen(void);

private:
  enum{
    SEQ_RECOVER_INITIALIZE,
    SEQ_RECOVER_INITIALIZE_WAIT,
    SEQ_CALL_FIELDPROC,
    SEQ_WAIT_READY,
    SEQ_WAIT_FADE_WAIT,
  };

private:
  FLD_MAPCTRL_CALLBACK_FUNC m_fadeProc;       ///< 画面フェード設定コールバック関数
  FLD_MAPCTRL_CALLBACK_FUNC m_fadeIsEndProc;  ///< 画面フェード待ちコールバック関数
  void* m_fadeWork; ///< 画面フェードワークポインタ

  FieldmapControlFadeParam  m_fadeParam;

  RecoverMapData m_RecoverMapData;  ///< マップデータリカバリー処理

  u32 m_Seq;

  FieldmapDesc m_Desc;
};


/**
 *  @class フィールドマップ破棄
 */
class EventFieldmapClose : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventFieldmapClose); //コピーコンストラクタ＋代入禁止

public:

  struct Desc
  {
    Desc()
      : closeType( FM_CLOSE_MODE_MAX )
      , isSkyboxKeep( false)
    {}

    FieldmapCloseMode  closeType;      ///< クローズタイプ
    bool        isSkyboxKeep;   ///< 天球リソースを残すか。trueで残す
  };


public:
  EventFieldmapClose( gfl2::heap::HeapBase* heap );
  virtual ~EventFieldmapClose();

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);


  //--------------------------------------------------------------
  /**
  * @brief   フェードコールバック設定
  */
  //--------------------------------------------------------------
  virtual void SetFadeCallBack
  (FLD_MAPCTRL_CALLBACK_FUNC fadeProc, FLD_MAPCTRL_CALLBACK_FUNC fadeIsEndProc, void * fadeWork);
  

  //----------------------------------------------------------------------------
  /**
   *  @brief  フェードコールバックをデフォルトに戻す。（デフォルト＝ブラックフェード）
   */
  //-----------------------------------------------------------------------------
  virtual void SetFadeCallBackDefault(void);
  virtual void SetFadeCallBackDefaultUpperOnly(void);
  virtual void SetFadeCallBackDefaultLowerOnly(void);

  // キャプチャーブラックフェードアウト
  virtual void SetFadeCallBackCaptureBlackFade(void);


  //--------------------------------------------------------------
  /**
  * @brief   カスタムフェード設定
  */
  //--------------------------------------------------------------
  virtual void SetFadeCallBackCustom
    (gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4& start_col, const gfl2::math::Vector4& end_col, u32 sync = gfl2::Fade::FADE_DEFAULT_SYNC);

  //----------------------------------------------------------------------------
  /**
   *  @brief 外部情報設定
   */
  //-----------------------------------------------------------------------------
  void SetDesc( const Desc& desc );

private:

  FLD_MAPCTRL_CALLBACK_FUNC m_fadeProc;       ///< 画面フェード設定コールバック関数
  FLD_MAPCTRL_CALLBACK_FUNC m_fadeIsEndProc;  ///< 画面フェード待ちコールバック関数
  void* m_fadeWork; ///< 画面フェードワークポインタ

  FieldmapControlFadeParam  m_fadeParam;

  Desc      m_Desc;       ///< 外部情報

};


GFL_NAMESPACE_END(Field);

#endif  // __EVENT_FIELDMAP_CONTROL_H__


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventEntranceIn.h
 *	@brief  エントランスアウト
 *	@author	tomoya takahashi
 *	@date		2015.05.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_ENTRANCE_IN_H__ )
#define __EVENT_ENTRANCE_IN_H__
#pragma once

#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Quaternion.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "niji_conv_header/field/mapjump/FieldMapJumpFieldType.h"
#include  "Field/FieldRo/include/Event/FieldEventEntranceType.h"
#include "Field/FieldRo/include/Event/EventEntranceCameraParameter.h"

GFL_NAMESPACE_BEGIN(Field);

GFL_NAMESPACE_BEGIN(MoveModel);
class FieldMoveModel;
GFL_NAMESPACE_END(MoveModel);

GFL_NAMESPACE_BEGIN(StaticModel);
class StaticModelManager;
class StaticModel;
GFL_NAMESPACE_END(StaticModel);

GFL_NAMESPACE_BEGIN(Camera);
class CameraManager;
class CameraUnit;
GFL_NAMESPACE_BEGIN(Controller);
class ControllerTypeEventScript;
GFL_NAMESPACE_END(Controller);
GFL_NAMESPACE_END(Camera);



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class EventEntranceIn : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventEntranceIn); //コピーコンストラクタ＋代入禁止

public:

  static const FADE_TYPE sFadeType[MC_FIELD_MAX][MC_FIELD_MAX];
  static const u32 sFadeTime[MC_FIELD_MAX][MC_FIELD_MAX];

  /**
   * @brief パラメーター
   */
  struct Desc {
    u32 mapJumpType;       ///<マップ接続タイプ
    u32 playSE;            ///< SE
    u32 eventID;           ///< イベントID
    ZoneID nowZoneID;      ///<ゾーンID
    ZoneID nextZoneID;     ///<次のゾーンID
    gfl2::math::Vector3 targetPos; ///< 吸着ポジション
    gfl2::math::Quaternion quaternion; ///< マップジャンプの方向
    b32 isExistenceCameraAnime; // カメラアニメがあるかどうか
    u32 cameraAnimeIndex; ///< カメラアニメーションのインデックス
    //u8 isSearchDoor; // ドアを検索する場合はtrue
    gfl2::math::Vector3 connectCenterPosition; ///< 接続の中心座標

    Desc()
    {
      InitializeDesc(this);
    }
  };

  /**
   *  @brief デスクリプション初期化
   */
  static void InitializeDesc( Desc* p_desc );


public:
  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventEntranceIn(gfl2::heap::HeapBase* p_heap) : GameSys::GameEvent(p_heap)
  {
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventEntranceIn()
  {
  }


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

  /**
   * @brief データをセットする
   * @param param パラメーター
   */
  void SetData(const EventEntranceIn::Desc& param);


private:
  /**
   * @brief エントランス・アウトを呼び出す
   * @param p_gman ゲームマネージャークラス
   * @param idx アタイプ
   */
  void CallEntranceIn(GameSys::GameManager* p_gman, u32 mapJumpType );


private:

  Desc m_Data;

};



//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの演出イベントのベースクラス 
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceBase : public GameSys::GameEvent {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceBase);

public:

    static const u8 WALK_WAIT_TIME = 11; // 歩き待機時間

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceBase(gfl2::heap::HeapBase* heap) : GameSys::GameEvent(heap),
    m_PerformSeq(0), // シーケンスステップ
    m_pPlayer(NULL),
    m_pStaticModelManager(NULL),
    walkCounter(0), ///< 歩きカウンター
    m_Seq(0),
    m_fadeCounter(-1),
    m_IsFadeEnd(false),
    m_FadeCaptureWait(0),
    m_CameraParameter(),
    m_IsCameraAnimation(false),
    m_CameraSeq(0),
    m_CameraPlayWait(0),
    m_pCameraManager(NULL),
    m_pCameraController(NULL),
    m_pCameraUnit(NULL),
    m_pCameraHeap(NULL),
    m_CameraSupportValue(1.0f)
    {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~EventEntranceInPerformanceBase()
    {
    }

    /**
     * @brief イベントを動かしてよいかどうかの判定関数 純粋仮想関数
     * @param p_gman ゲームマネージャークラス
     * @retval true 起動してよい
     * @retval false 起動しない
     */
    virtual bool BootChk(GameSys::GameManager* p_gman);

    /**
     * @brief bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
     * @param p_gman ゲームマネージャークラス
     */
    virtual void InitFunc(GameSys::GameManager* p_gman);

    /**
     * @brief 呼び出される関数 純粋仮想関数
     * @param p_gman ゲームマネージャークラス
     * @return GMEVENT_RESULT イベント制御関数の戻り値
     */
    virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

    /**
     * @brief 呼び出される関数 純粋仮想関数
     * @param p_gman ゲームマネージャークラス
     */
    virtual void EndFunc(GameSys::GameManager* p_gman);

    /**
     * @brief データをセットする
     * @param param パラメーター
     */
    void SetData(const EventEntranceIn::Desc& param);

private:

    /**
     * @brief シーケンス
     */
    enum {
        SEQ_FADE,
        SEQ_PERFORMANCE,
    };
    u32 m_Seq;


protected:

    /**
     * @brief 戻り値
     */
    enum RET {
        RET_CONTINUE,
        RET_CONTINUE_DIRECT,
        RET_FINISH,
    };

    /**
     *  @brief カメラプロセスの戻り値
     */
    enum CAMERA_PROC_RET
    {
      CAMERA_PROC_RET_CONTINUE,               ///< カメラのみ実行
      CAMERA_PROC_RET_CONTINUE_DO_PROCESS,    ///< カメラと演出を実装
      CAMERA_PROC_RET_FINISH,                 ///< カメラ終了
    };
     


    void FadeStart( void ) { m_Seq = SEQ_FADE; }
    b32 IsFadeEnd(void) const;

    /**
     * @brief 初期処理
     */
    virtual void InitProcess(void)
    {
    };

    /**
     * @brief フェード処理
     */
    void FadeProcess(ZoneDataLoader* zoneDataLoader);

    /**
     * @brief 演出処理
     */
    virtual EventEntranceInPerformanceBase::RET PerformanceProcess( GameSys::GameManager* p_gman )
    {
        return RET_FINISH;
    }

    /**
     *  @brief カメラパラメータの設定
     */
    virtual void SetupCameraProcess( GameSys::GameManager* p_gman );

    /**
     * @brief カメラ処理
     */
    virtual CAMERA_PROC_RET CameraProcess(void);

    /**
     * @brief カメラ終了
     */
    virtual void EndCameraProcess();

protected:
    EventEntranceIn::Desc m_Data; // パラメーター
    u32 m_PerformSeq; // シーケンスステップ
    MoveModel::FieldMoveModel* m_pPlayer;
    StaticModel::StaticModelManager * m_pStaticModelManager;

    //mmodel::MoveModelAcmdList mMoveList[2]; // アクションコマンドリスト
    u8 walkCounter; ///< 歩きカウンター
    s8 m_fadeCounter; ///< フェード開始カウンター
    u8 m_IsFadeEnd; ///< フェード終了を監視するか？ キャプチャフェードは監視しない。
    s8 m_FadeCaptureWait; ///< フェードキャプチャー待ち時間

    // カメラ操作
    EventEntranceCameraParameter::Param m_CameraParameter;
    b32 m_IsCameraAnimation;
    u32 m_CameraSeq;
    s32 m_CameraPlayWait;
    Camera::CameraManager*                         m_pCameraManager;
    Camera::Controller::ControllerTypeEventScript* m_pCameraController;
    Camera::CameraUnit*                            m_pCameraUnit;
    gfl2::heap::HeapBase*                          m_pCameraHeap;
    f32 m_CameraSupportValue; 

};



//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの通常演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceNORMAL : public EventEntranceInPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceNORMAL);

public:
  static const u32 FADE_START_WAIT = 2;

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceNORMAL(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceNORMAL()
    {
    }

private:

    /**
     * @brief 演出処理
     */
    virtual EventEntranceInPerformanceBase::RET PerformanceProcess( GameSys::GameManager* p_gman );

};

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのドア演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceDOOR : public EventEntranceInPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceDOOR);
public:
  static const u32 FADE_START_WAIT = 2;

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceDOOR(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceDOOR()
    {
    }

private:

    static const u8 WAIT_TIME = 11; // 演出時間

    /**
     * @brief 演出処理
     */
    virtual EventEntranceInPerformanceBase::RET PerformanceProcess( GameSys::GameManager* p_gman );


    StaticModel::StaticModel * m_pDoorModel;

};

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのドア演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceMAT : public EventEntranceInPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceMAT);

public:
  static const u32 FADE_START_WAIT = 2;

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceMAT(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceMAT()
    {
    }

private:

    /**
     * @brief 演出処理
     */
    virtual EventEntranceInPerformanceBase::RET PerformanceProcess( GameSys::GameManager* p_gman );

};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief 吸着後アニメーションを再生する演出ベース
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
class EventEntranceInPerformanceTargetMoveAndAnimationBase : public EventEntranceInPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceTargetMoveAndAnimationBase);


public:
  static const u32 FADE_START_WAIT = 6;

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceTargetMoveAndAnimationBase(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceTargetMoveAndAnimationBase()
    {
    }

protected:

  // 吸着するポイントの座標
  virtual void GetPos( gfl2::math::Vector3* p_pos ) = 0;
  // 吸着後の向き
  virtual void GetQua( gfl2::math::Quaternion* p_qua ) = 0;
  // 再生するアニメーションナンバー
  virtual u32 GetAnimationNo() = 0;

private:

    /**
     * @brief 演出処理
     */
    virtual EventEntranceInPerformanceBase::RET PerformanceProcess( GameSys::GameManager* p_gman );

};

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの梯子イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceLadderUp : public EventEntranceInPerformanceTargetMoveAndAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceLadderUp);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceLadderUp(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceTargetMoveAndAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceLadderUp()
    {
    }

private:
    // 吸着するポイントの座標
    virtual void GetPos( gfl2::math::Vector3* p_pos );
    // 吸着後の向き
    virtual void GetQua( gfl2::math::Quaternion* p_qua );
    // 再生するアニメーションナンバー
    virtual u32 GetAnimationNo();

};

class EventEntranceInPerformanceLadderDown : public EventEntranceInPerformanceTargetMoveAndAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceLadderDown);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceLadderDown(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceTargetMoveAndAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceLadderDown()
    {
    }

private:

    // 吸着するポイントの座標
    virtual void GetPos( gfl2::math::Vector3* p_pos );
    // 吸着後の向き
    virtual void GetQua( gfl2::math::Quaternion* p_qua );
    // 再生するアニメーションナンバー
    virtual u32 GetAnimationNo();

};

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのしゃがみ移動イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceInPerformanceThroughAGate : public EventEntranceInPerformanceTargetMoveAndAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceInPerformanceThroughAGate);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceInPerformanceThroughAGate(gfl2::heap::HeapBase* heap) : EventEntranceInPerformanceTargetMoveAndAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceInPerformanceThroughAGate()
    {
    }

private:

    // 吸着するポイントの座標
    virtual void GetPos( gfl2::math::Vector3* p_pos );
    // 吸着後の向き
    virtual void GetQua( gfl2::math::Quaternion* p_qua );
    // 再生するアニメーションナンバー
    virtual u32 GetAnimationNo();

};

GFL_NAMESPACE_END(Field);


#endif	// __EVENT_ENTRANCE_IN_H__


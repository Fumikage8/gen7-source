//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventEntranceOut.h
 *	@brief  エントランスアウト
 *	@author	tomoya takahashi
 *	@date		2015.05.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_ENTRANCE_OUT_H__ )
#define __EVENT_ENTRANCE_OUT_H__
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



namespace MoveModel{
  class FieldMoveModel;
  class FieldMoveModelPlayer;
}

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class EventEntranceOut : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOut); //コピーコンストラクタ＋代入禁止

public:

  static const FADE_TYPE sFadeType[MC_FIELD_MAX][MC_FIELD_MAX];
  static const u32 sFadeTime[MC_FIELD_MAX][MC_FIELD_MAX];


  /**
   * @brief パラメーター
   */
  struct Desc {
    u32 mapJumpType;       //<マップ接続タイプ
    u32 eventID;           //< イベントID
    ZoneID nowZoneID;      //<ゾーンID
    ZoneID nextZoneID;     //<次のゾーンID
    b32 isExistenceCameraAnime; // カメラアニメがあるかどうか
    u32 cameraAnimeIndex; ///< カメラアニメーションのインデックス
    gfl2::math::Quaternion rotate;  //<出る方向
    gfl2::math::Vector3 outPos; ///< 出現座標
    //u8 isSearchDoor; // ドアを検索する場合はtrue
    gfl2::math::Vector3 connectCenterPosition; ///< イベントの中心座標
    gfl2::math::Vector3 jumpCenterPosition; ///< ジャンプ位置の中心位置

    Desc()
    {
      InitializeDesc(this);
    }
  };


  static void InitializeDesc( Desc* p_desc );


public:
  /**
   * @brief コンストラクタ
   * @param p_heap ヒープ
   */
  EventEntranceOut(gfl2::heap::HeapBase* p_heap) : GameSys::GameEvent(p_heap)
  {
  }

  /**
   * @brief デストラクタ
   */
  virtual ~EventEntranceOut()
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
  void SetData(const EventEntranceOut::Desc& param);

private:
  /**
   * @brief エントランス・アウトを呼び出す
   * @param p_gman ゲームマネージャークラス
   * @param idx アタイプ
   */
  void CallEntranceOut(GameSys::GameManager* p_gman, u32 mapJumpType );


private:

  Desc m_Data;

};



//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの演出イベントのベースクラス 
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceBase : public GameSys::GameEvent {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceBase);

public:

    static const u8 WALK_WAIT_TIME = 4; // 歩き待機時間

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceBase(gfl2::heap::HeapBase* heap) : GameSys::GameEvent(heap),
    m_PerformSeq(0), // シーケンスステップ
    walkCounter(0), ///< 歩きカウンター
    m_pPlayer(NULL),
    m_pStaticModelManager(NULL),
    m_Seq(0),
    m_CameraParameter(),
    m_IsCameraAnimation(false),
    m_CameraSeq(0),
    m_pCameraManager(NULL),
    m_pCameraController(NULL),
    m_pCameraUnit(NULL),
    m_pCameraHeap(NULL),
    m_CameraBank(0),
    m_CameraDistance(0),
    m_CameraPlayWait(0)
    {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~EventEntranceOutPerformanceBase()
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
    void SetData(const EventEntranceOut::Desc& param);

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


    /**
     * @brief 初期処理
     */
    virtual bool SetUp(void)
    {
      return true;
    };

    /**
     * @brief フェード処理
     */
    void FadeProcess(ZoneDataLoader* zoneDataLoader);

    /**
     * @brief 演出処理
     */
    virtual EventEntranceOutPerformanceBase::RET PerformanceProcess(void)
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


    void FadeStart( void ) { m_Seq = SEQ_FADE; }
    b32 IsFadeEnd(void) const;


protected:
    EventEntranceOut::Desc m_Data; // パラメーター
    u32 m_PerformSeq; // シーケンスステップ
    //mmodel::MoveModelAcmdList mMoveList[2]; // アクションコマンドリスト
    u8 walkCounter; ///< 歩きカウンター
    MoveModel::FieldMoveModelPlayer* m_pPlayer;
    StaticModel::StaticModelManager* m_pStaticModelManager;

    // カメラ操作
    EventEntranceCameraParameter::Param m_CameraParameter;
    b32 m_IsCameraAnimation;
    u32 m_CameraSeq;
    Camera::CameraManager*                         m_pCameraManager;
    Camera::Controller::ControllerTypeEventScript* m_pCameraController;
    Camera::CameraUnit*                            m_pCameraUnit;
    gfl2::heap::HeapBase*                          m_pCameraHeap;

    // カメラ終着点情報
    gfl2::math::Vector3 m_CameraPosition;
    gfl2::math::Vector3 m_CameraTargetPosition;
    gfl2::math::Vector3 m_CameraRotate;
    f32                 m_CameraBank;
    f32                 m_CameraDistance;
    s32 m_CameraPlayWait;

};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの通常演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceMAT : public EventEntranceOutPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceMAT);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceMAT(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceMAT()
    {
    }

private:

    /**
     * @brief 初期処理
     */
    virtual bool SetUp(void);

    /**
     * @brief 演出処理
     */
    virtual EventEntranceOutPerformanceBase::RET PerformanceProcess(void);

};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの通常演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceNORMAL : public EventEntranceOutPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceNORMAL);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceNORMAL(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceBase(heap), m_IsPositionAdjust(false)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceNORMAL()
    {
    }

private:

    enum
    {
      SEQ_INIT,
      SEQ_FADESTART,
      SEQ_ANIMESTART,
      SEQ_WAIT,
    };

    /**
     * @brief 初期処理
     */
    virtual bool SetUp(void);

    /**
     * @brief 演出処理
     */
    virtual EventEntranceOutPerformanceBase::RET PerformanceProcess(void);

private:
    bool m_IsPositionAdjust;

};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのドア演出イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceDOOR : public EventEntranceOutPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceDOOR);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceDOOR(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceDOOR()
    {
    }

private:

    /**
     * @brief 初期処理
     */
    virtual bool SetUp(void);

    /**
     * @brief 演出処理
     */
    virtual EventEntranceOutPerformanceBase::RET PerformanceProcess(void);


private:
    StaticModel::StaticModel* m_pDoorModel;   //< ドアモデル
};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief 出現ポイントに出現方向を向いた情報でアニメーションを流すだけの演出ベース
 * @note EntranceOutカメラに非対応です。
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
class EventEntranceOutPerformanceAnimationBase : public EventEntranceOutPerformanceBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceAnimationBase);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceAnimationBase(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceAnimationBase()
    {
    }

protected:

  /**
   *  @brief アニメーションNoの取得
   */
  virtual u32 GetAnimationNo() = 0;

private:

    /**
     * @brief 演出処理
     */
    virtual EventEntranceOutPerformanceBase::RET PerformanceProcess(void);

};


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトの梯子イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceLadderUp : public EventEntranceOutPerformanceAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceLadderUp);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceLadderUp(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceLadderUp()
    {
    }

protected:

  /**
   *  @brief アニメーションNoの取得
   */
  virtual u32 GetAnimationNo();

};

class EventEntranceOutPerformanceLadderDown : public EventEntranceOutPerformanceAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceLadderDown);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceLadderDown(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceLadderDown()
    {
    }

private:

  /**
   *  @brief アニメーションNoの取得
   */
  virtual u32 GetAnimationNo();

};

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
/**
 * @brief エントランス・アウトのしゃがみ移動イベントクラス
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

class EventEntranceOutPerformanceThroughAGate : public EventEntranceOutPerformanceAnimationBase {
    GFL_FORBID_COPY_AND_ASSIGN(EventEntranceOutPerformanceThroughAGate);

public:

    /**
     * @brief コンストラクタ
     * @param heap ヒープ
     */
    EventEntranceOutPerformanceThroughAGate(gfl2::heap::HeapBase* heap) : EventEntranceOutPerformanceAnimationBase(heap)
    {
    }

    /**
     * @brief デストラクタ 
     */
    virtual ~EventEntranceOutPerformanceThroughAGate()
    {
    }

private:

  /**
   *  @brief アニメーションNoの取得
   */
  virtual u32 GetAnimationNo();

};

GFL_NAMESPACE_END(Field);


#endif	// __EVENT_ENTRANCE_OUT_H__


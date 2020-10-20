//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		EventGameStart.h
 *	@brief  ゲーム開始イベント
 *	@author	tomoya takahashi
 *	@date		2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_GAME_START_H__ )
#define __EVENT_GAME_START_H__
#pragma once


#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameData.h"

#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					クラス前方宣言
*/
//-----------------------------------------------------------------------------
class EventFieldmapOpen;

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

class EventGameStart
{
  GFL_FORBID_COPY_AND_ASSIGN(EventGameStart); //コピーコンストラクタ＋代入禁止

public:

  enum GameInitMode{
    GAMEINIT_MODE_FIRST,              ///< はじめからはじめる。
    GAMEINIT_MODE_CONTINUE,           ///< つづきからはじめる。

#if PM_DEBUG
    //GAMEINIT_MODE_DEBUG_CTRNAME,      ///< デバッグ起動(CTR本体名で開始)
    GAMEINIT_MODE_DEBUG_DEFAULTNAME,  ///< デバッグ起動(デフォルト名で開始)
    GAMEINIT_MODE_DEBUG_FIRST,        ///< デバッグデータではじめからはじめる
    GAMEINIT_MODE_DEBUG_ALLMAPJUMP,   ///< フィールドを開始し、全マップジャンプを自動で実行する。
    GAMEINIT_MODE_DEBUG_BATTLE_INST_TEST,   ///< フィールドを開始し、バトル施設のテストを行なう
    GAMEINIT_MODE_DEBUG_NETTEST,      ///< デバッグデータではじめからはじめる&通信テスト用の初期データ
    GAMEINIT_MODE_DEBUG_FIRST_TOP_PFM_DRESS,  ///< デバッグデータではじめからはじめる(最高処理負荷着せ替えでスタート)
#endif
  };
    
  //-------------------------------------
  ///	ゲームの初期化に必要な情報
  //=====================================
  struct Description{
    GameInitMode mode; ///< 起動モード
    GameSys::GameManager* gmgr;   ///< ゲームマネージャー
  };
    


public:

  // ゲーム開始イベントの生成
  static GameSys::GameEvent* CallEventGameStart(const Description& desc);

  // ゲーム開始イベントの生成
  static GameSys::GameEvent* ChangeEventGameStart(const Description& desc);


private:
  // ゲームデータ初期化
  static void GameInit( GameSys::GameManager* p_gman );
  
#if PM_DEBUG
  // デバッグ開始データ初期化
  static void GameInitDebug( GameSys::GameManager* p_gman, GameSys::GameData::DebugDataType gamedata_debug_type, bool b_ev_debug );
#endif
#if PM_DEBUG
  //捕獲チュートリアルDLLサイズテスト
  static void CaptureTutorialDllTest(void);
#endif

private:

  /**
   *  @class フィールドを最初から始める　ベースクラス
   */
  class EventFirstBase : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventFirstBase); //コピーコンストラクタ＋代入禁止

  public:
    EventFirstBase( gfl2::heap::HeapBase* heap );
    virtual ~EventFirstBase();

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
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void SetFadeinModeFunc(EventFieldmapOpen* p_event);

  private:
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr) = 0; 

  protected:

    gfl2::heap::HeapBase* m_pHeap;
    Location m_StartLocation;
    UpdateMapData m_UpdateMapData;
  };

  /**
   *  @class 最初から始める。
   */
  class EventFirst : public EventFirstBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventFirst); //コピーコンストラクタ＋代入禁止

  public:
    EventFirst( gfl2::heap::HeapBase* heap );
    virtual ~EventFirst();


  private:
  
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr);

    /**
     * @brief   フェードインモードの設定
     */
    virtual void SetFadeinModeFunc(EventFieldmapOpen* p_event);
  };

#if PM_DEBUG

  public:

  /**
   *  @class デバッグデータで最初から始める。
   */
  class EventDebugFirst : public EventFirstBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventDebugFirst); //コピーコンストラクタ＋代入禁止

  public:
    EventDebugFirst( gfl2::heap::HeapBase* heap );
    virtual ~EventDebugFirst();
    static void SetStartInfo( s32 zoneId, const gfl2::math::Vector3& rPosition, const gfl2::math::Quaternion& rQuaternion );

  private:
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr); 


    static b32 m_IsSetStartInfo;
    static s32 m_StartZone;
    static gfl2::math::Vector3 m_StartPositon;
    static gfl2::math::Quaternion m_StartQuaternion;
  };


  /**
   *  @class デバッグデータで最初から始める。
   */
  class EventDebugDefaultName : public EventFirstBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventDebugDefaultName); //コピーコンストラクタ＋代入禁止

  public:
    EventDebugDefaultName( gfl2::heap::HeapBase* heap );
    virtual ~EventDebugDefaultName();

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void EndFunc(GameSys::GameManager* gmgr);


  private:
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr); 
  };

  /**
   *  @class フィールドを開始し、全マップジャンプを自動で実行する
   */
  class EventDebugAllMapJump : public EventFirstBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventDebugAllMapJump); //コピーコンストラクタ＋代入禁止

  public:
    EventDebugAllMapJump( gfl2::heap::HeapBase* heap );
    virtual ~EventDebugAllMapJump();

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void EndFunc(GameSys::GameManager* gmgr);

  private:
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr); 
  };


  /**
   *  @class フィールドを開始し、バトル施設のテストを行なう
   */
  class EventDebugBattleInst : public EventFirstBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventDebugBattleInst); //コピーコンストラクタ＋代入禁止

  public:
    EventDebugBattleInst( gfl2::heap::HeapBase* heap );
    virtual ~EventDebugBattleInst();

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void EndFunc(GameSys::GameManager* gmgr);

  private:
    /**
     *  @brief  開始ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr); 
  };
#endif


  /**
   *  @class フィールドを最初から始める　ベースクラス
   */
  class EventContinueBase : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN(EventContinueBase); //コピーコンストラクタ＋代入禁止

  public:
    EventContinueBase( gfl2::heap::HeapBase* heap );
    virtual ~EventContinueBase();

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
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void SetFadeinModeFunc(EventFieldmapOpen* p_event);

  private:
    /**
     *  @brief  復帰ロケーションの設定
     */
    virtual void initStartLocation(GameSys::GameManager* gmgr); 

  protected:

    gfl2::heap::HeapBase* m_pHeap;
    Location m_StartLocation;
    UpdateMapData::Mode m_UpdateMapDataMode;
    UpdateMapData m_UpdateMapData;
  };

};


/**
 *  @class　ゲームクローズ　イベント
 */
class EventGameClose : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventGameClose); //コピーコンストラクタ＋代入禁止
public:
  EventGameClose( gfl2::heap::HeapBase* heap );
  virtual ~EventGameClose();

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


private:
  gfl2::heap::HeapBase* m_pHeap;
};

GFL_NAMESPACE_END(Field);

#endif	// __EVENT_GAME_START_H__


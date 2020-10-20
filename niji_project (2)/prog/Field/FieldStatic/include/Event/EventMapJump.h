//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventMapJump.h
 *  @brief  マップ遷移イベント
 *  @author takahashi tomoya
 *  @date   2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __EVENT_MAP_JUMP_H__ )
#define __EVENT_MAP_JUMP_H__
#pragma once
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/NijiTime.h"

#include "Field/FieldStatic/include/FieldLocation.h"

#include "Field/FieldRo/include/PlacementData/accessor/FieldMapJumpAccessor.h"

#include "Field/FieldRo/include/Event/FieldEventEntranceIn.h"
#include "Field/FieldRo/include/Event/FieldEventEntranceOut.h"

#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"


GFL_NAMESPACE_BEGIN(Field);


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

/**
 *  @class マップジャンプイベントコア
 */
class EventMapJumpCore : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventMapJumpCore); //コピーコンストラクタ＋代入禁止

public:
  /************************************************************************/
  /*  MapJumpタイプ                                                                  
  /************************************************************************/
  enum EventMapJumpType
  {
    EV_MAPJUMP_DEFAULT,       ///< 通常のブラックフェード(極短)
    EV_MAPJUMP_BLACK_FADE,    ///< ブラックフェード イベント演出用(通常とはsyncが違う)
    EV_MAPJUMP_NO_EFFECT,     ///< 演出無し。フェードなどは呼び出し側がやること
    EV_MAPJUMP_ENTRANCE,      ///< エントランス系演出あり
    EV_MAPJUMP_BLACK_FADE_DOUBLE,///< 両画面ブラックフェード（極短）
    EV_MAPJUMP_JF_OPEN,       ///< ジョインフェスタ遷移時専用フェード（フェードアウト有り、ロトムフェードイン有り）
    EV_MAPJUMP_JF_OPEN2,      ///< ジョインフェスタ遷移時専用フェード（フェードアウト無し、ロトムフェードイン有り）

    EV_MAPJUMP_MAX,           ///< 最大値
  };

  /************************************************************************/
  /*  MapJumpパラメータ                                                                  
  /************************************************************************/
  class Desc
  {
  public:
    GameSys::GameData* p_gdata;    ///< ゲームデータ
    ZoneID             nowZoneID;
    EventMapJumpType   evMapJumpType; ///< イベントマップジャンプタイプ
    Location           location;   ///< ジャンプ先位置
    b32                saveEscape;  ///< 脱出先保存処理を実行するか？　true 行なう
    FieldMapJumpAccessor::MapJumpData mapjump;  ///< マップジャンプパラメータ
    ///< エントランスINパラメータ
    EventEntranceIn::Desc entranceIn;
    ///< エントランスOUTパラメータ
    EventEntranceOut::Desc entranceOut;
    ///
    bool dispPlaceName;
    bool bgmKeepFlag;
    UpdateMapData::Mode updateMapDataMode;
    f32 mapJumpPosPercent;
    UpdateMapData::PcRideControlType pcRideControlType; // @fix GFNMCat[702]
    bool bUpdateEvTime;

  public:
    Desc() : 
      p_gdata(NULL),
      nowZoneID(ZONEID_ERROR),
      evMapJumpType(EV_MAPJUMP_DEFAULT),
      location(),
      saveEscape(true),
      dispPlaceName(true),
      bgmKeepFlag(false),
      updateMapDataMode( UpdateMapData::MODE_MAPJUMP ),
      mapJumpPosPercent(0.0f),   // @fix GFNMCat[702]
      pcRideControlType( UpdateMapData::PC_RIDECONTROL_CLEAR ),   // @fix GFNMCat[702]
      bUpdateEvTime(true)
    {
      gfl2::std::MemClear( &mapjump, sizeof(FieldMapJumpAccessor::MapJumpData) );
    }

    Desc( const Desc& desc )
    {
      *this = desc;
    }
  };


public:
  EventMapJumpCore( gfl2::heap::HeapBase* heap );
  virtual ~EventMapJumpCore();

  inline void SetDesc( const Desc& desc ){ m_Desc = desc; }
  inline void SetBGMKeepFlag( bool flag ){ m_Desc.bgmKeepFlag = flag; }
  inline void SetUpdateEvTime(bool flag){ m_Desc.bUpdateEvTime = flag; }

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



protected:

  //--------------------------------------------------------------
  /**
   * @brief   イベント初期化　直前に呼び出される関数
   * @param   gmgr       ゲームマネージャークラス
   * @note    フィールド遷移時に、何からの処理を挟みたい場合等にオーバライドして下さい
   */
  //--------------------------------------------------------------
  virtual void OnInitialize( GameSys::GameManager* gmgr ){}

  //--------------------------------------------------------------
  /**
   * @brief   フィールド破棄直後に呼び出される関数
   * @param   gmgr       ゲームマネージャークラス
   * @note    フィールド遷移時に、何からの処理を挟みたい場合等にオーバライドして下さい
   */
  //--------------------------------------------------------------
  virtual void OnFieldClose( GameSys::GameManager* gmgr ){}


  /// EscapeLocationの保存
  void saveEscapeLocation(GameSys::GameManager* gmgr);
  
  /// Fieldmapオープンモード取得
  FieldmapOpenMode getFmOpenMode(void) const;

  /// Fieldmapクローズモードの取得
  FieldmapCloseMode getFmCloseMode(void) const;

private:
  World* m_pWorld;  ///< ワールドデータ
  Area*  m_pArea;   ///< エリアデータ
  Desc  m_Desc;  ///< ジャンプ情報
  UpdateMapData m_UpdateMapData;  ///< マップデータ更新処理
};


//------------------------------------------------------------------------------
/**
 * @class       EventMapJumpOtherWorldTime
 * @brief       アローラ時間から別世界時間にジャンプ　or 戻ってくる。
 */
//------------------------------------------------------------------------------
class EventMapJumpOtherWorldTime : public EventMapJumpCore
{
  GFL_FORBID_COPY_AND_ASSIGN(EventMapJumpOtherWorldTime); //コピーコンストラクタ＋代入禁止
public:
  EventMapJumpOtherWorldTime( gfl2::heap::HeapBase* heap ) : EventMapJumpCore(heap), m_OtherWorldType(GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_MAX) {}
  virtual ~EventMapJumpOtherWorldTime(){}

  //--------------------------------------------------------------
  /**
   * @brief   フィールド破棄直後に呼び出される関数
   * @param   gmgr       ゲームマネージャークラス
   * @note    フィールド遷移時に、何からの処理を挟みたい場合等にオーバライドして下さい
   */
  //--------------------------------------------------------------
  virtual void OnFieldClose(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   別世界時間のタイプを設定する。
   *  @param  type 別世界時間　へのジャンプタイプ
      ALOLA_OTHER_WOLRD_TYPE_NIKKOU,    ///< 別世界タイプ　6時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_GEKKOU,    ///< 別世界タイプ 18時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_MAX,       ///< 通常時間に戻す
   *      
   */
  //--------------------------------------------------------------
  inline void SetOtherWorldType( GameSys::AlolaTime::AlolaOtherWorldType type ) { m_OtherWorldType = type; }

private:

  // 別世界時間タイプ
  GameSys::AlolaTime::AlolaOtherWorldType m_OtherWorldType;

};


//------------------------------------------------------------------------------
/**
 * @class       EventMapJumpGameOver
 * @brief       
 * @author      N.Takeda
 * @date        2015/07/15(水) 13:25:51
 */
//------------------------------------------------------------------------------
class EventMapJumpGameOver : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventMapJumpGameOver); //コピーコンストラクタ＋代入禁止

public:
  /************************************************************************/
  /*  MapJumpタイプ                                                                  
  /************************************************************************/
  enum EventMapJumpType
  {
    EV_MAPJUMP_FADE_NONE,     ///< フェードインはスクリプト任せ
    EV_MAPJUMP_BLACK_FADE,    ///< ブラックフェード イベント演出用(通常とはsyncが違う)

    EV_MAPJUMP_MAX,           ///< 最大値
  };

  /************************************************************************/
  /*  MapJumpパラメータ                                                                  
  /************************************************************************/
  class Desc
  {
  public:
    GameSys::GameData* p_gdata;    ///< ゲームデータ
    ZoneID             nowZoneID;
    EventMapJumpType   evMapJumpType; ///< イベントマップジャンプタイプ
    Location           location;   ///< ジャンプ先位置

  public:
    Desc() : 
      p_gdata(NULL),
      nowZoneID(ZONE_ID_INVALID), ///< Coverity.11499
      evMapJumpType(EV_MAPJUMP_BLACK_FADE),
      location()
    {
    }

    Desc( const Desc& desc )
    {
      *this = desc;
    }
  };


public:
  EventMapJumpGameOver( gfl2::heap::HeapBase* heap );
  virtual ~EventMapJumpGameOver();

  inline void SetDesc( const Desc& desc ){ m_Desc = desc; }

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

private:
  World* m_pWorld;  ///< ワールドデータ
  Area*  m_pArea;   ///< エリアデータ
  Desc  m_Desc;  ///< ジャンプ情報
  UpdateMapData m_UpdateMapData;  ///< マップデータ更新処理
};

//------------------------------------------------------------------------------
/**
 * @class       EventMapJumpWithoutField
 * @brief       
 * @author      Miyuki Iwasawa
 * @date        2015/11/15(月)
 */
//------------------------------------------------------------------------------
class EventMapJumpWithoutField : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventMapJumpWithoutField); //コピーコンストラクタ＋代入禁止

public:
  /************************************************************************/
  /*  MapJumpタイプ                                                                  
  /************************************************************************/
  enum EventMapJumpType
  {
    EV_MAPJUMP_BLACK_FADE,    ///< ブラックフェード イベント演出用(通常とはsyncが違う)
    EV_MAPJUMP_NO_EFFECT,     ///< フェードインはスクリプト任せ

    EV_MAPJUMP_MAX,           ///< 最大値
  };

  /************************************************************************/
  /*  MapJumpパラメータ                                                                  
  /************************************************************************/
  class Desc
  {
  public:
    GameSys::GameData* p_gdata;    ///< ゲームデータ
    ZoneID             nowZoneID;
    EventMapJumpType   evMapJumpType; ///< イベントマップジャンプタイプ
    Location           location;   ///< ジャンプ先位置
    bool dispPlaceName;
    bool bgmKeepFlag;
    UpdateMapData::Mode updateMapDataMode;

  public:
    Desc() : 
      p_gdata(NULL),
      evMapJumpType(EV_MAPJUMP_BLACK_FADE),
      location(),
      dispPlaceName(true),
      bgmKeepFlag(false),
      updateMapDataMode( UpdateMapData::MODE_MAPJUMP )
    {
    }

    Desc( const Desc& desc )
    {
      *this = desc;
    }
  };


public:
  EventMapJumpWithoutField( gfl2::heap::HeapBase* heap );
  virtual ~EventMapJumpWithoutField();

  inline void SetDesc( const Desc& desc ){ m_Desc = desc; }

   //NMCat[3533]「リザードン　フライト」で着地した際の自機の向きが南固定の為、着地場所によっては自機が不自然な方向を向く 修正のための関数も追加
  inline void SetDeskLocationQuaternion( const gfl2::math::Quaternion& quaternion ){ m_Desc.location.quaternion = quaternion; }

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

private:
  World* m_pWorld;  ///< ワールドデータ
  Area*  m_pArea;   ///< エリアデータ
  Desc  m_Desc;  ///< ジャンプ情報
  UpdateMapData m_UpdateMapData;  ///< マップデータ更新処理
};



/**
 *  @class マップジャンプイベントファクトリー
 */
class EventMapJump
{
  GFL_FORBID_COPY_AND_ASSIGN(EventMapJump); //コピーコンストラクタ＋代入禁止
public:

  enum WarpType{
    WARP_TYPE_RECOVER,  //復活
    WARP_TYPE_FLY,      //空を飛ぶ
  };  
public:

  /**
   *  @brief マップジャンプ　ゾーンのデフォルト位置にジャンプする
   */
  static GameSys::GameEvent* CallMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId);
  static GameSys::GameEvent* CallMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId, EventMapJumpCore::EventMapJumpType type);
  static GameSys::GameEvent* CreateMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId, EventMapJumpCore::EventMapJumpType type, bool isCall = false );

  /**
   *  @brief マップジャンプ　ゾーンの特定位置にジャンプする
   */
  static EventMapJumpCore*   CallMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos );
  static EventMapJumpCore*   CallMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName = true );
  static EventMapJumpCore*   CreateMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName = true, bool isCall = false );

  static GameSys::GameEvent* CallMapJumpGameOver( GameSys::GameManager* p_man );
  static GameSys::GameEvent* CallMapJumpGameOver( GameSys::GameManager* p_man, EventMapJumpGameOver::EventMapJumpType type);
  static GameSys::GameEvent* CreateMapJumpGameOver( GameSys::GameManager* p_man, EventMapJumpGameOver::EventMapJumpType type, bool isCall = false );


  /**
   *  @brief 脱出マップジャンプ
   */
  static GameSys::GameEvent* CallMapJumpEscape( GameSys::GameManager* p_man, EventMapJumpCore::EventMapJumpType type );

  /**
   *  @brief マップジャンプ　マップジャンプ配置情報を使用してジャンプする
   */
  static GameSys::GameEvent* CallMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump );
  static GameSys::GameEvent* CallMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump, EventMapJumpCore::EventMapJumpType type );
  static GameSys::GameEvent* CreateMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump, EventMapJumpCore::EventMapJumpType type, bool isCall = false );


  /**
   *  @brief アローラ時間の各世界にマップにジャンプする。
   *
   *  @param  type 各世界軸   
      ALOLA_OTHER_WOLRD_TYPE_NIKKOU,    ///< 別世界タイプ　6時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_GEKKOU,    ///< 別世界タイプ 18時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_MAX,       ///< 通常時間に戻す
   */
  static GameSys::GameEvent* CallMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type );
  static GameSys::GameEvent* CallMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type, EventMapJumpCore::EventMapJumpType type );
  static EventMapJumpCore*   CreateMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName = true, bool isCall = false );

  /**
   *  @brief マップジャンプ　フィールドが無い状態からゾーンの特定位置にジャンプする
   */
  static GameSys::GameEvent* CallMapJumpPosWithoutField( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos );

  static GameSys::GameEvent* CallMapJumpPosWithoutField( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos,
    EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName = true, bool bgmKeepFlag = false );

  static GameSys::GameEvent* CallMapJumpPosWithoutField( GameSys::GameManager* p_man, const u32 warp_id, WarpType warp_type,
    EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName = true, bool bgmKeepFlag = false );

  static EventMapJumpWithoutField*   CreateMapJumpPosWithoutField( GameSys::GameManager* p_man,
    ZoneID zoneId, const gfl2::math::Vector3& pos,
    EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName = true, bool bgmKeepFlag = false, bool isCall = false );

private:
};


GFL_NAMESPACE_END(Field);

#endif  // __EVENT_MAP_JUMP_H__


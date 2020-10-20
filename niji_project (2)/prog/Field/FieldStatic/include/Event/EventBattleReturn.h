//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file    EventBattleReturn.h
 *  @brief  バトル終了処理イベント
 *  @author  tomoya takahshi
 *  @author  tamada
 *  @date    2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.06.02
 */
//=============================================================================

#if !defined( EVENT_BATTLE_RETURN_H_INCLUDED )
#define EVENT_BATTLE_RETURN_H_INCLUDED
#pragma once

// gamesys
#include "GameSys/include/GameEvent.h"

// battle
#include <Battle/include/battle_proc.h>

// zukan
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"

// evolution
#include "NetStatic/NetEvent/include/EvolutionEvent.h"

// app
#include "App/Kawaigari/include/KawaigariProc.h"

// conv_header
#include "niji_conv_header/field/WeatherKindDef.h"

// 前方参照
namespace Field {
  class ZoneDataLoader;
  class BattleReturnBGM;
}
namespace PokeTool {
  class KawaigariParamCareCoreManager;
}

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------------------------------------
/**
 * バトルからの戻り値反映の状態遷移管理
 */
//-----------------------------------------------------------------------------
class EventBattleReturn : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleReturn); // コピーコンストラクタ＋代入禁止

public:

  /**
   * @struct セットアップパラメーター
   */
  struct SetupParam
  {
    SetupParam()
      : weather( weather::FORCE_WEATHER_NONE)
    {}

    Field::weather::WeatherKind     weather;      ///< フィールド天候
  };
  
public:

  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------
  EventBattleReturn( gfl2::heap::HeapBase* heap );

  //--------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------
  virtual ~EventBattleReturn();

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);


  //----------------------------------------------------------------------------
  /**
   *  @brief  バトルパラメータ設定
   */
  //-----------------------------------------------------------------------------
  void SetBattleParam( const BATTLE_SETUP_PARAM * cp_bpara, const ZoneDataLoader * cp_zone_loader );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 釣りデータの更新
   *
   * @param p_gman ゲームマネージャークラス
   */
  /* -------------------------------------------------------------------------*/
  void UpdateFishingParam( GameSys::GameManager* p_gman );

  //----------------------------------------------------------------------------
  /**
   *  @brief  セットアップパラメータ設定
   */
  //-----------------------------------------------------------------------------
  void SetSetupParam( const SetupParam& param );


private:

  //--------------------------------------------------------------
  // 特性の反映処理
  //--------------------------------------------------------------
  void ReflectTokuseiSizenkaihuku( pml::PokeParty * my_party );
  void ReflectTokuseiMonohiroiMitsuatsume( pml::PokeParty * my_party, gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager );


  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize);


  void SetResultDirt( pml::PokeParty* pParty);
  pml::pokepara::DirtType GetResultDirtType( const pml::pokepara::CoreParam* pPokeParam, const PokeResult& result);
  u32 GetResultDirtNum( const pml::pokepara::CoreParam* pPokeParam, const PokeResult& result);

  u32 GetPokePartyReserveNum( gfl2::heap::HeapBase* pHeap, GameSys::GameData* pGameData) const ;

  bool IsReplacedPartyIndexByCapturePokemon( u32 pokePartyIndex ) const ;

private:

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // ものひろいデータ単体
  struct MonohiroiDataCore
  {
    u16   itemID;
    u8    prob[10];
  };

  // ものひろいデータ全体
  struct MonohiroiData
  {
    u32               num;
    MonohiroiDataCore core[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

private:
  
  // バトルパラメータ
  const BATTLE_SETUP_PARAM * m_cpParam;
  const ZoneDataLoader * m_cpZoneDataLoader;

  // 図鑑登録
  app::event::ZukanRegisterEventParam m_zukanRegisterEventParam;
  pml::pokepara::PokemonParam* m_pCapturePoke;

  // 進化画面
  NetEvent::Evolution::EvolutionEventResult m_evolutionEventResult[ pml::PokeParty::MAX_MEMBERS ];
  bool m_levelup[ pml::PokeParty::MAX_MEMBERS ]; ///< レベルアップしたか？
  bool m_turnedLevelup[ pml::PokeParty::MAX_MEMBERS ];  ///<3DSさかさまでレベルアップしたか？

  // お手入れ
  App::Kawaigari::KawaigariProcParam m_kawaigariProcParam;
  PokeTool::KawaigariParamCareCoreManager* m_pKawaigariParamCareCoreManager;

  s32 m_Seq;
  u32 m_PokeCount;

  SetupParam    m_param;

  BattleReturnBGM*  m_pBattleReturnBGM;
};
  
GFL_NAMESPACE_END( Field )

#endif  // EVENT_BATTLE_RETURN_H_INCLUDED

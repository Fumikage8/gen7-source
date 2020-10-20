//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file    EventBattleReturn.cpp
 *  @brief  バトル終了処理イベント
 *  @author  tomoya takahshi
 *  @author  tamada
 *  @date    2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.06.02
 */
//=============================================================================

#include "Field/FieldStatic/include/Event/EventBattleReturn.h"
#include "./BattleReturnBGM.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

// sound
#include "Sound/include/Sound.h"

// gamesys
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameManager.h"

// system
#include "System/include/GflUse.h"

// field
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

// pmlib
#include <pml/include/pmlib.h>

// conv_header
#include <niji_conv_header/trainer/trid_def.h>

// poke_tool
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/PokeMemo.h"
#include "PokeTool/include/NakayoshiManager.h"
#include "PokeTool/include/EvolutionUtil.h"
#include "PokeTool/include/KawaigariParamCareCoreManager.h"
#include "PokeTool/include/PokeToolPokeGet.h"

// trainer
#include "Trainer/Trainer/include/tr_tool.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/field_tokusei.gaix>

// app
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"

// save
#include "Savedata/include/BoxPokemonSave.h"


GFL_NAMESPACE_BEGIN( Field )

// みつあつめ用確率テーブル      
static  const  u8  MitsuatsumeProb[]={
   5,    // ( 1 -  10)
  10,    // (11 -  20)
  15,    // (21 -  30)
  20,    // (31 -  40)
  25,    // (41 -  50)
  30,    // (51 -  60)
  35,    // (61 -  70)
  40,    // (71 -  80)
  45,    // (81 -  90)
  50,    // (91 - 100)
};

//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
EventBattleReturn::EventBattleReturn( gfl2::heap::HeapBase* heap )
  : GameSys::GameEvent( heap )
  , m_cpParam( NULL)
  , m_cpZoneDataLoader( NULL)
  , m_zukanRegisterEventParam()
  , m_pCapturePoke( NULL)
  , m_kawaigariProcParam()
  , m_pKawaigariParamCareCoreManager( NULL)
  , m_Seq( 0)
  , m_PokeCount( 0)
  , m_param()
  , m_pBattleReturnBGM( NULL )
{
  gfl2::std::MemClear( m_levelup, sizeof( m_levelup));
  gfl2::std::MemClear( m_turnedLevelup, sizeof( m_turnedLevelup));

  for( u32 i = 0; i < pml::PokeParty::MAX_MEMBERS; ++i)
  {
    m_evolutionEventResult[i] = NetEvent::Evolution::EvolutionEventResult();
  }
}

//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
EventBattleReturn::~EventBattleReturn()
{
}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------

bool EventBattleReturn::BootChk( GameSys::GameManager* p_gman )
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------

void EventBattleReturn::InitFunc( GameSys::GameManager* /*p_gman*/ )
{
  SAITA_PRINT( "Start EventBattleReturn\n");

  gfl2::heap::HeapBase* p_event_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
  // バトル後BGM管理インスタンスの生成と初期化
  {
    BattleReturnBGM::Description desc;
    desc.battleWinBGM = m_cpParam->btlEffData.bgm_win;
    m_pBattleReturnBGM = GFL_NEW( p_event_heap ) BattleReturnBGM( desc );
  }

  m_Seq = 0;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------

GameSys::GMEVENT_RESULT EventBattleReturn::MainFunc( GameSys::GameManager* p_gman )
{
  enum
  {
    SEQ_INIT,                     // 戻り値チェック

    SEQ_EVOLVE_CHECK,             // 進化チェック
    SEQ_CALL_EVOLVE_LOOP,         // 進化画面コール
    SEQ_EVOLVE_WAIT,              // 進化画面終了待ち

    SEQ_ZUKAN_REGISTER_CHECK,     // 図鑑登録チェック
    SEQ_CALL_CAPTURE_REGISTER,    // 図鑑登録アプリへ

    SEQ_OTEIRE_CHECK,             // お手入れチェック
    SEQ_OTEIRE_LOAD_WAIT,         // お手入れデータ読み込み待ち
    SEQ_CALL_OTEIRE,              // お手入れコール

    SEQ_END,                      // 終了
  };

  GameSys::GameData* p_gdata = p_gman->GetGameData( );

  switch ( m_Seq )
  {
    // 戻り値チェック
  case SEQ_INIT:
    if ( ( m_cpParam->competitor != BTL_COMPETITOR_WILD ) &&
        ( m_cpParam->competitor != BTL_COMPETITOR_TRAINER ) )
    {
      //野生戦・トレーナー戦以外はパス！！！
      m_Seq = SEQ_END;
      break;
    }

    // バトルパラメータの書き戻し。
    {
      gfl2::heap::HeapBase* p_event_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

      u32 markingIndex = p_gdata->GetPlayerParty()->GetMarkingIndex();
      // レベルアップチェック
      // パラメータ書き戻し後の進化チェックに使用する
      {
        const pml::PokeParty* old_party = p_gdata->GetPlayerParty();
        const pml::PokeParty* new_party = m_cpParam->party[ BTL_CLIENT_PLAYER ];
        u32 member_count = new_party->GetMemberCount();
        GFL_ASSERT( member_count == old_party->GetMemberCount() );
        for( u32 i=0; i<member_count; i++ )
        {
          u32 old_level = old_party->GetMemberPointerConst(i)->GetLevel();
          u32 new_level = new_party->GetMemberPointerConst(i)->GetLevel();
          m_levelup[i] = ( old_level < new_level );
          m_turnedLevelup[i] = GFL_BOOL_CAST( m_cpParam->turnedLvUpPokeIndex[i]);
        }

        // ポケモンparty書き戻し
        p_gdata->GetPlayerParty( )->CopyFrom( *m_cpParam->party[BTL_CLIENT_PLAYER] );
      }

      // かわいがり 満腹度となでなで度更新
      {
        pml::PokeParty* party = p_gdata->GetPlayerParty();
        PokeTool::nakayoshi::UpdateFeedAndNadenadeAfterBattle( party, m_cpParam->fightPokeIndex );
      }

      // ポケルス感染＆伝染チェック
      pml::PokeParty* party = p_gdata->GetPlayerParty();
      party->PokerusCatchCheck();
      party->PokerusInfectionCheck();

      // 実装 ものひろい＆みつあつめチェック（勝利時のみ）
      if ( EventBattleCall::IsWinResult( m_cpParam->result ) )
      {
        gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
        ReflectTokuseiMonohiroiMitsuatsume( party, p_event_heap, pAsyncFileManager );
      }

      //ミノムッチフォルムチェンジチェック
      PokeTool::form::UpdateMinomuttiForm( p_gdata, m_cpParam->fightPokeIndex,
          m_cpParam->fieldSituation.bgComponent.minomuttiFormNo );

      //ケルディオのしんぴの剣チェック
      PokeTool::form::UpdateKerudhioForm( p_gdata );

      //特性「しぜんかいふく」のチェック
      ReflectTokuseiSizenkaihuku( party );

      // バトルで取得したお金を追加
      if( 0 < m_cpParam->getMoney ) 
      {
        GameSys::GameData* game_data = p_gman->GetGameData( );
        Savedata::Misc* misc = game_data->GetMisc();
        misc->AddGold( m_cpParam->getMoney );
      }
      else if ( m_cpParam->getMoney < 0 )
      {
        //バトルで負けた時はお金を失う(バトルからの戻し処理でLOSE/DRAWの時しかgetMoneyはマイナスにならない)
        p_gman->GetGameData()->GetMisc()->SubGold( - m_cpParam->getMoney );
      }

      // 釣りパラメータを初期化
      UpdateFishingParam( p_gman );

      // 捕獲したポケモン保存
      if( m_cpParam->result == BTL_RESULT_CAPTURE)
      {
        m_pCapturePoke  = m_cpParam->party[BTL_CLIENT_ENEMY1]->GetMemberPointer( m_cpParam->capturedPokeIdx );

        p_gman->GetGameData()->GetLastBattleResultData()->captureMonsNo = m_pCapturePoke->GetMonsNo();

        // @fix 川田さん要望：ここで行っていたポケメモ設定はPokeTool::UpdatePokeAtPokeGetで行うので削除

        //ヒールボールの効果発現 GFBTS1086
        {
          if( m_pCapturePoke->GetGetBall() == item::BALLID_HIIRUBOORU ){
            m_pCapturePoke->RecoverAll(); //全回復
          }
        }
      }
    }
    m_Seq = SEQ_EVOLVE_CHECK;
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  // 進化チェック
  case SEQ_EVOLVE_CHECK:
    {
      m_PokeCount = 0;
      m_Seq = SEQ_CALL_EVOLVE_LOOP;
    }
    /* FALL THROUGH */

  // 進化画面コール
  case SEQ_CALL_EVOLVE_LOOP:
    {
      pml::PokeParty* party = p_gdata->GetPlayerParty();
      u32 member_count = party->GetMemberCount();
      for( ; m_PokeCount < member_count; m_PokeCount ++ )
      {
        if( m_levelup[m_PokeCount] == false )
        {
          continue;
        }

        pml::pokepara::EvolveSituation situation;
        PokeTool::SetupEvolveSituation( &situation, p_gman, m_turnedLevelup[m_PokeCount], m_param.weather );

        pml::pokepara::PokemonParam* poke = party->GetMemberPointer(m_PokeCount);
        MonsNo next_monsno = MONSNO_NULL;
        u32 evolution_root = 0;
        if( poke->CanEvolve( &situation, party, &next_monsno, &evolution_root ) == false )
        {
          continue;
        }

        // @fix GFNMCat[1575] 「手持ちの空きを呼び出し元で予約する数」を取得する
        gfl2::heap::HeapBase* p_event_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        u32 pokePartyReserveNum = this->GetPokePartyReserveNum( p_event_heap->GetLowerHandle(), p_gdata);

        NetEvent::Evolution::EvolutionEventParam param;
        param.m_pTargetPokemon    = poke;
        param.m_MonsNoEvolve      = next_monsno;
        param.m_EvolveRootNum     = evolution_root;
        // @fix NMCat[66] ヌケニン発生確認用のパラメータを初期化する
        param.m_pPokeParty        = party;
        param.m_pMyItem           = p_gdata->GetMyItem();
        // @fix GFNMCat[1575] 上で求めた「手持ちの空きを呼び出し元で予約する数」を設定する
        param.m_PokePartyReserveNum = pokePartyReserveNum;
        // 草野さん要望：進化したポケモンは汚れをなしにするため進化画面の結果を受け取る
        param.m_pResult           = &(m_evolutionEventResult[ m_PokeCount ]);
        NetEvent::Evolution::EvolutionEvent::StartEvent( p_gman, param);

        SAITA_PRINT( "Call EvolutionEvent : PokeCount = %f\n", m_PokeCount);

        m_Seq = SEQ_EVOLVE_WAIT; //進化画面へ
        
        // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
        // 進化画面のBGMを変更リクエスト (以前は無音にしていたが、その判定と処理はBattleReturnBGMに移譲)
        m_pBattleReturnBGM->ChangeBGMReq( BattleReturnBGM::SEQ_EVOLVE );

        return GameSys::GMEVENT_RES_CONTINUE;
      }
    }

    m_Seq = SEQ_ZUKAN_REGISTER_CHECK;
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  // 進化画面終了待ち
  case SEQ_EVOLVE_WAIT:
    // 進化デモのプロセスがいなくなったら先にすすむ
    if ( p_gman->IsProcessExists() == false )
    {
      m_PokeCount ++;
      m_Seq = SEQ_CALL_EVOLVE_LOOP;
    }
    break;

  // 図鑑登録画面を呼び出すかチェック
  case SEQ_ZUKAN_REGISTER_CHECK:
    {
      if( m_cpParam->result != BTL_RESULT_CAPTURE)
      {
        m_Seq = SEQ_OTEIRE_CHECK;
        break;
      }

      gfl2::heap::HeapBase* p_event_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

      // Checker、Eventで共通のものを使いたいのでここで初期化
      m_zukanRegisterEventParam.pp = m_pCapturePoke;

      // @fix 川田さん要望：Savedata::ZukanRegisterChecker::CheckThenPutをPokeTool::UpdatePokeAtPokeGetに置き換え
      PokeTool::PokeGetUtil::ReturnType::Tag tag;
      tag = PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
        p_event_heap,
        m_zukanRegisterEventParam.pp,
        m_zukanRegisterEventParam.callerType
        );

      // 図鑑登録画面アプリへ
      if( tag == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER)
      {
        m_Seq = SEQ_CALL_CAPTURE_REGISTER;
      }
      // お手入れチェックへ
      else
      {
        m_Seq = SEQ_OTEIRE_CHECK;
      }
    }
    break;

  // 図鑑登録画面アプリコール
  case SEQ_CALL_CAPTURE_REGISTER:
    {
      SAITA_PRINT( "Call ZukanRegisterEvent\n");
      app::event::ZukanRegisterEvent::StartEvent( p_gman, &m_zukanRegisterEventParam);

      // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
      // 図鑑画面のBGM変更リクエスト
      m_pBattleReturnBGM->ChangeBGMReq( BattleReturnBGM::SEQ_ZUKAN );

      m_Seq = SEQ_OTEIRE_CHECK;
    }
    break;

  // お手入れチェック
  case SEQ_OTEIRE_CHECK:
    {
      if( m_cpParam->oteireFlag)
      {
        gfl2::heap::HeapBase* pEventHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
        m_pKawaigariParamCareCoreManager = GFL_NEW( pEventHeap) PokeTool::KawaigariParamCareCoreManager( pEventHeap, pEventHeap->GetLowerHandle(), pAsyncFileManager);

        m_Seq = SEQ_OTEIRE_LOAD_WAIT;
      }
      else
      {
        m_Seq = SEQ_END;
      }
    }
    break;

  // お手入れデータ読み込み待ち
  case SEQ_OTEIRE_LOAD_WAIT:
    {
      if( m_pKawaigariParamCareCoreManager->IsLoaded())
      {
        m_Seq = SEQ_CALL_OTEIRE;
      }
    }
    break;

  // お手入れ画面コール
  case SEQ_CALL_OTEIRE:
    {
      pml::PokeParty* party = p_gdata->GetPlayerParty();

      // 汚れを設定
      this->SetResultDirt( party);

      // @fix GFNMCat[1852] 【ポケリフレ】戦闘終了時、よごれが発生しているポケモンと捕獲したポケモンを入れ替えると、捕獲したポケモンがよごれてしまいます
      // お手入れ対象indexのパーティーメンバーが、捕獲ポケモンと交換されていた場合でも、そのまま捕獲ポケモンをお手入れする
      // 捕まえたポケモンをすぐにリフレで見れるというメリットもある等、協議の上での決定
      const u32 fixOteireTargetIndex = m_cpParam->oteireTargetIndex;

      m_kawaigariProcParam.bgParam.groundType   = m_cpParam->fieldSituation.bgComponent.groudnType;
      m_kawaigariProcParam.bgParam.nearType     = m_cpParam->fieldSituation.bgComponent.nearType;
      m_kawaigariProcParam.bgParam.farType      = m_cpParam->fieldSituation.bgComponent.farType;
      m_kawaigariProcParam.bgParam.skyType      = static_cast<System::Skybox::Skybox::SkyType>(m_cpParam->fieldSituation.sky_type);
      m_kawaigariProcParam.pokemonParam         = party->GetMemberPointer( fixOteireTargetIndex );
      m_kawaigariProcParam.mode                       = App::Kawaigari::KAWAIGARI_MODE_NORMAL;

      SAITA_PRINT( "Call KawaigariEvent\n");
      App::Event::PokeParureEvent* pEvent = App::Event::PokeParureEvent::StartEvent( p_gman);
      pEvent->SetupParam( &m_kawaigariProcParam, fixOteireTargetIndex );

      // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
      // お手入れ画面のBGM変更リクエスト
      m_pBattleReturnBGM->ChangeBGMReq( BattleReturnBGM::SEQ_OTEIRE );

      m_Seq = SEQ_END;
    }
    break;

  // 終了
  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleReturn::EndFunc( GameSys::GameManager* p_gman )
{
  // @fix NMCat[11] 戦闘終了後に複数処理を重ねるとBGMがなくなる
  // バトル後BGM管理インスタンスの破棄
  GFL_SAFE_DELETE( m_pBattleReturnBGM );

  if( m_pKawaigariParamCareCoreManager)
  {
    m_pKawaigariParamCareCoreManager->Finalize();
    GFL_DELETE( m_pKawaigariParamCareCoreManager);
    m_pKawaigariParamCareCoreManager = NULL;
  }

  // 汚れのクリア
  pml::PokeParty* party = p_gman->GetGameData()->GetPlayerParty();
  for( u32 i = 0; i < party->GetMemberCount(); ++i)
  {
    party->GetMemberPointer( i)->ClearDirt();
  }

  SAITA_PRINT( "End EventBattleReturn\n");
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルパラメータ設定
 */
//-----------------------------------------------------------------------------
void EventBattleReturn::SetBattleParam( const BATTLE_SETUP_PARAM * cp_bpara, const ZoneDataLoader * cp_zone_loader )
{
  m_cpParam = cp_bpara;
  m_cpZoneDataLoader = cp_zone_loader;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 釣りデータの更新
 *
 * @param p_gman ゲームマネージャークラス
 */
/* -------------------------------------------------------------------------*/
void EventBattleReturn::UpdateFishingParam( GameSys::GameManager* p_gman )
{
  if( BATTLE_PARAM_CheckBtlStatusFlag( m_cpParam, BTL_STATUS_FLAG_FISHING ) )
  {
    // レアチェック
    pml::pokepara::PokemonParam* p_enemy = m_cpParam->party[BTL_CLIENT_ENEMY1]->GetMemberPointer( 0 );
    if( p_enemy->IsRare() )
    {
      // 釣り継続回数リセット
      Encount::EncountWork* p_encwork = p_gman->GetGameData()->GetEncountWork();
      p_encwork->ClearFishingContinueCount();
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  セットアップパラメータ設定
 */
//-----------------------------------------------------------------------------
void EventBattleReturn::SetSetupParam( const SetupParam& param )
{
  m_param = param;
}

//-----------------------------------------------------------------------------
/**
 * @brief 特性：しぜんかいふくの反映処理
 */
//-----------------------------------------------------------------------------
void EventBattleReturn::ReflectTokuseiSizenkaihuku( pml::PokeParty * my_party )
{
  int count = my_party->GetMemberCount();
  for ( int i = 0; i < count; ++ i )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer( i );
    if ( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) continue;
    MonsNo monsno = pp->GetMonsNo();
    if ( monsno != 0 && pp->GetTokuseiNo() == TOKUSEI_SIZENKAIHUKU )
    {
      pp->SetSick( pml::pokepara::SICK_NULL );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief 特性：ものひろい＆みつあつめ反映処理
 */
//-----------------------------------------------------------------------------
void EventBattleReturn::ReflectTokuseiMonohiroiMitsuatsume( pml::PokeParty * my_party, gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager )
{
  this->SyncArcOpen( pHeap->GetLowerHandle(), pAsyncFileManager, ARCID_FIELD_TOKUSEI);

  void* pMonohiroiBin = NULL;
  this->SyncArcLoad( pHeap, pAsyncFileManager, ARCID_FIELD_TOKUSEI, GARC_field_tokusei_monohiroi_COMP, &pMonohiroiBin, NULL);

  this->SyncArcClose( pHeap, pAsyncFileManager, ARCID_FIELD_TOKUSEI);

  s32 count = my_party->GetMemberCount();
  for ( s32 i = 0; i < count; ++ i )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer( i );
    MonsNo monsno = pp->GetMonsNo();

    // モンスターIDが不正、モンスターがタマゴ、モンスターがアイテムを持っている
    // いずれかの条件だったら、ものひろい、みつあつめ、は行われない
    if ( monsno == MONSNO_NULL || pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) || pp->GetItem() != ITEM_DUMMY_DATA )
    {
      continue;
    }

    // ものひろいチェック
    if ( pp->GetTokuseiNo() == TOKUSEI_MONOHIROI && System::GflUse::GetPublicRand( 10) == 0 )
    {
      MonohiroiData* pMonohiroiData = reinterpret_cast<MonohiroiData*>( pMonohiroiBin);

      u32 rnd = System::GflUse::GetPublicRand( 100);
      s32 LvOffset = ( pp->GetLevel() - 1 ) / 10;

      u16 itemID = ITEM_DUMMY_DATA;
      u8 probTotal = 0;
      for( u32 j = 0; j < pMonohiroiData->num; ++j)
      {
        probTotal += pMonohiroiData->core[j].prob[ LvOffset ];
        if( rnd < probTotal)
        {
          itemID = pMonohiroiData->core[j].itemID;
          break;
        }
      }
      GFL_ASSERT( itemID != ITEM_DUMMY_DATA); // データ上これはありえない(合計値が100になるのを担保している)
      pp->SetItem( itemID );

      Savedata::IncRecord( Savedata::Record::RECID_MONOHIROI);
      // @fix NMCat[100] ものひろいダイアリーは１:ポケモンの全国図鑑番号、２:アイテム番号
      PokeDiary::Set( PDID_COUNT_MONOHIROI, pp->GetMonsNo(), itemID);

    }
    //みつあつめチェック
    else if ( pp->GetTokuseiNo() == TOKUSEI_MITUATUME )
    {
      s32 LvOffset = (pp->GetLevel() - 1 ) / 10;
      GFL_ASSERT( LvOffset < GFL_NELEMS(MitsuatsumeProb) );
      if ( System::GflUse::GetPublicRand( 100 ) < MitsuatsumeProb[ LvOffset ] )
      {
        pp->SetItem( ITEM_AMAIMITU );
      }
    }
  }

  GflHeapSafeFreeMemory( pMonohiroiBin);
}

/**
 *  @brief  同期arcオープン
 */
void EventBattleReturn::SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = arcID;
  openReq.heapForFile = pHeap;
  openReq.heapForArcSrc = pHeap;
  openReq.heapForReq = pHeap->GetLowerHandle();
  pAsyncFileManager->SyncArcFileOpen( openReq );
}

/**
 *  @brief  同期arcクローズ
 */
void EventBattleReturn::SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
  closeReq.arcId = arcID;
  closeReq.heapForReq = pHeap;
  pAsyncFileManager->SyncArcFileClose( closeReq );
}

/**
 *  @brief  同期データ読み込み
 */
void EventBattleReturn::SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = arcID;
  loadReq.datId = datID;
  loadReq.ppBuf = ppBuff;
  loadReq.pBufSize = pSize;
  loadReq.heapForBuf = pHeap;
  loadReq.align = 4;
  loadReq.heapForCompressed = pHeap->GetLowerHandle();
  pAsyncFileManager->SyncArcFileLoadData( loadReq );
}

/**
 *  @brief パーティーのお手入れ対象ポケモンによごれを設定
 *  @param pParty ポケモンパーティー
 */
void EventBattleReturn::SetResultDirt( pml::PokeParty* pParty)
{
  for( u32 i = 0; i < pParty->GetMemberCount(); ++i)
  {
    pml::pokepara::CoreParam* pPokeParam = pParty->GetMemberPointer( i);

    // 念のためクリアしておく
    pPokeParam->ClearDirt();

    // @fix GFNMCat[1852] 捕獲ポケモンと入れ替えられていた場合はクリア状態のままにする
    if( this->IsReplacedPartyIndexByCapturePokemon( i ) )
    {
      continue;
    }

    // 草野さん要望：進化していたらクリア状態のままにする
    if( m_evolutionEventResult[ i ].m_IsEvolved == true )
    {
      continue;
    }

    // 戦闘結果と設定データからよごれの種類を取得する
    const PokeResult& result = m_cpParam->pokeResult[i];
    pml::pokepara::DirtType dirtType = this->GetResultDirtType( pPokeParam, result);

    // よごれがなければ何もしない
    if( dirtType == pml::pokepara::DIRT_TYPE_NONE)
    {
      continue;
    }

    // 汚れ種類を設定
    pPokeParam->SetDirtType( dirtType);

    // 汚れ位置を設定
    u32 dirtNum = this->GetResultDirtNum( pPokeParam, result);
    for( u32 pos = 0; pos < dirtNum; ++pos)
    {
      pPokeParam->SetDirtPos( pos);
    }
  }
}

/**
 *  @brief よごれ種類を選択
 *  @param pPokeParam ポケモンパラメーター
 *  @param partyIndex パーティー内位置
 *  @return よごれ種類
 */
pml::pokepara::DirtType EventBattleReturn::GetResultDirtType( const pml::pokepara::CoreParam* pPokeParam, const PokeResult& result)
{
  // 仕様変更。バトルからセットされたものをそのまま返す
  return result.dirtType;
}

/**
 *  @brief よごれ数を取得
 *  @param pPokeParam ポケモンパラメーター
 *  @param partyIndex パーティー内位置
 *  @return よごれ数
 */
u32 EventBattleReturn::GetResultDirtNum( const pml::pokepara::CoreParam* pPokeParam, const PokeResult& result)
{
  MonsNo monsNo = pPokeParam->GetMonsNo();
  FormNo formNo = pPokeParam->GetFormNo();

  // ターン経過数=よごれ数 (上限5箇所)
  u32 dirtNum = result.totalTurnCount;
  if( dirtNum > 5)
  {
    dirtNum = 5;
  }

  // ポケモンのジョイント数による上限チェック
  u32 jointCount = m_pKawaigariParamCareCoreManager->GetJointCount( monsNo, formNo, pPokeParam->GetSex());
  if( dirtNum > jointCount)
  {
    dirtNum = jointCount;
  }

  return dirtNum;
}

/**
 *  @brief 手持ちの空きを呼び出し元で予約する数を取得
 *  @param pHeap 一時確保に使用するヒープ
 *  @param pGameData ゲームデータ
 *  @return 手持ちの空きを呼び出し元で予約する数
 */
u32 EventBattleReturn::GetPokePartyReserveNum( gfl2::heap::HeapBase* pHeap, GameSys::GameData* pGameData) const
{
  u32 pokePartyReserveNum = 0;

  // @fix GFNMCat[1575] 手持ちの空きがひとつで、且つBOXが満杯のときに戦闘相手を捕獲した結果ツチニンを進化させ、ヌケニンを発生させると、捕獲対象の格納先がなくなってしまう
  // ヌケニン発生条件は、
  // ・進化後のポケモンがヌケニンである
  // ・「手持ち」に空きがある
  // ・「モンスターボール」を持っている
  // 捕獲ポケモンがいてボックスに空きがない場合は、捕獲ポケモンの格納先が手持ちしかないので「手持ちの空きを呼び出し元で予約する数」をインクリメント
  if( m_pCapturePoke)
  {
    u32 boxPokeSpaceNum = pGameData->GetBoxPokemon()->GetSpaceCountAll( pHeap );
    if( boxPokeSpaceNum == 0 )
    {
      pokePartyReserveNum++;
    }
  }

  return pokePartyReserveNum;
}

/**
 *  @brief 捕獲ポケモンと入れ替えられたポケパーティー位置か
 *  @param pokePartyIndex 比較するパーティー位置
 *  @retval true 入れ替えられた位置である
 *  @retval false それ以外
 */
bool EventBattleReturn::IsReplacedPartyIndexByCapturePokemon( u32 pokePartyIndex ) const
{
  const PokeTool::PokeGetUtil::ReturnParam& ret = m_zukanRegisterEventParam.returnParam;

  if( ret.isValid == false )
  {
    // 結果に有効な値が入っていない
    return false;
  }

  // 配置区分が手持ち
  if( ret.putPlace == PokeTool::PokeGetUtil::PutPlace::PARTY )
  {
    // 配置位置が、比較したい位置と等しい
    if( ret.pos0 == pokePartyIndex )
    {
      // このパーティー位置は捕獲ポケモンである
      return true;
    }
  }

  // 入れ替えられていなかった
  return false;
}

GFL_NAMESPACE_END( Field )

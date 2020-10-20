//=============================================================================
/**
 * @file    TrainerDataTest.cpp
 * @brief   トレーナーデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.20
 */
//=============================================================================
#if PM_DEBUG

#include <pml/include/pml_PokePara.h>
#include <System/include/HeapDefine.h>
#include <Print/include/WordSet.h>
#include <Trainer/Trainer/include/tr_tool.h>
#include <Trainer/Trainer/include/TrainerName.h>
#include "../include/TrainerDataTest.h"

#include <niji_conv_header/trainer/trtype_def.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
TrainerDataTestProc::TrainerDataTestProc( void ) :
  GameSys::GameProc(),
  ObataTestBase(),
  m_trainerId( 1 )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
TrainerDataTestProc::~TrainerDataTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerDataTestProc::InitFunc( gfl2::proc::Manager* procManager )
{
  this->SetupBase();

  GFL_PRINT( "\n" );
  GFL_PRINT( "操作方法\n" );
  GFL_PRINT( "START   終了\n" );
  GFL_PRINT( "UP      トレーナーID + 1\n" );
  GFL_PRINT( "RIGHT   トレーナーID +10\n" );
  GFL_PRINT( "DOWN    トレーナーID - 1\n" );
  GFL_PRINT( "LEFT    トレーナーID -10\n" );

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerDataTestProc::EndFunc( gfl2::proc::Manager* procManager )
{
  this->CleanupBase();
  return gfl2::proc::RES_FINISH;
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス更新関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerDataTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  const gfl2::ui::Button* uiButton = this->UiButton();

  this->UpdateTrainerId();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) ) 
  {
    trainer::TrainerID trid = static_cast<trainer::TrainerID>( m_trainerId );
    this->DumpTrainerName( trid );
    this->DumpTrainerData( trid );
    this->DumpTrainerPokeData( trid );
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) ) {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief 操作対象のトレーナーIDを更新する
 */
void TrainerDataTestProc::UpdateTrainerId( void )
{
  s32 prev_trainerId = m_trainerId;

  const gfl2::ui::Button* uiButton = this->UiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_UP    ) ) m_trainerId += 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_DOWN  ) ) m_trainerId -= 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) m_trainerId += 10;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT  ) ) m_trainerId -= 10;

  if( m_trainerId == prev_trainerId ) {
    return;
  }

  static const s32 TRID_MIN = 1;  // TRID_NULL(0) を除く
  static const s32 TRID_RANGE = trainer::TRID_MAX - TRID_MIN;
  m_trainerId = ( ( m_trainerId - TRID_MIN + TRID_RANGE ) % TRID_RANGE ) + 1;
  GFL_PRINT( "trainerId=%d\n", m_trainerId );
}

/**
 * @brief トレーナー名をダンプする
 * @param trainerId  ダンプするトレーナーデータのID
 */
void TrainerDataTestProc::DumpTrainerName( trainer::TrainerID trainerId ) const
{
  gfl2::heap::HeapBase* heap = this->SystemHeap();
  gfl2::str::StrBuf* strbuf = this->GetStrBuf( 0 );
  trainer::TrainerName::GetTrainerName( strbuf, heap, trainerId );
  gfl2::str::PrintCode( strbuf->GetPtr() );
}

/**
 * @brief トレーナーデータをダンプする
 * @param trainerId  ダンプするトレーナーデータのID
 */
void TrainerDataTestProc::DumpTrainerData( trainer::TrainerID trainerId ) const
{
  trainer::TRAINER_DATA data;
  trainer::LoadTrainerData( this->SystemHeap(), trainerId, &data );

  GFL_PRINT( "トレーナー分類                   %d\n",   data.tr_type );
  GFL_PRINT( "1vs1 or 2vs2 or 3vs3 or rotate   %d\n",   data.fight_type );
  GFL_PRINT( "所持ポケモン数                   %d\n",   data.poke_count );
  GFL_PRINT( "使用道具[0]                      %d\n",   data.use_item[0] );
  GFL_PRINT( "使用道具[1]                      %d\n",   data.use_item[1] );
  GFL_PRINT( "使用道具[2]                      %d\n",   data.use_item[2] );
  GFL_PRINT( "使用道具[3]                      %d\n",   data.use_item[3] );
  GFL_PRINT( "AIパターン                       0x%x\n", data.aibit );
  GFL_PRINT( "戦闘後回復するか？               %d\n",   data.hp_recover_flag );
  GFL_PRINT( "おこづかい係数                   %d\n",   data.gold );
  GFL_PRINT( "戦闘後もらえるアイテム           %d\n",   data.gift_item );
}

/**
 * @brief トレーナーの手持ちポケモンデータをダンプする
 * @param trainerId  ダンプするトレーナーデータのID
 */
void TrainerDataTestProc::DumpTrainerPokeData( trainer::TrainerID trainerId ) const
{
  gfl2::heap::HeapBase* heap = this->SystemHeap();
  pml::PokeParty* party = GFL_NEW( heap ) pml::PokeParty( heap );
  trainer::EncountTrainerPokeDataMake( trainerId, party, heap );

  const u32 memberNum = party->GetMemberCount();
  for( u32 memberIndex=0; memberIndex<memberNum; ++memberIndex )
  {
    GFL_PRINT( "\n" );
    GFL_PRINT( "-------------\n" );
    const pml::pokepara::PokemonParam* param = party->GetMemberPointerConst( memberIndex );
    this->DumpPokemonParam( *param );
  }

  GFL_DELETE( party );
}

/**
 * @brief ポケモンパラメータをダンプする
 * @param param  ダンプ対象のポケモンパラメータ
 */
void TrainerDataTestProc::DumpPokemonParam( const pml::pokepara::PokemonParam& param ) const
{
  gfl2::str::StrBuf* strbuf = this->GetStrBuf( 0 );

  this->GetMonsName( strbuf, &param );
  gfl2::str::PrintCode( strbuf->GetPtr() );

  GFL_PRINT( "モンスターNo.%d\n", param.GetMonsNo() );
  GFL_PRINT( "フォルムNo.%d\n", param.GetFormNo() );
  GFL_PRINT( "Lv.%d\n", param.GetLevel() );

  this->GetTokuseiName( strbuf, param.GetTokuseiNo() );
  GFL_PRINT( "とくせい = " );
  gfl2::str::PrintCode( strbuf->GetPtr() );

  this->GetSeikakuName( strbuf, param.GetSeikaku() );
  GFL_PRINT( "せいかく = " );
  gfl2::str::PrintCode( strbuf->GetPtr() );

  GFL_PRINT( "性別 = %d\n", param.GetSex() );
  GFL_PRINT( "ボールID = %d\n", param.GetGetBall() );

  GFL_PRINT( "色違い　" );
  GFL_PRINT( param.IsRare() ? "●\n" : "\n" );

  GFL_PRINT( "\n" );
  GFL_PRINT( "HP                   = %3d\n", param.GetMaxHp() );
  GFL_PRINT( "こうげき             = %3d\n", param.GetPower( pml::pokepara::POWER_ATK ) );
  GFL_PRINT( "ぼうぎょ             = %3d\n", param.GetPower( pml::pokepara::POWER_DEF ) );
  GFL_PRINT( "とくこう             = %3d\n", param.GetPower( pml::pokepara::POWER_SPATK ) );
  GFL_PRINT( "とくぼう             = %3d\n", param.GetPower( pml::pokepara::POWER_SPDEF ) );
  GFL_PRINT( "すばやさ             = %3d\n", param.GetPower( pml::pokepara::POWER_AGI ) );

  GFL_PRINT( "\n" );
  GFL_PRINT( "個体値( HP       )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_HP ) );
  GFL_PRINT( "個体値( こうげき )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_ATK ) );
  GFL_PRINT( "個体値( ぼうぎょ )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_DEF ) );
  GFL_PRINT( "個体値( とくこう )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_SPATK ) );
  GFL_PRINT( "個体値( とくぼう )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_SPDEF ) );
  GFL_PRINT( "個体値( すばやさ )   = %3d\n", param.GetTalentPower( pml::pokepara::POWER_AGI ) );

  GFL_PRINT( "\n" );
  GFL_PRINT( "努力値( HP       )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_HP ) );
  GFL_PRINT( "努力値( こうげき )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_ATK ) );
  GFL_PRINT( "努力値( ぼうぎょ )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_DEF ) );
  GFL_PRINT( "努力値( とくこう )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_SPATK ) );
  GFL_PRINT( "努力値( とくぼう )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_SPDEF ) );
  GFL_PRINT( "努力値( すばやさ )   = %3d\n", param.GetEffortPower( pml::pokepara::POWER_AGI ) );

  GFL_PRINT( "\n" );
  GFL_PRINT( "技\n" );
  u32 wazaNum = param.GetWazaCount();
  for( u32 wazaIndex=0; wazaIndex<wazaNum; ++wazaIndex )
  {
    WazaNo wazaNo = param.GetWazaNo( wazaIndex );
    this->GetWazaName( strbuf, wazaNo );
    GFL_PRINT( "%3d ", wazaNo );
    gfl2::str::PrintCode( strbuf->GetPtr() );
  }

}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理(下準備)
 * @param procManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
//---------------------------------------------------------------------------
void TrainerDataTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void TrainerDataTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

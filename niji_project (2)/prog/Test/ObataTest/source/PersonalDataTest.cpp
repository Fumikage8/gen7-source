//=============================================================================
/**
 * @file    PersonalDataTest.cpp
 * @brief   パーソナルデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.11
 */
//=============================================================================
#if PM_DEBUG

#include <niji_conv_header/poke_lib/monsno_def.h>
#include <pml/include/pml_Personal.h>

#include <System/include/HeapDefine.h>
#include "../include/PersonalDataTest.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
PersonalDataTestProc::PersonalDataTestProc( void ) :
  GameSys::GameProc(),
  ObataTestBase(),
  m_monsno( MONSNO_HUSIGIDANE ),
  m_formno( 0 )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
PersonalDataTestProc::~PersonalDataTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result PersonalDataTestProc::InitFunc( gfl2::proc::Manager* procManager )
{
  this->SetupBase();

  GFL_PRINT( "\n" );
  GFL_PRINT( "操作方法\n" );
  GFL_PRINT( "START 終了\n" );
  GFL_PRINT( "UP    モンスターNo. +1\n" );
  GFL_PRINT( "RIGHT モンスターNo. +10\n" );
  GFL_PRINT( "DOWN  モンスターNo. -1\n" );
  GFL_PRINT( "LEFT  モンスターNo. -10\n" );

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result PersonalDataTestProc::EndFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result PersonalDataTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  const gfl2::ui::Button* uiButton = this->UiButton();

  this->UpdateMonsNo();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) ) 
  {
    this->DumpPersonalData( m_monsno, m_formno );
    this->DumpWazaOboeData( m_monsno, m_formno );
    this->DumpEvolveData( m_monsno, m_formno );
    this->DumpMegaEvolveData( m_monsno );
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) ) {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief 操作対象のモンスターNo.を更新する
 */
void PersonalDataTestProc::UpdateMonsNo( void )
{
  s32 prev_monsno = m_monsno;

  const gfl2::ui::Button* uiButton = this->UiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_UP    ) ) m_monsno += 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_DOWN  ) ) m_monsno -= 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) m_monsno += 10;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT  ) ) m_monsno -= 10;

  if( m_monsno == prev_monsno ) {
    return;
  }

  static const s32 MONSNO_NUM = ( MONSNO_END + 1 );
  m_monsno = ( m_monsno + MONSNO_NUM ) % MONSNO_NUM;
  m_formno = 0;

  GFL_PRINT( "monsno=%d, formno=%d\n", m_monsno, m_formno );
}

/**
 * @brief パーソナルデータをダンプする
 * @param monsno ダンプ対象ポケモンのモンスターNo.
 * @param formno ダンプ対象ポケモンのフォルムNo.
 */
void PersonalDataTestProc::DumpPersonalData( s32 monsno, s32 formno ) const
{
  GFL_PRINT( "\n" );
  GFL_PRINT( "パーソナルデータ\n" );

  pml::personal::LoadPersonalData( static_cast<MonsNo>( monsno ), static_cast<pml::FormNo>( formno ) );
  GFL_PRINT( "PARAM_ID_basic_hp             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_hp ) );
  GFL_PRINT( "PARAM_ID_basic_atk            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_atk ) );
  GFL_PRINT( "PARAM_ID_basic_def            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_def ) );
  GFL_PRINT( "PARAM_ID_basic_agi            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_agi ) );
  GFL_PRINT( "PARAM_ID_basic_spatk          = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_spatk ) );
  GFL_PRINT( "PARAM_ID_basic_spdef          = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_spdef ) );
  GFL_PRINT( "PARAM_ID_type1                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type1 ) );
  GFL_PRINT( "PARAM_ID_type2                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_type2 ) );
  GFL_PRINT( "PARAM_ID_get_rate             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_get_rate ) );
  GFL_PRINT( "PARAM_ID_give_exp             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_give_exp ) );
  GFL_PRINT( "PARAM_ID_pains_hp             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_hp ) );
  GFL_PRINT( "PARAM_ID_pains_atk            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_atk ) );
  GFL_PRINT( "PARAM_ID_pains_def            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_def ) );
  GFL_PRINT( "PARAM_ID_pains_agi            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_agi ) );
  GFL_PRINT( "PARAM_ID_pains_spatk          = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_spatk ) );
  GFL_PRINT( "PARAM_ID_pains_spdef          = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_pains_spdef ) );
  GFL_PRINT( "PARAM_ID_no_jump              = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_no_jump ) );
  GFL_PRINT( "PARAM_ID_item1                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_item1 ) );
  GFL_PRINT( "PARAM_ID_item2                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_item2 ) );
  GFL_PRINT( "PARAM_ID_item3                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_item3 ) );
  GFL_PRINT( "PARAM_ID_sex                  = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) );
  GFL_PRINT( "PARAM_ID_egg_birth            = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_birth ) );
  GFL_PRINT( "PARAM_ID_initial_familiarity  = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_initial_familiarity ) );
  GFL_PRINT( "PARAM_ID_grow                 = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_grow ) );
  GFL_PRINT( "PARAM_ID_egg_group1           = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_group1 ) );
  GFL_PRINT( "PARAM_ID_egg_group2           = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_group2 ) );
  GFL_PRINT( "PARAM_ID_tokusei1             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei1 ) );
  GFL_PRINT( "PARAM_ID_tokusei2             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei2 ) );
  GFL_PRINT( "PARAM_ID_tokusei3             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei3 ) );
  GFL_PRINT( "PARAM_ID_form_index           = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_index ) );
  GFL_PRINT( "PARAM_ID_form_gra_index       = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_gra_index ) );
  GFL_PRINT( "PARAM_ID_form_max             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max ) );
  GFL_PRINT( "PARAM_ID_color                = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_color ) );
  GFL_PRINT( "PARAM_ID_rank                 = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_rank ) );
  GFL_PRINT( "PARAM_ID_height               = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_height ) );
  GFL_PRINT( "PARAM_ID_weight               = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_weight ) );
  GFL_PRINT( "PARAM_ID_machine1             = 0x%x\n", pml::personal::GetPersonalParam( pml::personal::PARAM_ID_machine1 ) );
  GFL_PRINT( "PARAM_ID_machine2             = 0x%x\n", pml::personal::GetPersonalParam( pml::personal::PARAM_ID_machine2 ) );
  GFL_PRINT( "PARAM_ID_machine3             = 0x%x\n", pml::personal::GetPersonalParam( pml::personal::PARAM_ID_machine3 ) );
  GFL_PRINT( "PARAM_ID_machine4             = 0x%x\n", pml::personal::GetPersonalParam( pml::personal::PARAM_ID_machine4 ) );
  GFL_PRINT( "PARAM_ID_waza_oshie1          = 0x%x\n", pml::personal::GetPersonalParam( pml::personal::PARAM_ID_waza_oshie_kyukyoku ) );
  GFL_PRINT( "PARAM_ID_sos_rate             = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sos_rate ) );
  GFL_PRINT( "PARAM_ID_zenryoku_waza_item   = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_item ) );
  GFL_PRINT( "PARAM_ID_zenryoku_waza_before = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_before ) );
  GFL_PRINT( "PARAM_ID_zenryoku_waza_after  = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_after ) );
  GFL_PRINT( "PARAM_ID_region               = %d\n",   pml::personal::GetPersonalParam( pml::personal::PARAM_ID_region ) );
}

/**
 * @brief 技覚えデータをダンプする
 * @param monsno ダンプ対象ポケモンのモンスターNo.
 * @param formno ダンプ対象ポケモンのフォルムNo.
 */
void PersonalDataTestProc::DumpWazaOboeData( s32 monsno, s32 formno ) const
{
  GFL_PRINT( "\n" );
  GFL_PRINT( "技覚え\n" );

  pml::personal::LoadWazaOboeData( static_cast<MonsNo>( monsno ), static_cast<pml::FormNo>( formno ) );
  u32 wazaOboeNum = pml::personal::GetWazaOboeNum();
  for( u32 i=0; i<wazaOboeNum; ++i )
  {
    u32 level = pml::personal::GetWazaOboeLevel( i );
    u32 wazano = pml::personal::GetWazaOboeWazaNo( i );
    pml::personal::WazaOboeData::OboeWazaKind kind = pml::personal::GetWazaOboeKind( i );
    switch( kind ) {
    case pml::personal::WazaOboeData::OBOEWAZA_KIND_EVOLVE: GFL_PRINT( "進化　 " );  break;
    case pml::personal::WazaOboeData::OBOEWAZA_KIND_BASE:   GFL_PRINT( "基本　 " );  break;
    case pml::personal::WazaOboeData::OBOEWAZA_KIND_LEVEL:  GFL_PRINT( "レベル " );  break;
    }
    GFL_PRINT( "Lv.%3d, wazano=%d\n", level, wazano );
  }
}

/**
 * @brief 進化データをダンプする
 * @param monsno ダンプ対象ポケモンのモンスターNo.
 * @param formno ダンプ対象ポケモンのフォルムNo.
 */
void PersonalDataTestProc::DumpEvolveData( s32 monsno, s32 formno ) const
{
  GFL_PRINT( "\n" );
  GFL_PRINT( "進化\n" );

  pml::personal::LoadEvolutionTable( static_cast<MonsNo>( monsno ), static_cast<FormNo>( formno ) );
  u32 routeNum = pml::personal::GetEvolutionRouteNum();
  for( u32 i=0; i<routeNum; ++i )
  {
    u8 level = pml::personal::GetEvolveEnableLevel( i );
    pml::personal::EvolveCond cond = pml::personal::GetEvolutionCondition( i );
    u32 param = pml::personal::GetEvolutionParam( i );
    MonsNo evolvedMonsno = pml::personal::GetEvolvedMonsNo( i );
    GFL_PRINT( "monsno=%d, level=%d, cond=%d, param=%d\n", evolvedMonsno, level, cond, param );
  }
}

/**
 * @brief メガ進化データをダンプする
 * @param monsno ダンプ対象ポケモンのモンスターNo.
 */
void PersonalDataTestProc::DumpMegaEvolveData( s32 monsno ) const
{
  GFL_PRINT( "\n" );
  GFL_PRINT( "メガ進化\n" );

  pml::personal::LoadMegaEvolveTable( static_cast<MonsNo>( monsno ) );
  u32 routeNum = pml::personal::GetMegaEvolveRouteNum();
  for( u32 i=0; i<routeNum; ++i )
  {
    pml::personal::MegaEvolveCond cond   = pml::personal::GetMegaEvolveCondition( i );
    u16                           param  = pml::personal::GetMegaEvolveParam( i );
    pml::FormNo                   formno = pml::personal::GetMegaEvolvedFormNo( i );
    GFL_PRINT( "formno=%d, cond=%d, param=%d\n", formno, cond, param );
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
void PersonalDataTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void PersonalDataTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

//=============================================================================
/**
 * @file    WazaDataTest.cpp
 * @brief   技データ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.15
 */
//=============================================================================
#if PM_DEBUG

#include <niji_conv_header/poke_lib/wazano_def.h>
#include <pml/include/pml_Waza.h>

#include <System/include/HeapDefine.h>
#include "../include/WazaDataTest.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
WazaDataTestProc::WazaDataTestProc( void ) :
  GameSys::GameProc(),
  ObataTestBase(),
  m_wazano( WAZANO_NULL )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
WazaDataTestProc::~WazaDataTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result WazaDataTestProc::InitFunc( gfl2::proc::Manager* procManager )
{
  this->SetupBase();

  GFL_PRINT( "\n" );
  GFL_PRINT( "操作方法\n" );
  GFL_PRINT( "START   終了\n" );
  GFL_PRINT( "UP      技No. + 1\n" );
  GFL_PRINT( "RIGHT   技No. +10\n" );
  GFL_PRINT( "DOWN    技No. - 1\n" );
  GFL_PRINT( "LEFT    技No. -10\n" );

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result WazaDataTestProc::EndFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result WazaDataTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  const gfl2::ui::Button* uiButton = this->UiButton();

  this->UpdateWazaNo();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) ) 
  {
    this->DumpWazaData( static_cast<WazaNo>( m_wazano ) );
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) ) {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief 操作対象の技No.を更新する
 */
void WazaDataTestProc::UpdateWazaNo( void )
{
  s32 prev_wazano = m_wazano;

  const gfl2::ui::Button* uiButton = this->UiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_UP    ) ) m_wazano += 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_DOWN  ) ) m_wazano -= 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) m_wazano += 10;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT  ) ) m_wazano -= 10;

  if( m_wazano == prev_wazano ) {
    return;
  }

  m_wazano = ( m_wazano + WAZANO_MAX ) % WAZANO_MAX;
  GFL_PRINT( "wazano=%d\n", m_wazano );
}

/**
 * @brief 技データをダンプする
 * @param wazano  ダンプする技の技No.
 */
void WazaDataTestProc::DumpWazaData( WazaNo wazano ) const
{
  GFL_PRINT( "PARAM_ID_TYPE                  = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_TYPE                  ) );
  GFL_PRINT( "PARAM_ID_CATEGORY              = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_CATEGORY              ) );
  GFL_PRINT( "PARAM_ID_DAMAGE_TYPE           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_DAMAGE_TYPE           ) );
  GFL_PRINT( "PARAM_ID_POWER                 = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_POWER                 ) );
  GFL_PRINT( "PARAM_ID_HITPER                = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_HITPER                ) );
  GFL_PRINT( "PARAM_ID_BASE_PP               = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_BASE_PP               ) );
  GFL_PRINT( "PARAM_ID_PRIORITY              = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_PRIORITY              ) );
  GFL_PRINT( "PARAM_ID_CRITICAL_RANK         = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_CRITICAL_RANK         ) );
  GFL_PRINT( "PARAM_ID_HITCOUNT_MAX          = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_HITCOUNT_MAX          ) );
  GFL_PRINT( "PARAM_ID_HITCOUNT_MIN          = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_HITCOUNT_MIN          ) );
  GFL_PRINT( "PARAM_ID_SHRINK_PER            = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SHRINK_PER            ) );
  GFL_PRINT( "PARAM_ID_SICK                  = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SICK                  ) );
  GFL_PRINT( "PARAM_ID_SICK_PER              = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SICK_PER              ) );
  GFL_PRINT( "PARAM_ID_SICK_CONT             = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SICK_CONT             ) );
  GFL_PRINT( "PARAM_ID_SICK_TURN_MIN         = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SICK_TURN_MIN         ) );
  GFL_PRINT( "PARAM_ID_SICK_TURN_MAX         = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_SICK_TURN_MAX         ) );
  GFL_PRINT( "PARAM_ID_RANKTYPE_1            = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKTYPE_1            ) );
  GFL_PRINT( "PARAM_ID_RANKTYPE_2            = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKTYPE_2            ) );
  GFL_PRINT( "PARAM_ID_RANKTYPE_3            = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKTYPE_3            ) );
  GFL_PRINT( "PARAM_ID_RANKVALUE_1           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKVALUE_1           ) );
  GFL_PRINT( "PARAM_ID_RANKVALUE_2           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKVALUE_2           ) );
  GFL_PRINT( "PARAM_ID_RANKVALUE_3           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKVALUE_3           ) );
  GFL_PRINT( "PARAM_ID_RANKPER_1             = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKPER_1             ) );
  GFL_PRINT( "PARAM_ID_RANKPER_2             = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKPER_2             ) );
  GFL_PRINT( "PARAM_ID_RANKPER_3             = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_RANKPER_3             ) );
  GFL_PRINT( "PARAM_ID_DAMAGE_RECOVER_RATIO  = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_DAMAGE_RECOVER_RATIO  ) );
  GFL_PRINT( "PARAM_ID_HP_RECOVER_RATIO      = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_HP_RECOVER_RATIO      ) );
  GFL_PRINT( "PARAM_ID_TARGET                = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_TARGET                ) );
  GFL_PRINT( "PARAM_ID_AI_SEQNO              = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_AI_SEQNO              ) );
  GFL_PRINT( "PARAM_ID_DAMAGE_REACTION_RATIO = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_DAMAGE_REACTION_RATIO ) );
  GFL_PRINT( "PARAM_ID_HP_REACTION_RATIO     = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_HP_REACTION_RATIO     ) );
  GFL_PRINT( "PARAM_ID_MAST_HIT_FLAG         = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_MAST_HIT_FLAG         ) );
  GFL_PRINT( "PARAM_ID_ZENRYOKU_WAZANO           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_ZENRYOKU_WAZANO           ) );
  GFL_PRINT( "PARAM_ID_ZENRYOKU_POWER            = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_ZENRYOKU_POWER            ) );
  GFL_PRINT( "PARAM_ID_ZENRYOKU_EFFECT           = %d\n",   pml::wazadata::GetParam( wazano, pml::wazadata::PARAM_ID_ZENRYOKU_EFFECT           ) );

  GFL_PRINT( "\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Touch          ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "接触技\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Tame           ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "１ターン溜めワザ\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Tire           ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "１ターン反動ワザ\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Mamoru         ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「まもる」でガード対象となるワザ\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_MagicCoat      ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「マジックコート」対象\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Yokodori       ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「よこどり」対象\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Oumu           ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「おうむがえし」対象\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Punch          ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「てつのこぶし」対象（パンチのイメージ）\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Sound          ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "「ぼうおん」対象（音による攻撃・効果のイメージ）\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_Flying         ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "重力時に失敗する（飛んだり浮いたりイメージ）\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_KooriMelt      ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "自分が凍りでも解除してワザを出す（高温イメージ）\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_TripleFar      ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "トリプルバトルで、離れた相手も選択出来る\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_KaifukuHuuji   ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "かいふく封じで封じられる\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_MigawariThru   ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "相手がみがわりでも当たる（ダメージワザ以外の判定に）\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_SkyBattleFail  ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "スカイバトルで失敗する\n" );
  GFL_PRINT( ( pml::wazadata::GetFlag( wazano, pml::wazadata::WAZAFLAG_EffectToFriend ) ) ? ( "●" ) : ( "　" ) );  GFL_PRINT( "味方対象のワザエフェクトを出す（効果範囲=通常ポケの場合に参照する）\n" );
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理(下準備)
 * @param procManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
//---------------------------------------------------------------------------
void WazaDataTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void WazaDataTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

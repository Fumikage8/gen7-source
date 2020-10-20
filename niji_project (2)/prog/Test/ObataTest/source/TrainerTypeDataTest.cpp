//=============================================================================
/**
 * @file    TrainerTypeDataTest.cpp
 * @brief   トレーナータイプデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.20
 */
//=============================================================================
#if PM_DEBUG

#include <niji_conv_header/trainer/trtype_def.h>

#include <System/include/HeapDefine.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include "../include/TrainerTypeDataTest.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
TrainerTypeDataTestProc::TrainerTypeDataTestProc( void ) :
  GameSys::GameProc(),
  ObataTestBase(),
  m_trainerType( 0 )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
TrainerTypeDataTestProc::~TrainerTypeDataTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerTypeDataTestProc::InitFunc( gfl2::proc::Manager* procManager )
{
  this->SetupBase();

  GFL_PRINT( "\n" );
  GFL_PRINT( "操作方法\n" );
  GFL_PRINT( "START   終了\n" );
  GFL_PRINT( "UP      トレーナータイプ + 1\n" );
  GFL_PRINT( "RIGHT   トレーナータイプ +10\n" );
  GFL_PRINT( "DOWN    トレーナータイプ - 1\n" );
  GFL_PRINT( "LEFT    トレーナータイプ -10\n" );

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerTypeDataTestProc::EndFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result TrainerTypeDataTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  const gfl2::ui::Button* uiButton = this->UiButton();

  this->UpdateTrainerType();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) ) 
  {
    this->DumpTrainerTypeData( static_cast<trainer::TrType>( m_trainerType ) );
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) ) {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief 操作対象のトレーナータイプを更新する
 */
void TrainerTypeDataTestProc::UpdateTrainerType( void )
{
  s32 prev_trainerType = m_trainerType;

  const gfl2::ui::Button* uiButton = this->UiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_UP    ) ) m_trainerType += 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_DOWN  ) ) m_trainerType -= 1;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) m_trainerType += 10;
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_LEFT  ) ) m_trainerType -= 10;

  if( m_trainerType == prev_trainerType ) {
    return;
  }

  m_trainerType = ( m_trainerType + trainer::TRTYPE_MAX ) % trainer::TRTYPE_MAX;
  GFL_PRINT( "trainerType=%d\n", m_trainerType );
}

/**
 * @brief トレーナータイプのデータをダンプする
 * @param trtype  ダンプするトレーナータイプ
 */
void TrainerTypeDataTestProc::DumpTrainerTypeData( trainer::TrType trtype ) const
{
  trainer::TrainerTypeData data;
  data.LoadData( this->SystemHeap(), trtype );

  GFL_PRINT( "性別                                    %d\n", data.GetSex() );
  GFL_PRINT( "複数トレーナーか？                      %d\n", data.GetPlural() );
  GFL_PRINT( "トレーナー分類                          %d\n", data.GetGroup() );
  GFL_PRINT( "ボールID                                %d\n", data.GetBallID() );
  GFL_PRINT( "視線BGM                                 %d\n", data.GetEyeBGM() );
  GFL_PRINT( "導入演出ID                              %d\n", data.GetBattleEffectID() );
  GFL_PRINT( "トレーナーアイコンID                    %d\n", data.GetTrainerIconID() );
  GFL_PRINT( "フィールドのキャラモデルID( メイン )    %d\n", data.GetFieldMainModelID() );
  GFL_PRINT( "フィールドのキャラモデルID( サブ )      %d\n", data.GetFieldSubModelID() );
  GFL_PRINT( "バトルのキャラモデルID( メイン )        %d\n", data.GetBattleMainModelID() );
  GFL_PRINT( "バトルのキャラモデルID( サブ )          %d\n", data.GetBattleSubModelID() );
  GFL_PRINT( "メガシンカアイテムNo.                   %d\n", trainer::TrainerTypeData::GetMegaItemId( trtype ) );
  GFL_PRINT( "見せ場に使用されるトレーナータイプか？  %d\n", trainer::TrainerTypeData::IsHighLightSceneTrainerType( trtype ) );
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理(下準備)
 * @param procManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
//---------------------------------------------------------------------------
void TrainerTypeDataTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void TrainerTypeDataTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

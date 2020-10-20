//=============================================================================
/**
 * @file    TrainerMessageTest.cpp
 * @brief   トレーナーデータ関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.20
 */
//=============================================================================
#if PM_DEBUG

#include <System/include/HeapDefine.h>
#include <Print/include/WordSet.h>
#include <Trainer/Trainer/include/TrainerMessage.h>
#include "../include/TrainerMessageTest.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
TrainerMessageTestProc::TrainerMessageTestProc( void ) :
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
TrainerMessageTestProc::~TrainerMessageTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TrainerMessageTestProc::InitFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result TrainerMessageTestProc::EndFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result TrainerMessageTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  const gfl2::ui::Button* uiButton = this->UiButton();

  this->UpdateTrainerId();

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) ) 
  {
    trainer::TrainerID trid = static_cast<trainer::TrainerID>( m_trainerId );
    this->DumpTrainerMessage( trid );
  }

  if( uiButton->IsTrigger( gfl2::ui::BUTTON_START ) ) {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief 操作対象のトレーナーIDを更新する
 */
void TrainerMessageTestProc::UpdateTrainerId( void )
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
 * @brief トレーナーデータをダンプする
 * @param trainerId  ダンプするトレーナーデータのID
 * 
 * @note メッセージデータを読み込むたびに文字列を出力してしまうと、
 *       トレーナーメッセージの文字列がライブラリ内での出力に埋もれてしまうため、
 *       いったん、文字列を配列に収集したのち、いっぺんに出力する。
 */
void TrainerMessageTestProc::DumpTrainerMessage( trainer::TrainerID trainerId ) const
{
  gfl2::heap::HeapBase* heap = this->SystemHeap();
  gfl2::str::StrBuf* str_array[ trainer::TRMSG_KIND_MAX ];

  for( u32 messageKind=0; messageKind<trainer::TRMSG_KIND_MAX; ++messageKind )
  {
    gfl2::str::StrBuf* strbuf = GFL_NEW( heap ) gfl2::str::StrBuf( 512, heap );
    if( trainer::TrainerMessage::GetString( strbuf, trainerId, static_cast<trainer::MessageKind>( messageKind ), heap ) ) 
    {
      str_array[ messageKind ] = strbuf;
      continue;
    }
    else 
    {
      str_array[ messageKind ] = NULL;
      GFL_SAFE_DELETE( strbuf );
    }
  }

  for( u32 messageKind=0; messageKind<trainer::TRMSG_KIND_MAX; ++messageKind )
  {
    if( str_array[ messageKind ] != NULL ) {
      gfl2::str::PrintCode( str_array[ messageKind ]->GetPtr() );
      GFL_SAFE_DELETE( str_array[ messageKind ] );
    }
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
void TrainerMessageTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void TrainerMessageTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

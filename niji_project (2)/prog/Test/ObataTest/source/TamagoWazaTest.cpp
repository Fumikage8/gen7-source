//=============================================================================
/**
 * @file    TamagoWazaTest.cpp
 * @brief   タマゴ技関連のテスト
 * @author  obata_toshihiro
 * @date    2015.05.11
 */
//=============================================================================
#if PM_DEBUG

#include <niji_conv_header/poke_lib/monsno_def.h>
#include <arc_def_index/arc_def.h>
#include <pml/include/pml_PokePara.h>
#include <pml/include/pml_Personal.h>
#include <System/include/HeapDefine.h>
#include "../include/TamagoWazaTest.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ObataTest )



//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
TamagoWazaTestProc::TamagoWazaTestProc( void ) :
  GameSys::GameProc(),
  ObataTestBase()
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
TamagoWazaTestProc::~TamagoWazaTestProc()
{
}


//---------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TamagoWazaTestProc::InitFunc( gfl2::proc::Manager* procManager )
{
  this->SetupBase();
  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
gfl2::proc::Result TamagoWazaTestProc::EndFunc( gfl2::proc::Manager* procManager )
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
gfl2::proc::Result TamagoWazaTestProc::UpdateFunc( gfl2::proc::Manager* procManager )
{
  gfl2::str::StrBuf*          strbuf         = this->GetStrBuf( 0 );
  gfl2::heap::HeapBase*       heap           = this->DeviceHeap();
  gfl2::fs::AsyncFileManager* arcFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
  char*                       arcFileName    = arcFileManager->GetUsableArcFileName( gfl2::fs::AsyncFileManager::ArcFileNameTableType::MAIN, heap, ARCID_PMLIB_WAZA_EGG );

  for( MonsNo monsno=MONSNO_HUSIGIDANE; monsno<=MONSNO_END; monsno=static_cast<MonsNo>( monsno+1 ) )
  {
    pml::personal::LoadPersonalData( monsno, 0 );
    u32 formNum = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );

    for( u32 formno=0; formno<formNum; ++formno )
    {
      GFL_PRINT( "------------------------------------ monsno=%d formno=%d ", monsno, formno );
      this->GetMonsName( strbuf, monsno );
      gfl2::str::PrintCode( strbuf->GetPtr() );

      pml::pokepara::EggWazaData eggWazaData( heap );
      eggWazaData.Load( arcFileName, monsno, formno );

      const u32 eggWazaNum = eggWazaData.GetEggWazaNum();
      for( u32 eggWazaIndex=0; eggWazaIndex<eggWazaNum; ++eggWazaIndex )
      {
        WazaNo wazano = eggWazaData.GetEggWazaNo( eggWazaIndex );
        this->GetWazaName( strbuf, wazano );
        gfl2::str::PrintCode( strbuf->GetPtr() );
      }
    }
  }

  GflHeapFreeMemory( arcFileName );

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理(下準備)
 * @param procManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
//---------------------------------------------------------------------------
void TamagoWazaTestProc::PreDrawFunc( gfl2::proc::Manager* procManager )
{
  // 何もしない
}

//---------------------------------------------------------------------------
/**
 * @brief プロセス描画関数
 * @param procManager  プロセスマネージャ
 */
//---------------------------------------------------------------------------
void TamagoWazaTestProc::DrawFunc( gfl2::proc::Manager* procManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  // 何もしない
}


GFL_NAMESPACE_END( ObataTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG

#if PM_DEBUG
//=============================================================================
/**
 * @file    d_obata.cpp
 * @brief   小幡用ランチャーリスト処理
 * @author  obata_toshihiro
 * @date    2015.02.20
 */
//=============================================================================
#include <heap/include/gfl2_heap.h>
#include <System/include/HeapDefine.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

#include <Debug/StartMenu/include/d_obata.h>
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_obata.h>

#include <Battle/include/battle_proc.h>
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>

#include <Test/ObataTest/include/ExpTest.h>
#include <Test/ObataTest/include/PersonalDataTest.h>
#include <Test/ObataTest/include/WazaDataTest.h>
#include <Test/ObataTest/include/TrainerTypeDataTest.h>
#include <Test/ObataTest/include/TrainerDataTest.h>
#include <Test/ObataTest/include/TrainerMessageTest.h>
#include <Test/ObataTest/include/TamagoWazaTest.h>




GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherObata::ItemList[LauncherObata::LIST_MAX] =
{
  label_obata_test_00,
  label_obata_test_01,
  label_obata_test_02,
  label_obata_test_03,
  label_obata_test_04,
  label_obata_test_05,
  label_obata_test_06,
  label_obata_test_07,
  label_obata_test_08,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherObata::LauncherObata( void ) : 
  m_strBuf( NULL ),
  m_debugPokeMakeParam()
{
  m_debugPokeMakeParam.poke = NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherObata::~LauncherObata()
{
  GFL_SAFE_DELETE( m_strBuf );
  GFL_SAFE_DELETE( m_debugPokeMakeParam.poke );
  GFL_SAFE_DELETE( m_debugPokeMakeParam.defaultPowerUpDesc );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherObata::GetArcDataID(void)
{
  return GARC_debug_message_d_obata_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherObata::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherObata::GetListTableSize(void)
{
  return LIST_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherObata::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case label_obata_test_00:  this->ExecExpTest();                                             return launcher::ListData::REQID_NONE;
  case label_obata_test_01:  this->CallDebugKeyboardProc();                                   return launcher::ListData::REQID_NONE;
  case label_obata_test_02:  this->CallDebugPokeMakeProc();                                   return launcher::ListData::REQID_NONE;
  case label_obata_test_03:  this->CallTestProc<Test::ObataTest::PersonalDataTestProc>();     return launcher::ListData::REQID_NONE;
  case label_obata_test_04:  this->CallTestProc<Test::ObataTest::WazaDataTestProc>();         return launcher::ListData::REQID_NONE;
  case label_obata_test_05:  this->CallTestProc<Test::ObataTest::TrainerTypeDataTestProc>();  return launcher::ListData::REQID_NONE;
  case label_obata_test_06:  this->CallTestProc<Test::ObataTest::TrainerDataTestProc>();      return launcher::ListData::REQID_NONE;
  case label_obata_test_07:  this->CallTestProc<Test::ObataTest::TrainerMessageTestProc>();   return launcher::ListData::REQID_NONE;
  case label_obata_test_08:  this->CallTestProc<Test::ObataTest::TamagoWazaTestProc>();       return launcher::ListData::REQID_NONE;
  }

  return launcher::ListData::REQID_NONE;
}

/**
 * @brief デバッグキーボードプロセスをコールする
 */
void LauncherObata::CallDebugKeyboardProc( void )
{
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  gfl2::heap::HeapBase* deviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  GFL_SAFE_DELETE( m_strBuf );
  m_strBuf = GFL_NEW( systemHeap ) gfl2::str::StrBuf( 256, systemHeap );

  debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
  proc->SetupParam( systemHeap, deviceHeap, m_strBuf );
}

/**
 * @brief デバッグポケ作成プロセスをコールする
 */
void LauncherObata::CallDebugPokeMakeProc( void )
{
  gfl2::heap::HeapBase* systemHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  GFL_SAFE_DELETE( m_debugPokeMakeParam.poke );
  GFL_SAFE_DELETE( m_debugPokeMakeParam.defaultPowerUpDesc );
  m_debugPokeMakeParam.poke               = GFL_NEW( systemHeap ) pml::pokepara::PokemonParam( systemHeap );
  m_debugPokeMakeParam.defaultPowerUpDesc = GFL_NEW( systemHeap ) DefaultPowerUpDesc();

  GameSys::CallDebugPokeMakeProc( &m_debugPokeMakeParam );
}

/**
 * @brief 経験値計算テストを実行する
 */
void LauncherObata::ExecExpTest()
{
  Test::ObataTest::ExpTest test;
  test.Exec();
}

/**
 * @brief テストプロセスを呼び出す
 * @brief TestProcClassName  テストプロセスのクラス名
 */
template <typename TestProcClassName> void LauncherObata::CallTestProc( void )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  GameSys::GameProcManager* gameProcManager = gameManager->GetProcManager();
  gfl2::fs::AsyncFileManager* fileManager = gameManager->GetAsyncFileManager();
  NIJI_RO_PROC_CALL< GameSys::GameProcManager, TestProcClassName>( gameProcManager, fileManager, "ObataTest.cro" );
}




GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG

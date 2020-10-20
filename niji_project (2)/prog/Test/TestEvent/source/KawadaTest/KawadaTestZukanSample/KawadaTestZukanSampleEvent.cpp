//==============================================================================
/**
 @file    KawadaTestZukanSampleEvent.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// poke_libのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

#include <System/include/PokemonVersion.h>

#include <debug/Launcher/include/LauncherProc.h>
#include <debug/StartMenu/include/StartLauncher.h>

#include <Savedata/include/ZukanSave.h>

#include <Sound/include/Sound.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProc.h>

// KawadaTestのインクルード
#include <Test/TestEvent/include/KawadaTest/KawadaTestZukanSample/KawadaTestZukanSampleEvent.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


void KawadaTestZukanSampleEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, KawadaTestZukanSampleEvent>(pGameEventManager);
}


KawadaTestZukanSampleEvent::KawadaTestZukanSampleEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_procParam(),
    m_proc(NULL)
{
  /*
  １匹もいない状態で図鑑が始まることがあるらしいのでコメントアウト。

  // １匹は捕まえたことにしておく
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
 
  {
    pml::pokepara::PokemonParam* pp = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, static_cast<MonsNo>(25), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );  // ピカチュウ
    pp->SetLangId(System::GetLang());
    svZukanData->SetPokeGet(*pp);
    GFL_SAFE_DELETE(pp);
  }


  // テストデータ
  {
    // １匹は見たことにしておく
    {
      pml::pokepara::PokemonParam* pp = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, static_cast<MonsNo>(26), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );  // ライチュウ
      pp->SetLangId(System::GetLang());
      svZukanData->SetPokeSee(*pp);
      GFL_SAFE_DELETE(pp);
    }
  }
  */

  {
    // コラッタ
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

    pml::pokepara::PokemonParam* pp = GFL_NEW(heap) pml::pokepara::PokemonParam( heap, static_cast<MonsNo>(19), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
    pp->SetLangId(System::GetLang());
    svZukanData->SetPokeGet(*pp);

    pp->SetLangId(POKEMON_LANG_JAPAN);
    svZukanData->SetPokeGet(*pp);
    pp->SetLangId(POKEMON_LANG_CHINA);
    svZukanData->SetPokeGet(*pp);
    pp->SetLangId(POKEMON_LANG_TAIWAN);
    svZukanData->SetPokeGet(*pp);
    
    GFL_SAFE_DELETE(pp);
  }
}
KawadaTestZukanSampleEvent::~KawadaTestZukanSampleEvent()
{}

bool KawadaTestZukanSampleEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void KawadaTestZukanSampleEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
{}
GameSys::GMEVENT_RESULT KawadaTestZukanSampleEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_WAIT_OTHER_PROC_END,
    SEQ_CALL_ZUKAN_PROC,
    SEQ_WAIT_ZUKAN_PROC_START,
    SEQ_WAIT_ZUKAN_PROC_END,
    SEQ_CALL_LAUNCHER,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;
  
  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  GameSys::GameProcManager* gameProcManager = gmgr->GetProcManager();
  
  switch(seq)
  {
  case SEQ_WAIT_OTHER_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        seq = SEQ_CALL_ZUKAN_PROC;
      }
      break;
    }
  case SEQ_CALL_ZUKAN_PROC:
    {
      Sound::StartBGMReq(STRM_BGM_SILENCE);  // 図鑑でMEを鳴らすので、BGMがかかってなければならない。
      // このイベントで扱うproc作成
      m_proc = GameSys::CallZukanProc(&m_procParam);
      seq = SEQ_WAIT_ZUKAN_PROC_START;
      break;
    }
  case SEQ_WAIT_ZUKAN_PROC_START:
    {
      if( gmgr->IsProcessExists() != false )
      {
        // このイベントで扱うprocが動作中
        seq = SEQ_WAIT_ZUKAN_PROC_END;
      }
      break;
    }
  case SEQ_WAIT_ZUKAN_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        seq = SEQ_CALL_LAUNCHER;
      }
      break;
    }
  case SEQ_CALL_LAUNCHER:
    {
      // このイベントを終了する前に、次のprocを作成しておく
      debug::launcher::LauncherProc::CallParam launcherCallParam;
      debug::launcher::LauncherProc::InitCallParam( &launcherCallParam );
      debug::launcher::LauncherProc* launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gameProcManager, &launcherCallParam );  // @todo ランチャーがDLLになったら呼び方を変えること。
      launcher->CreateStartListData<debug::startmenu::StartLauncher>();
      seq = SEQ_END;
      res = GameSys::GMEVENT_RES_FINISH;
      break;
    }
  case SEQ_END:
    {
      res = GameSys::GMEVENT_RES_FINISH;
      break;
    }
  }

  this->SetSeqNo(seq);  // 親であるGameSys::GameEventのメンバ関数

  return res;
}
void KawadaTestZukanSampleEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


//==============================================================================
/**
 @file    KawadaTestCallZukanRegisterSampleEvent.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.21
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// poke_libのインクルード
#include <pml/include/pmlib.h>
//#include <pml/include/item/item.h>                   // pmlib.hに含まれているのでコメントアウト
//#include <niji_conv_header/poke_lib/item/itemsym.h>  // pmlib.hに含まれているのでコメントアウト

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/poke_lib/monsno_def.h>

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

#include <PokeTool/include/PokeToolPokeGet.h>

// KawadaTestのインクルード
#include <Test/TestEvent/include/KawadaTest/KawadaTestCallZukanRegisterSample/KawadaTestCallZukanRegisterSampleEvent.h>




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


void KawadaTestCallZukanRegisterSampleEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, KawadaTestCallZukanRegisterSampleEvent>(pGameEventManager);
}


KawadaTestCallZukanRegisterSampleEvent::KawadaTestCallZukanRegisterSampleEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_eventParam(),
    m_event(NULL),
    m_exData(),
    m_registerIndex(0),
    m_itemId(0),
    m_eggFlag(false),
    m_rareFlag(false),
    m_pokeSetIndex(0),
    m_pokeSetVariation(0),
    m_heap(heap)
{
  // 初期化
  {
    for(u32 i=0; i<PP_NUM; ++i)
    {
      m_pp[i] = NULL;
    }
  }
}
KawadaTestCallZukanRegisterSampleEvent::~KawadaTestCallZukanRegisterSampleEvent()
{
  for(u32 i=0; i<PP_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_pp[i]);
  }
}

bool KawadaTestCallZukanRegisterSampleEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void KawadaTestCallZukanRegisterSampleEvent::InitFunc(GameSys::GameManager* gmgr)
{}

#if TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE==TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING
GameSys::GMEVENT_RESULT KawadaTestCallZukanRegisterSampleEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_WAIT_OTHER_PROC_END,
    SEQ_CALL_ZUKAN_REGISTER_EVENT,
    SEQ_WAIT_ZUKAN_REGISTER_EVENT_END,
    SEQ_CALL_LAUNCHER,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;
  
  GameSys::GameProcManager* gameProcManager = gmgr->GetProcManager();
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  
  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  switch(seq)
  {
  case SEQ_WAIT_OTHER_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        // 図鑑フラグを立てておく
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_GET);
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND1_OPEN);
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN);
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN);
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN);
        svEventWork->SetEventFlag(SYS_FLAG_BOUKEN_NAVI_GET);

        // 初期化
        this->initAsingData();

        seq = SEQ_CALL_ZUKAN_REGISTER_EVENT;
      }
      break;
    }
  case SEQ_CALL_ZUKAN_REGISTER_EVENT:
    {
      // イベント作成
      m_eventParam.pp = m_pp[0];
      m_eventParam.callerType = PokeTool::PokeGetUtil::CallerType::HOKAKU;
      app::event::ZukanRegisterEvent::StartEvent( gmgr, &m_eventParam );
      seq = SEQ_WAIT_ZUKAN_REGISTER_EVENT_END;
      break;
    }
  case SEQ_WAIT_ZUKAN_REGISTER_EVENT_END:
    {
      // イベントが終了するのを待つ
      seq = SEQ_CALL_LAUNCHER;
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
#else  // TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE
GameSys::GMEVENT_RESULT KawadaTestCallZukanRegisterSampleEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_WAIT_OTHER_PROC_END,
    SEQ_WAIT_USER_INPUT,
    SEQ_CALL_ZUKAN_REGISTER_EVENT,
    SEQ_WAIT_ZUKAN_REGISTER_EVENT_END,
    SEQ_CALL_LAUNCHER,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;
  
  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  GameSys::GameProcManager* gameProcManager = gmgr->GetProcManager();
  //Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  Field::EventWork* svEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  
  switch(seq)
  {
  case SEQ_WAIT_OTHER_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        // 図鑑を入手しておく
        svEventWork->SetEventFlag(SYS_FLAG_ZUKAN_GET);

        // 使い方
        {
          GFL_PRINT("ZukanRegisterSample====\n");
          // LRボタンで見た、捕まえたを決める
          GFL_PRINT("LR:see,get\n");
          // STARTボタンで道具を決める
          GFL_PRINT("START:item\n");
          // 十字キーで手持ち、ボックスの設定
          GFL_PRINT("CrossLR:party,box\n");
          // 十字キーでポケモンセット、バリエーションの変更
          GFL_PRINT("CrossUD:pokeSet,bingo\n");
          // Yでタマゴ化
          GFL_PRINT("Y:egg\n");
          // X,Bは未使用
          GFL_PRINT("BX:not use\n");
          // Aでポケモン生成
          GFL_PRINT("A:begin\n");
        }
        seq = SEQ_WAIT_USER_INPUT;
      }
      break;
    }
  case SEQ_WAIT_USER_INPUT:
    {
      static const u32 POKE_SET_NUM = 10;
      static const u32 POKE_SET[POKE_SET_NUM][PP_NUM] =
      {
        { 172,  25,  26,   0 },  // ピチュー(ノーマル)
        { 786, 785, 787, 788 },  // ティキちょう(準伝説)
        { 792, 791,   0,   0 },  // 伝説ムーン(伝説)
        { 735, 734,   0,   0 },  // デカグース  // 「NMCat[1339] 巨大フォルムの図鑑登録についての質問」のテスト用
        { 801,   0,   0,   0 },  // まぼろし
        { 741,   0,   0,   0 },  // オドリドリ  // 「NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう」のテスト用
        {  52,   0,   0,   0 },  // ニャース  // 「NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう」のテスト用
        { 778,   0,   0,   0 },  // ミミッキュ
        { 716,   0,   0,   0 },  // ゼルネアス
        { 646,   0,   0,   0 },  // キュレム
      };

      static const u32 VARIATION_NUM = 2;
      static const char* VARIATION_STR[VARIATION_NUM] =
      {
        "noBingo",
        "bingo",
      };

      bool goNext = false;
      const gfl2::ui::Button*       button   = gmgr->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      const gfl2::ui::VectorDevice* crossKey = gmgr->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);

      ///////////////////////////////////////
      // LRボタンで見た、捕まえたを決める
      ///////////////////////////////////////
      bool flagL = button->IsHold(gfl2::ui::BUTTON_L);  // 見た
      bool flagR = button->IsHold(gfl2::ui::BUTTON_R);  // 捕まえた

      ///////////////////////////////////////
      // STARTボタンで道具を決める
      ///////////////////////////////////////
      if(button->IsTrigger(gfl2::ui::BUTTON_START))
      {
        u32 HAVE_ITEM_ID_LIST[] = 
        {
          ITEM_DUMMY_DATA,
          ITEM_KURABONOMI,
          ITEM_HONOOBIIZU_1,
          ITEM_HONOOBIIZU,  // ポケモンには「_1付き」しか渡せない。「_1なし」はたいせつなものなので渡せない。ここでは持ち物預かりのメッセージ分岐のテストのために「_1なし」を渡している。
        };
        u32 currIndex = 0;
        u32 indexMax = sizeof(HAVE_ITEM_ID_LIST)/sizeof(HAVE_ITEM_ID_LIST[0]);
        for(u32 i=0; i<indexMax; ++i)
        {
          if(HAVE_ITEM_ID_LIST[i] == m_itemId)
          {
            currIndex = i;
            break;
          }
        }
        currIndex = (currIndex +1) % indexMax;
        m_itemId = HAVE_ITEM_ID_LIST[currIndex];
        GFL_PRINT("itemId=%d\n", m_itemId);
      }
      
      ///////////////////////////////////////
      // 十字キーで手持ち、ボックスの設定
      ///////////////////////////////////////
      if(crossKey->IsTrigger(gfl2::ui::DIR_UP))
      {
        // 手持ちフル
        // ボックスフル
        //this->setPartyMember(-1, m_itemId);
        //this->setBoxMember(-1, m_itemId);
        //GFL_PRINT("party=full,box=full\n");
      }
      else if(crossKey->IsTrigger(gfl2::ui::DIR_DOWN))
      {
        // 手持ち１
        // ボックスフル
        //this->setPartyMember(1, m_itemId);
        //this->setBoxMember(-1, m_itemId);
        //GFL_PRINT("party=1,box=full\n");
      }
      else if(crossKey->IsTrigger(gfl2::ui::DIR_LEFT))
      {
        // 手持ちフル
        // ボックス１
        this->setPartyMember(-1, m_itemId, m_eggFlag, m_rareFlag);
        this->setBoxMember(1, m_itemId, m_eggFlag, m_rareFlag);
        //this->setBoxMember(-1, m_itemId, m_eggFlag, m_rareFlag);  // ボックスもフルのテストをするときはこちら
        GFL_PRINT("party=full,box=1\n");
      }
      else if(crossKey->IsTrigger(gfl2::ui::DIR_RIGHT))
      {
        // 手持ち１
        // ボックス１
        this->setPartyMember(1, m_itemId, m_eggFlag, m_rareFlag);
        this->setBoxMember(1, m_itemId, m_eggFlag, m_rareFlag);
        GFL_PRINT("party=1,box=1\n");
      }

      ///////////////////////////////////////
      // 十字キーでポケモンセット、バリエーションの変更
      ///////////////////////////////////////
      {
        if(crossKey->IsTrigger(gfl2::ui::DIR_UP))
        {
          m_pokeSetIndex = (m_pokeSetIndex+1)%POKE_SET_NUM;
          GFL_PRINT("pokeSet={%d,%d,%d,%d}\n", POKE_SET[m_pokeSetIndex][0], POKE_SET[m_pokeSetIndex][1], POKE_SET[m_pokeSetIndex][2], POKE_SET[m_pokeSetIndex][3]);
        }
        else if(crossKey->IsTrigger(gfl2::ui::DIR_DOWN))
        {
          m_pokeSetVariation = (m_pokeSetVariation+1)%VARIATION_NUM;
          GFL_PRINT("vari=%s\n", VARIATION_STR[m_pokeSetVariation]);
        }
      }

      ///////////////////////////////////////
      // Yでタマゴ化
      ///////////////////////////////////////
      if(button->IsTrigger(gfl2::ui::BUTTON_Y))
      {
        m_eggFlag = !m_eggFlag;
        GFL_PRINT("egg=%d\n", m_eggFlag);
      }

      ///////////////////////////////////////
      // Xで色違い化
      ///////////////////////////////////////
      if(button->IsTrigger(gfl2::ui::BUTTON_X))
      {
        m_rareFlag = !m_rareFlag;
        GFL_PRINT("rare=%d\n", m_rareFlag);
      }

      ///////////////////////////////////////
      // Aでポケモン生成
      ///////////////////////////////////////
      PokeInfo pokeInfo[PP_NUM];
      bool decideFlag = false;

      if(button->IsTrigger(gfl2::ui::BUTTON_A))
      {
        // ユーザが設定したもので開始
        decideFlag = true;
      }
        
      if(decideFlag)
      {
        GFL_PRINT("pokeSet={%d(%d,%d),%d,%d,%d},vari=%s\n", POKE_SET[m_pokeSetIndex][0], flagL, flagR,
            POKE_SET[m_pokeSetIndex][1], POKE_SET[m_pokeSetIndex][2], POKE_SET[m_pokeSetIndex][3], VARIATION_STR[m_pokeSetVariation]);
        
        static const bool seeFlag0[PP_NUM] =
        {
          false,  // flagL, flagR任せ
          false,
          true,
          true,
        };
        static const bool seeFlag1[PP_NUM] =
        {
          false,  // flagL, flagR任せ
          true,
          true,
          true,
        };
        
        static const bool getFlag0[PP_NUM] =
        {
          false,  // flagL, flagR任せ
          false,
          false,
          true,
        };
        static const bool getFlag1[PP_NUM] =
        {
          false,  // flagL, flagR任せ
          true,
          true,
          true,
        };

        const bool* seeFlag = (m_pokeSetVariation==0)?(seeFlag0):(seeFlag1);
        const bool* getFlag = (m_pokeSetVariation==0)?(getFlag0):(getFlag1);

        u32 count = 0;
        if(POKE_SET[m_pokeSetIndex][0]) {  pokeInfo[0] = PokeInfo(POKE_SET[m_pokeSetIndex][0], m_itemId, m_eggFlag, m_rareFlag,      flagR,      flagL);  ++count;  }
        if(POKE_SET[m_pokeSetIndex][1]) {  pokeInfo[1] = PokeInfo(POKE_SET[m_pokeSetIndex][1], m_itemId, m_eggFlag, m_rareFlag, getFlag[1], seeFlag[1]);  ++count;  }
        if(POKE_SET[m_pokeSetIndex][2]) {  pokeInfo[2] = PokeInfo(POKE_SET[m_pokeSetIndex][2], m_itemId, m_eggFlag, m_rareFlag, getFlag[2], seeFlag[2]);  ++count;  }
        if(POKE_SET[m_pokeSetIndex][3]) {  pokeInfo[3] = PokeInfo(POKE_SET[m_pokeSetIndex][3], m_itemId, m_eggFlag, m_rareFlag, getFlag[3], seeFlag[3]);  ++count;  }
 
        this->createPoke(count, pokeInfo);
        m_registerIndex = 0;
       
        goNext = true;
      }

      if(goNext)
      {
        seq = SEQ_CALL_ZUKAN_REGISTER_EVENT;
      }
      break;
    }
  case SEQ_CALL_ZUKAN_REGISTER_EVENT:
    {
      // イベント作成
      m_eventParam.pp         = m_pp[m_registerIndex];
      if(m_eggFlag)
      {
        m_eventParam.callerType = PokeTool::PokeGetUtil::CallerType::MORAU_TAMAGO;
      }
      else
      {
        m_eventParam.callerType = PokeTool::PokeGetUtil::CallerType::HOKAKU;
        //m_eventParam.callerType = PokeTool::PokeGetUtil::CallerType::MORAU_NANASHI_PARTY;
      }
      app::event::ZukanRegisterEvent::StartEvent( gmgr, &m_eventParam );
      seq = SEQ_WAIT_ZUKAN_REGISTER_EVENT_END;
      this->printMember();
      break;
    }
  case SEQ_WAIT_ZUKAN_REGISTER_EVENT_END:
    {
      // イベントが終了するのを待つ
      seq = SEQ_CALL_LAUNCHER;
      break;
    }
  case SEQ_CALL_LAUNCHER:
    {
      this->printMember();
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
#endif  // TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE

void KawadaTestCallZukanRegisterSampleEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}


void KawadaTestCallZukanRegisterSampleEvent::setPartyMember(s32 memberNum, u32 itemId, bool eggFlag, bool rareFlag)
{
  pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
  
  if(memberNum < 0)
  {
    // フル
    this->setPartyMember(pml::PokeParty::MAX_MEMBERS, itemId, eggFlag, rareFlag);
  }
  else
  {
    // 指定数
    const s32 numMin = 0;
    const s32 numMax = pml::PokeParty::MAX_MEMBERS;
  
    if(memberNum < numMin) memberNum = numMin;
    if(memberNum > numMax) memberNum = numMax;

    // 減らす
    s32 currNum = party->GetMemberCount();
    while(currNum > memberNum)
    {
      party->RemoveMember(currNum -1);
      currNum = party->GetMemberCount();
    }

    // 増やす
    pml::pokepara::PokemonParam* pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, MONSNO_PIKATYUU, 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
    pp->SetLangId(System::GetLang());

    if( pp->GetItem() != ITEM_DUMMY_DATA ) pp->RemoveItem();
    if(itemId > 0) pp->SetItem(itemId);

    if(eggFlag) pp->SetEggFlag();
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
    if(rareFlag) pp->SetRare();
#endif

    while(currNum < memberNum)
    {
      PokeTool::PokeGetUtil::ReturnParam returnParam;
      PokeTool::PokeGetUtil::PutPoke(
          m_heap->GetLowerHandle(),
          pp,
          PokeTool::PokeGetUtil::PutPlace::PARTY,
          PokeTool::PokeGetUtil::PutSpace::FREE,
          0,
          0,
          &returnParam
      );
      currNum = party->GetMemberCount();
    }
    GFL_SAFE_DELETE(pp);
  }
}
void KawadaTestCallZukanRegisterSampleEvent::setBoxMember(s32 memberNum, u32 itemId, bool eggFlag, bool rareFlag)
{
  //Savedata::BOX*        svBox     = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
  Savedata::BoxPokemon* svBoxPoke = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
  
  if(memberNum < 0)
  {
    // フル
    pml::pokepara::PokemonParam* pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, MONSNO_PIKATYUU, 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
    pp->SetLangId(System::GetLang());

    if( pp->GetItem() != ITEM_DUMMY_DATA ) pp->RemoveItem();
    if(itemId > 0) pp->SetItem(itemId);
    
    if(eggFlag) pp->SetEggFlag();
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
    if(rareFlag) pp->SetRare();
#endif

    while( svBoxPoke->GetSpaceCountAll(m_heap->GetLowerHandle()) > 0 )
    {
      PokeTool::PokeGetUtil::ReturnParam returnParam;
      PokeTool::PokeGetUtil::PutPoke(
          m_heap->GetLowerHandle(),
          pp,
          PokeTool::PokeGetUtil::PutPlace::BOX,
          PokeTool::PokeGetUtil::PutSpace::FREE,
          0,
          0,
          &returnParam
      );
    }
      
    GFL_SAFE_DELETE(pp);
  }
  else if(memberNum == 0)
  {
    // ゼロ
    GFL_ASSERT(0);  // @todo まだデバッグ機能がない。本格デバッグ前に一度試しておくこと。
  }
  else
  {
    // 指定数
    pml::pokepara::PokemonParam* pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, MONSNO_PIKATYUU, 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
    pp->SetLangId(System::GetLang());

    if( pp->GetItem() != ITEM_DUMMY_DATA ) pp->RemoveItem();
    if(itemId > 0) pp->SetItem(itemId);
    
    if(eggFlag) pp->SetEggFlag();
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
    if(rareFlag) pp->SetRare();
#endif

    if(memberNum == 1)
    {
      if( svBoxPoke->GetPokemonCountAll(m_heap->GetLowerHandle()) == 0 )
      {
        PokeTool::PokeGetUtil::ReturnParam returnParam;
        PokeTool::PokeGetUtil::PutPoke(
            m_heap->GetLowerHandle(),
            pp,
            PokeTool::PokeGetUtil::PutPlace::BOX,
            PokeTool::PokeGetUtil::PutSpace::FREE,
            0,
            0,
            &returnParam
        );
      }
      GFL_SAFE_DELETE(pp);
    }
    else
    {
      GFL_ASSERT(0);  // @todo まだデバッグ機能がない。本格デバッグ前に一度試しておくこと。if節内もメンバー数1にできていないし。
    }
  }
}
void KawadaTestCallZukanRegisterSampleEvent::createPoke(u32 pokeInfoNum, PokeInfo pokeInfo[])
{
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
  
  // テスト用のポケモンを生成する
  for(u32 i=0; i<pokeInfoNum; ++i)
  {
    // ポケモン
    pml::pokepara::PokemonParam* pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, static_cast<MonsNo>(pokeInfo[i].monsNo), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
    pp->SetLangId(System::GetLang());

    // フォルム番号
    if(pokeInfo[i].monsNo == MONSNO_MANGUUSU2)  // 735  // 「NMCat[1339] 巨大フォルムの図鑑登録についての質問」のテスト用
    {
      pp->ChangeFormNo( FORMNO_MANGUUSU2_NUSHI );
    }
    else if(pokeInfo[i].monsNo == MONSNO_HORAA)  // 778
    {
      u32 formNo = 3;
      pp->ChangeFormNo( formNo );
    }
    else if(pokeInfo[i].monsNo == MONSNO_ZERUNEASU)  // 716
    {
      u32 formNo = FORMNO_ZERUNEASU_NORMAL;//FORMNO_ZERUNEASU_RADIATION;
      pp->ChangeFormNo( formNo );
    }
    else if(pokeInfo[i].monsNo == MONSNO_KYUREMU)  // 646
    {
      u32 formNo = FORMNO_KYUREMU_A;//FORMNO_KYUREMU_WHITE;//FORMNO_KYUREMU_BLACK;
      pp->ChangeFormNo( formNo );
    }

    // 道具
    if( pp->GetItem() != ITEM_DUMMY_DATA ) pp->RemoveItem();
    if(pokeInfo[i].itemId > 0)
    {
      pp->SetItem(pokeInfo[i].itemId); 
    }
   
    // タマゴ
    if(m_eggFlag) pp->SetEggFlag();

    // 色違い
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
    if(m_rareFlag) pp->SetRare();
#endif 

    // 捕まえた、見たフラグ
    svZukanData->DebugSetPokeDeleteForce(*pp);
    if(pokeInfo[i].getFlag)
    {
      svZukanData->SetPokeGet(*pp);

      // 既に捕まえているので、2回目以降の登録
      svZukanData->SetPokeRegisterShow( static_cast<MonsNo>(pokeInfo[i].monsNo) );  // 図鑑登録画面を呼んだ(名前入力画面とかは関係ない)
    }
    else
    {
      if(pokeInfo[i].seeFlag)
      {
        svZukanData->SetPokeSee(*pp);
      }
    }

    // 見たフラグ、フォルム番号
    if(pokeInfo[i].monsNo == MONSNO_MITUSUI)  // 741  // 「NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう」のテスト用
    {
      {
        pml::pokepara::PokemonParam* temp_pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, static_cast<MonsNo>(pokeInfo[i].monsNo), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
        temp_pp->SetLangId(System::GetLang());
        temp_pp->ChangeFormNo( FORMNO_MITUSUI_FLAMENCO );
        svZukanData->SetPokeSee(*temp_pp);
        GFL_SAFE_DELETE(temp_pp);
      }
      pp->ChangeFormNo( FORMNO_MITUSUI_HULA );
    }
    else if(pokeInfo[i].monsNo == MONSNO_NYAASU)  // 52  // 「NMCat[2360] オドリドリのフォルム違いを捕獲した時、最初に見た別のフォルムが図鑑登録時に表示されてしまう」のテスト用
    {
      {
        pml::pokepara::PokemonParam* temp_pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, static_cast<MonsNo>(pokeInfo[i].monsNo), 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
        temp_pp->SetLangId(System::GetLang());
        temp_pp->ChangeFormNo( FORMNO_NYAASU_NORMAL );
        svZukanData->SetPokeSee(*temp_pp);
        GFL_SAFE_DELETE(temp_pp);
      }
      pp->ChangeFormNo( FORMNO_NYAASU_ALOLA );
    }

    m_pp[i] = pp;
  }
}

void KawadaTestCallZukanRegisterSampleEvent::printMember(void) const
{
  const pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
  Savedata::BoxPokemon* svBoxPoke = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

  GFL_PRINT("party num=%d\n", party->GetMemberCount());
  for(u32 i=0; i<party->GetMemberCount(); ++i)
  {
    const pml::pokepara::PokemonParam* pp = party->GetMemberPointerConst(i);
    GFL_PRINT("%d: monsNo=%d, personalRnd=0x%X\n", i, pp->GetMonsNo(), pp->GetPersonalRnd());  // rand
  }

  GFL_PRINT("box num=%d\n", svBoxPoke->GetPokemonCountAll(m_heap->GetLowerHandle()));
  for(u32 tray=0; tray<Savedata::BoxPokemon::TRAY_MAX; ++tray)
  {
    for(u32 pos=0; pos<Savedata::BoxPokemon::TRAY_POKE_MAX; ++pos)
    {
      pml::pokepara::CoreParam* cp = svBoxPoke->GetPokemon(tray, pos, m_heap->GetLowerHandle());
      if(cp)
      {
        if(cp->GetMonsNo() != MONSNO_NULL)
        {
          GFL_PRINT("%d, %d: monsNo=%d, personalRnd=0x%X\n", tray, pos, cp->GetMonsNo(), cp->GetPersonalRnd());  // rand
        }
      }
      GFL_SAFE_DELETE(cp);
    }
  }
}


#if TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE==TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING
s32                    KawadaTestCallZukanRegisterSampleEvent::s_asingCount = 0;
PokeTool::SimpleParam  KawadaTestCallZukanRegisterSampleEvent::s_asingSp;

void KawadaTestCallZukanRegisterSampleEvent::initAsingData(void)
{
  Savedata::BoxPokemon* svBoxPoke = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
  Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();

  if(s_asingCount == 0)
  {
    s_asingSp.monsNo = MONSNO_HUSIGIDANE;
    s_asingSp.formNo = 0;
    s_asingSp.sex = pml::SEX_MALE;
    s_asingSp.isRare = false;
    ++s_asingCount;
  }
  else
  {
    setAsingNextPoke();
    if(s_asingSp.monsNo==MONSNO_HUSIGIDANE && s_asingSp.formNo==0 && s_asingSp.sex==pml::SEX_MALE && s_asingSp.isRare==false)
    {
      GFL_RELEASE_PRINT("s_asingCount=%d\n", s_asingCount);  // ○回目のエージングが終わった。○は1始まり。
      ++s_asingCount;
    }
  }

  // ボックスがあふれないように空きをつくっておく
  svBoxPoke->ClearPokemon(0, 0, m_heap);

  // 図鑑登録するポケモンを生成する
  pml::pokepara::PokemonParam* pp = GFL_NEW(m_heap) pml::pokepara::PokemonParam( m_heap, s_asingSp.monsNo, 30, pml::pokepara::INIT_SPEC_ID_RANDOM );
  pp->SetLangId(System::GetLang());
  pp->ChangeFormNo(s_asingSp.formNo);
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
  pp->ChangeSexDirect(s_asingSp.sex);
#endif
#if PML_DEBUG  // git_program/poke_lib/pml/include/pokepara/pml_PokemonCoreParam.h にてPML_DEBUGで括られていた。
  if(s_asingSp.isRare) pp->SetRare();
#endif
  svZukanData->DebugSetPokeDeleteForce(*pp);

  m_pp[0] = pp;
}
void KawadaTestCallZukanRegisterSampleEvent::setAsingNextPoke(void)
{
  // s_asingSpに今の値を入れた状態で呼び出すこと
  // s_asingSpに次の値を入れて返す
  bool sexUp = false;
  bool formNoUp = false;
  bool monsNoUp = false;

  // レア
  if(s_asingSp.isRare == false)
  {
    s_asingSp.isRare = true;
  }
  else
  {
    s_asingSp.isRare = false;
    sexUp = true;
  }

  // 性別
  if( sexUp )
  {
    pml::personal::LoadPersonalData( s_asingSp.monsNo, s_asingSp.formNo );
    pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );
 
    if(sexVec == pml::personal::SEX_VECTOR_ONLY_MALE || sexVec == pml::personal::SEX_VECTOR_ONLY_FEMALE || sexVec == pml::personal::SEX_VECTOR_UNKNOWN)
    {
      formNoUp = true;
    }
    else
    {
      if(s_asingSp.sex == pml::SEX_MALE)
      {
        s_asingSp.sex = pml::SEX_FEMALE;
      }
      else
      {
        formNoUp = true;
      }
    }
  }

  // フォルム
  if( formNoUp )
  {
    ++s_asingSp.formNo;
    
    pml::personal::LoadPersonalData( s_asingSp.monsNo, 0 );
    u32 formNum = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);

    if(s_asingSp.formNo < formNum)
    {
      // フォルムが変わったので、性別を初期化する
      initAsingSex();
    }
    else
    {
      monsNoUp = true;
    }
  }

  // ポケモン全国図鑑番号
  if( monsNoUp )
  {
    s_asingSp.monsNo = static_cast<MonsNo>(static_cast<s32>(s_asingSp.monsNo) +1);

    if(s_asingSp.monsNo > MONSNO_END)
    {
      s_asingSp.monsNo = MONSNO_HUSIGIDANE;
    }

    // ポケモン全国図鑑番号が変わったので、フォルムを初期化する
    initAsingFormNo();
  }
}
void KawadaTestCallZukanRegisterSampleEvent::initAsingFormNo(void)
{
  // s_asingSp.monsNoに適切な値を入れた状態で呼び出すこと
  // s_asingSp.formNoを初期化して返す(更に、s_asingSp.sexを初期化して返す)
  s_asingSp.formNo = 0;

  // フォルムが変わったので、性別を初期化する
  this->initAsingSex();
}
void KawadaTestCallZukanRegisterSampleEvent::initAsingSex(void)
{
  // s_asingSp.monsNo, s_asingSp.formNoに適切な値を入れた状態で呼び出すこと
  // s_asingSp.sexを初期化して返す
  pml::personal::LoadPersonalData( s_asingSp.monsNo, s_asingSp.formNo );
  pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );
  
  if(sexVec == pml::personal::SEX_VECTOR_ONLY_MALE)
  {
    s_asingSp.sex = pml::SEX_MALE;
  }
  else if(sexVec == pml::personal::SEX_VECTOR_ONLY_FEMALE)
  {
    s_asingSp.sex = pml::SEX_FEMALE;
  }
  else if(sexVec == pml::personal::SEX_VECTOR_UNKNOWN)
  {
    s_asingSp.sex = pml::SEX_UNKNOWN;
  }
  else
  {
    s_asingSp.sex = pml::SEX_MALE;
  }
}
#endif


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


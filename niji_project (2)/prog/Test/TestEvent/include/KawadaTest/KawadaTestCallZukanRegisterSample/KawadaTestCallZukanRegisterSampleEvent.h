#if !defined(NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTCALLZUKANREGISTERSAMPLEEVENT_H_INCLUDED)
#define NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTCALLZUKANREGISTERSAMPLEEVENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestCallZukanRegisterSampleEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.21
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <PokeTool/include/PokeToolPokeGet.h>
#include <PokeTool/include/PokeTool.h>

// 図鑑登録イベントのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>


#define TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_OLD    (0)
#define TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING  (1)

#define TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE        (TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_OLD)
//#define TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE        (TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING)
    // 必ずTEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_OLDでコミット


// 前方宣言
GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
  class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestCallZukanRegisterSampleEvent
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestCallZukanRegisterSampleEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestCallZukanRegisterSampleEvent);

public:
  //! @brief  このイベントを起動する関数
  static void StartEvent(GameSys::GameManager* gmgr);

public:
  KawadaTestCallZukanRegisterSampleEvent(gfl2::heap::HeapBase* heap);
  virtual ~KawadaTestCallZukanRegisterSampleEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  class PokeInfo
  {
  public:
    u32  monsNo;
    u32  itemId;  // 0のとき道具を持たせない
    bool eggFlag;
    bool rareFlag;
    bool getFlag;
    bool seeFlag;  // getFlagがtrueのとき使用しない
  public:
    PokeInfo(void)
      : monsNo(0),
        itemId(0),
        eggFlag(false),
        rareFlag(false),
        getFlag(false),
        seeFlag(false)
    {}
    PokeInfo(u32 a_monsNo, u32 a_itemId, bool a_eggFlag, bool a_rareFlag, bool a_getFlag, bool a_seeFlag)
      : monsNo(a_monsNo),
        itemId(a_itemId),
        eggFlag(a_eggFlag),
        rareFlag(a_rareFlag),
        getFlag(a_getFlag),
        seeFlag(a_seeFlag)
    {}
  };
  void setPartyMember(s32 memberNum, u32 itemId, bool eggFlag, bool rareFlag);  // -1のときフル、0以上のときメンバー数
  void setBoxMember(s32 memberNum, u32 itemId, bool eggFlag, bool rareFlag);  // -1のときフル、0以上のときメンバー数
  void createPoke(u32 pokeInfoNum, PokeInfo pokeInfo[]);
  void printMember(void) const;

private:
  app::event::ZukanRegisterEventParam m_eventParam;
  app::event::ZukanRegisterEvent*     m_event;
  PokeTool::PokeGetUtil::ExData m_exData;

  enum
  {
    PP_NUM  = 4
  };
  pml::pokepara::PokemonParam*  m_pp[PP_NUM];
  u32  m_registerIndex;
  u32  m_itemId;
  bool m_eggFlag;
  bool m_rareFlag;
  u32  m_pokeSetIndex;
  u32  m_pokeSetVariation;
  gfl2::heap::HeapBase*  m_heap;


#if TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE==TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING
private:
  static s32                    s_asingCount;
  static PokeTool::SimpleParam  s_asingSp;
  void initAsingData(void);
  void setAsingNextPoke(void);
  void initAsingFormNo(void);
  void initAsingSex(void);
#endif


};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_TESTEVENT_KAWADATESTCALLZUKANREGISTERSAMPLEEVENT_H_INCLUDED


#if !defined(NIJI_PROJECT_APP_APPEVENT_ZUKANREGISTER_ZUKANREGISTEREVENT_H_INCLUDED)
#define NIJI_PROJECT_APP_APPEVENT_ZUKANREGISTER_ZUKANREGISTEREVENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanRegisterEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.21
 @brief   図鑑登録の呼び出しEvent
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>

#include <PokeTool/include/PokeToolPokeGet.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>

// ポケモンリストのインクルード
#include <App/PokeList/include/PokeListSetupParam.h>
// ポケモンステータスのインクルード
#include <App/Status/include/StatusAppParam.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(GameSys)
  class GameProc;
GFL_NAMESPACE_END(GameSys)
  
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanProc;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeList)
  class Proc;
GFL_NAMESPACE_END(PokeList)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
  class Proc;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(pml)
  class PokeParty;
GFL_NAMESPACE_END(pml)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


//==============================================================================
/**
 @class     ZukanRegisterEventParam
 @brief     図鑑登録の呼び出しEventに渡す引数
 */
//==============================================================================
class ZukanRegisterEventParam
{
public:
  // git_program/niji_project/prog/PokeTool/include/PokeToolPokeGet.h
  // PokeGetUtil::UpdatePokeAtPokeGetやPokeGetUtil::PutPokeの引数を参考にして下さい。
  pml::pokepara::PokemonParam*            pp;           //!< [in]  対象のポケモン。
  PokeTool::PokeGetUtil::CallerType::Tag  callerType;   //!< [in]  呼び出す者。
  PokeTool::PokeGetUtil::ReturnParam      returnParam;  //!< [out] 戻り値パラメータ。実際に配置したところを入れて返す。                                              

public:
  ZukanRegisterEventParam(void)
    : pp(NULL),
      callerType(PokeTool::PokeGetUtil::CallerType::HOKAKU),
      returnParam()
  {}
};


//==============================================================================
/**
 @class     ZukanRegisterEvent
 @brief     図鑑登録の呼び出しEvent
 */
//==============================================================================
class ZukanRegisterEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanRegisterEvent);

public:
  //! @brief  このイベントを起動する関数
  //! @param[in,out]  param  引数。入力、出力のやりとりをする。
  //!                        本クラス内部ではparamのポインタを保持するだけなので、呼び出し元はparamを破棄しないようにして下さい。
  static void StartEvent(GameSys::GameManager* gmgr, ZukanRegisterEventParam* param);

public:
  ZukanRegisterEvent(gfl2::heap::HeapBase* heap);
  virtual ~ZukanRegisterEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:
  void setParam(ZukanRegisterEventParam* param);

private:
  ZukanRegisterEventParam*   m_param;  // ポインタを保持するだけ
  gfl2::heap::HeapBase*      m_heap;
  GameSys::GameProc*         m_proc;
  
  app::zukan::ZukanProcParam m_zukanProcParam;
  app::zukan::ZukanProc*     m_zukanProc;

  App::PokeList::CONTEXT_PARAM m_pokeListProcParam;
  App::PokeList::Proc*         m_pokeListProc;

  App::Status::APP_PARAM       m_pokeStatusProcParam;
  App::Status::Proc*           m_pokeStatusProc;
  pml::PokeParty*              m_newPokePartyForPokeStatusProc;
};


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_APPEVENT_ZUKANREGISTER_ZUKANREGISTEREVENT_H_INCLUDED


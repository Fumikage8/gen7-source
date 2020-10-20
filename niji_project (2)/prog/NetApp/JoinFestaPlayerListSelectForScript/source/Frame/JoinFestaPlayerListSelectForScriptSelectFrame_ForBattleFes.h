#if !defined(JOINFESTAPLAYERLISTSELECTFORSCRIPTSELECTFRAME_FORBATTLEFES_H_INCLUDED)
#define JOINFESTAPLAYERLISTSELECTFORSCRIPTSELECTFRAME_FORBATTLEFES_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes.h
 * @date   2016/12/06 Tue. 16:49:13
 * @author muto_yu
 * @brief  BattleFes用のTrainerSelect [0,2]のPlayerDataを返却する
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>

#include  <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewListener.h>
#include  <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h>
#include  <NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h>

#include  "../System/JoinFestaPlayerListSelectForScriptWork.h"
#include  "./JoinFestaPlayerListSelectForScriptFrameBase.h"

namespace NetApp {
namespace JoinFestaPlayerListSelectForScript {
namespace Frame {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief  BattleFes用のTrainerSelect [0,2]のPlayerDataを返却する
  *  @date   2016/12/06 Tue. 16:49:53
  */
//==============================================================================
class JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes
  : public    JoinFestaPlayerListSelectForScriptFrameBase
  , protected NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::IPokeparaProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes );
public:
  /*  ctor  */
  JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes(
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork*               pJoinFestaPlayerListSelectForScriptWork,
    NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptViewSystemAccessor* pJoinFestaPlayerListSelectForScriptViewSystemAccessor
  );
  /*  dtor  */
  virtual~JoinFestaPlayerListSelectForScriptSelectFrame_ForBattleFes();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

  void setupList();

  void setScriptSelectedFlag();

  void setActivePlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pActivePlayerData );

private:
  const pml::pokepara::CoreParam* CreatePokeparaFromPersonalData(const NetAppLib::JoinFesta::JoinFestaPersonalData& rJFPersonalData);
  bool  RegisterPokepara(const pml::pokepara::CoreParam* pPokepara);
  void  ReleasePokepara(void);

  /*  IPokeparaProvider  */
  virtual const pml::pokepara::CoreParam* OnRequestPokepara(NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase* pCaller, const NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewBase::ListType listType, const u32 dataIndex);

  /*
  */
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener        m_JoinFestaPlayerListSelectLowerViewListener;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener   m_JoinFestaPlayerListMessageMenuLowerViewListener;
  bool  m_bWaitForStatusEvent;  /**<  statusEventからの復帰待ちであるか  */

  /*  pokeicon  */
  app::util::Heap                 m_IconHeap;
  u32                             m_StartupState;

  static const u32                PokeparaCapacity  = NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX * 2;   /**<  guest + vip  */
  u32                             m_PokeparaItems;
  const pml::pokepara::CoreParam* m_pCoreParams[PokeparaCapacity];
};




}  /*  namespace Frame  */
}  /*  namespace JoinFestaPlayerListSelectForScript  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(JOINFESTAPLAYERLISTSELECTFORSCRIPTSELECTFRAME_FORBATTLEFES_H_INCLUDED)  */

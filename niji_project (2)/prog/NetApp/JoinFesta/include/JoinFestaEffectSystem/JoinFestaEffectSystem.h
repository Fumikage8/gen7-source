#if !defined(JOINFESTAEFFECTSYSTEM_H_INCLUDED)
#define JOINFESTAEFFECTSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaEffectSystem.h
 * @date   2015/10/22 Thu. 11:44:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*  gfl2  */
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>

/*  JoinFesta  */
#include  "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"
#include  "../JoinFestaDefine.h"
#include  "./JoinFestaEffectSystemDebug.h"

#include  "Field/FieldRo/include/Effect/content/FieldEffectFesWarp.h"

// =============================================================================
/**/
//==============================================================================
namespace Field   {
namespace Effect  {
  class EffectFesMessage;
  class EffectFesLevelUp;
  class EffectFesShopOpen;
  class EffectFesStartSplash;
  class EffectFesWarp;
}
}


namespace NetAppLib {
namespace JoinFesta {
  class JoinFestaPersonalData;
}
}



namespace NetApp    {
namespace JoinFesta {
  class JoinFestaResourceManager;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/22 Thu. 12:00:25
  */
//==============================================================================
class JoinFestaEffectSystem
  : public  Field::Effect::EffectFesWarp::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaEffectSystem);
public:
  enum RESULT
  {
    RESULT_CONTINUE,
    RESULT_FINISH,
    
    RESULT_
  };

  //==============================================================================
  /**
    *  @brief  イベントハンドラ
    *  @date   2016/02/12 Fri. 22:14:03
    */
  //==============================================================================
  class IEventHandler
  {
  public:
    enum  EventID
    {
      /*  warp  */
      EVENT_Warp_AppearCharacter, /**<  warp:キャラを出現させるタイミング  */
      EVENT_Warp_End,             /**<  warp:エフェクトが終了した          */
      
      EVENT_
    };

    virtual void  JoinFestaEffectSystem_OnEvent(JoinFestaEffectSystem* pCaller, const EventID eventID) = 0;

    virtual ~IEventHandler()  {}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  ShowMessageEffect(const NetAppLib::JoinFesta::JoinFestaPersonalData* pTargetPersonalData, const bool isImmediate = false);
  void  HideMessageEffect(const bool isImmediate = false);

  

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Start_Rankup(void);
  void  Start_NewShop(void);
  void  Start_Warp(const gfl2::math::Vector3& rPos);
  void  Start_Attraction(void);

  void  Stop_Rankup(void);
  void  Stop_NewShop(void);
  // @note モデルのアニメが未対応。使いたい時はモデルのアニメフレームを最終フレームにセットする対応が必要。
  //void  Stop_Warp(void);
  void  Stop_Attraction(void);


  /*
    ctor/dtor
  */

  JoinFestaEffectSystem(
    gfl2::heap::HeapBase*         pHeapBase
    , JoinFestaResourceManager*   pResourceManager
#ifdef JF_DEBUG_MENU
    , gfl2::debug::DebugWinGroup* pParentDebugWindowGroup
#endif
  );
  virtual ~JoinFestaEffectSystem();

  void    Initialize(void);
  void    Update(void);
  RESULT  Terminate(void);

#ifdef JF_DEBUG_MENU
  void    SetParentDebugGroup();
#endif


protected:
  virtual void  EffectFesWarp_OnEvent(Field::Effect::EffectFesWarp* pCaller, const Field::Effect::EffectFesWarp::IEventHandler::EventID eventID);

  void  Event(const IEventHandler::EventID eventID)
  {
    if(m_pHandler)
    {
      m_pHandler->JoinFestaEffectSystem_OnEvent(this, eventID);
    }
  }

  gfl2::heap::HeapBase*                 m_pHeapBase;
  JoinFestaResourceManager*             m_pResourceManager;
  IEventHandler*                        m_pHandler;

  /*  message  */
  Field::Effect::EffectFesMessage*      m_pFesMessage;
  Field::Effect::EffectFesLevelUp*      m_pFesLevelUp;
  Field::Effect::EffectFesShopOpen*     m_pFesShopOpen;
  Field::Effect::EffectFesStartSplash*  m_pFesStartSplash;
  Field::Effect::EffectFesWarp*         m_pFesWarp;

  JoinFestaFriendKey                    m_LastTalkerFriendKey;   /**<  前回発言したキャラクタのFriendKeyのキャッシュ  */
  bool                                  m_IsShowingMessage;






#ifdef JF_DEBUG_MENU
  JoinFestaEffectSystemDebug  m_Debug;
#endif
};




} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTAEFFECTSYSTEM_H_INCLUDED)  */

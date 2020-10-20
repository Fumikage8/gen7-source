#if !defined(FIELDEFFECTFESWARP_H_INCLUDED)
#define FIELDEFFECTFESWARP_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FieldEffectFesWarp.h
 * @date   2016/02/09 Tue. 19:06:25
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./FieldEffectFesEffectBase.h"


namespace Field   {
namespace Effect  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2016/02/09 Tue. 18:31:41
  */
//==============================================================================
class EffectFesWarp
  : public  EffectFesEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFesWarp);
public:

  //==============================================================================
  /**
    *  @class  
    *  @brief  
    *  @date   2016/02/12 Fri. 22:42:15
    */
  //==============================================================================
  class IEventHandler
  {
  public:
    enum  EventID
    {
      EVENT_AppearCharacter,
      EVENT_End,
      
      EVENT_
    };
  
    virtual void  EffectFesWarp_OnEvent(EffectFesWarp* pCaller, const EventID eventID)  = 0;
  
    virtual ~IEventHandler(){}
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}




  EffectFesWarp(void);
  virtual ~EffectFesWarp();


  void  Start(const gfl2::math::Vector3& rPos);
  void  Stop(void);


  /*  FieldEffectBase I/F  */
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, Field::IField3DObjectNode*  in_pParent );
//  virtual void  Initialize( void* in_pResource, gfl2::heap::HeapBase* in_pHeap, const gfl2::math::Vector3&  in_vPosition );
//  virtual bool  Terminate( void );
//  virtual void  Update( void );
//  virtual void  Delete( void );
//  virtual bool  IsAnimationLastFrame(void);


protected:
  virtual void  OnSetup(void);
  virtual void  OnUpdate(void);
  virtual bool  OnTerminate(void);

  void  Event(const IEventHandler::EventID eventID);


  gfl2::Effect::Handle  m_Handle;

  enum  ModelMotionResourceID
  {
    MODEL_MOTION_RESOURCE_Model,
    MODEL_MOTION_RESOURCE_Motion,
    
    MODEL_MOTION_RESOURCE_MAX
  };
  ModelMotionResource   m_ModelMotionResource[MODEL_MOTION_RESOURCE_MAX];

  ModelInstance         m_ModelInstance;
  IEventHandler*        m_pHandler;
  
  u32                   m_FrameCount;
  bool                  m_IsPlaying;
};


} /*  namespace Effect  */
} /*  namespace Field   */
#endif  /*  #if !defined(FIELDEFFECTFESWARP_H_INCLUDED)  */

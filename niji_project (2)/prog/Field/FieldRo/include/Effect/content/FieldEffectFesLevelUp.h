#if !defined(FIELDEFFECTFESLEVELUP_H_INCLUDED)
#define FIELDEFFECTFESLEVELUP_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FieldEffectFesLevelUp.h
 * @date   2016/02/09 Tue. 18:30:55
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
class EffectFesLevelUp
  : public  EffectFesEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFesLevelUp);
public:
  EffectFesLevelUp(void);
  virtual ~EffectFesLevelUp();


  void  Start(void);
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

  gfl2::Effect::Handle  m_Handle;
};


} /*  namespace Effect  */
} /*  namespace Field   */
#endif  /*  #if !defined(FIELDEFFECTFESLEVELUP_H_INCLUDED)  */

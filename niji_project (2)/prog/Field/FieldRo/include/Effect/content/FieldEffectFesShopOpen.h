#if !defined(FIELDEFFECTFESSHOPOPEN_H_INCLUDED)
#define FIELDEFFECTFESSHOPOPEN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   FieldEffectFesShopOpen.h
 * @date   2016/02/09 Tue. 19:05:07
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
class EffectFesShopOpen
  : public  EffectFesEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectFesShopOpen);
public:
  EffectFesShopOpen(void);
  virtual ~EffectFesShopOpen();


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
#endif  /*  #if !defined(FIELDEFFECTFESSHOPOPEN_H_INCLUDED)  */

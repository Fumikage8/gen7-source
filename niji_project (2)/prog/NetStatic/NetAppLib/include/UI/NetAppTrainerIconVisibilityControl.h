#if !defined(NETAPPTRAINERICONVISIBILITYCONTROL_H_INCLUDED)
#define NETAPPTRAINERICONVISIBILITYCONTROL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppTrainerIconVisibilityControl.h
 * @date   2016/05/24 Tue. 13:02:18
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>


namespace NetAppLib {
namespace UI        {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/05/24 Tue. 13:02:47
  */
//==============================================================================
class NetAppTrainerIconVisibilityControl
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppTrainerIconVisibilityControl);
public:
  NetAppTrainerIconVisibilityControl(gfl2::heap::HeapBase* pHeapBase, const u32 slots)
    : m_pControlInfos(NULL)
    , m_Slots(slots)
  {
    m_pControlInfos = GFL_NEW(pHeapBase) ControlInfo[m_Slots];
  }
   
  virtual ~NetAppTrainerIconVisibilityControl()                                             {GFL_SAFE_DELETE(m_pControlInfos);}
  
  void  RegisterTrainerIconVisibilityControlPane(const u32 slot, gfl2::lyt::LytPane* pPane) {if(IsValidSlot(slot))  m_pControlInfos[slot].SetControlPane(pPane);}
  void  UnregisterTrainerIconVisibilityControlPane(const u32 slot)                          {RegisterTrainerIconVisibilityControlPane(slot, NULL);}
  void  UnregisterAllTrainerIconVisibilityControlPane(void)                                 {for(u32 slot = 0; slot < m_Slots; ++slot) UnregisterTrainerIconVisibilityControlPane(slot);}
  
  void  SetAllTrainerIconVisibilityExternal(const bool isVisible)                           {for(u32 slot = 0; slot < m_Slots; ++slot) SetTrainerIconVisibilityExternal(slot, isVisible);}
  void  SetTrainerIconVisibilityExternal(const u32 slot, const bool isVisible)              {if(IsValidSlot(slot))  m_pControlInfos[slot].SetVisible_External(isVisible);}

  void  ApplyTrainerIconVisibility(void)                                                    {for(u32 slot = 0; slot < m_Slots; ++slot){if(IsValidSlot(slot)) m_pControlInfos[slot].Apply();}}


protected:
  void  SetAllTrainerIconVisibilityInternal(const bool isVisible)                           {for(u32 slot = 0; slot < m_Slots; ++slot) SetTrainerIconVisibilityInternal(slot, isVisible);}
  void  SetTrainerIconVisibilityInternal(const u32 slot, const bool isVisible)              {if(IsValidSlot(slot))  m_pControlInfos[slot].SetVisible_Internal(isVisible);}
  bool  IsValidSlot(const u32 slot) const                                                   {return (m_pControlInfos ? (slot < m_Slots) : false);}

  struct  ControlInfo
  {
    gfl2::lyt::LytPane* pControlPane;
    bool                isVisible_External;
    bool                isVisible_Internal;
    
    ControlInfo()
      : pControlPane(NULL)
      , isVisible_External(false)
      , isVisible_Internal(false)
    {}
    
    void  SetControlPane(gfl2::lyt::LytPane* pPane) {pControlPane = pPane;}
    void  SetVisible_External(const bool isVisible) {isVisible_External = isVisible;}
    void  SetVisible_Internal(const bool isVisible) {isVisible_Internal = isVisible;}
    bool  IsVisible(void) const                     {return (pControlPane ? (isVisible_External & isVisible_Internal) : false);}
    void  Apply(void)                               {if(pControlPane) pControlPane->SetVisible(IsVisible());}
  };

  ControlInfo*    m_pControlInfos;
  u32             m_Slots;
};


} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPTRAINERICONVISIBILITYCONTROL_H_INCLUDED)  */

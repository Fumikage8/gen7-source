#if !defined(DIVEUI_H_INCLUDED)
#define DIVEUI_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   DiveUI.h
 * @date   2016/11/29 Tue. 18:25:10
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
#include  <util/include/gfl2_List.h>

/*    */
#include  <Layout/include/gfl2_LytSys.h>
#include  <AppLib/include/Tool/GeneralView/Module/ResourceProvider.h>
#include  <AppLib/include/Ui/UIView.h>
//#include  <NetStatic/NetAppLib/include/Util/NetAppEffectBG.h>

/*  DiveUI  */
#include  "./Views/DiveUI_Lower_BG.h"

//namespace App  {
namespace LunaSolDive  {
namespace UI  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/14 Mon. 16:22:19
  */
//==============================================================================
class DiveUI
//  : protected NetAppLib::Util::INetAppEffectBGResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(DiveUI);
public:
  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static u32  GetRequiredHeapBytes(void);

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    *  @param    [in] pHeap               heap
    *  @param    [in] pRenderingManager   renderingManager
    */
  //------------------------------------------------------------------
  DiveUI(
    gfl2::heap::HeapBase*           pHeapBase,
    app::util::AppRenderingManager* pRenderingManager
  );

  /**
    dtor
  */
  virtual ~DiveUI();

  /*
    setup
  */
  void  SetResourceProvider(app::tool::ResourceProvider* pResourceProvider) {GFL_ASSERT(!m_pResourceProvider); m_pResourceProvider = pResourceProvider;}
  
  //------------------------------------------------------------------
  /**
    *  @brief   リソース読み込みやSetupなどの処理を進行させる
    */
  //------------------------------------------------------------------
  bool  ObserveSetup(void);


  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  virtual bool  IsDrawing(void) const;
protected:
  /*
    funcs
  */
  void  CreateHeap(gfl2::heap::HeapBase* pHeapBase);
  void  Setup(void);
  /*  class INetAppEffectBGResourceProvider */
//  virtual app::util::AppRenderingManager* GetRenderingManager(void);            /**<  renderingManager  */
//  virtual app::util::Heap*                GetUtilHeap(void);                    /**<  eset用HeapBase    */
//  virtual void*                           GetResource(const u32 resourceIndex); /**<  esetデータ        */

  app::util::Heap                 m_UtilHeap;
  app::util::AppRenderingManager* m_pRenderingManager;
  app::tool::ResourceProvider*    m_pResourceProvider;
  app::tool::ResourceProvider     m_DefaultResourceProvider;

  /*  views */
//  NetAppLib::Util::NetAppEffectBG m_EffectBG;
  DiveUI_Lower_BG*                m_pLower_BG;

  u32                             m_State;
};

} /*  namespace UI  */
} /*  namespace LunaSolDive  */
//} /*  namespace App  */
#endif  /*  #if !defined(DIVEUI_H_INCLUDED)  */

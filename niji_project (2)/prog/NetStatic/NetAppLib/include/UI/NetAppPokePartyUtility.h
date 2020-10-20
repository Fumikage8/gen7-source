#if !defined(NETAPPPOKEPARTYUTILITY_H_INCLUDED)
#define NETAPPPOKEPARTYUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppPokePartyUtility.h
 * @date   2015/10/15 Thu. 18:47:28
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  <pml/include/pmlib.h>

#include  "./NetAppPokeparaUtility.h"


namespace NetAppLib {
  namespace System  {class  ApplicationWorkBase;}
namespace UI        {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/15 Thu. 18:48:16
  */
//==============================================================================
class NetAppPokePartyUtility
  : protected NetAppPokeparaUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokePartyUtility);

public:
  NetAppPokePartyUtility(System::ApplicationWorkBase* pWorkBase);
  virtual ~NetAppPokePartyUtility();


  void  OnUpdate(void)        {NetAppPokeparaUtility::OnUpdate();}
  bool  IsStable(void) const  {return NetAppPokeparaUtility::IsStable();}   /**<  終了可能であるか  */



  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    [in] partyCapacity  管理パーティ数
    *  @param    [in] pTargetView   置換対象Paneが含まれるView（PokeIconToolがGetG2DUtilするためCreatedであること）
    *  @return   
    */
  //------------------------------------------------------------------
  void  Setup(const u32 partyCapacity, app::ui::UIView*  pTargetView, const u32 layoutID);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  Cleanup(void);


  //------------------------------------------------------------------
  /**
   *  PokeIconTool::SetPokeIcon呼び出し（アイコン差し替え）
   */
  //------------------------------------------------------------------
  void  SetPokeParty(const u32 partyIndex, const pml::PokeParty* pPokeParty);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void  RegisterStatusResource(const u32  partyIndex, const u32 iconIndex, const RegisterInfo& rRegisterInfo);



protected:
  u32 m_PartyCapacity;
};



} /*  namespace UI        */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPPOKEPARTYUTILITY_H_INCLUDED)  */

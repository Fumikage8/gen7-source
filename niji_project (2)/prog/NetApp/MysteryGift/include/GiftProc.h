#if !defined(GIFTPROC_H_INCLUDED)
#define GIFTPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftProc.h
 * @date   2015/07/31 Fri. 15:06:33
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>

#include  <GameSys/include/GameProc.h>
#include  "./GiftAppParam.h"


// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Gift {
  class ApplicationSystem;
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  GiftProc
  *  @brief  おくりものProc
  *  @date   2015.05.13 Wed.
  */
//==============================================================================
class GiftProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( GiftProc );

public:
  //------------------------------------------------------------------
  /**
    *  @func     GiftProc
    *  @brief    ctor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  GiftProc( APP_PARAM* pAppParam );

  //------------------------------------------------------------------
  /**
    *  @func     ~GiftProc
    *  @brief    dtor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual ~GiftProc();

  //------------------------------------------------------------------
  /**
    *  @func     InitFunc
    *  @brief    初期化
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     EndFunc
    *  @brief    終了
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     UpdateFunc
    *  @brief    更新
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     PreDrawFunc
    *  @brief    描画準備
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     DrawFunc
    *  @brief    描画
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void               DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  APP_PARAM*          m_pAppParam;             /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;    /**<  ApplicationSystem */
  GiftWork*           m_pWork;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTPROC_H_INCLUDED)  */


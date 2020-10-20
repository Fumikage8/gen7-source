#if !defined(IDCARDPARTS_UPPERMAIN_H_INCLUDED)
#define IDCARDPARTS_UPPERMAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   IDCardParts_UpperMain.h
 * @date   2015/07/16 Thu. 18:21:20
 * @author muto_yu
 * @brief  Live_MIN_UPP_000.flytに特化したIDCardPartsBase::Setupの実装
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./IDCardParts.h"


namespace NetApp  {
namespace Live    {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/16 Thu. 16:38:42
  */
//==============================================================================
class PokemonParts_UpperMain
  : public PokemonPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PokemonParts_UpperMain );

public:
  PokemonParts_UpperMain(LiveWork* pWork);
  virtual ~PokemonParts_UpperMain();

  virtual void  Setup(const u32 slotIndex, app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0);
};



//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/16 Thu. 16:37:58
  */
//==============================================================================
class IDCardParts_UpperMain
  : public IDCardPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IDCardParts_UpperMain );

public:
  IDCardParts_UpperMain(LiveWork* pWork);
  virtual ~IDCardParts_UpperMain();

  virtual void  Setup(app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0);
};



} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(IDCARDPARTS_UPPERMAIN_H_INCLUDED)  */

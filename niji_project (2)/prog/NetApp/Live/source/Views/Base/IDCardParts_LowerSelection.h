#if !defined(IDCARDPARTS_LOWERSELECTION_H_INCLUDED)
#define IDCARDPARTS_LOWERSELECTION_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   IDCardParts_LowerSelection.h
 * @date   2015/07/16 Thu. 16:36:37
 * @author muto_yu
 * @brief  Live_SLC_LOW_000.flytに特化したIDCardPartsBase::Setupの実装
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
class PokemonParts_LowerSelection
  : public PokemonPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PokemonParts_LowerSelection );

public:
  PokemonParts_LowerSelection(LiveWork* pWork);
  virtual ~PokemonParts_LowerSelection();

  virtual void  Setup(const u32 slotIndex, app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0);
};



//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/16 Thu. 16:37:58
  */
//==============================================================================
class IDCardParts_LowerSelection
  : public IDCardPartsBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IDCardParts_LowerSelection );

public:
  IDCardParts_LowerSelection(LiveWork* pWork);
  virtual ~IDCardParts_LowerSelection();

  virtual void  Setup(app::util::G2DUtil* pG2DUtil, u32 lytID = 0, const u32 layoutMultiResourceID = 0);
};



} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(IDCARDPARTS_LOWERSELECTION_H_INCLUDED)  */

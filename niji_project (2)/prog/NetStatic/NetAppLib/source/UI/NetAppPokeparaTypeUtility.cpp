//==============================================================================
/**
 * @file   NetAppPokeparaTypeUtility.cpp
 * @date   2017/01/26 Thu. 19:50:21
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../../include/UI/NetAppPokeparaTypeUtility.h"


namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_NetAppPokeparaTypeUtility
namespace FILELOCAL {
}


// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaTypeUtility::SetCoreParam(const pml::pokepara::CoreParam* pCoreParam)
{
  GFL_ASSERT(pCoreParam);

  const MonsNo  monsNo  = pCoreParam->GetMonsNo();
  const FormNo  formNo  = pCoreParam->GetFormNo();

  pml::personal::LoadPersonalData(monsNo, formNo);
  {
    PokeType_tag  poketype[2] = {
                    static_cast<PokeType_tag>(pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type1)),
                    static_cast<PokeType_tag>(pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type2))
                  };

    /*
      図鑑側の
        GFNMCat[3114] シルヴァディのタイプアイコンが「ノーマル」のまま
      と同じ対処を施す
    */
    if((monsNo == MONSNO_ARUSEUSU) || (monsNo == MONSNO_GURIPUSU2))
    {
      poketype[0] =
      poketype[1] = static_cast<PokeType_tag>(formNo);
    }
    
    /*  同値補正  */
    if(poketype[0] == poketype[1])  poketype[1] = POKETYPE_NULL;


    m_TypeIconUtil[0].SetVisibility(true);
    m_TypeIconUtil[1].SetVisibility(true);
    if(m_TypeIconUtil[0].SetType(poketype[0]))
    {
      /*  一つ目成功 */
      m_TypeIconUtil[1].SetType(poketype[1]);   /*  二つ目は通常設定  */
    }
    else
    {
      /*  一つ目失敗  */
      m_TypeIconUtil[0].SetType(poketype[1]);   /*  前詰めのため#0に対して行う  */
      m_TypeIconUtil[0].SetVisibility(false);   /*  二つ目は非表示に            */
    }
  }
}




}  /*  namespace UI  */
}  /*  namespace NetAppLib  */

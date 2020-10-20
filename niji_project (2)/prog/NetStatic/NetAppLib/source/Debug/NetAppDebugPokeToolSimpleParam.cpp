#if PM_DEBUG

//==============================================================================
/**
 * @file   NetAppDebugPokeToolSimpleParam.cpp
 * @date   2016/05/31 Tue. 19:00:46
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/Debug/NetAppDebugPokeToolSimpleParam.h"



namespace
{
  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  LoadPersonalData(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
  {
    if(pPersonalData)
    {
      pml::personal::LoadPersonalData(rSimpleParam.monsNo, rSimpleParam.formNo, pPersonalData);
    }
    else
    {
      pml::personal::LoadPersonalData(rSimpleParam.monsNo, rSimpleParam.formNo);
    }
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsValidMonsNo(const PokeTool::SimpleParam& rSimpleParam)
  {
    return ((MONSNO_NULL < rSimpleParam.monsNo) && (rSimpleParam.monsNo <= MONSNO_END));
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  u32 GetFormNum(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
  {
    const pml::personal::ParamID  paramID_formMax = pml::personal::PARAM_ID_form_max;
    
    if(pPersonalData)
    {
      return pPersonalData->GetParam(paramID_formMax);
    }

    return pml::personal::GetPersonalParam(paramID_formMax);
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  SetNextForm(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
  {
    const u32 formNum = GetFormNum(rSimpleParam, pPersonalData);

    /*  ※formNumは最低でも1だが、念のため  */
    if(!formNum)  return false;

    if(rSimpleParam.formNo < (formNum-1))
    {
      ++rSimpleParam.formNo;
      return true;
    }
    
    return false;
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  pml::personal::SexVector  GetSexVector(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
  {
    const pml::personal::ParamID  paramID_sexVector = pml::personal::PARAM_ID_sex;
    
    if(pPersonalData)
    {
      return static_cast<pml::personal::SexVector>(pPersonalData->GetParam(paramID_sexVector));
    }

    return static_cast<pml::personal::SexVector>(pml::personal::GetPersonalParam(paramID_sexVector));
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetFirstGender(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
  {
    switch(GetSexVector(rSimpleParam, pPersonalData))
    {
      /*  ♂  */
      case pml::personal::SEX_VECTOR_ONLY_MALE:
        rSimpleParam.sex  = pml::SEX_MALE;
        break;


      /*  ♀  */
      case pml::personal::SEX_VECTOR_ONLY_FEMALE:
        rSimpleParam.sex  = pml::SEX_FEMALE;
        break;


      /*  --  */
      case pml::personal::SEX_VECTOR_UNKNOWN:
        rSimpleParam.sex  = pml::SEX_UNKNOWN;
        break;

      /*  ♂♀  */
      default:
        rSimpleParam.sex  = pml::SEX_MALE;
        break;
    }
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  SetNextGender(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData, PokeTool::PokeModelSystem* pPokeModelSystem)
  {
    switch(GetSexVector(rSimpleParam, pPersonalData))
    {
      case pml::personal::SEX_VECTOR_ONLY_MALE:     /*  ♂  */
      case pml::personal::SEX_VECTOR_ONLY_FEMALE:   /*  ♀  */
      case pml::personal::SEX_VECTOR_UNKNOWN:       /*  --  */
        return false;

      /*  ♂♀  */
      default:
        if(rSimpleParam.sex == pml::SEX_MALE)
        {
          if(pPokeModelSystem)
          {
            const s32 id_Male   = pPokeModelSystem->GetDataIdx(rSimpleParam.monsNo, rSimpleParam.formNo, pml::SEX_MALE  );
            const s32 id_Female = pPokeModelSystem->GetDataIdx(rSimpleParam.monsNo, rSimpleParam.formNo, pml::SEX_FEMALE);

            if(id_Male == id_Female)
            {
              return false;
            }
          }
          
          rSimpleParam.sex = pml::SEX_FEMALE;
          return true;
        }
        
        return false;
    }
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Dump(PokeTool::SimpleParam& rSimpleParam)
  {
    GFL_PRINT("PokeToolSimpleParamDebugger: monsNo:%4d, formNo:%2d, gender:%d, isRare:%d\n", rSimpleParam.monsNo, rSimpleParam.formNo, rSimpleParam.sex, rSimpleParam.isRare);
  }
}


namespace NetAppLib   {
namespace Debug       {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void   PokeToolSimpleParamDebugger::SetFirst(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData)
{
  rSimpleParam.monsNo   = MONSNO_HUSIGIDANE;
  rSimpleParam.formNo   = 0;
  rSimpleParam.sex      = pml::SEX_MALE;
  rSimpleParam.isRare   = false;
  rSimpleParam.isEgg    = false;
  rSimpleParam.perRand  = false;

  LoadPersonalData(rSimpleParam, pPersonalData);

  Dump(rSimpleParam);
}


//------------------------------------------------------------------
/**
  *  @brief    
  *  @retval    true:   nextを設定した
  *  @retval    false:  末尾に達しており、nextを設定しなかった
  */
//------------------------------------------------------------------
bool   PokeToolSimpleParamDebugger::SetNext(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData, PokeTool::PokeModelSystem* pPokeModelSystem)
{
  if(IsValidMonsNo(rSimpleParam))
  {
    /*  SimpleParamとPersonalDataを一致させておく  */
    LoadPersonalData(rSimpleParam, pPersonalData);
  }
  else
  {
    GFL_PRINT("PokeToolSimpleParamDebugger::SetNext: invalid monsNo (%d)\n", rSimpleParam.monsNo);
    SetFirst(rSimpleParam, pPersonalData);
  }


  /*  isRare  */
  {
    if(!rSimpleParam.isRare)
    {
      rSimpleParam.isRare = true;
      Dump(rSimpleParam);
      return true;
    }
  }

  /*  sex  */
  {
    if(SetNextGender(rSimpleParam, pPersonalData, pPokeModelSystem))
    {
      /*  変更したら上流をリセット  */
      rSimpleParam.isRare = false;
      Dump(rSimpleParam);
      return true;
    }
  }

  /*  form  */
  {
    if(SetNextForm(rSimpleParam, pPersonalData))
    {
      /*  変更したら上流をリセット  */
      LoadPersonalData(rSimpleParam, pPersonalData);    /*  monsNo, formNo決定後に  */

      SetFirstGender(rSimpleParam, pPersonalData);
      rSimpleParam.isRare = false;
      Dump(rSimpleParam);
      return true;
    }
  }

  /*  monsNo  */
  {
    if(rSimpleParam.monsNo < MONSNO_END)
    {
      rSimpleParam.monsNo = static_cast<MonsNo>(static_cast<u32>(rSimpleParam.monsNo)+1);
      rSimpleParam.formNo = 0;

      /*  変更したら上流をリセット  */
      LoadPersonalData(rSimpleParam, pPersonalData);    /*  monsNo, formNo決定後に  */

      SetFirstGender(rSimpleParam, pPersonalData);
      rSimpleParam.isRare = false;
      Dump(rSimpleParam);
      return true;
    }
  }
  
  GFL_PRINT("PokeToolSimpleParamDebugger::SetNext done.\n");
  return false;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  PokeToolSimpleParamDebugger::Set(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData, PokeTool::PokeModelSystem* pPokeModelSystem)
{
  if(IsValidMonsNo(rSimpleParam))
  {
    {
      PokeTool::SimpleParam  tempParam = rSimpleParam;
      
      tempParam.formNo  = 0;
      LoadPersonalData(tempParam, pPersonalData);
      
      /*  form  */
      {
        const u32 formNum = GetFormNum(rSimpleParam, pPersonalData);
        if(formNum <= rSimpleParam.formNo)  rSimpleParam.formNo = formNum-1;
      }

      /*  gender  */
      {
        switch(GetSexVector(rSimpleParam, pPersonalData))
        {

          case pml::personal::SEX_VECTOR_ONLY_FEMALE:   /*  ♀  */
            rSimpleParam.sex  = pml::SEX_FEMALE;
            break;

          case pml::personal::SEX_VECTOR_UNKNOWN:       /*  --  */
            rSimpleParam.sex  = pml::SEX_UNKNOWN;
            break;

          /*  ♂♀  */
          case pml::personal::SEX_VECTOR_ONLY_MALE:     /*  ♂  */
          default:
            rSimpleParam.sex  = pml::SEX_MALE;
            break;
        }
      }
    }
  }
  else
  {
    GFL_PRINT("PokeToolSimpleParamDebugger::Set: invalid monsNo (%d)\n", rSimpleParam.monsNo);
    SetFirst(rSimpleParam, pPersonalData);
  }
}


} /*  namespace Debug       */
} /*  namespace NetAppLib   */

#endif /* PM_DEBUG  */


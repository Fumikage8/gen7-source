#if PM_DEBUG

//==============================================================================
/**
 @file    ArcIdGetter.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.03.26
 @brief   ARCIDゲッター
 */
//==============================================================================

// nijiのインクルード
#include "System/include/PokemonVersion.h"
#include "System/include/ArcIdGetter.h"


GFL_NAMESPACE_BEGIN(System)


template<typename T_VAL_TYPE>
static T_VAL_TYPE getArcIdVal(
    T_VAL_TYPE baseARCID,
    T_VAL_TYPE arcId_VERSION_A_DEBUG_NO,
    T_VAL_TYPE arcId_VERSION_A_DEBUG_YES,
    T_VAL_TYPE arcId_VERSION_B_DEBUG_NO,
    T_VAL_TYPE arcId_VERSION_B_DEBUG_YES
)
{
  T_VAL_TYPE currArcId = baseARCID;

  enum
  {
    CONFIG_DEBUG_NO,
    CONFIG_DEBUG_YES,
  };

  u8 cassetteVersion = System::GetVersion();
  s32 configDebug    = CONFIG_DEBUG_NO;
#if PM_DEBUG
  configDebug    = CONFIG_DEBUG_YES;
#endif  // PM_DEBUG

  switch(cassetteVersion)
  {
  case VERSION_MOON2:
    switch(configDebug)
    {
    case CONFIG_DEBUG_NO:  currArcId = arcId_VERSION_A_DEBUG_NO;  break;
    case CONFIG_DEBUG_YES: currArcId = arcId_VERSION_A_DEBUG_YES; break;
    };
    break;
  case VERSION_SUN2 :
    switch(configDebug)
    {
    case CONFIG_DEBUG_NO:  currArcId = arcId_VERSION_B_DEBUG_NO;  break;
    case CONFIG_DEBUG_YES: currArcId = arcId_VERSION_B_DEBUG_YES; break;
    };
    break;
  };
  
  return currArcId;
}


gfl2::fs::ArcFile::ARCID ArcIdGetter::getArcIdNum(
    gfl2::fs::ArcFile::ARCID baseARCID,
    gfl2::fs::ArcFile::ARCID arcId_VERSION_A_DEBUG_NO,
    gfl2::fs::ArcFile::ARCID arcId_VERSION_A_DEBUG_YES,
    gfl2::fs::ArcFile::ARCID arcId_VERSION_B_DEBUG_NO,
    gfl2::fs::ArcFile::ARCID arcId_VERSION_B_DEBUG_YES
)
{
  return getArcIdVal<gfl2::fs::ArcFile::ARCID>(
    baseARCID,
    arcId_VERSION_A_DEBUG_NO,
    arcId_VERSION_A_DEBUG_YES,
    arcId_VERSION_B_DEBUG_NO,
    arcId_VERSION_B_DEBUG_YES
  );
}

const char* ArcIdGetter::getArcIdStr(
    const char* baseARCID,
    const char* arcId_VERSION_A_DEBUG_NO,
    const char* arcId_VERSION_A_DEBUG_YES,
    const char* arcId_VERSION_B_DEBUG_NO,
    const char* arcId_VERSION_B_DEBUG_YES
)
{
  return getArcIdVal<const char*>(
    baseARCID,
    arcId_VERSION_A_DEBUG_NO,
    arcId_VERSION_A_DEBUG_YES,
    arcId_VERSION_B_DEBUG_NO,
    arcId_VERSION_B_DEBUG_YES
  );
}


GFL_NAMESPACE_END(System)

#endif  // PM_DEBUG

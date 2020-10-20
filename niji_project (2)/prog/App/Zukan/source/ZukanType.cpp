//==============================================================================
/**
 @file    ZukanType.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.01
 @brief   図鑑のタイプ
 */
//==============================================================================


#ifdef GF_PLATFORM_WIN32  // windows.hのmin/max対策
#define NOMINMAX
#endif




// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

// nijiのインクルード
#include <System/include/PokemonVersion.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include "ZukanType.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)




const u8 ZukanType::ZUKAN_LANG_2_POKEMON_LANG[ZUKAN_LANG_NUM] =
{
  POKEMON_LANG_JAPAN   ,
  POKEMON_LANG_ENGLISH ,
  POKEMON_LANG_FRANCE  ,
  POKEMON_LANG_ITALY   ,
  POKEMON_LANG_GERMANY ,
  POKEMON_LANG_SPAIN   ,
  POKEMON_LANG_KOREA   ,
  POKEMON_LANG_CHINA   ,
  POKEMON_LANG_TAIWAN  ,
};

const ZukanType::ZukanLang ZukanType::POKEMON_LANG_2_ZUKAN_LANG[POKEMON_LANG_NUM] =
{
  ZUKAN_LANG_DUMMY   ,  // POKEMON_LANG_NULL
  ZUKAN_LANG_JAPAN   ,  // POKEMON_LANG_JAPAN  
  ZUKAN_LANG_ENGLISH ,  // POKEMON_LANG_ENGLISH
  ZUKAN_LANG_FRANCE  ,  // POKEMON_LANG_FRANCE 
  ZUKAN_LANG_ITALY   ,  // POKEMON_LANG_ITALY  
  ZUKAN_LANG_GERMANY ,  // POKEMON_LANG_GERMANY
  ZUKAN_LANG_DUMMY   ,  // なし 
  ZUKAN_LANG_SPAIN   ,  // POKEMON_LANG_SPAIN  
  ZUKAN_LANG_KOREA   ,  // POKEMON_LANG_KOREA  
  ZUKAN_LANG_CHINA   ,  // POKEMON_LANG_CHINA 
  ZUKAN_LANG_TAIWAN  ,  // POKEMON_LANG_TAIWAN 
};


u8 ZukanType::ConvertZukanLang2PokemonLang(ZukanLang zukanLang)
{
  return ZUKAN_LANG_2_POKEMON_LANG[zukanLang];
}

ZukanType::ZukanLang ZukanType::ConvertPokemonLang2ZukanLang(u8 pokemonLang)
{
  return POKEMON_LANG_2_ZUKAN_LANG[pokemonLang];
}


const nw::ut::Color8 ZukanType::POKE_GET_COLOR(255,255,255,255);
const nw::ut::Color8 ZukanType::POKE_SEE_COLOR(80,80,80,255);

const gfl2::math::Vector4 ZukanType::CROSS_FADE_COLOR_START(0,0,0,0);
const gfl2::math::Vector4 ZukanType::CROSS_FADE_COLOR_END(0,0,0,0);


const u16 ZukanType::INITIAL_TYPE_NUM[ZUKAN_LANG_NUM] =
{
  47,  // ZUKAN_LANG_JAPAN  
  27,  // ZUKAN_LANG_ENGLISH
  27,  // ZUKAN_LANG_FRANCE 
  27,  // ZUKAN_LANG_ITALY  
  27,  // ZUKAN_LANG_GERMANY
  27,  // ZUKAN_LANG_SPAIN  
  15,  // ZUKAN_LANG_KOREA  
  24,  // ZUKAN_LANG_CHINA     簡体字simplified               中国内陸(北京をはじめ中国全土、中国大陸、中華人民共和国)、シンガポール  アルファベットで検索
  36,  // ZUKAN_LANG_TAIWAN    繁体字traditional  画数が多い  台湾、香港、マカオ                                                      画数で検索
};


u32 ZukanType::GetInitialTypeNum(ZukanLang zukanLang)
{
#if PM_DEBUG
  for(u32 i=0; i<ZUKAN_LANG_NUM; ++i)
  {
    GFL_ASSERT( INITIAL_TYPE_NUM_MAX >= INITIAL_TYPE_NUM[i] );  // 開発中に気付かせるためのASSERT。
  }
#endif
  return INITIAL_TYPE_NUM[zukanLang];
}
u32 ZukanType::GetInitialTypeElemNum(ZukanLang zukanLang)
{
  return (GetInitialTypeNum(zukanLang) +1);  // 頭文字数+未選択
}

void ZukanType::ResetSubjectPaneListPosInfo(PaneListPosInfo* paneListPosInfo)
{
  for(u32 i=0; i<SUBJECT_NUM; ++i)
  {
    paneListPosInfo[i] = PaneListPosInfo();  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
}
bool ZukanType::IsSubjectPaneListPosInfoReset(const PaneListPosInfo* paneListPosInfo)
{
  bool ret = true;
  for(u32 i=0; i<SUBJECT_NUM; ++i)
  {
    if( paneListPosInfo[i].pos != 0 )  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
    {
      ret = false;
      break;
    }
  }
  return ret;
}

ZukanType::Subject      ZukanType::GetSubjectFromPos(u32 pos)
{
  if(pos >= SUBJECT_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;
  }
  return static_cast<ZukanType::Subject>(pos);
}
ZukanType::OrderType    ZukanType::GetOrderTypeFromPos(u32 pos)
{
  if(pos >= ORDER_TYPE_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  return static_cast<ZukanType::OrderType>(pos);
}
ZukanType::InitialType  ZukanType::GetInitialTypeFromPos(u32 pos, ZukanLang zukanLang)
{
  ZukanType::InitialType initialType = INITIAL_TYPE_NONE;
  if(pos >= GetInitialTypeElemNum(zukanLang))
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) initialType = static_cast<ZukanType::InitialType>(pos-1);
  return initialType;
}
ZukanType::PoketypeType ZukanType::GetPoketypeTypeFromPos(u32 pos)
{
  ZukanType::PoketypeType poketypeType = POKETYPE_TYPE_NONE; 
  if(pos >= POKETYPE_TYPE_ELEM_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) poketypeType = static_cast<ZukanType::PoketypeType>(pos-1);
  return poketypeType;
}
ZukanType::ColorType    ZukanType::GetColorTypeFromPos(u32 pos)
{
  ZukanType::ColorType colorType = COLOR_TYPE_NONE;
  if(pos >= COLOR_TYPE_ELEM_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) colorType = static_cast<ZukanType::ColorType>(pos-1);
  return colorType;
}
ZukanType::StyleType    ZukanType::GetStyleTypeFromPos(u32 pos)
{
  ZukanType::StyleType styleType = STYLE_TYPE_NONE;
  if(pos >= STYLE_TYPE_ELEM_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) styleType = static_cast<ZukanType::StyleType>(pos-1);
  return styleType;
}
ZukanType::FigureType   ZukanType::GetFigureTypeFromPos(u32 pos)
{
  ZukanType::FigureType figureType = FIGURE_TYPE_NONE;
  if(pos >= FIGURE_TYPE_ELEM_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) figureType = static_cast<ZukanType::FigureType>(pos-1);
  return figureType;
}
ZukanType::GetType      ZukanType::GetGetTypeFromPos(u32 pos)
{
  ZukanType::GetType getType = GET_TYPE_NONE;
  if(pos >= GET_TYPE_ELEM_NUM)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。
    pos = 0;  // ZukanType::AaaTypeのAAA_TYPE_DEFAULTのposは0になるようにしてあるので、0で初期化。AAA_TYPE_DEFAULTはAAA_TYPE_NONEにしてある。
  }
  if(pos > 0) getType = static_cast<ZukanType::GetType>(pos-1);
  return getType;
}


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
// 図鑑デバッグ情報
ZukanDebugInfo::ZukanDebugInfo(void)
{
  this->Init();
}
void ZukanDebugInfo::Init(void)
{
  for(u32 i=0; i<TIME_NUM_MAX; ++i)
  {
    time[i] = 0;
  }
}
void ZukanDebugInfo::SetCurrTime(u32 index)
{
  time[index] = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
}
void ZukanDebugInfo::Print(void)
{
  GFL_PRINT("time[micro second]\n");
  for(u32 i=0; i<TIME_NUM_MAX; ++i)
  {
    GFL_PRINT("[%d] %lld\n", i, time[i]);
  }
}
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


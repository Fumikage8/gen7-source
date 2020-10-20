//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		PokeToolRoptomSound.cpp
 *	@brief  ロトム図鑑用SE
 *	@author	ariizumi nobuhiko
 *	@date		2017.04.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "PokeTool/include/PokeToolRotomSound.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <pml/include/pmlib.h>
#include "System/include/PokemonVersion.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(PokeTool)

#include "PokeToolRotomSoundTabel.cdat"


//SEテーブル
static const int SeTable[16] =
{
  SEQ_SE_MJ_SYS_046, SEQ_SE_MJ_SYS_047, SEQ_SE_MJ_SYS_048, SEQ_SE_MJ_SYS_049,
  SEQ_SE_MJ_SYS_050, SEQ_SE_MJ_SYS_051, SEQ_SE_MJ_SYS_052, SEQ_SE_MJ_SYS_053,
  SEQ_SE_MJ_SYS_054, SEQ_SE_MJ_SYS_055, SEQ_SE_MJ_SYS_056, SEQ_SE_MJ_SYS_057,
  SEQ_SE_MJ_SYS_058, SEQ_SE_MJ_SYS_059, SEQ_SE_MJ_SYS_060, SEQ_SE_MJ_SYS_061,
};


void PlayRotomSound(MonsNo monsno, FormNo formno, int sex, int idx)
{
  /*  check prms  */
  const bool  bValidMonsNo  = ((MONSNO_HUSIGIDANE <= monsno) && (monsno <= MONSNO_END));
  const bool  bValidFormNo  = (formno < MAX_FORM_NUM);
  const bool  bValidGender  = ((sex == pml::SEX_MALE) || (sex == pml::SEX_FEMALE) || (sex == pml::SEX_UNKNOWN));
  const bool  bValidIndex   = (idx < 4);


  if(!(bValidMonsNo && bValidFormNo && bValidGender && bValidIndex))
  {
    GFL_RELEASE_PRINT("invalid parameter: [%d:%d:%d][%d]\n", monsno, formno, sex, idx);
    GFL_ASSERT(bValidMonsNo);
    GFL_ASSERT(bValidFormNo);
    GFL_ASSERT(bValidGender);
    GFL_ASSERT(bValidIndex);
    return;
  }



  /**
    @fix  MMcat[612]   ポケモンから発せられる信号音の設定が正しくないものがいる。
      性別の一致について、personalでは M,F,S,Uの4パターンがあるため一致条件も4種必要となる。
      トーン再生についても完全一致でないと再生しない方針としたため、全体的に修正した
  */
  for (int i = 0; i < sRotomSeDataNum; i++)
  {
    const BTLV_ROTOM_SE_DATA& rSEData = sRotomSeDataTable[i];
    
    if (rSEData.monsno == monsno)
    {
      if(rSEData.formno == formno)
      {
        bool  bMatch  = false;
        
        switch(rSEData.genderLetter)
        {
          case  'M':  bMatch  = (sex == pml::SEX_MALE);     break;
          case  'F':  bMatch  = (sex == pml::SEX_FEMALE);   break;
          case  'S':  bMatch  = ((sex == pml::SEX_MALE) || (sex == pml::SEX_FEMALE)); break;    /*  Same  */
          case  'U':  bMatch  = (sex == pml::SEX_UNKNOWN);  break;  /*  Unknown  */
          default:  GFL_ASSERT_MSG(false, "invalid genderLetter #[%d]:0x%2x\n", i, rSEData.genderLetter); break;
        }
        
        if(bMatch)
        {
          GFL_RELEASE_PRINT("[%d:%d:%d]->[%d:0x%x]\n", monsno, formno, sex, idx, sRotomSeDataTable[i].seIdx[idx]);
          Sound::PlaySE(SeTable[sRotomSeDataTable[i].seIdx[idx]], 0);
          return;   /*  success  */
        }
      }
    }
  }

  /*  not found  */
  //範囲外のポケモンで音がならない
  GFL_ASSERT_MSG(false, "Rotom se poke not found!!!: mons#:%d, formno:%d, gender:%d, idx:%d\n", monsno, formno, sex, idx); //@check



}

GFL_NAMESPACE_END(PokeTool)

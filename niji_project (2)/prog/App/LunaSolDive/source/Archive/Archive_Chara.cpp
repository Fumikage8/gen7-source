//======================================================================
/**
 * @file Archive_Chara.cpp
 * @date 2016/11/29 21:41:00
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Chara.h"
#include "../Assert.h"
#include "arc_def_index/arc_def.h"
#include "arc_index/LunaSolDiveChara.gaix"

namespace LunaSolDive{
namespace Archive{
// constructor
Chara::Chara()
    : Base(){}
// Initialize
void Chara::Initialize(
            gfl2::heap::HeapBase* pHeap,
            gfl2::fs::AsyncFileManager* pFileManager)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pFileManager != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Chara();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  pInstance->Base::Initialize(
              pHeap,
              pFileManager,
              ARCID_LUNA_SOL_DIVE_CHARA,
              GARC_LunaSolDiveChara_DATA_NUM,
              false);
}
// Terminate
void Chara::Terminate()
{
  if(pInstance != NULL)
  {
    pInstance->Base::Terminate();
    GFL_DELETE(pInstance);
    pInstance = NULL;
  }
}
// getter
const Chara& Chara::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// Instance
Chara* Chara::pInstance = NULL;
// Index of hero's motion
MotionIndex HeroMotionIndex(
            const Version& version,
            const HeroSex& sex)
{
  LUNA_SOL_DIVE_ASSERT(version != VERSION__NONE);
  LUNA_SOL_DIVE_ASSERT(sex != HERO_SEX__NONE);
  if(sex == HERO_SEX__MALE && version == VERSION__SOL)
  {// Male, Sol
    const MotionIndex result = {
            GARC_LunaSolDiveChara_p1_base_sd001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_sd018_glide06_reset_GFBMOT};
    return result;
  }
  else if(sex == HERO_SEX__MALE && version == VERSION__LUNA)
  {// Male Luna
    const MotionIndex result = {
            GARC_LunaSolDiveChara_p1_base_ld001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p1_base_ld018_glide06_reset_GFBMOT};
    return result;
  }
  else if(sex == HERO_SEX__FEMALE && version == VERSION__SOL)
  {// Female, Sol
    const MotionIndex result = {
            GARC_LunaSolDiveChara_p2_base_sd001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_sd018_glide06_reset_GFBMOT};
    return result;
  }
  else
  {// Female, Luna
    const MotionIndex result = {
            GARC_LunaSolDiveChara_p2_base_ld001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_p2_base_ld018_glide06_reset_GFBMOT};
    return result;
  }
}
// Index of pokemon's motion
MotionIndex PokemonMotionIndex(
            const Version& version)
{
  LUNA_SOL_DIVE_ASSERT(version != VERSION__NONE);
  if(version == VERSION__SOL)
  {
    const MotionIndex result = {
            GARC_LunaSolDiveChara_pm0873_01_sd001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0873_01_sd018_glide06_reset_GFBMOT};
    return result;
  }
  else
  {
    const MotionIndex result = {
            GARC_LunaSolDiveChara_pm0874_01_ld001_walk01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld002_walk02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld003_walk03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld010_walk04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld011_walk05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld016_walk06_reset_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld004_run01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld005_run02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld006_run03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld012_run04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld013_run05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld017_run06_reset_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld007_glide01_straight_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld008_glide02_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld009_glide03_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld014_glide04_turn_left_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld015_glide05_turn_right_GFBMOT,
            GARC_LunaSolDiveChara_pm0874_01_ld018_glide06_reset_GFBMOT};
    return result;
  }
}
}// namespace Archive
}// namespace LunaSolDive

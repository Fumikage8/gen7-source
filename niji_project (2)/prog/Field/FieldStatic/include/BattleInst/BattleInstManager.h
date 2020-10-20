//======================================================================
/**
 * @file BattleInstManager.h
 * @date 2016/02/23 18:06:49
 * @author onoue_masayuki
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLEINSTMANAGER_H_INCLUDED__
#define __BATTLEINSTMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "Field/FieldRos/FieldGimmickBattleInst/include/FieldGimmickBattleInst.h"

GFL_NAMESPACE_BEGIN( BattleInst )

/**
 * @class 
 */
class BattleInstManager
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleInstManager );

public:
  BattleInstManager();
  ~BattleInstManager();

  void Create( GameSys::GameManager *p_gman );
  void Delete();
  BattleInst* GetBattleInst(){ return pBattleInst; }
  bool IsCreated(){ return pBattleInst != NULL; }
  BattleInst::MoveModelData* GetModelData(int i);

private:
  BattleInst* pBattleInst;
  gfl2::ro::Module* pModule;
};

GFL_NAMESPACE_END(BattleInst)

#endif // __BATTLEINSTMANAGER_H_INCLUDED__
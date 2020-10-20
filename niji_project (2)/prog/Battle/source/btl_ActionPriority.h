//=============================================================================
/**
 * @file   btl_ActionPriority.h
 * @date   2016/01/08 16:49:03
 * @author obata_toshihiro
 * @brief  ポケモンの行動優先度
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ACTIONPRIORITY_H_INCLUDED
#define BTL_ACTIONPRIORITY_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_action.h"

GFL_NAMESPACE_BEGIN( btl )

class BTL_POKEPARAM;

/**
 *  行動優先順計算用 定数群
 */
static const u32 ACTPRI_BIT_AGILITY = 13;
static const u32 ACTPRI_BIT_SP      =  3;
static const u32 ACTPRI_BIT_WAZA    =  6;
static const u32 ACTPRI_BIT_ACT     =  3;

static const u32 ACTPRI_BITMASK_AGILITY = ((1 << ACTPRI_BIT_AGILITY) - 1);
static const u32 ACTPRI_BITMASK_SP      = ((1 << ACTPRI_BIT_SP) - 1);
static const u32 ACTPRI_BITMASK_WAZA    = ((1 << ACTPRI_BIT_WAZA) - 1);
static const u32 ACTPRI_BITMASK_ACT     = ((1 << ACTPRI_BIT_ACT) - 1);

static const u32 ACTPRI_BITSHIFT_AGILITY = 0;
static const u32 ACTPRI_BITSHIFT_SP      = ACTPRI_BIT_AGILITY;
static const u32 ACTPRI_BITSHIFT_WAZA    = (ACTPRI_BITSHIFT_SP + ACTPRI_BIT_SP);
static const u32 ACTPRI_BITSHIFT_ACT     = (ACTPRI_BITSHIFT_WAZA + ACTPRI_BIT_WAZA);

static const u32 ACTPRI_BITMASK_HANDLER  = ((1 << (ACTPRI_BIT_AGILITY + ACTPRI_BIT_SP + ACTPRI_BIT_WAZA)) - 1);

extern u32    ActPri_Calc( BtlCompetitor competitor, const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* actParam, u8 clientId, u16 agility, WazaNo wazaNo, u8 wazaPriority );
extern u32    ActPri_Make( u8 actPri, u8 wazaPri, u8 spPri, u16 agility );
extern u32    ActPri_ChangeAgility( u32 defPriority, u16 agility );
extern u32    ActPri_ChangeWazaPriority( u32 defPriority, u8 wazaPri );
extern u8     ActPri_GetWazaPri( u32 priValue );
extern u8     ActPri_GetSpPri( u32 priValue );
extern u8     ActPri_GetActPri( u32 priValue );
extern u32    ActPri_SetSpPri( u32 priValue, u8 spPri );
extern u32    ActPri_ToHandlerPri( u32 priValue );
extern int    ActPri_ToWazaOrgPri( u32 priValue );


GFL_NAMESPACE_END( btl )

#endif // BTL_ACTIONPRIORITY_H_INCLUDED
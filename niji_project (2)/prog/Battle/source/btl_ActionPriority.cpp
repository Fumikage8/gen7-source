//=============================================================================
/**
 * @file   btl_ActionPriority.cpp
 * @date   2016/01/08 16:49:03
 * @author obata_toshihiro
 * @brief  ポケモンの行動優先度
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ActionPriority.h"
#include "./btl_pokeparam.h"

GFL_NAMESPACE_BEGIN( btl )


/**
 * @brief ポケモンの行動の優先度を計算する
 * @param competitor    対戦相手
 * @param bpp           行動するポケモン
 * @param actParam      行動パラメータ
 * @param clientId      行動するポケモンのクライアントID
 * @param agility       行動するポケモンのすばやさ
 * @param wazaNo        技を撃つ場合、使用する技( 技を撃たない場合は WAZANO_NULL )
 * @param wazaPriority  技を撃つ場合、使用する技の優先度( 技を撃たない場合は参照しません )
 * @return 行動の優先度( 優先度高いほど数値大 )
 */
u32 ActPri_Calc( BtlCompetitor competitor, const BTL_POKEPARAM* bpp, const BTL_ACTION_PARAM* actParam, u8 clientId, u16 agility, WazaNo wazaNo, u8 wazaPriority )
{
  u8 actionPri;
  u8 wazaPri;

  if( actParam->gen.cmd == BTL_ACTION_RECPLAY_TIMEOVER )
  {
    return 0;
  }

  if( actParam->gen.cmd == BTL_ACTION_NULL )
  {
    actionPri = 4;
    return ActPri_Make( actionPri, 0, BTL_SPPRI_DEFAULT, 0 );
  }


  // 行動による優先順
  switch( actParam->gen.cmd )
  {
  case BTL_ACTION_ESCAPE:
    if( ( competitor == BTL_COMPETITOR_WILD ) && 
        ( clientId == BTL_CLIENT_ENEMY1 ) )
    {
      actionPri = 0;  // 野生ポケモンの「にげる」は「たたかう」と同じプライオリティに（移動ポケ用）
    }
    else
    {
      actionPri = 4;
    }
    break;

  case BTL_ACTION_CHANGE:   actionPri = 3; break;
  case BTL_ACTION_ITEM:     actionPri = 2; break;
  case BTL_ACTION_MEGAEVO:  actionPri = 1; break;
  case BTL_ACTION_SKIP:     actionPri = 0; break;
  case BTL_ACTION_FIGHT:    actionPri = 0; break;

  default:
    GFL_ASSERT(0);
    actionPri = 0;
    break;
  }

  // ワザ優先度
  if( actParam->gen.cmd == BTL_ACTION_FIGHT )
  {
    wazaPri = wazaPriority;
  }
  else if( actParam->gen.cmd == BTL_ACTION_SKIP )
  {
    wazaPri = 0 - pml::wazadata::WAZAPRI_MIN;  // 「はんどうで動けない」場合、ワザ優先度フラットで計算
  }
  else
  {
    wazaPri = 0;  // それ以外は行動優先順のみで判定
  }

  // 優先度
  u32 priority = ActPri_Make( actionPri, wazaPri, BTL_SPPRI_DEFAULT, agility );
  BTL_Printf( "[ActionPriority] 行動プライオリティ決定！ priority=%d, actionPri=%d, wazaPri=%d, agility=%d\n", priority, actionPri, wazaPri, agility );

  return priority;
}


 u32 ActPri_Make( u8 actPri, u8 wazaPri, u8 spPri, u16 agility )
{
  /*
    行動優先順  ...  3 BIT
    ワザ優先順  ...  6 BIT
    特殊優先順  ...  3 BIT
    素早さ      ... 16 BIT
  */


  u32 pri =( ( (actPri & ACTPRI_BITMASK_ACT)   << ACTPRI_BITSHIFT_ACT)  |
             ( (wazaPri & ACTPRI_BITMASK_WAZA) << ACTPRI_BITSHIFT_WAZA) |
             ( (spPri & ACTPRI_BITMASK_SP)     << ACTPRI_BITSHIFT_SP)   |
             ( (agility & ACTPRI_BITMASK_AGILITY) )
          );

//  TAYA_Printf("actPri=%d<<%d, wazaPri=%d<<%d, spPri=%d<<%d, agi=%d, PRI=%08x\n",
//    actPri, ACTPRI_BITSHIFT_ACT, wazaPri, ACTPRI_BITSHIFT_WAZA, spPri, ACTPRI_BITSHIFT_SP, agility, pri );

  return pri;
}

 u32 ActPri_ChangeAgility( u32 defPriority, u16 agility )
{
  u32 mask = ~(ACTPRI_BITMASK_AGILITY);
  return (defPriority & mask) | (agility & ACTPRI_BITMASK_AGILITY);
}

 u32 ActPri_ChangeWazaPriority( u32 defPriority, u8 wazaPri )
{
  u8 actPri = ActPri_GetActPri( defPriority );
  u8 spPri = ActPri_GetSpPri( defPriority );
  u16 agility = defPriority & ACTPRI_BITMASK_AGILITY;

  return ActPri_Make( actPri, wazaPri, spPri, agility );
}

 u8 ActPri_GetWazaPri( u32 priValue )
{
  return ((priValue >> ACTPRI_BITSHIFT_WAZA) & ACTPRI_BITMASK_AGILITY);
}
 u8 ActPri_GetSpPri( u32 priValue )
{
  return ((priValue >> ACTPRI_BITSHIFT_SP) & ACTPRI_BITMASK_SP);
}
 u8 ActPri_GetActPri( u32 priValue )
{
  return ((priValue >> ACTPRI_BITSHIFT_ACT) & ACTPRI_BITMASK_ACT);
}
 u32 ActPri_SetSpPri( u32 priValue, u8 spPri )
{
  priValue &= ~(ACTPRI_BITMASK_SP << ACTPRI_BITSHIFT_SP);
  priValue |= ((spPri & ACTPRI_BITMASK_SP) << ACTPRI_BITSHIFT_SP);
  return priValue;
}
 u32 ActPri_ToHandlerPri( u32 priValue )
{
  return priValue & ACTPRI_BITMASK_HANDLER;
}
 int ActPri_ToWazaOrgPri( u32 priValue )
{
  int pri = ActPri_GetWazaPri( priValue );
  pri += pml::wazadata::WAZAPRI_MIN;
  return pri;
}


GFL_NAMESPACE_END( btl )
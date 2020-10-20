/**
* @brief  フィールドデバッグ制御フラググループの定義
* @file   FieldDebugSwitchGroup.h
* @author ikuta_junya
* @data   2015.12.18
*/

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );

struct SwitchGroup
{
  enum Value
  {
    TOP = 0,      // TOP
    ENCOUNT,      // エンカウント制御
    DRAW,         // 表示制御
    COL_DRAW,     // コリジョン表示制御
    SYSTEM,       // その他
    
    MAX
  };
};

// エンカウント制御
static const s32 sc_SwitchGroupTableEncount[] =
{
  DebugTypes::Switch::ENCOUNT_THROUGH,
  DebugTypes::Switch::ENABLE_WILD_ENCOUNT,
  DebugTypes::Switch::ENABLE_TRAINER_ENCOUNT,
  DebugTypes::Switch::ENABLE_GIMMICK_ENCOUNT,
  DebugTypes::Switch::GIMMICK_ENCOUNT_LOTTERY,
  DebugTypes::Switch::BATTLE_INTRUDE_FORCE_CALL,
  DebugTypes::Switch::BATTLE_INTRUDE_FORCE_APPEAR,
  DebugTypes::Switch::ENCOUNT_POKE_COLOR_RARE,
  DebugTypes::Switch::ENC_EFF_FLASH_SYNC_ENABLE,
  DebugTypes::Switch::ENC_EFF_FLASH_SYNC,
  DebugTypes::Switch::ENC_EFF_FLASH_NUM,
  DebugTypes::Switch::ENC_EFF_FLASH_INTERVAL,
  DebugTypes::Switch::ENC_EFF_FLASH_INTERVAL_REVERSE,
  DebugTypes::Switch::ENC_EFF_FLASH_COLOR,
  DebugTypes::Switch::ENCOUNT_LOTTERY_INDEX,
};

// 表示制御
static const s32 sc_SwitchGroupTableDraw[] =
{
  DebugTypes::Switch::TERRAIN,
  DebugTypes::Switch::TERRAIN_LOD,
  DebugTypes::Switch::TERRAIN_HIGH,
  DebugTypes::Switch::TERRAIN_DISTANT_VIEW,
  DebugTypes::Switch::TERRAIN_BG_PARTS,
  DebugTypes::Switch::VISIBLE_SKYBOX,
  DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST,
  DebugTypes::Switch::VISIBLE_STATICMODEL,
  DebugTypes::Switch::VISIBLE_MOVEMODEL,
  DebugTypes::Switch::VISIBLE_PLAYER,
  DebugTypes::Switch::VISIBLE_WHERE_IS_THIS,
  DebugTypes::Switch::VISIBLE_BAG,
  // カメラページで制御する事にした
  // DebugTypes::Switch::VISIBLE_CAMERA_TARGET,
  DebugTypes::Switch::VISIBLE_TRAFFIC_PATH,
  DebugTypes::Switch::VISIBLE_ENVSOUND_INFO,
};

// コリジョン表示
static const s32 sc_SwitchGroupTableColDraw[] =
{
  DebugTypes::Switch::COL_DARW_ONLY_ACTIVE,
  DebugTypes::Switch::VISIBLE_TERRAIN_COLLISION,
  DebugTypes::Switch::VISIBLE_WALL_COLLISION,
  DebugTypes::Switch::VISIBLE_WALL_COLLISION_NAMINORI,
  DebugTypes::Switch::VISIBLE_WALL_COLLISION_ROBA2,
  DebugTypes::Switch::VISIBLE_WALL_COLLISION_RIDE,
  DebugTypes::Switch::VISIBLE_PLACEMENT_COLLISION,
  DebugTypes::Switch::VISIBLE_EVENT_POS_COLLISION,
  DebugTypes::Switch::VISIBLE_EVENT_INTRPOS_COLLISION,
  DebugTypes::Switch::VISIBLE_EVENT_TALK_COLLISION,
  DebugTypes::Switch::VISIBLE_EVENT_PUSH_COLLISION,
  DebugTypes::Switch::VISIBLE_ENCOUNT_COLLISION,
  DebugTypes::Switch::VISIBLE_TRAINER_EYE_COLLISION,
  DebugTypes::Switch::VISIBLE_FOCUS_EYE_COLLISON,
  DebugTypes::Switch::VISIBLE_FOCUS_EYE_KEEP_COLLISON,
  DebugTypes::Switch::VISIBLE_CAMERA_AREA_COLLISION,
  DebugTypes::Switch::VISIBLE_CAMERA_EYE_COLLISION,  
  DebugTypes::Switch::VISIBLE_ENV_SOUND_COLLISION,
  DebugTypes::Switch::VISIBLE_GIMMICK_ENCOUNT_ACTION_COLLISION,
  DebugTypes::Switch::VISIBLE_GIMMICK_ENCOUNT_ENCOUNT_COLLISION,
  DebugTypes::Switch::VISIBLE_EVENT_POKEFINDER,
};

// その他、システム
static const s32 sc_SwitchGroupTableSystem[] =
{
  DebugTypes::Switch::INFO_LEVEL,
  DebugTypes::Switch::ENABLE_DUMP_BLOCK_INFO,
  DebugTypes::Switch::ENABLE_DUMP_BLOCK_INFO_FOR_SCRIPT,
  DebugTypes::Switch::ENABLE_ID_EDGE,
  DebugTypes::Switch::ENABLE_NORMAL_EDGE,
  DebugTypes::Switch::ENABLE_WALL_COLLISION,
  DebugTypes::Switch::ENABLE_TERRAIN_CULLING,
  DebugTypes::Switch::UPDATE_TERRAIN_ANIM_LOCAL,
  DebugTypes::Switch::UPDATE_TERRAIN_ANIM_ONE_DAY,
  DebugTypes::Switch::ENABLE_SCENESCRIPT,
  DebugTypes::Switch::ENABLE_CAMERA_FOCUS_TEST,
  DebugTypes::Switch::ENABLE_POKEFINDER_LIGHT,
  DebugTypes::Switch::ENABLE_POKEFINDER_VAL,
  DebugTypes::Switch::ENABLE_POKEFINDER_HOL,
  DebugTypes::Switch::ENABLE_POKEFINDER_TRIAL_CHECK,
  DebugTypes::Switch::ENABLE_POKEFINDER_TRIAL_SCREEN,
  DebugTypes::Switch::ENABLE_POKEFINDER_TRIAL_OUT,
  DebugTypes::Switch::ENABLE_POKEFINDER_TRIAL_LENGTH,
  DebugTypes::Switch::ENCGRASS_ON_LENGTH,
#if defined(GF_PLATFORM_WIN32)
  DebugTypes::Switch::ENABLE_EV_CAMERA_RECORD,
#endif  //#if defined(GF_PLATFORM_WIN32)
  DebugTypes::Switch::CHECK_VALID_GROUND,
  DebugTypes::Switch::ENABLE_DEBUG_PERFORMANCE_INFO_DRAW,
  DebugTypes::Switch::RERIVE_ITEM_PER,
  DebugTypes::Switch::ENABLE_BATTLE_HIO,
  DebugTypes::Switch::ENABLE_BATTLE_RARE_EFFECT,
  DebugTypes::Switch::ENABLE_BATTLE_WEAK_ENEMY,
  DebugTypes::Switch::ENABLE_ENTRANCE_FADE_CUT,
  DebugTypes::Switch::GM_SKIP_DRAWMODE_SET,
  DebugTypes::Switch::ENABLE_SKYBOX_ALWAYS_UPDATE,
  DebugTypes::Switch::ENABLE_DROPITEM_RND_NUM,
  DebugTypes::Switch::TERRAIN_DATA_CHECK,
  DebugTypes::Switch::ENABLE_MEMORY_DANGLING_CHECK,
  DebugTypes::Switch::ENABLE_EXTHEAP_CHECK,
  DebugTypes::Switch::ENABLE_MOVEMODEL_INTEREST,
  DebugTypes::Switch::ENABLE_UNREGIST_ANIM_STOP

  // カメラページで制御する事にした
  // DebugTypes::Switch::CAMERA_STOP,
  
  // DebugTypes::Switch::TERRAIN_DISTANT_VIEW,   // カメラ用メニューで制御
};

struct SwitchGroupInfo
{
  const c16* groupName;         // グループ名
  const s32* switchList;        // グループに含めるSwitchIDリスト
  s32        switchListSize;    // リストサイズ
};


static const SwitchGroupInfo sc_SwitchGroupTable[] =
{
  { L"", NULL, 0 },
  { L"Encount", &sc_SwitchGroupTableEncount[0], GFL_NELEMS( sc_SwitchGroupTableEncount ) },
  { L"Draw", &sc_SwitchGroupTableDraw[0], GFL_NELEMS( sc_SwitchGroupTableDraw ) },
  { L"ColDraw", &sc_SwitchGroupTableColDraw[0], GFL_NELEMS( sc_SwitchGroupTableColDraw ) },
  { L"System", &sc_SwitchGroupTableSystem[0], GFL_NELEMS( sc_SwitchGroupTableSystem ) },
#if 0
  L"Encount",
  L"Draw",
  L"ColDraw",
  L"System",
  L"",
#endif
};

GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Filed );

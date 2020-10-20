#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
*	@file			SkeltonModel_define.h
*	@brief
*	@author		tamada@gamefreak.co.jp
*	@date			2014.03.13
*/
//=================================================================================================

#include "Test/Skelton/include/SkeltonCharaDefine.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

  namespace ActionCmdBase{ class ActionCmdWork; };

class SkeltonCodeBase;
class SkeltonModel;
class SkeltonModelManager;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
enum SMDL_ID
{
  SMDL_PLAYER = 100,	//@todo 数値が適当なのでちゃんと定義する
  SMDL_NPC1 = 101,
  SMDL_NPC2,
  SMDL_NPC3,

  SMDL_POKE1,				// ピカチュウ
  SMDL_POKE2,				// フォッコ
  SMDL_POKE3,				// ピジョン
  SMDL_POKE4,				// ビビオン

  SMDL_BOSS_POKE,
  SMDL_DUMMY_STONE,  //石の代わり

  SMDL_NPC_EX_START = 150,

  SMDL_MAX = 20,
  SMDL_NONE = SMDL_MAX,
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
enum SKELTON_CODE
{
  SKELTON_CODE_NONE,
  SKELTON_CODE_PLAYER,
  SKELTON_CODE_NPC,

  SKELTON_CODE_STAY,
  SKELTON_CODE_FOLLOWER,

  SKELTON_CODE_NPC_TEST,

  SKELTON_CODE_NUM,
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

enum
{
  SKELTON_CODE_WORK_SIZE = 64,
};

//-------------------------------------
///	動作コード処理　共通ワーク
//=====================================
struct SkeltonCodeWork
{
  SkeltonModel*  p_model;                  // 動作モデルワーク
  ActionCmdBase::ActionCmdWork* p_acmd;     // アクションコマンドワーク
  u32             mv_code;                  // enum MvCode型だけ代入すること！
  u32             work[SKELTON_CODE_WORK_SIZE/4]; // フリーに使える共有ワーク
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct SkeltonModelHeader
{
  SMDL_ID		SMDL_id;
  chara::RSC_ID		rsc_id;
  SKELTON_CODE	SKELTON_CODE;

  //		u16				event_type;
  //		u16				event_flag;
  //		u16				script_id;
  //		u16				dir;
  u16				param0;
  u16				param1;
  u16				param2;
  //		イベント範囲指定
  //		エイリアス指定
  f32 x,y,z;
  f32 angle;
};

struct SkeltonModelModelResourcePath
{
  c16       modelPath[512];
  c16       animePathTable[chara::MOTION_ID_MAX][512];
};

struct SkeltonModelHeaderEx
{
  s32       enable;   // @todo b32型が変わってしまったので
  SMDL_ID		SMDL_id;
  chara::RSC_ID		rsc_id;
  SkeltonModelModelResourcePath resourcePath;
  SkeltonModelModelResourcePath resourcePathImpl;
  c16*                  animaPathPtrTable[chara::MOTION_ID_MAX];
  chara::ModelResource modelResource;
  SKELTON_CODE	SKELTON_CODE;

  //		u16				event_type;
  //		u16				event_flag;
  //		u16				script_id;
  //		u16				dir;
  u16				param0;
  u16				param1;
  u16				param2;
  //		イベント範囲指定
  //		エイリアス指定
  f32 x,y,z;
  f32 angle;
};


GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
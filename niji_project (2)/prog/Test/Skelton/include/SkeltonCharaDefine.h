#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
 *	@file			chara_define.h
 *	@brief    niji用キャラ指定定義
 *	@author		tamada@gamefreak.co.jp
 *	@date			2014.05.21

 　nijiにおいては、バトル用・フィールド用で同一のキャラモデルを使用するため、統一した定義を用意する。
 */
//=================================================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

namespace chara {

	//-------------------------------------------------------------------------------------------------
  /**
   @enum
   @brief キャラクター指定IDの定義
   */
	//-------------------------------------------------------------------------------------------------
	enum RSC_ID
	{
    RSCID_PLAYER_MALE,

    RSCID_NPC00,
    RSCID_NPC01,
    RSCID_NPC02,
    RSCID_NPC03,

    RSCID_TEST_NPC_01,
    RSCID_TEST_NPC_02,
    RSCID_TEST_NPC_03,

		RSCID_NUM,
	};

	//-------------------------------------------------------------------------------------------------
  /**
   @brief キャラのモーション指定ID
   現状、全キャラ同じIDで指定することを想定している
   */
	//-------------------------------------------------------------------------------------------------
	enum {
    MOTION_ID_WAIT = 0,
    MOTION_ID_WALK,
    MOTION_ID_RUN,
    MOTION_ID_WAIT_WALK,
    MOTION_ID_WALK_WAIT,
    MOTION_ID_WAIT_RUN,
    MOTION_ID_RUN_WAIT,
    MOTION_ID_WALK_RUN,
    MOTION_ID_RUN_WALK,
    MOTION_ID_WALK_100F,	//@fixme 暫定
    MOTION_ID_RUN_100F,	//@fixme 暫定
    MOTION_ID_SP,
    MOTION_ID_WALK_FAST,

    MOTION_ID_MAX
	};

	//-------------------------------------------------------------------------------------------------
  /**
   @struct ModelResource
   @brief  キャラの持つリソース定義構造体
   */
	//-------------------------------------------------------------------------------------------------
	struct ModelResource
	{

		//　SkeltonModel Resource define
		const wchar_t * model_file_name;
		const wchar_t * const * anime_file_table;
		u16	anime_max;
		f32 adjustHeightValue;

	};

  //-------------------------------------------------------------------------------------------------
  /**
   @brief リソース取得関数
   */
	//-------------------------------------------------------------------------------------------------
  extern const ModelResource * GetModelResource( RSC_ID id );

  extern void CheckModelResource( void );

  extern const c16 * const GetMotionName( u32 motion_id );


#define FLD_PLAYER_SIZE_RATE	1.14f
	

}; //end of namespace chara

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32
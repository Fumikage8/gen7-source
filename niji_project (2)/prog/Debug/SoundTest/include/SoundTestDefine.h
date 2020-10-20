//======================================================================
/**
 * @file SoundTestDefine.h
 * @date 2015/11/02 15:02:16
 * @author saito_nozomu
 * @brief サウンドテスト関連定義ファイル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __SOUND_TEST_DEFINE_H_INCLUDED__
#define __SOUND_TEST_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

/**
 * @enum Page
 * ページ定義
 */
enum Page
{
  PAGE_MIN = 0,//! ページ最小

	PAGE_BGM = 0, //! BGM
	PAGE_ME,//! ME
	PAGE_SE,//! SE
	PAGE_PV,//! 鳴き声　開発ナンバー順
  PAGE_PV_ZUKAN,//! 鳴き声　図鑑ナンバー順
	PAGE_PIKA,//!　ピカチュウ鳴き声
		
	PAGE_MAX
};

/**
 * タッチ定義
 */
enum
{
	TOUCH_SEARCH_NO,
	TOUCH_SEARCH_NAME,
	
	TOUCH_MAX,
};

/**
 * @struct TouchInfo
 * タッチ情報
 */
struct TouchInfo
{
	int x;
	int y;
	int width;
	int height;
};

#define POKE_VOICE_NUM (22)

    

GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __SOUND_TEST_DEFINE_H_INCLUDED__

#endif //PM_DEBUG
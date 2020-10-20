﻿//=============================================================================
/**
 * @file	 FieldMessageSampleAppParam.h
 * @brief	 継承したテスト
 * @author k.ohno
 * @date	 2014.1.16
 */
//=============================================================================
#if PM_DEBUG

#ifndef __FIELD_MESSAGE_SAMPLE_APP_PARAM_H__
#define __FIELD_MESSAGE_SAMPLE_APP_PARAM_H__
#pragma once

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( fs )
class AsyncFileManager;
GFL_NAMESPACE_END( fs )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( util )


GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

/// 外部設定パラメータ（初期化データ・終了時のデータなど）
struct APP_PARAM {
	// [ in ]
  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー
  app::util::AppRenderingManager* mpAppRenderingManager;   //レンダリングマネージャー
  app::util::Heap* mpAppHeap;  //appheap
  applib::frame::Manager* mpAppFrameManager;
	// [ out ]
	u8 outNo;			            ///< 選んだ番号
};


GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif //__FIELD_MESSAGE_SAMPLE_APP_PARAM_H__

#endif // PM_DEBUG

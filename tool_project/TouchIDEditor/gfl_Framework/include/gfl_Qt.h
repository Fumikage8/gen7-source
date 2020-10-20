//==============================================================================
/**
 * @file	gfl_Qt.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/05, 14:33
 */
// =============================================================================

#ifndef GFL_QT_H
#define	GFL_QT_H
#pragma once

#define GFL_COMPANY_NAME "GameFreak"

// gfl_Core と必要な Qt を内包している
#include <qt/gfl_QtGui.h>

// QApplication を継承したクラス
#include <qt/gfl_Application.h>

// レンダークラス
#include <qt/gfl_Render.h>
// QGLWidget を継承したウイジェットクラス
#include <qt/gfl_GLWidget.h>
// QGLWindow を継承したウィンドークラス
#include <qt/gfl_GLWindow.h>
// QMainWindow を継承したウィンドークラス
#include <qt/gfl_MainWindow.h>

#ifdef USE_GLEW
#define USE_VERTEX_BUFFER
#endif

#include <qt/gfl_QtModel.h>
#include <qt/gfl_CmdlModel.h>

// Qtプロパティクラス
#include <gfl_RttiUnity.h>
#include <qt/gfl_QtProperty.h>

// ファイルクラス
#include <qt/gfl_QtFile.h>

// Zlib クラス
#include <qt/gfl_Zip.h>

#ifndef NOT_USE_HIO_DEV
#include <qt/gfl_Client.h>
#endif

#include <qt/gfl_Tga.h>
#include <qt/gfl_Ctex.h>

namespace gfl {

// BASE ========================================================================

GFL_NAMESPACE_BEGIN( base )

class Thread : QThread {
public :
	Thread( gfl::heap::HeapBase* , u32 ){}
	virtual ~Thread(){}
	
	void Wait(void){}
	void Signal(void){}
	void Start(s32){ start(); }
	void Kill(void){}
	void PauseOn(void){}
	void PauseOff(void){}
};

GFL_NAMESPACE_END( base )
	
}


#endif	/* GFL_QT_H */

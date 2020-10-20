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

// gfl_Core �ƕK�v�� Qt �����Ă���
#include <qt/gfl_QtGui.h>

// QApplication ���p�������N���X
#include <qt/gfl_Application.h>

// �����_�[�N���X
#include <qt/gfl_Render.h>
// QGLWidget ���p�������E�C�W�F�b�g�N���X
#include <qt/gfl_GLWidget.h>
// QGLWindow ���p�������E�B���h�[�N���X
#include <qt/gfl_GLWindow.h>
// QMainWindow ���p�������E�B���h�[�N���X
#include <qt/gfl_MainWindow.h>

#ifdef USE_GLEW
#define USE_VERTEX_BUFFER
#endif

#include <qt/gfl_QtModel.h>
#include <qt/gfl_CmdlModel.h>

// Qt�v���p�e�B�N���X
#include <gfl_RttiUnity.h>
#include <qt/gfl_QtProperty.h>

// �t�@�C���N���X
#include <qt/gfl_QtFile.h>

// Zlib �N���X
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

/* 
 * File:   gfl_Qt.h
 * Author: s-kimura
 *
 * Created on 2010/06/09, 14:02
 */

#ifndef GFL_QTGUI_H
#define	GFL_QTGUI_H

#include <core/gfl_Config.h>

#ifdef GFL_PLATFORM_PC
#define USE_GLEW
#endif

#ifdef USE_GLEW // ����͍ŏ��ɃC���N���[�h���Ȃ��Ƃ����Ȃ�
#include <GL/glew.h> // glew
#pragma comment(lib, "glew32.lib")
#endif

#ifdef GFL_PLATFORM_PC

#ifdef WIN64
#pragma comment(lib, "glut64.lib")
#else
#pragma comment(lib, "glut32.lib")
#endif

// Qt �͍ŏ��ɃC���N���[�h���Ȃ��Ⴂ���Ȃ�
// �K�v�Œ���� Qt ���C���N���[�h
#include <QtCore/QtCore>
#include <QtGui/QtGui>
//#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>

// �t���[�����[�N�ɂ̓R�A�N���X���K�{
#include <gfl_Core2.h>

// GL utility
#include <GL/glut.h>
#endif

namespace gfl {
namespace qt {
/**
 * ���b�Z�[�W����莞�ԕ\������
 * @param str�@�\��������
 * @param sec�@�\���b��
 */
void TimerMessage( const QString& str, const s32 sec = 3 );

}
}

#endif	/* GFL_QTGUI_H */


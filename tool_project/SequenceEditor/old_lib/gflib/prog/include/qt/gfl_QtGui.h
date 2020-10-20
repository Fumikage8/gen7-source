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

#ifdef USE_GLEW // これは最初にインクルードしないといけない
#include <GL/glew.h> // glew
#pragma comment(lib, "glew32.lib")
#endif

#ifdef GFL_PLATFORM_PC

#ifdef WIN64
#pragma comment(lib, "glut64.lib")
#else
#pragma comment(lib, "glut32.lib")
#endif

// Qt は最初にインクルードしなきゃいけない
// 必要最低限の Qt をインクルード
#include <QtCore/QtCore>
#include <QtGui/QtGui>
//#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>

// フレームワークにはコアクラスが必須
#include <gfl_Core2.h>

// GL utility
#include <GL/glut.h>
#endif

namespace gfl {
namespace qt {
/**
 * メッセージを一定時間表示する
 * @param str　表示文字列
 * @param sec　表示秒数
 */
void TimerMessage( const QString& str, const s32 sec = 3 );

}
}

#endif	/* GFL_QTGUI_H */


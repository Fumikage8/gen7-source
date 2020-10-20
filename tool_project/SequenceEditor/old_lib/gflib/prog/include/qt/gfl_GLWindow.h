/* 
 * File:   gfl_GLWindow.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 15:35
 */

#ifndef GFL_GLWINDOW_H
#define	GFL_GLWINDOW_H

#include <gfl_Qt.h>

namespace gfl {
namespace qt {
// =============================================================================
// 他ヘッダに定義されているクラスを宣言
// =============================================================================
class GLWidget;

/**
 *   OpenGL 画面を持つウィジェットクラス
 */

class GLWindow : public QWidget {
public:
	// このウィジェットはカメラを持つ
	GLWidget* _pGLWidget;

	GLWindow( QWidget* parent = 0 );
	virtual ~GLWindow( void );

	gfl::core::Camera* GetCamera( void );

	GLWidget* GetGLWidget( void ) {
		return _pGLWidget;
	}

protected:
	// Qt virtual functions
	virtual void keyPressEvent( QKeyEvent * event );
	virtual void keyReleaseEvent( QKeyEvent * event );

public:
	/**
	 *   GLWidget とカメラを設定
	 * @param  wid OpenGL を使用するウィジェットポインタ
	 * @param  cam カメラ
	 * @return 無し
	 * @remark これをしないと動作しません。（いい設計じゃないかも…）
	 */
	void SetGLWidgetAndCamera( GLWidget* wid, gfl::core::Camera* cam = 0 );
};

}
}

#endif	/* GFL_GLWINDOW_H */


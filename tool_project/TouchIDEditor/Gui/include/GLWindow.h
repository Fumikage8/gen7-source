//==============================================================================
/**
 * @file	GLWindow.h
 * @brief	
 * @author	kimura_shin
 * @data	2012/03/19, 17:27
 */
// =============================================================================

#if !defined( __GLWINDOW_H__ )
#define	__GLWINDOW_H__

#include <gflib.h>
#include "GLWidget.h"

class GLWindow : public gfl::qt::GLWindow {
//	Q_OBJECT

protected:
	void dragEnterEvent( QDragEnterEvent* event );
	void dropEvent( QDropEvent* event );
	
public:
	enum {
		DEFAULT_OUT_FPS = 30
	};

	QLabel* _pFPS;

	GLWindow( );
	virtual ~GLWindow( void );

	GLWidget* GetGLWidget( void ) {
		return (GLWidget*) _pGLWidget;
	}

//public slots:
};


#endif	/* __GLWINDOW_H__ */


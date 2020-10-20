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
// ���w�b�_�ɒ�`����Ă���N���X��錾
// =============================================================================
class GLWidget;

/**
 *   OpenGL ��ʂ����E�B�W�F�b�g�N���X
 */

class GLWindow : public QWidget {
public:
	// ���̃E�B�W�F�b�g�̓J����������
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
	 *   GLWidget �ƃJ������ݒ�
	 * @param  wid OpenGL ���g�p����E�B�W�F�b�g�|�C���^
	 * @param  cam �J����
	 * @return ����
	 * @remark ��������Ȃ��Ɠ��삵�܂���B�i�����݌v����Ȃ������c�j
	 */
	void SetGLWidgetAndCamera( GLWidget* wid, gfl::core::Camera* cam = 0 );
};

}
}

#endif	/* GFL_GLWINDOW_H */


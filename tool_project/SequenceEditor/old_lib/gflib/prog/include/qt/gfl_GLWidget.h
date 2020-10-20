/* 
 * File:   gfl_GlWidget.h
 * Author: s-kimura
 *
 * Created on 2010/06/04, 15:53
 */

#ifndef GFL_GLWIDGET_H
#define	GFL_GLWIDGET_H

#include <gfl_Qt.h>

namespace gfl { namespace qt {

// =============================================================================
// ���w�b�_�ɒ�`����Ă���N���X��錾
// =============================================================================
class Camera;

/**
 *   OpenGL Widget �N���X
 * @remark �`�惋�[�v����A�}�E�X����Ȃǂ����ʉ����Ďg�p�������̂Ń��b�v����
 */

class GLWidget : public QGLWidget {
private:
	/*
	 * �f�t�H���g�@GLWidget �̑傫��
	 */
	enum {
		DEFAULT_GLWIDGET_SIZE_X = 640, /**< OpenGL��� �f�t�H���g X �T�C�Y */
		DEFAULT_GLWIDGET_SIZE_Y = 480 /**< OpenGL��� �f�t�H���g Y �T�C�Y */
	};

	//		s32 _TimerId;
	gfl::core::Time _Time;

protected:

	enum PickType {
		PICK_NO_USE = -1,
		PICK_MAX = 8
	};

	QPoint _LastPos;
	//		QTime _Time; // �s���m�Ȃ̂Ŏg���Ȃ��c
	gfl::core::Camera* _pCamera;
	s32 _PickId[PICK_MAX];

	// Ex functions
	virtual void Render( const GLenum mode );
	virtual void SetLight( void );
	gfl::core::Vector CalcMouseVector( const s32 x, const s32 y );

	// mouse pick functions
	s32 Select( const s32 x, const s32 y );
	s32 CheckPickId( const s32 id );
	void SetPickId( const s32 id );
	void AddPickId( const s32 id );

	inline void ClearPickId( void ) {
		SetPickId( PICK_NO_USE );
	}

	// Qt virtual functions
	virtual void initializeGL( );
	virtual void paintGL( );
	virtual void resizeGL( int width, int height );
	virtual QSize minimumSizeHint( ) const;
	virtual QSize sizeHint( ) const;
	virtual void mousePressEvent( QMouseEvent *event );
	virtual void mouseReleaseEvent( QMouseEvent *event );
	virtual void mouseMoveEvent( QMouseEvent *event );

	static bool LoadShaderSource( GLuint shader, const QString& fname );
	static void PrintShaderInfoLog( GLuint shader );
	static void PrintProgramInfoLog( GLuint program );
	
	void Ortho2D( s32 left, s32 right, s32 bottom, s32 top );

public:

	class Project {
	public:
		Project( );
		void SetProject( const gfl::core::Vector& pos, GLint viewport[], GLdouble modelview[], GLdouble projection[] );

		GLdouble l_x;
		GLdouble h_x;
		GLdouble l_y;
		GLdouble h_y;
		GLdouble l_z;
		GLdouble h_z;
	};

	s32 _TargetFPS;
	s32 _CurrentFPS;
	Qt::MouseButtons m_PickButton;
	u8 _ShiftMode;
	u8 _Pad_[3]; // �A���C�������g

	/**
	 *   �R���X�g���N�^
	 * @param  parent �e�E�B�W�F�b�g�|�C���^
	 * @return �Ȃ�
	 */
	GLWidget( QWidget *parent );

	/**
	 *   �f�X�g���N�^
	 * @param  �Ȃ�
	 * @return �Ȃ�
	 */
	virtual ~GLWidget( );

	/**
	 *   ��莞�Ԃ��o�߂���ƌĂ΂��R�[���o�b�N�֐�
	 * @param  t �^�C�}�[�C�x���g
	 * @return ����
	 * @remark QWidget ����p���������z�֐�
	 */
	virtual void timerEvent( QTimerEvent* t );

	/**
	 *   �J�����|�C���^�̎擾
	 * @param  ����
	 * @return �J�����|�C���^
	 */

	gfl::core::Camera* GetCamera( void ) {
		return _pCamera;
	}

	/**
	 *   �J�����|�C���^�̐ݒ�
	 * @param  cam �J�����|�C���^
	 * @return ����
	 */

	void SetCamera( gfl::core::Camera* cam ) {
		_pCamera = cam;
	}

	/**
	 *   �ڕWFPS�̐ݒ�
	 * @param  fps �ڕWFPS
	 * @return ����
	 */
	void SetTargetFPS( const s32 fps );

	const QPoint& GetMousePos( ) const {
		return _LastPos;
	}
	
	static void GlLoadMatrix( const gfl::core::Matrix44& mat );
	static void GlMultMatrix( const gfl::core::Matrix& mat );

#ifdef USE_GLEW
	/**
	 *   �V�F�[�_�[�̃��[�h
	 * @param  vert_fname ���@�[�e�b�N�X�V�F�[�_�[�t�@�C����
	 * @param  flag_fname �t���O�����g�V�F�[�_�[�t�@�C����
	 * @return �v���O����ID
	 */

	static u32 LoadShader( const QString& vert_fname, const QString& frag_fname );
#endif
};

}
}

#endif	/* GFL_GLWIDGET_H */

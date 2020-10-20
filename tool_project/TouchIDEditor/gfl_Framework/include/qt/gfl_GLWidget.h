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
// 他ヘッダに定義されているクラスを宣言
// =============================================================================
class Camera;

/**
 *   OpenGL Widget クラス
 * @remark 描画ループ制御、マウス操作などを共通化して使用したいのでラップする
 */

class GLWidget : public QGLWidget {
private:
	/*
	 * デフォルト　GLWidget の大きさ
	 */
	enum {
		DEFAULT_GLWIDGET_SIZE_X = 640, /**< OpenGL画面 デフォルト X サイズ */
		DEFAULT_GLWIDGET_SIZE_Y = 480 /**< OpenGL画面 デフォルト Y サイズ */
	};

	//		s32 _TimerId;
	gfl::core::Time _Time;

protected:

	enum PickType {
		PICK_NO_USE = -1,
		PICK_MAX = 8
	};

	QPoint _LastPos;
	//		QTime _Time; // 不正確なので使えない…
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
	u8 _Pad_[3]; // アラインメント

	/**
	 *   コンストラクタ
	 * @param  parent 親ウィジェットポインタ
	 * @return なし
	 */
	GLWidget( QWidget *parent );

	/**
	 *   デストラクタ
	 * @param  なし
	 * @return なし
	 */
	virtual ~GLWidget( );

	/**
	 *   一定時間が経過すると呼ばれるコールバック関数
	 * @param  t タイマーイベント
	 * @return 無し
	 * @remark QWidget から継承した仮想関数
	 */
	virtual void timerEvent( QTimerEvent* t );

	/**
	 *   カメラポインタの取得
	 * @param  無し
	 * @return カメラポインタ
	 */

	gfl::core::Camera* GetCamera( void ) {
		return _pCamera;
	}

	/**
	 *   カメラポインタの設定
	 * @param  cam カメラポインタ
	 * @return 無し
	 */

	void SetCamera( gfl::core::Camera* cam ) {
		_pCamera = cam;
	}

	/**
	 *   目標FPSの設定
	 * @param  fps 目標FPS
	 * @return 無し
	 */
	void SetTargetFPS( const s32 fps );

	const QPoint& GetMousePos( ) const {
		return _LastPos;
	}
	
	static void GlLoadMatrix( const gfl::core::Matrix44& mat );
	static void GlMultMatrix( const gfl::core::Matrix& mat );

#ifdef USE_GLEW
	/**
	 *   シェーダーのロード
	 * @param  vert_fname ヴァーテックスシェーダーファイル名
	 * @param  flag_fname フラグメントシェーダーファイル名
	 * @return プログラムID
	 */

	static u32 LoadShader( const QString& vert_fname, const QString& frag_fname );
#endif
};

}
}

#endif	/* GFL_GLWIDGET_H */

/* 
 * File:   gfl_Application.h
 * Author: s-kimura
 *
 * Created on 2010/08/24, 16:00
 */

#ifndef GFL_APPLICATION_H
#define	GFL_APPLICATION_H

//#include <QtGui/QApplication>

namespace gfl {
namespace qt {
// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
class Application;
class ProgressDialog;

/**
 *   �A�v���P�[�V����
 * @remark �K�{�N���X�@���C���֐��ō쐬���Ă�������
 */

class Application : public QApplication {
private:
	/**
	 *   �R���X�g���N�^
	 * @param  argc ���C���֐��̈��������̂܂ܓ����
	 * @param  argv ���C���֐��̈��������̂܂ܓ����
	 * @return ����
	 */
	Application( int & argc, char ** argv );

public:

	/**
	 *   �C���X�^���X�̍쐬
	 * @param  argc ���C���֐��̈��������̂܂ܓ����
	 * @param  argv ���C���֐��̈��������̂܂ܓ����
	 * @return �C���X�^���X
	 * @remark ����������̂�Ex::Singleton���g���Ȃ�
	 */

	inline static Application* CreateInstance( int & argc, char ** argv ) {
		Application* ins = _spInstance;
		if( !ins ){
			_spInstance = ins = new Application( argc, argv );
		}
		return ins;
	}

	/**
	 *   �C���X�^���X�̍쐬 �C���X�g�[�����Â��ꍇ�̓A�b�v�f�[�g����
	 * @param  argc ���C���֐��̈��������̂܂ܓ����
	 * @param  argv ���C���֐��̈��������̂܂ܓ����
	 * @param  installer �C���X�g�[��
	 * @param  uninstaller �A���C���X�g�[��
	 * @return �O�@�A�b�v�f�[�g���s�����@�O�ȊO�C���X�^���X�|�C���^
	 */
	static Application* CreateInstance( int & argc, char ** argv
									 , const c8 * const installer
									 , const c8 * const uninstaller = "uninstall.exe" );

	/**
	 *   �C���X�^���X�̍폜
	 * @param  ����
	 * @return ����
	 */

	inline static void DestroyInstance( void ) {
		delete _spInstance;
		_spInstance = 0;
	}

	/**
	 *   �C���X�^���X�̎擾
	 * @param  ����
	 * @return �C���X�^���X
	 */

	inline static Application* GetInstance( void ) {
		return _spInstance;
	}

private:
	static Application* _spInstance;
};

/**
 *   �҂��_�C�A���O�̕\��
 * @remark �ʃX���b�h�ŏd������������ꍇ�ɓ��쒆�ł��邱�Ƃ������_�C�A���O��\��
 */

class ProgressDialog : public QProgressDialog {
public:
	ProgressDialog( QObject* parent, const QString& txt );
};

}
}

/**
 *   �҂��_�C�A���O�̕\��
 * @param type �Ԃ�l�̌^
 * @param err �Ԃ�l ���̂�n���Ă�������
 * @param text  �\�����b�Z�[�W
 * @param exe ���s�֐�
 * @remark �ʃX���b�h�ŏd������������ꍇ�ɓ��쒆�ł��邱�Ƃ������_�C�A���O��\��
 */
#define PROGRESS_DIALOG(type, err, text, exe, ... ) \
		/* �҂��_�C�A���O�̕\������ */ \
		gfl::qt::ProgressDialog dialog( parent( ), text ); \
		/* �L�����Z���{�^����\�����Ȃ� */ \
		dialog.setCancelButton( 0 ); \
\
		/* �ʃX���b�h�Ŏ��s�����邽�߂̌������ */ \
		QFutureWatcher<type> futureWatcher; \
		/* �I�������_�C�A���O����� */ \
		QObject::connect( &futureWatcher, SIGNAL( finished( ) ), &dialog, SLOT( reset( ) ) ); \
\
		/* �ʃX���b�h�ŃX�^�e�B�b�N�֐������s */ \
		futureWatcher.setFuture( QtConcurrent::run( exe, __VA_ARGS__ ) ); \
\
		/* �҂��_�C�A���O�̕\�� */ \
		dialog.exec( ); \
\
		/* �I���҂� */ \
		err = futureWatcher.result( );

#endif	/* GFL_APPLICATION_H */


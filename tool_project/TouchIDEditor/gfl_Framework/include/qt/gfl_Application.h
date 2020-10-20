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
// このファイルに含まれるクラス群
// =============================================================================
class Application;
class ProgressDialog;

/**
 *   アプリケーション
 * @remark 必須クラス　メイン関数で作成してください
 */

class Application : public QApplication {
private:
	/**
	 *   コンストラクタ
	 * @param  argc メイン関数の引数をそのまま入れる
	 * @param  argv メイン関数の引数をそのまま入れる
	 * @return 無し
	 */
	Application( int & argc, char ** argv );

public:

	/**
	 *   インスタンスの作成
	 * @param  argc メイン関数の引数をそのまま入れる
	 * @param  argv メイン関数の引数をそのまま入れる
	 * @return インスタンス
	 * @remark 引数があるのでEx::Singletonが使えない
	 */

	inline static Application* CreateInstance( int & argc, char ** argv ) {
		Application* ins = _spInstance;
		if( !ins ){
			_spInstance = ins = new Application( argc, argv );
		}
		return ins;
	}

	/**
	 *   インスタンスの作成 インストーラより古い場合はアップデートする
	 * @param  argc メイン関数の引数をそのまま入れる
	 * @param  argv メイン関数の引数をそのまま入れる
	 * @param  installer インストーラ
	 * @param  uninstaller アンインストーラ
	 * @return ０　アップデートを行った　０以外インスタンスポインタ
	 */
	static Application* CreateInstance( int & argc, char ** argv
									 , const c8 * const installer
									 , const c8 * const uninstaller = "uninstall.exe" );

	/**
	 *   インスタンスの削除
	 * @param  無し
	 * @return 無し
	 */

	inline static void DestroyInstance( void ) {
		delete _spInstance;
		_spInstance = 0;
	}

	/**
	 *   インスタンスの取得
	 * @param  無し
	 * @return インスタンス
	 */

	inline static Application* GetInstance( void ) {
		return _spInstance;
	}

private:
	static Application* _spInstance;
};

/**
 *   待ちダイアログの表示
 * @remark 別スレッドで重い処理をする場合に動作中であることを示すダイアログを表示
 */

class ProgressDialog : public QProgressDialog {
public:
	ProgressDialog( QObject* parent, const QString& txt );
};

}
}

/**
 *   待ちダイアログの表示
 * @param type 返り値の型
 * @param err 返り値 実体を渡してください
 * @param text  表示メッセージ
 * @param exe 実行関数
 * @remark 別スレッドで重い処理をする場合に動作中であることを示すダイアログを表示
 */
#define PROGRESS_DIALOG(type, err, text, exe, ... ) \
		/* 待ちダイアログの表示準備 */ \
		gfl::qt::ProgressDialog dialog( parent( ), text ); \
		/* キャンセルボタンを表示しない */ \
		dialog.setCancelButton( 0 ); \
\
		/* 別スレッドで実行させるための見張り役 */ \
		QFutureWatcher<type> futureWatcher; \
		/* 終わったらダイアログを閉じる */ \
		QObject::connect( &futureWatcher, SIGNAL( finished( ) ), &dialog, SLOT( reset( ) ) ); \
\
		/* 別スレッドでスタティック関数を実行 */ \
		futureWatcher.setFuture( QtConcurrent::run( exe, __VA_ARGS__ ) ); \
\
		/* 待ちダイアログの表示 */ \
		dialog.exec( ); \
\
		/* 終了待ち */ \
		err = futureWatcher.result( );

#endif	/* GFL_APPLICATION_H */


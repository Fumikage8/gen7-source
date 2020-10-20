
■ファイル

	pawn\project\
		Pawn.vcxproj		PAWNプロジェクト

	pawn\include\			PAWNインクルードファイル
		amx.h
		amxcons.h
		amxdbg.h
		bugfix_pawn.h
		gfl2_Amx.h
		gfl2_Pawn.h			PAWNラップクラス
		gfl2_PawnBase.h		PAWN基底クラス
		gfl2_PawnDebug.h	PAWNデバッグクラス
		osdefs.h
		include\
			args.inc
			console.inc
			core.inc
			datagram.inc
			default.inc
			file.inc
			fixed.inc
			float.inc
			gfl_func.inc
			process.inc
			rational.inc
			string.inc
			time.inc

	pawn\source\
		amx.c				gfl2_UnityAmx.c でincludeされているPAWN基本ソース
		amxcore.c			gfl2_UnityAmx.c でincludeされているPAWN基本ソース
		amxdbg.c			gfl2_UnityAmx.c でincludeされているPAWN基本ソース
		amxprint.c			gfl2_UnityAmx.c でincludeされているPAWN基本ソース
		float.c				gfl2_UnityAmx.c でincludeされているPAWN基本ソース
		gfl2_UnityAmx.c		PAWN基本ソースを合わせたもの
		gfl2_PawnBase.cpp	PAWN基底クラス
		gfl2_PawnDebug.cpp	PAWNデバッグクラス

	pawn\Sample\			以下はサンプル実行に必要なファイル
		Sample.sln
		project\
			Sample.vcxproj
			test.amx
		source\
			main.cpp
			PawnTask.cpp
			PawnTask.h
		script\				スクリプト
			console.inc
			convert.bat
			pawncompiler.exe
			test.p



■クラス構成

	PawnTask(サンプルソース)
			カスタムコマンドを追加したクラス
		↓	---------------------------------------------------------------------------------
	Pawn
			プラットフォーム、ハードウェアに依存する処理のカスタマイズ
		↓	---------------------------------------------------------------------------------
	PawnDebug(デバッグターゲットのみ)
			デバッグ機能. PawnIDEとの連携とhio環境の提供
		↓
	PawnBase
			PAWNクラス基底. PAWNの基本機能


	PawnDebug 以下は環境に依存しない、基本的に修正する必要のないソース
		※予定、14/10/28現在ではRTTI、hash、hio、heapに依存している

	環境に依存する修正は Pawn で吸収(メモリ関連)。


■利用方法

	Pawnクラスを継承したものを作成
		class PawnTask : public gfl::pawn::Pawn

	スクリプトコマンド関数を作成
	cell AMX_NATIVE_CALL PawnTask::ExtScript( AMX*, const cell* ){
		return 0;
	}

	コマンド関数テーブルを作成
	static const GFL_PAWN_FUNC_TABLE s_FuncTable[] =
	{
		GFL_PAWN_FUNC( ExtScript )
		GFL_PAWN_FUNC_END,
	};

	テーブルを登録する処理
	RegisterPawnFunction( s_FuncTable );

	スクリプトバイナリーデータを読み込み Load に渡す
	size = FileLoad( "test.amx", pBuff );
	cPawn.Load( pBuff, iSize, "PawnTest" );

	GFL_PAWN_FINISHED が返るまで Updata を実行
	do{	iRes = cPawn.Update();
	}while( iRes != gfl::pawn::PawnBase::GFL_PAWN_FINISHED );




■環境依存

　環境依存対応のため関数群

	メモリ関連基底.
	virtual u8*	PawnNew( const s32 iSize, const s32 iCategory )
	virtual void	PawnDel( u8 *pPtr )
	
	メモリ操作関連.
	virtual void	PawnMemCpy( void *pD, const void *pS, const s32 iLength )
	virtual void	PawnMemFill( void *pPtr, const u8 _uVal, const s32 iLength )

	PawnBase.h で定義。基底ではスタンダードな処理が記述してあります。




■スクリプトテスト

	スクリプトファイルは test.p です。
	convert.bat
	を実行する事でコンパイル、作業フォルダにコピーされます。(デバッグ実行することが前提のパス)
	
	スクリプトファイルは 10000byte に収まっていることが前提です
		pawn/Sample/source/main.cpp(26): 	u8			Mem[ 10000 ];




■動作フロー

	pawn/Sample/script/convert.bat
	を実行すると test.amx が作成される
	
	pawn/Sample/source/PawnTask.cpp(44): cell AMX_NATIVE_CALL PawnTask::ExtScript( AMX*, const cell* )
	にブレークを張っておくとスクリプト	pawn/Sample/script/test.p(12): 	ExtScript();
	で ExtScript コマンドが実行されている事が確認できる
	
	pawn/Sample/source/main.cpp(94):     // 入力を待って終了
	を有効にすると、コマンドライン実行でも出力結果を確認できる。



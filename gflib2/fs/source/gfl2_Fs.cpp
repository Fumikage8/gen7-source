//==============================================================================
/**
 @file    gfl2_Fs.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2014.08.05
 @brief   ファイルシステム
 */
//==============================================================================


// これは何もしないファイルです。


/*
// ファイルシステムの使い方
// (0) インクルードファイル
#include <fs/include/gfl2_Fs.h>

// (1) ファイルシステムの初期化処理
gfl2::fs::Result res = gfl2::fs::System::Initialize(...);

// (2) デバイスのマウント
gfl2::fs::Result res = gfl2::fs::System::Mount...Device(...);

// (3) ファイルマネージャ生成
gfl2::fs::Result res = gfl2::fs::System::CreateAsyncFileManagerFor...Device(...);

// (その他初期化処理)
...

// (4) ファイル操作
...

// (その他終了処理)
...

// (5) ファイルマネージャ破棄
gfl2::fs::Result res = gfl2::fs::System::DestroyAsyncFileManagerFor...Device();

// (6) デバイスのアンマウント
gfl2::fs::Result res = gfl2::fs::System::Unmount...Device();

// (7) ファイルシステムの終了処理
gfl2::fs::Result res = gfl2::fs::System::Terminate();
*/


/*
気を付けておくこと

・名前の文字コードをutf-8にしたため、strlenは文字数ではなくバイト数を返しているから、lenという用語の使い方に注意。
・CTR版はchar*(utf-8 マルチバイト文字列)よりwchar_t*(ワイド文字列)のほうが有利。マルチバイト文字列だと1KB程度スタックのバッファを使うので注意。
・同期版も別スレッドで行っている。スレッド切り替え処理が重いならスレッドを切り替えない同期版もつくるべきか？
  その場合は呼び出し元のスレッド数分だけファイルアクセスが起こる。メインスレッドとファイルスレッドの場合、2スレッドが同時にファイルアクセスする可能性がある。
・gfl2_FsArcFile.cpp内においてPcUtilを使っているところをdeviceを使うように変えたほうがいいかもしれない。
・Win32FileCoreとWin32Utilの記述が似ているのが気になるがまとめていない。
・CtrHioFileCoreとCtrHioUtilの記述が似ているのが気になるがまとめていない。
・nn::hio::GetHioDaemonStatusの負荷が大きいなら、あまり呼ばないようにすること。
・gfl2_FsAsyncFileManager.cpp内において、、、CriticalSectionを書く位置が散らかっているのでまとめたい。同機能を１つにした関数にまとめる。このManager内に留める。。。のがいいなあ。CriticalSectionで検索。
・gfl2_FsAsyncFileManager.cpp内において、、、AddFileEasyReadUncomp, AddArcFileLoadDataUncomp, AddArcFileLoadDataBufUncompにおいて、reqを渡してelemをつくり直すのではなく、elemを移し替える形式にしてもいい。
  その場合はReqListCriticalSecitonも必要。getReqElemDeleteOnRemoveFlagもなくすことが可能になってくる。
・CTRのHIOでは、closeするときにフラッシュは要るか？CtrHioFileCoreとCtrHioUtilにcloseがある。
・CTRのHIOのファイルnn::hio::HostFileは、デバッグ用途なのでエラー処理をしていません。
・CTRのファイルgfl2_FsCtrFileCore.hは、CTRのSDカード(デバッグ用)で使うことしか想定されていないので、エラー処理をしていません。
・CTRのSDカード(デバッグ用)のデバイスnn::fs::MountSdmcは、デバッグ用途なのでエラー処理をしていません。(Sdmcに対するnn::fs::Unmountもエラー処理をしていません)
・gfl2_FsFileThread.cppのfuncArcFileCloseの処理をきれいにしたい。@noteのところ。
*/



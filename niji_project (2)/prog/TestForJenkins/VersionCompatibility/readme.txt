//==============================================================================
/**
 * @file   readme.txt
 * @date   2016/11/02 Wed. 15:57:28
 * @author muto_yu
 * @brief  VersionCompatibility
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

<概要>

本Projectは、niji<->momiji間の通信互換性維持のため、enumやプリプロセッサ定数の値に変更がないかをチェックするためのものです。
対象のSymbolをSTATIC_ASSERTで括っており、定数に変更があった場合はその部分がビルドエラーとして報告されます。

全ての構成においてビルド対象から外されています。
チェックするためには手動で本プロジェクトをビルドする必要があります。



<対象の登録の手順>

内容に応じて3通りの方法があります。


a) enumやstatic constのみで構成されたヘッダ

  基本的には旧環境の定数をチェック用のnamespaceで括るだけです。

  1) 「旧環境」のヘッダファイルを prog\TestForJenkins\VersionCompatibility\source\BaseHeader へコピー
  2) 1) をprog\TestForJenkins\VersionCompatibility\source\VersionCompatibility.hの namespace NijiID{} 内部でinclude
      → 旧 a::b::symbol → 新 NijiID::a::b::symbol となる
  3) VersionCompatibility.cppで、include-guardに使用しているプリプロセッサシンボルをundef（後方で「新環境」のヘッダファイルをincludeできるように）
  4) VersionCompatibility.cppで「新環境」のヘッダファイルをinclude（通常使用と同様に）
  5) ヘッダ内の全てのチェック対象シンボルを、VersionCompatibility.cppのDummyFunction(){}内に IS_COMPATIBLE_CC() マクロで括って記述
      → IS_COMPATIBLE_CC(a::b::symbol);
        → do{STATIC_ASSERT(a::b::symbol, NijiID::a::b::symbol)}while(0); に展開される


b) プリプロセッサシンボル（#define  CONSTVAL (0)）
  
  namespaceで括ることができないため、別シンボルに再定義する必要があります。
  本プロジェクトではマクロを使用してenum定数に定義しなおしています。

  1) 「旧環境」のヘッダファイルを prog\TestForJenkins\VersionCompatibility\source\BaseHeader\needRedefine 内のサブディレクトリへコピー
  2) needRedefineに、redefine_<ヘッダファイル名> の形で新規ヘッダを追加
  3) 2) のヘッダ内で全てのシンボルを TO_ENUM() で括って再定義
    3-i)    1) をinclude
    3-ii)   redefineMacro.hをinclude
    3-iii)  1) 内の全てのsymbolを TO_ENUM() で括って再定義
              →TO_ENUM( NIJI_SYMBOL )
                →namespace TestForJenkins{ namespace VersionCompatibility{ namespace NijiID{enum{enum_NIJI_SYMBOL = NIJI_SYMBOL}; }}} に展開される
    3-iv)   1) 内の全てのsymbolをundef
    3-v)    1) include-guardをundef
  4) 3) をVersionCompatibility.cppでinclude
  5) ヘッダ内の全てのチェック対象シンボルを、VersionCompatibility.cppのDummyFunction(){}内に IS_COMPATIBLE_REDEF() マクロで括って記述
      ※..._CC() ではない点に注意


c) 「旧環境」ヘッダ内で別のヘッダをincludeしていたり、classの定義などがある場合

  比較対象とするsymbolのみを取り出し、チェック用ヘッダファイルを新規作成します。
  その後の手順はsymbolの形式に応じて前述の a)/b) いずれかの手順を採ります。


<tips>

  ・VersionCompatibility.cpp内でsymbolが重複する場合→.cppを分離し、以降同様の手順で進めます。


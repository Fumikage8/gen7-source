# Touch ID Editor ビルド手順

## Windows SDKのインストール

1. Windows SDKをインストール
	* VC++2010Runtime(x86/x64)がインストールされているとWindows SDKのインストールに失敗するので一旦アンインストール
	* VC++2010 SP1 updateはチェックを外してインストールしない
	* Windows SDKをインストール
	* VC++2010 SP1 updateをインストール

## Qt 4.8.6のビルド

Qt 4.8.6のVS2012対応版はバイナリが配布されていないのでソースからのビルドを行います

## QtのホームページからQt 4.8.6をダウンロード

	1. 作成したアプリをどこで使うか聞かれるので、 In-house deploymentうんちゃらを選択
	2. アプリを外で使う気か？みたいなことを聞かれるのでNoと答える
	3. LGPLライセンスについてなにやら聞かれるので、適当にはいはい答える
	4. 4.8.6は既にDeprecatedされているのでArchiveから探す

## ソースコードを修正

そのままではビルドが通らないので、一部のソースを修正します。

src/3rdparty/webkit/Source/JacaScriptCore/wtf/HashSet.h, 93行目付近

```cpp
private:
    friend void deleteAllValues<>(const HashSet&);
    friend void fastDeleteAllValues<>(const HashSet&);
 
    // msvc2012 has trouble constructing a HashTableConstIteratorAdapter from a HashTableIterator
    // despite the existence of a const_iterator cast method on the latter class.
    pair<iterator, bool> iterator_const_cast(const pair<typename HashTableType::iterator, bool>& p)
    {
        // Spell out the full conversion chain for clarity, although any of the "hint"
        // given by the alternatives below would have been enough.
        return make_pair(iterator(HashTableType::const_iterator(p.first)), p.second);
        //     return make_pair(iterator(p.first), p.second);
        //     return make_pair(HashTableType::const_iterator(p.first), p.second);
    }
```
src/3rdparty/webkit/Source/JacaScriptCore/wtf/HashSet.h, 180行目付近

```cpp
//        return m_impl.add(value);
	    return iterator_const_cast(m_impl.add(value));
```
src/3rdparty/webkit/Source/JacaScriptCore/wtf/HashSet.h, 189行目付近

```cpp
//        return m_impl.template addPassingHashCode<T, T, Adapter>(value, value);
	    return iterator_const_cast(m_impl.template addPassingHashCode<T, T, Adapter>(value, value));
```

src/3rdparty/webkit/Source/WebCore/platform/DefaultLocalizationStrategy.cppの文字コードをUTF-8 w/ BOMに変更

## Qtをビルド

1. Qt/4.8.6/でコマンドプロンプトを開く
1. 以下のコマンドを実行
	* set PATH=C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\bin;%PATH%
	* set PATH=%QT_HOME%\bin;%PATH%
	* set INCLUDE=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include
	* set LIB=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib
	* set QT_HOME=C:Qt\4.8.6
	* configure -mp -opensource -nomake demos -nomake examples -platform win32-msvc2012
	* vcvars32.bat
	* nmake

## 各種ビルド用設定

* TouchIDEditor\_.batを実行してVisual Studioを起動
* 環境変数QT4_DIRにQtのインストールディレクトリのパス（Qt/4.8.6/）を設定
* rapidxmlプロジェクトをアンロード
* DebugDL/FinalCLRでビルド
	* 当初ビルド構成を間違っていたため、いびつな構成になっています
	* 時間があれば修正します

## 実行

実行にはQt, glew, glutのdllが必要になります

* TouchIDEditor_.exeと同じディレクトリにQt/4.8.6/lib内にあるdllをコピー
	* Debugビルドの場合はQtCored4.dll, QtGuid4.dll, QtOpenGLd4.dllをコピー
	* Releaseビルドの場合はQtCore4.dll, QtGui4.dll, QtOpenGL4.dllをコピー

* TouchIDEditor.exeと同じディレクトリにglew32.dllとglut32.dllをコピー
	* 該当のdllはTouchIDEditor_/DLLsに配置しています

## FAQ

* ビルドに成功したが実行時のエラーで停止する
	* グラフィックボードのドライバのバージョン違いにより、シェーダーのコンパイルに失敗している可能性が高い
	* シェーダープログラムに詳しい人に調査・修正を依頼するとよい

* マテリアルウィジェットでのテクスチャ表示がおかしい
	* QImageがABGRフォーマットに対応していないことが原因です

* rapidxmlプロジェクト内でビルドエラーが発生する
	* 該当プロジェクトをアンロードしてビルドしてください
	* ただし作業ログの途中まではrapidxmlプロジェクトを参照していますので、過去のコミットまで遡った際にビルドエラーが発生した場合は適宜ロード/アンロードを切り替えて下さい

* ベトベターなど、表面がアニメーションするポケモンが正常に表示されず、IDテクスチャを塗ることもできない
	* 表面にUVアニメーションするマスクテクスチャが貼られていることが原因です
	* マテリアルウィジェットの設定で、該当マテリアルを「無視」に設定することで体を塗ることができます

* 戻る/やり直しが動かない
	* ツール作成当初より未実装です
	* こまめに保存し、やりなおす場合はツールを再起動してください

* 元のモデルが表示されない
	* いったんIDテクスチャが作成されると、次回以降はIDテクスチャを読み込むよう実装されています

* 元αのバーを動かしても何も起こらない
	* 私も何のためのバーなのか知りません

## ツールの設計について

** ※このマニュアルを書いているプログラマー自身もコードの全容について把握しているわけではないので注意すること **

* Touch ID Editorは元々xy/sango時代に用いていた中間ファイル(.cmdl, .cskla)を読み込むことを前提としたツールです
	* 作成者のスケジュールの関係で、ある程度コードの品質に目を瞑って作成されています

* 同じリソースから読み込んだポケモンを2体（通常モデル、IDモデル）描画し、IDモデルのアルファを調整することで元のモデルに塗り絵をするような感覚でIDテクスチャを作成できるよう実装されています
	* 2体描画したのち、IDモデルのみテクスチャを張り替えたり、元モデルから作成したマテリアル情報をIDモデルのもので上書きしたりしており、移植作業者も挙動について把握しきれていません

* nijiプロジェクトへの移行に際し、社内独自の中間ファイル(.gfmdl, .gfmot)を読み込むよう、改造されています

* 旧gflibとgflib2の共存について
	* typedefの競合を避けるため、gfl_Typedef.hを変更しています
	* gitのコミットログ 785ce4205742a5091f2a7aafdfabde47154c2682
	* 旧gflibはデバッグ用コードの有効/無効がDebug/Releaseで切り替わるのに対し、gflib2はDebug/Develop/Releaseで有効、Finalで無効という構成になっています
	* 上の理由により、Touch ID EditorはDebug/Final構成でのビルドのみサポートしています
	* nijiのgflib2ではNOMINMAXマクロが有効になっているため、gflib内のmin/maxを使用している部分を::std::min/::std::maxを明示的に呼ぶよう変更しています
	* gitのコミットログ 3b8e90d940d40611547a9b7cc0650b0e6ae6abdf

* heapについて
	* nijiのgflib2の機能を利用するため、nijiプロジェクトのheapシステムをベタ移植しています
	* gflibとgflib2のheapシステムが共存しているため、双方からメモリリークの検出等のアサートが発生する場合があります
	* nijiのheapはmain.cppからMainHeap.hppをインクルードしています
	* gitのコミットログ a05abf4b15d3bac48ae810a1abe8af081034c4a2

* ランタイムライブラリについて
	* ランタイムライブラリ (/MD, /MDd)を使用するため、gflib2のプロジェクトは[xxxCLR]の構成を読み込む必要があります
	* /MT, /MTdでもビルドは可能ですが、デバッグが有効になっている場合はheapでアサートが発生します

* モデルデータの変換について
	* .gfmdlファイルを読み込み、リソースをgfl::cmdlクラスの形式に展開しています
	* リソースの変換、cmdlへの展開はGui/source/ResourceConv.cppで行っています

* モーションデータの変換について
	* gfl2の関数を用いて読み込み、モデルに適用しています
	* .csklaはローカル行列を用いているのに対し、.gfmotはワールド行列を用いているため、旧gflib内の関数を変更することで対応しています
	* 該当の変換部分はgfl_Framework/source/gfl\_CmdlModel.cppです
	* 詳細はgitのcommitログ: f26cd7e037e09d4bb5ecafb7a38d8fbb673d75c0 を参照してください

## 注意事項

* このプロジェクトを保守する場合、次の事項に注意すること
	* xy/sango世代のgflibとniji世代のgflib2を無理やり共存させており、各ライブラリ内の正常な挙動は保証されていないこと
	* 不要なコード、使用されていないコードが多く残っていること
	* 一見不要に思えるコードでも、想定外の場所から参照されている場合があること
	* そもそも正常な挙動がどのようなものだったか完全には把握されていないため、不用意なコードの修正は思わぬ不具合を招く可能性があること
	* C++で記述されているが、オブジェクト指向言語の設計にあるべきカプセル化は一切なされていないので、コードがばらけただけのC言語だと思ってとりかかること
	* 作成時から既にメモリは駄々漏れ状態なので、間違ってもメモリリークを解消しようとは思わないこと
	* 各種Getterの返り値はconst修飾されていないか、もしくはconst修飾されていたとしてもキャストによりconstを潰した上で書き換えられているので注意すること
	* QStringはVisual Studio2012以降のデバッガで内容を参照できないため(対応するVSアドインが必要、ただしQt4系のVS2012用アドインは存在しない)、qstring.toAscii().data()を呼び出す必要があること
	* printfおよびgfl::debug::Consoleは動作しないため、コンソール出力によるデバッグは非常に面倒であること
	* コメントとコードとの整合性は保証されていないこと

* 実行・デバッグ時の注意
	* ツールを動作させたとき、特に通知等なく新規にIDテクスチャが作成されることがあり、同じポケモンを読み込んでも初回と2回目以降で挙動が変わる場合があるのでリソース側にも注意を払うこと

* もし将来のプロジェクトで開発環境が再び大きく変わった場合、このツールに関しては保守するよりも新規作成することを強くお勧めします
	* それでももしあなたがこのツールの保守をせざるを得なくなってしまった場合は……検討を祈ります
	* このreadmeはそこそこ充実させおいたので参考にしてください

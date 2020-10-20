#ifdef  GF_PLATFORM_CTR
#ifndef __GFLNET_NETWORKDEFINE_H__
#define __GFLNET_NETWORKDEFINE_H__
//=============================================================================
/**
 * @file	 gflnet_NetworkDefine.h
 * @brief	 通信の定義が列挙してある
 * @author k.ohno
 * @date	 2011.5.24
 */
//=============================================================================
#pragma once

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>
#include <debug/include/gfl2_DebugPrint.h>

/// 通信バージョン
#define GFL_P2P_NETVERSION (1)     // P2P通信 速度は遅いが堅い通信
#define GFL_BEACON_NETVERSION (1)  // ビーコン  180バイトをユーザー管理にしている通信

#define GFLNET_VERSION_MASK  (0x3F)
#define GFLNET_PRODEV_MASK  (0xC0)

#define GFLNET_BEACON_DEVEROPMENT    (0x80)    //開発
#define GFLNET_BEACON_RELEASE        (0x40)    //製品



#define DEBUG_LOCALCOMMUNICATION_ID (101)



///デフォルトのスレッドの値
///DefaultPriorityより低く設定している 
#define GFLNET_DEFAULT_THREAD_PRIORITY  (gfl2::thread::Thread::DefaultPriority+3)


//任天堂ライブラリに定義が無いのでここに定義
//nn::pia::local::UdsNode::GetInstance()->SetApplicationDataToBeaconの関数の宣言
/// ハード側ライブラリ制限によるビーコンのマックスサイズ
#define GFL_BEACON_HARD_SIZEMAX  (184)
//ひろったビーコンにPIA用のヘッダがついているから除外して使用しないといけない
#define GFL_PIAHEADER_SIZE (0)


///< 接続人数最大
///< ゲームごとに異なると思うのですが、メモリ削減の為に定数定義  3vs3可能
///< 調整する場合には再コンパイル
#define GFLNET_P2PCONNECT_MAXNUM (4)


#define  GFLNET_NETBASE_STACK_SIZE ( 4096 )  //スタックチェッカーが必要
#define GFLNET_NETBASE_STACK_TYPE (gfl2::thread::Thread::STACK_TYPE_SAFE)



//---------------------------------------wifip2p

#define MATCHMAKE_MAX_PARTICIPANTS (2)   //マッチメイクの最大数
// マッチメイクモード
#define MATCHMAKE_GAME_MODE 1000


// 試行回数(失敗した場合はこの回数までマッチメイクを試みます)
#define MATCHMAKE_TRY_NUM 10

// フレンドマッチを行うか？
//#define MATCHMAKE_FRIEND_MODE

// 受信メッセージ表示 - ON の場合は受信時メッセージを表示します
#define VIEW_RECEIVE_MESSAGE

// 使用する検索属性のINDEX
#define MATCHMAKE_ATTR_INDEX 0

// 使用する検索属性のMAX値
#define MATCHMAKE_ATTR_MAX 2

// セッションを作成した状態で一人ぼっちのまま待機する時間
#define ORPHAN_WAIT_TIME 20000

// スリープする時間
#define DISPATCH_PERIOD 100

// ディスパッチの待ち時間
#define DISPATCH_TIMEOUT 3

// マッチメイク後、送受信を行う時間
#define MATCHMAKING_TIME 30000

// 送信バッファサイズ
#define TEST_SEND_DATA_SIZE 1288

//---------------------------------------wifip2p





///< パケットの最大サイズ  あまり大きいのは全体のメモリを圧迫します
///< ここを変える場合 gflnet_Baseも変更する必要があります
#define GFLNET_P2PCONNECT_MAXPACKET (2048)

//UDS_PACKET_PAYLOAD_MAX_SIZE   = 1478なので 1024は妥当


#define UDS_DEBUG_CHANNEL (11)  // デバッグ時のみ


// ****** アプリケーション開発者による変更が想定されるマクロ *******************
#define FRAME_PER_SECOND (30)

//n1589:NEX処理を行う周期
#define PROCESS_NEX_PERIOD  (1)
//#define PROCESS_NEX_PERIOD 30

#define SAFE_USER_THREAD_DISPATCH_UPDATE_FREQENCY FRAME_PER_SECOND


#define UDS_MAX_ENTRY (GFLNET_P2PCONNECT_MAXNUM)
//#define UDS_TITLE_ID  (0xF9301)

// "GFLNET_QoCXqIpL";   // パスフレーズ。前半はGF固有 後半は乱数
#define UDS_NETWORK_PASSPHRASE "GNgBwMcVotYs"
#define PASSPHRASE  "GNgBwMcVotYs"


#define SAMPLE_MAX_SILENCE_TIME (10 * 1000)
#define SAMPLE_KEEP_ALIVE_TIMEOUT (2 * 1000)
#define SAMPLE_MAX_WINDOW_MAP_SIZE 128
#define NETWORK_TOPOLOGY nn::nex::NetworkTopology::Peer2Peer

//n1589:インターネット通信モードで1台のAPに多数台接続する場合などにメッセージバンドルを行うとパケット数が減り接続台数を増やせます
#define ENABLE_MESSAGE_BUNDLING 1
//#define ENABLE_MESSAGE_BUNDLING 0
#define MESSAGE_BUNDLING_FLUSH_DELAY 75
#define MESSAGE_BUNDLING_FLUSH_CHECK_PERIOD 50

#define ENABLE_PACKET_BUNDLING 0
#define PACKET_BUNDLING_FLUSH_DELAY 30
//n1589:パケットバンドルのフラッシュ時間チェックはトランスポート送信処理（DeliverOutgoing）が呼ばれる度に自動的に行われます



/// ネットワークのログON=1
#if defined(DEBUG_ONLY_FOR_ohno_katsumi) || defined(DEBUG_ONLY_FOR_ichiraku_katsuhiko)
#define NETWORK_SIZE_LOG (0)
#define NETWORK_LOG (0)
#define NETWORK_RET_LOG (0)
#define NETWORK_TIMING_LOG (1)
#else
#define NETWORK_SIZE_LOG (0)
#define NETWORK_LOG (0)
#define NETWORK_RET_LOG (0)
#define NETWORK_TIMING_LOG (1)
#endif

#if NETWORK_LOG

#define ___FILENAME___ (strrchr(__FILE__,'\\' ) + 1 )
#define NET_PRINT(...)  NN_LOG("%s:(%d)|",___FILENAME___,__LINE__); gfl2::debug::UserPrint(__VA_ARGS__);

#else

#define NET_PRINT(...)  ((void)0)

#endif

#if NETWORK_RET_LOG
#define NET_RETCOM(...)  gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define NET_RETCOM(...)  ((void)0)
#endif

#if NETWORK_SIZE_LOG
#define NET_SIZELOG(...)  gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define NET_SIZELOG(...)  ((void)0)
#endif

#if NETWORK_TIMING_LOG
#define NET_TIMING(...)  gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define NET_TIMING(...)  ((void)0)
#endif

/*



○StationEvent::Participated が届いていないステーションからの通信
こちらについては破棄していただく方が安全とのことです。
（過去のやり取りで、すでにそのように実装いただいていると思います。）

○StationEvent::Noneについて
StationEvent::None のコールバックについては無視し、
何も処理を行わないようにしていただければと思います。

子機側のStation::StateがJoiningSessionやLeavingのような、参加途中、
離脱途中状態でも親機にコールバックが起こる場合があり、その場合に
StationEvent::None になります。
通常、その直前か直後にParticipated / Leaved どちらかのコールバック
が発生するはずですので、こちらでハンドリングを行っていただければと
思います。

>>
>>> >１．接続の状態はParticipatedがきた時に接続したという判定で大丈夫でしょう
>>> >か？
>>> はい、RegisterStationEventCallback で登録したコールバック中で
>>> 第2引数が StationEvent::Participated であれば、
>>> 新たにステーションが接続されたことを示しております。
>>
>>
>>> ただ、これは通信の成功・不成功ではなくアプリの仕様次第となりますが、
>>> 新たなステーションが追加されたことをトリガにアプリ側で何らかの処理を
>>> 行われる場合は、StationEvent::Participated が届いていないステーションからの
>>> データは破棄していただくのが安全かと思います。
>>
>>> 
>>> >１．接続の状態はParticipatedがきた時に接続したという判定で大丈夫でしょう
>>> >か？
>>> はい、RegisterStationEventCallback で登録したコールバック中で
>>> 第2引数が StationEvent::Participated であれば、
>>> 新たにステーションが接続されたことを示しております。
>>> 
>>> >２．Participatedの状態が子機に最初に来て、その直後に親機にむけて送信を開
>>> >始
>>> >し
>>> >ております。
>>> こちらですが、「Participatedの状態が子機に最初に来て」というのは、
>>> 新規接続の子機の nn::nex::Station::State が Participating になったとき、
>>> という理解でよろしいでしょうか。
>>> 
>>> そう仮定してサンプルで実験を行ったところ、私の手元でも、
>>> 親機側に StationEvent::Participated が届く前に、
>>> 受信コールバックが呼ばれるタイミングがあることを確認しました。
>>> 
>>> おそらく、通信自体はできており正常なデータが受信されていると思われますが、
>>> 念のため推奨される対応を担当部署に確認いたします。今しばらくお待ち下さい。
>>> 
>>> ただ、これは通信の成功・不成功ではなくアプリの仕様次第となりますが、
>>> 新たなステーションが追加されたことをトリガにアプリ側で何らかの処理を
>>> 行われる場合は、StationEvent::Participated が届いていないステーションから
>>> の
>>> データは破棄していただくのが安全かと思います。
>>> 
>>> 
>>> >
>>> >> ConnectionID の実体は nn::nex::DOHandleそのものです。
>>> >> 
>>> >> ですので、
>>> >> 　nn::nex::DOHandle::GetValue()
>>> >> で得られる qUnsignedInt32 が状態変化を起こしたStationの
>>> >> ConnectionIDとなります。
>>> >
>>> >ありがとうございました。ConnectionIDを使用して
>>> >通信を構築する事ができました。
>>> >
>>> >GameStationEventCallbackについてなのですが、
>>> >もう少し挙動について詳しくお聞かせください。
>>> >
>>> >
>>> >最初にGameStationEventCallbackは
>>> >StationEvent::Noneの状態で呼ばれています。
>>> >しばらくすると StationEvent::Participatedの状態が呼ばれます。
>>> >
>>> >１．接続の状態はParticipatedがきた時に接続したという判定で大丈夫でしょう
>>> >か？
>>> >２．Participatedの状態が子機に最初に来て、その直後に親機にむけて送信を開
>>> >始
>>> >し
>>> >ております。
>>> >    親機の通信受信が親機のParticipatedより先に到着してしまうのですが、
>>> >    そのコマンドは信用して良いのでしょうか？それとも破棄すべきなのでしょ
>>> > う
>>> >か？
>>> >    それとも子機の最初の送信を遅らせるべきでしょうか？
>>> >
>>> >以上になります。よろしくお願いいたします。
>>> >

>>> >> >nn::nex::ConnectionIDの事がわかりました。
>>> >> >こちらを利用してプログラムを組もうと思います。
>>> >> >
>>> >> >もう一つだけ質問させて下さい。
>>> >> >RegisterStationEventCallbackのコールバック関数
>>> >> >GameStationEventCallbackの引数に、
>>> >> >nn::nex::DOHandleがありますが、
>>> >> >ここからnn::nex::ConnectionIDが分かる手段はあるのでしょうか？
>>> >> >
>>> >> >> 
>>> >> >> はい、ご推察の通りこのコールバックに渡される第1引数は
>>> >> >> どのCTRから届いた物かを特定できるIDとなります。
>>> >> >> このIDですが、Sessionを作成、または参加した際にNEXが
>>> >> >> 自動で各CTR（Station）に割り当てるものとなります。
>>> >> >> 
>>> >> >> また自分のConnectionIDに関しましては、Session参加中に
>>> >> >> 　nn::nex::Station::GetLocalInstance()->GetConnectionID();
>>> >> >> で取得可能です。
>>> >> >> 
>>> >> >> 

 */

#endif //__GFLNET_NETWORKDEFINE_H__
#endif

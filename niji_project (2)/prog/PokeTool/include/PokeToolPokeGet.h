#if !defined(POKETOOLPOKEGET_H_INCLUDED)
#define POKETOOLPOKEGET_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    PokeToolPokeGet.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.07
 @brief   ポケモンをゲットしたときの処理
 */
//==============================================================================


// [図鑑登録アプリ]を構成する要素
//
// (1) 図鑑詳細画面   ・・・ 初めてGETしたポケモンか？→<初めて>[図鑑アプリ]
// (2) 名前入力画面   ・・・ 「名前を付けるか？はい／いいえ」→<はい>[名前入力アプリ]
// (3) 送り先選択画面 ・・・ 「どちらに送るか？手持ち／ボックス」→<手持ち>手持ちに空いているところがないか？→<ない>[入れ替える手持ちポケモンを選択するアプリ(ポケモンリストアプリ)]
// (4) 配置           ・・・ ポケモンを配置する。(ポケモンを手持ちかボックスに入れる。最初から入っているときは"配置"を行わなくてよい)
// (5) フラグ処理     ・・・ SEEフラグ、GETフラグを設定する。
//
// 上記のどれかを行いたいときは[図鑑登録アプリ]を呼べばいい。
// 上記の処理を画面遷移をすることなく済ませられるかどうかは[ポケモンゲットユーティリティのUpdatePokeAtPokeGet]を呼べば判定でき、済ませられる場合は済ましてくれる。
// "配置&フラグ処理"を行わない呼び出しもあるので、"配置&フラグ処理"だけを行いたいときは[ポケモンゲットユーティリティのPutPoke]を呼べばよい。←【使用制限！】あります。




// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
class ZukanFrameCell;  // friendに書くために前方宣言しておく。
class ZukanImpl;
class ZukanViewRegister;
class ZukanFramePokeNameCell;
class ZukanFramePokePutCell;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(PokeTool)




//! @brief  ポケモンゲットユーティリティ
class PokeGetUtil
{
  friend class app::zukan::ZukanFrameCell;
  friend class app::zukan::ZukanImpl;
  friend class app::zukan::ZukanViewRegister;
  friend class app::zukan::ZukanFramePokeNameCell;
  friend class app::zukan::ZukanFramePokePutCell;


public:

  //! @brief  呼び出す者
  class CallerType
  {
  public:
    enum Tag
    {                                                            // PokeMemo | 図鑑詳細画面 | 名前入力画面 | 送り先選択画面 | 配置 | フラグ処理 | 備考
      HOKAKU,               //!< 捕獲                            //    ○    |      ○      |      ○      |       ○       |  ○  |     ○     |
      HOKAKU_RESORT,        //!< リゾートで捕獲                  //    ○    |      ○      |      ○      |       ○       |  ○  |     ○     |
      KOUKAN,               //!< 交換(通信なし)                  //    ○    |      ○      |      ×      |       ×       |  ×  |     ○     | フィールドNPCと行う交換はこちら(リアル通信をしているわけではないから)。
      KOUKAN_NET,           //!< 通信交換                        //    ○    |      ○      |      ×      |       ×       |  ×  |     ×     | "配置&フラグ処理"だけ行いたいときはPokeGetUtil::PutPokeを呼んで下さい。
      SHINKA,               //!< 進化                            //    ○    |      ○      |      ×      |       ×       |  ×  |     ○     |
      SHINKA_NET,           //!< 通信進化                        //    ○    |      ○      |      ×      |       ×       |  ×  |     ×     | "配置&フラグ処理"だけ行いたいときはPokeGetUtil::PutPokeを呼んで下さい。
      TAMAGO_FUKA,          //!< タマゴ孵化                      //    ○    |      ○      |      ○      |       ×       |  ×  |     ○     | タマゴではなく孵化済みのポケモンを渡して下さい。
      TAMAGO_FUKA_RESORT,   //!< リゾートでタマゴ孵化            //    ○    |      ○      |      ○      |       ×       |  ×  |     ○     | タマゴではなく孵化済みのポケモンを渡して下さい。
      MORAU,                //!< もらう                          //    ○    |      ○      |      ×      |       ○       |  ○  |     ○     |
      MORAU_TAMAGO,         //!< タマゴをもらう                  //    ○    |      ×      |      ×      |       ○       |  ○  |     ○     | タマゴを渡して下さい。
      MORAU_NANASHI,        //!< 名無しをもらう                  //    ○    |      ○      |      ○      |       ○       |  ○  |     ○     |
      MORAU_NANASHI_PARTY,  //!< 手持ちに必ず置く名無しをもらう  //    ○    |      ○      |      ○      |       ×       |  ○  |     ○     | 手持ちに必ず空いているところがなければならない。「三匹選択」はここ。
      HAIFU,                //!< 配布ポケモンをもらう            //    ○    |      ○      |      ×      |       ○       |  ○  |     ○     |
      HAIFU_NANASHI,        //!< 名無しの配布ポケモンをもらう    //    ○    |      ○      |      ○      |       ○       |  ○  |     ○     |
      HAIFU_TAMAGO,         //!< 配布タマゴをもらう              //    ○    |      ×      |      ×      |       ○       |  ○  |     ○     | タマゴを渡して下さい。 
      KOUKAN_TAMAGO,        //!< タマゴ交換(通信なし)            //    ○    |      ×      |      ×      |       ×       |  ×  |     ○     | タマゴを渡して下さい。フィールドNPCと行う交換はこちら(リアル通信をしているわけではないから)。
      KOUKAN_TAMAGO_NET,    //!< タマゴ通信交換                  //    ○    |      ×      |      ×      |       ×       |  ×  |     ×     | タマゴを渡して下さい。"配置&フラグ処理"だけ行いたいときはPokeGetUtil::PutPokeを呼んで下さい。
    };
  };


  //! @brief  配置区分
  class PutPlace
  {
  public:
    enum Tag
    {
      NONE,    //!< 「なし」とか「配置済みなので使わないとか」
      PARTY,   //!< 手持ち
      BOX,     //!< ボックス
    };
  };


  //! @brief  配置場所
  class PutSpace
  {
  public:
    enum Tag
    { 
      NONE,  //!< 「なし」とか「配置済みなので使わないとか」
      FIX,   //!< 固定
      FREE,  //!< 空いているところ
    };
  };


  //! @brief  追加データ
  struct ExDataForDummy
  {
    u32 dummy[4];
  };
  struct ExDataForHaifu  // HAIFU_と付くもので使う
  {
    u32 placeNo;  // 場所（ふしぎな贈り物等に入っている場所）
    u32 year;     // 年（ふしぎな贈り物等に入っている年）
    u32 month;    // 月（ふしぎな贈り物等に入っている月）
    u32 day;      // 日（ふしぎな贈り物等に入っている日）
  };
  struct ExDataForMorauTamago  // MORAU_TAMAGOで使う(育て屋、ゲーム内イベント)
  {
    u32 placeNo;  // 捕まえた場所の地名メッセージ番号 ← このような説明が書かれていたが、正しくは人名(育て屋さん、○○さん)を渡す。
  };
  union ExDataUnion
  {
    ExDataForDummy       exDataForDummy;
    ExDataForHaifu       exDataForHaifu;
    ExDataForMorauTamago exDataForMorauTamago;
  };
  class ExData
  {
  public:
    ExDataUnion exDataUnion;
  public: 
    ExData(void)
    {
      gfl2::std::MemClear( exDataUnion.exDataForDummy.dummy, sizeof(exDataUnion.exDataForDummy.dummy) );
    }
  };


  //! @brief  戻り値タイプ
  class ReturnType
  {
  public:
    enum Tag
    {
      CALL_ZUKAN_REGISTER,  //!< 図鑑登録アプリを呼ぶこと。
                            //   PokeMemo設定済み。

      FINISHED,             //!< 図鑑登録アプリを呼ぶ必要はない。
                            //   PokeMemo設定済み。ポケモン配置済み。SEEフラグ、GETフラグ設定済み。
    };
  };


  //! @brief  戻り値パラメータ
  class ReturnParam
  {
  public:
    bool                              isValid;   //!< ReturnParamに有効な値が入っているときtrue。
                                                 //   "配置&フラグ処理"を行ったらtrue、"配置"を行わず"フラグ処理"だけ行ったらfalse、"配置&フラグ処理"どちらも行わなかったらfalse。
    
    PutPlace::Tag                     putPlace;  //!< 配置区分。

    u32                               pos0;      //!< 配置位置が入っている。
                                                 //   「putPlaceがPARTYのとき」はパーティ内インデックス、「putPlaceがBOXのとき」はトレーインデックス。
    u32                               pos1;      //!< 配置位置の追加情報が入っている。
                                                 //!  「putPlaceがBOXのとき」だけ使い、トレー内のインデックスが入っている。
  
  public:
    ReturnParam(void)
      : isValid(false),
        putPlace(PutPlace::NONE),
        pos0(0),
        pos1(0)
    {}
  };


public:
  //! @brief  ポケモンゲット時にポケモンに関して更新を行う。
  //          以下のことが行われる。
  //          ・トレーナーメモ(PokeMemo)をポケモンに設定する。
  //          ・図鑑登録アプリを呼び出す必要があるかチェックする。
  //          ・図鑑登録アプリを呼び出す必要がない場合はポケモンの"配置&フラグ処理"を行う。(callerTypeによっては"配置&フラグ処理"の"フラグ処理"だけ行ったりどちらも行わなかったりする)
  //
  //! @param[in]  heap         ヒープ。通常のヒープメモリでもデバイスメモリでもどちらでもよい。本関数内で一時的に使用するだけ。この関数内で後方確保で使っています。
  //! @param[in]  pp           対象のポケモン。
  //! @param[in]  callerType   呼び出す者。
  //
  //! @param[out] returnParam  戻り値パラメータ。実際に配置したところを入れて返す。NULLでもいい。
  //
  //! @param[in]  exData       追加データ。以下のcallerTypeのときは必須。それ以外のときはNULLで。
  //                               HAIFU          exDataForHaifuに設定する。
  //                               HAIFU_NANASHI  exDataForHaifuに設定する。
  //                               HAIFU_TAMAGO   exDataForHaifuに設定する。
  //                               MORAU_TAMAGO   exDataForMorauTamagoに設定する。
  //
  //! @return  戻り値タイプ。
  static ReturnType::Tag UpdatePokeAtPokeGet(
      gfl2::heap::HeapBase*         heap,
      pml::pokepara::PokemonParam*  pp,  // PokeMemoの都合でconstを付けられない
      CallerType::Tag               callerType,
      ReturnParam*                  returnParam = NULL,
      ExData*                       exData = NULL
  );

  //! @brief  ポケモンの"配置&フラグ処理"を行う。(必ず"配置&フラグ処理"の両方とも行う)
  //
  //! @param[in]  heap         ヒープ。通常のヒープメモリでもデバイスメモリでもどちらでもよい。本関数内で一時的に使用するだけ。この関数内で後方確保で使っています。
  //! @param[in]  pp           対象のポケモン。
  //! @param[in]  putPlace     配置区分。NONE以外。
  //! @param[in]  putSpace     配置場所。NONE以外。
  //! @param[in]  pos0         配置位置を指定する。「putSpaceがFIXのとき」だけ使う。「putPlaceがPARTYのとき」はパーティ内インデックス、「putPlaceがBOXのとき」はトレーインデックス。
  //! @param[in]  pos1         配置位置の追加情報。「putSpaceがFIXのとき」だけ使う。「putPlaceがBOXのとき」だけ使う。トレー内のインデックスを指定する。
  //
  //! @param[out] returnParam  戻り値パラメータ。実際に配置したところを入れて返す。
  // 
  //! @attention  【使用制限！】「callerTypeがKOUKAN_NETかSHINKA_NETかKOUKAN_TAMAGO_NETのとき」以外で使用する場合は、必ず@autherまで連絡して下さい。  todoとして残しておいて使用箇所をチェックするのを忘れないようにしたい。
  static void PutPoke(
      gfl2::heap::HeapBase*         heap,
      pml::pokepara::PokemonParam*  pp,  // そのうちconstを付けられない事情が出てくるかもしれないので、constを付けておかない。
      PutPlace::Tag                 putPlace,
      PutSpace::Tag                 putSpace,
      u32                           pos0,
      u32                           pos1,
      ReturnParam*                  returnParam
  );


  // 以下のメンバはfriendと本クラスからしかアクセスしない
private:
  //! @brief  図鑑登録アプリを呼び出す必要があるかチェックする。
  //          チェックするだけ。何も書き換えない。
  //! @return  呼び出す必要がある場合はtrueを返す。
  //           呼び出す必要がない場合はfalseを返す。このときは図鑑登録アプリを呼び出さないので、UpdatePokeAtPokeGetやPutPokeで、ポケモンの"配置&フラグ処理"を行って下さい。
  static bool Check(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );

  // 図鑑詳細画面へ行くか
  static bool DoesGoToInfoShow(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );
  // 名前入力画面へ行くか
  static bool DoesGoToPokeNameInput(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );
  // 送り先選択画面へ行くか
  static bool DoesGoToPokePutSelect(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );
  // 配置するか(フラグ処理は関係ない)
  static bool DoesPut_NotSetFlag(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );
  //! @brief  配置するときに、配置区分、配置場所を決められるか？
  //! @retval  true   DoesPut_NotSetFlagがtrue(配置するときだ！)　かつ　配置区分、配置場所を決められた！　このときputPlace, putSpaceには有効な値を入れて返す。
  //! @retval  false  DoesPut_NotSetFlagがfalse(配置しないときだ！)　または　DoesPut_NotSetFlagはtrueだが配置区分、配置場所を決められなかった！　このときputPlace, putSpaceには無効な値を入れて返す。
  static bool DoesDecidePlaceSpaceIfPut(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType,
      PutPlace::Tag*                      putPlace,    // [out]
      PutSpace::Tag*                      putSpace     // [out]
  );
  // フラグ処理するか(配置は関係ない)
  static bool DoesSetFlag(
      const pml::pokepara::PokemonParam*  pp,
      CallerType::Tag                     callerType
  );

  // 配置だけ行う(フラグ処理は行わない)
  //! @param[in]  heap         ヒープ。通常のヒープメモリでもデバイスメモリでもどちらでもよい。本関数内で一時的に使用するだけ。この関数内で後方確保で使っています。
  static void Put_NotSetFlag(
      gfl2::heap::HeapBase*               heap,
      const pml::pokepara::PokemonParam*  pp,
      PutPlace::Tag                       putPlace,
      PutSpace::Tag                       putSpace,
      u32                                 pos0,
      u32                                 pos1,
      ReturnParam*                        returnParam
  );

  // フラグ処理だけ行う(配置は行わない)
  static void SetFlag(const pml::pokepara::PokemonParam*  pp);


  // 以下のメンバは本クラスからしかアクセスしない
private:
  static void setPutPlacePartyInReturnParam(ReturnParam* returnParam, u32 pos0);
  static void setPutPlaceBoxInReturnParam(ReturnParam* returnParam, u32 pos0, u32 pos1);

  // トレーナーメモ(PokeMemo)を設定する
  //! @param[in]  heap         ヒープ。通常のヒープメモリでもデバイスメモリでもどちらでもよい。本関数内で一時的に使用するだけ。この関数内で後方確保で使っています。
  //! @param[in]  pp           対象のポケモン。
  //! @param[in]  callerType   呼び出す者。
  //! @param[in]  exData       追加データ。
  static void setPokeMemo(
      gfl2::heap::HeapBase*         heap,
      pml::pokepara::PokemonParam*  pp,
      CallerType::Tag               callerType,
      ExData*                       exData
  );

};




GFL_NAMESPACE_END(PokeTool)


#endif  // POKETOOLPOKEGET_H_INCLUDED




/*



図鑑登録　全パターンを列挙しました。



画面を機能ごとに分けると、以下のようになります。


[1] 登録されます（０）
[2] 登録演出＋情報画面（１～５）
[3] 名前入力QA（６）
    名前入力画面
[4] 転送先QA（７～８）
    選んで下さいメッセージ＋ポケモンリスト画面
[5] 道具預かりQA（９～１０）
    預かり結果メッセージ
[6] 転送結果メッセージ（１１）


（　）内は仕様書「niji_ポケモン図鑑_仕様書.xlsx」に書かれている番号。



これらの画面が各パターンでどうなるか
以下に全列挙してあります。








■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■■■■　　アローラポケモン初ゲット　　■■■■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■①a■名前入力あり手持ち固定（タマゴ孵化、手持ち６匹未満のときのゲット（名無しをもらう、捕獲））

[1] 登録されます

[2] 登録演出＋情報画面

[3] 名前入力QA
    名前入力画面

[6] 転送結果メッセージ＜※タマゴ孵化のときはなし＞
　　→「フシギバナを手持ちに加えました」


■①b■名前入力なし手持ち固定（進化、交換、手持ち６匹未満のときのゲット（名有りをもらう））

[1] 登録されます

[2] 登録演出＋情報画面

[6] 転送結果メッセージ＜※進化、交換のときはなし＞
　　→「フシギバナを手持ちに加えました」






■②a■名前入力あり手持ち６匹のときのゲット（名無しをもらう、捕獲）でボックス固定

[1] 登録されます

[2] 登録演出＋情報画面

[3] 名前入力QA
    名前入力画面

[6] 転送結果メッセージ
　　→「フシギバナをボックスに転送しました」

※[5]がないので新規ポケモンが持っていた道具を預かることはしません。


■②b■名前入力なし手持ち６匹のときのゲット（名有りをもらう）でボックス固定

[1] 登録されます

[2] 登録演出＋情報画面

[6] 転送結果メッセージ
　　→「フシギバナをボックスに転送しました」

※[5]がないので新規ポケモンが持っていた道具を預かることはしません。






■③a■名前入力あり手持ち６匹のときのゲット（名無しをもらう、捕獲）で転送先選択

[1] 登録されます

[2] 登録演出＋情報画面

[3] 名前入力QA
    名前入力画面

[4] 転送先QA
    選んで下さいメッセージ＋ポケモンリスト画面

[5] 道具預かりQA＜道具を持っていないときはなし＞
    預かり結果メッセージ
　　→道具を預かれないとき「[4] 転送先QA」へ戻る。

[6] 転送結果メッセージ
　　→転送先に応じて３パターンのメッセージがある。


■③b■名前入力なし手持ち６匹のときのゲット（名有りをもらう）で転送先選択

[1] 登録されます

[2] 登録演出＋情報画面

[4] 転送先QA
    選んで下さいメッセージ＋ポケモンリスト画面

[5] 道具預かりQA＜道具を持っていないときはなし＞
    預かり結果メッセージ
　　→道具を預かれないとき「[4] 転送先QA」へ戻る。

[6] 転送結果メッセージ
　　→転送先に応じて３パターンのメッセージがある。








■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
■■■■　　アローラポケモン２回目以降のゲット　or　アローラ外ポケモンゲット　　■■■■
■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

■①c■名前入力あり手持ち固定（タマゴ孵化、手持ち６匹未満のときのゲット（名無しをもらう、捕獲））

[3] 名前入力QA
    名前入力画面

[6] 転送結果メッセージ＜※タマゴ孵化のときはなし＞
　　→「フシギバナを手持ちに加えました」

※[1]がないのでいきなり「[3] 名前入力QA」から始まります。


■①d■名前入力なし手持ち固定（進化、交換、手持ち６匹未満のときのゲット（名有りをもらう））

※呼ばれない。


もしも間違って呼ばれたら・・・
何もせず抜ける。






■②c■名前入力あり手持ち６匹のときのゲット（名無しをもらう、捕獲）でボックス固定

[3] 名前入力QA
    名前入力画面

[6] 転送結果メッセージ
　　→「フシギバナをボックスに転送しました」

※[1]がないのでいきなり「[3] 名前入力QA」から始まります。
※[5]がないので新規ポケモンが持っていた道具を預かることはしません。


■②d■名前入力なし手持ち６匹のときのゲット（名有りをもらう）でボックス固定

※呼ばれない。


もしも間違って呼ばれたら・・・
何もせず抜ける。






■③c■名前入力あり手持ち６匹のときのゲット（名無しをもらう、捕獲）で転送先選択

[3] 名前入力QA
    名前入力画面

[4] 転送先QA
    選んで下さいメッセージ＋ポケモンリスト画面

[5] 道具預かりQA＜道具を持っていないときはなし＞
    預かり結果メッセージ
　　→道具を預かれないとき「[4] 転送先QA」へ戻る。

[6] 転送結果メッセージ
　　→転送先に応じて３パターンのメッセージがある。

※[1]がないのでいきなり「[3] 名前入力QA」から始まります。


■③d■名前入力なし手持ち６匹のときのゲット（名有りをもらう）で転送先選択

[4] 転送先QA
    選んで下さいメッセージ＋ポケモンリスト画面

[5] 道具預かりQA＜道具を持っていないときはなし＞
    預かり結果メッセージ
　　→道具を預かれないとき「[4] 転送先QA」へ戻る。

[6] 転送結果メッセージ
　　→転送先に応じて３パターンのメッセージがある。

※いきなり「[4] 転送先QA」から始まります。








以上で全パターンになります。




*/


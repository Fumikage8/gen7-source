//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		sv_zukan.h
 *	@brief  図鑑セーブデータ
 *	@author	Toru=Nagihashi
 *	@date		2012.05.17
 *	@author	kawada_koji
 *	@date		2015.08.12
 *	
 *
 *	kujiraからの要素追加について
 *
 *	１．地方エリアについて
 *    kujiraは地方図鑑が３つあります。
 *	
 *	２．捕獲状態について
 *	  kujiraから、ポケモンの捕獲状態には２種類あります。
 *	  ・kujira(カロス地方)で実際に捕まえた
 *	  ・過去作から持ってきて図鑑に登録された
 *	  
 *	  kujiraでは場所によって上記状態によって変わる処理があります。
 *	  例）
 *	    捕獲状態を表すアイコンを変える
 *	    賞状が捕獲状態で書き換わる等…。。
 *
 *
 *	  もし捕獲状態を使ってなにかする場合は上記を気にしてください。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __SV_ZUKAN_H__
#define __SV_ZUKAN_H__
#pragma once

#include <pml/include/pmlib.h>
#include <Savedata/include/SaveDataInterface.h>

#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeToolExtendData.h>

#include <niji_conv_header/poke_lib/monsno_def.h>


GFL_NAMESPACE_BEGIN(Savedata)
class ZukanData : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanData);
public:
  //=============================================================================
  //@brief 定数
  //=============================================================================
  /**
   *  @brief 図鑑モード
   */
  enum Mode
  {
    MODE_ZENKOKU, ///<全国
    MODE_LOCAL_A, ///<地方  アローラ全域エリア
    MODE_LOCAL_B, ///<地方  島１エリア
    MODE_LOCAL_C, ///<地方  島２エリア
    MODE_LOCAL_D, ///<地方  島３エリア
    MODE_LOCAL_E, ///<地方  島４エリア
    MODE_MAX
  };
  // 全国図鑑最大
  static const int ZENKOKU_MONSMAX   = MONSNO_END;

  static const int TEXTVER_VER_MAX = 9;				///< 言語種類数		日/英/仏/伊/独/西/韓/台/中
  
  // 図鑑OSのバージョン
  static const u32 ZUKAN_OS_VERSION_00 = 0;  // 0=ロトムと話したことがない;
  static const u32 ZUKAN_OS_VERSION_01 = 1;  // 1=ロトムと話したことがある;

private:
  //下記定数ですが、.cpp内で定義されているので注意
  // 全国図鑑完成に必要なポケモンの数
  static const int ZENKOKU_COMP_NOPOKE_NUM;
  static const int ZENKOKU_COMP_NUM;

public:
  //=============================================================================
  //@brief コンストラクタ・デストラクタ
  //=============================================================================
  /**
   * @brief コンストラクタ
   */
  ZukanData();
  /**
   * @brief デストラクタ
   */
  virtual ~ZukanData();
public:
  //=============================================================================
  //@brief 取得・操作
  //=============================================================================
 
  ////////////////////////////////
  //
  // オープン状態
  //
  ////////////////////////////////
  
  /**
   * @brief		全国図鑑オープンフラグをセット
   * @param		is_flag 全国モードかどうか
   * @return	none
   *
   * @note  nijiにおいては使っていない。
   *        nijiにおいて各図鑑のオープンにはSYS_FLAG_ZUKAN_ISLAND番号_OPENを使っている。
   */
  void SetZenkokuZukanFlag( bool is_flag );

  /**
   * @brief		全国図鑑オープン状態どうか？
   * @retval	"true = 全国図鑑可"
   * @retval	"false = それ以外"
   *
   * @note  nijiにおいては使っていない。
   *        nijiにおいて各図鑑のオープンにはSYS_FLAG_ZUKAN_ISLAND番号_OPENを使っている。
   */
  bool GetZenkokuZukanFlag( void ) const;
  
  /**
   * @brief 各モードのオープン状態を設定
   *
   * @param mode    モード
   * @param is_flag trueでオープン  falseでオープンしていない
   * 
   * @note modeにMODE_ZENKOKUをいれた場合は　SetZenkokuZukanFlag　と同じ
   *
   * @note  nijiにおいては使っていない。
   *        nijiにおいて各図鑑のオープンにはSYS_FLAG_ZUKAN_ISLAND番号_OPENを使っている。
   */
  void SetZukanOpenFlag( Mode mode, bool is_flag );
  
  /**
   * @brief 各モードのオープン状態を取得
   *
   * @param mode  モード
   *
   * @return trueでオープン falseオープンしていない
   * 
   * @note modeにMODE_ZENKOKUをいれた場合は　GetZenkokuZukanFlag　と同じ
   *
   * @note  nijiにおいては使っていない。
   *        nijiにおいて各図鑑のオープンにはSYS_FLAG_ZUKAN_ISLAND番号_OPENを使っている。
   */
  bool GetZukanOpenFlag( Mode mode ) const;

  /**
   * @brief 図鑑OSのバージョンを設定
   *
   * @param バージョン
   *
   * @note  nijiにおいてロトム起動フラグはSYS_FLAG_BOUKEN_NAVI_GETを使っている。
   *        nijiにおいてロトムと話したことがあるかないかにこれを使っている。
   */
  void SetZukanOSVersion(u32 ver);

  /**
   * @brief 図鑑OSのバージョンを取得
   *
   * @return バージョン
   *
   * @note  nijiにおいてロトム起動フラグはSYS_FLAG_BOUKEN_NAVI_GETを使っている。
   *        nijiにおいてロトムと話したことがあるかないかにこれを使っている。
   */
  u32 GetZukanOSVersion(void) const;


  ////////////////////////////////
  //
  // モード
  //
  ////////////////////////////////

  /**
   * @brief		閲覧中の図鑑モードを設定
   * @param		mode	図鑑モード 
   */
  void SetZukanMode( Mode mode );
  
  /**
   * @brief		閲覧中の図鑑モードを取得
   * @retval	図鑑モード
   */
  Mode GetZukanMode( void ) const;

  /**
   * @brief 現在のモードに沿った地方エリア番号を返す
   *
   * @return  地方エリア番号
   */
  PokeTool::ExtendData::LocalArea GetLocalAreaMode( void ) const;
 

  ////////////////////////////////
  //
  // 閲覧中
  //
  ////////////////////////////////

  /**
   * @brief		閲覧中のポケモン番号を設定
   * @param		mons	ポケモン番号
   */
  void SetDefaultMons( MonsNo mons );
  
  /**
   * @brief		閲覧中のポケモン番号を取得
   * @return	ポケモン番号
   */
  MonsNo GetDefaultMons( void ) const;
  
  /**
   * @brief		閲覧中データ設定
   * @param		mons		ポケモン番号
   * @param		sex			性別
   * @param		rare		レアフラグ true = RARE
   * @param		form		フォルム
   */
  void SetDrawData( MonsNo mons, pml::Sex sex, bool rare, FormNo form );
 
  /**
   * @brief		表示するフォルムデータを取得
   * @param		mons		ポケモン番号
   * @param		sex			性別
   * @param		rare		レアフラグ true = RARE
   * @param		form		フォルム
   */
  void GetDrawData( MonsNo mons, pml::Sex * sex, bool * rare, FormNo * form ) const;


  ////////////////////////////////
  //
  // フォルム
  //
  ////////////////////////////////

  /**
   * @brief		最大フォルム数取得
   * @param		mons		ポケモン番号
   * @return	最大フォルム数
   */
  u32	GetFormMax( MonsNo mons ) const;
  
  /**
   * @brief		メガフォルムもちを取得
   * @param		mons		ポケモン番号
   * @return	trueならばメガフォルム持ち
   */
  bool	IsMega( MonsNo mons ) const;  


  ////////////////////////////////
  //
  // 捕まえたフラグ
  //
  ////////////////////////////////

  /**
   * @brief		捕獲データセット
   * @param		pp		捕獲ポケモンデータ
   */
  void SetPokeGet( const pml::pokepara::CoreParam & param );
  void SetPokeGet( const PokeTool::SimpleParam & param, u32 lang );  // langはpml::pokepara::CoreParamからGetLangId()で取得できる値
  
  /**
   * @brief		捕獲チェック
   * @param		monsno	ポケモン番号
   * @retval	"true = 捕獲済み"
   * @retval	"false = それ以外"
   */
  bool GetPokeGetFlag( MonsNo monsno ) const;

  /**
   * @brief  初ゲット時に出る図鑑登録画面を呼んだ(ログ分しか記録できない)(名前入力画面とかは関係ない)
   * @param  monsno  ポケモン番号
   */
  void SetPokeRegisterShow( MonsNo monsno );

  /**
   * @brief  初ゲット時に出る図鑑登録画面を呼んでいるかチェック(名前入力画面とかは関係ない)
   * @param  monsno  ポケモン番号
   * @param[out]  monsno  seeフラグ。戻り値がfalseのときのみ有効。[1]のときはPokeGetフラグを立てたときのseeフラグ。[2]のときは現在のseeフラグ。
   * @retval true   図鑑登録画面を呼んでいる
   * @retval false  [1]PokeGetフラグが立っているのに図鑑登録画面を呼んでいない(ログ分しか正確には判定できない) or [2]PokeGetフラグが立っていない
   */
  bool GetPokeRegisterShowFlag( MonsNo monsno, bool* seeFlag ) const;

  /**
   * @brief		現在の図鑑モードのポケモン捕獲数取得
   * @param		ポケモン番号辞書クラス
   * @return	捕獲数
   */
  u16 GetZukanPokeGetCount( const PokeTool::ExtendData * poke_extend ) const;
  
  /**
   * @brief		ポケモン捕獲数取得（全国）
   * @return	捕獲数
   */
  u16 GetPokeGetCount(void) const;
  
  /**
   * @brief		ポケモン捕獲数取得（地方）
   * @param		ポケモン番号辞書クラス
   * @return	捕獲数
   */
  u16 GetLocalPokeGetCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const;

  // PokeTool::ExtendDataなしで使える版。LOCAL_AREA_Aのみ対応。
  u16 GetLocalPokeGetCountStatic( PokeTool::ExtendData::LocalArea local_area ) const;


  ////////////////////////////////
  //
  // 見たフラグ
  //
  ////////////////////////////////

  /**
   * @brief		見たデータセット
   * @param		pp		見たポケモンのデータ
   * @note		出会った回数の追加はしていません。
   *          出会った回数の追加はIncrementPokeSeeAmountで行って下さい。
   */
  void SetPokeSee( const pml::pokepara::CoreParam & param );
  void SetPokeSee( const PokeTool::SimpleParam & param );
  
  /**
   * @brief		見たチェック
   * @param		monsno	ポケモン番号
   * @retval	"true = 見た"
   * @retval	"false = それ以外"
   */
  bool GetPokeSeeFlag( MonsNo monsno ) const;
  /**
   * @brief		見たチェック
   * @param		monsno	ポケモン番号
   * @param		form	  フォルム
   * @retval	"true = 見た"
   * @retval	"false = それ以外"
   * @note    性別とレアは何でもいいから、monsnoとformが一致するものを見ているかを返す
   *
   * @note    formが図鑑に登録されないフォルム(「GetFormNoWhenSetDrawDataで変更されるフォルム」や「コフキムシ、コフーライの0より大のフォルム」)のとき
   *          falseを返すので注意して下さい。
   */
  bool GetPokeFormSeeFlag( MonsNo monsno, FormNo form ) const;
  
  /**
   * @brief		現在の図鑑モードのポケモンを見た数取得
   * @param		ポケモン番号辞書クラス
   * @return	見た数
   */
  u16 GetZukanPokeSeeCount( const PokeTool::ExtendData * poke_extend ) const;
  
  /**
   * @brief		ポケモンを見た数取得（全国）
   * @return	見た数
   */
  u16 GetPokeSeeCount( void ) const;
  
  /**
   * @brief		ポケモンを見た数取得（地方）
   * @param		ポケモン番号辞書クラス
   * @param   地方エリア
   *
   * @return	見た数
   */
  u16 GetLocalPokeSeeCount( const PokeTool::ExtendData * poke_extend, PokeTool::ExtendData::LocalArea local_area ) const;

  // PokeTool::ExtendDataなしで使える版。LOCAL_AREA_Aのみ対応。
  u16 GetLocalPokeSeeCountStatic( PokeTool::ExtendData::LocalArea local_area ) const;
  
  /**
   * @brief		指定タイプのポケモンを見たか
   * @param		monsno	ポケモン番号
   * @param		sex			性別
   * @param		rare		レア
   * @param		form		フォルム
   * @retval	"true = 見た"
   * @retval	"false = それ以外"
   */
  bool CheckPokeSeeForm( MonsNo monsno, pml::Sex sex, bool rare, FormNo form ) const;
  bool CheckPokeSeeForm( const PokeTool::SimpleParam & simple_param ) const;

  /**
   * @brief monsnoのポケモンを今までに見た総数を追加
   *
   * @param monsno ポケモン番号
  */
  void IncrementPokeSeeAmount( MonsNo monsno );

  /**
   * @brief monsnoのポケモンを今までに見た総数を取得
   *
   * @param monsno ポケモン番号
   *
   * @return monsnoのポケモンを今までに見た総数
  */
  u16 GetPokeSeeAmount( MonsNo monsno ) const;


  ////////////////////////////////
  //
  // 完成度
  //
  ////////////////////////////////

  /**
   * @brief		全国図鑑完成チェック
   * @retval	"true = 完成"
   * @retval	"false = それ以外"
   */
  bool CheckZenkokuComp(void) const;
  
  /**
   * @brief		地方図鑑完成チェック（捕獲数）
   * @param   地方エリア
   * @param		ポケモン番号辞書クラス
   * @retval	"true = 完成"
   * @retval	"false = それ以外"
   */
  bool CheckLocalGetComp( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const;
  
  /**
   * @brief		地方図鑑完成チェック（見た数）
   * @param   地方エリア
   * @param		ポケモン番号辞書クラス
   * @retval	"true = 完成"
   * @retval	"false = それ以外"
   */
  bool CheckLocalSeeComp( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const;
  
  /**
   * @brief		全国図鑑完成に必要なポケモンの捕獲数
   * @return	捕獲数
   */
  u16 GetZenkokuGetCompCount(void) const;
  
  /**
   * @brief		地方図鑑完成に必要なポケモンの捕獲数
   * @param   地方別エリア
   * @param		ポケモン番号辞書クラス
   * @return	捕獲数
   */
  u16 GetLocalGetCompCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const;
  
  /**
   * @brief		地方図鑑完成に必要なポケモンを見た数
   * @param		ポケモン番号辞書クラス
   * @param		ポケモン番号辞書クラス
   * @return	見た数
   */
  u16 GetLocalSeeCompCount( PokeTool::ExtendData::LocalArea local_area, const PokeTool::ExtendData * poke_extend ) const;

  /**
   * @brief 地方チェック最大数を取得
   *
   * @param local_area 地方エリア
   *
   * @return 地方チェック最大数を取得
   *
   * @note  この関数の戻り値は下記のようになっています。
   *        例： 302匹いる。番号は1～302。チェック外のポケモン数が2匹。このとき・・・
   *             302-2 = 300を返す。つまり、地方図鑑で完成に必要なポケモンの個数を返す。
   */
  u16 GetLocalCompNum( const PokeTool::ExtendData * poke_extend, PokeTool::ExtendData::LocalArea local_area ) const;
 

  ////////////////////////////////
  //
  // 言語
  //
  ////////////////////////////////

  /**
   * @brief		図鑑テキストバージョンチェック
   * @param		monsno				ポケモン番号
   * @param		country_code	国コード(POKEMON_LANG_JAPAN)  // niji_project/prog/System/include/PokemonVersion.h
   * @retval	"true = 表示可"
   * @retval	"false = それ以外"
   */
  bool GetTextVersionUpFlag( MonsNo monsno, u32 country_code ) const;
   

  ////////////////////////////////
  //
  // 番号
  //
  ////////////////////////////////

  /**
   * @brief 現在の図鑑モードの末尾ポケモンを取得
   *
   * @param poke_extend　ポケモン番号辞書クラス
   *
   * @return 末尾ポケモン
   */
  u16 GetZukanTailMonsNo( const PokeTool::ExtendData * poke_extend ) const;


  ////////////////////////////////
  //
  // 個性乱数
  //
  ////////////////////////////////

  /**
   * @brief		特殊ポケモンの個性乱数取得
   * @param		monsno	ポケモン番号
   * @return	個性乱数
   */
  //u32 GetPokeRandomFlag( MonsNo monsno ) const;
  u32 GetPokeRandomFlag( MonsNo monsno, pml::Sex sex, bool rare, FormNo form ) const;


#if PM_DEBUG
  ////////////////////////////////
  //
  // デバッグ機能
  //
  ////////////////////////////////

  //強制、捕獲、見た、登録解除
  void DebugSetPokeGetForce( const pml::pokepara::CoreParam & param );
  void DebugSetPokeSeeForce( const pml::pokepara::CoreParam & param );
  void DebugSetPokeDeleteForce( const pml::pokepara::CoreParam & param );

  // 見た総数を設定する
  void Debug_SetPokeSeeAmount( MonsNo monsno, int val );
#endif

  
  //=============================================================================
  //@brief SaveDataInterface実装
  //=============================================================================
public:
  /**
   *  @brief    読み込んだデータをセットする関数
    * @param    pData   先頭のポインタ
   */
  virtual void SetData(void* data);
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  virtual void* GetData(void);
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  virtual size_t GetDataSize(void);
  /**
   * @brief セーブデータのクリア
   * @param heap　クリアに使用するテンポラリヒープ
   */
  virtual void Clear( gfl2::heap::HeapBase * heap ); 


  //=============================================================================
  //@brief 以下private
  //=============================================================================
private:
  enum SeeFlag
  {
    SEE_FLAG_MALE = 0,		// ♂
    SEE_FLAG_FEMALE,			// ♀
    SEE_FLAG_M_RARE,			// ♂レア
    SEE_FLAG_F_RARE,			// ♀レア
    SEE_FLAG_MAX,
  };

  static const int MAGIC_NUMBER = 0x2f120f17;
  static const u16 POKESEEGET_AMOUNT_VALUE_MAX = 999;  // この数値を使うものの都合上、取り得る最大値を999にしておく。  //65535;  // この値まで取り得る
 
  static const int POKEZUKAN_ARRAY_LEN = 26; ///<ポケモン図鑑フラグ配列のおおきさ 26 * 32 = 832 bitまで大丈夫（ポケモンは最大804）(正確な値はmonsno_def.hのMONSNO_END)
  static const int FORM_ARRAY_LEN = 9;       ///< フォルムフラグ配列の大きさ 9 * 32 = 288 bitまで大丈夫 (今回セーブデータで扱うフォルムは259個、これは0番フォルムを除いた値、正確な値はmonsno_def.hのOTHER_FORM_MAX)
  static const int MONS_FORM_ARRAY_LEN = POKEZUKAN_ARRAY_LEN + FORM_ARRAY_LEN;
 
  static const int TEXTVER_ARRAY_LEN = 920;		///< 言語フラグ		ポケモン数 * 言語数 / 8 = 804 * 9 / 8 = 904.5 (最低905必要)  // u8配列に使うので4の倍数にしておく。

  static const int POKESEEGET_AMOUNT_LEN = POKEZUKAN_ARRAY_LEN * 32;  // 26 * 32 = 832 まで大丈夫。0<= <=(MONSNO_END-1)を使う。MONSNO_ENDは今作最後尾のポケモンの全国図鑑番号。
                                                                      // monsNo番のポケモンのデータは配列の[monsNo-1]に入っている。monsNoが正しいかのチェックをPOKESEEGET_AMOUNT_LENの値で行っているところは気を付けて！
                                                                      // u16配列に使うので偶数にしておく。

  struct ZukanCoreData
  {
    u32 zukan_magic;					///< マジックナンバー

    u32 zenkoku_flag          : 1;       ///< 全国図鑑解放フラグ          // nijiでは使っていない
    u32 area_a_flag           : 1;       ///< 地方図鑑エリアA解放フラグ   // nijiでは使っていない。nijiではアローラ図鑑。
    u32 area_b_flag           : 1;       ///< 地方図鑑エリアB解放フラグ   // nijiでは使っていない。nijiでは島１図鑑。代わりにSYS_FLAG_ZUKAN_ISLAND1_OPEN
    u32 area_c_flag           : 1;       ///< 地方図鑑エリアC解放フラグ   // nijiでは使っていない。nijiでは島２図鑑。代わりにSYS_FLAG_ZUKAN_ISLAND2_OPEN
    u32 area_d_flag           : 1;       ///< 地方図鑑エリアD解放フラグ   // nijiでは使っていない。nijiでは島３図鑑。代わりにSYS_FLAG_ZUKAN_ISLAND3_OPEN
    u32 area_e_flag           : 1;       ///< 地方図鑑エリアE解放フラグ   // nijiでは使っていない。nijiでは島４図鑑。代わりにSYS_FLAG_ZUKAN_ISLAND4_OPEN
    u32 zukan_mode            : 3;       ///< 閲覧中の図鑑モード
    u32 defaultMonsNo         :10;       ///< 閲覧中のポケモン番号 1023まで
    u32 os_ver                : 2;       ///< 図鑑OSのバージョン（0=ロトムと話したことがない; 1=ロトムと話したことがある;）  // nijiでロトムが目覚めているかはSYS_FLAG_BOUKEN_NAVI_GET
    u32 dummy0                :11;       ///< 使っていない。バージョンアップフラグで使うかも。

    u32 dummy1[31];  ///< 使っていない。512バイト境界まで余っていたので念のため取っておく。
    u32 pachi_see_get_flag;  ///< パッチール見た捕まえたフラグ。0<=ビット目<SEE_FLAG_MAX 見たフラグ。SEE_FLAG_MAX+0<=ビット目<SEE_FLAG_MAX+SEE_FLAG_MAX 捕まえたフラグ。

    u32 get_flag[POKEZUKAN_ARRAY_LEN];    ///< 捕まえたフラグ用ワーク
    
    u32 see_flag[SEE_FLAG_MAX][MONS_FORM_ARRAY_LEN];   ///< 見たフラグ用ワーク
    u32	draw_flag[SEE_FLAG_MAX][MONS_FORM_ARRAY_LEN];  ///< 閲覧中フラグ
    
    u8 TextVersionUp[TEXTVER_ARRAY_LEN];	///< 言語バージョンアップマスク

    u32 PachiRandom[SEE_FLAG_MAX];   ///< パッチール用個性乱数保持ワーク

    u16 see_amount[POKESEEGET_AMOUNT_LEN];  ///< 見た総数
  };


  // PokeRegisterShow -> PokeGet , PokeGet -> PokeRegisterShow のどちらの順番で呼ばれてもいいように用意したログ。
  struct ZukanNotSaveData
  {
    enum
    {
      LOG_NUM_MAX  = 16
    };
    s8  logNumForNotPokeRegisterShow;  // 0<=logNumForNotPokeRegisterShow<=LOG_NUM_MAX  配列のインデックスはLOG_NUM_MAX未満
    s8  logNumForNotPokeGet;           // 0<=logNumForNotPokeGet<=LOG_NUM_MAX  配列のインデックスはLOG_NUM_MAX未満
    u16 logNotPokeRegisterShowSeeFlag;  // logNotPokeRegisterShowMonsNoに連動。seeフラグ。logNotPokeRegisterShowMonsNo[i]の情報は「logNotPokeRegisterShowSeeFlag&(1<<i)」で取れる。
    u16 logNotPokeRegisterShowMonsNo[LOG_NUM_MAX];   // PokeGetフラグを立てたがまだ図鑑登録画面を呼んでいないポケモン(1番最新に呼ばれたもの[0]～1番最古に呼ばれたもの[LOG_NUM_MAX-1])
    u16 logNotPokeGetMonsNo[LOG_NUM_MAX];            // 図鑑登録画面を呼んだがまだPokeGetフラグを立てていないポケモン(1番最新に呼ばれたもの[0]～1番最古に呼ばれたもの[LOG_NUM_MAX-1])
  };


private:
  ////////////////////////////////
  //////// 不正チェック
  ////////////////////////////////
  void IncorrectZukan( void ) const;
  bool IncorrectMonsNo( MonsNo monsno ) const;
 
  ////////////////////////////////
  //////// 汎用的なビット操作
  ////////////////////////////////
  bool CheckBit(const u8 * array, u16 flag_id) const;
  bool CheckBitMons(const u8 * array, MonsNo flag_id) const;
  void SetBit(u8 * array, u16 flag_id, u32 array_len );
  void SetBitMons(u8 * array, MonsNo flag_id);
  void ResetBit( u8 * array, u16 flag_id, u32 array_len );
  void ResetBitMons( u8 * array, MonsNo flag_id );

  ////////////////////////////////
  //////// 見た、捕まえた
  ////////////////////////////////
  void SetGetBit( MonsNo flag_id );
  void SetSeeBit( MonsNo mons, pml::Sex sex, bool rare, FormNo form );
  
  bool CheckGetBit( MonsNo monsno ) const;
  bool CheckSeeBit( MonsNo mons, pml::Sex sex, bool rare, FormNo form ) const;

  ////////////////////////////////
  //////// フォルム補正
  ////////////////////////////////
public:
  // 表示用のデータを設定する際に、あるフォルムに変更して設定するときに使う関数
  void GetFormNoWhenSetDrawData( MonsNo mons, FormNo form, FormNo* form_return ) const;
private:
  // 見たフラグを設定する際に、あるフォルム(範囲指定可能)に変更して設定するときに使う関数(1個しか見てないのに複数見たことにすることも可能)
  void GetFormStartEndWhenSetSeeBit( MonsNo mons, FormNo form, FormNo* form_start, FormNo* form_end ) const;

  ////////////////////////////////
  //////// 個性乱数
  ////////////////////////////////
  //void SetZukanRandom( MonsNo monsno, u32 rand, bool isGet );
  void SetZukanRandom( MonsNo monsno, pml::Sex sex, bool rare, FormNo form, u32 rand, bool isGet );
  
  ////////////////////////////////
  //////// 言語
  ////////////////////////////////
  void SetZukanTextVersionUp( MonsNo monsno, u32 lang );
  void ResetZukanTextVersionUp( MonsNo monsno, u32 lang );
 
  ////////////////////////////////
  //////// 完成度
  ////////////////////////////////
  bool CheckZenkokuCompMonsno( MonsNo monsno ) const;
  bool CheckLocalCompMonsno( MonsNo monsno, PokeTool::ExtendData::LocalArea local_area ) const;
  const u16 * GetLocalNotCheckMonsNoTable( PokeTool::ExtendData::LocalArea local_area, u16 *table_num ) const;

  ////////////////////////////////
  //////// テーブルアクセス
  ////////////////////////////////
  s32 GetPokeFormTablePos( MonsNo mons ) const;
  s32 GetPokeMegaTablePos( MonsNo mons ) const;
  
  // MonsNo, FormNoを一列に並べたとき、何番目(0始まり)に来るか？
  static const u16 MONS_FORM_INDEX_INVALID = 0xFFFF;
  u16 GetMonsFormIndex(MonsNo monsNo, FormNo formNo) const;  // この関数が不正値MONS_FORM_INDEX_INVALIDを返したときも、呼び出し元は正常に動くようにして下さい。

  ////////////////////////////////
  //////// ZukanNotSaveData関連
  ////////////////////////////////
  // 初回のSetPokeGet時に呼んで、PokeRegisterShowを修正する。初回のSetPokeGet時のseeフラグを渡す。
  void ModifyPokeRegisterShowBySetPokeGet( MonsNo monsno, bool seeFlag );
#if PM_DEBUG
  void PrintNotSaveData(const char* mes) const;
#endif  // PM_DEBUG


private:
  ZukanCoreData m_data;
  ZukanNotSaveData m_notSaveData;

};

GFL_NAMESPACE_END(Savedata)

#endif		// __SV_ZUKAN_H__


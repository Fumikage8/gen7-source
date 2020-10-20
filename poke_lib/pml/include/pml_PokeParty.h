//========================================================================================
/**
 * @file    pml_PokeParty.h
 * @brief   ポケモン手持ちパーティ
 * @author  taya, obata
 * @date    2010.12.17 作成
 * @date    2011.01.28 インターフェースを引き継ぎ実装
 */
//========================================================================================
#ifndef __PML_POKE_PARTY_H__
#define __PML_POKE_PARTY_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <pml/include/pml_PokePara.h>


namespace pml {

  /**
   *  ポケモンパーティクラス
   */
  class PokeParty {
    GFL_FORBID_COPY_AND_ASSIGN(PokeParty);

  public:
    enum {
      MAX_MEMBERS = 6,        ///< 格納できる最大メンバー数
      MEMBER_INDEX_ERROR = MAX_MEMBERS, ///<メンバ参照Indexエラー値
    };

    /**
     * @brief シリアライズデータ
     * @attention セーブデータなどで固定サイズが必要な場合のために公開
     * @caution 公開されているが直接操作はしないこと！
     */
    //@attention __attribute__が通らなかったため記述を変えて移植
    //参考資料：http://msdn.microsoft.com/ja-jp/library/83ythb65.aspx
    __declspec(align(4)) struct SERIALIZE_DATA
    {
      friend class PokeParty;
      private:
      //@attention __attribute__が通らなかったため記述を変えて移植
      //参考資料：http://msdn.microsoft.com/ja-jp/library/83ythb65.aspx
      //u8 poke_param[260 * MAX_MEMBERS] __attribute__((aligned(4)));
      __declspec(align(4)) u8 poke_param[260 * MAX_MEMBERS];
      u8 m_memberCount;   ///< メンバー数
      u8 markingIndex;    ///< 特定のメンバーの位置
      u8 padding[2];      ///< 未使用領域

    };//__attribute__((aligned(4)));

    /**
     * @brief シリアライズデータのサイズ
     */
    static const size_t SERIALIZE_DATA_SIZE = 1564;  //260 * 6 + 4

  public:
    PokeParty( gfl2::heap::HeapBase* pHeap );
    ~PokeParty();


    /**
     * @brief   メンバーを末尾に追加
     *
     * @param[in]   pp    追加するメンバーのデータ
     *
     * @retval      bool  追加できたらtrue /  一杯だったらfalse
     *
     * @note
     * 引数として渡したパラメータはコピーされます.
     * 不要になった時点で削除してください.
     */
    bool
      AddMember( const pokepara::PokemonParam& pp );

    /**
     *  @brief  指定位置のメンバーを新メンバーと置換（新メンバーデータで上書きされるので注意してください）
     *
     *  @param  idx         置換するメンバー位置（ 0 origin ）
     *  @param  pp          新メンバーデータ
     *
     * @note
     * 引数として渡したパラメータはコピーされます.
     * 不要になった時点で削除してください.
     */
    void
      ReplaceMember( u32 idx, const pokepara::PokemonParam& pp );

    /**
     * @brief   メンバー１体削除
     *
     * @param      idx    削除するメンバーの位置（ 0 origin ）
     */
    void
      RemoveMember( u32 idx );

    /**
     * @brief ポケモンの位置を入れ替える
     *
     * @param pos1  入れ替えるポケモンの位置1
     * @param pos2  入れ替えるポケモンの位置2
     */
    void
      ExchangePosition( u8 pos1, u8 pos2 );

    /**
     * @brief 指定位置のポケモンを指定位置に入れる
     * @param src  元位置
     * @param dest 移動位置
     */
//    void InsertPosition( u32 src, u32 dest );

    /**
     * @brief ポケモンを配列の順番に並び替える
     * @param row 並び順配列
     *
     * @note rowは要素数m_memberCount以上で、それぞれに0〜m_memberCountが入っている必要がある
     */
    void ChangeRow( u8 * row );

    /**
     * @brief   メンバーのポインタを取得
     *
     * @param[in]   idx   何番目のメンバー？（0 origin）
     *
     * @retval      pml::pokepara::PokemonParam*    メンバーデータポインタ
     */
    pml::pokepara::PokemonParam*
      GetMemberPointer( u32 idx );

    const pml::pokepara::PokemonParam*
      GetMemberPointerConst( u32 idx ) const;

    /**
     * @brief   メンバー数の取得
     *
     * @retval      u32   メンバー数
     */
    u32
      GetMemberCount( void ) const;

    /**
     * @brief 指定したインスタンスのメンバーインデックスを取得する
     *
     * @param pokeParam  インデックスを調べるポケモン
     * 
     * @retval MAX_MEMBERS  指定したインスタンスがパーティに含まれていない場合
     */
    u32 GetMemberIndex( const pml::pokepara::PokemonParam* pokeParam ) const;

    //-------------------------------------------------------------------------------------
    /**
     * @brief メンバー数カウント条件の指定子
     */
    //-------------------------------------------------------------------------------------
    enum CountType {
      POKE_COUNT_TYPE_ALL,               // 全ての手持ちポケモン
      POKE_COUNT_TYPE_BATTLE_ENABLE,     // バトル可能なポケモン（ 非タマゴ & HP>0 ）
      POKE_COUNT_TYPE_NOT_EGG,           // タマゴ以外のポケモン
      POKE_COUNT_TYPE_ONLY_LEGAL_EGG,    // 正規タマゴのみ
      POKE_COUNT_TYPE_ONLY_ILLEGAL_EGG,  // 不正タマゴのみ
      POKE_COUNT_TYPE_BOTH_EGG,          // 正規タマゴ or 不正タマゴ
    };

    /**
     * @brief 条件付きメンバー数カウント
     *
     * @param type  カウントの条件
     *
     * @retval 条件にマッチするメンバー数（ 0 〜 MAX_MEMBERS ）
     */
    u32
      GetMemberCountEx( CountType type ) const;
    
    /**
     * @brief 条件付きメンバー数カウント（位置指定あり）
     *
     * @param type  カウントの条件
     * @param pass_idx_bit  カウントをパスする位置の指定（ビット単位）
     *
     * @retval 条件にマッチするメンバー数（ 0 〜 MAX_MEMBERS ）
     */
    u32 GetMemberCountEx( CountType type, u8 pass_idx_bit ) const;

    //-------------------------------------------------------------------------------------
    /**
     * @brief メンバーサーチ条件の指定子
     */
    //-------------------------------------------------------------------------------------
    enum SearchType {
      POKE_SEARCH_TYPE_BATTLE_ENABLE,     // バトル可能なポケモン（ 非タマゴ & HP>0 ）
      POKE_SEARCH_TYPE_NOT_EGG,           // タマゴ以外のポケモン
    };
  
    /**
     * @brief 条件付き先頭ポケモンの位置取得
     *
     * @param type  サーチ条件
     *
     * @retval 0-5  条件にマッチするポケモンの位置
     * @retval MEMBER_INDEX_ERROR  条件にマッチするポケモンがいなかった(ダメタマゴOnlyなど)
     */
    u32 
      GetMemberTopIndex( SearchType type ) const;

    /**
     * @brief   特定モンスターナンバーのポケモンがパーティ内にいるかどうか調べる
     *
     * @parma   monsno  モンスターナンバー指定
     *
     * @retval  指定したモンスターナンバーのポケモンがいれば true
     * @note     タマゴの中は見ない
     */
    bool
      CheckPokeExist( MonsNo monsno ) const;

    /**
     * @brief   メンバーが一杯かどうか判定
     *
     * @retval      bool    一杯なら true
     */
    bool
      IsFull( void ) const;

    /**
     * @brief   データコピー
     *
     * @param[in]   src   コピー元
     */
    void
      CopyFrom( const PokeParty& src );

    /**
     * @brief   内容をクリア（ポケモンデータが全てクリアされます）
     */
    void
        Clear( void );

    /**
     * @brief   シリアライズ
     *
     * @param[out]   dst    シリアライズデータ書き込みバッファ
     *
     * @retval       size_t    シリアライズデータサイズ（bytes）
     */
    size_t SerializeFull( SERIALIZE_DATA * dst ) const;
    size_t SerializeCore( SERIALIZE_DATA * dst ) const;

    /**
     * @brief   シリアライズ（PokemonParam6匹版、サーバに６匹だけアップするときなど）
     */
    u32 SerializeToShortStruct( void* dst ) const;

    /**
     * @brief   デシリアライズ
     *
     * @param[in]    data   デシリアライズデータポインタ
     */
    void DeserializeFull( const SERIALIZE_DATA * data );
    void DeserializeCore( const SERIALIZE_DATA * data );

    /**
     * @brief   デシリアライズ（PokemonParam6匹版、サーバに６匹だけアップするときなど）
     */
    void DeserializeFromShortStruct( const void* data );


    /**
     * @brief   シリアライズデータサイズ（最小サイズ）取得
     *
     * @note  通常のシリアライズデータと比較すると欠損するデータがあるので注意
     * @retval       u32    シリアライズデータサイズ（bytes）
     */
    static u32
      GetSerializeShortStructSize( void );



    /**
     * @brief ポケルスに感染しているメンバーがいるかどうかを調べる
     * @retval true   ポケルスに感染しているメンバーがいる場合
     * @retval false  ポケルスに感染しているメンバーがいない場合
     */
    bool CheckPokerusExist( void ) const;

    /**
     * @brief 戦闘一回分の新規ポケルス感染チェックを行う
     * @retval true   感染した
     * @retval false  感染しなかった
     */
    bool PokerusCatchCheck( void );

    /**
     * @brief 戦闘一回分のポケルス伝染チェックを行う
     * @retval true   伝染した
     * @retval false  伝染しなかった
     */
    bool PokerusInfectionCheck( void );

    /**
     * @brief       ポケルス感染の経過処理
     * @param[in]   passed_day_count
     */
    void DecreasePokerusDayCount( s32 passed_day_count );



    /**
     * @brief パーティー全回復
     */
    void RecoverAll( void );



    /**
     * @brief 特定のメンバーの位置を設定
     * @param pos   位置
     *
     * @note 最後に可愛がったメンバーの位置を表します。
     *       用途が分かる名前に改名したい。
     */
    void SetMarkingIndex( u32 pos );

    /**
     * @brief 特定のメンバーの位置を取得
     * @retval 特定のメンバーの位置
     *
     * @note 最後に可愛がったメンバーの位置を表します。
     *       用途が分かる名前に改名したい。
     */
  	u32 GetMarkingIndex(void) const;

    /**
     * @brief   交換できる状態かどうかの判定
     * @return  bool  trueのとき、交換できる
     */
    bool CanTrade( void ) const;

    /**
     * @brief 指定ポケモンを交換にだせるかどうかの判定
     * @param[in]   交換に出すポケモンの位置
     * @return      bool  交換に出せる
     *
     * @note    交換に出した後、戦闘可能かどうかを判定しています。
     */
    bool CanTradeMember( u32 idx ) const;

#if PML_DEBUG
    //! パラメータのデバッグ出力
    void Dump() const;
#else
    // 製品版は空にしておく
    void Dump() const{}
#endif

  private:
    pokepara::PokemonParam* m_member[ MAX_MEMBERS ];
    u8                      m_memberCount;   ///< メンバー数
    u8                      markingIndex;    ///< 特定のメンバーの位置

    void
        scootOver( void );

    void ClearMarkingIndex(void);


    size_t SerializeCommon( SERIALIZE_DATA * dst, const bool isFull ) const;
    void DeserializeCommon( const SERIALIZE_DATA * data, const bool isFull );


  };  // class PokeParty


} // namespace pml


#endif // __PML_POKE_PARTY_H__


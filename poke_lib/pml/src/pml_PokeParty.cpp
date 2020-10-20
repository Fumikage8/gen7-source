//========================================================================================
/**
 * @file    pml_PokeParty.cpp
 * @brief   ポケモン手持ちパーティ
 * @author  taya, obata
 * @date    2010.12.17 作成
 * @date    2011.01.28 インターフェースを引き継ぎ実装
 */
//========================================================================================
#include <pml/include/pml_Local.h>
#include <pml/include/pml_PokePara.h>
#include <pml/include/pml_PokeParty.h>

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

namespace pml {

  /**
   *  コンストラクタ
   */
  PokeParty::PokeParty( gfl2::heap::HeapBase* pHeap )
  {
    STATIC_ASSERT( sizeof(SERIALIZE_DATA) == SERIALIZE_DATA_SIZE );
    GFL_ASSERT_STOP( pokepara::PokemonParam::DATASIZE == 260 ); //@fix　起動時に必ずチェック

    markingIndex  = 0;
    m_memberCount = 0;

    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      m_member[i]  = GFL_NEW( pHeap ) pokepara::PokemonParam( pHeap, MONSNO_NULL, 1, 0 );
      if( m_member[i] == NULL )
      {
        GFL_ASSERT(0);  //@check
      }
    }
  }
  /**
   *  デストラクタ
   */
  PokeParty::~PokeParty( )
  {
    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      GFL_SAFE_DELETE( m_member[i] );
    }
  }

  /**
   * @brief   メンバーを末尾に追加
   *
   * @param[in]   pp    追加するメンバーのデータ
   *
   * @retval      bool  追加できたらtrue /  一杯だったらfalse
   */
  bool PokeParty::AddMember( const pokepara::PokemonParam& pp )
  {
    // 無効データは追加しない
    if( pp.IsNull() ) {
      return false;
    }

    if( m_memberCount < MAX_MEMBERS )
    {
      if( m_member[ m_memberCount ] ) {
        m_member[ m_memberCount ]->CopyFrom( pp );
        ++m_memberCount;
        // BTS[6929] Marking対象がタマゴのときは再設定
        if( m_member[markingIndex]->IsEgg(pokepara::CHECK_BOTH_EGG) == true ){
          ClearMarkingIndex();
        }
        return true;
      }
    }
    return false;
  }
  /**
   *  @brief  指定位置のメンバーを新メンバーと置換
   *
   *  @param  idx         置換するメンバー位置（ 0 origin ）
   *  @param  pp          新メンバーデータ
   *
   */
  void PokeParty::ReplaceMember( u32 idx, const pokepara::PokemonParam& pp )
  {
    if( idx < MAX_MEMBERS )
    {
      if( m_member[idx] && !pp.IsNull() )
      {
        bool  fExisted = !m_member[idx]->IsNull();

        m_member[idx]->CopyFrom( pp );

        if( !fExisted )
        {
          ++m_memberCount;
        }
        if( markingIndex == idx ){
          ClearMarkingIndex();
        }
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "idx=%d", idx); //@check 引数異常は何もせず戻る
    }
  }

  /**
   * @brief   メンバー１体削除
   *
   * @param      idx    削除するメンバーの位置（ 0 origin ）
   */
  void PokeParty::RemoveMember( u32 idx )
  {
    if( idx < MAX_MEMBERS )
    {
      if( m_memberCount > 0 )
      {
        if( (m_member[idx] != NULL) && (!m_member[idx]->IsNull()) ) {
          m_member[idx]->Clear();
          this->scootOver();
          --m_memberCount;

          if( markingIndex == idx ){
            ClearMarkingIndex();
          }else if( markingIndex > idx ){
            markingIndex--;
          }
        }
      }
      else
      {
        GFL_ASSERT(0);  // メンバー数==0 なのに呼ばれた //@check 何もせず戻る
      }
    }
    else
    {
      GFL_ASSERT_MSG( 0, "idx=%d", idx ); //@check 引数異常は何もせず戻る
    }
  }
  /**
   * @brief ポケモンの位置を入れ替える
   *
   * @param pos1  入れ替えるポケモンの位置1
   * @param pos2  入れ替えるポケモンの位置2
   */
  void PokeParty::ExchangePosition( u8 pos1, u8 pos2 )
  {
    if( (pos1<MAX_MEMBERS) && (pos2<MAX_MEMBERS) && (pos1!=pos2) )
    {
      // pos1 < pos2 にしておく
      if( pos1 > pos2 ){
        u8 tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
      }

      pokepara::PokemonParam* tmp_param = m_member[ pos1 ];
      m_member[ pos1 ] = m_member[ pos2 ];
      m_member[ pos2 ] = tmp_param;

      scootOver();

      if( markingIndex == pos1 ){
        markingIndex = pos2;
      }else if( markingIndex == pos2 ){
	      markingIndex = pos1;
      }
    }
    else
    {
      GFL_ASSERT_MSG( 0, "pos1=%d, pos2=%d", pos1, pos2 );  //@check　何もせず戻る
    }
  }

  /**
   * @brief 指定位置のポケモンを指定位置に入れる
   * @param src  元位置
   * @param dest 移動位置
   */
#if 0   // 追加したけど、使わなくなったのでコメントアウト
  void PokeParty::InsertPosition( u32 src, u32 dest )
  {
    if( src >= m_memberCount || dest >= m_memberCount || src == dest ){
      GFL_ASSERT_MSG( 0, "src=%d, dest=%d", src, dest );
    }

    pokepara::PokemonParam * tmp = m_member[src];

    if( src > dest ){
      for( int i=src; i>dest; i-- ){
        m_member[i] = m_member[i-1];
      }
			if( markingIndex >= dest && markingIndex <= src ){
				if( markingIndex == src ){
					markingIndex = dest;
				}else{
					markingIndex++;
				}
			}
    }else if( src < dest ){
      for( int i=src; i<dest; i++ ){
        m_member[i] = m_member[i+1];
      }
			if( markingIndex >= src && markingIndex <= dest ){
				if( markingIndex == src ){
					markingIndex = dest;
				}else{
					markingIndex--;
				}
			}
    }
    m_member[dest] = tmp;
  }
#endif

  /**
   * @brief ポケモンを配列の順番に並び替える
   * @param row 並び順配列
   *
   * @note rowは要素数m_memberCount以上で、それぞれに0〜m_memberCountが入っている必要がある
   */
  void PokeParty::ChangeRow( u8 * row )
	{
		pokepara::PokemonParam * tmp[MAX_MEMBERS];
		bool is_mark_change = false;

		for( int i=0; i<m_memberCount; i++ ){
			tmp[i] = m_member[i];
		}
		for( int i=0; i<m_memberCount; i++ ){
			m_member[i] = tmp[ row[i] ];
			if( is_mark_change == false && row[i] == markingIndex ){
				markingIndex = i;
				is_mark_change = true;
			}
		}
	}

  /**
   * @brief メンバー位置に空きが出来ていたら詰める
   */
  void PokeParty::scootOver( void )
  {
    for( int i=MAX_MEMBERS-2; i>=0; --i )
    {
      if( m_member[i]->IsNull() )
      {
        pokepara::PokemonParam* nullMember = m_member[i];

        for(int j=i+1; j<MAX_MEMBERS; ++j)
        {
          m_member[j-1] = m_member[j];
        }

        m_member[MAX_MEMBERS-1] = nullMember;
      }
    }
  }

  /**
   * @brief   メンバーのポインタを取得
   *
   * @param[in]   idx   何番目のメンバー？（0 origin）
   *
   * @retval      pml::pokepara::PokemonParam*    メンバーデータポインタ
   */
  pokepara::PokemonParam* PokeParty::GetMemberPointer( u32 idx )
  {
    return const_cast<pml::pokepara::PokemonParam*>( this->GetMemberPointerConst(idx) );
  }
  const pokepara::PokemonParam* PokeParty::GetMemberPointerConst( u32 idx ) const
  {
    if( idx < MAX_MEMBERS )
    {
      return m_member[ idx ];
    }
    else
    {
      GFL_ASSERT_STOP_MSG( 0, "idx=%d", idx );  // @fix NULLを返すと何が起きるか予想不可能なのでSTOP
      return NULL;
    }
  }

  /**
   * @brief 指定したインスタンスのメンバーインデックスを取得する
   *
   * @param pokeParam  インデックスを調べるポケモン
   * 
   * @retval MAX_MEMBERS  指定したインスタンスがパーティに含まれていない場合
   */
  u32 PokeParty::GetMemberIndex( const pml::pokepara::PokemonParam* pokeParam ) const
  {
    const u32 memberCount = this->GetMemberCount();

    for( u32 i=0; i<memberCount; ++i )
    {
      if( this->GetMemberPointerConst( i ) == pokeParam )
      {
        return i;
      }
    }

    return MAX_MEMBERS;
  }

  /**
   * @brief   メンバーが一杯かどうか判定
   *
   * @retval      bool    一杯なら true
   */
  bool PokeParty::IsFull( void ) const
  {
    return (m_memberCount == MAX_MEMBERS);
  }
  /**
   * @brief   メンバー数の取得
   *
   * @retval      u32   メンバー数
   */
  u32 PokeParty::GetMemberCount( void ) const
  {
    return m_memberCount;
  }

  /**
   * @brief 条件付きメンバー数カウント
   *
   * @param type  カウントの条件
   *
   * @retval 条件にマッチするメンバー数（ 0 〜 MAX_MEMBERS ）
   */
  u32 PokeParty::GetMemberCountEx( CountType type ) const
  {
    return GetMemberCountEx( type, 0 ); //全部対象
  }

  /**
   * @brief 条件付きメンバー数カウント
   *
   * @param type  カウントの条件
   * @param pass_idx_bit  カウントをパスする位置の指定（ビット単位）
   *
   * @retval 条件にマッチするメンバー数（ 0 〜 MAX_MEMBERS ）
   */
  u32 PokeParty::GetMemberCountEx( CountType type, u8 pass_idx_bit ) const
  {
    u32 count = 0;
    for( u32 i=0; i<m_memberCount; ++i, pass_idx_bit >>= 1 )
    {
      if ( pass_idx_bit & 1 ) continue;

      const pokepara::PokemonParam* param = this->GetMemberPointerConst(i);
      if( (param != NULL)
          &&  (!param->IsNull())
        ){
        switch( type ) {
        case POKE_COUNT_TYPE_ALL:  // 全ての手持ちポケモン
          ++count;
          break;
        case POKE_COUNT_TYPE_BATTLE_ENABLE:  // バトル可能なポケモン
          if( ( param->IsEgg( pokepara::CHECK_BOTH_EGG ) == false ) &&
              ( param->IsHpZero() == false ) ) {
            ++count;
          }
          break;
        case POKE_COUNT_TYPE_NOT_EGG:  // タマゴ以外のポケモン
          if( param->IsEgg( pokepara::CHECK_BOTH_EGG ) == false ) {
            ++count;
          }
          break;
        case POKE_COUNT_TYPE_ONLY_LEGAL_EGG:  // 正規タマゴのみ
          if( param->IsEgg( pokepara::CHECK_ONLY_LEGAL_EGG ) ) {
            ++count;
          }
          break;
        case POKE_COUNT_TYPE_ONLY_ILLEGAL_EGG:  // 不正タマゴのみ
          if( param->IsEgg( pokepara::CHECK_ONLY_ILLEGAL_EGG ) ) {
            ++count;
          }
          break;
        case POKE_COUNT_TYPE_BOTH_EGG:  // 正規タマゴ or 不正タマゴ
          if( param->IsEgg( pokepara::CHECK_BOTH_EGG ) ) {
            ++count;
          }
          break;
        default:
          GFL_ASSERT(0);  //@check　引数異常は何もしない
          break;
        }
      }
    }
    return count;
  }
  
  /**
   * @brief 条件付き先頭ポケモンの位置取得
   *
   * @param type  サーチ条件
   *
   * @retval 0-5  条件にマッチするポケモンの位置
   * @retval MEMBER_INDEX_ERROR  条件にマッチするポケモンがいなかった(ダメタマゴOnlyなど)
   */
  u32 PokeParty::GetMemberTopIndex( SearchType type ) const
  {
    for( u32 i=0; i<m_memberCount; ++i )
    {
      const pokepara::PokemonParam* param = this->GetMemberPointerConst(i);
      if( (param != NULL)
          &&  (!param->IsNull())
        ){
        switch( type ) {
        case POKE_SEARCH_TYPE_BATTLE_ENABLE:  // バトル可能なポケモン
          if( ( param->IsEgg( pokepara::CHECK_BOTH_EGG ) == false ) &&
              ( param->IsHpZero() == false ) ) {
            return i;
          }
          break;
        case POKE_SEARCH_TYPE_NOT_EGG:  // タマゴ以外のポケモン
          if( param->IsEgg( pokepara::CHECK_BOTH_EGG ) == false ) {
            return i;
          }
          break;
        default:
          GFL_ASSERT(0);  //@check　引数異常は何もしない
          break;
        }
      }
    }
    return MEMBER_INDEX_ERROR;
  }

  /**
   * @brief   特定モンスターナンバーのポケモンがパーティ内にいるかどうか調べる
   *
   * @param   monsno  モンスターナンバー指定
   *
   * @retval  指定したモンスターナンバーのポケモンがいれば true
   * @note    タマゴの中は見ない
   */
  bool PokeParty::CheckPokeExist( MonsNo monsno ) const
  {
    for( u32 i=0; i<MAX_MEMBERS; i++ )
    {
      const pokepara::PokemonParam* param = this->GetMemberPointerConst(i);
      if( param && (param->IsEgg(pokepara::CHECK_BOTH_EGG) == false) && ( param->GetMonsNo() == monsno ) ) {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief   データコピー
   *
   * @param[in]   src   コピー元
   */
  void PokeParty::CopyFrom( const PokeParty& src )
  {
    for( u32 i=0; i<MAX_MEMBERS; ++i )
    {
      m_member[i]->CopyFrom( *( src.m_member[i] ) );
    }
    m_memberCount = src.m_memberCount;
  }

  /**
   * @brief   内容をクリア（ポケモンデータが全てクリアされます）
   */
  void PokeParty::Clear( void )
  {
    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      m_member[ i ]->Clear();
    }
    m_memberCount = 0;
    markingIndex  = 0;
  }

  //----------------------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------------------

  /**
   * @brief   シリアライズ（PokemonParam6匹版、サーバに６匹だけアップするときなど）
   */
  u32  PokeParty::SerializeToShortStruct( void* dst ) const
  {
    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      m_member[i]->Serialize_Core( dst );

      dst = reinterpret_cast<void*>(
          reinterpret_cast<uptr>(dst) + pml::pokepara::PokemonParam::DATASIZE
          );
    }

    return GetSerializeShortStructSize();
  }

  /**
   * @brief   シリアライズ
   *
   * @param[out]   dst    シリアライズデータ書き込みバッファ
   *
   * @retval       size_t    シリアライズデータサイズ（bytes）
   */
  size_t PokeParty::SerializeFull( SERIALIZE_DATA * dst ) const
  {
    return SerializeCommon( dst, true );
  }
  size_t PokeParty::SerializeCore( SERIALIZE_DATA * dst ) const
  {
    return SerializeCommon( dst, false );
  }

  /**
   * @brief   デシリアライズ（PokemonParam6匹版、サーバに６匹だけアップするときなど）
   */
  void  PokeParty::DeserializeFromShortStruct( const void* data )
  {
    m_memberCount = 0;

    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      m_member[i]->Deserialize_Core( data );

      data = reinterpret_cast<const void*>(
          reinterpret_cast<uptr>(data) + pokepara::PokemonParam::DATASIZE
          );

      if( !m_member[i]->IsNull() ){
        ++m_memberCount;
      }
    }
  }

  /**
   * @brief   デシリアライズ
   *
   * @param[in]    data   デシリアライズデータポインタ
   */
  void  PokeParty::DeserializeFull( const SERIALIZE_DATA * data )
  {
    DeserializeCommon( data, true );
  }
  void  PokeParty::DeserializeCore( const SERIALIZE_DATA * data )
  {
    DeserializeCommon( data, false );
  }

  /**
   *  シリアライズデータサイズ取得
   */
  u32 PokeParty::GetSerializeShortStructSize( void )
  {
    return pokepara::PokemonParam::DATASIZE * MAX_MEMBERS;
  }







  /**
   * @brief ポケルスに感染しているメンバーがいるかどうかを調べる
   * @retval true   ポケルスに感染しているメンバーがいる場合
   * @retval false  ポケルスに感染しているメンバーがいない場合
   */
  bool PokeParty::CheckPokerusExist( void ) const
  {
    for( u32 i=0; i<m_memberCount; ++i ) {
      if( m_member[i]->HavePokerusUntilNow() ) {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief 戦闘一回分の新規ポケルス感染チェックを行う
   * @retval true   感染した
   * @retval false  感染しなかった
   */
  bool PokeParty::PokerusCatchCheck( void )
  {
    bool catch_flag = false;  // 感染したかどうか
    u32 rnd = pml::local::Random::GetValue( 0x10000 );
    if( ( rnd == 16384 ) || ( rnd == 32768 ) || ( rnd == 49152 ) )
    {
      u8 member_count = this->GetMemberCount();
      u8 target_member_index = 0;
      MonsNo target_monsno = MONSNO_NULL;
      bool target_is_egg = true;
      while( ( target_monsno == MONSNO_NULL ) || ( target_is_egg == true ) ) {
        target_member_index = pml::local::Random::GetValue( member_count );
        target_monsno = m_member[ target_member_index ]->GetMonsNo();
        target_is_egg = m_member[ target_member_index ]->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG );
      }
      if( !m_member[ target_member_index ]->HavePokerusUntilNow() ) {
        m_member[ target_member_index ]->CatchPokerus();
        catch_flag = true;
      }
    }
    return catch_flag;
  }

  /**
   * @brief 戦闘一回分のポケルス伝染チェックを行う
   * @retval true   伝染した
   * @retval false  伝染しなかった
   */
  bool PokeParty::PokerusInfectionCheck( void )
  {
    bool infection_flag = false;  // 伝染したかどうか
    if( pml::local::Random::GetValue(3) == 0 )
    {
      u8 member_count = this->GetMemberCount();
      for( u8 pos=0 ; pos<member_count; pos++ ) {
        const pml::pokepara::PokemonParam* source_pokemon = m_member[ pos ];
        pml::pokepara::PokemonParam* prev_pokemon = ( 0 < pos ) ? m_member[ pos - 1 ] : NULL;
        pml::pokepara::PokemonParam* next_pokemon = ( pos < member_count-1 ) ? m_member[ pos + 1 ] : NULL;
        if( source_pokemon->HavePokerusJustNow() ) {
          // 前のポケモンに伝染
          if( prev_pokemon ) {
            if( !prev_pokemon->HavePokerusUntilNow() ) {
              source_pokemon->InfectPokerusWith( prev_pokemon );
              infection_flag = true;
            }
          }
          // 次のポケモンに伝染
          if( next_pokemon ) {
            if( !next_pokemon->HavePokerusUntilNow() ) {
              source_pokemon->InfectPokerusWith( next_pokemon );
              infection_flag = true;
              pos++;  // 感染させられたポケモンが感染源にならないようにする
            }
          }
        }
      }
    }
    return infection_flag;
  }

  /**
   * @brief       ポケルス感染の経過処理
   * @param[in]   passed_day_count
   */
  void PokeParty::DecreasePokerusDayCount( s32 passed_day_count )
  {
    u8 member_count = this->GetMemberCount();
    for( u8 pos=0 ; pos<member_count; ++pos ) {
      pml::pokepara::PokemonParam* pp = this->GetMemberPointer(pos);
      pp->DecreasePokerusDayCount( passed_day_count );
    }
  }



  /**
   * @brief パーティー全回復
   */
  void PokeParty::RecoverAll( void )
  {
    u8 member_count = this->GetMemberCount();
    for( u8 i=0; i<member_count; i++ ) {
      pokepara::PokemonParam* pp = this->GetMemberPointer(i);
      pp->RecoverAll();
    }
  }



  /**
   * @brief 特定のメンバーの位置を設定
   * @param pos   位置
   */
	void PokeParty::SetMarkingIndex( u32 pos )
	{
    markingIndex = pos;
  }

  /**
   * @brief 特定のメンバーの位置を取得
   * @retval 特定のメンバーの位置
   */
	u32 PokeParty::GetMarkingIndex(void) const
  {
    return markingIndex;
	}

  /**
   * @brief markingIndexを初期化
   */
	void PokeParty::ClearMarkingIndex(void)
	{
    //タマゴ以外の位置にmarkingIndexを設定する
		for( int i=0; i<m_memberCount; i++ ){
			if( m_member[i]->IsEgg(pokepara::CHECK_BOTH_EGG) == false ){
				markingIndex = i;
				return;
			}
		}
		// BTS[6929] スクリプトで一時的にタマゴのみになる場合があるので、アサートを外した
//		GFL_ASSERT( 0 );  //@check どうしようもない場合はMarkingは０になる

		markingIndex = 0;
	}

  /**
   * @brief   交換できる状態かどうかの判定
   * @return  bool  trueのとき、交換できる
   */
  bool PokeParty::CanTrade( void ) const
  {
    //不正タマゴがいる場合は交換できない
    if ( GetMemberCountEx( POKE_COUNT_TYPE_ONLY_ILLEGAL_EGG ) > 0 ) return false;
    u32 egg_count = GetMemberCountEx( POKE_COUNT_TYPE_BOTH_EGG );
    return m_memberCount - egg_count >= 2;
  }


  /**
   * @brief 指定ポケモンを交換にだせるかどうかの判定
   * @param[in]   交換に出すポケモンの位置
   * @return      bool  交換に出せる
   *
   * @note    交換に出した後、戦闘可能かどうかを判定しています。
   */
  bool PokeParty::CanTradeMember( u32 idx ) const
  {
    u32 battle_count = GetMemberCountEx( POKE_COUNT_TYPE_BATTLE_ENABLE, 1 << idx );
    //交換しても戦えるポケモンが手持ちにいるか？
    return battle_count > 0;
  }


  size_t PokeParty::SerializeCommon( SERIALIZE_DATA * dst, const bool isFull ) const
  {
    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      // u8 * --> void *
      void * dst_poke = reinterpret_cast<void*>
          ( dst->poke_param + pokepara::PokemonParam::DATASIZE * i );
      
      // シリアライズCore版かどうか
      if( isFull )
      {
        m_member[i]->Serialize_Full( dst_poke );
      }
      else
      {
        m_member[i]->Serialize_Core( dst_poke );
      }
        
#if PML_DEBUG
      u8 *buf = (u8*)dst_poke;
      ICHI_PRINT("dst_poke %d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
#endif // PML_DEBUG
    }
    dst->m_memberCount = m_memberCount;
    dst->markingIndex =  markingIndex;
    dst->padding[0]     = 0;  //シリアライズバイナリが不定にならないように0クリア
    dst->padding[1]     = 0;

    return SERIALIZE_DATA_SIZE;
  }

  void PokeParty::DeserializeCommon( const SERIALIZE_DATA * data, const bool isFull )
  {
    u32 count = 0;
    for(u32 i=0; i<MAX_MEMBERS; ++i)
    {
      // u8 * --> void *
      const void * src_poke = reinterpret_cast<const void*>
        ( data->poke_param + pokepara::PokemonParam::DATASIZE * i );

      // デシリアライズCore版かどうか
      if( isFull )
      {
        m_member[i]->Deserialize_Full( src_poke );
      }
      else
      {
        m_member[i]->Deserialize_Core( src_poke );
      }

#if PML_DEBUG
      u8 *buf = (u8*)src_poke;
      ICHI_PRINT("src_poke %d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4]);
#endif // PML_DEBUG
      if( !m_member[i]->IsNull() ){
        ++ count;
      }
    }
    m_memberCount = data->m_memberCount;
    ICHI_PRINT("m_memberCount %d \n", m_memberCount );
    markingIndex  = data->markingIndex;

    if( count != m_memberCount )
    {
      //明らかにセーブ破壊等の問題が起きているがPartyは生成する
      //@attention 将来的には失敗を通知する仕組みがあればよいのかも
      GFL_ASSERT( 0 );  //@fix 問題は発生しているが継続可能にしている
      m_memberCount = count;
    }
  }


#if PML_DEBUG
  /**
   * @brief パラメータのデバッグ出力
   */
  void PokeParty::Dump() const
  {
    GFL_PRINT("===============================\n");
    GFL_PRINT("== PokeParty Dump =============\n");
    GFL_PRINT("===============================\n");

    u8 member_count = this->GetMemberCount();
    GFL_PRINT("PartyCount : %d\n", m_memberCount);

    GFL_PRINT("↓ AllPokeDump \n");
    for( u8 i=0; i<member_count; i++ ) {
      GetMemberPointerConst(i)->Dump();
    }
    GFL_PRINT("===============================\n");
  }
#endif

} // namespace pml

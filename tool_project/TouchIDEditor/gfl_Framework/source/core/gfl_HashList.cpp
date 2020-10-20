//==============================================================================
/**
 * @file	ExHashList.cpp
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/29, 11:50
 */
// =============================================================================

//==============================================================================
// VoidHashList 関数実体
//==============================================================================
//------------------------------------------------------------------------------
//  SetItem
//------------------------------------------------------------------------------
void gfl::core::VoidHashList::SetItem( const void* cls, const u32 mask, const void** tbl )
{
	// 文字列からハッシュを取得
	u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
	// オフセット加算数を設定　｜１してるのは１以上にするため
	u32 add = ( ofs << 1 ) | 1;
	// for(;;) でも大丈夫だが、安全性を高めるため
	for( u32 i = 0; i <= mask; ++i ){
		ofs = ( ofs + add ) & mask;
		// テーブル[オフセット] が空か？
		if( tbl[ofs] == 0 ){
			// 空ならアイテムをセット
			tbl[ofs] = cls;
			return;
		}
	}
	gfl::core::Debug::PrintConsole( "This hashtable is broken. Could not add (%s)\n", ( *m_pGetKeyFunc )( cls ) );
}

//------------------------------------------------------------------------------
//  ResizeTable
//------------------------------------------------------------------------------
void gfl::core::VoidHashList::ResizeTable( const s32 size )
{
	// デフォルトアロケーターを使ってメモリ確保
//	const void **new_tbl = new const void* [size];
	const void **new_tbl = m_pHeap?GFL_NEW_ARRAY(m_pHeap) const void* [size]:new const void* [size];
	// ０クリア
	Utility::Memset( new_tbl, 0x00, size * sizeof(void* ) );
	// size は２のべき数のはずなのでマスクはー１すればよい
	u32 mask = size - 1;
	// 元のテーブルアイテムを新しいテーブルに再配置
	for( s32 i = 0; i < m_Capacity; ++i ){
		const void* cls = m_ppTable[i];
		// 元のテーブルアイテムが空でないか？
		if( cls ){
			// 空でないので再配置
			SetItem( cls, mask, new_tbl );
		}
	}
	// 再配置されたので元のテーブルは開放
	//	gfl::core::LibAllocator::Free(m_ppTable);
	delete [] m_ppTable;
	// 新しいテーブルポインタを設定
	m_ppTable = new_tbl;
	// 新しいテーブルサイズを設定
	m_Capacity = size;
}

// -----------------------------------------------------------------
//  Append
// -----------------------------------------------------------------
void gfl::core::VoidHashList::Append( const void* cls )
{
	s32 tmax = m_Capacity;
	// テーブルアイテムの数が増えてきて、用意された数の半数を超えたら広げる
	if( tmax < ( ++m_Counter << 1 ) ){
		// 元のサイズの２倍に広げる
		tmax = ( tmax ) ? ( tmax << 1 ) : 1;
		// 広げたテーブルに再配置する
		ResizeTable( tmax );
	}
	// 新しいアイテムを追加する
	SetItem( cls, tmax - 1, m_ppTable );
}

// -----------------------------------------------------------------
//  Remove
// -----------------------------------------------------------------
void gfl::core::VoidHashList::Remove( const void* cls )
{
	const void **tbl = m_ppTable;
	s32 tmax = m_Capacity;
	u32 ofs = Utility::CalcHash( ( *m_pGetKeyFunc )( cls ) );
	u32 add = ( ofs << 1 ) | 1;
	u32 mask = tmax - 1;
#if GFL_HIO_ENABLE
	bool flag = false;
#endif
	for( u32 i = 0; i <= mask; i++ ){
		ofs = ( ofs + add ) & mask;
		if( tbl[ofs] == cls ){
			tbl[ofs] = 0;
#if GFL_HIO_ENABLE
			flag = true;
#endif
			break;
		}
	}
#if GFL_HIO_ENABLE
	if(!flag){
		gfl::core::Debug::PrintConsole( "No such class(%s) in this hashtable.\n", ( *m_pGetKeyFunc )( cls ) );
		return;
	}
#endif
	// アイテムが減ってきたのでテーブルを縮小する
	if( ( --m_Counter << 1 ) <= tmax ){
		ResizeTable( tmax >> 1 );
	}
}

// -----------------------------------------------------------------
//  Retrieve
// -----------------------------------------------------------------
void* gfl::core::VoidHashList::Retrieve( const c8 * const name ) const
{
	if( m_ppTable ){
		u32 ofs = Utility::CalcHash( name );
		u32 add = ( ofs << 1 ) | 1;
		u32 mask = m_Capacity - 1;
		for( u32 i = 0; i <= mask; i++ ){
			ofs = ( ofs + add ) & mask;
			const void* tbl = m_ppTable[ofs];
			// ハッシュから計算されたアイテムに名前が一致するか？
			// 一致しない場合は違うオフセット値の場所に格納されているのでループさせる
			if( tbl && 0 == Utility::Strcmp( name, ( *m_pGetKeyFunc )( tbl ) ) ){
				return const_cast < void* >( tbl );
			}
		}
	}
	return 0;
}

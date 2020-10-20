/**
 * @file	gfl_HashList.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/03/29, 11:43
 */

#ifndef GFL_HASHLIST_H
#define	GFL_HASHLIST_H
#pragma once

namespace gfl {
namespace core {

/**
 *  Void型ハッシュリストクラス
 * @remark テンプレートの肥大化を防ぐため
 */

class VoidHashList {
	GFL_FORBID_COPY_AND_ASSIGN( VoidHashList );
public:
	/**
	 *   キーワードを取得する関数型
	 */
	typedef const c8* (*GET_KEY_FUNC)(const void* cls);

protected:
	gfl::heap::HeapBase* m_pHeap;
	const void** m_ppTable;
	GET_KEY_FUNC m_pGetKeyFunc;
	s32 m_Capacity;
	s32 m_Counter;

	/**
	 *   アイテムをテーブルに設定
	 * @param  cls  設定したいアイテムポインタ
	 * @param  mask ハッシュテーブルの数ー１　（２のべき数−１）
	 * @param  tbl  ハッシュテーブル
	 * @return 無し
	 */
	void SetItem( const void* cls, const u32 mask, const void** tbl );

	/**
	 *   テーブルのサイズ変更
	 * @param  max  確保したいアイテムの数
	 * @return 無し
	 */
	void ResizeTable( const s32 size );

public:

	/**
	 *   コンストラクタ
	 * @param  func  クラス T 内のキーワードを取得する関数
	 * @param  size  テーブルサイズ　２のべき数
	 * @return 無し
	 */

	VoidHashList( GET_KEY_FUNC func = 0, const s32 size = 0, gfl::heap::HeapBase* heap = 0 ) : m_pHeap(heap), m_ppTable( 0 ), m_pGetKeyFunc( func ), m_Capacity( 0 ), m_Counter( 0 ) {
		if( size ){
			ResizeTable( size );
		}
	}

	/**
	 *   デストラクタ
	 * @param  無し
	 * @return 無し
	 * @remark テーブルメモリを開放しない
	 */

	~VoidHashList( void ) {
		clear( );
	}

	/**
	 *   キーワードを取得する関数を設定
	 * @param  func  クラス T 内のキーワードを取得する関数
	 * @return 無し
	 */

	void SetKeyFunction( GET_KEY_FUNC func ) {
		m_pGetKeyFunc = func;
	}

	/**
	 *   テーブルにクラスを追加する
	 * @param  cls 追加したいvoid型ポインタ
	 * @return 無し
	 */
	void Append( const void* cls );

	/**
	 *   テーブルからクラスを削除
	 * @param  cls 削除したいvoid型ポインタ
	 * @return 無し
	 * @remark マッチするクラスがない場合はアサートで停止する
	 */
	void Remove( const void* cls );

	/**
	 *   文字列からクラスポインタを取得
	 * @param  name クラス名
	 * @return void型ポインタ
	 * @remark マッチするクラスがない場合は０を返す
	 */
	void* Retrieve( const c8 * const name ) const;

	/**
	 *   テーブルをクリア
	 * @param  無し
	 * @return 無し
	 */

	void clear( void ) {
		delete [] m_ppTable;
		m_ppTable = 0;
		m_Capacity = m_Counter = 0;
	}

	/**
	 *   テーブルの各クラスをクリア
	 * @param  無し
	 * @return 無し
	 */

	template<typename T>
	void clear_and_delete( void ) {
		const void** tbl = m_ppTable;
		for( s32 i = 0; i < m_Capacity; i++ ){
			GFL_DELETE static_cast<const T*> (*tbl++);
		}
		clear( );
	}

	/**
	 *   テーブルを取得
	 * @param  無し
	 * @return テーブルポインタ
	 */

	const void** GetTable( void ) {
		return m_ppTable;
	}

	const void** GetTable( void ) const {
		return m_ppTable;
	}

	/**
	 *   テーブルの容量を取得
	 * @param  無し
	 * @return テーブルの容量　入っている数ではなく許容量
	 */

	s32 GetCapacity( void ) const {
		return m_Capacity;
	}

	/**
	 *   テーブルのサイズを取得
	 * @param  無し
	 * @return テーブルのサイズ 入っている数
	 */

	s32 GetCounter( void ) const {
		return m_Counter;
	}

	/**
	 *   クラステーブルの先頭アドレスを取得
	 * @param  無し
	 * @return クラステーブルの先頭アドレス
	 */

	inline const void** begin( void ) const {
		return m_ppTable;
	}

	/**
	 *   クラステーブルの終端アドレスを取得
	 * @param  無し
	 * @return クラステーブルの終端アドレス
	 */

	inline const void** end( void ) const {
		return &m_ppTable[m_Capacity];
	}

    void SetHeap( gfl::heap::HeapBase* Heap ) {
    	this->m_pHeap = Heap;
    }

}; // class VoidHashList

/**
 *   ハッシュリストクラス
 */

template<typename T>
class HashList : public VoidHashList {
	GFL_FORBID_COPY_AND_ASSIGN( HashList );
public:

	/**
	 *   デフォルトコンストラクタ
	 * @param  無し
	 * @return 無し
	 */

	HashList( void ) {
	}

	/**
	 *   コンストラクタ
	 * @param  func  クラス T 内のキーワードを取得する関数
	 * @return 無し
	 * @remark インスタンスの増大を防ぐため static な型の関数を渡す必要がある。
	 */

	HashList( GET_KEY_FUNC func, const s32 size = 0, gfl::heap::HeapBase* heap=0 ) : VoidHashList( func, size, heap ) {
	}

	/**
	 *   デストラクタ
	 * @param  無し
	 * @return 無し
	 * @remark テーブルメモリを開放
	 */

	~HashList( void ) {
	}

	/**
	 *   テーブルにクラスを追加する
	 * @param  cls 追加したいクラスの参照
	 * @return 無し
	 */

	inline void Append( const T& cls ) {
		VoidHashList::Append( static_cast<const void*> (&cls) );
	}

	/**
	 *   テーブルにクラスを追加する
	 * @param  cls 追加したいクラスのポインタ
	 * @return 無し
	 */

	inline void Append( T* cls ) {
		VoidHashList::Append( static_cast<const void*> (cls) );
	}

	/**
	 *   テーブルからクラスを削除
	 * @param  cls 削除したいクラスの参照
	 * @return 無し
	 * @remark マッチするクラスがない場合はアサートで停止する
	 */

	void Remove( const T& cls ) {
		VoidHashList::Remove( static_cast<const void*> (&cls) );
	}

	/**
	 *   文字列からクラスポインタを取得
	 * @param  name クラス名
	 * @return クラスポインタ
	 * @remark マッチするクラスがない場合は０を返す
	 */

	T* Retrieve( const c8 * const name ) const {
		return static_cast<T*> (VoidHashList::Retrieve( name ));
	}
}; // class HashList

}
}

#endif	/* GFL_HASHLIST_H */


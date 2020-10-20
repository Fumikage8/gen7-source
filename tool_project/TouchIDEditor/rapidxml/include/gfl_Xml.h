//==============================================================================
/**
 * @file	gfl_Xml.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/04/28, 12:59
 */
// =============================================================================

#ifndef GFL_XML_H
#define	GFL_XML_H

#include <gflib.h>
#include "rapidxml.hpp" // no need to link
#include "rapidxml_print.hpp"
#ifdef GFL_COMPILER_VC
//#define new  ::new(_NORMAL_BLOCK, __FILE__, __LINE__) // rapidxml でエラーが出るのでここでやる…
#endif

#ifdef QT_VERSION
#include <string>
#endif

using namespace rapidxml;

namespace gfl {
namespace xml {

/**
 * 	 * ノードクラス
 */
class Node : public xml_node<> {
public:

	/**
	 * 子供のノードを名前で検索
	 * @param val ノード名
	 * @return ノードポインタ
	 */
	Node* FirstNode( c8* val = 0 ) const {
		if( this ){
			return static_cast<Node*> (first_node( val ));
		}
		return 0;
	}

	/**
	 * 兄弟のノードを取得
	 * @name ノード名
	 * @return ノードポインタ
	 */
	Node* NextSibling( c8* name = 0 ) const {
		if( this ){
			return static_cast<Node*> (next_sibling( name ));
		}
		return 0;
	}

	/**
	 * ノードの作成
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* NewNode( c8* val ) const {
		return static_cast<Node*> (document( )->allocate_node( node_element, document( )->allocate_string( val ) ));
	}

	/**
	 * ノードのコピー
	 * @param new_node コピー先ノード
	 * @param org_node　コピー元ノード
	 */
	static void CopyNode( Node* new_node, const Node& org_node ) {
		new_node->remove_all_attributes( );
		// 属性をコピー
		for( xml_attribute<>* attr = org_node.first_attribute( ); attr; attr = attr->next_attribute( ) ){
			new_node->append_attribute( org_node.NewAttribute( attr ) );
		}

		new_node->remove_all_nodes( );
		// 子ノードを作成
		for( Node* c_node = org_node.FirstNode( ); c_node; c_node = c_node->NextSibling( ) ){
			Node* new_c_node = org_node.NewNode( c_node->name( ) );
			CopyNode( new_c_node, *c_node );
			new_node->append_node( new_c_node );
		}
	}

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* AddNode( c8* val ) {
		Node* node = NewNode( val );
		append_node( node );
		return node;
	}

	/**
	 * ノードの追加
	 * @param where インサート場所
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* InsertNode( gfl::xml::Node* where, c8* val ) {
		Node* node = NewNode( val );
		insert_node( where, node );
		return node;
	}

	/**
	 * 属性の作成
	 * @param key　属性名
	 * @param val　属性値
	 */
	xml_attribute<>* NewAttribute( c8* key, c8* val ) const {
		return document( )->allocate_attribute( document( )->allocate_string( key ), document( )->allocate_string( val ) );
	}

	xml_attribute<>* NewAttribute( c8* key, const c8* val ) const {
		return document( )->allocate_attribute( document( )->allocate_string( key ), val );
	}

	xml_attribute<>* NewAttribute( xml_attribute<>* attr ) const {
		return NewAttribute( attr->name( ), attr->value( ) );
	}

	/**
	 * 属性の追加
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	void AddAttribute( c8* key, c8* val ) {
		append_attribute( NewAttribute( key, val ) );
	}

	void AddAttribute( c8* key, const c8* val ) {
		append_attribute( NewAttribute( key, val ) );
	}

	void InsertAttribute( xml_attribute<>* where, c8* key, c8* val ) {
		insert_attribute( where, NewAttribute( key, val ) );
	}

	/**
	 * 属性の追加
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	void AddAttribute( c8* key, const f32 val ) {
		c8 buf[0x10]; // 0x10 is enough for float
		sprintf( buf, "%f", val );
		AddAttribute( key, buf );
	}

	void AddAttribute( c8* key, const f64 val ) {
		c8 buf[0x10]; // 0x10 is enough for float
		sprintf( buf, "%f", val );
		AddAttribute( key, buf );
	}

	/**
	 * 属性の追加
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	template<typename T>
	void AddAttribute( c8* key, const T& val ) {
		c8 buf[0x10]; // 0x10 is enough for float
		sprintf( buf, "%d", val ); // no itoa ...
		AddAttribute( key, buf );
	}

	/**
	 * ノード名の設定
	 * @param val　名
	 * @remark 名前をあとから変える場合
	 */
	void SetName( c8* val ) {
		name( document( )->allocate_string( val ) );
	}

	/**
	 * ノード名の設定
	 * @param val　名
	 * @remark 名前をあとから変える場合
	 */
	void SetName( const QString& val ) {
		SetName( val.toAscii( ).data( ) );
	}

	/**
	 * ノード値の設定
	 * @param val　値
	 * @remark 値をあとから変える場合
	 */
	void SetValue( c8* val, size_t size = 0 ) {
		if( !value( ) ){
			value( document( )->allocate_string( val, size ) );
		} else { // こうしないと駄目?
			Node* p_node = (gfl::xml::Node*)parent( );
			Node* new_node = p_node->InsertNode( this, name( ) );
			for( xml_attribute<> *attribute = first_attribute( ); attribute; attribute = attribute->next_attribute( ) ){
				new_node->AddAttribute( attribute->name( ), attribute->value( ) );
			}
			new_node->value( document( )->allocate_string( val, size ) );
			p_node->remove_node( this );
		}
	}

	/**
	 * ノード値の設定
	 * @param val　値
	 * @remark 値をあとから変える場合
	 */
	void SetValue( const QString& val ) {
		SetValue( val.toAscii( ).data( ) );
	}

	/**
	 * 属性の設定
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があれば上書き
	 */
	void SetAttribute( c8* key, c8* val ) {
		xml_attribute<>* label = first_attribute( key );
		if( label ){ // 存在する場合は置換
			InsertAttribute( label, key, val );
			remove_attribute( label );
		} else {
			AddAttribute( key, val );
		}
	}

	void SetAttribute( c8* key, const c8* val ) {
		SetAttribute( key, const_cast<c8*> (val) );
	}

	/**
	 * 属性の設定
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	void SetAttribute( c8* key, const f32 val ) {
		c8 buf[0x10]; // 0x10 is enough for float
		sprintf( buf, "%f", val );
		SetAttribute( key, buf );
	}

	/**
	 * 属性の設定
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	template<typename T>
	void SetAttribute( c8* key, const T& val ) {
		c8 buf[0x10]; // 0x10 is enough for integer
		sprintf( buf, "%d", val );
		SetAttribute( key, buf );
	}

	/**
	 * 文字列属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 属性文字列ポインタ
	 */
	c8* AttributeString( c8* name, c8* default_value = "" ) const {
		xml_attribute<>* label = first_attribute( name );
		return ( label) ? label->value( ) : default_value;
	}

	/**
	 * 整数属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 整数属性
	 */
	s32 AttributeInteger( c8* name, const s32 default_value = 0 ) const {
		xml_attribute<>* label = first_attribute( name );
		return ( label) ? atoi( label->value( ) ) : default_value;
	}

	/**
	 * 浮動小数属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 属性浮動小数
	 */
	f64 AttributeFloat( c8* name, const f32 default_value = 0.0f ) const {
		xml_attribute<>* label = first_attribute( name );
		return ( label) ? atof( label->value( ) ) : default_value;
	}

	/**
	 * bool 属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 整数属性
	 */
	bool AttributeBool( c8* name, const bool default_value = false ) const {
		xml_attribute<>* label = first_attribute( name );
		return ( label) ? ((0 == strcmp( "true", label->value( ) )) ? true : false) : default_value;
	}

#ifdef QT_VERSION

	/**
	 * 子供のノードを名前で検索
	 * @param val ノード名
	 * @return ノードポインタ
	 */
	Node* FirstNode( const QString& val ) const {
		return FirstNode( val.toAscii( ).data( ) );
	}

	/**
	 * 兄弟のノードを取得
	 * @name ノード名
	 * @return ノードポインタ
	 */
	Node* NextSibling( const QString& name ) const {
		return NextSibling( name.toAscii( ).data( ) );
	}

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* AddNode( const QString& val ) {
		return AddNode( (c8*) val.toAscii( ).data( ) );
	}

	/**
	 * 属性の追加
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	void AddAttribute( c8* key, const QString& val ) {
		AddAttribute( key, (c8*) val.toAscii( ).data( ) );
	}

	/**
	 * 属性の追加
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があっても追加
	 */
	template<typename T>
	void AddAttribute( const QString& key, const T& val ) {
		AddAttribute( document( )->allocate_string( key.toAscii( ).data( ) ), val );
	}

	/**
	 * 属性の設定
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があれば上書き
	 */
	void SetAttribute( c8* key, const QString& val ) {
		SetAttribute( key, val.toAscii( ).data( ) );
	}

	/**
	 * 属性の設定
	 * @param key　キー名
	 * @param val　値
	 * @remark 同じ属性名があれば上書き
	 */
	template<typename T>
	void SetAttribute( const QString& key, const T& val ) {
		SetAttribute( key.toAscii( ).data( ), val );
	}

	/**
	 * 文字列属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 属性文字列ポインタ
	 */
	c8* AttributeString( const QString& name, c8* default_value = "" ) const {
		return AttributeString( name.toAscii( ).data( ), default_value );
	}

	/**
	 * 整数属性の取得
	 * @param node　取得したいノード
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 整数属性
	 */
	s32 AttributeInteger( const QString& name, const s32 default_value = 0 ) const {
		return AttributeInteger( name.toAscii( ).data( ), default_value );
	}

	/**
	 * 浮動小数属性の取得
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 属性浮動小数
	 */
	f64 AttributeFloat( const QString& name, const f32 default_value = 0.0f ) const {
		return AttributeFloat( name.toAscii( ).data( ), default_value );
	}

	/**
	 * boolの取得
	 * @param node　取得したいノード
	 * @param name　属性名
	 * @param default_value　属性が無かった場合の値
	 * @return 整数属性
	 */
	bool AttributeBool( const QString& name, const bool default_value = false ) const {
		return AttributeBool( name.toAscii( ).data( ), default_value );
	}

#endif

};

/**
 * ドキュメント
 */
class Document : public xml_document<> {
private:
	void* m_pData;

public:
	//	Document(void) : m_pData( 0 ) {}

	explicit Document( c8* root_name = "root" ) : m_pData( 0 ) {
		SetRootNode( root_name );
	}

	~Document( void ) {
		delete [] m_pData;
	}

	bool CheckConflict( c8* data ){
		return QString(data).contains("<<<<<<< .mine");
	}

	bool Parse( c8* data, bool check_conflict = false ) {
		delete [] m_pData;
		clear( );

		if( check_conflict ){
			if(CheckConflict( data )){
				return false;
			}
		}

		m_pData = (void*) data;
		parse < parse_no_utf8 | parse_no_entity_translation > ((c8*) data);
		return true;
	}

	void ParseUtf8( c8* data ) {
		delete [] m_pData;
		clear( );
		m_pData = (void*) data;
		parse < parse_no_entity_translation > ((c8*) data);
	}

	/**
	 * XMLファイルのロード
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 * @remark リロード時には自動的にクリアされます
	 */
	bool Load( c8* fname, gfl::heap::HeapBase* heap = 0, size_t* read_size = 0, bool check_conflict = false ) {
		c8* data = reinterpret_cast<c8*> (gfl::core::File::Load( fname, heap, read_size, gfl::core::File::MODE_TEXT ));
		if( data ){
			return Parse( data, check_conflict );
		}
		return false;
	}

	/**
	 * XMLファイルのロード UTF8版
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 * @remark Qt で使う場合は　
	 * QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
	 * が必要
	 * リロード時には自動的にクリアされます
	 */
	bool LoadUtf8( c8* fname, gfl::heap::HeapBase* heap = 0, size_t* read_size = 0 ) {
		c8* data = reinterpret_cast<c8*> (gfl::core::File::Load( fname, heap, read_size, gfl::core::File::MODE_TEXT ));
		if( data ){
			ParseUtf8( data );
			return true;
		}
		return false;
	}

#ifdef QT_VERSION

	/**
	 * XMLファイルのセーブ
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 */
	bool Save( const QString& fname, const c8 * const head = "<?xml version=\"1.0\" encoding=\"shift-jis\"?>\n" ) const {
		QFile file( fname );
		if( file.open( QIODevice::WriteOnly | QIODevice::Text ) ){
			::std::string out( head );
			print( ::std::back_inserter( out ), *this, 0 );
			file.write( out.data( ), out.size( ) );
			file.close( );
			return true;
		}
		return false;
	}

	/**
	 * XMLファイルのロード
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 */
	bool Load( const QString& fname, bool check_conflict = false ) {
		if( !fname.isNull( ) ){
			return Load( fname.toAscii( ).data( ), 0, 0, check_conflict );
		}
		return false;
	}

	/**
	 * XMLファイルのロード
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 */
	bool LoadUtf8( const QString& fname ) {
		if( !fname.isNull( ) ){
			return LoadUtf8( fname.toAscii( ).data( ) );
		}
		return false;
	}
#endif

#if 0 // 

	/**
	 * 子供のノードを取得
	 * @return ノードポインタ
	 */
	Node* FirstNode( void ) const {
		return static_cast<Node*> (first_node( ));
	}

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* AddNode( c8* val ) {
		remove_all_nodes( ); // ここが普通のノードと違う　ルートはひとつのみしか許可されない！
		xml_node<>* node = allocate_node( node_element, val );
		append_node( node );
		return static_cast<Node*> (node);
	}

#ifdef QT_VERSION

	/**
	 * 子供のノードを取得
	 * @return ノードポインタ
	 */
	Node* FirstNode( const QString& name ) const {
		return FirstNode( name.toAscii( ).data( ) );
	}

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* AddNode( const QString& val ) {
		return AddNode( allocate_string( val.toAscii( ).data( ) ) );
	}

	//	QByteArray* GetData( ) {
	//		return &_Data;
	//	}
#endif
#else

	Node* RootNode( void ) const {
		return static_cast<Node*> (first_node( ));
	}

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* SetRootNode( c8* val ) {
		remove_all_nodes( ); // ここが普通のノードと違う　ルートはひとつのみしか許可されない！
		xml_node<>* node = allocate_node( node_element, val );
		append_node( node );
		return static_cast<Node*> (node);
	}
#ifdef QT_VERSION

	/**
	 * ノードの追加
	 * @param val　ノード名
	 * @return 作成されたノードポインタ
	 */
	Node* SetRootNode( const QString& val ) {
		return SetRootNode( allocate_string( val.toAscii( ).data( ) ) );
	}
#endif
#endif
};

}
}

#endif	/* GFL_XML_H */


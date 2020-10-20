#pragma once
//=================================================================================================
//
//  utilXml.h
//
//  rapidxml(オープンソース)ライブラリを使用するためのラッパークラス
//
//=================================================================================================
#include "rapidxml.hpp" // no need to link
#include "rapidxml_print.hpp"

using namespace rapidxml;
#if 0
//-------------------------------------------------------------------------------------------------
//  型定義  ※いずれcommonへ
//-------------------------------------------------------------------------------------------------
typedef unsigned char       u8;   // 符号無８ビット変数
typedef unsigned short      u16;  // 符号無16ビット変数
typedef unsigned int        u32;  // 符号無32ビット変数
typedef unsigned long long  u64;  // 符号無64ビット変数

typedef signed char         s8;   // 符号付８ビット変数
typedef signed short        s16;  // 符号付16ビット変数
typedef signed int          s32;  // 符号付32ビット変数
typedef signed long long    s64;  // 符号付64ビット変数

typedef float               f32;  // 短精度浮動少数点変数
typedef double              f64;  // 倍精度浮動少数点変数

typedef signed int          b32;  // 32ビットのブール

typedef unsigned short      f16;  // 16ビット浮動少数点変数
typedef char                c8;   // 8ビットの文字変数
#endif

//=================================================================================================
//  名前空間定義
//=================================================================================================
namespace utl { namespace xml {

//=================================================================================================
//
//
// ノードクラス
//
//
//=================================================================================================
class Node : public xml_node<> {
public:

//-------------------------------------------------------------------------------------------------
// 子供のノードを名前で検索
//	IN:   name  ノード名
//  OUT:        ノードポインタ
//-------------------------------------------------------------------------------------------------
	Node* FirstNode(c8* nameStr = 0) const
  {
		if(this){ return static_cast<Node*>(first_node(nameStr)); }
		return 0;
	}

//-------------------------------------------------------------------------------------------------
// 兄弟のノードを名前で検索
//	IN:   name  ノード名
//  OUT:        ノードポインタ
//-------------------------------------------------------------------------------------------------
	Node* NextSibling(c8* nameStr = 0) const
  {
		if(this){ return static_cast<Node*>(next_sibling(nameStr)); }
		return 0;
	}

//-------------------------------------------------------------------------------------------------
// 新規ノードの作成
//	IN:   name  ノード名
//  OUT:        作成されたノードポインタ
//-------------------------------------------------------------------------------------------------
	Node* NewNode(c8* nameStr) const
  {
		return static_cast<Node*>
            (document()->allocate_node(node_element, document()->allocate_string(nameStr)));
	}

//-------------------------------------------------------------------------------------------------
// ノードのコピー
//	IN:   dst_node  コピー先ノード
//        src_node　コピー元ノード
//-------------------------------------------------------------------------------------------------
	static void CopyNode(Node* dst_node, const Node& src_node)
	{
		dst_node->remove_all_attributes();
		// 属性をコピー
		for(xml_attribute<>* attr = src_node.first_attribute(); attr; attr = attr->next_attribute()){
			dst_node->append_attribute(src_node.NewAttribute(attr));
		}
		
		dst_node->remove_all_nodes();
		// 子ノードを作成
		for(Node* c_node = src_node.FirstNode(); c_node; c_node = c_node->NextSibling()){
			Node* new_c_node = src_node.NewNode(c_node->name());
			CopyNode(new_c_node, *c_node);
			dst_node->append_node( new_c_node );
		}
	}

//-------------------------------------------------------------------------------------------------
// ノードの追加
//	IN:   name  ノード名
//  OUT:        作成されたノードポインタ
//-------------------------------------------------------------------------------------------------
	Node* AddNode(c8* nameStr)
  {
		Node* node = NewNode(nameStr);
		append_node( node );
		return node;
	}

//-------------------------------------------------------------------------------------------------
// ノードの追加挿入
//	IN:   posNode インサート場所を示すノード
//	      name    ノード名
//  OUT:          作成されたノードポインタ
//-------------------------------------------------------------------------------------------------
	Node* InsertNode(utl::xml::Node* posNode, c8* name)
  {
		Node* node = NewNode(name);
		insert_node(posNode, node);
		return node;
	}

//-------------------------------------------------------------------------------------------------
// 属性(Attribute)の作成
//	IN:   key   属性名
//	      val   属性値
//  OUT:        作成された属性ポインタ
//-------------------------------------------------------------------------------------------------
	xml_attribute<>* NewAttribute(c8* key, c8* val) const
  {
		return document()->allocate_attribute(document()->allocate_string(key),
                                          document()->allocate_string(val));
	}

	xml_attribute<>* NewAttribute(c8* key, const c8* val) const
  {
		return document()->allocate_attribute(document( )->allocate_string(key),
                                          val);
	}

	xml_attribute<>* NewAttribute(xml_attribute<>* attr) const
  {
		return NewAttribute(attr->name(), attr->value());
	}

//-------------------------------------------------------------------------------------------------
// 属性(Attribute)の追加
//	IN:   key   属性名
//	      val   属性値
//  ※ 同じ属性名があっても追加
//-------------------------------------------------------------------------------------------------
// 文字列
  void AddAttribute(c8* key, c8* val)
  { append_attribute(NewAttribute(key,val)); }

	void AddAttribute(c8* key, const c8* val)
  { append_attribute(NewAttribute(key,val)); }
	
// 浮動小数値
	void AddAttribute(c8* key, const f32 val)
  { c8 buf[0x10]; sprintf_s(buf, 0x10, "%f", val); AddAttribute(key, buf); }

	void AddAttribute(c8* key, const f64 val)
  { c8 buf[0x10]; sprintf_s(buf, 0x10, "%f", val); AddAttribute(key, buf); }

// 整数値
	template<typename T>
	void AddAttribute(c8* key, const T& val )
  { c8 buf[0x10]; sprintf_s(buf, 0x10, "%d", val); AddAttribute(key, buf); }

//-------------------------------------------------------------------------------------------------
// 属性(Attribute)の追加挿入
//	IN:   posAttr インサート場所を示す属性
//	      key     属性名
//	      val     属性値
//-------------------------------------------------------------------------------------------------
  void InsertAttribute(xml_attribute<>* posAttr, c8* key, c8* val)
  { insert_attribute(posAttr, NewAttribute(key,val)); }

//-------------------------------------------------------------------------------------------------
// ノード名の設定 ※変更用
//	IN:   name  ノード名
//-------------------------------------------------------------------------------------------------
	void SetName(c8* newName) { name(document()->allocate_string(newName)); }

//-------------------------------------------------------------------------------------------------
// ノード値の設定
//	IN:   val   値を表す文字列
//	      size  値を表す文字列のサイズ
//-------------------------------------------------------------------------------------------------
	void SetValue(c8* val, size_t size = 0)
  {
		if(!value()){
      // 未設定の場合は新規設定
			value(document( )->allocate_string(val,size));
		}else{
      // 更新(このノードのコピーを作り、値の設定をやりなおす)
			Node* p_node = (utl::xml::Node*)parent();
			Node* new_node = p_node->InsertNode(this,name());
			for(xml_attribute<> *attr = first_attribute(); attr; attr = attr->next_attribute()){
				new_node->AddAttribute( attr->name(), attr->value() );
			}
			new_node->value(document()->allocate_string(val, size));
			p_node->remove_node(this);
		}
	}

//-------------------------------------------------------------------------------------------------
// 属性値の設定　※指定属性名がない場合は新規作成
//	IN:   key 属性名
//	      val 値
//-------------------------------------------------------------------------------------------------
	void SetAttribute(c8* key, c8* val)
  {
		xml_attribute<>* label = first_attribute( key );
		if(label){ // 既存の場合は更新処理
			InsertAttribute(label, key, val);
			remove_attribute(label);
		}else{
			AddAttribute(key, val);
		}
	}

	void SetAttribute(c8* key,const c8* val)
  { SetAttribute(key, const_cast<c8*>(val)); }

	void SetAttribute(c8* key, const f32 val)
  { c8 buf[0x10]; sprintf_s(buf, 0x10, "%f", val); SetAttribute(key, buf); }

	template<typename T>
	void SetAttribute(c8* key, const T& val)
  { c8 buf[0x10]; sprintf_s(buf, 0x10, "%d", val); SetAttribute(key, buf); }

//-------------------------------------------------------------------------------------------------
// 属性値の取得(文字列)
//	IN:   key           属性名
//	      default_value 属性が無かった場合の値
//  OUT:  文字列ポインタ
//-------------------------------------------------------------------------------------------------
	c8* AttributeString(c8* key, c8* default_value = "") const
  {
		xml_attribute<>* label = first_attribute(key);
		return (label)? label->value() : default_value;
	}

//-------------------------------------------------------------------------------------------------
// 属性値の取得(整数値)
//	IN:   key           属性名
//	      default_value 属性が無かった場合の値
//  OUT:  整数値
//-------------------------------------------------------------------------------------------------
	s32 AttributeInteger(c8* key, const s32 default_value = 0) const
  {
		xml_attribute<>* label = first_attribute(key);
		return (label)? atoi(label->value()) : default_value;
	}

//-------------------------------------------------------------------------------------------------
// 属性値の取得(浮動小数値)
//	IN:   key           属性名
//	      default_value 属性が無かった場合の値
//  OUT:  浮動小数値
//-------------------------------------------------------------------------------------------------
	f64 AttributeFloat(c8* key, const f32 default_value = 0.0f) const
  {
		xml_attribute<>* label = first_attribute(key);
		return (label)? atof(label->value()) : default_value;
	}

//-------------------------------------------------------------------------------------------------
// 属性値の取得(bool値)
//	IN:   key           属性名
//	      default_value 属性が無かった場合の値
//  OUT:  bool値
//-------------------------------------------------------------------------------------------------
	bool AttributeBool(c8* key, const bool default_value = false) const
  {
		xml_attribute<>* label = first_attribute(key);
    if(label){
      return (0 == strcmp("true", label->value()))? true : false;
    }
    return default_value;
	}
};


//=================================================================================================
//
//
// ドキュメントクラス
//
//
//=================================================================================================
class Document : public xml_document<> {

private:
	void* m_pData;

public:
	explicit Document(c8* root_name="root") : m_pData(0) { SetRootNode(root_name); }

  ~Document(void){ Cleanup(); }

  void Cleanup(void)
  {
    if(m_pData){ delete[] m_pData; }
    m_pData = NULL;
  }

  Node* RootNode(void) const
  {
		return static_cast<Node*> (first_node());
	}

  Node* SetRootNode(c8* val)
  {
		remove_all_nodes(); // ルートは一つのみしか許可されない
		xml_node<>* node = allocate_node( node_element, val );
		append_node( node );
		return static_cast<Node*> (node);
	}

  void Parse(c8* data)
  {
		delete [] m_pData;
		clear();
		//m_pData = (void*)data;
		parse < parse_no_utf8 | parse_no_entity_translation > ((c8*)data);
	}

  void ParseUtf8(c8* data)
  {
		delete [] m_pData;
		clear();
		//m_pData = (void*)data;
		parse < parse_no_entity_translation > ((c8*)data);
	}


#if 0
	/**
	 * XMLファイルのロード
	 * @param fname　ファイル名
	 * @return true 成功 false 失敗
	 * @remark リロード時には自動的にクリアされます
	 */
	bool Load( c8* fname, gfl::heap::HeapBase* heap=0, size_t* read_size = 0 ) {
		c8* data = reinterpret_cast<c8*>(gfl::core::File::Load( fname,heap,read_size,gfl::core::File::MODE_TEXT ));
		if( data ){
			Parse( data );
			return true;
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
	bool LoadUtf8( c8* fname, gfl::heap::HeapBase* heap=0, size_t* read_size = 0 ) {
		c8* data = reinterpret_cast<c8*>(gfl::core::File::Load( fname,heap,read_size,gfl::core::File::MODE_TEXT ));
		if( data ){
			ParseUtf8( data );
			return true;
		}
		return false;
	}
#endif

#if 0
private:
  c8* load(c8* data)
  {
    char* srcBuffer;
    {
      // ファイルオープン
	    HANDLE file = CreateFileW( XMLfilename, 
														    GENERIC_READ,
														    FILE_SHARE_READ,
														    NULL,
														    OPEN_EXISTING,
														    FILE_ATTRIBUTE_READONLY,
														    NULL);
	    if(file == INVALID_HANDLE_VALUE){ return NULL; }

	    // 読み込み用バッファ作成
	    DWORD fileSize = GetFileSize(file, NULL);
	    srcBuffer = new char[fileSize + 1];
	    srcBuffer[fileSize] = 0x00; // 終端コード

	    // 読み込み
      DWORD readSize;
	    ReadFile(file, srcBuffer, fileSize, &readSize, NULL);
	    CloseHandle(file);
	    if(readSize != fileSize){
        delete[] srcBuffer;
        return false;
      }
    }
  }
#endif
};

//=================================================================================================
} }



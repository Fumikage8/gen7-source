/**
* @file    NameManglingManager.h
* @brief   名前修飾を管理クラス
* @author  ikuta_junya
* @date    2014.10.09
*/

#if !defined( FIELDDEBUGNAMEMANGLINGMANAGER_H_INCLUDED )
#define FIELDDEBUGNAMEMANGLINGMANAGER_H_INCLUDED
#pragma once

// @note skeltonから移植、stdやxmlパーサーを使うのでwin32専用
#if defined(GF_PLATFORM_WIN32)

#include <heap/include/gfl2_heap.h>
#include <string>

//#include <iostream>
#include <sstream>

// @note 以下のテンプレート化できる


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );
GFL_NAMESPACE_BEGIN( NameMangling );

class NameManglingNodeBase;
class GroupNode;

//! @brief 値コールバックインターフェース
class INameManglingNodeCallbackable
{
public:

  /**
   * @brief コンストラクタ
   */
  INameManglingNodeCallbackable() {}

  /**
   * @brief デストラクタ
   */
  virtual ~INameManglingNodeCallbackable() {}

  /**
   * @brief 値設定後のコールバック関数
   */
  virtual void Callback( NameManglingNodeBase* pNameManglingNodeBase, b32 isChange ) = 0;
};

//! @brief グループノードコールバックインターフェース
class IGroupNodeCallbackable
{
public:

  /**
   * @brief コンストラクタ
   */
  IGroupNodeCallbackable() {}

  /**
   * @brief デストラクタ
   */
  virtual ~IGroupNodeCallbackable() {}

  /**
   * @brief 値設定後のコールバック関数
   */
  virtual void Callback( GroupNode* pGroupNode, b32 isChange ) = 0;
};



enum STRUCT_TYPE
{
  STRUCT_TYPE_F32,
  STRUCT_TYPE_S32,
  STRUCT_TYPE_U32,
  STRUCT_TYPE_U16,

  TYPE_MAX
};

class NameManglingNodeBase
{
public:

  NameManglingNodeBase( void ) : m_pPrevNode( NULL), m_pNextNode( NULL ), m_pCallBackInterFace( NULL )
  {
  }

  virtual ~NameManglingNodeBase( void )
  {
  }

  void PushBuck( NameManglingNodeBase* pNode )
  {
    NameManglingNodeBase* pEndNode = GetListEnd();

    pEndNode->m_pNextNode = pNode;
    pNode->m_pPrevNode = pEndNode;
  }

  virtual b32 SetValue( const char* pValue ) = 0;
  virtual STRUCT_TYPE GetInstanceType( void ) const = 0;

  void SetCallBack( INameManglingNodeCallbackable* pCallBack )
  {
    m_pCallBackInterFace = pCallBack;
  }
  
  INameManglingNodeCallbackable* GetCallBack( void )
  {
    return m_pCallBackInterFace;
  }


  // 自身の情報
  std::string           m_Name;

  // リスト構造
  NameManglingNodeBase* m_pPrevNode;
  NameManglingNodeBase* m_pNextNode;

private:
  NameManglingNodeBase* GetListEnd( void )
  {
    NameManglingNodeBase* pNowNode( this );

    while( pNowNode->m_pNextNode )
    {
      pNowNode = pNowNode->m_pNextNode;
    }

    return pNowNode;
  }

  INameManglingNodeCallbackable* m_pCallBackInterFace;

};

// ハッシュの代わり
class NameManglingEnumNode : public NameManglingNodeBase
{
public:

  NameManglingEnumNode( void ) : m_Value( 0 )
  {
  }

  ~NameManglingEnumNode( void )
  {
  }

  // 名前修飾を設定
  void SetNameMangling( const char* pName, s32 value )
  {
    m_Value = value;
    m_Name = std::string( pName );
  }

  // 値を設定
  void SetValue( s32 value ) 
  {
    m_Value = value;
  }

  // 値を取得
  s32 GetValue( void ) 
  {
    return m_Value;
  }

  // 値をテキストフォーマットで設定
  b32 SetValue( const char* pValue )
  {
    s32 work;
    if( sscanf_s( pValue, "%d", &work ) == 1 )
    {
      if( m_Value != work )
      {
        m_Value = work;
        return true;
      }
    }

    return false;
  }

  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_S32;
  }

private:
  s32    m_Value;
};

class NameManglingS32Node : public NameManglingNodeBase
{
public:

  NameManglingS32Node( void );
  ~NameManglingS32Node( void );

  // 名前修飾を設定
  void SetNameMangling( s32* pInstance, const char* pName );

  // 値を設定
  void SetValue( s32 value );

  // 値をテキストフォーマットで設定
  b32 SetValue( const char* pValue );
  
  // ハッシュ値を追加
  b32 AddHash( const char* pValue, s32 value );
 
  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_S32;
  }

private:
  s32*       m_pInstance;
  GroupNode* m_pHash;
};

class NameManglingU32Node : public NameManglingNodeBase
{
public:

  NameManglingU32Node( void );
  ~NameManglingU32Node( void );

  void SetNameMangling( u32* pInstance, const char* pName );
  void SetValue( u32 value );
  b32 SetValue( const char* pValue );
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_U32;
  }

private:
  u32*       m_pInstance;
};

class NameManglingU16Node : public NameManglingNodeBase
{
public:

  NameManglingU16Node( void );
  ~NameManglingU16Node( void );

  void SetNameMangling( u16* pInstance, const char* pName );
  void SetValue( u16 value );
  b32 SetValue( const char* pValue );
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_U16;
  }

private:
  u16*       m_pInstance;
};

class NameManglingStringNode : public NameManglingNodeBase
{
public:

  NameManglingStringNode( void );
  virtual ~NameManglingStringNode( void );

  // 名前修飾を設定
  void SetNameMangling( const char* pName, c8* pInstance, u32 size );

  // 値をテキストフォーマットで設定
  virtual b32 SetValue( const char* pValue );
 
  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_S32;
  }

private:
  c8*       m_pInstance;
  u32       m_Size;
};

class NameManglingWStringNode : public NameManglingNodeBase
{
public:

  NameManglingWStringNode( void );
  virtual ~NameManglingWStringNode( void );

  // 名前修飾を設定
  void SetNameMangling( const char* pName, c16* pInstance, u32 size );

  // 値をテキストフォーマットで設定
  virtual b32 SetValue( const char* pValue );
 
  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_S32;
  }

private:
  c16*       m_pInstance;
  u32       m_Size;
};



class NameManglingF32Node : public NameManglingNodeBase
{
public:

  NameManglingF32Node( void ) : m_pInstance( NULL )
  {
  }

  ~NameManglingF32Node( void )
  {
  }

  // 名前修飾を設定
  void SetNameMangling( f32* pInstance, const char* pName )
  {
    m_pInstance = pInstance;
    m_Name = std::string( pName );
  }

  // 値を設定
  void SetValue( f32 value ) 
  {
    if( m_pInstance == NULL )
    {
      return;
    }

    *m_pInstance = value;
  }

  // 値をテキストフォーマットで設定
  b32 SetValue( const char* pValue )
  {
    if( m_pInstance == NULL )
    {
      return false;
    }

    f32 work;
    if( sscanf_s( pValue, "%f", &work ) == 1 )
    {
      if( *m_pInstance != work )
      {
        *m_pInstance = work;
        return true;
      }
    }

    return false;
  }

  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_F32;
  }

private:
  f32*    m_pInstance;
};


class NameManglingB32Node : public NameManglingNodeBase
{
public:

  NameManglingB32Node( void ) : m_pInstance( NULL )
  {
  }

  ~NameManglingB32Node( void )
  {
  }

  // 名前修飾を設定
  void SetNameMangling( b32* pInstance, const char* pName )
  {
    m_pInstance = pInstance;
    m_Name = std::string( pName );
  }

  // 値を設定
  void SetValue( b32 value ) 
  {
    if( m_pInstance == NULL )
    {
      return;
    }

    *m_pInstance = value;
  }

  // 値をテキストフォーマットで設定
  b32 SetValue( const char* pValue )
  {
    if( m_pInstance == NULL )
    {
      return false;
    }

    b32 prev = *m_pInstance;

    c8 work[255];
    if( sscanf_s( pValue, "%s", &work, sizeof( work ) ) == 1 )
    {
      if( strcmp( work, "true" ) == 0 )
      {
        *m_pInstance = true;
      }
      else if( strcmp( work, "false" ) == 0 )
      {
        *m_pInstance = false;
      }
    }

    return ( prev != *m_pInstance );
  }

  // タイプを返す
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_F32;
  }

private:
  b32*    m_pInstance;
};

class GroupNode
{
public:

  GroupNode( const char* pName ) : m_pGroupNodeRoot( NULL ), m_pNameManglingNodeRoot( NULL ), m_pPrevGroupNode( NULL ), m_pNextGroupNode( NULL ), m_pCallBackObject( NULL )
  {
    m_Name = std::string( pName );
  }

  virtual ~GroupNode(void)
  {
    // 子を開放
    {
      GroupNode* pNowNode( m_pGroupNodeRoot );
      while( pNowNode )
      {
        GroupNode* pNext = pNowNode->m_pNextGroupNode;
        delete pNowNode;
        pNowNode = pNext;
      }
    }

    {
      NameManglingNodeBase* pNowNode( m_pNameManglingNodeRoot );
      while( pNowNode )
      {
        NameManglingNodeBase* pNext = pNowNode->m_pNextNode;
        delete pNowNode;
        pNowNode = pNext;
      }
    }
  }

  GroupNode* GetGroupNode( const char* pName )
  {
    return GetGroupNodeImpl( pName ); 
  }


  GroupNode* GetGroupNodeImpl( const char* pName )
  {
    const c8* pWork;
    if ( ( pWork = strstr( pName, "/" ) ) != NULL ) 
    {
      u32 nodeNameSize = pWork - pName;

      if( nodeNameSize == 0 )
      {
        // 先頭/は不正
        return NULL;
      }

      GroupNode* pNowNode( m_pGroupNodeRoot );
      while( pNowNode )
      {
        // スラッシュの手前まで比較
        if( pNowNode->m_Name.size() == nodeNameSize && strncmp( pNowNode->m_Name.c_str(), pName, nodeNameSize ) == 0 )
        {
          return pNowNode->GetGroupNode( pName + nodeNameSize + 1 );
        }
        pNowNode = pNowNode->m_pNextGroupNode;
      }
    } 
    else 
    {
      GroupNode* pNowNode( m_pGroupNodeRoot );
      while( pNowNode )
      {
        if( pNowNode->m_Name == pName )
        {
          return pNowNode;
        }
        pNowNode = pNowNode->m_pNextGroupNode;
      }
    }

    return NULL;
  }


  NameManglingNodeBase* GetNameManglingNode( const char* pName )
  {
    NameManglingNodeBase* pNowNode( m_pNameManglingNodeRoot );
    while( pNowNode )
    {
      if( pNowNode->m_Name == pName )
      {
        return pNowNode;
      }
      pNowNode = pNowNode->m_pNextNode;
    }
    return NULL;
  }

  b32 GetValue( const char* pName, s32* pValue )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingEnumNode* pNameManglingNode = dynamic_cast< NameManglingEnumNode* >( pNode );

    if( pNameManglingNode )
    {
      *pValue = pNameManglingNode->GetValue();
      return true;
    }
    else
    {
      return false;
    }
  }

  void SetValue( const char* pName, s32 value )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingS32Node* pNameManglingNode = dynamic_cast< NameManglingS32Node* >( pNode );

    if( pNameManglingNode )
    {
      pNameManglingNode->SetValue( value );
    }
  }
  void SetValue( const char* pName, u32 value )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingU32Node* pNameManglingNode = dynamic_cast< NameManglingU32Node* >( pNode );

    if( pNameManglingNode )
    {
      pNameManglingNode->SetValue( value );
    }
  }
  void SetValue( const char* pName, u16 value )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingU16Node* pNameManglingNode = dynamic_cast< NameManglingU16Node* >( pNode );

    if( pNameManglingNode )
    {
      pNameManglingNode->SetValue( value );
    }
  }

  void SetValue( const char* pName, f32 value )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingF32Node* pNameManglingNode = dynamic_cast< NameManglingF32Node* >( pNode );

    if( pNameManglingNode )
    {
      pNameManglingNode->SetValue( value );
    }
  }
  void SetBoolValue( const char* pName, b32 value )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );
    NameManglingB32Node* pNameManglingNode = dynamic_cast< NameManglingB32Node* >( pNode );

    if( pNameManglingNode )
    {
      pNameManglingNode->SetValue( value );
    }
  }

  b32 SetValue( const char* pName, const char* pText )
  {
    NameManglingNodeBase* pNode = GetNameManglingNode( pName );

    if( pNode )
    {
      b32 changeFlag;
      changeFlag = pNode->SetValue( pText );
      if( pNode->GetCallBack() )
      {
        pNode->GetCallBack()->Callback( pNode, changeFlag );
      }

      return changeFlag;
    }

    return false;
  }

  b32 AddGroup( GroupNode* pGroupNode )
  {
    if( GetGroupNode( pGroupNode->m_Name.c_str() ) )
    {
      // 同名ノードが既に存在する
      return false;
    }

    if( m_pGroupNodeRoot )
    {
      // 末尾に連結
      GroupNode* pEndNode = GetListEnd( m_pGroupNodeRoot );
      pEndNode->m_pNextGroupNode = pGroupNode;
      pGroupNode->m_pPrevGroupNode = pEndNode;
    }
    else
    {
      // ルート化
      m_pGroupNodeRoot = pGroupNode;
    }

    return true;
  }

  GroupNode* AddGroup( const char* pName )
  {
    if( GetGroupNode( pName ) )
    {
      // 同名ノードが既に存在する
      return NULL;
    }

    GroupNode* pNode = new GroupNode( pName );
    if( AddGroup( pNode ) )
    {
      return pNode;
    }
    else
    {
      // 追加失敗、現在の仕様では発生しない
      delete pNode;
      return NULL;
    }
  }

  void AddNameManglingNode( NameManglingNodeBase* pNode )
  {
    if( m_pNameManglingNodeRoot )
    {
      // 末尾に連結
      m_pNameManglingNodeRoot->PushBuck( pNode );
    }
    else
    {
      // ルート化
      m_pNameManglingNodeRoot = pNode;
    }
  }

  NameManglingS32Node* AddNameMangling( s32* p, const char* pName )
  {
    NameManglingS32Node* pNameManglingNode = new NameManglingS32Node;
    pNameManglingNode->SetNameMangling( p, pName );
    AddNameManglingNode( pNameManglingNode );
    return pNameManglingNode;
  }
  NameManglingU32Node* AddNameMangling( u32* p, const char* pName )
  {
    NameManglingU32Node* pNameManglingNode = new NameManglingU32Node;
    pNameManglingNode->SetNameMangling( p, pName );
    AddNameManglingNode( pNameManglingNode );
    return pNameManglingNode;
  }
  NameManglingU16Node* AddNameMangling( u16* p, const char* pName )
  {
    NameManglingU16Node* pNameManglingNode = new NameManglingU16Node;
    pNameManglingNode->SetNameMangling( p, pName );
    AddNameManglingNode( pNameManglingNode );
    return pNameManglingNode;
  }
  NameManglingF32Node* AddNameMangling( f32* p, const char* pName )
  {
    NameManglingF32Node* pNameManglingNode = new NameManglingF32Node;
    pNameManglingNode->SetNameMangling( p, pName );
    AddNameManglingNode( pNameManglingNode );
    return pNameManglingNode;
  }
#if 1
  NameManglingB32Node* AddBoolNameMangling( b32* p, const char* pName )
  {
    NameManglingB32Node* pNameManglingNode = new NameManglingB32Node;
    pNameManglingNode->SetNameMangling( p, pName );
    AddNameManglingNode( pNameManglingNode );
    return pNameManglingNode;
  }
#endif
  void AddNameMangling( const char* pName, s32 value )
  {
    NameManglingEnumNode* pNameManglingNode = new NameManglingEnumNode;
    pNameManglingNode->SetNameMangling( pName, value );
    AddNameManglingNode( pNameManglingNode );
  }

  void AddNameMangling( const char* pName, c8* pInstance, u32 size )
  {
    NameManglingStringNode* pNameManglingNode = new NameManglingStringNode;
    pNameManglingNode->SetNameMangling( pName, pInstance, size );
    AddNameManglingNode( pNameManglingNode );
  }

  void AddNameMangling( const char* pName, c16* pInstance, u32 size )
  {
    NameManglingWStringNode* pNameManglingNode = new NameManglingWStringNode;
    pNameManglingNode->SetNameMangling( pName, pInstance, size );
    AddNameManglingNode( pNameManglingNode );
  }


  // 解釈機能
  b32 LoadAndUpdate( const char* pFilePath );
  b32 Update( const char* pData );

  void SetCallBack( IGroupNodeCallbackable* pCallBack )
  {
    m_pCallBackObject = pCallBack;
  }

  IGroupNodeCallbackable* GetCallBack( void )
  {
    return m_pCallBackObject;
  }

private:

  GroupNode* GetListEnd( GroupNode* pGroupNode ) const
  {
    GroupNode* pNowNode( pGroupNode );
    if( pNowNode == NULL )
    {
      return NULL;
    }

    while( pNowNode->m_pNextGroupNode )
    {
      pNowNode = pNowNode->m_pNextGroupNode;
    }

    return pNowNode;
  }

  b32 UpdateImpl( c8* pData );
  c8* ReadFile( const char* pFileName, s32* pSize, s32 align = 16 );


  // 自身の情報
  std::string             m_Name;
  IGroupNodeCallbackable* m_pCallBackObject;

  // 保持する子供
  GroupNode*            m_pGroupNodeRoot;
  NameManglingNodeBase* m_pNameManglingNodeRoot;

  // リスト構造
  GroupNode*            m_pPrevGroupNode;
  GroupNode*            m_pNextGroupNode;
};

GFL_NAMESPACE_END( NameMangling );
GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#endif // #if defined(GF_PLATFORM_WIN32)
#endif // #if !defined( FIELDDEBUGNAMEMANGLINGMANAGER_H_INCLUDED )
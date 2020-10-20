/**
* @file    SkeltonNameManglingManager.h
* @brief   名前修飾を管理クラス
* @author  ikuta_junya
* @date    2014.10.09
*/

#if !defined( SKELTONNAMEMANGLINGMANAGER_H_INCLUDED )
#define SKELTONNAMEMANGLINGMANAGER_H_INCLUDED
#pragma once

// @note skeltonから移植、stdやxmlパーサーを使うのでwin32専用
#if defined(GF_PLATFORM_WIN32)

#include <heap/include/gfl2_heap.h>
#include <string>

//#include <iostream>
#include <sstream>

// @note 以下のテンプレート化できる

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)
GFL_NAMESPACE_BEGIN( SkeltonNameMangling );

class SkeltonNameManglingNodeBase;
class GroupNode;

//! @brief 値コールバックインターフェース
class ISkeltonNameManglingNodeCallbackable
{
public:

  /**
   * @brief コンストラクタ
   */
  ISkeltonNameManglingNodeCallbackable() {}

  /**
   * @brief デストラクタ
   */
  virtual ~ISkeltonNameManglingNodeCallbackable() {}

  /**
   * @brief 値設定後のコールバック関数
   */
  virtual void Callback( SkeltonNameManglingNodeBase* pSkeltonNameManglingNodeBase, b32 isChange ) = 0;
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

class SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingNodeBase( void ) : m_pPrevNode( NULL), m_pNextNode( NULL ), m_pCallBackInterFace( NULL )
  {
  }

  virtual ~SkeltonNameManglingNodeBase( void )
  {
  }

  void PushBuck( SkeltonNameManglingNodeBase* pNode )
  {
    SkeltonNameManglingNodeBase* pEndNode = GetListEnd();

    pEndNode->m_pNextNode = pNode;
    pNode->m_pPrevNode = pEndNode;
  }

  virtual b32 SetValue( const char* pValue ) = 0;
  virtual STRUCT_TYPE GetInstanceType( void ) const = 0;

  void SetCallBack( ISkeltonNameManglingNodeCallbackable* pCallBack )
  {
    m_pCallBackInterFace = pCallBack;
  }
  
  ISkeltonNameManglingNodeCallbackable* GetCallBack( void )
  {
    return m_pCallBackInterFace;
  }


  // 自身の情報
  std::string           m_Name;

  // リスト構造
  SkeltonNameManglingNodeBase* m_pPrevNode;
  SkeltonNameManglingNodeBase* m_pNextNode;

private:
  SkeltonNameManglingNodeBase* GetListEnd( void )
  {
    SkeltonNameManglingNodeBase* pNowNode( this );

    while( pNowNode->m_pNextNode )
    {
      pNowNode = pNowNode->m_pNextNode;
    }

    return pNowNode;
  }

  ISkeltonNameManglingNodeCallbackable* m_pCallBackInterFace;

};

// ハッシュの代わり
class SkeltonNameManglingEnumNode : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingEnumNode( void ) : m_Value( 0 )
  {
  }

  ~SkeltonNameManglingEnumNode( void )
  {
  }

  // 名前修飾を設定
  void SetSkeltonNameMangling( const char* pName, s32 value )
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

class SkeltonNameManglingS32Node : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingS32Node( void );
  ~SkeltonNameManglingS32Node( void );

  // 名前修飾を設定
  void SetSkeltonNameMangling( s32* pInstance, const char* pName );

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

class SkeltonNameManglingU32Node : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingU32Node( void );
  ~SkeltonNameManglingU32Node( void );

  void SetSkeltonNameMangling( u32* pInstance, const char* pName );
  void SetValue( u32 value );
  b32 SetValue( const char* pValue );
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_U32;
  }

private:
  u32*       m_pInstance;
};

class SkeltonNameManglingU16Node : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingU16Node( void );
  ~SkeltonNameManglingU16Node( void );

  void SetSkeltonNameMangling( u16* pInstance, const char* pName );
  void SetValue( u16 value );
  b32 SetValue( const char* pValue );
  STRUCT_TYPE GetInstanceType( void ) const
  {
    return STRUCT_TYPE_U16;
  }

private:
  u16*       m_pInstance;
};

class SkeltonNameManglingStringNode : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingStringNode( void );
  virtual ~SkeltonNameManglingStringNode( void );

  // 名前修飾を設定
  void SetSkeltonNameMangling( const char* pName, c8* pInstance, u32 size );

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

class SkeltonNameManglingWStringNode : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingWStringNode( void );
  virtual ~SkeltonNameManglingWStringNode( void );

  // 名前修飾を設定
  void SetSkeltonNameMangling( const char* pName, c16* pInstance, u32 size );

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



class SkeltonNameManglingF32Node : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingF32Node( void ) : m_pInstance( NULL )
  {
  }

  ~SkeltonNameManglingF32Node( void )
  {
  }

  // 名前修飾を設定
  void SetSkeltonNameMangling( f32* pInstance, const char* pName )
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


class SkeltonNameManglingB32Node : public SkeltonNameManglingNodeBase
{
public:

  SkeltonNameManglingB32Node( void ) : m_pInstance( NULL )
  {
  }

  ~SkeltonNameManglingB32Node( void )
  {
  }

  // 名前修飾を設定
  void SetSkeltonNameMangling( b32* pInstance, const char* pName )
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

  GroupNode( const char* pName ) : m_pGroupNodeRoot( NULL ), m_pSkeltonNameManglingNodeRoot( NULL ), m_pPrevGroupNode( NULL ), m_pNextGroupNode( NULL ), m_pCallBackObject( NULL )
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
      SkeltonNameManglingNodeBase* pNowNode( m_pSkeltonNameManglingNodeRoot );
      while( pNowNode )
      {
        SkeltonNameManglingNodeBase* pNext = pNowNode->m_pNextNode;
        delete pNowNode;
        pNowNode = pNext;
      }
    }
  }

  GroupNode* GetGroupNode( const char* pName )
  {
    return GetGroupNodeImpl( pName ); 
#if 0
    // @todo イテレーター化
    GroupNode* pNowNode( m_pGroupNodeRoot );
    while( pNowNode )
    {
      if( pNowNode->m_Name == pName )
      {
        return pNowNode;
      }
      pNowNode = pNowNode->m_pNextGroupNode;
    }
    return NULL;
#endif
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

      // @todo イテレーター化
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
      // @todo イテレーター化
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


  SkeltonNameManglingNodeBase* GetSkeltonNameManglingNode( const char* pName )
  {
    // @todo イテレーター化
    SkeltonNameManglingNodeBase* pNowNode( m_pSkeltonNameManglingNodeRoot );
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
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingEnumNode* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingEnumNode* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      *pValue = pSkeltonNameManglingNode->GetValue();
      return true;
    }
    else
    {
      return false;
    }
  }

  void SetValue( const char* pName, s32 value )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingS32Node* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingS32Node* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      pSkeltonNameManglingNode->SetValue( value );
    }
  }
  void SetValue( const char* pName, u32 value )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingU32Node* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingU32Node* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      pSkeltonNameManglingNode->SetValue( value );
    }
  }
  void SetValue( const char* pName, u16 value )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingU16Node* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingU16Node* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      pSkeltonNameManglingNode->SetValue( value );
    }
  }

  void SetValue( const char* pName, f32 value )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingF32Node* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingF32Node* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      pSkeltonNameManglingNode->SetValue( value );
    }
  }
  void SetBoolValue( const char* pName, b32 value )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );
    SkeltonNameManglingB32Node* pSkeltonNameManglingNode = dynamic_cast< SkeltonNameManglingB32Node* >( pNode );

    if( pSkeltonNameManglingNode )
    {
      pSkeltonNameManglingNode->SetValue( value );
    }
  }

  b32 SetValue( const char* pName, const char* pText )
  {
    SkeltonNameManglingNodeBase* pNode = GetSkeltonNameManglingNode( pName );

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

  void AddSkeltonNameManglingNode( SkeltonNameManglingNodeBase* pNode )
  {
    if( m_pSkeltonNameManglingNodeRoot )
    {
      // 末尾に連結
      m_pSkeltonNameManglingNodeRoot->PushBuck( pNode );
    }
    else
    {
      // ルート化
      m_pSkeltonNameManglingNodeRoot = pNode;
    }
  }

  SkeltonNameManglingS32Node* AddSkeltonNameMangling( s32* p, const char* pName )
  {
    SkeltonNameManglingS32Node* pSkeltonNameManglingNode = new SkeltonNameManglingS32Node;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( p, pName );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
    return pSkeltonNameManglingNode;
  }
  SkeltonNameManglingU32Node* AddSkeltonNameMangling( u32* p, const char* pName )
  {
    SkeltonNameManglingU32Node* pSkeltonNameManglingNode = new SkeltonNameManglingU32Node;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( p, pName );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
    return pSkeltonNameManglingNode;
  }
  SkeltonNameManglingU16Node* AddSkeltonNameMangling( u16* p, const char* pName )
  {
    SkeltonNameManglingU16Node* pSkeltonNameManglingNode = new SkeltonNameManglingU16Node;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( p, pName );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
    return pSkeltonNameManglingNode;
  }
  SkeltonNameManglingF32Node* AddSkeltonNameMangling( f32* p, const char* pName )
  {
    SkeltonNameManglingF32Node* pSkeltonNameManglingNode = new SkeltonNameManglingF32Node;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( p, pName );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
    return pSkeltonNameManglingNode;
  }
#if 1
  SkeltonNameManglingB32Node* AddBoolSkeltonNameMangling( b32* p, const char* pName )
  {
    SkeltonNameManglingB32Node* pSkeltonNameManglingNode = new SkeltonNameManglingB32Node;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( p, pName );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
    return pSkeltonNameManglingNode;
  }
#endif
  void AddSkeltonNameMangling( const char* pName, s32 value )
  {
    SkeltonNameManglingEnumNode* pSkeltonNameManglingNode = new SkeltonNameManglingEnumNode;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( pName, value );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
  }

  void AddSkeltonNameMangling( const char* pName, c8* pInstance, u32 size )
  {
    SkeltonNameManglingStringNode* pSkeltonNameManglingNode = new SkeltonNameManglingStringNode;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( pName, pInstance, size );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
  }

  void AddSkeltonNameMangling( const char* pName, c16* pInstance, u32 size )
  {
    SkeltonNameManglingWStringNode* pSkeltonNameManglingNode = new SkeltonNameManglingWStringNode;
    pSkeltonNameManglingNode->SetSkeltonNameMangling( pName, pInstance, size );
    AddSkeltonNameManglingNode( pSkeltonNameManglingNode );
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
  SkeltonNameManglingNodeBase* m_pSkeltonNameManglingNodeRoot;

  // リスト構造
  GroupNode*            m_pPrevGroupNode;
  GroupNode*            m_pNextGroupNode;
};

GFL_NAMESPACE_END( SkeltonNameMangling );
GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)


#endif // #if defined(GF_PLATFORM_WIN32)
#endif // #if !defined( SKELTONNAMEMANGLINGMANAGER_H_INCLUDED )
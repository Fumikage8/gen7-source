//===========================================================================
/**
 * @file    RttiManager.h
 * @brief   クラスのメンバ変数情報と継承関係をRTTで保持するマネージャー
 * @date    2014.11.18
 * @author  yy-ikeuchi
 */
//===========================================================================
#pragma once

#include <typeinfo>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_Map.h>
#include <util/include/gfl2_Pair.h>

#ifdef WIN32
  // RTTI_DEBUG機能のオン
  #define USE_RTTI_DEBUG PM_DEBUG
#else
  // RTTI_DEBUG機能のオフ
  // CTR版ではRTTIを利用しないプロジェクト構成になってしまうのでオンにするとメモリがあふれる可能性があるため
  #define USE_RTTI_DEBUG 0
#endif

#if USE_RTTI_DEBUG

namespace gfl2{ namespace rttidebug{

// ワイド文字のコピー
extern void wideCharaCopy(c16* pDst, size_t dstSize, const c16* pSrc);
// ワイド文字の連結
extern void wideCharaCat (c16* pDst, size_t dstSize, const c16* pSrc);

class RttiParamManager
{
public:

  //===========================================================================
  // クラスのメンバ変数情報
  //===========================================================================
  struct MemberProperty
  {
    // RTTI名のサイズ
    static const int DISP_NAME_SIZE = 28;

    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
    */
    //------------------------------------------------------------------
    MemberProperty()
    : _pTypeInfo(NULL), _offsetAddress(0), _attribute(0)
    {
      wideCharaCopy( _dispName, DISP_NAME_SIZE, L"default" );
    }

    //------------------------------------------------------------------
    /**
    * @brief   コピーコンストラクタ
    */
    //------------------------------------------------------------------
    MemberProperty(const type_info* pTypeInfo, uint64_t offsetAddress, unsigned int attribute, c16* dispName)
    : _pTypeInfo    (pTypeInfo)
    , _offsetAddress(offsetAddress)
    , _attribute    (attribute)
    {
      wideCharaCopy( _dispName, DISP_NAME_SIZE, dispName );
    }

    //------------------------------------------------------------------
    /**
    * @brief   代入
    */
    //------------------------------------------------------------------
    inline MemberProperty&  operator =( const MemberProperty&  rhs )
    {
      this->_pTypeInfo     = rhs._pTypeInfo;
      this->_offsetAddress = rhs._offsetAddress;
      this->_attribute     = rhs._attribute;
      wideCharaCopy( this->_dispName, DISP_NAME_SIZE, rhs._dispName );
      return *this;
    }

    const type_info*  _pTypeInfo;                 // メンバ変数の型情報
    uint64_t          _offsetAddress;             // メンバ変数の相対アドレス
    unsigned int      _attribute;                 // 属性
    c16               _dispName[DISP_NAME_SIZE];  // 表示名
  };

  //  属性
  enum
  {
    ATTR_READONLY       = 0x00000001 << 0,        // 書き込み禁止
    ATTR_ADD_SMALLEST   = 0x00000001 << 1,        // 数値を0.01 毎に増やす、他のATTR_ADD_XXXとは併用不可(float/double以外は無効)
    ATTR_ADD_SMALL      = 0x00000001 << 2,        // 数値を0.1  毎に増やす、他のATTR_ADD_XXXとは併用不可(float/double以外は無効)
    ATTR_ADD_BIG        = 0x00000001 << 3,        // 数値を10   毎に増やす、他のATTR_ADD_XXXとは併用不可(boolは無効)
    ATTR_ADD_BIGGEST    = 0x00000001 << 4,        // 数値を100  毎に増やす、他のATTR_ADD_XXXとは併用不可(boolは無効)
  };

  //------------------------------------------------------------------
  /**
  * @brief   シングルトンインスタンスを取得するアクセサ
  * @return  RTTIマネージャーへのポインタ
  */
  //------------------------------------------------------------------
  static RttiParamManager* getInstance() 
  {
    static RttiParamManager instance;
    return &instance;
  }

private:

  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ(使用禁止)
  */
  //------------------------------------------------------------------
  RttiParamManager      ()                               {}
  RttiParamManager      (const RttiParamManager &other)  {}

public:

  //------------------------------------------------------------------
  /**
  * @brief   初期化
  * @param   ヒープ
  * @param   バッファサイズ
  */
  //------------------------------------------------------------------
  inline void Initialize(gfl2::heap::HeapBase* pHeap, u32 bufferSize)
  {
    m_pHeap = pHeap;
    m_parameterManager.CreateBuffer(pHeap,bufferSize);
    m_relationManager.CreateBuffer(pHeap,bufferSize);
  }

  //------------------------------------------------------------------
  /**
  * @brief   破棄
  */
  //------------------------------------------------------------------
  inline void Terminate(void)
  {
    if( m_parameterManager.Size() > 0 )
    {
      for ( gfl2::util::Map< const type_info*,gfl2::util::List<MemberProperty*>*>::Iterator ite = m_parameterManager.Begin() ; ite != m_parameterManager.End() ; ite++  )
      {
        gfl2::util::List<MemberProperty*>* pListParam = ite->second;
        for ( gfl2::util::List<MemberProperty*>::Iterator ite = pListParam->Begin() ; ite != pListParam->End() ; ite++  )
        {
          GFL_SAFE_DELETE(*ite);
        }
        GFL_SAFE_DELETE(pListParam);
      }
    }
    m_parameterManager.Clear();
    m_relationManager.Clear();
  }

  //------------------------------------------------------------------
  /**
  * @brief   クラスとメンバ変数の関係保持マネージャーに情報を登録
  * @return  なし
  */
  //------------------------------------------------------------------
  inline bool insertToParam( const type_info* pTypeInfo, gfl2::util::List<MemberProperty*>* pMemberPropertyList)
  {
    if( m_parameterManager.Size() > 0 )
    {
      for ( gfl2::util::Map< const type_info*,gfl2::util::List<MemberProperty*>*>::Iterator ite = m_parameterManager.Begin() ; ite != m_parameterManager.End() ; ite++  )
      {
        if( ite->first == pTypeInfo )
        { 
          return false; 
        }
      }
    }
    m_parameterManager.Insert( gfl2::util::Pair<const type_info*,gfl2::util::List<MemberProperty*>*>( pTypeInfo , pMemberPropertyList ) );
    return true;
  }

  //------------------------------------------------------------------
  /**
  * @brief   クラスと親クラスの関係保持マネージャーに情報を登録
  * @return  なし
  */
  //------------------------------------------------------------------
  inline bool insertToReleation( const type_info* pClassTypeInfo, const type_info* pParentClassTypeInfo)
  {
    if( m_relationManager.Size() > 0 )
    {
      for ( gfl2::util::Map< const type_info*,const type_info* >::Iterator ite = m_relationManager.Begin() ; ite != m_relationManager.End() ; ite++  )
      {
        if( ite->first == pClassTypeInfo && ite->second == pParentClassTypeInfo )
        { 
          return false; 
        }
      }
    }
    m_relationManager.Insert ( gfl2::util::Pair<const type_info*, const type_info* >( pClassTypeInfo , pParentClassTypeInfo ) ); 
    return true;
  }

  //------------------------------------------------------------------
  /**
  * @brief   指定クラスが保持しているメンバ変数情報リストをコピー
  * @return  なし
  */
  //------------------------------------------------------------------
  void getParamList( const type_info*                   targetClassInfo,
                     gfl2::util::List<MemberProperty*>* pOutList,
                     c16*                               baseName,
                     uint64_t                           baseOffset = 0 );

  //------------------------------------------------------------------
  /**
  * @brief   初期化
  * @param   ヒープ
  * @param   バッファサイズ
  */
  //------------------------------------------------------------------
  gfl2::heap::HeapBase* getHeap(void){ return m_pHeap; }

private:

  // マネージャー全体で利用するヒープ
  gfl2::heap::HeapBase* m_pHeap;

  // クラスのメンバ情報保持マネジャー
  gfl2::util::Map< const type_info*,gfl2::util::List<MemberProperty*>* > m_parameterManager;  // first  : 型情報
                                                                                              // second : メンバ情報リスト
  // クラスの継承関係保持マネージャー
  gfl2::util::Map< const type_info*,const type_info* >                   m_relationManager;   // first  : 検索キーとなるクラスの型情報
                                                                                              // second : firstに登録されているクラスの親クラスの型情報
};

}}

#endif

//***********************************************************************
//
//              マクロ定義
//  (RttiParamManagerクラスを隠蔽し、簡単に利用できるようにするための物)
//
//***********************************************************************

// 親子関係登録用
#define RTTI_BASE_PARENT(ClassName,ParentClassName)                               gfl2::rttidebug::RttiParamManager::getInstance()->insertToReleation( &typeid( ClassName ) , &typeid( ParentClassName ) );
// クラスのメンバ情報登録開始
#define RTTI_START_REGIST(bufferSize)                                             gfl2::heap::HeapBase* pRttiHeap = gfl2::rttidebug::RttiParamManager::getInstance()->getHeap();  \
                                                                                  gfl2::util::List< gfl2::rttidebug::RttiParamManager::MemberProperty* >* pRttiParameterList;     \
                                                                                  pRttiParameterList = GFL_NEW(pRttiHeap) gfl2::util::List< gfl2::rttidebug::RttiParamManager::MemberProperty* >(); \
                                                                                  pRttiParameterList->CreateBuffer(pRttiHeap,bufferSize);
// クラスのメンバ情報登録
#define RTTI_PARAM_REGIST(ClassName,MemberName,DispName)                          pRttiParameterList->PushBack( GFL_NEW(pRttiHeap) gfl2::rttidebug::RttiParamManager::MemberProperty( &typeid(  MemberName ) , (uint64_t)&((ClassName*)0)->MemberName , 0     , DispName ) );
#define RTTI_PARAM_REGIST_ATTR(ClassName,MemberName,DispName,Attr)                pRttiParameterList->PushBack( GFL_NEW(pRttiHeap) gfl2::rttidebug::RttiParamManager::MemberProperty( &typeid(  MemberName ) , (uint64_t)&((ClassName*)0)->MemberName , Attr  , DispName ) );
#define RTTI_ARRAY_REGIST(ClassName,ArrayName,ArrayCount,DispName)                for( unsigned int i=0 ; i<ArrayCount ; i++ ){ RTTI_PARAM_REGIST         (ClassName,ArrayName[i],DispName);      }
#define RTTI_ARRAY_REGIST_ATTR(ClassName,ArrayName,ArrayCount,DispName,Attr)      for( unsigned int i=0 ; i<ArrayCount ; i++ ){ RTTI_PARAM_REGIST_ATTR    (ClassName,ArrayName[i],DispName,Attr); }
// クラスのメンバ情報登録終了
#define RTTI_END_REGIST(ClassName)                                                b32  isResult = gfl2::rttidebug::RttiParamManager::getInstance()->insertToParam( &typeid( ClassName ) , pRttiParameterList ); \
                                                                                  if( !isResult ) \
                                                                                  { \
                                                                                    for ( gfl2::util::List<gfl2::rttidebug::RttiParamManager::MemberProperty*>::Iterator ite = pRttiParameterList->Begin() ; ite != pRttiParameterList->End() ; ite++ ){ GFL_SAFE_DELETE(*ite); } \
                                                                                    GFL_SAFE_DELETE(pRttiParameterList); \
                                                                                  }

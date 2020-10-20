//===========================================================================
/**
 * @file    RttiManager.cpp
 * @brief   クラスのメンバ変数情報と継承関係をRTTで保持するマネージャー
 * @date    2014.11.18
 * @author  yy-ikeuchi
 */
//===========================================================================

#include <typeinfo>
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_Map.h>
#include "../../include/RttiDebug/RttiManager.h"

#if USE_RTTI_DEBUG

namespace gfl2{ namespace rttidebug{

// ワイド文字のコピー
void wideCharaCopy(c16* pDst, size_t dstSize, const c16* pSrc)
{
  for( u32 i=0 ; i<dstSize ; i++ )
  {
    pDst[i] = pSrc[i];
    if( pDst[i] == L'\0' )
    { 
      return; 
    }
  }
}

// ワイド文字の連結
void wideCharaCat(c16* pDst, size_t dstSize, const c16* pSrc)
{
  u32 i=0;
  for( ; i<dstSize ; i++ )
  {
    if( pDst[i] == L'\0' ){ break; }
  }
  wideCharaCopy( &pDst[i] , dstSize-i , pSrc );
}

//------------------------------------------------------------------
/**
* @brief   指定クラスが保持しているメンバ変数情報リストをコピー
* @return  リストへのアドレス
*/
//------------------------------------------------------------------
void RttiParamManager::getParamList( const type_info*                   targetClassInfo ,
                                     gfl2::util::List<MemberProperty*>* pOutList,
                                     c16*                               baseName,
                                     uint64_t                           baseOffset)
{
  if( m_parameterManager.Size() < 1 )
  {
    return;
  }

  for ( gfl2::util::Map< const type_info*,gfl2::util::List<MemberProperty*>*>::Iterator ite = m_parameterManager.Begin() ; ite != m_parameterManager.End() ; ite++  )
  {
    const type_info* pInfo = ite->first;

    // 指定したクラスと同じかどうか
    if( pInfo == targetClassInfo )
    {
      gfl2::util::List<MemberProperty*>* pListParam = ite->second;
      for( gfl2::util::List<MemberProperty*>::Iterator iteListParam = pListParam->Begin() ; iteListParam != pListParam->End() ; iteListParam++ )
      {
        MemberProperty* pParam = *iteListParam;

        // MemberPropertyが組み込み型で無い場合はユーザー定義の構造体・クラスだと見なし再起処理
        if( typeid(         bool      ) != *pParam->_pTypeInfo
        &&  typeid(         float     ) != *pParam->_pTypeInfo
        &&  typeid(         double    ) != *pParam->_pTypeInfo
        &&  typeid(         char      ) != *pParam->_pTypeInfo
        &&  typeid(         short     ) != *pParam->_pTypeInfo
        &&  typeid(         int       ) != *pParam->_pTypeInfo
        &&  typeid(         long      ) != *pParam->_pTypeInfo
        &&  typeid(         long long ) != *pParam->_pTypeInfo
        &&  typeid(unsigned char      ) != *pParam->_pTypeInfo
        &&  typeid(unsigned short     ) != *pParam->_pTypeInfo
        &&  typeid(unsigned int       ) != *pParam->_pTypeInfo
        &&  typeid(unsigned long      ) != *pParam->_pTypeInfo
        &&  typeid(unsigned long long ) != *pParam->_pTypeInfo
        &&  typeid(         bool*     ) != *pParam->_pTypeInfo
        &&  typeid(         float*    ) != *pParam->_pTypeInfo
        &&  typeid(         double*   ) != *pParam->_pTypeInfo
        &&  typeid(         char*     ) != *pParam->_pTypeInfo
        &&  typeid(         short*    ) != *pParam->_pTypeInfo
        &&  typeid(         int*      ) != *pParam->_pTypeInfo
        &&  typeid(         long*     ) != *pParam->_pTypeInfo
        &&  typeid(         long long*) != *pParam->_pTypeInfo
        &&  typeid(unsigned char*     ) != *pParam->_pTypeInfo
        &&  typeid(unsigned short*    ) != *pParam->_pTypeInfo
        &&  typeid(unsigned int*      ) != *pParam->_pTypeInfo
        &&  typeid(unsigned long*     ) != *pParam->_pTypeInfo
        &&  typeid(unsigned long long*) != *pParam->_pTypeInfo )
        {
          c16 newBaseName[MemberProperty::DISP_NAME_SIZE];
          wideCharaCopy( newBaseName, MemberProperty::DISP_NAME_SIZE, baseName );
          wideCharaCat ( newBaseName, MemberProperty::DISP_NAME_SIZE, pParam->_dispName );
          getParamList( pParam->_pTypeInfo , pOutList , newBaseName , baseOffset + pParam->_offsetAddress );
        }
        // 組み込み型の場合はMemberPropertyをコピーし、おおもとのクラスからのオフセット値にする
        else
        {
          RttiParamManager::MemberProperty* pNewMemProp = GFL_NEW(m_pHeap) gfl2::rttidebug::RttiParamManager::MemberProperty();
          *pNewMemProp = *pParam;
          pNewMemProp->_offsetAddress += baseOffset;

          c16 newName[MemberProperty::DISP_NAME_SIZE];
          wideCharaCopy( newName, MemberProperty::DISP_NAME_SIZE, baseName );
          wideCharaCat ( newName, MemberProperty::DISP_NAME_SIZE, pNewMemProp->_dispName );
          wideCharaCopy( pNewMemProp->_dispName, MemberProperty::DISP_NAME_SIZE, newName );
          pOutList->PushBack( pNewMemProp );
        }
      }
    }
  }

  // 指定クラスの親クラスのメンバ情報リストをコピー
  gfl2::util::Map< const type_info*,const type_info* >::Iterator ite = m_relationManager.Find( targetClassInfo );
  if( ite != m_relationManager.End() && ite->second != NULL )
  {
    getParamList( ite->second , pOutList , baseName , baseOffset );
  }
}

} }

#endif

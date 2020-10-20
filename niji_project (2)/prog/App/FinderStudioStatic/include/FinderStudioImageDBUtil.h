#if !defined(__FINDER_STUDIO_IMAGEDB_UTIL_H_INCLUDED__)
#define __FINDER_STUDIO_IMAGEDB_UTIL_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioImageDBUtil.h
 * @date    2016/11/17 18:19:13
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：ImageDBユーティリティ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDefine.h"
#include "FinderStudioErrorConverter.h"

#include <imagedb/include/gfl2_ImageDBAccessor.h>
#include <imagedb/include/gfl2_ImageDBSearcher.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector4.h>
#include <Layout/include/gfl2_Layout.h>
#include <heap/include/gfl2_Heap.h>

#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRTexture.h>
#endif


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    ImageDBユーティリティ
///
//////////////////////////////////////////////////////////////////////////
class ImageDBUtil
{
  //////////////////////////////////////////////////////////////////////////
  /// 
  ///     Static
  /// 
  //////////////////////////////////////////////////////////////////////////
private:
  //-------------------------------------------------------
  // @brief   ImageDBAccessor 生成・破棄
  //-------------------------------------------------------
  static gfl2::imagedb::ImageDBAccessor* CreateImageDBAccessor( gfl2::heap::HeapBase* workHeap );
  static void                            DeleteImageDBAccessor( gfl2::imagedb::ImageDBAccessor* accessor );

  //-------------------------------------------------------
  // @brief   ImageDBSearcher 生成・破棄
  //-------------------------------------------------------
  static gfl2::imagedb::ImageDBSearcher* CreateImageDBSearcher( gfl2::heap::HeapBase* workHeap );
  static void                            DeleteImageDBSearcher( gfl2::imagedb::ImageDBSearcher* searcher );

public:
  //-------------------------------------------------------
  // @brief   保存可能枚数チェック
  //-------------------------------------------------------
  static s32 CheckSDCard( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap );

  //-------------------------------------------------------
  // @brief   写真の読込み処理
  //-------------------------------------------------------
  static gfl2::imagedb::ImageDBAccessor::ResultCode LoadPhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, void* buffer, wchar_t* filepath, bool bThumbnail = false );

  //-------------------------------------------------------
  // @brief   写真の保存処理
  //-------------------------------------------------------
  static gfl2::imagedb::ImageDBAccessor::ResultCode SavePhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, void* buffer, wchar_t* filepath );

  //-------------------------------------------------------
  // @brief   写真の削除処理
  //-------------------------------------------------------
  static void DeletePhotoData( gfl2::imagedb::ImageDBAccessor::imgDbResult* dstRes, gfl2::heap::HeapBase* workHeap, wchar_t* filepath );

  //-------------------------------------------------------
  // @brief   写真の存在チェック
  // @caution 関数本来の目的とは違うため使用時には注意が必要
  //-------------------------------------------------------
  static bool IsExistsPhoto( gfl2::heap::HeapBase* workHeap, const wchar_t* filepath );

  //-------------------------------------------------------
  // @brief   写真の検索処理
  //-------------------------------------------------------
  static void SearchPhotoData( gfl2::heap::HeapBase* workHeap, gfl2::heap::HeapBase* infoHeap );
  static void DeletePictureInfo( void );

  //-------------------------------------------------------
  // @brief   写真データの取得
  //-------------------------------------------------------
  static gfl2::imagedb::ImageDBPictureInfo& GetImageDBPictureInfo( u32 index )
  {
    GFL_ASSERT( index < m_InfoCount );
    return m_pImgdbPicInfo[index];
  }
  static u32 GetPictureInfoCount( void ) { return m_InfoCount; }

  //-------------------------------------------------------
  // @brief   コンバート
  //-------------------------------------------------------
  static void ConvertImageLinerToBlock( void* srcBuffer, void* dstBuffer, bool bThumbnail );
  static void ConvertImageBlockToLiner( void* srcBuffer, void* dstBuffer );

  //-------------------------------------------------------
  // @brief   専用ヒープ 設定・取得
  // @caution ファインダースタジオ・マイアルバム以外での使用は禁止
  //-------------------------------------------------------
  static void                  SetImageDBHeap( gfl2::heap::HeapBase* heap ) { m_pImgdbHeap = heap; }
  static gfl2::heap::HeapBase* GetImageDBHeap( void ) { return m_pImgdbHeap; }

  //-------------------------------------------------------
  // @brief   バッファ 生成・破棄
  //-------------------------------------------------------
  static void* CreateColorBuffer( gfl2::heap::HeapBase* heap, bool bThumbnail = false, u32 align = PHOTO_BUFFER_ALIGN );
  static void  DeleteColorBuffer( void* buffer );

private:
  //-------------------------------------------------------
  // @brief   サイズ取得：非公開
  //-------------------------------------------------------
  static void _GetBufferSizeWH( u32* width, u32* height, bool bThumbnail );

private:
  static gfl2::heap::HeapBase*                m_pImgdbHeap;
  static gfl2::imagedb::ImageDBPictureInfo*   m_pImgdbPicInfo;
  static u32                                  m_InfoCount;



  //////////////////////////////////////////////////////////////////////////
  /// 
  ///     非Static
  /// 
  //////////////////////////////////////////////////////////////////////////
public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ImageDBUtil( void );
  ~ImageDBUtil( void );

public:
  //-------------------------------------------------------
  // @brief   写真の読込み
  //-------------------------------------------------------
  void StartLoadImage( gfl2::heap::HeapBase* heap, wchar_t* filepath, void* dstBuffer, bool bThumbnail );
  bool IsFinishLoadImage( void );

  //-------------------------------------------------------
  // @brief   写真の保存
  //-------------------------------------------------------
  void StartSaveImage( gfl2::heap::HeapBase* heap, wchar_t* filepath, void* srcBuffer );
  bool IsFinishSaveImage( void );

  //-------------------------------------------------------
  // @brief   写真の削除
  //-------------------------------------------------------
  void DeleteImage( gfl2::heap::HeapBase* heap, wchar_t* filepath );

  //-------------------------------------------------------
  // @brief   リザルト
  //-------------------------------------------------------
  ErrorConverter::ImageDBResultCode GetResult( void ) const { return m_ResultCode; }
  void ResetResult( void ) { m_ResultCode = ErrorConverter::IMGDB_RESULT_NUM; }

private:
  //-------------------------------------------------------
  // @brief   便利関数：非公開
  //-------------------------------------------------------
  void _ResetData( void );

private:
  gfl2::heap::HeapBase*   m_pHeap;

  u8      m_Seq;

private:
  wchar_t*    m_pFilepath;

  void*       m_pPhotoData;
  void*       m_pTempPhotoData;
  void*       m_pTempConvData;

  bool        m_bThumbnail;

  u32         m_BufSize;

  ErrorConverter::ImageDBResultCode   m_ResultCode;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_IMAGEDB_UTIL_H_INCLUDED__
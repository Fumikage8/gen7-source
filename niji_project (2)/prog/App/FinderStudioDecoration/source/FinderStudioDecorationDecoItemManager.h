#if !defined( __FINDER_STUDIO_DECORATION_DECO_ITEM_MANAGER_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATION_DECO_ITEM_MANAGER_H_INCLUDED__
#pragma once
//======================================================================
/**
* @file    FinderStudioDecorationDecoItemManager.h
* @date    2017/02/03 11:27:31
* @author  fukushima_yuya
* @brief   ファインダースタジオ「デコレーション」：デコ管理クラス
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include <math/include/gfl2_Math.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコ管理クラス
///
//////////////////////////////////////////////////////////////////////////
class DecoItemManager
{
  GFL_FORBID_COPY_AND_ASSIGN( DecoItemManager ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DecoItemManager( void );
  virtual ~DecoItemManager( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   追加・削除
  //-------------------------------------------------------
  void AddDeco( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  void RemoveDeco( u32 index );

  void SetFrame( u32 frameIndex );

  //-------------------------------------------------------
  // @brief   データセット
  //-------------------------------------------------------
  void SetDecoPos( u32 index, gfl2::math::VEC3 pos );

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  bool IsCanNotAdd( void ) const;
  bool IsDecoration( void ) const;

  //-------------------------------------------------------
  // @brief   取得
  //-------------------------------------------------------
  Static::StampData GetStampData( u8 index );
  u8  GetDecoCount( void ) const { return m_DecoCount; }

private:
  //-------------------------------------------------------
  // @brief   データセット：非公開
  //-------------------------------------------------------
  Static::StampData _SetStampData( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  void _ResetStampData( u32 index );
  void _StuffingStampData( u32 startIndex );

  void _ResetDecorationData( void );
  void _ResetStampData( Static::StampData* data );

private:
  static const u8 ADD_DECO_MAX = Static::DECO_ITEM_MAX;

private:
  Static::DecorationData    m_DecoData;

  u32   m_FrameID;

  u8    m_DecoCount;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_DECO_ITEM_MANAGER_H_INCLUDED__

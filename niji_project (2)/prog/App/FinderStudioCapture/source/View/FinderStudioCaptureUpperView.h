#if !defined( __FINDER_STUDIO_CAPTURE_UPPER_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_UPPER_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureUpperView.h
 * @date    2016/11/24 15:25:24
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：上画面View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    上画面View
///
//////////////////////////////////////////////////////////////////////////
class UpperView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView ); //コピーコンストラクタ＋代入禁止

private:
  /// カーテンアニメ
  enum CurtainAnim {
    CURTAIN_ANIM_OPEN,
    CURTAIN_ANIM_OPEN_KEEP,
    CURTAIN_ANIM_CLOSE,
    CURTAIN_ANIM_CLOSE_KEEP,
    CURTAIN_ANIM_NUM,
  };
  /// カーテンアニメシーケンス
  enum CurtainSeq {
    CURTAIN_SEQ_INIT,
    CURTAIN_SEQ_OPEN,
    CURTAIN_SEQ_OPEN_KEEP,
    CURTAIN_SEQ_CLOSE,
    CURTAIN_SEQ_CLOSE_KEEP,
    CURTAIN_SEQ_NUM,
  };
  /// 写真シーケンス
  enum PhotoSeq {
    PHOTO_SEQ_IDLE,
    PHOTO_SEQ_INVISIBLE,
    PHOTO_SEQ_MAKE_TEXTURE,
    PHOTO_SEQ_REPLACE_TEXTURE,
    PHOTO_SEQ_VISIBLE,
    PHOTO_SEQ_NUM,
  };
  /// シャッターアニメシーケンス
  enum ShutterSeq {
    SHUTTER_SEQ_OPEN,
    SHUTTER_SEQ_OPEN_KEEP,
    SHUTTER_SEQ_CLOSE,
    SHUTTER_SEQ_CLOSE_KEEP,
    SHUTTER_SEQ_NUM,
  };
  /// 照準タイプ
  enum SightType
  {
    SIGHT_TYPE_CROSS,     //!< 通常
    SIGHT_TYPE_NONE,      //!< 非表示
    SIGHT_TYPE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  UpperView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~UpperView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新
  //-------------------------------------------------------
  virtual void Update( void );

  //-------------------------------------------------------
  // @brief   カーテンアニメ
  //-------------------------------------------------------
  void CurtainOpen( void );
  void CurtainClose( bool autoAnim = true );
  void CurtainCloseKeep( bool autoAnim = true );

  //-------------------------------------------------------
  // @brief   照準
  //-------------------------------------------------------
  void VisibleSight( void );
  void InvisibleSight( void );

  //-------------------------------------------------------
  // @brief   シャッター
  //-------------------------------------------------------
  void ShutterOpen( void );
  void ShutterClose( void );

  bool IsShutterOpen( void );
  bool IsShutterClose( void );

  //-------------------------------------------------------
  // @brief   写真 表示・非表示
  //-------------------------------------------------------
  void VisiblePhotoImage( void );
  void InvisiblePhotoImage( void );

  //-------------------------------------------------------
  // @brief   リクエスト関数
  //-------------------------------------------------------
  // 上画面に表示する写真のリクエスト
  void RequestPhotoImage( void* texture );

  //-------------------------------------------------------
  // @brief   チェック関数
  //-------------------------------------------------------
  // カーテンが開ききったかどうか
  bool IsCurtainOpen( void ) const { return m_CurtainSeq == CURTAIN_SEQ_OPEN_KEEP; }
  // カーテンが閉まりきったかどうか
  bool IsCurtainClose( void ) const { return m_CurtainSeq == CURTAIN_SEQ_CLOSE_KEEP; }
  // ビューワーの更新が可能かどうか
  bool IsUpdateViewer( void ) { return m_CurtainSeq == CURTAIN_SEQ_OPEN_KEEP; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateCurtainAnim( void );
  void _UpdatePhotoImage( void );
  void _UpdateShutterAnim( void );

  //-------------------------------------------------------
  // @brief   カーテンアニメの再生：非公開
  //-------------------------------------------------------
  void _StartCurtainAnim( CurtainAnim animID );

  //-------------------------------------------------------
  // @brief   照準：非公開
  //-------------------------------------------------------
  void _SetSightAnim( SightType type );

  //-------------------------------------------------------
  // @brief   写真描画処理：非公開
  //-------------------------------------------------------
  void _MakeTexture( void );
  void _ReplaceTexture( void );
  void _NextTexInfo( void );
  void _MakeTexInfo( gfl2::lyt::LytResTextureInfo* dstTexInfo, void* srcTexture );
  void _SetTexture( const gfl2::lyt::LytResTextureInfo* texInfo );

  //-------------------------------------------------------
  // @brief   写真データ用バッファ 生成・破棄
  //-------------------------------------------------------
  void CreatePhotoTexture( void );
  void DeletePhotoTexture( void );

private:
  enum {
    TEXTURE_INFO_NUM = 2,
  };
private:
  void*                         m_pPhotoTexture;
  gfl2::lyt::LytPicture*        m_pPhotoPicture;
  gfl2::lyt::LytResTextureInfo  m_PhotoTexInfo[TEXTURE_INFO_NUM];

  CurtainSeq  m_CurtainSeq;
  PhotoSeq    m_PhotoSeq;
  ShutterSeq  m_ShutterSeq;

  u8    m_TexInfoFlg;

  bool  m_AutoFlg;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_UPPER_VIEW_H_INCLUDED__

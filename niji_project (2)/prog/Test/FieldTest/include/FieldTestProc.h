/**
* @brief  フィールドテスト環境構築用プロセス
* @file   FieldTestProc.h
* @author ikuta_junya
* @data   2015.02.07
*/

#if !defined( __FIELDTESTPROC_H_INCLUDED__ )
#define __FIELDTESTPROC_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include "System/include/nijiAllocator.h"
#include "MoveModel/FieldMoveModelManager.h"
#include "MoveCode/FieldMoveCodeManager.h"
#include "ActionCommand/FieldActionCommandManager.h"

// 前方宣言

class BaseCollisionScene;

namespace fld{ namespace mmodel { 
	class MoveModelManager;
}}

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
class MyRenderingPipeLine;
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN( Field );

class FieldSystem;

GFL_NAMESPACE_BEGIN( Camera );
class FieldCamera;
struct CAMERA_DATA;
GFL_NAMESPACE_END( Camera );

GFL_NAMESPACE_BEGIN( debug );
class FieldDebugMenu;
GFL_NAMESPACE_END( debug );

GFL_NAMESPACE_BEGIN( Effect );
class Manager;
class FootManager;
GFL_NAMESPACE_END( Effect );

GFL_NAMESPACE_BEGIN( Weather );
class Weather;
GFL_NAMESPACE_END( Weather );

GFL_NAMESPACE_BEGIN( Terrain );
class FieldTerrain;
GFL_NAMESPACE_END( Terrain );

GFL_NAMESPACE_END( Field );




GFL_NAMESPACE_BEGIN( Test );
GFL_NAMESPACE_BEGIN( FieldTest );
class TestEncountGrass; // エンカウント草テスト
class TestShadow;       // 影テスト
class MyRenderingPipeLine;

#if PM_DEBUG
class DebugFrustumDraw;
class DebugBoxDraw;
#endif 

// 天球テスト
// 前方参照
GFL_NAMESPACE_BEGIN( Skybox );
class Skybox;
GFL_NAMESPACE_END( Skybox );

/**
* @brief フィールドテスト環境構築用プロセスクラス
*/
class FieldTestProc : public GameSys::GameProc
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldTestProc );

public:

  //-----------------------------------------------------------------------------
  /*
  *          内部クラス宣言
  *          @class GLAllocator
  */
  //-----------------------------------------------------------------------------
  class GLAllocatorFromHeap : public gfl2::gfx::IGLAllocator
  {
  public:
    
    //----------------------------------------------------------------------------
    /**
    *  @brief  コンストラクタ
    */
    //-----------------------------------------------------------------------------
    GLAllocatorFromHeap():IGLAllocator(){}

    //----------------------------------------------------------------------------
    /**
    *  @brief  デストラクタ
    */
    //-----------------------------------------------------------------------------
    virtual ~GLAllocatorFromHeap(){}

    //----------------------------------------------------------------------------
    /**
    *  @brief  コンストラクタ
    */
    //-----------------------------------------------------------------------------
    void  init(gfl2::heap::HeapBase* pHeap){ m_pHeap = pHeap; }

    //----------------------------------------------------------------------------
    /**
    *  @brief  メインメモリの取得
    *  @param  取得したいメモリサイズ
    *  @param  アライメント
    *  @return メモリ
    */
    //-----------------------------------------------------------------------------
    virtual void*  SystemMemoryMalloc(u32 size, u32 alignment){ return GflHeapAllocMemoryAlign( m_pHeap , size , alignment ); }

    //----------------------------------------------------------------------------
    /**
    *  @brief  メインメモリの解放
    *  @param  解放したいメモリのポインタ
    */
    //-----------------------------------------------------------------------------
    virtual void  SystemMemoryFree(void* const address){ return GflHeapFreeMemory( address ); }

    //----------------------------------------------------------------------------
    /**
    *  @brief  ビデオメモリの取得
    *  @param  取得したいメモリサイズ
    *  @param  アライメント
    *  @return メモリ
    */
    //-----------------------------------------------------------------------------
    virtual void*  LocalMemoryMalloc(u32 size, u32 alignment){ return System::GflUse::m_pAllocator->LocalMemoryMalloc(size,alignment); }

    //----------------------------------------------------------------------------
    /**
    *  @brief  ビデオメモリの解放
    *  @param  解放したいメモリのポインタ
    */
    //-----------------------------------------------------------------------------
    virtual void  LocalMemoryFree(void* const address){ System::GflUse::m_pAllocator->LocalMemoryFree(address); }
    
  private:

    gfl2::heap::HeapBase *m_pHeap;
  };

public:

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  FieldTestProc( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldTestProc( void );

  /**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  /**
  * @brief   PROCのメインを行う関数
  *  1/30フレームの②フレーム目の処理です。
  *  1/60フレーム動作時には、使われません。
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief   FrameModeの取得
  * @retval   FrameMode
  *
  * フレームモードを切り替えるときは、戻り値を変えてください。
  */
  virtual GameSys::FrameMode GetFrameMode( void ) const { return GameSys::FRAMEMODE_30; }

private:

  /**
  * @brief   天球の生成
  *
  * @param   pHeap 生成に使用するヒープ (Skyboxにも渡される)
  * @param   pDeviceHeap メモリ使用量計測に使用する
  * @param   pGLAllocator 天球がリソースを作成する際に使用する
  */
  void CreateSkybox( gfl2::heap::HeapBase* pHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::gfx::IGLAllocator* pGLAllocator);

  // object
  fld::mmodel::MoveModelManager* m_moveModelManager;
  Field::Camera::CAMERA_DATA*    m_pDefaultCameraData;
  Field::Camera::FieldCamera*    m_pFieldCamera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode *m_pDrawEnvNode;

  // system
  Field::FieldSystem*                  m_pFieldSystem;



  Test::FieldTest::MyRenderingPipeLine*          m_pRenderingPipeLine;    // @todo field用に置き換える 150215 ikuta_junya
  gfl2::math::Matrix44			m_ProjMatTable[gfl2::gfx::CtrDisplayNo::NumberOf];
  gfl2::math::Matrix34			m_ViewMatTable[gfl2::gfx::CtrDisplayNo::NumberOf];
  gfl2::gfx::Texture			*m_pDepthStencilTexture;

  // skybox
  Skybox::Skybox*               m_pSkybox;

  gfl2::heap::HeapBase         *m_pFieldHeap;
  gfl2::heap::HeapBase         *m_pFieldDeviceHeap;
  System::nijiAllocator                *m_pfieldHeapAllocator;
  System::nijiAllocator                *m_pfieldDeviceHeapAllocator;
  gfl2::gfx::IGLAllocator      *m_pFieldGfglHeapOld;
  // TestEncountGrass
  TestEncountGrass * m_pTestEncountGrass;
  // TestShadow
  TestShadow * m_pTestShadow;

  // Effect
  Field::Effect::Manager*       m_pEffectManager;
  Field::Weather::Weather*      m_pWeather;
  Field::Effect::FootManager*   m_pEffectFootManager;
  
  // コリジョン
  BaseCollisionScene*              m_pCollsionScene3D;
  BaseCollisionScene*              m_pCollsionScene2D;

  // 地形
  Field::Terrain::FieldTerrain*    m_pFieldTerrain;

  // 
  Field::MoveModel::FieldMoveModelManager     m_FieldCharaModelManager;
  Field::MoveModel::FieldMoveCodeManager      m_FieldMoveCodeManager;
  Field::MoveModel::FieldActionCommandManager m_FieldActionComandManager;
  b32                                         m_isDrawRttiEditor;
  
#if PM_DEBUG
  Field::debug::FieldDebugMenu*    m_pFieldDebugMenu;


  DebugFrustumDraw * m_pDebugFrustumDraw;
  DebugBoxDraw  * m_pDebugBoxDraw;
#endif

};  // class FieldTestProc

GFL_NAMESPACE_END( FieldTest );
GFL_NAMESPACE_END( Test );


#endif // __FIELDTESTPROC_H_INCLUDED__ 
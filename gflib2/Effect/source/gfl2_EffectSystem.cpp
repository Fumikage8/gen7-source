//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_EffectSystem.cpp
 *	@brief  エフェクトシステム
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <effect/include/gfl2_EffectSystem.h>
#include <gfx/include/gfl2_GFGL.h>

#if defined(GF_PLATFORM_CTR)
#include <gfx/include/CTR/gfl2_CTRGL.h>
#include <nn.h>
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Effect)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define UNUSED_VARIABLE(x) (void)(x)

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *  [CTR]
 *    SystemのコンストラクタでCommandListにVRAM転送コマンドが詰まれます。
 *    Config.SetCommandList(NULL) => カレントのコマンドリストにつみます。
 *    Config.SetCommandList(Ptr） => 指定されたコマンドリストにコマンドをつみます。
 *    
 *    NULLの場合は、コマンドジャンプ等への対応もライブラリ内で行ないます。
 */
System::System( const Config& config )
{
  Config l_config = config;

#if defined(GF_PLATFORM_CTR)
  // CTRの場合 CommandListにVRAM転送処理が詰まれる。
  // 積み先のコマンドリストを明示的に指定されていない場合はカレントコマンドが
  // 使用されるので、対策を取る。
	nn::gr::CommandBufferJumpHelper* jh;
  jh = gfl2::gfx::GFGL::GetJumpHelper();
  gfl2::gfx::ctr::CTRCommandList  * p_cmd = gfl2::gfx::ctr::CTRGL::GetCurrentCommandList();

  if( l_config.GetCommandList() == NULL ){
    l_config.SetCommandList(p_cmd->GetGxObject());
    gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperBefore(p_cmd, jh);
  }
#endif 


  void * ptr = l_config.GetEffectHeap()->Alloc( sizeof(nw::eft::System) );
  m_pSystem = new(ptr) nw::eft::System(l_config);

#if defined(GF_PLATFORM_CTR)
  // CTRの場合 CommandListにVRAM転送処理が詰まれる。
  // 積み先のコマンドリストを明示的に指定されていない場合はカレントコマンドが
  // 使用されるので、対策を取る。
  if( l_config.GetCommandList() == NULL ){
    p_cmd->Flush3DCommandNoCacheFlush();
    gfl2::gfx::ctr::CTRGL::SyncCommandListAndJumpHelperAfter(p_cmd, jh);
  }
#endif 

  m_pHeap    = l_config.GetEffectHeap();
  m_pHeapMem = ptr;
  
  for( u32 i=0; i<GROUPID_MAX; ++i )
  {
    m_FrameRate[i] = 0.0f;
  }
}

/**
 * @brief デストラクタ
 */
System::~System()
{
  m_pSystem->~System();
  m_pHeap->Free( m_pHeapMem );
}


//===========================================================
/**
 *    リソース管理
 */
//===========================================================
//---------------------------------------------------------------------------
//! @brief        リソースを登録する。登録後もリソースはメモリ上に保持して下さい。
//!               [Win32/CTR]
//!
//! @param[in]    heap     メモリ確保先のヒープ。
//! @param[in]    resource メモリ上のエフェクトバイナリ。128Byteアラインに配置する必要があります。
//! @param[in]    resId    登録するリソースID。指定できるIDの上限数はConfigで指定します。
//! @param[in]    envInfo  環境設定情報。
//!
//! @return       なし。
//---------------------------------------------------------------------------
void System::EntryResource( Heap* heap, void* resource, s32 resId, EnvInfo* envInfo )
{
#if defined(GF_PLATFORM_CTR)
  m_pSystem->EntryResource( heap, resource, resId, envInfo );
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->EntryResource( heap, resource, resId );
#endif
}


//---------------------------------------------------------------------------
//! @brief        指定IDのリソースをエフェクトシステムから破棄します。
//!               [Win32/CTR]
//!
//! @param[in]    heap     NULLの場合は、EntryResourceで渡されたHeapからメモリ解放処理を行います。
//! @param[in]    resId    破棄するリソースID。
//!
//! @return       なし。
//---------------------------------------------------------------------------
void System::ClearResource( Heap* heap, s32 resId )
{
  m_pSystem->ClearResource( heap, resId );
}


//---------------------------------------------------------------------------
//! @brief        指定IDのリソースを取得します。
//!               [Win32/CTR]
//!
//! @param[in]    resId    リソースID。
//!
//! @return       エフェクトリソース。
//---------------------------------------------------------------------------
Resource *System::GetResource( u32 resId )
{
  return m_pSystem->GetResource( resId );
}

//===========================================================
/**
 *    エミッター生成、取得、破棄
 */
//===========================================================
//---------------------------------------------------------------------------
//! @brief        指定IDのエミッタセットを放出します。
//!               [Win32/CTR]
//!
//! @param[in]    handle           エフェクトハンドル。
//! @param[in]    pos              エミッタを配置する座標。
//! @param[in]    emitterSetID     エミッタセットID。
//! @param[in]    resourceID       リソースID。
//! @param[in]    groupID          グループID(0〜63,63はゲーム内ビューアと共用)。
//! @param[in]    emitterMask      エミッタを出すかどうかのビットフラグ。
//!
//! @return       １つでもエミッタが生成出来れば true, できなれば false。
//---------------------------------------------------------------------------
bool System::CreateEmitterSetID( Handle *handle, const math::Vector3& pos, s32 emitterSetID,
                            s32 resourceID, u8 groupID, u32 emitterMask )
{
  nw::math::VEC3 nw_pos( pos.GetX(), pos.GetY(), pos.GetZ() );
  return m_pSystem->CreateEmitterSetID( handle, nw_pos, emitterSetID, resourceID, groupID, emitterMask );
}


//---------------------------------------------------------------------------
//! @brief        指定IDのエミッタセットを放出します。引数のmtxにスケールが入っていた場合、表示が壊れます。
//!               [Win32/CTR]
//!
//! @param[in]    handle           エフェクトハンドル。
//! @param[in]    mtx              ワールド行列。
//! @param[in]    emitterSetID     エミッタセットID。
//! @param[in]    resourceID       リソースID。
//! @param[in]    groupID          グループID(0〜63,63はゲーム内ビューアと共用)。
//! @param[in]    emitterMask      エミッタを出すかどうかのビットフラグ。
//!
//! @return       １つでもエミッタが生成出来れば true, できなれば false。
//---------------------------------------------------------------------------
bool System::CreateEmitterSetID( Handle *handle , const math::Matrix34 &mtx , s32 emitterSetID ,
                            s32 resourceID, u8 groupID, u32 emitterMask )
{
    nw::math::MTX34 nw_mtx(
      mtx.GetElem(0,0), mtx.GetElem(0,1), mtx.GetElem(0,2), mtx.GetElem(0,3),
      mtx.GetElem(1,0), mtx.GetElem(1,1), mtx.GetElem(1,2), mtx.GetElem(1,3),
      mtx.GetElem(2,0), mtx.GetElem(2,1), mtx.GetElem(2,2), mtx.GetElem(2,3)
      );
  return m_pSystem->CreateEmitterSetID( handle, nw_mtx, emitterSetID, resourceID, groupID, emitterMask );
}


//---------------------------------------------------------------------------
//! @brief        再生中の全てのエミッタを削除します。
//!               [Win32/CTR]
//---------------------------------------------------------------------------
void System::KillAllEmitter()
{
  m_pSystem->KillAllEmitter();
}

//---------------------------------------------------------------------------
//! @brief        指定グループに所属するエミッタを削除します。
//!               [Win32/CTR]
//!
//! @param[in]    groupID   グループID。
//---------------------------------------------------------------------------
void System::KillEmitterGroup( u8 groupID )
{
  m_pSystem->KillEmitterGroup(groupID);
}

//---------------------------------------------------------------------------
//! @brief        引数のエミッタセットを削除します。
//!               [Win32/CTR]
//!
//! @param[in]    emitterSet   エミッタセットインスタンス。
//---------------------------------------------------------------------------
void System::KillEmitterSet( EmitterSet *emitterSet )
{
  m_pSystem->KillEmitterSet(emitterSet);
}


//---------------------------------------------------------------------------
//! @brief        引数名のエミッタセットを削除します。
//!               [Win32/CTR]
//!
//! @param[in]    emitterSetName   エミッタセット名。
//! @param[in]    resId            リソースID。
//---------------------------------------------------------------------------
void System::KillEmitterSet( const char* emitterSetName, s32 resId )
{
  m_pSystem->KillEmitterSet(emitterSetName, resId);
}

//---------------------------------------------------------------------------
//! @brief        引数のエミッタを削除します。
//!               [Win32/CTR]
//!
//! @param[in]    emitter   エミッタインスタンス。
//---------------------------------------------------------------------------
void System::KillEmitter( EmitterInstance *emitter )
{
  m_pSystem->KillEmitter(emitter);
}

//===========================================================
/**
 *    パーティクル単位での破棄
 */
//===========================================================

//---------------------------------------------------------------------------
//! @brief        指定エミッタ所属の指定パーティクルを削除します。
//!               [Win32/CTR]
//!
//!               引数に渡す ptcl がチャイルドの場合、動作が不定になりますので注意してください。
//!
//! @param[in]    emitter   削除するパーティクルが所属するエミッタ。
//! @param[in]    ptcl      削除するパーティクル。
//---------------------------------------------------------------------------
void System::KillParticle( EmitterInstance *emitter, PtclInstance* ptcl )
{
#if defined(GF_PLATFORM_CTR)
  m_pSystem->KillParticle(emitter, ptcl);
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->KillParticle(ptcl);
#endif
}

//---------------------------------------------------------------------------
//! @brief        指定エミッタ所属の指定チャイルドパーティクルを削除します。
//!               [Win32/CTR]
//!
//!               引数に渡す ptcl が親パーティクルの場合、動作が不定になりますので注意してください。
//!
//! @param[in]    emitter   削除するパーティクルが所属するエミッタ。
//! @param[in]    ptcl      削除するチャイルドパーティクル。
//---------------------------------------------------------------------------
void System::KillChildParticle( EmitterInstance *emitter, PtclInstance* ptcl )
{
#if defined(GF_PLATFORM_CTR)
  m_pSystem->KillChildParticle(emitter, ptcl);
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->KillChildParticle(ptcl);
#endif
}



//===========================================================
/**
 *    計算
 */
//===========================================================
//---------------------------------------------------------------------------
//! @brief        フレーム開始処理を行います。
//!               [Win32/CTR]
//!
//!               エフェクトのフレーム処理開始時に一度だけ呼び出してください。
//!               Win32では内部で、SwapDoubleBufferも実行します。
//---------------------------------------------------------------------------
void System::BeginFrame(void)
{
  m_pSystem->BeginFrame();

#if defined(GF_PLATFORM_CTR)
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->SwapDoubleBuffer();
#endif
}

//---------------------------------------------------------------------------
//! @brief        エミッタ/パーティクルの計算処理を行います。
//!
//!               [Win32]
//!               シングルコアで動作させる場合は、本メソッドのみをコールします。
//!               BeginFrame -> Calc( 本メソッド )の順で呼び出します。
//!
//! @param[in]    groupID           計算処理を行うグループIDのビットを立てた64ビットフラグ。
//! @param[in]    frameRate         フレームレート(標準で1.0f)。フレームレートに0.0fは指定できません。
//! @param[in]    cacheFlush        計算処理で生成したメモリ空間のCPU/GPUキャッシュフラッシュ(GXコマンドを積む)を行います。
//! @param[in]    forwardFrame      フレームを進めるかどうか。
//---------------------------------------------------------------------------
void System::Calc( u8 groupID, f32 frameRate, bool cacheFlush, bool forwardFrame )
{
#if defined(GF_PLATFORM_CTR)
  UNUSED_VARIABLE(groupID);
  UNUSED_VARIABLE(cacheFlush);
  UNUSED_VARIABLE(forwardFrame);
  if( groupID < GROUPID_MAX )
  {
    m_FrameRate[groupID] = frameRate;
  }
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->Calc( groupID, frameRate, cacheFlush, forwardFrame );
#endif

}


#if 0
// フレームのコントロールが不可能なので、非対応に
//---------------------------------------------------------------------------
//! @brief        エミッタセット単位で計算処理を行います。
//!               [Win32]
//!
//! @param[in]    emitterSet        計算処理を行うグループIDのビットを立てた64ビットフラグ。
//! @param[in]    frameRate         フレームレート(標準で1.0f)。フレームレートに0.0fは指定できません。
//! @param[in]    forwardFrame      フレームを進めるかどうか。
//---------------------------------------------------------------------------
void System::CalcEmitterSet( EmitterSet* emitterSet, f32 frameRate, bool forwardFrame )
{
#if defined(GF_PLATFORM_CTR)
  UNUSED_VARIABLE(emitterSet);
  UNUSED_VARIABLE(frameRate);
  UNUSED_VARIABLE(forwardFrame);
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->CalcEmitterSet( emitterSet, frameRate, forwardFrame );
#endif
}
#endif

//---------------------------------------------------------------------------
//! @brief        エミッタ計算処理を行います。
//!               [Win32]
//!
//                このメソッドをコールしないことでグループ単位で再生の一時停止を行えます。
//!
//! @param[in]    groupID               エミッタ計算処理を行うグループID。
//! @param[in]    frameRate             フレームレート(標準で1.0f)。フレームレートに0.0fは指定できません。
//---------------------------------------------------------------------------
void System::CalcEmitter( u8 groupID, f32 frameRate )
{
#if defined(GF_PLATFORM_CTR)
  UNUSED_VARIABLE(groupID);
  UNUSED_VARIABLE(frameRate);

  if( groupID < GROUPID_MAX )
  {
    m_FrameRate[groupID] = frameRate;
  }
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->CalcEmitter( groupID, frameRate );
#endif
}


//===========================================================
/**
 *    描画
 */
//===========================================================
//---------------------------------------------------------------------------
//! @brief        描画開始処理を行います。
//!
//!               引数で指定するビューに関するユニフォームブロックを描画コマンドに積みます。
//!
//! @param[in]    ctr_command   [CTR]コマンドリスト格納先 [Win32]NULL
//! @param[in]    proj   プロジェクションマトリクス。
//! @param[in]    view   ビューマトリクス。
//! @param[in]    camPos カメラ位置。
//! @param[in]    near    プロジェクション Near値。
//! @param[in]    far     プロジェクション Far値。
//! @retval       [CTR]格納したコマンドポインタ [Win32] NULL
//---------------------------------------------------------------------------
u32* System::BeginRender( u32* ctr_command, const math::Matrix44& proj, const math::Matrix34& view, const math::Vector3& camPos, f32 nearClip, f32 farClip )
{
  nw::math::MTX44 nw_proj(
      proj.GetElem(0,0), proj.GetElem(0,1), proj.GetElem(0,2), proj.GetElem(0,3),
      proj.GetElem(1,0), proj.GetElem(1,1), proj.GetElem(1,2), proj.GetElem(1,3),
      proj.GetElem(2,0), proj.GetElem(2,1), proj.GetElem(2,2), proj.GetElem(2,3),
      proj.GetElem(3,0), proj.GetElem(3,1), proj.GetElem(3,2), proj.GetElem(3,3)
      );
  nw::math::MTX34 nw_view(
      view.GetElem(0,0), view.GetElem(0,1), view.GetElem(0,2), view.GetElem(0,3),
      view.GetElem(1,0), view.GetElem(1,1), view.GetElem(1,2), view.GetElem(1,3),
      view.GetElem(2,0), view.GetElem(2,1), view.GetElem(2,2), view.GetElem(2,3)
      );
  nw::math::VEC3 nw_camPos( camPos.GetX(), camPos.GetY(), camPos.GetZ() );

#if defined(GF_PLATFORM_CTR)
  UNUSED_VARIABLE(nearClip);
  UNUSED_VARIABLE(farClip);
  return m_pSystem->MakeViewCommand( ctr_command, nw_proj, nw_view, nw_camPos );
#elif defined(GF_PLATFORM_WIN32)
  UNUSED_VARIABLE(ctr_command);
  //m_pSystem->SwapPreDrawDoubleBuffer();
  m_pSystem->BeginRender( nw_proj, nw_view, nw_camPos, nearClip, farClip );
  return NULL;
#endif
}

//---------------------------------------------------------------------------
//! @brief        描画処理を行います。
//!               [Win32/CTR]
//!
//! @param[in]    ctr_command   [CTR]コマンドリスト格納先 [Win32]NULL
//! @param[in]    groupID       [CTR/Win32]描画を行うグループID。
//! @param[in]    skipBehavior  [CTR]更新処理をスキップするときtrue
//! @retval       [CTR]格納したコマンドポインタ [Win32] NULL
//---------------------------------------------------------------------------
u32* System::RenderParticle( u32* ctr_command, u8 groupID, bool skipBehavior )
{
#if defined(GF_PLATFORM_CTR)
  f32 frame = m_FrameRate[groupID];
  m_FrameRate[groupID] = 0.0f;
  return m_pSystem->MakeDrawCommand( ctr_command, groupID, skipBehavior, frame );

#elif defined(GF_PLATFORM_WIN32)
  UNUSED_VARIABLE( ctr_command );
  UNUSED_VARIABLE( skipBehavior );
  m_pSystem->RenderParticle( groupID, false, NULL, NULL );
  return NULL;
#endif
}

//---------------------------------------------------------------------------
//! @brief        描画終了処理を行います。
//---------------------------------------------------------------------------
void System::EndRender()
{
#if defined(GF_PLATFORM_CTR)
#elif defined(GF_PLATFORM_WIN32)
  m_pSystem->EndRender();
#endif
}



GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(gfl2)

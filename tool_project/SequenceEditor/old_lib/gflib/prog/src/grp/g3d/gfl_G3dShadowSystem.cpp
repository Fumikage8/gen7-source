//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowSystem.cpp
 *	@brief  3D影システム
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>
#include <grp/g3d/gfl_G3dShadowSystem.h>


namespace gfl {
namespace grp {
namespace g3d {

static const char* GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME = "rom:/kawada/shadow_resource/ShadowModel.bcmdl";

// 影描画
static void BeginShadowDrawCommandInvoke(nw::gfx::RenderContext* render_context, void* work)
{
  nw::gfx::Material* shadow_setting_material = reinterpret_cast<nw::gfx::Material*>(work);
  render_context->SetMaterial(shadow_setting_material);
  render_context->ActivateContext();
  // 1Pass では RENDERMODE_IGNORE_MATERIAL を用いて以降のマテリアル設定を行いません。
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);

/*
  // この順番ならうまくいく
  render_context->SetMaterial(shadow_setting_material);
  render_context->ActivateContext();  // @note この中身を読むこと
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);

  // この順番だとうまくいかない
  render_context->SetMaterial(shadow_setting_material);
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);
  render_context->ActivateContext();
*/
}
static void EndShadowDrawCommandInvoke(nw::gfx::RenderContext* render_context, void* work)
{
  GFL_UNUSED(work);

  // レンダーモードをデフォルトに戻し、マテリアル設定が通常通り行われるようにします。
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_DEFAULT);
}


//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D影システムクラス
//=====================================

//-----------------------------------------------------------------------------
/**
 *					static関数定義
 */
//-----------------------------------------------------------------------------
ShadowSystem::ReceiverType ShadowSystem::CheckShadowReceiverModel(
    const Model*                      model,
    const s32                         material_index
)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  return CheckShadowReceiverMaterial(
      material
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief    リソースにあるテクスチャを影のテクスチャとして使ったプロジェクションシャドウのreceiverをセットアップする
 *            この影システム以外からも呼び出してもよいつくりになっている。
 *
 *  @param[in]  model
 *  @param[in]  material_index
 *  @param[in]  receiver_type
 *  @param[in]  shadow_intensity               0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
 *                                             shadow_intensity=1のときres_shadow_textureの色のままの影になる。
 *                                             shadow_intensity=0のとき影は出ない。
 *  @param[in]  projection_scene_camera_index
 *  @param[in]  texture_resource_pack          影のところは白でない色(黒とか)、影でないところは白になっている。
 *  @param[in]  texture_resource               影のところは白でない色(黒とか)、影でないところは白になっている。
 *  @param[in]  index_in_resource
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    ReceiverType receiver_type,
    f32          shadow_intensity,
    s32          projection_scene_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  SetupProjectionShadowReceiver(
    model,
    material_index,
    receiver_type,
    shadow_intensity,
    projection_scene_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

void ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    ReceiverType ,//receiver_type,  // receiver_typeが増えたら分岐しなければならない
    f32          shadow_intensity,
    s32          projection_scene_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
#if GFL_DEBUG
#define SETUP_PROJECTION_SHADOW_RECEIVER_PRINT (0)  // 1のときデバッグ出力あり、0のときなし。必ず0にしておくこと。
#else
#define SETUP_PROJECTION_SHADOW_RECEIVER_PRINT (0)  // デバッグ版でないときは必ず0にするので、ここは変更しないこと。変更するなら上の行を。
#endif

#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  {
    nw::gfx::ResModel res_model = model->GetNwModel()->GetResModel();
    const char* name = res_model.GetName();
    if( name )
    {
      GFL_PRINT("model name = %s\n", name);
    }
    else
    {
      GFL_PRINT("model name = NULL\n");
    }
  }
#endif
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // セットアップ済みか否か判定する
  ///////////////////////////////////////////////////////
  {
    // テクスチャコンバイナの4段目のコンスタントカラーが5であり、かつ、ソースRGBの1でそれを使用しているとき、セットアップ済みと判定する
    b32 b_setup = true;
    res_material = material->GetTextureCombinerResMaterial();
    nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
    if( res_tex_combiner.GetConstant() != nw::gfx::ResTextureCombiner::CONSTANT5 )
    {
      b_setup = false;
    }
    if( res_tex_combiner.GetSourceRgb(1) != nw::gfx::ResTextureCombiner::SOURCE_CONSTANT )
    {
      b_setup = false;
    }
    // セットアップ済みのとき、何もせず戻る
    if( b_setup ) return;
  }

  ///////////////////////////////////////////////////////
  // テクスチャ0を空ける
  ///////////////////////////////////////////////////////
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  {
    const char* name = res_material.GetName();
    if( name )
    {
      GFL_PRINT("material name = %s\n", name);
    }
    else
    {
      GFL_PRINT("material name = NULL\n");
    }
  }
#endif
  s32 tex_coord_count = res_material.GetTextureCoordinatorsCount();
  s32 active_tex_coord_count = res_material.GetActiveTextureCoordinatorsCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_coord_count=%d, active_tex_coord_count=%d\n", tex_coord_count, active_tex_coord_count);
  for( s32 i=0; i<tex_coord_count; ++i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(i);
    GFL_PRINT("v%d, e%d, d%d\n", res_tex_coord.IsValid(), res_tex_coord.IsEnabled(), res_tex_coord.IsDirty());
  }
#endif
  if(tex_coord_count == 0) return;  // テクスチャなしはダメ。
  GFL_ASSERT_STOP(tex_coord_count <= 3);  // @note プロシージャルテクスチャが使えるようになったら対応する必要がある。
  if(active_tex_coord_count == 0) return;  // テクスチャなしはダメ。
  GFL_ASSERT_STOP(active_tex_coord_count <= tex_coord_count);  // @note プロシージャルテクスチャが使えるようになったら対応する必要がある。
  //テコ有効だけfor( s32 i=tex_coord_count -1; i>0; --i )  // gfx_MaterialState.cpp:362 Panic:Projection mapping can use coordinator0 or coordinator1.
  for( s32 i=active_tex_coord_count -1; i>0; --i )         // 上記のようなエラーになったので、有効なテクスチャコーディネータの数だけで済ますことにした。
                                                           // 「テコ有効だけ」がその対応を行った場所に付けてあるキーワード。
  {
    nw::gfx::ResTextureCoordinator res_tex_coord_a = res_material.GetTextureCoordinators(i);
    nw::gfx::ResTextureCoordinator res_tex_coord_b = res_material.GetTextureCoordinators(i-1);
    // res_tex_coord_a = res_tex_coord_b;  // res_tex_coord_b(i-1)をres_tex_coord_a(i)にコピーする
    if( res_tex_coord_b.IsValid() )
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetScale           ( res_tex_coord_b.GetScale()            );
        res_tex_coord_a.SetRotate          ( res_tex_coord_b.GetRotate()           );
        res_tex_coord_a.SetTranslate       ( res_tex_coord_b.GetTranslate()        );
        res_tex_coord_a.SetSourceCoordinate( res_tex_coord_b.GetSourceCoordinate() );
        res_tex_coord_a.SetMappingMethod   ( res_tex_coord_b.GetMappingMethod()    );
        res_tex_coord_a.SetReferenceCamera ( res_tex_coord_b.GetReferenceCamera()  );
        res_tex_coord_a.SetMatrixMode      ( res_tex_coord_b.GetMatrixMode()       );
        res_tex_coord_a.SetDirty           ( res_tex_coord_b.IsDirty()             );  // セットしなくてもOKかも
        res_tex_coord_a.SetEnabled         ( res_tex_coord_b.IsEnabled()           );  // セットしなくてもOKかも
        res_tex_coord_a.SetTextureMatrix   ( res_tex_coord_b.GetTextureMatrix()    );
      }
      else
      {
        GFL_ASSERT_STOP( res_tex_coord_a.IsValid() );  // リソースがない場合、つくる方法がない。
      }
    }
    else
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetEnabled         ( false                                 );  // セットしなくてもOKかも
      }
      else
      {
        // 何もしない
      }
    }
  }
  //テコ有効だけactive_tex_coord_count += 1;
  //テコ有効だけif(active_tex_coord_count > 3) active_tex_coord_count = 3;  // 元々3つテクスチャを使用していたようなので1個上書きされてしまった。
  //テコ有効だけ                                                            // @note プロシージャルテクスチャが使えるようになったら対応する必要がある。
  res_material.SetActiveTextureCoordinatorsCount(active_tex_coord_count);
  res_material.SetTextureCoordinateConfig(nw::gfx::res::ResMaterial::CONFIG_0120);  // Getがない

  // テクスチャマッパー
  // 何とかnw::gfx::ResPixelBasedTextureMapperをプログラムで生成できないか試みたがダメだったので、
  // 最後のテクスチャをダミーでセットしておいてもらうことにした。
  res_material = material->GetTextureMapperResMaterial();
  s32 tex_mapper_count = res_material.GetTextureMappersCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_mapper_count=%d\n", tex_mapper_count);
  for( s32 i=0; i<tex_mapper_count; ++i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(i);
    GFL_PRINT("m%d\n", res_tex_mapper.IsValid());
    if( res_tex_mapper.IsValid() )
    {
      nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
      GFL_PRINT("s%d\n", res_tex_sampler.IsValid());
    }
  }
#endif
  GFL_ASSERT_STOP(tex_mapper_count == 3);  // @note 3固定のようだが、プロシージャルテクスチャが使えるようになったら対応する必要がある。

  b32 first = true;
  for( s32 i=3-1; i>0; --i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_a = res_material.GetTextureMappers(i);
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_b = res_material.GetTextureMappers(i-1);
    //res_tex_mapper_a = res_tex_mapper_b;  // res_tex_mapper_b(i-1)をres_tex_mapper_a(i)にコピーする
    if( res_tex_mapper_b.IsValid() )
    {
      if( res_tex_mapper_a.IsValid() )
      {
        // テクスチャ
        nw::gfx::ResTexture res_tex = res_tex_mapper_a.GetTexture();
        if( first )
        {
          if( res_tex.IsValid() )
          {
            res_tex = res_tex.Dereference();
            //res_tex.Cleanup();  // Cleanupはしないので、このテクスチャはSetupしなくてもまた使用できる
          }
          first = false;
        }
        res_tex_mapper_a.SetTexture( res_tex_mapper_b.GetTexture().Dereference() );
      
        // テクスチャサンプラー
        nw::gfx::ResTextureSampler res_tex_sampler_a = res_tex_mapper_a.GetSampler();
        nw::gfx::ResTextureSampler res_tex_sampler_b = res_tex_mapper_b.GetSampler();
        if( res_tex_sampler_b.IsValid() )
        {
          res_tex_sampler_a.SetMinFilter( res_tex_sampler_b.GetMinFilter() );
          res_tex_sampler_a.SetMagFilter( res_tex_sampler_b.GetMagFilter() );
          res_tex_sampler_a.SetSamplerType( res_tex_sampler_b.GetSamplerType() );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_a = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_a);
          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_b = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_b);
          if( res_standard_tex_sampler_b.IsValid() )
          {
            res_standard_tex_sampler_a.SetBorderColor( res_standard_tex_sampler_b.GetBorderColor() );
            res_standard_tex_sampler_a.SetWrapS      ( res_standard_tex_sampler_b.GetWrapS() );
            res_standard_tex_sampler_a.SetWrapT      ( res_standard_tex_sampler_b.GetWrapT() );
            res_standard_tex_sampler_a.SetMinLod     ( res_standard_tex_sampler_b.GetMinLod() );
            res_standard_tex_sampler_a.SetLodBias    ( res_standard_tex_sampler_b.GetLodBias() );
            res_standard_tex_sampler_a.SetMinFilter  ( res_standard_tex_sampler_b.GetMinFilter() );
            res_standard_tex_sampler_a.SetMagFilter  ( res_standard_tex_sampler_b.GetMagFilter() );
          }
          else
          {
            // 何もしない
          }
        }
        else
        {
          // 何もしない
        }
      }
      else
      {
        // 何もしない
      }
    }
    else
    {
      if( res_tex_mapper_a.IsValid() )
      {
        //res_tex_mapper_a.Cleanup();  // nw::gfx::ResPixelBasedTextureMapperをCleanupしなかったらどうなるのだろうか?
      }
    }
  }

  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ0～3段目のテクスチャの番号を1個大きくする
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  res_material = material->GetTextureCombinerResMaterial();
  nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
  GFL_ASSERT_STOP( res_fragment_shader.IsValid() );  // フラグメントシェーダは0段目は必ずあるはず
  s32 tex_combiner_count = res_fragment_shader.GetTextureCombinersCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_combiner_count=%d\n", tex_combiner_count);
#endif
  GFL_ASSERT_STOP(tex_combiner_count == 6);  // 6固定のようだ。
  STATIC_ASSERT(
         nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1
      && nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2
  );  // SOURCE_TEXTURE0からSOURCE_TEXTURE2の並び順が変わっていないことの確認のため
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  for( s32 i=0; i<6; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);
    GFL_PRINT("v%d\n", res_tex_combiner.IsValid());
  }
#endif
  for( s32 i=0; i<=3; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);

    for( s32 j=0; j<3; ++j )
    {
      nw::gfx::ResTextureCombiner::Source source;
      
      source = res_tex_combiner.GetSourceRgb(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner.SetSourceRgb(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        // テクスチャを3枚使用しているので、影テクスチャに置き換えたらおかしな結果になってしまう。
        //GFL_ASSERT_STOP(0);  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
      }
      
      source = res_tex_combiner.GetSourceAlpha(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner.SetSourceAlpha(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        // テクスチャを3枚使用しているので、影テクスチャに置き換えたらおかしな結果になってしまう。
        //GFL_ASSERT_STOP(0);  // @note 一時的にコメントアウト。コメントアウトはやめて、このASSERTが効くように戻しておいたほうがいい。
      }
    }
  }

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  // マテリアルカラー
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetReferenceCamera(projection_scene_camera_index);
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    }
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
    res_tex_coord.SetEnabled( true );  // セットしなくてもOKかも
  }

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
    
    // テクスチャ
    model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);
    
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_LINEAR_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
      
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }

  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ4段目、5段目を設定する
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  {
    // 3段目の出力結果をバッファする
    // GetBufferInputRgbとSetBufferInputRgbにはNW_MINMAX_ASSERT(index, 1, 4);という制限あり。
    // 0段目の出力結果をバッファする場合は、1段目にBUFFERINPUT_PREVIOUSを設定する。よって0は範囲外。
    // 4段目の出力結果をバッファしたものは、5段目で前段のバッファとして使っても前段の出力結果として使っても同じ。よって5は範囲外。
    res_fragment_shader.SetBufferInputRgb(4, nw::gfx::res::ResFragmentShader::BUFFERINPUT_PREVIOUS);
    //res_fragment_shader.SetBufferInputAlpha(4, nw::gfx::res::ResFragmentShader::BUFFERINPUT_PREVIOUS);  // アルファは3段目の出力結果を最終的にそのまま出すのでコメントアウト。
  }
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
    res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // テクスチャ0を反転
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
    res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
    res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
    res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
    res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
    res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
    res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }

#if 0
  //テスト記述、コメントアウトのまま
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ5段目を変更する
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }
#endif

  ///////////////////////////////////////
  // 変更を反映する
  ///////////////////////////////////////
  res_material.SetTextureCoordinatorsHash(0x0);
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
  res_material.SetTextureCombinersHash(0x0);
  res_material.SetMaterialColorHash(0x0);

#undef SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
}



//-----------------------------------------------------------------------------
/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
//-----------------------------------------------------------------------------
b32 ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SetupProjectionShadowReceiver(
    model,
    material_index,
    shadow_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // テクスチャコンバイナ
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // テクスチャ0を反転
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);


      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource, index_in_resource );
  
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionShadowReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // テクスチャコンバイナ
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // テクスチャ0を反転
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);

      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SwitchOnProjectionShadowReceiver(
    model,
    material_index,
    shadow_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource, index_in_resource );
    
    ret = true;
  }
  
  return ret;
}

b32 ShadowSystem::SwitchOffProjectionShadowReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
      
    ret = true;
  }

  return ret;
}


#if GFL_GRP_G3D_H3D_USE
// h3d
//----------------------------------------------------------------------------
/**
 *	@brief  H3d版
 */
//-----------------------------------------------------------------------------
b32 ShadowSystem::SetupProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      
      // テクスチャコンバイナ
      {
        // 4段目
        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // テクスチャ0を反転
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // テクスチャ0

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // コンスタントカラー5(0だと強度0なので光なし、1だと強度1なのでテクスチャ0の色がそのまま出る)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5段目
        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE );  // 乗算

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER);  // 3段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);  // 4段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);

        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource );
  
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      {
        // 4段目
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // テクスチャ0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // テクスチャ0を反転
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  

        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5段目
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR );
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif
      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
    ret = true; 
  }

  return ret;
}


b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource );
    
    ret = true;
  }
  
  return ret;
}

b32 ShadowSystem::SwitchOffProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
      
    ret = true;
  }

  return ret;
}

#endif // GFL_GRP_G3D_H3D_USE



//-----------------------------------------------------------------------------
/*
 *  @brief    リソースにあるテクスチャを光のテクスチャとして使ったプロジェクションライトのreceiverをOn/Offする
 *            この影システム以外からも呼び出してもよいつくりになっている。
 *            Setupはデフォルトの設定を行いOnにする
 *            Cleanupは最低限の設定戻しを行いOffにする(完全に設定を戻すことはできません)
 *            SwitchOnは設定を変更せずOnにする
 *            SwitchOffは設定を変更せずにOffにする
 *
 *  @param[in]  model
 *  @param[in]  material_index
 *  @param[in]  light_intensity                0～1  // texture_resource*light_intensity が光の色になる。
 *                                             light_intensity=1のときtexture_resourceの色のままの光になる。
 *                                             light_intensity=0のとき光は出ない。
 *  @param[in]  projection_camera_index
 *  @param[in]  texture_resource_pack          加算するところは黒でない色(白とか)、加算しないところは黒になっている。
 *  @param[in]  texture_resource               加算するところは黒でない色(白とか)、加算しないところは黒になっている。
 *  @param[in]  index_in_resource
 *
 *  @retval     b32   On/Offできたらtrueを返す。On/Offできなかったらfalseを返す(ユーザデータのProjectionTypeが間違っている場合などfalseになる)
 */
//-----------------------------------------------------------------------------
/// gfx版
b32 ShadowSystem::SetupProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SetupProjectionLightReceiver(
    model,
    material_index,
    light_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SetupProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      // 影を落とす設定を光を落とす設定に変更する

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // テクスチャコンバイナ
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // テクスチャ0
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // コンスタントカラー5(0だと強度0なので光なし、1だと強度1なのでテクスチャ0の色がそのまま出る)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
      }
      {
        // 5段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4段目の出力結果
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_ADD );  // 加算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT);

      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource, index_in_resource );
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionLightReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // テクスチャコンバイナ
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // テクスチャ0を反転
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5段目
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // 乗算
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);

      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
  
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SwitchOnProjectionLightReceiver(
    model,
    material_index,
    light_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}
b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource, index_in_resource );
    ret = true;
  }
  
  return ret;
}
b32 ShadowSystem::SwitchOffProjectionLightReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っている場合もあるので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
    ret = true;
  }

  return ret;
}


#if GFL_GRP_G3D_H3D_USE
// H3d版
b32 ShadowSystem::SetupProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      // 影を落とす設定を光を落とす設定に変更する

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      
      // テクスチャコンバイナ
      {
        // 4段目
        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // テクスチャ0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  // テクスチャ0

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // コンスタントカラー5(0だと強度0なので光なし、1だと強度1なのでテクスチャ0の色がそのまま出る)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5段目
        
        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_ADD ); // 乗算

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER);  // 3段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);  // 4段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT );
#endif
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource );
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // 光を落とす設定を影を落とす設定に戻す

      ///////////////////////////////////////////////////////
      // テクスチャコンバイナ4段目、5段目を設定する
      ///////////////////////////////////////////////////////
      
      // テクスチャコンバイナ
      {
        // 4段目
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // テクスチャ0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // テクスチャ0を反転
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // コンスタントカラー5(0だと強度0なので影なし、1だと強度1なのでテクスチャ0の影色がそのまま出る)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  

        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5段目
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER );  // 3段目の出力結果
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR );
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);          // 4段目の出力結果を反転
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // 乗算
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif
      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
  
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource );
    ret = true;
  }
  
  return ret;
}
b32 ShadowSystem::SwitchOffProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っている場合もあるので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
    ret = true;
  }

  return ret;
}
#endif // GFL_GRP_G3D_H3D_USE


// friendクラスからのみアクセスしてもよいprivateメンバ関数

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
ShadowSystem::ShadowSystem(void)
  : m_shadow_setting_resource_array(NULL),
    m_shadow_setting_model_array(NULL),
    m_shadow_group_num(0),
    m_shadow_group_array(NULL)
{
  // 何もしない
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
ShadowSystem::~ShadowSystem()
{
  Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         生成 / 破棄
 */
//-----------------------------------------------------------------------------
void ShadowSystem::Create(
    gfl::heap::NwAllocator*         heap_allocator,
    gfl::heap::NwAllocator*         device_allocator,
    const Description*              description )
{
  Description  l_description;
  if( description )
  {
    l_description = *description;
  }

  {
    m_shadow_setting_resource_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource*[SHADOW_SETTING_RESOURCE_MAX];
    
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW]->LoadAndSetup(
      device_allocator,
      GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME
    );

    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP]->LoadAndSetup(
      device_allocator,
      GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME
    );

    m_shadow_setting_model_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model*[SHADOW_SETTING_MODEL_MAX];
    
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->Create(
      heap_allocator,
      device_allocator,
      m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW],
      0
    );
    SetupProjectionShadowSettingModel( m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel() );
    
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP]->Create(
      heap_allocator,
      device_allocator,
      m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP],
      0
    );
    SetupShadowMapSettingModel( m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP]->GetNwModel() );
  }

  m_shadow_group_num = l_description.shadow_group_num;
  m_shadow_group_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup*[m_shadow_group_num];
  for( u32 i=0; i<m_shadow_group_num; ++i )
  {
    if( l_description.shadow_group_description_array )
    {
      m_shadow_group_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup(
          heap_allocator,
          device_allocator,
          &l_description.shadow_group_description_array[i] );
    }
    else
    {
      m_shadow_group_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup(
          heap_allocator,
          device_allocator );
    }
  }
}

void ShadowSystem::Destroy(void)
{
  if( m_shadow_group_array )
  {
    for( u32 i=0; i<m_shadow_group_num; ++i )
    {
      if( m_shadow_group_array[i] )
      {
        GFL_DELETE m_shadow_group_array[i];
        m_shadow_group_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_group_array;
    m_shadow_group_array = NULL;
  }
  m_shadow_group_num = 0;

  if( m_shadow_setting_model_array )
  {
    for( u32 i=0; i<SHADOW_SETTING_MODEL_MAX; ++i )
    {
      if( m_shadow_setting_model_array[i] )
      {
        GFL_DELETE m_shadow_setting_model_array[i];
        m_shadow_setting_model_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_setting_model_array;
    m_shadow_setting_model_array = NULL;
  }

  if( m_shadow_setting_resource_array )
  {
    for( u32 i=0; i<SHADOW_SETTING_RESOURCE_MAX; ++i )
    {
      if( m_shadow_setting_resource_array[i] )
      {
        GFL_DELETE m_shadow_setting_resource_array[i];
        m_shadow_setting_resource_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_setting_resource_array;
    m_shadow_setting_resource_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         影のreceiverとなるモデルをセットアップする
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  s32 shadow_camera_index = GetShadowGroupCameraIndex(shadow_group_index);
  {
    nw::gfx::res::ResTexture res_texture = GetShadowGroupRenderTarget(shadow_group_index)->GetNwResTexture();

    SetupShadowReceiverMaterial(
      material,
      res_texture,
      type,
      shadow_intensity,
      receiver_type, 
      shadow_camera_index
    );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         影のreceiverとしてセットアップ済みのモデル(SetupShadowReceiverModelが完了しているモデル)のOn/Off
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SwitchShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, b32 receive_on)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  s32 shadow_camera_index = GetShadowGroupCameraIndex(shadow_group_index);
  {
    nw::gfx::res::ResTexture res_texture = GetShadowGroupRenderTarget(shadow_group_index)->GetNwResTexture();

    if( receive_on )
    {
      SwitchOnShadowReceiverMaterial(
        material,
        res_texture,
        type,
        receiver_type
      );
    }
    else
    {
      SwitchOffShadowReceiverMaterial(
        material,
        res_texture,
        type,
        receiver_type
      );
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         影のreceiverとしてセットアップ済みのモデル(SetupShadowReceiverModelが完了しているモデル)の影の強度を設定する
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetShadowReceiverModelShadowIntensity(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity)
    // @note 使っていない引数があるので、引数を精査したほうがいい。
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  {
    SetShadowReceiverMaterialShadowIntensity(
      material,
      type,
      shadow_intensity,
      receiver_type
    );  // @note 使っていない引数があるので、引数を精査したほうがいい。
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーン描画グループを影用にセットアップする
 */
//-----------------------------------------------------------------------------
void  ShadowSystem::SetupSceneDrawGroupForShadow(const gfl::grp::g3d::ShadowGroup::Type type, gfl::grp::g3d::SceneDrawGroup* scene_draw_group) const
{
  switch( type )
  {
  case gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW:
    {
      nw::gfx::Material* material = m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel()->GetMaterial(0);
      
      scene_draw_group->SetBeginCommandInvokeCallback(
        BeginShadowDrawCommandInvoke,
        material
      );
      scene_draw_group->SetEndCommandInvokeCallback(
        EndShadowDrawCommandInvoke,
        NULL
      );
    }
    break;
  case gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP:
    {
      nw::gfx::Material* material = m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel()->GetMaterial(0);
      
      scene_draw_group->SetBeginCommandInvokeCallback(
        BeginShadowDrawCommandInvoke,
        material
      );
      scene_draw_group->SetEndCommandInvokeCallback(
        EndShadowDrawCommandInvoke,
        NULL
      );
    }
    break;
  }
}


// friendクラスからもアクセスしてはいけないprivateメンバ関数
// 便利関数や処理内容を抽出した関数

void ShadowSystem::SetupShadowReceiverMaterial(
    nw::gfx::Material*                      material,
    const nw::gfx::ResTexture               shadow_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
    const gfl::grp::g3d::ShadowGroup::Type  type,
    const f32                               shadow_intensity,  // 0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
                                                               // shadow_intensity=1のときres_shadow_textureの色のままの影になる。
                                                               // shadow_intensity=0のとき影は出ない。
    const ReceiverType                      receiver_type,  // @note receiver_typeが増えたら分岐しなければならない
    const s32                               shadow_camera_index
)
{
  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // テクスチャ0を空ける
  ///////////////////////////////////////////////////////
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  s32 tex_coord_count = res_material.GetTextureCoordinatorsCount();
  s32 active_tex_coord_count = res_material.GetActiveTextureCoordinatorsCount();
#if 0
  GFL_PRINT("tex_coord_count=%d, active_tex_coord_count=%d\n", tex_coord_count, active_tex_coord_count);
  for( s32 i=0; i<tex_coord_count; ++i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(i);
    GFL_PRINT("v%d, e%d, d%d\n", res_tex_coord.IsValid(), res_tex_coord.IsEnabled(), res_tex_coord.IsDirty());
  }
#endif
  GFL_ASSERT_STOP(tex_coord_count > 0);  // テクスチャなしはダメ。
  GFL_ASSERT_STOP(tex_coord_count <= 3);  // @note プロシージャルテクスチャが使えるようになったら対応する必要がある。
  GFL_ASSERT_STOP(active_tex_coord_count > 0);  // テクスチャなしはダメ。
  GFL_ASSERT_STOP(active_tex_coord_count <= tex_coord_count);  // @note プロシージャルテクスチャが使えるようになったら対応する必要がある。
  for( s32 i=tex_coord_count -1; i>0; --i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord_a = res_material.GetTextureCoordinators(i);
    nw::gfx::ResTextureCoordinator res_tex_coord_b = res_material.GetTextureCoordinators(i-1);
    // res_tex_coord_a = res_tex_coord_b;
    if( res_tex_coord_b.IsValid() )
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetScale           ( res_tex_coord_b.GetScale()            );
        res_tex_coord_a.SetRotate          ( res_tex_coord_b.GetRotate()           );
        res_tex_coord_a.SetTranslate       ( res_tex_coord_b.GetTranslate()        );
        res_tex_coord_a.SetSourceCoordinate( res_tex_coord_b.GetSourceCoordinate() );
        res_tex_coord_a.SetMappingMethod   ( res_tex_coord_b.GetMappingMethod()    );
        res_tex_coord_a.SetReferenceCamera ( res_tex_coord_b.GetReferenceCamera()  );
        res_tex_coord_a.SetMatrixMode      ( res_tex_coord_b.GetMatrixMode()       );
        res_tex_coord_a.SetDirty           ( res_tex_coord_b.IsDirty()             );  // セットしなくてもOKかも
        res_tex_coord_a.SetEnabled         ( res_tex_coord_b.IsEnabled()           );  // セットしなくてもOKかも
        res_tex_coord_a.SetTextureMatrix   ( res_tex_coord_b.GetTextureMatrix()    );
      }
      else
      {
        GFL_ASSERT_STOP( res_tex_coord_a.IsValid() );  // リソースがない場合、つくる方法がない。
      }
    }
    else
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetEnabled         ( false                                 );  // セットしなくてもOKかも
      }
      else
      {
        // 何もしない
      }
    }
  }
  //テコ有効だけres_material.SetActiveTextureCoordinatorsCount(active_tex_coord_count+1);
  res_material.SetTextureCoordinateConfig(nw::gfx::res::ResMaterial::CONFIG_0120);  // Getがない

  // テクスチャマッパー
  // 何とかnw::gfx::ResPixelBasedTextureMapperをプログラムで生成できないか試みたがダメだったので、
  // 最後のテクスチャをダミーでセットしておいてもらうことにした。
  res_material = material->GetTextureMapperResMaterial();
  s32 tex_mapper_count = res_material.GetTextureMappersCount();
#if 0
  GFL_PRINT("tex_mapper_count=%d\n", tex_mapper_count);
  for( s32 i=0; i<tex_mapper_count; ++i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(i);
    GFL_PRINT("m%d\n", res_tex_mapper.IsValid());
    if( res_tex_mapper.IsValid() )
    {
      nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
      GFL_PRINT("s%d\n", res_tex_sampler.IsValid());
    }
  }
#endif
  GFL_ASSERT_STOP(tex_mapper_count == 3);  // @note 3固定のようだが、プロシージャルテクスチャが使えるようになったら対応する必要がある。

  b32 first = true;
  for( s32 i=3-1; i>0; --i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_a = res_material.GetTextureMappers(i);
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_b = res_material.GetTextureMappers(i-1);
    //res_tex_mapper_a = res_tex_mapper_b;
    if( res_tex_mapper_b.IsValid() )
    {
      if( res_tex_mapper_a.IsValid() )
      {
        // テクスチャ
        nw::gfx::ResTexture res_tex = res_tex_mapper_a.GetTexture();
        if( first )
        {
          if( res_tex.IsValid() )
          {
            res_tex = res_tex.Dereference();
            res_tex.Cleanup();
          }
          first = false;
        }
        res_tex_mapper_a.SetTexture( res_tex_mapper_b.GetTexture().Dereference() );
      
        // テクスチャサンプラー
        nw::gfx::ResTextureSampler res_tex_sampler_a = res_tex_mapper_a.GetSampler();
        nw::gfx::ResTextureSampler res_tex_sampler_b = res_tex_mapper_b.GetSampler();
        if( res_tex_sampler_b.IsValid() )
        {
          res_tex_sampler_a.SetMinFilter( res_tex_sampler_b.GetMinFilter() );
          res_tex_sampler_a.SetMagFilter( res_tex_sampler_b.GetMagFilter() );
          res_tex_sampler_a.SetSamplerType( res_tex_sampler_b.GetSamplerType() );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_a = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_a);
          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_b = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_b);
          if( res_standard_tex_sampler_b.IsValid() )
          {
            res_standard_tex_sampler_a.SetBorderColor( res_standard_tex_sampler_b.GetBorderColor() );
            res_standard_tex_sampler_a.SetWrapS      ( res_standard_tex_sampler_b.GetWrapS() );
            res_standard_tex_sampler_a.SetWrapT      ( res_standard_tex_sampler_b.GetWrapT() );
            res_standard_tex_sampler_a.SetMinLod     ( res_standard_tex_sampler_b.GetMinLod() );
            res_standard_tex_sampler_a.SetLodBias    ( res_standard_tex_sampler_b.GetLodBias() );
            res_standard_tex_sampler_a.SetMinFilter  ( res_standard_tex_sampler_b.GetMinFilter() );
            res_standard_tex_sampler_a.SetMagFilter  ( res_standard_tex_sampler_b.GetMagFilter() );
          }
          else
          {
            // 何もしない
          }
        }
        else
        {
          // 何もしない
        }
      }
      else
      {
        // 何もしない
      }
    }
    else
    {
      if( res_tex_mapper_a.IsValid() )
      {
        res_tex_mapper_a.Cleanup();
      }
    }
  }

  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ0段目、1段目を空ける
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  res_material = material->GetTextureCombinerResMaterial();
  nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
  GFL_ASSERT_STOP( res_fragment_shader.IsValid() );  // フラグメントシェーダは0段目は必ずあるはず
  s32 tex_combiner_count = res_fragment_shader.GetTextureCombinersCount(); 
//  GFL_PRINT("tex_combiner_count=%d\n", tex_combiner_count);
  GFL_ASSERT_STOP(tex_combiner_count == 6);  // 6固定のようだ。
  STATIC_ASSERT(
         nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1
      && nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2
  );  // SOURCE_TEXTURE0からSOURCE_TEXTURE2の並び順が変わっていないことの確認のため
#if 0
  for( s32 i=0; i<6; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);
    GFL_PRINT("v%d\n", res_tex_combiner.IsValid());
  }
#endif
  for( s32 i=6-1-1; i>2; --i )  // GetBufferInputRgbとSetBufferInputRgbにはNW_MINMAX_ASSERT(index, 1, 4);という制限あり。
  {
    res_fragment_shader.SetBufferInputRgb(i, res_fragment_shader.GetBufferInputRgb(i-2));
    res_fragment_shader.SetBufferInputAlpha(i, res_fragment_shader.GetBufferInputAlpha(i-2));
  }
  for( s32 i=6-1; i>1; --i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner_a = res_fragment_shader.GetTextureCombiners(i);
    nw::gfx::ResTextureCombiner res_tex_combiner_b = res_fragment_shader.GetTextureCombiners(i-2);
    //res_tex_combiner_a = res_tex_combiner_b;
    res_tex_combiner_a.SetConstant    ( res_tex_combiner_b.GetConstant()     ); 
    res_tex_combiner_a.SetCombineRgb  ( res_tex_combiner_b.GetCombineRgb()   ); 
    res_tex_combiner_a.SetCombineAlpha( res_tex_combiner_b.GetCombineAlpha() ); 
    res_tex_combiner_a.SetScaleRgb    ( res_tex_combiner_b.GetScaleRgb()     ); 
    res_tex_combiner_a.SetScaleAlpha  ( res_tex_combiner_b.GetScaleAlpha()   ); 

    for( s32 j=0; j<3; ++j )
    {
      res_tex_combiner_a.SetSourceRgb   ( j, res_tex_combiner_b.GetSourceRgb(j)    ); 
      res_tex_combiner_a.SetSourceAlpha ( j, res_tex_combiner_b.GetSourceAlpha(j)  ); 
      res_tex_combiner_a.SetOperandRgb  ( j, res_tex_combiner_b.GetOperandRgb(j)   ); 
      res_tex_combiner_a.SetOperandAlpha( j, res_tex_combiner_b.GetOperandAlpha(j) ); 

      nw::gfx::ResTextureCombiner::Source source;
      
      source = res_tex_combiner_a.GetSourceRgb(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner_a.SetSourceRgb(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        GFL_ASSERT_STOP(0);
      }
      
      source = res_tex_combiner_a.GetSourceAlpha(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner_a.SetSourceAlpha(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        GFL_ASSERT_STOP(0);
      }
    }
  }

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する  // コンスタントカラー0でなくコンスタントカラー5にした
  ///////////////////////////////////////////////////////
  // マテリアルカラー
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetReferenceCamera(shadow_camera_index);
    if( type == gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW )
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    }
    else if( type == gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP )
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_SHADOW);
    }
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
    res_tex_coord.SetEnabled( true );  // セットしなくてもOKかも
  }

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
    
    // テクスチャ
    res_tex_mapper.SetTexture(shadow_res_texture);
    
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    if( type == gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW )
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
      
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
    else if( type == gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP )
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_SHADOW );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

      //nw::gfx::ResShadowTextureSampler res_shadow_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResShadowTextureSampler>(res_tex_sampler);
    }
  }

  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ0段目、1段目を設定する
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(0);
    res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 ); 
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );
  }
  {
    if( receiver_type == RECEIVER_TYPE_ADD )  // @note RECEIVER_TYPE_ADD→何かの覚え書き？
    {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(1);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
    }
    else
    {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(1);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 );
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );
    } 
  }

  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ2段目を変更する
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(2);
    for( s32 j=0; j<3; ++j )
    {
      if( res_tex_combiner.GetSourceRgb(j) == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )  // テクスチャ1→前段の出力結果
      {
        res_tex_combiner.SetSourceRgb(j, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
      }
    }
  }

#if 0 
  //テスト記述、コメントアウトのまま
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ5段目を変更する
  ///////////////////////////////////////////////////////
  // テクスチャコンバイナ
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb(0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0);
  }
#endif

  ///////////////////////////////////////
  // 変更を反映する
  ///////////////////////////////////////
  res_material.SetTextureCoordinatorsHash(0x0);
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
  res_material.SetTextureCombinersHash(0x0);
  res_material.SetMaterialColorHash(0x0);
}


ShadowSystem::ReceiverType  ShadowSystem::CheckShadowReceiverMaterial(
    const nw::gfx::Material*                //material,
)
{
  // テクスチャが2枚以下しか貼られていないか確認(プロシージャルテクスチャ除く)

  // テクスチャコンバイナが4段以下しか使われていないか確認(法線と直角方向から投影する場合に透明にする処理を入れる場合は3段以下)
  
  // コンスタントカラーが5個以下しか使われていないか確認

  // 設定がRECEIVER_TYPE_UNKNOWN以外か確認

  return RECEIVER_TYPE_UNKNOWN;
}

void ShadowSystem::SwitchOnShadowReceiverMaterial(
    nw::gfx::Material*                      ,//material,
    const nw::gfx::ResTexture               ,//shadow_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
    const gfl::grp::g3d::ShadowGroup::Type  ,//type,
    const ReceiverType                      //receiver_type
)
{
  // テクスチャを割り当てる
}

void ShadowSystem::SwitchOffShadowReceiverMaterial(
    nw::gfx::Material*                      ,//material,
    const nw::gfx::ResTexture               ,//dummy_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
    const gfl::grp::g3d::ShadowGroup::Type  ,//type,
    const ReceiverType                      //receiver_type
)
{
  // ダミーのテクスチャを割り当てる以外に何か方法はないか？
}

void ShadowSystem::SetShadowReceiverMaterialShadowIntensity(
    nw::gfx::Material*                      material,
    const gfl::grp::g3d::ShadowGroup::Type  /*type*/,
    const f32                               shadow_intensity,  // 0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
                                                               // shadow_intensity=1のときres_shadow_textureの色のままの影になる。
                                                               // shadow_intensity=0のとき影は出ない。
    const ReceiverType                      /*receiver_type*/
)  // @note 使っていない引数があるので、引数を精査したほうがいい。
{
  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する  // コンスタントカラー0でなくコンスタントカラー5にした
  ///////////////////////////////////////////////////////
  // マテリアルカラー
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////
  // 変更を反映する
  ///////////////////////////////////////
  res_material.SetMaterialColorHash(0x0);
}

//-----------------------------------------------------------------------------
/*
 *  @brief            プロジェクションシャドウ用のマテリアルとシェーダーを含むモデルのセットアップ
 *
 *  @param[in]        shadow_setting_model    セットアップするモデル
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowSettingModel( nw::gfx::Model* shadow_setting_model )
{
/*
  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);

  // 影の強度（濃さ）を設定します。
  nw::gfx::ResMaterialColor materialColor = material->GetOriginal().GetMaterialColor();
  materialColor.SetDiffuse(0.0f, 0.0f, 0.0f);

  // 影の色を設定します。
  // コンスタントカラーで影の色を設定することにした。
*/

  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  nw::gfx::ResMaterial res_material;
  res_material = material->GetMaterialColorResMaterial();
  nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
  res_mat_color.SetDiffuse(0.0f, 0.0f, 0.0f);
  res_material.SetMaterialColorHash(0x0);
}
//-----------------------------------------------------------------------------
/*
 *  @brief            シャドウマップ用のマテリアルとシェーダーを含むモデルのセットアップ
 *
 *  @param[in]        shadow_setting_model    セットアップするモデル
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupShadowMapSettingModel( nw::gfx::Model* shadow_setting_model )
{
/*
  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  
  // シャドウ用のマテリアルにシャドウモードを設定します。
  // シャドウマップにシャドウ強度とデプス情報を書き込むために必要となります。
  nw::gfx::ResFragmentOperation fragmentOperation = material->GetOriginal().GetFragmentOperation();
  fragmentOperation.SetFragmentOperationMode(nw::gfx::ResFragmentOperation::FRAGMENT_OPERATION_MODE_SHADOW);

  // シャドウオブジェクトの色を設定します。
  // シャドウマップでは必ず黒(0.0f, 0.0f, 0.0f ,0.0f) に設定する必要があります。
  // 影の色の設定ははコンバイナでコンスタントカラーによって行っています。
  nw::gfx::ResMaterialColor materialColor = material->GetOriginal().GetMaterialColor();
  materialColor.SetDiffuse(0.0f, 0.0f, 0.0f, 0.0f);
*/

  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  nw::gfx::ResMaterial res_material;
  
  res_material = material->GetFragmentOperationResMaterial();
  nw::gfx::ResFragmentOperation res_fra_ope = res_material.GetFragmentOperation();
  res_fra_ope.SetFragmentOperationMode(nw::gfx::ResFragmentOperation::FRAGMENT_OPERATION_MODE_SHADOW);

  res_material = material->GetMaterialColorResMaterial();
  nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
  res_mat_color.SetDiffuse(0.0f, 0.0f, 0.0f);

  res_material.SetFragmentOperationHash(0x0);
  res_material.SetMaterialColorHash(0x0);
}

/// gfx版
//----------------------------------------------------------------------------
/**
 *	@brief  影投影の設定を行う  GFX版
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource )
{
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, shadow_intensity.r, shadow_intensity.g, shadow_intensity.b, shadow_intensity.a);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(projection_camera_index);
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // テクスチャ
  model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

//----------------------------------------------------------------------------
/**
 *	@brief  影投影の設定の解除  GFX版
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color )
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, 0.0f, 0.0f, 0.0f, 1.0f);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(0);  // @note 0番カメラならきっと存在するだろうから使ってしまって大丈夫だろう→万一ない場合はGetActiveCameraで取得したものを使ってもらえる。
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // テクスチャ
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  {//KW13修正
    gfl::grp::g3d::Texture   *p_texture = GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTexture(); 
    if ( p_texture )
      model->SetMaterialTextureMapperTexture(material_index, 0, p_texture);
  }

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

#if GFL_GRP_G3D_H3D_USE
/// H3d版
//----------------------------------------------------------------------------
/**
 *	@brief  影投影の設定を行う  H3D版
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, H3dResTexture* texture_resource )
{
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを白にしておく。これでテクスチャの範囲外は影が描かれない。

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, shadow_intensity);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  
  // テクスチャコーディネータ
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, projection_camera_index );
    model->SetMaterialTextureCoordinatorScaleUV( material_index, 0, gfl::math::VEC2(1.0f, 1.0f) );
    model->SetMaterialTextureCoordinatorRotateUV( material_index, 0, 0.0f );
    model->SetMaterialTextureCoordinatorTranslateUV( material_index, 0, gfl::math::VEC2(0.0f, 0.0f) );
  }

  // テクスチャ
  model->SetMaterialTextureMapperTexture(material_index, 0, *texture_resource);

  // テクスチャマッパー
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );// テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

    model->SetMaterialTextureMapperMinFilter( material_index, 0, H3dTextureSamplerExConst::MIN_FILTER_NEAREST_MIPMAP_LINEAR );
    model->SetMaterialTextureMapperMagFilter( material_index, 0, H3dTextureSamplerExConst::MAG_FILTER_LINEAR );
    

    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
    model->SetMaterialTextureMapperWrapU( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
    model->SetMaterialTextureMapperWrapV( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  影投影の設定の解除  H3D版
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionShadowModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color )
{
  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, gfl::grp::ColorF32(0.0f,0.0f,0.0f,1.0f));
  

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  
  // テクスチャコーディネータ
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, 0 );
  }
  

  // テクスチャ
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  model->SetMaterialTextureMapperTexture(material_index, 0, *GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataH3dSmallTexture());

  // テクスチャマッパー
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );
    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
  }
}
#endif // GFL_GRP_G3D_H3D_USE


//----------------------------------------------------------------------------
/**
 *  @brief    リソースにあるテクスチャを光のテクスチャとして使ったプロジェクションライトのreceiverをOn/Offする
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionLighModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource )
{
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  //model->SetMaterialColorConstant(material_index, 5, light_intensity, light_intensity, light_intensity, 1.0f);
  model->SetMaterialColorConstant(material_index, 5, light_intensity.r, light_intensity.g, light_intensity.b, light_intensity.a);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(projection_camera_index);
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // テクスチャ
  model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_LINEAR_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projectionの設定を元に戻す。
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionLightModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color )
{

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, 0.0f, 0.0f, 0.0f, 1.0f);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // テクスチャコーディネータ
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(0);  // @note 0番カメラならきっと存在するだろうから使ってしまって大丈夫だろう→万一ない場合はGetActiveCameraで取得したものを使ってもらえる。
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  {//KW14修正
    // テクスチャ
    //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
    gfl::grp::g3d::Texture    *pTexture = GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTexture();
    if ( pTexture )
      model->SetMaterialTextureMapperTexture(material_index, 0, pTexture);
  }

  // テクスチャマッパー
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // テクスチャサンプラー
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

#if GFL_GRP_G3D_H3D_USE
/// H3d版
//----------------------------------------------------------------------------
/**
 *  @brief    リソースにあるテクスチャを光のテクスチャとして使ったプロジェクションライトのreceiverをOn/Offする
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionLighModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, H3dResTexture* texture_resource )
{
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // ボーダーカラークランプでテクスチャを貼っているので、ボーダーカラーを黒にしておく。これでテクスチャの範囲外は光が描かれない。
  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, light_intensity);

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  
  // テクスチャコーディネータ
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, projection_camera_index );
    model->SetMaterialTextureCoordinatorScaleUV( material_index, 0, gfl::math::VEC2(1.0f, 1.0f) );
    model->SetMaterialTextureCoordinatorRotateUV( material_index, 0, 0.0f );
    model->SetMaterialTextureCoordinatorTranslateUV( material_index, 0, gfl::math::VEC2(0.0f, 0.0f) );
  }

  // テクスチャ
  model->SetMaterialTextureMapperTexture(material_index, 0, *texture_resource);

  // テクスチャマッパー
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );// テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

    model->SetMaterialTextureMapperMinFilter( material_index, 0, H3dTextureSamplerExConst::MIN_FILTER_LINEAR_MIPMAP_LINEAR );
    model->SetMaterialTextureMapperMagFilter( material_index, 0, H3dTextureSamplerExConst::MAG_FILTER_LINEAR );
    

    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
    model->SetMaterialTextureMapperWrapU( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
    model->SetMaterialTextureMapperWrapV( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projectionの設定を元に戻す。
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionLightModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color )
{

  ///////////////////////////////////////////////////////
  // コンスタントカラー5を設定する
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, gfl::grp::ColorF32(0.0f,0.0f,0.0f,1.0f));
  

  ///////////////////////////////////////////////////////
  // テクスチャ0を設定する
  ///////////////////////////////////////////////////////
  
  // テクスチャコーディネータ
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, 0 );
  }
  

  // テクスチャ
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  model->SetMaterialTextureMapperTexture(material_index, 0, *GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataH3dSmallTexture());

  // テクスチャマッパー
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );
    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
  }
}
#endif // GFL_GRP_G3D_H3D_USE



}  // namespace g3d
}  // namespace grp
}  // namespace gfl


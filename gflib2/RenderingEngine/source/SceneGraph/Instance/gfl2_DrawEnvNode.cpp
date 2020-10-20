#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>

#include <math/include/gfl2_math_control.h>

//kawa20150507
#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#endif  // GF_PLATFORM_CTR


using namespace gfl2::gfx;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {




//kawa20150507
#if defined(GF_PLATFORM_CTR)

// CTR_SDK/include/nn/gr/CTR/gr_RenderState.h
// class WBuffer
// NW_CTR/h3d/include/nw/h3d/h3d_DrawCommandHelper.h
// struct DepthBufferingCommand
// がWBufferの参考になる。

// NW_CTR/h3d/include/nw/h3d/res/h3d_FogContent.h
// void CreateZBufLutAndApplyTo(
// void CreateWBufLutAndApplyTo(
// が使い方の参考になる。

// NW_CTR/h3d/demo/Demos/sources/scene.h
// struct OptimizingInfo
// u16 currentFogId;
// が最適化の参考になる。

// NW_CTR/h3d/include/nw/h3d/fnd/h3d_FogUtil.h
// static f32 CalcFogLutZBuf(
// static f32 CalcFogLutWBuf(
// static f32 CalcFogLutImpl(
// class ZBufferingLutIndexToViewDepth
// class WBufferingLutIndexToViewDepth
// class LutCalculatorLinear
// のコピペ。

// CTR_SDK/sources/libraries/gr/CTR/gr_Fog.cpp
// void Fog::SetTable(
// のコピペ。


//! @details :category フォグの定義
//!
//! @brief 線形関数によるフォグ係数の計算クラスです。
class LutCalculatorLinear
{
public:
  static f32 CalcLutElement(f32 z, f32 fogMaxDepth, f32 fogMinDepth, f32 fogDepth, f32 /*fogDensity*/)
  {
    if (z <= fogMinDepth)
    {
      // フォグの最小深度よりも手前なので、フラグメントのカラーを 1.0 にする。
      return 1.0f;
    }
    else if (z > fogMaxDepth)
    {
      // フォグの最大深度よりも奥なので、フラグメントのカラーを 0.0 にする。
      return 0.0f;
    }
    else
    {
      return (fogMaxDepth - z) / fogDepth;
    }
  }
};

//! @details :category 深度値の計算
//!
//! @brief Z バッファリングでの LUT のインデックス値からビュー座標系での深度値を計算するクラスです。
class ZBufferingLutIndexToViewDepth
{
public:
  //! @param[in] inverseProjectionMatrix 射影行列の逆行列です。テーブルの生成が完了するまで参照されます。
  ZBufferingLutIndexToViewDepth(const nn::math::MTX44* inverseProjectionMatrix, f32 depthRangeNear, f32 depthRangeFar)
    : m_InverseProjectionMatrix(inverseProjectionMatrix)
  {
    GFL_ASSERT(0.0f <= depthRangeNear && depthRangeNear < depthRangeFar && depthRangeFar <= 1.0f);
    m_DepthRangeNear = depthRangeNear;
    m_DepthRangeInvDepth = 1.0f / (depthRangeFar - depthRangeNear);
  }

  f32 LutIndexToDepth(u32 lutIndex) const
  {
    f32 depth = - static_cast<f32>(lutIndex) / nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM;
    depth = (depth + m_DepthRangeNear) * m_DepthRangeInvDepth;
    depth = (depth<-1.0f)?(-1.0f):((depth>0.0f)?(0.0f):(depth));  // -1.0f<=depth<=0.0f // 透視射影などの場合、far より向こう側で計算が破綻するのでクランプする。
    f32 w = m_InverseProjectionMatrix->f._32 * depth + m_InverseProjectionMatrix->f._33;
    return -(m_InverseProjectionMatrix->f._22 * depth + m_InverseProjectionMatrix->f._23) / w;
  }
private:
  const nn::math::MTX44* m_InverseProjectionMatrix;
  f32 m_DepthRangeNear;
  f32 m_DepthRangeInvDepth;
};

//! @details :category 深度値の計算
//!
//! @brief W バッファリングでの LUT のインデックス値からビュー座標系での深度値を計算するクラスです。
class WBufferingLutIndexToViewDepth
{
public:

  //! @param[in] cameraFar カメラのファークリップ値です。
  //! @param[in] cameraNear カメラのニアクリップ値です。
  //! @param[in] cameraWScale WScale です。
  WBufferingLutIndexToViewDepth(f32 cameraFar, f32 cameraNear, f32 cameraWScale)
  {
    m_Scale =
      ( cameraFar - cameraNear ) / cameraFar / cameraWScale /
      nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM;
    m_Offset = cameraNear;
  }

  f32 LutIndexToDepth(u32 lutIndex) const
  {
    return lutIndex * m_Scale + m_Offset;
  }
private:
  f32 m_Scale;
  f32 m_Offset;
};


/*

線形関数のフォグのintensityの説明
・Zバッファ
・Zフリップなし



■fogStrength=1.0fの掛かり方(標準の掛かり方)

             fogValue
                 |
全く掛からない 1 |＼
                 |  ＼
                 |    ＼
                 |      ＼
                 |        ＼
                 |          ＼
                 |            ＼
                 |              ＼
最大に掛かる   0 |                ＼
                 +---------------------
                   近               遠
                   い               い



■0.0f<fogStrength<1.0fの掛かり方

             fogValue
                 |
全く掛からない 1 |──
                 |    ──
                 |        ──
                 |            ──
                 |                ──
                 |
                 |
                 |
最大に掛かる   0 |
                 +---------------------
                   近               遠
                   い               い


fogStrength=0.0fの掛かり方(全く掛からない)

             fogValue
                 |
全く掛からない 1 |──────────
                 |
                 |
                 |
                 |
                 |
                 |
                 |
最大に掛かる   0 |
                 +---------------------
                   近               遠
                   い               い



■1.0f<fogStrength<=2.0fの掛かり方

             fogValue
                 |
全く掛からない 1 |
                 |
                 |
                 |
                 |──
                 |    ──
                 |        ──
                 |            ──
最大に掛かる   0 |                ──
                 +---------------------
                   近               遠
                   い               い
                   
                   
fogStrength=2.0fの掛かり方(全体に掛かっている)

             fogValue
                 |
全く掛からない 1 |
                 |
                 |
                 |
                 |
                 |
                 |
                 |
最大に掛かる   0 |──────────
                 +---------------------
                   近               遠
                   い               い
                   
*/


template <class FogFunction, class DepthCalculator>
f32
CalcFogLutImpl(
  f32 table[nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM],
  f32 fogMinDepth,
  f32 fogMaxDepth,
  f32 fogDensity,
  const DepthCalculator& depthCalculator,
  f32 fogIntensity)
{
  const int TABLE_LEN = nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM;

  f32 viewDepth;
  f32 fogValue;
  f32 fogDepth = fogMaxDepth - fogMinDepth;

  int i = 0;

  // 最後の差分値を求める都合で、for ループは使用しない。
  while (true)
  {
    // LUT のインデックスから深度値を計算する。
    // 計算方法は DepthCalculator によって定義される。
    // ZBuffering であれば ZBufferingLutIndexToViewDepth により、
    // WBuffering であれば WBufferingLutIndexToViewDepth により
    // ビュー座標系での深度値が計算される。
    viewDepth = depthCalculator.LutIndexToDepth(i);

    // FogFunction に従ってフォグの濃度を計算する。
    // フォグの影響が最大であるときは z = 0 になることに注意。
    fogValue = FogFunction::CalcLutElement(viewDepth, fogMaxDepth, fogMinDepth, fogDepth, fogDensity);

    if(fogIntensity <= 1.0f) fogValue = 1.0f - (1.0f - fogValue) * fogIntensity;
    else                     fogValue = fogValue * (2.0f - fogIntensity);
    if(fogValue < 0.0f)      fogValue = 0.0f;
    else if(fogValue > 1.0f) fogValue = 1.0f;

    if (i >= TABLE_LEN)
    {
      // i が TABLE_LEN であれば、すでに table の全要素に値を設定しているので、ループから抜ける。
      break;
    }

    table[i++] = fogValue;
  }

  // gr::Fog::SetTable() で使用するために、最後の差分値を計算する。
  // この時点で fogValue には table[TABLE_LEN] に相当する値が代入されている。
  return fogValue - table[TABLE_LEN - 1];
}

//! @brief フォグ参照テーブルを設定します。
//! @param[in] command 描画コマンドの書き込み先の先頭アドレスです。
//! @param[in] table 128個の参照データです。
//! @param[in] lastDiffValue 参照データの最後の差分値です。
//! @return    書き込まれた描画コマンドの終端の次のアドレスを返します。
bit32* SetTable( bit32* command,
                 const f32 table[ nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM ],
                 const f32 lastDiffValue = 0.0f )
{
  // 0x0e6
  *command++ = PICA_CMD_DATA_FOG_LUT_INDEX( 0 );
  *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FOG_LUT_INDEX );
  
  // 0x0e8
  int i = 0;
  *command++ = ( nn::gr::CTR::Float32ToUnsignedFix11( table[ i ] ) << 13 ) | 
                 nn::gr::CTR::Float32ToFix13Fraction11( table[ i + 1 ] - table[ i ] );
  *command++ = PICA_CMD_HEADER_BURST( PICA_REG_FOG_LUT_DATA0, 128 );
  
  for ( ++i; i < 0x7F; ++i )
  {
      *command++ = ( nn::gr::CTR::Float32ToUnsignedFix11( table[ i ] ) << 13 ) | 
                     nn::gr::CTR::Float32ToFix13Fraction11( table[ i + 1 ] - table[ i ] );
  }

  *command++ = ( nn::gr::CTR::Float32ToUnsignedFix11( table[ i ] ) << 13 ) |
                 nn::gr::CTR::Float32ToFix13Fraction11( lastDiffValue );
  *command++ = 0;

  return command;
}

#endif  // GF_PLATFORM_CTR


//kawa20150507
#if defined(GF_PLATFORM_CTR)
//! @brief  FogOuterParamの内容が同じならtrue、異なるならfalseを返す。
bool IsFogOuterParamSame(const DrawEnvNode::FogOuterParam& a, const DrawEnvNode::FogOuterParam& b)
{
  bool isSame = true;
  if(
       a.wScale         == b.wScale
    && a.cameraNear     == b.cameraNear
    && a.cameraFar      == b.cameraFar
    && a.depthRangeNear == b.depthRangeNear
    && a.depthRangeFar  == b.depthRangeFar
  )
  {
    for(int i=0; i<4; ++i)
    {
      for(int j=0; j<4; ++j)
      {
        if(a.projMatrix.GetElem(i,j) != b.projMatrix.GetElem(i,j))
        {
          isSame = false;
          break;
        }
      }
    }
  }
  else
  {
    isSame = false;
  }
  return isSame;
}
#endif  // GF_PLATFORM_CTR




DrawEnvNode::DepthOfFieldParam::DepthOfFieldParam():
	m_Near(0.0f),
	m_Far(0.0f),
	m_Scale(0.0f)
{
		
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  DrawEnvNodeクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
DrawEnvNode::DrawEnvNode() : NodeTrait<DrawEnvNode, CullingObjectNode>(),
	m_Priority( 0 ),
	m_IsLinkEnv( false ),
  m_FogEnable( false ),
	m_FogNearLength( 0.0f ),
	m_FogFarLength( 0.0f ),
  m_FogStrength( 1.0f ),
  m_FogColor(),
  m_GlareEnable( false ),
  m_GlareReduceNum( 0 ),
  m_GlareStrength( 0.0f ),
	m_GlareRange( 0.0f ),
	m_GlareIntensity( 0.0f ),
  m_GlareWeight()
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  , m_FogMutableCommandParam(),
  m_IsFogMutableCommandCreated(false),
  m_pFogTableCommandCache(NULL),
  m_pFogColorCommandCache(NULL)
#endif  // GF_PLATFORM_CTR
{
	std::memset( m_EnvTextureFilePath, 0, sizeof( m_EnvTextureFilePath ) );

	for( s32 i = 0; i < LightSetMax; ++i )
		m_LightSets[i] = NULL;

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  this->allocFogMutableCommandCache();
#endif  // GF_PLATFORM_CTR
}

DrawEnvNode::~DrawEnvNode()
{
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  this->freeFogMutableCommandCache();
#endif  // GF_PLATFORM_CTR
}

void DrawEnvNode::ReadDrawEnvResourceNode( const resource::DrawEnvResourceNode &rDrawEnvResourceNode )
{
	const resource::DrawEnvResourceNode::EnvData		*pData = rDrawEnvResourceNode.GetData();

	{
		math::Matrix34				mat = math::Matrix34::GetIdentity();
    mat[0][3] = pData->m_WorldPos.x;
    mat[1][3] = pData->m_WorldPos.x;
    mat[2][3] = pData->m_WorldPos.x;
		this->SetWorldMatrix( mat );
	}

	this->SetAABB( pData->m_AABB );

	this->SetFogEnable( pData->m_Fog[0].enable != 0 );
	this->SetFogNearLength( pData->m_Fog[0].nearLength );
	this->SetFogFarLength( pData->m_Fog[0].farLength );
  this->SetFogStrength( pData->m_Fog[0].strength );
  this->SetFogColor( pData->m_Fog[0].color );

	{
		DepthOfFieldParam dof;
		dof.m_Near = pData->m_FrontDOF.x;
		dof.m_Far = pData->m_FrontDOF.y;
		dof.m_Scale = pData->m_FrontDOF.z;
		this->SetFrontDOF( dof );
	}

	{
		DepthOfFieldParam dof;
		dof.m_Near = pData->m_BackDOF.x;
		dof.m_Far = pData->m_BackDOF.y;
		dof.m_Scale = pData->m_BackDOF.z;
		this->SetBackDOF( dof );
	}

	this->SetPriority( pData->m_Priority );
	this->SetIsLinkEnv( pData->m_IsLinkEnv );

  this->SetGlareEnable( pData->m_GlareFilter.enable != 0 );
  this->SetGlareReduceNum( pData->m_GlareFilter.reduceNum );
  this->SetGlareStrength( pData->m_GlareFilter.strength );
	this->SetGlareRange( pData->m_GlareFilter.range );
	this->SetGlareIntensity( pData->m_GlareFilter.intensity );
  this->SetGlareWeight( pData->m_GlareFilter.weight );

	std::memcpy( m_EnvTextureFilePath, pData->m_EnvTextureFilePath, sizeof( m_EnvTextureFilePath ) );
}

LightSetNode* DrawEnvNode::GetLightSet( s32 setNo )
{
	if ( setNo >= LightSetMax)
		return NULL;

	return m_LightSets[setNo];
}

//kawa20150507
void DrawEnvNode::UpdateFog(const FogOuterParam& param)
{
#if defined(GF_PLATFORM_CTR)
  bool isTableChanged = false;
  bool isColorChanged = false;
  
  if(!m_IsFogMutableCommandCreated)
  {
    m_FogMutableCommandParam.nearLength = m_FogNearLength;
    m_FogMutableCommandParam.farLength  = m_FogFarLength;
    m_FogMutableCommandParam.strength   = m_FogStrength;
    m_FogMutableCommandParam.outerParam = param;
    
    m_FogMutableCommandParam.color = m_FogColor;
    
    isTableChanged = true;
    isColorChanged = true;
    
    m_IsFogMutableCommandCreated = true;
  }
  else
  {
    if(
         m_FogNearLength != m_FogMutableCommandParam.nearLength
      || m_FogFarLength != m_FogMutableCommandParam.farLength
      || m_FogStrength != m_FogMutableCommandParam.strength
      || IsFogOuterParamSame(param, m_FogMutableCommandParam.outerParam) == false
    )
    {
      m_FogMutableCommandParam.nearLength = m_FogNearLength;
      m_FogMutableCommandParam.farLength  = m_FogFarLength;
      m_FogMutableCommandParam.strength   = m_FogStrength;
      m_FogMutableCommandParam.outerParam = param;
      isTableChanged = true;
    }
    if( m_FogColor != m_FogMutableCommandParam.color )
    {
      m_FogMutableCommandParam.color = m_FogColor;
      isColorChanged = true;
    }
  }

  if(isTableChanged)
  {
    // 設定値作成
    f32 fogDensity = 0.0f;  // フォグ濃度。線形関数では使わないのでてきとー。

    math::Matrix44  invProjMatrix  = m_FogMutableCommandParam.outerParam.projMatrix.Inverse();  // 射影行列の逆行列
    nn::math::MTX44 nnInvProjMatrix(
      invProjMatrix.GetElem(0,0), invProjMatrix.GetElem(0,1), invProjMatrix.GetElem(0,2), invProjMatrix.GetElem(0,3),
      invProjMatrix.GetElem(1,0), invProjMatrix.GetElem(1,1), invProjMatrix.GetElem(1,2), invProjMatrix.GetElem(1,3),
      invProjMatrix.GetElem(2,0), invProjMatrix.GetElem(2,1), invProjMatrix.GetElem(2,2), invProjMatrix.GetElem(2,3),
      invProjMatrix.GetElem(3,0), invProjMatrix.GetElem(3,1), invProjMatrix.GetElem(3,2), invProjMatrix.GetElem(3,3)
    );

    // テーブル作成
    f32 table[ nn::gr::CTR::Fog::FOG_LOOKUP_TABLE_ELEMENT_NUM ];
    f32 lastDiff = 0.0f;
    if(m_FogMutableCommandParam.outerParam.wScale == 0)
    {
      // wバッファが無効なので、Zバッファ(デプス値が大きいほど比較精度が落ちる)
      ZBufferingLutIndexToViewDepth depthCalculator(&nnInvProjMatrix, m_FogMutableCommandParam.outerParam.depthRangeNear, m_FogMutableCommandParam.outerParam.depthRangeFar);
      lastDiff = CalcFogLutImpl<LutCalculatorLinear, ZBufferingLutIndexToViewDepth>(
        table,
        m_FogMutableCommandParam.nearLength,
        m_FogMutableCommandParam.farLength,
        fogDensity,
        depthCalculator,
        m_FogMutableCommandParam.strength);
    }
    else
    {
      // Wバッファ(比較精度が一律に保たれる)
      WBufferingLutIndexToViewDepth depthCalculator(m_FogMutableCommandParam.outerParam.cameraFar, m_FogMutableCommandParam.outerParam.cameraNear, m_FogMutableCommandParam.outerParam.wScale);
      lastDiff = CalcFogLutImpl<LutCalculatorLinear, WBufferingLutIndexToViewDepth>(
        table,
        m_FogMutableCommandParam.nearLength,
        m_FogMutableCommandParam.farLength,
        fogDensity,
        depthCalculator,
        m_FogMutableCommandParam.strength);
    }

    // コマンド作成
    m_pFogTableCommandCache->ResetCurrentPtrOfBackBuffer();  // コマンドバッファの先頭に戻すことで、スワップする前にコマンドをつくり直す場合にも対応できる。決まったコマンドしか積まないのでResetしても問題ない。
    bit32* command = m_pFogTableCommandCache->GetCurrentPtrOfBackBuffer();
    command = SetTable(command, table, lastDiff);
    m_pFogTableCommandCache->SetCurrentPtrOfBackBuffer(command);
    m_pFogTableCommandCache->AddSubroutineCommandToBuckBuffer();
  }

  if(isColorChanged)
  {
    // @fix GFNMCat[2538] 0.0f～1.0fの範囲に収まらないケースがあったためClampする
    m_FogMutableCommandParam.color.x = math::Clamp( m_FogMutableCommandParam.color.x, 0.0f, 1.0f );
    m_FogMutableCommandParam.color.y = math::Clamp( m_FogMutableCommandParam.color.y, 0.0f, 1.0f );
    m_FogMutableCommandParam.color.z = math::Clamp( m_FogMutableCommandParam.color.z, 0.0f, 1.0f );

    // 設定値作成
    u8 r = static_cast<u8>(m_FogMutableCommandParam.color.x * 255.0f);
    u8 g = static_cast<u8>(m_FogMutableCommandParam.color.y * 255.0f);
    u8 b = static_cast<u8>(m_FogMutableCommandParam.color.z * 255.0f);
    
    // コマンド作成
    m_pFogColorCommandCache->ResetCurrentPtrOfBackBuffer();  // コマンドバッファの先頭に戻すことで、スワップする前にコマンドをつくり直す場合にも対応できる。決まったコマンドしか積まないのでResetしても問題ない。
    bit32* command = m_pFogColorCommandCache->GetCurrentPtrOfBackBuffer();
    *command++ = r | g << 8 | b << 16;
    *command++ = PICA_CMD_HEADER_SINGLE( PICA_REG_FOG_COLOR );
    m_pFogColorCommandCache->SetCurrentPtrOfBackBuffer(command);
    m_pFogColorCommandCache->AddSubroutineCommandToBuckBuffer();
  }
#endif  // GF_PLATFORM_CTR
}

bool DrawEnvNode::UpdateMaterialFog(ModelInstanceNode* pModelInstanceNode) const
{
  bool isUpdated = false;

#if defined(GF_PLATFORM_WIN32)
  // ModelInstanceNodeのMeshDrawTagListを使う方式に変えたほうがいいかも。
  // AABBが更新された後に呼ばれないと正しい結果が得られない。
  
  math::AABB modelWorldAABB = pModelInstanceNode->GetWorldAABB();
  math::AABB envWorldAABB   = this->GetWorldAABB();

  if( envWorldAABB.IsCross(modelWorldAABB) )
  {
    u32 materialNum = pModelInstanceNode->GetMaterialNum();
    for(u32 materialNo=0; materialNo<materialNum; ++materialNo)
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(materialNo);
      this->UpdateMaterialFog(pMaterialInstanceNode);
      isUpdated = true;
    }
  }
#endif  // GF_PLATFORM_WIN32

  return isUpdated;
}

void DrawEnvNode::UpdateMaterialFog(MaterialInstanceNode* pMaterialInstanceNode) const
{
#if defined(GF_PLATFORM_WIN32)
  // Win32版のフォグはマテリアルに設定を反映しておかなければならない。
  
  if(this->GetFogEnable())
  {
    const scenegraph::resource::MaterialResourceNode::AttributeParam* pMaterialInstanceNodeAttributeParam = pMaterialInstanceNode->GetAttributeParam();
    if(pMaterialInstanceNodeAttributeParam->m_FogEnable)
    {
      pMaterialInstanceNode->SetFogType( 1 );  // 0=NONE; 1=LINEAR;
      pMaterialInstanceNode->SetFogColor(
        gfl2::math::Vector4(
          this->GetFogColor().x,
          this->GetFogColor().y,
          this->GetFogColor().z,
          1.0f  // 1固定
        )
      );
      pMaterialInstanceNode->SetFogParam(
        gfl2::math::Vector4(
          this->GetFogStrength(),
          this->GetFogNearLength(),
          this->GetFogFarLength(),
          1.0f  // 1固定
        )
      );
    }
    else
    {
      pMaterialInstanceNode->SetFogType(0);
    }
  }
  else
  {
    pMaterialInstanceNode->SetFogType(0);
  }
#endif  // GF_PLATFORM_WIN32
}

//kawa20150507
#if defined(GF_PLATFORM_CTR)
void DrawEnvNode::allocFogMutableCommandCache(void)
{
  m_pFogTableCommandCache = gfx::ctr::CTRCommandBufferManager<TABLE_COMMAND_BUFFER_SIZE>::Create();
  m_pFogColorCommandCache = gfx::ctr::CTRCommandBufferManager<16>::Create();
}
void DrawEnvNode::freeFogMutableCommandCache(void)
{
  if(m_pFogColorCommandCache)
  {
    delete m_pFogColorCommandCache;
  }
  if(m_pFogTableCommandCache)
  {
    delete m_pFogTableCommandCache;
  }
}
#endif  // GF_PLATFORM_CTR


}}}}


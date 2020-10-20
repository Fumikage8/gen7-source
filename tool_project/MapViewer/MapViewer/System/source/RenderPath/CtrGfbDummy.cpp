//===================================================================================================================================================
// 
// 
//===================================================================================================================================================
#include <math/include/gfl2_Math.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include "System/include/RenderPath/CtrGfbDummy.h"
#if USE_AURA_SHADER

#include <nn/fs.h>
#include <nn/hio.h>


using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

//===================================================================================================================================================
// 
//  ローカル関数
// 
//===================================================================================================================================================
#define COMMAND_BUF_SIZE    (1024*6)
#define COMMAND_HEADER_SIZE (16)  // = u32 commandHeader[4] = { 0, 0, 0, 0 } 

enum {
  SHADERTYPE_VERTEX = 0,  // gfl2_ShaderResoureceNode.cpp参照
  SHADERTYPE_FRAGMENT,
  SHADERTYPE_GEOMETRY
};

struct binHeader {
  u32   topHeader_date;
  u32   topHeader_dataCount;
  u32   dummy0[2];
  char  topHeader_type[8];
  u32   topHeader_dataBlockSize;  // 全サイズ - 0x20(topHeaderサイズ)
  u32   dummy1;

  char  shaderName[64];
  u32   shaderNameHash;
  u32   shaderCount;
  u32   dummy2[2];

  u32   shaderCommandDataSize;
  u32   shaderType;
  u32   shaderCommandDataHash;
  u32   dummy3;

  char  shaderFileName[64]; // 未使用
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  メモリ操作
void memclr( c8* pDst, u32 size ){ for( u32 i=0; i<size; ++i ){ pDst[i] = 0; } }
void memcpy( c8* pDst, c8* pSrc, u32 size ){ for( u32 i=0; i<size; ++i ){ pDst[i] = pSrc[i]; } }
void namecpy( c8* pDst, c8* pSrc )
{
  for( u32 i=0; i<64; ++i ){
    if( pSrc[i] == '\0') { break; }
    pDst[i] = pSrc[i];
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  ファイル読み込み
void* ReadFile( const char* pFilePath )
{//自前データロード
  nn::fs::FileReader file( pFilePath );
  u32 fileSize = file.GetSize();
  void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
  file.Read(pShaderBinary, fileSize);
	return pShaderBinary;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  バイナリーヘッダー書き込み
void writeBinaryHeader( char* pDst, char* pDataBlock, u32 pDataSize, char* shaderName, int shaderType )
{
  binHeader header;
  memclr( (c8*)&header, sizeof(binHeader) );

  {
    header.topHeader_date = 0x15042118; // 日付(適当)
    header.topHeader_dataCount = 1;
    char type[] = { "shader" };
    namecpy( header.topHeader_type, type );
    header.topHeader_dataBlockSize = pDataSize + sizeof(binHeader) - 0x20;  // このブロック以降のサイズ。ここまでが0x20
  }
  {
    namecpy( header.shaderName, shaderName );
    util::FnvHash hash( (u8*)shaderName );
    header.shaderNameHash = hash.value;
    header.shaderCount = 1;
  }
  {
    header.shaderCommandDataSize = pDataSize;
    util::FnvHash hash( (u8*)pDataBlock, pDataSize );
    header.shaderCommandDataHash = hash.value;
    header.shaderType = shaderType;
  }
  memcpy( pDst, (c8*)&header, sizeof(binHeader) );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  データヘッダー書き込み
void writeDataHeader( char* pDst, u32 pDataSize )
{
  u32 commandHeader[4] = { pDataSize, 0, 0, 0 };
  u32* pHeader = (u32*)pDst;

  for( int i=0; i<4; ++i ){ pHeader[i] = commandHeader[i]; }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  コマンド整形
void packCommand( u32* pCom, u32* pComSize )
{
  // 追加コマンド定義(Jump後のreturnコマンド)
  u32			dummyCommand[] =  { 0x00000000, 0x00000000 };
  u32			returnCommand[] = { 0x00000001, 0x000f023d };

  // 生成されたコマンドは8byte単位
  // これに追加コマンド(Jump後のreturnコマンド)を加え、16byteAlignに整形する
  u32 sizeOrgin = *pComSize;
  u32 wp = sizeOrgin/sizeof(u32);

	if( (sizeOrgin % 16) == 0 ){
    pCom[ wp + 0 ] = dummyCommand[0];
    pCom[ wp + 1 ] = dummyCommand[1];
    pCom[ wp + 2 ] = returnCommand[0];
    pCom[ wp + 3 ] = returnCommand[1];

    *pComSize = sizeOrgin + 16;
  } else {
    pCom[ wp + 0 ] = returnCommand[0];
    pCom[ wp + 1 ] = returnCommand[1];

    *pComSize = sizeOrgin + 8;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  デバッグダンプ
void debugDump( u8* data, u32 size )
{
  u32 L = size / 16;
  u32 C = size % 16;

  for( int i=0; i<L; ++i ){
    for( int j=0; j<16; ++j ){ GFL_PRINT("[%02x]", data[i*16 + j]); }
    GFL_PRINT("\n");
  }
  if( C ){
    for( int j=0; j<C; ++j ){ GFL_PRINT("[%02x]", data[L*16 + j]); }
    GFL_PRINT("\n");
  }
}





//===================================================================================================================================================
// 
//  ctr.gfbvsh フォーマット準拠シェーダーコマンドバイナリクラス
//
//  ctr_VertexShaderAssembler32.exe(コンパイラ)
// 	ctr_VertexShaderLinker32.exe(リンカ)
//  によって作成されたシェーダーバイナリを参照し、実行用のコマンドに変換した後
//  ctr.gfbvshファイルフォーマットのデータを作成する
// 
//===================================================================================================================================================
// バイナリ読み込み
void CtrGfbvshDummy::Setup( char* filePath, char* shaderName, bool useGeometryShader )
{
  c8* pShaderBin = (c8*)ReadFile( filePath );

  // MakeFullCommandの格納先
  u32* m_fullCommand = (u32*)GLMemory::Malloc( COMMAND_BUF_SIZE, 128 );
  memclr( (c8*)m_fullCommand, COMMAND_BUF_SIZE ); 
  u32 m_fullCommandSize = 0;
  // MakeShaderCommand( isMakePrepareCommand = true)の格納先
  u32* m_shaderCommand0 = (u32*)GLMemory::Malloc( COMMAND_BUF_SIZE, 128 );
  memclr( (c8*)m_shaderCommand0, COMMAND_BUF_SIZE ); 
  u32 m_shaderCommand0Size = 0;
  // MakeShaderCommand( isMakePrepareCommand = false)の格納先
  u32* m_shaderCommand1 = (u32*)GLMemory::Malloc( COMMAND_BUF_SIZE, 128 );
  memclr( (c8*)m_shaderCommand1, COMMAND_BUF_SIZE ); 
  u32 m_shaderCommand1Size = 0;

  // 実行用コマンド作成
  {
    nn::gr::CTR::ShaderLite shaderLite; // シェーダーバイナリ設定クラス

    // シェーダーバイナリからSetupBinaryに必要なサイズを取得
		u32		workSize = shaderLite.GetRequiredBufferSize( pShaderBin );
		u32*	pWork = (u32*)GLMemory::Malloc( workSize, 128 );
    memclr( (c8*)pWork, workSize );
 
    // シェーダーバイナリをもとに、コマンド生成用の情報をセットアップ
#if 0
    shaderLite.SetupBinary( pShaderBin, 0, -1, pWork );
#else
    if( useGeometryShader == true ){
		  shaderLite.SetupBinary( pShaderBin, 0, 1, pWork );
    } else {
		  shaderLite.SetupBinary( pShaderBin, 0, -1, pWork );
    }
#endif
		u32*  pTail;

    // コマンド生成( MakeFullCommand )
    pTail = shaderLite.MakeFullCommand( m_fullCommand );
		m_fullCommandSize = (u32)pTail - (u32)m_fullCommand;
    packCommand( m_fullCommand, &m_fullCommandSize );
 
    // コマンド生成( MakeShaderCommand )
		pTail = shaderLite.MakeShaderCommand( m_shaderCommand0, true );
		m_shaderCommand0Size = (u32)pTail - (u32)m_shaderCommand0;
    packCommand( m_shaderCommand0, &m_shaderCommand0Size );

    // コマンド生成( MakeShaderCommand )
		pTail = shaderLite.MakeShaderCommand( m_shaderCommand1, false );
		m_shaderCommand1Size = (u32)pTail - (u32)m_shaderCommand1;
    packCommand( m_shaderCommand1, &m_shaderCommand1Size );

    GLMemory::Free( pWork );
  }

  // 仮想ファイルデータ作成
  {
    u32 dataBlockSize = m_fullCommandSize + m_shaderCommand0Size + m_shaderCommand1Size;
    dataBlockSize += COMMAND_HEADER_SIZE * 3;  // データヘッダー分の領域追加
    m_CtrGfbvshSize = sizeof(binHeader) + dataBlockSize;  // バイナリヘッダー分の領域追加
      
    m_CtrGfbvsh = (c8*)GLMemory::Malloc( m_CtrGfbvshSize, 128 );
    u32 wp = sizeof(binHeader);

    // MakeFullCommand の書き込み
    writeDataHeader( &m_CtrGfbvsh[wp], m_fullCommandSize );
    wp += COMMAND_HEADER_SIZE;
    memcpy( &m_CtrGfbvsh[wp], (c8*)m_fullCommand, m_fullCommandSize );
    wp += m_fullCommandSize;

    // MakeShaderCommand(pattern0)の書き込み
    writeDataHeader( &m_CtrGfbvsh[wp], m_shaderCommand0Size );
    wp += COMMAND_HEADER_SIZE;
    memcpy( &m_CtrGfbvsh[wp], (c8*)m_shaderCommand0, m_shaderCommand0Size );
    wp += m_shaderCommand0Size;

    // MakeShaderCommand(pattern1)の書き込み
    writeDataHeader( &m_CtrGfbvsh[wp], m_shaderCommand1Size );
    wp += COMMAND_HEADER_SIZE;
    memcpy( &m_CtrGfbvsh[wp], (c8*)m_shaderCommand1, m_shaderCommand1Size );
    wp += m_shaderCommand1Size;

    // モデルヘッダーの書き込み
    //int shaderType = ( useGeometryShader == true )? SHADERTYPE_GEOMETRY : SHADERTYPE_VERTEX;
    int shaderType = SHADERTYPE_VERTEX;
    writeBinaryHeader( &m_CtrGfbvsh[0], &m_CtrGfbvsh[sizeof(binHeader)], dataBlockSize, shaderName, shaderType );
  }
  GLMemory::Free( m_shaderCommand1 );
  GLMemory::Free( m_shaderCommand0 );
  GLMemory::Free( m_fullCommand );

  GLMemory::Free( pShaderBin );

  //debugDump( (u8*)m_CtrGfbvsh, sizeof(binHeader) );
}


//===================================================================================================================================================
// 
//  ctr.gfbfsh フォーマット準拠シェーダーコマンドバイナリクラス
//
//  コンバイナ操作を、実行用のコマンドに変換した後
//  ctr.gfbfshファイルフォーマットのデータを作成する
// 
//===================================================================================================================================================
//  バイナリ読み込み
//void Setup( char* filePath, char* shaderName )
void CtrGfbfshDummy::Setup( char* shaderName )
{
  //c8* pShaderBin = (c8*)ReadFile( filePath );

  // MakeCommandの格納先
  u32* m_Command = (u32*)GLMemory::Malloc( COMMAND_BUF_SIZE, 128 );
  memclr( (c8*)m_Command, COMMAND_BUF_SIZE ); 
  u32 m_CommandSize = 0;

  // 実行用コマンド作成
  u32 connectWp = 0;
  {
    //任天堂Api経由でコンバイナ計算式書き込み
    {
      nn::gr::CTR::Combiner combiner;

      // バッファカラー
      combiner.bufferColorR = m_convinerData.bufferCol.r;
		  combiner.bufferColorG = m_convinerData.bufferCol.g;
		  combiner.bufferColorB = m_convinerData.bufferCol.b;
		  combiner.bufferColorA = m_convinerData.bufferCol.a;

      // ステージデータ
      CONV_MATH_FML      mathFml;
      CONV_MATH_SRC      mathSrc;
      CONV_MATH_OPR_RGB  mathOprRgb;
      CONV_MATH_OPR_A    mathOprA;
      CONV_MATH_SCALE    mathScale;
      CONV_BUFFERINPUT   buffInput;

      for( int idxStage = 0; idxStage < CONV_STAGE_MAX; ++idxStage ){
			  nn::gr::CTR::Combiner::Stage* pStageDst = &combiner.stage[idxStage];
        convinerStage*                pStageSrc = &m_convinerData.stage[idxStage];

        // ステージ計算パラメータ
        pStageDst->rgb.combine       = mathFml.getFormula( pStageSrc->color.idxMathFormula );
        pStageDst->rgb.operand[0]    = mathOprRgb.getOperand( pStageSrc->color.idxOperandA );
			  pStageDst->rgb.operand[1]    = mathOprRgb.getOperand( pStageSrc->color.idxOperandB );
			  pStageDst->rgb.operand[2]    = mathOprRgb.getOperand( pStageSrc->color.idxOperandC );
        pStageDst->rgb.source[0]     = mathSrc.getSource( pStageSrc->color.idxSourceA );
			  pStageDst->rgb.source[1]     = mathSrc.getSource( pStageSrc->color.idxSourceB );
			  pStageDst->rgb.source[2]     = mathSrc.getSource( pStageSrc->color.idxSourceC );
        pStageDst->rgb.scale         = mathScale.getScale( pStageSrc->color.idxScale );
        pStageDst->rgb.bufferInput   = buffInput.getInput( pStageSrc->color.idxInputBufferFlag );
                
        pStageDst->alpha.combine     = mathFml.getFormula( pStageSrc->alpha.idxMathFormula );
        pStageDst->alpha.operand[0]  = mathOprA.getOperand( pStageSrc->alpha.idxOperandA );
			  pStageDst->alpha.operand[1]  = mathOprA.getOperand( pStageSrc->alpha.idxOperandB );
			  pStageDst->alpha.operand[2]  = mathOprA.getOperand( pStageSrc->alpha.idxOperandC );
        pStageDst->alpha.source[0]   = mathSrc.getSource( pStageSrc->alpha.idxSourceA );
			  pStageDst->alpha.source[1]   = mathSrc.getSource( pStageSrc->alpha.idxSourceB );
			  pStageDst->alpha.source[2]   = mathSrc.getSource( pStageSrc->alpha.idxSourceC );
        pStageDst->alpha.scale       = mathScale.getScale( pStageSrc->alpha.idxScale );
        pStageDst->alpha.bufferInput = buffInput.getInput( pStageSrc->alpha.idxInputBufferFlag );

        // ステージで使用されるコンスタントカラー
        u32Color* constantColor = &m_convinerData.constantCol[pStageSrc->constantColIdx];
        pStageDst->constColorR = constantColor->r;
			  pStageDst->constColorG = constantColor->g;
			  pStageDst->constColorB = constantColor->b;
			  pStageDst->constColorA = constantColor->a;
		  }

      // コマンド作成
      u32* pStart = &m_Command[connectWp];
      u32* pTail = combiner.MakeCommand(pStart);
		  u32 size = (u32)pTail - (u32)pStart;
      m_CommandSize += size;
    }
    connectWp = m_CommandSize/sizeof(u32);

    //任天堂Api経由でレンダーステート書き込み
    {
		  nn::gr::CTR::RenderState  renderState;

      // ブレンダー
      RDST_BLENDEQ   blendEq;
      RDST_BLENDFUNC blendFunc;
      renderState.blend.isEnable = m_renderStateData.brend.isEnable;
      renderState.blend.eqRgb = blendEq.getCommand( m_renderStateData.brend.idxEqRGB );
      renderState.blend.srcRgb = blendFunc.getCommand( m_renderStateData.brend.idxFuncSrcRGB );
      renderState.blend.dstRgb = blendFunc.getCommand( m_renderStateData.brend.idxFuncDstRGB );
      renderState.blend.eqAlpha = blendEq.getCommand( m_renderStateData.brend.idxEqA );
      renderState.blend.srcAlpha = blendFunc.getCommand( m_renderStateData.brend.idxFuncSrcA );
      renderState.blend.dstAlpha = blendFunc.getCommand( m_renderStateData.brend.idxFuncDstA );
      renderState.blend.colorR = m_renderStateData.brend.constantCol.r;
		  renderState.blend.colorG = m_renderStateData.brend.constantCol.g;
		  renderState.blend.colorB = m_renderStateData.brend.constantCol.b;
		  renderState.blend.colorA = m_renderStateData.brend.constantCol.a;

      // アルファテスト
      RDST_ALPHATEST alphaTest;
      renderState.alphaTest.isEnable = m_renderStateData.alphaTest.isEnable;
      renderState.alphaTest.func = alphaTest.getCommand( m_renderStateData.alphaTest.idxFunc );
		  renderState.alphaTest.refValue = m_renderStateData.alphaTest.refValue.value;

      // デプステスト
      RDST_DEPTHTEST depthTest;
      renderState.depthTest.isEnable = m_renderStateData.depthTest.isEnable;
      renderState.depthTest.func = depthTest.getCommand( m_renderStateData.depthTest.idxFunc );
      renderState.depthTest.isEnableWrite = m_renderStateData.depthTest.isEnableWrite;

      // ステンシルテスト
      RDST_STENCILTEST stencilTest;
      RDST_STENCILOP   stencilOp;
      renderState.stencilTest.isEnable = m_renderStateData.stencilTest.isEnable;
      renderState.stencilTest.maskOp = m_renderStateData.stencilTest.maskOp;
      renderState.stencilTest.func = stencilTest.getCommand( m_renderStateData.stencilTest.idxFunc );
      renderState.stencilTest.ref = m_renderStateData.stencilTest.refValue;
      renderState.stencilTest.mask = m_renderStateData.stencilTest.mask;
      renderState.stencilTest.opFail = stencilOp.getCommand( m_renderStateData.stencilTest.idxOpFail );
      renderState.stencilTest.opZFail = stencilOp.getCommand( m_renderStateData.stencilTest.idxOpZFail );
      renderState.stencilTest.opZPass = stencilOp.getCommand( m_renderStateData.stencilTest.idxOpZPass );

      // カリングモード
      RDST_CULL cullMode;
      renderState.cullingTest.isEnable = m_renderStateData.cullMode.isEnable;
      renderState.cullingTest.cullFace = cullMode.getCommand( m_renderStateData.cullMode.mode );

      // ポリゴンオフセット
      renderState.wBuffer.isEnablePolygonOffset = m_renderStateData.polyOffs.isEnable;
		  renderState.wBuffer.polygonOffsetUnit = m_renderStateData.polyOffs.offset;

      //カラーマスク初期化
      renderState.colorMask = nn::gr::CTR::RenderState::COLOR_MASK_RGBA;

      // コマンド作成
      u32* pStart = &m_Command[connectWp];
      u32* pTail = renderState.MakeCommand(pStart);
		  u32 size = (u32)pTail - (u32)pStart;
      m_CommandSize += size;
	  }
    connectWp = m_CommandSize/sizeof(u32);

    //任天堂Api経由でテクスチャステート（サンプラー）書き込み
    {
		  nn::gr::CTR::Texture texture;

      TXST_FILTER texFilter;
      TXST_WRAP   texWrap;

      // テクスチャスロット0
      texture.unit0.texType = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;
			texture.unit0.format = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;
      texture.unit0.physicalAddr = 0;   // 描画時に上書きするので、ここでの設定はなし
			texture.unit0.width = 0;          // 描画時に上書きする、ここでの設定はなし
			texture.unit0.height = 0;         // 描画時に上書きする、ここでの設定はなし
			texture.unit0.lodBias = 0;        // ミップ初期値は０とする。これはテクスチャコマンドに積む
			texture.unit0.minLodLevel = 0;
			texture.unit0.maxLodLevel = 0;
      texture.unit0.magFilter = texFilter.getCommand_Mag( m_textureStateData.slot[0].idxMinMagFilter );
      texture.unit0.minFilter = texFilter.getCommand_Min( m_textureStateData.slot[0].idxMinMagFilter );
      texture.unit0.wrapS = texWrap.getCommand( m_textureStateData.slot[0].idxWrapU );
			texture.unit0.wrapT = texWrap.getCommand( m_textureStateData.slot[0].idxWrapV );
      texture.unit0.borderColorR = m_textureStateData.slot[0].borderCol.r;
      texture.unit0.borderColorG = m_textureStateData.slot[0].borderCol.g;
      texture.unit0.borderColorB = m_textureStateData.slot[0].borderCol.b;
      texture.unit0.borderColorA = m_textureStateData.slot[0].borderCol.a;

      // テクスチャスロット1
      texture.unit1.texType = PICA_DATA_TEXTURE1_SAMPLER_TYPE_TEXTURE_2D;
			texture.unit1.format = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;
      texture.unit1.physicalAddr = 0;   // 描画時に上書きするので、ここでの設定はなし
			texture.unit1.width = 0;          // 描画時に上書きする、ここでの設定はなし
			texture.unit1.height = 0;         // 描画時に上書きする、ここでの設定はなし
			texture.unit1.lodBias = 0;        // ミップ初期値は０とする。これはテクスチャコマンドに積む
			texture.unit1.minLodLevel = 0;
			texture.unit1.maxLodLevel = 0;
      texture.unit1.magFilter = texFilter.getCommand_Mag( m_textureStateData.slot[1].idxMinMagFilter );
      texture.unit1.minFilter = texFilter.getCommand_Min( m_textureStateData.slot[1].idxMinMagFilter );
      texture.unit1.wrapS = texWrap.getCommand( m_textureStateData.slot[1].idxWrapU );
			texture.unit1.wrapT = texWrap.getCommand( m_textureStateData.slot[1].idxWrapV );
      texture.unit1.borderColorR = m_textureStateData.slot[1].borderCol.r;
      texture.unit1.borderColorG = m_textureStateData.slot[1].borderCol.g;
      texture.unit1.borderColorB = m_textureStateData.slot[1].borderCol.b;
      texture.unit1.borderColorA = m_textureStateData.slot[1].borderCol.a;

      // テクスチャスロット0
      texture.unit2.texType = PICA_DATA_TEXTURE2_SAMPLER_TYPE_TEXTURE_2D;
			texture.unit2.format = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;
      texture.unit2.physicalAddr = 0;   // 描画時に上書きするので、ここでの設定はなし
			texture.unit2.width = 0;          // 描画時に上書きする、ここでの設定はなし
			texture.unit2.height = 0;         // 描画時に上書きする、ここでの設定はなし
			texture.unit2.lodBias = 0;        // ミップ初期値は０とする。これはテクスチャコマンドに積む
			texture.unit2.minLodLevel = 0;
			texture.unit2.maxLodLevel = 0;
      texture.unit2.magFilter = texFilter.getCommand_Mag( m_textureStateData.slot[2].idxMinMagFilter );
      texture.unit2.minFilter = texFilter.getCommand_Min( m_textureStateData.slot[2].idxMinMagFilter );
      texture.unit2.wrapS = texWrap.getCommand( m_textureStateData.slot[2].idxWrapU );
			texture.unit2.wrapT = texWrap.getCommand( m_textureStateData.slot[2].idxWrapV );
      texture.unit2.borderColorR = m_textureStateData.slot[2].borderCol.r;
      texture.unit2.borderColorG = m_textureStateData.slot[2].borderCol.g;
      texture.unit2.borderColorB = m_textureStateData.slot[2].borderCol.b;
      texture.unit2.borderColorA = m_textureStateData.slot[2].borderCol.a;

      // コマンド作成
      u32* pStart = &m_Command[connectWp];
      u32* pTail = texture.MakeCommand(pStart);
		  u32 size = (u32)pTail - (u32)pStart;
      m_CommandSize += size;
	  }
  }
  packCommand( m_Command, &m_CommandSize );

  // 仮想ファイルデータ作成
  {
    u32 dataBlockSize = m_CommandSize;
    m_CtrGfbfshSize = sizeof(binHeader) + dataBlockSize;  // バイナリヘッダー分の領域追加
      
    m_CtrGfbfsh = (c8*)GLMemory::Malloc( m_CtrGfbfshSize, 128 );
    u32 wp = sizeof(binHeader);

    // Command の書き込み
    memcpy( &m_CtrGfbfsh[wp], (c8*)m_Command, m_CommandSize );

    // モデルヘッダーの書き込み
    writeBinaryHeader( &m_CtrGfbfsh[0], &m_CtrGfbfsh[sizeof(binHeader)], dataBlockSize, shaderName, SHADERTYPE_FRAGMENT );
  }

  GLMemory::Free( m_Command );

  //GLMemory::Free( pShaderBin );
}

#endif //USE_AURA_SHADER

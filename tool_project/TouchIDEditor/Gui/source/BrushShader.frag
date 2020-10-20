uniform sampler2D Texture[3];//テクスチャユニット

// -------------------------------------------------------------
// 頂点プログラムからフラグメントプログラムに渡すデータ
// -------------------------------------------------------------
varying vec4 outUv;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform vec4 cPsBrushColor;

// ------------------------------------------------------------
// フラグメントシェーダプログラム
// ------------------------------------------------------------
void main (void)
{
	vec4				outColor = cPsBrushColor;

	outColor.w = texture2D( Texture[0], outUv.xy ).w;

	gl_FragColor = outColor;
}

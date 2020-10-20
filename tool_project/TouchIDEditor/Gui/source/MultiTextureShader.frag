uniform sampler2D Texture[3];//テクスチャユニット

// -------------------------------------------------------------
// 頂点プログラムからフラグメントプログラムに渡すデータ
// -------------------------------------------------------------
varying vec4 outUv;

// -------------------------------------------------------------
// グローバル変数
// -------------------------------------------------------------
uniform vec4 cPsBlendAlpha;

// ------------------------------------------------------------
// フラグメントシェーダプログラム
// ------------------------------------------------------------
void main (void)
{
	vec4	outColor;
	vec4	color[3];

	for( int i = 0; i < 3; i ++ )
	{
		color[i] = texture2D( Texture[i], outUv.xy );
	}

	outColor.xyz = (color[1].xyz * color[1].w) + (color[0].xyz * (1 - color[1].w));
	outColor.xyz = (color[2].xyz * color[2].w) + (outColor.xyz * (1 - color[2].w));
	outColor.w = color[0].w;

	outColor.xyz = (outColor.xyz * cPsBlendAlpha.w) + (color[0].xyz * (1 - cPsBlendAlpha.w));

	gl_FragColor = outColor;
}

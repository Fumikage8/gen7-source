// 定数バッファのデータ定義① 逆ビューポート行列
cbuffer cbNeverChanges : register(b0) { // 常にスロット「0」を使う
    matrix InvViewPort;
};

// 頂点シェーダの入力データ定義
struct VS_INPUT {
    float4 Pos : POSITION0;   // 頂点座標(モデル座標系)
	float4 Uv  : TEXCOORD0;
    float4 Col : COLOR0;      // 頂点色
};

// ジオメトリシェーダの入力データ定義
struct GS_INPUT {
    float4 Pos : POSITION0;   // 頂点座標(モデル座標系)
	float4 Uv  : TEXCOORD0;
    float4 Col : COLOR0;      // 頂点色
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(スクリーン座標座標)
	float4 Uv   : TEXCOORD0;
    float4 Col  : COLOR;       // 頂点色
};

// ジオメトリシェーダの入力・出力データ定義

// FIXME: 固定長にするのは危険
Texture2D		g_Texture[5] : register( t0 );
SamplerState	g_Sampler[5] : register( s0 );

// 頂点シェーダの関数
GS_INPUT VS(VS_INPUT input) {
    GS_INPUT output;

    // 頂点座標　モデル座標系→ビュー座標系
    output.Pos = input.Pos;
	//output.Pos = mul(input.Pos, InvViewPort);
	        
    // 頂点色
    output.Col = input.Col;

	output.Uv = input.Uv;
    // 出力
    return output;
}

[maxvertexcount(4)]	// ジオメトリシェーダで出力する最大頂点数
void GS(point GS_INPUT input[1],
		  inout TriangleStream<PS_INPUT> stream
		  )
{
	PS_INPUT output;

	float4 position = {input[0].Pos.x, input[0].Pos.y, 0.0f, 1.0f};
	float2 diff = {input[0].Pos.z * InvViewPort[0][0], input[0].Pos.w * InvViewPort[1][1]};
	position = mul(position, InvViewPort);

	// 0番目: 基準となる頂点
	output.Pos = position;
	output.Uv = input[0].Uv;
	output.Col = input[0].Col;
	stream.Append(output);

	// 1番目: 右に伸ばした頂点
	output.Pos.x += diff.x;
	output.Uv.x += output.Uv.z;
	stream.Append(output);

	// 2番目: 下に伸ばした頂点
	output.Pos = position;
	output.Pos.y += diff.y;
	output.Uv = input[0].Uv;
	output.Uv.y += output.Uv.w;
	stream.Append(output);

	// 3番目: 右下に伸ばした頂点
	output.Pos.x += diff.x;
	output.Uv.x += output.Uv.z;
	stream.Append(output);

	stream.RestartStrip();
}

// ピクセル シェーダの関数
float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 color;

	// UV座標のvの要素は, テクスチャが5枚だったら,
	// 0.0f - 5.0fの範囲で受け取るので, このようにして端数を弾き,
	// テクスチャのインデックス番号と0.0f - 1.0f範囲内のv座標を受け取る
	int index = (int)floor(input.Uv.y);
	input.Uv.y -= (float)index;

	// color = index * g_Texture[index].Sample( g_Sampler[index], input.Uv.xy) * input.Col;
	// ↑本当はこう書きたかったが, 動的配列アクセスをサポートしていないので,
	// if文のオンパレードになっている.
	// TODO : ボリュームテクスチャに対応したら切り替える
	if(index == 0)
	{
		color = g_Texture[0].Sample( g_Sampler[0], input.Uv.xy) * input.Col;
	}
	else if(index == 1)
	{
		color = g_Texture[1].Sample( g_Sampler[1], input.Uv.xy) * input.Col;
	}
	else if(index == 2)
	{
		color = g_Texture[2].Sample( g_Sampler[2], input.Uv.xy) * input.Col;
	}
	else if(index == 3)
	{
		color = g_Texture[3].Sample( g_Sampler[3], input.Uv.xy) * input.Col;
	}
	else if(index == 4)
	{
		color = g_Texture[4].Sample( g_Sampler[4], input.Uv.xy) * input.Col;
	}
	return color;
}

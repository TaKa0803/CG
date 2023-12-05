#include"Particle.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    int32_t enableTexture;
    float32_t enableHalfLambert;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    
    
    //テクスチャ関係の処理
    float32_t4 textureColor;   
    if (gMaterial.enableTexture != 0)
    {
        textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    }
    else
    {
        textureColor = gMaterial.color;
    }
    
    
 
    output.color = gMaterial.color * textureColor;
    //α値が0なら棄却
    if (output.color.a == 0.0) {
        discard;
    }
    output.color = gMaterial.color * textureColor;
    
    
    return output;
}
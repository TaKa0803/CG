#include"Particle.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct Material {
    float32_t4 color;
    
    int32_t enableLighting;
    
    float32_t4x4 uvTransform;
    
    int32_t enableTexture;
    
    int32_t enableHalfLambert;
    
    float32_t discardNum;
    
    
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

struct DirectionalLight {
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    //float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
    
    
    if (gMaterial.enableTexture != 0) {
    }
    else {
        textureColor = gMaterial.color;
    }
    
    
   
    output.color = gMaterial.color * textureColor;
    
    
    //textureのα値が0の時Pixelを棄却
    if (output.color.a <= gMaterial.discardNum) {
        discard;
    }
    
    return output;
}
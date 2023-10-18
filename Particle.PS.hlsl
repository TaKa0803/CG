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
ConstantBuffer<Material> gMaterial : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);


PixelShaderOutput main(VertexShaderOutput input)
{
    //float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
    
    if (gMaterial.enableTexture != 0)
    {
    }
    else
    {
        textureColor = gMaterial.color;
    }
    
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0)
    {
        float cos;
        
        
             //Half Lambert
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        cos = pow(NdotL * 0.5f + 0.5f, 1.0f);
              
            //Lambertian Reflectance
            //cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        //計算
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}
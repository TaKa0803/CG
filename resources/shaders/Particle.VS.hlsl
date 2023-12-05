#include"Particle.hlsli"

struct TransformationMatrix {
    float32_t4x4 WVP;
    float32_t4x4 World;
	
};

StructuredBuffer<TransformationMatrix> gTransformationMatrixes : register(t0);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input,uint32_t instanced : SV_InstanceID) {
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrixes[instanced].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrixes[instanced].World));
    
    return output;
}
#include "Material.h"

Material::Material(XMVECTOR tint, float spec, SimplePixelShader* pixShd, SimpleVertexShader* verShd, 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffText, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nMap,
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampOpt)
{
    XMStoreFloat3(&colorTint, tint);

    specular = spec;

    pixelShader = pixShd;
    vertexShader = verShd;

    diffuseTexture = diffText;
    normalMap = nMap;

}

SimplePixelShader* Material::GetPixelShader()
{
    return pixelShader;
}

SimpleVertexShader* Material::GetVertexShader()
{
    return vertexShader;
}

void Material::SetColorTint(XMVECTOR tint)
{
    XMStoreFloat3(&colorTint, tint);
}

XMFLOAT3 Material::GetColorTint()
{
    return colorTint;
}

void Material::SetSpec(float spec)
{
    specular = spec;
}

float Material::GetSpec()
{
    return specular;
}

void Material::SetDiffuseTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffText)
{
    diffuseTexture = diffText;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetDiffuseTexure()
{
    return diffuseTexture;
}

#pragma once
#include "Material.h"
#include "Model.h"
#include "SkinnedModel.h"
#include "RenderTexture.h"
#include "..\Scenegraph And Physics\Transform.h"

//TODO: Dispose Model* properly, currently leaking memory
class SkinnedModel;
class Renderable
{
private:
	Material* m_material = nullptr;
	ModelInterface* m_model = nullptr;
	bool isSkinnedModel = false;
	friend class Animator;
public:
	Renderable();
	Renderable(Material* mat, Model* model);
	Renderable(Material* mat, SkinnedModel* model);
	~Renderable();
	bool getIfSkinnedModel()const;
	Material* getMaterial() const;
	ModelInterface* getModel() const;
	void draw(ID3D11DeviceContext* context, const XMMATRIX& viewProjectionMatrix);
	void overrideMaterialDraw(ID3D11DeviceContext* context, Material* material, const XMMATRIX& viewProjectionMatrix);
	void overridePixelShaderDraw(ID3D11DeviceContext* context, PixelShader* material, const XMMATRIX& viewProjectionMatrix);
	void overrideWorldMatrixDraw(ID3D11DeviceContext* context, const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void overrideWorldMatrixAndMaterial(ID3D11DeviceContext* context, Material* material, const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);
	void setShadowMapTexture(RenderTexture* texture);
	Transform transform;
};
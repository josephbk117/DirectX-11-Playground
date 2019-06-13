#pragma once
#include "Model.h"
#include "SkinnedMesh.h"
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
#include "Animator.h"
class Animator;

class Joint
{
private:
	unsigned int jointId = 0;
	std::string name = "";
	std::vector<Joint> children;
	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX localBindTransform;
	DirectX::XMMATRIX inverseBindTransform;
public:
	Joint() = default;
	Joint(unsigned int id, const std::string& name, const DirectX::XMMATRIX& bindLocalTransform)
	{
		jointId = id;
		this->name = name;
		this->localBindTransform = bindLocalTransform;
	}
	void addChild(const Joint& child)
	{
		children.push_back(child);
	}
	DirectX::XMMATRIX getTransform()const
	{
		return transform;
	}
	void setTransform(const DirectX::XMMATRIX& transform)
	{
		this->transform = transform;
	}
};

class JointTransform
{
private:
	DirectX::XMFLOAT3 pos = { 0, 0, 0 };
	DirectX::XMVECTOR rotation = { 0,0,0,0 };
public:
	JointTransform() = default;
	JointTransform(const DirectX::XMFLOAT3& position, const DirectX::XMVECTOR& quat) : pos(position), rotation(quat) {}
	XMMATRIX getLocalTransform() const
	{
		XMMATRIX mat = DirectX::XMMatrixIdentity();
		mat = mat * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		mat = mat * DirectX::XMMatrixRotationQuaternion(rotation);
		return mat;
	}

	static JointTransform interpolate(const JointTransform& frameA, const JointTransform& frameB, float progress)
	{
		DirectX::XMFLOAT3 interpPos;
		DirectX::XMStoreFloat3(&interpPos, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&frameA.pos), DirectX::XMLoadFloat3(&frameB.pos), progress));
		DirectX::XMVECTOR quat = DirectX::XMQuaternionSlerp(frameA.rotation, frameB.rotation, progress);
		return JointTransform(interpPos, quat);
	}
};

//class KeyFrame
//{
//private:
//	float timeStamp = 0.0f;
//	std::map<std::string, JointTransform> pose;
//public:
//	KeyFrame(float timeStamp, const std::map<std::string, JointTransform>& jointKeyFrames)
//	{
//		this->timeStamp = timeStamp;
//		pose = jointKeyFrames;
//	}
//	float getTimeStamp()
//	{
//		return timeStamp;
//	}
//	std::map <std::string, JointTransform> getJointKeyFrames()
//	{
//		return pose;
//	}
//};


class SkinnedModel : public BaseModel<SkinnedVertex, CB_VS_Skinned_VertexShader>
{
public:
	bool init(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc = nullptr)override;
	bool init(std::vector<SkinnedVertex> vertices, std::vector<DWORD> indices, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, VertexConstantBuffer<CB_VS_Skinned_VertexShader>& cb_vs_vertexShader, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc = nullptr)override;
	void setTexture(ID3D11ShaderResourceView* texture)override;
	void setTexture2(ID3D11ShaderResourceView* texture)override;
	void draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)override;
	typedef std::vector<std::vector<JointTransform>> JointTransformCollection;
	typedef std::multimap<double, std::pair<unsigned int, JointTransform>> AnimationMap;
	friend class Animator;
private:
	ID3D11ShaderResourceView* texture2 = nullptr;
	std::vector<SkinnedMesh> meshes;
	JointTransformCollection tempBoneTransformCollection;
	AnimationMap animationMap;
	bool loadModel(const std::string& filePath, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc ) override;
	void processNode(aiNode* node, const aiScene* scene, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc ) override;
	SkinnedMesh processMesh(aiMesh* mesh, const aiScene* scene, std::function<void(XMFLOAT3& vertex)> vertexProcessFunc );

};
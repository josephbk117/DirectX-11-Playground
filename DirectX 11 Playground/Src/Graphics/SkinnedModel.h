#pragma once
#include "Model.h"
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
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
	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMVECTOR rotation;
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

class KeyFrame
{
private:
	float timeStamp = 0.0f;
	std::map<std::string, JointTransform> pose;
public:
	KeyFrame(float timeStamp, const std::map<std::string, JointTransform>& jointKeyFrames)
	{
		this->timeStamp = timeStamp;
		pose = jointKeyFrames;
	}

	float getTimeStamp()
	{
		return timeStamp;
	}

	std::map < std::string, JointTransform> getJointKeyFrames()
	{
		return pose;
	}
};


class SkinnedModel : public Model
{
private:
	Joint root;
	unsigned int jointCount = 0;
public:

};


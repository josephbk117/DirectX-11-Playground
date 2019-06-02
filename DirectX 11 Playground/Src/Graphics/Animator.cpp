#include "Animator.h"

void Animator::animate(float time, XMMATRIX * jointMatrices, Renderable * renderable)
{
	SkinnedModel* skModel = dynamic_cast<SkinnedModel*>(renderable->getModel());

	for (unsigned int boneIndex = 0; boneIndex < skModel->tempBoneTransformCollection.size(); boneIndex++)
	{
		unsigned int indexA, indexB;
		indexA = DirectX::XMMin(static_cast<size_t>(time), skModel->tempBoneTransformCollection.at(boneIndex).size() - 1);
		indexB = DirectX::XMMin(static_cast<size_t>(time + 1), skModel->tempBoneTransformCollection.at(boneIndex).size() - 1);
		JointTransform A = skModel->tempBoneTransformCollection[boneIndex].at(indexA);
		JointTransform B = skModel->tempBoneTransformCollection[boneIndex].at(indexB);
		jointMatrices[boneIndex] = JointTransform::interpolate(A, B, time - indexA).getLocalTransform();
	}
}

#include "..\Graphics\Vertex.h"
#include "..\Graphics\VertexBuffer.h"
#include "..\Graphics\IndexBuffer.h"
#include "..\Graphics\ConstantBuffer.h"
#include <DirectXCollision.h>
#include <algorithm>

using namespace DirectX;

class OBB
{
private:
	XMFLOAT3 m_negExtent;
	XMFLOAT3 m_posExtent;
	VertexBuffer<Vertex> m_vertexBuffer;
	IndexBuffer m_indexBuffer;
	BoundingOrientedBox m_boundOrientedBox;
public:
	void init(ID3D11Device * device, std::vector<Vertex>& vertices);
	void draw(ID3D11DeviceContext* context)const;	
};
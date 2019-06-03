#pragma once
#include "Material.h"
class DebugViewer
{
private:
	static Material* m_material;
	static Material* m_prevActiveMaterial;
	static DirectX::XMVECTOR* m_colour;
	static DirectX::XMVECTOR m_colourToSet;
	static DirectX::XMVECTOR m_prevColour;
public:
	static void setDebugMaterialAndColourData(Material* material, DirectX::XMVECTOR* colour)
	{
#if _DEBUG
		if (material == nullptr || colour == nullptr)
		{
			ErrorLogger::log("Cannot initialize Debug view material and colour vector as null");
			exit(-1);
		}
#endif
		m_material = material;
		m_colour = colour;
	}
	static void setColour(float r, float g, float b)
	{
#if _DEBUG
		if (m_material == nullptr || m_colour == nullptr)
		{
			ErrorLogger::log("Cannot call 'setColour' without specifying the debug material and colour vector");
			exit(-1);
		}
#endif
		m_colourToSet.m128_f32[0] = r;
		m_colourToSet.m128_f32[1] = g;
		m_colourToSet.m128_f32[2] = b;
	}
	static void startDebugView(ID3D11DeviceContext* context)
	{
#if _DEBUG
		if (m_material == nullptr || m_colour == nullptr)
		{
			ErrorLogger::log("Cannot call 'startDebugView' without specifying the debug material and colour vector");
			exit(-1);
		}
#endif
		m_prevActiveMaterial = Material::getCurrentBoundMaterial();

		m_prevColour.m128_f32[0] = m_colour->m128_f32[0];
		m_prevColour.m128_f32[1] = m_colour->m128_f32[1];
		m_prevColour.m128_f32[2] = m_colour->m128_f32[2];

		m_colour->m128_f32[0] = m_colourToSet.m128_f32[0];
		m_colour->m128_f32[1] = m_colourToSet.m128_f32[1];
		m_colour->m128_f32[2] = m_colourToSet.m128_f32[2];

		m_material->bind(context);
	}
	static void endDebugView(ID3D11DeviceContext* context)
	{
#if _DEBUG
		if (m_material == nullptr || m_colour == nullptr)
			ErrorLogger::log("Cannot call 'endDebugView' without specifying the debug material and colour vector");
#endif
		m_colour->m128_f32[0] = m_prevColour.m128_f32[0];
		m_colour->m128_f32[1] = m_prevColour.m128_f32[1];
		m_colour->m128_f32[2] = m_prevColour.m128_f32[2];

		m_prevActiveMaterial->bind(context);
	}
};
Material* DebugViewer::m_material = nullptr;
Material* DebugViewer::m_prevActiveMaterial = nullptr;
DirectX::XMVECTOR* DebugViewer::m_colour = nullptr;
DirectX::XMVECTOR DebugViewer::m_colourToSet = { 1,1,1 };
DirectX::XMVECTOR DebugViewer::m_prevColour = { 1,1,1 };
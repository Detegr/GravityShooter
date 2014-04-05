#pragma once

#include <Magnum/Buffer.h>
#include <Magnum/ColorFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Texture.h>
#include <Magnum/TextureFormat.h>

#include "main.h"

class PrimitiveLoader : public AbstractResourceLoader<Trade::MeshData3D>
{
	void doLoad(ResourceKey key) override;
};

class MeshLoader : public AbstractResourceLoader<Mesh>
{
	void doLoad(ResourceKey key) override;
};

class TexLoader : public AbstractResourceLoader<Texture2D>
{
	private:
		std::unordered_map<ResourceKey, std::string> m_Map;
	public:
		explicit TexLoader();
		void doLoad(ResourceKey key) override;
};

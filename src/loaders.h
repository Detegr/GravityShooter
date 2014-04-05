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
	void doLoad(ResourceKey key) override
	{
		if(key == ResourceKey("plane")) set(key, Primitives::Plane::solid(Primitives::Plane::TextureCoords::Generate));
		else if(key == ResourceKey("cube")) set(key, Primitives::Cube::solid());
		else if(key == ResourceKey("ball")) set(key, Primitives::UVSphere::solid(20, 20, Primitives::UVSphere::TextureCoords::Generate));
		else setNotFound(key);
	}
};

class MeshLoader : public AbstractResourceLoader<Mesh>
{
	void doLoad(ResourceKey key) override
	{
		auto primitive = GravityShooterResourceManager::instance().get<Trade::MeshData3D>(key);
		if(!primitive) setNotFound(key);
		else
		{
			std::optional<Mesh> mesh;
			std::unique_ptr<Buffer> buffer, indexBuffer;
			std::tie(mesh,buffer,indexBuffer) = MeshTools::compile(*primitive, BufferUsage::StaticDraw);

			// Transfer ownership to manager
			buffer.release();
			if(indexBuffer) indexBuffer.release();

			set(key, new Mesh(std::move(*mesh)));
		}
	}
};

class TexLoader : public AbstractResourceLoader<Texture2D>
{
	private:
		std::unordered_map<ResourceKey, std::string> m_Map;
	public:
		TexLoader()
		{
			m_Map.insert({ResourceKey("wall"), "../brickwall.jpg"});
		}
		void doLoad(ResourceKey key) override
		{
			auto jpegImporter = GravityShooterResourceManager::instance().get<Trade::AbstractImporter>("jpegimporter");
			CORRADE_INTERNAL_ASSERT(jpegImporter);
			if(!jpegImporter->openFile(m_Map[key]))
			{
				setNotFound(key);
				return;
			}
			std::optional<Trade::ImageData2D> teximg = jpegImporter->image2D(0);
			if(!teximg || (teximg->format() != ColorFormat::RGB && teximg->format() != ColorFormat::BGR))
			{
				setNotFound(key);
				return;
			}
			auto& tex = (new Texture2D)->setWrapping(Sampler::Wrapping::ClampToEdge)
				.setMinificationFilter(Sampler::Filter::Linear)
				.setMagnificationFilter(Sampler::Filter::Linear)
				.setImage(0, TextureFormat::RGB8, *teximg)
				.generateMipmap();
			set(key, std::move(tex));
		}
};

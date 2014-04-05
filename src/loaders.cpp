#include "loaders.h"

void PrimitiveLoader::doLoad(ResourceKey key)
{
	if(key == ResourceKey("plane")) set(key, Primitives::Plane::solid(Primitives::Plane::TextureCoords::Generate));
	else if(key == ResourceKey("cube")) set(key, Primitives::Cube::solid());
	else if(key == ResourceKey("ball")) set(key, Primitives::UVSphere::solid(20, 20, Primitives::UVSphere::TextureCoords::Generate));
	else setNotFound(key);
}

void MeshLoader::doLoad(ResourceKey key)
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

TexLoader::TexLoader()
{
	m_Map.insert({ResourceKey("wall"), "../brickwall.jpg"});
}

void TexLoader::doLoad(ResourceKey key)
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

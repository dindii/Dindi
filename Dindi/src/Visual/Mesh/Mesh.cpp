#include "Dindipch.h"
#include "Mesh.h"
#include "Utils/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION 1
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT 1 //more robust triangulation, requires c++11
#include "../vendor/tinyobjloader/tiny_obj_loader.h"


namespace Dindi
{
	Mesh::Mesh(const std::vector<vec3>& vertexPositions)
	{
		//#TODO: Convert this to shared ptr, maybe.
	//	m_VertexPositions = vertexPositions;

	//	RegisterMeshData(m_VertexPositions);
	}

	Mesh::Mesh(std::string& filepath) : m_bHasNormal(false), m_bHasTexCoord(false)
	{
		//#TODO: When creating a GLTF loader, please, move all of this into a static Loader class.

		tinyobj::ObjReaderConfig readerConfig;
		
		//Probably the material files will be in the same path as the geometry.
	//	readerConfig.mtl_search_path = "./";

		//Quick workaround for the .mtl directory (this current one)
		std::string directory;
		const size_t last_slash_idx = filepath.rfind('/');
		if (std::string::npos != last_slash_idx)
		{
			directory = filepath.substr(0, last_slash_idx);
		}

		readerConfig.mtl_search_path = directory;

		tinyobj::ObjReader loader;

		if (!loader.ParseFromFile(filepath, readerConfig))
		{
			if (!loader.Error().empty())
				DND_LOG_ERROR("Failed to load .OBJ file: ", loader.Error());
			return;
		}

		if (!loader.Warning().empty())
		{
			DND_LOG_WARNING("Be careful on loading .OBJ file: ", loader.Warning());
		}

		
		const tinyobj::attrib_t& attrib = loader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = loader.GetShapes();
		const std::vector<tinyobj::material_t>& materials = loader.GetMaterials();

		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					//No indices support yet
					//m_Indices.emplace_back(idx.vertex_index);

					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					m_VertexPositions.emplace_back(vx, vy, vz );

					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0)
					{
						tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					
						m_Normal.emplace_back(nx, ny, nz);
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0)
					{
						tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
	
						m_TextureCoord.emplace_back(tx, ty);
					}
					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}

		m_bHasNormal = m_Normal.size() > 0;
		m_bHasTexCoord = m_TextureCoord.size() > 0;

		RegisterMeshData(m_VertexPositions, m_Normal, m_TextureCoord);

		//#TODO: Make this dynamic, please. It is hardcoded for now.
		
		std::string dirPrefix = directory + "/";

		if(loader.GetMaterials()[0].diffuse_texname.size())
			m_diffuseMap = new Texture2D(dirPrefix + loader.GetMaterials()[0].diffuse_texname);
		
		if(loader.GetMaterials()[0].specular_texname.size())
			m_specularMap = new Texture2D(dirPrefix + loader.GetMaterials()[0].specular_texname);

		//#TODO Probably i'm loading normal maps twice, one as texture and another as attributes, fix this later.
		if(m_bHasNormal)
			m_normalMap = new Texture2D(dirPrefix + loader.GetMaterials()[0].bump_texname);
	}

	void Mesh::BindTextures() const
	{
		if(m_diffuseMap)
			m_diffuseMap->Bind(1);
		
		if(m_specularMap)
			m_specularMap->Bind(2);
		
		if(m_normalMap)
			m_normalMap->Bind(3);
	}

}
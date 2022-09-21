#include "Dindipch.h"
#include "ModelLoader.h"
#include <filesystem>
#include "Utils/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "../vendor/tinyobjloader/tiny_obj_loader.h"

#include <unordered_map>

namespace Dindi
{
	bool ModelLoader::Load(std::string filepath, Model& modelToFill)
	{
		if (std::filesystem::path(filepath).extension() == ".obj")
			return LoadOBJ(filepath, modelToFill);

		DND_LOG_ERROR("This type of file is not supported!");
		return false;
	}

	bool ModelLoader::LoadOBJ(std::string filepath, Model& modelToFill)
	{
		tinyobj::ObjReaderConfig readerConfig;

		//Quick workaround for the .mtl directory (this current one)
		std::string directory;
		const size_t last_slash_idx = filepath.rfind('/');

		if (std::string::npos != last_slash_idx)
		{
			directory = filepath.substr(0, last_slash_idx);
		}
		else
		{
			const size_t last_backslash_idx = filepath.rfind('\\');

			if (std::string::npos != last_backslash_idx)
				directory = filepath.substr(0, last_backslash_idx);
		}

		readerConfig.mtl_search_path = directory;

		tinyobj::ObjReader loader;

		if (!loader.ParseFromFile(filepath, readerConfig))
		{
			if (!loader.Error().empty())
				DND_LOG_ERROR("Failed to load .OBJ file: ", loader.Error());

			return false;
		}

		if (!loader.Warning().empty())
		{
			DND_LOG_WARNING("Be careful on loading .OBJ file: ", loader.Warning());
		}

		const tinyobj::attrib_t& attrib = loader.GetAttrib();
		const std::vector<tinyobj::shape_t>& shapes = loader.GetShapes();
		const std::vector<tinyobj::material_t>& materials = loader.GetMaterials();

		std::unordered_map<uint32_t, Material*> meshMaterial;

		std::vector<Mesh*>& meshToFill = modelToFill.GetMeshes();

		for (size_t shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++)
		{
			meshToFill.emplace_back(new Mesh());

			std::vector<vec3> temporaryVertexPositions;
			std::vector<vec3> temporaryNormals;
			std::vector<vec2> temporaryTextureCoords;

			size_t index_offset = 0;

			// Loop over faces(polygon)
			for (size_t f = 0; f < shapes[shapeIndex].mesh.num_face_vertices.size(); f++)
			{
				size_t fv = size_t(shapes[shapeIndex].mesh.num_face_vertices[f]);

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++)
				{
					// access to vertex
					tinyobj::index_t idx = shapes[shapeIndex].mesh.indices[index_offset + v];

					//No indices support yet
					//m_Indices.emplace_back(idx.vertex_index);

					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					temporaryVertexPositions.emplace_back(vx, vy, vz);

					// Check if `normal_index` is zero or positive. negative = no normal data
					if (idx.normal_index >= 0)
					{
						tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

						temporaryNormals.emplace_back(nx, ny, nz);
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					if (idx.texcoord_index >= 0)
					{
						tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

						temporaryTextureCoords.emplace_back(tx, ty);
					}
				}

				index_offset += fv;
			}

			meshToFill[shapeIndex]->SetVertexPositionData(std::move(temporaryVertexPositions));
			meshToFill[shapeIndex]->SetNormalData(std::move(temporaryNormals));
			meshToFill[shapeIndex]->SetTextureCoordData(std::move(temporaryTextureCoords));

			Texture2D *temporaryDiffuse = nullptr, *temporarySpecular = nullptr, *temporaryNormal = nullptr;

			std::string dirPrefix = directory + "\\";

			const tinyobj::material_t& meshMaterial = loader.GetMaterials()[shapes[shapeIndex].mesh.material_ids[0]];

			if (loader.GetMaterials().size())
			{
				if (!meshMaterial.diffuse_texname.empty())
				{
					temporaryDiffuse = Texture2D::Load(dirPrefix + meshMaterial.diffuse_texname);
					meshToFill[shapeIndex]->GetMaterial()->SetDiffuseMap(temporaryDiffuse);
				}

				if (!meshMaterial.specular_texname.empty())
				{
					temporarySpecular = Texture2D::Load(dirPrefix + meshMaterial.specular_texname);
					meshToFill[shapeIndex]->GetMaterial()->SetSpecularMap(temporarySpecular);
				}

				if (!meshMaterial.bump_texname.empty())
				{
					temporaryNormal = Texture2D::Load(dirPrefix + meshMaterial.bump_texname);
					meshToFill[shapeIndex]->GetMaterial()->SetNormalMap(temporaryNormal);
				}

			}
		}

		return true;
	}

}
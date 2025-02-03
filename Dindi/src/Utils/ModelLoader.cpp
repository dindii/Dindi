#include "Dindipch.h"
#include "ModelLoader.h"
#include <filesystem>
#include "Utils/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "../vendor/tinyobjloader/tiny_obj_loader.h"


#include <unordered_map>

#include <Core/Core.h>

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
		std::chrono::duration now = std::chrono::system_clock::now().time_since_epoch();
		auto libTimeBefore = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

		tinyobj::ObjReaderConfig readerConfig;

		//Quick workaround for the .mtl directory (this current one)
		std::string directory;
		const size_t last_slash_idx = filepath.rfind('/');
		size_t last_backslash_idx = 0;
		size_t periodIdx = filepath.find('.');
		const char* name;

		if (std::string::npos != last_slash_idx)
		{
			directory = filepath.substr(0, last_slash_idx);
			name = filepath.substr(last_slash_idx, periodIdx - last_slash_idx).c_str();
		}
		else
		{
			last_backslash_idx = filepath.rfind('\\');

			if (std::string::npos != last_backslash_idx)
				directory = filepath.substr(0, last_backslash_idx);
		
			//name = filepath.substr(last_backslash_idx + 1, periodIdx - (last_backslash_idx + 1)).c_str();
			name = filepath.substr(last_backslash_idx, periodIdx - last_backslash_idx).c_str();
		}

		modelToFill.SetName(name + 1);

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

		std::chrono::duration anow = std::chrono::system_clock::now().time_since_epoch();
		auto libTimeThen = std::chrono::duration_cast<std::chrono::milliseconds>(anow).count();

		DND_LOG_TRACE("LibTime to Load Sponza: ", libTimeThen - libTimeBefore);

		std::chrono::duration noww = std::chrono::system_clock::now().time_since_epoch();
		auto sortTimeBefore = std::chrono::duration_cast<std::chrono::milliseconds>(noww).count();

		meshToFill.reserve(shapes.size());

		for (size_t shapeIndex = 0; shapeIndex < shapes.size(); shapeIndex++)
		{
			meshToFill.emplace_back(new Mesh());

			std::vector<glm::vec3> temporaryVertexPositions;
			std::vector<glm::vec3> temporaryNormals;
			std::vector<glm::vec2> temporaryTextureCoords;
			std::vector<glm::vec3> temporaryTangents;

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

			for (uint32_t i = 0; i < temporaryVertexPositions.size(); i+=3)
			{
				if (temporaryTextureCoords.size() == 0)
				{
					DND_LOG_WARNING("Mesh", shapeIndex, " doesn't contain UVs! Model: ", filepath);
					temporaryTangents.push_back(glm::vec3(0.0f));
					continue;
				}
					
				glm::vec3 p1 = temporaryVertexPositions[i];
				glm::vec3 p2 = temporaryVertexPositions[i + 1];
				glm::vec3 p3 = temporaryVertexPositions[i + 2];

				glm::vec2 uv1 = temporaryTextureCoords[i];
				glm::vec2 uv2 = temporaryTextureCoords[i + 1];
				glm::vec2 uv3 = temporaryTextureCoords[i + 2];

				glm::vec3 ed1 = p2 - p1;
				glm::vec3 ed2 = p3 - p1;

				glm::vec2 deltaUV1 = uv2 - uv1;
				glm::vec2 deltaUV2 = uv3 - uv1;

				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				glm::vec3 tangent;

				tangent.x = f * (deltaUV2.y * ed1.x - deltaUV1.y * ed2.x);
				tangent.y = f * (deltaUV2.y * ed1.y - deltaUV1.y * ed2.y);
				tangent.z = f * (deltaUV2.y * ed1.z - deltaUV1.y * ed2.z);

				temporaryTangents.push_back(tangent);
			}

			meshToFill[shapeIndex]->SetVertexPositionData(std::move(temporaryVertexPositions));
			meshToFill[shapeIndex]->SetNormalData(std::move(temporaryNormals));
			meshToFill[shapeIndex]->SetTextureCoordData(std::move(temporaryTextureCoords));
			meshToFill[shapeIndex]->SetTangentData(std::move(temporaryTangents));
			meshToFill[shapeIndex]->SetPosition(modelToFill.GetPosition());
			Ref<Texture2D> temporaryDiffuse, temporarySpecular, temporaryNormal;

			std::string dirPrefix = directory + "\\";


			if (loader.GetMaterials().size())
			{
				const tinyobj::material_t& meshMaterial = loader.GetMaterials()[shapes[shapeIndex].mesh.material_ids[0]];
				
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

		std::chrono::duration nowaw = std::chrono::system_clock::now().time_since_epoch();
		auto sortTimeThen = std::chrono::duration_cast<std::chrono::milliseconds>(nowaw).count();

		DND_LOG_TRACE("Sort time to Load Sponza: ", sortTimeThen - sortTimeBefore);

		return true;
	}

}
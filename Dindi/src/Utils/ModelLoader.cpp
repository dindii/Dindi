#include "Dindipch.h"
#include "ModelLoader.h"
#include <filesystem>
#include "Utils/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "../vendor/tinyobjloader/tiny_obj_loader.h"

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

			if(std::string::npos != last_backslash_idx)
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


		std::vector<vec3> temporaryVertexPositions;
		std::vector<vec3> temporaryNormals;
		std::vector<vec2> temporaryTextureCoords;

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
					// Optional: vertex colors
					// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += fv;

				// per-face material
				//shapes[s].mesh.material_ids[f];
			}
		}
	
		Mesh* meshToFill = modelToFill.GetMesh();

		meshToFill->SetVertexPositionData(std::move(temporaryVertexPositions));
		meshToFill->SetNormalData        (std::move(temporaryNormals));
		meshToFill->SetTextureCoordData  (std::move(temporaryTextureCoords));

		//#TODO: Make this dynamic, please. It is hardcoded for now. (the [0] of GetMaterials() part)

		Texture2D *temporaryDiffuse = nullptr, *temporarySpecular = nullptr, *temporaryNormal = nullptr;

		std::string dirPrefix = directory + "\\";

		if (loader.GetMaterials().size())
		{
			if (loader.GetMaterials()[0].diffuse_texname.size())
				temporaryDiffuse = new Texture2D(dirPrefix + loader.GetMaterials()[0].diffuse_texname);

			if (loader.GetMaterials()[0].specular_texname.size())
				temporarySpecular = new Texture2D(dirPrefix + loader.GetMaterials()[0].specular_texname);

			//#TODO Probably i'm loading normal maps twice, one as texture and another as attributes, fix this later. Or find a way
			//to detect if we have attributes but not maps and make it use only attributes.
			if (loader.GetMaterials()[0].bump_texname.size())
				temporaryNormal = new Texture2D(dirPrefix + loader.GetMaterials()[0].bump_texname);
		}
		
		Material* materialToFill;
		materialToFill = modelToFill.GetMaterial();

		//Pass the ownership of the texture pointers to the material.
		if(temporaryDiffuse)
			materialToFill->SetDiffuseMap (temporaryDiffuse);
		
		if(temporarySpecular)
			materialToFill->SetSpecularMap(temporarySpecular);
		
		if(temporaryNormal)
			materialToFill->SetNormalMap  (temporaryNormal);

		return true;
	}

}
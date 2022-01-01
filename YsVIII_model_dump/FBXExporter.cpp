#include "FBXExporter.h"
#include <assimp\scene.h>
#include <assimp\Exporter.hpp>
#include <iostream>
#include <unordered_map>

void assign_property(aiMaterial& mat_to_update, material mat, unsigned int index_tex, aiTextureType type) { //aiTextureType_DIFFUSE
	if (index_tex < mat.textures.size()) {
		if (mat.textures[index_tex].name.compare("") != 0) {
			aiString name;
			name.Set(mat.textures[index_tex].name + ".dds");
			size_t nb_tex = mat_to_update.GetTextureCount(type);
			mat_to_update.AddProperty(&name, AI_MATKEY_TEXTURE(type, nb_tex));
		}
		//there was no texture with that index, so we don't do anything
	}
	//there was no texture with that index, so we don't do anything

}

void assign_property_based_on_name(aiMaterial& mat_to_update, material mat) {

	

	for (unsigned int idx_tex = 0; idx_tex < mat.textures.size(); idx_tex++) {
		//we guess from the texture name...
		aiString name;
		name.Set(mat.textures[idx_tex].name + ".dds");


		size_t idx = mat.textures[idx_tex].name.find_last_of("_");
		if (idx != std::string::npos) {
			std::string suffix = mat.textures[idx_tex].name.substr(idx + 1);

			//removing all numbers from the suffix, ex 01n becomes n, t1 becomes t, but i'm not sure if t means anything actually
			suffix.erase(std::remove_if(std::begin(suffix), std::end(suffix),
				[](auto ch) { return std::isdigit(ch); }),
				suffix.end());

			aiTextureType type;

			if (suffix.compare("n") == 0)
				type = aiTextureType_NORMALS;
			else if (suffix.compare("s") == 0)
				type = aiTextureType_SPECULAR;
			else
				type = aiTextureType_DIFFUSE;

			size_t nb_tex = mat_to_update.GetTextureCount(type);
			mat_to_update.AddProperty(&name, AI_MATKEY_TEXTURE(type, nb_tex));
			aiTextureMapping mapping = aiTextureMapping_UV;
			mat_to_update.AddProperty<int>((int *)&mapping,1, AI_MATKEY_MAPPING(type, nb_tex));

			/*aiTextureMapMode Wrap = aiTextureMapMode_Wrap;
			mat_to_update.AddProperty<int>((int*)&Wrap, 1, AI_MATKEY_MAPPINGMODE_U(type, nb_tex));
			mat_to_update.AddProperty<int>((int*)&Wrap, 1, AI_MATKEY_MAPPINGMODE_V(type, nb_tex));*/

			int channel = 0;//They all use the same channel, I believe (same set of coordinates in the vertice part of VPAX)
			mat_to_update.AddProperty<int>((int*)&channel, 1, AI_MATKEY_UVWSRC(type, nb_tex));
			
		}


	}
}

aiMaterial* guess_material_from_mat_struct(material mat, unsigned int variant) {
	//0638870 <= I hate this

	/*we will miss a LOT of parameters, because I'm certainly not going to reverse every layout of material parameters
	(afaik their parsing is hardcoded and very specific); it will also not be perfect and more of a guess than anything*/

	aiMaterial* material = new aiMaterial();

	aiString name;
	name.Set(mat.name);
	material->AddProperty(&name, AI_MATKEY_NAME);

	switch (variant) {
	case 0:
		break;
	case 8: //lambert, osef
		break;
	/*case 15:

		break;
	case 17:
		break;
	case 32:
		break;
	case 110:
		break;
	case 115:
		break;
	case 172:
		break;*/

	/*actually you know what, fuck it*/
	default: 
		assign_property_based_on_name(*material, mat);
		break;
	}
	return material;
}



void FBXExporter::GenerateScene(IT3File file){
	size_t nb_meshes = 0;
	size_t nb_materials = 0;
	size_t nb_nodes = file.nodes.size();

	for (const node& nd : file.nodes) {
		if (nd.vpax) {
			std::cout << nd.info->text_id1 << " meshes: " << nd.vpax->meshes_d.size()    << std::endl;
			nb_meshes += nd.vpax->meshes_d.size();
		}
		if (nd.mat6) {
			std::cout << nd.info->text_id1 << " materials: " << nd.mat6->mats.size() << std::endl;
			nb_materials += nd.mat6->mats.size();
		}
	}
	std::cout << "NB MATERIALS: " << nb_materials << std::endl;
	std::cout << "NB MESHES: " << nb_meshes << std::endl;
	if (nb_materials != nb_meshes){
		
		//throw std::exception("Here the number of materials and meshes in the node are different. Go investigate again");
	}
		//Seems like it's always the case?
	aiNode** ainodes = new aiNode* [nb_nodes];
	aiMesh** aimeshes = new aiMesh * [nb_meshes];
	aiMaterial** aimaterials = new aiMaterial * [nb_materials];

	unsigned int count_total_mesh = 0, count_total_material = 0;
	//first we create all ainodes separately, then we will organize them.
	for (unsigned int idx_node = 0; idx_node < nb_nodes; idx_node++) {

		node current_node = file.nodes[idx_node];

		ainodes[idx_node] = new aiNode();
		ainodes[idx_node]->mName = current_node.info->text_id1;
		//creating mesh if there is any in the node
		size_t count_material_ = 0;
		std::unordered_map<unsigned int, unsigned int> node_materials;

		if (current_node.vpax) {
			size_t nb_mesh_in_vpax = current_node.vpax->meshes_d.size();
			ainodes[idx_node]->mMeshes = new unsigned[nb_mesh_in_vpax];
			ainodes[idx_node]->mNumMeshes = nb_mesh_in_vpax;
			size_t count_mesh_ = 0;
			for (auto mesh_ : current_node.vpax->meshes_d) {
				aiVector3D* vertices = new aiVector3D[mesh_.vertices.size()];
				aiVector3D* uv = new aiVector3D[mesh_.vertices.size()];

				std::cout << "OBJ EXPORT " << std::endl;
				for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
				{
					vertices[idx_v].x = mesh_.vertices[idx_v].position.x;
					vertices[idx_v].y = mesh_.vertices[idx_v].position.y;
					vertices[idx_v].z = mesh_.vertices[idx_v].position.z;

					uv[idx_v].x = abs(mesh_.vertices[idx_v].uv.x);
					uv[idx_v].y = abs(1-mesh_.vertices[idx_v].uv.y);
					uv[idx_v].z = 0;
					//std::cout << vertices[idx_v].x << ", " << vertices[idx_v].y << ", " << vertices[idx_v].z << std::endl;
				}

				size_t nb_faces = mesh_.indexes.size() / 3; // each face has 3 vertices?
				aiFace* faces = new aiFace[nb_faces];

				for (unsigned int idx_face = 0; idx_face < nb_faces; idx_face++)
				{
					aiFace f;
					faces[idx_face].mNumIndices = 3;
					faces[idx_face].mIndices = new unsigned[3];
					faces[idx_face].mIndices[0] = mesh_.indexes[0 + idx_face * 3];
					faces[idx_face].mIndices[1] = mesh_.indexes[1 + idx_face * 3];
					faces[idx_face].mIndices[2] = mesh_.indexes[2 + idx_face * 3];
					//std::cout << faces[idx_face].mIndices[0] << ", " << faces[idx_face].mIndices[1] << ", " << faces[idx_face].mIndices[2] << std::endl;

				}

				aiMesh* mesh = new aiMesh();                        // deleted: Version.cpp:150
				mesh->mNumVertices = mesh_.vertices.size();
				mesh->mVertices = vertices;
				mesh->mNumFaces = nb_faces;
				mesh->mFaces = faces;
				mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE; // workaround, issue #3778
				mesh->mName = current_node.vpax->name;
				
				std::cout << "Name: " << current_node.vpax->name << std::endl;
				std::cout << "Nb vertices: " << std::hex << mesh->mNumVertices << std::endl;
				std::cout << "Nb faces: " << std::hex << mesh->mNumFaces << std::endl;
				std::cout << "max vertex index: " << *max_element(std::begin(mesh_.indexes), std::end(mesh_.indexes)) << std::endl;
				// a valid material is needed, even if its empty

				if ((node_materials.find(mesh_.material_id) == node_materials.end())) {
					node_materials[mesh_.material_id] = count_total_material;
					aiMaterial* material = guess_material_from_mat_struct(current_node.mat6->mats[mesh_.material_id], current_node.rty2->material_variant);
					
					
					mesh->mMaterialIndex = count_total_material;
					aimaterials[count_total_material] = material;
					count_total_material++;
					


				}
				else {
					mesh->mMaterialIndex = node_materials[mesh_.material_id];
				}
				mesh->mNumUVComponents[0] = 2;
				mesh->mTextureCoords[0] = uv;
					
				//our material: current_node.mat6->mats[count_mesh_].name;
				

				aimeshes[count_total_mesh] = mesh;
				ainodes[idx_node]->mMeshes[count_mesh_] = count_total_mesh;
				
				              // deleted: scene.cpp:77
				count_mesh_++;
				count_total_mesh++;
				
			}
			

		}
		else {
			ainodes[idx_node]->mNumMeshes = 0;
		}
	
	}
	//now we need to organize them.

	aiNode* rootNode = NULL;
	for (unsigned int idx_node = 0; idx_node < nb_nodes; idx_node++) {
		node current_node = file.nodes[idx_node];
		aiNode * current_ainode = ainodes[idx_node];
		std::string current_ainode_name = std::string(current_ainode->mName.C_Str());
		if (current_ainode_name.compare("root") == 0)
			rootNode = current_ainode;
		if (current_node.chid) {

			aiNode** children = new aiNode*[current_node.chid->strs.size()];
			unsigned int idx_child = 0;
			for (std::string name : current_node.chid->strs) {
				//find the corresponding node in ainodes
				for (unsigned int idx_node_search = 0; idx_node_search < nb_nodes; idx_node_search++) {
					aiNode* ptr_node = ainodes[idx_node_search];
					std::string str_name = std::string(ptr_node->mName.C_Str());
					if (str_name.compare(name) == 0) {
						children[idx_child++] = ptr_node;
						
						break;
					}
				}
			}
			current_ainode->addChildren(idx_child, children);
			current_ainode->mNumChildren = idx_child;
			delete[] children;
		}
		else {
			current_ainode->mNumChildren = 0;
		}
	}

	// pack mesh(es), material, and root node into a new minimal aiScene

	aiScene *out = new aiScene();                       // deleted: by us after use
	out->mNumMeshes = nb_meshes;
	out->mMeshes = aimeshes;            // deleted: Version.cpp:151
	out->mNumMaterials = nb_materials;
	out->mMaterials = aimaterials; // deleted: Version.cpp:158
	out->mRootNode = rootNode;
	out->mMetaData = new aiMetadata(); // workaround, issue #3781

	// and we're good to go. do whatever:

	Assimp::Exporter exporter;
	//try{
	std::cout << "format supported: " << std::endl;
	for (unsigned i = 0; i < exporter.GetExportFormatCount(); i++) {
		const aiExportFormatDesc * desc = exporter.GetExportFormatDescription(i);
		std::cout << desc->description << " " << desc->id << std::endl;
	}
	//exporter.Export(out, "objnomtl", "model.obj");
	exporter.Export(out, "collada",  "model.dae");
	//}
	//catch (std::exception e) {
		//std::cout << "FAIL: " << e.what() << std::endl;
		//}
	// deleting the scene will also take care of the vertices, faces, meshes, materials, nodes, etc.

	//delete out;

}
FBXExporter::FBXExporter()
{
	
}


FBXExporter::~FBXExporter()
{
}

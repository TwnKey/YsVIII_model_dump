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

	for (auto it : file.nodes) {
		
		if (it.second.vpax) {
			nb_meshes += it.second.vpax->meshes_d.size();
		}
		if (it.second.mat6) {
			nb_materials += it.second.mat6->mats.size();
		}
	}
	if (nb_materials != nb_meshes){
		
		//throw std::exception("Here the number of materials and meshes in the node are different. Go investigate again");
	}
		//Seems like it's always the case?
	aiNode** ainodes = new aiNode* [nb_nodes];
	aiMesh** aimeshes = new aiMesh * [nb_meshes];
	aiMaterial** aimaterials = new aiMaterial * [nb_materials];

	unsigned int count_total_mesh = 0, count_total_material = 0;
	//first we create all ainodes separately, then we will organize them.
	unsigned int idx_node = 0;
	for (auto it_nd : file.nodes){
		node current_node = it_nd.second;

		ainodes[idx_node] = new aiNode();
		ainodes[idx_node]->mName = current_node.info->text_id1;

		aiMatrix4x4 transformNode = aiMatrix4x4(current_node.info->transform.a.x, current_node.info->transform.a.y, current_node.info->transform.a.z, current_node.info->transform.a.t,
			current_node.info->transform.b.x, current_node.info->transform.b.y, current_node.info->transform.b.z, current_node.info->transform.b.t,
			current_node.info->transform.c.x, current_node.info->transform.c.y, current_node.info->transform.c.z, current_node.info->transform.c.t,
			current_node.info->transform.d.x, current_node.info->transform.d.y, current_node.info->transform.d.z, current_node.info->transform.d.t);

		ainodes[idx_node]->mTransformation = transformNode;
		//creating mesh if there is any in the node
		size_t count_material_ = 0;
		std::unordered_map<unsigned int, unsigned int> node_materials;
		
		

		if (current_node.vpax) {
			std::cout << "Name: " << current_node.vpax->name << std::endl << std::endl;
			size_t nb_mesh_in_vpax = current_node.vpax->meshes_d.size();
			ainodes[idx_node]->mMeshes = new unsigned[nb_mesh_in_vpax];
			ainodes[idx_node]->mNumMeshes = nb_mesh_in_vpax;
			size_t count_mesh_ = 0;
			for (auto mesh_ : current_node.vpax->meshes_d) {
				aiVector3D* vertices = new aiVector3D[mesh_.vertices.size()];
				aiVector3D* uv = new aiVector3D[mesh_.vertices.size()];

				
				std::map<std::string, std::pair<std::vector<unsigned int>, std::vector<float>>> weights_map;
				/*The mesh refers to a few (max 8) bones, that we need to create specifically for this mesh
				*/
				for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
				{
					vertices[idx_v].x = mesh_.vertices[idx_v].position.x;
					vertices[idx_v].y = mesh_.vertices[idx_v].position.y;
					vertices[idx_v].z = mesh_.vertices[idx_v].position.z;

					uv[idx_v].x = abs(mesh_.vertices[idx_v].uv.x);
					uv[idx_v].y = abs(1-mesh_.vertices[idx_v].uv.y);
					uv[idx_v].z = 0;

					for (unsigned int idx_weight = 0; idx_weight < 8; idx_weight++) {
						uint8_t weight = mesh_.vertices[idx_v].weights[idx_weight];
						float weight_f = (float)weight / 255;
						uint8_t joint_id = mesh_.vertices[idx_v].bones_indexes[idx_weight]-1;
						if (joint_id != 0xFF) {
							std::string joint_name = current_node.bon3->joints_names[joint_id];

							if (weights_map.count(joint_name) == 0)
								weights_map[joint_name].second = std::vector<float>();
							
							weights_map[joint_name].second.push_back(weight_f);
							weights_map[joint_name].first.push_back(idx_v);
						}
						
					}

				}

				aiBone** bones = NULL;
				size_t nb_bones = 0;
				if (current_node.bon3) {

					nb_bones = weights_map.size();
					bones = new aiBone * [nb_bones];
					unsigned int i = 0;
					for (auto it_w : weights_map){
					//for (unsigned int i = 0; i < nb_bones; i++) {
						if (current_node.bon3->bones.count(it_w.first) == 0)
						{
							
							nb_bones--;
						

							
						}
						else {
							bones[i] = new aiBone();

							bones[i]->mNumWeights = it_w.second.second.size();
							bones[i]->mWeights = new aiVertexWeight[bones[i]->mNumWeights]();

							for (unsigned int idx_w = 0; idx_w < bones[i]->mNumWeights; idx_w++) {

								aiVertexWeight weight;
								weight.mWeight = it_w.second.second[idx_w];
								weight.mVertexId = it_w.second.first[idx_w];
								bones[i]->mWeights[idx_w] = weight;
								idx_w++;
							}
							matrix4 mat = current_node.bon3->bones[it_w.first].offset_matrix;
							aiMatrix4x4 aiMat = aiMatrix4x4(mat.a.x, mat.a.y, mat.a.z, mat.a.t,
								mat.b.x, mat.b.y, mat.b.z, mat.b.t,
								mat.c.x, mat.c.y, mat.c.z, mat.c.t,
								mat.d.x, mat.d.y, mat.d.z, mat.d.t);
							bones[i]->mName = it_w.first;
							std::cout << "Created bone: " << bones[i]->mName.C_Str() << std::endl;
							bones[i]->mOffsetMatrix = aiMat;

							/*std::cout << "inverse bind matrix: " << std::endl;
							std::cout << mat.to_string() << std::endl;*/

							/*std::cout << "transform: " << std::endl;
							matrix4 transformMat = file.nodes[bones[i]->mName.C_Str()].info->transform;
							std::cout << transformMat.to_string() << std::endl;

							std::string bone_name = it_w.first;*/


							/*if (file.nodes.count(bone_name)) {
								node corresponding_node = file.nodes[it_w.first];
								if (corresponding_node.kan7)
								{

								

								}
								else {
								
								}
							}*/
							i++;
						}
					}
					
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
				mesh->mName = current_node.vpax->name;//+"_"+std::to_string(count_mesh_);

				
				if (bones) {
					mesh->mBones = bones;
					mesh->mNumBones = nb_bones;
				}
				
				
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
		idx_node++;
	}
	//now we need to organize them.

	aiNode* rootNode = NULL;

	idx_node = 0;

	for (auto it : file.nodes) {
		node current_node = it.second;
		aiNode * current_ainode = ainodes[idx_node];
		std::string current_ainode_name = std::string(current_ainode->mName.C_Str());
		if (current_ainode_name.compare("root") == 0)
			rootNode = current_ainode;
		if (current_node.chid) {

			aiNode** children = new aiNode*[current_node.chid->children.size()];
			unsigned int idx_child = 0;
			for (std::string name : current_node.chid->children) {
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
			current_ainode->mChildren = children;
			current_ainode->mNumChildren = idx_child;
			
		}
		else {
			current_ainode->mNumChildren = 0;
		}
		idx_node++;
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

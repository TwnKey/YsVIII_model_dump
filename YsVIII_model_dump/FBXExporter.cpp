#include "FBXExporter.h"
#include <assimp\scene.h>
#include <assimp\Exporter.hpp>
#include <iostream>
#include <unordered_map>
#include "Scene.h"
void assign_property(aiMaterial& mat_to_update, material mat, unsigned int index_tex, aiTextureType type) { //aiTextureType_DIFFUSE
	if (index_tex < mat.textures_paths.size()) {
		if (mat.textures_paths[index_tex].compare("") != 0) {
			aiString name;
			name.Set(mat.textures_paths[index_tex]);
			size_t nb_tex = mat_to_update.GetTextureCount(type);
			mat_to_update.AddProperty(&name, AI_MATKEY_TEXTURE(type, nb_tex));
		}
		//there was no texture with that index, so we don't do anything
	}
	//there was no texture with that index, so we don't do anything

}

void assign_property_based_on_name(aiMaterial& mat_to_update, material mat) {

	

	for (unsigned int idx_tex = 0; idx_tex < mat.textures_paths.size(); idx_tex++) {
		//we guess from the texture name...
		aiString name;
		name.Set(mat.textures_paths[idx_tex]);


		size_t idx = mat.textures_paths[idx_tex].find_last_of("_");
		if (idx != std::string::npos) {
			std::string suffix = mat.textures_paths[idx_tex].substr(idx + 1);

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



void FBXExporter::ExportScene(Scene scene){
	
	size_t nb_meshes_nodes = scene.meshes.size();
	size_t nb_simple_nodes = scene.simple_nodes.size();
	size_t nb_materials = scene.mats.size();
	size_t nb_ani = scene.anis.size();
	size_t nb_nodes = nb_meshes_nodes + nb_simple_nodes;
	
	size_t real_nb_of_meshes = 0;
	for (auto mesh : scene.meshes)
		real_nb_of_meshes += mesh.second.size();


	aiNode** aiNodes = new aiNode* [nb_nodes];
	aiMesh** aimeshes = new aiMesh * [real_nb_of_meshes];
	aiMaterial** aimaterials = new aiMaterial * [nb_materials];

	

	std::map<std::string, aiNode*> map_ptr;
	std::vector <std::vector<std::string>> children;

	std::unordered_map<unsigned int, unsigned int> mat_id_to_idx;
	unsigned int count_total_mesh = 0, count_total_material = 0;
	/*first we create all aiNodes separately, then we will organize them.
	We start from the simple nodes*/
	unsigned int idx_node = 0;

	for (auto it_nd : scene.simple_nodes) {

		aiNodes[idx_node] = new aiNode();
		aiNodes[idx_node]->mName = it_nd.second.name;
		std::cout << aiNodes[idx_node]->mName.C_Str() << std::endl;
		map_ptr[aiNodes[idx_node]->mName.C_Str()] = aiNodes[idx_node];
		aiMatrix4x4 transform_node = aiMatrix4x4(it_nd.second.transform.a.x, it_nd.second.transform.a.y, it_nd.second.transform.a.z, it_nd.second.transform.a.t,
			it_nd.second.transform.b.x, it_nd.second.transform.b.y, it_nd.second.transform.b.z, it_nd.second.transform.b.t,
			it_nd.second.transform.c.x, it_nd.second.transform.c.y, it_nd.second.transform.c.z, it_nd.second.transform.c.t,
			it_nd.second.transform.d.x, it_nd.second.transform.d.y, it_nd.second.transform.d.z, it_nd.second.transform.d.t);

		aiNodes[idx_node]->mTransformation = transform_node; 
		
		children.push_back(it_nd.second.children);
		size_t nb_children = it_nd.second.children.size();
		aiNodes[idx_node]->mNumChildren = nb_children;
		aiNodes[idx_node]->mChildren = new aiNode * [nb_children]();

		idx_node++;

	}
	
	unsigned int count_materials = 0;
	unsigned int count_mesh = 0;
	for (auto it_msh : scene.meshes) {

		aiNodes[idx_node] = new aiNode();
		aiNodes[idx_node]->mName = it_msh.first;
		map_ptr[aiNodes[idx_node]->mName.C_Str()] = aiNodes[idx_node];

		size_t nb_children = it_msh.second[0].children.size();
		children.push_back(it_msh.second[0].children);

		aiNodes[idx_node]->mNumChildren = nb_children;
		aiNodes[idx_node]->mChildren = new aiNode *[nb_children]();
		aiMatrix4x4 transform_node = aiMatrix4x4(it_msh.second[0].transform.a.x, it_msh.second[0].transform.a.y, it_msh.second[0].transform.a.z, it_msh.second[0].transform.a.t,
			it_msh.second[0].transform.b.x, it_msh.second[0].transform.b.y, it_msh.second[0].transform.b.z, it_msh.second[0].transform.b.t,
			it_msh.second[0].transform.c.x, it_msh.second[0].transform.c.y, it_msh.second[0].transform.c.z, it_msh.second[0].transform.c.t,
			it_msh.second[0].transform.d.x, it_msh.second[0].transform.d.y, it_msh.second[0].transform.d.z, it_msh.second[0].transform.d.t);

		aiNodes[idx_node]->mTransformation = transform_node;
		aiNodes[idx_node]->mMeshes = new unsigned int[it_msh.second.size()];
		aiNodes[idx_node]->mNumMeshes = it_msh.second.size();
		unsigned int count_mesh_within_node = 0;
		for (auto mesh_ : it_msh.second) {
			

			aimeshes[count_mesh] = new aiMesh();

			aimeshes[count_mesh]->mVertices = new aiVector3D[mesh_.vertices.size()];
			aimeshes[count_mesh]->mNumUVComponents[0] = 2;
			aimeshes[count_mesh]->mTextureCoords[0] = new aiVector3D[mesh_.vertices.size()];
			
			for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
			{
				aimeshes[count_mesh]->mVertices[idx_v].x = mesh_.vertices[idx_v].x;
				aimeshes[count_mesh]->mVertices[idx_v].y = mesh_.vertices[idx_v].y;
				aimeshes[count_mesh]->mVertices[idx_v].z = mesh_.vertices[idx_v].z;

				aimeshes[count_mesh]->mTextureCoords[0][idx_v].x = abs(mesh_.vertices[idx_v].x);
				aimeshes[count_mesh]->mTextureCoords[0][idx_v].y = abs(1 - mesh_.vertices[idx_v].y);
				aimeshes[count_mesh]->mTextureCoords[0][idx_v].z = 0;
			}

			size_t nb_faces = mesh_.faces_indexes.size(); 
			aiFace* faces = new aiFace[nb_faces];

			for (unsigned int idx_face = 0; idx_face < nb_faces; idx_face++)
			{
				
				faces[idx_face].mNumIndices = 3;
				faces[idx_face].mIndices = new unsigned[3];
				faces[idx_face].mIndices[0] = mesh_.faces_indexes[idx_face].x;
				faces[idx_face].mIndices[1] = mesh_.faces_indexes[idx_face].y;
				faces[idx_face].mIndices[2] = mesh_.faces_indexes[idx_face].z;
				
			}


			aiBone** bones = new aiBone*[mesh_.bones.size()];
			size_t nb_bones = 0;

			nb_bones = mesh_.bones.size();
			unsigned int count_bones = 0;
			for (auto it_b : mesh_.bones) {
				bones[count_bones] = new aiBone();
				bones[count_bones]->mNumWeights = it_b.second.weights.size();
				bones[count_bones]->mWeights = new aiVertexWeight[bones[count_bones]->mNumWeights]();

				for (unsigned int idx_w = 0; idx_w < bones[count_bones]->mNumWeights; idx_w++) {

					aiVertexWeight weight;
					weight.mWeight = it_b.second.weights[idx_w];
					weight.mVertexId = it_b.second.idx_v[idx_w];
					bones[count_bones]->mWeights[idx_w] = weight;
					idx_w++;
				}
				matrix4 mat = it_b.second.offset_matrix;
				aiMatrix4x4 aiMat = aiMatrix4x4(mat.a.x, mat.a.y, mat.a.z, mat.a.t,
					mat.b.x, mat.b.y, mat.b.z, mat.b.t,
					mat.c.x, mat.c.y, mat.c.z, mat.c.t,
					mat.d.x, mat.d.y, mat.d.z, mat.d.t);
				bones[count_bones]->mName = it_b.second.name;
				std::cout << "Created bone: " << bones[count_bones]->mName.C_Str() << std::endl;
				bones[count_bones]->mOffsetMatrix = aiMat; 

				aiVector3t< float> scaling;
				aiQuaterniont< float > rotation;
				aiVector3t< float > position;
				aiMat.Decompose(scaling, rotation, position);
				std::cout << "position " << position.x << ", " << position.y << ", " << position.z << std::endl;
				std::cout << "rotation " << rotation.w << ", " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
				std::cout << "scaling " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl << std::endl;

				std::cout << "inverse bind matrix: " << std::endl;
				std::cout << mat.to_string() << std::endl;

				count_bones++;
			}
			
			if (mat_id_to_idx.count(mesh_.mat_id) == 0) {
				
				aimaterials[count_total_material] = guess_material_from_mat_struct(scene.mats[mesh_.mat_id], mesh_.mat_variant);
				mat_id_to_idx[mesh_.mat_id] = count_total_material;
				count_total_material++;
			}
			aimeshes[count_mesh]->mMaterialIndex = mat_id_to_idx[mesh_.mat_id];
		
			aiNodes[idx_node]->mMeshes[count_mesh_within_node] = count_mesh;


			
			count_mesh_within_node++;
			aimeshes[count_mesh]->mNumBones = mesh_.bones.size();
			aimeshes[count_mesh]->mBones = bones;
			aimeshes[count_mesh]->mNumVertices = mesh_.vertices.size();
			aimeshes[count_mesh]->mNumFaces = nb_faces;
			aimeshes[count_mesh]->mFaces = faces;
			aimeshes[count_mesh]->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
			aimeshes[count_mesh]->mName = mesh_.name;

			
			count_mesh++;
		}
		idx_node++;
	}
	aiAnimation** animations = new aiAnimation * [scene.anis.size()];
	unsigned int id_ani = 0;
	for (auto ani_ : scene.anis) {//mtb.data.size()

		
		aiAnimation* ani = new aiAnimation;
		ani->mDuration = ani_.second.duration;
		ani->mName = ani_.second.name;
		ani->mTicksPerSecond = ani_.second.ticks_per_second;
		aiNodeAnim** ani_nodes = new aiNodeAnim * [ani_.second.bones_data.size()];
		unsigned int bone_node_id = 0;
		for (auto it_nd : ani_.second.bones_data) {
			aiNodeAnim* node_ani = new aiNodeAnim();
			std::string current_bone = it_nd.first;
			
			std::vector<key_frame> keys = it_nd.second;

			std::vector<aiVectorKey> position_keys;
			std::vector<aiQuatKey> rotation_keys;
			std::vector<aiVectorKey> scaling_keys;

			aiVector3t< float> scaling;
			aiQuaterniont< float > rotation;
			aiVector3t< float > position;
			//aiMat.Decompose(scaling, rotation, position);

			if (!keys.empty()) {
				double first_key = keys[0].tick;
				std::ofstream f;
				f.open("debugkan7.txt", std::ios::app);
				f << "Name: " << current_bone << std::endl;
				for (auto key : keys) {

					aiVector3D pos = aiVector3D(key.position.x, key.position.y, key.position.z);
					aiVector3D scaling = aiVector3D(key.scaling.x, key.scaling.y, key.scaling.z);


					f << "Time: " << key.tick - first_key << " " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl;
					position_keys.push_back(aiVectorKey(key.tick - first_key, pos));
				
			
					rotation_keys.push_back(aiQuatKey(key.tick - first_key, aiQuaternion(key.rotation.x, key.rotation.y, key.rotation.z)));
			
					scaling_keys.push_back(aiVectorKey(key.tick - first_key, aiVector3D(scaling.x, scaling.y, scaling.z)));
			


				}
				f.close();
			}
			node_ani->mNodeName = current_bone;
			node_ani->mNumPositionKeys = position_keys.size();
			node_ani->mNumRotationKeys = rotation_keys.size();
			node_ani->mNumScalingKeys = scaling_keys.size();

			node_ani->mPositionKeys = new aiVectorKey[node_ani->mNumPositionKeys];
			node_ani->mRotationKeys = new aiQuatKey[node_ani->mNumRotationKeys];
			node_ani->mScalingKeys = new aiVectorKey[node_ani->mNumScalingKeys];

			memcpy(node_ani->mPositionKeys, position_keys.data(), node_ani->mNumPositionKeys * sizeof(aiVectorKey));
			memcpy(node_ani->mRotationKeys, rotation_keys.data(), node_ani->mNumRotationKeys * sizeof(aiQuatKey));
			memcpy(node_ani->mScalingKeys, scaling_keys.data(), node_ani->mNumScalingKeys * sizeof(aiVectorKey));


			ani_nodes[bone_node_id] = node_ani;
			bone_node_id++;
			

		}
		ani->mChannels = ani_nodes;
		ani->mNumChannels = bone_node_id;

		animations[id_ani] = ani;
		id_ani++;
	}

	//now we need to organize them.

	aiNode* rootNode = NULL;

	idx_node = 0;
	for (idx_node = 0; idx_node < nb_nodes; idx_node++) {
		aiNode* current_node = aiNodes[idx_node];
		std::string name = current_node->mName.C_Str();
		if (name.compare("root") == 0)
			rootNode = current_node;
		std::vector<std::string> children_ = children[idx_node];
		
		for (unsigned int i = 0; i < children_.size(); i++){

			current_node->mChildren[i] = map_ptr[children_[i]];
		}
	}
	

	aiScene *out = new aiScene();                       // deleted: by us after use
	out->mNumMeshes = real_nb_of_meshes;
	out->mMeshes = aimeshes;            // deleted: Version.cpp:151
	out->mNumMaterials = count_total_material;
	out->mMaterials = aimaterials; // deleted: Version.cpp:158
	out->mRootNode = rootNode;
	out->mMetaData = new aiMetadata(); // workaround, issue #3781
	out->mAnimations = animations;
	out->mNumAnimations = 1;
	// mtb.data.size();
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
	// deleting the scene will also take care of the vertices, faces, meshes, materials, chunks, etc.

	//delete out;

}
FBXExporter::FBXExporter()
{
	
}


FBXExporter::~FBXExporter()
{
}

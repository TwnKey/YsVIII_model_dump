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

void recur_setting_offset_matrix(aiNode* currentNode, std::map<std::string, std::vector<aiBone*>> bones_map, aiMatrix4x4 CurrentTransform) {
	std::string name_current_node = currentNode->mName.C_Str();
	aiMatrix4x4 NewTransform = CurrentTransform * currentNode->mTransformation;
	if (bones_map.count(name_current_node) > 0) {
		std::ofstream f;
		f.open("InversesBindMatrixes.txt", std::ios::app);
		f << "Name: " << name_current_node << std::endl;
		aiMatrix4x4 new_mat = NewTransform;
		std::string result = "";
		result = result + std::to_string(new_mat.a1) + ", " + std::to_string(new_mat.a2) + ", " + std::to_string(new_mat.a3) + ", " + std::to_string(new_mat.a4) + "\n";
		result = result + std::to_string(new_mat.b1) + ", " + std::to_string(new_mat.b2) + ", " + std::to_string(new_mat.b3) + ", " + std::to_string(new_mat.b4) + "\n";
		result = result + std::to_string(new_mat.c1) + ", " + std::to_string(new_mat.c2) + ", " + std::to_string(new_mat.c3) + ", " + std::to_string(new_mat.c4) + "\n";
		result = result + std::to_string(new_mat.d1) + ", " + std::to_string(new_mat.d2) + ", " + std::to_string(new_mat.d3) + ", " + std::to_string(new_mat.d4) + "\n";
		result = result + "\n";
		f << result << std::endl;
		f.close();
		for (aiBone* current_bone : bones_map[name_current_node]) {
			current_bone->mOffsetMatrix = new_mat.Inverse();
		}
		

	}

	if (currentNode->mNumChildren != 0) {

		for (unsigned int idx_child = 0; idx_child < currentNode->mNumChildren; idx_child++) {

			recur_setting_offset_matrix(currentNode->mChildren[idx_child], bones_map, NewTransform);
		}

	}


}
bool GetWorldTransform(aiNode* currentNode, aiMatrix4x4 CurrentTransform, std::string node_name, aiMatrix4x4 &result) {
	std::string name_current_node = currentNode->mName.C_Str();
	aiMatrix4x4 NewTransform = CurrentTransform * currentNode->mTransformation;
	if (name_current_node.compare(node_name) == 0) {
		result = NewTransform;
		return true;
	}
		
	else {
		if (currentNode->mNumChildren != 0) {

			for (unsigned int idx_child = 0; idx_child < currentNode->mNumChildren; idx_child++) {
				bool r = GetWorldTransform(currentNode->mChildren[idx_child], NewTransform, node_name, result);
				if (r) {
					return true;
				}
			}

		}
	}
	return false;

	

	


}

void get_components(aiMatrix4x4 mat) {



aiVector3t< float> scaling_bp;
aiQuaterniont< float > rotation_bp;
aiVector3t< float > position_bp;

mat.Decompose(scaling_bp, rotation_bp, position_bp);
std::cout << "scaling: " << scaling_bp.x << ", " << scaling_bp.y << ", " << scaling_bp.z << std::endl;
std::cout << "rotation: " << rotation_bp.w << ", " << rotation_bp.x << ", " << rotation_bp.y << ", " << rotation_bp.z << std::endl;
std::cout << "position: " << position_bp.x << ", " << position_bp.y << ", " << position_bp.z << std::endl;
}
void FBXExporter::ExportScene(Scene scene){
	
	aiMatrix4x4 transform_root = aiMatrix4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0, 
		0, 0, 0, 1);
	aiMatrix4x4 transform_NULL_ALL = aiMatrix4x4(1, 0, 0, 0,
												0, 1, 0, 0,
												0, 0, 1, 0,
												0, 0.000144, 1.2, 1);

	aiMatrix4x4 transform_Bone_root = aiMatrix4x4(0, 0, 1, 0,
		0, 1, 0, 0,
		-1, 0, 0, 0,
		0, 0, -0.061861, 1);

	aiMatrix4x4 transform_Bone_upper01 = aiMatrix4x4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0.027422, 0, 0, 1);

	transform_Bone_upper01 = transform_Bone_upper01.FromEulerAnglesXYZ(3.14, 3.14, 3.14);


	std::cout << scene.simple_nodes["Null_ALL"].transform.to_string() << std::endl;
	std::cout << scene.bones["Bone_root"]->transform.to_string() << std::endl;
	std::cout << scene.bones["Bone_upper01"]->transform.to_string() << std::endl;
	
	aiMatrix4x4 global_transform = transform_Bone_upper01;
	global_transform = global_transform.Inverse();
	std::string result = "";
	matrix4 inverse_bind_pose = scene.bones["Bone_upper01"]->offset_matrix;
	aiMatrix4x4 aiMat_bp = aiMatrix4x4(inverse_bind_pose.a.x, inverse_bind_pose.a.y, inverse_bind_pose.a.z, inverse_bind_pose.a.t,
		inverse_bind_pose.b.x, inverse_bind_pose.b.y, inverse_bind_pose.b.z, inverse_bind_pose.b.t,
		inverse_bind_pose.c.x, inverse_bind_pose.c.y, inverse_bind_pose.c.z, inverse_bind_pose.c.t,
		inverse_bind_pose.d.x, inverse_bind_pose.d.y, inverse_bind_pose.d.z, inverse_bind_pose.d.t);
	

	result = result + std::to_string(aiMat_bp.a1) + ", " + std::to_string(aiMat_bp.a2) + ", " + std::to_string(aiMat_bp.a3) + ", " + std::to_string(aiMat_bp.a4) + "\n";
	result = result + std::to_string(aiMat_bp.b1) + ", " + std::to_string(aiMat_bp.b2) + ", " + std::to_string(aiMat_bp.b3) + ", " + std::to_string(aiMat_bp.b4) + "\n";
	result = result + std::to_string(aiMat_bp.c1) + ", " + std::to_string(aiMat_bp.c2) + ", " + std::to_string(aiMat_bp.c3) + ", " + std::to_string(aiMat_bp.c4) + "\n";
	result = result + std::to_string(aiMat_bp.d1) + ", " + std::to_string(aiMat_bp.d2) + ", " + std::to_string(aiMat_bp.d3) + ", " + std::to_string(aiMat_bp.d4) + "\n";
	result = result + "\n";
	std::cout << result << std::endl;
	get_components(aiMat_bp);
	result = "";
	result = result + std::to_string(global_transform.a1) + ", " + std::to_string(global_transform.a2) + ", " + std::to_string(global_transform.a3) + ", " + std::to_string(global_transform.a4) + "\n";
	result = result + std::to_string(global_transform.b1) + ", " + std::to_string(global_transform.b2) + ", " + std::to_string(global_transform.b3) + ", " + std::to_string(global_transform.b4) + "\n";
	result = result + std::to_string(global_transform.c1) + ", " + std::to_string(global_transform.c2) + ", " + std::to_string(global_transform.c3) + ", " + std::to_string(global_transform.c4) + "\n";
	result = result + std::to_string(global_transform.d1) + ", " + std::to_string(global_transform.d2) + ", " + std::to_string(global_transform.d3) + ", " + std::to_string(global_transform.d4) + "\n";
	result = result + "\n";
	std::cout << result << std::endl;
	get_components(global_transform);
	for (auto child : scene.meshes["c000r_main"][0].bones) {
		std::cout << child.first << std::endl;
		std::cout << scene.bones[child.first]->offset_matrix.to_string() << std::endl;
		matrix4 inverse_bind_pose = scene.bones[child.first]->offset_matrix;
		aiMatrix4x4 aiMat_bp = aiMatrix4x4(inverse_bind_pose.a.x, inverse_bind_pose.a.y, inverse_bind_pose.a.z, inverse_bind_pose.a.t,
			inverse_bind_pose.b.x, inverse_bind_pose.b.y, inverse_bind_pose.b.z, inverse_bind_pose.b.t,
			inverse_bind_pose.c.x, inverse_bind_pose.c.y, inverse_bind_pose.c.z, inverse_bind_pose.c.t,
			inverse_bind_pose.d.x, inverse_bind_pose.d.y, inverse_bind_pose.d.z, inverse_bind_pose.d.t);

		get_components(aiMat_bp);
	}


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
	std::map<std::string, std::vector<aiBone*>> bones_ptr;
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

		aiNodes[idx_node]->mTransformation = transform_node.Transpose(); 
		
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

		aiNodes[idx_node]->mTransformation = transform_node.Transpose();
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
				bones[count_bones]->mOffsetMatrix = aiMat;
				bones_ptr[bones[count_bones]->mName.C_Str()].push_back(bones[count_bones]);
				/*aiVector3t< float> scaling;
				aiQuaterniont< float > rotation;
				aiVector3t< float > position;
				aiMat.Decompose(scaling, rotation, position);
				std::cout << "position " << position.x << ", " << position.y << ", " << position.z << std::endl;
				std::cout << "rotation " << rotation.w << ", " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
				std::cout << "scaling " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl << std::endl;

				std::cout << "inverse bind matrix: " << std::endl;
				std::cout << mat.to_string() << std::endl;*/

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
			map_ptr[children_[i]]->mParent = current_node;
		}
	}
	
	recur_setting_offset_matrix(rootNode, bones_ptr, aiMatrix4x4());
	/*
	for (aiBone* b_ptr : bones_ptr) {
		aiNode* current_node = map_ptr[b_ptr->mName.C_Str()];
		aiMatrix4x4 global_transform = current_node->mTransformation;
		while (current_node->mParent != NULL) {
			aiMatrix4x4 local_transform = current_node->mParent->mTransformation;
			/*std::cout << " parent " << current_node->mParent->mName.C_Str() << std::endl;
			std::string result = "";
			result = result + std::to_string(local_transform.a1) + ", " + std::to_string(local_transform.a2) + ", " + std::to_string(local_transform.a3) + ", " + std::to_string(local_transform.a4) + "\n";
			result = result + std::to_string(local_transform.b1) + ", " + std::to_string(local_transform.b2) + ", " + std::to_string(local_transform.b3) + ", " + std::to_string(local_transform.b4) + "\n";
			result = result + std::to_string(local_transform.c1) + ", " + std::to_string(local_transform.c2) + ", " + std::to_string(local_transform.c3) + ", " + std::to_string(local_transform.c4) + "\n";
			result = result + std::to_string(local_transform.d1) + ", " + std::to_string(local_transform.d2) + ", " + std::to_string(local_transform.d3) + ", " + std::to_string(local_transform.d4) + "\n";
			result = result + "\n";
			std::cout << result << std::endl;


			global_transform = local_transform * global_transform;
			current_node = current_node->mParent;

		}
		global_transform = global_transform.Inverse();
		b_ptr->mOffsetMatrix = global_transform;
		
	}*/
	
	aiAnimation** animations = new aiAnimation * [scene.anis.size()];
	unsigned int id_ani = 0;
	for (auto ani_ : scene.anis) {//mtb.data.size()


		aiAnimation* ani = new aiAnimation;
		ani->mDuration = ani_.second.duration;
		ani->mName = ani_.second.name;
		ani->mTicksPerSecond = ani_.second.ticks_per_second;
		size_t nb_of_bones_with_ibm = scene.bones.size();

		aiNodeAnim** ani_nodes = new aiNodeAnim * [nb_of_bones_with_ibm];

		unsigned int count_bones_with_ibm = 0;
		for (auto it_nd : ani_.second.bones_data) {
			std::string current_bone = it_nd.first;
			aiNode* current_node = map_ptr[current_bone];
			if (scene.bones.count(current_bone) > 0)
			{
				aiNodeAnim* node_ani = new aiNodeAnim();


				std::vector<key_frame> keys = it_nd.second;

				std::vector<aiVectorKey> position_keys;
				std::vector<aiQuatKey> rotation_keys;
				std::vector<aiVectorKey> scaling_keys;

				aiVector3t< float> scaling;
				aiQuaterniont< float > rotation;
				aiVector3t< float > position;
				//aiMat.Decompose(scaling, rotation, position);


				//matrix4 inverse_bind_pose = scene.bones[current_bone]->offset_matrix;

				aiVector3t< float> scaling_bp;
				aiQuaterniont< float > rotation_bp;
				aiVector3t< float > position_bp;
				/*aiMatrix4x4 aiMat_bp = aiMatrix4x4(inverse_bind_pose.a.x, inverse_bind_pose.a.y, inverse_bind_pose.a.z, inverse_bind_pose.a.t,
					inverse_bind_pose.b.x, inverse_bind_pose.b.y, inverse_bind_pose.b.z, inverse_bind_pose.b.t,
					inverse_bind_pose.c.x, inverse_bind_pose.c.y, inverse_bind_pose.c.z, inverse_bind_pose.c.t,
					inverse_bind_pose.d.x, inverse_bind_pose.d.y, inverse_bind_pose.d.z, inverse_bind_pose.d.t);*/
				aiMatrix4x4 world;
				bool result = GetWorldTransform(rootNode, aiMatrix4x4(), current_bone, world);

				std::ofstream f;
				f.open("WorldTransforms.txt", std::ios::app);
				f << "Name: " << current_bone << std::endl;
				aiMatrix4x4 new_mat = world;
				std::string result_str = "";
				result_str = result_str + std::to_string(new_mat.a1) + ", " + std::to_string(new_mat.a2) + ", " + std::to_string(new_mat.a3) + ", " + std::to_string(new_mat.a4) + "\n";
				result_str = result_str + std::to_string(new_mat.b1) + ", " + std::to_string(new_mat.b2) + ", " + std::to_string(new_mat.b3) + ", " + std::to_string(new_mat.b4) + "\n";
				result_str = result_str + std::to_string(new_mat.c1) + ", " + std::to_string(new_mat.c2) + ", " + std::to_string(new_mat.c3) + ", " + std::to_string(new_mat.c4) + "\n";
				result_str = result_str + std::to_string(new_mat.d1) + ", " + std::to_string(new_mat.d2) + ", " + std::to_string(new_mat.d3) + ", " + std::to_string(new_mat.d4) + "\n";
				result_str = result_str + "\n";
				f << result_str << std::endl;
				f.close();


				aiMatrix4x4 test = world * bones_ptr[current_bone][0]->mOffsetMatrix;

				if (result)
					world.Decompose(scaling_bp, rotation_bp, position_bp);

				/*scaling_bp.x = 1;
				scaling_bp.y = 1;
				scaling_bp.z = 1;

				position_bp.x = 0;
				position_bp.y = 0;
				position_bp.z = 0;

				rotation_bp.w = 1;
				rotation_bp.x = 0;
				rotation_bp.y = 0;
				rotation_bp.z = 0;*/

				if (!keys.empty()) {
					double first_key = keys[0].tick;
					std::ofstream f;
					f.open("debugkan7.txt", std::ios::app);
					f << "Name: " << current_bone << std::endl;
					for (auto key : keys) {

						aiVector3D pos = position_bp;//+ aiVector3D(key.position.x, key.position.y, key.position.z);
						aiVector3D scaling = scaling_bp;// +aiVector3D(key.scaling.x, key.scaling.y, key.scaling.z);
						aiQuaternion rotation = rotation_bp;// *aiQuaternion(key.rotation.x, key.rotation.y, key.rotation.z);

						f << "Time: " << key.tick - first_key << " " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
						f << "" << key.tick - first_key << " " << scaling.x << ", " << scaling.y << ", " << scaling.z << std::endl;
						f << "" << key.tick - first_key << " " << rotation.w << " " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
						position_keys.push_back(aiVectorKey(key.tick - first_key, pos));


						rotation_keys.push_back(aiQuatKey(key.tick - first_key, rotation));

						scaling_keys.push_back(aiVectorKey(key.tick - first_key, scaling));



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


				ani_nodes[count_bones_with_ibm] = node_ani;
				count_bones_with_ibm++;
			}
		}
		if (count_bones_with_ibm > 0) {
			ani->mChannels = ani_nodes;
			ani->mNumChannels = count_bones_with_ibm;
			animations[id_ani] = ani;
			id_ani++;
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
	exporter.Export(out, "glb2",  "model.glb");
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

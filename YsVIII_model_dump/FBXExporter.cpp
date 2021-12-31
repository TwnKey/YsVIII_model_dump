#include "FBXExporter.h"
#include <assimp\scene.h>
#include <assimp\Exporter.hpp>
#include <iostream>


void FBXExporter::GenerateScene(IT3File file){
	file.nodes.erase(file.nodes.begin() + 0x6E);
	size_t nb_meshes = 0;
	size_t nb_nodes = file.nodes.size();

	for (const node& nd : file.nodes) {
		if (nd.vpax) {
			nb_meshes+= nd.vpax->meshes_d.size();
		}
	}

	aiNode** ainodes = new aiNode* [nb_nodes];
	aiMesh** aimeshes = new aiMesh * [nb_meshes];
	aiMaterial** aimaterials = new aiMaterial * [nb_meshes];

	unsigned int count_total_mesh = 0;
	//first we create all ainodes separately, then we will organize them.
	for (unsigned int idx_node = 0; idx_node < nb_nodes; idx_node++) {

		node current_node = file.nodes[idx_node];

		ainodes[idx_node] = new aiNode();
		ainodes[idx_node]->mName = current_node.info->text_id1;
		//creating mesh if there is any in the node
		if (current_node.vpax) {
			size_t nb_mesh_in_vpax = current_node.vpax->meshes_d.size();
			ainodes[idx_node]->mMeshes = new unsigned[nb_mesh_in_vpax];
			ainodes[idx_node]->mNumMeshes = nb_mesh_in_vpax;
			size_t count_mesh_ = 0;
			for (auto mesh_ : current_node.vpax->meshes_d) {
				aiVector3D* vertices = new aiVector3D[mesh_.vertices.size()];

				std::cout << "OBJ EXPORT " << std::endl;
				for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
				{
					vertices[idx_v].x = mesh_.vertices[idx_v].position.x;
					vertices[idx_v].y = mesh_.vertices[idx_v].position.y;
					vertices[idx_v].z = mesh_.vertices[idx_v].position.z;
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

				aiMaterial* material = new aiMaterial();            // deleted: Version.cpp:155
				aimaterials[count_total_mesh] = material;
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
	out->mNumMaterials = nb_meshes;
	out->mMaterials = aimaterials; // deleted: Version.cpp:158
	out->mRootNode = rootNode;
	out->mMetaData = new aiMetadata(); // workaround, issue #3781

	// and we're good to go. do whatever:

	Assimp::Exporter exporter;
	//try{
	/*std::cout << "format supported: " << std::endl;
	for (unsigned i = 0; i < exporter.GetExportFormatCount(); i++) {
		const aiExportFormatDesc * desc = exporter.GetExportFormatDescription(i);
		std::cout << desc->description << " " << desc->id << std::endl;
	}*/
	//exporter.Export(out, "objnomtl", "model.obj");
	exporter.Export(out, "fbx",  "model.fbx");
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


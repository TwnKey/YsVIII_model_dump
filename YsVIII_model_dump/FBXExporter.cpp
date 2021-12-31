#include "FBXExporter.h"
#include <assimp\scene.h>
#include <assimp\Exporter.hpp>
#include <iostream>


void GenerateScene_doesntwork(VPAX vpax) {
	
	/*aiVector3D *vertices = new aiVector3D[vpax.vertices.size()];
	for (unsigned int idx_v = 0; idx_v < vpax.vertices.size(); idx_v++)
	{
		vertices[idx_v].x = vpax.vertices[idx_v].position.x;
		vertices[idx_v].y = vpax.vertices[idx_v].position.y;
		vertices[idx_v].z = vpax.vertices[idx_v].position.z;
		
	}*/
	aiVector3D *vertices = new aiVector3D[3]{          // deleted: mesh.h:758
		{ -1, -1, 0 },
		{ 0, 1, 0 },
		{ 1, -1, 0 }
	};
	
	/*size_t nb_faces = vpax.indexes.size() / 3; // each face has 3 vertices?
	aiFace * faces = new aiFace[nb_faces];
	
	for (unsigned int idx_face = 0; idx_face < nb_faces; idx_face++)
	{
		aiFace f;
		faces[idx_face].mNumIndices = 3;
		faces[idx_face].mIndices = new unsigned[3];
		faces[idx_face].mIndices[0] = vpax.indexes[0 + idx_face * 3];
		faces[idx_face].mIndices[1] = vpax.indexes[1 + idx_face * 3];
		faces[idx_face].mIndices[2] = vpax.indexes[2 + idx_face * 3];
	}*/
	
	aiFace *faces = new aiFace[1];                      // deleted: mesh.h:784
	faces[0].mNumIndices = 3;
	faces[0].mIndices = new unsigned [3] { 0, 1, 2 };    // deleted: mesh.h:149


	aiMesh *mesh = new aiMesh();                        // deleted: Version.cpp:150
	mesh->mNumVertices = 3;//vpax.vertices.size();
	mesh->mVertices = vertices;
	mesh->mNumFaces = 1;//nb_faces;
	mesh->mFaces = faces;
	mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE; // workaround, issue #3778

	// a valid material is needed, even if its empty

	aiMaterial *material = new aiMaterial();            // deleted: Version.cpp:155

	// a root node with the mesh list is needed; if you have multiple meshes, this must match.

	aiNode *root = new aiNode();                        // deleted: Version.cpp:143
	root->mNumMeshes = 1;
	root->mMeshes = new unsigned [1] { 0 };              // deleted: scene.cpp:77

	// pack mesh(es), material, and root node into a new minimal aiScene

	aiScene *out = new aiScene();                       // deleted: by us after use
	out->mNumMeshes = 1;
	out->mMeshes = new aiMesh * [1] { mesh };            // deleted: Version.cpp:151
	out->mNumMaterials = 1;
	out->mMaterials = new aiMaterial * [1] { material }; // deleted: Version.cpp:158
	out->mRootNode = root;
	out->mMetaData = new aiMetadata(); // workaround, issue #3781

	// and we're good to go. do whatever:

	Assimp::Exporter exporter;
	if (exporter.Export(out, "objnomtl", vpax.name+".obj") != AI_SUCCESS)
		std::cerr << "ok" << std::endl;

	// deleting the scene will also take care of the vertices, faces, meshes, materials, nodes, etc.
	
	delete out;
	//delete out;
}

void FBXExporter::GenerateScene(VPAX vpax){
	aiVector3D *vertices = new aiVector3D[vpax.vertices.size()];
	
	std::cout << "OBJ EXPORT: " << std::endl;
	for (unsigned int idx_v = 0; idx_v < vpax.vertices.size(); idx_v++)
	{
		
		vertices[idx_v].x = vpax.vertices[idx_v].position.x;
		vertices[idx_v].y = vpax.vertices[idx_v].position.y;
		vertices[idx_v].z = vpax.vertices[idx_v].position.z;
		std::cout << vertices[idx_v].x << ", " << vertices[idx_v].y << ", " << vertices[idx_v].z << std::endl;
	}

	/*
aiVector3D *vertices = new aiVector3D[3]{          // deleted: mesh.h:758
		{ -1, -1, 0 },
		{ 0, 1, 0 },
		{ 1, -1, 0 }
};*/
/*
aiFace *faces = new aiFace[1];                      // deleted: mesh.h:784
faces[0].mNumIndices = 3;
faces[0].mIndices = new unsigned [3] { 0, 1, 2 };*/    // deleted: mesh.h:149

	size_t nb_faces = vpax.indexes.size() / 3; // each face has 3 vertices?
	aiFace * faces = new aiFace[nb_faces];

	for (unsigned int idx_face = 0; idx_face < nb_faces; idx_face++)
	{
		aiFace f;
		faces[idx_face].mNumIndices = 3;
		faces[idx_face].mIndices = new unsigned[3];
		faces[idx_face].mIndices[0] = vpax.indexes[0 + idx_face * 3];
		faces[idx_face].mIndices[1] = vpax.indexes[1 + idx_face * 3];
		faces[idx_face].mIndices[2] = vpax.indexes[2 + idx_face * 3];
		std::cout << faces[idx_face].mIndices[0] << ", " << faces[idx_face].mIndices[1] << ", " << faces[idx_face].mIndices[2] << std::endl;

	}

aiMesh *mesh = new aiMesh();                        // deleted: Version.cpp:150
mesh->mNumVertices = vpax.vertices.size();
mesh->mVertices = vertices;
mesh->mNumFaces = nb_faces;
mesh->mFaces = faces;
mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE; // workaround, issue #3778

std::cout << "Nb vertices: " << std::hex << mesh->mNumVertices << std::endl;
std::cout << "Nb faces: " << std::hex << mesh->mNumFaces << std::endl;

// a valid material is needed, even if its empty

aiMaterial *material = new aiMaterial();            // deleted: Version.cpp:155

// a root node with the mesh list is needed; if you have multiple meshes, this must match.

aiNode *root = new aiNode();                        // deleted: Version.cpp:143
root->mNumMeshes = 1;
root->mMeshes = new unsigned [1] { 0 };              // deleted: scene.cpp:77

// pack mesh(es), material, and root node into a new minimal aiScene

aiScene *out = new aiScene();                       // deleted: by us after use
out->mNumMeshes = 1;
out->mMeshes = new aiMesh * [1] { mesh };            // deleted: Version.cpp:151
out->mNumMaterials = 1;
out->mMaterials = new aiMaterial * [1] { material }; // deleted: Version.cpp:158
out->mRootNode = root;
out->mMetaData = new aiMetadata(); // workaround, issue #3781

// and we're good to go. do whatever:

Assimp::Exporter exporter;

exporter.Export(out, "objnomtl", vpax.name + ".obj");
// deleting the scene will also take care of the vertices, faces, meshes, materials, nodes, etc.

delete out;

}
FBXExporter::FBXExporter()
{
	
}


FBXExporter::~FBXExporter()
{
}


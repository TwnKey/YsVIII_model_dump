#pragma once
#include <map>
#include <string>
#include "utilities.h"
#include "IT3File.h"
#include "MTBFile.h"
#include <map>

class node {
public:
	node() = default;
	std::string name;
	matrix4 transform;
	std::vector<std::string> children;
	node(std::string name, matrix4 mat) :name(name), transform(mat) {}
};


struct simple_node : node { 
	simple_node() = default;
	simple_node(std::string name, matrix4 mat) :node(name,mat) {}
};

struct material {
	std::string name;
	std::vector<std::string> textures_paths;
};
struct bone : node { //keep in mind a bone can be shared between different meshes
	std::string name;
	matrix4 offset_matrix;
	std::vector<float> weights;
	std::vector<unsigned int> idx_v;

	bone() = default;

	bone(std::string name, matrix4 mat) :node(name, mat) {}
};
struct mesh : node {
	std::map<std::string, bone> bones;
	std::vector<vector3<float>> vertices;
	std::vector<vector2<float>> uv;
	std::vector<vector3<unsigned int>> faces_indexes;
	unsigned int mat_id;
	unsigned int mat_variant;
	mesh() = default;
	mesh(std::string name, matrix4 mat) :node(name, mat) {}

};



struct key_frame {
	unsigned int tick;
	vector3<float> position;
	vector3<float> rotation;
	vector3<float> scaling;
	key_frame(unsigned int tick, vector3<float> p, vector3<float> r, vector3<float> s) : tick(tick), position(p), rotation(r), scaling(s) {}
};
struct animation {
	std::string name;
	unsigned int duration;
	unsigned int ticks_per_second;
	std::map<std::string, std::vector<key_frame>> bones_data; //bone name
	animation() = default;
	animation(std::string name, unsigned int duration, unsigned int ticks_per_second) : name(name), duration(duration), ticks_per_second(ticks_per_second){}
};



class Scene
{
public:
	Scene(IT3File it3_p, IT3File it3_m, MTBFile mtb);
	~Scene() = default;
	std::map<std::string, simple_node> simple_nodes; //node name
	std::map<std::string, std::vector<mesh>> meshes; //node name
	std::map<std::string, animation> anis; //animation name
	std::map<unsigned int, material> mats;
	std::map<std::string, bone *> bones;
};


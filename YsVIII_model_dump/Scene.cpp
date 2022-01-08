#include "Scene.h"
Scene::Scene(IT3File it3_p, IT3File it3_m, MTBFile mtb) {

	it3_p.add_kan7_from_m_file(it3_m);

	/* The plan is to: 
	* - go through each chunk in the it3_p file
	* - if it has vpax, create a mesh node that we will add to our map of meshes 
	* - if it has bon3, create a bone node that we will add to our map of bones 
	* - if it has none of the previous stuff, create a simple node
	* - if it has kan7, create animation data for the bone
	* - if it has a material chunk, get the path of the textures, that's all we need
	* everytime a node is created, it is inserted in the node map of the scene
	* 
	* */
	size_t start_count_materials = 0;
	for (auto it : it3_p.chunks) {

		INFO * current_info = it.second.info;
		BON3* current_bone = it.second.bon3;
		VPAX* current_vpax = it.second.vpax;
		MAT6* current_mat6 = it.second.mat6;
		CHID* current_chid = it.second.chid;

		if (!current_vpax) { //if it doesn't have a mesh, it's a "simple" node
			simple_node nd = simple_node(current_info->text_id1, current_info->transform);

			if (current_chid) {
				std::cout << "CHILDREN!! " << current_info->text_id1 << std::endl;
				for (auto name : current_chid->children) {
					

						std::cout << " " << name << std::endl;

						nd.children.push_back(name);
					
					
				}
					
			}

			simple_nodes[current_info->text_id1] = nd;
			
		}
		else {
			
			size_t nb_mesh_in_vpax = current_vpax->meshes_d.size();
			size_t count_materials = 0;
			std::map<unsigned int, material> mat_map;

			for (auto mesh_ : current_vpax->meshes_d) {
				mesh m(current_info->text_id1, current_info->transform);
				
				for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
				{
					vector3<float> vertices;
					vector2<float> uv;

					vertices.x = mesh_.vertices[idx_v].position.x;
					vertices.y = mesh_.vertices[idx_v].position.y;
					vertices.z = mesh_.vertices[idx_v].position.z;

					uv.x = abs(mesh_.vertices[idx_v].uv.x);
					uv.y = abs(1 - mesh_.vertices[idx_v].uv.y);
					
					m.vertices.push_back(vertices);
					m.uv.push_back(uv);


					for (unsigned int idx_weight = 0; idx_weight < 8; idx_weight++) {
						uint8_t weight = mesh_.vertices[idx_v].weights[idx_weight];
						float weight_f = (float)weight / 255;
						uint8_t joint_id = mesh_.vertices[idx_v].bones_indexes[idx_weight] - 1;
						if (joint_id != 0xFF) {
							
							std::string joint_name = current_bone->joints_names[joint_id];
							m.bones[joint_name].name = joint_name;
							m.bones[joint_name].weights.push_back(weight_f);
							m.bones[joint_name].idx_v.push_back(idx_v);
						}

					}

				}

				size_t nb_faces = mesh_.indexes.size() / 3; // each face has 3 vertices?
				
				for (unsigned int idx_face = 0; idx_face < nb_faces; idx_face++)
				{
					vector3<unsigned int> indexes;
					indexes.x = mesh_.indexes[0 + idx_face * 3];
					indexes.y = mesh_.indexes[1 + idx_face * 3];
					indexes.z = mesh_.indexes[2 + idx_face * 3];
					m.faces_indexes.push_back(indexes);
				}

				//adding inverse bind matrix and transform to the bones
				for (auto &b : m.bones) {
					
					INFO* bone_info = it3_p.chunks[b.second.name].info;
					m.bones[b.second.name].transform = bone_info->transform;
					m.bones[b.second.name].offset_matrix = current_bone->bones[b.second.name].offset_matrix;
					if (b.second.name.compare("Bone_bag_cloth") == 0)
					{
						std::cout << "Bone_bag_cloth!! " << m.name << std::endl;
						std::cout << m.bones[b.second.name].offset_matrix.to_string() << std::endl;
					}
					bones[b.second.name] = &m.bones[b.second.name];
				}

				if (current_mat6) {
					if (mat_map.count(start_count_materials + mesh_.material_id) > 0)
						m.mat_id = start_count_materials + mesh_.material_id;
					else {
						material mat;
						material_data current_mat_data = current_mat6->mats[mesh_.material_id];
						mat.name = current_mat_data.name;
						for (unsigned int i = 0; i < current_mat_data.textures.size(); i++) {
							mat.textures_paths.push_back(current_mat_data.textures[i].name + ".dds");
						}
						m.mat_id = start_count_materials + mesh_.material_id;
						m.mat_variant = it.second.rty2->material_variant;
						mats[m.mat_id] = mat;
						count_materials++;
					}
					
				}

				if (current_chid) {
					for (auto name : current_chid->children) {
						
							std::cout << "CHILDREN!! " << current_info->text_id1 << " " << name << std::endl;
						
						m.children.push_back(name);
					}
						
				}
				
				meshes[current_info->text_id1].push_back(m);
			}
			start_count_materials += count_materials;

		}
	}

	//Creating animations
	for (unsigned int id_ani = 0; id_ani < 1; id_ani++) {//mtb.data.size()

		motion_data m_d = mtb.data[id_ani];
		animation ani(m_d.name, mtb.data[id_ani].duration, 24);
		
		
		unsigned int bone_chunk_id = 0;
		for (auto it_nd : it3_p.chunks) {
			chunk current_chunk = it_nd.second;
			if (current_chunk.kan7) {
				
				auto first = std::find_if(current_chunk.kan7->kans[0].begin(), current_chunk.kan7->kans[0].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.start; });
				auto last = std::find_if(current_chunk.kan7->kans[0].begin(), current_chunk.kan7->kans[0].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.end; });

				std::vector<key_animation> keys(first, last);

				for (auto key : keys) {

					
					vector3<float> position_keys;
					vector3<float> rotation_keys;
					vector3<float> scaling_keys;

					position_keys = { key.position.x, key.position.y, key.position.z };
					rotation_keys = { key.rotation.x, key.rotation.y, key.rotation.z };
					scaling_keys = { key.scale.x, key.scale.y, key.scale.z };
					key_frame kf(key.tick, position_keys, rotation_keys, scaling_keys);
					ani.bones_data[it_nd.second.info->text_id1].push_back(kf);
				}
				
				
			}

		}
		anis[m_d.name] = ani;
	}

}
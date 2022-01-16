#include "Scene.h"
#include <assimp\scene.h>
#include <algorithm>






matrix4 construct_matrix_from_row(vector4<float> row) {
	matrix4 result;
	float fVar7 = 0.0;
	float fVar1 = row.x;
	float fVar2 = row.y;
	float fVar3 = row.z;
	float fVar4 = row.t;
	float fVar5 = fVar1 * fVar1 + fVar2 * fVar2 + fVar3 * fVar3 + fVar4 * fVar4;
	if (0.0 < fVar5) {
		fVar7 = 2.0 / fVar5;
	}
	result.c.t = 0.0;
	float fVar6 = fVar3 * fVar7;
	result.b.t = 0.0;
	result.a.t = 0.0;
	fVar5 = fVar2 * fVar7;
	result.d.z = 0.0;
	result.d.y = 0.0;
	float fVar8 = fVar1 * fVar1 * fVar7;
	result.d.x = 0.0;
	result.d.t = 1.0;
	fVar7 = fVar4 * fVar1 * fVar7;
	result.a.x = 1.0 - (fVar3 * fVar6 + fVar2 * fVar5);
	result.b.x = fVar4 * fVar6 + fVar1 * fVar5;
	result.a.y = fVar1 * fVar5 - fVar4 * fVar6;
	result.c.x = fVar1 * fVar6 - fVar4 * fVar5;
	result.a.z = fVar4 * fVar5 + fVar1 * fVar6;
	result.b.y = 1.0 - (fVar3 * fVar6 + fVar8);
	result.c.z = 1.0 - (fVar2 * fVar5 + fVar8);
	result.c.y = fVar7 + fVar2 * fVar6;
	result.b.z = fVar2 * fVar6 - fVar7;

	return result;
}
vector4<float> a_single_change(float euler, vector4<float> identity_r) {
	vector4<float> res = identity_r;
	float dVar2 = euler / 2;
	dVar2 = sin(dVar2);
	float fVar3 = identity_r.x * dVar2;
	float fVar4 = identity_r.y * dVar2;
	float fVar1 = identity_r.z * dVar2;
	dVar2 = euler / 2;
	dVar2 = cos(dVar2);
	float fVar6 = dVar2;

	dVar2 = (double)(fVar4 * fVar4 + fVar3 * fVar3 + fVar1 * fVar1 + dVar2 * dVar2);
	dVar2 = sqrt(dVar2);


	if (dVar2 == 0) {
		res.x = 1;
		res.y = 0;
		res.z = 0;
		res.t = 0;

	}
	else {
		float fVar5 = 1 / dVar2;
		res.x = fVar3 * fVar5;
		res.y = fVar4 * fVar5;
		res.z = fVar1 * fVar5;
		res.t = fVar5 * fVar6;

	}
	return res;


}

matrix4 reproduce_the_rot_mat_from_ghidra(vector3<float> fs, unsigned int variant) {


	vector4<float> identity_row1 = { 1.0, 0.0, 0.0, 0.0 };
	vector4<float> identity_row2 = { 0.0, 1.0, 0.0, 0.0 };
	vector4<float> identity_row3 = { 0.0, 0.0, 1.0, 0.0 };
	vector4<float> identity_row4 = { 0.0, 0.0, 0.0, 1.0 };

	vector4<float> quat1 = a_single_change(fs.x, identity_row1);
	vector4<float> quat2 = a_single_change(fs.y, identity_row2);
	vector4<float> quat3 = a_single_change(fs.z, identity_row3);

	matrix4 result;

	switch (variant) {
	case 0x02: {
		result.a = quat1;
		result.b = quat3;
		result.c = quat2;
		break;
	}
	case 0x03: {
		//132
		result.b = quat1;
		result.a = quat3;
		result.c = quat2;
		break;
	}
	case 0x04: {
		result.c = quat1;
		result.a = quat3;
		result.b = quat2;
		break;
	}
	case 0x05: {
		result.b = quat1;
		result.c = quat3;
		result.a = quat2;
		break;
	}
	case 0x06: {
		result.c = quat1;
		result.b = quat3;
		result.a = quat2;
		break;
	}
	default: {
		result.a = quat1;
		result.b = quat3;
		result.c = quat2;
		break;
	}

	}
	result.d = identity_row4;
	return result;
}


vector3<matrix4> produce_four_matrixes(vector3<float> fs, unsigned int variant) {
	vector3<matrix4> mats;

	matrix4 tmp = reproduce_the_rot_mat_from_ghidra(fs, variant);

	mats.x = construct_matrix_from_row(tmp.a);
	
	mats.y = construct_matrix_from_row(tmp.b);
	mats.z = construct_matrix_from_row(tmp.c);
	return mats;
}

vector4<float> FUN_006dacf0(matrix4 mat)

{
	float fVar1;
	float fVar2;
	int iVar3;
	unsigned int uVar4;
	unsigned int uVar5;
	unsigned int uVar6;
	float fVar7;
	double dVar8;
	float fVar9;

	vector4<float> result;

	fVar9 = mat.a.x;
	fVar1 = mat.b.y;
	fVar2 = mat.c.z; 

	result.x = 0.0;
	result.y = 0.0;
	result.z = 0.0;
	result.t = 0.0;

	float multiplied_mat[16];

	multiplied_mat[0] = mat.a.x;
	multiplied_mat[1] = mat.a.y;
	multiplied_mat[2] = mat.a.z;
	multiplied_mat[3] = mat.a.t;
	multiplied_mat[4] = mat.b.x;
	multiplied_mat[5] = mat.b.y;
	multiplied_mat[6] = mat.b.z;
	multiplied_mat[7] = mat.b.t;
	multiplied_mat[8] = mat.c.x;
	multiplied_mat[9] = mat.c.y;
	multiplied_mat[10] = mat.c.z;
	multiplied_mat[11] = mat.c.t;
	multiplied_mat[12] = mat.d.x;
	multiplied_mat[13] = mat.d.y;
	multiplied_mat[14] = mat.d.z;
	multiplied_mat[15] = mat.d.t;

	

	fVar7 = fVar9 + fVar1 + fVar2 + mat.d.t;
	if (1.0 <= fVar7) {
		dVar8 = (double)fVar7;
		dVar8 = sqrt(dVar8);
		fVar9 = 1.0 / ((float)dVar8 * 2.0);
		result.t = (float)dVar8 * 2.0 * 0.25;
		result.x = (mat.c.y - mat.b.z) * fVar9;
		result.y = (mat.a.z - mat.c.x) * fVar9;
		result.z = (mat.b.x - mat.a.y) * fVar9;
		return result;
	}
	
	uVar6 = (unsigned int)(fVar9 <= fVar1);
	if (multiplied_mat[uVar6 * 5] <= fVar2 && fVar2 != multiplied_mat[uVar6 * 5]) {
		uVar6 = 2;
	}
	
	
	uVar4 = (uVar6 + 1) % 3;
	uVar5 = (uVar4 + 1) % 3;
	dVar8 = (double)(((multiplied_mat[uVar6 * 5] - multiplied_mat[uVar4 * 5]) -
		multiplied_mat[uVar5 * 5]) + 1.0);
	dVar8 = sqrt(dVar8);
	fVar9 = (float)dVar8 * 2.0;
	if (uVar6 == 0) {
		result.x = fVar9 * 0.25;
	}
	else {
		if (uVar6 == 1) {
			result.y = fVar9 * 0.25;
		}
		else {
			if (uVar6 == 2) {
				result.z = fVar9 * 0.25;
			}
		}
	}
	if (1e-07 <= fVar9) {
		fVar9 = 1.0 / fVar9;
	}
	else {
		fVar9 = 0.0;
	}
	if (uVar4 == 0) {
		result.x = (multiplied_mat[uVar6 * 4] + multiplied_mat[uVar6]) * fVar9;
	}
	else {
		if (uVar4 == 1) {
			result.y = (multiplied_mat[uVar6 + 4] + multiplied_mat[uVar6 * 4 + 1]) * fVar9;
		}
		else {
			if (uVar4 == 2) {
				result.z = (multiplied_mat[uVar6 + 8] + multiplied_mat[uVar6 * 4 + 2]) * fVar9;
			}
		}
	}
	if (uVar5 == 0) {
		result.x = (multiplied_mat[uVar6 * 4] + multiplied_mat[uVar6]) * fVar9;
	}
	else {
		if (uVar5 == 1) {
			result.y = (multiplied_mat[uVar6 + 4] + multiplied_mat[uVar6 * 4 + 1]) * fVar9;
		}
		else {
			if (uVar5 == 2) {
				result.z = (multiplied_mat[uVar6 + 8] + multiplied_mat[uVar6 * 4 + 2]) * fVar9;
			}
		}
	}
	result.t = (multiplied_mat[uVar4 + uVar5 * 4] - multiplied_mat[uVar5 + uVar4 * 4]) * fVar9;
	
	return result;
}



vector4<float> top_fun(vector3<float> fs, unsigned int variant) {
	vector3<matrix4> mats = produce_four_matrixes(fs, variant);
	mats.x = mats.x.multiply(mats.y);
	/*1.00 -0.04 0.00 0.00
	0.04 0.98 -0.21 0.00
	0.01 0.21 0.98 0.00
	0.00 0.00 0.00 1.00
	*/
	mats.x = mats.x.multiply(mats.z);
	/*1.00 -0.04 0.01 0.00
	0.04 0.98 -0.21 0.00
	0.00 0.21 0.98 0.00
	0.00 0.00 0.00 1.00
	*/
	vector4<float> res = FUN_006dacf0(mats.x);
	return res;
}


Scene::Scene(std::string name, IT3File it3_p, IT3File it3_m, MTBFile mtb) {
	this->name = name;
	it3_p.add_kan7_from_m_file(it3_m);

	
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
				//std::cout << "CHILDREN!! " << current_info->text_id1 << std::endl;
				nd.parent = current_chid->parent;
				for (auto name : current_chid->children) {
					

						//std::cout << " " << name << std::endl;

						nd.children.push_back(name);
					
					
				}
					
			}

			simple_nodes[current_info->text_id1] = nd;
			
		}
		else {
			
			size_t nb_mesh_in_vpax = current_vpax->meshes_d.size();
			size_t count_materials = 0;
			std::map<unsigned int, material> mat_map;
			size_t count_mesh = 0;
			for (auto mesh_ : current_vpax->meshes_d) {
				//std::string new_name = current_info->text_id1 + "_" + std::to_string(count_mesh);
				mesh m(current_info->text_id1, current_info->transform);
				count_mesh++;
				material_data current_mat_data = current_mat6->mats[mesh_.material_id];

				for (unsigned int idx_v = 0; idx_v < mesh_.vertices.size(); idx_v++)
				{
					vector3<float> vertices;
					vector2<float> uv;
					vector2<float> uv_n;
					vertices.x = mesh_.vertices[idx_v].position.x;
					vertices.y = mesh_.vertices[idx_v].position.y;
					vertices.z = mesh_.vertices[idx_v].position.z;

					
					uv.x = (mesh_.vertices[idx_v].uv.x);
					uv.y = 1 - (mesh_.vertices[idx_v].uv.y);
					
					uv_n.x = (mesh_.vertices[idx_v].uv3.x);
					uv_n.y = 1 - (mesh_.vertices[idx_v].uv3.y);
					if (uv.x > 1)
						uv.x = (uv.x - floor(uv.x));
					else if (uv.x < 0) {
						if (current_mat_data.textures[0].XWrap == 0)
							uv.x = 1 - abs(uv.x);
						else if (current_mat_data.textures[0].XWrap == 2) {
							uv.x = abs(uv.x);
						}
					}
						

					if (uv.y > 1) 
						uv.y = 1 - (uv.y - floor(uv.y));
					else if (uv.y < 0)  
						uv.y = abs(uv.y);

					if (uv_n.x > 1) uv_n.x = (uv_n.x - floor(uv_n.x));
					else if (uv_n.x < 0) {
						if (current_mat_data.textures[0].XWrap == 0)
							uv_n.x = 1 - abs(uv_n.x);
						else if (current_mat_data.textures[0].XWrap == 2) {
							uv_n.x = abs(uv_n.x);
						}
					}

					if (uv_n.y > 1) uv_n.y = 1 - (uv_n.y - floor(uv_n.y));
					else if (uv_n.y < 0)  
						uv_n.y = abs(uv_n.y);


					m.vertices.push_back(vertices);
					m.uv.push_back(uv);
					m.uv_n.push_back(uv_n);

					
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
					
					bones[b.second.name].push_back(&m.bones[b.second.name]);
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
					m.parent = current_chid->parent;
					for (auto name : current_chid->children) {
						
						m.children.push_back(name);
					}
						
				}
				
				meshes[current_info->text_id1].push_back(m);
			}
			start_count_materials += count_materials;

		}
	}

	

	//Creating animations
	size_t nb_animations = 1; //mtb.data.size();
	if (mtb.data.size() == 0)
		nb_animations = 0;
	for (unsigned int id_ani = 0x00; id_ani < (0x00+nb_animations); id_ani++) {//mtb.data.size()
		if (id_ani > mtb.data.size()) break;

		motion_data m_d = mtb.data[id_ani];
		if (m_d.start <= m_d.end) break;
		animation ani(m_d.name, mtb.data[id_ani].duration, 24);
		
		
		unsigned int bone_chunk_id = 0;
		for (auto it_nd : it3_p.chunks) {
			chunk current_chunk = it_nd.second;
			matrix4 transform = current_chunk.info->transform;
			aiMatrix4x4 transform_node = aiMatrix4x4(transform.a.x, transform.a.y, transform.a.z, transform.a.t,
				transform.b.x, transform.b.y, transform.b.z, transform.b.t,
				transform.c.x, transform.c.y, transform.c.z, transform.c.t,
				transform.d.x, transform.d.y, transform.d.z, transform.d.t);
			transform_node.Transpose();
			aiVector3D posi, scal;
			aiQuaternion qua;
			transform_node.Decompose(scal, qua, posi);

			if (current_chunk.kan7) {
				
				//Translation
				auto first_t = std::find_if(current_chunk.kan7->kans[0].begin(), current_chunk.kan7->kans[0].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.start; });
				auto last_t = std::find_if(current_chunk.kan7->kans[0].begin(), current_chunk.kan7->kans[0].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.end; });

				auto first_r = std::find_if(current_chunk.kan7->kans[1].begin(), current_chunk.kan7->kans[1].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.start; });
				auto last_r = std::find_if(current_chunk.kan7->kans[1].begin(), current_chunk.kan7->kans[1].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.end; });

				auto first_s = std::find_if(current_chunk.kan7->kans[2].begin(), current_chunk.kan7->kans[2].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.start; });
				auto last_s = std::find_if(current_chunk.kan7->kans[2].begin(), current_chunk.kan7->kans[2].end(),
					[m_d](key_animation kan) { return kan.tick >= m_d.end; });

				std::vector<key_animation> keys_t(first_t, last_t);
				std::vector<key_animation> keys_r(first_r, last_r);
				std::vector<key_animation> keys_s(first_s, last_s);

				
				/* 0.10 0.00 0.02 0.99 */
				

				
				//std::cout << std::hex << "NAME: " << current_chunk.info->text_id1 << std::endl;
				
				for (key_animation &key_r : keys_r) {

					//std::cout << std::hex << "VARIANT: " << " " << key_r.tick << " " << key_r.type << " " << key_r.unit << " " << key_r.something << ", " << key_r.data.x << ", " << key_r.data.y << ", " << key_r.data.z << ", " << key_r.data.t << std::endl;

					
					if (key_r.something == 8) {
						switch (key_r.unit) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 6: {
							vector3<float> vec;

							vec.x = key_r.data.x * 2 *AI_MATH_PI/360 ;
							vec.y = key_r.data.y * 2 * AI_MATH_PI / 360;
							vec.z = key_r.data.z * 2 * AI_MATH_PI / 360 + AI_MATH_PI; //à voir selon ce qu'a dit julian
							
							aiVector3D test_p;
							aiVector3D test_s;
							aiQuaternion test_r;

							aiMatrix4x4 X, Y, Z;

							aiMatrix4x4::RotationX(vec.x, X);
							aiMatrix4x4::RotationY(vec.y, Y);
							aiMatrix4x4::RotationZ(vec.z, Z);



							//aiMatrix4x4 productZXY = Z * X * Y; NOPE
							//aiMatrix4x4 productXYZ = X * Y * Z;
							//aiMatrix4x4 productXZY = X * Z * Y;  
							aiMatrix4x4 productZYX = Z * Y * X; 
							//aiMatrix4x4 productYXZ = Y * X * Z;
							/*aiMatrix4x4 productYZX = Y * Z * X;*/
							aiMatrix4x4 product = productZYX;

							aiMatrix3x3 rot_mat = aiMatrix3x3(product.a1, product.a2, product.a3,
								product.b1, product.b2, product.b3,
								product.c1, product.c2, product.c3);


							//aiQuaternion res = aiQuaternion(vec.x, vec.y, vec.z);
							aiQuaternion resx = aiQuaternion(rot_mat);
							vector4<float> res = top_fun(vec, key_r.unit);

							//if (current_chunk.info->text_id1.compare("Bone_head") == 0) {

							//}


							//
							
							

							/*std::cout << std::hex << "QUAT: " << res.t << " " << res.x << ", " << res.y << ", " << res.z << std::endl;
							std::cout << std::hex << "Bind: " << qua.w << " " << qua.x << ", " << qua.y << ", " << qua.z << std::endl;
							*/
							//w is the fourth component here xyzw
							key_r.data.x = res.x;
							key_r.data.y = res.y;
							key_r.data.z = res.z;
							key_r.data.t = res.t;

							//std::cout << std::hex << "QUAT: " <<  key_r.data.x << " " << key_r.data.y << " " << key_r.data.z << " " << key_r.data.t << std::endl;

							break;
						}

						case 8:
							throw std::exception("what is that");
							break;
						default: {
							aiQuaternion quat_rad = aiQuaternion(key_r.data.x, key_r.data.y, key_r.data.z);

							key_r.data.x = quat_rad.x;
							key_r.data.y = quat_rad.y;
							key_r.data.z = quat_rad.z;
							key_r.data.t = quat_rad.w;
							break;
						}

						}
					}
					
					else if(key_r.something == 4) {

						float w = key_r.data.t;
						float x = key_r.data.x;
						float y = key_r.data.y;
						float z = key_r.data.z;

						key_r.data.x = x;
						key_r.data.y = y;

						key_r.data.z = z;
						key_r.data.t = w;


					}
				}
					
				
				std::vector<key_animation> all_keys;
				all_keys.insert(all_keys.begin(), keys_t.begin(), keys_t.end());
				all_keys.insert(all_keys.begin(), keys_r.begin(), keys_r.end());
				all_keys.insert(all_keys.begin(), keys_s.begin(), keys_s.end());
				std::sort(all_keys.begin(), all_keys.end());

				unsigned int idx_k = 0;
				int previous_idx_p = -1, previous_idx_r = -1, previous_idx_s = -1;
				int current_idx_p = -1, current_idx_r = -1, current_idx_s = -1;
				for (idx_k = 0; idx_k < all_keys.size(); idx_k++) {

					std::vector<key_animation> keys_current_tick;
					unsigned int current_tick = all_keys[idx_k].tick;

					if (all_keys[idx_k].type == 1)
						current_idx_p = idx_k;
					else if (all_keys[idx_k].type == 2)
						current_idx_r = idx_k;
					else if (all_keys[idx_k].type == 3)
						current_idx_s = idx_k;

					while (idx_k < all_keys.size()) {

						
						if (all_keys[idx_k].tick != current_tick){
							idx_k--;
							break; 
						}
						keys_current_tick.push_back(all_keys[idx_k]);
						if (all_keys[idx_k].type == 1)
							current_idx_p = idx_k;
						else if (all_keys[idx_k].type == 2)
							current_idx_r = idx_k;
						else if (all_keys[idx_k].type == 3)
							current_idx_s = idx_k;
						idx_k++;
					}
					

					vector3<float> position_keys;
					vector4<float> rotation_keys;
					vector3<float> scaling_keys;

					bool emptyp = true;
					bool emptyr = true;
					bool emptys = true;



					for (auto k : keys_current_tick) {
						if (k.type == 1) {
							emptyp = false;
							position_keys.x = k.data.x;
							position_keys.y = k.data.y;
							position_keys.z = k.data.z;
						}
						else if (k.type == 2) {
							emptyr = false;
							rotation_keys.x = k.data.x;
							rotation_keys.y = k.data.y;
							rotation_keys.z = k.data.z;
							rotation_keys.t = k.data.t;
						}
						else if (k.type == 3) {
							emptys = false;
							scaling_keys.x = k.data.x;
							scaling_keys.y = k.data.y;
							scaling_keys.z = k.data.z;
						}
					
					}


					



					//If the data is missing for a component, we will interpolate it. 
					int next_idx_p = -1, next_idx_r = -1, next_idx_s = -1;
					if (emptyp || emptyr || emptys) {
						for (unsigned int idx_k_ = idx_k; idx_k_ < all_keys.size(); idx_k_++) {
							key_animation k = all_keys[idx_k_];
							if (k.type == 1) {
								if (next_idx_p == -1)
									next_idx_p = idx_k_;
							}
							else if (k.type == 2) {
								if (next_idx_r == -1)
									next_idx_r = idx_k_;
							}
							else if (k.type == 3) {
								if (next_idx_s == -1)
									next_idx_s = idx_k_;
							}
						}
					
					}
					if (emptyp) {
						if (previous_idx_p == -1) {
							if (next_idx_p == -1) {
								
								//Not sure if it can happen, but if it does, instead of doing this I'll need to get the transform of the node
								position_keys.x = posi.x;
								position_keys.y = posi.y;
								position_keys.z = posi.z;
							}
							else {
								position_keys.x = all_keys[next_idx_p].data.x;
								position_keys.y = all_keys[next_idx_p].data.y;
								position_keys.z = all_keys[next_idx_p].data.z;
							}
						
						}
						else {
							if (next_idx_p == -1) {
								position_keys.x = all_keys[previous_idx_p].data.x;
								position_keys.y = all_keys[previous_idx_p].data.y;
								position_keys.z = all_keys[previous_idx_p].data.z;
							}
							else {
								//both are present, we interpolate
								key_animation before = all_keys[previous_idx_p];
								key_animation after = all_keys[next_idx_p];
								float t = ((float)(current_tick - before.tick))/(after.tick - before.tick);

								position_keys.x = (1- t) * before.data.x + t * after.data.x;
								position_keys.y = (1 - t) * before.data.y + t * after.data.y;
								position_keys.z = (1 - t) * before.data.z + t * after.data.z;
							
							}
						
						}

					}

					if (emptyr) {
						if (previous_idx_r == -1) {
							if (next_idx_r == -1) {
								//throw std::exception("ALLO!");
								//Not sure if it can happen, but if it does, instead of doing this I'll need to get the transform of the node
								rotation_keys.x = qua.x;
								rotation_keys.y = qua.y;
								rotation_keys.z = qua.z;
								rotation_keys.t = qua.w;
							}
							else {
								rotation_keys.x = all_keys[next_idx_r].data.x;
								rotation_keys.y = all_keys[next_idx_r].data.y;
								rotation_keys.z = all_keys[next_idx_r].data.z;
								rotation_keys.t = all_keys[next_idx_r].data.t;
							}

						}
						else {
							if (next_idx_r == -1) {
								rotation_keys.x = all_keys[previous_idx_r].data.x;
								rotation_keys.y = all_keys[previous_idx_r].data.y;
								rotation_keys.z = all_keys[previous_idx_r].data.z;
								rotation_keys.t = all_keys[previous_idx_r].data.t;
							}
							else {
								//both are present, we interpolate
								key_animation before = all_keys[previous_idx_r];
								key_animation after = all_keys[next_idx_r];
								float t = ((float)(current_tick - before.tick)) / (after.tick - before.tick);

								rotation_keys.x = (1 - t) * before.data.x + t * after.data.x;
								rotation_keys.y = (1 - t) * before.data.y + t * after.data.y;
								rotation_keys.z = (1 - t) * before.data.z + t * after.data.z;
								rotation_keys.t = (1 - t) * before.data.t + t * after.data.t;

							}

						}

					}

					if (emptys) {
						if (previous_idx_s == -1) {
							if (next_idx_s == -1) {
								//throw std::exception("ALLO!");
								//Not sure if it can happen, but if it does, instead of doing this I'll need to get the transform of the node
								scaling_keys.x = scal.x;
								scaling_keys.y = scal.y;
								scaling_keys.z = scal.z;
							}
							else {
								scaling_keys.x = all_keys[next_idx_s].data.x;
								scaling_keys.y = all_keys[next_idx_s].data.y;
								scaling_keys.z = all_keys[next_idx_s].data.z;
							}

						}
						else {
							if (next_idx_s == -1) {
								scaling_keys.x = all_keys[previous_idx_s].data.x;
								scaling_keys.y = all_keys[previous_idx_s].data.y;
								scaling_keys.z = all_keys[previous_idx_s].data.z;
							}
							else {
								//both are present, we interpolate
								key_animation before = all_keys[previous_idx_s];
								key_animation after = all_keys[next_idx_s];
								float t = ((float)(current_tick - before.tick)) / (after.tick - before.tick);
								
								scaling_keys.x = (1 - t) * before.data.x + t * after.data.x;
								scaling_keys.y = (1 - t) * before.data.y + t * after.data.y;
								scaling_keys.z = (1 - t) * before.data.z + t * after.data.z;

							}

						}

					}

					
					key_frame kf(current_tick, position_keys, rotation_keys, scaling_keys);
					ani.bones_data[it_nd.second.info->text_id1].push_back(kf);

					previous_idx_p = current_idx_p;
					previous_idx_r = current_idx_r;
					previous_idx_s = current_idx_s;
				}
				
				
				
				
			}

		}
		anis[m_d.name] = ani;
	}

}
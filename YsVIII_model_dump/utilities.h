#pragma once
#include <vector>
#include <iostream>
#include <string>


template<typename T>
struct vector4 {
	T x;
	T y;
	T z; 
	T t;
};
template<typename T>
struct vector3 {
	T x;
	T y;
	T z;
};

template<typename T>
struct vector2 {
	T x;
	T y;
};
struct matrix4 {
	vector4<float> a, b, c, d;

	matrix4 multiply(matrix4 m) {
		float ax = this->a.x * m.a.x + this->a.y * m.b.x + this->a.z * m.c.x + this->a.t * m.d.x;
		float ay = this->a.x * m.a.y + this->a.y * m.b.y + this->a.z * m.c.y + this->a.t * m.d.y;
		float az = this->a.x * m.a.z + this->a.y * m.b.z + this->a.z * m.c.z + this->a.t * m.d.z;
		float at = this->a.x * m.a.t + this->a.y * m.b.t + this->a.z * m.c.t + this->a.t * m.d.t;

		float bx = this->b.x * m.a.x + this->b.y * m.b.x + this->b.z * m.c.x + this->b.t * m.d.x;
		float by = this->b.x * m.a.y + this->b.y * m.b.y + this->b.z * m.c.y + this->b.t * m.d.y;
		float bz = this->b.x * m.a.z + this->b.y * m.b.z + this->b.z * m.c.z + this->b.t * m.d.z;
		float bt = this->b.x * m.a.t + this->b.y * m.b.t + this->b.z * m.c.t + this->b.t * m.d.t;

		float cx = this->c.x * m.a.x + this->c.y * m.b.x + this->c.z * m.c.x + this->c.t * m.d.x;
		float cy = this->c.x * m.a.y + this->c.y * m.b.y + this->c.z * m.c.y + this->c.t * m.d.y;
		float cz = this->c.x * m.a.z + this->c.y * m.b.z + this->c.z * m.c.z + this->c.t * m.d.z;
		float ct = this->c.x * m.a.t + this->c.y * m.b.t + this->c.z * m.c.t + this->c.t * m.d.t;

		float dx = this->d.x * m.a.x + this->d.y * m.b.x + this->d.z * m.c.x + this->d.t * m.d.x;
		float dy = this->d.x * m.a.y + this->d.y * m.b.y + this->d.z * m.c.y + this->d.t * m.d.y;
		float dz = this->d.x * m.a.z + this->d.y * m.b.z + this->d.z * m.c.z + this->d.t * m.d.z;
		float dt = this->d.x * m.a.t + this->d.y * m.b.t + this->d.z * m.c.t + this->d.t * m.d.t;




		matrix4 result = { ax, ay, az, at,
			bx, by, bz, bt,
			cx, cy, cz, ct, 
			dx, dy, dz, dt
		};
		return result;
	}

	std::string to_string() {
		std::string result = "";
		result = result + std::to_string(this->a.x) + ", " +  std::to_string(this->a.y) + ", " + std::to_string(this->a.z)+ ", " + std::to_string(this->a.t)+ "\n";
		result = result + std::to_string(this->b.x ) + ", " + std::to_string(this->b.y) + ", " + std::to_string(this->b.z)+ ", " + std::to_string(this->b.t)+ "\n";
		result = result + std::to_string(this->c.x ) + ", " + std::to_string(this->c.y) + ", " + std::to_string(this->c.z)+ ", " + std::to_string(this->c.t)+ "\n";
		result = result + std::to_string(this->d.x ) + ", " + std::to_string(this->d.y) + ", " + std::to_string(this->d.z)+ ", " + std::to_string(this->d.t)+ "\n";
		result = result + "\n";
		return result;
	}
};
template<typename T>
T read_data(const std::vector<uint8_t> &file_content, unsigned int &addr) {
	T result;
	memcpy(&result, file_content.data() + addr, sizeof(T));
	addr += sizeof(T);
	return result;
}
std::string read_string(const std::vector<uint8_t> &file_content, unsigned int &addr);
std::string id_to_ascii(unsigned int identifier_uint);
std::vector<uint8_t> int_to_bytes(int x);
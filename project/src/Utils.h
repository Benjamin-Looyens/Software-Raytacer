#pragma once
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& curHit, bool ignoreHitRecord = false)
		{
			const float a = Vector3::Dot(ray.direction, ray.direction);
			const float b = 2 * Vector3::Dot(ray.direction, (ray.origin - sphere.origin));
			
			const Vector3 sphereToRay = (ray.origin - sphere.origin);

			const float c = Vector3::Dot(sphereToRay, sphereToRay) - (sphere.radius * sphere.radius);
			const float discriminant = (b * b) - 4 * (a * c);


			if (discriminant > 0) {
				// TODO : Double check the intersection point & t
				const float sqrtDiscriminant = sqrtf(discriminant);

				const float t0 = (-b - sqrtDiscriminant) / (2 * a);

				// TODO : add HitRecord normal
				if (t0 > ray.min) {
					if (t0 < ray.max) {
						curHit.t = t0;
						curHit.didHit = true;
						curHit.materialIndex = sphere.materialIndex;
						curHit.origin = ray.origin + (t0 * ray.direction);
						return true;
					}
				}
				else {
					const float t1 = (-b + sqrtDiscriminant) / (2 * a);
					if (t1 > ray.min && t1 < ray.max) {
						curHit.t = t1;
						curHit.didHit = true;
						curHit.materialIndex = sphere.materialIndex;
						curHit.origin = ray.origin + (t1 * ray.direction);
						return true;
					}
				}
			}

			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& curHit, bool ignoreHitRecord = false)
		{
			const Vector3 rayToPlane = plane.origin - ray.origin;
			const float t = Vector3::Dot(rayToPlane, plane.normal) / Vector3::Dot(ray.direction, plane.normal);

			if (t > ray.min && t < ray.max) {
				// TODO : add HitRecord normal
				curHit.t = t;
				curHit.didHit = true;
				curHit.materialIndex = plane.materialIndex;
				curHit.origin = ray.origin + (t * ray.direction);
			}

			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			throw std::runtime_error("Not Implemented Yet");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			throw std::runtime_error("Not Implemented Yet");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			//todo W3
			throw std::runtime_error("Not Implemented Yet");
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			throw std::runtime_error("Not Implemented Yet");
			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}
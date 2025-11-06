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

				// I kinda hate how this reads but yea
				bool withinBoundaries{ false };
				if (t0 > ray.min) {
					if (t0 < ray.max) {
						if (!ignoreHitRecord) {
							curHit.t = t0;
							curHit.origin = ray.origin + (t0 * ray.direction);
						}
						withinBoundaries = true;
					}
				}
				else {
					const float t1 = (-b + sqrtDiscriminant) / (2 * a);
					if (t1 > ray.min && t1 < ray.max) {
						if (!ignoreHitRecord) {
							curHit.t = t1;
							curHit.origin = ray.origin + (t1 * ray.direction);
						}
						withinBoundaries = true;
					}
				}

				if (withinBoundaries) {
					if (!ignoreHitRecord) {
						curHit.didHit = true;
						curHit.materialIndex = sphere.materialIndex;
						curHit.normal = (curHit.origin - sphere.origin).Normalized();
					}
					return true;
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
				if (!ignoreHitRecord) {
					curHit.t = t;
					curHit.didHit = true;
					curHit.materialIndex = plane.materialIndex;
					curHit.origin = ray.origin + (t * ray.direction);
					curHit.normal = plane.normal;
				}
				return true;
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
			const Vector3 a = triangle.v1 - triangle.v0;
			const Vector3 b = triangle.v2 - triangle.v0;
			
			float backFront = Vector3::Dot(triangle.normal, ray.direction);
			if (ignoreHitRecord) backFront = -backFront; // Potentially some way to collapse into the backFront initialization, so that I can make it const
			
			if (AreEqual(backFront, 0.0f)) return false;
			else if (backFront > 0.0f) {
				// Hit the back face
				if (triangle.cullMode == TriangleCullMode::BackFaceCulling) return false;
			}
			else {
				// Hit the front face
				if (triangle.cullMode == TriangleCullMode::FrontFaceCulling) return false;
			}

			const Vector3 rayToV0 = triangle.v0 - ray.origin;
			const float t = Vector3::Dot(rayToV0, triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);

			if (t < ray.min || t > ray.max) return false;

			const Vector3 p = ray.origin + (ray.direction * t); // Intersection point
			
			const Vector3 cV0 = p - triangle.v0;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(a, cV0)) < 0) return false;

			const Vector3 eV1 = triangle.v2 - triangle.v1;
			const Vector3 cV1 = p - triangle.v1;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(eV1, cV1)) < 0) return false;

			const Vector3 eV2 = triangle.v0 - triangle.v2;
			const Vector3 cV2 = p - triangle.v2;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(eV2, cV2)) < 0) return false;

			if (!ignoreHitRecord) {
				hitRecord.t = t;
				hitRecord.normal = triangle.normal;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.didHit = true;
				hitRecord.origin = p;
			}

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region AABB Slabtest
		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMin.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMax.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMin.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMax.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMin.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMax.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}
#pragma endregion AABB Slabtest
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest_TriangleMesh(mesh, ray)) return false;

			const size_t maxTri{ mesh.indices.size() / 3 };
			for (int triNum{ }; triNum < maxTri; triNum++) {
				Triangle tri;
				tri.v0 = mesh.transformedPositions[mesh.indices[triNum * 3 + 0]];
				tri.v1 = mesh.transformedPositions[mesh.indices[triNum * 3 + 1]];
				tri.v2 = mesh.transformedPositions[mesh.indices[triNum * 3 + 2]];
				tri.normal = mesh.transformedNormals[triNum];
				tri.cullMode = mesh.cullMode;
				tri.materialIndex = mesh.materialIndex;

				HitRecord temp = hitRecord;
				if (HitTest_Triangle(tri, ray, temp, ignoreHitRecord)) {
					if (ignoreHitRecord) return true;
					else if (temp.t < hitRecord.t) hitRecord = temp;
				}
			}

			return hitRecord.didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
	}
#pragma endregion

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
			ColorRGB retColor;
			
			switch (light.type) {
			case LightType::Directional:
				retColor = light.color * light.intensity;
				break;
			case LightType::Point:
				const Vector3 lightToTarget = light.origin - target;
				retColor = light.color * (light.intensity / Vector3::Dot(lightToTarget, lightToTarget));
				break;
			}

			return retColor;
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
#pragma once
#include <stdexcept>
#include <vector>
#include "Math.h"
#include <algorithm>

namespace dae
{
#pragma region GEOMETRY
	struct Sphere final
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane final
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle final
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }, normal{ _normal.Normalized() } {}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh final
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), cullMode(_cullMode)
		{
			//Calculate Normals
			CalculateNormals();

			//Update Transforms
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{ TriangleCullMode::BackFaceCulling };

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.push_back(triangle.v0);
			positions.push_back(triangle.v1);
			positions.push_back(triangle.v2);

			indices.push_back(startIndex);
			indices.push_back(++startIndex);
			indices.push_back(++startIndex);

			normals.push_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if (!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			const size_t maxTri{ indices.size() / 3 };
			normals.resize(maxTri);

			for (int triNum{ }; triNum < maxTri; triNum++) {
				const Vector3 v0 = positions[indices[triNum * 3 + 0]];
				const Vector3 v1 = positions[indices[triNum * 3 + 1]];
				const Vector3 v2 = positions[indices[triNum * 3 + 2]];

				const Vector3 a = v1 - v0;
				const Vector3 b = v2 - v0;
				const Vector3 normal = Vector3::Cross(a, b).Normalized();

				normals[triNum] = normal;
			}
		}

		void UpdateTransforms()
		{
			// Slides say:
			// Make sure you’re code is ‘optimized’, make use of vector::reserve & vector::emplace_back

			// Calculate Final Transform 
			const Matrix transform = scaleTransform * rotationTransform * translationTransform;
			
			// Transform Positions (positions > transformedPositions)
			transformedPositions.resize(positions.size());
			std::transform(begin(positions), end(positions), begin(transformedPositions),
				[transform](const Vector3& vec) { return transform.TransformPoint(vec); }
			);

			// TODO : Check lambda [transform], whether that copies it?
		
			// Transform Normals (normals > transformedNormals)
			const Matrix inv = Matrix::Inverse(transform);
			const Matrix invTransp = Matrix::Transpose(inv);

			transformedNormals.resize(normals.size());
			std::transform(begin(normals), end(normals), begin(transformedNormals),
				[invTransp](const Vector3& n) { return invTransp.TransformVector(n).Normalized(); }
			);
		}
	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light final
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray final
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord final
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}
#pragma once
#include "Math.h"
#include <iostream>

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{
			// P / pi
			// P = diffuse reflectance = cd

			return (kd / PI) * cd;
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			return (kd / PI) * cd;
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			const Vector3 reflect = -l - (2.0f * (Vector3::Dot(-l, n) * n));
			const float cosAngle = std::max(Vector3::Dot(reflect, v), 0.0f);
			const float specular = ks * std::pow(cosAngle, exp); // HACK : std::powf instead?

			return { specular, specular, specular };
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			// f0 + (1 - f0)(1 -(h dot v))^5

			const float hvCalc = std::pow(1.0f - Vector3::Dot(h, v), 5);
			constexpr ColorRGB oneColor = { 1.0f, 1.0f, 1.0f };

			return f0 + ((oneColor - f0) * hvCalc);
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			// roughness^2 / (PI ((n dot h)^2 (roughness^2 - 1) + 1)

			// According to slides, a = roughness squared, but then shows a squared, so shouild I square it twice?
			const float a = roughness * roughness;
			const float aSqr = a * a;
			const float nhSqr = std::pow(Vector3::Dot(n, h), 2);

			return aSqr / (PI * std::pow(nhSqr * (aSqr - 1.0f) + 1.0f, 2));
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			// (n dot v) / (n dot v) (1 - k) + k
			// k = (a + 1)^2 / 8

			const float a = roughness * roughness;
			const float k = std::pow(a + 1, 2) / 8.0f;
			const float nDotV = Vector3::Dot(n, v);

			return nDotV / (nDotV * (1.0f - k) + k);
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			return GeometryFunction_SchlickGGX(n, v, roughness) * GeometryFunction_SchlickGGX(n, l, roughness); // I think ??
		}

	}
}
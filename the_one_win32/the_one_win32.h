#include "gpk_the_one.h"

#ifndef TEST_GPK_MESH_H_324234234
#define TEST_GPK_MESH_H_324234234

namespace gpk
{
	GDEFINE_ENUM_TYPE(SHAPE_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Custom		, 0);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Rectangle	, 1);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Circle		, 2);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Ring			, 3);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cube			, 4);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Sphere		, 5);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Cylinder		, 6);
	GDEFINE_ENUM_VALUE(SHAPE_TYPE, Torus		, 7);

} // namespace


#include "gpk_framework.h"
#include "gpk_gui.h"

#ifndef APPLICATION_H_098273498237423
#define APPLICATION_H_098273498237423

struct SApplication {
	::gpk::SFramework								Framework				;

	::gpk::SGUI										GUI						= {};
	int32_t											IdViewport				= -1;

	int32_t											EntityLightDirectional	= -1;
	int32_t											EntityLightPoint		= -1;
	int32_t											EntityLightSpot			= -1;
	int32_t											EntityBox				= -1;
	int32_t											EntitySphere			= -1;

	::gpk::SImage<::gpk::SColorBGR>					Image;
	::gpk::SImage<::gpk::SColorBGRA>				ImageAlpha;

	::the1::STheOne									TheOne					= {};

													SApplication			(::gpk::SRuntimeValues& runtimeValues)			noexcept	: Framework(runtimeValues) {}
};

#endif // APPLICATION_H_098273498237423

#endif // GPK_MESH_H_324234234

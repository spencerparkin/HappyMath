#pragma once

#if defined HAPPY_MATH_EXPORT
#	define HAPPY_MATH_API		__declspec(dllexport)
#elif defined HAPPY_MATH_IMPORT
#	define HAPPY_MATH_API		__declspec(dllimport)
#else
#	define HAPPY_MATH_API
#endif

#define HM_MIN(a, b)			((a) < (b) ? (a) : (b))
#define HM_MAX(a, b)			((a) > (b) ? (a) : (b))
#define HM_CLAMP(x, a, b)		HM_MAX(HM_MIN(x, b), a)
#define HM_SQUARED(x)			((x) * (x))
#define HM_SIGN(x)				((x) < 0.0 ? -1.0 : 1.0)
#define HM_DEGS_TO_RADS(x)		((x) * (M_PI / 180.0))
#define HM_RADS_TO_DEGS(x)		((x) * (180.0 / M_PI))
#define HM_IS_POW_TWO(x)		((x & (x - 1)) == 0)
#define HM_ALIGNED(x, y)		(((x) + ((y) - 1)) & ~((y) - 1))
#define HM_PI					M_PI
#define HM_PHI					1.618033988749
#define HM_SMALL_EPS			1e-9
#define HM_MEDIUM_EPS			1e-5
#define HM_FAT_EPS				1e-2
#define HM_OBESE_EPS			0.1

#if defined _DEBUG
#	define HM_ASSERT(x)
#	define HM_ASSERT_FATAL(x)
#else
#	define HM_ASSERT(x)
#	define HM_ASSERT_FATAL(x)
#endif

#define HM_AXIS_FLAG_X			0x00000001
#define HM_AXIS_FLAG_Y			0x00000002
#define HM_AXIS_FLAG_Z			0x00000004
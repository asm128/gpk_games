#include "gpk_engine.h"
#include "gpk_image.h"
#include "gpk_array_static.h"

#ifndef GPK_POOL_GAME_H_098273498237423
#define GPK_POOL_GAME_H_098273498237423

namespace the1
{
	GDEFINE_ENUM_TYPE(POOL_GAME_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 8Ball, 0);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 9Ball, 1);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, 10Ball, 2);
	GDEFINE_ENUM_VALUE(POOL_GAME_MODE, Test2Balls, 3);

	static constexpr	uint8_t					MAX_BALLS						= 16;

#pragma pack(push, 1)
	typedef ::gpk::SContactResult	SContactResultBall;
	typedef ::gpk::SContact			SContactBall;

	struct SContactResultCushion {
		::gpk::SCoord3<float>						ContactPosition					= {};
		::gpk::SCoord3<float>						InitialVelocityA				= {};
		::gpk::SCoord3<float>						FinalVelocityA					= {};
		::gpk::ALIGN								CushionSides					= ::gpk::ALIGN_CENTER;
	};

	struct SContactCushion {
		uint32_t									BallA							= 0;
		::gpk::SCoord3<float>						Distance						= {};
		::the1::SContactResultCushion				Result							= {};
	};

	struct SPoolBall {
		uint32_t									Entity							= (uint32_t)-1;
		//float										BallRadius						= .5f;
	};

	struct SPoolPocket {
		uint32_t									Entity							= (uint32_t)-1;
	};


	static constexpr float						FOOT_SCALE						= 1.0f / 3.281f;
	static constexpr float						INCH_SCALE						= 2.54f;

	struct SPoolTableDimensions {
		::gpk::SCoord2<float>						Slate							= {8 * ::the1::FOOT_SCALE, 4 * ::the1::FOOT_SCALE};
		::gpk::SCoord2<float>						PlayingSurface					= {92 * ::the1::INCH_SCALE, 46 * ::the1::INCH_SCALE};
		float										Height							= .0265f;
	};

	static constexpr ::the1::SPoolTableDimensions	COMMON_TABLE_SIZES	[]	 =
	{	{ {::the1::FOOT_SCALE * 8.0f, ::the1::FOOT_SCALE * 4.0f}	// 8-foot
		, {::the1::INCH_SCALE * 92.f, ::the1::INCH_SCALE * 46.f}
		, .0265f
		},	
		{ {::the1::FOOT_SCALE *  9.0f, ::the1::FOOT_SCALE * 4.5f}	// 9-foot
		, {::the1::INCH_SCALE * 100.f, ::the1::INCH_SCALE * 50.f}
		, .0265f
		},	
	};

	struct SPoolTable {
		uint32_t									Entity							= (uint32_t)-1;
		SPoolTableDimensions						Dimensions						= COMMON_TABLE_SIZES[0];
		float										CushionDepth					= .234f;
		SPoolPocket									Pockets	[6]						= {};
		float										PocketRadius					= .057f;
	};

	struct SPoolStick {
		uint32_t									Entity							= (uint32_t)-1;
		float										Radius							= .25f;
		::gpk::SCoord2<float>						Shift							= {};
		::gpk::SCoord2<float>						Tilt							= {};
		float										Angle							= 0;
		float										Velocity						= 0;
	};

	struct SPoolPlayer {
		uint32_t									Team							= 0;
		::the1::SPoolStick							Stick							= {};
	};

	struct SPoolPhysicsState {
		float										DampingClothDisplacement		= .7f;
		float										DampingClothRotation			= .01f;
		float										DampingCollision				= .975f;
		float										DampingCushion					= .95f;
		float										DampingPocket					= .95f;
		float										DampingGround					= .5f;
		float										Gravity							= 9.8f;	// m/s
		float										BallWeight						= 165;	// grams
	};

	struct SPoolStartState {
		POOL_GAME_MODE								Mode							= ::the1::POOL_GAME_MODE_8Ball;
		uint8_t										BallCount						= ::the1::MAX_BALLS;
		uint8_t										PlayerCount						= ::the1::MAX_BALLS;
		uint64_t									Seed							= (uint64_t)::gpk::timeCurrentInUs();
		float										BallRadius						= .057f * .5f;	// meters
		uint8_t										TeamActive						= 0;
		uint8_t										TeamStripped					= 0;
		uint64_t									TimeInSeconds					= 0;
		::the1::SPoolPhysicsState					Physics							= {};

		::the1::SPoolTable							Table							= {};

		::gpk::array_static<::the1::SPoolPlayer		, ::the1::MAX_BALLS> Player			= {};
		::gpk::array_static<::the1::SPoolBall		, ::the1::MAX_BALLS> Ball			= {};
		::gpk::array_static<uint32_t				, ::the1::MAX_BALLS> BallOrder		= {};
		::gpk::array_static<::gpk::SCoord3<float>	, ::the1::MAX_BALLS> BallPositions	= {};
		::gpk::array_static<::gpk::SColorFloat		, ::the1::MAX_BALLS> BallColors		= 
			{ ::gpk::WHITE
			, ::gpk::YELLOW
			, ::gpk::BLUE
			, ::gpk::RED
			, ::gpk::PURPLE 
			, ::gpk::ORANGE
			, ::gpk::GREEN
			, ::gpk::DARKRED * .5f
			, ::gpk::BLACK
			, ::gpk::YELLOW
			, ::gpk::BLUE
			, ::gpk::RED
			, ::gpk::PURPLE 
			, ::gpk::ORANGE
			, ::gpk::GREEN
			, ::gpk::DARKRED * .5f
			};
	};

	struct SPoolControllerStick {
		::gpk::SCoord2<float>						Shift							= {};
		::gpk::SCoord2<float>						Tilt							= {};
		float										Angle							= 0;
		float										Velocity						= 0;
		bool										Cancel							= false;
	};

	struct SPoolControllerCamera {
		::gpk::SCoord3<float>						Position						= {};
		::gpk::SCoord3<float>						Target							= {};
		bool										ToggleLockPosition				= false;
		bool										ToggleLockTarget				= false;
	};
#pragma pack(pop)

	struct SPoolGame {
		::the1::SPoolStartState						StateStart						= {};
		::the1::SPoolStartState						StateCurrent					= {};

		::the1::SPoolControllerStick				ControlStick					= {};
		::the1::SPoolControllerCamera				ControlCamera					= {};

		::gpk::array_pod<::gpk::SLine3<float>>		PositionDeltas	[MAX_BALLS]		= {};
		::gpk::array_pod<::the1::SContactBall>		LastFrameContactsBall			= {};
		::gpk::array_pod<::the1::SContactCushion>	LastFrameContactsCushion		= {};

		::gpk::SEngine								Engine							= {};

		::gpk::error_t								GetBallPosition					(uint32_t iBall, ::gpk::SCoord3<float> & ballPosition) {
			ballPosition = Engine.Integrator.Centers[Engine.ManagedEntities.Entities[StateCurrent.Ball[iBall].Entity].RigidBody].Position;
			return 0;
		}
	};

	::gpk::error_t								poolGameReset					(::the1::SPoolGame & pool, POOL_GAME_MODE mode);
	::gpk::error_t								poolGameSetup					(::the1::SPoolGame & pool, POOL_GAME_MODE mode = POOL_GAME_MODE_8Ball);
	::gpk::error_t								poolGameUpdate					(::the1::SPoolGame & pool, double secondsElapsed);
	::gpk::error_t								poolGameDraw					(::the1::SPoolGame & pool
		, ::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>	& backBuffer
		, const ::gpk::SCoord3<float>						& cameraPosition
		, const ::gpk::SCoord3<float>						& cameraTarget
		, const ::gpk::SCoord3<float>						& cameraUp
		, double											totalSeconds
		);
} // namespace

#endif // GPK_POOL_GAME_H_098273498237423

// Pool is played on a six pocket table. Modern pool tables generally range in size from 3.5 by 7 feet (1.07 by 2.13 m), to 4.5 by 9 feet (1.37 by 2.74 m).
// Under World Pool-Billiard Association (WPA) official equipment specifications, pool balls have a diameter of 2.25 inches (57.15 mm) and weigh from 5.5 to 6 oz (160 to 170 g) with a diameter of 2.25 inches (57 mm), 
// plus or minus 0.005 inches (0.13 mm). Modern coin-operated pool tables generally use one of three methods to distinguish and return the cue ball to the front of the table while the numbered balls 
// return to an inaccessible receptacle until paid for again: the cue ball is larger and heavier than the other balls, or denser and heavier, or has a magnetic core.
// Modern cue sticks are generally 58.5 inches (148.6 cm) long for pool while cues prior to 1980 were designed for straight pool and had an average length of 57.5 inches (146.1 cm). 
// By comparison, carom billiards cues are generally shorter with larger tips, and snooker cues longer with smaller tips.
//
// Source: https://en.wikipedia.org/wiki/Pool_(cue_sports)


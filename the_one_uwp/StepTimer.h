﻿#include <wrl.h>

#ifndef STEPTIMER_H_2983749823
#define STEPTIMER_H_2983749823

namespace DX
{
	// Helper class for animation and simulation timing.
	// Change the timer settings if you want something other than the default variable timestep mode.// e.g. for 60 FPS fixed timestep update logic, call:
	// Timer.SetFixedTimeStep(true);
	// Timer.SetTargetElapsedSeconds(1.0 / 60);
	class StepTimer {
	public:
		// Source timing data uses QPC units.
		LARGE_INTEGER				m_qpcFrequency					= {};
		LARGE_INTEGER				m_qpcLastTime					= {};
		uint64_t					m_qpcMaxDelta					= 0;

		// Derived timing data uses a canonical tick format.		
		uint64_t					m_elapsedTicks					= 0;
		uint64_t					m_totalTicks					= 0;
		uint64_t					m_leftOverTicks					= 0;

		// Members for tracking the framerate.
		uint32_t					m_frameCount					= 0;
		uint32_t					m_framesPerSecond				= 0;
		uint32_t					m_framesThisSecond				= 0;
		uint64_t					m_qpcSecondCounter				= 0;

		// Members for configuring fixed timestep m	ode.
		bool						m_isFixedTimeStep				= false;
		uint64_t					m_targetElapsedTicks			= TicksPerSecond / 60;

									StepTimer						()								{
			if (!QueryPerformanceFrequency	(&m_qpcFrequency))	throw ref new Platform::FailureException();
			if (!QueryPerformanceCounter	(&m_qpcLastTime))	throw ref new Platform::FailureException();
			m_qpcMaxDelta				= m_qpcFrequency.QuadPart / 10;	// Initialize max delta to 1/10 of a second.
		}

		uint64_t					GetElapsedTicks					() const						{ return m_elapsedTicks; }
		double						GetElapsedSeconds				() const						{ return TicksToSeconds(m_elapsedTicks); }

		uint64_t					GetTotalTicks					() const						{ return m_totalTicks; }
		double						GetTotalSeconds					() const						{ return TicksToSeconds(m_totalTicks); }
		
		uint32_t					GetFrameCount					() const						{ return m_frameCount; }	// Get total number of updates since start of the program.
		uint32_t					GetFramesPerSecond				() const						{ return m_framesPerSecond; }	// Get the current framerate.

		void						SetFixedTimeStep				(bool isFixedTimestep)			{ m_isFixedTimeStep = isFixedTimestep; }
		void						SetTargetElapsedTicks			(uint64_t targetElapsed)		{ m_targetElapsedTicks = targetElapsed; }
		void						SetTargetElapsedSeconds			(double targetElapsed)			{ m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

		// Integer format represents time using 10,000,000 ticks per second.
		static constexpr uint64_t	TicksPerSecond					= 10000000;

		static constexpr double		TicksToSeconds					(uint64_t ticks)				{ return static_cast<double>(ticks) / TicksPerSecond; }
		static constexpr uint64_t	SecondsToTicks					(double seconds)				{ return static_cast<uint64_t>(seconds * TicksPerSecond); }

		// After an intentional timing discontinuity (for instance a blocking IO operation) call this to avoid having the fixed timestep logic attempt a set of catch-up  Update calls.
		void						ResetElapsedTime				()								{
			if (!QueryPerformanceCounter(&m_qpcLastTime))
				throw ref new Platform::FailureException();

			m_qpcSecondCounter = m_leftOverTicks = m_framesPerSecond = m_framesThisSecond = 0;
		}

		// Update timer state, calling the specified Update function the appropriate number of times.
		template<typename TUpdate>
		void						Tick							(const TUpdate& update)	{
			// Query the current time.
			LARGE_INTEGER					currentTime						= {};
			if (!QueryPerformanceCounter(&currentTime))
				throw ref new Platform::FailureException();

			uint64_t						timeDelta						= currentTime.QuadPart - m_qpcLastTime.QuadPart;
			m_qpcLastTime				= currentTime;
			m_qpcSecondCounter			+= timeDelta;

			if (timeDelta > m_qpcMaxDelta)	// Clamp excessively large time deltas (e.g. after paused in the debugger).
				timeDelta					= m_qpcMaxDelta;

			// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
			timeDelta					*= TicksPerSecond;
			timeDelta					/= m_qpcFrequency.QuadPart;

			uint32_t						lastFrameCount					= m_frameCount;
			if (m_isFixedTimeStep) { // Fixed timestep update logic
				// If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp the clock to exactly match the target value. This prevents tiny and irrelevant errors
				// from accumulating over time. Without this clamping, a game that requested a 60 fps fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
				// accumulate enough tiny errors that it would drop a frame. It is better to just round small deviations down to zero to leave things running smoothly.
				if (abs(static_cast<int64_t>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
					timeDelta					= m_targetElapsedTicks;

				m_leftOverTicks				+= timeDelta;
				while (m_leftOverTicks >= m_targetElapsedTicks) {
					m_elapsedTicks				= m_targetElapsedTicks;
					m_totalTicks				+= m_targetElapsedTicks;
					m_leftOverTicks				-= m_targetElapsedTicks;
					++m_frameCount;
					update();
				}
			}
			else { // Variable timestep update logic.
				m_elapsedTicks				= timeDelta;
				m_totalTicks				+= timeDelta;
				m_leftOverTicks				= 0;
				++m_frameCount;
				update();
			}
			
			if (m_frameCount != lastFrameCount)	// Track the current framerate.
				++m_framesThisSecond;

			if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart)) {
				m_framesPerSecond			= m_framesThisSecond;
				m_framesThisSecond			= 0;
				m_qpcSecondCounter			%= m_qpcFrequency.QuadPart;
			}
		}
	};
}

#endif // STEPTIMER_H_2983749823
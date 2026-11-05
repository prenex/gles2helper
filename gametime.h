#ifndef _GAMETIME_H
#define _GAMETIME_H
/* *******************************************************************
 * Simple C++11 header-only library for handling timing for games.   *
 * - Provides the gametime class.                                    *
 * - You need to #define _DEFINE_GAMETIME somewhere in a cpp file    *
 * - before inclusion of this header! (common header-only practice)  *
 *                                                                   *
 * Created by: prenex                                                *
 * LICENCE: The UNLICENCE                                            *
 * ******************************************************************/

#include <chrono>    /* timing */

#ifdef _WIN32
	/** Winfos does not have a proper usleep so we wrote one which is probably efficient */
	int my_usleep(unsigned int usec) {
		HANDLE timer;
		LARGE_INTEGER ft;

		ft.QuadPart = -(10 * (__int64)usec);

		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
		return 0; // TODO: Error handling?
	}
#else
#include <unistd.h>
	/** Linux totally have usleep because properly POSIX */
	int my_usleep(unsigned int usec) {
		return usleep(usec);
	}
#endif

/** Simple, lightweight c++14 helper class for measuring game time for the update and draw loops */
class gametime {
private:
	static unsigned long long init_ms;
	unsigned long long last_ms;
	unsigned long long now_ms;
public:
	/** Construct a gametime object with the difference values */
	gametime(unsigned long long _last_ms, unsigned long long _now_ms) {
		last_ms = _last_ms;
		now_ms = _now_ms;
	}

	/** Gets milliseconds relative to the last frame. */
	const inline unsigned long long get_diff_ms() const {
		return now_ms - last_ms;
	}

	/** How many SECONDS (float) the last frame took? AKA the frame time. */
	const inline float delta() const {
		return (float)get_diff_ms() / 1000.0f;
	}

	/** How many frames per second if its as if the last frame? */
	const inline float fps() const {
		// printf("- %d %d\n", last_ms, now_ms);
		return delta() > 0 ? (1 / delta()) : 60; // Rem.: I know it might be 120 but its fine...
	}

	/** Gets milliseconds since the game has started. ONLY WORKS AFTER setStartMs is properly called at app start! */
	const inline unsigned long long get_ms() const {
		return now_ms - init_ms;
	}

	/** Saves the start time - needed for any later getMs() calls */
	static inline void set_init_ms(unsigned long long ms) {
		init_ms = ms;
	}

	/** Gets the milliseconds since 1970 when init was called */
	static inline unsigned long long get_init_ms() {
		return init_ms;
	}

	/** Initialize game timing */
	static inline void init() {
		/* Save start time */
		gametime::set_init_ms(std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch()).count());
	}

	/** When called in the main loop of an app, always return the proper gametime */
	static inline const gametime mainloop_get_current() {
		unsigned long long now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
		static unsigned long long previous_ms = -1;
		if(previous_ms == (unsigned long long) -1) {
			previous_ms = gametime::init_ms;
		}

		gametime t = gametime(previous_ms, now_ms);
		previous_ms = now_ms;
		return t;
	}
};

class fpscounter {
	unsigned long long framecunt = 0;
	unsigned long long last_cunt = 0;
	/** The ensure(..) call uses this */
	unsigned long long current_sleeptime_us = 0;
public:
	/** Returns 0 when there is no change and change_analysis == 1 */
	inline const unsigned long long get(gametime &gt, bool change_analysis = true) {
		++framecunt;

		// FPS counter
		static unsigned long long last_fps_shown_at_ms = gt.get_ms();
		unsigned long long now = gt.get_ms();
		unsigned long long diff = now - last_fps_shown_at_ms;
		if(diff >= 1000) {
			last_fps_shown_at_ms = now;
			// printf("FPS: %f, framecount: %zd, diff: %zd\n", gt.fps(), framecunt, diff);
			last_cunt = framecunt * 1000 / diff;
			framecunt = 0;
			return last_cunt;
		}

		return (1 - change_analysis) * last_cunt;
	}

	/**
	 * Ensure the game loop runs at given fps if we run faster - uses usleep for compatibility.
	 *
	 * Remark: This does nothing if we see we are below target fps
	 *
	 * @param gt The gametime
	 * @param target_fps The target frames per second. 0 means no frame cap!
	 * @param close_enough_percent OPTIONAL: If we are below target fps by this amount, we stop throttling at all!
	 * @returns The current fps - as if asked with fps.get(gt, true). So 0 when unchanged and a value ~each second.
	 */
	inline const long long ensure(gametime &gt, long long target_fps, long long close_enough_percent = 20) {

		// The true here and (*) ensures "around" 1s have passed
		long long fps = this->get(gt, true);

		// Exit if they do not want frame capping
		if(target_fps <= 0) return fps;

		// (*) Handle the case where 1s was NOT passed yet!
		// Rem.: If we get real 0 fps, we might enter here ONCE and wait extra... fine...
		if(fps == 0) {
			if(current_sleeptime_us > 0) my_usleep(current_sleeptime_us);
			return fps;
		}

		// Calculate an c% headroom to avoid too often re-measuring
		long long diminished_target_fps = target_fps - (target_fps / (100 / close_enough_percent));

		// If we are BELOW the target fps, just do nothing...
		if(fps <= diminished_target_fps) { current_sleeptime_us = 0; return fps; }

		// Here is the idea for frame capping...
		// 1000 * 1000 us -> fps
		//   target_us us -> target_fps
		//
		// and: target_fps < fps
		//
		// If 1000 ms leads to 100 frames..
		// How many ms leads to 25 frames?
		// => 250 ms
		//
		// => 1000 * 1000 us / (fps / target_fps)
		// => 1000 * 1000 us / (100 / 25)
		// => (1000 * 1000 * 25) us / 100
		// => (1M * target_fps) / fps
		//
		// => Then to get 25 frames / sec,
		//    we need to sleep 750ms or 750 000 us!
		//    BUT THIS IS PER SECONDS! Not every frame!
		//
		// => So in every 1000ms we will sleep 750ms
		//    which should be split between The 25 frames!
		//    Basically I need to divide 750 by 25 too!
		// => 30ms PER FRAME sleep needed (30000 us)
		long long one_sec_us = 1000L * 1000L;
		// Time it takes to draw "25" frames (for target_fps == 25).
		// This measurement contain the drawn frames in the last
		// second - BUT: affected by our own possible usleep!!!
		// So this has (target_fps * current_sleeptime_us)
		// Or for the example: 25 * time_slept...
		long long time_target_frame_count_needs_us = (one_sec_us * target_fps) / fps;
		// Need to add `+ current_sleeptime_us` here because
		// we've SUBTRACTED the sleeps as if they were work!
		// As you can see we did 25x subtraction from the 1s
		// budget, but then it got divided by 25 so its 1x,
		// that we have been removing. We just add that back
		// otherwise we will keep oscillating between once
		// having 25fps, then next second 100fps, then 23... etc...
		long long sleeptime_us = (one_sec_us - time_target_frame_count_needs_us) / target_fps + current_sleeptime_us;
		if(sleeptime_us > 0) my_usleep(sleeptime_us);

		// Need to save this so its applied on every call - not just when a second have happened!
		// Otherwise we would only sleep whenever needed...
		current_sleeptime_us = sleeptime_us;

		return fps;
	}
};

#ifdef _DEFINE_GAMETIME
unsigned long long gametime::init_ms; // Need to be defined to work!
#endif /* _DEFINE_GAMETIME */

#endif /* _GAMETIME_H */

/* vim: set ts=4 sw=4 tw=0 noet : */

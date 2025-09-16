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

/** Simple, lightweight c++14 helper class for measuring game time for the update and draw loops */
class gametime {
private:
	static volatile unsigned long long init_ms;
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
		/* Save start time and last time so that we provide differentials */
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

		// This will be return value optimized anyways - especially being inlined
		return gametime (previous_ms, now_ms);
	}
};
#ifdef _DEFINE_GAMETIME
volatile unsigned long long gametime::init_ms; // Need to be defined to work!
#endif /* _DEFINE_GAMETIME */

#endif /* _GAMETIME_H */

/* vim: set ts=4 sw=4 tw=0 noet : */

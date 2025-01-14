#ifndef INPUT_CONTROLLEr_H
#define INPUT_CONTROLLEr_H

struct InputController
{
	static inline bool m_forward	= false;
	static inline bool m_back		= false;
	static inline bool m_left		= false;
	static inline bool m_right		= false;
	static inline bool m_up			= false;
	static inline bool m_down		= false;

	static inline bool r_posPitch	= false;
	static inline bool r_negPitch	= false;
	static inline bool r_posRoll	= false;
	static inline bool r_negRoll	= false;
	static inline bool r_posYaw		= false;
	static inline bool r_negYaw		= false;

	static inline bool speed1		= false;
	static inline bool speed2		= false;

	static inline bool stop_rot		= false;

	static inline bool Lshift		= false;
	static inline bool Rshift		= false;
	static inline bool Lctrl		= false;
	static inline bool Rctrl		= false;
	static inline bool Lalt			= false;
	static inline bool Ralt			= false;
	
	static inline bool zCommand		= false;
	static inline bool xCommand		= false;
	static inline bool findpluto	= false;
};

#endif

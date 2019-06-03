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
};

#endif

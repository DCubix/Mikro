#ifndef MIK_AUDIO_H
#define MIK_AUDIO_H

#if !__has_include("SDL_audio.h")
#include "SDL2/SDL_audio.h"
#else
#include "SDL_audio.h"
#endif

#include "types.h"
#include "dr_wav.h"

#include <array>
#include <vector>
#include <string>

namespace mik {
	constexpr u32 MikAudioVoices = 16;
	constexpr u32 MikAudioFreq = 44100;

	class Sound {
	public:
		Sound() = default;
		virtual ~Sound() = default;

		Sound(std::string const& fileName);

		u32 frequency() const { return m_frequency; }
		std::vector<f32>& data() { return m_data; }

	private:
		std::vector<f32> m_data;
		u32 m_frequency{ 0 };
	};

	struct Voice {
		enum {
			Stopped = 0,
			Playing
		} state;
		Sound *sound;
		f32 position;
		f32 gain;
		f32 pitch;
		f32 pan;
	};

	class MikAudio {
	public:
		MikAudio();
		virtual ~MikAudio() = default;

		Voice* play(Sound* sound, f32 gain = 1.0f, f32 pitch = 1.0f, f32 pan = 0.0f);
		void stop(Sound* sound);

		void mix(f32* out, u32 samples);

	private:
		std::array<Voice, MikAudioVoices> m_voices;
		u32 m_frequency{ MikAudioFreq };
		f32 m_gain{ 1.0f };

		void resetVoice(u32 id);
		Voice* getFreeVoice();
	};
}

#endif // MIK_AUDIO_H
#ifndef MIK_ANIMATOR_H
#define MIK_ANIMATOR_H

#include <map>
#include <vector>
#include <initializer_list>

#include "types.h"

namespace mik {
	/// Sprite sheet animator class
	class Animator {
		friend class Mik;
	public:
		Animator() = default;
		virtual ~Animator() = default;

		void update(float timeStep);

		void play(const std::string& name, float speed, bool loop);
		void add(const std::string& name, const std::initializer_list<u32>& frames = {});
		void add(const std::string& name, const std::vector<u32>& frames);

		u32 frame() const { return m_currentFrame; }
		const std::string& animation() const { return m_currentAnimation; }

	protected:
		struct Animation {
			f32 speed, time;
			u32 frame;
			bool loop;
			std::vector<u32> frames;
		};

		std::map<std::string, Animation> m_animations;
		std::string m_currentAnimation{ "" };
		u32 m_currentFrame{ 0 };
	};
}

#endif // MIK_ANIMATOR_H
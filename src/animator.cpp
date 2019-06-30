#include "animator.h"

#include "log.h"

namespace mik {
	void Animator::update(float timeStep) {
		if (m_currentAnimation.empty()) return;
		if (m_animations.find(m_currentAnimation) == m_animations.end()) return;

		Animation& anim = m_animations[m_currentAnimation];
		u32 frameCount = anim.frames.size();

		anim.time += timeStep;
		if (anim.time >= anim.speed) {
			anim.time = 0;
			if (anim.frame++ >= frameCount - 1) {
				if (anim.loop) {
					anim.frame = 0;
				} else {
					anim.frame = frameCount - 1;
				}
			}
		}
		m_currentFrame = anim.frames[anim.frame];
	}

	void Animator::play(const std::string& name, float speed, bool loop) {
		if (m_animations.find(name) == m_animations.end()) {
			LogE("Animation \"", name, "\" not found.");
			return;
		}
		Animation& anim = m_animations[name];
		anim.speed = speed;
		anim.loop = loop;
		anim.frame = 0;
		m_currentAnimation = name;
	}

	void Animator::add(const std::string& name, const std::vector<u32>& frames) {
		if (name.empty()) {
			LogE("Invalid animation name.");
			return;
		}

		Animation anim{};
		anim.frames = std::vector<u32>(frames);
		anim.loop = false;
		anim.speed = 0;
		anim.frame = 0;
		anim.time = 0;

		m_animations.insert({ name, anim });
		if (m_currentAnimation.empty()) {
			play(name, 0.25f, true);
		}
	}

	void Animator::add(const std::string& name, const std::initializer_list<u32>& frames) {
		add(name, frames);
	}
}
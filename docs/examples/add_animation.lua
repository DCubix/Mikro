-- Adding animations to a sprite
local ninja = nil

function _init()
	spr = mik.load_sprite("../res/ninja.png")

	-- The add_animation function operates on a Sprite.
	-- An animation is just a list of frames.
	mik.add_animation(ninja, "test", { 0, 4, 8, 12 })

	-- Plays the "test" animation on the "ninja" sprite
	-- The speed is 0.1 seconds, so it will advance to the next
	-- frame every 100 ms
	mik.play(ninja, "test", 0.1, true)
end

function _draw()
	mik.clear()
	mik.spr(ninja, 10, 10)
end
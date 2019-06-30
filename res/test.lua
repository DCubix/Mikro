charMap = " !\"#$%&'()*+,-./0123456789:;=  ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ      abcdefghijklmnopqrstuvwxyz    "
ninja = nil
font = nil
anim = nil

ctx = 0
cty = 0

txt = "Hello World!  Circular Text!  "
letters = {}

t = 0

function _init()
	ninja = mik.load_sprite("../res/ninja.png")
	font = mik.load_sprite("../res/font.png")

	anim = mik.create_animator()
	mik.add_animation(anim, "test", { 0, 4, 8, 12 })
	mik.play(anim, "test", 0.1, true)

	math.randomseed(os.time())
end

function _update(dt)
	t = t + dt * 2.0

	local angle = 0.0
	local step = (math.pi * 2.0) / (#txt)
	if #letters ~= #txt then
		for i = 1, #txt do
			local c = string.sub(txt, i, i)
			local lt = {
				x = 0,
				y = 0,
				c = c,
				spd = math.random(100, 200)
			}
			table.insert(letters, lt)
			angle = angle + step
		end
	else
		ctx = mik.mousex()
		cty = mik.mousey()
		for i = 1, #letters do
			local lt = letters[i]
			local tx = math.cos(angle - t * 0.5) * 50 + ctx
			local ty = math.sin(angle - t * 0.5) * 50 + cty

			local dx = tx - lt.x
			local dy = ty - lt.y
			local len = math.sqrt(dx * dx + dy * dy)
			if len >= 2 then
				if dx ~= 0 and dy ~= 0 then
					dx = dx / len
					dy = dy / len
					lt.x = lt.x + dx * lt.spd * dt
					lt.y = lt.y + dy * lt.spd * dt
				end
				done = false
			end
			angle = angle + step
		end
	end
end

function _draw()
	mik.clear()
	mik.tile(ninja, 7, 4,  mik.frame(anim),  160, 120)

	for i = 1, #letters do
		local lt = letters[i]
		mik.chr(font, charMap, lt.c, lt.x, lt.y)
	end
end
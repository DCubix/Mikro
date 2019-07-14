local MAX_PARTICLES = 2048
local GRAVITY = 12.0
local FRICTION = 0.99
local particles = {}
local fireTime = 0.0

function table.apply(from, to)
	for k, v in pairs(from) do
		to[k] = v
	end
end

function particle_get_dead()
	local p = nil
	for _, pa in pairs(particles) do
		if pa.dead then
			p = pa
			p.dead = false
			p.update = nil
			p.ended = nil
			p.final = false
			break
		end
	end
	return p
end

function particle_new()
	local ded = particle_get_dead()
	if ded == nil then
		local p = {
			x = 0,
			y = 0,
			vx = 0,
			vy = 0,
			life = 1,
			maxLife = 1,
			colorA = { 5, 5, 5 },
			colorB = { 5, 0, 0 },
			color = { 0, 0, 0 },
			dead = false,
			ended = nil,
			update = nil,
			final = false,
			emitTime = 0.0
		}
		if #particles < MAX_PARTICLES then
			table.insert(particles, p)
		end
		return p
	end
	return ded
end

function _make_spark(parent)
	local rangle = math.random() * math.pi * 2.0 - math.pi
	local spd = math.random(1, 4)
	local sublife = math.random(2, 4)
	local nconf = {
		x = parent.x,
		y = parent.y,
		maxLife = sublife,
		life = sublife,
		colorA = parent.colorA,
		colorB = parent.colorB,
		vx = math.cos(rangle) * spd,
		vy = math.sin(rangle) * spd
	}
	local p = particle_new()
	table.apply(nconf, p)
end

function make_emitter(x, y, vx, vy, colorA, colorB, spd, final, lif)
	local life = 270 / spd
	if lif then
		life = lif
	end
	local count = math.random(1, 2)
	local conf = {
		x = x,
		y = y,
		vx = vx,
		vy = vy,
		maxLife = life,
		life = life,
		final = final,
		update = function(p)
			for i = 1, count do _make_spark(p) end
		end,
		ended = function(p)
			if p.final then return end

			local pcount = math.random(10, 20)
			local spd = math.random(30, 50)
			local lf = math.random(50, 120) / spd
			for i = 1, pcount do
				local swapRGB = math.random(0, 10) >= 3
				local angle = (math.pi * 2.0) * (i / pcount) + (math.random() * math.pi / 4.0)

				local colA = nil
				if not swapRGB then
					colA = {
						math.clamp(p.colorA[1] + 2, 0, 5),
						math.clamp(p.colorA[2] + 2, 0, 5),
						math.clamp(p.colorA[3] + 2, 0, 5)
					}
				else
					colA = {
						math.clamp(p.colorA[3] + 2, 0, 5),
						math.clamp(p.colorA[2] + 2, 0, 5),
						math.clamp(p.colorA[1] + 2, 0, 5)
					}
				end

				make_emitter(
					p.x, p.y,
					math.cos(angle) * spd, math.sin(angle) * spd,
					colA, p.colorB,
					spd, true, lf
				)
			end

			print("PARTICLES: " .. #particles)
		end,
		colorA = colorA,
		colorB = colorB
	}
	local p = particle_new()
	table.apply(conf, p)
end

function _init()
	math.randomseed(os.time())
	fireTime = math.random(1, 2)
end

function _update(dt)
	if fireTime <= 0.0 then
		local angle = ((math.random() * math.pi / 4.0) - math.pi / 8.0) - math.pi / 2
		local spd = math.random(80, 160)

		make_emitter(
			math.random(20, mik.width()-20), mik.height(),
			math.cos(angle) * spd, math.sin(angle) * spd,
			{ math.random(2, 5), math.random(2, 5), math.random(2, 5) }, { 0, 0, 0 },
			spd, false
		)
		fireTime = math.random(3, 7)
	else
		fireTime = fireTime - dt
	end

	for _, p in pairs(particles) do
		if not p.dead then
			local t = 1.0 - (p.life / p.maxLife)

			p.color[1] = math.lerp(p.colorA[1], p.colorB[1], t)
			p.color[2] = math.lerp(p.colorA[2], p.colorB[2], t)
			p.color[3] = math.lerp(p.colorA[3], p.colorB[3], t)

			p.vy = p.vy + GRAVITY * dt
			p.x = p.x + p.vx * dt
			p.y = p.y + p.vy * dt
			p.vx = p.vx * FRICTION
			p.vy = p.vy * FRICTION

			p.emitTime = p.emitTime + dt
			if p.emitTime >= 0.05 then
				if p.update then
					p.update(p)
				end
				p.emitTime = 0.0
			end

			p.life = p.life - dt
			if p.life <= 0.0 then
				if p.ended then p.ended(p) end
				p.dead = true
			end

			if p.x > mik.width() or p.x < 0 or p.y > mik.height() or p.y < 0 then
				p.dead = true
			end
		end
	end
end

function _draw()
	mik.clear()
	for _, p in pairs(particles) do
		if not p.dead then
			mik.dot(p.x, p.y, p.color[1], p.color[2], p.color[3])
		end
	end
end
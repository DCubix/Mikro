SPR = {
	gems = nil
}

boardSize = 8
board = {
	data = {},
	x = mik.width() - (16 + boardSize * 26),
	y = mik.height() / 2 - (boardSize * 26) / 2
}
boardCount = 0

selected = nil
target = nil
time = 0
score = 0
matched = 0

STATE_IDLE = 0
STATE_SELECT = 1
STATE_SWAP = 2
STATE_SWAP_ANIM = 3
STATE_CLEAR = 4
STATE_ADD = 5
STATE_SHIFT = 6
STATE_CHECK = 7
STATE_SHIFT_ANIM = 8

state = STATE_IDLE
selectorAnim = nil
delay = 0.0

-- Tween...
function inout_back(t, s)
	local b = 0.0
	local c = 1.0
	local d = 1.0

	-- 1.70158
	if not s then s = 1.0 end
	s = s * 1.525
	t = t / d * 2
	if t < 1 then
		return c / 2 * (t * t * ((s + 1) * t - s)) + b
	else
		t = t - 2
		return c / 2 * (t * t * ((s + 1) * t + s) + 2) + b
	end
end

function out_bounce(t)
	local b = 0.0
	local c = 1.0
	local d = 1.0

	t = t / d
	if t < 1 / 2.75 then
		return c * (7.5625 * t * t) + b
	elseif t < 2 / 2.75 then
		t = t - (1.5 / 2.75)
		return c * (7.5625 * t * t + 0.75) + b
	elseif t < 2.5 / 2.75 then
		t = t - (2.25 / 2.75)
		return c * (7.5625 * t * t + 0.9375) + b
	else
		t = t - (2.625 / 2.75)
		return c * (7.5625 * t * t + 0.984375) + b
	end
end

function board_get(x, y)
	if y == nil then
		y = x[2]
		x = x[1]
	end
	if x < 1 or y < 1 or x > boardSize or y > boardSize then
		return 0
	end
	return board.data[y][x]
end

function board_set(v, x, y)
	if y == nil then
		y = x[2]
		x = x[1]
	end
	if x < 1 or y < 1 or x > boardSize or y > boardSize then
		return
	end
	board.data[y][x] = v
end

function board_step(i, dt)
	board.data[i].t = board.data[i].t + dt
end

function new_gem(val, x, y)
	local rx = (x - 1) * 26
	local ry = (y - 1) * 26
	local b = { gem = val, x = rx, y = ry, xa = rx, ya = ry, xb = rx, yb = ry, t = 0, matched = false }
	board_set(b, x, y)
end

function board_print()
	print('======================')
	for y = 1, boardSize do
		for x = 1, boardSize do
			local b = board.data[y][x]
			local v = type(b) == "table" and b.gem or b
			io.write(v .. " ")
		end
		io.write("\n")
	end
end

function board_swap(fx, fy, tx, ty)
	local tmp = board.data[fy][fx]
	board.data[fy][fx] = board.data[ty][tx]
	board.data[ty][tx] = tmp
end

function _init()
	SPR.gems = mik.load_sprite("../res/gems.png")
	selectorAnim = mik.create_animator()
	mik.add_animation(selectorAnim, "sel", { 32, 33, 34, 35, 36, 37 })
	mik.play(selectorAnim, "sel", 0.1, true)

	math.randomseed(os.time())
	-- Create board
	for y = 1, boardSize do
		local row = {}
		for x = 1, boardSize do
			table.insert(row, 0)
		end
		table.insert(board.data, row)
	end
end

function _update(dt)
	time = time + dt

	if delay > 0.0 then
		delay = delay - dt
	else
		if state == STATE_IDLE then
			if boardCount < boardSize * boardSize then
				state = STATE_ADD
			else
				if mik.btnp("ml") then
					local mx = mik.mousex()
					local my = mik.mousey()
					for y = 1, boardSize do
						for x = 1, boardSize do
							local tx = board.data[y][x].x + board.x
							local ty = board.data[y][x].y + board.y
							if mx > tx and mx < tx + 26 and my > ty and my < ty + 26 then
								selected = { x, y }
								state = STATE_SELECT
								break
							end
						end
					end
				end
			end
		elseif state == STATE_SELECT then
			if mik.btnp("ml") then
				local mx = mik.mousex()
				local my = mik.mousey()
				for y = 1, boardSize do
					for x = 1, boardSize do
						local tx = (x-1) * 26 + board.x
						local ty = (y-1) * 26 + board.y
						if mx > tx and mx < tx + 26 and my > ty and my < ty + 26 then
							target = { x, y }
							state = STATE_SWAP
							delay = 0.1
							break
						end
					end
				end
			end
		elseif state == STATE_SWAP then
			local from = board_get(selected)
			local to = board_get(target)
			from.xa = from.x
			from.xb = to.x
			to.xa = to.x
			to.xb = from.x
			from.ya = from.y
			from.yb = to.y
			to.ya = to.y
			to.yb = from.y

			from.t = 0
			to.t = 0

			state = STATE_SWAP_ANIM
		elseif state == STATE_SWAP_ANIM then
			local from = board_get(selected)
			local to = board_get(target)

			local fsx = from.xa
			local fsy = from.ya
			local ftx = to.xa
			local fty = to.ya
			from.x = math.lerp(fsx, ftx, inout_back(from.t))
			from.y = math.lerp(fsy, fty, inout_back(from.t))

			local tsx = to.xa
			local tsy = to.ya
			local ttx = from.xa
			local tty = from.ya
			to.x = math.lerp(tsx, ttx, inout_back(to.t))
			to.y = math.lerp(tsy, tty, inout_back(to.t))

			from.t = from.t + dt * 2.0
			to.t = to.t + dt * 2.0

			if from.t >= 0.8 and to.t >= 0.8 then
				local tmp = from
				board_set(to, selected)
				board_set(tmp, target)
				selected = nil
				target = nil
				state = STATE_CHECK
				delay = 0.2
			end
		elseif state == STATE_CLEAR then
			for y = 1, boardSize do
				for x = 1, boardSize do
					local b = board.data[y][x]
					if b ~= 0 and b.matched then
						board.data[y][x] = 0
						boardCount = boardCount - 1
					end
				end
			end

			state = STATE_SHIFT
		elseif state == STATE_ADD then
			local addOnTop = board.data[2][1] ~= 0
			local prev = 0
			for x = 1, boardSize do
				if board.data[1][x] == 0 then
					-- unique number
					local val = math.random(1, 4)
					while val == prev do
						val = math.random(1, 4)
					end

					new_gem(val, x, 1)
					if addOnTop then
						local g = board_get(x, 1)
						g.y = g.y - 52
						g.ya = g.ya - 52
					end
					boardCount = boardCount + 1
					prev = val
				end
			end
			state = STATE_SHIFT
		elseif state == STATE_SHIFT then
			for x = 1, boardSize do
				for y = 1, boardSize do
					if board.data[y][x] ~= 0 then
						local chain = 1
						while y + chain <= boardSize and board.data[y + chain][x] == 0 do
							chain = chain + 1
						end
						if chain - 1 > 0 then
							local ny = y + chain - 1
							board.data[y][x].yb = ((ny-1) * 26)
							board.data[y][x].t = 0
							board_swap(x, y, x, ny)
							break
						end
					end
				end
			end
			state = STATE_SHIFT_ANIM
		elseif state == STATE_SHIFT_ANIM then
			local done = true
			for y = 1, boardSize do
				for x = 1, boardSize do
					local b = board_get(x, y)
					if b ~= 0 and b.t < 1.0 then
						b.y = math.lerp(b.ya, b.yb, (b.t))
						b.t = b.t + dt * 8.0
						done = false
					end
				end
			end
			if done then
				for y = 1, boardSize do
					for x = 1, boardSize do
						local b = board_get(x, y)
						if b ~= 0 then
							b.x = (x - 1) * 26
							b.y = (y - 1) * 26
						end
					end
				end
				state = STATE_CHECK
			end
		elseif state == STATE_CHECK then
			if boardCount < boardSize * boardSize then
				state = STATE_ADD
			else
				matched = 0
				for y = 1, boardSize do
					for x = 1, boardSize do
						local b = board_get(x, y)
						if b ~= 0 and not b.matched then
							-- Horizontal
							local chain = 1
							while x + chain <= boardSize and board.data[y][x + chain] ~= 0 and b.gem == board.data[y][x + chain].gem do
								chain = chain + 1
							end

							if chain >= 3 then
								while chain > 0 do
									board.data[y][x + chain - 1].matched = true
									chain = chain - 1
									matched = matched + 1
								end
							end

							-- Vertical
							chain = 1
							while y + chain <= boardSize and board.data[y + chain][x] ~= 0 and b.gem == board.data[y + chain][x].gem do
								chain = chain + 1
							end
							if chain >= 3 then
								while chain > 0 do
									board.data[y + chain - 1][x].matched = true
									chain = chain - 1
									matched = matched + 1
								end
							end
						end
					end
				end
				if matched > 0 then
					state = STATE_CLEAR
				else
					state = STATE_IDLE
				end
			end
		end
	end

end

function _draw()
	mik.clear()

	-- Frame
	local bs = boardSize + 1
	mik.clip(board.x - 13, board.y - 13, bs * 26, bs * 26)
	for y = 1, bs+1 do
		for x = 1, bs+1 do
			local tx = (x - 1) * 26 + board.x - 26
			local ty = (y - 1) * 26 + board.y - 26
			mik.tile(SPR.gems,  5, 8,  12,  tx, ty)
		end
	end
	mik.clip()

	for y = 1, bs do
		for x = 1, bs do
			local tx = (x - 1) * 26 + board.x - 13
			local ty = (y - 1) * 26 + board.y - 13
			local tile = 2
			if x == 1 and y == 1 then
				tile = 2
			elseif x == bs and y == 1 then
				tile = 3
			elseif x == 1 and y == bs then
				tile = 10
			elseif x == bs and y == bs then
				tile = 11
			elseif x >= 2 and x <= bs-1 and y == 1 then
				tile = 4
			elseif x >= 2 and x <= bs-1 and y == bs then
				tile = 5
			elseif y >= 2 and y <= bs-1 and x == 1 then
				tile = 6
			elseif y >= 2 and y <= bs-1 and x == bs then
				tile = 7
			else
				tile = -1
			end
			if tile ~= -1 then
				mik.tile(SPR.gems,  5, 8,  tile,  tx, ty)
			end
		end
	end

	mik.clip(board.x - 2, board.y - 2, boardSize * 26 + 4, boardSize * 26 + 4)
	for y = 1, boardSize do
		for x = 1, boardSize do
			local b = board_get(x, y)
			if b ~= 0 then
				local idx = (b.gem - 1) * 8 + (b.matched and 1 or 0)
				mik.tile(SPR.gems,  5, 8,  idx,  b.x + board.x, b.y + board.y)
			end
		end
	end
	mik.clip()

	local sel = selected
	if sel ~= nil and state ~= STATE_SWAP_ANIM then
		local x = sel[1]
		local y = sel[2]
		local tx = board.data[y][x].x + board.x
		local ty = board.data[y][x].y + board.y
		mik.tile(SPR.gems,  5, 8,  mik.frame(selectorAnim),  tx, ty)
	end
end
SPR = {
    gems = nil
}

boardSize = 8
board = {
    data = {},
    x = mik.width() - (16 + boardSize * 26),
    y = mik.height() / 2 - (boardSize * 26) / 2
}

selected = 0
target = 0
time = 0

STATE_IDLE = 0
STATE_SELECT = 1
STATE_SWAP = 2
STATE_SWAP_ANIM = 3

state = STATE_IDLE

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

function tile_id(x, y)
    return ((x-1) + (y-1) * boardSize) + 1
end

function board_geti(i)
    return board.data[i]
end

function board_get(x, y)
    return board.data[tile_id(x, y)]
end

function board_set(x, y, v)
    board.data[tile_id(x, y)].gem = v
end

function board_step(i, dt)
    board.data[i].t = board.data[i].t + dt
end

function _init()
    SPR.gems = mik.load_sprite("../res/gems.png")

    math.randomseed(os.time())
    -- Create board
    for y = 1, boardSize do
        for x = 1, boardSize do
            local tx = (x-1) * 26 + board.x
            local ty = (y-1) * 26 + board.y
            table.insert(board.data, { gem = math.random(1, 4), x = tx, y = ty, xo = tx, yo = ty, t = 0 })
        end
    end
end

function _update(dt)
    time = time + dt

    if state == STATE_IDLE then
        if mik.btnp("ml") then
            local mx = mik.mousex()
            local my = mik.mousey()
            for y = 1, boardSize do
                for x = 1, boardSize do
                    local tx = (x-1) * 26 + board.x
                    local ty = (y-1) * 26 + board.y
                    if mx > tx and mx < tx + 26 and my > ty and my < ty + 26 then
                        selected = tile_id(x, y)
                        state = STATE_SELECT
                        break
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
                        target = tile_id(x, y)
                        state = STATE_SWAP
                        break
                    end
                end
            end
        end
    elseif state == STATE_SWAP then
        board.data[selected].xo = board.data[selected].x
        board.data[selected].yo = board.data[selected].y
        board.data[selected].t = 0
        board.data[target].xo = board.data[target].x
        board.data[target].yo = board.data[target].y
        board.data[target].t = 0
        
        state = STATE_SWAP_ANIM
    elseif state == STATE_SWAP_ANIM then
        local fsx = board.data[selected].x
        local fsy = board.data[selected].y
        local ftx = board.data[target].xo
        local fty = board.data[target].yo
        board.data[selected].x = math.lerp(fsx, ftx, inout_back(board.data[selected].t))
        board.data[selected].y = math.lerp(fsy, fty, inout_back(board.data[selected].t))

        local tsx = board.data[target].x
        local tsy = board.data[target].y
        local ttx = board.data[selected].xo
        local tty = board.data[selected].yo
        board.data[target].x = math.lerp(tsx, ttx, inout_back(board.data[target].t))
        board.data[target].y = math.lerp(tsy, tty, inout_back(board.data[target].t))

        board_step(selected, dt * 2)
        board_step(target, dt * 2)

        if board.data[selected].t >= 0.8 and board.data[selected].t >= 0.8 then
            local tmp = board.data[selected]
            board.data[selected] = board.data[target]
            board.data[target] = tmp
            selected = 0
            target = 0
            state = STATE_IDLE
        end
    end
end

function _draw()
    mik.clear()

    for y = 1, boardSize do
        for x = 1, boardSize do
            local g = board_get(x, y)
            mik.tile(SPR.gems,  2, 2, g.gem-1,  g.x, g.y)
        end
    end

    local sel = selected
    if sel ~= 0 and state ~= STATE_SWAP_ANIM then
        local x = ((sel-1) % boardSize)
        local y = math.floor((sel-1) / boardSize)
        local tx = x * 26 + board.x
        local ty = y * 26 + board.y
        local s = math.abs(math.sin(sel + time * 8.0)) * 4
        mik.rect(tx + s, ty + s, 26 - s * 2, 26 - s * 2,  math.sin(time * 10.0) * 128 + 128, 128, 255)
    end
end
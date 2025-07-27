SRCS = src/gamelogic/bitboard.c src/gamelogic/fen.c src/gamelogic/movegen.c
OUT = src/main.mjs
EMCC_FLAGS = \
	-s ENVIRONMENT='web' \
	-s SINGLE_FILE=1 \
	-s ASSERTIONS=1 \
	-s EXPORT_ES6=1 \
	-s EXPORT_NAME='createModule' \
	-s EXPORTED_FUNCTIONS='["_get_fen", "_get_bitboards", "_find_possible_board_moves", "_malloc", \
	 "_free", "_make_react_move", "_can_enpassant", "_can_castle", "_start_game", _is_king_attacked, "_is_game_over"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "HEAP32", "HEAPU8"]' \
	-O3

$(OUT): $(SRCS)
	emcc $(SRCS) -o $(OUT) $(EMCC_FLAGS)
	
clean:
	rm -f $(OUT)
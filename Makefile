SRCS = src/gamelogic/updateboard.c src/gamelogic/fen.c src/gamelogic/movegen.c src/gamelogic/eval.c
OUT = src/main.mjs
EMCC_FLAGS = \
	-s ENVIRONMENT='web' \
	-s SINGLE_FILE=1 \
	-s ASSERTIONS=1 \
	-s EXPORT_ES6=1 \
	-s EXPORT_NAME='createModule' \
	-s ALLOW_MEMORY_GROWTH=1 \
	-fsanitize=address -g \
	-s EXPORTED_FUNCTIONS='["_find_possible_board_moves", "_malloc", "_get_best_move", \
	 "_free", "_make_react_move", "_start_game", _is_king_attacked, "_is_game_over", \
	 "_get_random_move", "_get_move_notation"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "HEAP32", "HEAPU8", "UTF8ToString"]' \
	-O3

$(OUT): $(SRCS)
	emcc $(SRCS) -o $(OUT) $(EMCC_FLAGS)
	
clean:
	rm -f $(OUT)

test: 
	gcc -g -fsanitize=address -fno-omit-frame-pointer Unity/src/unity.c src/gamelogic/test_moves.c \
    src/gamelogic/updateboard.c src/gamelogic/movegen.c \
    src/gamelogic/fen.c -o test_moves
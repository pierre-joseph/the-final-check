import "../index.css";
import { useState, useEffect } from "react";
import createModule from "../main.mjs";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from "./FlipBoard";
import { convertFENToBoard, makeMove } from "../updateBoard";
import StartGame from "./StartGame";

export default function Board(props) {
  const [state, setState] = useState({
    Module: null,
    board: convertFENToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"),
    fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
    makeMove: null,
    squareElements: null,
    squareSelected: null,
    isWhite: true,
    canCastle: "KQkq",
    canEnPassant: -1,
    boardPossibleMoves: null,
    getPossibleMoves: null,
    flipBoard: false,
    result: null,
    pendingMove: null,
    gameType: null,
    AITurn: null,
  });

  useEffect(() => {
    createModule().then((Module) => {
      const makeMove = Module.cwrap("make_move", "string", [
        "string",
        "number",
      ]);
      const setBoardMovesFunc = Module.cwrap(
        "find_possible_board_moves",
        "number",
        ["string", "number", "string", "number"]
      );
      const newMovesPtr = setBoardMovesFunc(
        state.fen,
        state.isWhite,
        state.canCastle,
        state.canEnPassant
      );
      const moves = getMovesFromC(newMovesPtr, Module)
      
      setState((prevState) => {
        return {
          ...prevState,
          Module: Module,
          makeMove: makeMove,
          getPossibleMoves: setBoardMovesFunc,
          boardPossibleMoves: moves
        };
      });
    });
  }, []);

  function toggleBoard(row, col) {
    const curSq = (7 - row) * 8 + (7 - col);
    if (state.squareSelected && state.boardPossibleMoves
      .some((move) => move.from === state.squareSelected 
      && move.to === curSq)
    ) {

      const newfen = makeMoveInC(curSq);
      const newMovesPtr = state.getPossibleMoves(
        newfen,
        !state.isWhite,
        state.canCastle,
        state.canEnPassant
      );
      const moves = getMovesFromC(newMovesPtr, state.Module)

      setState((prevState) => ({
        ...prevState,
        squareSelected: null,
        fen: newfen,
        board: convertFENToBoard(newfen),
        isWhite: !prevState.isWhite,
        boardPossibleMoves: moves
      }));
    } else {
      setState((prevState) => {
        return { ...prevState, squareSelected: curSq };
      });
    }
  }

  function getMovesFromC(newMovesPtr, mod){
    const MAX_MOVES = 256;
    const MOVE_SIZE = 4;

    const memBuffer = mod.HEAPU8.buffer;
    const view = new DataView(memBuffer);

    const countOffset = newMovesPtr + MAX_MOVES * MOVE_SIZE;
    const count = view.getInt32(countOffset, true);

    const moves = [];
    for (let i = 0; i < count; i++) {
      const moveOffset = newMovesPtr + i * MOVE_SIZE;
      const move = view.getInt32(moveOffset, true);
      moves.push({from: move & 0x3F, to: (move >> 6) & 0x3F});
    }
    return moves;
  }

  function makeMoveInC(curSq){
    const moveStr = String(state.squareSelected) + "-" + String(curSq) + "\0";
      const length = moveStr.length;
      const wantedMovePtr = state.Module._malloc(length);

      for (let i = 0; i < length; i++) {
        state.Module.HEAP8[wantedMovePtr + i] = moveStr.charCodeAt(i);
      }

      state.Module._free(wantedMovePtr);
      return state.makeMove(state.fen, wantedMovePtr);
  }

  const squareElements = state.board.map((rank, rowNum) =>
    rank.map((curSquare, colNum) => (
      <Square
        key={rowNum * 8 + colNum}
        board={state.board}
        value={curSquare}
        row={rowNum}
        col={colNum}
        handleClick={() => toggleBoard(rowNum, colNum)}
        isSelected={state.squareSelected === (7 - rowNum) * 8 + (7 - colNum)}
        isPossibleMove={
          state.squareSelected &&
          state.boardPossibleMoves
            .some((move) => move.from === state.squareSelected && move.to === (7 - rowNum) * 8 + (7 - colNum))
        }
      />
    ))
  );

  return (
    <main>
      <section className="boardElement">
        {state.flipBoard
          ? squareElements.toReversed().map((rank) => rank.toReversed())
          : squareElements}
      </section>
      {state.gameType == null && (
        <StartGame
          setGameMode={(type) =>
            setState((prevState) => {
              return { ...prevState, gameType: type };
            })
          }
        />
      )}

      {state.gameType && state.result == null && (
        <FlipBoard
          flip={() =>
            setState((prevState) => {
              return {
                ...prevState,
                flipBoard: !prevState.flipBoard,
              };
            })
          }
          isOn={state.flipBoard}
        />
      )}
    </main>
  );
}

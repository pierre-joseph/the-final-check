import "../index.css";
import { useState, useEffect } from "react";
import createModule from "../main.mjs";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from "./FlipBoard";
import { convertFENToBoard } from "../updateBoard";
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
    canCastleFunc: null,
    canEnPassant: -1,
    enpassantFunc: null,
    boardPossibleMoves: null,
    getPossibleMoves: null,
    flipBoard: false,
    kingAttackedFunc: null,
    gameOverFunc: null,
    result: 0,
    pendingMove: null,
    gameType: null,
    startGameFunc: null,
    AITurn: null,
  });

  useEffect(() => {
    createModule().then((Module) => {
      const makeMove = Module.cwrap(
        "make_react_move",
        "string", 
        [
        "number",
      ]);
      const setStartGameFunc = Module.cwrap(
        "start_game",
        null,
        ["string"]
      );
      const setBoardMovesFunc = Module.cwrap(
        "find_possible_board_moves",
        "number",
        ["number", "string", "number"]
      );
      const setEnpassantFunc = Module.cwrap(
        "can_enpassant",
        "number",
        ["number"]
      );
      const setCanCastleFunc = Module.cwrap(
        "can_castle",
        "string",
        ["string", "number"]
      );
      const setKingAttacked = Module.cwrap(
        "is_king_attacked",
        "number",
        ["number"]
      );
      const setGameOverFunc = Module.cwrap(
        "is_game_over",
        "number",
        ["number"]
      );

      setState((prevState) => {
        return {
          ...prevState,
          Module: Module,
          makeMove: makeMove,
          getPossibleMoves: setBoardMovesFunc,
          enpassantFunc: setEnpassantFunc,
          canCastleFunc: setCanCastleFunc,
          startGameFunc: setStartGameFunc,
          kingAttackedFunc: setKingAttacked, 
          gameOverFunc: setGameOverFunc
        };
      });
    });
  }, []);

  function toggleBoard(row, col) {
    const curSq = (7 - row) * 8 + (7 - col);
    if (
      state.squareSelected != null &&
      state.boardPossibleMoves.some(
        (move) => move.from == state.squareSelected && move.to == curSq
      )
    ) {
      const promotionSquare =
        state.board[7 - Math.floor(state.squareSelected / 8)][7 - (state.squareSelected % 8)] &&
        state.board[7 - Math.floor(state.squareSelected / 8)][7 - (state.squareSelected % 8)].type == "p" &&
        (row == 0 || row == 7);

      if (promotionSquare) {
        setState((prevState) => ({
          ...prevState,
          pendingMove: curSq
        }));
        return;
      }

      const {newfen, newEnpassant, newCanCastle} = makeMoveInC(curSq, 0);
      const newMovesPtr = state.getPossibleMoves(
        !state.isWhite,
        state.canCastle,
        newEnpassant
      );
      const moves = getMovesFromC(newMovesPtr, state.Module);
      const newResult = state.gameOverFunc(state.isWhite);

      setState((prevState) => ({
        ...prevState,
        squareSelected: null,
        fen: newfen,
        board: convertFENToBoard(newfen),
        isWhite: !prevState.isWhite,
        boardPossibleMoves: moves,
        canEnPassant: newEnpassant,
        canCastle: newCanCastle,
        result: newResult
      }));
    } else {
      setState((prevState) => {
        return { ...prevState, squareSelected: curSq };
      });
    }
  }

  function updatePromote(type) {
    const to = state.pendingMove;
    const {newfen, newEnpassant, newCanCastle} = makeMoveInC(to, type);
    const newMovesPtr = state.getPossibleMoves(
      !state.isWhite,
      state.canCastle,
      newEnpassant
    );
    setState((prevState) => ({
      ...prevState,
      pendingMove: null
    }));
    const moves = getMovesFromC(newMovesPtr, state.Module);
    const newResult = state.gameOverFunc(state.isWhite);
    setState((prevState) => ({
      ...prevState,
      squareSelected: null,
      fen: newfen,
      board: convertFENToBoard(newfen),
      isWhite: !prevState.isWhite,
      boardPossibleMoves: moves,
      canEnPassant: newEnpassant,
      canCastle: newCanCastle,
      result: newResult
    }));
  }

  function getMovesFromC(newMovesPtr, mod) {
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
      moves.push({
        full_move: move,
        from: move & 0x3f,
        to: (move >> 6) & 0x3f,
        promote: (move >> 12) & 0x7,
        type: (move >> 15) & 0x7,
      });
    }
    return moves;
  }

  function makeMoveInC(curSq, promoteTo) {
    const curMove = state.boardPossibleMoves.find(
      (move) =>
        move.from === state.squareSelected &&
        move.to === curSq &&
        move.promote === promoteTo)
    const newfen = state.makeMove(curMove.full_move);
    const newEnpassant = state.enpassantFunc(curMove.full_move)
    const newCanCastle = state.canCastleFunc(state.canCastle, curMove.full_move);
    return {newfen, newEnpassant, newCanCastle};
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
          state.squareSelected != null &&
          state.boardPossibleMoves.some(
            (move) =>
              move.from === state.squareSelected &&
              move.to === (7 - rowNum) * 8 + (7 - colNum)
          )
        }
        isKingAttacked={state.kingAttackedFunc}
      />
    ))
  );

  return (
    <main>
      <section className="boardElement">
        {state.flipBoard
          ? squareElements.toReversed().map((rank) => rank.toReversed())
          : squareElements}
        {state.pendingMove && (
          <PromoteModal
            isWhite={state.isWhite}
            promoteTo={updatePromote}
            col={7 - (state.pendingMove % 8)}
            moves={state.boardPossibleMoves
              .filter(
                (move) => move.from === state.squareSelected && move.to === state.pendingMove
              )}
          />
        )}
      </section>
      {state.gameType == null && (
        <StartGame
          setGameMode={(type) =>
            setState((prevState) => {
              prevState.startGameFunc(state.fen);
              const newMovesPtr = state.getPossibleMoves(
                state.isWhite,
                state.canCastle,
                state.canEnPassant
              );
              const moves = getMovesFromC(newMovesPtr, state.Module);
              return { ...prevState, gameType: type, boardPossibleMoves: moves };
            })
          }
        />
      )}

      {state.result != 0 && <GameOver winner={state.result} restart={props.restart} />}

      {state.gameType && state.result == 0 && (
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

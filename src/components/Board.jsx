import "../index.css";
import { useState, useEffect } from "react";
import createModule from "../main.mjs";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from "./FlipBoard";
import StartGame from "./StartGame";
import ChooseColor from "./ChooseColor";
import TurnDisplay from "./TurnDisplay";
import MovePanel from "./MovePanel";
import ReplayPanel from "./ReplayPanel";

export default function Board(props) {
  const [state, setState] = useState({
    Module: null,
    board: convertFENToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"),
    fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
    fenList: ["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"],
    moveIdx: 0,
    canCastle: "KQkq",
    makeMove: null,
    squareElements: null,
    squareSelected: null,
    isWhite: true,
    boardPossibleMoves: null,
    getPossibleMoves: null,
    flipBoard: false,
    kingAttackedFunc: null,
    gameOverFunc: null,
    result: 0,
    pendingMove: null,
    gameType: null,
    startGameFunc: null,
    aiIsWhite: null,
    AIFuncRandom: null,
    AIFuncBest: null,
    makingAIMove: false,
    moveNotationFunc: null,
    moveList: [],
  });

  useEffect(() => {
    createModule().then((Module) => {
      const makeMove = Module.cwrap("make_react_move", "string", ["number"]);
      const setStartGameFunc = Module.cwrap("start_game", null, [
        "string",
        "number",
        "string",
      ]);
      const setBoardMovesFunc = Module.cwrap(
        "find_possible_board_moves",
        "number",
        []
      );
      const setKingAttacked = Module.cwrap("is_king_attacked", "number", [
        "number",
      ]);
      const setGameOverFunc = Module.cwrap("is_game_over", "number", []);
      const setAIFuncRandom = Module.cwrap("get_random_move", "number", []);
      const setAIFuncBest = Module.cwrap("get_best_move", "number", ["number"]);
      const setMoveNotationFunc = Module.cwrap("get_move_notation", "string", [
        "number",
      ]);

      setState((prevState) => {
        return {
          ...prevState,
          Module: Module,
          makeMove: makeMove,
          getPossibleMoves: setBoardMovesFunc,
          startGameFunc: setStartGameFunc,
          kingAttackedFunc: setKingAttacked,
          gameOverFunc: setGameOverFunc,
          AIFuncRandom: setAIFuncRandom,
          AIFuncBest: setAIFuncBest,
          moveNotationFunc: setMoveNotationFunc,
        };
      });
    });
  }, []);

  useEffect(() => {
    if (
      state.gameType == "bot" &&
      state.result == 0 &&
      state.isWhite == state.aiIsWhite &&
      state.moveIdx == state.fenList.length - 1
    ) {
      setState((prevState) => ({
        ...prevState,
        makingAIMove: true,
      }));
    }
  }, [state.isWhite, state.aiIsWhite]);

  useEffect(() => {
    if (state.makingAIMove) {
      makeAIMove();
    }
  }, [state.makingAIMove]);

  function toggleBoard(row, col) {
    const curSq = (7 - row) * 8 + (7 - col);
    if (
      state.moveIdx == state.fenList.length - 1 &&
      state.squareSelected != null &&
      state.boardPossibleMoves.some(
        (move) => move.from == state.squareSelected && move.to == curSq
      )
    ) {
      const promotionSquare =
        state.board[7 - Math.floor(state.squareSelected / 8)][
          7 - (state.squareSelected % 8)
        ] &&
        state.board[7 - Math.floor(state.squareSelected / 8)][
          7 - (state.squareSelected % 8)
        ].type == "p" &&
        (row == 0 || row == 7);

      if (promotionSquare) {
        setState((prevState) => ({
          ...prevState,
          pendingMove: curSq,
        }));
        return;
      }
      makeMoveHelper(state.squareSelected, curSq, 0);
    } else if (state.moveIdx == state.fenList.length - 1) {
      setState((prevState) => {
        return { ...prevState, squareSelected: curSq };
      });
    }
  }

  function updatePromote(type) {
    const to = state.pendingMove;
    makeMoveHelper(state.squareSelected, to, type);
    setState((prevState) => ({
      ...prevState,
      pendingMove: null,
    }));
  }

  function makeAIMove() {
    const chosen_move = state.AIFuncBest(4);
    const move_vars = {
      full_move: chosen_move,
      from: chosen_move & 0x3f,
      to: (chosen_move >> 6) & 0x3f,
      promote: (chosen_move >> 12) & 0x7,
      type: (chosen_move >> 15) & 0x7,
    };

    makeMoveHelper(move_vars.from, move_vars.to, move_vars.promote);
  }

  function getMovesFromC(newMovesPtr, mod) {
    const MAX_MOVES = 256;
    const MOVE_SIZE = 4;

    const countOffset = newMovesPtr + MAX_MOVES * MOVE_SIZE;
    const count = mod.HEAP32[countOffset >> 2];

    const moves = [];
    for (let i = 0; i < count; i++) {
      const moveOffset = newMovesPtr + i * MOVE_SIZE;
      const move = mod.HEAP32[moveOffset >> 2];
      moves.push({
        full_move: move,
        from: move & 0x3f,
        to: (move >> 6) & 0x3f,
        promote: (move >> 12) & 0x7,
        type: (move >> 15) & 0x7,
      });
    }

    mod._free(newMovesPtr);
    return moves;
  }

  function makeMoveHelper(from, to, promote) {
    const curMove = state.boardPossibleMoves.find(
      (move) => move.from === from && move.to === to && move.promote === promote
    );
    const moveNotation = state.moveNotationFunc(curMove.full_move);
    const newfen = state.makeMove(curMove.full_move);
    const newMovesPtr = state.getPossibleMoves();
    const moves = getMovesFromC(newMovesPtr, state.Module);
    const newResult = state.gameOverFunc();

    setState((prevState) => ({
      ...prevState,
      squareSelected: null,
      fen: newfen,
      fenList: [...prevState.fenList, newfen],
      moveIdx: prevState.moveIdx + 1,
      board: convertFENToBoard(newfen),
      isWhite: !prevState.isWhite,
      boardPossibleMoves: moves,
      result: newResult,
      makingAIMove: false,
      moveList: [...prevState.moveList, moveNotation],
    }));
  }

  function changeAppearingPosition(moveShift) {
    if (
      (moveShift == -1 && state.moveIdx > 0) ||
      (moveShift == 1 && state.moveIdx < state.fenList.length - 1)
    ) {
      const newIdx = state.moveIdx + moveShift;
      const newfen = state.fenList[newIdx];
      setState((prevState) => ({
        ...prevState,
        squareSelected: null,
        fen: newfen,
        board: convertFENToBoard(newfen),
        moveIdx: newIdx,
      }));
    }
  }

  async function rewatchGame() {
    const newIdx = state.moveIdx + 1;
    const newfen = state.fenList[newIdx];
    await sleep(500);
    setState((prevState) => ({
      ...prevState,
      squareSelected: null,
      fen: newfen,
      board: convertFENToBoard(newfen),
      moveIdx: newIdx,
    }));
  }

  if (state.result != 0 && state.moveIdx != state.fenList.length - 1) {
    rewatchGame();
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
        flipped={state.flipBoard}
        result={state.result}
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
            flipped={state.flipBoard}
            moves={state.boardPossibleMoves.filter(
              (move) =>
                move.from === state.squareSelected &&
                move.to === state.pendingMove
            )}
          />
        )}
      </section>
      {state.gameType == null && (
        <StartGame
          setGameMode={(type) =>
            setState((prevState) => {
              prevState.startGameFunc(
                state.fen,
                state.isWhite,
                state.canCastle
              );

              const newMovesPtr = state.getPossibleMoves();
              const moves = getMovesFromC(newMovesPtr, state.Module);
              return {
                ...prevState,
                gameType: type,
                boardPossibleMoves: moves,
              };
            })
          }
        />
      )}

      {state.gameType == "bot" && state.aiIsWhite == null && (
        <ChooseColor
          setAIWhite={(botIsWhite) =>
            setState((prevState) => {
              return {
                ...prevState,
                flipBoard: botIsWhite,
                aiIsWhite: botIsWhite,
              };
            })
          }
        />
      )} 

      {state.result != 0 && state.moveIdx == state.fenList.length - 1 && (
        <GameOver
          winner={state.result}
          restart={props.restart}
          rewatch={() => {
            const newfen = state.fenList[0];
            setState((prevState) => {
              return {
                ...prevState,
                squareSelected: null,
                fen: newfen,
                board: convertFENToBoard(newfen),
                moveIdx: 0,
              };
            });
          }}
        />
      )}

      {(state.gameType == "human" || state.aiIsWhite != null) &&
        state.result == 0 && <MovePanel moveList={state.moveList} />}

      {(state.gameType == "human" || state.aiIsWhite != null) &&
        state.result == 0 && (
          <ReplayPanel
            onClick={(moveShift) => changeAppearingPosition(moveShift)}
          />
        )}

      {(state.gameType == "human" || state.aiIsWhite != null) &&
        state.result == 0 && <TurnDisplay isWhite={state.isWhite} />}

      {(state.gameType == "human" || state.aiIsWhite != null) &&
        state.result == 0 && (
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

function convertFENToBoard(fen) {
  const fenBoard = fen.split(" ")[0];
  let board = [];
  let curRank = [];
  let col = 0;
  let row = 0;

  for (let char of fenBoard) {
    if (char == "/") {
      board.push(curRank);
      curRank = [];
      row = row + 1;
      col = 0;
    } else {
      if (Number.isInteger(Number(char))) {
        for (let i = 0; i < Number(char); i++) {
          curRank.push(null);
        }
        col = col + Number(char);
      } else {
        const isUpperCase = char === char.toUpperCase();
        const type = char.toLowerCase();
        curRank.push({ iswhite: isUpperCase, type: type });
        col += 1;
      }
    }
  }
  board.push(curRank);
  return board;
}

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

import "./index.css";
import { useState } from "react";
import Square from "./Square";
import GameOver from "./GameOver";
import PromoteModal from "./PromoteModal";
import FlipBoard from './FlipBoard';
import {
  getNewBoard,
  getOppColor,
  findAllPossibleBoardMoves,
  isKingAttacked,
  checkIfOver,
  checkIfCanCastle,
  checkIfCanEnPassant,
} from "./updateBoard";

export default function Board(props) {
  const [board, setBoard] = useState([
    [
      { color: "black", type: "r" },
      { color: "black", type: "n" },
      { color: "black", type: "b" },
      { color: "black", type: "q" },
      { color: "black", type: "k" },
      { color: "black", type: "b" },
      { color: "black", type: "n" },
      { color: "black", type: "r" },
    ],
    [
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
      { color: "black", type: "p" },
    ],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [null, null, null, null, null, null, null, null],
    [
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
      { color: "white", type: "p" },
    ],
    [
      { color: "white", type: "r" },
      { color: "white", type: "n" },
      { color: "white", type: "b" },
      { color: "white", type: "q" },
      { color: "white", type: "k" },
      { color: "white", type: "b" },
      { color: "white", type: "n" },
      { color: "white", type: "r" },
    ],
  ]);
  const [squareSelected, setSquareSelected] = useState(null);
  const [curTurn, setCurTurn] = useState("white");
  const [canCastle, setCanCastle] = useState({
    whiteQueenside: true,
    whiteKingside: true,
    blackQueenside: true,
    blackKingside: true,
  });
  const [canEnPassant, setCanEnPassant] = useState(null);
  const [boardPossibleMoves, setBoardPossibleMoves] = useState(
    findAllPossibleBoardMoves(board, "white", canEnPassant, canCastle)
  );
  const [result, setResult] = useState(null);
  const [pendingMove, setPendingMove] = useState(null);
  const [flipBoard, setFlipBoard] = useState(false);

  function toggleSquareSelected(curRow, curCol) {
    const newSquare = {
      value: board[curRow][curCol],
      row: curRow,
      col: curCol,
    };
    if (
      squareSelected &&
      boardPossibleMoves[squareSelected.row][squareSelected.col].some(
        (sq) => sq.row === curRow && sq.col === curCol
      )
    ) {
      const promotionSquare =
        squareSelected.value.type == "p" &&
        (newSquare.row == 0 || newSquare.row == 7);

      if (promotionSquare) {
        setPendingMove({
          startSquare: squareSelected,
          endSquare: newSquare,
        });
        return;
      }

      setSquareSelected(null);
      setCurTurn((prevTurn) => getOppColor(prevTurn));
      setBoard((prevBoard) => {
        const newBoard = getNewBoard(prevBoard, squareSelected, newSquare);
        setBoardPossibleMoves(() => {
          const moves = findAllPossibleBoardMoves(
            newBoard,
            getOppColor(curTurn),
            canEnPassant,
            canCastle
          );
          setResult(() => checkIfOver(newBoard, moves, curTurn));
          return moves;
        });

        return newBoard;
      });

      setCanCastle((prev) => checkIfCanCastle(prev, squareSelected));
      setCanEnPassant(() => checkIfCanEnPassant(squareSelected, newSquare));
    } else if (newSquare.value && newSquare.value.color == curTurn) {
      setSquareSelected(newSquare);
    }
  }

  function updatePromote(type) {
    const {startSquare, endSquare} = pendingMove;
    setPendingMove(null);
    setSquareSelected(null);
    setCurTurn((prevTurn) => getOppColor(prevTurn));
    setBoard((prevBoard) => {
      const newBoard = getNewBoard(prevBoard, startSquare, endSquare, type);
      setBoardPossibleMoves(() => {
        const moves = findAllPossibleBoardMoves(
          newBoard,
          getOppColor(curTurn),
          canEnPassant,
          canCastle
        );
        setResult(() => checkIfOver(newBoard, moves, curTurn));
        return moves;
      });

      return newBoard;
    });
    setCanCastle((prev) => checkIfCanCastle(prev, startSquare));
    setCanEnPassant(() => checkIfCanEnPassant(startSquare, endSquare));
  }

  const squareElements = board.map((rank, rowNum) => {
    return rank.map((curSquare, colNum) => {
      return (
        <Square
          key={rowNum * 8 + colNum}
          board={board}
          value={curSquare}
          row={rowNum}
          col={colNum}
          isSelected={
            squareSelected &&
            squareSelected.row == rowNum &&
            squareSelected.col == colNum
          }
          handleBoardChange={toggleSquareSelected}
          isPossibleMove={
            squareSelected &&
            boardPossibleMoves[squareSelected.row][squareSelected.col].some(
              (sq) => sq.row === rowNum && sq.col === colNum
            )
          }
          isKingAttacked={isKingAttacked}
        />
      );
    });
  });

  return (
    <main>
      <section className="boardElement">
        {flipBoard ? squareElements.toReversed().map(rank => rank.toReversed()) :  squareElements}
        {pendingMove && (
          <PromoteModal
            color={curTurn}
            col={pendingMove.endSquare.col}
            promoteTo={updatePromote}
            moves={boardPossibleMoves[squareSelected.row][squareSelected.col].filter(
              (sq) => sq.row === pendingMove.endSquare.row && sq.col === pendingMove.endSquare.col
            )}
          />
        )}
      </section>
      {result && <GameOver winner={result} restart={props.restart} />}
      <FlipBoard flip={() => setFlipBoard(!flipBoard)} isOn={flipBoard}/>
    </main>
  );
}

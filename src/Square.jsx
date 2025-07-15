import "./index.css";
import whitePawn from "./assets/whitePawn.svg";
import whiteKnight from "./assets/whiteKnight.svg";
import whiteBishop from "./assets/whiteBishop.svg";
import whiteRook from "./assets/whiteRook.svg";
import whiteQueen from "./assets/whiteQueen.svg";
import whiteKing from "./assets/whiteKing.svg";
import blackPawn from "./assets/blackPawn.svg";
import blackKnight from "./assets/blackKnight.svg";
import blackBishop from "./assets/blackBishop.svg";
import blackRook from "./assets/blackRook.svg";
import blackQueen from "./assets/blackQueen.svg";
import blackKing from "./assets/blackKing.svg";

export default function Square(props) {
  function lightSquare() {
    if (props.row % 2 == 0 && props.col % 2 == 0) {
      return true;
    } else if (props.row % 2 == 1 && props.col % 2 == 1) {
      return true;
    } else {
      return false;
    }
  }

  function getImage() {
    if (props.value.color == "white") {
      switch (props.value.type) {
        case "p":
          return whitePawn;
        case "n":
          return whiteKnight;
        case "b":
          return whiteBishop;
        case "r":
          return whiteRook;
        case "q":
          return whiteQueen;
        case "k":
          return whiteKing;
      }
    } else if (props.value.color == "black") {
      switch (props.value.type) {
        case "p":
          return blackPawn;
        case "n":
          return blackKnight;
        case "b":
          return blackBishop;
        case "r":
          return blackRook;
        case "q":
          return blackQueen;
        case "k":
          return blackKing;
      }
    }
  }

  function handleClick() {
    props.handleBoardChange(props.row, props.col);
  }

  function getBackgroundColor() {
    if (
      props.value != null &&
      props.value.type == "k" &&
      props.isKingAttacked(props.board, props.value.color)
    ) {
      return "#E63946";
    } else if (props.isSelected) {
      return "#e6c95f";
    } else if (lightSquare()) {
      return "#f9f5e9";
    } else {
      return "#b49b7f";
    }
  }

  const backgroundColor = getBackgroundColor();
  const squareStyles = { backgroundColor: backgroundColor };

  return (
    <section
      className="squareElement"
      style={squareStyles}
      onClick={handleClick}
    >
      {props.value != null && <img className="pieceImg" src={getImage()} />}
      {props.isPossibleMove && <div className="lightCircle"></div>}
    </section>
  );
}

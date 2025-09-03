import "../index.css";
import whitePawn from "../assets/whitePawn.svg";
import whiteKnight from "../assets/whiteKnight.svg";
import whiteBishop from "../assets/whiteBishop.svg";
import whiteRook from "../assets/whiteRook.svg";
import whiteQueen from "../assets/whiteQueen.svg";
import blackPawn from "../assets/blackPawn.svg";
import blackKnight from "../assets/blackKnight.svg";
import blackBishop from "../assets/blackBishop.svg";
import blackRook from "../assets/blackRook.svg";
import blackQueen from "../assets/blackQueen.svg";

export default function PlayerTags(props) {
  const whiteCaptures = [];
  const blackCaptures = [];
  let whiteMaterial = 0;
  let blackMaterial = 0;

  const pieceTypes = ["q", "r", "b", "n", "p"];
  const whiteImages = [
    whiteQueen,
    whiteRook,
    whiteBishop,
    whiteKnight,
    whitePawn,
  ];
  const blackImages = [
    blackQueen,
    blackRook,
    blackBishop,
    blackKnight,
    blackPawn,
  ];

  for (let i = 0; i < 5; i++) {
    for (let j = 0; j < props.capturedPieces.length; j++) {
      const capture = props.capturedPieces[j];
      if (capture && capture.type == pieceTypes[i]) {
        if (capture.iswhite) {
          blackCaptures.push(
            <img key={j} className="smallPieceImg" src={whiteImages[i]} />
          );
        } else {
          whiteCaptures.push(
            <img key={j} className="smallPieceImg" src={blackImages[i]} />
          );
        }
      }
    }
  }

  for (let row of props.board) {
    for (let sq of row) {
      if (sq) {
        if (sq.iswhite) {
          switch (sq.type) {
            case "p":
              whiteMaterial += 1;
              break;
            case "n":
              whiteMaterial += 3;
              break;
            case "b":
              whiteMaterial += 3;
              break;
            case "r":
              whiteMaterial += 5;
              break;
            case "q":
              whiteMaterial += 9;
              break;
          }
        } else {
          switch (sq.type) {
            case "p":
              blackMaterial += 1;
              break;
            case "n":
              blackMaterial += 3;
              break;
            case "b":
              blackMaterial += 3;
              break;
            case "r":
              blackMaterial += 5;
              break;
            case "q":
              blackMaterial += 9;
              break;
          }
        }
      }
    }
  }

  return (
    <section className="playerTags">
      <div
        className="whiteTag"
        style={{ top: props.flipped ? "108px" : "785px" }}
      >
        <section
          className="whiteSq"
          style={{ backgroundColor: "#FFFFFF" }}
        ></section>
        <span style={{ color: "#FFFFFF", fontSize: "15px" }}> White </span>
        {whiteCaptures}
        {whiteMaterial > blackMaterial && (
          <span
            style={{ color: "#FFFFFF", fontSize: "10px", marginLeft: "10px" }}
          >
            + {whiteMaterial - blackMaterial}
          </span>
        )}
      </div>

      <div
        className="blackTag"
        style={{ top: !props.flipped ? "108px" : "785px" }}
      >
        <section
          className="blackSq"
          style={{ backgroundColor: "#000000", fontSize: "15px" }}
        ></section>
        <span style={{ color: "#000000" }}> Black </span>
        {blackCaptures}
        {blackMaterial > whiteMaterial && (
          <span
            style={{ color: "#000000", fontSize: "10px", marginLeft: "10px" }}
          >
            + {blackMaterial - whiteMaterial}
          </span>
        )}
      </div>
    </section>
  );
}

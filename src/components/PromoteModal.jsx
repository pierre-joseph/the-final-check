import "../index.css";
import whiteKnight from "../assets/whiteKnight.svg";
import whiteBishop from "../assets/whiteBishop.svg";
import whiteRook from "../assets/whiteRook.svg";
import whiteQueen from "../assets/whiteQueen.svg";
import blackKnight from "../assets/blackKnight.svg";
import blackBishop from "../assets/blackBishop.svg";
import blackRook from "../assets/blackRook.svg";
import blackQueen from "../assets/blackQueen.svg";

export default function PromoteModal(props) {
  function getImages() {
    return props.moves.map((move) => {
      return { type: move.promote, img: getImage(move.promote) };
    });
  }

  function getImage(type) {
    if (props.isWhite) {
      switch (type) {
        case 1:
          return whiteRook;
        case 2:
          return whiteKnight;
        case 3:
          return whiteBishop;
        case 4:
          return whiteQueen;
      }
    } else {
      switch (type) {
        case 1:
          return blackRook;
        case 2:
          return blackKnight;
        case 3:
          return blackBishop;
        case 4:
          return blackQueen;
      }
    }
  }

  const images = getImages();
  const imageElements = images.map((piece, idx) => {
    return (
      <section
        key={idx}
        className="squareElement"
        style={{
          backgroundColor: "#FFFDD0"
        }}
      >
        <img
          className="pieceImg"
          src={piece.img}
          onClick={() => props.promoteTo(piece.type)}
        />
      </section>
    );
  });

  const leftOffset = props.flipped ? 75 * (7 - props.col) : 75 * props.col;
  return (
    <section
      style={{
        position: "absolute",
        top: ((props.isWhite && !props.flipped) || (!props.isWhite && props.flipped)) ? "0px" : "300px",
        left: `${leftOffset}px`,
      }}
    >
      {imageElements}
    </section>
  );
}

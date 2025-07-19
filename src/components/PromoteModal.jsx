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
      return {type: move.promoteTo, img: getImage(move.promoteTo)}
    })
  }

  function getImage(type) {
    if (props.color == "white") {
      switch (type) {
        case "n":
          return whiteKnight;
        case "b":
          return whiteBishop;
        case "r":
          return whiteRook;
        case "q":
          return whiteQueen;
      }
    } else if (props.color == "black") {
      switch (type) {
        case "n":
          return blackKnight;
        case "b":
          return blackBishop;
        case "r":
          return blackRook;
        case "q":
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
        style={{ backgroundColor: "#FFFFFF" }}
      >
        <img className="pieceImg" src={piece.img} onClick={() => props.promoteTo(piece.type)}/>
      </section>
    );
  });

  const leftOffset = 75 * props.col


  return (
    <section style={{ 
      position: 'absolute',
      top: props.color == "white" ? "0px" : "300px", 
      left: `${leftOffset}px`
    }}>
      {imageElements}
    </section>
  );
}

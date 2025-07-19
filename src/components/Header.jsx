import creamPawn from "../assets/creamPawn.png";
import "../index.css";

export default function Header() {
  return (
    <header>
      <img src={creamPawn} alt="Logo" />
      <span>A Really Basic Chess App</span>
    </header>
  );
}

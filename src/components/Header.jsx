import creamPawn from "../assets/creamPawn.png";
import "../index.css";

export default function Header() {
  return (
    <header>
      <img src={creamPawn} alt="Logo" />
      <span>The Final Check</span>
    </header>
  );
}

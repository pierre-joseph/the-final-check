import Header from "./Header";
import Board from "./Board";
import { useState } from "react";
import "../index.css";

function App() {
  const [resetKey, setResetKey] = useState(0);

  function restartApp() {
    setResetKey(resetKey + 1);
  }

  return (
    <>
      <Header />
      <Board key={resetKey} restart={restartApp} />
    </>
  );
}

export default App;

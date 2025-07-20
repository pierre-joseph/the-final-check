export function convertFENToBoard(fen) {
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

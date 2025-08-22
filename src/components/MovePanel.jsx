import { useEffect, useRef } from "react";
import '../index.css';

export default function MovePanel(props) {

    const panelRef = useRef(null);

    useEffect(() => {
        if (panelRef.current) {
        panelRef.current.scrollTop = panelRef.current.scrollHeight;
        }
    }, [props.moveList]);
    
    const individualMoves = props.moveList.map((move, idx) => 
        <p key={idx}> {move}</p>
    )

    const movesLength = Math.ceil(individualMoves.length / 2) - 1;
    const moveNums = [];
    for (let i = 1; i <= movesLength + 1; i++){
        moveNums.push(<p key={individualMoves.length + i}> {i}. </p>);
    }

    const movesWithNums = []
    const movesWithNumsLength = individualMoves.length + movesLength;
    let numsIdx = 0;
    let movesIdx = 0;
    for (let i = 0; i <= movesWithNumsLength; i++){
        if (i % 3 == 0) {
            movesWithNums.push(moveNums[numsIdx])
            numsIdx += 1;
        } else {
            movesWithNums.push(individualMoves[movesIdx])
            movesIdx += 1;
        }
    }

    return (
        <section className='movePanel' ref={panelRef}>
            <span> Moves: </span>
            <section className='moveTexts'>
                {movesWithNums}
            </section>
        </section>
    )
}
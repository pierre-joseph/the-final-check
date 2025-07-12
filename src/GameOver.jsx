import './index.css'

export default function GameOver(props){
    return (
        <section className='winningMessage'>
            {props.winner == "Draw" ? <span>Draw!</span> : 
            <span>{props.winner == "white" ? "White": "Black"} has won!</span>}
            <button onClick={props.restart}>Play Again</button>
        </section>
    )
}
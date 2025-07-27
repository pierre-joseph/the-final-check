import '../index.css'

export default function GameOver(props){
    return (
        <section className='winningMessage'>
            {props.winner == 1 ? <span>Draw!</span> : 
            <span>{props.winner == 2 ? "White": "Black"} has won!</span>}
            <button onClick={props.restart}>Play Again</button>
        </section>
    )
}
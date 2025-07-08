import './index.css'

export default function GameOver(props){
    return (
        <section className='winningMessage'>
            {props.winner == "Draw" ? <span>Draw!</span> : <span>{props.winner} has won!</span>}
            <button onClick={props.restart}>Restart</button>
        </section>
    )
}
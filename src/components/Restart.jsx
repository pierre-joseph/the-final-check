import '../index.css'

export default function Restart(props){
    return (
        <section className='restartButton'>
            <button onClick={props.restart}>Restart Game</button>
        </section>
    )
}
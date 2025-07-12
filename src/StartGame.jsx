import './index.css'

export default function StartGame(props){
    return (
        <section className='startGame'>
            <span>Let's Play Chess</span>
            <button onClick={() => props.setGameMode("bot")}>Human Vs Bot</button>
            <button onClick={() => props.setGameMode("human")}>Human vs Human</button>
        </section>
    )
}
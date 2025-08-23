import '../index.css';

export default function ReplayPanel(props) {
    return (
        <section className='replayPanel'>
            <div className='replayButtons'>
                <button onClick={() => props.onClick(-1)}> {"<"} </button>
                <span> | </span>
                <button onClick={() => props.onClick(1)}> {">"}  </button>
            </div>

            <span className="replayText">Replay Moves</span>
        </section>
    )
}
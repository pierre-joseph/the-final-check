import "./index.css";

export default function FlipBoard(props){
    return (
        <section className="flipElement">
            <div className={`flipButton ${props.isOn ? 'on' : ''}`} onClick={props.flip}>
                <div className="flipCircle" />
            </div>
            <span className="flipText">Flip Board</span>
        </section>
    )
}
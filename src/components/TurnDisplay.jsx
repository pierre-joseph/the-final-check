import '../index.css'

export default function TurnDisplay(props){
    const color = props.isWhite ? "#FFFFFF" : "#000000";
    const colorStyles = { color: color };
    const bgStyles = { backgroundColor: color };

    return (
        <section className='turnDisplay'>
            <section
                className="turnSq"
                style={bgStyles}> 
            </section>
            {props.isWhite ? 
            <span style={colorStyles}> White to Play </span> 
            : <span style={colorStyles}> Black to Play </span>} 
        </section>
    )
}
import '../index.css'

export default function ChooseColor(props){
    return (
        <section className='chooseColor'>
            <span>Choose Color</span>
            <button onClick={() => props.setAIWhite(false)}>White</button>
            <button onClick={() => props.setAIWhite(true)}>Black</button>
        </section>
    )
}
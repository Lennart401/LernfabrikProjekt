import Box from "@/app/components/box";

const VARIANTS = {
    'left-wrap': 'flex-row justify-start flex-wrap pr-24',
    'center-horizontal': 'flex-row justify-center flex-nowrap pr-24',
    'center-vertical': 'flex-col items-center flex-nowrap pb-16',
}

export default function BoxesList({ variant, boxes, className }) {
    const classes = VARIANTS[variant];

    return (
        <div className={`flex gap-2 ${classes} ${className} p-2 m-2 border border-gray-400`}>
            {boxes.map((box, index) => (
                <Box key={index} number={box.number} state={box.state}/>
            ))}
            {boxes.length === 0 && <Box placeholder={true}/>}
        </div>
    )
}
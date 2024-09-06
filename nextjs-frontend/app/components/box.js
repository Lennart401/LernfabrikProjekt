const STATE_TO_COLOR = {
    1:  {'bg': 'bg-green-700',   'border': 'border-green-700',   'fg': 'text-white', 'opacity': 'opacity-100'}, // SUPPLY QUEUE
    2:  {'bg': 'bg-yellow-300',  'border': 'border-yellow-500',  'fg': 'text-black', 'opacity': 'opacity-75' }, // IN USE
    3:  {'bg': 'bg-red-100',     'border': 'border-red-400',     'fg': 'text-black', 'opacity': 'opacity-50'  }, // READY FOR PICKUP
    4:  {'bg': 'bg-pink-100',    'border': 'border-pink-400',    'fg': 'text-black', 'opacity': 'opacity-50'  }, // WAITING MOVE TO WAREHOUSE
    5:  {'bg': 'bg-rose-100',    'border': 'border-rose-500',    'fg': 'text-black', 'opacity': 'opacity-50'  }, // MOVING TO WAREHOUSE
    6:  {'bg': 'bg-purple-100',  'border': 'border-purple-600',  'fg': 'text-black', 'opacity': 'opacity-50'  }, // WAITING FOR REFILL
    7:  {'bg': 'bg-neutral-300', 'border': 'border-neutral-500', 'fg': 'text-black', 'opacity': 'opacity-75' }, // REFILLING
    8:  {'bg': 'bg-neutral-600', 'border': 'border-neutral-600', 'fg': 'text-white', 'opacity': 'opacity-100'}, // WAITING FOR WAY BACK
    9:  {'bg': 'bg-sky-400',     'border': 'border-sky-400',     'fg': 'text-white', 'opacity': 'opacity-100'}, // MOVING TO PRODUCTION
    10: {'bg': 'bg-sky-800',     'border': 'border-sky-800',     'fg': 'text-white', 'opacity': 'opacity-100'}, // WAITING MOVE TO RAMP
}

export const dynamic = 'force-dynamic'

export default function Box({ number, state, placeholder = false }) {
    if (placeholder) {
        return (
            <div className={'w-20 h-14 flex justify-center items-center text-gray-400 border-2 border-gray-100'}>
                ...
            </div>
        )
    } else {
        const textColor = STATE_TO_COLOR[state]['fg']
        const bgColor = STATE_TO_COLOR[state]['bg']
        const borderColor = STATE_TO_COLOR[state]['border']
        const opacity = STATE_TO_COLOR[state]['opacity']

        return (
            <div
                className={`w-20 h-14 flex justify-center items-center ${textColor} ${bgColor} border-2 ${borderColor}`}>
                # {number}
            </div>
        )
    }
}
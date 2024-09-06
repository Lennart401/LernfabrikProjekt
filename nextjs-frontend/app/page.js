'use client';

import useWebSocket from "@/app/hooks/useWebSocket";
import BoxesList from "@/app/components/boxes-list";
import {useMemo} from "react";

export default function Home() {
    const { messages } = useWebSocket(process.env.NEXT_PUBLIC_WEBSOCKET_URL);

    const stateToLocation = {
        1: 'workstation',
        2: 'workstation',
        3: 'workstation',
        4: 'production_to_warehouse',
        5: 'production_to_warehouse',
        6: 'warehouse_pre',
        7: 'warehouse_refilling',
        8: 'warehouse_post',
        9: 'warehouse_to_production',
        10: 'warehouse_to_production',
    }

    const states = useMemo(() => {
        // get last message
        const lastMessage = messages[messages.length - 1];

        if (!lastMessage) {
            return [];
        }

        // get the box states from "states" key
        return JSON.parse(lastMessage).states;
    }, [messages]);

    // create an initial value for the accumulation that contains the unique values from the stateToLocation object
    // and an empty array for each of them
    const initialValue = Object.values(stateToLocation).reduce((acc, location) => {
        acc[location] = [];
        return acc;
    }, {});

    // create an object that uses the values from the stateToLocation object as keys, and then puts a box with the corresponding state into the array
    const boxes = states.reduce((acc, {box_id, state}) => {
        const location = stateToLocation[state];

        acc[location].push({ number: box_id, state: state });

        return acc;
    }, initialValue);

    return (
        <div id={'view_wrapper'} className={'h-screen flex flex-col justify-center items-center'}>
            <div id={'outer_box'} className={'flex flex-row justify-center items-stretch gap-8 m-2'}>
                <div id={'warehouse_wrapper'} className={'flex flex-row justify-center items-center border-2 border-dashed p-2'}>
                    <div id={'warehouse_main'} className={'flex flex-col gap-2 justify-center items-center'}>
                        <BoxesList variant={'left-wrap'} boxes={boxes['warehouse_pre']}/>
                        <img src={'/warehouse.svg'} alt={'warehouse'} className={'w-44'}/>
                        <BoxesList variant={'left-wrap'} boxes={boxes['warehouse_post']}/>
                    </div>
                    <div id={'warehouse_refilling'}>
                        <BoxesList variant={'center-vertical'} boxes={boxes['warehouse_refilling']}/>
                    </div>
                </div>
                <div id={'transitions'} className={'flex flex-col gap-32 justify-between'}>
                    <div id={'production_to_warehouse'} className={'flex flex-col items-center border-2 border-dashed p-2'}>
                        <img src={'/wagon.svg'} alt={'wagon'} className={'w-20'}/>
                        <BoxesList variant={'center-horizontal'} boxes={boxes['production_to_warehouse']}/>
                        <img src={'/arrow_left.svg'} alt={'arrow'} className={'w-16'}/>
                    </div>
                    <div id={'warehouse_to_production'} className={'flex flex-col items-center border-2 border-dashed p-2'}>
                        <img src={'/wagon.svg'} alt={'wagon'} className={'w-20'}/>
                        <BoxesList variant={'center-horizontal'} boxes={boxes['warehouse_to_production']}/>
                        <img src={'/arrow_right.svg'} alt={'arrow'} className={'w-16'}/>
                    </div>
                </div>
                <div id={'workstation_0'} className={'flex flex-col justify-center items-center gap-2 border-2 border-dashed p-2'}>
                    <div className={'flex flex-col justify-center items-center border border-gray-600 w-44 h-32 relative gap-2'}>
                        <img src={'/worker.svg'} alt={'worker'} className={'w-14'}/>
                        <p className={'text-xl'}>AP #1</p>
                    </div>
                    <BoxesList variant={'left-wrap'} boxes={boxes['workstation']}/>
                </div>
            </div>
        </div>
    );
}

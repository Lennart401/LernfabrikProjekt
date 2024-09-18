import { useEffect, useState } from 'react';

const useWebSocket = (port) => {
    const [messages, setMessages] = useState([]);
    const [ws, setWs] = useState(null);

    useEffect(() => {
        const socket = new WebSocket(`ws://${window.location.hostname}:${port}/`);
        setWs(socket);

        socket.onmessage = (event) => {
            setMessages((prevMessages) => [...prevMessages, event.data]);
        };

        return () => {
            socket.close();
        };
    }, [port]);

    const sendMessage = (message) => {
        if (ws) {
            ws.send(message);
        }
    };

    return { messages, sendMessage };
};

export default useWebSocket;

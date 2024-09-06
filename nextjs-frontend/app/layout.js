import { Inter } from "next/font/google";
import "./globals.css";

const inter = Inter({ subsets: ["latin"] });

export const metadata = {
    title: "Lernfabrik: KI-Kanban-Behälter",
    description: "Frontend für die KI-Kanban-Behälter in der Lernfabrik",
};

export default function RootLayout({ children }) {
    return (
        <html lang="de">
            <body className={inter.className}>
                <div className={'v-screen flex flex-col justify-center items-center'}>
                    <header className={'flex flex-row justify-center items-center w-full p-4 bg-gray-200 text-gray-900'}>
                        <h1>{metadata.title}</h1>
                        {/*<nav>*/}
                        {/*    <ul className={'flex flex-row gap-4'}>*/}
                        {/*        <li><a href={'/'}>Home</a></li>*/}
                        {/*        <li><a href={'/about'}>About</a></li>*/}
                        {/*    </ul>*/}
                        {/*</nav>*/}
                    </header>
                </div>
                {children}
            </body>
        </html>
    );
}

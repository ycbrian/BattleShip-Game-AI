#pragma once

#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#include <BattleShipGame/Board.h>
#include <BattleShipGame/Ship.h>

namespace TA
{
    class GUIInterface
    {
    public:
        virtual void title() = 0;
        virtual void appendText(std::string str) = 0;
        virtual void updateGame(Board p1b, std::vector<Ship> p1s, Board p2b, std::vector<Ship> p2s) = 0;
    };

    using std::printf;
    #define ESC "\033"
    class ASCII: public GUIInterface
    {
        const static int GRAPH_HIGHT = 6+22;
        const static int GRAPH_WIDTH = 80;
        
        const static int TEXT_HIGHT = 10;
        const static int TEXT_WIDTH = 80;

        std::string m_preparedText;
        std::string m_textbuf;

        void cls()
        {
            printf( ESC "[H" ESC "[J" );
        }

        void gotoxy(int y, int x)
        {
            printf( ESC "\033[%d;%df", y, x);
        }

        void updateTextBuf()
        {
            std::stringstream ss(m_textbuf);
            const std::string ban(TEXT_WIDTH, '-');
            std::string tmp, last;

            m_preparedText.clear();
            for(int L=0 ; L < TEXT_HIGHT - 2; ++L)
            {
                if( last == "" )
                    getline(ss, last);

                tmp = last.substr(0, std::min((size_t)TEXT_WIDTH, last.size()) );
    
                if( tmp.size() == last.size() )
                    last = "";
                else 
                    last = last.substr(TEXT_WIDTH);
    
                m_preparedText = tmp + "\n" + m_preparedText;
            }
            m_textbuf = m_textbuf.substr(0, TEXT_HIGHT * TEXT_WIDTH);
            m_preparedText =  ban + "\n" + m_preparedText + ban;
        }

        void showText()
        {
            gotoxy(GRAPH_HIGHT+1, 0);
            printf( ESC "[J" );
            gotoxy(GRAPH_HIGHT+1, 0);
            puts(m_preparedText.c_str());
            gotoxy(GRAPH_HIGHT+TEXT_HIGHT+1, 0);
            std::fflush(stdout);
        }

    public:
        virtual void title() override
        {
            cls();
            puts(R"(    ____        __  __  __    _____ __    _     )");
            puts(R"(   / __ )____ _/ /_/ /_/ /__ / ___// /_  (_)___ )");
            puts(R"(  / __  / __ `/ __/ __/ / _ \\__ \/ __ \/ / __ \)");
            puts(R"( / /_/ / /_/ / /_/ /_/ /  __/__/ / / / / / /_/ /)");
            puts(R"(/_____/\__,_/\__/\__/_/\___/____/_/ /_/_/ .___/ )");
            puts(R"(                                       /_/      )");
        }

        virtual void appendText(std::string str)
        {
            m_textbuf = str + m_textbuf;
            updateTextBuf();
            showText();
        }

        virtual void updateGame(Board p1b, std::vector<Ship> p1s, Board p2b, std::vector<Ship> p2s)
        {
            std::string charmap[22][80];
            const int dY = 40;

            for(int i=0;i<22;++i)
            {
                for(int j=0;j<80;++j)
                {
                    charmap[i][j] = " ";
                }
                charmap[i][dY] = "|";
            }

            auto draw = [&](Board board, std::vector<Ship> ships, int dy)
            {
                const int dyBase = 10;
                const int dxBase = 1;

                for( Ship ship:ships )
                {
                    for( int i = 0 ; i < ship.size ; ++i )
                        for( int j = 0 ; j < ship.size ; ++j )
                        {
                            int x = dxBase + ship.x + i ;
                            int y = dyBase + ship.y + j + dy;

                            charmap[x][y] = "#";
                            if( ship.state == Ship::State::Hit )
                                charmap[x][y] = "@";
                            if( ship.state == Ship::State::Sink )
                                charmap[x][y] = "%";
                        }

                }
                for( int i = 0 ; i < board.size() ; ++i )
                    for( int j = 0 ; j < board.size() ; ++j )
                    {
                        int x = dxBase + i ;
                        int y = dyBase + j + dy;
                    
                        if( board[i][j] == Board::State::Hit )
                        {
                            charmap[x][y] = ESC "[1;91m" +  charmap[x][y] + ESC "[0m";
                        }
                        else if( board[i][j] == Board::State::Empty )
                        {
                            charmap[x][y] = ESC "[1;96m" "~" ESC "[0m";
                        }
                    }
            };

            draw(p1b, p1s, 0);
            draw(p2b, p2s, dY);

            gotoxy(6+1, 0);
            for(int i=0;i<22;++i)
            {
                for(int j=0;j<80;++j)
                {
                    std::printf("%s", charmap[i][j].c_str());
                }
                std::putchar('\n');
            }
            gotoxy(GRAPH_HIGHT+TEXT_HIGHT+1, 0);
        }
    };
    #undef ESC
}

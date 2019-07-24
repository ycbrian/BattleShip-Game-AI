#pragma once

#include <BattleShipGame/Board.h>
#include <BattleShipGame/Wrapper/AI.h>
#include <GUI/GUIInterface.h>

#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <future>
#include <type_traits>

namespace TA
{
class BattleShipGame
{
public:
    BattleShipGame(
        int size,
        std::chrono::milliseconds runtime_limit = std::chrono::milliseconds( 1000 )
    ):
        m_size( size ),
        m_runtime_limit( runtime_limit ),
        m_P1( nullptr ),
        m_P2( nullptr ),
        m_P1Board( size ),
        m_P2Board( size )
    {
        gui = new ASCII;
        m_ship_size = {3, 3, 5, 7};
    }

    void setPlayer1( AIInterface* ptr )
    {
        assert( checkAI( ptr ) );
        m_P1 = ptr;
    }
    void setPlayer2( AIInterface* ptr )
    {
        assert( checkAI( ptr ) );
        m_P2 = ptr;
    }
    std::vector<Ship>& get_ship( int idx )
    {
        if ( idx == 1 )
        {
            return m_P1Ship;
        }
        return m_P2Ship;
    }
    AIInterface* get_ai( int idx )const
    {
        if ( idx == 1 )
        {
            return m_P1;
        }
        return m_P2;
    }
    Board& get_board( int idx )
    {
        if ( idx == 1 )
        {
            return m_P1Board;
        }
        return m_P2Board;
    }

    bool ship_attack( int attacker_index, std::vector<std::pair<int, int>>& attacker_record,
                      std::vector<std::pair<int, int>>& defender_record )
    {
        bool over = false;
        int defender_index = ( attacker_index == 1 ? 2 : 1 );
        auto& attacker_ships = get_ship( attacker_index );
        AIInterface* attacker_ai = get_ai( attacker_index );


        auto& defender_ships = get_ship( defender_index );
        auto& defender_board = get_board( defender_index );
        attacker_ai->callbackReportEnemy( defender_record );
        defender_record.clear();

        std::string win_message = ( attacker_index == 1 ? "P2 win!\n" : "P1 win!\n" );
        for ( auto ship : attacker_ships )
        {
            if ( ship.state != Ship::State::Sink )
            {
                auto index = attacker_ai->queryWhereToHit(
                                 defender_board ); //auto index=call(&AIInterface::queryWhereToHit, m_P1,m_P2Board)
                int x = index.first, y = index.second;
                if ( x < 0 || x >= m_size || y < 0 || y >= m_size )
                {
                    putToGui( win_message.c_str() );
                    over = true;
                    break;
                }
                if ( defender_board[x][y] != Board::State::Unknown )
                {
                    putToGui( win_message.c_str() );
                    over = true;
                    break;
                }
                //judge

                bool no_hit = true;
                for ( auto& defender_ship : defender_ships )
                {

                    if (
                        x >= defender_ship.x && x <= defender_ship.x + defender_ship.size - 1 &&
                        y >= defender_ship.y && y <= defender_ship.y + defender_ship.size - 1
                    )
                    {
                        if ( x == defender_ship.x + defender_ship.size / 2 &&
                                y == defender_ship.y + defender_ship.size / 2 )
                        {
                            defender_ship.state = Ship::State::Sink;
                        }
                        else if ( defender_ship.state != Ship::State::Sink )
                        {
                            defender_ship.state = Ship::State::Hit;
                        }
                        no_hit = false;
                        defender_board[x][y] = Board::State::Hit;
                        attacker_ai->callbackReportHit( true );
                    }//judge ship core
                }
                if ( no_hit )
                {
                    defender_board[x][y] = Board::State::Empty;
                    attacker_ai->callbackReportHit( false );
                }// change board state
                over = checkGameover();
                if ( over )
                {
                    break;
                }
                attacker_record.push_back( index );
            }
        }
        if ( !over )
        {
            over = ship_move( attacker_ai, attacker_ships,
                              attacker_index );
        }
        return over;
    }
    bool ship_move_check( int move_x, int move_y, int ship_size,
                          bool move_horizon, int attacker_index )
    {
        bool over = false;
        auto& attacker_board = get_board( attacker_index );
        for ( int i = 0; i < ship_size; i++ )
        {
            int x, y;
            if ( move_horizon )
            {
                x = move_x + i;
                y = move_y;
            }
            else
            {
                x = move_x;
                y = move_y + i;
            }
            if ( attacker_board[x][y] != Board::State::Unknown )
            {
                over = true;
                break;
            }
        }
        return over;
    }
    bool ship_move( AIInterface* attacker_ai, std::vector<Ship>& attacker_ships,
                    int attacker_index )
    {
        bool over = false;
        std::vector<std::pair<int, int>> moving_position;
        std::string win_message = ( attacker_index == 1 ? "P2 win!\n" : "P1 win!\n" );

        moving_position = attacker_ai->queryHowToMoveShip( attacker_ships );
        for ( int i = 0; i < 4; i++ )
        {
            bool move_horizon = true;
            int move_x = moving_position[i].first;
            int move_y = moving_position[i].second;
            int origin_x = attacker_ships[i].x;
            int origin_y = attacker_ships[i].y;
            int ship_size = attacker_ships[i].size;
            if ( attacker_ships[i].state != Ship::State::Available &&
                    origin_x != move_x &&
                    origin_y != move_y )
            {
                putToGui( win_message.c_str() );
                over = true;
                break;
            }//check no ship moved without available
            if ( origin_x != move_x &&
                    origin_y != move_y )
            {
                putToGui( win_message.c_str() );
                over = true;
                break;
            }// check no moving with both x & y
            if ( origin_x == move_x &&
                    origin_y == move_y )
            {
                continue;
            }//check no move than continue
            if ( move_x - origin_x > 1 ||
                    move_x - origin_x < -1 ||
                    move_y - origin_y > 1 ||
                    move_y - origin_y < -1 )
            {
                putToGui( win_message.c_str() );
                over = true;
                break;
            }//check no move more than 1 block;
            if ( move_x < 0 || move_x < 0 ||
                    move_x + ship_size - 1 >= m_size ||
                    move_y + ship_size - 1 >= m_size )
            {
                putToGui( win_message.c_str() );
                over = true;
                break;
            }//check no move outside board
            if ( move_x == origin_x )
            {
                over =  ship_move_check( move_x, move_y, ship_size,
                                         move_horizon, attacker_index );
                if ( !over )
                {
                    attacker_ships[i].y = move_y;
                }
            }// move right or left;
            if ( move_y == origin_y )
            {
                move_horizon = false;
                over = ship_move_check( move_x, move_y, ship_size,
                                        move_horizon, attacker_index );
                if ( !over )
                {
                    attacker_ships[i].x = move_x;
                }
            }// move up or down;
            if ( !checkShipPosition( attacker_ships ) )
            {
                putToGui( win_message.c_str() );
                over = true;
            }

        }
        return over;
    }
    void run()
    {
        gui->title();

        if ( !prepareState() ) { return ; }


        updateGuiGame();
        bool over = false;
        std::vector<std::pair<int, int>> p1_record;
        std::vector<std::pair<int, int>> p2_record;

        while ( 1 )
        {
            over = ship_attack( 1, p1_record, p2_record );
            updateGuiGame();
            if ( over )
            {
                break;
            }
            over = ship_attack( 2, p2_record, p1_record );
            updateGuiGame();
            if ( over )
            {
                break;
            }
        }
        //Todo: Play Game
    }

private:
    void updateGuiGame()
    {
        gui->updateGame( m_P1Board, m_P1Ship, m_P2Board, m_P2Ship );
    }
    bool checkGameover()
    {
        bool flag = true;

        for ( Ship s : m_P1Ship )
        {
            if ( s.state != Ship::State::Sink )
            {
                flag = false;
            }
        }
        if ( flag )
        {
            putToGui( "P2 win!\n" );
            return true;
        }
        for ( Ship s : m_P2Ship )
        {
            if ( s.state != Ship::State::Sink )
            {
                return false;
            }
        }
        putToGui( "P1 win!\n" );
        return true;
    }

    bool prepareState()
    {
        std::vector<Ship> initPos;

        putToGui( "P1 Prepareing...\n" );
        initPos = call( &AIInterface::init, m_P1, m_size, m_ship_size, true, m_runtime_limit );
        if ( !checkShipPosition( initPos ) )
        {
            putToGui( "P1 Init() Invaild...\n" );
            return false;
        }
        for ( auto& ship : initPos )
        {
            ship.state = Ship::State::Available;
        }
        m_P1Ship = initPos;
        putToGui( "Done.\n" );

        initPos.clear();

        putToGui( "P2 Prepareing...\n" );
        initPos = call( &AIInterface::init, m_P2, m_size, m_ship_size, false, m_runtime_limit );
        if ( !checkShipPosition( initPos ) )
        {
            putToGui( "P2 Init() Invaild...\n" );
            return false;
        }
        for ( auto& ship : initPos )
        {
            ship.state = Ship::State::Available;
        }
        m_P2Ship = initPos;
        putToGui( "Done.\n" );
        return true;
    }

    template<typename Func , typename... Args,
             std::enable_if_t< std::is_void<
                                   std::invoke_result_t<Func, AIInterface, Args...>
                                   >::value , int> = 0 >
    void call( Func func, AIInterface* ptr, Args... args )
    {
        std::future_status status;
        auto val = std::async( std::launch::async, func, ptr, args... );
        status = val.wait_for( std::chrono::milliseconds( m_runtime_limit ) );

        if ( status != std::future_status::ready )
        {
            exit( -1 );
        }
        val.get();
    }

    template<typename Func , typename... Args,
             std::enable_if_t< std::is_void<
                                   std::invoke_result_t<Func, AIInterface, Args...>
                                   >::value == false, int> = 0 >
    auto call( Func func, AIInterface* ptr, Args... args )
    -> std::invoke_result_t<Func, AIInterface, Args...>
    {
        std::future_status status;
        auto val = std::async( std::launch::async, func, ptr, args... );
        status = val.wait_for( std::chrono::milliseconds( m_runtime_limit ) );

        if ( status != std::future_status::ready )
        {
            exit( -1 );
        }
        return val.get();
    }

    void putToGui( const char* fmt, ... )
    {
        va_list args1;
        va_start( args1, fmt );
        va_list args2;
        va_copy( args2, args1 );
        std::vector<char> buf( 1 + std::vsnprintf( nullptr, 0, fmt, args1 ) );
        va_end( args1 );
        std::vsnprintf( buf.data(), buf.size(), fmt, args2 );
        va_end( args2 );

        if ( buf.back() == 0 ) { buf.pop_back(); }
        gui->appendText( std::string( buf.begin(), buf.end() ) );
    }

    bool checkAI( AIInterface* ptr )
    {
        return ptr->abi() == AI_ABI_VER;
    }

    bool checkShipPosition( std::vector<Ship> ships )
    {

        if ( ships.size() != m_ship_size.size() )
        {
            putToGui( "Ship number not match : real %zu ,expect %zu,\n", ships.size(), m_ship_size.size() );
            return false;
        }

        std::sort( ships.begin(), ships.end(), []( Ship a, Ship b )
        {
            return a.size < b.size;
        } );

        std::vector<std::vector<int>> map( m_size, std::vector<int>( m_size ) );

        int id = -1;
        for ( auto [size, x, y, state] : ships )
        {
            id++;
            if ( size != m_ship_size[id] )
            {
                putToGui( "Ship %d size not match : real %zu ,expect %zu,\n", id, size, m_ship_size[id] );
                return false;
            }

            for ( int dx = 0 ; dx < size ; dx++ )
                for ( int dy = 0 ; dy < size ; dy++ )
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    if ( nx < 0 || nx >= m_size || ny < 0 || ny >= m_size )
                    {
                        putToGui( "Ship %d out of range at (%d,%d),\n", id, nx, ny );
                        return false;
                    }

                    if ( map[nx][ny] != 0 )
                    {
                        putToGui( "Ship %d and %d overlapping at (%d,%d),\n", id,  map[nx][ny] - 1, nx, ny );
                        return false;
                    }
                    map[nx][ny] = id + 1;
                }
        }
        return true;
    }

    int m_size;
    std::vector<int> m_ship_size;
    std::chrono::milliseconds m_runtime_limit;

    AIInterface* m_P1;
    AIInterface* m_P2;
    GUIInterface* gui;

    std::vector<Ship> m_P1Ship;
    std::vector<Ship> m_P2Ship;
    Board m_P1Board;
    Board m_P2Board;
} ;
}

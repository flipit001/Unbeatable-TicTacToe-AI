#include <iostream>
#include <vector>
#include <array>

typedef unsigned int BB;
typedef unsigned int Move;

inline const int NULL_MOVE = 9;
inline int INFINITY = 11;
inline char SYMBOLS[2] = {'O', 'X'};
inline BB MOVE_TO_BB[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256}; // MOVE_TO_BB[x] = pow(2, x) or 1 << x

constexpr BB zeroes_start(const BB &board) {
    return __builtin_ctzll(board);
}

constexpr BB pop_first_one(BB &board) {
    BB first = zeroes_start(board);
    board &= board - 1;
    return first;
}

void print_board(const BB &board) {
    int cur;
    for (int i = 8; i > -1; i--) {
        cur = 1u << i;
        if ((i+1) % 3 == 0) {
            std::cout << std::endl;
        }
        if (board & cur) {
            std::cout << "1 ";
        }
        else {
            std::cout << "0 ";
        }
    }
    std::cout << std::endl;
}


enum State {
    RUNNING,
    DRAW,
    WIN
};

struct tic_tac_toe_board {
    BB full;
    BB empties;
    BB pieces[2];
    bool turn;
    State state;

    tic_tac_toe_board() {
        this->full = 0;
        this->empties = 0b111111111;
        this->pieces[0] = 0;
        this->pieces[1] = 0;
        this->turn = 1; // Os = 0; Xs = 1
        this->state = RUNNING;
    }
};

void print_tic_tac_toe(const tic_tac_toe_board &board) {
    int cur;
    for (int i = 8; i > -1; i--) {
        cur = 1u << i;
        if ((i+1) % 3 == 0) {
            std::cout << std::endl;
        }

        if (board.empties & cur) {
            std::cout << "0 ";
        }

        else if (board.pieces[0] & cur) {
            std::cout << "O ";
        }

        else {
            std::cout << "X ";
        }

    }
    std::cout << std::endl;
}

void update_state(tic_tac_toe_board &board) {
    BB other_team_pieces = board.pieces[board.turn ^ 1];
    if ((other_team_pieces & 0b111) == 0b111 || (other_team_pieces & 0b111000) == 0b111000 || (other_team_pieces & 0b111000000) == 0b111000000 || (other_team_pieces & 0b100010001) == 0b100010001 || (other_team_pieces & 0b001010100) == 0b001010100 || (other_team_pieces & 0b100100100) == 0b100100100 || (other_team_pieces & 0b010010010) == 0b010010010 || (other_team_pieces & 0b001001001) == 0b001001001) {
        board.state = WIN;
    }

    else if (board.full == 0b111111111) {
        board.state = DRAW;
    }
}

void play_move(const Move &move, tic_tac_toe_board &board) {
    board.full ^= MOVE_TO_BB[move];
    board.empties ^= MOVE_TO_BB[move];
    board.pieces[board.turn] ^= MOVE_TO_BB[move];
    board.turn ^= 1; // next turn
    update_state(board);
}

void undo_move(const Move &move, tic_tac_toe_board &board) {
    board.state = RUNNING;
    board.turn ^= 1; // previous turn
    board.full ^= MOVE_TO_BB[move]; // xors are able to be undoed by just doing the operation again
    board.empties ^= MOVE_TO_BB[move];
    board.pieces[board.turn] ^= MOVE_TO_BB[move];
}


void generate_moves(const tic_tac_toe_board &board, std::vector<Move> &moves) {
    if (board.state != RUNNING) {
        return;
    }
    // std::cout << "got here" << std::endl;
    BB possible_moves = board.empties;
    // print_board(possible_moves);
    int pos;
    while (possible_moves) {
        pos = pop_first_one(possible_moves);
        // std::cout << pos << std::endl;
        moves.emplace_back(pos);
    }
}

inline Move best_move = NULL_MOVE;
inline int best_eval = -INFINITY;

int search(tic_tac_toe_board &board, const int &depth_from_start=0) {
    if (board.state == DRAW) {
        return 0;
    }
    if (board.state == WIN) {
        return -INFINITY + depth_from_start;
    }
    int eval = -INFINITY;
    std::vector<Move> moves;
    generate_moves(board, moves);
    for (Move move : moves) {
        // if (depth_from_start == 0) {
        //     std::cout << move << std::endl;
        // }
        play_move(move, board);
        eval = std::max(eval, -search(board, depth_from_start+1));
        undo_move(move, board);
        if (eval > best_eval && depth_from_start == 0) {
            best_eval = eval;
            best_move = move;
        }
    }
    return eval;
}

int main() {

    tic_tac_toe_board board = tic_tac_toe_board();
    // bool running = true;
    bool said_statement, bad_move, bad_move_before = false;
    std::cout << "topleft is 9, bottom right is 1" << std::endl;
    char input;
    int cur_move;
    while (true) {
        if (board.state == DRAW) {
            std::cout << "good job, that was a draw" << std::endl;
            break;
        }
        if (board.state == WIN) {
            std::cout << "You lost, I didn't program a state where you win because I know you can't :)" << std::endl;
            break;
        }
        if (board.turn) {
            best_move = NULL_MOVE;
            best_eval = -INFINITY;
            search(board);
            play_move(best_move, board);
            print_tic_tac_toe(board);
            if (best_eval != 0 && !said_statement) {
                std::cout << "you already lost :)" << std::endl;
                said_statement = true;
            }
        }
        else {
            std::cout << "Your turn" << std::endl;
            std::cin >> input;
            cur_move = input - 49; // ex. '1' - 49 == 0, '2' - 49 == 1
            if (!(board.empties & MOVE_TO_BB[cur_move])) {
                if (!bad_move_before) {
                    std::cout << "something is already there" << std::endl;
                }
                else {
                    std::cout << "stop" << std::endl;
                }
                bad_move = true;
                bad_move_before |= bad_move;
            }
            if (cur_move <= -1 || cur_move >= 9) {
                if (!bad_move_before) {
                    std::cout << "you can not play that move" << std::endl;
                }
                else {
                    std::cout << "stop" << std::endl;
                }
                bad_move = true;
                bad_move_before |= bad_move;
            }
            if (!bad_move) {
                play_move(cur_move, board);
            }
            bad_move = false;
        }
    }
    // print_board(board.full);

    // std::array<Move, 9> moves;
    // generate_moves(board, moves);
    // std::cout << moves[0] << std::endl;

    return 0;
}
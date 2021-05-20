/*
 * TODO: Add check/checkmate/stalemate conditions.
 * TODO: Add castling.
 * TODO: Add promotions.
 * TODO: Add en passant
 */
#include "game.h"

uint64_t
mvPawn(const uint8_t *board, uint8_t i)
{
	uint8_t row = i >> 3, column = i & 7,
		white = board[i] == W_PAWN;
	int8_t offsetRow = (white << 4) - 8;
	uint64_t out = 0;

	if (row == 7 || row == 0)
		return 0;
	if (!board[i + offsetRow]) {
		out |= (uint64_t)1 << (i + offsetRow);
		if (((white && row == 1) || (!white && row == 6)) &&
			!board[i + 2 * offsetRow])
			out |= (uint64_t)1 << (i + 2 * offsetRow);
	}
	if (column && board[i + offsetRow - 1] &&
			(board[i + offsetRow - 1] & 1) ^ white)
		out |= (uint64_t)1 << (i + offsetRow - 1);
	if (column != 7 && board[i + offsetRow + 1] &&
			(board[i + offsetRow + 1] & 1) ^ white)
		out |= (uint64_t)1 << (i + offsetRow + 1);
	return out;
}

uint64_t
mvKnight(const uint8_t *board, uint8_t i)
{
	uint8_t row = i >> 3, column = i & 7,
		white = board[i] == W_KNIGHT,
		space[2] = { 7 - column, 7 - row };
	uint64_t out = 0;

	if (space[0] >= 2 && space[1] >= 1 &&
		(!board[i + 10] || ((board[i + 10] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 10);
	if (space[0] >= 1 && space[1] >= 2 &&
		(!board[i + 17] || ((board[i + 17] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 17);
	if (space[1] >= 2 && column >= 1 &&
		(!board[i + 15] || ((board[i + 15] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 15);
	if (space[1] >= 1 && column >= 2 &&
		(!board[i + 6] || ((board[i + 6] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 6);
	if (column >= 2 && row >= 1 &&
		(!board[i - 10] || ((board[i - 10] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 10);
	if (column >= 1 && row >= 2 &&
		(!board[i - 17] || ((board[i - 17] & 1) ^ white)))
		out |= (uint64_t)1 << (i -17);
	if (row >= 2 && space[0] >= 1 &&
		(!board[i - 15] || ((board[i - 15] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 15);
	if (row >= 1 && space[0] >= 2 &&
		(!board[i - 6] || ((board[i - 6] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 6);
	return out;
}

uint64_t
mvBishop(const uint8_t *board, uint8_t i)
{
	int8_t n;
	uint8_t white = board[i] & 1;
	uint64_t out = 0;

	/* Northeast */
	for (n = i + 9; (n & 7) && n < 64 && !board[n]; n += 9)
		out |= (uint64_t)1 << n;
	if ((n & 7) && n < 64 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* Southwest */
	for (n = i - 9; (n & 7) != 7 && n >= 0 && !board[n]; n -= 9)
		out |= (uint64_t)1 << n;
	if ((n & 7) != 7 && n >= 0 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* Northwest */
	for (n = i + 7; n < 64 && (n & 7) != 7 && !board[n]; n += 7)
		out |= (uint64_t)1 << n;
	if (n < 64 && (n & 7) != 7 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* Southeast */
	for (n = i - 7; n >= 0 && (n & 7) && !board[n]; n -= 7)
		out |= (uint64_t)1 << n;
	if (n >= 0 && (n & 7) && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	return out;
}

uint64_t
mvRook(const uint8_t *board, uint8_t i)
{
	int8_t n;
	uint8_t white = board[i] & 1;
	uint64_t out = 0;

	/* East */
	for (n = i + 1; (n & 7) && !board[n]; ++n)
		out |= (uint64_t)1 << n;
	if ((n & 7) && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* West */
	for (n = i - 1; (n & 7) != 7 && !board[n]; --n)
		out |= (uint64_t)1 << n;
	if ((n & 7) != 7 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* North */
	for (n = i + 8; n < 64 && !board[n]; n += 8)
		out |= (uint64_t)1 << n;
	if (n < 64 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	/* South */
	for (n = i - 8; n >= 0 && !board[n]; n -= 8)
		out |= (uint64_t)1 << n;
	if (n >= 0 && ((board[n] & 1) ^ white))
		out |= (uint64_t)1 << n;
	return out;
}

inline uint64_t
mvQueen(const uint8_t *board, uint8_t i)
{
	return mvBishop(board, i) | mvRook(board, i);
}

uint64_t
mvKing(const uint8_t *board, uint8_t i)
{
	uint8_t row = i >> 3, column = i & 7, white = board[i] == W_KING;
	uint64_t out = 0;

	if (column != 7 &&
	(!board[i + 1] || ((board[i + 1] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 1);
	if (column != 7 && row < 6 &&
	(!board[i + 9] || ((board[i + 9] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 9);
	if (row < 6 &&
	(!board[i + 8] || ((board[i + 8] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 8);
	if (column && row < 6 &&
	(!board[i + 7] || ((board[i + 7] & 1) ^ white)))
		out |= (uint64_t)1 << (i + 7);
	if (column &&
	(!board[i - 1] || ((board[i - 1] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 1);
	if (column && row &&
	(!board[i - 9] || ((board[i - 9] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 9);
	if (row &&
	(!board[i - 8] || ((board[i - 8] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 8);
	if ((i & 7) != 7 && row &&
	(!board[i - 7] || ((board[i - 7] & 1) ^ white)))
		out |= (uint64_t)1 << (i - 7);
	return out;
}

uint64_t
getMoves(const game_t *game, uint8_t i)
{
	if (!game->board[i] || (game->board[i] & 1) == game->state)
		return 0;
	switch (game->board[i]) {
	case W_PAWN:
	case B_PAWN:
		return mvPawn(game->board, i);
	case W_KNIGHT:
	case B_KNIGHT:
		return mvKnight(game->board, i);
	case W_BISHOP:
	case B_BISHOP:
		return mvBishop(game->board, i);
	case W_ROOK:
	case B_ROOK:
		return mvRook(game->board, i);
	case W_QUEEN:
	case B_QUEEN:
		return mvQueen(game->board, i);
	case W_KING:
	case B_KING:
		return mvKing(game->board, i);
	}
	return 0;
}

int8_t
getTile(int ix, int iy)
{
	float j = (float)((SCREEN_H >> 1) - iy) / (TILE_H + 1);
	float i = (float)(ix - (ORIGIN_X)) / TILE_W;
	return ((int8_t)(i + j) << 3) + (i - j);
}

inline uint8_t
validMove(uint64_t bitBoard, uint8_t i)
{
	return (bitBoard >> i) & 1;
}

void
move(game_t *game, uint8_t prevI, uint8_t i)
{
	game->board[i] = game->board[prevI];
	game->board[prevI] = EMPTY;
	game->state = !game->state;
}
 /*
enum directions { NONE, E, NE, N, NW };

uint8_t
atkKnight(uint8_t i, uint8_t *out)
{
	uint8_t move, row = i >> 3, column = i & 7;

	if (row) {
		out[row - 1] |= column > 1 ? 0b10001 << (column - 2) : 0b100 << column;
		move |= 1;
	}
	if (row > 1) {
		out[row - 2] |= column ? 0b101 << (column - 1) : 0b10;
		move |= 1;
	}
	if (row < 7) {
		out[row + 1] |= column > 1 ? 0b10001 << (column - 2) : 0b100 << column;
		move |= 1;
	}
	if (row < 6) {
		out[row + 2] |= column ? 0b101 << (column - 1) : 0b10;
		move |= 1;
	}
	return move;
}

uint8_t
atkPawn(uint8_t white, uint8_t i, uint8_t *out)
{
	uint8_t column = i & 7;

	return (out[(i >> 3) + 2 * white - 1] |= column ? 0b101 << (column - 1) : 0b10) != 0;
}

uint16_t
detectPin(const uint8_t *board, uint8_t i, uint8_t j)
{
	uint8_t n, dir, white = board[j] & 1;
	int8_t diff;

	if ((i >> 3) == (j >> 3)) {
		diff = 2 * (i > j) - 1;
		dir = E;
	} else if ((i & 7) - (j & 7) == (i >> 3) - (j >> 3)) {
		diff = 18 * (i > j) - 9;
		dir = NE;
	} else if ((i & 7) == (j & 7)) {
		diff = 16 * (i > j) - 8;
		dir = N;
	} else if ((i & 7) - (j & 7) == (j >> 3) - (i >> 3)) {
		diff = 14 * (i > j) - 7;
		dir = NW;
	} else {
		return NONE;
	}
	for (n = j + diff; !board[n]; n += diff);
	if (n != i)
		return NONE;
	for (n += diff; !board[n] && (n >> 3) && (n >> 3) < 7 && (n & 7) && (n & 7) < 7;
			n += diff);
	return ((board[n] == W_ROOK + white || board[n] == W_QUEEN + white)
			* dir << 8) | n;
}

inline uint8_t
pinDir(const uint8_t *board, uint8_t i, uint8_t j)
{
	return detectPin(board, i, j) >> 8;
}

inline uint8_t
pinSquare(const uint8_t *board, uint8_t i, uint8_t j)
{
	return detectPin(board, i, j) & 0xFF;
}

uint8_t
blockMvs(const game_t *game, uint8_t i, uint8_t *out)
{
	uint8_t diff, j, n, tmp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	tmp[i >> 3] = 1 << (i & 7);
	switch (game->board[i]) {
	case W_BISHOP:
	case B_BISHOP:
	case W_ROOK:
	case B_ROOK:
	case W_QUEEN:
	case B_QUEEN:
		j = game->board[i] & 1 ? game->kings >> 8 : game->kings & 0xFF;
		if ((i >> 3) == (j >> 3))
			diff = 2 * (i > j) - 1;
		else if ((i & 7) - (j & 7) == (i >> 3) - (j >> 3))
			diff = 18 * (i > j) - 9;
		else if ((i & 7) == (j & 7))
			diff = 16 * (i > j) - 8;
		else if ((i & 7) - (j & 7) == (j >> 3) - (i >> 3))
			diff = 14 * (i > j) - 7;
		for (n = j + diff; !game->board[n]; n += diff)
			tmp[n >> 3] |= 1 << (n & 7);
	}
	j = 0;
	for (n = 0; n < 8; ++n) {
		out[n] &= tmp[n];
		if (out[n])
			++j;
	}
	return j;
}
*/

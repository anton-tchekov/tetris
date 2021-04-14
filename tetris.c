#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#define FIELD_WIDTH         10
#define FIELD_HEIGHT        20
#define BLOCK_SIZE          20

#define FALL_SPEED_DEFAULT 200
#define FALL_SPEED_FAST     30

#define WINDOW_WIDTH          (FIELD_WIDTH * BLOCK_SIZE + 6 * BLOCK_SIZE)
#define WINDOW_HEIGHT         (FIELD_HEIGHT * BLOCK_SIZE)

#define WINDOW_TITLE          "Tetris"

typedef enum
{
	RUNNING  = 1
} Status;

typedef enum { I, J, L, O, S, T, Z } PieceType;

typedef struct
{
	int X, Y, Rotation;
	PieceType Type;
} Piece;

struct
{
	uint16_t Blocks[4];
	struct { uint8_t R, G, B; } Color;
}
const Pieces[] =
{
	{
		/* I */
		{ 0x0F00, 0x2222, 0x00F0, 0x4444 },
		{ 0x00, 0xFF, 0xFF } /* Cyan */
	},
	{
		/* J */
		{ 0x44C0, 0x8E00, 0x6440, 0x0E20 },
		{ 0x00, 0x00, 0xFF } /* Blue */
	},
	{
		/* L */
		{ 0x4460, 0x0E80, 0xC440, 0x2E00 },
		{ 0xFF, 0x7F, 0x00 } /* Orange */
	},
	{
		/* O */
		{ 0xCC00, 0xCC00, 0xCC00, 0xCC00 },
		{ 0xFF, 0xFF, 0x00 } /* Yellow */
	},
	{
		/* S */
		{ 0x06C0, 0x8C40, 0x6C00, 0x4620 },
		{ 0x00, 0xFF, 0x00 } /* Green */
	},
	{
		/* T */
		{ 0x0E40, 0x4C40, 0x4E00, 0x4640 },
		{ 0xFF, 0x00, 0xFF } /* Purple */
	},
	{
		/* Z */
		{ 0x0C60, 0x4C80, 0xC600, 0x2640 },
		{ 0xFF, 0x00, 0x00 } /* Red */
	}
};

static int _ctor(SDL_Window **window, SDL_Renderer **renderer);
static void _dtor(SDL_Window *window, SDL_Renderer *renderer);

static void _draw_field(SDL_Renderer *renderer, int *field);
static void _clear_field(int *field);
static int _field_get(int *field, int x, int y);
static void _field_rows(int *field);
static void _field_shift(int *field, int row);

static void _draw_grid(SDL_Renderer *renderer);

static int _new_piece(Piece *p);
static void _draw_piece(SDL_Renderer *renderer, Piece *p);
static int _valid_position(int *field, Piece *p);
static void _to_field(int *field, Piece *p);

int main(void)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Event event;

	/* Current Piece, Next Piece */
	Piece cp, np;
	Status status;
	int ticks, last_ticks = 0, ticks_update = 200, score = 0;
	int field[FIELD_WIDTH * FIELD_HEIGHT];

	np.X = 11;
	np.Y = 1;
	np.Rotation = 1;

	if(_ctor(&window, &renderer))
	{
		return 1;
	}

	_clear_field(field);
	np.Type = _new_piece(&cp);
	status = RUNNING;
	while(status & RUNNING)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					status &= ~RUNNING;
					break;

				case SDLK_DOWN:
					ticks_update = FALL_SPEED_FAST;
					break;

				case SDLK_UP:
					if(--cp.Rotation == -1)
					{
						cp.Rotation = 3;
					}

					if(_valid_position(field, &cp))
					{
						if(++cp.Rotation == 4)
						{
							cp.Rotation = 0;
						}
					}
					break;

				case SDLK_LEFT:
					--cp.X;
					if(_valid_position(field, &cp))
					{
						++cp.X;
					}
					break;

				case SDLK_RIGHT:
					++cp.X;
					if(_valid_position(field, &cp))
					{
						--cp.X;
					}
					break;

				default:
					break;
				}
				break;

			case SDL_KEYUP:
				switch(event.key.keysym.sym)
				{
				case SDLK_DOWN:
					ticks_update = FALL_SPEED_DEFAULT;
					break;

				default:
					break;
				}
				break;

			case SDL_QUIT:
				status &= ~RUNNING;
				break;
			}
		}

		/* White clear */
		SDL_SetRenderDrawColor(renderer,
			255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		_draw_piece(renderer, &np);
		_draw_piece(renderer, &cp);
		_draw_field(renderer, field);
		_draw_grid(renderer);
		_draw

		if((ticks = SDL_GetTicks()) > last_ticks + ticks_update)
		{
			last_ticks = ticks;
			++cp.Y;
			if(_valid_position(field, &cp))
			{
				--cp.Y;
				_to_field(field, &cp);
				score += _field_rows(field);
				np.Type = _new_piece(&cp);
				ticks_update = FALL_SPEED_DEFAULT;
				if(_valid_position(field, &cp))
				{
					_clear_field(field);
				}
			}
		}

		SDL_RenderPresent(renderer);
	}

	_dtor(window, renderer);
	return 0;
}

static void _draw_field(SDL_Renderer *renderer, int *field)
{
	int x, y, v;
	for(y = 0; y < FIELD_HEIGHT; ++y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			if((v = field[y * FIELD_WIDTH + x]))
			{
				--v;
				SDL_SetRenderDrawColor(renderer,
					Pieces[v].Color.R,
					Pieces[v].Color.G,
					Pieces[v].Color.B, SDL_ALPHA_OPAQUE);

				SDL_Rect rect = {
					x * BLOCK_SIZE,
					y * BLOCK_SIZE,
					BLOCK_SIZE, BLOCK_SIZE };

				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

static void _clear_field(int *field)
{
	int i;
	for(i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; ++i)
	{
		field[i] = 0;
	}
}

static int _field_get(int *field, int x, int y)
{
	return (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT) ? -1 :
		field[y * FIELD_WIDTH + x];
}

static int _field_rows(int *field)
{
	int x, y, score;
	score = 0;
	for(y = 0; y < FIELD_HEIGHT; ++y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			if(!field[y * FIELD_WIDTH + x])
			{
				break;
			}
		}

		if(x == FIELD_WIDTH)
		{
			_field_shift(field, y);
			score += 100;
		}
	}

	return score;
}

static void _field_shift(int *field, int row)
{
	int x, y;
	for(y = row; y > 0; --y)
	{
		for(x = 0; x < FIELD_WIDTH; ++x)
		{
			field[y * FIELD_WIDTH + x] =
				field[(y - 1) * FIELD_WIDTH + x];
		}
	}
}

static void _draw_grid(SDL_Renderer *renderer)
{
	int i;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	for(i = 0; i <= BLOCK_SIZE * FIELD_WIDTH; i += BLOCK_SIZE)
	{
		SDL_RenderDrawLine(renderer, i, 0, i, WINDOW_HEIGHT);
	}

	for(i = 0; i < BLOCK_SIZE * FIELD_HEIGHT; i += BLOCK_SIZE)
	{
		SDL_RenderDrawLine(renderer, 0, i, BLOCK_SIZE * FIELD_WIDTH, i);
	}
}

static int _new_piece(Piece *p)
{
	static int next;
	static int idx = 6;
	static int bag[7];

	if(idx == 6)
	{
		/* create a new shuffled bag of pieces */
		int i, j, tmp;
		for(i = 0; i < 7; ++i)
		{
			bag[i] = i;
		}

		for(i = 0; i < 7; ++i)
		{
			j = rand() % 7;
			tmp = bag[i];
			bag[i] = bag[j];
			bag[j] = tmp;
		}

		idx = 0;
	}

	p->X = 4;
	p->Y = 0;
	p->Rotation = 0;
	p->Type = next;
	next = bag[idx++];
	return next;
}

static void _draw_piece(SDL_Renderer *renderer, Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = 0;
	col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if(blocks & bit)
		{
			SDL_SetRenderDrawColor(renderer,
				Pieces[p->Type].Color.R,
				Pieces[p->Type].Color.G,
				Pieces[p->Type].Color.B, SDL_ALPHA_OPAQUE);

			SDL_Rect rect = {
				(p->X + col) * BLOCK_SIZE,
				(p->Y + row) * BLOCK_SIZE,
				BLOCK_SIZE, BLOCK_SIZE };

			SDL_RenderFillRect(renderer, &rect);
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}
}

static int _valid_position(int *field, Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = 0;
	col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if((blocks & bit) && _field_get(field, p->X + col, p->Y + row))
		{
			return 1;
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}

	return 0;
}

static void _to_field(int *field, Piece *p)
{
	int bit, row, col;
	uint16_t blocks;
	row = col = 0;
	blocks = Pieces[p->Type].Blocks[p->Rotation];
	for(bit = 0x8000; bit > 0; bit >>= 1)
	{
		if(blocks & bit)
		{
			field[(p->Y + row) * FIELD_WIDTH + (p->X + col)] =
				p->Type + 1;
		}

		if(++col == 4)
		{
			col = 0;
			++row;
		}
	}
}

static int _ctor(SDL_Window **window, SDL_Renderer **renderer)
{
	int error = 0;

	enum
	{
		ERROR_INIT_SDL        = 1,
		ERROR_CREATE_WINDOW   = 2,
		ERROR_CREATE_RENDERER = 3
	};

	do
	{
		/* Init SDL */
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Error initializing SDL; SDL_Init: %s\n",
				SDL_GetError());
			error = ERROR_INIT_SDL;
			break;
		}

		/* Create SDL_Window */
		if((*window = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT, 0)) == NULL)
		{
			printf("Error creating SDL_Window: %s\n",
				SDL_GetError());
			error = ERROR_CREATE_WINDOW;
			break;
		}

		/* Create SDL_Renderer */
		if((*renderer = SDL_CreateRenderer
			(*window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
		{
			printf("Error creating SDL_Renderer: %s\n",
				SDL_GetError());
			error = ERROR_CREATE_RENDERER;
			break;
		}
	}
	while(0);

	switch(error)
	{
		case ERROR_CREATE_RENDERER:
			SDL_DestroyWindow(*window);
			/* fall through */

		case ERROR_CREATE_WINDOW:
			SDL_Quit();
			/* fall through */

		case ERROR_INIT_SDL:
			return 1;
	}

	SDL_SetRenderDrawColor(*renderer, 255, 255, 255, 255);
	return 0;
}

static void _dtor(SDL_Window *window, SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

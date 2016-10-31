#include <stdlib.h> /* srand(), rand() */
#include <stdio.h> /* printf() */
#include <time.h> /* time() */
#ifdef DOS
#include <conio.h> /* KEY_nnn */
#include <io.h> /* read(), select(), ioctl() */
#else
#include "unix.c"
#endif

/* dimensions of playing area */
#define	SCN_WID		15
#define	SCN_HT		20
/* direction vectors */
#define	DIR_UP		{ 0, -1 }
#define	DIR_DN		{ 0, +1 }
#define	DIR_LT		{ -1, 0 }
#define	DIR_RT		{ +1, 0 }
#define	DIR_UP2		{ 0, -2 }
#define	DIR_DN2		{ 0, +2 }
#define	DIR_LT2		{ -2, 0 }
#define	DIR_RT2		{ +2, 0 }
/* ANSI colors */
#define	COLOR_BLACK	0
#define	COLOR_RED	1
#define	COLOR_GREEN	2
#define	COLOR_YELLOW	3
#define	COLOR_BLUE	4
#define	COLOR_MAGENTA	5
#define	COLOR_CYAN	6
#define	COLOR_WHITE	7

typedef struct
{
	int delta_x, delta_y;
} vector_t;

typedef struct
{
/* pointer to shape rotated +/- 90 degrees */
	unsigned plus_90, minus_90;
/* shape color */
	unsigned color;
/* drawing instructions for this shape */
	vector_t dir[4];
} shape_t;

static shape_t _shapes[] =
{
/* shape #0:			cube */
	{
		0, 0, COLOR_BLUE,
		{
			DIR_UP, DIR_RT, DIR_DN, DIR_LT
		}
	},
/* shapes #1 & #2:		bar */
	{
		2, 2, COLOR_GREEN,
		{
			DIR_LT, DIR_RT, DIR_RT, DIR_RT
		}
	},
	{
		1, 1, COLOR_GREEN,
		{
			DIR_UP, DIR_DN, DIR_DN, DIR_DN
		}
	},
/* shapes #3 & #4:		'Z' shape */
	{
		4, 4, COLOR_CYAN,
		{
			DIR_LT, DIR_RT, DIR_DN, DIR_RT
		}
	},
	{
		3, 3, COLOR_CYAN,
		{
			DIR_UP, DIR_DN, DIR_LT, DIR_DN
		}
	},
/* shapes #5 & #6:		'S' shape */
	{
		6, 6, COLOR_RED,
		{
			DIR_RT, DIR_LT, DIR_DN, DIR_LT
		}
	},
	{
		5, 5, COLOR_RED,
		{
			DIR_UP, DIR_DN, DIR_RT, DIR_DN
		}
	},
/* shapes #7, #8, #9, #10:	'J' shape */
	{
		8, 10, COLOR_MAGENTA,
		{
			DIR_RT, DIR_LT, DIR_LT, DIR_UP
		}
	},
	{
		9, 7, COLOR_MAGENTA,
		{
			DIR_UP, DIR_DN, DIR_DN, DIR_LT
		}
	},
	{
		10, 8, COLOR_MAGENTA,
		{
			DIR_LT, DIR_RT, DIR_RT, DIR_DN
		}
	},
	{
		7, 9, COLOR_MAGENTA,
		{
			DIR_DN, DIR_UP, DIR_UP, DIR_RT
		}
	},
/* shapes #11, #12, #13, #14:	'L' shape */
	{
		12, 14, COLOR_YELLOW,
		{
			DIR_RT, DIR_LT, DIR_LT, DIR_DN
		}
	},
	{
		13, 11, COLOR_YELLOW,
		{
			DIR_UP, DIR_DN, DIR_DN, DIR_RT
		}
	},
	{
		14, 12, COLOR_YELLOW,
		{
			DIR_LT, DIR_RT, DIR_RT, DIR_UP
		}
	},
	{
		11, 13, COLOR_YELLOW,
		{
			DIR_DN, DIR_UP, DIR_UP, DIR_LT
		}
	},
/* shapes #15, #16, #17, #18:	'T' shape */
	{
		16, 18, COLOR_WHITE,
		{
			DIR_UP, DIR_DN, DIR_LT, DIR_RT2
		}
	},
	{
		17, 15, COLOR_WHITE,
		{
			DIR_LT, DIR_RT, DIR_UP, DIR_DN2
		}
	},
	{
		18, 16, COLOR_WHITE,
		{
			DIR_DN, DIR_UP, DIR_RT, DIR_LT2
		}
	},
	{
		15, 17, COLOR_WHITE,
		{
			DIR_RT, DIR_LT, DIR_DN, DIR_UP2
		}
	}
};

static unsigned char _dirty[SCN_HT], _screen[SCN_WID][SCN_HT];
/*****************************************************************************
*****************************************************************************/
static void draw_block(unsigned x_pos, unsigned y_pos, unsigned color)
{
	if(x_pos >= SCN_WID)
		x_pos = SCN_WID - 1;
	if(y_pos >= SCN_HT)
		y_pos = SCN_HT - 1;
	color &= 7;

	_screen[x_pos][y_pos] = color;
	_dirty[y_pos] = 1;
}
/*****************************************************************************
*****************************************************************************/
static int detect_block_hit(unsigned x_pos, unsigned y_pos)
{
	return _screen[x_pos][y_pos];
}
/*****************************************************************************
*****************************************************************************/
static void draw_shape(unsigned x_pos, unsigned y_pos, unsigned which_shape)
{
	unsigned i;

	for(i = 0; i < 4; i++)
	{
		draw_block(x_pos, y_pos, _shapes[which_shape].color);
		x_pos += _shapes[which_shape].dir[i].delta_x;
		y_pos += _shapes[which_shape].dir[i].delta_y;
	}
	draw_block(x_pos, y_pos, _shapes[which_shape].color);
}
/*****************************************************************************
*****************************************************************************/
static void erase_shape(unsigned x_pos, unsigned y_pos, unsigned which_shape)
{
	unsigned i;

	for(i = 0; i < 4; i++)
	{
		draw_block(x_pos, y_pos, COLOR_BLACK);
		x_pos += _shapes[which_shape].dir[i].delta_x;
		y_pos += _shapes[which_shape].dir[i].delta_y;
	}
	draw_block(x_pos, y_pos, COLOR_BLACK);
}
/*****************************************************************************
*****************************************************************************/
static int detect_shape_hit(unsigned x_pos, unsigned y_pos,
		unsigned which_shape)
{
	unsigned i;

	for(i = 0; i < 4; i++)
	{
		if(detect_block_hit(x_pos, y_pos))
			return 1;
		x_pos += _shapes[which_shape].dir[i].delta_x;
		y_pos += _shapes[which_shape].dir[i].delta_y;
	}
	if(detect_block_hit(x_pos, y_pos))
		return 1;
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static void init_screen(void)
{
	unsigned x_pos, y_pos;

	for(y_pos = 0; y_pos < SCN_HT; y_pos++)
	{
/* force entire screen to be redrawn */
		_dirty[y_pos] = 1;
		for(x_pos = 1; x_pos < (SCN_WID - 1); x_pos++)
			_screen[x_pos][y_pos] = 0;
/* draw vertical edges of playing field */
		_screen[0][y_pos] = _screen[SCN_WID - 1][y_pos] = COLOR_BLUE;
	}
/* draw horizontal edges of playing field */
	for(x_pos = 0; x_pos < SCN_WID; x_pos++)
		_screen[x_pos][0] = _screen[x_pos][SCN_HT - 1] = COLOR_BLUE;
}
/*****************************************************************************
*****************************************************************************/
static void refresh(void)
{
	unsigned x_pos, y_pos;

	for(y_pos = 0; y_pos < SCN_HT; y_pos++)
	{
		if(!_dirty[y_pos])
			continue;
/* gotoxy(0, y_pos) */
		printf("\x1B[%d;1H", y_pos + 1);
		for(x_pos = 0; x_pos < SCN_WID; x_pos++)
/* 0xDB is a solid rectangular block in the PC character set */
			printf("\x1B[%dm\xDB\xDB", 30 + _screen[x_pos][y_pos]);
		_dirty[y_pos] = 0;
	}
/* reset foreground color to gray */
	printf("\x1B[37m");
	fflush(stdout);
}
/*****************************************************************************
*****************************************************************************/
static unsigned collapse(void)
{
	unsigned char solid_row[SCN_HT];
	unsigned solid_rows;
	int row, col, temp;

/* determine which rows are solidly filled */
	solid_rows = 0;
	for(row = 1; row < SCN_HT - 1; row++)
	{
		temp = 0;
		for(col = 1; col < SCN_WID - 1; col++)
		{
			if(detect_block_hit(col, row))
				temp++;
		}
		if(temp == SCN_WID - 2)
		{
			solid_row[row] = 1;
			solid_rows++;
		}
		else
			solid_row[row] = 0;
	}
	if(solid_rows == 0)
		return solid_rows;
/* collapse them */
	for(temp = row=SCN_HT - 2; row > 0; row--, temp--)
	{
/* find a solid row */
		while(solid_row[temp])
			temp--;
/* copy it */
		if(temp < 1)
		{
			for(col = 1; col < SCN_WID - 1; col++)
				_screen[col][row] = COLOR_BLACK;
		}
		else
		{
			for(col = 1; col < SCN_WID - 1; col++)
				_screen[col][row] = _screen[col][temp];
		}
		_dirty[row] = 1;
	}
	refresh();
	return solid_rows;
}
/*****************************************************************************
*****************************************************************************/
static unsigned get_key(void)
{
	static unsigned long timeout = 350000L;
/**/
	unsigned char ret_val;

/*	delay(200);
 	if(select(0, timeout) != 0)
	{
 * wait up to 200 ms for key *
		timeout = 200000L;
		ret_val = 0;
	}
	else
		(void)read(0, &ret_val, 1); */
	if (unix_kbhit_wait(timeout)) ret_val = getchar();
	return ret_val;
}
/*****************************************************************************
for MinGW32
*****************************************************************************/
#ifdef __WIN32__
int __main(void) { return 0; }
#endif
/*****************************************************************************
*****************************************************************************/
int main(void)
{
/*	unsigned char fell, new_shape, new_x, new_y;
	unsigned char shape = 0, x = 0, y = 0;
	unsigned char key; */
	unsigned fell, new_shape, new_x, new_y, key;
	unsigned shape = 0, x = 0, y = 0, lines;

/* re-seed the random number generator */
	srand((unsigned)time(NULL));
/* turn off keyboard input buffering */
	term_init();
	term_character();
	(void)ioctl(0, 0, 1);
/* banner screen */
	printf("\x1B[2J""\x1B[1;%dH""TETRIS by Alexei Pazhitnov",
		SCN_WID * 2 + 2);
	printf("\x1B[2;%dH""Software by Chris Giese", SCN_WID * 2 + 2);
	printf("\x1B[4;%dH""'n' and 'm' rotate shape", SCN_WID * 2 + 2);
	printf("\x1B[5;%dH""',' and '.' move shape", SCN_WID * 2 + 2);
	printf("\x1B[6;%dH""Esc or q quits", SCN_WID * 2 + 2);

NEW:	printf("\x1B[11;%dH""Press any key to begin", SCN_WID * 2 + 2);
	fflush(stdout);
/* await key pressed */
	(void)read(0, &key, 1);
/* erase banner */
	printf("\x1B[10;%dH""                      ", SCN_WID * 2 + 2);
	printf("\x1B[11;%dH""                      ", SCN_WID * 2 + 2);
	init_screen();
	lines = 0;
	goto FOO;

	while(1)
	{
		fell = 0;
		new_shape = shape;
		new_x = x;
		new_y = y;
		key = get_key();
		if(key == 0)
		{
			new_y++;
			fell = 1;
		}
		else
		{
			if(key == 'q' || key == 'Q' || key == 27)
				/*break;*/
				goto FIN;
			if(key == 'n')
				new_shape = _shapes[shape].plus_90;
			else if(key == 'm')
				new_shape = _shapes[shape].minus_90;
			else if(key == ',')
			{
				if(x > 0)
					new_x = x - 1;
			}
			else if(key == '.')
			{
				if(x < SCN_WID - 1)
					new_x = x + 1;
			}
			else if(key == 'x')
			{
				if(y > 0)
					new_y = y - 1; 	/* cheat */
			}
			else if(key == 'y')
			{
				if(y < SCN_HT - 1)
					new_y = y + 1;
			}
			fell = 0;
		}
/* if nothing has changed, skip the bottom half of this loop */
		if(new_x == x && new_y == y && new_shape == shape)
			continue;
/* otherwise, erase old shape from the old pos'n */
		erase_shape(x, y, shape);
/* hit anything? */
		if(detect_shape_hit(new_x, new_y, new_shape) == 0)
		{
/* no, update pos'n */
			x = new_x;
			y = new_y;
			shape = new_shape;
		}
/* yes -- did the piece hit something while falling on its own? */
		else if(fell)
		{
/* yes, draw it at the old pos'n... */
			draw_shape(x, y, shape);
/* ... and spawn new shape */
FOO:			y = 3;
			x = SCN_WID / 2;
			shape = rand() % 19;

			lines += collapse();
			printf("\x1B[8;%dH""Lines: %u   ",
				SCN_WID * 2 + 2, lines);
/* if newly spawned shape hits something, game over */
			if(detect_shape_hit(x, y, shape))
FIN:			{
				printf("\x1B[10;%dH""\x1B[37;40;1m"
					"       GAME OVER""\x1B[0m",
					SCN_WID * 2 + 2);
				goto END; /* NEW; */
			}
		}
/* hit something because of user movement/rotate OR no hit: just redraw it */
		draw_shape(x, y, shape);
		refresh();
	}
/* turn on keyboard input buffering */
END:
	term_exit();
	(void)ioctl(0, 0, 0);
	return 0;
}

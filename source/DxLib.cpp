#include "DxLib.h"

SDL_Joystick* joystick;
bool keysHeld[SDLK_LAST];
bool sound = true;
void deinit();
//Main screen
SDL_Surface *mainscreen;
SDL_Surface *screen;
//double resModX, resModY;

int DxLib_Init()
{
    atexit(deinit);
    setlocale(LC_CTYPE, "ja_JP.UTF-8");

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
	return -1;
    }

    if (!(mainscreen =
	 SDL_SetVideoMode(0 /*(int)fmax/100 */ ,
			  0 /*(int)fymax/100 */ , 32,
			  SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF))) {
	SDL_Quit();
	return -1;
    }

//    resModX = (double) mainscreen->w / 480;
//    resModY = (double) mainscreen->h / 420;
    screen = SDL_CreateRGBSurface(0, 480, 420, 32,
                                  0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

//    SDL_WM_SetCaption("Syobon Action (しょぼんのアクション)", NULL);
    SDL_ShowCursor(SDL_DISABLE);

    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        fprintf(stderr, "Unable to init SDL_img: %s\n", IMG_GetError());
        return -1;
    }

    //Initialize font
    if (TTF_Init() == -1) {
	fprintf(stderr, "Unable to init SDL_ttf: %s\n", TTF_GetError());
	return -1;
    }

    //Audio Rate, Audio Format, Audio Channels, Audio Buffers
    if (sound && Mix_OpenAudio(48000, AUDIO_F32, 2, 2048)) {
        fprintf(stderr, "Unable to init SDL_mixer: %s\n", Mix_GetError());
        sound = false;
    }
    Mix_Init(MIX_INIT_OGG);

    //Try to get a joystick
    joystick = SDL_JoystickOpen(0);

    for (int i = 0; i < SDLK_LAST; i++)
	keysHeld[i] = false;
    for (int i = 0; i < FONT_MAX; i++)
	font[i] = NULL;
    srand(time(NULL));

    return 0;
}

void UpdateScreen()
{
    SDL_SoftStretch(screen, NULL, mainscreen, NULL);
    SDL_UpdateRect(mainscreen, 0, 0, 0, 0);
}

//Fonts
byte fontsize = 0;
TTF_Font *font[FONT_MAX];

//Strings
void SetFontSize(byte size)
{
    fontsize = size;
    if (font[size] == NULL) {
	font[size] = TTF_OpenFont("/dev_hdd0/game/DRON30001/USRDIR/res/sazanami-gothic.ttf", size);
	if (font[size] == NULL) {
	    printf("Unable to load font: %s\n", TTF_GetError());
	    exit(1);
	}
    }
}

byte fontType = DX_FONTTYPE_NORMAL;
void ChangeFontType(byte type)
{
    fontType = type;
}

void DrawString(int a, int b, const char *x, Uint32 c)
{
    SDL_Color color = { c >> 16, c >> 8, c };
    SDL_Surface *rendered = TTF_RenderUTF8_Solid(font[fontsize], x, color);
    if (fontType == DX_FONTTYPE_EDGE) {
	SDL_Color blk = { 0, 0, 0 };
	SDL_Surface *shadow = TTF_RenderUTF8_Solid(font[fontsize], x, blk);
	DrawGraphZ(a - 1, b - 1, shadow);
	DrawGraphZ(a, b - 1, shadow);
	DrawGraphZ(a + 1, b - 1, shadow);
	DrawGraphZ(a - 1, b, shadow);
	DrawGraphZ(a + 1, b, shadow);
	DrawGraphZ(a - 1, b + 1, shadow);
	DrawGraphZ(a, b + 1, shadow);
	DrawGraphZ(a + 1, b + 1, shadow);
	SDL_FreeSurface(shadow);
    }
    DrawGraphZ(a, b, rendered);
    SDL_FreeSurface(rendered);
}

void DrawFormatString(int a, int b, Uint32 color, const char *str, ...)
{
    va_list args;
    char *newstr = new char[strlen(str) + 16];
    va_start(args, str);
    vsprintf(newstr, str, args);
    va_end(args);
    DrawString(a, b, newstr, color);
    delete newstr;
}

bool ex = false;

void UpdateKeys()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
	switch (event.type) {
	case SDL_KEYDOWN:
	    keysHeld[event.key.keysym.scancode] = true;
	    break;
	case SDL_KEYUP:
	    keysHeld[event.key.keysym.scancode] = false;
	    break;
/*	case SDL_JOYAXISMOTION:
	    if(event.jaxis.which == 0)
	    {
		if(event.jaxis.axis == JOYSTICK_XAXIS)
		{
		    if(event.jaxis.value < 0) keysHeld[KEY_INPUT_LEFT] = true;
		    else if(event.jaxis.value > 0) keysHeld[KEY_INPUT_RIGHT] = true;
		    else {
			keysHeld[KEY_INPUT_LEFT] = false;
			keysHeld[KEY_INPUT_RIGHT] = false;
		    }
		}
		else if(event.jaxis.axis == JOYSTICK_XAXIS)
		{
		    if(event.jaxis.value < 0) keysHeld[KEY_INPUT_UP] = true;
		    else if(event.jaxis.value > 0) keysHeld[KEY_INPUT_DOWN] = true;
		    else {
			keysHeld[KEY_INPUT_UP] = false;
			keysHeld[KEY_INPUT_DOWN] = false;
		    }
		}
	    }
	    break;
*/	case SDL_QUIT:
	    ex = true;
	    break;
	}
    }
}

byte ProcessMessage()
{
    return ex;
}

byte CheckHitKey(int key)
{
    if(key == SDL_SCANCODE_Z && keysHeld[SDL_SCANCODE_SEMICOLON]) return true;
    return keysHeld[key];
}

byte WaitKey()
{
    SDL_Event event;
    while (true) {
	while (SDL_PollEvent(&event))
	    if (event.type == (SDL_KEYDOWN))
		return event.key.keysym.scancode;
	    if (event.type == (SDL_JOYBUTTONDOWN))
		return event.jbutton.button;
    }
}

/*Uint32 GetColor(byte r, byte g, byte b)
{
    return r << 8 * 3 | g << 8 * 2 | b << 8 | 0xFF;
}*/

void DrawGraphZ(int a, int b, SDL_Surface * mx)
{
    if(mx)
    {
        SDL_Rect offset;
        offset.x = a;
        offset.y = b;
        SDL_BlitSurface(mx, NULL, screen, &offset);
    }
}

void DrawTurnGraphZ(int a, int b, SDL_Surface * mx)
{
    if(mx)
    {
        SDL_Rect offset;
        offset.x = a;
        offset.y = b;

        SDL_Surface *flipped = zoomSurface(mx, -1, 1, 0);
        SDL_SetColorKey(flipped, SDL_SRCCOLORKEY,
                SDL_MapRGB(flipped->format, 9 * 16 + 9, 255, 255));
        SDL_BlitSurface(flipped, NULL, screen, &offset);
        SDL_FreeSurface(flipped);
    }
}

void DrawVertTurnGraph(int a, int b, SDL_Surface * mx)
{
    if(mx)
    {
        SDL_Rect offset;
        offset.x = a - mx->w / 2;
        offset.y = b - mx->h / 2;

        SDL_Surface *flipped = rotozoomSurface(mx, 180, 1, 0);
        SDL_SetColorKey(flipped, SDL_SRCCOLORKEY,
                SDL_MapRGB(flipped->format, 9 * 16 + 9, 255, 255));
        SDL_BlitSurface(flipped, NULL, screen, &offset);
        SDL_FreeSurface(flipped);
    }
}

SDL_Surface *DerivationGraph(int srcx, int srcy, int width, int height,
			     SDL_Surface * src)
{
    SDL_Surface *img =
	SDL_CreateRGBSurface(0 /*SDL_SWSURFACE*/, width, height,
			     32 /*screen->format->BitsPerPixel*/,
			     src->format->Rmask, src->format->Gmask,
			     src->format->Bmask, src->format->Amask);

    SDL_Rect offset;
    offset.x = srcx;
    offset.y = srcy;
    offset.w = width;
    offset.h = height;

    SDL_BlitSurface(src, &offset, img, NULL);
    SDL_SetColorKey(img, SDL_SRCCOLORKEY,
		    SDL_MapRGB(img->format, 9 * 16 + 9, 255, 255));
    return img;
}

//Noticably different than the original
SDL_Surface *LoadGraph(const char *filename)
{
    SDL_Surface *image = IMG_Load(filename);

    if (image) return image;
	fprintf(stderr, "Error: Unable to load %s: %s\n", filename, IMG_GetError());
	exit(1);
}

void PlaySoundMem(Mix_Chunk* s, int l)
{
    if(sound) Mix_PlayChannel(-1, s, l);
}

Mix_Chunk* LoadSoundMem(const char* f)
{
    if(!sound) return NULL;

    Mix_Chunk* s = Mix_LoadWAV(f);
    if(s) return s;
    fprintf(stderr, "Error: Unable to load sound %s: %s\n", f, Mix_GetError());
    return NULL;
}

Mix_Music* LoadMusicMem(const char* f)
{
    if(!sound) return NULL;

    Mix_Music* m = Mix_LoadMUS(f);
    if(m) return m;
    fprintf(stderr, "Error: Unable to load music %s: %s\n", f, Mix_GetError());
    return NULL;
}


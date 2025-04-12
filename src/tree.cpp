#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <functional>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

#include "makrons.h"
#include "atree.h"


/* --------------------------------------------------------------------- *
            DECLARE GLOBAL VARIABLES:
 * --------------------------------------------------------------------- */
// OpenGL context
GLFWwindow* window;
GLuint screenTexture;
GLuint frameBuffer;
GLuint shaderProgram;
GLuint fontTextures[3]; // For the three font sizes
GLuint quadVAO, quadVBO;

// A number of "always useful" constants:
const double pi = 3.1415f;

// These 'constants' are initiated in the function "doInit":
double rad;             // A radian.
double pii;            // 2 x pi.
double phi;            // The golden ratio.

// A number of "always useful" variables:
bool tflag;            // A temporary flag.
int tmp;               // A temporary integer number.
double temp;           // A temporary floating point number.
double angle;          // Used to store an angle, (radians or degrees)
double cosx;           // Cos of an angle.
double siny;           // Sin of an angle.
double amp;            // The amplitude of ...
double length;         // The length of ...
double xlen;           // Length of x.
double ylen;           // Length of y.
double zlen;           // Length of z.
double llength;        // If two lengths of ... is needed.

// Temporary index variables:
// int treeIndex;
int i;
int j;
int xi;
int yi;

// Temporary n-counters:
int n;
int p;
int r;

//**** Four dimensions: ****

// Variable + temp dito:
double x, tmpx;
double y, tmpy;
double z, tmpz;
double t, tmpt;               // Also used for 'temp'.

// Temp storage if a square is used more than once:
double xx;
double yy;
double zz;
double tt;

// Constant:
double a;
double b;
double c;
double d;

// Structures for trees:
#define NUMTREES 32
DTBRA branches[NUMTREES][8];
DTIFS trees[NUMTREES];

// Main task flags:
bool runflag = false;
bool renderactive = false;
int programMode = 1;

// Image buffers & stuff:
uint32_t* lpBuf;                               // Pointer to screen buffer.
uint32_t pixelBuffer[WIDTH * HEIGHT];          // The main pixel buffer
uint32_t pict[BHEIGHT][BWIDTH];                // Buffer for picture.
int bpict[BHEIGHT][BWIDTH];                    // Z-buffer for picture, max value = ZDEPTH.
int light[LHEIGHT][LWIDTH];                    // Z-buffer for shadows, max value = ZDEPTH.
uint32_t lpCols[PALSIZE];                      // Buffer for colour palette.

// Used for writing lines:
int lixs, liys, lixe, liye;
uint32_t lcol;
double lxs, lys, lxe, lye;
double RATIO, RERAT;

// Variable to store the background colour -
//(initiate to default):
uint32_t bgcolor = 0x00103050;

// Flag if a new set is to be rendered:
bool newset = false;

// Flag if shadowmap was locked:
bool lockshadow = false;

// Variables to store the current background modes:
int showbackground;
int groundsize;

// Number of branches texts:
const char* textbrmess[8] = {
    "Random branches",
    "Two branches",
    "Three branches",
    "Four branches",
    "Five branches",
    "Six branches",
    "Seven branches",
    "Eight branches"
};

// Background mode, text, colours and text colours:
const char* textbgmess[5] = {
    "On skyblue",
    "On blue",
    "On black",
    "Off black",
    "Off white"
};

// note: RGB
uint32_t bgcol[5] = {
    0x006080C0,
    0x00103050,
    0x00000000,
    0x00000000,
    0x00FFFFFF
};

// note: BGR for original, but RGB for OpenGL
uint32_t txcol[5] = {
    0x00FFFF40,
    0x00FFFF80,
    0x00FFFFFF,
    0x00FFFF00,
    0x00000080
};

// Ground size texts:
const char* textgrmess[3] = {
    "Big ground",
    "Medium ground",
    "Small ground"
};

// Sizes for ground square:
double grounds[3] = {
    0.67f,
    0.45f,
    0.3f
};

// Light model texts:
const char* textlight[2] = {
    "Dark",
    "Light"
};

const char* textpales[3] = {
    "Normal (sunspot)",
    "Flourescent (moonspot)",
    "Filament (noonspot)"
};

const char* textfunky[2] = {
    "FUNKYCOLOURS off",
    "FUNKYCOLOURS on"
};

// Temporary string buffer:
char stringbuf[256];

/* ---------------------------------------------------------------------
        VARIABLES FOR THE ITERATION-LOOP:
   --------------------------------------------------------------------- */
// Image scale ratios, (pic & shadows):
double ims = 2500, lims = 5000, size;

// Zoomfactor and zoom in/out factors:
double imszoom = 1.0f;

// Camera translation:
double CPOSX = 0.0f;
double CPOSY = 0.353f;
double CPOSZ = 0.0f;

// Physical screen coordinates, (x, y & z-buffer) + temp dito:
int nX, nXt;
int nY, nYt;
int nZ, nZt;

//// Light! ////

// Flag if surface normal is in use:
bool donormal;

// Flag if pixel is written to the shadows map:
bool doshadow;

// Light models:
int whitershade = 0;
int lightness = 1;

// Various temp variables used to calculate lights:
int ncols, bright, blight, tbright, overexpose, toverexpose;
double minbright, maxbright, luma, tluma;

// Light rotation angles & cos + sin for these:
double lrxx, lrxy, lrxv;
double lryx, lryy, lryv;

// Attractor-glow!
bool useglow;
double glow = 1.0f, largel = 0.0001f;
double bglow = 1.0f, blargel = 0.0001f;
double dglow = 1.0f, dlargel = 0.0001f;

// Palette index:
int pali = 0;
// "Second root" palette index:
int pali2 = 0;

// Temp storage for colour, (used for "second root").
int tcolr, tcolg, tcolb;

// Rotator, (cos, sin, angle):
double rxx, rxy, rxv;
double ryx, ryy, ryv;

//// IFS! IFS!! IFS!!! ////
long itersdone = 0;                // Number of iterations done so far.
long pixelswritten = 0;            // Number of pixels written to the image Z-buffer.
long spixelswritten;               // Storage for number of pixels written.
long shadowswritten = 0;           // Number of pixels written to the shadow map Z-buffer.
long sshadowswritten;              // Storage for number of shadows written.
int pti;                           // Index counter for IFS loop.
double btx, bty, btz;              // Bottom plane 3D point.
double dtx, dty, dtz;              // 3D point - the fractal.
double ltx, lty, ltz;              // 3D point Light position.
double xt, yt, zt;                 // Pixel position in scene.
double ntx, nty, ntz;              // Normal of pixel (if needed).
double dntx, dnty, dntz;           // Normal of pixel (if needed)!
double nxt, nyt, nzt;              // Pixel normal position in scene.

// IFD random index, ("background" and "dragon"):
int bi, di;

// Translators for IFS:
//( + 4 is the background 4 point space fill square)
double tx[ANTAL + 4];
double ty[ANTAL + 4];
double tz[ANTAL + 4];

// Height of stem where the branch resides:
//( + 4 is the background 4 point space fill square)
double brheight[ANTAL + 4];

// Storage and temp for scale factors for IFS:
double sc[ANTAL + 4];

// Rotators for IFS, (cos, sin, angle):
double drxx, drxy, drxv;
double dryx, dryy, dryv;
double drzx, drzy, drzv;

// Colours for IFS:
unsigned char tcr[ANTAL + 4];
unsigned char tcg[ANTAL + 4];
unsigned char tcb[ANTAL + 4];

// Various variables used to store R, G, B values:
int bcr, bcg, bcb, dcr, dcg, dcb, crt, cgt, cbt;
int tRed, tGreen, tBlue;

// Various variables used to store xRGB values:
uint32_t color, tcolor, bcolor, dcolor;

// Number of colours used:
int nCols;

// Parameter positions saved here:
double xbuf[10];
double ybuf[10];
double zbuf[10];
// Index for buffer:
int ui = 9;

// Number of levels used
short int ilevels = 18;

// Number of branches selected:
int numbranch;
int selnumbranch;

// Index for presets buffers:
int treeinuse = 0;

// RGB values for maximum 32 levels:
unsigned char lcr[32];
unsigned char lcg[32];
unsigned char lcb[32];

// Increaser/decreaser for tree sizes:
double twup, twdwn;

// Preset scale ratios:
double scales[8];

// Index for log + foliage, log and foliage modes:
int logfoliage = 0;

// Index for use log or cube or sphear:
int useLoCoS = 0;

// Coloursation-mode:
int colourmode = 0;
int LoCoSPali = 0;

// **************
// **** FILE ****
// **************
// Buffer for source file:
char filebuf[32768];
// Size of input file:
unsigned int filesize;
// File handle:
FILE* infile;

// Buffer for trees:
ATREE tree[NUMTREES];

// Key state handler
bool keyStates[512]; // More than enough for all GLFW keys
bool keyPressed[512]; // To track if a key was just pressed this frame

void CamAng(void);
void clearallbufs(uint32_t RGBdata);
void clearscreen(uint32_t RGBdata);
void clearscreenbufs(uint32_t RGBdata);
void clearViewmess(void);
void createbackground(void);
void CreatePalette(void);
bool doInit(void);
void DoMyStuff(void);
void drawBox(void);
void drawBoxi(void);
void drawLine(void);
void drawMulticolLine(void);
void finiObjects(void);
double getlevel(double xmin, double xmax, double ystart, double yend, double x, double Q);
void IFSlight(void);
void IFSplot(void);
void initiateIFS(void);
void leafcols(void);
void LitAng(void);
void loadtrees(void);
void loadtree(void);
void manual(void);
void newrender(void);
void newsetup(void);
void setupQuad(void);
int opensource(const char* fname);
void pixelsmess(void);
void printsceneinfo(void);
void printtreeinfo(void);
void randombranch(int indx);
void rotatelight(void);
void rotateview(void);
int SGN(double x);
void ShowPalette(int mode);
void showpic(void);
void spacemess(void);
void stemcols(void);
void unrotatelight(void);
void unrotateview(void);
void viewcols(void);
void writecols(void);
void renderToTexture(void);
void processInput(void);
void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// OpenGL helper functions
void createShaders(void);
void setupFramebuffer(void);
void initOpenGL(void);
void drawScreenTexture(void);
GLuint loadTexture(const std::vector<unsigned char>& data, int width, int height);
void renderText(const char* text, float x, float y, float scale, uint32_t color);
void drawText(float x, float y, const char* text, float r, float g, float b);
void unpackColor(unsigned int col, float *r, float *g, float *b);

int SGN(double x)
{
    if (x < 0.0f)
        return (-1);
    else
        return (1);
} // SGN.

/* --------------------------------------------------------------------- *
            GLFW error callback
 * --------------------------------------------------------------------- */
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

/* --------------------------------------------------------------------- *
            GLFW key callback 
 * --------------------------------------------------------------------- */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 512) {
        if (action == GLFW_PRESS) {
            keyStates[key] = true;
            keyPressed[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keyStates[key] = false;
        }
    }
}

/* --------------------------------------------------------------------- *
            Process input
 * --------------------------------------------------------------------- */
void processInput(void)
{
    // Handle keys similar to original Windows message loop
    if (!runflag)
        return;

    if (programMode == 0 && renderactive) {
        // Render screen keys
        
        // ESC key
        if (keyPressed[GLFW_KEY_ESCAPE]) {
            renderactive = false;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        
        // SPACE key
        if (keyPressed[GLFW_KEY_SPACE]) {
            renderactive = false;
            programMode = 1;
        }
        
        // PAGE_UP key
        if (keyPressed[GLFW_KEY_PAGE_UP]) {
            imszoom *= twup;
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        // PAGE_DOWN key
        if (keyPressed[GLFW_KEY_PAGE_DOWN]) {
            imszoom *= twdwn;
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        // HOME key
        if (keyPressed[GLFW_KEY_HOME]) {
            ryv = 0.0f * rad;
            rxv = 0.0f * rad;
            CamAng();
            imszoom = 1.0f;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        // Arrow keys
        if (keyPressed[GLFW_KEY_UP]) {
            rxv += 0.01f;
            if (int(rxv) > 180)
                rxv = 180.0f;
            CamAng();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_RIGHT]) {
            ryv += 0.01f;
            if (int(ryv) > 180)
                ryv = 180.0f;
            CamAng();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_DOWN]) {
            rxv -= 0.01f;
            if (int(rxv) < -180)
                rxv = -180.0f;
            CamAng();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_LEFT]) {
            ryv -= 0.01f;
            if (int(ryv) < -180)
                ryv = -180.0f;
            CamAng();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        // F1-F4 keys
        if (keyPressed[GLFW_KEY_F1]) {
            trees[treeinuse].usehig = (trees[treeinuse].usehig + 1) & 0x01;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            
        }
        
        if (keyPressed[GLFW_KEY_F2]) {
            trees[treeinuse].glblscl = (trees[treeinuse].glblscl + 1) & 0x01;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            
        }
        
        if (keyPressed[GLFW_KEY_F3]) {
            trees[treeinuse].sctrnsl = (trees[treeinuse].sctrnsl + 1) & 0x01;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            
        }
        
        if (keyPressed[GLFW_KEY_F4]) {
            trees[treeinuse].usetwst = (trees[treeinuse].usetwst + 1) & 0x01;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            
        }
        
        // Number keys 1-8
        if (keyPressed[GLFW_KEY_1] || keyPressed[GLFW_KEY_2] || keyPressed[GLFW_KEY_3] || 
            keyPressed[GLFW_KEY_4] || keyPressed[GLFW_KEY_5] || keyPressed[GLFW_KEY_6] || 
            keyPressed[GLFW_KEY_7] || keyPressed[GLFW_KEY_8]) {
            
            int numKey = 0;
            if (keyPressed[GLFW_KEY_1]) numKey = 0;
            else if (keyPressed[GLFW_KEY_2]) numKey = 1;
            else if (keyPressed[GLFW_KEY_3]) numKey = 2;
            else if (keyPressed[GLFW_KEY_4]) numKey = 3;
            else if (keyPressed[GLFW_KEY_5]) numKey = 4;
            else if (keyPressed[GLFW_KEY_6]) numKey = 5;
            else if (keyPressed[GLFW_KEY_7]) numKey = 6;
            else if (keyPressed[GLFW_KEY_8]) numKey = 7;
            
            randombranch(numKey);
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            writecols();
            clearViewmess();
            
        }
        
        // Alphabet keys
        if (keyPressed[GLFW_KEY_A]) {
            ryv = -180.0f * rad + RND * 360.0f * rad;
            ryx = cos(ryv);
            ryy = sin(ryv);
            rxv = -10.0f * rad + RND * 100.0f * rad;
            rxx = cos(rxv);
            rxy = sin(rxv);
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_B]) {
            showbackground++;
            if (showbackground > 4)
                showbackground = 0;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            writecols();
            clearViewmess();
        }
        
        if (keyPressed[GLFW_KEY_C]) {
            clearscreenbufs(bgcol[showbackground]);
            trees[treeinuse].radius *= sqrt(twup);
        }
        
        if (keyPressed[GLFW_KEY_D]) {
            trees[treeinuse].radius *= twup;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_E]) {
            if (++logfoliage >= 3)
                logfoliage = 0;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printsceneinfo();
            if (logfoliage != 1)
                writecols();
        }
        
        if (keyPressed[GLFW_KEY_F]) {
            trees[treeinuse].radius *= twdwn;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_G]) {
            groundsize++;
            if (groundsize > 2)
                groundsize = 0;
            createbackground();
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printsceneinfo();
        }
        
        if (keyPressed[GLFW_KEY_I]) {
            printsceneinfo();
            lcol = txcol[showbackground];
            pixelsmess();
            printtreeinfo();
            writecols();
            if (colourmode)
                ShowPalette(ABSZ);
            
        }
        
        if (keyPressed[GLFW_KEY_K]) {
            if (!lockshadow)
                lockshadow = true;
            trees[treeinuse].radius *= sqrt(twup);
            clearscreenbufs(bgcol[showbackground]);
        }
        
        if (keyPressed[GLFW_KEY_L]) {
            lightness++;
            if (lightness > 1)
                lightness = 0;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printsceneinfo();
        }
        
        if (keyPressed[GLFW_KEY_M]) {
            if (++treeinuse >= NUMTREES)
                treeinuse = 0;
            newsetup();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printtreeinfo();
            printsceneinfo();
            writecols();
        }
        
        if (keyPressed[GLFW_KEY_N]) {
            if (++selnumbranch > 7)
                selnumbranch = 0;
            newsetup();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printtreeinfo();
            printsceneinfo();
            writecols();
        }
        
        if (keyPressed[GLFW_KEY_O]) {
            clearscreen(bgcol[showbackground]);
            if (++colourmode > 1)
                colourmode = 0;
            if (colourmode) {
                CreatePalette();
                ShowPalette(ABSZ);
            }
            stemcols();
            leafcols();
            printsceneinfo();
            newrender();
            writecols();
            clearViewmess();
            clearscreenbufs(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_P]) {
            clearscreen(bgcol[showbackground]);
            if (colourmode) {
                CreatePalette();
                ShowPalette(ABSZ);
            }
            stemcols();
            leafcols();
            printsceneinfo();
            newrender();
            writecols();
            clearViewmess();
            clearscreenbufs(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_R]) {
            treeinuse = 31;
            newsetup();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            printtreeinfo();
            writecols();
            clearViewmess();
            
        }
        
        if (keyPressed[GLFW_KEY_S]) {
            trees[treeinuse].height *= twdwn;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_T]) {
            FILLBOX(0, 0, WIDTH, HEIGHT, 0x00FFFFFF);
            
            lcol = 0x00000080;
            pixelsmess();
        }
        
        if (keyPressed[GLFW_KEY_U]) {
            if (++useLoCoS >= 3)
                useLoCoS = 0;
            else
                trees[treeinuse].radius = fabs(trees[treeinuse].height / 2.0f);
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
        
        if (keyPressed[GLFW_KEY_V]) {
            showpic();
        }
        
        if (keyPressed[GLFW_KEY_W]) {
            whitershade++;
            if (whitershade > 2)
                whitershade = 0;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
            printsceneinfo();
        }
        
        if (keyPressed[GLFW_KEY_X]) {
            trees[treeinuse].height *= twup;
            newrender();
            clearallbufs(bgcol[showbackground]);
            clearscreen(bgcol[showbackground]);
            
        }
    }
    else if (programMode == 1) {
        // Info screen keys
        // printf("Info screen program mode = 1\n");
        
        // ESC key
        if (keyPressed[GLFW_KEY_ESCAPE]) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        
        // SPACE key
        if (keyPressed[GLFW_KEY_SPACE]) {
            programMode = 0;
            renderactive = true;
            clearscreen(bgcol[showbackground]);
            showpic();
            newrender();
            
        }
        
        // F1-F4 keys
        if (keyPressed[GLFW_KEY_F1]) {
            trees[treeinuse].usehig = (trees[treeinuse].usehig + 1) & 0x01;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_F2]) {
            trees[treeinuse].glblscl = (trees[treeinuse].glblscl + 1) & 0x01;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_F3]) {
            trees[treeinuse].sctrnsl = (trees[treeinuse].sctrnsl + 1) & 0x01;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_F4]) {
            trees[treeinuse].usetwst = (trees[treeinuse].usetwst + 1) & 0x01;
            newrender();
            clearscreenbufs(bgcol[showbackground]);
            printtreeinfo();
        }
        
        // Various other keys for info screen
        if (keyPressed[GLFW_KEY_B]) {
            showbackground++;
            if (showbackground > 4)
                showbackground = 0;
            newrender();
            newset = true;
            printsceneinfo();
            viewcols();
        }
        
        if (keyPressed[GLFW_KEY_E]) {
            if (++logfoliage >= 3)
                logfoliage = 0;
            newrender();
            clearallbufs(bgcol[showbackground]);
        }
        
        if (keyPressed[GLFW_KEY_I]) {
            printsceneinfo();
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_L]) {
            lightness++;
            if (lightness > 1)
                lightness = 0;
            newrender();
            newset = true;
            printsceneinfo();
        }
        
        if (keyPressed[GLFW_KEY_M]) {
            if (++treeinuse >= NUMTREES)
                treeinuse = 0;
            newsetup();
            clearallbufs(bgcol[showbackground]);
            viewcols();
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_N]) {
            if (++selnumbranch > 7)
                selnumbranch = 0;
            newsetup();
            clearallbufs(bgcol[showbackground]);
            viewcols();
            printsceneinfo();
            printtreeinfo();
        }
        
        if (keyPressed[GLFW_KEY_O]) {
            if (++colourmode > 1)
                colourmode = 0;
            if (colourmode)
                CreatePalette();
            stemcols();
            leafcols();
            newrender();
            viewcols();
            printsceneinfo();
            clearscreenbufs(bgcol[showbackground]);
        }
        
        if (keyPressed[GLFW_KEY_P]) {
            if (colourmode)
                CreatePalette();
            stemcols();
            leafcols();
            newrender();
            viewcols();
            clearscreenbufs(bgcol[showbackground]);
        }
        
        if (keyPressed[GLFW_KEY_R]) {
            treeinuse = 31;
            newsetup();
            newset = true;
            printtreeinfo();
            viewcols();
        }
        
        if (keyPressed[GLFW_KEY_U]) {
            if (++useLoCoS >= 3)
                useLoCoS = 0;
            else
                trees[treeinuse].radius = (fabs(trees[treeinuse].height) / 3.0f);
            newrender();
        }
        
        if (keyPressed[GLFW_KEY_W]) {
            whitershade++;
            if (whitershade > 2)
                whitershade = 0;
            newrender();
            newset = true;
            printsceneinfo();
        }
    }
    
    // Reset key pressed state for next frame
    for (int i = 0; i < 512; i++) {
        keyPressed[i] = false;
    }
}

/* --------------------------------------------------------------------- *
            Main function:
 * --------------------------------------------------------------------- */
int main(int argc, char** argv)
{
    if (!doInit()) {
        return -1;
    }
    
    // Setup fonts and fractal functions
    initiateIFS();
    
    runflag = true;
    
    // Main loop
    // printf("Start main loop...\n");
    while (!glfwWindowShouldClose(window) && runflag) {
        // Process input
        processInput();
        // printf("Processed input\n");
        
        if (programMode == 0 && renderactive) {
            // printf("Rendering...\n");
            DoMyStuff();
        }
        
        // Render
        if (programMode == 0) {
            // Copy & anti-alias from pixel-buffer to screen
            showpic();
            
            // Initiate iteration parameters
            if (newset) {
                clearallbufs(bgcol[showbackground]);
                newset = false;
                // Then - again!
                showpic();
            }
            
            // "tag" screen
            
        }
        else if (programMode == 1) {
            // View info screen
            // printf("Viewing info screen...\n");
            manual();
        }
        
        // Render to screen
        // printf("Rendering to screen...\n");
        renderToTexture();
        // printf("Draw screen texture...\n");
        drawScreenTexture();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
        // printf("Swapped buffers and polled events\n");
    }
    
    // Cleanup
    finiObjects();
    glfwTerminate();
    
    return 0;
}

/* --------------------------------------------------------------------- *
            Render pixel buffer to texture
 * --------------------------------------------------------------------- */
void renderToTexture(void)
{
    // Copy from our pixel buffer to OpenGL texture
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
}

/* --------------------------------------------------------------------- *
            Draw screen texture
 * --------------------------------------------------------------------- */
void drawScreenTexture(void)
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Use shader program
    glUseProgram(shaderProgram);
    
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    
    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/* --------------------------------------------------------------------- *
            Initialize OpenGL
 * --------------------------------------------------------------------- */
void initOpenGL(void)
{
    printf("Initializing OpenGL...\n");
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed: %s\n", glewGetErrorString(err));
        exit(1);
    }
    
    // Create shaders
    createShaders();
    
    // Setup quad for rendering
    setupQuad();
    
    // Setup framebuffer and texture
    setupFramebuffer();
    
    // Set up OpenGL state
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
}

/* --------------------------------------------------------------------- *
            Setup framebuffer with texture
 * --------------------------------------------------------------------- */
void setupFramebuffer(void)
{
    // Create texture for rendering
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
}

void setupQuad() {
    float vertices[] = {
        // positions        // texture coords
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
         1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // bottom right
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f  // top right
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    GLuint quadEBO;
    glGenBuffers(1, &quadEBO);
    
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

/* --------------------------------------------------------------------- *
            Create shader program
 * --------------------------------------------------------------------- */
void createShaders(void)
{
    // Simple vertex shader
    const char* vertexShaderSource = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   TexCoord = aTexCoord;\n"
        "}\n";
    
    // Simple fragment shader
    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D screenTexture;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(screenTexture, TexCoord);\n"
        "}\n";
    
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // Check for compilation errors
    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "Shader vertex compilation failed\n%s\n", infoLog);
    }
    
    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check for compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "Shader fragment compilation failed\n%s\n", infoLog);
    }
    
    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "Shader program linking failed\n%s\n", infoLog);
    }
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

/* --------------------------------------------------------------------- *
            Setup: randomize,
                create some constants,
                create window,
                set screen-mode,
                get pointer to screen.
 * --------------------------------------------------------------------- */
bool doInit(void)
{
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }
    
    // Set error callback
    glfwSetErrorCallback(error_callback);
    
    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window = glfwCreateWindow(WIDTH, HEIGHT, NAME, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }
    
    // Make OpenGL context current
    glfwMakeContextCurrent(window);
    
    // Set key callback
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize OpenGL
    initOpenGL();
    
    // Randomize & Create constants:
    srand((unsigned int)time(NULL));
    phi = (1.0f + sqrt(5.0f)) / 2.0f;
    pii = 2.0f * pi;
    rad = pii / 360.0f;
    RATIO = (double)WIDTH / HEIGHT;
    RERAT = (double)HEIGHT / WIDTH;
    
    // Set up screen buffer
    lpBuf = pixelBuffer;

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        pixelBuffer[i] = bgcol[showbackground];
    }
    
    return true;
}

/* --------------------------------------------------------------------- *
            Cleanup
 * --------------------------------------------------------------------- */
void finiObjects(void)
{
    // Clean up OpenGL resources
    glDeleteTextures(1, &screenTexture);
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteProgram(shaderProgram);
    
    // Clean up font textures
    glDeleteTextures(3, fontTextures);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteTextures(1, &screenTexture);
    glDeleteProgram(shaderProgram);
    
    // GLFW window and context are handled by glfwTerminate in main
}

/* --------------------------------------------------------------------- *
            Program setup:
 * --------------------------------------------------------------------- */
void initiateIFS(void)
{
    // Load preset trees:
    loadtrees();

    // Create a fractal set:
    newsetup();

    // Also create a colour-palette:
    CreatePalette();

    // Background is visible:
    showbackground = 0;

    // Clear buffers:
    clearallbufs(bgcol[showbackground]);

    return;
}

/* --------------------------------------------------------------------- *
        Here goes the iteration-loop:
 * --------------------------------------------------------------------- */
void DoMyStuff(void)
{
    // Don't count pixels written for bottom plane:
    // Comment: The counter is updated inside the plot function.
    //          This is why I save the value, (to restore later).
    //          Ok! I could use a flag instead, but did not =)
    spixelswritten = pixelswritten;
    sshadowswritten = shadowswritten;

    // Show background?
    if (showbackground < 3)
    {
        // ************************* //
        // * Bottom ******** IFS ! * //
        // ************************* //

        // Turn glow on:
        useglow = true;

        // Don't write to shadow map:
        doshadow = false;

        // Don't use normal, (maybe I will fix this later):
        donormal = false;

        // Iteration loop:
        for (pti = 6; pti >= 0; pti--)
        {
            bi = int(RND * 4);

            btx = (btx - tx[bi]) * sc[bi];
            bty = (bty - ty[bi]) * sc[bi];
            btz = (btz - tz[bi]) * sc[bi];

            // Attractor-glow:
            if (useglow)
            {
                t = sqrt(btx*btx + bty*bty + btz*btz);
                if (t > blargel)
                {
                    blargel = t;
                }    // if blargel.
                t = pow((1.0f - t / blargel), 16.0f);
                bglow = (bglow + t) / 2.0f;
            } // Attractor-glow.

            btx += tx[bi];
            bty += ty[bi];
            btz += tz[bi];

            // Color:
            switch (colourmode)
            {
                case NORMALCOLOURS:
                default:
                    bcr = ((bcr + tcr[bi]) >> 1) & 0xFF;
                    bcg = ((bcg + tcg[bi]) >> 1) & 0xFF;
                    bcb = ((bcb + tcb[bi]) >> 1) & 0xFF;
                break; // NORMALCOLOURS.
            } // Switch colourmode.

            // Scale & translate to scene:
            xt = btx + CPOSX;
            yt = bty + CPOSY;
            zt = btz + CPOSZ;

            // it's light:
            glow = bglow;
            IFSlight();

            // Scale & translate to scene:
            xt = btx + CPOSX;
            yt = bty + CPOSY;
            zt = btz + CPOSZ;

            // Select colour for pixel:
            crt = bcr;
            cgt = bcg;
            cbt = bcb;

            // Plot pixel to scene:
            IFSplot();

        } // End of the iteration loop (the bottom-plane).
    } // if showbackground.

    // ************************//
    // * Fractal ******* IFS ! //
    // ************************//

    // FIRST DO IFS FOR FOLIAGE:

    if (logfoliage != 1)
    {
        // Coordinate:
        dtx = xbuf[ui];
        dty = ybuf[ui];
        dtz = zbuf[ui];

        // Use shadow map?:
        if (lockshadow)
            doshadow = false;
        else
            doshadow = true;

        // Do not use normal for foliage:
        donormal = false;

        // Restore iteration counter:
        pixelswritten = spixelswritten;
        shadowswritten = sshadowswritten;

        // IFS-snurran!
        for (pti = 8; pti >= 0; pti--)
        {
            itersdone++;
            di = 4 + int(RND * trees[treeinuse].branches);

            // Translate to scene:
            xt = dtx + CPOSX;
            yt = dty + CPOSY;
            zt = dtz + CPOSZ;

            // Save position:
            tmpx = xt;
            tmpy = yt;
            tmpz = zt;

            // Get luminousity & plot in the light's Z-table:
            IFSlight();

            // Translate to scene:
            xt = dtx + CPOSX;
            yt = dty + CPOSY;
            zt = dtz + CPOSZ;

            // Select colour for pixel:
            crt = dcr;
            cgt = dcg;
            cbt = dcb;

            // Plot pixel to scene:
            IFSplot();

            // **** TRANSFORMATIONS **** //
            // ** Pixel ** //
            // Rotations:

            i = di - 4;
            // Do twist?:
            if (trees[treeinuse].usetwst)
            {
                t = branches[treeinuse][i].twistc * dtz - branches[treeinuse][i].twists * dtx;
                dtx = branches[treeinuse][i].twistc * dtx + branches[treeinuse][i].twists * dtz;
                dtz = t;
            } // Do twist?.
            // Lean:
            t = branches[treeinuse][i].leanc * dtx - branches[treeinuse][i].leans * dty;
            dty = branches[treeinuse][i].leanc * dty + branches[treeinuse][i].leans * dtx;
            dtx = t;
            // Rotate:
            t = branches[treeinuse][i].rotatec * dtz - branches[treeinuse][i].rotates * dtx;
            dtx = branches[treeinuse][i].rotatec * dtx + branches[treeinuse][i].rotates * dtz;
            dtz = t;

            // Scale!
            // Global scale?
            if (trees[treeinuse].glblscl)
            {
                dtx *= branches[treeinuse][0].scale;
                dty *= branches[treeinuse][0].scale;
                dtz *= branches[treeinuse][0].scale;
            }
            // No!, all diffrent:
            else
            {
                dtx *= branches[treeinuse][i].scale;
                dty *= branches[treeinuse][i].scale;
                dtz *= branches[treeinuse][i].scale;
            }

            // Scale by heigth of branch?
            if (trees[treeinuse].sctrnsl)
            {
                dtx *= branches[treeinuse][i].height;
                dty *= branches[treeinuse][i].height;
                dtz *= branches[treeinuse][i].height;
            }

            // Translate!
            // Use heights?
            if (trees[treeinuse].usehig)
                dty += trees[treeinuse].height * branches[treeinuse][i].height;
            // No!, all at top of stem:
            else
                dty += trees[treeinuse].height;

            // Color!
            dcr = ((dcr + tcr[di]) >> 1) & 0xFF;
            dcg = ((dcg + tcg[di]) >> 1) & 0xFF;
            dcb = ((dcb + tcb[di]) >> 1) & 0xFF;

        } // End of the iteration loop (the IFS random walk).
    } // If logfoliage != 1.

    // Save current coordinate:
    xbuf[ui] = dtx;
    ybuf[ui] = dty;
    zbuf[ui] = dtz;

    // THEN DO L-IFSYS FOR STUB AND STEM:

    if (logfoliage != 2)
    {
        switch (useLoCoS)
        {
            case USECUBES:
                t = trees[treeinuse].radius;
                t += t * float(1 + RNDBOOL) * 0.01f;
                // Any point on a square:
                dtx = t;
                dty = -t + RND * t * 2.0f;
                dtz = -t + RND * t * 2.0f;
                // Create surface normal:
                dntx = dtx - 1.0f;
                dnty = dty;
                dntz = dtz;
                // Square to any side of a cube:
                if (0 != (tmp = 1 + int(RND * 6)))
                    for (i = 0; i < tmp; i++)
                    {
                        t = -dty;
                        dty = dtz;
                        dtz = dtx;
                        dtx = t;
                        t = -dnty;
                        dnty = dntz;
                        dntz = dntx;
                        dntx = t;
                    }
                // Translate to start position:
                dty = dty + trees[treeinuse].height;
                dnty = dnty + trees[treeinuse].height;
            break; // Cubes.

            case USESPHEARS:
                t = trees[treeinuse].radius;
                t += t * float(1 + RNDBOOL) * 0.01f;
                // Any point in a cube:
                dtx = (-1.0f) + RND * 2.0f;
                dty = (-1.0f) + RND * 2.0f;
                dtz = (-1.0f) + RND * 2.0f;
                if (0 < (length = sqrt(dtx*dtx + dty*dty + dtz*dtz)))
                {
                    // Normalise to length = 1.0:
                    dtx /= length;
                    dty /= length;
                    dtz /= length;
                    // Create surface normal:
                    dntx = -dtx;
                    dnty = -dty;
                    dntz = -dtz;
                    // Set radius:
                    dtx *= t;
                    dty *= t;
                    dtz *= t;
                }
                // Translate to start position:
                dty = dty + trees[treeinuse].height;
                dnty = dnty + trees[treeinuse].height;
            break; // Sphears.

            case USELOGS:
            default:
                // Create log!
                angle = 360.0f * rad * RND;
                t = RND;
                x = trees[treeinuse].radius * branches[treeinuse][0].scale;
                x = trees[treeinuse].radius - x;
                x = trees[treeinuse].radius - t * x;
                dtx = sin(angle) * x;
                dty = t * trees[treeinuse].height;
                dtz = cos(angle) * x;
                dntx = -sin(angle);
                dnty = t * trees[treeinuse].height;
                dntz = -cos(angle);
            break; // Logs.
        } // Switch useLoCoS.

        // Color:
        LoCoSPali = (PALSIZE >> 1);
        switch (colourmode)
        {
            case FUNKYCOLOURS:
                tcolor = lpCols[LoCoSPali];
                dcr = (tcolor >> 16) & 0xFF;
                dcg = (tcolor >> 8) & 0xFF;
                dcb = tcolor & 0xFF;
            break; // FUNKYCOLOURS.
            case NORMALCOLOURS:
            default:
                dcr = lcr[ilevels];
                dcg = lcg[ilevels];
                dcb = lcb[ilevels];
            break; // NORMALCOLOURS.
        } // Switch colourmode.

        // Use shadow map?:
        if (lockshadow)
            doshadow = false;
        else
            doshadow = true;

        // Use normal for stem and branches:
        donormal = true;

        // Restore iteration counter:
        pixelswritten = spixelswritten;
        shadowswritten = sshadowswritten;

        // IFS-snurran!
        for (pti = (ilevels - 1); pti >= 0; pti--)
        {
            itersdone++;
            di = 4 + int(RND * trees[treeinuse].branches);

            // Translate to scene:
            xt = dtx + CPOSX;
            yt = dty + CPOSY;
            zt = dtz + CPOSZ;

            // Save position:
            tmpx = xt;
            tmpy = yt;
            tmpz = zt;

            // Get luminousity & plot in the light's Z-table:
            IFSlight();

            // Translate to scene:
            xt = dtx + CPOSX;
            yt = dty + CPOSY;
            zt = dtz + CPOSZ;

            // Select colour for pixel:
            crt = dcr;
            cgt = dcg;
            cbt = dcb;

            // Plot pixel to scene:
            IFSplot();

            // **** TRANSFORMATIONS **** //
            // ** Pixel ** //
            // Rotations:

            i = di - 4;
            // Do twist?:
            if (trees[treeinuse].usetwst)
            {
                t = branches[treeinuse][i].twistc * dtz - branches[treeinuse][i].twists * dtx;
                dtx = branches[treeinuse][i].twistc * dtx + branches[treeinuse][i].twists * dtz;
                dtz = t;
            } // Do twist?.
            // Lean:
            t = branches[treeinuse][i].leanc * dtx - branches[treeinuse][i].leans * dty;
            dty = branches[treeinuse][i].leanc * dty + branches[treeinuse][i].leans * dtx;
            dtx = t;
            // Rotate:
            t = branches[treeinuse][i].rotatec * dtz - branches[treeinuse][i].rotates * dtx;
            dtx = branches[treeinuse][i].rotatec * dtx + branches[treeinuse][i].rotates * dtz;
            dtz = t;

            // Scale!
            // Global scale?
            if (trees[treeinuse].glblscl)
            {
                dtx *= branches[treeinuse][0].scale;
                dty *= branches[treeinuse][0].scale;
                dtz *= branches[treeinuse][0].scale;
            }
            // No!, all diffrent:
            else
            {
                dtx *= branches[treeinuse][i].scale;
                dty *= branches[treeinuse][i].scale;
                dtz *= branches[treeinuse][i].scale;
            }

            // Scale by heigth of branch?
            if (trees[treeinuse].sctrnsl)
            {
                dtx *= branches[treeinuse][i].height;
                dty *= branches[treeinuse][i].height;
                dtz *= branches[treeinuse][i].height;
            }

            // Translate!
            // Use heights?
            if (trees[treeinuse].usehig)
                dty += trees[treeinuse].height * branches[treeinuse][i].height;
            // No!, all at top of stem:
            else
                dty += trees[treeinuse].height;

            // ** Pixel normal ** //
            // Rotations:

            // Do twist?:
            if (trees[treeinuse].usetwst)
            {
                t = branches[treeinuse][i].twistc * dntz - branches[treeinuse][i].twists * dntx;
                dntx = branches[treeinuse][i].twistc * dntx + branches[treeinuse][i].twists * dntz;
                dntz = t;
            } // Do twist?.
            // Lean:
            t = branches[treeinuse][i].leanc * dntx - branches[treeinuse][i].leans * dnty;
            dnty = branches[treeinuse][i].leanc * dnty + branches[treeinuse][i].leans * dntx;
            dntx = t;
            // Rotate:
            t = branches[treeinuse][i].rotatec * dntz - branches[treeinuse][i].rotates * dntx;
            dntx = branches[treeinuse][i].rotatec * dntx + branches[treeinuse][i].rotates * dntz;
            dntz = t;

            // Scale!
            // Global scale?
            if (trees[treeinuse].glblscl)
            {
                dntx *= branches[treeinuse][0].scale;
                dnty *= branches[treeinuse][0].scale;
                dntz *= branches[treeinuse][0].scale;
            }
            // No!, all diffrent:
            else
            {
                dntx *= branches[treeinuse][i].scale;
                dnty *= branches[treeinuse][i].scale;
                dntz *= branches[treeinuse][i].scale;
            }

            // Scale by heigth of branch?
            if (trees[treeinuse].sctrnsl)
            {
                dntx *= branches[treeinuse][i].height;
                dnty *= branches[treeinuse][i].height;
                dntz *= branches[treeinuse][i].height;
            }

            // Translate!
            // Use heights?
            if (trees[treeinuse].usehig)
                dnty += trees[treeinuse].height * branches[treeinuse][i].height;
            // No!, all at top of stem:
            else
                dnty += trees[treeinuse].height;

            // Re-normalize!
            dntx -= dtx;
            dnty -= dty;
            dntz -= dtz;
            t = sqrt(dntx*dntx + dnty*dnty + dntz*dntz);
            dntx /= t;
            dnty /= t;
            dntz /= t;

            // Color:
            switch (colourmode)
            {
                case FUNKYCOLOURS:
                    // Diffrent modes for diffrent
                    // number of branches:
                    switch (trees[treeinuse].branches - 1)
                    {
                        case 1:
                            if (i & 0x1)
                                LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
                            else
                                LoCoSPali >>= 1;
                        break;

                        case 2:
                        case 3:
                            if (i & 0x1)
                                LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
                            else
                                LoCoSPali >>= 1;
                            tmp = (PALSIZE >> 1) * ((i >> 1) & 0x01);
                            LoCoSPali = tmp + (LoCoSPali >> 1);
                        break;

                        case 7:
                        default:
                            if (i & 0x1)
                                LoCoSPali += ((PALSIZE - LoCoSPali) >> 1);
                            else
                                LoCoSPali >>= 1;

                            tmp = (PALSIZE >> 2) * ((i >> 1) & 0x03);
                            LoCoSPali = tmp + (LoCoSPali >> 2);
                        break;
                    } // switch numbranch.

                    // Get selected colour from palette:
                    tcolor = lpCols[LoCoSPali];
                    tRed = (tcolor >> 16) & 0xFF;
                    tGreen = (tcolor >> 8) & 0xFF;
                    tBlue = tcolor & 0xFF;
                    dcr = ((dcr + (tRed * 3)) >> 2) & 0xFF;
                    dcg = ((dcg + (tGreen * 3)) >> 2) & 0xFF;
                    dcb = ((dcb + (tBlue * 3)) >> 2) & 0xFF;
                break; // FUNKYCOLOURS.

                case NORMALCOLOURS:
                default:
                    dcr = ((dcr + (lcr[pti] * 3)) >> 2) & 0xFF;
                    dcg = ((dcg + (lcg[pti] * 3)) >> 2) & 0xFF;
                    dcb = ((dcb + (lcb[pti] * 3)) >> 2) & 0xFF;
                break; // NORMALCOLOURS.
            } // Switch colourmode.
        } // End of the iteration loop (the tree L-IFS).
    } // If logfoliage != 2.

    return;
}

/* --------------------------------------------------------------------- *
            Make it light:

          Get luminousity, plot in light scene Z-table.

 * --------------------------------------------------------------------- */
void IFSlight(void)
{
    // Rotate to light position:
    rotatelight();

    // Clip z:
    if ((zt > -1.0f) && (zt < 1.0f))
    {
        // Do normal?:
        if (donormal)
        {
            nxt = dntx + CPOSX;
            nyt = dnty + CPOSY;
            nzt = dntz + CPOSZ;

            // Rotate normal z to light position!
            t = lryx * nzt - lryy * nxt;
            nxt = lryx * nxt + lryy * nzt;
            nzt = t;

            t = lrxx * nyt - lrxy * nzt;
            nzt = lrxx * nzt + lrxy * nyt;
            nyt = t;

            // Get light normal!
            x = dtx - ntx;
            y = dty - nty;
            z = dtz - ntz;

            t = sqrt(x*x + y*y + z*z);
            nzt -= (z / t);
            nzt = (0.5f + nzt) / 2.0f;
        }
        // Do normal.

        // Get distance to light:
        size = (3.0f + zt) / 2.0f;
        t = (2.0f - size);

        // Calculate & calibrate luminousity:
        if (donormal)
            t = t * t * nzt;

        // Calculate & calibrate luminousity:
        if (donormal)
            if ((logfoliage != USELOGS) && (lightness == 0))
                t = ((2.0f * t) + pow(nzt, 3.0f)) / 3.0f;
            else
                t = t * t * nzt;

        t = (1.0f + t) / 2.0f;

        if (t < minbright)
            minbright = t;
        t = t - minbright;
        if (t > maxbright)
            maxbright = t;
        t = t / maxbright;

        t = t * 2.0f;
        luma = t - 1.0f;
        if (luma < 0.0f)
            luma = 0.0f;
        luma = 1.0f + pow(luma, 8.0f);
        if (t > 1.0f)
            t = 1.0f;
        overexpose = int(255.0 * luma) - 0xFF;
        if (overexpose < 0)
            overexpose = 0;

        if (useglow)
            bright = int(48 * glow + 208 * t) & 0xFF;
        else
            bright = int(255 * t) & 0xFF;

        blight = bright >> 1;

        nZ = int((2.0f - size) * (ZDEPTH >> 1)) & 0x7FFF;
        zt = (lims * imszoom) / size;

        nY = LMIDY + int(yt * zt);
        nX = LMIDX + int(xt * zt);
        // Clip y:
        if ((nY >= 0) && (nY < LHEIGHT))
        {
            // Clip x:
            if ((nX >= 0) && (nX < LWIDTH))
            {
                if (light[nY][nX] > (nZ + 2))
                {
                    // Create shadow.
                    bright = blight;
                    overexpose = 0;
                    luma = 1.0f;
                }
                else if (doshadow)
                {
                    // Update counter for pixels written to shadow map:
                    if ((light[nY][nX] + 2) < nZ)
                        shadowswritten++;
                    light[nY][nX] = nZ;
                } // Shadow or not.
            } // clip - X.
        }    // clip - Y.
    } // clip - Z.

    return;
} // IFSlight.

/* --------------------------------------------------------------------- *
            Make it show:

            Rotate to scene §, plot to Z-table and pixel-buffer,
            anti-anilize and plot to screen from pixel-buffer.

                § No translation, just views towards the center here =)

 * --------------------------------------------------------------------- */
void IFSplot(void)
{
    // Rotate to angle of view:
    rotateview();

    // Clip z:
    if ((zt > -1.0f) && (zt < 1.0f))
    {
        size = (3.0f + zt) / 2.0f;
        nZ = int((2.0f - size) * (ZDEPTH >> 1)) & 0x7FFF;
        zt = (ims * imszoom) / size;

        nY = BMIDY + int(yt * zt);
        nX = BMIDX + int(xt * zt);
        // Clip y:
        if ((nY >= 0) && (nY < BHEIGHT))
        {
            // Clip x:
            if ((nX >= 0) && (nX < BWIDTH))
            {
                // Plot if Point closer to viewer than
                // the previous at the position:
                if (bpict[nY][nX] < nZ)
                {
                    // Write new depth to z-buffer:
                    bpict[nY][nX] = nZ;

                    // Update pixel counter:
                    pixelswritten++;

                    // Brighter than average?
                    if (overexpose)
                    {
                        crt = int((crt + overexpose) / luma);
                        cgt = int((cgt + overexpose) / luma);
                        cbt = int((cbt + overexpose) / luma);
                    } // Overexpose.

                    // Whiter shade of pale?:
                    if (whitershade)
                    {
                        // Cold in varm:
                        if (whitershade == 1)
                        {
                            crt = (((crt * 3) + bright) >> 2) & 0xFF;
                            cgt = (((cgt << 1) + bright) / 3) & 0xFF;
                            cbt = ((cbt + bright) >> 1) & 0xFF;
                        }
                        // Varm in cold:
                        else
                        {
                            crt = ((crt + bright) >> 1) & 0xFF;
                            cgt = (((cgt << 1) + bright) / 3) & 0xFF;
                            cbt = (((cbt * 3) + bright) >> 2) & 0xFF;
                        }
                    } // Whiter shade of pale.

                    crt = ((crt * bright) >> 8) & 0xFF;
                    cgt = ((cgt * bright) >> 8) & 0xFF;
                    cbt = ((cbt * bright) >> 8) & 0xFF;
                    tcolor = ((crt << 16) + (cgt << 8) + cbt) & 0xFFFFFF;
                    pict[nY][nX] = tcolor;

                    // ******************************
                    // Anti anlize from pixel-buffer:
                    // ******************************
                    // 2x2 grid:
                    nY = nY & 0xFFFE;
                    nX = nX & 0xFFFE;

                    // Reset colours:
                    ncols = 4;
                    tRed = 0x00;
                    tBlue = 0x00;
                    tGreen = 0x00;

                    // 2x2 pixels to 1 pixel:
                    for (yi = 0; yi < 2; yi++)
                    {
                        nYt = nY + yi;
                        if ((nYt >= 0) && (nYt < BHEIGHT))
                        {
                            for (xi = 0; xi < 2; xi++)
                            {
                                nXt = nX + xi;
                                if ((nXt >= 0) && (nXt < BWIDTH))
                                {
                                    tcolor = pict[nYt][nXt];
                                    tRed += (tcolor >> 16) & 0xFF;
                                    tGreen += (tcolor >> 8) & 0xFF;
                                    tBlue += tcolor & 0xFF;
                                } // Clip x.
                            } // for xi.
                        } // Clip y.
                    } // for yi.
                    tRed = (tRed / ncols) & 0xFF;
                    tGreen = (tGreen / ncols) & 0xFF;
                    tBlue = (tBlue / ncols) & 0xFF;
                    // End anti anilize.

                    // Convert 8-bit red, green & blue to 32-bit xRGB:
                    tcolor = ((tRed << 16) + (tGreen << 8) + tBlue) & 0x00FFFFFF;

                    // ***********************
                    // Write to screen buffer:
                    // ***********************
                    // 2x2 grid to 1x1 dito:
                    nY = nY >> 1;
                    nX = nX >> 1;
                    if (nY < HEIGHT && nX < WIDTH) {
                        pixelBuffer[nX + nY * WIDTH] = tcolor;
                    }
                } // Z-plot view.
            } // clip - X.
        } // clip - Y.
    } // clip - Z.

    return;
} // IFSplot.

/* --------------------------------------------------------------------- *
        Rotate to view position:
 * --------------------------------------------------------------------- */
void rotateview(void)
{
    t = ryx * zt - ryy * xt;
    xt = ryx * xt + ryy * zt;
    zt = t;

    t = rxx * yt - rxy * zt;
    zt = rxx * zt + rxy * yt;
    yt = t;

    return;
} // rotateview.

/* --------------------------------------------------------------------- *
        Rotate back from view position:
 * --------------------------------------------------------------------- */
void unrotateview(void)
{
    t = ryx * zt - (-ryy) * xt;
    xt = ryx * xt + (-ryy) * zt;
    zt = t;

    t = rxx * yt - (-rxy) * zt;
    zt = rxx * zt + (-rxy) * yt;
    yt = t;

    return;
} // unrotatelight.

/* --------------------------------------------------------------------- *
        Rotate to light position:
 * --------------------------------------------------------------------- */
void rotatelight(void)
{
    t = lryx * zt - lryy * xt;
    xt = lryx * xt + lryy * zt;
    zt = t;

    t = lrxx * yt - lrxy * zt;
    zt = lrxx * zt + lrxy * yt;
    yt = t;

    return;
} // rotatelight.

/* --------------------------------------------------------------------- *
        Rotate back from light position:
 * --------------------------------------------------------------------- */
void unrotatelight(void)
{
    t = lrxx * zt - (-lrxy) * xt;
    xt = lrxx * xt + (-lrxy) * zt;
    zt = t;

    t = lryx * yt - (-lryy) * zt;
    zt = lryx * zt + (-lryy) * yt;
    yt = t;

    return;
} // unrotatelight.

/* --------------------------------------------------------------------- *
        Initiate a new render:
 * --------------------------------------------------------------------- */
void newrender(void)
{
    // Reset parameters:
    btx = tx[0];
    bty = ty[0];
    btz = tz[0];

    bcr = tcr[0];
    bcg = tcg[0];
    bcb = tcb[0];

    dtx = 1.0f;
    dty = 1.0f;
    dtz = 1.0f;

    dcr = tcr[4];
    dcg = tcg[4];
    dcb = tcb[4];

    // Palette index:
    pali = 0;

    // Min & max brightness:
    switch (lightness)
    {
        case 1:
            minbright = (1.0f / 3.0f);
            maxbright = (1.0f / 2.0f);
        break;
        default:
            minbright = 1.0f;
            maxbright = 0.0001f;
        break;
    } // color selector.

    ui = 0;
    xbuf[ui] = 0.0f;
    ybuf[ui] = 0.0f;
    zbuf[ui] = 0.0f;

    return;
} // newrender.

/* --------------------------------------------------------------------- *
        Initiate a new setup:
 * --------------------------------------------------------------------- */
void newsetup(void)
{
    ui = 0;

    // Preset a number of scale ratios:
    scales[0] = sqrt(2.0f / 3.0f);
    scales[1] = sqrt(1.0f / 2.0f);
    scales[2] = (2.0f / 3.0f);
    scales[3] = (phi - 1.0f);
    scales[4] = sqrt(1.0f / 3.0f);
    scales[5] = (1.0f / 2.0f);
    scales[6] = (2.0f - phi);


    //////////////////////////////
    // *** FRACTAL SETUPS ! *** //

    //////////////////////////////

    // 2D Sierpinski squares as ground plate:
    createbackground();

    // Use 'half-note' steps for size modifier:
    t = (1.0f / 64.0f);
    twup = pow(2.0f, t);
    twdwn = pow(2.0f, -t);

    // ********************
    // RANDOMIZE PRESET #31
    // ********************
    //(And that's #00 in the demo, I'm using (n+1) AND 0x1F
    // for print, but it is not the first tree in the list =)

    // First name the tree:
    strcpy(trees[31].name, "Random Tree");

    // Set number of branches for the tree:
    // (First case is 'random branches',
    //  second uses the preset numbers).
    if (selnumbranch == 0)
        numbranch = 2 + RND*RND * 7;
    else
        numbranch = selnumbranch + 1;
    trees[31].branches = numbranch;

    // Randomize flags:
    trees[31].usehig = RNDBOOL;
    trees[31].glblscl = RNDBOOL;
    trees[31].sctrnsl = RNDBOOL;
    trees[31].usetwst = RNDBOOL;

    // Randomize radius and height for the root-stem (stub):
    t = (1.0 + RND);
    trees[31].radius = 0.002f + 0.04f * RND*RND;
    trees[31].height = -t / (4.0f + (12.0f * RND));

    // Randomize branches:
    // (Height, scale, slope and rotation).
    for (i = 0; i < 8; i++)
        randombranch(i);

    // Create colours for the leafs:
    leafcols();

    // And the colours for the stem:
    stemcols();

    // End Fractal!

/// ******* Light & Camera ******* ///

    // Light-angle:
    lryv = -26.0f * rad;
    lrxv = 45.0f * rad;
    LitAng();

    // Calculate position of light source!
    ntx = 0.0f;
    nty = 0.0f;
    ntz = 1.0f;

    // Rotate to position!
    t = lryx * yt - lryy * zt;
    zt = lryx * zt + lryy * yt;
    yt = t;

    t = lrxx * zt - lrxy * xt;
    xt = lrxx * xt + lrxy * zt;
    zt = t;

    // Camera-angle:
    ryv = 0.0f * rad;
    rxv = 0.0f * rad;
    CamAng();
    // Camera-angle.

    // IFS rotators!
    drzv = 0.0f * rad;
    drzx = cos(drzv);
    drzy = sin(drzv);
    dryv = 0.0f * rad;
    dryx = cos(dryv);
    dryy = sin(dryv);

    // Reset parameters!
    btx = tx[0];
    bty = ty[0];
    btz = tz[0];

    bcr = tcr[0];
    bcg = tcg[0];
    bcb = tcb[0];

    dtx = tx[4];
    dty = ty[4];
    dtz = tz[4];

    dcr = tcr[4];
    dcg = tcg[4];
    dcb = tcb[4];

    newrender();

    return;
} // newsetup.

/* --------------------------------------------------------------------- *
        IFS setup for randomizing a branch:
 * --------------------------------------------------------------------- */
void randombranch(int indx)
{
    if (indx == 0)
    {
        // Always set 'stem' to top of the stub:
        branches[31][indx].height = 1.0f;
        // Choose phase for slope angle:
        // Rather point up than sideways for stem-branch (RND^3):
        t = RND*RND*RND;
    }
    else
    {
        // Set height of stem where the brach resides:
        branches[31][indx].height = 1.0f - RND*RND * 0.9f;
        // Choose phase for slope angle:
        t = RND*RND;
    }

    // Set scale:
    branches[31][indx].scale = scales[int(RND * 7)];

    // Set random slope angle:
    angle = 180.0f * rad * t;
    branches[31][indx].leans = sin(angle);
    branches[31][indx].leanc = cos(angle);

    // Set random rotation angle:
    angle = 360.0f * rad * RND;
    branches[31][indx].rotates = cos(angle);
    branches[31][indx].rotatec = sin(angle);

    // Set random twist angle:
    angle = 360.0f * rad * RND;
    branches[31][indx].twistc = cos(angle);
    branches[31][indx].twists = sin(angle);

    return;
} // randombranch.

/* --------------------------------------------------------------------- *
        IFS setup for the ground:
 * --------------------------------------------------------------------- */
void createbackground(void)
{
    // Size of ground square:
    x = grounds[groundsize];
    // Heihgt: (center as is =)
    t = 0.0f;

    // Translation coordinates:
    tx[0]     = -x;
    ty[0]     = t;
    tz[0]     = x;

    tx[1]     = x;
    ty[1]     = t;
    tz[1]     = x;

    tx[2]     = x;
    ty[2]     = t;
    tz[2]     = -x;

    tx[3]     = -x;
    ty[3]     = t;
    tz[3]     = -x;

    // Colours:
    tcr[2] = tcr[0] = 0xFF * RND;
    tcg[2] = tcg[0] = 0xFF * RND;
    tcb[2] = tcb[0] = 0xFF * RND;
    tcr[3] = tcr[1] = 0xFF;
    tcg[3] = tcg[1] = 0xFF;
    tcb[3] = tcb[1] = 0xFF;

    // Scale:
    for (i = 0; i < 4; i++)
        sc[i] = (1.0f / 2.0f);

} // createbackground.

/* --------------------------------------------------------------------- *
        Randomize colours for the leafs:
 * --------------------------------------------------------------------- */
void leafcols(void)
{
    switch (colourmode)
    {
        case FUNKYCOLOURS:
            for (i = 4; i < (ANTAL + 4); i++)
            {
                tcr[i] = 0xFF * RND;
                tcg[i] = 0xFF * RND;
                tcb[i] = 0xFF * RND;
            }
        break; // FUNKYCOLOURS.
        case NORMALCOLOURS:
        default:
            for (i = 4; i < (ANTAL + 4); i++)
            {
                tcr[i] = 0x60 + 160 * RND*RND;
                tcg[i] = 0x80 + 128 * RND;
                tcb[i] = 0x20 + 32 * RND;
            }
        break; // NORMALCOLOURS.
    } // Switch colourmode.
} // leafcols.

/* --------------------------------------------------------------------- *
        Set-up colours for the stem:
 * --------------------------------------------------------------------- */
void stemcols(void)
{
    for (i = 0; i <= ilevels; i++)
    {
        lcr[i] = 0xFF;
        lcg[i] = 0xC0;
        lcb[i] = 0x80;
    }

    lcr[0] = 0x00;
    lcg[0] = 0x80;
    lcb[0] = 0x00;

    lcr[1] = 0x30;
    lcg[1] = 0x90;
    lcb[1] = 0x10;

    lcr[3] = 0x60;
    lcg[3] = 0xA0;
    lcb[3] = 0x30;

    lcr[4] = 0xC0;
    lcg[4] = 0xB0;
    lcb[4] = 0x50;

    lcr[ilevels] = 0x10 * 12;
    lcg[ilevels] = 0x0C * 12;
    lcb[ilevels] = 0x08 * 12;

    lcr[ilevels - 1] = 0x10 * 13;
    lcg[ilevels - 1] = 0x0C * 13;
    lcb[ilevels - 1] = 0x08 * 13;

    lcr[ilevels - 2] = 0x10 * 14;
    lcg[ilevels - 2] = 0x0C * 14;
    lcb[ilevels - 2] = 0x08 * 14;

    lcr[ilevels - 3] = 0x10 * 15;
    lcg[ilevels - 3] = 0x0C * 15;
    lcb[ilevels - 3] = 0x08 * 15;
} // stemcols.

/* --------------------------------------------------------------------- *
        Create light angle:
 * --------------------------------------------------------------------- */
void LitAng(void)
{
    lryx = cos(lryv);
    lryy = sin(lryv);
    lrxx = cos(lrxv);
    lrxy = sin(lrxv);
} // LitAng.

/* --------------------------------------------------------------------- *
        Create camera angle:
 * --------------------------------------------------------------------- */
void CamAng(void)
{
    ryx = cos(ryv);
    ryy = sin(ryv);
    rxx = cos(rxv);
    rxy = sin(rxv);
} // CamAng.

/* --------------------------------------------------------------------- *
            "Manual" =)
 * --------------------------------------------------------------------- */
void manual(void)
{
    // CLS:
    clearscreen(0x00FFF0E0);

    // Head texts:
    printsceneinfo();
    printtreeinfo();
    lcol = 0x00000080;

    float r, g, b;
    unpackColor(lcol, &r, &g, &b);

    drawText(10, 10, "SunCode's 3D Dragon Tree IFS, the Demo, (first version eight edition of May 2005)", r, g, b);
    
    // Keys:
    TEXTBOX(30, 38, 556, 258, 0x00A0A0A0, 0x00D8F8E8);
    lcol = 0x00006000;
    tmp = 48;
    yi = 56;
    xi = 15;
    drawText(tmp - 16, 38, "Keys to use in this demo:", r, g, b);
    drawText(tmp, yi + xi * 0, "[Esc] = Exit program! | [Alt] = exit from render | [SPACE] = Toggle this screen and render mode screen.", r, g, b);
    drawText(tmp, yi + xi * 1, "[R] = Randomize tree. | [A] = Randomize angle of view | [Arrow keys] = Rotate the scene.", r, g, b);
    drawText(tmp, yi + xi * 2, "[N] = Select number of branches, (random, 2, 3, 4, 5, 6, 7 or 8) | [1] - [8] = Randomize single branch.", r, g, b);
    drawText(tmp, yi + xi * 3, "[C] = Clear buffers, (useful after, for example a palette selection to clear out old pixels)", r, g, b);
    drawText(tmp, yi + xi * 4, "[V] = Clear view, (useful after a selection, also removes my SunCode 'tag' from the screen =)", r, g, b);
    drawText(tmp, yi + xi * 5, "[X] = Bigger tree. | [S] = Smaller tree. | [D] = Thicker tree. | [F] = Thinner tree.", r, g, b);
    drawText(tmp, yi + xi * 6, "[Page Down] / [Page Up] = Zoom in / out. | [Home] = Reset zoom and angle of view.", r, g, b);
    drawText(tmp, yi + xi * 7, "[I] = View information. | [T] = Test render, a white sheet, are pixels still written?, this will show that.", r, g, b);
    drawText(tmp, yi + xi * 8, "[B] = Background, (on skyblue, on blue, on black, off black, off white). | [G] = Size of ground, (B, M, S).", r, g, b);
    drawText(tmp, yi + xi * 9, "[P] = Randomize colours for foliage. | [E] = Extra foliage off, on, solo. | [U] = Use logs, cubes or sphears.", r, g, b);
    drawText(tmp, yi + xi * 10, "[W] = Turn the whiter shade of pale to normal, flourescent or filament. | [L] = Toggle lights = light or dark", r, g, b);
    drawText(tmp, yi + xi * 11, "[O] = Turn FUNKYCOLOURS on/off.", r, g, b);
    drawText(tmp, yi + xi * 12, "[M] = More trees. | [F1] - [F4] = Toggle flags: 'Use heights', 'Global scale', 'Scale by height' and 'Use twist'.", r, g, b);

    // Colours:
    viewcols();

    // Notes:
    TEXTBOX(322, 270, 784, 568, 0x00A0A0A0, 0x00D8E8F8);
    tmp = 340;
    lcol = 0x00600000;
    drawText(tmp - 16, 270, "Notes:", r, g, b);
    drawText(tmp, 290, "Most trees looks awful but some don't and those are the ones we are looking for ;)", r, g, b);
    drawText(tmp, 310, "A good idea is to render the image once to fill out the shadow-map and then press [C] to clear", r, g, b);
    drawText(tmp, 322, "out the pixel and z-buffers and [V] to clear the view. And then render the image once again", r, g, b);
    drawText(tmp, 334, "using the previously compleated shadow-map. This will make the shadows better.", r, g, b);
    drawText(tmp, 354, "To save the current image you can press [Print Screen] and then [Esc] (to exit), and then the", r, g, b);
    drawText(tmp, 366, "image is, (hopefully) on your clipboard, ready to get pasted into any graphics tool.", r, g, b);
    drawText(tmp, 386, "The colours are choosen by random. Press [O] and you will enter 'FUNKYCOLOURS' mode. If", r, g, b);
    drawText(tmp, 398, "you do not like the current set of colours, then simply press [P] until random finds a set you like.", r, g, b);
    drawText(tmp, 550, "This program is part of the public domain, (PD), distribute and make copys freely.", r, g, b);

    spacemess();

    return;
} // manual.

/* --------------------------------------------------------------------- *
        Press [V] to clear view message:
 * --------------------------------------------------------------------- */
void clearViewmess(void)
{
    float r, g, b;
    unpackColor(txcol[showbackground], &r, &g, &b);
    drawText(360, 2, "Press [V] to clear view.", r, g, b);
    return;
} // clearViewmess.

/* --------------------------------------------------------------------- *
        View foliage coloursation:
 * --------------------------------------------------------------------- */
void viewcols(void)
{
    TEXTBOX(30, 270, 312, 568, 0x00A0A0A0, bgcol[showbackground]);
    lcol = txcol[showbackground];
    float r, g, b;
    unpackColor(lcol, &r, &g, &b);
    drawText(32, 270, "Foliage coloursation:", r, g, b);
    drawText(132, 556, "Press [P] to randomize", r, g, b);
    writecols();
    return;
} // viewcols.

/* --------------------------------------------------------------------- *
        Write text & boxes for the above function:
        Coz this one is also use in the render screen
    whitout the background and leadtexts:
 * --------------------------------------------------------------------- */
void writecols(void)
{
    tmp = 32;
    for (i = 0; i < trees[treeinuse].branches; i++)
    {
        snprintf(stringbuf, sizeof(stringbuf), "Branch # %i;", i + 1);
        float r, g, b;
        unpackColor(txcol[showbackground], &r, &g, &b);
        drawText(48, 290 + i * tmp, stringbuf, r, g, b);

        tcolor = ((tcr[4 + i] << 16) + (tcg[4 + i] << 8) + tcb[4 + i]) & 0x00FFFFFF;
        FILLBOX(160, 290 + i * tmp, 306, 318 + i * tmp, tcolor);
    }
    return;
} // writecols.

/* --------------------------------------------------------------------- *
        Press space to ... :
 * --------------------------------------------------------------------- */
void spacemess(void)
{
    float r, g, b;
    unpackColor(txcol[showbackground], &r, &g, &b);
    if (itersdone == 0)
    {
        snprintf(stringbuf, sizeof(stringbuf), "Press space to start render! %i iterations done, %i image pixels, %i shadow pixels.", itersdone, pixelswritten, shadowswritten);
        drawText(64, 574, stringbuf, r, g, b);
    }
    else
    {
        snprintf(stringbuf, sizeof(stringbuf), "Press space to continue render! %i iterations done, %i image pixels, %i shadow pixels.", itersdone, pixelswritten, shadowswritten);
        drawText(64, 574, stringbuf, r, g, b);
    }
} // spacemess.

/* --------------------------------------------------------------------- *
        Print number of pixels written:
 * --------------------------------------------------------------------- */
void pixelsmess(void)
{
    snprintf(stringbuf, sizeof(stringbuf), "%i iterations done, %i image pixels, %i shadow pixels.  (press [V] to clear view).", itersdone, pixelswritten, shadowswritten);
    float r, g, b;
    unpackColor(txcol[showbackground], &r, &g, &b);
    drawText(192, 2, stringbuf, r, g, b);
} // pixelsmess.

/* --------------------------------------------------------------------- *
        Print info about scene:
 * --------------------------------------------------------------------- */
void printsceneinfo(void)
{
    TEXTBOX(700, 64, 784, 114, 0x00A0A0A0, 0x00FFFFFF);

    tmp = 8;
    float r, g, b;
    unpackColor(0x00000080, &r, &g, &b);
    drawText(701, 65 + 0 * tmp, textbrmess[selnumbranch], r, g, b);
    drawText(701, 65 + 1 * tmp, textbgmess[showbackground], r, g, b);
    drawText(701, 65 + 2 * tmp, textgrmess[groundsize], r, g, b);
    drawText(701, 65 + 3 * tmp, textlight[lightness], r, g, b);
    drawText(701, 65 + 4 * tmp, textpales[whitershade], r, g, b);
    drawText(701, 65 + 5 * tmp, textfunky[colourmode], r, g, b);
    return;
} // printcoordinfo.

/* --------------------------------------------------------------------- *
        Print info about scene:
 * --------------------------------------------------------------------- */
void printtreeinfo(void)
{
    TEXTBOX(670, 10, 784, 60, 0x00A0A0A0, 0x00FFFFFF);
    lcol = 0x00000080;
    tmp = 12;
    float r, g, b;
    unpackColor(lcol, &r, &g, &b);
    snprintf(stringbuf, sizeof(stringbuf), "Now growing tree #%02i.", ((treeinuse + 1) & 0x1F));
    drawText(672, 10 + tmp * 0, stringbuf, r, g, b);
    snprintf(stringbuf, sizeof(stringbuf),"Name:%s.", trees[treeinuse].name);
    drawText(672, 10 + tmp * 1, stringbuf, r, g, b);
    snprintf(stringbuf, sizeof(stringbuf),"%s.", textbrmess[trees[treeinuse].branches - 1]);
    drawText(672, 10 + tmp * 2, stringbuf, r, g, b);
    snprintf(stringbuf, sizeof(stringbuf),"Uh=%i Gs=%i Sbh=%i Ut=%i", trees[treeinuse].usehig, trees[treeinuse].glblscl, trees[treeinuse].sctrnsl, trees[treeinuse].usetwst);
    drawText(672, 10 + tmp * 3, stringbuf, r, g, b);
    return;
} // printcoordinfo.

/* --------------------------------------------------------------------- *
        Clear pixel & Z-buffers:
 * --------------------------------------------------------------------- */
void clearallbufs(uint32_t RGBdata)
{
    // Clear pixel counter for shadow map:
    shadowswritten = 0;

    if (lockshadow)
        trees[treeinuse].radius /= sqrt(twup);

    lockshadow = false;

    // Light-source Z-buffer:
    for (p=0; p < LHEIGHT; p++)
        for (r=0; r < LWIDTH; r++)
            light[p][r] = 0;

    // Image pixel & z-buffer:
    clearscreenbufs(RGBdata);

    return;
} // clearallbufs.

/* --------------------------------------------------------------------- *
        Clear pixel & Z-buffer: (screen only, NOT shadow z-buffer)
 * --------------------------------------------------------------------- */
void clearscreenbufs(uint32_t RGBdata)
{
    // Clear iteration and pixel counter:
    itersdone = 0;
    pixelswritten = 0;

    for (p=0; p < BHEIGHT; p++)
    {
        for (r=0; r < BWIDTH; r++)
        {
            // pixels-buffer:
            pict[p][r] = RGBdata;
            // View Z-buffer:
            bpict[p][r] = 0;
        }
    }
    return;
} // clearscreenbufs.

/* --------------------------------------------------------------------- *
        Initialize text output:
 * --------------------------------------------------------------------- */
 void unpackColor(unsigned int col, float *r, float *g, float *b) {
    *r = ((col >> 16) & 0xFF) / 255.0f;
    *g = ((col >> 8) & 0xFF) / 255.0f;
    *b = (col & 0xFF) / 255.0f;
}

void drawText(float x, float y, const char *text, float r, float g, float b) {
    char buffer[99999]; // output buffer
    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer); // each quad is 4 vertices, 2D positions
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

/* --------------------------------------------------------------------- *
        Down and below: "los graphicos"!

            First: "clean the screen" =)
 * --------------------------------------------------------------------- */
void clearscreen(uint32_t RGBdata)
{
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            pixelBuffer[y * WIDTH + x] = RGBdata;
        }
    }
}

/* --------------------------------------------------------------------- *
        picture this:
 * --------------------------------------------------------------------- */
void showpic(void)
{
    int x, y, xx, yy;

    for (y = 0; y < HEIGHT; y++)
    {
        yy = y << 1;
        for (x = 0; x < WIDTH; x++)
        {
            xx = x << 1;

            tcolor = pict[yy][xx];
            tRed = (tcolor >> 16) & 0xFF;
            tGreen = (tcolor >> 8) & 0xFF;
            tBlue = tcolor & 0xFF;

            tcolor = pict[yy + 1][xx];
            tRed += (tcolor >> 16) & 0xFF;
            tGreen += (tcolor >> 8) & 0xFF;
            tBlue += tcolor & 0xFF;

            tcolor = pict[yy + 1][xx + 1];
            tRed += (tcolor >> 16) & 0xFF;
            tGreen += (tcolor >> 8) & 0xFF;
            tBlue += tcolor & 0xFF;

            tcolor = pict[yy][xx + 1];
            tRed += (tcolor >> 16) & 0xFF;
            tGreen += (tcolor >> 8) & 0xFF;
            tBlue += tcolor & 0xFF;

            tRed = (tRed >> 2) & 0xFF;
            tGreen = (tGreen >> 2) & 0xFF;
            tBlue = (tBlue >> 2) & 0xFF;

            tcolor = ((tRed << 16) + (tGreen << 8) + tBlue) & 0x00FFFFFF;
            pixelBuffer[y * WIDTH + x] = tcolor;
        }
    }

    return;
}

/* --------------------------------------------------------------------- *
        Create a palette:
 * --------------------------------------------------------------------- */
void CreatePalette(void)
{
    bool invert, vertin, lightobject, heatvawe, sinvawe, bakwrds;
    int fade, i;
    uint32_t tc;
    float fdout, fdin, fdout2, fdin2, fdouts, fdins, ufade0, ufade1, ufade2, ufade3, ampl;
    float rf, gf, bf, freq, rl, gl, bl;

    freq = 1.0f + RND * RND * RND * 256.0f;
    rf = freq * (1.0f + RND * pi);
    gf = freq * (1.0f + RND * pi);
    bf = freq * (1.0f + RND * pi);

    // Randomize palette-modes:
    invert = (RND > 0.5f);
    lightobject = (RND > 0.75f);
    vertin = (RND > 0.75f);
    heatvawe = (RND > 0.95f);
    sinvawe = (RND > 0.75f);
    bakwrds = (RND > 0.5f);

    fade = int(RND * 2);
    for (i = 0; i != PALSIZE; i++)
    {
        fdout = float(PALSIZE - i) / PALSIZE;
        if (bakwrds)
            fdout = float(i) / PALSIZE;

        fdout2 = fdout * fdout;
        fdouts = sqrt(fdout);

        fdin = 1.0f - fdout;
        fdin2 = 1.0f - fdouts;
        fdins = 1.0f - fdout2;

        ufade0 = fdout;
        ufade1 = fdins;
        ufade2 = fdout2;

        freq = rf * ufade0 * sqrt(ufade0);
        rl = (1.0f + cos(freq)) * 0.5f;
        freq = gf * ufade0 * sqrt(ufade0);
        gl = (1.0f + cos(freq)) * 0.5f;
        freq = bf * ufade0 * sqrt(ufade0);
        bl = (1.0f + cos(freq)) * 0.5f;

        if (lightobject)
            if (vertin)
            {
                length = sqrt(rl*rl + gl*gl + bl*bl) * 2.0f;
                rl = ((1.0f + rl) / length) * fdin;
                gl = ((1.0f + gl) / length) * fdin;
                bl = ((1.0f + bl) / length) * fdin;
            }
            else
                rl = gl = bl = fdin2;

        if (heatvawe)
        {
            freq = fdin2 * pii;
            rl = (1.0f - ((1.0f + sin(freq + rad * 240.0f)) * 0.5f * fdout)) * fdins;
            gl = (1.0f - ((1.0f + sin(freq + rad * 120.0f)) * 0.5f * fdout)) * fdins;
            bl = (1.0f - ((1.0f + sin(freq)) * 0.5f * fdout)) * fdins;
        }//heatvawe.

        if (sinvawe)
        {
            rl *= (1.0f + sin(fdout * pii * 4.1f)) / 2.0f;
            gl *= (1.0f + sin(fdout * pii * 4.2f)) / 2.0f;
            bl *= (1.0f + sin(fdout * pii * 4.3f)) / 2.0f;
        }//sinvawe.

        if (invert && vertin)
        {
            rl = (2.0f - rl) / 2.0f;
            gl = (2.0f - gl) / 2.0f;
            bl = (2.0f - bl) / 2.0f;
        }
        else if (invert)
        {
            rl = 1.0f - rl;
            gl = 1.0f - gl;
            bl = 1.0f - bl;
        } // Inverts.

        // Calibrate luminousity:
        amp = sqrt(rl*rl + gl*gl + bl*bl);
        if (amp > 0.0f)
        {
            rl /= amp;
            gl /= amp;
            bl /= amp;
        }
        else
        {
            rl = 0.5f;
            gl = 0.5f;
            bl = 0.5f;
        }
        // Calibrate luminousity.

        tc = int(rl * float(0x01000000)) & 0x00ff0000;
        tc += int(gl * float(0x00010000)) & 0x0000ff00;
        tc += int(bl * float(0x00000100)) & 0x000000ff;
        lpCols[i] = tc;
    }
    return;
}//Create palette.

/* --------------------------------------------------------------------- *
        View palette:
 * --------------------------------------------------------------------- */
void ShowPalette(int mode)
{
    int pr;
    int pp;
    int pz;

    // Draw border:
    FILLBOX(WIDTH - 306, 294, WIDTH - 46, 554, 0x00A0A0A0);

    tmp = WIDTH - 304;
    switch (mode)
    {
        case SERP:
            for (pr = (256 - 1); pr >= 0; pr--)
                for (pp = 0; pp < 256; pp++)
                    pixelBuffer[(HEIGHT - 304 + pr) * WIDTH + tmp + pp] = lpCols[(int(pp * (PALSIZE / 256)) | int(pr * (PALSIZE / 256)))];
        break;

        case ABSZ:
            for (pr = (256 - 1); pr >= 0; pr--)
                for (pp = 0; pp < 256; pp++)
                {
                    pz = 1 + int((sqrt((pr - 128) * (pr - 128) + (pp - 128) * (pp - 128)) / 725) * PALSIZE);
                    pz = sqrt(pz) * sqrt(PALSIZE);
                    pixelBuffer[(HEIGHT - 304 + pr) * WIDTH + tmp + pp] = lpCols[PALSIZE - (pz & (PALSIZE - 1))];
                }
        break;

        case HOTB:
        default:
        break;
    }

    return;
}// Show palette.

/* --------------------------------------------------------------------- *
        Draw a line:
 * --------------------------------------------------------------------- */
void drawLine(void)
{
    long double llen, ldx, ldy;
    long int lpx, lpy, lnum;

    // clip to (-4 / 3) < x < (4 / 3) / -1 < y < 1
    if (fabs(lxs) > RATIO)
    {
        temp = RATIO / fabs(lxs);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lxe) > RATIO)
    {
        temp = RATIO / fabs(lxe);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    if (fabs(lys) > 1.0f)
    {
        temp = 1.0f / fabs(lys);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lye) > 1.0f)
    {
        temp = 1.0f / fabs(lye);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    // clip ends.

    ldx = lxe - lxs;
    ldy = lye - lys;
    llen = sqrt(ldx*ldx + ldy*ldy);
    ldx = ldx / llen;
    ldy = -ldy / llen;
    lxs = lxs * MIDX;
    lys = -lys * MIDY;
    lnum = int(llen * MIDY) + 1;

    do
    {
        lpx = int(MIDX + lxs);
        lpy = int(MIDY + lys);
        if (lpx >= 0 && lpx < WIDTH && lpy >= 0 && lpy < HEIGHT) {
            pixelBuffer[lpy * WIDTH + lpx] = lcol;
        }
        lxs += ldx;
        lys += ldy;
    } while (--lnum);

    return;
} // drawLine.

/* --------------------------------------------------------------------- *
        Draw a multi-color line:
 * --------------------------------------------------------------------- */
void drawMulticolLine(void)
{
    long double llen, ldx, ldy;
    long int lpx, lpy, lnum;

    // clip to (-4 / 3) < x < (4 / 3) / -1 < y < 1
    if (fabs(lxs) > RATIO)
    {
        temp = RATIO / fabs(lxs);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lxe) > RATIO)
    {
        temp = RATIO / fabs(lxe);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    if (fabs(lys) > 1.0f)
    {
        temp = 1.0f / fabs(lys);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lye) > 1.0f)
    {
        temp = 1.0f / fabs(lye);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    // clip ends.

    ldx = lxe - lxs;
    ldy = lye - lys;
    llen = sqrt(ldx*ldx + ldy*ldy);
    ldx = ldx / llen;
    ldy = -ldy / llen;
    lxs = lxs * MIDY;
    lys = -lys * MIDY;
    lnum = int(llen * MIDY) + 1;

    do
    {
        lpx = int(MIDX + lxs);
        lpy = int(MIDY + lys);
        if (lpx >= 0 && lpx < WIDTH && lpy >= 0 && lpy < HEIGHT) {
            pixelBuffer[lpy * WIDTH + lpx] = lpCols[lnum & (PALSIZE - 1)];
        }
        lxs += ldx;
        lys += ldy;
    } while (--lnum);

    return;
} // drawMulticolLine.

/* --------------------------------------------------------------------- *
        Draw a box from floats: (hi-tech mode =)
 * --------------------------------------------------------------------- */
void drawBox(void)
{
    long int lpx, lpy, lnx, lny;

    // clip to (-4 / 3) < x < (4 / 3) / -1 < y < 1
    if (fabs(lxs) > RATIO)
    {
        temp = RATIO / fabs(lxs);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lxe) > RATIO)
    {
        temp = RATIO / fabs(lxe);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    if (fabs(lys) > 1.0f)
    {
        temp = 1.0f / fabs(lys);
        lxs = (fabs(lxs) * temp) * SGN(lxs);
        lys = (fabs(lys) * temp) * SGN(lys);
    }
    if (fabs(lye) > 1.0f)
    {
        temp = 1.0f / fabs(lye);
        lxe = (fabs(lxe) * temp) * SGN(lxe);
        lye = (fabs(lye) * temp) * SGN(lye);
    }
    // clip ends.

    if (lxs > lxe)
        temp = lxs, lxs = lxe, lxe = temp;
    if (lys > lye)
        temp = lys, lys = lye, lye = temp;

    lnx = int((lxe - lxs) * MIDY);
    lny = int((lye - lys) * MIDY);
    lpx = MIDX + int(MIDY * lxs);
    lpy = MIDY - int(MIDY * lys);

    do
    {
        for (i = 0; i < lnx; i++) {
            if (lpx + i >= 0 && lpx + i < WIDTH && lpy >= 0 && lpy < HEIGHT) {
                pixelBuffer[lpy * WIDTH + lpx + i] = lcol;
            }
        }
        --lpy;
    } while (--lny != 0);

    return;
} // drawBox.

/* --------------------------------------------------------------------- *
        Draw a box: (from ints)
 * --------------------------------------------------------------------- */
void drawBoxi(void)
{
    int boxw, boxh;

    // clip:
    if (lixs < 0)
        lixs = 0;
    if (lixs >= WIDTH)
        lixs = WIDTH - 1;

    if (lixe < 0)
        lixe = 0;
    if (lixe >= WIDTH)
        lixe = WIDTH - 1;

    if (liys < 0)
        liys = 0;
    if (liys >= HEIGHT)
        liys = HEIGHT - 1;

    if (liye < 0)
        liye = 0;
    if (liye >= HEIGHT)
        liye = HEIGHT - 1;
    // clip ends.

    // Swap coords?
    if (lixs > lixe)
    {
        tmp = lixs;
        lixs = lixe;
        lixe = tmp;
    }
    if (liys > liye)
    {
        tmp = liys;
        liys = liye;
        liye = tmp;
    }

    // Calculate width & height of box:
    boxw = lixe - lixs;
    boxh = liye - liys;

    do
    {
        for (i = 0; i < boxw; i++) {
            pixelBuffer[liys * WIDTH + lixs + i] = lcol;
        }
        liys++;
    } while (--boxh > 0);

    return;
} // drawBoxi.

///////////////////////////////////////////////////////////////////////////
/* --------------------------------------------------------------------- *
                    SPECIAL MATHS
 * --------------------------------------------------------------------- */
///////////////////////////////////////////////////////////////////////////

/* --------------------------------------------------------------------- *
        Get level function
 * --------------------------------------------------------------------- */
double getlevel(double xmin, double xmax, double ystart, double yend, double x, double Q)
{
    double lev;

    lev = (x - xmin) / (xmax - xmin);
    lev = pow(lev, Q);
    lev = ystart + lev * (ystart - yend);

    return (lev);
} // get level.

/* --------------------------------------------------------------------- *
		Load trees:
 * --------------------------------------------------------------------- */
void loadtrees ( void )
{

#define USESETUPS
#if defined USESETUPS

	numbranch = 2;
	t = sqrtl ( 1.0f / 2.0f );
  for ( j = 0; j < NUMTREES; j++ )
  {
		// Preset default branches for one tree:
		angle = 45.0f * rad;
	  for ( i = 0; i < 8; i++ )
	  {
			branches [ j ] [ i ].height = 0.5f;
			branches [ j ] [ i ].scale = t;
			branches [ j ] [ i ].leanc = cosl ( angle );
			branches [ j ] [ i ].leans = sinl ( angle );
			branches [ j ] [ i ].rotatec = cosl ( angle * 3.0f + angle * i * 3.0f );
			branches [ j ] [ i ].rotates = sinl ( angle * 3.0f + angle * i * 3.0f );
			branches [ j ] [ i ].twistc = cosl ( angle );
			branches [ j ] [ i ].twists = sinl ( angle );
      angle = - angle;
    }
		// Preset default trees:
		strcpy( trees [ j ].name, "Default tree" );
		trees [ j ].branches = numbranch;
		trees [ j ].usehig = false;
		trees [ j ].glblscl = false;
		trees [ j ].sctrnsl = false;
		trees [ j ].usetwst = false;
		trees [ j ].radius = 0.011f;
		trees [ j ].height = -0.235f;
		trees [ j ].stem = branches [ i ] [ 0 ];
		trees [ j ].branch_1 = branches [ j ] [ 1 ];
		trees [ j ].branch_2 = branches [ j ] [ 2 ];
		trees [ j ].branch_3 = branches [ j ] [ 3 ];
		trees [ j ].branch_4 = branches [ j ] [ 4 ];
		trees [ j ].branch_5 = branches [ j ] [ 5 ];
		trees [ j ].branch_6 = branches [ j ] [ 6 ];
		trees [ j ].branch_7 = branches [ j ] [ 7 ];
  }

	numbranch = 3;
  i = 0;
	strcpy( trees [ i ].name, "Sierpinski Tree" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = true;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = false;
	trees [ i ].radius = 0.005f;
	trees [ i ].height = -0.35f;
	trees [ i ].stem = branches [ 0 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	t = ( 1.0f / 2.0f );
	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = t;
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );
  angle = 90.0f * rad;
  branches [ i ] [ 0 ].twistc = cosl ( angle );
  branches [ i ] [ 0 ].twists = sinl ( angle );

	branches [ i ] [ 1 ].height = 0.5f;
	angle = 120.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );

	branches [ i ] [ 2 ].height = 0.5f;
	angle = 120.0f * rad;
	branches [ i ] [ 2 ].leanc = cosl ( angle );
	branches [ i ] [ 2 ].leans = sinl ( angle );
	angle = 180.0f * rad;
	branches [ i ] [ 2 ].rotatec = cosl ( angle );
	branches [ i ] [ 2 ].rotates = sinl ( angle );

	numbranch = 4;
  i = 1;
	strcpy( trees [ i ].name, "Sierps tetra" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = true;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = false;
	trees [ i ].radius = 0.005f;
	trees [ i ].height = -0.35f;
	trees [ i ].stem = branches [ 0 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	t = ( 1.0f / 2.0f );
	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = t;
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );
  angle = 90.0f * rad;
  branches [ i ] [ 0 ].twistc = cosl ( angle );
  branches [ i ] [ 0 ].twists = sinl ( angle );

	branches [ i ] [ 1 ].height = 0.5f;
	angle = 110.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 60.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );

	branches [ i ] [ 2 ].height = 0.5f;
	angle = 110.0f * rad;
	branches [ i ] [ 2 ].leanc = cosl ( angle );
	branches [ i ] [ 2 ].leans = sinl ( angle );
	angle = 180.0f * rad;
	branches [ i ] [ 2 ].rotatec = cosl ( angle );
	branches [ i ] [ 2 ].rotates = sinl ( angle );

	branches [ i ] [ 3 ].height = 0.5f;
	angle = 110.0f * rad;
	branches [ i ] [ 3 ].leanc = cosl ( angle );
	branches [ i ] [ 3 ].leans = sinl ( angle );
	angle = 300.0f * rad;
	branches [ i ] [ 3 ].rotatec = cosl ( angle );
	branches [ i ] [ 3 ].rotates = sinl ( angle );

	numbranch = 2;
  i = 2;
	strcpy( trees [ i ].name, "L�vy curve Tree" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = true;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = false;
	trees [ i ].radius = 0.015f;
	trees [ i ].height = -0.23f;
	trees [ i ].stem = branches [ 1 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	t = sqrtl ( 1.0f / 2.0f );
	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = t;
	angle = 45.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );

	angle = 45.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 180.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );

	numbranch = 2;
  i = 3;
	strcpy( trees [ i ].name, "Y-Tree!" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = true;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = true;
	trees [ i ].radius = 0.013f;
	trees [ i ].height = -0.25f;
	trees [ i ].stem = branches [ 2 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	t = sqrtl ( 1.0f / 2.0f );
	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = t;
	angle = 45.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );
  angle = 90.0f * rad;
  branches [ i ] [ 0 ].twistc = cosl ( angle );
  branches [ i ] [ 0 ].twists = sinl ( angle );

	angle = -45.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );
  angle = -90.0f * rad;
  branches [ i ] [ 1 ].twistc = cosl ( angle );
  branches [ i ] [ 1 ].twists = sinl ( angle );

	numbranch = 4;
  i = 4;
	strcpy( trees [ i ].name, "Paratrooper" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = true;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = false;
	trees [ i ].radius = 0.010f;
	trees [ i ].height = -0.375f;
	trees [ i ].stem = branches [ 2 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	t = ( 1.0f / 2.0f );
	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = t;
	angle = 45.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );

	angle = 45.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 90.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );

	angle = 45.0f * rad;
	branches [ i ] [ 2 ].leanc = cosl ( angle );
	branches [ i ] [ 2 ].leans = sinl ( angle );
	angle = 180.0f * rad;
	branches [ i ] [ 2 ].rotatec = cosl ( angle );
	branches [ i ] [ 2 ].rotates = sinl ( angle );

	angle = 45.0f * rad;
	branches [ i ] [ 3 ].leanc = cosl ( angle );
	branches [ i ] [ 3 ].leans = sinl ( angle );
	angle = 270.0f * rad;
	branches [ i ] [ 3 ].rotatec = cosl ( angle );
	branches [ i ] [ 3 ].rotates = sinl ( angle );

	numbranch = 5;
  i = 5;
	strcpy( trees [ i ].name, "Twister spruce" );
	trees [ i ].branches = numbranch;
	trees [ i ].usehig = false;
	trees [ i ].glblscl = false;
	trees [ i ].sctrnsl = false;
	trees [ i ].usetwst = true;
	trees [ i ].radius = 0.005f;
	trees [ i ].height = -0.26f;
	trees [ i ].stem = branches [ 2 ] [ 0 ];
	trees [ i ].branch_1 = branches [ i ] [ 1 ];
	trees [ i ].branch_2 = branches [ i ] [ 2 ];
	trees [ i ].branch_3 = branches [ i ] [ 3 ];
	trees [ i ].branch_4 = branches [ i ] [ 4 ];
	trees [ i ].branch_5 = branches [ i ] [ 5 ];
	trees [ i ].branch_6 = branches [ i ] [ 6 ];
	trees [ i ].branch_7 = branches [ i ] [ 7 ];

	branches [ i ] [ 0 ].height = 1.0f;
	branches [ i ] [ 0 ].scale = ( phi - 1.0f );
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].leanc = cosl ( angle );
	branches [ i ] [ 0 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 0 ].rotatec = cosl ( angle );
	branches [ i ] [ 0 ].rotates = sinl ( angle );
    angle = 22.5f * rad;
    branches [ i ] [ 0 ].twistc = cosl ( angle );
    branches [ i ] [ 0 ].twists = sinl ( angle );

 	t = ( 2.0f - phi );
	branches [ i ] [ 1 ].height = 0.5f;
	branches [ i ] [ 1 ].scale = t;
	angle = 90.0f * rad;
	branches [ i ] [ 1 ].leanc = cosl ( angle );
	branches [ i ] [ 1 ].leans = sinl ( angle );
	angle = 0.0f * rad;
	branches [ i ] [ 1 ].rotatec = cosl ( angle );
	branches [ i ] [ 1 ].rotates = sinl ( angle );
    angle = 0.0f * rad;
    branches [ i ] [ 1 ].twistc = cosl ( angle );
    branches [ i ] [ 1 ].twists = sinl ( angle );

	branches [ i ] [ 2 ].height = 0.5f;
	branches [ i ] [ 2 ].scale = t;
	angle = 90.0f * rad;
	branches [ i ] [ 2 ].leanc = cosl ( angle );
	branches [ i ] [ 2 ].leans = sinl ( angle );
	angle = 90.0f * rad;
	branches [ i ] [ 2 ].rotatec = cosl ( angle );
	branches [ i ] [ 2 ].rotates = sinl ( angle );
    angle = 0.0f * rad;
    branches [ i ] [ 2 ].twistc = cosl ( angle );
    branches [ i ] [ 2 ].twists = sinl ( angle );

	branches [ i ] [ 3 ].height = 0.5f;
	branches [ i ] [ 3 ].scale = t;
	angle = 90.0f * rad;
	branches [ i ] [ 3 ].leanc = cosl ( angle );
	branches [ i ] [ 3 ].leans = sinl ( angle );
	angle = 180.0f * rad;
	branches [ i ] [ 3 ].rotatec = cosl ( angle );
	branches [ i ] [ 3 ].rotates = sinl ( angle );
    angle = 0.0f * rad;
    branches [ i ] [ 3 ].twistc = cosl ( angle );
    branches [ i ] [ 3 ].twists = sinl ( angle );

	branches [ i ] [ 4 ].height = 0.5f;
	branches [ i ] [ 4 ].scale = t;
	angle = 90.0f * rad;
	branches [ i ] [ 4 ].leanc = cosl ( angle );
	branches [ i ] [ 4 ].leans = sinl ( angle );
	angle = 270.0f * rad;
	branches [ i ] [ 4 ].rotatec = cosl ( angle );
	branches [ i ] [ 4 ].rotates = sinl ( angle );
    angle = 0.0f * rad;
    branches [ i ] [ 4 ].twistc = cosl ( angle );
    branches [ i ] [ 4 ].twists = sinl ( angle );

#endif // USESETUPS

// 	if ( WASERROR == opensource ( "trees.IFS" ) )
//   {
//   	textline ( 7, 5, "** ERROR **", BIGFONT, 0x008080FF );
//   	textline ( 6, 4, "** ERROR **", BIGFONT, 0x000000FF );
//   	textline ( 7, 25, "The preset file 'Trees.IFS' was not found", BIGFONT, 0x00808080 );
//   	textline ( 6, 24, "The preset file 'Trees.IFS' was not found.", BIGFONT, 0x00FFFFFF );
//   	textline ( 7, 40, "Use the compiler to create a new one.", BIGFONT, 0x00808080 );
//   	textline ( 6, 39, "Use the compiler to create a new one.", BIGFONT, 0x00FFFFFF );
//   	textline ( 7, 70, "Press [Escape] to exit!", BIGFONT, 0x00808080 );
//   	textline ( 6, 69, "Press [Escape] to exit!", BIGFONT, 0x00FFFFFF );
//     while ( 0 <= GetAsyncKeyState ( VK_ESCAPE ) )
//     	;
// 		PostMessage ( hwnd, WM_CLOSE, 0, 0 );
//   }

	// Load the preset trees and
  // branches from file-buffer:
  loadtree ( );

} // loadtrees.
/* -------------------------------------------------------------------- *
			Open and read source file:
 * --------------------------------------------------------------------- */
int opensource ( const char * fname )
{
  if ( NULL == ( infile = fopen ( fname, "r" ) ) )
        return ( WASERROR );

 	if ( 0 == ( filesize = fread ( tree, sizeof ( char ), sizeof ( tree [ 0 ] ) * 31, infile ) ) )
        return ( WASERROR );

  return ( WASNOERROR );
} // opensource.


void loadtree ( void )
{
  for ( i = 0; i < 31; i++ )
  printf("Loading tree %d\n", i);
  {
        memcpy(trees[i].name, tree[i].name, sizeof(trees[i].name) - 1);
        trees[i].name[sizeof(trees[i].name) - 1] = '\0';
        trees [ i ].name [ 15 ] = 0;
		trees [ i ].branches = tree [ i ].branches;
		trees [ i ].usehig = tree [ i ].usehig;
		trees [ i ].glblscl = tree [ i ].glblscl;
		trees [ i ].sctrnsl = tree [ i ].sctrnsl;
		trees [ i ].usetwst = tree [ i ].usetwst;
		trees [ i ].radius = tree [ i ].radius;
 		trees [ i ].height = tree [ i ].height;
        
 		branches [ i ] [ 0 ].height = tree [ i ].height0;
		branches [ i ] [ 0 ].scale = tree [ i ].scale0;
 		branches [ i ] [ 0 ].leanc = tree [ i ].leanc0;
 		branches [ i ] [ 0 ].leans = tree [ i ].leans0;
 		branches [ i ] [ 0 ].rotatec = tree [ i ].rotatec0;
 		branches [ i ] [ 0 ].rotates = tree [ i ].rotates0;
 		branches [ i ] [ 0 ].twistc = tree [ i ].twistc0;
 		branches [ i ] [ 0 ].twists = tree [ i ].twists0;

 		branches [ i ] [ 1 ].height = tree [ i ].height1;
		branches [ i ] [ 1 ].scale = tree [ i ].scale1;
 		branches [ i ] [ 1 ].leanc = tree [ i ].leanc1;
 		branches [ i ] [ 1 ].leans = tree [ i ].leans1;
 		branches [ i ] [ 1 ].rotatec = tree [ i ].rotatec1;
 		branches [ i ] [ 1 ].rotates = tree [ i ].rotates1;
 		branches [ i ] [ 1 ].twistc = tree [ i ].twistc1;
 		branches [ i ] [ 1 ].twists = tree [ i ].twists1;

 		branches [ i ] [ 2 ].height = tree [ i ].height2;
		branches [ i ] [ 2 ].scale = tree [ i ].scale2;
 		branches [ i ] [ 2 ].leanc = tree [ i ].leanc2;
 		branches [ i ] [ 2 ].leans = tree [ i ].leans2;
 		branches [ i ] [ 2 ].rotatec = tree [ i ].rotatec2;
 		branches [ i ] [ 2 ].rotates = tree [ i ].rotates2;
 		branches [ i ] [ 2 ].twistc = tree [ i ].twistc2;
 		branches [ i ] [ 2 ].twists = tree [ i ].twists2;

 		branches [ i ] [ 3 ].height = tree [ i ].height3;
		branches [ i ] [ 3 ].scale = tree [ i ].scale3;
 		branches [ i ] [ 3 ].leanc = tree [ i ].leanc3;
 		branches [ i ] [ 3 ].leans = tree [ i ].leans3;
 		branches [ i ] [ 3 ].rotatec = tree [ i ].rotatec3;
 		branches [ i ] [ 3 ].rotates = tree [ i ].rotates3;
 		branches [ i ] [ 3 ].twistc = tree [ i ].twistc3;
 		branches [ i ] [ 3 ].twists = tree [ i ].twists3;

 		branches [ i ] [ 4 ].height = tree [ i ].height4;
		branches [ i ] [ 4 ].scale = tree [ i ].scale4;
 		branches [ i ] [ 4 ].leanc = tree [ i ].leanc4;
 		branches [ i ] [ 4 ].leans = tree [ i ].leans4;
 		branches [ i ] [ 4 ].rotatec = tree [ i ].rotatec4;
 		branches [ i ] [ 4 ].rotates = tree [ i ].rotates4;
 		branches [ i ] [ 4 ].twistc = tree [ i ].twistc4;
 		branches [ i ] [ 4 ].twists = tree [ i ].twists4;

 		branches [ i ] [ 5 ].height = tree [ i ].height5;
		branches [ i ] [ 5 ].scale = tree [ i ].scale5;
 		branches [ i ] [ 5 ].leanc = tree [ i ].leanc5;
 		branches [ i ] [ 5 ].leans = tree [ i ].leans5;
 		branches [ i ] [ 5 ].rotatec = tree [ i ].rotatec5;
 		branches [ i ] [ 5 ].rotates = tree [ i ].rotates5;
 		branches [ i ] [ 5 ].twistc = tree [ i ].twistc5;
 		branches [ i ] [ 5 ].twists = tree [ i ].twists5;

 		branches [ i ] [ 6 ].height = tree [ i ].height6;
		branches [ i ] [ 6 ].scale = tree [ i ].scale6;
 		branches [ i ] [ 6 ].leanc = tree [ i ].leanc6;
 		branches [ i ] [ 6 ].leans = tree [ i ].leans6;
 		branches [ i ] [ 6 ].rotatec = tree [ i ].rotatec6;
 		branches [ i ] [ 6 ].rotates = tree [ i ].rotates6;
 		branches [ i ] [ 6 ].twistc = tree [ i ].twistc6;
 		branches [ i ] [ 6 ].twists = tree [ i ].twists6;

 		branches [ i ] [ 7 ].height = tree [ i ].height7;
		branches [ i ] [ 7 ].scale = tree [ i ].scale7;
 		branches [ i ] [ 7 ].leanc = tree [ i ].leanc7;
 		branches [ i ] [ 7 ].leans = tree [ i ].leans7;
 		branches [ i ] [ 7 ].rotatec = tree [ i ].rotatec7;
 		branches [ i ] [ 7 ].rotates = tree [ i ].rotates7;
 		branches [ i ] [ 7 ].twistc = tree [ i ].twistc7;
 		branches [ i ] [ 7 ].twists = tree [ i ].twists7;
  } // forlooping
} // loadtree.
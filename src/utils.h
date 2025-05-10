#define NAME 		"3D L-system Tree Generator"
#define RND 		( ( float ) rand ( ) / RAND_MAX )
#define RNDBOOL	( 0 == int ( RND * 2 ) )
#define	RNDSGN	( SGN ( 0.5f + RND ) )

// Z buffer depth:
#define ZDEPTH	16384

// Palette size:
#define PALSIZE	8192	// *** Always 2 ^ int ( |n| ) ***

// Screen, pixel & Z-buffer sizes:
#define WIDTH		( 1920 )
#define HEIGHT	( 1080 )
#define MIDX		( WIDTH>>1 )
#define MIDY		( HEIGHT>>1 )
#define BWIDTH	( WIDTH<<1 )
#define BHEIGHT	( HEIGHT<<1 )
#define BMIDX		( MIDX<<1 )
#define BMIDY		( MIDY<<1 )
#define LWIDTH	( 4096 )
#define LHEIGHT	( 4096 )
#define LMIDX		( LWIDTH>>1 )
#define LMIDY		( LHEIGHT>>1 )

//// IFS! IFS!! IFS!!! ////
#define ANTAL		8
#define MAX_FRUITS 100

// Palette displaymodes:
#define SERP		( 1 )
#define ABSZ		( 2 )
#define HOTB		( 3 )

// Coloursation modes:
#define	NORMALCOLOURS	( 0 )
#define FUNKYCOLOURS	( 1 )

// Fonts:
#define SMALLFONT		( 0 )
#define MEDIUMFONT	( 1 )
#define BIGFONT			( 2 )

// Selectable axioms:
#define	USELOGS 		( 0 )
#define	USECUBES 		( 1 )
#define	USESPHEARS 	( 2 )

// Error codes:
#define	WASNOERROR	( 0 )
#define	WASERROR 		( 1 )


#define FILLBOX(xstart, ystart, xend, yend, fillcol)\
				(	lixs = xstart,\
        	liys = ystart,\
          lixe = xend,\
          liye = yend,\
          lcol = fillcol,\
          drawBoxi ( )\
        )

#define TEXTBOX(xstart, ystart, xend, yend, bordercol, fillcol)\
				(	FILLBOX ( xstart - 1, ystart - 1, xend + 1, yend + 1, bordercol ),\
          FILLBOX ( xstart, ystart, xend, yend, fillcol )\
        )

#define COLPAL\
				{	tcolor = lpCols [ pali ];\
				  tRed = ( tcolor >> 16 ) & 0xFF;\
				  tGreen = ( tcolor >> 8 ) & 0xFF;\
				  tBlue = tcolor & 0xFF;\
				  dcr = ( ( dcr + tRed) >> 1 ) & 0xFF;\
				  dcg = ( ( dcg + tGreen) >> 1 ) & 0xFF;\
				  dcb = ( ( dcb + tBlue) >> 1 ) & 0xFF;\
        }

#define COLMOD\
				{	dcr = ( ( dcr + tcr [ coli ] ) >> 1 ) & 0xFF;\
					dcg = ( ( dcg + tcg [ coli ] ) >> 1 ) & 0xFF;\
					dcb = ( ( dcb + tcb [ coli ] ) >> 1 ) & 0xFF;\
        }

// Branch structure:
struct DTBRA {
  double height;
  double scale;
  double leanc;
  double leans;
  double rotatec;
  double rotates;
  double twistc;
  double twists;
};

struct DTIFS {
  char name[16];
  int branches;
  bool usehig;
  bool glblscl;
  bool sctrnsl;
  bool usetwst;
  double radius;
  double height;
  DTBRA stem;
  DTBRA branch_1;
  DTBRA branch_2;
  DTBRA branch_3;
  DTBRA branch_4;
  DTBRA branch_5;
  DTBRA branch_6;
  DTBRA branch_7;
};

typedef struct {
  float x, y, z; 
  float radius; 
} Fruit;

#ifdef __cplusplus
extern "C" {
#endif

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
// void manual(void);
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
int sign(double x);
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
uint32_t* getPixelBufferPtr();
void simulateKeyPress(int key);

#ifdef __cplusplus
}
#endif


// void drawText(float x, float y, const char *text, float r, float g, float b) {
//   static char buffer[9999];
  
//   glColor4f(r, g, b, 1.0);
  
//   int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));
//   // printf("Drawing text '%s' at (%.1f, %.1f) with %d quads\n", text, x, y, num_quads);
  
//   glEnableClientState(GL_VERTEX_ARRAY);
//   glVertexPointer(2, GL_FLOAT, 16, buffer); // 16 bytes per vertex
//   glDrawArrays(GL_QUADS, 0, num_quads * 4); // 4 vertices per quad
//   glDisableClientState(GL_VERTEX_ARRAY);
// }

/*************************** INCLUDE *****************************/
#include "stdafx.h"
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <GL/glui.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <gl\GL.h> 
#include <gl\glut.h>
#include <gl\glu.h> 
#include <list>

using namespace std;
/*************************** DEFINE *****************************/
//#define GAP					 2
#define CUBE				 0
#define SPHERE				 1
#define CONE				 2
#define TETRAHEDRON			 3
#define OCTAHEDRON			 4
#define DODECAHEDRON		 5
#define ICOSAHEDRON			 6
#define TORUS				 7
#define TEAPOT				 8
#define OBJFILE				 9
#define SPLINE				 10
#define CYLINDER			 11
#define PLANE				 12
#define NUM					 50
#define SEGMENT_LIMIT		 50
#define OBJLIMIT			 10
#define WIEW_TOP			 101
#define WIEW_SIDE			 102
#define WIEW_FRONT			 103
#define WIEW_PERP			 104
#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  260
#define GET_FILE			 270
#define ENABLE_ID            300
#define DISABLE_ID           301
#define ADD_ID				 302
#define RESET_ID			 1000

/*************************** MENU VARIABLES *****************************/
int id_MENU1;
int id_MENU2;

/*************************** GLUI POINTERS ******************************/
GLUI_FileBrowser *fb;
GLUI *glui, *glui2;
GLUI_Spinner    *light0_spinner, *light1_spinner;
GLUI_Panel      *obj_panel, *object_panel, *translation_panel;
GLUI_Rotation *light0_rot, *light1_rot, *piked_rot;
GLUI_Translation *trans_xy, *trans_x, *trans_y, *trans_z,*pick_trans_xy, *pick_trans_x, *pick_trans_y, *pick_trans_z;

/*************************** VARIABLES USED IN GLUI INSTRUCTION *********/
int   wireframe = 0;
int	  show_axes = 1;
int   obj_type = 1;
int   obj_view = 1;
int   previous_view = 1;
int   segments = 40;
int   WhichObject = CUBE;
float scale = 1.0;
float scaleX = 1.0;
float scaleY = 1.0;
float scaleZ = 1.0;
float pick_scale = 1.0;
float pick_scaleX = 1.0;
float pick_scaleY = 1.0;
float pick_scaleZ = 1.0;
float obj_pos[] = { 0.0, 0.0, 0.0 };
float pick_obj_pos[] = { 0.0, 0.0, 0.0 };
string file_name;

GLfloat object_color[] =  {1.0f, 1.0f, 1.0f, 1.0f};

////LIGHTS VARIABLE
//LIGHT0
int   light0_enabled = 1;
float light0_intensity = 1.0;
GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};//0.1f, 0.1f, 0.3f, 1.0f
GLfloat light0_diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};//.6f, .6f, 1.0f, 1.0f
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};//.5f, .5f, 1.0f, 0.0f
GLfloat light0_rotation[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

//LIGHT1
int   light1_enabled = 0;
float light1_intensity = .4;
GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = { -.5f, -.5f, 1.0f, 0.0f};
GLfloat light1_rotation[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

//VIEW STATEMENTS
int show_top = 0;
int show_front = 1;
int show_side = 0;
int show_persp = 0;

//OBJECTS ROTATION MATRIX
float view_rotate[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
float picking_rotate[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

/*************************** WINDOWS VARIABLES ******************************/
HDC hDesktopDC = GetDC(NULL);
int   main_window;
int main_window1;
const int screen_width = GetDeviceCaps(hDesktopDC, HORZRES);
const int screen_height = GetDeviceCaps(hDesktopDC, VERTRES);
int window_width = screen_width / 1.2;
int window_height = screen_height / 1.2;
int spline_window_height = window_height / 1.5;
int spline_window_width = window_width / 1.5;
int window_x;
int window_y;
float xy_aspect;
bool isStarted;
GLfloat idleSpin;

/*************************** OBJ READER VARIABLES ********************/
typedef struct ModelData
{
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
}
ModelData;
typedef struct Position
{
	double x;
	double y;
	double z;
}
Position;
typedef struct Texel
{
	double u;
	double v;
}
Texel;
typedef struct Normal
{
	double x;
	double y;
	double z;
}
Normal;
typedef struct Face
{
	int position1[3];
	int texel1[3];
	int normal1[3];
}
Face;
typedef struct Model
{
	vector<Position> Vertices;
	vector<Texel> Textures;
	vector<Normal> Normals;
	vector<Face> Faces;
}
Model;

Model Figure;
int figureSelected = 0;
Model OBJFiles[OBJLIMIT];
int objfile_number = 0;

/*************************** SPLINE VARIABLES ******************************/
int dimpunto = 2;
static int last = 0;
int num_segments;
int tmp;

typedef struct GLFloatPoint
{
    GLfloat x, y, z;
} 
GLFloatPoint; //Definizione Struttura Punti
static GLFloatPoint Punti[NUM]; //memorizziamo via via i vertici cliccati sulla finestra

int n = 0;
float w[50];
float t[50];
int mod_pesi_spline;
float val_pesos;
int m = 4; //ordine della spline
int k;//numero dei nodi veri m+k=N ->k=N-m
float Nodi[50];
char molt[50];
int mod_moltep;
int val_moltep;
float val_nodo;
float B[1000][1000];
int i_nodo;
int i_p_s;
int spline_segments = 20;
GLUI_Panel *rotation_panel;
GLFloatPoint controlVertex[SEGMENT_LIMIT][NUM+1];//tante righe quante il massimo dei segmenti, tante colonne quante il massimo di vertici di controllo per ogni spline
GLFloatPoint curveDots[10001];

typedef struct SplineObject
{
	GLFloatPoint vertex[SEGMENT_LIMIT+1][SEGMENT_LIMIT+1];
	int spline_seg;
}
SplineObject;
SplineObject spob;
SplineObject Splines[OBJLIMIT];
int splines_number = -1;

typedef struct Faccia
{
	GLFloatPoint vertex[3];
}
Faccia;
list<Faccia> face_list[10];
vector<Normal> normal_list[10];
/*************************** MOUSE IN SPLINE WINDOW VARIABLES ********************/
static int scelta;
static bool first = true;

/*************************** CAMERA POSITION VARIABLES ********************/
double eyeX = -1, eyeY = 0, eyeZ = 0;
double centerX = 0, centerY = 0, centerZ = 0;
double upX = 0, upY = 1, upZ = 0;

/*************************** PICKING VARIABLES ********************/
GLint hits;
GLuint buff[512] = {0};
GLint piked_object = 0;
GLint last_piked_object = -1;
int selected = 0;

typedef struct Object_In_List
{         
    int object_type;    
    int picking_value;
	float transf_matrix[16];
	float translation_vector[3];// = { 0.0, 0.0, 0.0 };
	float scale_vector[4];//scale,scaleX,scaleY,scaleZ
	string name;
	int objseq;
	int splineseq;
	bool alive;
	int segment;
	GLfloat color[4];
} 
Object_In_List;

list<Object_In_List> Object_List;
list<Object_In_List>::iterator p;
bool first_pick = true;
int cube_number = 0,sphere_number = 0,cone_number = 0,ico_number = 0,dode_number = 0, cyl_number = 0, plane_number = 0,
	octa_number = 0,tetra_number = 0,teap_number = 0,torus_number = 0, objf_number = 0, spl_number = 0;

const char * fi;

/**************************************** functionPrototypes: *********************/
void LightsSetting(void);
void reset_cb(int);
void control_cb(int);
void menu1(int);
void myGlutKeyboard(unsigned char, int, int);
void myGlutIdle(void);
void myGlutMouse(int, int, int, int);
void myGlutMotion(int, int);
void myGlutReshape(int, int);
void draw_axes(float);
void myGlutDisplay(void);
void GluiSetting(void);
void DrawObjFile(int);
void DrawAxes(void);
void gl_select(int, int);
void list_hits(GLint, GLuint *);
ModelData getOBJinfo(string);
Model extractOBJdata(string);
Model NormalizzazioneFigura(Model);
void DrawObjects2(GLenum);
void listbox_cb(int);
string AssignName(int);
void deleteElement(void);
void display2(void);
void myMouse2(int, int, int, int);
void myinit(void);
void revolution(int);
void drawRevolutionSurface(int);
SplineObject dotsRevolution(void);
void normalizzaSupRiv(void);
void crea_Normali(int);
void crea_Facce(int);
void Transformation(void);


///////////////////////////////////////////FUNCTIONS///////////////////////////////////////////////////////////

/***************************************** getOBJInfo() *************/
ModelData getOBJinfo(string fp)
{
    // 2
    ModelData model = {0};
 
    // 3
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    if(!inOBJ.good())
    {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
 
    // 4
    // Read OBJ file
    while(!inOBJ.eof())
    {
        // 5
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
 
        // 6
        if(type.compare("v ") == 0)
            model.positions++;
        else if(type.compare("vt") == 0)
            model.texels++;
        else if(type.compare("vn") == 0)
            model.normals++;
        else if(type.compare("f ") == 0)
            model.faces++;
    }
 
    // 7
    model.vertices = model.faces*3;
 
    // 8
    // Close OBJ file
    inOBJ.close();
 
    // 9
    return model;
}

/***************************************** list_hits() **************/
void list_hits(GLint hits, GLuint *names)
{
	int i;
 
	/*
 		For each hit in the buffer are allocated 4 bytes:
 		1. Number of hits selected (always one,
 									beacuse when we draw each object
 									we use glLoadName, so we replace the
 									prevous name in the stack)
 		2. Min Z
 		3. Max Z
 		4. Name of the hit (glLoadName)
	*/
 
	printf("%d hits:\n", hits);
 
	for (i = 0; i < hits; i++)
 		printf(	"Number: %d\n"
 				"Min Z: %d\n"
 				"Max Z: %d\n"
 				"Name on stack: %d\n",
 				(GLubyte)names[i * 4],
 				(GLubyte)names[i * 4 + 1],
 				(GLubyte)names[i * 4 + 2],
 				(GLubyte)names[i * 4 + 3]
 				);
 
	printf("\n");
}

/***************************************** gl_select() **************/
void gl_select(int x, int y)
{
	GLint view[4];
	int id;
 
	/*
 		This choose the buffer where store the values for the selection data
	*/
	glSelectBuffer(256, buff);
 
	/*
 		This retrieve info about the viewport
	*/
	glGetIntegerv(GL_VIEWPORT, view);
 
	/*
 		Switching in selecton mode
	*/
	glRenderMode(GL_SELECT);
 
	/*
 		Clearing the name's stack
 		This stack contains all the info about the objects
	*/
	glInitNames();
 
	/*
 		Now fill the stack with one element (or glLoadName will generate an error)
	*/
	glPushName(0);
 
	/*
 		Now modify the vieving volume, restricting selection area around the cursor
	*/
	//glMatrixMode(GL_PROJECTION);//funziona
	glMatrixMode(GL_MODELVIEW);//sostituto
	glPushMatrix();
 		glLoadIdentity();//funziona
 
 		/*
 			restrict the draw to an area around the cursor
 		*/
		gluPickMatrix(x, y, 1.0, 1.0, view);//x, y, 1.0, 1.0, view
		glFrustum(-xy_aspect * .04, xy_aspect * .04, -.043, .043, .1, 15.0);

 		/*
 			Draw the objects onto the screen
 		*/

 		glMatrixMode(GL_MODELVIEW);//FUNZIONA
		
 
 		/*
 			draw only the names in the stack, and fill the array
 		*/
 		glutSwapBuffers();

		////////////////           TOP      ///////////////////////
		if(obj_view == 0) {
        glRotatef(0, 0, 0, 1);
        glRotatef(0, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glTranslatef(0.0, -2.0, 0);
		}

		////////////////           SIDE     ///////////////////////
		if(obj_view == 1) {
			glRotatef(0, 0, 0, 1);
			glRotatef(0, 0, 1, 0);
			glRotatef(0, 1, 0, 0);
			glTranslatef(0.0, 0.0, -2.0);
		}

		////////////////           FRONT     ///////////////////////
		if(obj_view == 2) {
			glRotatef(0, 0, 0, 1);
			glRotatef(-90, 0, 1, 0);
			glRotatef(0, 1, 0, 0);
			glTranslatef(-2.0, 0.0, 0.0);
		}

		////////////////           45GRADI     /////////////////////
		if(obj_view == 3) {
			glRotatef(0, 0, 0, 1);
			glRotatef(-45, 0, 1, 0);
			glRotatef(0, 1, 0, 0);
			glTranslatef(-1.5, -0.2, -1.5);
		}

		glRotatef(idleSpin,0,1,0);//rotazione con l'idle
		glTranslatef(obj_pos[0], obj_pos[1], -obj_pos[2]);
		glMultMatrixf(view_rotate);

		glScalef(scale,scale,scale);
		glScalef(scaleX, scaleY, scaleZ); 

		//DrawAxes();
		////////////////////////////////////////////////////////////////
		DrawObjects2(GL_SELECT);
		glutSwapBuffers();
 		/*
 			Do you remeber? We do pushMatrix in PROJECTION mode
 		*/
 		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
 
	/*
 		get number of objects drawed in that area
 		and return to render mode
	*/
	hits = glRenderMode(GL_RENDER);
 
	if(hits == 0)
		cout << "No Item Selected" << endl;
	if(hits > 0)
		cout << "Item Selected" << endl;
	/*
 		Print a list of the objects
	*/
	//list_hits(hits, buff);                         ////////////da scommentare
 
	/*
 		uncomment this to show the whole buffer
	* /
	gl_selall(hits, buff);
	*/
 
	glMatrixMode(GL_MODELVIEW);

}

/***************************************** extractOBJdata() *********/
Model extractOBJdata(const char *fp)
//Model extractOBJdata(string fp)
{

	Model Figure;

	Position Vertici;
	Texel Texture;
	Normal Normali;
	Face Facce;
    // Open OBJ file
    ifstream inOBJ;
	inOBJ.open(fp);
	//inOBJ.open(fi);
	if(!inOBJ.good())
	{
		cout << "ERROR OPENING OBJ FILE" << endl;
		exit(1);
	}
 
	// Read OBJ file
	while(!inOBJ.eof())
	{
		string line;
		getline(inOBJ, line);
		string type = line.substr(0,2);
 
		// Positions
		if(type.compare("v ") == 0)
		{
			char* l = new char[line.size()+1];
			memcpy(l, line.c_str(), line.size()+1);
 
			strtok(l, " ");

			Vertici.x = atof(strtok(NULL, " "));
			Vertici.y = atof(strtok(NULL, " "));
			Vertici.z = atof(strtok(NULL, " "));

			delete[] l;
			Figure.Vertices.push_back(Vertici);
		}
 
		// Texels
		else if(type.compare("vt") == 0)
		{
			char* l = new char[line.size()+1];
			memcpy(l, line.c_str(), line.size()+1);
 
			strtok(l, " ");

			Texture.u = atof(strtok(NULL, " "));
			Texture.v = atof(strtok(NULL, " "));
 
			delete[] l;
			Figure.Textures.push_back(Texture);
		}
 
		// Normals
		else if(type.compare("vn") == 0)
		{
			char* l = new char[line.size()+1];
			memcpy(l, line.c_str(), line.size()+1);
 
			strtok(l, " ");

			Normali.x = atof(strtok(NULL, " "));
			Normali.y = atof(strtok(NULL, " "));
			Normali.z = atof(strtok(NULL, " "));
 
			delete[] l;
			Figure.Normals.push_back(Normali);
		}
 
		// Faces
		else if(type.compare("f ") == 0)
		{
			char* l = new char[line.size()+1];
			memcpy(l, line.c_str(), line.size()+1);
 
			strtok(l, " ");

			//Primo Vertice
			Facce.position1[0] = atof(strtok(NULL, " /"));
			Facce.texel1[0] = atof(strtok(NULL, " /"));
			Facce.normal1[0] =  atof(strtok(NULL, " /"));
			//Secondo Vertice
			Facce.position1[1] = atof(strtok(NULL, " /"));
			Facce.texel1[1] = atof(strtok(NULL, " /"));
			Facce.normal1[1] =  atof(strtok(NULL, " /"));
			//Terzo Vertice
			Facce.position1[2] = atof(strtok(NULL, " /"));
			Facce.texel1[2] = atof(strtok(NULL, " /"));
			Facce.normal1[2] =  atof(strtok(NULL, " /"));

			delete[] l;
			Figure.Faces.push_back(Facce);
		}
	}
 
	// Close OBJ file
	inOBJ.close();
	
	return Figure;
}

/***************************************** NormalizzazioneFigura() **/
Model NormalizzazioneFigura(Model Figure)
{
	double maxX;
	double maxY;
	double maxZ;
	double Max = 0;
	double minX;
	double minY;
	double minZ;
	double diffX,diffY,diffZ;
	int positionX,positionY,positionZ;
	for (unsigned i=0; i<Figure.Vertices.size(); i++)
		{
			if(i==0) 
			{
				maxX = Figure.Vertices.at(i).x;
				maxY = Figure.Vertices.at(i).y;
				maxZ = Figure.Vertices.at(i).z;
				positionX = i;
				positionY = i;
				positionZ = i;
			} else {
				if(Figure.Vertices.at(i).x > maxX)
				{
					maxX = Figure.Vertices.at(i).x;
					positionX = i;
				}
				if(Figure.Vertices.at(i).y > maxY)
				{
					maxY = Figure.Vertices.at(i).y;
					positionY = i;
				}
				if(Figure.Vertices.at(i).z > maxZ)
				{
					maxZ = Figure.Vertices.at(i).z;
					positionZ = i;
				}
			}	
		}
	if(maxZ >= maxY && maxZ >= maxX)
		Max = maxZ;
	if(maxY >= maxZ && maxY >= maxX)
		Max = maxY;
	if(maxX >= maxZ && maxX >= maxY)
		Max = maxX;

	for(unsigned i=0; i<Figure.Vertices.size(); i++)
	{
		Figure.Vertices.at(i).x = Figure.Vertices.at(i).x/Max;
		Figure.Vertices.at(i).y = Figure.Vertices.at(i).y/Max;
		Figure.Vertices.at(i).z = Figure.Vertices.at(i).z/Max;
		if(i==0)
		{
			minX = Figure.Vertices.at(i).x;
			maxX = Figure.Vertices.at(i).x;
			minY = Figure.Vertices.at(i).y;
			maxY = Figure.Vertices.at(i).y;
			minZ = Figure.Vertices.at(i).z;
			maxZ = Figure.Vertices.at(i).z;
		}
		else
		{
			maxX = max(maxX,Figure.Vertices.at(i).x);
			minX = min(minX,Figure.Vertices.at(i).x);
			maxY = max(maxY,Figure.Vertices.at(i).y);
			minY = min(minY,Figure.Vertices.at(i).y);
			maxZ = max(maxZ,Figure.Vertices.at(i).z);
			minZ = min(minZ,Figure.Vertices.at(i).z);
		}
	}
	
	diffX = maxX - minX;
	diffY = maxY - minY;
	diffZ = maxZ - minZ;
	for(unsigned i=0; i<Figure.Vertices.size(); i++)
	{
		if(minX > 0)
			Figure.Vertices.at(i).x -= diffX/2;
		if(minY > 0)
			Figure.Vertices.at(i).y -= diffY/2;
		if(minZ > 0)
			Figure.Vertices.at(i).z -= diffZ/2;
	}
	return Figure;
}

/***************************************** reset_cb() ***************/
void reset_cb(int reset){

	//OBJECTS VARIABLES RESET
	wireframe = 0;
	show_axes = 1;
	obj_pos[0] = obj_pos[1] = obj_pos[2] = 0.0;
	obj_type = 1; obj_view = 1;
	segments = 40;
	scaleX = 1.0;
	scaleY = 1.0;
	scaleZ = 1.0;
	scale = 1.0;
	idleSpin = 0;
	figureSelected = 0;
	object_color[0] = object_color[1] = object_color[2] = 1;

	//MATRIX ROTATION RESET
	float rotate[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	for(int i=0;i<16;i++)
	{
		picking_rotate[i] = rotate[i];
		view_rotate[i] = rotate[i];
		light0_rotation[i] = rotate[i];
		light1_rotation[i] = rotate[i];
	}

	//LIGHTS RESET
	light0_rot->enable(); light0_spinner->enable();
	light1_rot->disable(); light1_spinner->disable();
	light0_enabled = 1; light1_enabled = 0;
	light0_intensity = 1.0; light1_intensity = .4;

	light0_ambient[0] = light0_ambient[1] = 0.1f; light0_ambient[2] = 0.3f; light0_ambient[3] = 1.0f;
	//light0_ambient[0] = light0_ambient[1] = light0_ambient[2] = 0.0f; light0_ambient[3] = 1.0f;
	light0_diffuse[0] = light0_diffuse[1] = light0_diffuse[2] = light0_diffuse[3] = 1.0f;
	light0_position[0] = light0_position[1] = .5f; light0_position[2] = 1.0f; light0_position[3] = 0.0f;

	light1_ambient[0] = light1_ambient[1] = 0.1f; light1_ambient[2] = 0.3f; light1_ambient[3] = 1.0f;
	light1_diffuse[0] = 0.9f; light1_diffuse[1] = .6f; light1_diffuse[2] = 0.0f; light1_diffuse[3] = 1.0f;
	light1_position[0] = light1_position[1] = -.5f;light1_position[2] = 1.0f; light1_position[3] = 0.0f;

	//OBJECTLIST CLEANING
	list<Object_In_List> Empty_List;
	Object_List = Empty_List;

	//WINDOWS REFRESH
	glui->sync_live();
	glui2->sync_live();
	LightsSetting();
	glutPostRedisplay();
}

/***************************************** control_cb() *************/
void control_cb(int control) {
	
	if (control == GET_FILE) {
		if(objfile_number <= OBJLIMIT-1)
		{
			fi = fb->get_file();
			//file_name = fb->get_file();
			figureSelected = 1;
			int s_size = strlen(fi);
			//if(file_name.at(file_name.length()-3)=='o' && file_name.at(file_name.length()-2)=='b' && file_name.at(file_name.length()-1)=='j')
			if( fi[s_size-3] =='o' && fi[s_size-2] =='b' && fi[s_size-1] =='j')
			{
				//cout << "Extracting Data from " << file_name << endl;
				cout << "Extracting Data..." << endl;
				//Figure = extractOBJdata(file_name);
				Figure = extractOBJdata(fi);
				Figure = NormalizzazioneFigura(Figure);
				OBJFiles[objfile_number] = Figure;
				objfile_number++;
				cout << "Succesfull!! " << endl;
			}
			else
			{
				//cout << file_name << " THIS IS NOT AN OBJ FILE!!" << endl;
				cout << " THIS IS NOT AN OBJ FILE!!" << endl;
			}
		}
		else
		{
			cout << "Impossibile caricare altri file .obj" << endl;
		}
	  }
	
	if(control == LIGHT0_ENABLED_ID) {
        if(light0_enabled) {
            glEnable(GL_LIGHT0);
			light0_rot->enable();
            light0_spinner->enable();
        }
        else {
            glDisable(GL_LIGHT0);
			light0_rot->disable();
            light0_spinner->disable();
        }
    }
    else
        if(control == LIGHT1_ENABLED_ID) {
            if(light1_enabled) {
                glEnable(GL_LIGHT1);
				light1_rot->enable();
                light1_spinner->enable();
            }
            else {
                glDisable(GL_LIGHT1);
				light1_rot->disable();
                light1_spinner->disable();
            }
        }
        else
            if(control == LIGHT0_INTENSITY_ID) {
                float v[] = {
                    light0_diffuse[0],  light0_diffuse[1],
                    light0_diffuse[2],  light0_diffuse[3]
                };

                v[0] *= light0_intensity;
                v[1] *= light0_intensity;
                v[2] *= light0_intensity;

                glLightfv(GL_LIGHT0, GL_DIFFUSE, v);
            }
            else
                if(control == LIGHT1_INTENSITY_ID) {
                    float v[] = {
                        light1_diffuse[0],  light1_diffuse[1],
                        light1_diffuse[2],  light1_diffuse[3]
                    };

                    v[0] *= light1_intensity;
                    v[1] *= light1_intensity;
                    v[2] *= light1_intensity;

                    glLightfv(GL_LIGHT1, GL_DIFFUSE, v);
                }
}

/***************************************** menu1() ******************/
void menu1(int id_entry)
{
	float rotate[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	switch(id_entry){
	//sub menu View
	case(WIEW_TOP):
		obj_view = 0;
		for(int i=0;i<16;i++)
		{
			view_rotate[i] = rotate[i];
		}
		obj_pos[0] = 0;
		obj_pos[1] = 0;
		obj_pos[2] = 0;
		scale = 1;
		scaleX = 1;
		scaleY = 1;
		scaleZ = 1;
		glui->sync_live();
		glui2->sync_live();
		glutPostRedisplay();
		break;
	case(WIEW_SIDE):
		obj_view = 1;
		for(int i=0;i<16;i++)
		{
			view_rotate[i] = rotate[i];
		}
		obj_pos[0] = 0;
		obj_pos[1] = 0;
		obj_pos[2] = 0;
		scale = 1;
		scaleX = 1;
		scaleY = 1;
		scaleZ = 1;
		glui->sync_live();
		glui2->sync_live();
		glutPostRedisplay();
		break;
	case(WIEW_FRONT):
		obj_view = 2;
		for(int i=0;i<16;i++)
		{
			view_rotate[i] = rotate[i];
		}
		obj_pos[0] = 0;
		obj_pos[1] = 0;
		obj_pos[2] = 0;
		scale = 1;
		scaleX = 1;
		scaleY = 1;
		scaleZ = 1;
		glui->sync_live();
		glui2->sync_live();
		glutPostRedisplay();
		break;
	case(WIEW_PERP):
		obj_view = 3;
		for(int i=0;i<16;i++)
		{
			view_rotate[i] = rotate[i];
		}
		obj_pos[0] = 0;
		obj_pos[1] = 0;
		obj_pos[2] = 0;
		scale = 1;
		scaleX = 1;
		scaleY = 1;
		scaleZ = 1;
		glui->sync_live();
		glui2->sync_live();
		glutPostRedisplay();
		break;
	default:
		break;
		
	}
}

/***************************************** deleteElement() **********/
void deleteElement(void)
{
	glutPostRedisplay();
	list<Object_In_List>::iterator p1;
	for (p = Object_List.begin(); p != Object_List.end(); p++)
		{ 
			if(piked_object == (*p).picking_value)
			{
				cout << (*p).name << " Deleted" << endl;
				(*p).alive = false;
			}
		}
	glutPostRedisplay();
}

/***************************************** myGlutKeyboard() *********/
void myGlutKeyboard(unsigned char Key, int x, int y) {
    switch(Key) {
	case 'd':
	case 'D':
		deleteElement();
		break;
	case 'i':
	case 'I':
		if(isStarted)
			{
				isStarted = false;
				glutIdleFunc(NULL);
			}
		else
			{
				isStarted = true;
				glutIdleFunc(myGlutIdle);
			}
		break;
	case 27:
    case 'q':
	case 'Q':
        exit(0);
        break;
    };

    glutPostRedisplay();
}

/***************************************** myGlutIdle() *************/
void myGlutIdle(void) {

	if(objf_number > 0)
		idleSpin = idleSpin+1.5;
	else
		idleSpin = idleSpin+0.05;
    if(glutGetWindow() != main_window)
        glutSetWindow(main_window);

    //GLUI_Master.sync_live_all();

    glutPostRedisplay();
}

/***************************************** myGlutMouse() ************/
void myGlutMouse(int button, int state, int x, int y)
{ 
    if 	((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		gl_select(x,window_height-y);
	}
}

/***************************************** myGlutMotion() ***********/
void myGlutMotion(int x, int y)
{
	glutPostRedisplay();
}

/***************************************** myGlutReshape() **********/
void myGlutReshape(int x, int y) {
    int tx, ty, tw, th;
    GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	//cout << tx << " " << ty << " " << tw << " " << th << " " << endl;
    glViewport(tx, ty, tw, th);

    xy_aspect = (float)tw / (float)th;

    glutPostRedisplay();
}

/***************************************** myGlutDisplay() **********/
void myGlutDisplay(void) {
    glClearColor(.9f, .9f, .9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	int tx, ty, tw, th;
    GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);

	glFrustum(-xy_aspect * .04, xy_aspect * .04, -.043, .043, .1, 15.0);//-xy_aspect * .04, xy_aspect * .04, -.04, .04, .1, 15.0
																	   //-xy_aspect * .04, xy_aspect * .04, -.04, .045, .1, 15.0
	//gluPerspective(45,xy_aspect,.1,15.0);
	//glOrtho(-xy_aspect, xy_aspect, -1, 1, 1, 15.0);

	//gluInvertMatrix(view_rotate);
    ////////////////           TOP     ///////////////////////
    if(obj_view == 0) {
		glRotatef(0, 0, 0, 1);
        glRotatef(0, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glTranslatef(0.0, -2.0, 0);
    }

    ////////////////           SIDE     ///////////////////////
    if(obj_view == 1) {
		glRotatef(0, 0, 0, 1);
        glRotatef(0, 0, 1, 0);
        glRotatef(0, 1, 0, 0);
        glTranslatef(0.0, 0.0, -2.0);
    }

    ////////////////           FRONT     ///////////////////////
    if(obj_view == 2) {
		glRotatef(0, 0, 0, 1);
        glRotatef(-90, 0, 1, 0);
        glRotatef(0, 1, 0, 0);
        glTranslatef(-2.0, 0.0, 0.0);
    }

	////////////////           45GRADI     /////////////////////
	if(obj_view == 3) {
		glRotatef(0, 0, 0, 1);
        glRotatef(-45, 0, 1, 0);
        glRotatef(0, 1, 0, 0);
        glTranslatef(-1.5, 0, -1.5);
    }

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glMultMatrixf(light0_rotation);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	if(light0_enabled == 1)
	{
		glLoadIdentity();
		glMultMatrixf(light0_rotation);
		glTranslatef(light0_position[0],light0_position[1],light0_position[2]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, light0_ambient);
		glutSolidSphere(0.02,40,40);
	}

	glLoadIdentity();
    glMultMatrixf(light1_rotation);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	if(light1_enabled == 1)
	{
		glLoadIdentity();
		glMultMatrixf(light1_rotation);
		glTranslatef(light1_position[0],light1_position[1],light1_position[2]);
		glMaterialfv(GL_FRONT, GL_AMBIENT, light1_ambient);
		glutSolidSphere(0.02,40,40);
	}

    glLoadIdentity();

	glRotatef(idleSpin,0,1,0);//rotazione con l'idle
    glTranslatef(obj_pos[0], obj_pos[1], -obj_pos[2]);
	glMultMatrixf(view_rotate);

	glScalef(scale,scale,scale);
    glScalef(scaleX, scaleY, scaleZ); 

	if(show_axes == 1)
		DrawAxes();
	DrawObjects2(GL_RENDER);

    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

/***************************************** DrawObjFile() ************/
void DrawObjFile(int index)
{
	//  OBJ File
	glPushMatrix();
	if(wireframe && figureSelected == 1)
	{
		//glScalef( 0.50, 0.50, 0.50 );
		for(unsigned i=0; i< OBJFiles[index].Faces.size() ; i++)
		{
			glBegin(GL_LINE_STRIP);
			//Primo Vertice

			glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).x,
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).y,//x
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).z);//x

			glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).x,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).y,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).z);

			//Secondo Vertice

			glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).x,//y
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).y,
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).z);//y

			glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).x,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).y,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).z);

			//Terzo Vertice
			
			glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).x,//z
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).y,//z
				OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).z);

			glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).x,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).y,
				OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).z);
			glEnd();
		}

	}
	else
	{
		if(figureSelected == 1)
		{
			//glScalef( 0.50, 0.50, 0.50 );
			for(unsigned i=0; i< OBJFiles[index].Faces.size() ; i++)
			{
				glBegin(GL_TRIANGLES);
				//Primo Vertice

				glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).x,
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).y,//x
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[0]-1).z);//x

				glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).x,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).y,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[0]-1).z);

				//Secondo Vertice

				glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).x,//y
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).y,
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[1]-1).z);//y

				glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).x,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).y,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[1]-1).z);

				//Terzo Vertice

				glNormal3f(OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).x,//z
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).y,//z
					OBJFiles[index].Normals.at(OBJFiles[index].Faces.at(i).normal1[2]-1).z);

				glVertex3f(OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).x,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).y,
					OBJFiles[index].Vertices.at(OBJFiles[index].Faces.at(i).position1[2]-1).z);
					glEnd();
			}
			glutPostRedisplay();
		}
	}
	glPopMatrix();
}

/***************************************** DrawAxes() ***************/
void DrawAxes(void)
{
	glDisable(GL_LIGHTING);
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_LINES);
		glVertex3f(0.75, 0.05, 0.0);
		glVertex3f(0.8, 0.1, 0.0);
		glVertex3f(0.75, 0.1, 0.0);
		glVertex3f(0.8, 0.05, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.8,0.0,0.0);
	glEnd();
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINES);
		glVertex3f(0.05, 0.8, 0.0);
		glVertex3f(0.075, 0.77, 0.0);
		glVertex3f(0.1, 0.8, 0.0);
		glVertex3f(0.075, 0.77, 0.0);
		glVertex3f(0.075, 0.77, 0.0);
		glVertex3f(0.075, 0.74, 0.0);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.8,0.0);
	glEnd();
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, 0.05, 0.75);
		glVertex3f(0.0, 0.05, 0.8);
		glVertex3f(0.0, 0.1, 0.75);
		glVertex3f(0.0, 0.1, 0.8);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.0,0.0,0.0);
		glVertex3f(0.0,0.0,0.8);
	glEnd();
	glEnable(GL_LIGHTING);
}

/***************************************** DrawObjects2() ***********/
void DrawObjects2(GLenum mode)
{
	//glEnable(GL_NORMALIZE);

	if(hits == 0)
	{
		piked_object = -10;
	}
	
	if(mode == GL_RENDER && hits > 0 && piked_object != (GLubyte)buff[(hits-1) * 4 + 3])
	{
		piked_object = (GLubyte)buff[(hits-1) * 4 + 3];
		first_pick = false;
		//cout << "Piked Object: " << piked_object << endl;
		for (p = Object_List.begin(); p != Object_List.end(); p++)
		{ 
			if(piked_object == (*p).picking_value)
				cout << "Piked Object: " << (*p).name << endl;
		}
	}
	for (p = Object_List.begin(); p != Object_List.end(); p++)
    { 
		glMaterialfv(GL_FRONT, GL_AMBIENT, (*p).color);
		if(piked_object == (*p).picking_value)
			{
				if(first_pick == false)
				{
					first_pick = true;
					pick_obj_pos[0] = (*p).translation_vector[0];
					pick_obj_pos[1] = (*p).translation_vector[1];
					pick_obj_pos[2] = (*p).translation_vector[2];
					
					pick_scale = (*p).scale_vector[0];
					pick_scaleX = (*p).scale_vector[1];
					pick_scaleY = (*p).scale_vector[2];
					pick_scaleZ = (*p).scale_vector[3];

					object_color[0] = (*p).color[0];
					object_color[1] = (*p).color[1];
					object_color[2] = (*p).color[2];

					segments = (*p).segment;

					for(int j=0;j<16;j++)
					{
						picking_rotate[j] = (*p).transf_matrix[j];
					}

					glui->sync_live();
					glui2->sync_live();
					glutPostRedisplay();

				}
				//rotazione
				for(int i=0;i<16;i++)
				{
					(*p).transf_matrix[i] = picking_rotate[i];
				}
				//traslazione
				(*p).translation_vector[0] = pick_obj_pos[0];
				(*p).translation_vector[1] = pick_obj_pos[1];
				(*p).translation_vector[2] = pick_obj_pos[2];
				//scalatura
				(*p).scale_vector[0] = pick_scale;
				(*p).scale_vector[1] = pick_scaleX;
				(*p).scale_vector[2] = pick_scaleY;
				(*p).scale_vector[3] = pick_scaleZ;
				//colore
				(*p).color[0] = object_color[0];
				(*p).color[1] = object_color[1];
				(*p).color[2] = object_color[2];
				//segmenti
				(*p).segment = segments;

				glutPostRedisplay();
			}//end if picked
			if((*p).alive == true)
			{
				if (mode == GL_SELECT)
					glLoadName((*p).picking_value);

				glPushMatrix();
				glTranslatef((*p).translation_vector[0],(*p).translation_vector[1],-(*p).translation_vector[2]);
				glMultMatrixf((*p).transf_matrix);
				glScalef((*p).scale_vector[0],(*p).scale_vector[0],(*p).scale_vector[0]);
				glScalef((*p).scale_vector[1],(*p).scale_vector[2],(*p).scale_vector[3]);

				if((*p).object_type == OBJFILE)//OBJFILE
					DrawObjFile((*p).objseq);
				else if((*p).object_type == SPLINE)
					drawRevolutionSurface((*p).splineseq);
				else if(wireframe && (*p).object_type == CUBE)//CUBE
					glutWireCube(.5);
				else if((*p).object_type == CUBE)
					glutSolidCube(.5);
				else if(wireframe && (*p).object_type == SPHERE)//SPHERE
					glutWireSphere(.4, (*p).segment, (*p).segment);
				else if((*p).object_type == SPHERE)
					glutSolidSphere(.4, (*p).segment, (*p).segment);
				else if(wireframe && (*p).object_type == CONE)//CONE
					glutWireCone(.5, .75, (*p).segment, (*p).segment);
				else if((*p).object_type == CONE)
					glutSolidCone(.5, .75, (*p).segment, (*p).segment);
				else if(wireframe && (*p).object_type == CYLINDER)//CYLINDER
					{
						glTranslatef(0.0,-0.35,0.0);
						glRotatef(-90,1,0,0);
						GLUquadricObj *cilindro;
						cilindro = gluNewQuadric();
						gluQuadricDrawStyle(cilindro,GLU_LINE);
						gluCylinder(cilindro, 0.2, 0.2, 0.7, (*p).segment, (*p).segment);
					}
				else if((*p).object_type == CYLINDER)
					{
						glTranslatef(0.0,-0.35,0.0);
						glRotatef(-90,1,0,0);
						GLUquadricObj *cilindro;
						cilindro = gluNewQuadric();
						gluQuadricDrawStyle(cilindro,GLU_FILL);
						gluCylinder(cilindro, 0.2, 0.2, 0.7, (*p).segment, (*p).segment);
					}
				else if(wireframe && (*p).object_type == PLANE)//PLANE
					{
						glScalef( 1, 1, 0.002 );
						glutWireCube(1);
					}
				else if((*p).object_type == PLANE)
					{
						glScalef( 1, 1, 0.002 );
						glutSolidCube(1);
					}
				else if(wireframe && (*p).object_type == TETRAHEDRON)//TETRAHEDRON
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutWireTetrahedron();
					}
				else if((*p).object_type == TETRAHEDRON)
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutSolidTetrahedron();
					}
				else if(wireframe && (*p).object_type == OCTAHEDRON)//OCTAHEDRON
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutWireOctahedron();
					}
				else if((*p).object_type == OCTAHEDRON)
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutSolidOctahedron();
					}
				else if(wireframe && (*p).object_type == DODECAHEDRON)//DODECAHEDRON
					{
						glScalef( 0.25, 0.25, 0.25 );
						glutWireDodecahedron();
					}
				else if((*p).object_type == DODECAHEDRON)
					{
						glScalef( 0.25, 0.25, 0.25 );
						glutSolidDodecahedron();
					}
				else if(wireframe && (*p).object_type == ICOSAHEDRON)//ICOSAHEDRON
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutWireIcosahedron();
					}
				else if((*p).object_type == ICOSAHEDRON)
					{
						glScalef( 0.50, 0.50, 0.50 );
						glutSolidIcosahedron();
					}
				else if(wireframe && (*p).object_type == TORUS)//TORUS
					glutWireTorus(.15, .3, 16, (*p).segment);
				else if((*p).object_type == TORUS)
					glutSolidTorus(.15, .3, 16, (*p).segment);
				else if(wireframe && (*p).object_type == TEAPOT)//TEAPOT
					glutWireTeapot(0.5);
				else if((*p).object_type == TEAPOT)
					glutSolidTeapot(0.5);

				glPopMatrix();

		}//if alive end

	}//end for

}//function end

/***************************************** listbox_cb() *************/
void listbox_cb(int control)
{
    if(WhichObject == SPLINE)
    {
        //Finestra per le Spline
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
        glutInitWindowPosition((screen_width - spline_window_width) / 2 , (screen_height - spline_window_height) / 2);
        glutInitWindowSize(spline_window_width, spline_window_height);
        main_window1 = glutCreateWindow("Spline");
        myinit();

        glutDisplayFunc(display2);
        GLUI_Master.set_glutMouseFunc(myMouse2);

        glui = GLUI_Master.create_glui_subwindow(main_window1, GLUI_SUBWINDOW_RIGHT);
        rotation_panel = glui->add_panel( " Revolution Surface " );
        GLUI_Button *button_y = glui->add_button_to_panel(rotation_panel, "Asse Y", 1, (GLUI_Update_CB)revolution);
		new GLUI_StaticText(glui," ");
		GLUI_Spinner *spinner =  new GLUI_Spinner(rotation_panel, "Segments:", &spline_segments);
		spinner->set_int_limits(4, SEGMENT_LIMIT);
		spinner->set_alignment(GLUI_ALIGN_RIGHT);

    }
    else
    {
		if(WhichObject == OBJFILE && figureSelected == 0)
		{
			cout << "No .obj file selected " << endl;
		}
		else
		{
			int i = Object_List.size() + 1;
			Object_In_List ob;
			for(int j = 0; j < 16; j++)
			{
				if(j % 5 == 0)
					ob.transf_matrix[j] = 1;
				else
					ob.transf_matrix[j] = 0;
			}
			ob.picking_value = i;
			ob.object_type = WhichObject;
			ob.translation_vector[0] = 0.0;
			ob.translation_vector[1] = 0.0;
			ob.translation_vector[2] = 0.0;
			ob.scale_vector[0] = 1.0;
			ob.scale_vector[1] = 1.0;
			ob.scale_vector[2] = 1.0;
			ob.scale_vector[3] = 1.0;
			string name = AssignName(ob.object_type);
			ob.name = name;
			ob.alive = true;
			ob.segment = 40;
			for(int i=0;i<4;i++)
				ob.color[i] = object_color[i];
			ob.splineseq = -1;
			if(ob.object_type == OBJFILE)
				ob.objseq = objfile_number-1;
			else
				ob.objseq = -1;
			Object_List.push_back(ob);
			glutPostRedisplay();
		}
    }
}

/***************************************** AssignName() *************/
string AssignName(int obj_type)
{
	if(obj_type == CUBE)
	{
		cube_number++;
		return "CUBE " + to_string((long double)cube_number);
	}
	if(obj_type == SPHERE)
	{
		sphere_number++;
		return "SPHERE " + to_string((long double)sphere_number);
	}
	if(obj_type == CONE)
	{
		cone_number++;
		return "CONE " + to_string((long double)cone_number);
	}
	if(obj_type == CYLINDER)
	{
		cyl_number++;
		return "CYLINDER " + to_string((long double)cyl_number);
	}
	if(obj_type == PLANE)
	{
		plane_number++;
		return "PLANE " + to_string((long double)plane_number);
	}
	if(obj_type == TETRAHEDRON)
	{
		tetra_number++;
		return "TETRAHEDRON " + to_string((long double)tetra_number);
	}
	if(obj_type == OCTAHEDRON)
	{
		octa_number++;
		return "OCTAHEDRON " + to_string((long double)octa_number);
	}
	if(obj_type == DODECAHEDRON)
	{
		dode_number++;
		return "DODECAHEDRON " + to_string((long double)dode_number);
	}
	if(obj_type == ICOSAHEDRON)
	{
		ico_number++;
		return "ICOSAHEDRON " + to_string((long double)ico_number);
	}
	if(obj_type == TORUS)
	{
		torus_number++;
		return "TORUS " + to_string((long double)torus_number);
	}
	if(obj_type == TEAPOT)
	{	
		teap_number++;
		return "TEAPOT " + to_string((long double)teap_number);
	}
	if(obj_type == OBJFILE)
	{
		objf_number++;
		return "OBJFILE " + to_string((long double)objf_number);
	}
	if(obj_type == SPLINE)
	{
		spl_number++;
		return "SPLINE " + to_string((long double)spl_number);
	}
}

/***************************************** normalizzaSuRiv() ********/
void normalizzaSupRiv()
{
    float max = 0;
	float m1 = 0;
	float m2 = 0;
	float min = 100;
	float diff;
	//curveDots
	for(int k = 0; k < 10000; k++)
	{
		m1 = max(curveDots[k].x,curveDots[k].y);
		m2 = max(m1, curveDots[k].z);
		if(m2 > max)
			max = m2;
	}

	for(int k = 0; k < 10000; k++)
	{
		curveDots[k].x /= max;
		curveDots[k].y /= max;
		curveDots[k].z /= max;
	}

	for(int i = 0; i < 10000; i++)
	{
		if(i == 0)
		{
			max = curveDots[i].y;
			min = curveDots[i].y;
		}
		else
		{
			max = max(max,curveDots[i].y);
			min = min(min,curveDots[i].y);
		}
	}
	diff = max - min;
	for(int i = 0; i < 10000; i++)
	{
		curveDots[i].y -= diff/2;
	}
}

/***************************************** drawRevolutionSurface() **/
void drawRevolutionSurface(int index)
{
	int i=0;
	list<Faccia>::iterator p;
	for (p = face_list[index].begin(); p != face_list[index].end(); p++)
    {
		
		if(wireframe)
		{
			glBegin(GL_LINE_STRIP);
				glNormal3f(-normal_list[index].at(i).x, -normal_list[index].at(i).y, -normal_list[index].at(i).z);
				glVertex3f((*p).vertex[0].x,(*p).vertex[0].y,(*p).vertex[0].z);
				glVertex3f((*p).vertex[1].x,(*p).vertex[1].y,(*p).vertex[1].z);
				glVertex3f((*p).vertex[2].x,(*p).vertex[2].y,(*p).vertex[2].z);
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
				glNormal3f(-normal_list[index].at(i).x, -normal_list[index].at(i).y, -normal_list[index].at(i).z);
				glVertex3f((*p).vertex[0].x,(*p).vertex[0].y,(*p).vertex[0].z);
				glVertex3f((*p).vertex[1].x,(*p).vertex[1].y,(*p).vertex[1].z);
				glVertex3f((*p).vertex[2].x,(*p).vertex[2].y,(*p).vertex[2].z);
			glEnd();
		}
		i++;
	}
}

/***************************************** revolution() *************/
void revolution(int c)
{
    num_segments = spline_segments;
    tmp = 1;

    normalizzaSupRiv();
	spob = dotsRevolution();
	splines_number++;
	spob.spline_seg = spline_segments;
	Splines[splines_number]=spob;
	crea_Facce(splines_number);
	crea_Normali(splines_number);

	int i = Object_List.size() + 1;
    Object_In_List ob;
    for(int j = 0; j < 16; j++)
    {
        if(j % 5 == 0)
            ob.transf_matrix[j] = 1;
        else
            ob.transf_matrix[j] = 0;
    }
    ob.picking_value = i;
    ob.object_type = SPLINE;
    ob.translation_vector[0] = 0.0;
    ob.translation_vector[1] = 0.0;
    ob.translation_vector[2] = 0.0;
    ob.scale_vector[0] = 1.0;
    ob.scale_vector[1] = 1.0;
    ob.scale_vector[2] = 1.0;
    ob.scale_vector[3] = 1.0;
    string name = AssignName(ob.object_type);
    ob.name = name;
	ob.alive = true;
	ob.splineseq = splines_number;
	ob.objseq = -1;
	ob.segment = num_segments;
	for(int i=0;i<4;i++)
			ob.color[i] = object_color[i];
    Object_List.push_back(ob);
}

/***************************************** crea_Normali() ***********/
void crea_Normali(int index)
{
	vector<Normal> normali;
	Normal normal;
	list<Faccia>::iterator p;

	for (p = face_list[index].begin(); p != face_list[index].end(); p++)
    {
		normal.x = (((*p).vertex[1].y - (*p).vertex[0].y) * ((*p).vertex[2].z - (*p).vertex[0].z)) - (((*p).vertex[1].z - (*p).vertex[0].z) * ((*p).vertex[2].y - (*p).vertex[0].y));
		normal.y = (((*p).vertex[1].z - (*p).vertex[0].z) * ((*p).vertex[2].x - (*p).vertex[0].x)) - (((*p).vertex[1].x - (*p).vertex[0].x) * ((*p).vertex[2].z - (*p).vertex[0].z));
		normal.z = (((*p).vertex[1].x - (*p).vertex[0].x) * ((*p).vertex[2].y - (*p).vertex[0].y)) - (((*p).vertex[1].y - (*p).vertex[0].y) * ((*p).vertex[2].x - (*p).vertex[0].x));
		normali.push_back(normal);
	}
	normal_list[index] = normali;
}

/***************************************** crea_Facce() *************/
void crea_Facce(int index)
{
	//crea facce triangolari per la superfice di rivoluzione
	list<Faccia> Facce;
	Faccia face;
	int a,b,c,d;
	for(int i = 0; i < Splines[index].spline_seg; i++)
        {
			a = i;
			b = i+1;
			for(int j = 0; j < Splines[index].spline_seg; j++)
			{
				c = j;
				d = j+1;
				if(j == Splines[index].spline_seg-1)
				{
					d=0;
				}
				face.vertex[0].x = Splines[index].vertex[c][a].x;
				face.vertex[0].y = Splines[index].vertex[c][a].y;
				face.vertex[0].z = Splines[index].vertex[c][a].z;
				face.vertex[1].x = Splines[index].vertex[c][b].x;
				face.vertex[1].y = Splines[index].vertex[c][b].y;
				face.vertex[1].z = Splines[index].vertex[c][b].z;
				face.vertex[2].x = Splines[index].vertex[d][a].x;
				face.vertex[2].y = Splines[index].vertex[d][a].y;
				face.vertex[2].z = Splines[index].vertex[d][a].z;
				Facce.push_back(face);
				face.vertex[0].x = Splines[index].vertex[c][b].x;
				face.vertex[0].y = Splines[index].vertex[c][b].y;
				face.vertex[0].z = Splines[index].vertex[c][b].z;
				face.vertex[1].x = Splines[index].vertex[d][b].x;
				face.vertex[1].y = Splines[index].vertex[d][b].y;
				face.vertex[1].z = Splines[index].vertex[d][b].z;
				face.vertex[2].x = Splines[index].vertex[d][a].x;
				face.vertex[2].y = Splines[index].vertex[d][a].y;
				face.vertex[2].z = Splines[index].vertex[d][a].z;
				Facce.push_back(face);
			}
		}
	face_list[index] = Facce;
}

/***************************************** dotsRevolution() *********/
SplineObject dotsRevolution()
{
	SplineObject ob;

	for(int i = 0; i <= spline_segments; i++)
        {
            for(int j = 0; j <= spline_segments; j++)
            {
				float r = curveDots[i*(10000/spline_segments)].x;
                float theta = 2.0f * 3.1415926f * float(j) / float(spline_segments);//get the current angle

				ob.vertex[j][i].x = r * cosf(theta);
				ob.vertex[j][i].y = curveDots[i*(10000/spline_segments)].y;
				ob.vertex[j][i].z = r * sinf(theta);

            }
        }
	for(int j = 0; j <= spline_segments; j++)
	{
		float r = curveDots[9999].x;
        float theta = 2.0f * 3.1415926f * float(j) / float(spline_segments);//get the current angle
		ob.vertex[j][spline_segments].x = r * cosf(theta);
		ob.vertex[j][spline_segments].y = curveDots[9999].y;
		ob.vertex[j][spline_segments].z = r * sinf(theta);
	}

	return ob;
}

/***************************************** Parametrizzazione_Corde()*/
void Parametrizzazione_Corde()
{
    //i valori di t seguono l'andamento dei punti
    int i;
    t[1] = 0;
    for(i = 2; i <= n; i++)
        t[i] = t[i - 1] + sqrt((Punti[i].x - Punti[i - 1].x) * (Punti[i].x - Punti[i - 1].x) + (Punti[i].y - Punti[i - 1].y) * (Punti[i].y - Punti[i - 1].y));

    for (i = 1; i <= n; i++)
    {
        t[i] = t[i] / t[n];
    }
}

/***************************************** Bisezione() **************/
int Bisezione(float z)
{
    int  mid;
    int l = m, u = m + k + 1;
    while((u - l) > 1)
    {
        mid = (l + u) / 2;
        if(z < Nodi[mid])
            u = mid;
        else
            l = mid;
    }
    return l;
}

/***************************************** Costruisci_Nodi() ********/
void Costruisci_Nodi()
{
    int i, cont = 3;
    k = n - m;
    //[a,b] -> [0,1]
    for(i = 1; i <= m; i++) //m nodi fittizzi a sx in corrispondenza di a
    {
        Nodi[i] = 0;
        molt[i] = '4';
    }
    for(i = m + 1; i <= m + k; i++) //k nodi veri
    {
        Nodi[i] = t[cont];
        cont++;
        molt[i] = '1';
    }
    for(i = m + k + 1; i <= k + 2 * m; i++) //m nodi fittizzi a dx in corrispondenza di b
    {
        Nodi[i] = 1;
    }

    if(mod_moltep == 1)
    {
        //più aumenta la molteplicità, più si riducono le condizioni di raccordo nei nodi
        //se m1=m non c'è raccordo nel nodo i-esimo
        val_nodo = Nodi[i_nodo];
        if(val_moltep == 2)
            molt[i] = '2';
        if(val_moltep == 3)
            molt[i] = '3';
        for(i = 1; i <= val_moltep; i++)
        {
            Nodi[i_nodo + i - 1] = val_nodo;
        }
    }
}

/***************************************** De_Boor() ****************/
void De_Boor()
{
    float tg, cx[5], cy[5], cz[5], denom, a1, a2, sx, sy;
    int l, j, i;
	int n;
    for(tg = 0.0; tg <= 1; tg += 0.0001) //per ogni parametro
    {
        //localizziamo l'intervallo internodale a cui tg appartiene [Nodi[l],Nodi[l+1]]
        l = Bisezione(tg);

        //nell'algoritmo di de boor sostituisco alla b spline di ordine m la combinazione di due b-spline di ordine inferiore
        for(j = 1; j <= m; j++)
        {   //moltiplico per i pesi...all'inizio sono tutti 1...poi li posso variare con una funzione apposita
            cx[j] = Punti[j + l - m].x * w[j + l - m];
            cy[j] = Punti[j + l - m].y * w[j + l - m];
            cz[j] = w[j + l - m];
        }
        for(j = 2; j <= m; j++) //valuto la curva in un punto mediante la combinazione di coefficienti
        {   //il valore della spline calcolata in x è quello di una spline di ordine uno per un opportuno coefficiente
            for(i = m; i >= j; i--)
            {
                denom = Nodi[i + l - j + 1] - Nodi[i + l - m];
                a1 = (tg - Nodi[i + l - m]) / denom;
                a2 = 1 - a1;
                cx[i] = a1 * cx[i] + a2 * cx[i - 1];
                cy[i] = a1 * cy[i] + a2 * cy[i - 1];
                cz[i] = a1 * cz[i] + a2 * cz[i - 1];

            }
        }
        sx = cx[m] / cz[m];
        sy = cy[m] / cz[m];
		n=tg*10000;
		curveDots[n].x = sx;
		curveDots[n].y = sy;
		curveDots[n].z = 0;
        glColor3f(1.0,1.0,1.0);
		glBegin(GL_POINTS);
        glVertex2f(sx, sy);
        glEnd();
    }
    glFlush();
}

/***************************************** display2() ***************/
void display2()
{
    glClear(GL_COLOR_BUFFER_BIT);
    int i;
	glBegin(GL_LINES);
	//y
		glVertex2f(5,spline_window_height-5);
		glVertex2f(10,spline_window_height-15);
		glVertex2f(15,spline_window_height-5);
		glVertex2f(5,spline_window_height-25);
	//x
		glVertex2f(spline_window_width-215,20);
		glVertex2f(spline_window_width-200,5);
		glVertex2f(spline_window_width-200,20);
		glVertex2f(spline_window_width-215,5);
	glEnd();
    glBegin(GL_POINTS);//per visualizzare anche i [ dove si clicca di colore blu
    for (i = 1; i <= last; i++)
        glVertex2f(Punti[i].x, Punti[i].y);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP); //poligono di controllo
    for (i = 1; i <= last; i++)
        glVertex2f(Punti[i].x, Punti[i].y);
    glEnd();
    n = last; //setto n al numero di punti disegnato

    for(i = 1; i <= n; i++)
        w[i] = 1; //metto tutti i pesi a 1

    Parametrizzazione_Corde();

    glPointSize(2);//la curva la disegno sempre di dimensione 2

    if (last > 3)
    {
        Costruisci_Nodi();
        De_Boor();
    }

    glFlush();
}

/***************************************** myMouse2() ***************/
void myMouse2(int button, int state, int x, int y)
{
    const int screenHeight = spline_window_height;
    float y1 = spline_window_height - y;
    int i;
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && last < (NUM - 1)) //se il sinistro è premuto
    {
        Punti[++last].x = x; //aggiungo un punto nel vettore di coordinate (x,screenHeight-y),perchè il (0,0) parte da basso a sx
        Punti[last].y = screenHeight - y;
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        for (i = 1; i <= last; i++)
            glVertex2f(Punti[i].x, Punti[i].y);
        glEnd();
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (i = 1; i <= last; i++)
            glVertex2f(Punti[i].x, Punti[i].y);
        glEnd();
        glutPostRedisplay();
    }

    else if(button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) //col tasto centrale torno indietro di un punto
    {
        if(last > 0)
            last = last - 1;
        glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        for (i = 1; i <= last; i++)
            glVertex2f(Punti[i].x, Punti[i].y);
        glEnd();
		glBegin(GL_LINES);
		//y
			glVertex2f(5,spline_window_height-5);
			glVertex2f(10,spline_window_height-15);
			glVertex2f(15,spline_window_height-5);
			glVertex2f(5,spline_window_height-25);
		//x
			glVertex2f(spline_window_width-215,20);
			glVertex2f(spline_window_width-200,5);
			glVertex2f(spline_window_width-200,20);
			glVertex2f(spline_window_width-215,5);
		glEnd();
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINE_STRIP);
        for (i = 1; i <= last; i++)
            glVertex2f(Punti[i].x, Punti[i].y);
        glEnd();
    }

    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        for (i = 1; i <= last; i++) //scorro tutti i punti della lista
        {
            // se ho preso in un punto
            if (x >= Punti[i].x - dimpunto / 2 && x <= Punti[i].x + dimpunto / 2 && y1 >= Punti[i].y - dimpunto / 2 && y1 <= Punti[i].y + dimpunto / 2)
            {
                scelta = i;//conosco quale punto ho selezionato
                if (first == true)//alla prima volta che vede che il tasto è premuto lo setto a false, in modo tale da dirgli che da adesso in poi può trascinare
                    first = false;
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
        first = true;

    glFlush();
}

/***************************************** myinit() *****************/
void myinit(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);       //sfondo:   rgb + trasparenza
    glPointSize(4.0);                   //un punto è quadrato di lat 4 pixel
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, spline_window_width, 0, spline_window_height);
}

/***************************************** GluiSetting() ************/
void GluiSetting(void)
{
	/************************************ SIDE SUBWINDOW *******************/
	/*** CREATE SIDE SUBWINDOW ***/
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);


    /***** OBJECTS PARAMS CONTROL *****/
	obj_panel = new GLUI_Rollout(glui, "Properties", false);
    
	new GLUI_Checkbox(obj_panel, "Wireframe", &wireframe, 1, control_cb);
	new GLUI_Checkbox(obj_panel, "Show Axes", &show_axes, 1, control_cb);
	new GLUI_StaticText(glui," ");

    /******** LIGHTS CONTROL ********/
    GLUI_Rollout *roll_lights = new GLUI_Rollout(glui, "Lights", false);

    GLUI_Panel *light0 = new GLUI_Panel(roll_lights, "Light 1");
    GLUI_Panel *light1 = new GLUI_Panel(roll_lights, "Light 2");

	//LIGHT1
    new GLUI_Checkbox(light0, "Enabled", &light0_enabled, LIGHT0_ENABLED_ID, control_cb);
    light0_spinner = new GLUI_Spinner(light0, "Intensity:", &light0_intensity, LIGHT0_INTENSITY_ID, control_cb);
    light0_spinner->set_float_limits(0.0, 1.0);
    GLUI_Scrollbar *sb;
	new GLUI_StaticText(light0,"Red:");
    sb = new GLUI_Scrollbar(light0, "Red", GLUI_SCROLL_HORIZONTAL, &light0_diffuse[0], LIGHT0_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	new GLUI_StaticText(light0,"Green:");
    sb = new GLUI_Scrollbar(light0, "Green", GLUI_SCROLL_HORIZONTAL, &light0_diffuse[1], LIGHT0_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	new GLUI_StaticText(light0,"Blue:");
    sb = new GLUI_Scrollbar(light0, "Blue", GLUI_SCROLL_HORIZONTAL, &light0_diffuse[2], LIGHT0_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	//LIGHT1 ROTATION
	light0_rot = new GLUI_Rotation(light0, "Light 1", light0_rotation);
    light0_rot->set_spin(.82);

    //LIGHT2
	new GLUI_Checkbox(light1, "Enabled", &light1_enabled, LIGHT1_ENABLED_ID, control_cb);
    light1_spinner = new GLUI_Spinner(light1, "Intensity:", &light1_intensity, LIGHT1_INTENSITY_ID, control_cb);
    light1_spinner->set_float_limits(0.0, 1.0);
	new GLUI_StaticText(light1,"Red:");
    sb = new GLUI_Scrollbar(light1, "Red", GLUI_SCROLL_HORIZONTAL, &light1_diffuse[0], LIGHT1_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	new GLUI_StaticText(light1,"Green:");
    sb = new GLUI_Scrollbar(light1, "Green", GLUI_SCROLL_HORIZONTAL, &light1_diffuse[1], LIGHT1_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	new GLUI_StaticText(light1,"Blue:");
    sb = new GLUI_Scrollbar(light1, "Blue", GLUI_SCROLL_HORIZONTAL, &light1_diffuse[2], LIGHT1_INTENSITY_ID, control_cb);
    sb->set_float_limits(0, 1);
	//LIGHT2 ROTATION
	light1_rot = new GLUI_Rotation(light1, "Light 2", light1_rotation);
    light1_rot->set_spin(.82);
	new GLUI_StaticText(glui," ");
	/*** OBJECTS LISTBOX ***/
	object_panel = glui->add_panel( " Objects " );

	GLUI_Listbox *listbox = glui->add_listbox_to_panel(object_panel, "Select   ", &WhichObject);
	listbox->add_item(CUBE,"Cube");
	listbox->add_item(SPHERE,"Sphere");
	listbox->add_item(CONE,"Cone");
	listbox->add_item(CYLINDER,"Cylinder");
	listbox->add_item(PLANE,"Plane");
	listbox->add_item(TETRAHEDRON,"Tetrahedron");
	listbox->add_item(OCTAHEDRON,"Octahedron");
	listbox->add_item(DODECAHEDRON,"Dodecahedron");
	listbox->add_item(ICOSAHEDRON,"Icosahedron");
	listbox->add_item(TORUS,"Torus");
	listbox->add_item(TEAPOT,"Teapot");
	listbox->add_item(OBJFILE,"OBJ File");
	listbox->add_item(SPLINE,"Revolution Surf");

	GLUI_Scrollbar *sbo;
	new GLUI_StaticText(object_panel,"Red:");
    sbo = new GLUI_Scrollbar(object_panel, "Red", GLUI_SCROLL_HORIZONTAL, &object_color[0]);
    sbo->set_float_limits(0, 1);
	new GLUI_StaticText(object_panel,"Green:");
    sbo = new GLUI_Scrollbar(object_panel, "Green", GLUI_SCROLL_HORIZONTAL, &object_color[1]);
    sbo->set_float_limits(0, 1);
	new GLUI_StaticText(object_panel,"Blue:");
    sbo = new GLUI_Scrollbar(object_panel, "Blue", GLUI_SCROLL_HORIZONTAL, &object_color[2]);
    sbo->set_float_limits(0, 1);
	new GLUI_StaticText(object_panel," ");
	new GLUI_StaticText(glui," ");
	/*** FILE BROWSER ***/
	new GLUI_StaticText(object_panel,"Select Obj File:");
	fb = new GLUI_FileBrowser(object_panel,"", false, GET_FILE, control_cb);
	fb->set_h(150);
	GLUI_Button *button = glui->add_button_to_panel(object_panel, "Add", ADD_ID, (GLUI_Update_CB)listbox_cb);
	new GLUI_StaticText(glui," ");
	/****** CLEAR BUTTON *****/
	new GLUI_Button(glui, "Clear Scene", RESET_ID,(GLUI_Update_CB) reset_cb);

    //new GLUI_StaticText(glui, "");

    /****** QUIT BUTTON *****/
    new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	/************************************ SIDE SUBWINDOW END *******************/


	/************************************ BOTTOM SUBWINDOW *******************/
    /*** CREATE BOTTOM SUBWINDOW ***/
    glui2 = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
    
	/**CREATE PANEL**/
	GLUI_Panel *camera_panel = new GLUI_Panel(glui2, "Scene Transformation");
    new GLUI_Column(glui2, false);
	GLUI_Panel *picked_panel = new GLUI_Panel(glui2, "Picked Object Transformation");

	/**CAMERA PANEL**/
	
	//SCENE ROTATION
	GLUI_Rotation *view_rot = new GLUI_Rotation(camera_panel, "Scene", view_rotate);
    view_rot->set_spin(1.0);

	//X TRANSLATION
    new GLUI_Column(camera_panel, false);
	trans_x = glui2->add_translation_to_panel(camera_panel, "Scene X", GLUI_TRANSLATION_X, obj_pos);
    trans_x->set_speed(.005);

	//Y TRANSLATION
    new GLUI_Column(camera_panel, false);
	trans_y = glui2->add_translation_to_panel(camera_panel, "Scene Y", GLUI_TRANSLATION_Y, &obj_pos[1]);
    trans_y->set_speed(.005);

	//Z TRANSLATION
    new GLUI_Column(camera_panel, false);
	trans_z = glui2->add_translation_to_panel(camera_panel, "Scene Z", GLUI_TRANSLATION_Z, &obj_pos[2]);
    trans_z->set_speed(.005);

	new GLUI_Column(camera_panel, false);
	GLUI_Spinner *scale_spinner = new GLUI_Spinner(camera_panel, "Scale :", &scale);
    scale_spinner->set_float_limits(.01f, 4.0);
    scale_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Spinner *scaleX_spinner = new GLUI_Spinner(camera_panel, "Scale X:", &scaleX);
    scaleX_spinner->set_float_limits(.01f, 4.0);
    scaleX_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	GLUI_Spinner *scaleY_spinner = new GLUI_Spinner(camera_panel, "Scale Y:", &scaleY);
    scaleY_spinner->set_float_limits(.01f, 4.0);
    scaleY_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	GLUI_Spinner *scaleZ_spinner = new GLUI_Spinner(camera_panel, "Scale Z:", &scaleZ);
    scaleZ_spinner->set_float_limits(.01f, 4.0);
    scaleZ_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	/**PICKED OBJECT PANEL**/

	//PICKED ROTATION
	piked_rot = new GLUI_Rotation(picked_panel, "Object", picking_rotate);
    piked_rot->set_spin(1.0);//era 0.98

	//X TRANSLATION
    new GLUI_Column(picked_panel, false);
	pick_trans_x = glui2->add_translation_to_panel(picked_panel, "Object X", GLUI_TRANSLATION_X, pick_obj_pos);
    pick_trans_x->set_speed(.005);

	//Y TRANSLATION
    new GLUI_Column(picked_panel, false);
	pick_trans_y = glui2->add_translation_to_panel(picked_panel, "Object Y", GLUI_TRANSLATION_Y, &pick_obj_pos[1]);
    pick_trans_y->set_speed(.005);

	//Z TRANSLATION
    new GLUI_Column(picked_panel, false);
	pick_trans_z = glui2->add_translation_to_panel(picked_panel, "Object Z", GLUI_TRANSLATION_Z, &pick_obj_pos[2]);
    pick_trans_z->set_speed(.005);

	new GLUI_Column(picked_panel, false);
	GLUI_Spinner *pick_scale_spinner = new GLUI_Spinner(picked_panel, "Scale :", &pick_scale);
    pick_scale_spinner->set_float_limits(.01f, 4.0);
    pick_scale_spinner->set_alignment(GLUI_ALIGN_RIGHT);

    GLUI_Spinner *pick_scaleX_spinner = new GLUI_Spinner(picked_panel, "Scale X:", &pick_scaleX);
    pick_scaleX_spinner->set_float_limits(.01f, 4.0);
    pick_scaleX_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	GLUI_Spinner *pick_scaleY_spinner = new GLUI_Spinner(picked_panel, "Scale Y:", &pick_scaleY);
    pick_scaleY_spinner->set_float_limits(.01f, 4.0);
    pick_scaleY_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	GLUI_Spinner *pick_scaleZ_spinner = new GLUI_Spinner(picked_panel, "Scale Z:", &pick_scaleZ);
    pick_scaleZ_spinner->set_float_limits(.01f, 4.0);
    pick_scaleZ_spinner->set_alignment(GLUI_ALIGN_RIGHT);

	GLUI_Spinner *spinner =  new GLUI_Spinner(picked_panel, "Segments:", &segments);
    spinner->set_int_limits(3, 60);
    spinner->set_alignment(GLUI_ALIGN_RIGHT);

	glui2->set_main_gfx_window(main_window);
	/************************************ BOTTOM SUBWINDOW END *******************/


	/*** CREATE MOUSE_RIGHT_BUTTON MENU ***/
	id_MENU1=glutCreateMenu(menu1);

	//Creazione SubMenu
	id_MENU2=glutCreateMenu(menu1);
	glutAddMenuEntry("Top",WIEW_TOP);	
	glutAddMenuEntry("Side",WIEW_SIDE);
	glutAddMenuEntry("Front",WIEW_FRONT);
	glutAddMenuEntry("45 Gradi",WIEW_PERP);

	glutSetMenu(id_MENU1);		
	glutAddSubMenu("Viste",id_MENU2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/***************************************** LightsSetting() **********/
void LightsSetting(void)
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 100.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glShadeModel (GL_SMOOTH);

	glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

}

/***************************************** main() *******************/
int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition((screen_width - window_width) / 2 , (screen_height - window_height) / 2);
    glutInitWindowSize(window_width, window_height);
    main_window = glutCreateWindow("Maya Simulator");
    glutDisplayFunc(myGlutDisplay);
    GLUI_Master.set_glutReshapeFunc(myGlutReshape);
    GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);
    GLUI_Master.set_glutSpecialFunc(NULL);
    GLUI_Master.set_glutMouseFunc(myGlutMouse);
    glutMotionFunc(myGlutMotion);

    glEnable(GL_DEPTH_TEST);

	GluiSetting();
	LightsSetting();
    reset_cb(0);

#if 0
    /**** We register the idle callback with GLUI, *not* with GLUT ****/
    GLUI_Master.set_glutIdleFunc(myGlutIdle);
#endif

    glutMainLoop();

    return EXIT_SUCCESS;
}

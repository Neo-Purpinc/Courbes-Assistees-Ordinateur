#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
/* dimensions de la fenetre */
int width = 1600;
int height = 900;

/*************************************************************************/
/* Bezier */
/*************************************************************************/
#define MAX_POINTS 100
typedef struct
{
	float x, y, z;
} Point;
typedef struct
{
	int selected;
	Point pdc[4][4];
} Carreau;
typedef enum {
	NONE,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_BACK,
	MOVE_FRONT,
	MOVE_UP,
	MOVE_DOWN,
} MoveType;
typedef enum {
	POINTS,
	QUADS
} MODE;
MoveType moveType = NONE;
MODE mode = POINTS;

Carreau exemple = {0, {
	{
		{0, 0, 0},
		{0, 1, 0},
		{0, 2, 0},
		{0, 3, 0}
	},
	{
		{1, 0, 0},
		{1, 1, 2},
		{1, 2, 1},
		{1, 3, 1}
	},
	{
		{2, 0, 0},
		{2, 1, 2},
		{2, 2, 1},
		{2, 3, 1}
	},
	{
		{3, 0, 0},
		{3, 1, 0},
		{3, 2, 0},
		{3, 3, 0}
	}
}};
Point pt(float x, float y, float z)
{
	Point p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}
Point bezier3D(float s, float t){
	Point p;
	return p;
 }

/*************************************************************************/
/* Fonctions de dessin */
/*************************************************************************/

/* rouge vert bleu entre 0 et 1 */
void chooseColor(double r, double g, double b)
{
	glColor3d(r,g,b);
}

void drawPoint(Point p)
{
	glBegin(GL_POINTS);
	glVertex3d(p.x,p.y,p.z);
	glEnd();
}

void drawLine(Point p1, Point p2)
{
	glBegin(GL_LINES);
	glVertex3d(p1.x,p1.y,p1.z);
	glVertex3d(p2.x,p2.y,p2.z);
	glEnd();
}

void drawQuad(Point p1, Point p2, Point p3, Point p4)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex3d(p1.x,p1.y,p1.z);
	glVertex3d(p2.x,p2.y,p2.z);
	glVertex3d(p3.x,p3.y,p3.z);
	glVertex3d(p4.x,p4.y,p4.z);
	glEnd();
}

void drawSurface(){
	switch (mode)
	{
		case POINTS:
			chooseColor(1,0,0);
			for(float s=0.; s<=1.; s+=0.001){
				for(float t=0.; t<=1.; t+=0.001){
					Point p = bezier3D(s,t);
					drawPoint(p);
				}
			}
			break;
		case QUADS:
			chooseColor(0.6,0,0.5);
			// draw small quads inside the big ones
			float pas = 0.1;
			for(float s=0.; s<=1.; s+=pas){
				for(float t=0.; t<=1.; t+=pas){
					Point p = bezier3D(s,t);
					Point p1 = bezier3D(s,t+pas);
					Point p2 = bezier3D(s+pas,t+pas);
					Point p3 = bezier3D(s+pas,t);
					drawQuad(p,p1,p2,p3);
				}
			}
			break;
	}
}
/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i,j;
	
	glEnable(GL_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 60, (float)width/height, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-4,-2,-5);
	glRotated(25,1,0,0);
	glRotated(90,0,1,0);
	glRotated(-120,1,1,1);
	
	// ** Repere du Monde **
	chooseColor(1.0,0,0); // axe x=rouge
	drawLine(pt(0,0,0), pt(10,0,0));
	chooseColor(0,1.0,0); // axe y=vert
	drawLine(pt(0,0,0), pt(0,10,0));
	chooseColor(0,0,1.0); // axe z=bleu
	drawLine(pt(0,0,0), pt(0,0,10));
	
	// ** Grille sur plan XY **
	chooseColor(0.2,0.2,0.2); 
	glLineWidth(2.0);
	for (i=1; i<10; i++)
	{
		drawLine(pt(i,0,0), pt(i,9,0));
		drawLine(pt(0,i,0), pt(9,i,0));
	}

	// ** Dessinez ici **
	chooseColor(1,1,1);
	drawQuad(exemple.pdc[0][0], exemple.pdc[0][1], exemple.pdc[0][2], exemple.pdc[0][3]);
	drawQuad(exemple.pdc[1][0], exemple.pdc[1][1], exemple.pdc[1][2], exemple.pdc[1][3]);
	drawQuad(exemple.pdc[2][0], exemple.pdc[2][1], exemple.pdc[2][2], exemple.pdc[2][3]);
	drawQuad(exemple.pdc[3][0], exemple.pdc[3][1], exemple.pdc[3][2], exemple.pdc[3][3]);
	// drawSurface();
	chooseColor(0,0,1);
	glutSwapBuffers();
}

void selectNextControlPoint(Carreau* c){
	if(++c->selected > 15)
		c->selected = 0;
}

void moveSelectedPoint(Carreau *c, MoveType type){
	int a = c->selected /4;
	int b = c->selected %4;
	switch(type){
		case MOVE_LEFT:
			c->pdc[a][b].x += 0.1;
			break;
		case MOVE_RIGHT:
			c->pdc[a][b].x -= 0.1;
			break;
		case MOVE_BACK:
			c->pdc[a][b].y += 0.1;
			break;
		case MOVE_FRONT:
			c->pdc[a][b].y -= 0.1;
			break;
		case MOVE_UP:
			c->pdc[a][b].z += 0.1;
			break;
		case MOVE_DOWN:
			c->pdc[a][b].z -= 0.1;
			break;
		default:
			break;
	}
}
void keyboard(unsigned char keycode, int x, int y)
{
	bool move = false;
	if(keycode == 27)
		exit(0);
	else if(keycode == 'a' || keycode == 'A'){
		moveType = MOVE_LEFT;
		move = true;
	}
	else if(keycode == 'z' || keycode == 'Z'){
		moveType = MOVE_BACK;
		move = true;
	}
	else if(keycode == 'e' || keycode == 'E'){
		moveType = MOVE_UP;
		move = true;
	}
	else if(keycode == 'q' || keycode == 'Q'){
		moveType = MOVE_RIGHT;
		move = true;
	}
	else if(keycode == 's' || keycode == 'S'){
		moveType = MOVE_FRONT;
		move = true;
	}
	else if(keycode == 'd' || keycode == 'D'){
		moveType = MOVE_DOWN;
		move = true;
	}
	else if(keycode == 'f' || keycode == 'F'){
		selectNextControlPoint(&exemple);
	}
	else if (keycode == 'w' || keycode == 'W'){
		if (mode == QUADS)
			mode = POINTS;
		else
			mode = QUADS;
		printf("Mode %s\n", mode == QUADS ? "Quads" : "Points");
	}
	if(move)
		moveSelectedPoint(&exemple, moveType);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width=w;
	height=h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D( 0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n",x,y);
		glutPostRedisplay();
	}

}



/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[]) 
{
	/* Initialisations globales */
	glutInit(&argc, argv);

	/* D�finition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Cr�ation de la fenetre */
    glutCreateWindow("Carreau de Bezier");

	/* Choix de la fonction d'affichage */
	glutDisplayFunc(display);

	/* Choix de la fonction de redimensionnement de la fenetre */
	glutReshapeFunc(reshape);
	
	/* Choix des fonctions de gestion du clavier */
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	
	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);

	/* Boucle principale */
    glutMainLoop();

	/* M�me si glutMainLoop ne rends JAMAIS la main, il faut d�finir le return, sinon
	le compilateur risque de crier */
    return 0;
}

#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>

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
typedef Point Carreau[4];
Carreau exemple = {
	{4, 1, 1},
	{4, 4, 3},
	{1, 3, 1},
	{1, 1, 3}
};
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
	p.x = (1-t)*(1-s)*exemple[0].x + t*(1-s)*exemple[1].x + t*s*exemple[2].x + (1-t)*s*exemple[3].x;
	p.y = (1-t)*(1-s)*exemple[0].y + t*(1-s)*exemple[1].y + t*s*exemple[2].y + (1-t)*s*exemple[3].y;
	p.z = (1-t)*(1-s)*exemple[0].z + t*(1-s)*exemple[1].z + t*s*exemple[2].z + (1-t)*s*exemple[3].z;
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
	glVertex3d(p2.x,p2.y,p2.z);typedef Point Vecteur[MAX_POINTS];
	glEnd();
}

void drawQuad(Carreau c)
{
	chooseColor(1,1,1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex3f(c[0].x,c[0].y,c[0].z);
	glVertex3f(c[1].x,c[1].y,c[1].z);
	glVertex3f(c[2].x,c[2].y,c[2].z);
	glVertex3f(c[3].x,c[3].y,c[3].z);
	glEnd();
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
	drawQuad(exemple);
	chooseColor(1,0,0);
	for(float s=0.; s<=1.; s+=0.005){
		float t = 1-s;
		Point p = bezier3D(s,t);
		drawPoint(p);
	}
	glutSwapBuffers();
}

void keyboard(unsigned char keycode, int x, int y)
{
	/* touche ECHAP */
	if (keycode==27) 
		exit(0);
	/* touche ECHAP */
	if (keycode=='z')
		printf("La touche z a ete enfoncee\n");

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

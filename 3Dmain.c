#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;

/*************************************************************************/
/* Bezier */
/*************************************************************************/

#define MAX_POINTS 100
typedef struct
{
	float x, y, z;
} Point;

Point pt(float x, float y, float z)
{
	Point p;
	p.x = x;
	p.y = y;
	p.z = z;
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
	glVertex3f(p1.x,p1.y,p1.z);
	glVertex3f(p2.x,p2.y,p2.z);
	glVertex3f(p3.x,p3.y,p3.z);
	glVertex3f(p4.x,p4.y,p4.z);
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

	/* Définition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Création de la fenetre */
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

	/* Même si glutMainLoop ne rends JAMAIS la main, il faut définir le return, sinon
	le compilateur risque de crier */
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
//#include <GL/glx.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;


/*************************************************************************/
/* Bezier */
/*************************************************************************/

#define MAX_POINTS 100
typedef struct
{
	float x, y;
} Point;
int nbPoints = 0;
typedef Point Vecteur[MAX_POINTS];
Vecteur poly;

Point minus(Point a, Point b)
{
	return (Point){ a.x - b.x, a.y - b.y };
}

Point Hermite(float t, int noHermite)
{
	noHermite *= 4;

	Point a = poly[0 + noHermite];
	Point b = poly[2 + noHermite];
	Point ta = minus(poly[1 + noHermite], a);
	Point tb = minus(poly[3 + noHermite], b);

	float t2 = t*t;
	float t3 = t2*t;
	float h0 = 1.0 - 3.0*t2 + 2.0*t3;
	float h1 = 3.0*t2 - 2.0*t3;
	float h2 = t - 2.0*t2 + t3;
	float h3 = -t2 + t3;

	return (Point)
	{
		h0*a.x + h1*b.x + h2*ta.x + h3*tb.x,
		h0*a.y + h1*b.y + h2*ta.y + h3*tb.y
	};
}

/*************************************************************************/
/* Fonctions de dessin */
/*************************************************************************/

/* rouge vert bleu entre 0 et 1 */
void chooseColor(double r, double g, double b)
{
	glColor3d(r,g,b);
}

void drawPoint(double x, double  y)
{
	glBegin(GL_POINTS);
	glVertex2d(x,y);
	glEnd();
}

void drawLine(double x1, double  y1, double x2, double y2)
{
	glBegin(GL_LINES);
	glVertex2d(x1,y1);
	glVertex2d(x2,y2);
	glEnd();
}

void drawHermite()
{
	chooseColor(1,1,1);
	int nbHermite = nbPoints/4;
	int res = 30;
	for(int i = 0; i < nbHermite; i++)
	{
		Point a;
		for(int j = 0; j <= res; j++)
		{
			float t = (float)j/res;
			Point b = Hermite(t, i);
			if(j > 0)
			{
				drawLine(a.x, a.y, b.x, b.y);
			}
			a = b;
		}
	}
}


/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT);

	// tracé du polygone de controle
	chooseColor(1,0,0);
	if (nbPoints == 1) 
		drawPoint(poly[0].x, poly[0].y);
	else if (nbPoints > 1) 
	{
		for (i=0;i<nbPoints-1;i+=2)
		{
			drawLine(poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
		}
	}

	// ** Dessiner ici ! **
	drawHermite();


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

/* Evenement : clic souris */
void mouse(int button, int state, int x, int y)
{
	int i;
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n",x,y);
		poly[nbPoints].x = x;
		poly[nbPoints].y = y;
		nbPoints++;
		glutPostRedisplay();
	}
}

/* Evenement : souris bouge */
void mousemove(int x, int y)
{
}

/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[]) 
{
	/* Initialisations globales */
	glutInit(&argc, argv);

	/* Définition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Création de la fenetre */
    glutCreateWindow("Courbes");

	/* Choix de la fonction d'affichage */
	glutDisplayFunc(display);

	/* Choix de la fonction de redimensionnement de la fenetre */
	glutReshapeFunc(reshape);
	
	/* Choix des fonctions de gestion du clavier */
	glutKeyboardFunc(keyboard);
	//glutSpecialFunc(special);
	
	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);

	/* Boucle principale */
    glutMainLoop();

	/* Même si glutMainLoop ne rends JAMAIS la main, il faut définir le return, sinon
	le compilateur risque de crier */
    return 0;
}

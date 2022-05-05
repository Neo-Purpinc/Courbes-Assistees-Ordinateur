#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
//#include <GL/glx.h>
#include <math.h>
/* dimensions de la fenetre */
int width = 600;
int height = 400;
int N = 100;

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
float fact(int n)
{
	if(n==0)
		return 1;
	else
		return n*fact(n-1);
}	
float k_parmi_n(int n, int k)
{
	if(k>n)
		return 0;
	else
		return fact(n)/(fact(k)*fact(n-k));
}

float Bernstein(int n, int i, float t)
{
	return k_parmi_n(n,i)*pow(t,i)*pow(1-t,n-i);
}

Point sum(Point a, Point b)
{
	return (Point){ a.x + b.x, a.y + b.y };
}
Point mult(Point a, float b)
{
	return (Point){ a.x * b, a.y * b };
}
Point Bezier(float t){
	Point p = {0,0};
	for(int i = 0; i < nbPoints; i++){
		p = sum(p,mult(poly[i],Bernstein(nbPoints-1,i,t)));
	}
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


/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT);

	// trac� du polygone de controle
	chooseColor(1,1,1);
	if (nbPoints == 1) 
		drawPoint(poly[0].x, poly[0].y);
	else if (nbPoints > 1) 
	{
		for (i=0;i<nbPoints-1;i++)
			drawLine(poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
	}

	// ** Dessiner ici ! **
	Vecteur tmp;
	Point p ;
	float pas = 1./N;
	if(nbPoints > 2){
		chooseColor(1,0,0);
		int i = 0;
		for(float t = 0.0; t <= 1.0; t += pas,i++)
		{
			p = Bezier(t);
			tmp[i] = p;
		}
		for(int i = 0; i < N-1; i++){
			drawLine(tmp[i].x, tmp[i].y, tmp[i+1].x, tmp[i+1].y);
		}
	} else if (nbPoints <= 1) {
		drawPoint(poly[0].x, poly[0].y);
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

	/* D�finition des attributs de la fenetre OpenGL */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	/* Placement de la fenetre */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	
	/* Cr�ation de la fenetre */
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

	/* M�me si glutMainLoop ne rends JAMAIS la main, il faut d�finir le return, sinon
	le compilateur risque de crier */
    return 0;
}

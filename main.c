#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
//#include <GL/glx.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;
typedef enum{ LIGNES, POINTS } Mode;
Mode MODE = LIGNES;
bool bezierConstru = false;

int selected = -1;

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
float pas = 0.01;
float t = 0.5;

Point Bezier(float t,bool draw){
	Vecteur tmp;
	for(int i = 0; i < nbPoints; i++){
		tmp[i] = poly[i];
	}
	for(int k = 1; k < nbPoints; k++){
		for(int i = 0; i < nbPoints - k; i++){
			tmp[i].x = t * tmp[i].x + (1-t) * tmp[i+1].x;
			tmp[i].y = t * tmp[i].y + (1-t) * tmp[i+1].y;
		}
		if(draw){
			for(int i = 0; i < nbPoints - k; i++){
				drawLine(tmp[i].x,tmp[i].y,tmp[i+1].x,tmp[i+1].y);
			}
			if(k==1)
				drawLine(poly[0].x, poly[0].y, tmp[0].x, tmp[0].y);	
		}

	}
	return tmp[0];
}

void drawBezierCurve(bool draw){
	Vecteur bezier_points;
	int i = 0;
	for(float t = 0; t <= 1; t += pas,i++){
		bezier_points[i] = Bezier(t,false);
	}
	if(draw){
		chooseColor(1,0,0);
		Bezier(t,draw);
	}
	chooseColor(0,1,0);
	for(i = 0; i < (int)ceil(1/pas) - 1; i++){
		drawLine(bezier_points[i].x, bezier_points[i].y, bezier_points[i+1].x, bezier_points[i+1].y);
	}
	drawLine(poly[0].x, poly[0].y,bezier_points[(int)ceil(1/pas) - 1].x, bezier_points[(int)ceil(1/pas) - 1].y);
	
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
	switch(MODE){
		case LIGNES:
			if (nbPoints == 1) 
				drawPoint(poly[0].x, poly[0].y);
			else if (nbPoints > 1) 
			{
				for (i=0;i<nbPoints-1;i++)
					drawLine(poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
			}
			break;
		case POINTS:
			for (i=0;i<nbPoints;i++)
				drawPoint(poly[i].x, poly[i].y);
			break;
	}
	drawBezierCurve(bezierConstru);
	glutSwapBuffers();
}

void keyboard(unsigned char keycode, int x, int y)
{
	/* touche ECHAP */
	if (keycode==27)
		exit(0);
	else if (keycode==' ')
		nbPoints = 0;
	else if (keycode=='b'){
		bezierConstru = !bezierConstru;
	}
	else if (keycode=='c'){
		if(MODE!=LIGNES)
			MODE = LIGNES;
		else
			MODE = POINTS;
	}
	glutPostRedisplay();
}
void special(int keycode, int x, int y)
{
	if (keycode==GLUT_KEY_UP && pas < 0.5)
		pas *= 2;
	else if (keycode==GLUT_KEY_DOWN && pas > 0.01)
		pas /= 2;
	else if (keycode==GLUT_KEY_LEFT && t > 0)
		t -= 0.05;
	else if (keycode==GLUT_KEY_RIGHT && t < 1)
		t += 0.05;
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
int getPointIndex(int x, int y){
	for(int i = 0; i < nbPoints; i++){
		if(abs(poly[i].x - x) < 10 && abs(poly[i].y - y) < 10)
			return i;
	}
	return -1;
}
/* Evenement : clic souris */
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n",x,y);
		selected = getPointIndex(x,y);
		if(selected == -1){
			poly[nbPoints].x = x;
			poly[nbPoints].y = y;
			nbPoints++;
		}
		glutPostRedisplay();
	}
}

/* Evenement : souris bouge */
void mousemove(int x, int y)
{
	if(selected != -1){
		poly[selected].x = x;
		poly[selected].y = y;
		glutPostRedisplay();
	}
	printf("Mouse move at %d %d\n",x,y);
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
	glutSpecialFunc(special);
	
	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);

	/* Boucle principale */
    glutMainLoop();

	/* M�me si glutMainLoop ne rends JAMAIS la main, il faut d�finir le return, sinon
	le compilateur risque de crier */
    return 0;
}

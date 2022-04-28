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

#define MAX_POINTS 100000
#define GRID_SIDE 4
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

Point grille[GRID_SIDE][GRID_SIDE];

Point pdc[MAX_POINTS];

int cur_line = 0;
int cur_col = 0;

void moveXpoint(Point *p, float value)
{
	p->x += value;
}

void moveYpoint(Point *p, float value)
{
	p->y += value;
}

void moveZpoint(Point *p, float value)
{
	p->z += value;
}

Point BezierCarreau(Point A, Point B, Point C, Point D, float s, float t)
{
	Point P01;
	Point P11;
	Point P02;

	P01.x = s * A.x + (1 - s) * B.x;
	P01.y = s * A.y + (1 - s) * B.y;
	P01.z = s * A.z + (1 - s) * B.z;

	P11.x = s * C.x + (1 - s) * D.x;
	P11.y = s * C.y + (1 - s) * D.y;
	P11.z = s * C.z + (1 - s) * D.z;

	P02.x = t * P01.x + (1 - t) * P11.x;
	P02.y = t * P01.y + (1 - t) * P11.y;
	P02.z = t * P01.z + (1 - t) * P11.z;

	return P02;
}

Point Bezier(float s, float t)
{
	if (t < 0. || t > 1. || s < 0. || t > 1.)
	{
		printf("t ou s n'est pas entre 0 et 1\n");
		exit(-1);
	}

	int nb_passes = GRID_SIDE - 1;
	Point grid_c[GRID_SIDE][GRID_SIDE];
	int i, j, k;

	for (i = 0; i < GRID_SIDE; i++)
		for (j = 0; j < GRID_SIDE; j++)
			grid_c[i][j] = grille[i][j];

	for (i = 0; i < nb_passes; i++)
	{
		Point tmp_grid[GRID_SIDE][GRID_SIDE];
		for (j = 0; j < GRID_SIDE - i - 1; j++)
			for (k = 0; k < GRID_SIDE - i - 1; k++)
			{
				tmp_grid[j][k] = BezierCarreau(grid_c[j][k], grid_c[j][k + 1], grid_c[j + 1][k + 1], grid_c[j + 1][k], s, t);
				grid_c[j][k] = tmp_grid[j][k];
			}
	}

	return grid_c[0][0];
}

/*************************************************************************/
/* Fonctions de dessin */
/*************************************************************************/

/* rouge vert bleu entre 0 et 1 */
void chooseColor(double r, double g, double b)
{
	glColor3d(r, g, b);
}

void drawPoint(Point p)
{
	glBegin(GL_POINTS);
	glVertex3d(p.x, p.y, p.z);
	glEnd();
}

void drawLine(Point p1, Point p2)
{
	glBegin(GL_LINES);
	glVertex3d(p1.x, p1.y, p1.z);
	glVertex3d(p2.x, p2.y, p2.z);
	glEnd();
}

void drawQuad(Point p1, Point p2, Point p3, Point p4)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p4.x, p4.y, p4.z);
	glEnd();
}

/*************************************************************************/
/* Fonctions callback */
/*************************************************************************/

void display()
{
	int i, j;

	glEnable(GL_DEPTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)width / height, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-4, -2, -5);
	glRotated(25, 1, 0, 0);
	glRotated(90, 0, 1, 0);
	glRotated(-120, 1, 1, 1);

	// ** Repere du Monde **
	chooseColor(1.0, 0, 0); // axe x=rouge
	drawLine(pt(0, 0, 0), pt(10, 0, 0));
	chooseColor(0, 1.0, 0); // axe y=vert
	drawLine(pt(0, 0, 0), pt(0, 10, 0));
	chooseColor(0, 0, 1.0); // axe z=bleu
	drawLine(pt(0, 0, 0), pt(0, 0, 10));

	// ** Grille sur plan XY **
	chooseColor(0.2, 0.2, 0.2);
	glLineWidth(2.0);
	for (i = 1; i < 10; i++)
	{
		drawLine(pt(i, 0, 0), pt(i, 9, 0));
		drawLine(pt(0, i, 0), pt(9, i, 0));
	}

	// ** Dessinez ici **
	chooseColor(1, 1, 1);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			drawQuad(grille[i][j], grille[i][j + 1], grille[i + 1][j + 1], grille[i + 1][j]);

	float s, t;
	int nb_pts = 0;
	for (s = 0.; s <= 1.; s += 0.01)
		for (t = 0.; t <= 1.; t += 0.01)
		{
			pdc[nb_pts] = Bezier(s, t);
			nb_pts++;
		}

	chooseColor(0.0, 1.0, 0.);
	for (i = 0; i < nb_pts; i++)
		drawPoint(pdc[i]);

	glutSwapBuffers();
}

void keyboard(unsigned char keycode, int x, int y)
{
	/* touche ECHAP */
	if (keycode == 27)
		exit(0);
	if (keycode == 'a')
		moveZpoint(&grille[cur_line][cur_col], -0.1);
	if (keycode == 'z')
		moveYpoint(&grille[cur_line][cur_col], 0.1);
	if (keycode == 'e')
		moveZpoint(&grille[cur_line][cur_col], 0.1);
	if (keycode == 'q')
		moveXpoint(&grille[cur_line][cur_col], -0.1);
	if (keycode == 's')
		moveYpoint(&grille[cur_line][cur_col], -0.1);
	if (keycode == 'd')
		moveXpoint(&grille[cur_line][cur_col], 0.1);
	if (keycode == 'f')
		cur_line = (cur_line + 1) % 4;
	if (keycode == 'g')
		cur_col = (cur_col + 1) % 4;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		printf("Clic at %d %d\n", x, y);
		glutPostRedisplay();
	}
}

/*************************************************************************/
/* Fonction principale */
/*************************************************************************/

int main(int argc, char *argv[])
{
	grille[0][0] = pt(0, 0, 0); // P(0,0)
	grille[0][1] = pt(0, 1, 0); // P(0,1)
	grille[0][2] = pt(0, 2, 0); // P(0,2)
	grille[0][3] = pt(0, 3, 0); // P(0,3)

	grille[1][0] = pt(1, 0, 0); // P(1,0)
	grille[1][1] = pt(1, 1, 2); // P(1,1)
	grille[1][2] = pt(1, 2, 1); // P(1,2)
	grille[1][3] = pt(1, 3, 1); // P(1,3)

	grille[2][0] = pt(2, 0, 0); // P(2, 0)
	grille[2][1] = pt(2, 1, 2); // P(2, 1)
	grille[2][2] = pt(2, 2, 1); // P(2, 2)
	grille[2][3] = pt(2, 3, 1); // P(2, 3)

	grille[3][0] = pt(3, 0, 0); // P(3, 0)
	grille[3][1] = pt(3, 1, 0); // P(3, 1)
	grille[3][2] = pt(3, 2, 0); // P(3, 2)
	grille[3][3] = pt(3, 3, 0); // P(3, 3)

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
	// glutSpecialFunc(special);

	/* Choix de la fonction de gestion de la souris */
	glutMouseFunc(mouse);

	/* Boucle principale */
	glutMainLoop();

	/* M�me si glutMainLoop ne rends JAMAIS la main, il faut d�finir le return, sinon
	le compilateur risque de crier */
	return 0;
}
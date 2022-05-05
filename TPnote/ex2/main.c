#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* dimensions de la fenetre */
int width = 600;
int height = 400;

/*************************************************************************/
/* Bezier */
/*************************************************************************/

#define STEP 10.0
#define MAX_POINTS 100
#define M 4
#define N 3
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

Point mult(Point a, float s)
{
    Point res;
    res.x = a.x * s;
    res.y = a.y * s;
    res.z = a.z * s;
    return res;
}

Point add(Point a, Point b)
{
    Point res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

Point grille[M][N];

int fact(int n)
{
    int i, fact = 1;
    for (i = 1; i <= n; i++)
        fact = fact * i;
    return fact;
}

float Bernstein(int i, int n, float t)
{
    float n_i = (i>0 && i<n) ? fact(n)/(fact(i)*fact(n-i)) : 0;
    return n_i*pow(t,i)*pow(1-t,n-i);
}

Point Bezier3D(float s, float t)
{
    Point res;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            res = add(res, mult(grille[i][j], Bernstein(i, N, s)*Bernstein(j, M, t)));
        }
    }
    return res;
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

    chooseColor(1, 1, 1);
    for (int i=0; i<M-1; i++) {
        for (int j=0; j<N-1; j++) {
            drawQuad(grille[i][j], grille[i][j+1], grille[i+1][j+1], grille[i+1][j]);
        }
    }

    for (float s = 0.0; s <= 1.0; s += 1.0/STEP) {
        for (float t = 0.0; t <= 1.0; t += 1.0/STEP) {
            Point p = Bezier3D(s, t);
            drawPoint(p);
        }
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

    grille[4][0] = pt(4, 0, 0); // P(4, 0)
    grille[4][1] = pt(4, 1, 2); // P(4, 1)
    grille[4][2] = pt(4, 2, 1); // P(4, 2)
    grille[4][3] = pt(4, 3, 1); // P(4, 3)

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
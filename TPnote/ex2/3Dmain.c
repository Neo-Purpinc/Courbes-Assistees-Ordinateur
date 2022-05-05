#include <GL/glut.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
// A l'appui sur la touche Z, passe du mode Nuage de points au mode Quads

/* dimensions de la fenetre */
int width = 600;
int height = 400;
typedef enum {POINTS,QUADS} Mode;
Mode mode = POINTS;
int n = 100;
/*************************************************************************/
/* Bezier */
/*************************************************************************/
#define MAX_POINTS 100
typedef struct
{
	float x, y, z;
} Point;
Point grille[4][4];

Point pt(float x, float y, float z)
{
	Point p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}
Point sum(Point a, Point b)
{
	return (Point){ a.x + b.x, a.y + b.y, a.z + b.z };
}
Point mult(Point a, float b)
{
	return (Point){ a.x * b, a.y * b, a.z * b };
}
int matB[4][4] = {{-1,3,-3,1},{3,-6,3,0},{-3,3,0,0},{1,0,0,0}};

Point Bezier3D(float t, float s){
	float matT[1][4] = {t*t*t,t*t,t,1};
	float matS[4][1] = {s*s*s,s*s,s,1};

	// mult matT * matB => res is a mat of floats
	float matTB[1][4] = {0,0,0,0};
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			matTB[0][i] += matT[0][j] * matB[j][i];
		}
	}

	// mult matTB * points => res is matP, a mat of points
	Point matP[1][4] = {pt(0,0,0),pt(0,0,0),pt(0,0,0),pt(0,0,0)};
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			matP[0][i] = sum(matP[0][i], mult(grille[j][i], matTB[0][j]));
		}
	}

	// mult matP by matB^T => res is matP2, a mat of points (where matB^T is equal to matB)
	Point matP2[1][4] = {pt(0,0,0),pt(0,0,0),pt(0,0,0),pt(0,0,0)};
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			matP2[0][i] = sum(matP2[0][i], mult(matP[0][j], matB[i][j]));
		}
	}
	// mult matP2 by matS => res is the point of Bezier
	Point res = {0,0,0};
	for(int i=0;i<4;i++){
		res = sum(res,mult(matP2[0][i],matS[0][i]));
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
	// display control points
	chooseColor(1,1,1);
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++)
			drawQuad(grille[i][j], grille[i][j + 1], grille[i + 1][j + 1], grille[i + 1][j]);
	}
	chooseColor(1,0,0.7);
	// display Bezier surface
	for(float t=0.0;t<=1.0;t+=1./n){
		for(float s=0.0;s<=1.0;s+=1./n){
			Point p = Bezier3D(t,s);
			if(mode == POINTS)
				drawPoint(p);
			else{
				if(t!=1.)
					drawLine(p, Bezier3D(t+1./n,s));
				else
					drawLine(p, Bezier3D(t-1./n,s));
				if(s!=1.)
					drawLine(p, Bezier3D(t,s+1./n));
			}
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
	if (keycode=='z'){
		if(mode == POINTS){
			mode = QUADS;
			n=10;
		}
		else{
			mode = POINTS;
			n=100;
		}
	}
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

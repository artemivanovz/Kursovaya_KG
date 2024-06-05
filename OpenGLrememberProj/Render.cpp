#include "Render.h"
#include <future>
#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

#define PI 3.14159265
void bSort(double list[500][3], int listLength);
void CyrcleSort(double points[][3], double base_point[3], int n);


//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}
void ChangeTexture();
void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	if (key == 'X') {
		ChangeTexture();
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;


void ChangeTexture() {
	static int count = 0;
	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	if (count % 2 != 0) {
		OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	}
	else {
		OpenGL::LoadBMP("kirpich.bmp", &texW, &texH, &texarray);
	}
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);
	count++;

	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}


//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	double A[] = { 0,1,0 };
	double B[] = { 7,5,0 };
	double C[] = { 9,-1,0 };
	double D[] = { 3.512,-5.39,0 };
	double E[] = { 0,-1,0 };
	double F[] = { -3,-4,0 };
	double G[] = { -6,0,0 };
	double H[] = { -3,4,0 };

	double A1[] = { 0,1,2 };
	double B1[] = { 7,5,2 };
	double C1[] = { 9,-1,2 };
	double D1[] = { 3.512,-5.39,2 };
	double E1[] = { 0,-1,2 };
	double F1[] = { -3,-4,2 };
	double G1[] = { -6,0,2 };
	double H1[] = { -3,4,2 };

	float xcos, ysin, tx, ty, angle, radian, x, y, r = 2;



	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 29);
	for (angle = 0.0; angle < 360.0; angle += 2.0) {
		radian = angle * (PI / 180.0f);
		xcos = (float)cos(radian);
		ysin = (float)sin(radian);
		x = xcos * r + 12;
		y = ysin * r + 2;
		tx = xcos * 0.5 + 0.5;
		ty = ysin * 0.5 + 0.5;
		glTexCoord2f(tx, ty);
		glVertex3f(x, y, 2);
	}
	glEnd();


	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	//glColor3d(0.6, 0.3, 0.8);
	// ������ ���������
	glNormal3d(0, 0, -40);
	glColor3d(0.1, 0.3, 0.8);
	glTexCoord2f(0.4f, 0.53f);
	glVertex3dv(A);
	glTexCoord2f(0.86f, 0.8f);
	glVertex3dv(B);
	glTexCoord2f(1, 0.4f);
	glVertex3dv(C);
	glTexCoord2f(0.63f, 0.04f);
	//glTexCoord2f(0.6f, 0.06f);
	glVertex3dv(D);
	glTexCoord2f(0.4f, 0.4f);
	glVertex3dv(E);
	glTexCoord2f(0.2f, 0.2f);
	glVertex3dv(F);
	glTexCoord2f(0, 0.46f);
	glVertex3dv(G);
	glTexCoord2f(0.2f, 0.73f);
	glVertex3dv(H);
	glEnd();

	glColor3d(0.1, 0.3, 0.8);
	//������
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3d(0.5, 0.5, 0.5);
	glNormal3d(-8, 14, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 0);
	glVertex3dv(A);


	glColor3d(0, 1, 0);
	glNormal3d(12, 4, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(1, 0);
	glVertex3dv(B);

	glColor3d(0.6, 0.3, 0.8);
	glNormal3d(8.78, -11.696, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(1, 0);
	glVertex3dv(C);

	glColor3d(0.5, 0.5, 0.5);
	glNormal3d(-8.78, -6.304, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(1, 0);
	glVertex3dv(D);

	glNormal3d(6, -6, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(1, 0);
	glVertex3dv(E);

	glNormal3d(-8, -6, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 0);
	glVertex3dv(G);
	glTexCoord2d(1, 0);
	glVertex3dv(F);

	glNormal3d(-8, 6, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(1, 0);
	glVertex3dv(G);

	glNormal3d(6, 6, 0);
	glTexCoord2d(1, 1);
	glVertex3dv(H1);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 0);
	glVertex3dv(H);

	glEnd();

	//������� ���������
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 40);
	glColor4f(1, 0, 0, 0.3);
	glTexCoord2f(0.4f, 0.53f);
	glVertex3dv(A1);
	glTexCoord2f(0.86f, 0.8f);
	glVertex3dv(B1);
	glTexCoord2f(1, 0.4f);
	glVertex3dv(C1);
	glTexCoord2f(0.63f, 0.04f);
	//glTexCoord2f(0.6f, 0.06f);
	glVertex3dv(D1);
	glTexCoord2f(0.4f, 0.4f);
	glVertex3dv(E1);
	glTexCoord2f(0.2f, 0.2f);
	glVertex3dv(F1);
	glTexCoord2f(0, 0.46f);
	glVertex3dv(G1);
	glTexCoord2f(0.2f, 0.73f);
	glVertex3dv(H1);
	glEnd();



	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "X - ����������� ��������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}



double angle(double base[3], double point[3]) {
	return atan2(point[1] - base[1], point[0] - base[0]);
}

void CyrcleSort(double points[][3], double base_point[3], int n) {
	for (int i = 1; i < n; ++i) {
		int j = i;
		while (j > 0 && angle(base_point, points[j - 1]) > angle(base_point, points[j])) {
			std::swap(points[j], points[j - 1]);
			--j;
		}
	}
}
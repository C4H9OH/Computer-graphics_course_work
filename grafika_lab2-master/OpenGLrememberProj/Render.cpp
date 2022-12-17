#include "Render.h"

#include<math.h>

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

#include <cstdlib>
#include <ctime>

#include <list>

using namespace std;

list<double> vectProz(double A[], double B[], double C[], int k);

void batman(double centre[3], double radius, int color);
void joker(double centre[3], double radius, int color);
bool textureMode = true;
bool lightMode = true;






double score = 0;

double A[3] = { -10, -10,0 };
double B[3] = { 10, -10,0 };
double C[3] = { 10, 10,0 };
double D[3] = { -10, 10,0 };

double radius = 1;
double centre[3] = { 0,0,radius};

//srand((unsigned int)time(NULL));
int a_ = rand() % 16 - 5;

//srand((unsigned int)time(NULL));
int b_ = rand() % 16 - 5;

//markerCentre[0] = a_;
//markerCentre[1] = b_;
//markerCentre[2] = (C[2] * (C[0] / a_ + C[1] / b_) / 2);


double markerCentre[3] = { a_, b_, (C[2] * (C[0] / a_ + C[1] / b_) / 2) };

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
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

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	/*if (key == 'L')
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
	}*/

	if (key == 'W')
	{
		if ((A[2] < 5) && (B[2] < 5))
		{
			A[2]++;
			B[2]++;

			A[1] = -sqrt(10 * 10 - A[2] * A[2]);
			B[1] = -sqrt(10 * 10 - B[2] * B[2]);



			C[2] --;
			D[2] --;

			C[1] = sqrt(10 * 10 - C[2] * C[2]);
			D[1] = sqrt(10 * 10 - D[2] * D[2]);
		}
	}

	if (key == 'S')
	{
		if ((C[2] < 5) && (D[2] < 5))
		{
			A[2]--;
			B[2]--;

			A[1] = -sqrt(10 * 10 - A[2] * A[2]);
			B[1] = -sqrt(10 * 10 - B[2] * B[2]);

			C[2]++;
			D[2]++;

			C[1] = sqrt(10 * 10 - C[2] * C[2]);
			D[1] = sqrt(10 * 10 - D[2] * D[2]);

		}
	}


	if (key == 'D')
	{
		if ((A[2] < 5) && (D[2] < 5))
		{
			A[2]++;
			D[2]++;

			A[0] = -sqrt(10 * 10 - A[2] * A[2]);
			D[0] = -sqrt(10 * 10 - D[2] * D[2]);

			B[2]--;
			C[2]--;

			B[0] = sqrt(10 * 10 - B[2] * B[2]);
			C[0] = sqrt(10 * 10 - C[2] * C[2]);
		}
	}

	if (key == 'A')
	{
		if ((B[2] < 5) && (C[2] < 5))
		{
			A[2]--;
			D[2]--;

			A[0] = -sqrt(10 * 10 - A[2] * A[2]);
			D[0] = -sqrt(10 * 10 - D[2] * D[2]);

			B[2]++;
			C[2]++;

			B[0] = sqrt(10 * 10 - B[2] * B[2]);
			C[0] = sqrt(10 * 10 - C[2] * C[2]);
		}

	}

	if (key == 'H')
	{
		score = 0;

		A[2] = 0;
		B[2] = 0;
		C[2] = 0;
		D[2] = 0;


		//srand((unsigned int)time(NULL));
		int _a = rand() % 16 - 5;

		//srand((unsigned int)time(NULL));
		int _b = rand() % 16 - 5;

		//markerCentre[0] = a_;
		//markerCentre[1] = b_;
		//markerCentre[2] = (C[2] * (C[0] / a_ + C[1] / b_) / 2);

		centre[0] = 0;
		centre[1] = 0;
		centre[2] = radius;

		markerCentre[0] = _a;
		markerCentre[1] = _b;
		markerCentre[2] = (C[2] * (C[0] / a_ + C[1] / b_) / 2);
	}
	


	
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{

	list<double> N;
	light.pos = camera.pos;


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  


	//Начало рисования квадратика станкина
	

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);

	N = vectProz(A, B, D, 1);
	auto iter = N.begin();
	double a, b, c;
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);


	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0.859, 0);
	glVertex3dv(B);
	glTexCoord2d(0.859, 1);
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(D);

	glEnd();

	if ((centre[0] > C[0]) || (centre[1] > C[1]) || (centre[0] < A[0]) || (centre[1] < A[1]) || (centre[2] != radius))
	{
		centre[2]--;
	}

	centre[0] += ((A[2] + D[2]) / 2) *(0.1 + (score/4000));

	centre[1] += ((A[2] + B[2]) / 2) * (0.1 + (score / 4000));
	
	batman(centre, radius, 1);
	joker(markerCentre, radius, 0);

	if ((centre[0] < markerCentre[0] + radius) && (centre[1] < markerCentre[1] + radius)&& (centre[0] > markerCentre[0] - radius) && (centre[1] > markerCentre[1] - radius))
	{
		score += 100;
		//srand((unsigned int)time(NULL));
		int a = rand() % 16 - 5;

		//srand((unsigned int)time(NULL));
		int b = rand() % 16 - 5;

		markerCentre[0] = a;
		markerCentre[1] = b;
		//markerCentre[2] = (C[2] * (C[0] / a + C[1] / b) / 2);

		
	}

	if (centre[2] < -10)
	{
		score = 0;

		A[2] = 0;
		B[2] = 0;
		C[2] = 0;
		D[2] = 0;


		//srand((unsigned int)time(NULL));
		int __a = rand() % 16 - 5;

		//srand((unsigned int)time(NULL));
		int __b = rand() % 16 - 5;

		//markerCentre[0] = a_;
		//markerCentre[1] = b_;
		//markerCentre[2] = (C[2] * (C[0] / a_ + C[1] / b_) / 2);

		centre[0] = 0;
		centre[1] = 0;
		centre[2] = radius;

		markerCentre[0] = __a;
		markerCentre[1] = __b;
		markerCentre[2] = (C[2] * (C[0] / a_ + C[1] / b_) / 2);
	}
	

	//конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
//	ss << "T - вкл/выкл текстур" << std::endl;
	//ss << "L - вкл/выкл освещение" << std::endl;
	//ss << "F - Свет из камеры" << std::endl;
	//ss << "G - двигать свет по горизонтали" << std::endl;
	//ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	//ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	//ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	//ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	ss << "Очки:" << score << std::endl;

	ss << "Игрок" << centre[0] << " " << centre[1] << std::endl;

	ss << "Маркер" << markerCentre[0] <<" " << markerCentre[1] << std::endl;

	ss << "Скорость" << (0.1 + (score / 4000)) << std::endl;

	ss << "Н - начать игру сначала" << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void batman(double centre[3], double radius, int color)
{
	glBegin(GL_QUADS);

	if (color == 0)
	{
		glColor3f(0.0f, 0.5f, 1.0f);
	}

	if (color == 1)
	{
		glColor3f(0.5f, 0.8f, 1.0f);
	}
	
	double cap_a[3] = { (centre[0]), (centre[1] - radius), (centre[2] - radius) };
	double cap_b[3] = { (centre[0] + radius), (centre[1] - radius/1.4), (centre[2] - radius) };
	double cap_c[3] = { (centre[0] + radius*0.5), (centre[1]), (centre[2] - radius) };
	double cap_d[3] = { (centre[0] + radius*0.25), (centre[1] + radius), (centre[2] - radius) };
	double cap_e[3] = { (centre[0] - radius*0.25), (centre[1] + radius), (centre[2] - radius) };
	double cap_f[3] = { (centre[0] - radius*0.5), (centre[1]), (centre[2] - radius) };
	double cap_g[3] = { (centre[0] - radius), (centre[1] - radius / 1.4), (centre[2] - radius) };


	double cap_a1[3] = { (centre[0]), (centre[1] - radius), (centre[2] + radius) };
	double cap_b1[3] = { (centre[0] + radius), (centre[1] - radius / 1.4), (centre[2] + radius) };
	double cap_c1[3] = { (centre[0] + radius * 0.5), (centre[1]), (centre[2] + radius) };
	double cap_d1[3] = { (centre[0] + radius * 0.25), (centre[1] + radius), (centre[2] + radius) };
	double cap_e1[3] = { (centre[0] - radius * 0.25), (centre[1] + radius), (centre[2] + radius) };
	double cap_f1[3] = { (centre[0] - radius * 0.5), (centre[1]), (centre[2] + radius) };
	double cap_g1[3] = { (centre[0] - radius), (centre[1] - radius / 1.4), (centre[2] + radius) };


	double point_a[3] = { (centre[0] - radius), (centre[1] + radius), (centre[2] - radius) };
	double point_b[3] = { (centre[0] + radius), (centre[1] + radius), (centre[2] - radius) };
	double point_c[3] = { (centre[0] + radius), (centre[1] - radius), (centre[2] - radius) };
	double point_d[3] = { (centre[0] - radius), (centre[1] - radius), (centre[2] - radius) };

	double point_a1[3] = { (centre[0] - radius), (centre[1] + radius), (centre[2] + radius) };
	double point_b1[3] = { (centre[0] + radius), (centre[1] + radius), (centre[2] + radius) };
	double point_c1[3] = { (centre[0] + radius), (centre[1] - radius), (centre[2] + radius) };
	double point_d1[3] = { (centre[0] - radius), (centre[1] - radius), (centre[2] + radius) };

	list<double> N;

	N = vectProz(point_a, point_b, point_d, -1);
	auto iter = N.begin();
	double a, b, c;
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_a); glVertex3dv(point_b); glVertex3dv(point_c); glVertex3dv(point_d);

	N = vectProz(point_a1, point_b1, point_d1, -1);
	iter = N.begin();
	
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glTexCoord2d(0.86, 1);

//	//glTexCoord2d(1, 1);

	glVertex3dv(point_a1);

	glTexCoord2d(1, 1);

	glVertex3dv(point_b1);

////	glTexCoord2d(1, 1);
	glTexCoord2d(1, 0.84);


	glVertex3dv(point_c1);

	glTexCoord2d(0.86, 0.84);


//
	glVertex3dv(point_d1);

	/*glVertex3dv(cap_a1); glVertex3dv(cap_b1); glVertex3dv(cap_c1); glVertex3dv(cap_d1);
	glVertex3dv(cap_e1); glVertex3dv(cap_f1); glVertex3dv(cap_g1);*/

	

	N = vectProz(point_a, point_a1, point_b1, 1);
	iter = N.begin();
	
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_a); glVertex3dv(point_a1); glVertex3dv(point_b1); glVertex3dv(point_b);

	

	N = vectProz(point_b, point_b1, point_c1, 1);
	iter = N.begin();
	
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_b); glVertex3dv(point_b1); glVertex3dv(point_c1); glVertex3dv(point_c);

	N = vectProz(point_c, point_c1, point_d1, 1);
	iter = N.begin();
	
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_c); glVertex3dv(point_c1); glVertex3dv(point_d1); glVertex3dv(point_d);

	N = vectProz(point_d, point_d1, point_a1, 1);
	 iter = N.begin();
	
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_d); glVertex3dv(point_d1); glVertex3dv(point_a1); glVertex3dv(point_a);

	glEnd();
}


void joker(double centre[3], double radius, int color)
{
	glBegin(GL_QUADS);

	if (color == 0)
	{
		glColor3f(0.0f, 0.5f, 1.0f);
	}

	if (color == 1)
	{
		glColor3f(0.5f, 0.8f, 1.0f);
	}

	double point_a[3] = { (centre[0] - radius), (centre[1] + radius), (centre[2] - radius) };
	double point_b[3] = { (centre[0] + radius), (centre[1] + radius), (centre[2] - radius) };
	double point_c[3] = { (centre[0] + radius), (centre[1] - radius), (centre[2] - radius) };
	double point_d[3] = { (centre[0] - radius), (centre[1] - radius), (centre[2] - radius) };

	double point_a1[3] = { (centre[0] - radius), (centre[1] + radius), (centre[2] + radius) };
	double point_b1[3] = { (centre[0] + radius), (centre[1] + radius), (centre[2] + radius) };
	double point_c1[3] = { (centre[0] + radius), (centre[1] - radius), (centre[2] + radius) };
	double point_d1[3] = { (centre[0] - radius), (centre[1] - radius), (centre[2] + radius) };

	list<double> N;

	N = vectProz(point_a, point_b, point_d, -1);
	auto iter = N.begin();
	double a, b, c;
	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_a); glVertex3dv(point_b); glVertex3dv(point_c); glVertex3dv(point_d);

	N = vectProz(point_a1, point_b1, point_d1, -1);
	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glTexCoord2d(0.86, 0.84);
	//glTexCoord2d(1, 1);
	glVertex3dv(point_a1);
	glTexCoord2d(1, 0.84);
	glVertex3dv(point_b1);
	//	glTexCoord2d(1, 1);
	glTexCoord2d(1, 0.68);
	glVertex3dv(point_c1);
	glTexCoord2d(0.86, 0.68);
	glVertex3dv(point_d1);

	N = vectProz(point_a, point_a1, point_b1, 1);
	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_a); glVertex3dv(point_a1); glVertex3dv(point_b1); glVertex3dv(point_b);

	N = vectProz(point_b, point_b1, point_c1, 1);
	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_b); glVertex3dv(point_b1); glVertex3dv(point_c1); glVertex3dv(point_c);

	N = vectProz(point_c, point_c1, point_d1, 1);
	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_c); glVertex3dv(point_c1); glVertex3dv(point_d1); glVertex3dv(point_d);

	N = vectProz(point_d, point_d1, point_a1, 1);
	iter = N.begin();

	a = *iter;
	b = *++iter;
	c = *++iter;
	glNormal3d(a, b, c);

	glVertex3dv(point_d); glVertex3dv(point_d1); glVertex3dv(point_a1); glVertex3dv(point_a);

	glEnd();
}


list<double> vectProz(double A[], double B[], double C[], int k)
{
	double A1[] = { B[0] - A[0], B[1] - A[1], B[2] - A[2] };
	double B1[] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };

	list <double> list_ = { k * (A1[1] * B1[2] - A1[2] * B1[1]), k * (A1[2] * B1[0] - A1[0] * B1[2]), k * (A1[0] * B1[1] - A1[1] * B1[0]) };
	return list_;
}
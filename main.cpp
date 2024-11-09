#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <vector>

struct Point
{
  float x, y;
};

std::vector<Point> points;
int curveAlgorithm = 0;
int selectedPointIndex = 2;
bool addPointsEnabled = false;
bool polilineaEnabled = false;
bool controlPointsEnabled = false;

float factorial(int n)
{
  unsigned long long fact = 1;

  for (int i = 1; i <= n; ++i)
  {
    fact *= i;
  }
  return fact;
}

/*
// Función para calcular la tangente entre dos puntos
Point calculateTangent(Point p0, Point p1)
{
  return {p1.x - p0.x, p1.y - p0.y}; // Tangente como la diferencia de los puntos consecutivos
}

// Función de Hermite para un par de puntos con sus tangentes
std::pair<float, float> hermite(float t, Point p0, Point t0, Point p1, Point t1)
{
  float h1 = 2 * t * t * t - 3 * t * t + 1;
  float h2 = -2 * t * t * t + 3 * t * t;
  float h3 = t * t * t - 2 * t * t + t;
  float h4 = t * t * t - t * t;

  float x = h1 * p0.x + h2 * p1.x + h3 * t0.x + h4 * t1.x;
  float y = h1 * p0.y + h2 * p1.y + h3 * t0.y + h4 * t1.y;

  return {x, y};
}

// Función para dibujar la curva de Hermite entre los puntos de control
void drawHermiteCurve()
{
  glBegin(GL_LINE_STRIP);

  // Para cada par de puntos consecutivos, calcula la curva de Hermite
  for (int i = 0; i < points.size() - 1; ++i)
  {
    Point p0 = points[i];
    Point p1 = points[i + 1];

    // Calcula las tangentes entre los puntos consecutivos
    Point t0 = (i == 0) ? calculateTangent(p0, points[i + 2]) : calculateTangent(points[i - 1], p0);
    Point t1 = (i + 2 < points.size()) ? calculateTangent(p1, points[i + 2]) : calculateTangent(p0, p1);

    // Dibuja la curva Hermite para el segmento
    for (float t = 0.0f; t <= 1.0f; t += 0.01f)
    {
      auto [x, y] = hermite(t, p0, t0, p1, t1);
      glVertex2f(x, y);
    }
  }

  glEnd();
}
*/
// Función de interpolación de Bézier cúbica
std::pair<float, float> bezierPoint(float t)
{

  float x = 0.0f, y = 0.0f;
  for (int i = 0; i < points.size(); i++)
  {
    x += (factorial(points.size() - 1) /
          (factorial(i) * factorial(points.size() - 1 - i))) *
         pow(1 - t, points.size() - 1 - i) * pow(t, i) * points[i].x;
    y += (factorial(points.size() - 1) /
          (factorial(i) * factorial(points.size() - 1 - i))) *
         pow(1 - t, points.size() - 1 - i) * pow(t, i) * points[i].y;
  }

  return {x, y};
}

void drawBezierCurve2D()
{
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(5.0f);
  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= 1.0; t += 0.01)
  {
    std::pair<float, float> result = bezierPoint(t);
    float x = result.first;
    float y = result.second;
    glVertex2f(x, y);
  }
  glEnd();
}

void drawPoints2D()
{
  glPointSize(4.0f);
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_POINTS);
  for (const auto &point : points)
  {
    glVertex2f(point.x, point.y);
  }
  glEnd();
}

void drawBezierCurve3D()
{
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(5.0f);

  glBegin(GL_QUAD_STRIP);
  for (float t = 0; t <= 1.0; t += 0.01)
  {
    std::pair<float, float> point = bezierPoint(t);
    glVertex3f(point.first, 20.0f, point.second);
    glVertex3f(point.first, 0.0f, point.second);
  }
  glEnd();

  /*  glBegin(GL_LINE_STRIP);
   for (float t = 0; t <= 1.0; t += 0.01)
   {
     auto [x, y] = bezierPoint(t);
     glVertex3f(x, 10.0f, y);
     glVertex3f(x, 0.0f, y);
     glVertex3f(x, 10.0f, y);
   }
   glEnd();
   */
  glBegin(GL_QUAD_STRIP);
  for (float t = 0; t <= 1.0; t += 0.01)
  {
    std::pair<float, float> result = bezierPoint(t);
    float x = result.first;
    float y = result.second;
    glVertex3f(x, 10.0f, y);
    glVertex3f(x, 0.0f, y);
  }
  glEnd();

  // glutSwapBuffers();
}

void drawPoints3D()
{
  glPointSize(5.0f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_POINTS);
  for (std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
  {
    glVertex2f(it->x, it->y);
  }
  glEnd();
}

void drawPolilineas()
{
  glColor3f(0.0f, 1.0f, 1.0f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_STRIP);
  for (std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
  {
    glVertex2f(it->x, it->y);
  }
  glEnd();
}

void printPoints()
{
  for (std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
  {
    std::cout << it->x << it->y << "\n";
  }
}

void drawEjes()
{
  glLineWidth(1.0f);
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(-100.0f, 0.0f, 0.0f);
  glVertex3f(100.0f, 0.0f, 0.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, -100.f);
  glVertex3f(0.0f, 0.0f, 100.0f);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0.0f, -100.0f, 0.0f);
  glVertex3f(0.0f, 100.0f, 0.0f);
  glEnd();
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /*if (curveAlgorithm == 0)
  {
    drawHermiteCurve();
  }
  else if (curveAlgorithm == 1)
  {
    drawBezierCurve2D();
  }*/
  drawBezierCurve2D();

  if (controlPointsEnabled)
    drawPoints2D();
  if (polilineaEnabled)
    drawPolilineas();
  // Vista derecha (3D)
  glViewport(400, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 1.0f, 1.0f, 400.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(30.0f, 50.0f, 200.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  drawEjes();
  drawPoints3D();
  drawBezierCurve3D();
  //  printPoints();
  glutSwapBuffers();
}

// Función para detectar si se hizo clic en un punto
int getPointIndexAt(float x, float y)
{
  for (int i = 0; i < points.size(); ++i)
  {
    if ((x - points[i].x) * (x - points[i].x) +
            (y - points[i].y) * (y - points[i].y) <
        100.0f)
    {
      return i;
    }
  }
  return -1;
}

// Manejador de eventos del mouse
void mouseHandler(int button, int state, int x, int y)
{

  // std::cout << "x: " << x << " y: " << y << "\n";
  float glX = (float)(x - (400.0f / 2));
  float glY = -(float)(y - (400.0f / 2));
  // std::cout << "En pantalla: " << glX << " " << glY << "\n";
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    int index = getPointIndexAt(glX, glY);
    if (index != -1)
    {
      selectedPointIndex = index;
      // std::cout << "Seleccionado: " << selectedPointIndex << "\n";
    }
    else if (addPointsEnabled)
    {
      points.push_back({glX, glY});
    }
  }
  else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
  {
    selectedPointIndex = -1;
  }
  glutPostRedisplay();
}

// Manejador para arrastrar el punto
void motionHandler(int x, int y)
{
  if (selectedPointIndex != -1)
  {
    points[selectedPointIndex].x = (float)(x - (400.0f / 2));
    points[selectedPointIndex].y = -(float)(y - (400.0f / 2));
    glutPostRedisplay();
  }
}

void menuAddPointHandler(int option)
{
  switch (option)
  {
  case 1:
    addPointsEnabled = true;
    break;
  case 2:
    addPointsEnabled = false;
    break;
  }
  glutPostRedisplay();
}

// Función de menú para activar o desactivar agregar puntos
void menuPrincipalHandler(int option)
{
  switch (option)
  {
  default:
    break;
  }
  glutPostRedisplay();
}

// Configuración del menú contextual
void createContextMenu()
{
  int menuAgregarPuntos = glutCreateMenu(menuAddPointHandler);
  glutAddMenuEntry("Activado", 1);
  glutAddMenuEntry("Desactivar", 2);

  int menu = glutCreateMenu(menuPrincipalHandler);
  glutAddSubMenu("Agregar Puntos", menuAgregarPuntos);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Configuración inicial de OpenGL
void initGL()
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
}

// Función principal
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 400);
  glutCreateWindow("Arrastrar y soltar puntos con GLUT");

  points.push_back({-80.0f, -80.0f});
  points.push_back({-40.0f, 80.0f});
  points.push_back({40.0f, -80.0f});
  points.push_back({80.0f, 80.0f});

  initGL();
  glutDisplayFunc(display);
  glutMouseFunc(mouseHandler);
  glutMotionFunc(motionHandler);

  createContextMenu();
  glutMainLoop();
  return 0;
}

#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <vector>

struct Point {
  float x, y;
};

std::vector<Point> points;
int selectedPointIndex = 2;

float factorial(int n) {
  unsigned long long fact = 1;

  for (int i = 1; i <= n; ++i) {
    fact *= i;
  }
  return fact;
}

// Función de interpolación de Bézier cúbica
std::pair<float, float> bezierPoint(float t) {

  float x = 0.0f, y = 0.0f;
  for (int i = 0; i < points.size(); i++) {
    x += (factorial(points.size() - 1) /
          (factorial(i) * factorial(points.size() - 1 - i))) *
         pow(1 - t, points.size() - 1 - i) * pow(t, i) * points[i].x;
    y += (factorial(points.size() - 1) /
          (factorial(i) * factorial(points.size() - 1 - i))) *
         pow(1 - t, points.size() - 1 - i) * pow(t, i) * points[i].y;
  }

  return {x, y};
}

void drawBezierCurve2D() {

  // Dibujar la curva de Bézier
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(5.0f);
  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= 1.0; t += 0.01) {
    auto [x, y] = bezierPoint(t);
    glVertex2f(x, y);
  }
  glEnd();

  // glutSwapBuffers();
}

void drawPoints2D() {
  glPointSize(10.0f);
  glBegin(GL_POINTS);
  for (const auto &point : points) {
    glVertex2f(point.x, point.y);
  }
  glEnd();

  // glutSwapBuffers();
}

// Función para dibujar la curva de Bézier
void drawBezierCurve() {

  // Dibujar la curva de Bézier
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(5.0f);
  glBegin(GL_LINE_STRIP);
  for (float t = 0; t <= 1.0; t += 0.01) {
    auto [x, y] = bezierPoint(t);
    glVertex3f(x, 10.0f, y);
    glVertex3f(x, 0.0f, y);
    glVertex3f(x, 10.0f, y);
  }
  glEnd();

  // glutSwapBuffers();
}
// Función para dibujar los puntos
void drawPoints() {
  glPointSize(10.0f);
  glBegin(GL_POINTS);
  for (const auto &point : points) {
    glVertex3f(point.x, 10.0f, point.y);
  }
  glEnd();

  // glutSwapBuffers();
}

void printPoints() {
  for (const auto &point : points) {
    std::cout << point.x << point.y << "\n";
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  drawBezierCurve2D();
  drawPoints2D();
  // Vista derecha (3D)
  glViewport(400, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 1.0f, 1.0f, 200.0f);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  gluLookAt(30.0f, 50.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 0.0f);
  glVertex3f(-100.0f, 0.0f, 0.0f);
  glVertex3f(100.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, -100.f);
  glVertex3f(0.0f, 0.0f, 100.0f);
  glVertex3f(0.0f, -100.0f, 0.0f);
  glVertex3f(0.0f, 100.0f, 0.0f);
  glEnd();

  drawPoints();

  drawBezierCurve();
  //  printPoints();

  glutSwapBuffers();
}

// Función para detectar si se hizo clic en un punto
int getPointIndexAt(float x, float y) {
  for (int i = 0; i < points.size(); ++i) {
    if ((x - points[i].x) * (x - points[i].x) +
            (y - points[i].y) * (y - points[i].y) <
        100.0f) {
      return i;
    }
  }
  return -1;
}

// Manejador de eventos del mouse
void mouseHandler(int button, int state, int x, int y) {

  std::cout << "x: " << x << " y: " << y << "\n";

  float glX = (float)(x - (400.0f / 2));
  float glY = -(float)(y - (400.0f / 2));
  std::cout << "En pantalla: " << glX << " " << glY << "\n";
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    int index = getPointIndexAt(glX, glY);
    if (index != -1) {
      selectedPointIndex = index;
      std::cout << "Seleccionado: " << selectedPointIndex << "\n";

    } else {
      points.push_back({glX, glY});
    }
  } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    selectedPointIndex = -1;
  }
  glutPostRedisplay();
}

// Manejador para arrastrar el punto
void motionHandler(int x, int y) {
  if (selectedPointIndex != -1) {
    points[selectedPointIndex].x = (float)(x - (400.0f / 2));
    points[selectedPointIndex].y = -(float)(y - (400.0f / 2));
    glutPostRedisplay();
  }
}

// Configuración inicial de OpenGL
void initGL() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  //  glMatrixMode(GL_PROJECTION);
  // glOrtho(-800.0, 800.0, -600.0, 600.0, -600.0, 600.0);
  // glMatrixMode(GL_MODELVIEW);
}

// Función principal
int main(int argc, char **argv) {
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

  glutMainLoop();
  return 0;
}

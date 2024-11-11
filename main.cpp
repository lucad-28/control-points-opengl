#include <GL/glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

struct Point {
  float x, y;
};

std::vector<Point> points;
float ang[] = {0.0f, 0.0f, 0.0f};
float ang3[] = {0.0f, 0.0f, 0.0f};
int eje = 1;

int curveAlgorithm = 0;
int selectedPointIndex = 2;
bool addPointsEnabled = false;
bool dragPointEnabled = false;
bool showPolilinea = false;
bool showControlPoints = false;

const int W_WIDTH = 400;
const int W_HEIGHT = 400;
const float X_MAX = 200.0f;
const float Y_MAX = 200.0f;

/*------------- Código para el manejo de sliders ---------------------- */
/* --------- Modificado a partir de lo dado por ChatGPT -------------- */
void printText(float, float, void *, const char *);
void drawPoint(int, int, float, float, float);

struct Slider {
  float x;
  float y;
  float c_height;
  float c_width;
  float b_height;
  float b_width;
  int value;
  int max;
  int min;
  bool dragging;
};

Slider angE2;
Slider angE3;

void initSlider();
void drawSlider(Slider, const char *);
std::tuple<bool, int> mouseSlider(Slider, float, float);
int motionSlider(Slider, float);
/* ------------------------------------------------------------------- */

float factorial(int n) {
  unsigned long long fact = 1;

  for (int i = 1; i <= n; ++i) {
    fact *= i;
  }
  return fact;
}

// Matriz de Catmull-Rom
const float catmullRomMatrix[4][4] = {{-0.5, 1.5, -1.5, 0.5},
                                      {1.0, -2.5, 2.0, -0.5},
                                      {-0.5, 0.0, 0.5, 0.0},
                                      {0.0, 1.0, 0.0, 0.0}};

Point calculateCatmullRomPoint(float t, Point p0, Point p1, Point p2,
                               Point p3) {
  float tVec[4] = {t * t * t, t * t, t, 1};
  float x = 0.0f, y = 0.0f, z = 0.0f;

  for (int row = 0; row < 4; ++row) {
    float coeff = tVec[row];
    x += coeff *
         (catmullRomMatrix[row][0] * p0.x + catmullRomMatrix[row][1] * p1.x +
          catmullRomMatrix[row][2] * p2.x + catmullRomMatrix[row][3] * p3.x);
    y += coeff *
         (catmullRomMatrix[row][0] * p0.y + catmullRomMatrix[row][1] * p1.y +
          catmullRomMatrix[row][2] * p2.y + catmullRomMatrix[row][3] * p3.y);
  }

  return {x, y};
}

void drawCamul2D() {
  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(5.0f);
  glBegin(GL_LINE_STRIP);
  for (size_t i = 1; i < points.size() - 2; ++i) {
    Point p0 = points[i - 1];
    Point p1 = points[i];
    Point p2 = points[i + 1];
    Point p3 = points[i + 2];

    // Genera puntos en la curva entre p1 y p2
    for (float t = 0; t <= 1.0f; t += 0.05f) {
      Point pt = calculateCatmullRomPoint(t, p0, p1, p2, p3);
      glVertex2f(pt.x, pt.y);
      glVertex2f(pt.x, pt.y);
    }
  }
  glEnd();
}

// Dibuja la curva usando la interpolación Catmull-Rom
void drawCatmullRomSpline() {
  glColor3f(1.0, 0.0, 0.0); // Color de la curva
  glLineWidth(2.0);
  glBegin(GL_QUAD_STRIP);

  for (size_t i = 1; i < points.size() - 2; ++i) {
    Point p0 = points[i - 1];
    Point p1 = points[i];
    Point p2 = points[i + 1];
    Point p3 = points[i + 2];

    // Genera puntos en la curva entre p1 y p2
    for (float t = 0; t <= 1.0f; t += 0.05f) {
      Point pt = calculateCatmullRomPoint(t, p0, p1, p2, p3);
      glVertex3f(pt.x, 20.0f, pt.y);
      glVertex3f(pt.x, 0.0f, pt.y);
    }
  }

  glEnd();
}

// Dibuja la curva (sólo para visualización)
void drawCurveSegmentada() {
  glColor3ub(1.0, 0.0, 1.0);
  glLineWidth(2.0f);
  glBegin(GL_QUAD_STRIP);
  for (size_t i = 1; i < points.size() - 2; ++i) {
    Point p0 = points[i - 1];
    Point p1 = points[i];
    Point p2 = points[i + 1];
    Point p3 = points[i + 2];

    // Genera puntos en la curva entre p1 y p2
    for (float t = 0; t <= 1.0f; t += 0.2f) {
      Point pt = calculateCatmullRomPoint(t, p0, p1, p2, p3);
      glVertex3f(pt.x, 20.0f, pt.y);
      glVertex3f(pt.x, 0.0f, pt.y);
    }
  }
  glEnd();
}

void drawPoints2D() {
  glPointSize(5.0f);
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_POINTS);
  for (const auto &point : points) {
    glVertex2f(point.x, point.y);
  }
  glEnd();
}

void drawPoints3D() {
  glPointSize(5.0f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_POINTS);
  for (std::vector<Point>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    glVertex2f(it->x, it->y);
  }
  glEnd();
}

void drawPolilineas() {
  glColor3f(0.0f, 1.0f, 1.0f);
  glLineWidth(3.0f);
  glBegin(GL_LINE_STRIP);
  for (std::vector<Point>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    glVertex2f(it->x, it->y);
  }
  glEnd();
}

void printPoints() {
  for (std::vector<Point>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    std::cout << it->x << it->y << "\n";
  }
}

void drawEjes() {
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

const float baseSize = 200.0f; // Tamaño de la base del cuadrado
const float altura = 5.0f;     // Altura del cuadrado en 3D

void drawPointsPlataforma() {
  // Cara superior
  glVertex3f(-baseSize / 2, altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, baseSize / 2);
  glVertex3f(-baseSize / 2, altura / 2, baseSize / 2);

  // Cara inferior
  glVertex3f(-baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, -altura / 2, baseSize / 2);
  glVertex3f(-baseSize / 2, -altura / 2, baseSize / 2);

  // Cara frontal
  glVertex3f(-baseSize / 2, -altura / 2, baseSize / 2);
  glVertex3f(baseSize / 2, -altura / 2, baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, baseSize / 2);
  glVertex3f(-baseSize / 2, altura / 2, baseSize / 2);

  // Cara trasera
  glVertex3f(-baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, -baseSize / 2);
  glVertex3f(-baseSize / 2, altura / 2, -baseSize / 2);

  // Cara izquierda
  glVertex3f(-baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(-baseSize / 2, -altura / 2, baseSize / 2);
  glVertex3f(-baseSize / 2, altura / 2, baseSize / 2);
  glVertex3f(-baseSize / 2, altura / 2, -baseSize / 2);

  // Cara derecha
  glVertex3f(baseSize / 2, -altura / 2, -baseSize / 2);
  glVertex3f(baseSize / 2, -altura / 2, baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, baseSize / 2);
  glVertex3f(baseSize / 2, altura / 2, -baseSize / 2);
}

void drawPlataforma() {
  glPushMatrix();
  glTranslatef(0.0f, -altura, -0.0f);
  glBegin(GL_QUADS);
  glColor3f(0.902f, 0.0f, 1.0f);
  drawPointsPlataforma();
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 0.0f);
  drawPointsPlataforma();
  glEnd();

  glPopMatrix();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  drawCamul2D();
  if (showControlPoints)
    drawPoints2D();
  if (showPolilinea)
    drawPolilineas();
  // Vista CENTRO (3D)
  drawSlider(angE2, "Aumento angE2:");
  drawSlider(angE3, "Aumento angE3:");
  glViewport(400, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 1.0f, 1.0f, 800.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(50.0f, 100.0f, 300.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  drawEjes();
  drawPlataforma();
  drawPoints3D();

  glPushMatrix();

  glRotatef(ang[0], 1.0f, 0.0f, 0.0f);
  glRotatef(ang[1], 0.0f, 1.0f, 0.0f);
  glRotatef(ang[2], 1.0f, 0.0f, 1.0f);
  drawCurveSegmentada();

  glPopMatrix();
  //  printPoints();

  // Vista DERECHA (3D)
  glViewport(800, 0, 400, 400);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 1.0f, 1.0f, 800.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(50.0f, 100.0f, 300.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  drawEjes();
  drawPlataforma();
  drawPoints3D();

  glPushMatrix();

  glRotatef(ang3[0], 1.0f, 0.0f, 0.0f);
  glRotatef(ang3[1], 0.0f, 1.0f, 0.0f);
  glRotatef(ang3[2], 1.0f, 0.0f, 1.0f);

  drawCatmullRomSpline();
  glPopMatrix();

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
  // std::cout << "x: " << x << " y: " << y << "\n";

  float posx = -1 * X_MAX + (2 * X_MAX) * (x) / W_WIDTH;
  float posy = Y_MAX - (2 * Y_MAX) * y / (W_HEIGHT);
  // std::cout << "En pantalla: " << glX << " " << glY << "\n";
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    int index = getPointIndexAt(posx, posy);
    if (index != -1) {
      selectedPointIndex = index;
      // std::cout << "Seleccionado: " << selectedPointIndex << "\n";
    } else if (addPointsEnabled) {
      points.push_back({posx, posy});
    }
    std::cout << "posx: " << posx << " posy: " << posy << "\n";
    std::tie(angE2.dragging, angE2.value) = mouseSlider(angE2, posx, posy);
    std::cout << "angE2.dragging=" << angE2.dragging << "\n";
    std::tie(angE3.dragging, angE3.value) = mouseSlider(angE3, posx, posy);
    glutPostRedisplay();
  } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    if (dragPointEnabled)
      selectedPointIndex = -1;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
      if (angE2.dragging) {
        angE2.dragging = false;
      } else if (angE3.dragging) {
        angE3.dragging = false;
      }
    }
    glutPostRedisplay();
  }
}

// Manejador para arrastrar el punto
void motionHandler(int x, int y) {
  if (selectedPointIndex != -1 && dragPointEnabled) {
    points[selectedPointIndex].x = (float)(x - (400.0f / 2));
    points[selectedPointIndex].y = -(float)(y - (400.0f / 2));
  }
  float posx = -1 * X_MAX + (2 * X_MAX) * x / W_WIDTH;
  if (angE2.dragging) {
    // Actualizar el valor del slider mientras se arrastra
    angE2.value = motionSlider(angE2, posx);
    std::cout << "angE2.value=" << angE2.value << "\n";
  }
  if (angE3.dragging) {
    // Actualizar el valor del slider mientras se arrastra
    angE3.value = motionSlider(angE3, posx);
    std::cout << "angE3.value=" << angE3.value << "\n";
  }
  glutPostRedisplay();
}

void menuDragPointHandler(int option) {
  switch (option) {
  case 1:
    dragPointEnabled = true;
    break;
  case 2:
    dragPointEnabled = false;
    break;
  }
  glutPostRedisplay();
}

void menuShowControlPoints(int option) {
  switch (option) {
  case 1:
    showControlPoints = true;
    break;
  case 2:
    showControlPoints = false;
    break;
  }
  glutPostRedisplay();
}

void menuShowPolilineas(int option) {
  switch (option) {
  case 1:
    showPolilinea = true;
    break;
  case 2:
    showPolilinea = false;
    break;
  }
  glutPostRedisplay();
}

void menuAddPointHandler(int option) {
  switch (option) {
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
void menuPrincipalHandler(int option) {
  switch (option) {
  default:
    break;
  }
  glutPostRedisplay();
}

// Configuración del menú contextual
void createContextMenu() {
  int menuAgregarPuntos = glutCreateMenu(menuAddPointHandler);
  glutAddMenuEntry("Activar", 1);
  glutAddMenuEntry("Desactivar", 2);

  int menuDragPoint = glutCreateMenu(menuDragPointHandler);
  glutAddMenuEntry("Activar", 1);
  glutAddMenuEntry("Desactivar", 2);

  int menuControlPoints = glutCreateMenu(menuShowControlPoints);
  glutAddMenuEntry("Activar", 1);
  glutAddMenuEntry("Desactivar", 2);

  int menuPolilineas = glutCreateMenu(menuShowPolilineas);
  glutAddMenuEntry("Activar", 1);
  glutAddMenuEntry("Desactivar", 2);

  int menu = glutCreateMenu(menuPrincipalHandler);
  glutAddSubMenu("Agregar Puntos", menuAgregarPuntos);
  glutAddSubMenu("Arrastrar Puntos", menuDragPoint);
  glutAddSubMenu("Mostrar Puntos de Control", menuControlPoints);
  glutAddSubMenu("Mostrar Polilineas", menuPolilineas);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

const std::string KEYS_AVAILABLE = "QWASZX";
void teclado(unsigned char key, int x, int y) {
  if (KEYS_AVAILABLE.find(key) != std::string::npos) {
    if (key == 'Q') {
      ang[0] += angE2.value;
      ang3[0] += angE3.value;
    } else if (key == 'W') {
      ang[0] -= angE2.value;
      ang3[0] -= angE3.value;
    } else if (key == 'A') {
      ang[1] += angE2.value;
      ang3[1] += angE3.value;
    } else if (key == 'S') {
      ang[1] -= angE2.value;
      ang3[1] -= angE3.value;
    } else if (key == 'Z') {
      ang[2] += angE2.value;
      ang3[2] += angE3.value;
    } else if (key == 'X') {
      ang[2] -= angE2.value;
      ang3[2] -= angE3.value;
    }
    glutPostRedisplay();
  }
}

// Configuración inicial de OpenGL
void initGL() {
  initSlider();
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glEnable(GL_DEPTH_TEST);
}

// Función principal
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1200, 400);
  glutCreateWindow("Arrastrar y soltar puntos con GLUT");

  points.push_back({-80.0f, -80.0f});
  points.push_back({-40.0f, 80.0f});
  points.push_back({40.0f, -80.0f});
  points.push_back({80.0f, 80.0f});

  initGL();
  glutKeyboardFunc(teclado);
  glutDisplayFunc(display);
  glutMouseFunc(mouseHandler);
  glutMotionFunc(motionHandler);

  createContextMenu();
  glutMainLoop();
  return 0;
}

/* ------------- Código para el manejo de sliders ---------------------- */
void initSlider() {
  angE2 = {-120, 120, 20.0, 15.0, 10.0, 100.0, 50, 60, 1, false};

  angE3 = {10, 120, 20.0, 15.0, 10.0, 100.0, 50, 130, 20, false};
}

void drawSlider(Slider slider, const char *name) {
  // Dibujar barra del slider
  glPushMatrix();

  //  Dibujar cursor del slider
  glColor3f(0.2f, 0.2f, 0.8f); // Color azul para el cursor
  float cursorX =
      slider.x +
      (slider.value - slider.min) * slider.b_width /
          (slider.max -
           slider.min); // Posición del cursor según el valor del slider
  glBegin(GL_QUADS);
  glVertex2f(cursorX - slider.c_width / 2, slider.y);
  glVertex2f(cursorX + slider.c_width / 2, slider.y);
  glVertex2f(cursorX + slider.c_width / 2, slider.y + slider.c_height);
  glVertex2f(cursorX - slider.c_width / 2, slider.y + slider.c_height);
  glEnd();

  glColor3f(0.8f, 0.8f, 0.8f); // Color gris para la barra
  glBegin(GL_QUADS);
  glVertex2f(slider.x, slider.y);
  glVertex2f(slider.x + slider.b_width, slider.y);
  glVertex2f(slider.x + slider.b_width, slider.y + slider.b_height);
  glVertex2f(slider.x, slider.y + slider.b_height);
  glEnd();

  // std::cout << "display: " << "num_vertex.value=" << num_vertex.value <<
  // "\n";

  glColor3f(0.0f, 0.0f, 0.0f); // Color azul para el cursor
  char buffer[5];
  sprintf_s(buffer, "%d", slider.value);
  printText(cursorX - 3, slider.y + slider.c_height + 3,
            GLUT_BITMAP_HELVETICA_18, buffer);

  printText(slider.x, slider.y + slider.c_height + 25, GLUT_BITMAP_HELVETICA_12,
            name);
  glPopMatrix();
}

std::tuple<bool, int> mouseSlider(Slider slider, float posx, float posy) {
  float cursorX = slider.x + (slider.value - slider.min) * slider.b_width /
                                 (slider.max - slider.min);
  if (posx >= cursorX - slider.c_width / 2 &&
      posx <= cursorX + slider.c_width / 2 && posy >= slider.y &&
      posy <= slider.y + slider.c_height) {
    slider.dragging = true;
    slider.value =
        float(posx - slider.x) * (slider.max - slider.min) / slider.b_width +
        slider.min; // Actualizar valor del slider según posición del mouse
  }

  return std::make_tuple(slider.dragging, slider.value);
}

int motionSlider(Slider slider, float posx) {
  slider.value =
      float(posx - slider.x) * (slider.max - slider.min) / slider.b_width +
      slider.min;
  if (slider.value < slider.min)
    slider.value = slider.min; // Limitar el valor a 0.0
  if (slider.value > slider.max)
    slider.value = slider.max; // Limitar el valor a 1.0

  return slider.value;
}

void printText(float x, float y, void *font, const char *string) {
  glRasterPos2f(x, y);
  while (*string) {
    glutBitmapCharacter(font, *string);
    string++;
  }
}

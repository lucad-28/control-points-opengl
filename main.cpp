/*
   * Tema: SEGUNDA PRÁCTICA CALIFICADA
   * Curso: Computación Visual
   * Sección : G1
   * Integrantes:

       * - Sernaque Cobeñas José Manuel [22200182]
       * - Rivera Llancari Aldair Alejandro [22200134]


   * Fecha: 09/11/2024
*/

#include <GL/glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <random>


// Estructura para representar un punto de control
struct Point {
    float x, y;
};

std::vector<Point> points;

// Variables para la rotación de la curva
float ang[] = { 0.0f, 0.0f, 0.0f };
float ang3[] = { 0.0f, 0.0f, 0.0f };
int eje = 1;

// Variables para el color de la curva

std::vector<float> CatmullColor = { 0.0f, 0.0f, 1.0f };
std::vector<float> PoligonalColor = { 0.62f, 0.05f, 1.0f };
std::vector<float> SplineColor = { 0.92f,0.46f, 0.08f };

//Grosor de la curva

float grosorCurva = 3.0f;

// Bandera para el manejo del mouse y de las opciones del menú jerárquico
int curveAlgorithm = 0;
int selectedPointIndex = 2;
bool addPointsEnabled = false;
bool drawCurve = true;
bool dragPointEnabled = false;
bool showPolilinea = true;
bool showControlPoints = true;

// Constantes para el tamaño y proyección de la ventana
const int W_WIDTH = 400;
const int W_HEIGHT = 400;
const float X_MAX = 200.0f;
const float Y_MAX = 200.0f;

/*------------- Código para el manejo de sliders ---------------------- */
/*
    * un slider es un control que usamos para cambiar el ángulo de rotación
    * en los campos 2 y 3 dinámicamente arrastrando el mouse sobre el slider
*/
/* --------- Modificado a partir de código proporcionado por ChatGPT -------------- */
void printText(float, float, void*, const char*);

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
Slider angE1;

void initSlider();
void drawSlider(Slider, const char*);
std::tuple<bool, int> mouseSlider(Slider, float, float, int);
int motionSlider(Slider, float, int);
/* ------------------------------------------------------------------- */


// Función para calcular la posición de un punto en la curva Catmull-Rom - Modificado a partir de código proporcionado por CHATGPT

const float catmullRomMatrix[4][4] = { {-0.5, 1.5, -1.5, 0.5},
                                      {1.0, -2.5, 2.0, -0.5},
                                      {-0.5, 0.0, 0.5, 0.0},
                                      {0.0, 1.0, 0.0, 0.0} };

Point calculateCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3) {
    float tVec[4] = { t * t * t, t * t, t, 1 };
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

    return { x, y };
}

// Dibuja la curva usando la interpolación Catmull-Rom en el primer espacio y los puntos de control ingresados
void drawCamul2D() {
    glColor3fv(CatmullColor.data());
    glLineWidth(angE1.value);
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

// Dibuja la curva usando la interpolación Catmull-Rom en el tercer espacio
void drawCamulFluido3D() {
    glColor3fv(SplineColor.data()); // Color de la curva
    glLineWidth(2.0);
    glBegin(GL_QUAD_STRIP);

    for (size_t i = 1; i < points.size() - 2; ++i) {
        Point p0 = points[i - 1];
        Point p1 = points[i];
        Point p2 = points[i + 1];
        Point p3 = points[i + 2];

        // Genera puntos en la curva entre p1 y p2
        for (float t = 0; t <= 1.0f; t += 0.01f) {
            Point pt = calculateCatmullRomPoint(t, p0, p1, p2, p3);
            glVertex3f(pt.x, 20.0f, pt.y);
            glVertex3f(pt.x, 0.0f, pt.y);
        }
    }

    glEnd();
}

// Dibuja la cinta poligonal en el segundo espacio
void drawCamulSuperficie3D() {
    glColor3fv(PoligonalColor.data());
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

// Dibuja los puntos de control en el espacio 2D (Primer espacio)
void drawPoints2D() {
    glPointSize(5.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& point : points) {
        glVertex2f(point.x, point.y);
    }
    glEnd();
}

// Dibujo de la polilínea formada por los puntos de control
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

// Dibuja los ejes coordenados, usado en los tres espacios
void drawEjes() {
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-150.0f, 0.0f, 0.0f);
    glVertex3f(150.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -150.f);
    glVertex3f(0.0f, 0.0f, 150.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, -150.0f, 0.0f);
    glVertex3f(0.0f, 150.0f, 0.0f);
    glEnd();
}

/*--------- Funciones para el dibujo de una plataforma para los espacios 2 y 3 -------------*/

const GLfloat baseSize = 200.0f; // Tamaño de la base de la plataforma
const GLfloat altura = 5.0f;     // Altura de la plataforma

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

// Dibujo de una plataforma 3D para los espacios 2 y 3
void drawPlataforma() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0.0f, -altura, 0.0f);

    glBegin(GL_QUADS);
    glColor4f(0.196f, 0.36f, 0.43f, 0.3f);
    drawPointsPlataforma();
    glEnd();

    glBegin(GL_LINES);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    drawPointsPlataforma();
    glEnd();

    glPopMatrix();
    glDisable(GL_BLEND);
}


/*-----------------------------------------------------------------------------------*/


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0, 0, 400, 400);
    drawSlider(angE1, "Grosor de la Curva");
    glLoadIdentity();
    drawEjes();

    if (drawCurve)
        drawCamul2D();
    if (showControlPoints)
        drawPoints2D();
    if (showPolilinea)
        drawPolilineas();

    // Creación y Actualización del espacio central (Espacio 2 - Cinta poligonal)
    glViewport(400, 0, 400, 400);
    drawSlider(angE2, "Angulo de Rotacion:");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 1.0f, 800.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(50.0f, 100.0f, 300.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    drawEjes();

    glPushMatrix();
    glRotatef(ang[0], 1.0f, 0.0f, 0.0f);
    glRotatef(ang[1], 0.0f, 1.0f, 0.0f);
    glRotatef(ang[2], 1.0f, 0.0f, 1.0f);
    drawCamulSuperficie3D();
    drawPlataforma();
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-200.0, 200.0, -200.0, 200.0, -200.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Creación y Actualización del espacio derecho (Espacio 3 - cinta Spline de Catmull Rom.)
    glViewport(800, 0, 400, 400);
    drawSlider(angE3, "Angulo de Rotacion:");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 1.0f, 1.0f, 800.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(50.0f, 100.0f, 300.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    drawEjes();

    glPushMatrix();
    glRotatef(ang3[0], 1.0f, 0.0f, 0.0f);
    glRotatef(ang3[1], 0.0f, 1.0f, 0.0f);
    glRotatef(ang3[2], 1.0f, 0.0f, 1.0f);
    drawCamulFluido3D();
    drawPlataforma();
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

// Controlador para el mouse cuando se presiona un botón

void mouseHandler(int button, int state, int x, int y) {

    std::cout << "x: " << x << " y: " << y << "\n";

    float posx = -1 * X_MAX + (2 * X_MAX) * (x) / W_WIDTH;
    float posy = Y_MAX - (2 * Y_MAX) * y / (W_HEIGHT);

    if (x >= 400 && x < 800) {  // Cambio de posición si presionamos en el segundo espacio (viewport 2)
        posx = -1 * X_MAX + (2 * X_MAX) * (x - 400) / 400;
        posy = Y_MAX - (2 * Y_MAX) * y / 400;
    }
    else if (x >= 800 && x < 1200) {  // Cambio de posición si presionamos en el tercer espacio (viewport 3)
        posx = -1 * X_MAX + (2 * X_MAX) * (x - 800) / 400;
        posy = Y_MAX - (2 * Y_MAX) * y / 400;
    }
    else if (x >= 0 && x < 400) {
        posx = -1 * X_MAX + (2 * X_MAX) * (x) / W_WIDTH;
        posy = Y_MAX - (2 * Y_MAX) * y / (W_HEIGHT);
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Solo incluir puntos de control si presionamos en el primer espacio (viewport 1)
        if (x <= 400) {
            int index = getPointIndexAt(posx, posy);
            if (index != -1) {
                selectedPointIndex = index;
            }
            else if (addPointsEnabled) {
                points.push_back({ posx, posy });
            }
        }
        std::cout << "posx: " << posx << " posy: " << posy << "\n";

        if (x >= 400 && x < 800) {  // Si presionamos en el segundo espacio, iniciamos el control del slider2 (viewport 2)
            std::tie(angE2.dragging, angE2.value) = mouseSlider(angE2, posx, posy, x);
            std::cout << "angE2.dragging=" << angE2.dragging << "\n";
        }
        else if (x >= 800 && x < 1200) {  // Si presionamos en el tercer espacio, iniciamos el control del slider3 (viewport 3)
            std::tie(angE3.dragging, angE3.value) = mouseSlider(angE3, posx, posy, x);
        }
        else if (x >= 0 && x < 400) {
            std::tie(angE1.dragging, angE1.value) = mouseSlider(angE1, posx, posy, x);
        }

        glutPostRedisplay();
    }
    // Si se suelta el botón del mouse
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (dragPointEnabled)
            selectedPointIndex = -1;
        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            if (angE2.dragging) {
                angE2.dragging = false;
            }
            else if (angE3.dragging) {
                angE3.dragging = false;
            }
            else if (angE1.dragging) {
                angE1.dragging = false;
            }
        }
        glutPostRedisplay();
    }
}

// Controlador para el mouse en movimiento con el botón izquierdo presionado
void motionHandler(int x, int y) {
    if (selectedPointIndex != -1 && dragPointEnabled) {
        points[selectedPointIndex].x = (float)(x - (400.0f / 2));
        points[selectedPointIndex].y = -(float)(y - (400.0f / 2));
    }
    float posx = -1 * X_MAX + (2 * X_MAX) * x / W_WIDTH;

    if (x >= 400 && x < 800) {  // Segundo espacio (viewport 2)
        posx = -1 * X_MAX + (2 * X_MAX) * (x - 400) / 400;
    }
    else if (x >= 800 && x < 1200) {  // Tercer espacio (viewport 3)
        posx = -1 * X_MAX + (2 * X_MAX) * (x - 800) / 400;
    }
    else if (x >= 0 && x < 400) {
        posx = -1 * X_MAX + (2 * X_MAX) * x / W_WIDTH;
    }

    if (angE2.dragging) {
        // Actualizar el valor del slider mientras se arrastra
        angE2.value = motionSlider(angE2, posx, x);
        std::cout << "angE2.value=" << angE2.value << "\n";
    }
    if (angE3.dragging) {
        // Actualizar el valor del slider mientras se arrastra
        angE3.value = motionSlider(angE3, posx, x);
        std::cout << "angE3.value=" << angE3.value << "\n";
    }
    if (angE1.dragging) {
        // Actualizar el valor del slider mientras se arrastra
        angE1.value = motionSlider(angE1, posx, x);
        std::cout << "angE1.value=" << angE1.value << "\n";
    }
    glutPostRedisplay();
}

/*-------------- Campos y Subcampos del Menú jeráruico -------------------*/

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

void menudibujarCurva(int opcion) {

    switch (opcion) {
    case 1:
        drawCurve = true;
        break;
    case 2:
        drawCurve = false;
        break;
    }
    glutPostRedisplay();

}

void menuCurvaColor(int opcion) {

    switch (opcion) {
    case 1:
        CatmullColor = { 1.0f, 0.0f ,0.0f };
        break;
    case 2:
        CatmullColor = { 0.0f, 1.0f ,0.0f };
        break;
    case 3:
        CatmullColor = { 0.0f, 0.0f ,1.0f };
        break;
    case 4:
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        CatmullColor = { (float)dis(gen),(float)dis(gen), (float)dis(gen) };
        break;
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

void menuColorPoligonalCurve(int option) {
    switch (option) {
    case 1:
        PoligonalColor = { 0.62f, 0.05f, 1.0f };
        break;
    case 2:
        PoligonalColor = { 0.0f, 0.0f ,0.0f };
        break;
    }
    glutPostRedisplay();
}

void menuColorSplineCurve(int option) {
    switch (option) {
    case 1:
        SplineColor = { 0.92f,0.46f, 0.08f };
        break;
    case 2:
        SplineColor = { 0.03f,0.717f , 0.98f };
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

void menuPrincipalHandler(int option) {
    switch (option) {
    default:
        break;
    }
    glutPostRedisplay();
}

// Configuración y estructura del menú jerárquico
void createContextMenu() {
    int menuAgregarPuntos = glutCreateMenu(menuAddPointHandler);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menuControlPoints = glutCreateMenu(menuShowControlPoints);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);

    int menuDibujarCurva = glutCreateMenu(menudibujarCurva);
    glutAddMenuEntry("Activado", 1);
    glutAddMenuEntry("Desactivado", 2);

    int menuColorCurva = glutCreateMenu(menuCurvaColor);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Aleatorio", 4);

    int menuDragPoint = glutCreateMenu(menuDragPointHandler);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);

    int menuColorPoligonal = glutCreateMenu(menuColorPoligonalCurve);
    glutAddMenuEntry("Morado", 1);
    glutAddMenuEntry("Negro", 2);

    int menuColorSpline = glutCreateMenu(menuColorSplineCurve);
    glutAddMenuEntry("Naranja", 1);
    glutAddMenuEntry("Celeste", 2);


    int menuPolilineas = glutCreateMenu(menuShowPolilineas);
    glutAddMenuEntry("Activar", 1);
    glutAddMenuEntry("Desactivar", 2);

    int menu = glutCreateMenu(menuPrincipalHandler);
    glutAddSubMenu("Insertar los puntos de control", menuAgregarPuntos);
    glutAddSubMenu("Graficar Puntos de Control", menuControlPoints);
    glutAddSubMenu("Graficar Curva", menuDibujarCurva);
    glutAddSubMenu("Color de la Curva", menuColorCurva);

    glutAddSubMenu("Cambiar posición de los puntos de control", menuDragPoint);


    glutAddSubMenu("Color de la Cinta Poligonal", menuColorPoligonal);

    glutAddSubMenu("Color de la Cinta Spline", menuColorSpline);

    glutAddSubMenu("Mostrar Polilineas", menuPolilineas);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Manejo de Entradas por teclado para las rotaciones en los campos 2 y 3
const std::string KEYS_AVAILABLE = "qwaszx";
void teclado(unsigned char key, int x, int y) {
    if (KEYS_AVAILABLE.find(key) != std::string::npos) {
        if (key == 'q') {
            ang[0] += angE2.value;
            ang3[0] += angE3.value;
        }
        else if (key == 'w') {
            ang[0] -= angE2.value;
            ang3[0] -= angE3.value;
        }
        else if (key == 'a') {
            ang[1] += angE2.value;
            ang3[1] += angE3.value;
        }
        else if (key == 's') {
            ang[1] -= angE2.value;
            ang3[1] -= angE3.value;
        }
        else if (key == 'z') {
            ang[2] += angE2.value;
            ang3[2] += angE3.value;
        }
        else if (key == 'x') {
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
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 400);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("Segunda Práctica Calificada");

    points.push_back({ -100.0f, -40.0f });
    points.push_back({ -40.0f, 30.0f });
    points.push_back({ 40.0f, -30.0f });
    points.push_back({ 80.0f, 60.0f });

    initGL();
    glutKeyboardFunc(teclado);
    glutDisplayFunc(display);
    glutMouseFunc(mouseHandler);
    glutMotionFunc(motionHandler);

    createContextMenu();
    glutMainLoop();
    return 0;
}

/*------------- Código para el manejo de sliders ---------------------- */
/*
    * un slider es un control que usamos para cambiar el ángulo de rotación
    * en los campos 2 y 3 dinámicamente arrastrando el mouse sobre el slider
*/
/* --------- Modificado a partir de código proporcionado por ChatGPT -------------- */

void initSlider() {
    angE2 = { -150, 140, 20.0, 15.0, 10.0, 100.0, 50, 60, 1, false };

    angE3 = { -150, 140, 20.0, 15.0, 10.0, 100.0, 50, 60, 1, false
    };

    angE1 = { -150, 120, 20.0, 15.0, 10.0, 100.0, 5, 10, 1, false };
}

void drawSlider(Slider slider, const char* name) {
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


    glColor3f(0.0f, 0.0f, 0.0f); // Color azul para el cursor
    char buffer[5];
    sprintf_s(buffer, "%d", slider.value);
    printText(cursorX - 3, slider.y + slider.c_height + 3,
        GLUT_BITMAP_HELVETICA_18, buffer);

    printText(slider.x, slider.y + slider.c_height + 25, GLUT_BITMAP_HELVETICA_12,
        name);
    glPopMatrix();
}

std::tuple<bool, int> mouseSlider(Slider slider, float posx, float posy, int x) {
    float cursorX = slider.x + (slider.value - slider.min) * slider.b_width /
        (slider.max - slider.min);

    if (posx >= cursorX - slider.c_width / 2 &&
        posx <= cursorX + slider.c_width / 2 && posy >= slider.y &&
        posy <= slider.y + slider.c_height)
    {
        slider.dragging = true;
        slider.value =
            float(posx - slider.x) * (slider.max - slider.min) / slider.b_width +
            slider.min; // Actualizar valor del slider según posición del mouse
    }
    return std::make_tuple(slider.dragging, slider.value);

    return std::make_tuple(false, slider.value);
}

int motionSlider(Slider slider, float posx, int x) {
    slider.value =
        float(posx - slider.x) * (slider.max - slider.min) / slider.b_width +
        slider.min;
    if (slider.value < slider.min)
        slider.value = slider.min; // Limitar el valor a 0.0
    if (slider.value > slider.max)
        slider.value = slider.max; // Limitar el valor a 1.0

    return slider.value;
}

void printText(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(font, *string);
        string++;
    }
}
/*----------------------------------------------------------------------------- */
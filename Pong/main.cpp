//
//  main.cpp
//  LeapPong
//
//  Created by Carlos González on 10/01/14.
//  El codigo de el juego sin ninguna modificacion lo encontrareis
//  http://www.foro.lospillaos.es/viewtopic.php?p=24772
//  Copyright (c) 2014 Carlos González. All rights reserved.
//

#include <iostream>
#include <Leap.h>
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace Leap;

#define PI 3.141592

bool teclas[5];

//Clase Barra, clase donde se gestionara
//todo lo relacionado los las dos barras
class Barra {
public:
    float posicion;
    bool lado;
    
    //Inicializamos las Barras en su posición inicial
    void Iniciar(bool lado_barra) {
        posicion = 12;
        lado = lado_barra;
    }
    //Funcion que delimita los movimientos del las barras
    void Avanzar(bool arriba, bool abajo) {
        if (arriba) posicion += 0.5f;
        if (abajo) posicion -= 0.5f;
        if (posicion < 0) posicion = 0;
        if (posicion > 24) posicion = 24;
    }
    //Funcion que dibuja las barras, si no estan siendo detectadoas por el Leap Motion
    void Pintar(bool lado) {
        glPushMatrix();
        glTranslatef(5+(lado*30),posicion,0);
        glColor3f(1,0,0);
        glRectf(-0.25,0,0.25f,1);
        glColor3f(1,1,1);
        glRectf(-0.25f,1,0.25f,5);
        glColor3f(1,0,0);
        glRectf(-0.25,5,0.25f,6);
        glPopMatrix();
    }
    //Funcion que dibuja las barras, si estan siendo detectadoas por el Leap Motion
    void PintarDetectado(bool lado) {
        glPushMatrix();
        glTranslatef(5+(lado*30),posicion,0);
        glColor3f(0,1,0);
        glRectf(-0.25,0,0.25f,1);
        glColor3f(1,1,1);
        glRectf(-0.25f,1,0.25f,5);
        glColor3f(0,1,0);
        glRectf(-0.25,5,0.25f,6);
        glPopMatrix();
    }
};
//Clase para gestionar la bola
class Bola {
public:
    
    float posicion[2], velocidad, rapida, angulo;
    bool pausado;
    
    //Inicializamos la bola en su posicion innicial y con su primer movimiento
    void Iniciar() {
        posicion[0] = 20;
        posicion[1] = 15;
        velocidad = 0.25f;
        rapida = 1;
        angulo = 1.3*PI;
        pausado = 1;
    }
    
    void desPausar(bool pausa) {
        if (pausa) pausado = 0;
    }
    //Establacemos la aceleracion que vamos ha tener de la bola en relacion al tiempo
    void Acelerar() {
        velocidad += 0.0001f;
    }
    //Funcion que gestina el movimiento de la bola dependiendo de la direccion
    void Avanzar(int direccion) {
        posicion[0] += (!pausado)*direccion*rapida*velocidad*cosf(angulo);
        posicion[1] += (!pausado)*direccion*rapida*velocidad*sinf(angulo);
    }
    //Funcion que gestiona la colisiones
    void Rebotar(Barra *bar) {
        if (posicion[1] > 30 || posicion[1] < 0) {
            Avanzar(-1);
            angulo = 2*PI - angulo;
            Avanzar(1);
        }
        if (posicion[0] < 5.5f && posicion[0] > 4.5f && posicion[1] > bar[0].posicion && posicion[1] < bar[0].posicion + 6) {
            Avanzar(-1);
            angulo = (-PI/2) + ((PI/8) + ((posicion[1] - bar[0].posicion)/6)*(PI - PI/4));
            if (posicion[1] - bar[0].posicion < 1 || posicion[1] - bar[0].posicion > 5)
                rapida = 1;//rapida = 2;
            else
                rapida = 1;
            Avanzar(1);
        }
        if (posicion[0] < 35.5f && posicion[0] > 34.5f && posicion[1] > bar[1].posicion && posicion[1] < bar[1].posicion + 6) {
            Avanzar(-1);
            angulo = (-PI/2) - ((PI/8) + ((posicion[1] - bar[1].posicion)/6)*(PI - PI/4));
            if (posicion[1] - bar[1].posicion < 1 || posicion[1] - bar[1].posicion > 5)
                rapida = 1;//rapida = 2;
            else
                rapida = 1;
            Avanzar(1);
        }
    }
    //Funcion para pintar la bola
    void Pintar() {
        glPushMatrix();
        glColor3f(1,1,1);
        glTranslatef(posicion[0],posicion[1],0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0,0);
        for (float ang = 0; ang <= PI*2; ang += PI/8) glVertex2f(0.25*cosf(ang),0.25*sinf(ang));
        glEnd();
        glPopMatrix();
    }
};
//Clase que gestiona el marcador
class Marcador {
public:
    int jugador[2];
    //Dependiendo de por donde
    void Puntuar(Bola& bola) {
        if (bola.posicion[0] < 2) {
            jugador[0]++;
            bola.Iniciar();
        }
        if (bola.posicion[0] > 38) {
            jugador[1]++;
            bola.Iniciar();
        }
    }
    //Pintamos el marcador en la parte superior de la ventana
    void Pintar() {
        
        char marcador[15];
        sprintf(marcador,"J1: %i | J2: %i",jugador[1],jugador[0]);
        glRasterPos3f(18,29,0);
        for (int i = 0; i < strlen(marcador); i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, marcador[i]);
    }
};

//Creamos dos Barras, una bola y el marcador
Barra barras[2];
Bola bola;
Marcador puntos;
bool J1detectado = false;
bool J2detectado = false;

//Pintamos todo al escena
void PintarEscena() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,40,0,30,-1,1);
    
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    //Las dos barras
    for (int i = 0; i < 2; i++)
        barras[i].Pintar(i);
    //la bola y punto
    bola.Pintar();
    puntos.Pintar();
    
    glutSwapBuffers();
}
//Pintamos la escena dependiendo de la deteccion del Leap Motion
void PintarEscenaDetectado() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,40,0,30,-1,1);
    
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    //dependiendo si detecta al jugador pintara de un color u otro
    if(J1detectado)
        barras[0].PintarDetectado(0);
    else
        barras[0].Pintar(0);
    if(J2detectado)
        barras[1].PintarDetectado(1);
    else
        barras[1].Pintar(1);
    
    bola.Pintar();
    puntos.Pintar();
    
    glutSwapBuffers();
}
//Pintamos la escena en la ventana
void ReProyectar(int w, int h) {
    glutReshapeWindow(800,600);
    glViewport(0, 0, w, h);
    
    PintarEscenaDetectado();
}
//Ademas de ponder usar el Leap Motion podemos usar las teclas de teclado
void Teclas(unsigned char key, int x, int y) {
    if (key == 13) teclas[0] = 1;
    if (key == 'w' || key == 'W') teclas[1] = 1;
    if (key == 's' || key == 'S') teclas[2] = 1;
    if (key == 'i' || key == 'I') teclas[3] = 1;
    if (key == 'k' || key == 'K') teclas[4] = 1;
}

void TeclasUp(unsigned char key, int x, int y) {
    if (key == 13) teclas[0] = 0;
    if (key == 'w' || key == 'W') teclas[1] = 0;
    if (key == 's' || key == 'S') teclas[2] = 0;
    if (key == 'i' || key == 'I') teclas[3] = 0;
    if (key == 'k' || key == 'K') teclas[4] = 0;
}
//funcion que se encargara de refrescar la pantalla
void Mover(int value) {
    glutTimerFunc(17,Mover,1);
    glutPostRedisplay();
    
    barras[0].Avanzar(teclas[1],teclas[2]);
    barras[1].Avanzar(teclas[3],teclas[4]);
    bola.Avanzar(1);
    bola.desPausar(teclas[0]);
    bola.Rebotar(barras);
    bola.Acelerar();
    puntos.Puntuar(bola);
}
//Clase sample, que sera la encangada de gestionar el Leap Motion
class SampleListener : public Listener {
public:
    
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    
    int pos1 ;
    int pos2 ;
    float t ;
    
};
//Iniciamos el Sample
void SampleListener::onInit(const Controller& controller) {
    std::cout << "Initialized" << std::endl;
}

//Comporbamos que esta conectado
void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
}
//Comprobamos si el dispositivo se ha desconectado
void SampleListener::onDisconnect(const Controller& controller) {
    //Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}
//
void SampleListener::onExit(const Controller& controller) {
    std::cout << "Exited" << std::endl;
}

//funcion que gestionara el movimiento con el Leap Motion
void SampleListener::onFrame(const Controller& controller) {
    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    Hand hand1;
    Hand hand2;
    
    if(frame.hands().count() == 1){
        Hand hand = frame.hands()[0];
        if(hand.palmPosition().x <= 0){
            hand1 = hand;
        }else{
            hand2 = hand;
        }
        
    }else if(frame.hands().count() == 2){
        Hand Ha1 = frame.hands()[0];
        Hand Ha2 = frame.hands()[1];
        
        if(Ha1.palmPosition().x <= 0){
            hand1 = Ha1;
            hand2 = Ha2;
        }else{
            hand2 = Ha1;
            hand1 = Ha2;
        }
        
    }
    
    const FingerList fingers1 = hand1.fingers();
    const FingerList fingers2 = hand2.fingers();
    
    //Siempre se necesitaran la mano abierta para mover las barras
    if(fingers1.count() == 5){
        int npos1 = fingers1[0].tipPosition()[1];
        int j1 = fingers1[0].tipPosition()[0];
        if ( pos1 < npos1 and j1 < 0){
            barras[0].Avanzar(1, 0);
            pos1 = npos1;
            J1detectado = true;
        }else if (pos1 > npos1 and j1 < 0){
            barras[0].Avanzar(0,1);
            pos1 = npos1;
            J1detectado = true;
        }
    }else{
        J1detectado = false;
    }
    
    if(fingers2.count() == 5){
        int npos2 = fingers2[0].tipPosition()[1];
        int j2 = fingers2[0].tipPosition()[0];
        if ( pos2 < npos2 and j2 > 0){
            barras[1].Avanzar(1, 0);
            pos2 = npos2;
            J2detectado = true;
        }else if (pos2 > npos2 and j2 > 0){
            barras[1].Avanzar(0,1);
            pos2 = npos2;
            J2detectado = true;
        }
    }else{
        J2detectado = false;
    }
    //Si el jugador uno abre la mano la bola se pone en juego
    if(hand1.pinchStrength()){
        teclas[0] = 1;
    }
    else{
        teclas[0] = 0;
    }
}

void SampleListener::onFocusGained(const Controller& controller) {
    std::cout << "Focus Gained" << std::endl;
    
}

void SampleListener::onFocusLost(const Controller& controller) {
    std::cout << "Focus Lost" << std::endl;
}

int main(int argc, char **argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(40,40);
    glutCreateWindow("Pong");
    
    for (int i = 0; i < 2; i++)
        barras[i].Iniciar(i);
    bola.Iniciar();
    
    glutReshapeFunc(ReProyectar);
    glutDisplayFunc(PintarEscenaDetectado);
    glutKeyboardFunc(Teclas);
    glutKeyboardUpFunc(TeclasUp);
    glutTimerFunc(17,Mover,1);
    
    // Create a sample listener and controller
    SampleListener listener;
    Controller controller;
    
    // Have the sample listener receive events from the controller
    controller.addListener(listener);
    
    glutMainLoop();
    
    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();
    
    // Remove the sample listener when done
    controller.removeListener(listener);
    return 0;
}
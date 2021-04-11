#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
//#include "image.h"
#include <SOIL/SOIL.h>


#define PI 3.1415


using namespace std;

GLint WIDTH =320;
GLint HEIGHT=240;

GLfloat obs[3]= {0.0,7.0,0.0};
GLfloat olho[3]= {0.0,3.0,0.0};

GLfloat plano_difusa[]    = { 0.5, 0.5, 0.0, 1.0 };
GLfloat plano_especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat plano_brilho[]    = { 50.0 };

GLfloat mat_a_difusa[]    = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_a_especular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_a_brilho[]    = { 50.0 };

GLfloat mat_b_difusa[]    = { 0.7, 0.7, 0.7, 0.5 };
GLfloat mat_b_especular[] = { 1.0, 1.0, 1.0, 0.5 };
GLfloat mat_b_brilho[]    = { 50.0 };

GLfloat posicao_luz0[]    = { 0.0, 10.0, 0.0, 1.0};
GLfloat cor_luz0[]        = { 1.0, 1.0, 1.0, 1.0};
GLfloat cor_luz0_amb[]    = { 0.3, 0.3, 0.3, 1.0};

GLfloat posicao_luz1[]    = { 0.0, 10.0, 5.0, 1.0};
GLfloat cor_luz1[]        = { 0.0, 0.0, 1.0, 1.0};
GLfloat direcao_luz1[]    = { 0.0, -10.0, -5.0, 1.0};
GLint   spot_luz1         = 30;

GLfloat sem_cor[]         = { 0.0, 0.0, 0.0, 1.0};

GLint gouraud=0;

GLfloat tetaxz=0;
GLfloat raioxz=14;

static float m = 0.2;
static float s = 50.0;          // Expoente especular do material (shininess)

float matShine[] = { s };                       // expoente especular (shininess)

float theta=0.0;

bool iluminacao = true;
bool executando=true;
bool f1 = false;
bool f2 = false;
bool fa = false;
bool fb = false;
static bool usarTextura = true;
static bool localViewer = false;
bool planoOrbital = true;


//ILUMINACAO
float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
float lightDif0[] = { 5, 5, 5, 1.0 };
float lightSpec0[] = { 5, 5, 5, 1.0 };
float lightPos0[] = { 0.5, 0.5, 0.5, 1 };

//VARIAVEIS DE TEXTURA
static int eletron;
static int nucleo;

//Musica de fundo do jogo


double anguloEsferaY=0;

void solidSphere(int radius, int stacks, int columns) {
    // cria uma quádrica
    GLUquadric* quadObj = gluNewQuadric();
    // estilo preenchido... poderia ser GLU_LINE, GLU_SILHOUETTE
    // ou GLU_POINT
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    // chama 01 glNormal para cada vértice.. poderia ser
    // GLU_FLAT (01 por face) ou GLU_NONE
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    // chama 01 glTexCoord por vértice
    gluQuadricTexture(quadObj, GL_TRUE);
    // cria os vértices de uma esfera
    gluSphere(quadObj, radius, stacks, columns);
    // limpa as variáveis que a GLU usou para criar
    // a esfera
    gluDeleteQuadric(quadObj);
}

void loadTextura() {
    glClearColor(0,0,0, 0.0);
    glEnable(GL_DEPTH_TEST);

    //textura nucleo
    nucleo = SOIL_load_OGL_texture
             (
                 "sol.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
             );
    if(nucleo == 0) {
        cout << "Problema ao carregar textura 0: " << SOIL_last_result() << endl;
    }

    //textura eletrons
    eletron = SOIL_load_OGL_texture
              (
                  "netuno.jpg",
                  SOIL_LOAD_AUTO,
                  SOIL_CREATE_NEW_ID,
                  SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
              );
    if(eletron == 0) {
        cout << "Problema ao carregar textura 0: " << SOIL_last_result() << endl;
    }

}
//TEXTURA QUE VOCE SEGURIU PRA GENTE USAR(NAO FUNCIONOU)
GLuint carregaTextura(const char* arquivo) {
    // cria um id para a textura
    GLuint idTextura;
    glGenTextures(1, &idTextura);
    glBindTexture(GL_TEXTURE_2D, idTextura);

    int largura, altura, canais;
    unsigned char* texels = SOIL_load_image(arquivo, &largura, &altura, &canais, SOIL_LOAD_RGBA);

    // inverte o eixo t (a SOIL_load_OGL_texture fazia isso automaticamente...)
    for(int j = 0; j < altura / 2; j++ ) {
        int index1 = j * largura * 4;
        int index2 = (altura - 1 - j) * largura * canais;

        for(int i = largura * canais; i > 0; i--) {
            unsigned char temp = texels[index1];
            texels[index1] = texels[index2];
            texels[index2] = temp;
            ++index1;
            ++index2;
        }
    }

    // envia os texels da textura para a VRAM (memória da placa de vídeo_
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texels);

    // libera a RAM que estava guardando a textura (já que ela já foi para a VRAM)
    SOIL_free_image_data(texels);

    if (texels == nullptr) {
        printf("Erro do SOIL '%s' tentando carregar o arquivo '%s'.\n", SOIL_last_result(), arquivo);
    }
    return idTextura;
}


void reshape(int width, int height) {
    WIDTH=width;
    HEIGHT=height;
    glViewport(0,0,(GLint)width,(GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0,width/(float)height,0.1,30.0);
    glMatrixMode(GL_MODELVIEW);
}

void desenhaPlanoOrbital() {
    int subdivisoes = 100;

    int i, j;
    float alturaDoSubTriangulo = 1000.0f / (subdivisoes);
    float larguraDoSubTriangulo = 1000.0f / (subdivisoes);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glPushMatrix();
    glTranslatef(-500, -500, 0);
    for (i = 0; i < subdivisoes; i++) {
        glBegin(GL_TRIANGLE_STRIP);

        // define o vetor normal do ladrilho
        glNormal3f(0,  0, 1);
        float yVerticesLinhaDeBaixo = alturaDoSubTriangulo * (i);
        float yVerticesLinhaDeCima = yVerticesLinhaDeBaixo
                                     + alturaDoSubTriangulo;

        for (j = 0; j < subdivisoes + 1; j++) {
            float xVertice = larguraDoSubTriangulo * j;
            glTexCoord2f(xVertice, yVerticesLinhaDeCima);
            glVertex2f(xVertice, yVerticesLinhaDeCima);
            glTexCoord2f(xVertice, yVerticesLinhaDeBaixo);
            glVertex2f(xVertice, yVerticesLinhaDeBaixo);
        }
        glEnd();
    }
    glPopMatrix();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void desenhaSol() {
// Desenha a esfera grande e bem arredondada

    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nucleo);
        glDisable(GL_LIGHTING);
    }

    glPushMatrix();

    glTranslatef(0,0,0);
    glRotatef(theta, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(3, 100, 100);

    glPopMatrix();
    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }
}

void desenhaMarte() {
    int distanciaAteOSol = 4;
    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(theta,0,0,1);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 200, 200);
    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }

}
void desenhaNetuno() {
    int distanciaAteOSol = 5.5;

    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(3*theta,1,0,0);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 25, 10);

    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }

}
void desenhaJupiter() {
    int distanciaAteOSol = 7;

    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(2*theta,1,1,0);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 25, 10);

    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }


}
void desenhaLua() {
    int distanciaAteOSol = 8.5;

    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(0.5*theta,0,1,0);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 25, 10);

    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }

}

void desenhaBola6() {
    int distanciaAteOSol = 10;

    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(1.5*theta,0,1,-2);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 25, 10);

    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }

}

void desenhaBola7() {
    int distanciaAteOSol = 11.5;

    // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, eletron);
    }

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotatef(5*theta,-1,0,0);
    glTranslatef(-distanciaAteOSol*1.2*cos(anguloEsferaY/25),0,distanciaAteOSol*sin(anguloEsferaY/25));
    solidSphere(1.0, 25, 10);

    glPopMatrix();

    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }

}

void desenhaCena(void) {

    int i=0;
    float globAmb[] = { m, m, m, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);        // Luz ambiente global
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);// Enable local viewpoint

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    /* calcula a posicao do observador */
    theta+=0.1;
    glutPostRedisplay();
    obs[0]=raioxz*cos(2*PI*tetaxz/360);
    obs[2]=raioxz*sin(2*PI*tetaxz/360);

    gluLookAt(obs[0],obs[1],obs[2]+i,olho[0],olho[1],olho[2],0.0,1.0,0.0);

    if(planoOrbital) {
        glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        desenhaPlanoOrbital();
        glPopMatrix();
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    desenhaSol();

    if(iluminacao) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0); //Habilita iluminacaos
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0); //Desabilita iluminacaos
    }

    desenhaMarte();
    desenhaNetuno();
    desenhaJupiter();
    desenhaLua();
    desenhaBola6();
    desenhaBola7();
    glutSwapBuffers();


}
void special(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        if(fa==false) {
            if(f2==true) {
                obs[1]+=14;

            } else {
                obs[2]-=10;
                obs[1]+=10;
                f1=true;
            }
            fa=true;
            fb=false;

        }

        break;

    case GLUT_KEY_F2:

        if(fb==false) {
            if(f1==true) {
                obs[2]+=10;
                obs[1]-=14;
            } else {
                obs[1]-=4;
                f2=true;
            }
            fb=true;
            fa=false;
        }


        break;

    case GLUT_KEY_LEFT:
        tetaxz=tetaxz+2;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        tetaxz=tetaxz-2;
        glutPostRedisplay();
        break;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'g':
        gouraud = !gouraud;
        glutPostRedisplay();
        break;
    case 'r':
        raioxz=raioxz+1;
        glutPostRedisplay();
        break;
    case 'R':
        if(raioxz>1) {
            raioxz=raioxz-1;
            glutPostRedisplay();
        }
        break;
    case 'i':
    case 'I':
        iluminacao = !iluminacao;
        break;
    }
}
void setup(void) {
    glClearColor(0,0,0, 0.0);
    glEnable(GL_DEPTH_TEST);  // Ativa teste Z

    loadTextura();

    //iniciar_musica("SpaceJam.mp3");

    float matAmbAndDif[] = {1.0, 1.0, 1.0, 1.0};    // cor ambiente e difusa: branca
    float matSpec[] = { 1.0, 1.0, 1,0, 1.0 };       // cor especular: branca

    // Definindo as propriedades do material
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

}

void rotacionaEsfera() {
    anguloEsferaY += .3f;
    glutPostRedisplay();
}


int main(int argc,char *argv[]) {

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Modelo atomico");
    glutDisplayFunc(desenhaCena);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(rotacionaEsfera);

    setup();


    glutSpecialFunc(special);


    glutAddMenuEntry("-sem spec", 0);
    glutAddMenuEntry("-spec média", 1);
    glutAddMenuEntry("-spec alta", 2);
    glutAddMenuEntry("-sem difusa", 3);
    glutAddMenuEntry("-difusa média", 4);
    glutAddMenuEntry("-difusa alta", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    glutPostRedisplay();
    return(0);
}




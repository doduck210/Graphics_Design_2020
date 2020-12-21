#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <gl/glut.h>
#include <gl/freeglut.h>
#include <gl/glext.h>
#include <math.h>
#include <windows.h>
#include "ObjParser.h"
#include "bmpfuncs.h"

using namespace std;

#define M_PI 3.1415926535897
//using std::cos;
//using std::sin;
//using std::sqrt;

// global variable for counting fps
volatile int frame = 0, atime, timebase = 0;
volatile float fps;

/* texture mapping set variable */
GLuint textureMonkey;
GLuint glass;
GLuint g_nCubeTex;

bool antialiase_on = true;
float radius = 10;
float theta = 70, phi = 90;
float cam[3];
float center[3] = { 0, 0, 0 };
float up[3] = { 0, 1, 0 };
float rot[9] = { 0,0,20,30,10,40,0,20,30 };
bool mode[9] = { 0,0,0,0,0,0,0,0,0 };
bool cammode = false;
int texturemode = 0;
bool emi[9] = { 0,0,0,0,0,0,0,0,0 };
GLfloat emission[4] = { 0,0,0,1 };
float bgRgb[3] = { 0,0,0.5 };

bool rolling;
char* str = (char*)" ";

// object var
ObjParser* roller;
ObjParser* box;
ObjParser* poll;
ObjParser* finger;
ObjParser* paddle;

// user-defined function
void init(void);
void light_default();
void get_roller(const char* str);
void get_box(const char* str);
void get_poll(const char* str);
void get_finger(const char* str);
void get_paddle(const char* str);
void mouse(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);
void keyboard(unsigned char, int, int);
void special_keyboard(int, int, int);
void draw(void);
void resize(int, int);
void idle();
void sub_menu(int);
void main_menu(int);
void mouseWheelFunc(int, int, int, int);
//...

/* Main method */
int main(int argc, char** argv)
{
    // glut initialize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("12161725 Duckhyung Ryu");	// 위의 순서 무조건 고정

    init();

    get_roller("obj/roller.obj");
    get_box("obj/box.obj");
    get_poll("obj/poll.obj");
    get_finger("obj/finger.obj");
    get_paddle("obj/paddle.obj");


    /* Create a single window with a keyboard and display callback */
    glutMouseFunc(&mouse);
    glutMotionFunc(&motion);
    glutPassiveMotionFunc(&passiveMotion);
    glutKeyboardFunc(&keyboard);
    glutSpecialFunc(&special_keyboard);
    glutDisplayFunc(&draw);
    glutReshapeFunc(&resize);
    glutMouseWheelFunc(mouseWheelFunc);

    glutIdleFunc(&idle);

    int submenu1 = glutCreateMenu(sub_menu);
    glutAddMenuEntry("Default Wood", 0);
    glutAddMenuEntry("Light Wood", 1);
    glutAddMenuEntry("Copper Metal", 2);

    glutCreateMenu(main_menu);
    glutAddMenuEntry("Quit", 999);
    glutAddMenuEntry("Init", 11);
    glutAddSubMenu("Texture", submenu1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* Run the GLUT event loop */
    glutMainLoop();

    return EXIT_SUCCESS;
}

void light_default() {
    //glClearColor(0.f, 0.f, 0.f, 1.0f);

    /* Light0 조명 관련 설정 */
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.9f, 1.0f };
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    /********* light point position setting **********/
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    /* Light1 조명 관련 설정 */
    GLfloat ambientLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat diffuseLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat specularLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat light_position1[] = { 0.0, 0.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

    /********* light point position setting **********/
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    /************* spot position setting *************/
    /*GLfloat spot_direction[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/

    GLfloat specularMaterial[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    /************* Material  setting *************/
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
    glMaterialf(GL_FRONT, GL_SHININESS, 30);


    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    /* DEPTH TEST ENABLE */
    glFrontFace(GL_CW);	
}
/*
void enviromentMapSetting(void) {
    glGenTextures(1, &g_nCubeTex);
    int width, height, channels;
    uchar* img0 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-4.bmp", &width, &height, &channels);
    uchar* img1 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-1.bmp", &width, &height, &channels);
    uchar* img2 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-2.bmp", &width, &height, &channels);
    uchar* img3 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-5.bmp", &width, &height, &channels);
    uchar* img4 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-6.bmp", &width, &height, &channels);
    uchar* img5 = readImageData("SaintPetersSquare1/Webp.net-resizeimage-3.bmp", &width, &height, &channels);

    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_CUBE_MAP);
}

void draw_skybox() {
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
    float g_nSkySize = 50;
    glBegin(GL_QUADS);
    //px
    glTexCoord3d(1.0, -1.0, -1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
    glTexCoord3d(1.0, -1.0, 1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(1.0, 1.0, 1.0);
    glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
    glTexCoord3d(1.0, 1.0, -1.0);
    glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);
    // nx
    glTexCoord3d(-1.0, -1.0, -1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
    glTexCoord3d(-1.0, -1.0, 1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, 1.0, 1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, 1.0, -1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);

    // py
    glTexCoord3d(1.0, 1.0, 1.0);
    glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, 1.0, 1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, 1.0, -1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
    glTexCoord3d(1.0, 1.0, -1.0);
    glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

    // ny
    glTexCoord3d(1.0, -1.0, 1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, -1.0, 1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, -1.0, -1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
    glTexCoord3d(1.0, -1.0, -1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);

    // pz
    glTexCoord3d(1.0, -1.0, 1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, -1.0, 1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
    glTexCoord3d(-1.0, 1.0, 1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
    glTexCoord3d(1.0, 1.0, 1.0);
    glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);

    // nz
    glTexCoord3d(1.0, -1.0, -1.0);
    glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
    glTexCoord3d(-1.0, -1.0, -1.0);
    glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
    glTexCoord3d(-1.0, 1.0, -1.0);
    glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
    glTexCoord3d(1.0, 1.0, -1.0);
    glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

    glEnd();
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_CUBE_MAP);
}*/

void setTextureMapping() {
    int imgWidth, imgHeight, channels;
    uchar* img = readImageData("materials/Webp.net-resizeimage.bmp", &imgWidth, &imgHeight, &channels);

    int texNum = 1;
    glGenTextures(texNum, &textureMonkey);
    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}
void setTextureMapping1() {
    int imgWidth, imgHeight, channels;
    uchar* img = readImageData("materials/Webp.net-resizeimage1.bmp", &imgWidth, &imgHeight, &channels);

    int texNum = 1;
    glGenTextures(texNum, &glass);
    glBindTexture(GL_TEXTURE_2D, glass);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    //GL_REPEAT 둘중 하나 선택
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}
void setTextureMapping2() {
    int imgWidth, imgHeight, channels;
    uchar* img = readImageData("materials/Webp.net-resizeimage2.bmp", &imgWidth, &imgHeight, &channels);

    int texNum = 1;
    glGenTextures(texNum, &textureMonkey);
    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    //GL_REPEAT 둘중 하나 선택
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}
void setTextureMapping3() {
    int imgWidth, imgHeight, channels;
    uchar* img = readImageData("materials/Webp.net-resizeimage3.bmp", &imgWidth, &imgHeight, &channels);

    int texNum = 1;
    glGenTextures(texNum, &textureMonkey);
    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    //GL_REPEAT 둘중 하나 선택
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);
}

void readme() {
    printf("****************************************************\n");
    printf("Arrow Keys : Moving View\n");
    printf("-,= : Zoom out/in (You can use mouse wheel as weel)\n");
    printf("r : Rolling(Typing) and the music on/off\n");
    printf("l : Change Light position\n");
    printf("z,x,c,v | m,',','.','/' : Finger Light Emission on/off\n");
    printf("You can change the background colour by clicking the colours on right\n");
    printf("You can change the material of objet in menu(mouse righ-click)\n");
    printf("Enjoy!\n");
    printf("****************************************************\n");
}

void init()
{
    //printf("init func called\n");
    // clear background color
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glColor3f(1, 1, 1);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluOrtho2D(0, 500, 500, 0);
    readme();
    // set blend
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // set antialiasing
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    glEnable(GL_LINE_SMOOTH);

    light_default();

    //glEnable(GL_CULL_FACE);

    /* TEXTURE MAPPING SET */

    glEnable(GL_TEXTURE_2D);
    setTextureMapping();
    setTextureMapping1();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
    //enviromentMapSetting();

    glEnable(GL_DEPTH_TEST);
}

void special_keyboard(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT) {
        phi -= 5;
        if (phi < 0) phi = 355;
    }
    else if (key == GLUT_KEY_RIGHT) {
        phi += 5;
        if (phi >= 360) phi = 0;
    }
    else if (key == GLUT_KEY_UP) {
        theta -= 5;
        if (theta <= 0) theta += 360;
    }
    else if (key == GLUT_KEY_DOWN) {
        theta += 5;
        if (theta > 360) theta -= 360;
    }

    if (theta > 180)
        up[1] = -1;
    else
        up[1] = 1;

    //std::cout << "theta : " << theta << ", phi : " << phi << "\n";
    glutPostRedisplay();
}

/* Keyboard callback function */
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        /* Exit on escape key press */
    case '\x1B':
    {
        exit(EXIT_SUCCESS);
        break;
    }
    case '=':
    {
        radius -= 1;
        break;
    }
    case '-':
    {
        radius += 1;
        break;
    }
    case 'r':
    {
        rolling = !rolling;
        if (rolling)
            PlaySound(TEXT("./materials/musicbox.wav"), NULL, SND_ASYNC | SND_LOOP);
        else
            PlaySound(NULL, 0, 0);
        break;
    }
    case 'l':
    {
        cammode = !cammode;
        break;
    }
    case 'z':
    {
        emi[8] = !emi[8];
        break;
    }
    case 'x':
    {
        emi[7] = !emi[7];
        break;
    }
    case 'c':
    {
        emi[6] = !emi[6];
        break;
    }
    case 'v':
    {
        emi[5] = !emi[5];
        break;
    }
    case 'm':
    {
        emi[4] = !emi[4];
        break;
    }
    case ',':
    {
        emi[3] = !emi[3];
        break;
    }
    case '.':
    {
        emi[2] = !emi[2];
        break;
    }
    case '/':
    {
        emi[1] = !emi[1];
        break;
    }
    
    }
    glutPostRedisplay();
}

void draw_axis(void)
{
    glLineWidth(1.5f);
    glBegin(GL_LINES);

    glColor4f(1.f, 0.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(4.f, 0.f, 0.f);

    glColor4f(0.f, 1.f, 0.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 4.f, 0.f);

    glColor4f(0.f, 0.f, 1.f, 1.f);
    glVertex3f(0.f, 0.f, 0.f);
    glVertex3f(0.f, 0.f, 4.f);

    glEnd();
    glLineWidth(1);
}

void draw_obj(ObjParser* objParser)
{
    glBegin(GL_TRIANGLES);
    for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
        glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
            objParser->normal[objParser->normalIdx[n] - 1].y,
            objParser->normal[objParser->normalIdx[n] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
            objParser->vertices[objParser->vertexIdx[n] - 1].y,
            objParser->vertices[objParser->vertexIdx[n] - 1].z);

        glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
            objParser->normal[objParser->normalIdx[n + 1] - 1].y,
            objParser->normal[objParser->normalIdx[n + 1] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
            objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
            objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

        glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
            objParser->normal[objParser->normalIdx[n + 2] - 1].y,
            objParser->normal[objParser->normalIdx[n + 2] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
            objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
            objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
    }
    glEnd();
}

void draw_obj_with_texture(ObjParser* objParser, int texture)
{
    glColor3f(1, 1, 1);
    //glDisable(GL_BLEND);
    //glEnable(GL_TEXTURE_2D);	// texture 색 보존을 위한 enable
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    if (texture == 1) {
        glBindTexture(GL_TEXTURE_2D, textureMonkey);
    }
    if (texture == 2) {
        glBindTexture(GL_TEXTURE_2D, glass);
    }
    glBegin(GL_TRIANGLES);
    for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
        glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
            objParser->textures[objParser->textureIdx[n] - 1].y);
        glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
            objParser->normal[objParser->normalIdx[n] - 1].y,
            objParser->normal[objParser->normalIdx[n] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
            objParser->vertices[objParser->vertexIdx[n] - 1].y,
            objParser->vertices[objParser->vertexIdx[n] - 1].z);

        glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
            objParser->textures[objParser->textureIdx[n + 1] - 1].y);
        glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
            objParser->normal[objParser->normalIdx[n + 1] - 1].y,
            objParser->normal[objParser->normalIdx[n + 1] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
            objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
            objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

        glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
            objParser->textures[objParser->textureIdx[n + 2] - 1].y);
        glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
            objParser->normal[objParser->normalIdx[n + 2] - 1].y,
            objParser->normal[objParser->normalIdx[n + 2] - 1].z);
        glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
            objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
            objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
    }
    glEnd();
    glEnable(GL_BLEND);
}

void draw_cube_textures()
{
    int size = 2;
    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    //Quad 1
    glNormal3f(1.0f, 0.0f, 0.0f);   //N1
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
  //Quad 2
    glNormal3f(0.0f, 0.0f, -1.0f);  //N2
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
  //Quad 3
    glNormal3f(-1.0f, 0.0f, 0.0f);  //N3
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
  //Quad 4
    glNormal3f(0.0f, 0.0f, 1.0f);   //N4
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
  //Quad 5
    glNormal3f(0.0f, 1.0f, 0.0f);   //N5
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
  //Quad 6
    glNormal3f(1.0f, -1.0f, 0.0f);  //N6
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
    glEnd();
}

void draw_textureCube() {
    glColor3f(1.0, 1.0, 1.0);	//

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(-1.0, 0, 0);	// -x axis
    glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1, 0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(1.0, 0, 0);	//x axis
    glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(0, 1); glVertex3f(1.0, 1.0, -1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(0, -1.0, 0);	// -y axis
    glTexCoord2f(0, 0); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, 1.0);
    glTexCoord2f(0, 1); glVertex3f(-1.0, -1.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(0, 1.0, 0);	// y axis
    glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1, 0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(1, 1); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, -1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1.0);	//z axis
    glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
    glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, 1.0);
    glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
    glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, 1.0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textureMonkey);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1.0);	//-z축
    glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, -1.0);
    glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, -1.0);
    glEnd();

    draw_axis();
}

void draw_colour() {
    glDisable(GL_TEXTURE_2D);
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-5, 5, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0, 0, 0.5);
    glBegin(GL_QUADS);
    glVertex2d(4.8, 3);
    glVertex2d(5, 3);
    glVertex2d(5, 5);
    glVertex2d(4.8, 5);
    glEnd();

    glColor3f(0.4, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2d(4.8, 1);
    glVertex2d(5, 1);
    glVertex2d(5, 3);
    glVertex2d(4.8, 3);
    glEnd();

    glColor3f(0.4, 0.6, 0.8);
    glBegin(GL_QUADS);
    glVertex2d(4.8, -1);
    glVertex2d(5, -1);
    glVertex2d(5, 1);
    glVertex2d(4.8, 1);
    glEnd();

    glColor3f(0.4, 0.7, 0.6);
    glBegin(GL_QUADS);
    glVertex2d(4.8, -3);
    glVertex2d(5, -3);
    glVertex2d(5, -1);
    glVertex2d(4.8, -1);
    glEnd();

    glColor3f(0.5, 0.4, 0);
    glBegin(GL_QUADS);
    glVertex2d(4.8, -5);
    glVertex2d(5, -5);
    glVertex2d(5, -3);
    glVertex2d(4.8, -3);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    glEnable(GL_TEXTURE_2D);
}

void draw_string(void* font, const char* str, float x_position, float y_position, float red, float green, float blue) {
    glDisable(GL_TEXTURE_2D);
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-5, 5, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(red, green, blue);
    glRasterPos3f(x_position, y_position, 0);
    for (unsigned int i = 0; i < strlen(str); i++) {
        glutBitmapCharacter(font, str[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    glEnable(GL_TEXTURE_2D);
}

/* Display callback function */
void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bgRgb[0], bgRgb[1], bgRgb[2], 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

    cam[0] = radius * sin(theta * M_PI / 180) * sin(phi * M_PI / 180);
    cam[1] = radius * cos(theta * M_PI / 180);
    cam[2] = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);

    gluLookAt(cam[0], cam[1], cam[2], center[0], center[1], center[2], up[0], up[1], up[2]);

    glDisable(GL_LIGHT1);
 
    if (cammode) {
        GLfloat light_position[] = { 0, 10, 0, 1 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }
    else {
        GLfloat light_position[] = { cam[0],cam[1],cam[2],1 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    //draw_axis();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glColor3f(1.f, 1.f, 1.f);
    glTranslatef(-1.5, 0.5, 0);

    draw_obj_with_texture(box, 1);
    draw_obj_with_texture(poll, 2);

    glPushMatrix();
    glRotatef(rot[0], 0, 0, 1);
    draw_obj_with_texture(roller, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, 2.3);
    glRotatef(rot[8], 0, 0, 1);
    if (emi[8]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger8
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, 2.3);
    glRotatef(rot[8], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle8
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, 1.85);
    glRotatef(rot[7], 0, 0, 1);
    if (emi[7]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger7
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, 1.85);
    glRotatef(rot[7], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle7
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, 1.4);
    glRotatef(rot[6], 0, 0, 1);
    if (emi[6]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger6
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, 1.4);
    glRotatef(rot[6], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle6
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, 0.95);
    glRotatef(rot[5], 0, 0, 1);
    if (emi[5]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger5
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, 0.95);
    glRotatef(rot[5], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle5
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, 0);
    glRotatef(rot[4], 0, 0, 1);
    if (emi[4]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger4
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, -0);
    glRotatef(rot[4], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle4
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, -0.45);
    glRotatef(rot[3], 0, 0, 1);
    if (emi[3]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger3
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, -0.45);
    glRotatef(rot[3], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle3
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, -0.9);
    glRotatef(rot[2], 0, 0, 1);
    if (emi[2]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger2
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, -0.9);
    glRotatef(rot[2], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle2
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4.1, -0.85, -1.35);
    glRotatef(rot[1], 0, 0, 1);
    if (emi[1]) {
        emission[0] = 1; emission[1] = 1; emission[2] = 1;
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    }
    draw_obj_with_texture(finger, 1); //finger1
    glPopMatrix();
    glPushMatrix();
    glTranslatef(1, -0.8, -1.35);
    glRotatef(rot[1], 0, 0, 1);
    draw_obj_with_texture(paddle, 1); //paddle1
    emission[0] = 0; emission[1] = 0; emission[2] = 0;
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glPopMatrix();


    draw_colour();
    draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -4, -4, 1, 1, 1);


    glutSwapBuffers();
    glFlush();
}

void mouse(int button, int state, int x, int y)
{
    if (state) {
        //printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
    }
    else {
        //printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
        if (x >= 1255) {
            if (y <= 144) {
                bgRgb[0] = 0; bgRgb[1] = 0; bgRgb[2] = 0.5;
            }
            else if (y < 288 && y>144) {
                bgRgb[0] = 0.4; bgRgb[1] = 0.2; bgRgb[2] = 0.2;
            }
            else if (y <= 432 && y > 288) {
                bgRgb[0] = 0.4; bgRgb[1] = 0.6; bgRgb[2] = 0.8;
            }
            else if (y <= 576 && y > 432) {
                bgRgb[0] = 0; bgRgb[1] = 0.7; bgRgb[2] = 0.6;
            }
            else if (y > 576) {
                bgRgb[0] = 0.5; bgRgb[1] = 0.4; bgRgb[2] = 0;
            }
        }
    }
    glutPostRedisplay();
}

void mouseWheelFunc(int wheel, int direction, int x, int y)
{
    if (direction > 0) {
        radius -= 0.7;
    }
    else if (direction < 0) {
        radius += 0.7;
    }
    glutPostRedisplay();
}


void passiveMotion(int x, int y)
{
    //printf("Mouse movement x, y = (%d, %d)\n", x, y);
    glutPostRedisplay();
}

void motion(int x, int y)
{
    //printf("Mouse movement x, y = (%d, %d)\n", x, y);
    glutPostRedisplay();
}

void main_menu(int option)
{
    if (option == 999) exit(0);
    else if (option == 11) {
        radius = 10;
        theta = 70; phi = 90; up[1] = 1;
        bgRgb[0] = 0; bgRgb[1] = 0; bgRgb[2] = 0.5;
        setTextureMapping();
    }
    glutPostRedisplay();
}

void sub_menu(int option)
{
    if (option == 0) {
        setTextureMapping();
    }
    if (option == 1) {
        setTextureMapping2();
    }
    if (option == 2) {
        setTextureMapping3();
    }
    glutPostRedisplay();
}


void idle()
{
    if (rolling) {
        rot[0] -= 0.9;
        if (rot[0] < 0)
            rot[0] += 360;

        for (int i = 1; i < 9; i++) {
            if (mode[i]) rot[i] -= 0.65;
            else    rot[i] += 0.65;
            if (rot[i] >= 60 || rot[i] <= 0) mode[i] = !mode[i];
        }
        if (GetTickCount() / 500 % 9 == 1) str = (char*)"T";
        else if (GetTickCount() / 500 % 9 == 2) str = (char*)"Ty";
        else if (GetTickCount() / 500 % 9 == 3) str = (char*)"Typ";
        else if (GetTickCount() / 500 % 9 == 4) str = (char*)"Typi";
        else if (GetTickCount() / 500 % 9 == 5) str = (char*)"Typin";
        else if (GetTickCount() / 500 % 9 == 6) str = (char*)"Typing";
        else if (GetTickCount() / 500 % 9 == 7) str = (char*)"Typing.";
        else if (GetTickCount() / 500 % 9 == 8) str = (char*)"Typing..";
        else if (GetTickCount() / 500 % 9 == 0) str = (char*)"Typing...";

        
    }
    else {
        str = (char*)" ";
    }

    glutPostRedisplay();
}

void resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluOrtho2D(0, 500, 0, 500);
    gluPerspective(60, (double)width / (double)height, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void get_roller(const char* str)
{
    roller = new ObjParser(str);
}


void get_box(const char* str)
{
    box = new ObjParser(str);
}

void get_paddle(const char* str)
{
    paddle = new ObjParser(str);
}

void get_finger(const char* str)
{
    finger = new ObjParser(str);
}

void get_poll(const char* str)
{
    poll = new ObjParser(str);
}

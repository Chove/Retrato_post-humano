#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
//#define USAR_PS3EYE  // si esta linea esta comentada va a compilar para usar la camara del macbook, sino usa la ps3eye

#ifdef USAR_PS3EYE
#include "ofxPS3EyeGrabber.h"
#endif
class boton : public ofRectangle{
public:
    boton(){}
    void setup(string path1, string pathPress){
        img.loadImage(path1);
        imgPress.loadImage(pathPress);
        width = defaultWidth = img.width;
        height = defaultHeight = img.height;
        
    }
    void draw(int xx, int yy, int w = 0, int h = 0){
        x = xx;
        y = yy;
        if (w ==0) {
            width = defaultWidth;
        }else{
            width=w;
        }
        if (h == 0) {
            height = defaultHeight;
        }else{
            height = h;
        }
        if ((this->inside(ofGetMouseX(), ofGetMouseY()) && ofGetMousePressed()) || bActivo){
            bActivo = true;
            imgPress.draw(*this);
        }else{
            img.draw(*this);
        }
    }
    ofImage img, imgPress;
    int defaultWidth, defaultHeight;
    bool bActivo;
};//cosas_pa_botones


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void exit();
    void setupCuadrados();
    void pantallazo();
    void resetRetratos();
    
	void keyPressed(int key);
    void setMinMax(int index, float val);
    void mouseReleased(int x, int y, int button);
    void keyReleased(int key);
#ifdef USAR_PS3EYE
	ofxPS3EyeGrabber cam;
#else
    ofVideoGrabber cam;
#endif

    ofxFaceTrackerThreaded tracker;
    
    float MOUTH_WIDTH, MOUTH_HEIGHT,LEFT_EYEBROW_HEIGHT, RIGHT_EYEBROW_HEIGHT, LEFT_EYE_OPENNESS, RIGHT_EYE_OPENNESS, JAW_OPENNESS, NOSTRIL_FLARE;
    float orientacionCabeza;
    float orienCabeza2;
    
    
    float mins [10];
    float maxs [10];
    
    bool bSuavizar;
    float factorSuavizado;
    
    bool bMostrarFaceTracker;
    int modo;
    ofSoundPlayer sound;
    
    
    ///trian_chicos
    ofImage trian1;
    ofImage trian2;
    ///trian_chicos
    
    //portada
    ofImage portada1;
    ofImage portada2;
    //portada
    
    ///cuad_chic
#define CUANTOS_CUAD_X 69 // cambia aca si quieres cambiar cuantos cuadrados hay en la horizontal
#define CUANTOS_CUAD_Y 43 // cambia aca si quieres cambiar cuantos cuadrados hay en la vertical
    ofColor colores[CUANTOS_CUAD_X][CUANTOS_CUAD_Y];
    int anchoCuadrado = 20;
    int altoCuadrado = 20;
    int margenCuadrados = 1;//cuantos pixeles separa un cuadrado y otro.
    ///cuad-chic
    
    ///poli chicos
    ofImage poli1;
    ofImage relleno1;
    ///poli chicos
    
    //circulos_retrato
    int elliX;
    int elliY;
    int elliW;
    int elliH;
    //circulos_retrato
    
    //cuad_retrato
    int rectX;
    int rectY;
    int rectW;
    int rectH;
    //cuad_retrato
    
    boton botonCirculo, botonCuadrado, botonCirculoChico, botonTriangulo, botonTriaguloChico, botonCuadradoChico, botonPoligon, botonPoligonoChico, botonInicio,botonRetrato, botonRetrato1,botonRetrato2,botonRetrato3,botonRetrato4, botonRetrato5;
   ofImage fondoBotones;
   ofImage fondoBoton2;
    
    
    int contador, contadorFrames;
    vector<float>anchosBoca;
    vector<ofColor>coloresRetrato;
    bool bFboDrawReady;
    
    vector<ofImage> imagenes;
    ofFbo fbo;
    bool bDrawToFbo;
    bool bGuardarCompImagenes;
	ofTexture videoTexture;
    
    ofTrueTypeFont font;
	
};

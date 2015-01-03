#include "testApp.h"
#define ANCHO 1440
#define ALTO 900
using namespace ofxCv;

//--------------------------------------------------------------
void testApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    ofSetDataPathRoot("../data/");
#ifdef USAR_PS3EYE
    cam.setDeviceID(0);
    cam.setDesiredFrameRate(60);
#endif
    cam.initGrabber(640,480);
#ifdef USAR_PS3EYE
    cam.setAutogain(true);
    cam.setAutoWhiteBalance(false);
#endif
    tracker.setup();
    bMostrarFaceTracker = false;
    for (int i = 0; i < 9; i++) {
        mins[i] = FLT_MAX;
        maxs[i] = FLT_MIN;
    }
    ofSetCircleResolution(100);
    modo = 0;
    
    sound.loadSound("synth.wav");//sound
    sound.setLoop(true);
    sound.play();

    
    setupCuadrados();

    ////imagenes
    ofImage("images/trian1.png");
    trian1.loadImage("images/trian1.png");
    ofImage("images/trian2.png");
    trian2.loadImage("images/trian2.png");
    ofImage("images/poligonos_fondo2.png");
    poli1.loadImage("images/poligonos_fondo2.png");
    ofImage("images/relleno2.png");
    relleno1.loadImage("images/relleno2.png");
    ////imagenes
    
    ////presentacion
    ofImage("images/portada1.png");
    portada1.loadImage("images/portada1.png");
    ofImage("images/portada2.png");
    portada2.loadImage("images/portada2.png");
    ofImage("images/b_fondo1.png");
    fondoBotones.loadImage("images/b_fondo1.png");
    ofImage("images/b_fondo2.png");
    fondoBoton2.loadImage("images/b_fondo2.png");
    ////presentacion
    
    ////botones
    botonCirculo.setup("images/b_circ1.png", "images/b_circ1_press.png");
    botonCuadrado.setup("images/b_cuad1.png","images/b_cuad1_press.png");
    botonCirculoChico.setup("images/b_circ2.png","images/b_circ2_press.png");
    botonTriangulo.setup("images/b_trian1.png","images/b_trian1_press.png");
    botonTriaguloChico.setup("images/b_trian2.png","images/b_trian2_press.png");
    botonCuadradoChico.setup("images/b_cuad2.png","images/b_cuad2_press.png");
    botonPoligon.setup("images/b_poli1.png","images/b_poli1_press.png");
    botonPoligonoChico.setup("images/b_poli2.png","images/b_poli2_press.png");
    botonInicio.setup("images/b_inicio.png","images/b_inicio.png");
    botonRetrato.setup("images/b_retrato1.png","images/b_retrato1.png");
    botonRetrato1.setup("images/fondo_retrato1.png","images/fondo_retrato1.png");
    botonRetrato2.setup("images/fondo_retrato2.png","images/fondo_retrato2.png");
    botonRetrato3.setup("images/fondo_retrato3.png","images/fondo_retrato3.png");
    botonRetrato4.setup("images/fondo_retrato4.png","images/fondo_retrato4.png");
    botonRetrato5.setup("images/fondo_retrato5.png","images/fondo_retrato5.png");
    ////botones
    

    bSuavizar = true;
    factorSuavizado = 0.3;
    
    //    ofHideCursor();  //para esconder el mouse en la aplicación
    //    ofShowCursor();
    
    contador = contadorFrames = 0;
    bDrawToFbo = bGuardarCompImagenes = bFboDrawReady = false;
    cout << "width: " << ANCHO << " height: " << ALTO << endl;
    
    ////tipografía
    font.loadFont("Consolas", 12, true, true);
    
}
//--------------------------------------------------------------
void testApp::setupCuadrados(){
    
    for (int i =0; i < CUANTOS_CUAD_Y; i++){
        for (int j =0; j < CUANTOS_CUAD_X; j++){
            float r = ofRandom(100);// este es un numero random entre 0 y 100. Cada valor que este numero puede tomar tiene igual probabilidad de ocurrir. Por lo tanto lo que hacemos es tomar tramos entre 0 y 100 para cada color que queremos, asi cambiamos la probabilidad que tiene cada color de aparecer.
            
            if(r >=0 && r <5){// 5 % de probabilidad de aparecer. 0 a 5
                colores[j][i].set(225,11,38);
            }else if(r >=5 && r <15){// 10 % de probabilidad de aparecer. 5 a 15
                colores[j][i].set(255,237,101);
            }else if(r >=15 && r <25){// 10 % de probabilidad de aparecer. 15 a 25
                colores[j][i].set(255,237,101);
            }else if(r >=25 && r <35){// 10 % de probabilidad de aparecer. 25 a 35
                colores[j][i].set(255,191,217);
            }else if(r >=35 && r <45){// 10 % de probabilidad de aparecer. 35 a 45
                colores[j][i].set(252,200,94);
            }else if(r >=45 && r <55){// 10 % de probabilidad de aparecer. 45 a 55
                colores[j][i].set(232,92,88);
            }else {// 45 % de probabilidad de aparecer. cualquiera que no cumple con los if anteriores, o sea de 55 a 100.
                colores[j][i].set(180,234,240);//celeste
            }}}
  
    
}
//--------------------------------------------------------------
void testApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
#ifdef USAR_PS3EYE
        cv::Mat cm = toCv(cam.getPixelsRef());
        cv::Mat rgb;
        cv::cvtColor(cm, rgb, CV_RGBA2GRAY);
        tracker.update(rgb);
#else
        tracker.update(toCv(cam));
#endif
        videoTexture.loadData(cam.getPixelsRef());
        if(tracker.getFound()) {
            
            /* opciones de movimientos del rostro
             MOUTH_WIDTH, MOUTH_HEIGHT,LEFT_EYEBROW_HEIGHT, RIGHT_EYEBROW_HEIGHT, LEFT_EYE_OPENNESS, RIGHT_EYE_OPENNESS, JAW_OPENNESS, NOSTRIL_FLARE,orientación_cabeza
             */
            
            if (bSuavizar) {//// suavisado del movimiento
                MOUTH_HEIGHT = MOUTH_HEIGHT *factorSuavizado + (tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT) * (1-factorSuavizado));
                MOUTH_WIDTH = MOUTH_WIDTH *factorSuavizado + (tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH) * (1-factorSuavizado));
                LEFT_EYE_OPENNESS =  LEFT_EYE_OPENNESS *factorSuavizado + (tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS) * (1-factorSuavizado));
                LEFT_EYEBROW_HEIGHT =  LEFT_EYEBROW_HEIGHT *factorSuavizado + (tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT) * (1-factorSuavizado));
                RIGHT_EYE_OPENNESS =  RIGHT_EYE_OPENNESS *factorSuavizado + (tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS) * (1-factorSuavizado));
                RIGHT_EYEBROW_HEIGHT = RIGHT_EYEBROW_HEIGHT *factorSuavizado + (tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT) * (1-factorSuavizado));
                JAW_OPENNESS =  JAW_OPENNESS *factorSuavizado + (tracker.getGesture(ofxFaceTracker::JAW_OPENNESS) * (1-factorSuavizado));
                NOSTRIL_FLARE =  NOSTRIL_FLARE *factorSuavizado + (tracker.getGesture(ofxFaceTracker::NOSTRIL_FLARE) * (1-factorSuavizado));
                orientacionCabeza =  orientacionCabeza *factorSuavizado + (tracker.getOrientation().y * (1-factorSuavizado));
                
                orienCabeza2 =  orienCabeza2 *factorSuavizado + (tracker.getOrientation().x * (1-factorSuavizado));
                
                
            }else{
                MOUTH_HEIGHT = tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT);//llamando al gesto
                MOUTH_WIDTH = tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH);
                LEFT_EYE_OPENNESS = tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS);
                LEFT_EYEBROW_HEIGHT = tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT);
                RIGHT_EYE_OPENNESS = tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS);
                RIGHT_EYEBROW_HEIGHT = tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT);
                JAW_OPENNESS = tracker.getGesture(ofxFaceTracker::JAW_OPENNESS);
                NOSTRIL_FLARE = tracker.getGesture(ofxFaceTracker::NOSTRIL_FLARE);
                orientacionCabeza = tracker.getOrientation().y;
                orienCabeza2 = tracker.getOrientation().x;
                
                
            }
            setMinMax(0, MOUTH_HEIGHT);
            setMinMax(1, MOUTH_WIDTH);
            setMinMax(2, LEFT_EYE_OPENNESS);
            setMinMax(3, LEFT_EYEBROW_HEIGHT);
            setMinMax(4, RIGHT_EYE_OPENNESS);
            setMinMax(5, RIGHT_EYEBROW_HEIGHT);
            setMinMax(6, JAW_OPENNESS);
            setMinMax(7, NOSTRIL_FLARE);
            setMinMax(8, orientacionCabeza);
            setMinMax(9, orienCabeza2);
            
            sound.setSpeed(ofMap(orientacionCabeza, mins[8], maxs[8], -3, 3));
        }
    }
}

//--------------------------------------------------------------
void testApp::setMinMax(int index, float val){
    if(mins[index] > val){
        mins[index] = val;
    }
    if(maxs[index] < val){
        maxs[index] = val;
    }
}
//--------------------------------------------------------------
void testApp::pantallazo(){
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    ofClear(255,0);
    fbo.end();
    bDrawToFbo = true;
    bFboDrawReady = false;
}
//--------------------------------------------------------------
void testApp::draw() {
    
    ofSetColor(255);
    ofPushMatrix();
    ofScale(ofGetWidth()/1440.0f, ofGetHeight()/900.0f);
    if(bMostrarFaceTracker){
        videoTexture.draw(0, 0);
        tracker.draw();
        string str = "";
        
        //numeros en pantalla. transformar los numeros en texto para poner en pantalla
        str += "MOUTH_WIDTH: " + ofToString(MOUTH_WIDTH) +"\n";
        str += "MOUTH_HEIGHT: " + ofToString(MOUTH_HEIGHT)+"\n";
        str += "LEFT_EYEBROW_HEIGHT: " + ofToString(LEFT_EYEBROW_HEIGHT)+"\n";
        str += "RIGHT_EYEBROW_HEIGHT: " + ofToString(RIGHT_EYEBROW_HEIGHT)+"\n";
        str += "LEFT_EYE_OPENNESS: " + ofToString(LEFT_EYE_OPENNESS)+"\n";
        str += "RIGHT_EYE_OPENNESS: " + ofToString(RIGHT_EYE_OPENNESS)+"\n";
        str += "JAW_OPENNESS: " + ofToString(JAW_OPENNESS)+"\n";
        str += "NOSTRIL_FLARE: " + ofToString(NOSTRIL_FLARE)+"\n";
        str += "Orientacion: " + ofToString(tracker.getOrientation());
        
        font.drawString(str, 100, 100);
        font.drawString(ofToString((int) ofGetFrameRate()), 10, ALTO - 20);
    }
    
    else{
        
        botonCirculo.bActivo = false;
        botonCuadrado.bActivo = false;
        botonCirculoChico.bActivo = false;
        botonTriangulo.bActivo = false;
        botonTriaguloChico.bActivo = false;
        botonCuadradoChico.bActivo = false;
        botonPoligon.bActivo = false;
        botonPoligonoChico.bActivo = false;
        botonInicio.bActivo = false;
        botonRetrato.bActivo = false;
        
        
        if(modo == 0){
            
            botonCirculo.bActivo = true;
          
            
            ofBackground(255);
            float diam = ofMap(MOUTH_HEIGHT, mins[0], maxs[0], 80, 160);//alto_boca
            float ojoL = ofMap(MOUTH_WIDTH, mins[1], maxs[1], 0, 255);//ancho_boca
            ofColor col;
            col.ofColor::setHsb(ojoL, 255, 255);
            ofSetColor(col);// se utiliza la apertura del ojo para el cambio de color
            
            ofCircle(300, 250, diam);
            ofCircle(300, 450, diam);
            ofCircle(300, 650, diam);
            ofCircle(500, 450, diam);
            ofCircle(500, 250, diam);
            ofCircle(500, 650, diam);
            ofCircle(700, 250, diam);
            ofCircle(700, 450, diam);
            ofCircle(700, 650, diam);
            ofCircle(900, 450, diam);
            ofCircle(900, 250, diam);
            ofCircle(900, 650, diam);
            ofCircle(1100, 450, diam);
            ofCircle(1100, 250, diam);
            ofCircle(1100, 650, diam);// circulos_mayores
            
            ofSetColor(195,208,72);
            
            ofCircle(300, 250, 60);
            ofCircle(300, 450, 60);
            ofCircle(300, 650, 60);
            ofCircle(500, 450, 60);
            ofCircle(500, 250, 60);
            ofCircle(500, 650, 60);
            ofCircle(700, 250, 60);
            ofCircle(700, 450, 60);
            ofCircle(700, 650, 60);
            ofCircle(900, 450, 60);
            ofCircle(900, 250, 60);
            ofCircle(900, 650, 60);
            ofCircle(1100, 450, 60);
            ofCircle(1100, 250, 60);
            ofCircle(1100, 650, 60);// circulos_medianos
            
            ofSetColor(ojoL,200,200);
            
            ofCircle(300, 250, 30);
            ofCircle(300, 450, 30);
            ofCircle(300, 650, 30);
            ofCircle(500, 450, 30);
            ofCircle(500, 250, 30);
            ofCircle(500, 650, 30);
            ofCircle(700, 250, 30);
            ofCircle(700, 450, 30);
            ofCircle(700, 650, 30);
            ofCircle(900, 450, 30);
            ofCircle(900, 250, 30);
            ofCircle(900, 650, 30);
            ofCircle(1100, 450, 30);
            ofCircle(1100, 250, 30);
            ofCircle(1100, 650, 30);// circulos_menores
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            

            ofSetColor(120,120,120);
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 350, 780);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
            
        }//////////////////////////////////////////////////////circulos_gran(boca)
        
        else if(modo == 1){
            
            botonCirculoChico.bActivo = true;
            ofBackground(255,255,255);
            
            float ojoL = ofMap(MOUTH_WIDTH, mins[1], maxs[1], 0, 255);//ancho_boca
            
            ofColor col;
            col.ofColor::setHsb(ojoL, 255, 255);
            
            ofSetColor(col);// se utiliza la apertura del ojo para el cambio de color
            
            float radio = ofMap(MOUTH_HEIGHT, mins[0], maxs[0], 9, 30);//alto_boca_maximo y minimo pelotas
            
            for(int i=0; i<=ANCHO; i+= radio*2+ 5){ // distancia entre pelotas
                for(int j=0; j<=ALTO; j+= radio*2 +5){
                    ofCircle(i, j, radio);
                    
                }}
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(120,120,120);
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 450, 780);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
            
        }////////////////////////////////////////////////circulos chic(boca)
        
        else if(modo == 2){
            botonTriangulo.bActivo = true;
            float miExis = ofMap(NOSTRIL_FLARE, mins[7], maxs[7], 250, 490);//movimiento_nariz-trian-rosados
            float miYgriega = ofMap(NOSTRIL_FLARE, mins[7], maxs[7], 230, 470);//movimiento_nariz-trian-azules_abajo
            float ojoL = ofMap(NOSTRIL_FLARE, mins[7], maxs[7], 0, 255);
            
            ofColor col;
            col.ofColor::setHsb(ojoL, 255, 255);
            
            ofBackground(ojoL,200, 200);
            
            ofSetColor(col);// se utiliza la apertura del ojo para el cambio de color
            ofTriangle(650,280,700,200,750,280);
            ofTriangle(650,750,700,670,750,750);
            
            ofSetColor(226,28,182);
            ofTriangle(miExis,400, miExis+100,400-160,miExis+200,400);//rosados_arriba
            ofTriangle(miExis+200,400, miExis+300,400-160,miExis+400,400);
            ofTriangle(miExis+400,400, miExis+500,400-160,miExis+600,400);
            
            ofTriangle(miExis,710, miExis+100,550,miExis+200,710);//rosados_abajo
            ofTriangle(miExis+200,710, miExis+300,550,miExis+400,710);
            ofTriangle(miExis+400,710, miExis+500,550,miExis+600,710);
            
            ofSetColor(121,167,30);
            ofTriangle(400,400, 450,320,500,400);//verde_arriba
            ofTriangle(500,400, 550,320,600,400);
            ofTriangle(600,400, 650,320,700,400);
            ofTriangle(700,400, 750,320,800,400);
            ofTriangle(800,400, 850,320,900,400);
            ofTriangle(900,400, 950,320,1000,400);
            
            ofSetColor(121,167,30);
            ofTriangle(400,miYgriega+150, 450,miYgriega+230,500,miYgriega+150);//verde_abajo
            ofTriangle(500,miYgriega+150, 550,miYgriega+230,600,miYgriega+150);
            ofTriangle(600,miYgriega+150, 650,miYgriega+230,700,miYgriega+150);
            ofTriangle(700,miYgriega+150, 750,miYgriega+230,800,miYgriega+150);
            ofTriangle(800,miYgriega+150, 850,miYgriega+230,900,miYgriega+150);
            ofTriangle(900,miYgriega+150, 950,miYgriega+230,1000,miYgriega+150);
            
            ofSetColor(col);// se utiliza la apertura del ojo para el cambio de color
            ofTriangle(650,280,700,200,750,280); //medianos
            ofTriangle(650,750,700,670,750,750);
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(120,120,120);
            
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 750, 780);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
        }////////////////////////////////////////////////trian gran(nariz)
        
        else if(modo == 3){
            
            
            ofBackground(11,36,251);
            float anchoTrian = ofMap(NOSTRIL_FLARE, mins[7], maxs[7], 350, 290);
            float altoTrian = ofMap(NOSTRIL_FLARE, mins[7] , maxs[7], 290, 200);
            //altoTrian=250;
            
            trian1.draw(-anchoTrian,0);//arriba
            trian2.draw(-anchoTrian/2,0);
            trian1.draw(0,0);
            trian2.draw(anchoTrian/2,0);
            trian1.draw(anchoTrian,0);
            trian2.draw(anchoTrian + 154,0);
            trian1.draw(anchoTrian*2,0);
            trian2.draw(anchoTrian*2+154,0);
            trian1.draw(anchoTrian*3,0);
            trian2.draw(anchoTrian*3+154,0);
            trian1.draw(anchoTrian*4,0);
            trian2.draw(anchoTrian*4+154,0);
            trian1.draw(anchoTrian*5,0);
            
            trian2.draw(-anchoTrian,altoTrian);//al medio
            trian1.draw(-anchoTrian/2,altoTrian);
            trian2.draw(0,altoTrian);
            trian1.draw(anchoTrian/2,altoTrian);
            trian2.draw(anchoTrian,altoTrian);
            trian1.draw(anchoTrian+ 154,altoTrian);
            trian2.draw(anchoTrian*2,altoTrian);
            trian1.draw(anchoTrian*2 + 154,altoTrian);
            trian2.draw(anchoTrian*3,altoTrian);
            trian1.draw(anchoTrian*3 + 154,altoTrian);
            trian2.draw(anchoTrian*4,altoTrian);
            trian1.draw(anchoTrian*4 + 154,altoTrian);
            trian2.draw(anchoTrian*5,altoTrian);
            
            trian1.draw(-anchoTrian,altoTrian*2);//abajo
            trian2.draw(-anchoTrian/2,altoTrian*2);
            trian1.draw(0,altoTrian*2);
            trian2.draw(anchoTrian/2,altoTrian*2);
            trian1.draw(anchoTrian,altoTrian*2);
            trian2.draw(anchoTrian + 154,altoTrian*2);
            trian1.draw(anchoTrian*2,altoTrian*2);
            trian2.draw(anchoTrian*2 + 154,altoTrian*2);
            trian1.draw(anchoTrian*3,altoTrian*2);
            trian2.draw(anchoTrian*3 + 154,altoTrian*2);
            trian1.draw(anchoTrian*4,altoTrian*2);
            trian2.draw(anchoTrian*4 + 154,altoTrian*2);
            trian1.draw(anchoTrian*5,altoTrian*2);
            
            trian2.draw(-anchoTrian,altoTrian*3);//más abajo
            trian1.draw(-anchoTrian/2,altoTrian*3);
            trian2.draw(0,altoTrian*3);
            trian1.draw(anchoTrian/2,altoTrian*3);
            trian2.draw(anchoTrian,altoTrian*3);
            trian1.draw(anchoTrian+ 154,altoTrian*3);
            trian2.draw(anchoTrian*2,altoTrian*3);
            trian1.draw(anchoTrian*2 + 154,altoTrian*3);
            trian2.draw(anchoTrian*3,altoTrian*3);
            trian1.draw(anchoTrian*3 + 154,altoTrian*3);
            trian2.draw(anchoTrian*4,altoTrian*3);
            trian1.draw(anchoTrian*4 + 154,altoTrian*3);
            trian2.draw(anchoTrian*5,altoTrian*3);
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(255);
            
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 850, 780);
            
            
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
            
            
        }////////////////////////////////////////////////trian chic (nariz)
        
        else if(modo == 4){
            
            float lugarCuad = ofMap(LEFT_EYEBROW_HEIGHT, mins[3], maxs[3], 1.4, 1);
            float altoCuad = ofMap(RIGHT_EYE_OPENNESS, mins[4], maxs[4], 7, 2);
            
            botonCuadrado.bActivo = true;
            ofBackground(180,234,240);
            
            float arista = ofMap(ALTO,0, ANCHO,150,150); //formacion de cuadrados
            for(int i=200; i<=1100; i+= arista*lugarCuad){ // distancia entre cuadrados
                for(int j=200; j<=600; j+= arista*lugarCuad){
                    
                    ofSetColor(252,200,94);
                    ofRect(i, j, arista, arista);
                    
                    ofSetColor(246,168,171);
                    ofRect(i+25, j+25, arista/2, arista/2);
                    
                    ofSetColor(180,234,240);
                    
                    ofRect(i+25, j+25, arista/altoCuad, arista/altoCuad);//chicos
                    
                }}
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(120,120,120);
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 550, 780);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);

            
        }////////////////////////////////////////////////cuad gran (ojos)
        
        else if(modo == 5){
            
            float lugarCuad = ofMap(LEFT_EYEBROW_HEIGHT, mins[3], maxs[3], 7, 120);
            
            botonCuadradoChico.bActivo = true;
            ofBackground(179,254,121);
            
            for (int i = 0; i < CUANTOS_CUAD_Y; i++){
            for (int j =0; j < CUANTOS_CUAD_X; j++){
                ofSetColor(colores[j][i]);
                ofRect(j * (anchoCuadrado + margenCuadrados), i * (altoCuadrado + margenCuadrados), anchoCuadrado, altoCuadrado);
                }}
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(255,237,101);
            ofRect(100,100,ANCHO-200,lugarCuad);
            ofRect(100,295,ANCHO-200,lugarCuad);
            ofRect(100,490,ANCHO-200,lugarCuad);
            ofRect(100,685,ANCHO-200,lugarCuad);
           
            ofSetColor(120,120,120);
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 650, 780);
            
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
            
        }////////////////////////////////////////////////cuad chic(ojos)
        
        else if(modo == 6){
            botonPoligon.bActivo = true;
            ofBackground(22,18,19);
            
            ofSetColor(0xEEEEEE);
            
            float ojoL = ofMap(orienCabeza2, mins[9], maxs[9], 0, 255);
            ofColor col;
            col.ofColor::setHsb(ojoL, 200, 200);
            ofSetColor(col);
            // se utiliza la apertura de la boca para el cambio de color
            
            ofBeginShape(); //comenzar forma polígono
            
            //set number of sides based on mouth with
            int numSteps = ofMap(orientacionCabeza, mins[8], maxs[8], 3, 14);
            
            //make sure we don't go bellow 3 sides
            numSteps = MAX(3, numSteps);
            
            float step		= TWO_PI / (numSteps);
            float angle		= 0.0;
            float cenX		= ANCHO/2;  //centrox
            float cenY		= ALTO/2;  //centroy
            float radius	= 110;  //radio poligono
            
            for(int i = 0; i < numSteps; i++){
                float rx = cenX + cos(angle) * radius;
                float ry = cenY + sin(angle) * radius;
                
                ofVertex(rx, ry);
                angle += step;
            }
            ofEndShape(true);
            
            
            glLineWidth(20); //20 = 5 pixeles de ancho
            
            ofSetColor(252,28,173);
            ofLine(0, 50, ANCHO-200,50); //arriba
            ofLine(ANCHO-205,50,ANCHO-205,ALTO);
            ofSetColor(254,116,34);//naranjo
            ofLine(0, 50+30, ANCHO-200-30,50+30);
            ofLine(ANCHO-205-30,50+30,ANCHO-205-30,ALTO);
            ofSetColor(208,213,217);//gris
            ofLine(0, 50+30*2, ANCHO-200-30*2,50+30*2);
            ofLine(ANCHO-205-30*2,50+30*2,ANCHO-205-30*2,ALTO);
            ofSetColor(255,237,53);//amarillo
            ofLine(0, 50+30*3, ANCHO-200-30*3,50+30*3);
            ofLine(ANCHO-205-30*3,50+30*3,ANCHO-205-30*3,ALTO);
            ofSetColor(252,28,173);
            ofLine(0, 50+30*4, ANCHO-200-30*4,50+30*4);
            ofLine(ANCHO-205-30*4,50+30*4,ANCHO-205-30*4,ALTO);
            ofSetColor(24,139,152);//azul
            ofLine(0, 50+30*5, ANCHO-200-30*5,50+30*5);
            ofLine(ANCHO-205-30*5,50+30*5,ANCHO-205-30*5,ALTO);
            ofSetColor(224,14,73);//rojo
            ofLine(0, 50+30*6, ANCHO-200-30*6,50+30*6);
            ofLine(ANCHO-205-30*6,50+30*6,ANCHO-205-30*6,ALTO);
            
            ofLine(201, ALTO-50, ANCHO,ALTO-50); //abajo
            ofLine(205, ALTO-50, 205,0);
            ofSetColor(24,139,152);//azul
            ofLine(200+31, ALTO-50-30, ANCHO,ALTO-50-30);
            ofLine(205+30, ALTO-50-30, 205+30,0);
            ofSetColor(252,28,173);
            ofLine(201+30*2, ALTO-50-30*2, ANCHO,ALTO-50-30*2);
            ofLine(205+30*2, ALTO-50-30*2, 205+30*2,0);
            ofSetColor(255,237,53);//amarillo
            ofLine(201+30*3, ALTO-50-30*3, ANCHO,ALTO-50-30*3);
            ofLine(205+30*3, ALTO-50-30*3, 205+30*3,0);
            ofSetColor(208,213,217);//gris
            ofLine(201+30*4, ALTO-50-30*4, ANCHO,ALTO-50-30*4);
            ofLine(205+30*4, ALTO-50-30*4, 205+30*4,0);
            ofSetColor(254,116,34);//naranjo
            ofLine(201+30*5, ALTO-50-30*5, ANCHO,ALTO-50-30*5);
            ofLine(205+30*5, ALTO-50-30*5, 205+30*5,0);
            ofSetColor(252,28,173);
            ofLine(201+30*6, ALTO-50-30*6, ANCHO,ALTO-50-30*6);
            ofLine(205+30*6, ALTO-50-30*6, 205+30*6,0);
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(255);
            
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 950, 780);
            
            ofSetColor(255);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
        }////////////////////////////////////////////////hexagono_gran(cara)
        
        else if(modo == 7){
            
            botonPoligonoChico.bActivo = true;
            
            float avance = ofMap(orientacionCabeza, mins[8], maxs[8], 200, -200);
            float avancE = ofMap(orientacionCabeza, mins[8], maxs[8], -50, 50);
            
            ofBackground(22,18,19);
            
            poli1.draw(0+avancE,0);
            poli1.draw(-1375+avancE,0);
            poli1.draw(1375+avancE,0);
            
            relleno1.draw(0+avance,0);
            relleno1.draw(-1375+avance,0);
            relleno1.draw(1375+avance,0);
            
            ofSetColor(255);
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
            
            ofSetColor(255);
            
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"jugar";
            font.drawString(str, 1050, 780);
            
            ofSetColor(255);
            string str2 = "";
            str2=+"retrato";
            font.drawString(str2, 1300, 780);
            
        }////////////////////////////////////////////////hexagono_chic(cara)
        
        else if(modo >= 8 && modo <= 11){
            ofColor colorOne;
            ofColor colorTwo;
            colorOne.set(15,94,203);
            colorTwo.set(231,232,233);
            
            ofBackgroundGradient(colorOne,colorTwo,OF_GRADIENT_LINEAR);
            
            float radioMin = 10;
            float radioMax = 70;
            float margenCirculos = 80;
            float variable;
            
            ///////////////////////////// movimiento para cada forma del retrato
            if (modo == 8) {
                variable = ofMap(MOUTH_WIDTH, mins[1], maxs[1], radioMin, radioMax);//circ-ancho_boca_retrato
            }else if (modo == 9) {
                variable = ofMap(RIGHT_EYE_OPENNESS, mins[4], maxs[4], radioMin, radioMax);//cuad_ojos_retrato
            }else if (modo == 10) {
                variable = ofMap(NOSTRIL_FLARE, mins[7], maxs[7], radioMin, radioMax);//trian_nariz_retrato
            }else if (modo == 11) {
                variable = ofMap(orienCabeza2, mins[9], maxs[9], radioMin, radioMax);//poli_cabeza_retrato
                
            }
            
            //////////////////////////////////
            
            int nx = ANCHO / (margenCirculos);
            int frames = 4; //velocidad de avance
            contadorFrames++;
           
            float ojoL = ofMap(orientacionCabeza, mins[8], maxs[8], 0, 255); //variacion de color
            ofColor col;
            col.ofColor::setHsb(ojoL, 255, 255,120);
            
            if (contadorFrames > frames) {
                contadorFrames = 0;
                anchosBoca.push_back(variable);
                coloresRetrato.push_back(col);
                contador++;
            }
            if (bDrawToFbo) {
                fbo.begin();
            }
            ofPushStyle();
            
            
            
            ofFill();
            float sn60 = sin(DEG_TO_RAD*60);
            float cs60 = cos(DEG_TO_RAD*60);
            for(int i = 0; i < contador && i < anchosBoca.size(); i++){
                
                ////////formas de registro para el retrato(cada MODO)
                ofSetColor(coloresRetrato[i]);
                if (modo == 8) {
                    ofCircle(radioMax + (margenCirculos)*(i%nx) , radioMax + (margenCirculos)*floor(i/float(nx)), anchosBoca[i]);
                
                }//circulo
                
                else if(modo == 9){
                    ofRect((margenCirculos)*(i%nx) , (margenCirculos)*floor(i/float(nx)), anchosBoca[i], anchosBoca[i]);
                    
                }//cuadrado
                
                else if(modo == 10){
                    ofPoint centro (radioMax + (margenCirculos)*(i%nx) , radioMax + (margenCirculos)*floor(i/float(nx)));
                    float r = anchosBoca[i];
                    ofTriangle(centro.x, centro.y +r , centro.x - sn60*r , centro.y -cs60*r, centro.x +sn60*r , centro.y -cs60*r);
                    
                }//triangulo
                
                else if(modo == 11){
                    ofBeginShape();
                    int numSteps = 6;
                    
                    float step		= TWO_PI / (numSteps);
                    float angle		= 0.0;
					//float cenX		= radioMax;//ANCHO/2;  //centrox
                    //float cenY		= radioMax;//ALTO/2;  //centroy
					ofPoint centro (radioMax + (margenCirculos)*(i%nx) , radioMax + (margenCirculos)*floor(i/float(nx)));
					
                    for(int j = 0; j < numSteps; j++){
                    	float rx =  cos(angle) * anchosBoca[i];
                    	float ry =  sin(angle) * anchosBoca[i];
                    
                    	ofVertex(rx + centro.x, ry + centro.y);
                    	angle += step;
                    }
                    ofEndShape(true);
                }//hexagono
                
                /////////////////////////////
            }
            ofPopStyle();
            if (bDrawToFbo) {
                fbo.end();
                fbo.draw(0, 0);
                imagenes.push_back(ofImage());
                fbo.readToPixels(imagenes.back().getPixelsRef());
                imagenes.back().update();
                resetRetratos();
                bFboDrawReady = true;
                //modo++;
                
                //bDrawToFbo = false;
            }
            
            if (floor(contador/nx) > ALTO/float(margenCirculos)) {
                //resetRetratos();
                pantallazo();
                //modo++;
            }
            
            ofSetColor(120,120,120);
            
            //////////////pa escribir en la pantalla!!!
            string str = "";
            str =+"retrato";
            font.drawString(str, 415, 780);
            

        }
        else if(modo == 12){
            ofColor colorOne;
            ofColor colorTwo;
            float ojoL = ofMap(orientacionCabeza, mins[8], maxs[8], 210, 255);
            float ojoL2 = ofMap(orientacionCabeza, mins[8], maxs[8], 130, 170);
            
            colorOne.set(ojoL,14,73);
            colorTwo.set(24,139,ojoL2);
            ofBackgroundGradient(colorOne,colorTwo,OF_GRADIENT_LINEAR);
            ofEnableAlphaBlending();
            ofSetColor(255);
            
            for (int i = 0; i < imagenes.size(); i++) {
                imagenes[i].draw(0, 0);
            }
            
        }////////////////////////////////////////////////suma_retrato
        
        
        else if(modo == 13) {
            
            ofBackground(255);
            
            portada1.draw (600,600);
            portada2.draw(-300,0);
            
        }///// portada
        
        else if(modo == 14){
            //pantalla_facetracker_arriba
            ofPushMatrix();
            ofPushStyle();
            ofSetColor(255);
            ofSetLineWidth(1);
            //ofTranslate(ANCHO-300, ALTO - 200);
            ofScale(ANCHO/640.0, ALTO/480.0);
            videoTexture.draw(0, 0);
            tracker.getImageMesh().drawWireframe();
            ofPopStyle();
            ofPopMatrix();
            
            fondoBotones.draw(320,ALTO-110);
            fondoBoton2.draw(1280,ALTO-110);
        } //////////////facetracker en grande
        
        ofSetColor(255);
        

        
        //ubicación_botones_en_pantalla
        if((modo>=0 && modo < 8 ) || modo == 14 ){
            botonCirculo.draw(345, ALTO - 80, 50, 50);
            botonCirculoChico.draw(445, ALTO - 80, 50, 50);
            botonCuadrado.draw(545, ALTO - 80, 50, 50);
            botonCuadradoChico.draw(645, ALTO -80, 50, 50);
            botonTriangulo.draw(745, ALTO - 80, 50, 50);
            botonTriaguloChico.draw(845, ALTO - 80, 50, 50);
            botonPoligon.draw(945, ALTO - 80, 50, 50);
            botonPoligonoChico.draw(1045, ALTO - 80, 50, 50);
            botonRetrato.draw(1300, ALTO-90, 70, 65);
        }else if(modo==8) {
            botonRetrato1.draw(420, ALTO - 90, 600, 90);//boton_retrato1
            
        }else if(modo==9) {
            botonRetrato2.draw(420, ALTO - 90, 600, 90);//boton_retrato2
        }
        else if(modo==10) {
            botonRetrato3.draw(420, ALTO - 90, 600, 90);//boton_retrato3
        }
        else if(modo==11) {
            botonRetrato4.draw(420, ALTO - 90, 600, 90);//boton_retrato4
        }
        else if(modo==12) {
           // botonRetrato5.draw(420, ALTO - 90, 600, 90);//boton_retrato4
        }
        
        ofSetColor(120,120,120);
        string str3 = "";
        str3=+"inicio";
        font.drawString(str3, 62, 780);
        ofSetColor(255);
        botonInicio.draw(60, ALTO-90, 70, 70); //boton_inicio
        
        if(modo >=0 && modo < 13){
            
            //pantalla_facetracker
            ofPushMatrix();
            ofPushStyle();
            ofSetColor(255);
            ofSetLineWidth(1);
            ofTranslate(ANCHO- ANCHO/2 + 120, ALTO-920);//lugar...probar
            ofScale(-300/640.0, 200/480.0);
            videoTexture.draw(0, 0);
            tracker.getImageMesh().drawWireframe();
            
            //colorear_zona_que_se_mueve_del_rostro
            if(modo==0 || modo == 1){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::OUTER_MOUTH).draw();
                
            }if(modo==2 || modo == 3){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::NOSE_BRIDGE).draw();
                tracker.getImageFeature(ofxFaceTracker::NOSE_BASE).draw();
            }else if(modo==4 || modo == 5){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).draw();
                tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).draw();
                tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW).draw();
                tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW).draw();
            }else if(modo==6 || modo == 7){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE).draw();
            }else if(modo==8){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::OUTER_MOUTH).draw();
            }else if(modo==9){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::LEFT_EYE).draw();
                tracker.getImageFeature(ofxFaceTracker::RIGHT_EYE).draw();
                tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW).draw();
                tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW).draw();
            }else if(modo==10){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
                tracker.getImageFeature(ofxFaceTracker::NOSE_BRIDGE).draw();
                tracker.getImageFeature(ofxFaceTracker::NOSE_BASE).draw();
            }else if(modo==11){
                ofSetColor(13,255,0);
                ofSetLineWidth(3);
            tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE).draw();
            }
            //////////////////
            
            ofPopStyle();
            ofPopMatrix();
            if (modo == 12) {
                if (bGuardarCompImagenes) {
                    bGuardarCompImagenes = false;
                    ofSaveScreen(ofToDataPath(ofGetTimestampString()+".jpg"));
                }
            }
        }
        if (bDrawToFbo && bFboDrawReady) {
            bDrawToFbo = false;
            bFboDrawReady = false;
            
            modo++;
            if (modo == 12) {
                bGuardarCompImagenes = true;
            }
        }
        
    }
    ofPopMatrix();
}
//--------------------------------------------------------------
void testApp::resetRetratos(){
    anchosBoca.clear();
    coloresRetrato.clear();
    contador=0;
}
//--------------------------------------------------------------
void testApp::exit(){
    tracker.waitForThread();
}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    ofPoint escala (1440.0f/ofGetWidth(),900.0f/ofGetHeight());
    x*=escala.x;
    y*=escala.y;
    
    ///////////////////definir_botones:_donde_lleva_cada_uno
    //primera_parte
    if((modo>=0 && modo < 8 ) || modo == 14 ){
        if(botonCirculo.inside(x, y)){
            modo = 0;
        }
        if(botonCirculoChico.inside(x, y)){
            modo = 1;
        }
        if(botonTriangulo.inside(x, y)){
            modo = 2;
        }
        if(botonTriaguloChico.inside(x, y)){
            modo = 3;
        }
        if(botonCuadrado.inside(x, y)){
            modo = 4;
        }
        if(botonCuadradoChico.inside(x, y)){
            modo = 5;
        }
        if(botonPoligon.inside(x, y)){
            modo = 6;
        }
        if(botonPoligonoChico.inside(x, y)){
            modo = 7;
        }
        if(botonRetrato.inside(x, y)){//retrato
            modo = 8;
        }
        
        //segunda_parte_botones_retrato
    }else if(modo==8) {
        if(botonRetrato1.inside(x, y)){
            //modo = 9;
            pantallazo();
            //resetRetratos();
        }
    }else if(modo==9) {
        if(botonRetrato2.inside(x, y)){
            //modo = 10;
            pantallazo();
            //resetRetratos();
        }
    }
    else if(modo==10) {
        if(botonRetrato3.inside(x, y)){
            //modo = 11;
            pantallazo();
            //resetRetratos();
        }
    }
    else if(modo==11) {
    if(botonRetrato4.inside(x, y)){
        //modo = 12;
         pantallazo();
        //resetRetratos();
   }
    } else if(modo==13) {
        modo = 14;
    }
    if(botonInicio.inside(x, y)){//define el modo inicio
        modo = 13;
        imagenes.clear();
    }
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
    if(key == 'r') {
        tracker.reset();
    }
    if (key == ' ') {
        bMostrarFaceTracker = !bMostrarFaceTracker;
    }
    if (key == '0') { // se mencionan los distintos modos que existen para ser llamados con teclas
        modo = 0;
    }else if (key == '1') {
        modo = 1;
    }else if (key == '2') {
        modo = 2;
    }else if (key == '3') {
        modo = 3;
    }else if (key == '4') {
        modo = 4;
    }else if (key == '5') {
        modo = 5;
    }else if (key == '6') {
        modo = 6;
    }else if (key == '7') {
        modo = 7;
    }else if (key == 'a') {
        modo = 8;
    }else if (key == 'b') {
        modo = 9;
    }else if (key == 'c') {
        modo = 10;
    }else if (key == 'd') {
        modo = 11;
    }else if (key == 'e') {
        modo = 12;
    }else if (key == 'f') {
        modo = 13;
    }
    
    if (key == 'l') {//si apretras la letra l se regeneran los colores.
        setupCuadrados();
    }
}
//--------------------------------------------------------------
void testApp:: keyReleased(int key){
    
    
}
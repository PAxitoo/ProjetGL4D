#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <son.h>

// Variables Globales
// IDs de texture
static GLuint _sphereTexId = 0, _carreTexId = 0, _cubeTexId = 0, _coneTexId = 0;

// IDs de forme
static GLuint _sphereId = 0, _carreId = 0, _cubeId = 0, _coneId = 0, _voroId = 0;;

// IDs de dessiner
static GLuint _pIdSphere = 0, _pIdCarre = 0, _pIdCube = 0, _pIdCone, _pIdRing;
static GLuint currentDrawingId = 0;

// Dimensions de la fenêtre
static GLuint _wW = 1280, _wH = 960;

// Variables de rotation
static GLfloat rotationSpeed = 90.0f; // Vitesse de rotation du cube (en degrés par seconde)

// Variables de couleur
static float target_r, target_g, target_b;
static float current_r, current_g, current_b;

// Variables pour coordonnées et couleurs
static GLfloat *coords = NULL, *colors = NULL;

// Variables pour les sites
static int sites_affiches = 1;
const int nb_sites = 400;

// Prototypes de fonctions
static void init(void);
static void resize(int width, int height);
void dessiner_sphere(double t, GLfloat sphereEchelle);
void dessiner_carre(double t, GLfloat carreEchelle, GLfloat bleu[]);
void dessiner_cube(double t, GLfloat cubeEchelle, GLfloat a, GLfloat rouge[]);
void dessiner_cone(double t, GLfloat coneEchelle);
void dessiner_voronoi();
static void dessiner(void);
static void sortie(void);

int main(int argc, char **argv)
{
  
    if (!gl4duwCreateWindow(argc, argv, "bases 3D", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
                            _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN))
    {
        fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
        return 1;
    }
    init();
    atexit(sortie);
    gl4duwResizeFunc(resize);
    gl4duwDisplayFunc(dessiner);
    gl4duwMainLoop();
    return 0;
}

GLfloat* generer_couleur_aleatoire()
{
    static GLfloat couleur[4];
    couleur[0] = (float)rand() / (float)RAND_MAX;
    couleur[1] = (float)rand() / (float)RAND_MAX;
    couleur[2] = (float)rand() / (float)RAND_MAX;
    couleur[3] = 1.0f;
    return couleur;
}

void mise_a_jour_couleur_arriere_plan(double dt)
{
    // Mettre à jour la couleur actuelle pour se rapprocher de la couleur cible
    current_r += (target_r - current_r) * dt;
    current_g += (target_g - current_g) * dt;
    current_b += (target_b - current_b) * dt;

    // Une fois que la couleur actuelle est suffisamment proche de la couleur cible, générer une nouvelle couleur cible
    if (fabs(current_r - target_r) < 0.01 && fabs(current_g - target_g) < 0.01 && fabs(current_b - target_b) < 0.01)
    {
        target_r = (float)rand() / (float)RAND_MAX;
        target_g = (float)rand() / (float)RAND_MAX;
        target_b = (float)rand() / (float)RAND_MAX;
    }

    // Définir la couleur de fond OpenGL sur la couleur actuelle
    glClearColor(current_r, current_g, current_b, 1.0f);
}


void diag_init(int n, GLfloat **coords, GLfloat **colors)
{
    *coords = malloc((GLuint64)n * 2 * sizeof(**coords));
    assert(*coords);

    *colors = malloc((GLuint64)n * 4 * sizeof(GLfloat));
    assert(*colors);

    for (int i = 0; i < n; ++i)
    {
        (*coords)[i * 2] = gl4dmURand();
        (*coords)[i * 2 + 1] = gl4dmURand();

        (*colors)[i * 4] = gl4dmURand();
        (*colors)[i * 4 + 1] = gl4dmURand();
        (*colors)[i * 4 + 2] = gl4dmURand();
        (*colors)[i * 4 + 3] = 1.;
    }
}


void init(void)
{
    current_r = target_r = (float)rand() / (float)RAND_MAX;
    current_g = target_g = (float)rand() / (float)RAND_MAX;
    current_b = target_b = (float)rand() / (float)RAND_MAX;

    _sphereId = gl4dgGenSpheref(50, 50);
    _coneId = gl4dgGenConef(50, 50);
    _carreId = gl4dgGenQuadf();
    _cubeId = gl4dgGenCubef();

    _pIdRing = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
    _pIdSphere = gl4duCreateProgram("<vs>shaders/sphere.vs", "<fs>shaders/sphere.fs", NULL);
    _pIdCarre = gl4duCreateProgram("<vs>shaders/carre.vs", "<fs>shaders/carre.fs", NULL);
    _pIdCube = gl4duCreateProgram("<vs>shaders/cube.vs", "<fs>shaders/cube.fs", NULL);
    _pIdCone = gl4duCreateProgram("<vs>shaders/cone.vs", "<fs>shaders/cone.fs", NULL);
    _voroId = gl4duCreateProgram("<vs>shaders/voronoi.vs", "<fs>shaders/voronoi.fs", NULL);

    glEnable(GL_DEPTH_TEST);
    gl4duGenMatrix(GL_FLOAT, "proj");
    gl4duGenMatrix(GL_FLOAT, "mod");
    gl4duGenMatrix(GL_FLOAT, "view");

    glGenTextures(1, &_sphereTexId);
    glBindTexture(GL_TEXTURE_2D, _sphereTexId);

    // Générer une texture blanche (4x4 pixels)
    GLuint pixelsBlancs[] = {
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255)
    };

    SDL_Surface *imageCarre = IMG_Load("02.png");
    SDL_Surface *surfaceCarre = SDL_CreateRGBSurface(0, imageCarre->w, imageCarre->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_BlitSurface(imageCarre, NULL, surfaceCarre, NULL);
    SDL_FreeSurface(imageCarre);
    glGenTextures(1, &_carreTexId);
    glBindTexture(GL_TEXTURE_2D, _carreTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceCarre->w, surfaceCarre->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceCarre->pixels);
    SDL_FreeSurface(surfaceCarre);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &_sphereTexId);
    glBindTexture(GL_TEXTURE_2D, _sphereTexId);
    SDL_Surface *image = IMG_Load("fr.jpg");
    SDL_Surface *surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_BlitSurface(image, NULL, surface, NULL);
    SDL_FreeSurface(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_Surface *imageCube = IMG_Load("car.jpg");
    SDL_Surface *surfaceCube = SDL_CreateRGBSurface(0, imageCube->w, imageCube->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_BlitSurface(imageCube, NULL, surfaceCube, NULL);
    SDL_FreeSurface(imageCube);
    glGenTextures(1, &_cubeTexId);
    glBindTexture(GL_TEXTURE_2D, _cubeTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surfaceCube->w, surfaceCube->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceCube->pixels);
    SDL_FreeSurface(surfaceCube);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &_coneTexId);
    glBindTexture(GL_TEXTURE_2D, _coneTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsBlancs);
    glBindTexture(GL_TEXTURE_2D, 0);

    diag_init(nb_sites, &coords, &colors);
    ahInitAudio("music.mp3");
    ahPlayMusic();

    // Spécifier les paramètres de filtrage de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Charger les pixels de la texture dans la mémoire graphique
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsBlancs);
    glBindTexture(GL_TEXTURE_2D, 0);

    resize(_wW, _wH);
}

void resize(int width, int height)
{
    GLfloat ratio;
    _wW = width;
    _wH = height;
    glViewport(0, 0, _wW, _wH);
    ratio = _wW / ((GLfloat)_wH);
    gl4duBindMatrix("proj");
    gl4duLoadIdentityf();
    gl4duFrustumf(-1 * ratio, 1 * ratio, -1 /* / ratio */, 1 /* / ratio */, 2, 1000);
}

void dessiner_sphere(double t, GLfloat sphereEchelle)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _sphereTexId);
    glUniform1i(glGetUniformLocation(_pIdSphere, "tex"), 0);
    gl4duScalef(sphereEchelle, sphereEchelle, sphereEchelle);
}

void dessiner_carre(double t, GLfloat carreEchelle, GLfloat bleu[])
{
    gl4duScalef(carreEchelle, carreEchelle, carreEchelle);
    glUseProgram(_pIdCarre);
    glUniform4fv(glGetUniformLocation(_pIdCarre, "scolor"), 1, bleu);
    glUniform1f(glGetUniformLocation(_pIdCarre, "fade"), 1.0f - carreEchelle);
    glBindTexture(GL_TEXTURE_2D, _carreTexId);
}

void dessiner_cube(double t, GLfloat cubeEchelle, GLfloat a, GLfloat rouge[])
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _cubeTexId);
    gl4duScalef(cubeEchelle, cubeEchelle, cubeEchelle);
    gl4duRotatef(a, 0, 1, 0);
    gl4duRotatef(a, 1, 0, 0);
    gl4duTranslatef(0.0f, 5.0f * sin(t), 0.0f);
    glUseProgram(_pIdCube);
    glUniform4fv(glGetUniformLocation(_pIdCube, "scolor"), 1, rouge);
    glUniform1f(glGetUniformLocation(_pIdCube, "fade"), cubeEchelle);
}

void dessiner_cone(double t, GLfloat coneEchelle)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _coneTexId);
    glUniform1i(glGetUniformLocation(_pIdCone, "tex"), 0);
    gl4duScalef(coneEchelle, coneEchelle, coneEchelle);
    gl4dgDraw(_coneId);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void dessiner_voronoi()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_voroId);

    if (sites_affiches > nb_sites)
    {
        sites_affiches = 1;
    }

    glUniform2fv(glGetUniformLocation(_voroId, "coords"), sites_affiches, coords);
    glUniform1i(glGetUniformLocation(_voroId, "nb_sites"), sites_affiches);
    glUniform4fv(glGetUniformLocation(_voroId, "colors"), nb_sites, colors);

    gl4dgDraw(_carreId);

    glUseProgram(0);

    ++sites_affiches;
}

void dessiner(void)
{

    static double t0 = 0.0;
    double t = gl4dGetElapsedTime() / 1000.0;
    double dt = t - t0;
    t0 = t;

    mise_a_jour_couleur_arriere_plan(dt);
    GLfloat rouge[] = {1.0f, 0.0f, 0.0f, 1.0f}; // couleur pour le cube : rouge
    static GLfloat a = 0.0f;

    GLfloat sphereEchelle = 1.0f;
    GLfloat cubeEchelle = 1.0f;
    GLfloat coneEchelle = 1.0f;

    // Animation de la sphère pendant les 10 premières secondes
    if (t < 10.0f)
    {
        sphereEchelle = 1.0f + cos(4 * t) * 0.5f;
        currentDrawingId = _sphereId;
    }
    // Animation de la sphère qui grossit pendant 3 secondes
    else if (t < 13.0f)
    {
        sphereEchelle = 1.0f + 3.0f * (t - 10.0f);
        currentDrawingId = _sphereId;
    }
    // Apparition du carré et rétrécissement pendant 5 secondes
    else if (t < 18.0f)
    {
        coneEchelle = fmax(0.0f, 1.0f - (t - 13.0f) / 5.0f);
        currentDrawingId = _coneId;
    }
    // Animation du cube pendant 7 secondes
    else if (t < 25.0f)
    {
        cubeEchelle = 1.0f + sin(2 * (t - 18.0f)) * 0.5f;
        currentDrawingId = _cubeId;
    }
    // Apparition de voro après 7 secondes
    else if (t < 30.0f)
    {
        gl4dgDelete(_cubeId);
        dessiner_voronoi();
        currentDrawingId = _voroId;
    }


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl4duBindMatrix("view");
    gl4duLoadIdentityf();

    if (currentDrawingId == _cubeId)
    {
        GLfloat camX = 8.0f * cos(4.0 * t); // Vitesse de rotation doublée
        GLfloat camY = 8.0f * sin(4.0 * t) + cos(4.0 * t); // Vitesse de rotation doubl
        GLfloat camZ = 8.0f * sin(4.0 * t); // Vitesse de rotation doublée
        gl4duLookAtf(camX, camY, camZ, 0, 0, 0, 0, 1.0f, 0);
    }
    else
    {
        GLfloat camX = 8.0f * cos(2.0 * t);
        GLfloat camZ = 8.0f * sin(2.0 * t);
        gl4duLookAtf(camX, 0, camZ, 0, 0, 0, 0, 1.0f, 0);
    }

    gl4duBindMatrix("mod");
    gl4duLoadIdentityf();

    if (currentDrawingId == _sphereId)
    {
        dessiner_sphere(t, sphereEchelle);
    }
    else if (currentDrawingId == _coneId)
    {
        dessiner_cone(t, coneEchelle);
    }
    else if (currentDrawingId == _cubeId)
    {
        dessiner_cube(t, cubeEchelle, a, rouge);
    }
    else if (currentDrawingId == _voroId)
    {
        dessiner_voronoi();
    }

    glUseProgram(_pIdSphere);
    glUseProgram(_pIdCarre);
    glUseProgram(_pIdCube);
    glUseProgram(_pIdCone);

    GLfloat lightPos[3] = {5.0f * cos(t), 0.0f, 5.0f * sin(t)};
    GLfloat lightColor[4] = {(GLfloat)(fabs(sin(t))), (GLfloat)(fabs(cos(t))), 0.5f, 1.0f};
    glUniform3fv(glGetUniformLocation(_pIdSphere, "lightPos"), 1, lightPos);
    glUniform4fv(glGetUniformLocation(_pIdSphere, "lcolor"), 1, lightColor);

    gl4duSendMatrices();

    gl4dgDraw(currentDrawingId);
    glBindTexture(GL_TEXTURE_2D, 0);

    a += rotationSpeed * dt; // Vitesse de rotation doublée
}


void sortie(void)
{
    glDeleteProgram(_pIdSphere);
    glDeleteTextures(1, &_sphereTexId);
    glDeleteTextures(1, &_cubeTexId);

    gl4dgDelete(_sphereId);

    gl4dgDelete(_cubeId);

    ahClean();
    gl4duClean(GL4DU_ALL);
}

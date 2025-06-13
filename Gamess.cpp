#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <stdio.h>  // untuk sprintf
#include <stdlib.h>
#include <ctime> 
// #include <stdlib.h>  // DIHAPUS - konflik dengan <cstdlib>
#include <vector>
#include <cstdlib> // untuk rand()
#define PI 3.14159265
#define M_PI 3.14159265358979323846
int getStrokeStringWidth(void* font, const char* str);
void init();
void timerEnhanced(int value);  
bool isGameStarted = false;
int lane = 1;  // 0 = kiri, 1 = tengah, 2 = kanan
float lanePositions[] = {-2.0f, 0.0f, 2.0f}; // posisi X untuk tiap jalur
float slideSpeed = 0.2f;  // kecepatan transisi geser
float angle = 0.0f;
GLUquadric* quadric;
int score = 0;
int health = 3;
std::vector<std::vector<float> > coins;
std::vector<std::vector<float> > bombs;
std::vector<std::vector<float> > magnets;
std::vector<std::vector<float> > poisons;
std::vector<std::vector<float> > rocks;

float cameraAngle = 0.0f;
bool isAutoRunning = true;
float runSpeed = 0.15f;
// deklarasi global atau di header
bool magnetActive = false;
float magnetDuration = 0.0f;

// Variabel posisi dan kamera
float baymaxPosX = 0.0f;
float baymaxPosZ = 0.0f;
float baymaxAngle = 0.0f;

float cameraHeight = 5.0f;
float cameraDistance = 12.0f;
// Definisi global posisi Baymax
float baymaxX = 0.0f;
float baymaxZ = 0.0f;

double rotX = 0.0;
double rotY = 0.0;
bool recentlyHit = false;
float hitBlinkTimer = 0.0f;
bool blink = false; // global

// Untuk animasi kaki
bool isWalking = false;
float legSwing = 0.0f;
bool legDirection = true;
float baymaxPosY = 0.0f;   // posisi vertikal Baymax
bool isJumping = false;
float jumpHeight = 0.0f;
float jumpSpeed = 0.05f;

float playerX = 0.0f;
float playerY = 0.0f;
float playerZ = 0.0f;
bool gameOver = false;

// Tambahkan variabel yang hilang untuk jalan dan mobil
float roadWidth = 4.0f;
float roadLength = 20.0f;
float armSwingLeft = 0.0f;
float armSwingRight = 0.0f;
bool armDirection = true;
float laneChangeProgress = 0.0f;
bool isChangingLane = false;
int targetLane = 1;
float laneChangeHeight = 0.0f;
// == MAGNET ==

float magnetTimer = 0.0f;

float magnetRadius = 5.0f;

// == RACUN JAMUR ==
bool isPoisoned = false;
float poisonTimer = 0.0f;
float poisonDuration = 3.0f;


struct Bird {
    float x, y, z;
    float dx, dy, dz;
    float colorR, colorG, colorB;
};

std::vector<Bird> birds;

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Main light (sun-like)
    const GLfloat light_position[] = { 10.0f, 15.0f, 10.0f, 1.0f };
    const GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Secondary light for better illumination
    glEnable(GL_LIGHT1);
    const GLfloat light1_position[] = { -8.0f, 10.0f, -5.0f, 1.0f };
    const GLfloat light1_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat light1_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialf(GL_FRONT, GL_SHININESS, 60.0f);
}

/////////////////////////GROUND//////////////////////////////
void drawGround() {
    float baymaxPosZ = 0.0f; // atau ambil dari variabel global
    int tileLength = 4;
    int numSegments = 70;
    float laneX[] = {-2.0f, 0.0f, 2.0f};

    int centerZ = (int)(baymaxPosZ / tileLength);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);

    for (int i = centerZ - numSegments; i <= centerZ + numSegments; i++) {
        float z = i * tileLength;

        // rumput kiri
        glColor3f(0.4f, 0.7f, 0.2f);
        glVertex3f(-10.0f, 0.0f, z);
        glVertex3f(-3.0f, 0.0f, z);
        glVertex3f(-3.0f, 0.0f, z + tileLength);
        glVertex3f(-10.0f, 0.0f, z + tileLength);

        // rumput kanan
        glVertex3f(3.0f, 0.0f, z);
        glVertex3f(10.0f, 0.0f, z);
        glVertex3f(10.0f, 0.0f, z + tileLength);
        glVertex3f(3.0f, 0.0f, z + tileLength);

        // jalur aspal
        for (int lane = 0; lane < 3; lane++) {
            float x = laneX[lane];
            float colorVar = 0.6f + 0.1f * (lane == 1);
            glColor3f(0.3f * colorVar, 0.3f * colorVar, 0.3f * colorVar);

            glVertex3f(x - 1.0f, 0.0f, z);
            glVertex3f(x + 1.0f, 0.0f, z);
            glVertex3f(x + 1.0f, 0.0f, z + tileLength);
            glVertex3f(x - 1.0f, 0.0f, z + tileLength);
        }
    }
    glEnd();

    // Tambahkan garis putih di tengah jalur
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    for (int i = centerZ - numSegments; i <= centerZ + numSegments; i++) {
        float z = i * tileLength + tileLength / 2;
        for (int lane = 0; lane < 2; lane++) {
            float x1 = laneX[lane] + 1.0f;
            glVertex3f(x1, 0.01f, z);
            glVertex3f(x1, 0.01f, z + 1.0f);
        }
    }
    glEnd();
    
}



/////////////////////////KOIN//////////////////////////////
void drawCoin(float radius, float thickness, int segments, bool withRidges = true) {
    // Face depan
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, 1);
    glVertex3f(0, 0, thickness / 2);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2 * PI * i / segments;
        glVertex3f(radius * cos(angle), radius * sin(angle), thickness / 2);
    }
    glEnd();

    // Face belakang
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, -1);
    glVertex3f(0, 0, -thickness / 2);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2 * PI * i / segments;
        glVertex3f(radius * cos(angle), radius * sin(angle), -thickness / 2);
    }
    glEnd();

    // Samping silinder (bergerigi kalau withRidges = true)
    if (withRidges) {
        glBegin(GL_QUADS);
        for (int i = 0; i < segments; ++i) {
            float angle1 = 2 * PI * i / segments;
            float angle2 = 2 * PI * (i + 1) / segments;

            float x1 = radius * cos(angle1);
            float y1 = radius * sin(angle1);
            float x2 = radius * cos(angle2);
            float y2 = radius * sin(angle2);

            float nx = cos(angle1);
            float ny = sin(angle1);

            if (i % 4 == 0) {
                float ridgeOffset = 0.01f;
                glNormal3f(nx, ny, 0);
                glVertex3f(x1, y1, thickness / 2);
                glVertex3f(x1, y1, -thickness / 2);
                glVertex3f(x1 + nx * ridgeOffset, y1 + ny * ridgeOffset, -thickness / 2);
                glVertex3f(x1 + nx * ridgeOffset, y1 + ny * ridgeOffset, thickness / 2);
            } else {
                glNormal3f(nx, ny, 0);
                glVertex3f(x1, y1, thickness / 2);
                glVertex3f(x1, y1, -thickness / 2);
                glVertex3f(x2, y2, -thickness / 2);
                glVertex3f(x2, y2, thickness / 2);
            }
        }
        glEnd();
    }
}

void drawStar(float radius, float z, float height) {
    const int num_points = 5;
    const float inner_radius = radius * 0.5f;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, z + height);
    for (int i = 0; i <= 2 * num_points; ++i) {
        float angle = PI / num_points * i;
        float r = (i % 2 == 0) ? radius : inner_radius;
        glVertex3f(r * cos(angle), r * sin(angle), z + height);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 2 * num_points; ++i) {
        float angle = PI / num_points * i;
        float r = (i % 2 == 0) ? radius : inner_radius;
        float x = r * cos(angle);
        float y = r * sin(angle);
        float nx = cos(angle);
        float ny = sin(angle);
        glNormal3f(nx, ny, 0.0f);
        glVertex3f(x, y, z + height);
        glVertex3f(x, y, z);
    }
    glEnd();
}

float coinPositions[][3] = {
    {0.0f, 1.5f, 0.0f},
    {2.0f, 1.5f, 5.0f},
    {-2.5f, 1.5f, -3.0f}
};

void generateCoins(int count) {
    coins.clear();
    for (int i = 0; i < count; ++i) {
        float laneX[] = {-2.0f, 0.0f, 2.0f};
        int laneIndex = rand() % 3;
        float x = laneX[laneIndex];
        float y = 1.5f;
        float z = -(rand() % 300);  // tersebar di sepanjang -Z
        std::vector<float> coin;
		coin.push_back(x);
		coin.push_back(y);
		coin.push_back(z);
		coins.push_back(coin);

    }
}

void renderCoins() {
    for (int i = 0; i < coins.size(); ++i) {
    glPushMatrix();
    glTranslatef(coins[i][0], coins[i][1], coins[i][2]);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glScalef(0.6f, 0.6f, 0.6f);
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCoin(0.8f, 0.15f, 100, true);
    drawStar(0.3f, 0.075f, 0.03f);
    glPopMatrix();
    }
}

void drawBomb();

void renderBombs() {
    for (int i = 0; i < bombs.size(); ++i) {
        glPushMatrix();
        glTranslatef(bombs[i][0], bombs[i][1], bombs[i][2]);
        glScalef(0.5f, 0.5f, 0.5f);
        drawBomb();
        glPopMatrix();
    }
}

/////////////////////////MAGNET DAN JAMUR//////////////////////////////

void drawMagnet() {
    glColor3f(1.0f, 0.2f, 1.0f);
    glPushMatrix();
    glScalef(0.4f, 0.4f, 0.2f);
    glutSolidTorus(0.2, 0.6, 16, 32);
    glPopMatrix();
}

void drawPoisonMushroom() {
    glPushMatrix();
    glColor3f(0.6f, 0.0f, 0.6f); // ungu tua
    glTranslatef(0.0f, 0.3f, 0.0f);
    glutSolidSphere(0.4, 20, 20); // kepala
    glColor3f(0.3f, 0.2f, 0.1f);
    glTranslatef(0.0f, -0.6f, 0.0f);
    glScalef(0.2f, 0.5f, 0.2f); // batang
    glutSolidCube(1.0);
    glPopMatrix();
}

void generateMagnets(int count = 5) {
    magnets.clear();
    for (int i = 0; i < count; ++i) {
        int laneIndex = rand() % 3;
        float x = lanePositions[laneIndex];
        float y = 0.5f;
        float z = baymaxPosZ - (rand() % 100 + 30); 

        std::vector<float> m;
        m.push_back(x);
        m.push_back(y);
        m.push_back(z);
        magnets.push_back(m);
    }
}

void updateMagnets() {
    // Hapus magnet yang sudah dilewati Baymax
    for (int i = 0; i < magnets.size(); ++i) {
        if (magnets[i][2] > baymaxPosZ + 5.0f) {
            magnets.erase(magnets.begin() + i);
            --i;
        }
    }

    // Tambah magnet baru kalau jumlahnya kurang dari 2
    while (magnets.size() < 2) {
        int laneIndex = rand() % 3;
        float x = lanePositions[laneIndex];
        float y = 0.5f;
        float z = baymaxPosZ - (rand() % 100 + 30);

        std::vector<float> m = {x, y, z};
        magnets.push_back(m);
    }
}


void generatePoisons(int count = 5) {
    poisons.clear();
    for (int i = 0; i < count; ++i) {
        int laneIndex = rand() % 3;
        float x = lanePositions[laneIndex];
        float y = 0.5f;
        float z = -(rand() % 300);

        std::vector<float> poisonPoint;
        poisonPoint.push_back(x);
        poisonPoint.push_back(y);
        poisonPoint.push_back(z);

        poisons.push_back(poisonPoint);
    }
}



void renderPoisons() {
    for (size_t i = 0; i < poisons.size(); ++i) {
        std::vector<float>& p = poisons[i];
        glPushMatrix();
        glTranslatef(p[0], p[1], p[2]);
        drawPoisonMushroom();
        glPopMatrix();
    }
}

void renderMagnets() {
    for (int i = 0; i < magnets.size(); ++i) {
        glPushMatrix();
        glTranslatef(magnets[i][0], magnets[i][1], magnets[i][2]);
        drawMagnet();
        glPopMatrix();
    }
}


void activateMagnet() {
    magnetActive = true;
    magnetDuration = 5.0f; // misal magnet aktif selama 5 detik
    // tambah efek lainnya sesuai kebutuhan
}

void checkMagnetCollision() {
    if (gameOver) return;
    for (int i = 0; i < magnets.size(); ++i) {
        float dx = baymaxPosX - magnets[i][0];
        float dz = baymaxPosZ - magnets[i][2];
        float dist = sqrt(dx * dx + dz * dz);
        if (dist < 1.0f) {
            activateMagnet();
            magnets.erase(magnets.begin() + i);
            break;
        }
    }
}


void checkPoisonCollision() {
    if (gameOver || isPoisoned) return;
    for (int i = 0; i < poisons.size(); ++i) {
        float dx = baymaxPosX - poisons[i][0];
        float dz = baymaxPosZ - poisons[i][2];
        float dist = sqrt(dx * dx + dz * dz);
        if (dist < 1.0f) {
            isPoisoned = true;
            poisonTimer = 0.0f;
            health -= 1;
            if (health < 0) health = 0;
            poisons.erase(poisons.begin() + i);
            break;
        }
    }
}


/////////////////////////AWAN//////////////////////////////
void drawCloud() {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    glColor3f(0.95f, 0.95f, 0.98f);

    // Main cloud structure
    glPushMatrix(); glTranslatef(-0.4f, -0.1f, 0.0f); gluSphere(quad, 0.4, 20, 20); glPopMatrix();
    glPushMatrix(); glTranslatef(0.4f, -0.1f, 0.0f); gluSphere(quad, 0.45, 20, 20); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.25f, -0.05f, 0.4f); gluSphere(quad, 0.3, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, -0.05f, 0.4f); gluSphere(quad, 0.35, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.5f); gluSphere(quad, 0.3, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.25f, -0.05f, -0.4f); gluSphere(quad, 0.35, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, -0.05f, -0.4f); gluSphere(quad, 0.3, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.0f, -0.5f); gluSphere(quad, 0.3, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.4f, 0.0f); gluSphere(quad, 0.4, 20, 20); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.3f, 0.3f, 0.0f); gluSphere(quad, 0.3, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.3f, 0.3f, 0.0f); gluSphere(quad, 0.35, 16, 16); glPopMatrix();

    glColor3f(0.98f, 0.98f, 1.0f);
    glPushMatrix(); glTranslatef(0.15f, 0.25f, 0.15f); gluSphere(quad, 0.22, 12, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.18f, -0.3f, 0.18f); gluSphere(quad, 0.18, 12, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(0.28f, 0.05f, -0.15f); gluSphere(quad, 0.18, 12, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.28f, 0.05f, 0.15f); gluSphere(quad, 0.2, 12, 12); glPopMatrix();

    gluDeleteQuadric(quad);
}
void drawCloudsInfinite(float baymaxPosZ) {
    int tileLength = 20;
    int numSegments = 25;
    int centerZ = (int)(baymaxPosZ / tileLength);

    srand(0); // agar posisi awan konsisten di setiap run

    for (int i = centerZ - numSegments; i <= centerZ + numSegments; i++) {
        float baseZ = i * tileLength;

        int cloudCount = 6 + rand() % 6;  // 6–11 awan per segmen

        for (int c = 0; c < cloudCount; ++c) {
            float offsetX = -25.0f + rand() % 51;           // X: -25 to +25 (lebar langit)
            float offsetY = 8.0f + (rand() % 40) / 10.0f;   // Y: 8.0 - 12.0 (tinggi)
            float offsetZ = baseZ + (rand() % tileLength);  // Z posisi

            float scale = 2.5f + (rand() % 20) / 100.0f;     // Skala 0.6 - 1.3

            // ?? 50% awan statis, 50% dinamis
            bool isDynamic = rand() % 2 == 0;

            float move = 0.0f;
            if (isDynamic) {
                // gerak sinus pelan
                move = sin(glutGet(GLUT_ELAPSED_TIME) / 3500.0f + offsetZ * 0.1f) * 1.5f;
            }

            glPushMatrix();
            glTranslatef(offsetX + move, offsetY, offsetZ);
            glScalef(scale, scale, scale);
            drawCloud(); // fungsi kamu sendiri
            glPopMatrix();
        }
    }
}

/////////////////////////TREES//////////////////////////////
void drawFruitOrFlower(float x, float y, float z) {
    // Warna buah/bunga acak (merah-pink-kuning)
    float r = 0.8f + (rand() % 20) / 100.0f; // 0.8 - 1.0
    float g = 0.2f + (rand() % 30) / 100.0f; // 0.2 - 0.5
    float b = 0.2f + (rand() % 30) / 100.0f; // 0.2 - 0.5
    glColor3f(r, g, b);

    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(0.25f, 20, 20);
    glPopMatrix();
}



void drawTree(GLUquadric* quadric, float height, float trunkRadius) {
    // === Batang pohon (tertutup atas & bawah) ===
    glColor3f(0.4f, 0.2f, 0.1f); // coklat batang

    // Dasar batang (tutup bawah)
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // orientasi silinder vertikal
        gluDisk(quadric, 0.0f, trunkRadius, 12, 1); // tutup bawah
        gluCylinder(quadric, trunkRadius, trunkRadius * 0.8f, height, 12, 8); // batang
        glTranslatef(0.0f, 0.0f, height); // ke atas batang
        gluDisk(quadric, 0.0f, trunkRadius * 0.8f, 12, 1); // tutup atas
    glPopMatrix();

    // === Daun bagian bawah ===
    glColor3f(0.1f, 0.6f, 0.1f); // hijau daun
    glPushMatrix();
        glTranslatef(0.0f, height * 0.7f, 0.0f);
        gluSphere(quadric, height * 0.4f, 16, 16);
    glPopMatrix();

    // === Daun bagian atas ===
    glPushMatrix();
        glTranslatef(0.0f, height * 0.9f, 0.0f);
        gluSphere(quadric, height * 0.3f, 16, 16);
    glPopMatrix();
    // Buah/bunga - tempelkan beberapa secara acak di sekitar daun
	for (int i = 0; i < 5; ++i) {
	    float angle = (rand() % 360) * 3.14f / 180.0f;
	    float radius = height  * 0.35f;
	    float fx = cos(angle) * radius;
	    float fz = sin(angle) * radius;
	    float fy = height * 0.9f + ((rand() % 10) / 30.0f); // dekat atas daun
	
	    drawFruitOrFlower(fx, fy, fz);
	}

}

void drawTreesInfinite(GLUquadric* quadric, float baymaxPosZ) {
    int tileLength = 4;
    int numSegments = 70;
    int centerZ = (int)(baymaxPosZ / tileLength);

    for (int i = centerZ - numSegments; i <= centerZ + numSegments; i++) {
        float z = i * tileLength;

        // Variasi tinggi dan radius batang pohon berdasarkan indeks i
        float h = 4.0f + (i % 3) * 0.7f;       // tinggi: 3.0, 3.5, 4.0
        float r = 0.35f + (i % 2) * 0.02f;     // radius: 0.25, 0.3

        // === Sisi kiri jalan ===
        glPushMatrix();
        glTranslatef(-6.5f, 0.0f, z);
        drawTree(quadric, h, r);  // <== dipanggil di sini
        glPopMatrix();

        // === Sisi kanan jalan ===
        glPushMatrix();
        glTranslatef(6.5f, 0.0f, z + tileLength / 2);
        drawTree(quadric, h, r);  // <== dipanggil di sini juga
        glPopMatrix();
    }
}


/////////////////////////ROCKS//////////////////////////////
void drawRock(float size) {
    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glScalef(size, size * 0.6f, size * 0.8f);
    gluSphere(quadric, 1.0f, 12, 8);
    glPopMatrix();
}


void updateRocks() {
    for (int i = 0; i < rocks.size(); ++i) {
        if (rocks[i][2] > baymaxPosZ + 5.0f) {
            rocks.erase(rocks.begin() + i);
            --i;
        }
    }

    while (rocks.size() < 10) { // tambahkan jika kurang
        float laneX[] = {-5.0f, 0.0f, 5.0f};
        int laneIndex = rand() % 3;
        float x = laneX[laneIndex];
        float y = 0.0f;
        float z = baymaxPosZ - (rand() % 100 + 30);

        rocks.push_back({x, y, z});
    }
}

void renderRocks() {
    for (auto& rock : rocks) {
        glPushMatrix();
        glTranslatef(rock[0], rock[1], rock[2]);
        drawRock(0.4f + ((rand() % 3) * 0.1f));
        glPopMatrix();
    }
}

/////////////////////////BUILDING//////////////////////////////

struct Building {
    float x, y, z;
    float width, height, depth;
    int floors;
    int windowsPerFloor;
    float r, g, b;
    bool isRightSide; // true jika gedung di kanan jalan
};

std::vector<Building> buildings;

void generateBuildings() {
    buildings.clear();

    float startZ = -300.0f;
    float endZ = 300.0f;
    float currentZ = startZ;
    float leftX = -10.5f;
    float rightX = 10.5f;

    while (currentZ <= endZ) {
        // Random ukuran gedung
        float width = 2.0f + (rand() % 10) * 0.2f;  // lebar 2.0 - 4.0
        float depth = 2.5f;
        int floors = 2 + rand() % 4;
        float height = floors * 2.0f;
        int windows = 2 + rand() % 2;

        // Warna bangunan
        float r = 0.5f + (rand() % 50) / 100.0f;
        float g = 0.5f + (rand() % 50) / 100.0f;
        float b = 0.5f + (rand() % 50) / 100.0f;

        // Simpan bangunan kiri
        buildings.push_back({leftX, height / 2.0f, currentZ, width, height, depth, floors, windows, r, g, b, false});

        // Warna baru untuk kanan
        float r2 = 0.5f + (rand() % 50) / 100.0f;
        float g2 = 0.5f + (rand() % 50) / 100.0f;
        float b2 = 0.5f + (rand() % 50) / 100.0f;
        int floors2 = 2 + rand() % 4;
        float height2 = floors2 * 2.0f;

        buildings.push_back({rightX, height2 / 2.0f, currentZ, width, height2, depth, floors2, windows, r2, g2, b2, true});

        currentZ += width + 0.4f;  // Jarak antar gedung kecil
    }
}

void drawBuildingRoof(float width, float height, float depth, float r, float g, float b) {
    float roofHeight = height * 0.3f;
    glPushMatrix();
    glTranslatef(0.0f, height / 2.0f + roofHeight / 2.0f, 0.0f);
    float darkR = r * 0.8f;
    float darkG = g * 0.8f;
    float darkB = b * 0.8f;
    glBegin(GL_TRIANGLES);
    glColor3f(darkR, darkG, darkB);
    // depan
    glVertex3f(-width / 2, -roofHeight / 2, depth / 2);
    glVertex3f(width / 2, -roofHeight / 2, depth / 2);
    glVertex3f(0.0f, roofHeight / 2, depth / 2);
    // belakang
    glVertex3f(-width / 2, -roofHeight / 2, -depth / 2);
    glVertex3f(width / 2, -roofHeight / 2, -depth / 2);
    glVertex3f(0.0f, roofHeight / 2, -depth / 2);
    glEnd();
    glBegin(GL_QUADS);
    // kiri
    glVertex3f(-width / 2, -roofHeight / 2, -depth / 2);
    glVertex3f(-width / 2, -roofHeight / 2, depth / 2);
    glVertex3f(0.0f, roofHeight / 2, depth / 2);
    glVertex3f(0.0f, roofHeight / 2, -depth / 2);
    // kanan
    glVertex3f(width / 2, -roofHeight / 2, -depth / 2);
    glVertex3f(width / 2, -roofHeight / 2, depth / 2);
    glVertex3f(0.0f, roofHeight / 2, depth / 2);
    glVertex3f(0.0f, roofHeight / 2, -depth / 2);
    glEnd();
    glPopMatrix();
}

void drawDetailedBuilding(float width, float height, float depth, int floors, int windowsPerFloor,
                          float r, float g, float b, bool isRightSide) {
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();


    drawBuildingRoof(width, height, depth, r, g, b);
}

void drawBuildingsInfinite(float baymaxPosZ) {
    float viewRange = 100.0f;
    for (int i = 0; i < buildings.size(); ++i) {
        const Building& b = buildings[i];
        if (fabs(b.z - baymaxPosZ) > viewRange) continue;
        
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        
        // Rotasi bangunan agar jendela menghadap ke tengah jalan (ke karakter)
        if (b.isRightSide) {
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);  // gedung kanan: kaca menghadap ke kiri (ke tengah)
        } else {
            glRotatef(-90.0f, 0.0f, 1.0f, 0.0f); // gedung kiri: kaca menghadap ke kanan (ke tengah)
        }
        
        glColor3f(b.r, b.g, b.b);
        drawDetailedBuilding(b.width, b.height, b.depth, b.floors, b.windowsPerFloor, b.r, b.g, b.b, b.isRightSide);
        glPopMatrix();
    }
}
/////////////////////////TANTANGAN//////////////////////////


/////////////////////////BOMB//////////////////////////////

void drawCylinder(GLUquadric* quad, float base, float top, float height) {
    gluCylinder(quad, base, top, height, 32, 32);
}

void drawDisk(GLUquadric* quad, float inner, float outer) {
    gluDisk(quad, inner, outer, 32, 1);
}

void drawSphere(GLUquadric* quad, float radius) {
    gluSphere(quad, radius, 32, 32);
}

void drawBomb() {
    GLUquadric* quad = gluNewQuadric();

    // Badan bom
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    drawSphere(quad, 1.0f);
    glPopMatrix();

    // Tabung
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(0.0f, 0.7f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCylinder(quad, 0.35f, 0.35f, 0.3f);
    glTranslatef(0.0f, 0.0f, 0.3f);
    drawDisk(quad, 0.0f, 0.35f);
    glPopMatrix();

    // Api
    glPushMatrix();
    glColor3f(1.0f, 0.6f, 0.0f);
    glTranslatef(0.0f, 1.18f, 0.0f);
    glRotatef(90, 1, 0, 0);
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glRotatef(i * 45, 0, 1, 0);
        glRotatef(30, 1, 0, 0);
        glScalef(0.08f, 0.2f, 0.08f);
        drawSphere(quad, 1.0f);
        glPopMatrix();
    }
    glPopMatrix();

    // Tanda silang
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 1.01f);
    glScalef(0.5f, 0.1f, 0.01f);
    drawSphere(quad, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 1.01f);
    glScalef(0.1f, 0.5f, 0.01f);
    drawSphere(quad, 1.0f);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

// Tambahkan fungsi yang hilang untuk jalan dan mobil
void drawRoad(float x, float z, float width, float length) {
    glColor3f(0.3f, 0.3f, 0.3f); // Abu-abu gelap untuk aspal
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x - width/2, 0.02f, z);
    glVertex3f(x + width/2, 0.02f, z);
    glVertex3f(x + width/2, 0.02f, z + length);
    glVertex3f(x - width/2, 0.02f, z + length);
    glEnd();
    
    // Garis putih di tengah jalan
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x - 0.05f, 0.03f, z);
    glVertex3f(x + 0.05f, 0.03f, z);
    glVertex3f(x + 0.05f, 0.03f, z + length);
    glVertex3f(x - 0.05f, 0.03f, z + length);
    glEnd();
}

void updateJump() {
    if (isJumping) {
        jumpHeight += jumpSpeed;
        baymaxPosY = jumpHeight;

        if (jumpHeight >= 1.0f) {  // batas tinggi loncat
            jumpSpeed = -jumpSpeed; // mulai turun
        }
        if (jumpHeight <= 0.0f) {  // sudah kembali ke tanah
            jumpHeight = 0.0f;
            baymaxPosY = 0.0f;
            jumpSpeed = 0.05f;
            isJumping = false;
        }
    }
    glutPostRedisplay();
}

void renderBitmapString(float x, float y, void *font, const char *string) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void drawEllipsoid(float rx, float ry, float rz, int stacks = 20, int slices = 20) {
    for (int i = 0; i < stacks; ++i) {
        float lat0 = PI * (-0.5 + (float)i / stacks);
        float lat1 = PI * (-0.5 + (float)(i + 1) / stacks);
        float sinLat0 = sin(lat0), cosLat0 = cos(lat0);
        float sinLat1 = sin(lat1), cosLat1 = cos(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * PI * (float)(j) / slices;
            float sinLng = sin(lng), cosLng = cos(lng);

            float x = cosLng;
            float y = sinLng;

            glNormal3f(rx * x * cosLat0, ry * sinLat0, rz * y * cosLat0);
            glVertex3f(rx * x * cosLat0, ry * sinLat0, rz * y * cosLat0);

            glNormal3f(rx * x * cosLat1, ry * sinLat1, rz * y * cosLat1);
            glVertex3f(rx * x * cosLat1, ry * sinLat1, rz * y * cosLat1);
        }
        glEnd();
    }
}



void drawHead() {
    glPushMatrix();
    glTranslatef(0.0f, 2.8f, 0.0f);
    
    // Sedikit goyang kepala saat berlari
    if (isWalking) {
        glRotatef(sin(legSwing * PI / 180.0f) * 2.0f, 0.0f, 0.0f, 1.0f);
    }
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(0.8f, 0.55f, 0.65f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    gluSphere(quadric, 1.0f, 30, 30);
    glPopMatrix();
    glTranslatef(0.0f, -0.2f, -0.55f);
    glPopMatrix();
}

void drawBody() {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Sedikit condong ke depan saat berlari
    if (isWalking) {
        glRotatef(5.0f, 1.0f, 0.0f, 0.0f);
    }
    
    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);
    glScalef(1.25f, 1.0f, 1.2f);
    gluSphere(quadric, 1.3f, 30, 30);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glScalef(1.55f, 1.8f, 1.3f);
    gluSphere(quadric, 1.5f, 30, 30);
    glPopMatrix();
    glPopMatrix();
}

void drawArms() {
    // Tangan kiri - ayunan yang lebih dramatis
    glPushMatrix();
    glTranslatef(-1.6f, 0.5f, 0.0f);
    
    // Rotasi bahu untuk ayunan tangan
    glRotatef(armSwingLeft, 1.0f, 0.0f, 0.0f);
    
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f); // posisi dasar tangan
    glScalef(2.0f, 4.0f, 1.0f);
    drawEllipsoid(0.3f, 0.6f, 0.4f);
    glPopMatrix();

    // Tangan kanan - ayunan berlawanan
    glPushMatrix();
    glTranslatef(1.6f, 0.5f, 0.0f);
    
    // Rotasi bahu untuk ayunan tangan
    glRotatef(armSwingRight, 1.0f, 0.0f, 0.0f);
    
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f); // posisi dasar tangan
    glScalef(2.0f, 4.0f, 1.0f);
    drawEllipsoid(0.3f, 0.6f, 0.4f);
    glPopMatrix();
}

void drawLegs() {
    // Kaki kiri dengan animasi berlari yang lebih realistis
    glPushMatrix();
    glTranslatef(-0.7f, -3.2f, 0.0f);
    
    // Rotasi pinggul untuk langkah
    glRotatef(legSwing, 1.0f, 0.0f, 0.0f);
    
    // Tekuk lutut saat berlari
    if (isWalking && legSwing > 0) {
        glTranslatef(0.0f, -0.3f, 0.3f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    }
    
    glScalef(1.6f, 3.8f, 1.3f);
    drawEllipsoid(0.5f, 0.35f, 0.6f);
    glPopMatrix();

    // Kaki kanan dengan animasi berlawanan
    glPushMatrix();
    glTranslatef(0.7f, -3.2f, 0.0f);
    
    // Rotasi pinggul untuk langkah berlawanan
    glRotatef(-legSwing, 1.0f, 0.0f, 0.0f);
    
    // Tekuk lutut saat berlari
    if (isWalking && legSwing < 0) {
        glTranslatef(0.0f, -0.3f, 0.3f);
        glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    }
    
    glScalef(1.6f, 3.8f, 1.3f);
    drawEllipsoid(0.5f, 0.35f, 0.6f);
    glPopMatrix();
}

void drawBaymax() {
    glPushMatrix();
    
    // Efek bouncing saat berlari
    if (isWalking) {
        float bounce = abs(sin(legSwing * PI / 90.0f)) * 0.05f;
        glTranslatef(0.0f, bounce, 0.0f);
    }
    
    // Efek lompat saat ganti jalur
    if (isChangingLane) {
        glTranslatef(0.0f, laneChangeHeight, 0.0f);
        // Sedikit rotasi saat lompat
        glRotatef(laneChangeHeight * 10.0f, 0.0f, 0.0f, 1.0f);
    }
    
    glScalef(0.4f, 0.4f, 0.4f);
    drawHead();
    drawBody();
    drawArms();
    drawLegs();
    glPopMatrix();
}

// Fungsi untuk update animasi berlari yang lebih smooth
void updateRunningAnimation() {
    if (isWalking) {
        // Update ayunan kaki dengan kecepatan yang lebih cepat
        if (legDirection) {
            legSwing += 15.0f; // lebih cepat dari sebelumnya
            if (legSwing > 35.0f) legDirection = false;
        } else {
            legSwing -= 15.0f;
            if (legSwing < -35.0f) legDirection = true;
        }
        
        // Update ayunan tangan (berlawanan dengan kaki)
        if (armDirection) {
            armSwingLeft += 17.0f;
            armSwingRight -= 13.0f;
            if (armSwingLeft > 35.0f) armDirection = false;
        } else {
            armSwingLeft -= 17.0f;
            armSwingRight += 13.0f;
            if (armSwingLeft < -35.0f) armDirection = true;
        }
    } else {
        // Kembalikan ke posisi normal saat tidak berlari
        if (legSwing > 0.0f) {
            legSwing -= 2.0f;
            if (legSwing < 0.0f) legSwing = 0.0f;
        } else if (legSwing < 0.0f) {
            legSwing += 2.0f;
            if (legSwing > 0.0f) legSwing = 0.0f;
        }
        
        // Reset ayunan tangan
        if (armSwingLeft > 0.0f) {
            armSwingLeft -= 2.0f;
            if (armSwingLeft < 0.0f) armSwingLeft = 0.0f;
        } else if (armSwingLeft < 0.0f) {
            armSwingLeft += 2.0f;
            if (armSwingLeft > 0.0f) armSwingLeft = 0.0f;
        }
        
        if (armSwingRight > 0.0f) {
            armSwingRight -= 2.0f;
            if (armSwingRight < 0.0f) armSwingRight = 0.0f;
        } else if (armSwingRight < 0.0f) {
            armSwingRight += 2.0f;
            if (armSwingRight > 0.0f) armSwingRight = 0.0f;
        }
    }
}

// Fungsi untuk animasi lompat saat ganti jalur
void updateLaneChangeAnimation() {
    if (isChangingLane) {
        laneChangeProgress += 0.1f;
        
        // Kurva lompatan parabolik
        if (laneChangeProgress < 0.5f) {
            laneChangeHeight = laneChangeProgress * 2.0f; // naik
        } else {
            laneChangeHeight = (1.0f - laneChangeProgress) * 2.0f; // turun
        }
        
        // Selesai animasi lompat
        if (laneChangeProgress >= 1.0f) {
            laneChangeProgress = 0.0f;
            laneChangeHeight = 0.0f;
            isChangingLane = false;
            lane = targetLane;
        }
    }
}

// Modifikasi fungsi keyboardSpecial untuk menambah animasi lompat
void keyboardSpecialEnhanced(int key, int x, int y) {
    if (isChangingLane) return; // Tidak bisa ganti jalur saat masih animasi
    
    switch (key) {
        case GLUT_KEY_LEFT:
            if (lane > 0) {
                targetLane = lane - 1;
                isChangingLane = true;
                laneChangeProgress = 0.0f;
            }
            break;
        case GLUT_KEY_RIGHT:
            if (lane < 2) {
                targetLane = lane + 1;
                isChangingLane = true;
                laneChangeProgress = 0.0f;
            }
            break;
    }
    glutPostRedisplay();
}

void drawBird(float wingAngle, float r, float g, float b) {
    // ?? Warna badan
    glColor3f(r, g, b);
    glutSolidSphere(0.3f, 16, 16);

    // Mata (hitam)
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix(); glTranslatef(0.15f, 0.1f, 0.25f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.15f, 0.1f, 0.25f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();

    // Sayap kiri
    glPushMatrix();
    glRotatef(-wingAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b); // ?? Panggil ulang sebelum menggambar sayap
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.3f, 0.1f, 0.0f);
    glVertex3f(-1.0f, 0.15f, 0.0f);
    glVertex3f(-0.3f, -0.1f, 0.0f);
    glEnd();
    glPopMatrix();

    // Sayap kanan
    glPushMatrix();
    glRotatef(wingAngle, 0.0f, 0.0f, 1.0f);
    glColor3f(r, g, b); // ?? Panggil ulang lagi
    glBegin(GL_TRIANGLES);
    glVertex3f(0.3f, 0.1f, 0.0f);
    glVertex3f(1.0f, 0.15f, 0.0f);
    glVertex3f(0.3f, -0.1f, 0.0f);
    glEnd();
    glPopMatrix();
}

void spawnBirds(int count = 10) {
    for (int i = 0; i < count; ++i) {
        Bird b;
        b.x = -30.0f + rand() % 60;
        b.y = b.y = 6.5f + (rand() % 20) / 5.0f; // Y = 6.5 - 10.5, terlihat tapi tidak ganggu
        b.z = baymaxPosZ - (rand() % 100 + 20); 

        b.dx = 0.01f + (rand() % 10) / 1000.0f;
        b.dy = (rand() % 2 == 0) ? 0.0005f : -0.0005f;
        b.dz = 0.02f + (rand() % 10) / 1000.0f;

        b.colorR = (rand() % 100) / 100.0f;
        b.colorG = (rand() % 100) / 100.0f;
        b.colorB = (rand() % 100) / 100.0f;

        birds.push_back(b);
    }
}

void drawBirds() {
    float wingAngle = fabs(sin(glutGet(GLUT_ELAPSED_TIME) / 100.0f)) * 45.0f;

    for (int i = 0; i < birds.size(); ++i) {
        Bird& b = birds[i];
        b.x += b.dx;
        b.y += b.dy;
        b.z += b.dz;

        // Loop ulang ke belakang saat melewati Z
        if (b.z > 50.0f) {
            b.z = -150.0f;
            b.x = -30.0f + rand() % 60;
            b.y = 6.0f + (rand() % 30) / 5.0f;
        }

        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        drawBird(wingAngle, b.colorR, b.colorG, b.colorB);
        glPopMatrix();
    }

    // Spawn baru jika jumlah terlalu sedikit
    if (birds.size() < 30) {
        spawnBirds(5);
    }
}


/////////////////////////////////START////////////////////

// Fungsi untuk menggambar heart/love
void drawHeart(float x, float y, float size) {
    glBegin(GL_POLYGON);
    // Gambar heart sederhana menggunakan polygon
    // Bagian kiri atas
    glVertex2f(x, y + size * 0.5f);
    glVertex2f(x - size * 0.3f, y + size * 0.8f);
    glVertex2f(x - size * 0.5f, y + size * 0.5f);
    glVertex2f(x - size * 0.5f, y + size * 0.2f);
    glVertex2f(x - size * 0.3f, y);
    
    // Bagian tengah bawah
    glVertex2f(x, y - size * 0.8f);
    
    // Bagian kanan
    glVertex2f(x + size * 0.3f, y);
    glVertex2f(x + size * 0.5f, y + size * 0.2f);
    glVertex2f(x + size * 0.5f, y + size * 0.5f);
    glVertex2f(x + size * 0.3f, y + size * 0.8f);
    glEnd();
}

// Alternatif heart yang lebih sederhana menggunakan circles
void drawSimpleHeart(float x, float y, float size) {
    // Lingkaran kiri
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x - size * 0.25f + cos(angle) * size * 0.25f, 
                   y + size * 0.25f + sin(angle) * size * 0.25f);
    }
    glEnd();
    
    // Lingkaran kanan
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + size * 0.25f + cos(angle) * size * 0.25f, 
                   y + size * 0.25f + sin(angle) * size * 0.25f);
    }
    glEnd();
    // Segitiga bawah
    glBegin(GL_TRIANGLES);
    glVertex2f(x - size * 0.4f, y);
    glVertex2f(x + size * 0.4f, y);
    glVertex2f(x, y - size * 0.6f);
    glEnd();
}

void drawUI() {
    // Setup 2D mode
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);  // Ubah ke ukuran window kamu
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Nonaktifkan depth test agar selalu tampil di atas
    glDisable(GL_DEPTH_TEST);
    
    // === BARIS ATAS: SCORE ===
    glColor3f(1, 1, 0);  // Kuning
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    renderBitmapString(10, 570, GLUT_BITMAP_HELVETICA_18, scoreText);
    
    // === BARIS KEDUA: HEALTH TEXT + HEARTS (SEJAJAR DENGAN SCORE) ===
    float scoreY = 546;  // Posisi Y yang sama dengan Score untuk hearts
    
    // Label "Health:" di sebelah kiri
    glColor3f(1, 1, 1);  // Putih
    renderBitmapString(10, 540, GLUT_BITMAP_HELVETICA_18, "Health:");
    
    // Health Hearts - Sejajar dengan angka di "Score: 0"
    float heartSize = 15.0f;
    float heartSpacing = 30.0f;
    float startX = 60.0f;  // Posisi X sejajar dengan angka score
    
    // Gambar heart berdasarkan health yang tersisa
    for (int i = 0; i < 3; i++) {
        if (i < health) {
            // Heart penuh (merah)
            glColor3f(1, 0, 0);
        } else {
            // Heart kosong (abu-abu)
            glColor3f(0.3f, 0.3f, 0.3f);
        }
        
        // Pilih salah satu fungsi heart di bawah ini:
        drawHeart(startX + i * heartSpacing, scoreY, heartSize);
        // atau gunakan: drawHeart(startX + i * heartSpacing, scoreY, heartSize);
    }
    
    // Aktifkan kembali depth test
    glEnable(GL_DEPTH_TEST);
    
    // Kembalikan mode matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
void renderStrokeString(void* font, const char* string) {
	while (*string) {
		glutStrokeCharacter(font, *string++);
	}
}

void drawGameOverScreen() {
    // Set latar belakang abu gelap
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Nonaktifkan efek 3D (untuk teks 2D)
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);  // Warna teks putih cerah

    // TULISAN GAME OVER
    const char* gameOverText = "GAME OVER";
    int textWidth = getStrokeStringWidth(GLUT_STROKE_ROMAN, gameOverText);
    glPushMatrix();
    glTranslatef((800 - textWidth * 0.3f) / 2.0f, 350, 0.0f);
    glScalef(0.3f, 0.3f, 0.3f);
    renderStrokeString(GLUT_STROKE_ROMAN, gameOverText);
    glPopMatrix();

    // TOTAL SKOR
    char finalScore[50];
    sprintf(finalScore, "Total Score: %d", score);
    int scoreWidth = getStrokeStringWidth(GLUT_STROKE_ROMAN, finalScore);
    glPushMatrix();
    glTranslatef((800 - scoreWidth * 0.2f) / 2.0f, 250, 0.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    renderStrokeString(GLUT_STROKE_ROMAN, finalScore);
    glPopMatrix();

    // Aktifkan kembali lighting dan depth untuk scene berikutnya
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

void drawStartScreen() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // background gelap
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
   // Judul - ditengah
	const char* title = "BAYMAX RUNNER 3D";
	int titleWidth = getStrokeStringWidth(GLUT_STROKE_ROMAN, title);
	glPushMatrix();
	glTranslatef((800 - titleWidth * 0.3f) / 2.0f, 400, 0.0f);
	glScalef(0.3f, 0.3f, 1.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, title);
	glPopMatrix();
	
	// Petunjuk - ditengah
	const char* subtitle = "Press SPACE to Start";
	int subtitleWidth = getStrokeStringWidth(GLUT_STROKE_ROMAN, subtitle);
	glPushMatrix();
	glTranslatef((800 - subtitleWidth * 0.2f) / 2.0f, 280, 0.0f);
	glScalef(0.2f, 0.2f, 1.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, subtitle);
	glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y) {
    if (!isGameStarted && key == 32) { // Tekan spasi untuk mulai
        isGameStarted = true;

        init();  

        glutTimerFunc(0, timerEnhanced, 0);
        return;
    }

    if (key == 32 && !isJumping && isGameStarted) {
        isJumping = true;
    }
}

void keyboardSpecial(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            if (lane > 0) lane--; // pindah ke kiri
            break;
        case GLUT_KEY_RIGHT:
            if (lane < 2) lane++; // pindah ke kanan
            break;
    }
    glutPostRedisplay();
}


void display() {
    if (!isGameStarted) {
        drawStartScreen();
        return;
    }

    if (gameOver) {
        drawGameOverScreen();
        return;
    }

	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // === Kamera mengikuti Baymax ===
    float camOffsetX = sin(baymaxAngle * PI / 180.0f) * cameraDistance;
    float camOffsetZ = cos(baymaxAngle * PI / 180.0f) * cameraDistance;

    float camX = baymaxPosX + camOffsetX;
    float camY = cameraHeight; // Tinggi kamera
    float camZ = baymaxPosZ + camOffsetZ;

    gluLookAt(
        camX, camY, camZ,               // Posisi kamera
        baymaxPosX, 2.0f, baymaxPosZ,   // Titik yang dilihat kamera (ke arah Baymax)
        0.0f, 1.0f, 0.0f                // Arah atas
    );

    // === Gambar lantai ===
    drawGround();

    // === Gambar coin berputar di tengah ===
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glScalef(0.6f, 0.6f, 0.6f);
    glColor3f(1.0f, 0.85f, 0.0f);
    drawCoin(0.8f, 0.15f, 100, true);
    glColor3f(1.0f, 0.65f, 0.0f);
    drawStar(0.3f, 0.075f, 0.03f);
    glPopMatrix();

	if (isPoisoned || recentlyHit) {  // recentlyHit nanti kita tambahkan
	    blink = !blink;
	    if (blink)
	        glDisable(GL_LIGHTING);
	    else
	        glEnable(GL_LIGHTING);
	} else {
	    glEnable(GL_LIGHTING);
	}


    // === Gambar Baymax ===
    glPushMatrix();
    glTranslatef(baymaxPosX, 2.0f, baymaxPosZ);      // Posisi Baymax
    glRotatef(baymaxAngle, 0.0f, 1.0f, 0.0f);         // Arah hadap
    glScalef(0.9f, 0.9f, 0.9f);
    drawBaymax();
    glPopMatrix();
	glEnable(GL_LIGHTING);

    // === Awan ===
    glPushMatrix();
	drawCloudsInfinite(baymaxPosZ); 
    glPopMatrix();
    drawBirds();


    // === Pohon ===
	drawTreesInfinite(quadric, baymaxPosZ);

	drawBuildingsInfinite(baymaxPosZ);


	// === Gedung bertingkat ===
	// === Awan ===
	drawCloudsInfinite(baymaxPosZ);
	


    // === Batu ===
    glPushMatrix();
    glTranslatef(-3.0f, 0.0f, -2.0f);
    drawRock(0.4f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.0f, 0.0f, 3.0f);
    drawRock(0.3f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-7.0f, 0.0f, 7.0f);
    drawRock(0.5f);
    glPopMatrix();

    // === Bom ===
    glPushMatrix();
    glTranslatef(2.0f, 0.4f, 1.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    drawBomb();
    glPopMatrix();

    // === UI (score, health) ===
    drawUI();
    renderCoins();
    renderBombs();
    renderMagnets();
	renderPoisons();
	renderRocks();


    glutSwapBuffers();

}


void checkCoinCollision() {
    if (gameOver) return; // Jangan cek jika sudah game over

    for (int i = 0; i < coins.size(); ++i) {
        float dx = baymaxPosX - coins[i][0];
        float dz = baymaxPosZ - coins[i][2];
        float distance = sqrt(dx * dx + dz * dz);

        if (distance < 1.0f) {
            score += 1;
            coins.erase(coins.begin() + i);
            break;
        }
    }
}

void checkBombCollision() {
    if (gameOver) return;

    for (int i = 0; i < bombs.size(); ++i) {
        float dx = baymaxPosX - bombs[i][0];
        float dy = baymaxPosY - bombs[i][1];  // jika Baymax melompat
        float dz = baymaxPosZ - bombs[i][2];
        float distance = sqrt(dx * dx + dy * dy + dz * dz);

        if (distance < 1.0f) {
            if (health > 0) {
                health -= 1;
                if (health < 0) health = 0;
            }

            // ? Tambahkan ini:
            recentlyHit = true;
            hitBlinkTimer = 0.0f;

            bombs.erase(bombs.begin() + i);
            break;
        }
    }
}



void updateCoins() {
    // Hapus koin yang sudah jauh di belakang Baymax
    for (int i = 0; i < coins.size(); ++i) {
        if (coins[i][2] > baymaxPosZ + 5.0f) {
            coins.erase(coins.begin() + i);
            --i; // karena indeks bergeser setelah erase
        }
    }

    // Tambah koin baru jika kurang dari 20
    while (coins.size() < 20) {
        float laneX[] = {-2.0f, 0.0f, 2.0f};
        int laneIndex = rand() % 3;
        float x = laneX[laneIndex];
        float y = 1.5f;
        float z = baymaxPosZ - (rand() % 100 + 30); // di depan Baymax
        std::vector<float> coin;
        coin.push_back(x);
        coin.push_back(y);
        coin.push_back(z);
        coins.push_back(coin);
    }
}

void updateBombs() {
    // Hapus bom yang sudah dilewati Baymax
    for (int i = 0; i < bombs.size(); ++i) {
        if (bombs[i][2] > baymaxPosZ + 5.0f) {
            bombs.erase(bombs.begin() + i);
            --i;
        }
    }

    // Tambah bom baru secara acak jika kurang dari 5
    while (bombs.size() < 5) {
        // 25% kemungkinan untuk tambahkan bom
        if (rand() % 4 == 0) {
            float laneX[] = {-2.0f, 0.0f, 2.0f};
            int laneIndex = rand() % 3;
            float x = laneX[laneIndex];
            float y = 0.4f;  // tinggi bom
            float z = baymaxPosZ - (rand() % 100 + 30); // jauh di depan Baymax

            std::vector<float> bomb;
            bomb.push_back(x);
            bomb.push_back(y);
            bomb.push_back(z);
            bombs.push_back(bomb);
        } else {
            break; // keluar loop jika tidak dapat acak
        }
    }
}



void timer(int value) {
    angle += 2.0f;
    cameraAngle += 0.3f;

    if (angle > 360.0f) angle -= 360.0f;
    if (cameraAngle > 360.0f) cameraAngle -= 360.0f;

    if (isAutoRunning) {
        baymaxPosZ -= runSpeed;
        isWalking = true;
    }

    float targetX = lanePositions[lane];
    if (fabs(baymaxPosX - targetX) > 0.01f) {
        if (baymaxPosX < targetX)
            baymaxPosX += slideSpeed;
        else if (baymaxPosX > targetX)
            baymaxPosX -= slideSpeed;
    }

    if (isWalking) {
        if (legDirection) {
            legSwing += 2.0f;
            if (legSwing > 20.0f) legDirection = false;
        } else {
            legSwing -= 2.0f;
            if (legSwing < -20.0f) legDirection = true;
        }
    } else {
        if (legSwing > 0.0f) {
            legSwing -= 2.0f;
            if (legSwing < 0.0f) legSwing = 0.0f;
        } else if (legSwing < 0.0f) {
            legSwing += 2.0f;
            if (legSwing > 0.0f) legSwing = 0.0f;
        }
    }

    isWalking = false;
    updateJump();

    // ? TAMBAHKAN DI SINI:
    checkCoinCollision();
    checkBombCollision();
    updateCoins();
    updateBombs();
    updateRocks(); 
    
    
    if (health <= 0 && !gameOver) {
        gameOver = true;
    }

    if (gameOver) {
        return; // Hentikan update jika sudah game over
    }


    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}


// Modifikasi fungsi timer untuk include animasi baru
void timerEnhanced(int value) {
    angle += 2.0f;
    cameraAngle += 0.3f;
    static int lastTime = glutGet(GLUT_ELAPSED_TIME); // waktu sebelumnya
    int currentTime = glutGet(GLUT_ELAPSED_TIME);     // waktu sekarang
    float deltaTime = (currentTime - lastTime) / 1000.0f; // selisih dalam detik
    lastTime = currentTime;

    if (angle > 360.0f) angle -= 360.0f;
    if (cameraAngle > 360.0f) cameraAngle -= 360.0f;

    if (isAutoRunning) {
        baymaxPosZ -= runSpeed;
        isWalking = true;
    }

    // Update posisi X dengan animasi lompat saat ganti jalur
    if (!isChangingLane) {
        float targetX = lanePositions[lane];
        if (fabs(baymaxPosX - targetX) > 0.01f) {
            if (baymaxPosX < targetX)
                baymaxPosX += slideSpeed;
            else if (baymaxPosX > targetX)
                baymaxPosX -= slideSpeed;
        }
    } else {
        // Interpolasi smooth saat ganti jalur
        float startX = lanePositions[lane];
        float endX = lanePositions[targetLane];
        baymaxPosX = startX + (endX - startX) * laneChangeProgress;
    }

    // Update semua animasi
    updateRunningAnimation();
    updateLaneChangeAnimation();
    updateJump();

    // Collision detection dan update game objects
    checkCoinCollision();
    checkBombCollision();
    updateCoins();
    updateBombs();
    updateMagnets();

    
    if (health <= 0 && !gameOver) {
        gameOver = true;
    }

    if (gameOver) {
        return;
    }
	// Tambah kecepatan lari secara bertahap
	if (runSpeed < 1.0f) {
	    runSpeed += 0.0005f; // makin lama makin cepat, tapi perlahan
	}
	// Update magnet
	if (magnetActive) {
	    magnetTimer += deltaTime;
	    
	    // Tarik koin dalam radius
	    for (int i = 0; i < coins.size(); ++i) {
	        float dx = baymaxPosX - coins[i][0];
	        float dz = baymaxPosZ - coins[i][2];
	        float dist = sqrt(dx * dx + dz * dz);
	
	        if (dist < magnetRadius) {
	            // Interpolasi koin menuju Baymax
	            coins[i][0] += dx * 0.15f;
	            coins[i][2] += dz * 0.15f;
	        }
	    }
	
	    if (magnetTimer > magnetDuration) {
	        magnetActive = false;
	    }
	}

	if (recentlyHit) {
	    hitBlinkTimer += deltaTime;
	    if (hitBlinkTimer > 1.0f) { // 1 detik kedip-kedip
	        recentlyHit = false;
	    }
	}

    isWalking = false; // Reset setelah update animasi
    glutPostRedisplay();
    int frameTime = glutGet(GLUT_ELAPSED_TIME) - currentTime;
    int delay = 16 - frameTime; // target 60 FPS (1000/60 = ~16ms per frame)
    if (delay < 0) delay = 0;

    glutTimerFunc(16, timerEnhanced, 0);
    
	checkMagnetCollision();
	checkPoisonCollision();
	
	if (isPoisoned) {
	    poisonTimer += deltaTime;
	    if (poisonTimer > poisonDuration) {
	        isPoisoned = false;
	    }
	}

}

void init() {
	generateMagnets();
	generatePoisons();
	generateBuildings();
    // Setup background color (sky blue gradient)
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    
    // Enable depth testing and smooth shading
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    
    // Setup lighting
    setupLighting();
    
    // Setup fog for atmospheric effect
    glEnable(GL_FOG);
    GLfloat fogColor[] = {0.6f, 0.8f, 1.0f, 1.0f};
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.02f);
    glFogf(GL_FOG_START, 15.0f);
    glFogf(GL_FOG_END, 35.0f);
    
    // Setup quadric for sphere rendering
    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    
    // Setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 800.0 / 600.0, 0.1, 100.0);
    
    //awan
    glMatrixMode(GL_MODELVIEW);
    spawnBirds(50); // inisialisasi awal
	srand(time(NULL));

    //koin
    generateCoins(100); // misalnya 100 koin
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    float aspect = (float)width / (float)height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(60.0f, aspect, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("3D Game World - Third Person View");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timerEnhanced, 0);
    glutSpecialFunc(keyboardSpecial); 
    glutKeyboardFunc(keyboard);

    
    glutMainLoop();
    return 0;
}

int getStrokeStringWidth(void* font, const char* str) {
    int width = 0;
    while (*str) {
        width += glutStrokeWidth(font, *str);
        str++;
    }
    return width;
}

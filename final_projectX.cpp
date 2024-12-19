#include "iGraphics.h"
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

// Screen Dimensions
const int screenWidth = 1280, screenHeight = 720;
int gameState = 0;
int gamePhase = 0;

// introducing gravity;
int tme = 1;
int t1, t2, t3, t4, t5, t6, t7, t8;
int showTime = 60;
int timeMin = 2;
/*
0= gameStarting
1= mainMenu
2=startGame
3=gameStory
4=highScoreShow
5=gameSettings
6=instructions
7=credits
8=quit
9=nameInput

have to introduce gamePhases
10.gamePhase0
11.gamePhase0
12.gamePhase0
*/

// secrete code
bool sCodeIsActive = false;
bool allBossDed = false;
bool missionComplete = false;

// Structs
struct Player
{
    int x, y, health, score;
    int frameIndex, flyingFrameIndex, rotationIndex;
    bool isAlive, isFlying, isRotating, iswalking;
    char walkingFrames[4][100];
    char flyingFrames[2][100];
    char rotatingFrames[12][100];
    double rotationAngle;
};


struct Enemy
{
    int x, y, health, flip;
    int walkingframeIndex, ufoFrameIndex, bossFrameIndex, bossFlyUpdate;
    bool isAlive;
    char walkingFrames[4][100];
    char ufoFrames[9][100];
    char bossFrames[28][100];
    double rotationAngle;
};

struct Bullet
{
    double x, y, velocity; // Bullet position
    double velX, velY;     // Bullet velocity in x and y directions
    bool isActive;         // Whether the bullet is active or not
};

struct Obstacle
{
    int x, y, track, dx, dy;
    bool isActive;
};
const int totalObstacles = 16;
const int totalObstacles_1 = 14;
Obstacle obs[totalObstacles];
Obstacle obs_1[totalObstacles_1];
struct HighScore
{
    char name[100];
    int score;
};

// Global Variables
Player player; // = {50, 10, 1000, 0, 0, 0, 0, true, false, false, true};
// Enemy enemy = {1350, 10, 5, 0, 0, false,false};
Enemy enemy[5];     // zombie as a enemy
Enemy enemyUfo[9];  // ufo as a enemy
Enemy enemyBoss[3]; // boss enemy
char eDeadE[6][100];
char eUfoE[6][100];
int eDeadi = 0;
int eUfoi = 0;
const int ammo = 100;
Bullet bullets[ammo];     // player bullets
int bulletIndex = 0;      // player bullets Index;
Bullet enemyBullets[100]; // enemy bullets
int eBidx = 0;            // enemy bullet index
Bullet ufoBullets[100];   // ufo bullets
int uBidx = 0;            // enemy ufo index
Bullet bossBullets[100];
int bBidx = 0;
double bulletSpeed = 10.0;
int mouseX = 0, mouseY = 0;
const int totalRotationFrames = 12;
const double anglePerFrame = 360.0 / totalRotationFrames;

bool musicOn = true; // musicOn
bool shootBgm = false;
bool enemyDeadM = false;
bool ufoExplodeM = false;

// Global High Scores Array
const int maxHighScores = 5;
HighScore highScores[maxHighScores];
char playerName[100] = "";
int nameIndex = 0;

// Background
const int totalBackgroundFrames = 126;
char backgroundFrames[126][100];
int currentBackgroundFrame = 0;

// Function Prototypes
void loadPlayerAssets();
void loadEnemyVariables();
void loadEnemyAssets();
void loadObstacle();
void loadBackgroundFrames();
void drawPlayer();
void updatePlayerRotation();
double calculateAngle(int playerX, int playerY, int mouseX, int mouseY);
void playerFlying();
void drawEnemy();
void moveEnemy();
void shootEnemy();
void drawObstacles();
void obstacleCollison();

void playerHealthcheck();
void enemyHealthcheck();
void bulletHitDetect();
int collisonCheck(int x, int y, int p, int q, int s, int t);
void moveBullets();
void drawBullets();
void gameReset();

void gameStarting();
void mainMenu();
void mainMenuSetup(int button, int state, int mx, int my);
void mainGame();
void gameStory();
void highScoreShow();
void gameSettings();
void instructions();
void credits();
void quit();
void backButton();

void loadHighScores();
void saveHighScores();
void updateHighScores(const char *playerName, int playerScore);
void displayHighScores();
void displayPlayerNameInput();
void jump();
void playerRateOfFire();
void displayeTime();
void enemyDeadEffect();
void ufoDeadEffect();
void gameOverScene();
void playerDeadScene();

int qFlag = -1;
double gU = 5;
double g = 0.63;
void jump()
{
    if (qFlag % 2 != 0)
    {
        player.y = player.y + abs(int(gU * tme - .5 * g * tme * tme));
        player.x += 1;
        for (int j = 0; j < totalObstacles; j++)
        {
            if (collisonCheck(player.x, player.y, obs[j].x - obs[j].track - 90, obs[j].y, 90 + obs[j].dx, 15))
            {
                player.y = obs[j].y + obs[j].dy;
                player.x -= 1;
                tme = 1;
            }
        }
    }
    else if (qFlag % 2 != 1)
    {
        player.y = player.y - int(gU * tme) + int(.5 * g * tme * tme);
        player.x += 1;
        for (int j = 0; j < totalObstacles; j++)
        {
            if (collisonCheck(player.x, player.y, obs[j].x - obs[j].track - 90, obs[j].y, 90 + obs[j].dx, 15))
            {
                player.y = obs[j].y + obs[j].dy;
                player.x -= 1;
                tme = 1;
            }
        }
    }
    if (player.y <= 10)
    {
        player.y = 10;
        qFlag = -1;
        tme = 1;
        iPauseTimer(t1);
    }
    tme++;
    if (tme == 10)
    {
        tme = 1;
        qFlag++;
        if (!collisonCheck(player.x, player.y, obs[0].x - obs[0].track - 90, obs[0].y, 90 + obs[0].dx, 15) && player.y >= 10 && qFlag % 2 != 0)
        {
            qFlag--;
            tme++;
        }
    }
}

// Initialization
void initializeGame()
{
    srand(time(NULL));
    loadObstacle();
    loadBackgroundFrames();
    loadPlayerAssets();
    loadEnemyVariables();
    loadEnemyAssets();
    loadHighScores();
}

// Load Player Frames
void loadPlayerAssets()
{
    for (int i = 0; i < 4; i++)
    {
        sprintf(player.walkingFrames[i], "pictures\\walk\\tile%03d.bmp", i);
    }
    for (int i = 0; i < 2; i++)
    {
        sprintf(player.flyingFrames[i], "pictures\\fly\\tile%03d.bmp", i);
    }
    for (int i = 0; i < 12; i++)
    {
        sprintf(player.rotatingFrames[i], "pictures\\rotate\\tile%03d.bmp", i);
    }
}

// Load Enemy Frames
void loadEnemyVariables()
{
    enemy[0] = {obs[3].x + obs[3].dx / 2, obs[3].y + obs[3].dy, 5, 0, 0, 0, 0, 0, true, 0};
    enemy[1] = {obs[5].x + obs[5].dx, 10, 5, 0, 0, 0, 0, 0, false, 0};
    enemy[2] = {obs[6].x + obs[6].dx / 2, obs[6].y + obs[6].dy, 5, 0, 0, 0, 0, 0, false, 0};
    enemy[3] = {obs[10].x + obs[10].dx, 10, 5, 0, 0, 0, 0, 0, false, 0};
    enemy[4] = {obs[14].x - 10, obs[14].y + obs[14].dy, 5, 0, 0, 0, 0, 0, false, 0};
    for (int i = 0; i < 9; i++)
    {
        enemyUfo[i] = {((rand() % 2560) / 1280) * 1280, 10 + rand() % 630, 10, 0, 0, 0, 0, 0, false, 0};
    }

    // for boss
    enemyBoss[0] = {640, 10, 20, 0, 0, 0, 0, 0, false, 0};
    enemyBoss[1] = {640, 540, 20, 0, 0, 0, 0, 0, false, 0};
    enemyBoss[2] = {1600, 10, 20, 0, 0, 0, 0, 0, false, 0};
}
void loadEnemyAssets()
{
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            sprintf(enemy[j].walkingFrames[i], "pictures\\enemy_walk\\tile%03d.bmp", i);
        }
    }
    for (int i = 0; i < 9; i++)
    {
        sprintf(enemyUfo[i].ufoFrames[i], "pictures\\UFO\\tile%03d.bmp", i);
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            sprintf(enemyBoss[i].bossFrames[j], "pictures\\boss\\tile%03d.bmp", j);
        }
    }
    for (int i = 0; i < 6; i++)
    {
        sprintf(eDeadE[i], "pictures\\enemyDeadEffect\\%d.bmp", i);
        sprintf(eUfoE[i], "pictures\\enemyDeadEffect\\ufo\\tile%03d.bmp", i);
    }
}
void loadObstacle()
{
    // obstacles for gamephase 0
    obs[0] = {630, 10, 0, 10, 80, true};
    obs[1] = {640, 70, 0, 100, 20, true};
    obs[2] = {740, 10, 0, 10, 160, true};
    obs[3] = {750, 150, 0, 100, 20, true};
    obs[4] = {850, 110, 0, 10, 60, true};
    obs[5] = {860, 110, 0, 100, 20, true};
    obs[6] = {1100, 90, 0, 80, 20, true};
    obs[7] = {1180, 10, 0, 10, 200, true};
    obs[8] = {1190, 190, 0, 60, 20, true};
    obs[9] = {1250, 10, 0, 10, 200, true};
    obs[10] = {1260, 130, 0, 150, 20, true};
    // gamePhase 0 ->1
    obs[11] = {1850, 10, 0, 20, 310, true};
    obs[12] = {1850, 410, 0, 20, 310, true};

    obs[13] = {1450, 180, 0, 120, 20, true};
    obs[14] = {1640, 220, 0, 120, 20, true};
    obs[15] = {1700, 260, 0, 140, 20, true};

    // obstacles for gamephase 1
    // gamePhase 1 -> 2
    obs_1[0] = {1850, 10, 0, 20, 310, true};
    obs_1[1] = {1850, 410, 0, 20, 310, true};

    // enemy box 1
    obs_1[2] = {640, 200, 0, 200, 20, true};
    obs_1[3] = {620, 10, 0, 20, 210, true};
    obs_1[4] = {840, 10, 0, 20, 210, true};

    // enemy box 2
    obs_1[5] = {640, 520, 0, 200, 20, true};
    obs_1[6] = {620, 520, 0, 20, 210, true};
    obs_1[7] = {840, 520, 0, 20, 210, true};

    // enemy box 3
    obs_1[8] = {1600, 200, 0, 200, 20, true};
    obs_1[9] = {1580, 10, 0, 20, 210, true};
    obs_1[10] = {1800, 10, 0, 20, 210, true};

    // main Game key
    obs_1[11] = {1600, 520, 0, 200, 20, true};
    obs_1[12] = {1580, 520, 0, 20, 210, true};
    obs_1[13] = {1800, 520, 0, 20, 210, true};
}
void loadBackgroundFrames()
{
    for (int i = 0; i < totalBackgroundFrames; i++)
    {
        sprintf(backgroundFrames[i], "pictures\\%d.bmp", i);
    }
}

// Draw Player
void drawPlayer()
{
    if (player.iswalking && gamePhase == 0)
    {
        iShowBMP2(player.x, player.y, player.walkingFrames[player.frameIndex], 0);
    }
    else if ((gamePhase == 1 || gamePhase == 2) && player.isRotating)
    {
        iShowBMP2(player.x, player.y, player.rotatingFrames[player.rotationIndex], 0);
    }
}
void updatePlayerRotation()
{
    // for player
    player.rotationAngle = calculateAngle(player.x, player.y, mouseX, mouseY);                // Calculate angle
    player.rotationIndex = (int)(player.rotationAngle / anglePerFrame) % totalRotationFrames; // Map angle to frame
    player.rotationIndex = (player.rotationIndex + 3) % totalRotationFrames;                  // Adjust speed by offset
    // for boss
    for (int i = 0; i < 3; i++)
    {
        if (enemyBoss[i].isAlive)
        {
            enemyBoss[i].rotationAngle = calculateAngle(player.x, player.y, enemyBoss[i].x - obs_1[0].track + 60, enemyBoss[i].y + 115) + 240; // Calculate angle
            enemyBoss[i].bossFrameIndex = (2 * (int)(enemyBoss[i].rotationAngle / (360 / 14))) % 28;
            if (enemyBoss[i].bossFrameIndex > 14)
            {
                enemyBoss[i].bossFrameIndex += 2;
                enemyBoss[i].bossFrameIndex = enemyBoss[i].bossFrameIndex % 28;
            }
            else
            {
                enemyBoss[i].bossFrameIndex += 2;
                enemyBoss[i].bossFrameIndex = enemyBoss[i].bossFrameIndex % 28;
            }
        }
    }
    // Map angle to frame
    // enemyBoss.rotationIndex = (player.rotationIndex + 3) % totalRotationFrames;
}
void playerFlying()
{
    if (player.isFlying || player.isRotating) //&& press_right % 2 == 0
    {
        player.flyingFrameIndex++;
        player.flyingFrameIndex = player.flyingFrameIndex % 2;
        player.y--;
        player.isRotating = true;
        if (player.y <= 10)
        {
            player.y = 10;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (enemyBoss[i].isAlive)
        {
            enemyBoss[i].bossFlyUpdate++;
            enemyBoss[i].bossFlyUpdate = enemyBoss[i].bossFlyUpdate % 2;
        }
    }
}

// Move Bullets
void moveBullets()
{
    // for player
    for (int i = 0; i < ammo; i++)
    {
        if (bullets[i].isActive)
        {
            if (gamePhase == 0)
            {
                bullets[i].x += bulletSpeed;
                for (int j = 0; j < totalObstacles; j++)
                {
                    if (bullets[i].x < 0 || bullets[i].x > screenWidth || bullets[i].y < 0 || bullets[i].y > screenHeight ||
                        collisonCheck(bullets[i].x, bullets[i].y, obs[j].x - obs[j].track, obs[j].y, obs[j].dx, obs[j].dy))
                    { //
                        //
                        bullets[i].isActive = false;
                    }
                }
            }
            else
            {
                bullets[i].x += bullets[i].velX;
                bullets[i].y += bullets[i].velY;
                for (int j = 0; j < totalObstacles_1; j++)
                {
                    if (bullets[i].x < 0 || bullets[i].x > screenWidth || bullets[i].y < 0 || bullets[i].y > screenHeight ||
                        (collisonCheck(bullets[i].x, bullets[i].y, obs_1[j].x - obs_1[j].track, obs_1[j].y, obs_1[j].dx, obs_1[j].dy) && obs_1[j].isActive))
                    {
                        bullets[i].isActive = false;
                    }
                }
            }
        }
    }
    // ENEMY BULLETS
    for (int i = 0; i < 100; i++)
    {
        if (enemyBullets[i].isActive)
        {

            enemyBullets[i].x -= bulletSpeed;
            for (int j = 0; j < totalObstacles; j++)
            {
                if (enemyBullets[i].x < 0 || enemyBullets[i].x > screenWidth || enemyBullets[i].y < 0 || enemyBullets[i].y > screenHeight || collisonCheck(enemyBullets[i].x, enemyBullets[i].y, obs[j].x - obs[j].track, obs[j].y, obs[j].dx, obs[j].dy))
                {
                    enemyBullets[i].isActive = false;
                }
            }
        }
    }
    // UFO BULLETS
    for (int i = 0; i < 100; i++)
    {
        if (ufoBullets[i].isActive)
        {

            ufoBullets[i].x += ufoBullets[i].velX;
            ufoBullets[i].y += ufoBullets[i].velY;

            if (ufoBullets[i].x < 0 || ufoBullets[i].x > screenWidth || ufoBullets[i].y < 0 || ufoBullets[i].y > screenHeight)
            {
                ufoBullets[i].isActive = false;
            };
        }
    }
    // Boss bullets
    for (int i = 0; i < 100; i++)
    {
        if (bossBullets[i].isActive)
        {

            bossBullets[i].x += bossBullets[i].velX;
            bossBullets[i].y += bossBullets[i].velY;

            if (bossBullets[i].x < 0 || bossBullets[i].x > screenWidth || bossBullets[i].y < 0 || bossBullets[i].y > screenHeight)
            {
                bossBullets[i].isActive = false;
            };
        }
    }
}

// Draw Bullets
void drawBullets()
{
    // for player
    for (int i = 0; i < ammo; i++)
    {
        if (bullets[i].isActive)
        {
            iShowBMP2(bullets[i].x, bullets[i].y, "pictures\\bullet\\12.bmp", 0);
        }
    }
    // ENEMY BULLETS
    for (int i = 0; i < 100; i++)
    {
        if (enemyBullets[i].isActive)
        {
            iShowBMP2(enemyBullets[i].x, enemyBullets[i].y, "pictures\\bullet\\enemyB.bmp", 0);
        }
    }
    // UFO BULLETS
    for (int i = 0; i < 100; i++)
    {
        if (ufoBullets[i].isActive)
        {
            iShowBMP2(ufoBullets[i].x, ufoBullets[i].y, "pictures\\bullet\\enemyB.bmp", 0);
        }
    }
    // BOSS BULLETS
    for (int i = 0; i < 100; i++)
    {
        if (bossBullets[i].isActive)
        {
            iShowBMP2(bossBullets[i].x, bossBullets[i].y, "pictures\\bullet\\enemyB.bmp", 0);
        }
    }
}

// Draw Enemy
void drawEnemy()
{
    // draw zombie enemy
    for (int i = 0; i < 5; i++)
    {
        if (enemy[i].isAlive)
        {
            iShowBMP2(enemy[i].x - obs[0].track, enemy[i].y, enemy[i].walkingFrames[enemy[i].walkingframeIndex], 0);
        }
    }
    enemyDeadEffect();
    ufoDeadEffect();
    // draw ufo
    for (int i = 0; i < 9; i++)
    {
        if (enemyUfo[i].isAlive)
        {
            iShowBMP2(enemyUfo[i].x, enemyUfo[i].y, enemyUfo[i].ufoFrames[i], 0);
        }
    }
    // draw boss
    for (int i = 0; i < 3; i++)
    {
        if (enemyBoss[i].isAlive)
        {
            iShowBMP2(enemyBoss[i].x - obs_1[0].track, enemyBoss[i].y, enemyBoss[i].bossFrames[enemyBoss[i].bossFrameIndex + enemyBoss[i].bossFlyUpdate], 0);
        }
    }
}
void moveEnemy()
{
    // for zombie enemy
    for (int i = 0; i < 5; i++)
    {
        // if (enemy[i].isAlive)
        // {
        //     enemy[i].walkingframeIndex++;
        //     enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
        //     enemy[i].x = enemy[i].x - 5;
        // }
        switch (i)
        {
        case 0:
        {
            if (enemy[i].isAlive)
            {
                if (enemy[i].x >= obs[3].x + obs[3].dx - 30)
                    enemy[i].flip++;
                if (enemy[i].x + 20 <= obs[3].x)
                    enemy[i].flip--;
                if (enemy[i].flip == 1)
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x - 5;
                }
                else
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x + 5;
                }
            }
            break;
        }
        case 1:
        {
            if (enemy[i].isAlive)
            {
                if (enemy[i].x >= obs[6].x + obs[6].dx - 30)
                    enemy[i].flip++;
                if (enemy[i].x + 20 <= obs[5].x)
                    enemy[i].flip--;
                if (enemy[i].flip == 1)
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x - 5;
                }
                else
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x + 5;
                }
            }
            break;
        }
        case 2:
        {
            if (enemy[i].isAlive)
            {
                if (enemy[i].x >= obs[6].x + obs[6].dx - 30)
                    enemy[i].flip++;
                if (enemy[i].x + 20 <= obs[6].x)
                    enemy[i].flip--;
                if (enemy[i].flip == 1)
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x - 5;
                }
                else
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x + 5;
                }
            }
            break;
        }
        case 3:
        {
            if (enemy[i].isAlive)
            {
                if (enemy[i].x >= obs[11].x + obs[11].dx - 30)
                    enemy[i].flip++;
                if (enemy[i].x + 20 <= obs[10].x)
                    enemy[i].flip--;
                if (enemy[i].flip == 1)
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x - 5;
                }
                else
                {
                    enemy[i].walkingframeIndex++;
                    enemy[i].walkingframeIndex = enemy[i].walkingframeIndex % 4;
                    enemy[i].x = enemy[i].x + 5;
                }
            }
            break;
        }
        case 4:
        {
        }
            // default: break;
        }
    }
    // for ufo
    for (int i = 0; i < 9; i++)
    {
        if (enemyUfo[i].isAlive && enemyUfo[i].x >= 620)
        {
            enemyUfo[i].x = enemyUfo[i].x - 5;
        }
        else if (enemyUfo[i].isAlive && enemyUfo[i].x <= 620)
        {
            enemyUfo[i].x = enemyUfo[i].x + 5;
        }
    }
    // for boss
    for (int i = 0; i < 3; i++)
    {
        // while(enemyBoss[i].isAlive)
        if (enemyBoss[i].isAlive && sqrt(pow(player.x - enemyBoss[i].x + obs_1[0].track, 2) + pow(player.y - enemyBoss[i].y, 2)) > 100 && gamePhase == 1)
        {
            // enemyBoss.x = enemyBoss.x - 5;
            if (player.x > 840 - obs_1[0].track)
            {
                if (enemyBoss[i].x - obs_1[0].track >= player.x && enemyBoss[i].y >= player.y)
                {
                    enemyBoss[i].x -= 10;
                    enemyBoss[i].y -= 10;
                }
                else if (enemyBoss[i].x - obs_1[0].track <= player.x && enemyBoss[i].y >= player.y)
                {
                    enemyBoss[i].x += 10;
                    enemyBoss[i].y -= 10;
                }
                else if (enemyBoss[i].x - obs_1[0].track <= player.x && enemyBoss[i].y <= player.y)
                {
                    enemyBoss[i].x += 10;
                    enemyBoss[i].y += 10;
                }
                else
                {
                    enemyBoss[i].x -= 10;
                    enemyBoss[i].y += 10;
                }
            }
        }
    }
}

void shootEnemy()
{
    // for zombie enemy
    for (int i = 0; i < 5; i++)
    {
        if (enemy[i].isAlive && player.y + 100 >= enemy[i].y && player.y <= enemy[i].y + 80)
        {
            // if (shootBgm)
            // {
            //     PlaySound("D:\\Project_1_1\\music\\shootPlayer.wav", NULL, SND_ASYNC);
            // }
            enemyBullets[eBidx].x = enemy[i].x - obs[0].track;
            enemyBullets[eBidx].y = enemy[i].y + 42;
            enemyBullets[eBidx].isActive = true;
            eBidx++;
            eBidx = eBidx % 20;
        }
    }
    // for ufo
    for (int i = 0; i < 9; i++)
    {
        if (enemyUfo[i].isAlive && enemyUfo[i].x <= 1200)
        {
            ufoBullets[uBidx].x = enemyUfo[i].x + 60;
            ufoBullets[uBidx].y = enemyUfo[i].y + 42;
            ufoBullets[uBidx].isActive = true;
            // rotating bullet
            double dr = sqrt(pow(player.x - enemyUfo[i].x, 2) + pow(player.y - enemyUfo[i].y, 2));
            ufoBullets[uBidx].velX = ((player.x - enemyUfo[i].x) / dr) * bulletSpeed;
            ufoBullets[uBidx].velY = ((player.y - enemyUfo[i].y) / dr) * bulletSpeed;
            uBidx++;
            uBidx = uBidx % 100;
        }
    }
    // for boss enemy
    for (int i = 0; i < 3; i++)
    {
        if (enemyBoss[i].isAlive && enemyBoss[i].x - obs_1[0].track <= 1200)
        {
            bossBullets[bBidx].x = enemyBoss[i].x + 60 - obs_1[0].track;
            bossBullets[bBidx].y = enemyBoss[i].y + 115;
            bossBullets[bBidx].isActive = true;
            // rotating bullet
            double dr = sqrt(pow(player.x - 50 - enemyBoss[i].x + obs_1[0].track, 2) + pow(player.y - 50 - enemyBoss[i].y, 2));
            bossBullets[bBidx].velX = ((player.x - 50 - enemyBoss[i].x + obs_1[0].track) / dr) * bulletSpeed;
            bossBullets[bBidx].velY = ((player.y - 50 - enemyBoss[i].y) / dr) * bulletSpeed;
            bBidx++;
            bBidx = bBidx % 100;
        }
    }
}

// draw Obstacles
void drawObstacles()
{
    if (gamePhase == 0)
    {
        for (int i = 0; i < totalObstacles; i++)
        {
            iSetColor(255, 255, 255);
            iFilledRectangle(obs[i].x - obs[i].track, obs[i].y, obs[i].dx, obs[i].dy);
        }
        iShowBMP2(1830 - obs[0].track, 320, "pictures\\health and ammo\\health.bmp", 0);
        iShowBMP2(1850 - obs[0].track, 320, "pictures\\health and ammo\\ammo.bmp", 0);
    }
    if (gamePhase == 1)
    {
        for (int i = 0; i < totalObstacles_1; i++)
        {
            if (obs_1[i].isActive)
            {
                iSetColor(255, 255, 255);
                iFilledRectangle(obs_1[i].x - obs_1[i].track, obs_1[i].y, obs_1[i].dx, obs_1[i].dy);
            }
        }
        iShowBMP2(1830 - obs_1[0].track, 320, "pictures\\health and ammo\\health.bmp", 0);
        iShowBMP2(1850 - obs_1[0].track, 320, "pictures\\health and ammo\\ammo.bmp", 0);
    }
}
void obstacleCollison()
{
    if (gamePhase == 0)
    {
        for (int j = 0; j < totalObstacles; j++)

        {
            if (collisonCheck(player.x, player.y, obs[j].x - obs[j].track - 80, obs[j].y, 10, obs[j].dy - 5))
            {
                player.x -= 9;
            }

            if (collisonCheck(-player.x, player.y, -(obs[j].x - obs[j].track), obs[j].y, 80, obs[j].dy - 10))
            {
                player.x += 9;
            }
        }
    }
    else if (gamePhase == 1)
    { // case check
        if (collisonCheck(-player.x, player.y, -(obs_1[4].x - obs_1[4].track), obs_1[4].y, 80, obs_1[4].dy - 10) && obs_1[4].isActive)
        {
            enemyBoss[0].isAlive = true;
            obs_1[2].isActive = false;
            obs_1[3].isActive = false;
            obs_1[4].isActive = false;
        }
        if (collisonCheck(-player.x, player.y, -(obs_1[7].x - obs_1[7].track), obs_1[7].y, 80, obs_1[7].dy - 10) && obs_1[7].isActive)
        {
            enemyBoss[1].isAlive = true;
            obs_1[6].isActive = false;
            obs_1[7].isActive = false;
            obs_1[5].isActive = false;
        }
        if (collisonCheck(player.x, player.y, obs_1[9].x - obs_1[9].track - 80, obs_1[9].y, 20, obs_1[9].dy - 5) && obs_1[9].isActive)
        {
            enemyBoss[2].isAlive = true;
            obs_1[8].isActive = false;
            obs_1[9].isActive = false;
            obs_1[10].isActive = false;
        }
        if (collisonCheck(player.x, player.y, obs_1[12].x - obs_1[12].track - 80, obs_1[12].y, 20, obs_1[12].dy - 5) && obs_1[12].isActive && allBossDed)
        {
            sCodeIsActive = true;
            obs_1[11].isActive = false;
            obs_1[12].isActive = false;
            obs_1[12].isActive = false;
        }

        for (int j = 0; j < totalObstacles_1; j++)

        {
            if (collisonCheck(player.x, player.y, obs_1[j].x - obs_1[j].track - 80, obs_1[j].y, 10, obs_1[j].dy - 5) && obs_1[j].isActive)
            {
                player.x -= 7;
            }

            if (collisonCheck(-player.x, player.y, -(obs_1[j].x - obs_1[j].track), obs_1[j].y, 80, obs_1[j].dy - 10) && obs_1[j].isActive)
            {
                player.x += 7;
            }
            if (collisonCheck(player.x, player.y, obs_1[j].x - obs_1[j].track - 80, obs_1[j].y, 80 + obs_1[j].dx, 15) && obs_1[j].isActive)
            {
                player.y = player.y + 1;
            }
        }
    }
}

// health check for enemy
void playerHealthcheck()
{
    if (player.health == 0)
    {
        player.isAlive = false;
        gameState = 9; // Assuming 9 is the state for name input
        nameIndex = 0;
        memset(playerName, 0, sizeof(playerName));
    }
}
int pFlag = -1;
int eX = -100, eY = -100;
void enemyHealthcheck()
{
    if (gamePhase == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            if (enemy[i].health == 0 && enemy[i].isAlive)
            {
                // PlaySound("D:\\Project_1_1\\music\\zombieDead.wav", NULL, SND_ASYNC);
                player.score += 50;
                eX = enemy[i].x - obs[0].track + 30;
                eY = enemy[i].y + 30;
                iResumeTimer(t4);
                enemy[i].isAlive = false;
                if (i != 4)
                    enemy[i + 1].isAlive = true;
                else
                    pFlag++;
            }
            if ((player.y >= obs[11].y + obs[11].dy - 20 && player.y <= obs[12].y + 20 && player.x + 90 >= obs[11].x - obs[11].track) && pFlag == 0) // collisonCheck(player.x, player.y, obs[15].x - obs[15].track - 80, obs[15].y, 10, obs[15].dy - 5)&& pFlag == 0
            {
                player.score = player.score + (180 - (timeMin * 60 + 60 - showTime)) * 10;
                gamePhase = 1;
                player.isRotating = true;
                iPauseTimer(t1);
                currentBackgroundFrame = 0;
                player.health = 50;
                player.x = 400;
                bulletIndex = 0;
                iResumeTimer(t6);
            }
        }
    }
    if (gamePhase == 2)
    {
        for (int i = 0; i < 9; i++)
        {
            if (enemyUfo[i].health == 0 && enemyUfo[i].isAlive)
            {
                // PlaySound("D:\\Project_1_1\\music\\explosion.wav", NULL, SND_ASYNC);
                player.score += 300;
                eX = enemyUfo[i].x + 30;
                eY = enemyUfo[i].y + 30;
                iResumeTimer(t4);
                enemyUfo[i].isAlive = false;
                if (i != 8)
                    enemyUfo[i + 1].isAlive = true;
                else
                {
                    player.score = player.score + (180 - (timeMin * 60 + showTime)) * 10;
                    player.isAlive = false;
                    missionComplete = true;
                    gameState = 9; // Assuming 9 is the state for name input
                    nameIndex = 0;
                    memset(playerName, 0, sizeof(playerName));
                }
            }
        }
    }
    if (gamePhase == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (enemyBoss[i].health == 0 && enemyBoss[i].isAlive && gamePhase == 1)
            {
                // PlaySound("D:\\Project_1_1\\music\\zombieDead.wav", NULL, SND_ASYNC);
                player.score += 200;
                eX = enemyBoss[i].x - obs[0].track + 30;
                eY = enemyBoss[i].y + 30;
                iResumeTimer(t4);
                enemyBoss[i].isAlive = false;
                if (i == 2)
                {
                    pFlag++;
                    allBossDed = true;
                }
            }
            if (player.y + 10 >= obs_1[0].y + obs_1[0].dy && player.y <= obs_1[1].y && pFlag != -1 && player.x >= obs_1[0].x - obs_1[0].track && pFlag == 1)
            {
                player.score = player.score + (180 - (timeMin * 60 + 60 - showTime)) * 10;
                gamePhase = 2;
                player.health = 50;
                currentBackgroundFrame = 0;
                enemyUfo[0].isAlive = true;
                player.x = 400;
            }
        }
    }
}
// detect collison
void bulletHitDetect()
{
    for (int i = 0; i < ammo; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (collisonCheck(bullets[i].x, bullets[i].y, enemy[j].x - obs[j].track, enemy[j].y, 50, 66) && bullets[i].isActive && enemy[j].isAlive)
            {
                enemy[j].health--;
                bullets[i].isActive = false;
            }
        }
        for (int j = 0; j < 9; j++)
        {
            if (bullets[i].x >= enemyUfo[j].x && bullets[i].x <= enemyUfo[j].x + 50 && bullets[i].y >= enemyUfo[j].y && bullets[i].y <= enemyUfo[j].y + 60 && bullets[i].isActive && enemyUfo[j].isAlive)
            {
                enemyUfo[j].health--;
                bullets[i].isActive = false;
            }
        }
        for (int j = 0; j < 3; j++)
        {
            if (bullets[i].x >= enemyBoss[j].x - obs_1[0].track && bullets[i].x <= enemyBoss[j].x + 70 - obs_1[0].track && bullets[i].y >= enemyBoss[j].y && bullets[i].y <= enemyBoss[j].y + 150 && bullets[i].isActive && enemyBoss[j].isAlive)
            {
                enemyBoss[j].health--;
                bullets[i].isActive = false;
            }
        }
    }
    for (int i = 0; i < 100; i++)
    {
        if (enemyBullets[i].x >= player.x && enemyBullets[i].x <= player.x + 40 && enemyBullets[i].y >= player.y && enemyBullets[i].y <= player.y + 88 && enemyBullets[i].isActive)
        {
            player.health--;
            enemyBullets[i].isActive = false;
        }
        if (ufoBullets[i].x >= player.x && ufoBullets[i].x <= player.x + 40 && ufoBullets[i].y >= player.y && ufoBullets[i].y <= player.y + 88 && ufoBullets[i].isActive)
        {
            player.health--;
            ufoBullets[i].isActive = false;
        }
        if (bossBullets[i].x >= player.x && bossBullets[i].x <= player.x + 40 && bossBullets[i].y >= player.y && bossBullets[i].y <= player.y + 88 && bossBullets[i].isActive)
        {
            player.health--;
            bossBullets[i].isActive = false;
        }
    }
    playerHealthcheck();
    enemyHealthcheck();
}

int collisonCheck(int x, int y, int p, int q, int s, int t)
{
    if (x >= p && x <= p + s && y >= q && y <= q + t)
    {
        return 1;
    }
    else
        return 0;
}

// Drawing
void iDraw()
{
    iClear();
    switch (gameState)
    {
    // gameStaring
    case 0:
    {
        gameStarting();
        break;
    }
    case 1:
    {
        mainMenu();
        break;
    }
    case 2:
    {
        mainGame();
        break;
    }
    case 3:
    {
        gameStory();
        break;
    }
    case 4:
    {
        highScoreShow();
        break;
    }
    case 5:
    {
        gameSettings();
        break;
    }
    case 6:
    {
        instructions();
        break;
    }
    case 7:
    {
        credits();
        break;
    }
    case 8:
    {
        quit();
    }
    case 9:
    {
        displayPlayerNameInput();
    }

    default:
        break;
    }
}

// Mouse Controls
// Mouse move handler
void iMouseMove(int mx, int my)
{
    printf("x = %d, y= %d\n", mx, my);
}
void iPassiveMouseMove(int mx, int my)
{
    mouseX = mx;
    mouseY = my;
}
double calculateAngle(int playerX, int playerY, int mouseX, int mouseY)
{
    double deltaX = mouseX - (playerX + 45);               // Player center offset
    double deltaY = mouseY - (playerY + 55);               // Adjust according to player's dimensions
    double angle = atan2(deltaY, deltaX) * (180.0 / M_PI); // Convert radians to degrees
    if (angle < 0)
        angle += 360.0; // Normalize angle to [0, 360]
    return angle;
}
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mainMenuSetup(button, state, mx, my);
        if (gameState == 2)
        {
            playerRateOfFire();
            // // iResumeTimer(t2);
            // // shootBgm = true;
            // PlaySound("D:\\Project_1_1\\music\\gunshot.wav", NULL, SND_ASYNC);
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        if (gameState == 2)
        {
        }
    }
}

void playerRateOfFire()
{
    Bullet &bullet = bullets[bulletIndex];
    // Set bullet's initial position
    if (gamePhase == 0)
    {
        bullet.x = player.x + 45;
        bullet.y = player.y + 55;
    }
    else
    {
        bullet.x = player.x + 45;
        bullet.y = player.y + 45;
    }

    // Get the player's current rotation angle for firing
    // double angle = player.rotationAngle;
    double angle;
    if (player.rotationIndex >= 0 && player.rotationIndex <= 5)
        angle = abs((player.rotationIndex + 10) % 12) * anglePerFrame;
    else
        angle = abs((player.rotationIndex + 9) % 12) * anglePerFrame;
    double angleRad = angle * (M_PI / 180.0); // Convert to radians

    // Set bullet speed and direction based on the rotation angle
    bullet.velocity = bulletSpeed;
    bullet.velX = cos(angleRad) * bullet.velocity;
    bullet.velY = sin(angleRad) * bullet.velocity;

    bullet.isActive = true;
    // Update the bullet index to point to the next bullet
    bulletIndex = (bulletIndex + 1);
}
// Keyboard Controls
void iKeyboard(unsigned char key)
{
    if (gameState == 9)
    { // Name Input State
        if (key == '\r')
        { // Enter key
            updateHighScores(playerName, player.score);
            gameState = 4; // Transition to high score display
        }
        else if (key == '\b' && nameIndex > 0)
        { // Backspace
            nameIndex--;
            playerName[nameIndex] = '\0';
        }
        else if (nameIndex < 99 && key >= 32 && key <= 126)
        { // Valid character
            playerName[nameIndex++] = key;
            playerName[nameIndex] = '\0';
        }
    }
    else
    {
        if (key == 'd')
        {
            player.x += 9;
            if (player.iswalking)
            {
                player.frameIndex = (player.frameIndex + 1) % 4;
            }
            // for (int i = 0; i < 5; i++)
            {
                if (player.x > 400 && currentBackgroundFrame != 125)
                {
                    player.x = 400;
                    if (gamePhase == 0)
                    {
                        for (int j = 0; j < totalObstacles; j++)
                        {
                            obs[j].track += 5;
                        }
                    }
                    if (gamePhase == 1)
                    {
                        for (int j = 0; j < totalObstacles_1; j++)
                        {
                            obs_1[j].track += 5;
                        }
                    }
                    currentBackgroundFrame++;
                    currentBackgroundFrame = currentBackgroundFrame % totalBackgroundFrames;
                }
            }
            // obstacleCollison();
        }
        else if (key == 'a')
        {

            player.x -= 9;
            if (player.iswalking)
            {
                player.frameIndex = (player.frameIndex + 3) % 4;
            }
            if (player.x < 0)
                if (currentBackgroundFrame >= 0 && currentBackgroundFrame < 125)
                {
                    player.x = 0;
                    if (gamePhase == 0)
                    {
                        for (int j = 0; j < totalObstacles; j++)
                        {
                            obs[j].track -= 5;
                        }
                    }
                    if (gamePhase == 1)
                    {
                        for (int j = 0; j < totalObstacles_1; j++)
                        {
                            obs_1[j].track -= 5;
                        }
                    }
                    currentBackgroundFrame += 126;
                    currentBackgroundFrame--;
                    currentBackgroundFrame = abs(currentBackgroundFrame % totalBackgroundFrames);
                }
            // obstacleCollison();
            // player.isRotating = false;
        }
        else if (key == 'w')
        {
            if (gamePhase == 0)
            {
                for (int j = 0; j < totalObstacles; j++)
                {
                    if (collisonCheck(player.x, player.y, obs[j].x - obs[j].track - 80, obs[j].y, 80 + obs[j].dx, 20))
                    {
                        qFlag = 1;
                        tme = 1;
                        iResumeTimer(t1);
                    }
                }
                if (player.y <= 10)
                {
                    qFlag = 1;
                    tme = 1;
                    iResumeTimer(t1);
                }
            }
            else
                player.y += 12;
            if (player.y > 650)
                player.y = 650;
        }
        else if (key == 's')
        {
            if (gameState == 0)
            {
                gameState = 1;
            }
            else
                player.y -= 7;
            if (player.y <= 10)
            {
                player.y = 10;
            }
        }
    }
}
// Special keyboard handler
void iSpecialKeyboard(unsigned char key)
{
    // Add behavior if needed
}

void gameStarting()
{
    iShowBMP(0, 0, "pictures\\Front\\background.bmp");
    iSetColor(255, 255, 255);
    iText(10, 10, "press s for starting");
}
void mainMenu()
{
    iShowBMP(0, 0, "pictures\\mainMenu\\mainMenu.bmp");
    gameReset();
}
void mainMenuSetup(int button, int state, int mx, int my)
{
    if (gameState == 1)
    {
        if (mx >= 540 && mx <= 740 && my <= 418 && my >= 387)
        {
            gameReset();
            // musicOn = false;
            // if (!musicOn)
            // {
            //     PlaySound(0, 0, 0);
            // }
            gameState = 2;
            iResumeTimer(t3);
            iResumeTimer(t5);
            iResumeTimer(t7);
            iResumeTimer(t8);
            shootBgm = true;
        }
        else if (mx >= 530 && mx <= 765 && my <= 355 && my >= 327)
        {
            gameState = 3;
        }
        else if (mx >= 543 && mx <= 735 && my <= 304 && my >= 276)
        {
            gameState = 4;
        }
        else if (mx >= 531 && mx <= 751 && my <= 254 && my >= 228)
        {
            gameState = 5;
        }
        else if (mx >= 540 && mx <= 746 && my <= 206 && my >= 180)
        {
            gameState = 6;
        }
        else if (mx >= 560 && mx <= 725 && my <= 150 && my >= 123)
        {
            gameState = 7;
        }
        else if (mx >= 588 && mx <= 693 && my <= 98 && my >= 67)
        {
            gameState = 8;
        }
    }
    else if (gameState == 2 || gameState == 3 || gameState == 4 || gameState == 5 || gameState == 6 || gameState == 7)
    {
        {
            if (mx >= 40 && mx <= 120 && my <= 680 && my >= 600)
            {
                gameState = 1;
            }
        }
    }
    if (gameState == 5 && mx >= 290 && mx <= 928 && my <= 395 && my >= 322)
    {
        printf("Pressed");
        musicOn = !(musicOn);
        if (musicOn)
        {
            PlaySound("music\\Swordland.wav", NULL, SND_LOOP | SND_ASYNC);
        }
        else
        {
            PlaySound(0, 0, 0);
        }
    }
}

// mainGame draw
void mainGame()
{
    iShowBMP(0, 0, backgroundFrames[currentBackgroundFrame]); // Draw Background
    // printf("%d",currentBackgroundFrame);
    drawObstacles();

    updatePlayerRotation();
    drawEnemy();
    drawBullets();
    drawPlayer();

    // Display Scores and Health
    iSetColor(255, 255, 255);
    char scoreText[100];
    sprintf(scoreText, "Score: %d", player.score);
    iText(750, 680, scoreText);

    char ammoText[100];
    sprintf(ammoText, "Ammo: %d", 100 - bulletIndex + 1);
    iText(750, 650, ammoText);

    char healthText[100];
    sprintf(healthText, "Your Health: %d", player.health);
    iText(610, 650, healthText);
    iSetColor(84, 130, 255);
    iFilledRectangle(610, 620, player.health * 5, 10);
    iSetColor(255, 255, 255);
    if (gamePhase == 0)
    {
        iRectangle(610, 620, 100, 10);
    }
    else
    {
        iRectangle(610, 620, 250, 10);
    }
    // showTime
    char timeShowM[100];
    sprintf(timeShowM, "Time : %d", timeMin);
    iText(610, 680, timeShowM);

    char timeShow[100];
    sprintf(timeShow, ": %02d", showTime);
    iText(690, 680, timeShow);

    // char enemyHealthText[100];
    // sprintf(enemyHealthText, "Enemy Health: %d", enemy.health);
    if (gamePhase == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            if (enemy[i].isAlive)
            {
                iSetColor(107, 0, 20);
                for (int j = 0; j < totalObstacles; j++)
                {
                    iFilledRectangle(enemy[i].x + 10 - obs[j].track, enemy[i].y + 90, enemy[i].health * 10, 10);
                }
            }
        }
    }
    if (gamePhase == 2)
    {
        for (int i = 0; i < 9; i++)
        {
            if (enemyUfo[i].isAlive)
            {
                iSetColor(84, 130, 255);
                iFilledRectangle(enemyUfo[i].x - 10, enemyUfo[i].y + 150, enemyUfo[i].health * 10, 10);
            }
        }
    }
    if (gamePhase == 1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (enemyBoss[i].isAlive)
            {
                iSetColor(84, 130, 255);
                for (int j = 0; j < totalObstacles_1; j++)
                {
                    iFilledRectangle(enemyBoss[i].x + 20 - obs_1[j].track, enemyBoss[i].y + 150, enemyBoss[i].health * 5, 10);
                }
            }
        }
        if (!sCodeIsActive)
        {
            iSetColor(255, 255, 255);
            iText(obs_1[11].x - obs_1[11].track + 20, 550, "Code", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        else
        {
            iSetColor(255, 255, 255);
            iText(500, 320, "You got the code!", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }
    if (timeMin == 0 && showTime == 0)
    {
        gameState = 9;
    }
    obstacleCollison();
    bulletHitDetect();
    backButton();
}
void gameStory()
{
    iShowBMP(0, 0, "pictures\\Story\\storybg.bmp");
    backButton();
}
void highScoreShow()
{
    iShowBMP(0, 0, "pictures\\highScore\\highScore.bmp");
    displayHighScores();
    backButton();
}
void gameSettings()
{
    if (musicOn)
        iShowBMP(0, 0, "pictures\\settings\\soundOff.bmp");
    else
        iShowBMP(0, 0, "pictures\\settings\\soundOn.bmp");
    backButton();
}
void instructions()
{
    iShowBMP(0, 0, "pictures\\instructions\\instructions.bmp");
    backButton();
}
void credits()
{
    iShowBMP(0, 0, "pictures\\credits\\credits.bmp");
    backButton();
}
void quit()
{
    exit(0);
}
void gameReset()
{
    gamePhase = 0;
    player = {50, 10, 20, 0, 0, 0, 0, true, false, false, true};
    initializeGame();
    bulletIndex = 0;
    eBidx = 0;
    uBidx = 0;
    currentBackgroundFrame = 0;
    for (int i = 0; i < 100; i++)
    {
        enemyBullets[i].isActive = false;
        ufoBullets[i].isActive = false;
    }
    for (int i = 0; i < 100; i++)
    {
        bullets[i].isActive = false;
    }
    for (int i = 0; i < 100; i++)
    {
        bossBullets[i].isActive = false;
    }
    timeMin = 2;
    showTime = 60;
    pFlag = -1;
    eX = -100;
    eY = -100;
    bool musicOn = true; // musicOn
    bool shootBgm = false;
    bool enemyDeadM = false;
    bool ufoExplodeM = false;
    int eDeadi = 0;
    int eUfoi = 0;
    bool sCodeIsActive = false;
    bool allBossDed = false;
    bool missionComplete = false;
}
void backButton()
{
    iShowBMP2(40, 600, "pictures\\backButton\\backButton.bmp", 0);
}

// Load high scores from a file
void loadHighScores()
{
    FILE *file = fopen("highscores.txt", "r");
    if (file)
    {
        for (int i = 0; i < maxHighScores; i++)
        {
            if (fscanf(file, "%s %d", highScores[i].name, &highScores[i].score) != 2)
            {
                // Fill remaining slots with default values
                strcpy(highScores[i].name, "Player");
                highScores[i].score = 0;
            }
        }
        fclose(file);
    }
    else
    {
        // Initialize with default values if file doesn't exist
        for (int i = 0; i < maxHighScores; i++)
        {
            strcpy(highScores[i].name, "Player");
            highScores[i].score = 0;
        }
    }
}

void saveHighScores()
{
    FILE *file = fopen("highscores.txt", "w");
    if (file)
    {
        for (int i = 0; i < maxHighScores; i++)
        {
            fprintf(file, "%s %d\n", highScores[i].name, highScores[i].score);
        }
        fclose(file);
    }
}

void updateHighScores(const char *playerName, int playerScore)
{
    for (int i = 0; i < maxHighScores; i++)
    {
        if (playerScore > highScores[i].score)
        {
            // Shift lower scores down the list
            for (int j = maxHighScores - 1; j > i; j--)
            {
                highScores[j] = highScores[j - 1];
            }
            // Insert new high score
            strcpy(highScores[i].name, playerName);
            highScores[i].score = playerScore;
            break;
        }
    }
    saveHighScores();
}

void displayHighScores()
{
    int y = screenHeight - 200; // Start position for displaying scores
    iText(500, y, "High Scores:", GLUT_BITMAP_HELVETICA_18);

    for (int i = 0; i < maxHighScores; i++)
    {
        char buffer[200];
        sprintf(buffer, "%d. %s - %d", i + 1, highScores[i].name, highScores[i].score);
        y -= 30; // Move down for the next entry
        iText(500, y, buffer, GLUT_BITMAP_HELVETICA_18);
    }
}

void displayPlayerNameInput()
{
    iShowBMP(0, 0, "pictures\\highScore\\highScore.bmp");
    if (missionComplete)
    {
        gameOverScene();
    }
    else
        playerDeadScene();
    char scrShow[100];
    sprintf(scrShow, "Your Score: %d", player.score);
    iText(500, 480, scrShow, GLUT_BITMAP_TIMES_ROMAN_24);
    char prompt[200];
    sprintf(prompt, "Enter your name: %s", playerName);
    iText(500, screenHeight / 2, prompt, GLUT_BITMAP_TIMES_ROMAN_24);
}
void displayeTime()
{
    showTime--;
    if (showTime == -1)
    {
        timeMin--;
        showTime = 59;
    }
}

void effectTimer()
{
    eDeadi++;
    eUfoi++;
    if (eDeadi == 6)
    {
        iPauseTimer(t4);
        eX = -100;
        eY = -100;
        eDeadi = 0;
    }
    if (eUfoi == 6)
    {
        iPauseTimer(t4);
        eX = -100;
        eY = -100;
        eUfoi = 0;
    }
}
void enemyDeadEffect()
{
    iShowBMP2(eX, eY, eDeadE[eDeadi], 0);
}
void ufoDeadEffect()
{
    if (gamePhase == 2)
        iShowBMP2(eX, eY, eUfoE[eUfoi], 0);
}
void playerDeadScene()
{
    shootBgm = false;
    PlaySound(0, 0, 0);
    iSetColor(255, 255, 255);
    iText(500, 520, "Mission Incomplete!", GLUT_BITMAP_TIMES_ROMAN_24);
}
void gameOverScene()
{
    shootBgm = false;
    PlaySound(0, 0, 0);
    iSetColor(255, 255, 255);
    iText(500, 520, "Mission Completed!! Yeee", GLUT_BITMAP_TIMES_ROMAN_24);
}

int main()
{
    // iSetTimer(1,bossFlyFrameUpdate);
    initializeGame();
    t5 = iSetTimer(10, moveBullets); // Update bullet positions every 10 ms
    t6 = iSetTimer(1, playerFlying);
    t7 = iSetTimer(100, moveEnemy);
    t8 = iSetTimer(500, shootEnemy);
    t1 = iSetTimer(40, jump);
    t2 = iSetTimer(200, playerRateOfFire);
    t3 = iSetTimer(1000, displayeTime);
    t4 = iSetTimer(100, effectTimer);
    iPauseTimer(t4);
    iPauseTimer(t3);
    iPauseTimer(t2);
    iPauseTimer(t1);
    iPauseTimer(t5);
    iPauseTimer(t6);
    iPauseTimer(t7);
    iPauseTimer(t8);
    if (musicOn)
        PlaySound("music\\Swordland.wav", NULL, SND_LOOP | SND_ASYNC);
    iInitialize(screenWidth, screenHeight, "PROJECT LUNAR-X");
    return 0;
}

// .\runner.bat final_projectX.cpp
#include "MainWindow.h"

#include "Timing.h"

#include <FL/Fl.H>
#include <FL/glu.h>

#include <assert.h>
#include <stdio.h>  // debug

static const float gFaceRotX[6] = { -90,   0,   0,   0,   0,  90 };
static const float gFaceRotY[6] = {   0,   0,  90, 180, -90,   0 };

#define BIT(i) (1<<(i))

static const int gFPS = 50;
static const float gFaceTurnTime = 0.4;

static const float gFaceAxis[6][3] = {
    {  0,  1,  0 }, {  0,  0,  1 }, {  1,  0,  0 },
    {  0,  0, -1 }, { -1,  0,  0 }, {  0, -1,  0 } };

static const int gFaceMap[6][3][3] = {
    { { BIT(0)|BIT(3)|BIT(4), BIT(0)|BIT(3), BIT(0)|BIT(3)|BIT(2) },
      { BIT(0)       |BIT(4), BIT(0),        BIT(0)       |BIT(2) },
      { BIT(0)|BIT(1)|BIT(4), BIT(0)|BIT(1), BIT(0)|BIT(1)|BIT(2) } },

    { { BIT(1)|BIT(0)|BIT(4), BIT(1)|BIT(0), BIT(1)|BIT(0)|BIT(2) },
      { BIT(1)       |BIT(4), BIT(1),        BIT(1)       |BIT(2) },
      { BIT(1)|BIT(5)|BIT(4), BIT(1)|BIT(5), BIT(1)|BIT(5)|BIT(2) } },

    { { BIT(2)|BIT(0)|BIT(1), BIT(2)|BIT(0), BIT(2)|BIT(0)|BIT(3) },
      { BIT(2)       |BIT(1), BIT(2),        BIT(2)       |BIT(3) },
      { BIT(2)|BIT(5)|BIT(1), BIT(2)|BIT(5), BIT(2)|BIT(5)|BIT(3) } },

    { { BIT(3)|BIT(0)|BIT(2), BIT(3)|BIT(0), BIT(3)|BIT(0)|BIT(4) },
      { BIT(3)       |BIT(2), BIT(3),        BIT(3)       |BIT(4) },
      { BIT(3)|BIT(5)|BIT(2), BIT(3)|BIT(5), BIT(3)|BIT(5)|BIT(4) } },

    { { BIT(4)|BIT(0)|BIT(3), BIT(4)|BIT(0), BIT(4)|BIT(0)|BIT(1) },
      { BIT(4)       |BIT(3), BIT(4),        BIT(4)       |BIT(1) },
      { BIT(4)|BIT(5)|BIT(3), BIT(4)|BIT(5), BIT(4)|BIT(5)|BIT(1) } },

    { { BIT(5)|BIT(1)|BIT(4), BIT(5)|BIT(1), BIT(5)|BIT(1)|BIT(2) },
      { BIT(5)       |BIT(4), BIT(5),        BIT(5)       |BIT(2) },
      { BIT(5)|BIT(3)|BIT(4), BIT(5)|BIT(3), BIT(5)|BIT(3)|BIT(2) } } };


void main_window_tick_callback(void *arg)
{
    MainWindow *mw = (MainWindow*)arg;
    if (mw->tick()) Fl::repeat_timeout(1.0/gFPS, main_window_tick_callback, arg);
    mw->redraw();
}

MainWindow::MainWindow( int x, int y, int w, int h, const char *winTitle,
                        Fl_RGB_Image *labels )
    : Fl_Gl_Window(x, y, w, h, winTitle),
      mCube(gSolvedCube), mLabelsTexture(0), mLabelsImage(labels),
      mAnimRotFace(-1), mAnimRotAngle(0)
{
}

MainWindow::~MainWindow()
{
    glDeleteTextures(1, &mLabelsTexture);
    delete mLabelsImage;
}

void MainWindow::animate(std::vector<int> moves)
{
    Fl::remove_timeout(&main_window_tick_callback);

    mAnimMoves.assign(moves.begin(), moves.end());
    if (!moves.empty())
    {
        mAnimRotFace = moves.front();
        mAnimRotTime = 0;
        mAnimLastTime = time_now();

        Fl::add_timeout(1.0/gFPS, main_window_tick_callback, this);
    }
}

void MainWindow::draw()
{
    if (!valid())
    {
        // Set up viewport
        glViewport(0, 0, w(), h());

        // Set up projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, (float)w()/(float)h(), 1, 100);

        // Set background color
        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);

        // Set Z buffering
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Enable backface culling
        glEnable(GL_CULL_FACE);

        // Set up texturing
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // Load texture
        glDeleteTextures(1, &mLabelsTexture);
        glGenTextures(1, &mLabelsTexture);
        glBindTexture(GL_TEXTURE_2D, mLabelsTexture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
                      mLabelsImage->w(), mLabelsImage->h(), 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, mLabelsImage->data()[0] );
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up model transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -15);
    glRotatef(mRotX, 1, 0, 0);
    glRotatef(mRotY, 0, 1, 0);

    // Use texture for rendering faces
    glBindTexture(GL_TEXTURE_2D, mLabelsTexture);

    for (int f = 0; f < 6; ++f)
    {
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                glPushMatrix();
                if ( mAnimRotFace >= 0 &&
                     (gFaceMap[f][r][c] & BIT(mAnimRotFace)) )
                {
                    const float *axis = gFaceAxis[mAnimRotFace];
                    glRotatef(mAnimRotAngle, axis[0], axis[1], axis[2]);
                }
                glRotatef(gFaceRotX[f], 1, 0, 0);
                glRotatef(gFaceRotY[f], 0, 1, 0);

                const Face &face = mCube.face(f, r, c);

                int lr = face.sym/3, lc = face.sym%3, rrot = (-face.rot + 4)%4;

                float labelCoords[4][2] = { { (lc + 0)/3.0f, (lr + 1)/3.0f },
                                            { (lc + 1)/3.0f, (lr + 1)/3.0f },
                                            { (lc + 1)/3.0f, (lr + 0)/3.0f },
                                            { (lc + 0)/3.0f, (lr + 0)/3.0f } };

                glBegin(GL_QUADS);

                // Textured front face
                glColor3f(1, 1, 1);
                glTexCoord2f( labelCoords[(rrot + 0)%4][0],
                              labelCoords[(rrot + 0)%4][1] );
                glVertex3f(2*c - 3, 1 - 2*r, 3);
                glTexCoord2f( labelCoords[(rrot + 1)%4][0],
                              labelCoords[(rrot + 1)%4][1] );
                glVertex3f(2*c - 1, 1 - 2*r, 3);
                glTexCoord2f( labelCoords[(rrot + 2)%4][0],
                              labelCoords[(rrot + 2)%4][1] );
                glVertex3f(2*c - 1, 3 - 2*r, 3);
                glTexCoord2f( labelCoords[(rrot + 3)%4][0],
                              labelCoords[(rrot + 3)%4][1] );
                glVertex3f(2*c - 3, 3 - 2*r, 3);

                // Black backface
                glColor3f(0, 0, 0);
                glVertex3f(2*c - 3, 3 - 2*r, 3);
                glVertex3f(2*c - 1, 3 - 2*r, 3);
                glVertex3f(2*c - 1, 1 - 2*r, 3);
                glVertex3f(2*c - 3, 1 - 2*r, 3);

                glEnd();

                glPopMatrix();
            }
        }
    }
}

int MainWindow::handle(int event)
{
    switch (event)
    {
    case FL_PUSH:
        mLastX = Fl::event_x();
        mLastY = Fl::event_y(); 
        return 1;

    case FL_DRAG:
        mRotY += (Fl::event_x() - mLastX);
        mRotX += (Fl::event_y() - mLastY);
        mLastX = Fl::event_x();
        mLastY = Fl::event_y();
        redraw();
        return 1;

    case FL_KEYDOWN:
        switch (Fl::event_key())
        {
        case 'r':
            // Reset rotation
            mRotX = mRotY = 0;
            redraw();
            return 1;

        case 't':
            {
                int a[12] = { 0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0 };
                animate(std::vector<int>(a, a + 12));
            }
            return 1;

        case '0': animate(std::vector<int>(1, 0)); return 1;
        case '1': animate(std::vector<int>(1, 1)); return 1;
        case '2': animate(std::vector<int>(1, 2)); return 1;
        case '3': animate(std::vector<int>(1, 3)); return 1;
        case '4': animate(std::vector<int>(1, 4)); return 1;
        case '5': animate(std::vector<int>(1, 5)); return 1;

        default:
            break;
        }
        break;

    default:
        break;
    }

    return Fl_Gl_Window::handle(event);
}

bool MainWindow::tick()
{
    double now = time_now(), dt = now - mAnimLastTime;
    mAnimLastTime = now;

    mAnimRotTime += dt;
    while (mAnimRotTime >= gFaceTurnTime)
    {
        mAnimRotTime -= gFaceTurnTime;

        // Update cube with current move
        mCube.move(mAnimMoves.front());
        mAnimMoves.pop_front();

        // Start rotating face for next move
        if (mAnimMoves.empty())
        {
            mAnimRotFace = -1;
            return false;
        }
        mAnimRotFace = mAnimMoves.front();
    }
    mAnimRotAngle = 90.0f*mAnimRotTime/gFaceTurnTime;
    return true;
}

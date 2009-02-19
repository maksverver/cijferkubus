#include "MainWindow.h"
#include <FL/Fl.H>
#include <FL/glu.h>

#include <stdio.h>  // debug

static const float gFaceRotX[6] = { -90,   0,   0,   0,   0,  90 };
static const float gFaceRotY[6] = {   0,   0,  90, 180, -90,   0 };

MainWindow::MainWindow( int x, int y, int w, int h, const char *winTitle,
                        Fl_RGB_Image *labels )
    : Fl_Gl_Window(x, y, w, h, winTitle),
      mCube(gSolvedCube), mLabelsTexture(0), mLabelsImage(labels)
{
}

MainWindow::~MainWindow()
{
    glDeleteTextures(1, &mLabelsTexture);
    delete mLabelsImage;
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
        glPushMatrix();
        glRotatef(gFaceRotX[f], 1, 0, 0);
        glRotatef(gFaceRotY[f], 0, 1, 0);
        glBegin(GL_QUADS);
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                const Face &face = mCube.face(f, 2 - r, c);

                int lr = face.sym/3, lc = face.sym%3, rrot = (-face.rot + 4)%4;

                float labelCoords[4][2] = { { (lc + 0)/3.0f, (lr + 1)/3.0f },
                                            { (lc + 1)/3.0f, (lr + 1)/3.0f },
                                            { (lc + 1)/3.0f, (lr + 0)/3.0f },
                                            { (lc + 0)/3.0f, (lr + 0)/3.0f } };

                glTexCoord2f( labelCoords[(rrot + 0)%4][0],
                              labelCoords[(rrot + 0)%4][1] );
                glVertex3f(2*c - 3, 2*r - 3, 3);
                glTexCoord2f( labelCoords[(rrot + 1)%4][0],
                              labelCoords[(rrot + 1)%4][1] );
                glVertex3f(2*c - 1, 2*r - 3, 3);
                glTexCoord2f( labelCoords[(rrot + 2)%4][0],
                              labelCoords[(rrot + 2)%4][1] );
                glVertex3f(2*c - 1, 2*r - 1, 3);
                glTexCoord2f( labelCoords[(rrot + 3)%4][0],
                              labelCoords[(rrot + 3)%4][1] );
                glVertex3f(2*c - 3, 2*r - 1, 3);
            }
        }
        glEnd();
        glPopMatrix();
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
        case '0':
            mRotX = mRotY = 0;
            redraw();
            return 1;
        default:
            break;
        }
        break;

    default:
        break;
    }

    return Fl_Gl_Window::handle(event);
}

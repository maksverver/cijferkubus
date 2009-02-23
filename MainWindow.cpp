#include "MainWindow.h"

#include "Timing.h"

#include <FL/glu.h>
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>

#include <assert.h>
#include <stdio.h>  // debug

#include <fstream>

#define BIT(i) (1<<(i))

static const float gFaceRotX[6] = { -90,   0,   0,   0,   0,  90 };
static const float gFaceRotY[6] = {   0,   0,  90, 180, -90,   0 };

static const int gFPS = 50;
static const float gFaceTurnTime = 0.5;

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

/*  Double signed area of triangle.
    abs(0.5*area(p,q,r)) == area of triangle p,q,r

    Triangle from p to q to r counterclockwise iff
    r on the left of the line from p to q iff

        |  p.x  p.y  1  |
    det |  q.y  q.y  1  | > 0
        |  r.y  r.y  1  |
*/
static double area(const Point &p, const Point &q, const Point &r)
{
    return  p.x * q.y - p.y * q.x +
            p.y * r.x - p.x * r.y +
            q.x * r.y - r.x * q.y;
}

/*  Determines if point `p` lies inside or on the boundary of the
    quadrilateral `quad' (which must be in anticlockwise order) */
static bool point_in_quad(const Point (&quad)[4], const Point &p)
{
    for (int n = 0; n < 4; ++n)
    {
        if (area(quad[n], quad[(n + 1)%4], p) < 0) return false;
    }
    return true;
}


void main_window_tick_callback(void *arg)
{
    MainWindow *mw = (MainWindow*)arg;
    mw->tick();
    mw->redraw();
    Fl::repeat_timeout(1.0/gFPS, main_window_tick_callback, arg);
}

void main_window_save_cube_callback(Fl_File_Chooser *fc, void *arg)
{
    MainWindow *mw = (MainWindow*)arg;
    if (fc->count() == 1)
    {
        const char *path = fc->value();
        std::ofstream ofs(path);
        if (writeCube(ofs, mw->cube()))
        {
            printf("Cube saved at %s.\n", path);
        }
        else
        {
            printf("Failed to save cube at %s!\n", path);
        }
    }
}

MainWindow::MainWindow( int x, int y, int w, int h, const char *winTitle,
                        Fl_RGB_Image *labels )
    : Fl_Gl_Window(x, y, w, h, winTitle),
      mCube(gSolvedCube),
      mLabelsTexture(0), mLabelsImage(labels),
      mAnimLastTime(time_now()), mAnimTime(0), mAnimPos(0), mAnimGoal(0),
      mSelectedFace(-1)
{
    setView();
    Fl::add_timeout(1.0/gFPS, main_window_tick_callback, this);
}

MainWindow::~MainWindow()
{
    Fl::remove_timeout(main_window_tick_callback, this);
    glDeleteTextures(1, &mLabelsTexture);
    delete mLabelsImage;
}

void MainWindow::addMove(const Move &move)
{
    mAnimMoves.push_back(move);
}

void MainWindow::addMoves(const std::vector<Move> &moves)
{
    mAnimMoves.insert(mAnimMoves.end(), moves.begin(), moves.end());
}

void MainWindow::setView(float rotX, float rotY)
{
    mRotX = rotX;
    mRotY = rotY;
}

void MainWindow::advanceAnim(int moves)
{
    if (moves < 0 && (size_t)-moves > mAnimGoal)
    {
        mAnimGoal = 0;
    }
    else
    if (moves > 0 && mAnimMoves.size() - mAnimGoal < (size_t)moves)
    {
        mAnimGoal = mAnimMoves.size();
    }
    else
    {
        mAnimGoal = mAnimGoal + moves;
    }
    printf("pos=%d goal=%d\n", (int)mAnimPos, (int)mAnimGoal);
}

void MainWindow::draw()
{
    if (!valid())
    {
        // Set up viewport
        glViewport(0, 0, w(), h());

        // Set background color
        glClearColor(0.1f, 0.1f, 0.2f, 0.0f);

        // Set Z buffering
        glClearDepth(1.0f);
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
        gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB,
                           mLabelsImage->w(), mLabelsImage->h(), 
                           GL_RGBA, GL_UNSIGNED_BYTE, mLabelsImage->data()[0] );
        /*
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,
                      mLabelsImage->w(), mLabelsImage->h(), 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, mLabelsImage->data()[0] );
        */
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w()/(float)h(), 1, 100);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Set up model transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -15);
    glRotatef(mRotX, 1, 0, 0);
    glRotatef(mRotY, 0, 1, 0);

    // Use texture for rendering faces
    glBindTexture(GL_TEXTURE_2D, mLabelsTexture);

    // Check which face animation we're doing (if any)
    int anim_face = -1;
    float anim_angle = 0;
    if (mAnimPos < mAnimMoves.size())
    {
        anim_face = mAnimMoves[mAnimPos].face;
        switch (mAnimMoves[mAnimPos].turn)
        {
        case 1:  anim_angle =   90; break;
        case 2:  anim_angle = -180; break;
        case 3:  anim_angle =  -90; break;
        default: anim_angle =    0; break;
        }
        anim_angle *= mAnimTime/gFaceTurnTime;
    }

    glEnable(GL_DEPTH_TEST);
    for (int f = 0; f < 6; ++f)
    {
        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                glPushMatrix();
                if (gFaceMap[f][r][c] & BIT(anim_face))
                {
                    const float *axis = gFaceAxis[anim_face];
                    glRotatef(anim_angle, axis[0], axis[1], axis[2]);
                }
                glRotatef(gFaceRotX[f], 1, 0, 0);
                glRotatef(gFaceRotY[f], 0, 1, 0);

                const Face &face = mCube.face(f, r, c);
                const int face_id = 9*f + 3*r + c;

                int lr = face.sym/3, lc = face.sym%3;

                // Determine texture coordinates for this face subrectangle
                float texCoords[4][2] = { { (lc + 0)/3.0f, (lr + 1)/3.0f },
                                          { (lc + 1)/3.0f, (lr + 1)/3.0f },
                                          { (lc + 1)/3.0f, (lr + 0)/3.0f },
                                          { (lc + 0)/3.0f, (lr + 0)/3.0f } };

                std::rotate( &texCoords[0][0], &texCoords[-face.rot&3][0],
                             &texCoords[4][0] );

                // Determine coordinates for this face subrectangle
                float quad[4][3] = { { 2*c - 3, 1 - 2*r, 3 },
                                     { 2*c - 1, 1 - 2*r, 3 },
                                     { 2*c - 1, 3 - 2*r, 3 },
                                     { 2*c - 3, 3 - 2*r, 3 } };

                // Determine on-screen coordinates of this face subrectangle
                {
                    GLdouble modelview[16], projection[16];
                    GLint viewport[4];
                    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
                    glGetDoublev(GL_PROJECTION_MATRIX, projection);
                    glGetIntegerv(GL_VIEWPORT, viewport);
                    for (int n = 0; n < 4; ++n)
                    {
                        GLdouble winX, winY, winZ;
                        gluProject( quad[n][0], quad[n][1], quad[n][2], 
                                    modelview, projection, viewport,
                                    &winX, &winY, &winZ );
                        /* NOTE: since OpenGL y coordinates start at the bottom,
                           but FLTK coordinates start at the top, we must flip
                           the coordinates vertically. This also reverses the
                           order of the points, so we must assign them in
                           reverse to maintain counterclockwise order. */
                        mFaceWinCoords[face_id][3 - n] =
                            Point(winX, h() - winY - 1);
                    }
                }

                // Textured front face
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                if (face_id != mSelectedFace)
                {
                    glColor3f(1, 1, 1);
                }
                else
                {
                    glColor3f(1, 1, 0);
                }
                for (int n = 0; n < 4; ++n)
                {
                    glTexCoord2f(texCoords[n][0], texCoords[n][1]);
                    glVertex3f(quad[n][0], quad[n][1], quad[n][2]);
                }
                glEnd();

                // Black backface
                glDisable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                glColor3f(0, 0, 0);
                for (int n = 3; n >= 0; --n)
                {
                    glVertex3f(quad[n][0], quad[n][1], quad[n][2]);
                }
                glEnd();

                glPopMatrix();
            }
        }
    }

    // Set-up 2D overlay
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w(), 0, h(), 0, 1);

    if (!mCube.seemsValid())
    {
        gl_font(FL_HELVETICA | FL_BOLD, 12);
        gl_color(FL_RED);
        gl_draw("Warning: invalid cube!", 10, 10);
    }
}

int MainWindow::handle(int event)
{
    switch (event)
    {
    case FL_PUSH:
        if (Fl::event_state(FL_BUTTON1))
        {
            mLastX = Fl::event_x();
            mLastY = Fl::event_y(); 
        }
        if (Fl::event_state(FL_BUTTON3))
        {
            Point p(Fl::event_x(), Fl::event_y());
            mSelectedFace = -1;
            for (int n = 0; n < Cube::num_faces; ++n)
            {
                if (point_in_quad(mFaceWinCoords[n], p))
                {
                    mSelectedFace = n;
                    break;
                }
            }
            redraw();
        }
        return 1;

    case FL_DRAG:
        if (Fl::event_state(FL_BUTTON1))
        {
            mRotY += (Fl::event_x() - mLastX);
            mRotX += (Fl::event_y() - mLastY);
            mLastX = Fl::event_x();
            mLastY = Fl::event_y();
            redraw();
        }
        return 1;

    case FL_KEYDOWN:
        switch (Fl::event_key())
        {
        case 's':
            if (Fl::event_state(FL_CTRL))
            {
                Fl_File_Chooser *fc = new Fl_File_Chooser(
                    ".", "Text Files (*.txt)\tAll Files (*)",
                    Fl_File_Chooser::CREATE, "Save Cube As");
                fc->show();
                fc->callback(main_window_save_cube_callback, this);
            }
            break;

        case 'c':
            setView();
            redraw();
            return 1;

        case 'u': moveManual(0, Fl::event_state(FL_SHIFT)); return 1;
        case 'f': moveManual(1, Fl::event_state(FL_SHIFT)); return 1;
        case 'r': moveManual(2, Fl::event_state(FL_SHIFT)); return 1;
        case 'b': moveManual(3, Fl::event_state(FL_SHIFT)); return 1;
        case 'l': moveManual(4, Fl::event_state(FL_SHIFT)); return 1;
        case 'd': moveManual(5, Fl::event_state(FL_SHIFT)); return 1;

        case '1': setSelectedFaceSymbol(0); redraw(); return 1;
        case '2': setSelectedFaceSymbol(1); redraw(); return 1;
        case '3': setSelectedFaceSymbol(2); redraw(); return 1;
        case '4': setSelectedFaceSymbol(3); redraw(); return 1;
        case '5': setSelectedFaceSymbol(4); redraw(); return 1;
        case '6': setSelectedFaceSymbol(5); redraw(); return 1;
        case '7': setSelectedFaceSymbol(6); redraw(); return 1;
        case '8': setSelectedFaceSymbol(7); redraw(); return 1;
        case '9': setSelectedFaceSymbol(8); redraw(); return 1;

        case FL_Right:      advanceAnim( +1); break;
        case FL_Left:       advanceAnim( -1); break;
        case FL_Page_Up:    advanceAnim(+10); break;
        case FL_Page_Down:  advanceAnim(-10); break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    return Fl_Gl_Window::handle(event);
}

void MainWindow::tick()
{
    double now = time_now(), dt = now - mAnimLastTime;
    mAnimLastTime = now;

    assert(mAnimTime >= 0 && mAnimTime < gFaceTurnTime);

    if (mAnimPos < mAnimGoal)
    {
        mAnimTime += dt;
        while (mAnimTime >= gFaceTurnTime)
        {
            mCube.move(mAnimMoves[mAnimPos].face, mAnimMoves[mAnimPos].turn);
            mAnimPos += 1;
            mAnimTime -= gFaceTurnTime;
            if (mAnimPos == mAnimGoal)
            {
                mAnimTime = 0;
                break;
            }
        }
    }
    else
    if (mAnimPos > mAnimGoal || (mAnimPos == mAnimGoal && mAnimTime > 0))
    {
        mAnimTime -= dt;
        while (mAnimTime < 0)
        {
            if (mAnimPos == mAnimGoal)
            {
                mAnimTime = 0;
                break;
            }
            mAnimTime += gFaceTurnTime;
            mAnimPos -= 1;
            mCube.move(mAnimMoves[mAnimPos].face, 4 - mAnimMoves[mAnimPos].turn);
        }
    }
}

void MainWindow::moveManual(int face, bool ccw)
{
    if (mAnimGoal != mAnimMoves.size())
    {
        printf("WARNING: manual move ignored (not at end of animation)\n");
        return;
    }

    Move move = { face, ccw ? 1 : 3 };
    addMove(move);
    advanceAnim(1);
}

void MainWindow::setSelectedFaceSymbol(int sym)
{
    if (mSelectedFace >= 0 && mSelectedFace < Cube::num_faces)
    {
        Face &face = mCube.face(mSelectedFace);
        if (face.sym != sym)
        {
            face.sym = sym;
        }
        else
        {
            face.rotate(1);
        }
    }
}

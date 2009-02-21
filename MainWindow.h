#ifndef MAIN_WINDOW_H_INCLUDED
#define MAIN_WINDOW_H_INCLUDED

#include "Cube.h"
#include "Move.h"

#include <FL/Fl_Gl_Window.h>
#include <FL/Fl_RGB_Image.h>
#include <FL/gl.h>

#include <vector>


class MainWindow
    : public Fl_Gl_Window
{
public:
    MainWindow( int x, int y, int w, int h, const char *winTitle,
                Fl_RGB_Image *labels );
    ~MainWindow();

    void setCube(Cube &cube) { mCube = cube; }
    const Cube &cube() { return mCube; }

    void addMove(const Move &move);
    void addMoves(const std::vector<Move> &moves);
    void setView(float rotX = 30, float rotY = -30);

    void advanceAnim(int moves = 1);

protected:
    void draw();
    int handle(int event);
    void tick();
    void moveManual(int face, bool ccw);

private:
    Cube mCube;
    int mLastX, mLastY;
    float mRotX, mRotY;
    GLuint mLabelsTexture;
    Fl_RGB_Image *mLabelsImage;

    std::vector<Move> mAnimMoves;
    double mAnimLastTime, mAnimTime;
    size_t mAnimPos, mAnimGoal;

    friend void main_window_tick_callback(void *arg);
};

#endif /* ndef MAIN_WINDOW_H_INCLUDED */

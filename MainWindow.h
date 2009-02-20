#ifndef MAIN_WINDOW_H_INCLUDED
#define MAIN_WINDOW_H_INCLUDED

#include "Cube.h"

#include <FL/Fl_Gl_Window.h>
#include <FL/Fl_RGB_Image.h>
#include <FL/gl.h>

#include <vector>
#include <deque>


class MainWindow
    : public Fl_Gl_Window
{
public:
    MainWindow( int x, int y, int w, int h, const char *winTitle,
                Fl_RGB_Image *labels );
    ~MainWindow();

    void setCube(Cube &cube) { mCube = cube; }
    const Cube &cube() { return mCube; }

    void animate(std::vector<int> moves);

protected:
    void draw();
    int handle(int event);
    bool tick();

private:
    Cube mCube;
    int mLastX, mLastY;
    double mRotX, mRotY;
    GLuint mLabelsTexture;
    Fl_RGB_Image *mLabelsImage;
    std::deque<int> mAnimMoves;

    double mAnimLastTime;
    int mAnimRotFace;
    float mAnimRotAngle;
    float mAnimRotTime;

    friend void main_window_tick_callback(void *arg);
};

#endif /* ndef MAIN_WINDOW_H_INCLUDED */

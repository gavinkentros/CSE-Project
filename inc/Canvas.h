#ifndef CANVAS_H
#define CANVAS_H

/*
A simple canvas.h file. Feel free to put this wherever you want, the graph will be represented as a circle
around the centerpoint.
You won't need to call anything through here in application.cpp, do everything through the visual graph object.
See VisualGraph.h
*/

#include <bobcat_ui/all.h>
#include <VisualGraph.h>
#include <Graph.h>

class Canvas : public bobcat::Canvas_ {

public:
    VisualGraph* vg = new VisualGraph(0, 0, 0.8);
    Canvas(int x, int y, int w, int h) : Canvas_(x, y, w, h) {}

    void render() {
        glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        vg->draw();
    }

    ~Canvas() {
        delete vg;
    }
    
};

#endif
#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include <FL/Fl_Scroll.H>
#include <Graph.h>
#include <ArrayList.h>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    ArrayList<Vertex*> cities;
    Graph g;

    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Dropdown* method;
    bobcat::Button* searchButton;
    Fl_Scroll* resultScrollArea;

    void initData();
    void initInterface();
    void onClick(bobcat::Widget* sender);

public:
    Application();
};

#endif

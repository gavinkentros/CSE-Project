#ifndef VISUALGRAPH_H
#define VISUALGRAPH_H

/*
This creates all classes for the visual representation and draws everything.
There is a public visual graph object in Canvas.h, so everything can be done through canvas->vg.
I haven't implemented the destructor yet, but I wanted to push this out so you have it. I'll do that as soon as I can.
Also, I'll try to create text boxes above the graphs if I have time and if applicable
(I didn't initially because I was concerned the graph would clutter).

Final note: if you want to change the size of each vertex in the graph, feel free to do that
in the VisualVertex draw function.
*/



#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <bobcat_ui/all.h>
#include <Graph.h>


// Visual Vertex class
class VisualVertex {
    float x;
    float y;
    Vertex* vertex;
  
public:
  VisualVertex(Vertex* vertex) {
    this->vertex = vertex;
    x = 0;
    y = 0;
  }

  float getX() {return x;}
  float getY() {return y;}
  void updateX(float x) {this->x = x;}
  void updateY(float y) {this->y = y;} 

  void draw() {
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(13.0);
    glBegin(GL_POINTS);
        glVertex2f(x, y);
    glEnd();

    gl_font(FL_HELVETICA, 9);
    const std::string& name = vertex->data;
    if (x >= 0) {
        gl_draw(name.c_str(), x + 0.04f, y - 0.015f);
    } else {
        float textWidth = name.length() * 0.04f;
        gl_draw(name.c_str(), x - 0.04f - textWidth, y - 0.015f);
    }
  }

    Vertex* getVertex() {
        return vertex;
    }

    
};


// Visual Edge class
class VisualEdge {
    
    VisualVertex* from;
    VisualVertex* to;
    bool inPath;

public:
    VisualEdge(VisualVertex* from, VisualVertex* to) {
        this->from = from;
        this->to = to;
        inPath = false;
    }    

    void draw() {
        float startX = from->getX();
        float startY = from->getY();
        float endX = to->getX();
        float endY = to->getY();
        if (inPath) {
            glColor3f(1.0, 0.0, 0.0);
        } else {
            glColor3f(0.0, 0.0, 0.0);
        }
        glLineWidth(2.0);
        glBegin(GL_LINES);
            glVertex2f(startX, startY);
            glVertex2f(endX, endY);
        glEnd();
    }

    void select() {
        inPath = true;
    }

    void deselect() {
        inPath = false;
    }

    Vertex* getFromVertex() {
        return from->getVertex();
    }

    Vertex* getToVertex() {
        return to->getVertex();
    }

};


// Visual Graph
class VisualGraph {
    ArrayList<VisualVertex*> vertices;
    ArrayList<VisualEdge*> edges;

    float x;
    float y;
    float radius;
    
public:
    VisualGraph(float x, float y, float radius) {
        this->x = x;
        this->y = y;
        this->radius = radius;
    }

// Adds a vertex to the visual graph. Use input from cities[i] during data initialization in Application.cpp
    void addVertex(VisualVertex* vertex) {
        vertices.append(vertex);
    }

// Adds an edge to the visual graph. Use the from and to integers from data initialization during this.
// All vertexes will already be in an array, so they will line up one-to-one.
    void addEdge(int from, int to) {
        edges.append(new VisualEdge(vertices[from], vertices[to]));
    }

// Call this after adding all vertexes and edges into the graph
    void updateVertexPositions() {
        int count = vertices.size();

        if (count == 0)
            return;

        float angleStep = (2.0f * 3.141592653589793238) / count;

        for (int i = 0; i < count; i++) {
            float angle = i * angleStep;

            vertices[i]->updateX(x + radius * std::cos(angle));
            vertices[i]->updateY(y + radius * std::sin(angle));
        }
    }

    void deselectAllEdges() {
        for (int i = 0; i < edges.size(); i++) {
            edges[i]->deselect();
        }
    }

    void draw() {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i]->draw();
        }
        for (int i = 0; i < edges.size(); i++) {
            edges[i]->draw();
        }
    }

// Call this when representing a path visually. It automatically does everything with the waypoint, so just plug in the waypoint path
    void updatePath(Waypoint* path) {
        deselectAllEdges();
        while(path->parent != nullptr) {
            for (int i = 0; i < edges.size(); i++) {
                if ((edges[i]->getFromVertex() == path->vertex || edges[i]->getToVertex() == path->vertex) && (edges[i]->getFromVertex() == path->parent->vertex || edges[i]->getToVertex() == path->parent->vertex)) {
                    edges[i]->select();
                    break;
                }
            }
            path = path->parent;
        }
    }

    ~VisualGraph() {
        for (int i = 0; i < vertices.size(); i++) {
            delete vertices[i];
        }
        for (int i = 0; i < edges.size(); i++) {
            delete edges[i];
        }
    }

};

#endif
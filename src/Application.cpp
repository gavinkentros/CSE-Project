#include <Application.h>
#include <bobcat_ui/dropdown.h>
#include <fstream>
#include <sstream>

using namespace bobcat;
using namespace std;

void Application::initData() {
    cout << "init Data start." << endl;
    fstream file;

    // read vertices from file
    file.open("./flights/vertices.csv", ios::in);
    if (file.is_open()) {
        cout << "File open" << endl;
        string line;
        
        while(getline(file, line)) {
            cities.append(new Vertex(line));
            cout << line << endl;
        }

        file.close();
    } else {
        cout << "file did not open" << endl;
    }

    // add vertices to graph
    for (int i = 0; i < cities.size(); i++) {
        g.addVertex(cities[i]);
        //canvas->vg->addVertex(new VisualVertex(cities[i]));
    }
    
    // read edges from file and add them as edges to the graph
    cout << "starting edges." << endl;
    file.open("./flights/edges.csv", ios::in);
    if (file.is_open()) {
        string line;
        
        while(getline(file, line)) {
            stringstream ss(line);
            string from, to, time, cost;

            getline(ss, from, ',');
            getline(ss, to, ',');
            getline(ss, time, ',');
            getline(ss, cost, ',');

            g.addEdge(cities[stoi(from)], cities[stoi(to)], stoi(time), stoi(cost));
            //canvas->vg->addEdge(stoi(from), stoi(to));
        }

        file.close();
    } else {
        cout << "edges did not open." << endl;
    }
}

void Application::initInterface() {

    fromDropdown = new Dropdown(25, 25, 350, 25, "Origin");
    toDropdown = new Dropdown(25, 75, 350, 25, "Destination");
    method = new Dropdown(25, 125, 350, 25, "Method");

    //canvas->vg->updateVertexPositions();
    //canvas->redraw();
    
    //adding methods to method dropdown
    method->add("UCS Shortest Travel Time");
    method->add("UCS Cheapest Price");
    method->add("BFS Least Number of Stops");

    for (int i = 0; i < cities.size(); i++) {
        fromDropdown->add(cities[i]->data);
        toDropdown->add(cities[i]->data);
    }

    searchButton = new Button(25, 175, 350, 25, "Search");

    resultScrollArea = new Fl_Scroll(25, 225, 350, 150, "Results");
    resultScrollArea->align(FL_ALIGN_TOP_LEFT);
    resultScrollArea->box(FL_THIN_UP_BOX);

    // register events
    ON_CLICK(searchButton, Application::onClick);

    window->show();
}

void Application::onClick(bobcat::Widget* sender) {
    if (sender == searchButton) {

        resultScrollArea->clear();

        int fromIndex = fromDropdown->value();
        int toIndex = toDropdown->value();
        string searchMethod = method->text();
        cout << "searchMethod: " << searchMethod << endl;

        // run search algorithm
        if (searchMethod == "UCS Shortest Travel Time") {
            Waypoint* path = g.ucs(cities[fromIndex], cities[toIndex], "travel");

            if (path) {

                // canvas->vg->updatePath(path);
                // canvas->redraw();
                cout << "Found path" << endl;
                int y = resultScrollArea->y() + 10;

                Waypoint* temp = path;

                while(temp != nullptr) {
                    resultScrollArea->add(new TextBox(40, y, 300, 25, temp->vertex->data));

                    y += 40;

                    if (temp->parent != nullptr) {
                        resultScrollArea->add(new TextBox(40, y, 300, 25, "\t Flight Time: " + to_string(temp->weight) + " hours"));
                        y += 40;
                    }
                    temp = temp->parent;
                }

                addSummary(path, y);

                window->redraw();

                deletePath(path);

            } else {
                cout << "There is no path" << endl;
                // canvas->vg->deselectAllEdges();
                // canvas->redraw();
            }
        } else if (searchMethod == "UCS Cheapest Price") {
            Waypoint* path = g.ucs(cities[fromIndex], cities[toIndex], "price");

            if (path) {
                cout << "Found path" << endl;
                int y = resultScrollArea->y() + 10;

                Waypoint* temp = path;

                while(temp != nullptr) {
                    resultScrollArea->add(new TextBox(40, y, 300, 25, temp->vertex->data));

                    y += 40;

                    if (temp->parent != nullptr) {
                        resultScrollArea->add(new TextBox(40, y, 300, 25, "\t Price: " + to_string(temp->price) + " dollars"));
                        y += 40;
                    }
                    temp = temp->parent;
                }

                addSummary(path, y);

                window->redraw();

                deletePath(path);

            } else {
                cout << "There is no path" << endl;
            }
        } else if (searchMethod == "BFS Least Number of Stops") {
            Waypoint* path = g.bfs(cities[fromIndex], cities[toIndex]);

            if (path) {
                cout << "Found path" << endl;
                int y = resultScrollArea->y() + 10;

                Waypoint* temp = path;

                while(temp != nullptr) {
                    resultScrollArea->add(new TextBox(40, y, 300, 25, temp->vertex->data));

                    y += 40;

                    //if (temp->parent != nullptr) {
                    //    resultScrollArea->add(new TextBox(40, y, 300, 25, "\t Flight Time:" + to_string(temp->weight) + " hours"));
                    //    y += 40;
                    //}
                    temp = temp->parent;
                }

                addSummary(path, y);

                window->redraw();

                deletePath(path);

            } else {
                cout << "There is no path" << endl;
            }
        }
    }
}

void Application::addSummary(Waypoint* path, int& y) {
    int nodeCount = 0;
    Waypoint* counter = path;
    while (counter != nullptr) {
        nodeCount++;
        counter = counter->parent;
    }
    int stops = (nodeCount >= 2) ? (nodeCount - 2) : 0;

    resultScrollArea->add(new TextBox(40, y, 300, 25, "Total Time: " + to_string(path->partialCost) + " hours"));
    y += 40;
    resultScrollArea->add(new TextBox(40, y, 300, 25, "Total Price: $" + to_string(path->partialPrice)));
    y += 40;
    resultScrollArea->add(new TextBox(40, y, 300, 25, "Stops: " + to_string(stops)));
    y += 40;
}

void Application::deletePath(Waypoint* path) {
    if (!path) return;
    Waypoint* root = path;
    while (root->parent != nullptr) {
        root = root->parent;
    }
    delete root;
}

Application::Application() {
    window = new Window(25, 75, 400, 400, "Simple Navigation Project");
    // canvas = new Canvas(400, 400, 200, 200); // (x, y, w, h) (will be circle around center of canvas)
    initData();
    initInterface();
}

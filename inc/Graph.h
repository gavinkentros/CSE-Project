#ifndef GRAPH_H
#define GRAPH_H

#include <LinkedList.h>
#include <ArrayList.h>
#include <HashTable.h>
#include <Queue.h>
#include <Stack.h>
#include <ostream>
#include <string>

struct Edge;

struct Vertex {
    std::string data;
    ArrayList<Edge *> edgeList;

    Vertex(std::string data) { this->data = data; }

    ~Vertex() {
        for (int i = 0; i < edgeList.size(); i++) {
            delete edgeList[i];
        }
    }

    Vertex(const Vertex&) = delete;
    Vertex& operator=(const Vertex&) = delete;
};

inline std::ostream &operator<<(std::ostream &os, Vertex *v) {
    os << v->data;

    return os;
}

struct Edge {
    Vertex *from;
    Vertex *to;
    int weight;
    int cost;

    Edge(Vertex *from, Vertex *to, int weight, int cost) {
        this->from = from;
        this->to = to;
        this->weight = weight;
        this->cost = cost;
    }
};

inline std::ostream &operator<<(std::ostream &os, Edge *e) {
    os << "(" << e->from << ", " << e->to << ") - " << e->weight;

    return os;
}

struct Waypoint {
    Waypoint *parent;
    Vertex *vertex;
    ArrayList<Waypoint *> children;
    int partialCost;
    int weight;
    int partialPrice; // NEW: tracks the accumulated total price
    int price;        // NEW: tracks the individual edge price

    Waypoint(Vertex *v) {
        parent = nullptr;
        vertex = v;
        weight = 0;
        partialCost = 0;
        price = 0;        // NEW
        partialPrice = 0; // NEW
    }

    void expand() {
        for (int i = 0; i < vertex->edgeList.size(); i++) {
            Waypoint *temp = new Waypoint(vertex->edgeList[i]->to);
            temp->parent = this;

            // Track Travel Time
            temp->weight = vertex->edgeList[i]->weight;
            temp->partialCost = partialCost + vertex->edgeList[i]->weight;

            // Track Ticket Price
            temp->price = vertex->edgeList[i]->cost;
            temp->partialPrice = partialPrice + vertex->edgeList[i]->cost;

            children.append(temp);
        }
    }

    ~Waypoint() {
        for (int i = 0; i < children.size(); i++) {
            delete children[i];
        }
    }

    Waypoint(const Waypoint&) = delete;
    Waypoint& operator=(const Waypoint&) = delete;
};

inline std::ostream &operator<<(std::ostream &os, Waypoint *wp) {
    std::string p = "null";
    if (wp->parent != nullptr) {
        p = wp->parent->vertex->data;
    }

    os << p << " -> " << wp->vertex->data;

    return os;
}

struct Graph {
    ArrayList<Vertex *> vertices;

    Graph() = default;

    ~Graph() {
        for (int i = 0; i < vertices.size(); i++) {
            delete vertices[i];
        }
    }

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    void addVertex(Vertex *v) { vertices.append(v); }

    void addEdge(Vertex *x, Vertex *y, int w, int c) {
        x->edgeList.append(new Edge(x, y, w, c));
        y->edgeList.append(new Edge(y, x, w, c));
    }

    void addDirectedEdge(Vertex *x, Vertex *y, int w, int c) {
        x->edgeList.append(new Edge(x, y, w, c));
    }

    Waypoint *bfs(Vertex *start, Vertex *destination) {
        std::cout << "Running Breadth-First Search" << std::endl;
        Queue<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);

        frontier.enqueue(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (!frontier.isEmpty()) {
            result = frontier.dequeue();

            if (result->vertex == destination) {
                return result;
            }

            result->expand();
            // Get the neighbors of the current vertex
            // that we are on...

            // The neighbors are stored in the result->children array
            std::cout << std::endl
                      << "Expanding " << result->vertex->data << std::endl;

            for (int i = 0; i < result->children.size(); i++) {
                // For every child of the result node
                // If we have not seen it
                // We add it to the frontier (as a queue)
                // We mark it as seen
                if (!seen.search(result->children[i]->vertex->data)) {
                    std::cout << "Adding " << result->children[i]->vertex->data
                              << std::endl;
                    frontier.enqueue(result->children[i]);
                    seen.insert(result->children[i]->vertex->data);
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            Link<Waypoint *> *temp = frontier.list.front;
            while (temp != nullptr) {
                std::cout << "(" << temp->data->vertex->data << ","
                          << temp->data->partialCost << ")";
                temp = temp->next;

                if (temp != nullptr) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        return nullptr;
    }

    Waypoint *dfs(Vertex *start, Vertex *destination) {
        std::cout << "Running Depth-First Search" << std::endl;

        Stack<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);

        frontier.push(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (!frontier.isEmpty()) {
            result = frontier.pop();

            if (result->vertex == destination) {
                return result;
            }

            result->expand();

            std::cout << std::endl
                      << "Expanding " << result->vertex->data << std::endl;

            for (int i = 0; i < result->children.size(); i++) {
                if (!seen.search(result->children[i]->vertex->data)) {
                    std::cout << "Adding " << result->children[i]->vertex->data
                              << std::endl;
                    frontier.push(result->children[i]);
                    seen.insert(result->children[i]->vertex->data);
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            Link<Waypoint *> *temp = frontier.front;
            while (temp != nullptr) {
                std::cout << "(" << temp->data->vertex->data << ","
                          << temp->data->partialCost << ")";
                temp = temp->next;

                if (temp != nullptr) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        return nullptr;
    }

    Waypoint *ucs(Vertex *start, Vertex *destination, std::string option) {
        std::cout << "Running Uniform Cost Search" << std::endl;

        // Should be a priority queue
        ArrayList<Waypoint *> frontier;
        HashTable<std::string> seen;
        if (option == "travel") {
            Waypoint *first = new Waypoint(start);

            frontier.append(first);
            seen.insert(first->vertex->data);

            Waypoint *result = nullptr;

            while (frontier.size() != 0) {
                result = frontier.removeLast();

                if (result->vertex == destination) {
                    return result;
                }

                result->expand();

                std::cout << "Expanding " << result->vertex->data << std::endl;

                for (int i = 0; i < result->children.size(); i++) {
                    // Look at each child
                    if (!seen.search(result->children[i]->vertex->data)) {
                        // If not in the seen list, let's add it
                        std::cout << "Adding " << result->children[i]->vertex->data
                                << std::endl;
                        frontier.append(result->children[i]);


                        // Sort the frontier....
                        int j = frontier.size() - 1;
                        while (j > 0 && frontier.data[j]->partialCost >
                                            frontier.data[j - 1]->partialCost) {

                            Waypoint *temp = frontier.data[j];
                            frontier.data[j] = frontier.data[j - 1];
                            frontier.data[j - 1] = temp;
                            j--;
                        }

                        seen.insert(result->children[i]->vertex->data);
                    } else {
                        // If it is in the seen list, we may have to do some work

                        // First we will check if it is still in the frontier but
                        // with a higher partial cost
                        Waypoint *worsePath = nullptr;

                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->vertex->data ==
                                result->children[i]->vertex->data) {
                                if (frontier[k]->partialCost >
                                    result->children[i]->partialCost) {
                                    worsePath = frontier[k];
                                    // The same node was visited before,
                                    // but with a higher partial cost
                                    break;
                                }
                            }
                        }

                        // If we had a worse node before, we need to change it.
                        if (worsePath) {
                            std::cout
                                << "Found another way to get to "
                                << result->children[i]->vertex->data << ". Was "
                                << worsePath->partialCost << ", but now it is "
                                << result->children[i]->partialCost << std::endl;

                            // Make it so that the children of the worse waypoint
                            // become our children
                            for (int k = 0; k < frontier.size(); k++) {
                                if (frontier[k]->parent->vertex->data ==
                                    result->children[i]->vertex->data) {
                                    frontier[k]->parent = result->children[i];
                                }
                            }

                            // Replace the worse one with the better one
                            for (int k = 0; k < frontier.size(); k++) {
                                if (frontier[k]->vertex->data ==
                                    result->children[i]->vertex->data) {
                                    delete frontier[k];
                                    frontier[k] = result->children[i];
                                    break;
                                }
                            }

                            // Sort the frontier because the replacement above
                            // may have caused things to fall out of order
                            for (int a = 1; a < frontier.size(); a++) {
                                int b = a;
                                while (b > 0 && frontier[b]->partialCost >
                                                    frontier[b - 1]->partialCost) {
                                    Waypoint *x = frontier[b];
                                    frontier[b] = frontier[b - 1];
                                    frontier[b - 1] = x;
                                    b--;
                                }
                            }
                        }
                    }
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            for (int k = frontier.size() - 1; k >= 0; k--) {
                std::cout << "(" << frontier[k]->vertex->data << ", "
                          << frontier[k]->partialCost << ") ";
                if (k > 0) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        } else if (option == "price") {
            Waypoint *first = new Waypoint(start);

            frontier.append(first);
            seen.insert(first->vertex->data);

            Waypoint *result = nullptr;

            while (frontier.size() != 0) {
                result = frontier.removeLast();

                if (result->vertex == destination) {
                    return result;
                }

                result->expand();

                std::cout << "Expanding " << result->vertex->data << std::endl;

                for (int i = 0; i < result->children.size(); i++) {
                    // Look at each child
                    if (!seen.search(result->children[i]->vertex->data)) {
                        // If not in the seen list, let's add it
                        std::cout << "Adding " << result->children[i]->vertex->data
                                << std::endl;
                        frontier.append(result->children[i]);


                        // Sort the frontier....
                        int j = frontier.size() - 1;
                        // CHANGED: Use partialPrice instead of weight
                        while (j > 0 && frontier.data[j]->partialPrice >
                                            frontier.data[j - 1]->partialPrice) {

                            Waypoint *temp = frontier.data[j];
                            frontier.data[j] = frontier.data[j - 1];
                            frontier.data[j - 1] = temp;
                            j--;
                        }

                        seen.insert(result->children[i]->vertex->data);
                    } else {
                        // If it is in the seen list, we may have to do some work

                        // First we will check if it is still in the frontier but
                        // with a higher partial cost
                        Waypoint *worsePath = nullptr;

                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->vertex->data ==
                                result->children[i]->vertex->data) {
                                // CHANGED: Use partialPrice instead of weight
                                if (frontier[k]->partialPrice >
                                    result->children[i]->partialPrice) {
                                    worsePath = frontier[k];
                                    break;
                                }
                            }
                        }

                        if (worsePath) {
                            std::cout
                                << "Found another way to get to "
                                << result->children[i]->vertex->data << ". Was "
                                << worsePath->partialPrice << ", but now it is " // CHANGED
                                << result->children[i]->partialPrice << std::endl; // CHANGED

                            // Make it so that the children of the worse waypoint
                            // become our children
                            for (int k = 0; k < frontier.size(); k++) {
                                if (frontier[k]->parent->vertex->data ==
                                    result->children[i]->vertex->data) {
                                    frontier[k]->parent = result->children[i];
                                }
                            }

                            // Replace the worse one with the better one
                            for (int k = 0; k < frontier.size(); k++) {
                                if (frontier[k]->vertex->data ==
                                    result->children[i]->vertex->data) {
                                    delete frontier[k];
                                    frontier[k] = result->children[i];
                                    break;
                                }
                            }

                            // Sort the frontier because the replacement above
                            // may have caused things to fall out of order
                            for (int a = 1; a < frontier.size(); a++) {
                                int b = a;
                                // CHANGED: Use partialPrice instead of weight
                                while (b > 0 && frontier[b]->partialPrice >
                                                    frontier[b - 1]->partialPrice) {
                                    Waypoint *x = frontier[b];
                                    frontier[b] = frontier[b - 1];
                                    frontier[b - 1] = x;
                                    b--;
                                }
                            }
                        }
                    }
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            for (int k = frontier.size() - 1; k >= 0; k--) {
                std::cout << "(" << frontier[k]->vertex->data << ", "
                          << frontier[k]->partialPrice << ") "; // CHANGED
                if (k > 0) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
        }

        return nullptr;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Graph &g) {
    for (int i = 0; i < g.vertices.size(); i++) {
        os << g.vertices[i]->edgeList << std::endl;
    }

    return os;
}

#endif

#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <unordered_set>
#include <algorithm>

#define POINT_SIZE 8

struct Point {
    double x, y;
    std::vector<Point *> edge;

    Point(double x, double y) : x(x), y(y) {}

    bool operator==(const Point &to_compare) const {
        return to_compare.x <= x + POINT_SIZE / 2 && to_compare.x >= x - POINT_SIZE / 2
               && to_compare.y <= y + POINT_SIZE / 2 && to_compare.y >= y - POINT_SIZE / 2;
    }

    bool operator<(const Point &to_compare) const {
        if (y == to_compare.y) {
            return x < to_compare.x;
        }

        return y > to_compare.y;
    }
};

struct Edge {
    int weight;
    Point *from;
    Point *to;

    Edge(Point *from, Point *to, int weight = 1) : from(from), to(to), weight(weight) {}
};

struct Chain {
    std::vector<Point *> chain;

    Chain() {
        chain = std::vector<Point *>();
    }

    Chain(const Chain &to_construct) {
        chain = to_construct.chain;
    }

    void addVertex(Point *vertex) {
        chain.push_back(vertex);
    }

    void pop() {
        chain.erase(chain.end() - 1);
    }

    bool operator==(const Chain &to_compare) const {
        if (chain.size() != to_compare.chain.size()) {
            return false;
        }

        for (int i = 0, size = chain.size(); i < size; ++i) {
            if (*(chain[i]) == *(to_compare.chain[i])) {
                continue;
            }

            return false;
        }

        return true;
    }
};

int mode = 0; // 0 - selecting points, 1 - setting lines, 2 - apply algorithm
std::vector<Point *> vertexes;
std::vector<Edge *> edges;
std::vector<Chain *> chains;
Point* point_to_check = nullptr;

void display(void);
void drawPointToCheck();
void drawPoints();
void drawEdges();
void reshape(int w, int h);
void mouseHandler(int button, int state, int x, int y);
void sortVertexes();
void generateEdges();
void algorithmTopToBottom();
void algorithmBottomToTop();
void getChains();
std::pair<Chain*, Chain*> locatePoint();
bool pointToTheLeft(Chain* chain, Point* point);

int main(int argc, char **argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(250, 250);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Lab 2");
    glutDisplayFunc(display);
    glutMouseFunc(mouseHandler);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawPoints();
    drawEdges();

    if (mode == 2){
        drawPointToCheck();
    }

    glutSwapBuffers();
}

void drawPointToCheck(){
    if (point_to_check != nullptr){
        glPointSize(POINT_SIZE);
        glColor3f(0.0, 1.0, 0.0);

        glBegin(GL_POINT);
            glVertex2d(point_to_check->x, point_to_check->y);
        glEnd();
    }
}

void drawPoints() {
    glPointSize(POINT_SIZE);
    glColor3f(0.0, 0.0, 1.0);

    glBegin(GL_POINTS);
    for (auto &vertex : vertexes) {
        glVertex2d(vertex->x, vertex->y);
    }
    glEnd();
}

void drawEdges() {
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    for (auto &vertex : vertexes) {
        for (auto &connect : vertex->edge) {
            glVertex2d(vertex->x, vertex->y);
            glVertex2d(connect->x, connect->y);
        }
    }
    glEnd();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glScalef(1, -1, 1);
    glTranslatef(0, -h, 0);
}

void sortVertexes() {
    std::sort(vertexes.begin(), vertexes.end(), [](Point *first, Point *second) {
        if (first->y == second->y) {
            return first->x < second->x;
        }

        return first->y > second->y;
    });

    for (Point *p : vertexes) {
        std::cout << p->x << " " << p->y << ", ";
    }

    std::cout << "\n";
}

void generateEdges() {
    sortVertexes();

    edges.clear();

    for (Point *p : vertexes) {
        std::sort(p->edge.begin(), p->edge.end(), [](Point *first, Point *second) {
            if (first->x == second->x) {
                return first->y > second->y;
            }

            return first->x < second->x;
        });

        for (Point *next : p->edge) {
            if (*p < *next) {
                edges.push_back(new Edge(p, next));
            }
        }
    }
}

void algorithmBottomToTop() {
    for (int i = 1, size = vertexes.size(); i < size - 1; ++i) {
        int input = 0, output = 0;
        int *input_ptr = &input;
        int *output_ptr = &output;
        std::count_if(edges.begin(), edges.end(), [i, input_ptr](Edge *edge) {
            if (*(edge->to) == *vertexes[i]) {
                *input_ptr += edge->weight;
                return true;
            }

            return false;
        });

        std::count_if(edges.begin(), edges.end(), [i, output_ptr](Edge *edge) {
            if (*(edge->from) == *vertexes[i]) {
                *output_ptr += edge->weight;
                return true;
            }

            return false;
        });

        if (input > output) {
            auto it = std::find_if(edges.begin(), edges.end(), [i](Edge *edge) {
                return *(edge->from) == *vertexes[i];
            });
            (*it)->weight += input - output;
        }
    }
}

void algorithmTopToBottom() {
    for (int size = vertexes.size(), i = size - 2; i > 0; --i) {
        int input = 0, output = 0;
        int *input_ptr = &input;
        int *output_ptr = &output;

        std::count_if(edges.begin(), edges.end(), [i, input_ptr](Edge *edge) {
            if (*(edge->to) == *vertexes[i]) {
                *input_ptr += edge->weight;
                return true;
            }

            return false;
        });

        std::count_if(edges.begin(), edges.end(), [i, output_ptr](Edge *edge) {
            if (*(edge->from) == *vertexes[i]) {
                *output_ptr += edge->weight;
                return true;
            }

            return false;
        });

        if (input < output) {
            auto it = std::find_if(edges.begin(), edges.end(), [i](Edge *edge) {
                return *(edge->to) == *vertexes[i];
            });
            (*it)->weight += output - input;
        }
    }
}

void getChains() {
    int chains_size = 0;

    for (int i = 0, size = vertexes[0]->edge.size(); i < size; ++i) {
        auto it = std::find_if(edges.begin(), edges.end(), [i](Edge *edge) {
            return *(edge->from) == *vertexes[0] && *(edge->to) == *vertexes[0]->edge[i];
        });

        if (it != edges.end()) {
            chains_size += (*it)->weight;
        }
    }

    chains.clear();
    Point* startPoint = vertexes[0];
    Point* endPoint = vertexes[vertexes.size() - 1];

    Point* current;
    bool reset = true;
    while (true){
        if(chains.size() == chains_size && reset){
            break;
        }

        if (reset){
            reset = false;
            current = startPoint;
            chains.push_back(new Chain());
            chains[chains.size() - 1]->addVertex(current);
        }

        for (Point* next : current->edge){
            auto it = std::find_if(edges.begin(), edges.end(), [current, next](Edge *edge) {
                return *(edge->from) == *current && *(edge->to) == *next;
            });

            if (*next == *endPoint && it != edges.end() && (*it)->weight > 0) {
                    chains[chains.size() - 1]->addVertex(next);
                    --((*it)->weight);
                    reset = true;
                    break;
            }

            if (it != edges.end() && (*it)->weight > 0) {
                current = next;
                --((*it)->weight);
                chains[chains.size() - 1]->addVertex(next);
                break;
            }
        }
    }
}

bool pointToTheLeft(Chain* chain, Point* point){

}

std::pair<Chain*, Chain*> locatePoint(){
    int leftIndex = 0, rightIndex = chains.size()-1;

    while(rightIndex - leftIndex != 1){
        int curIndex = (rightIndex - leftIndex) /2;
        Chain* curChain = chains[curIndex];
        if (left){
            rightIndex = curIndex;
        }else{
            leftIndex = curIndex;
        }
    }
}

void mouseHandler(int button, int state, int x, int y) {
    static auto prev = vertexes.end();

    auto curPoint = new Point((double) x, (double) y);

    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        if (mode == 0) {
            //entering points
            if (std::find(vertexes.begin(), vertexes.end(), curPoint) == vertexes.end()) {
                std::cout << x << " " << y << "\n";
                vertexes.push_back(curPoint);

                prev = vertexes.end();
            }
        } else if (mode == 1) {
            //entering edges
            auto temp = std::find_if(vertexes.begin(), vertexes.end(), [curPoint](Point *to_check) {
                return *curPoint == *to_check;
            });

            if (temp != vertexes.end()) {
                if (prev == vertexes.end()) {
                    prev = temp;
                } else {
                    if (**temp == **prev) {
                        return;
                    }

                    Point *prev_ptr = *prev;
                    if (std::find_if((*temp)->edge.begin(), (*temp)->edge.end(), [prev_ptr](Point *to_check) {
                        return *prev_ptr == *to_check;
                    }) == (*temp)->edge.end()) {
                        std::cout << (*temp)->x << " " << (*temp)->y << " : " << (*prev)->x << " " << (*prev)->y
                                  << "\n";
                        (*temp)->edge.push_back(*prev);
                        (*prev)->edge.push_back(*temp);
                    }
                    prev = vertexes.end();
                }
            }
        }
    } else if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
        std::cout << "MODE CHANGED\n";
        mode = (mode + 1) % 3;

        if (mode == 2) {
            std::cout << "Applying algorithm\n";

            generateEdges();
            for (Edge *ed : edges) {
                std::cout << "(" << ed->from->x << ", " << ed->from->y << "):(" << ed->to->x << ", " << ed->to->y
                          << ") ";
            }
            std::cout << "\n";

            algorithmBottomToTop();
            algorithmTopToBottom();
            getChains();

            for (Chain *ch : chains) {
                for (Point *pt : ch->chain) {
                    std::cout << pt->x << " " << pt->y << ", ";
                }
                std::cout << "\n";
            }

            point_to_check = curPoint;



        }else{
            point_to_check = nullptr;
        }
    }
}
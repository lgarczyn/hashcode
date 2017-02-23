#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

class Calculator {
    struct EndPoint {
        int mainLatency;
        map<int, int> serverLatencies;
    };

    struct Request{
        int videoId;
        int endpointId;
        int count;
    };

    int v_videoNum = 0;
    int e_endpointNum = 0;
    int r_requestNum = 0;
    int c_serverNum = 0;
    int x_serverMB = 0;

    vector<int> videos;
    vector<EndPoint> endpoints;
    vector<Request> requests;


    public:
    void init(string name) {
        string line;
        ifstream file (name);

        file >> v_videoNum;
        file >> e_endpointNum;
        file >> r_requestNum;
        file >> c_serverNum;
        file >> x_serverMB;

        videos = vector<int>(v_videoNum);
        for (int i = 0; i < v_videoNum; i++)
            file >> videos[i];

        endpoints = vector<EndPoint>(e_endpointNum);
        for (int i = 0; i < e_endpointNum; i++) {
            file >> endpoints[i].mainLatency;
            int connections;
            file >> connections;
            for (int j = 0; j < connections; j++) {
                int id;
                file >> id;
                file >> endpoints[i].serverLatencies[id];
            }
        }

        requests = vector<Request>(r_requestNum);
        for (int i = 0; i < r_requestNum; i++) {
            file >> requests[i].videoId;
            file >> requests[i].endpointId;
            file >> requests[i].count;
        }

        std::cout << "test\n";
    }
};

int main(int argc, char **argv) {
    string name;

    if (argc == 1)
        name = "in.txt";
    else
        name = string(argv[0]);

    Calculator calc;
    calc.init(name);
}


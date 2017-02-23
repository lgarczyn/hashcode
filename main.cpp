#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
using namespace std;

class Calculator {

public:
    struct EndPoint {
        int serverLatency;
        map<int, int> cacheLatencies;
    };

    struct Request{
        int videoId;
        int endpointId;
        int count;
    };

    size_t v_videoNum = 0;
    size_t e_endpointNum = 0;
    size_t r_requestNum = 0;
    size_t c_cacheNum = 0;
    size_t x_cacheMB = 0;

    vector<int> videos;
    vector<EndPoint> endpoints;
    vector<Request> requests;

    void init(string name) {
        string line;
        ifstream file (name);

        file >> v_videoNum;
        file >> e_endpointNum;
        file >> r_requestNum;
        file >> c_cacheNum;
        file >> x_cacheMB;

        videos = vector<int>(v_videoNum);
        for (int i = 0; i < v_videoNum; i++)
            file >> videos[i];

        endpoints = vector<EndPoint>(e_endpointNum);
        for (int i = 0; i < e_endpointNum; i++) {
            file >> endpoints[i].serverLatency;
            int connections;
            file >> connections;
            for (int j = 0; j < connections; j++) {
                int id;
                file >> id;
                file >> endpoints[i].cacheLatencies[id];
            }
        }

        requests = vector<Request>(r_requestNum);
        for (int i = 0; i < r_requestNum; i++) {
            file >> requests[i].videoId;
            file >> requests[i].endpointId;
            file >> requests[i].count;
        }
    }

    float getScore(int latencyServer, int latencyCache, int size, int requestNum) {
        return (requestNum * (latencyServer - latencyCache)) / (float)size;
    }

    //matches video to score
    using ScoreMap=map<int,float>;

    vector<ScoreMap> videoScores;

    void process() {
        videoScores = vector<ScoreMap>(c_cacheNum);

        //foreach request
        for (int i = 0; i < r_requestNum; i++) {
            //get request
            const Request& r = requests[i];
            //get request destination
            const EndPoint& ep = endpoints[r.endpointId];

            //for each cache near destination
            for (auto it = ep.cacheLatencies.begin(); it != ep.cacheLatencies.end(); it++ ) {
                int cache = it->first;
                int video = r.videoId;

                int serverLatency = ep.serverLatency;
                int cacheLatency = it->second;
                int videoSize = videos[r.videoId];
                int requestNum = r.count;

                //increment the video score for the cache by the score of the video
                videoScores[cache][video] += getScore(
                        serverLatency, //latency from server to endpoint
                        cacheLatency,//latency to this cache
                        videoSize,
                        requestNum);
            }
        }
    }


    using VideoMap=map<float, int>;

    using VideoList=vector<int>;
    vector<VideoList> videoLists;

    void filter() {

        videoLists = vector<VideoList>(c_cacheNum);
        for (int i = 0; i < c_cacheNum; i++) {

            VideoMap map;
            for (auto it = videoScores[i].begin(); it != videoScores[i].end(); it++) {
                map[it->second] = it->first;
            }
            int sum = 0;
            for (auto it = map.begin(); it != map.end(); it++) {

                int video = it->second;
                int size = videos[video];

                if (sum + size <= x_cacheMB) {
                    videoLists[i].push_back(video);
                }
            }
        }
    }

    void write(string name) {
        ofstream file(name);
        file << c_cacheNum << endl;

        for (int i = 0; i < c_cacheNum; i++) {
            file << i;

            for (auto it = videoLists[i].begin(); it != videoLists[i].end(); it++) {
                file << " " << *it;
            }
            file << endl;
        }
    }
};

int main(int argc, char **argv) {
    string name;

    if (argc > 1)
        name = string(argv[0]);
    else
        name = "in.txt";

    Calculator calc;
    calc.init(name);
    calc.process();
    calc.filter();
    calc.write("out.txt");
    //cout << "done" << endl;
}


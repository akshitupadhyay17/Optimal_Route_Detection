#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdlib>

using namespace std;

template<typename T>
class Graph {
private:
    unordered_map<T, vector<pair<float, T>>> adjacencyList;
    vector<T> path;

    bool check(const T& src, const T& dest) {
        int f = 0;
        for (const auto& node : path) {
            if (node == src) f++;
            else if (node == dest) f++;
        }
        return f == 2;
    }

public:
    void addEdge(T u, T v, float dist, bool bidirectional) {
        adjacencyList[u].push_back({dist, v});
        if (bidirectional) {
            adjacencyList[v].push_back({dist, u});
        }
    }

    void dijkstraSSSP(T source, unordered_map<T, float>& dist, unordered_map<T, T>& prev) {
        priority_queue<pair<float, T>, vector<pair<float, T>>, greater<pair<float, T>>> pq;
        dist.clear();
        prev.clear();

        for (const auto& pair : adjacencyList) {
            dist[pair.first] = numeric_limits<float>::max();
        }

        dist[source] = 0.0f;
        pq.push({0.0f, source});

        while (!pq.empty()) {
            auto current = pq.top();
            float currentDist = current.first;
            T currentNode = current.second;
            pq.pop();

            if (currentDist > dist[currentNode])
                continue;

            for (const auto& neighbor : adjacencyList[currentNode]) {
                float weight = neighbor.first;
                T neighborNode = neighbor.second;
                float newDist = currentDist + weight;

                if (newDist < dist[neighborNode]) {
                    dist[neighborNode] = newDist;
                    prev[neighborNode] = currentNode;
                    pq.push({newDist, neighborNode});
                }
            }
        }
    }

    void getShortestPathTo(T destination, unordered_map<T, T>& prev) {
        path.clear();
        for (T at = destination; prev.find(at) != prev.end() || at == destination; ) {
            path.push_back(at);
            if (prev.find(at) == prev.end()) break;
            at = prev[at];
        }
        reverse(path.begin(), path.end());

        cout << "\t\t\tPath:\n";
        for (const auto& station : path) {
            cout << "\t\t\t" << station << "\n";
        }
    }

    void calcPrice(const string& sourceStation, const string& destStation) {
        const int maxStations = 235;
        vector<vector<int>> fareMatrix(maxStations, vector<int>(maxStations, 0));
        unordered_map<string, int> stationMap;

        ifstream fareReader("fare.csv");
        if (!fareReader.is_open()) {
            cerr << "Error reading fare.csv\n";
            return;
        }

        string line;
        int i = 0;
        while (getline(fareReader, line)) {
            stringstream ss(line);
            string fareStr;
            int j = 0;
            while (getline(ss, fareStr, ',')) {
                fareStr.erase(0, fareStr.find_first_not_of(" \t\r\n"));
                fareStr.erase(fareStr.find_last_not_of(" \t\r\n") + 1);
                if (!fareStr.empty()) {
                    fareMatrix[i][j] = stoi(fareStr);
                }
                j++;
            }
            i++;
        }
        fareReader.close();

        ifstream stationReader("stations.csv");
        if (!stationReader.is_open()) {
            cerr << "Error reading stations.csv\n";
            return;
        }
        getline(stationReader, line); // Read the header line
        while (getline(stationReader, line)) {
            stringstream ss(line);
            string idStr, stationName;
            if (getline(ss, idStr, ',') && getline(ss, stationName, ',')) {
                stationName.erase(0, stationName.find_first_not_of(" \t\r\n"));
                stationName.erase(stationName.find_last_not_of(" \t\r\n") + 1);
                stationMap[stationName] = stoi(idStr);
            }
        }
        stationReader.close();

        if (stationMap.find(sourceStation) == stationMap.end() || stationMap.find(destStation) == stationMap.end()) {
            cout << "\n\t\t\tInvalid Station Entered!\n";
            return;
        }

        int srcId = stationMap[sourceStation];
        int destId = stationMap[destStation];

        int fare = fareMatrix[srcId - 1][destId - 1];
        cout << "\n\t\t\t--> Fare is: ₹" << fare << "\n";
    }

    void makeDotFile(const string& inputFile, const string& outputFile) {
        string clr = "red";
        string delimiter = ",";

        ifstream reader(inputFile);
        ofstream writer(outputFile);

        if (!reader.is_open() || !writer.is_open()) {
            cerr << "Error opening files for DOT creation.\n";
            return;
        }

        writer << "graph G {\n";
        writer << "node [shape=rect,dpi=600] margin=0.75\n\n";
        writer << "//" << clr << "\n";

        string line;
        while (getline(reader, line)) {
            stringstream ss(line);
            string a, b, label;
            if (getline(ss, a, ',') && getline(ss, b, ',') && getline(ss, label, ',')) {
                a.erase(0, a.find_first_not_of(" \t\r\n"));
                a.erase(a.find_last_not_of(" \t\r\n") + 1);
                b.erase(0, b.find_first_not_of(" \t\r\n"));
                b.erase(b.find_last_not_of(" \t\r\n") + 1);
                label.erase(0, label.find_first_not_of(" \t\r\n"));
                label.erase(label.find_last_not_of(" \t\r\n") + 1);

                if (a == "Seelampur") {
                    clr = "blue";
                } else if (a == "Golf Course") {
                    clr = "green";
                } else if (a == "Sant Guru Ram Singh Marg") {
                    clr = "violet";
                } else if (a == "JL Nehru Stadium") {
                    clr = "yellow";
                }

                writer << "// Current color: " << clr << "\n";

                if (check(a, b)) {
                    writer << "\"" << a << "\" -- \"" << b << "\" [label=\"" << label << "\",color=" << clr
                           << ",penwidth=\"8\"];\n";
                } else {
                    writer << "\"" << a << "\" -- \"" << b << "\" [label=\"" << label << "\",color=" << clr
                           << ",penwidth=\"2\"];\n";
                }
            }
        }
        writer << "}\n";
        cout << "DOT file created successfully: " << outputFile << "\n";
    }
};

int main() {
    Graph<string> metro;

    metro.addEdge("Rithala", "Netaji Subhash Place", 5.2f, true);
    metro.addEdge("Netaji Subhash Place", "Keshav Puram", 1.2f, true);
    metro.addEdge("Keshav Puram", "Kanhaiya Nagar", 0.8f, true);
    metro.addEdge("Kanhaiya Nagar", "Inderlok", 1.2f, true);
    metro.addEdge("Inderlok", "Shastri Nagar", 1.2f, true);
    metro.addEdge("Shastri Nagar", "Pratap Nagar", 1.7f, true);
    metro.addEdge("Pratap Nagar", "Pulbangash", 0.8f, true);
    metro.addEdge("Pulbangash", "Tis Hazari", 0.9f, true);
    metro.addEdge("Tis Hazari", "Kashmere Gate", 1.1f, true);
    metro.addEdge("Kashmere Gate", "Shastri Park", 2.2f, true);
    metro.addEdge("Shastri Park", "Seelampur", 1.6f, true);
    metro.addEdge("Seelampur", "Welcome", 1.1f, true);
    metro.addEdge("Rajouri Garden", "Ramesh Nagar", 1.0f, true);
    metro.addEdge("Ramesh Nagar", "Moti Nagar", 1.2f, true);
    metro.addEdge("Moti Nagar", "Kirti Nagar", 1.0f, true);
    metro.addEdge("Kirti Nagar", "Shadipur", 0.7f, true);
    metro.addEdge("Shadipur", "Patel Nagar", 1.3f, true);
    metro.addEdge("Patel Nagar", "Rajender Place", 0.9f, true);
    metro.addEdge("Rajender Place", "Karol Bagh", 1.0f, true);
    metro.addEdge("Karol Bagh", "Rajiv Chowk", 3.4f, true);
    metro.addEdge("Rajiv Chowk", "Barakhamba Road", 0.7f, true);
    metro.addEdge("Barakhamba Road", "Mandi House", 1.0f, true);
    metro.addEdge("Mandi House", "Pragati Maiden", 0.8f, true);
    metro.addEdge("Pragati Maiden", "Inderprastha", 0.8f, true);
    metro.addEdge("Inderprastha", "Yamuna Bank", 1.8f, true);
    metro.addEdge("Yamuna Bank", "Laxmi Nagar", 1.3f, true);
    metro.addEdge("Laxmi Nagar", "Nirman Vihar", 1.1f, true);
    metro.addEdge("Nirman Vihar", "Preet Vihar", 1.0f, true);
    metro.addEdge("Preet Vihar", "Karkar Duma", 1.2f, true);
    metro.addEdge("Karkar Duma", "Anand Vihar", 1.1f, true);
    metro.addEdge("Anand Vihar", "Kaushambi", 0.8f, true);
    metro.addEdge("Kaushambi", "Vaishali", 1.6f, true);
    metro.addEdge("Yamuna Bank", "Akshardham", 1.3f, true);
    metro.addEdge("Akshardham", "Mayur Vihar Phase-1", 1.8f, true);
    metro.addEdge("Mayur Vihar Phase-1", "Mayur Vihar Extention", 1.2f, true);
    metro.addEdge("Mayur Vihar Extention", "New Ashok Nagar", 0.9f, true);
    metro.addEdge("New Ashok Nagar", "Noida Sector-15", 1.0f, true);
    metro.addEdge("Noida Sector-15", "Noida Sector-16", 1.1f, true);
    metro.addEdge("Noida Sector-16", "Noida Sector-18", 1.1f, true);
    metro.addEdge("Noida Sector-18", "Botanical Garden", 1.1f, true);
    metro.addEdge("Botanical Garden", "Golf Course", 1.2f, true);
    metro.addEdge("Golf Course", "Noida City Center", 1.3f, true);

    // Green Line
    metro.addEdge("Madipur", "Shivaji Park", 1.1f, true);
    metro.addEdge("Shivaji Park", "Punjabi Bagh", 1.6f, true);
    metro.addEdge("Punjabi Bagh", "Ashok Park", 0.9f, true);
    metro.addEdge("Ashok Park", "Inderlok", 1.4f, true);
    metro.addEdge("Ashok Park", "Sant Guru Ram Singh Marg", 1.1f, true);
    metro.addEdge("Sant Guru Ram Singh Marg", "Kirti Nagar", 1.0f, true);
    metro.addEdge("Kashmere Gate", "Lal Qila", 1.5f, true);
    metro.addEdge("Lal Qila", "Jama Masjid", 0.8f, true);
    metro.addEdge("Jama Masjid", "Delhi Gate", 1.4f, true);
    metro.addEdge("Delhi Gate", "ITO", 1.3f, true);
    metro.addEdge("ITO", "Mandi House", 0.8f, true);
    metro.addEdge("Mandi House", "Janptah", 1.4f, true);
    metro.addEdge("Janptah", "Central Secretariat", 1.3f, true);
    metro.addEdge("Central Secretariat", "Khan Market", 2.1f, true);
    metro.addEdge("Khan Market", "JL Nehru Stadium", 1.4f, true);
    metro.addEdge("JL Nehru Stadium", "Jangpura", 0.9f, true);
    
    // Yellow Line
    metro.addEdge("Vishwavidyalaya", "Vidhan Sabha", 1.0f, true);
    metro.addEdge("Vidhan Sabha", "Civil Lines", 1.3f, true);
    metro.addEdge("Civil Lines", "Kashmere Gate", 1.1f, true);
    metro.addEdge("Kashmere Gate", "Chandni Chowk", 1.1f, true);
    metro.addEdge("Chandni Chowk", "Chawri Bazar", 1.0f, true);
    metro.addEdge("Chawri Bazar", "New Delhi", 0.8f, true);
    metro.addEdge("New Delhi", "Rajiv Chowk", 1.1f, true);
    metro.addEdge("Rajiv Chowk", "Patel Chowk", 1.3f, true);
    metro.addEdge("Patel Chowk", "Central Secretariat", 0.9f, true);
    metro.addEdge("Central Secretariat", "Udyog Bhawan", 0.3f, true);
    metro.addEdge("Udyog Bhawan", "Lok Kalyan Marg", 1.6f, true);
    metro.addEdge("Lok Kalyan Marg", "Jor Bagh", 1.2f, true);
    metro.addEdge("Samaypur Badli", "Rohini Sector - 18", 0.8f, true);
    metro.addEdge("Rohini Sector - 18", "Haiderpur Badli Mor", 1.3f, true);
    metro.addEdge("Haiderpur Badli Mor", "Jahangirpuri", 1.3f, true);
    metro.addEdge("Jahangirpuri", "Adarsh Nagar", 1.3f, true);
    metro.addEdge("Adarsh Nagar", "Azadpur", 1.5f, true);
    metro.addEdge("Azadpur", "Model Town", 1.4f, true);
    metro.addEdge("Model Town", "GTB Nagar", 1.4f, true);
    metro.addEdge("GTB Nagar", "Vishwavidyalaya", 0.8f, true);
    metro.addEdge("Jor Bagh", "INA", 1.3f, true);
    metro.addEdge("INA", "AIIMS", 0.8f, true);
    metro.addEdge("AIIMS", "Green Park", 1.0f, true);
    metro.addEdge("Green Park", "Hauz Khas", 1.8f, true);
    metro.addEdge("Hauz Khas", "Malviya Nagar", 1.7f, true);
    metro.addEdge("Malviya Nagar", "Saket", 0.9f, true);
    metro.addEdge("Saket", "Qutab Minar", 1.7f, true);
    metro.addEdge("Qutab Minar", "Chhattarpur", 1.3f, true);
    metro.addEdge("Chhattarpur", "Sultanpur", 1.6f, true);
    metro.addEdge("Sultanpur", "Ghitorni", 1.3f, true);
    metro.addEdge("Ghitorni", "Arjan Garh", 2.7f, true);
    metro.addEdge("Arjan Garh", "Guru Dronacharya", 2.3f, true);
    metro.addEdge("Guru Dronacharya", "Sikandarpur", 1.0f, true);
    metro.addEdge("Sikandarpur", "MG Road", 0.9f, true);
    metro.addEdge("MG Road", "Iffco Chowk", 1.5f, true);
    metro.addEdge("Iffco Chowk", "Sushant Lok", 1.5f, true);

    cout << "\n\n\n";
    cout << "\t\tEnter source station in capital case: ";
    string sourceStation;
    getline(cin, sourceStation);
    cout << "\t\tEnter destination station in capital case: ";
    string destStation;
    getline(cin, destStation);

    unordered_map<string, float> dist;
    unordered_map<string, string> prev;

    metro.dijkstraSSSP(sourceStation, dist, prev);

    if (dist.find(destStation) == dist.end() || dist[destStation] == numeric_limits<float>::max()) {
        cout << "\n\t\t\tInvalid Station Entered!\n";
        return 0;
    }

    cout << "\n\t\tDistance from " << sourceStation << " to " << destStation << " - " << dist[destStation] << " Kms\n";
    metro.getShortestPathTo(destStation, prev);
    metro.calcPrice(sourceStation, destStation);
    metro.makeDotFile("data.txt", "finalmap.dot");
    
    int result = system("dot -Tpng finalmap.dot -o path.png");
    if (result == 0) {
        cout << "Graph image generated successfully as path.png\n";
    } else {
        cout << "Error running Graphviz 'dot' command.\n";
    }

    return 0;
}

#include "UCI.h"
#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

UCI::UCI(const UCINames &name, const string &prefix) {
    setDetails(name);
    loadFromFile(prefix);
}

void UCI::setDetails(const UCINames &name) {
    switch(name) {
        case UCINames::IRIS:
            details = &UCI_IRIS_DETAILS;
            break;
        case UCINames::CANCER:
            details = &UCI_CANCER_DETAILS;
            break;
        case UCINames::WINE:
            details = &UCI_WINE_DETAILS;
            break;
    }
}

void UCI::loadFromFile(const string &prefix) {
    data = vector<CSVRow>(details->num_rows);

    ifstream file;
    file.open(prefix + "/" + details->file_name);

    int lines_read = 0;
    string line;
    // XXX we just stop reading after we run out of lines or read as many as is expected
    // without checking or reporting which case happens
    while(getline(file, line) && lines_read < details->num_rows) {
        parseLine(data[lines_read], line);
        lines_read++;
    }

    file.close();
}

void UCI::parseLine(CSVRow &csv_row, const std::string &comma_separated_line) {
    vector<string> items;
    istringstream iss(comma_separated_line);
    string item;
    while(getline(iss, item, ',')) {
        items.push_back(item);
    }
    // XXX assuming that items.size() is consistent with details->num_cols
    csv_row.x = vector<float>(0);
    for(int i = 0; i < details->num_cols; i++) {
        if(std::none_of(details->indices_to_ignore.cbegin(), details->indices_to_ignore.cend(), [i](int j){ return i == j; })) {
            if(i == details->label_index) {
                csv_row.y = items[i];
                labels.insert(items[i]);
            } else {
                csv_row.x.push_back(stof(items[i]));
            }
        }
    }
}

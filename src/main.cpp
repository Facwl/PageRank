// Simple implementation of the PageRank algorithm.
// Author: Jin Yeom (jinyeom@utexas.edu)
//
// This program reads a graph from a file with DIMACS format, and performs
// the PageRank algorithm to rank each node in the graph.

#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <exception>

#include <math.h>

#include "../include/graph.h"
#include "../include/pagerank.h"
#include "../include/util.h"

#define THRESHOLD 10e-4

void Usage(char *prog) {
    std::cout << "Usage: " << prog
              << " --filename FILENAME"
              << " --fmt FORMAT"
              << " --style STYLE"
              << " --df DF (default=0.85)"
              << std::endl;
}

int main(int argc, char **argv) {    
    std::string filename;
    std::string filefmt;
    std::string style;
    double d = 0.85;    // dampening factor (default = 0.85)

    // Parse program arguments.
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--filename") == 0) {
            filename = argv[++i];
        } else if (strcmp(argv[i], "--fmt") == 0) {
            filefmt = argv[++i];
        } else if (strcmp(argv[i], "--style") == 0) {
            style = argv[++i];
            if (style.compare("pull") != 0 && style.compare("push") != 0) {
                Usage(argv[0]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "--df") == 0) {
            d = std::stod(argv[++i]);
        } else {
            Usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (filename.empty() || filefmt.empty() || style.empty()) {
        Usage(argv[0]);
        return EXIT_FAILURE;
    }

    std::cout << "Graph file name: " << filename << std::endl;
    std::cout << "Graph file format: " << filefmt << std::endl;
    std::cout << "PageRank algorithm style: " << style << std::endl;
    std::cout << "Damping factor: " << d << std::endl;

    PageRank* pr = new PageRank();
    try {
        std::cout << "Initializing PageRank...";
        pr->InitGraph(filename, filefmt);
        std::cout << "\x1b[32mdone\x1b[0m" << std::endl;
    } catch (std::exception &e) {
        std::cout << "\x1b[31mFAILED\x1b[0m" << std::endl;
        std::cout << "\x1b[31merror\x1b[0m: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Decide the algorithm style.
    std::vector<double> (PageRank::*update)(double) = NULL;
    if (style.compare("pull") == 0) {
        update = &PageRank::PullUpdate;
    } else {
        update = &PageRank::PushUpdate;
    }

    std::vector<double> r1 = (*pr.*update)(d);
    std::vector<double> r2 = (*pr.*update)(d);
    while (Progress(r1, r2, THRESHOLD)) {
        r1 = pr->PullUpdate(d);
        r2 = pr->PullUpdate(d);
    }

    // Clean up the final ranks to sum up to 1.0.
    std::vector<double>* ranks = pr->PageRanks();
    double sum = 0.0;
    for (std::vector<double>::const_iterator it = (*ranks).begin(); it < (*ranks).end(); ++it) {
        sum += *it;
    }
    for (std::vector<double>::iterator it = (*ranks).begin(); it < (*ranks).end(); ++it) {
        *it = *it / sum;
    }

    std::cout << "Result:" << std::endl;
    PrintVector(*ranks);

    return EXIT_SUCCESS;
}

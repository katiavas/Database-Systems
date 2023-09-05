#include "../include/Bench.h"
#include <iostream>
#include <chrono>
#include "../include/SimpleGraph.h"
#include "../include/Estimator.h"
#include "../include/SimpleEstimator.h"
#include "../include/SimpleEvaluator.h"
#include "../include/Query.h"
#include "../include/QueryParser.h"


std::vector<Triple> parseQueries(std::string &fileName) {
	
	std::vector<Triple> queries {};
	
	std::string line;
	std::ifstream graphFile { fileName };
	
	std::regex edgePat (R"((.+),(.+),(.+))");
	
	while(std::getline(graphFile, line)) {
		std::smatch matches;
		
		// match edge data
		if(std::regex_search(line, matches, edgePat)) {
            QueryParser parser(line);
            if(!parser.haveErrors()) {
                queries.emplace_back(parser.triple());
            } else {
                std::cerr << "error parsing query\n" << std::endl;
            }
		}
	}
	
	graphFile.close();
	
	if(queries.size() == 0) std::cout << "Did not parse any queries... Check query file." << std::endl;
	
	return queries;
}

int estimatorBench(std::string &graphFile, std::string &queriesFile) {
	
	std::cout << "\n(1) Reading the graph into memory and preparing the estimator...\n" << std::endl;
	
	// read the graph
	auto g = std::make_shared<SimpleGraph>();
	
	auto start = std::chrono::steady_clock::now();
	try {
		g->readFromContiguousFile(graphFile);
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
		return 0;
	}
	
	auto end = std::chrono::steady_clock::now();
	std::cout << "Time to read the graph into memory: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << std::endl;
	
	// prepare the estimator
	auto est = std::make_unique<SimpleEstimator>(g);
	start = std::chrono::steady_clock::now();
	est->prepare();
	end = std::chrono::steady_clock::now();
	std::cout << "Time to prepare the estimator: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << std::endl;
	
	std::cout << "\n(2) Running the query workload..." << std::endl;
	
	auto queries = parseQueries(queriesFile);
	
	for(auto query : queries) {
		
		// perform estimation
		// parse the query into an AST
		std::cout << "\nProcessing query: " << query.toString();
		
		start = std::chrono::steady_clock::now();
		auto estimate = est->estimate(query);
		end = std::chrono::steady_clock::now();
		
		std::cout << "\nEstimation (noOut, noPaths, noIn) : ";
		estimate.print();
		std::cout << "Time to estimate: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << std::endl;
		
		// perform evaluation
		auto ev = std::make_unique<SimpleEvaluator>(g);
		ev->prepare();
		start = std::chrono::steady_clock::now();
		auto actual = ev->evaluate(query);
		end = std::chrono::steady_clock::now();
		
		std::cout << "Actual (noOut, noPaths, noIn) : ";
		actual.print();
		std::cout << "Time to evaluate: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << std::endl;
		
	}
	
	return 0;
}

struct benchresult_t evaluatorBench(std::string &graphFile, std::string &queriesFile) {
	struct benchresult_t result = {};

	std::cout << "\n(1) Reading the graph into memory and preparing the evaluator...\n" << std::endl;

	// read the graph
	auto g = std::make_shared<SimpleGraph>();

	auto start = std::chrono::steady_clock::now();
	try {
		g->readFromContiguousFile(graphFile);
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
		return {};
	}

	auto end = std::chrono::steady_clock::now();
	result.loadTime = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "Time to read the graph into memory: " << result.loadTime << " ms" << std::endl;

	// prepare the evaluator
	auto est = std::make_shared<SimpleEstimator>(g);
	auto ev = std::make_unique<SimpleEvaluator>(g);

	start = std::chrono::steady_clock::now();
	ev->attachEstimator(est);
	ev->prepare();
	end = std::chrono::steady_clock::now();
	result.prepTime = std::chrono::duration<double, std::milli>(end - start).count();
	std::cout << "Time to prepare the evaluator: " << result.prepTime << " ms" << std::endl;

	std::cout << "\n(2) Running the query workload..." << std::endl;

	auto queries = parseQueries(queriesFile);

	for(auto query : queries) {

		// perform evaluation
		// parse the query into an AST
		std::cout << "\nProcessing query: " << query.toString();

		// perform the evaluation
		start = std::chrono::steady_clock::now();
		auto actual = ev->evaluate(query);
		end = std::chrono::steady_clock::now();

		std::cout << "\nActual (noOut, noPaths, noIn) : ";
		actual.print();
		long localEvalTime = std::chrono::duration<double, std::milli>(end - start).count();
		std::cout << "Time to evaluate: " << localEvalTime << " ms" << std::endl;
		result.evalTime += localEvalTime;
	}

	return result;
}
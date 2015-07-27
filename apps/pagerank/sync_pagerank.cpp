/*
 *  PageRank tutorial application.
 *  pagerank.cpp
 *
 *  For description of the PageRank algorithm, see Wikipedia article
 *  http://en.wikipedia.org/wiki/Pagerank
 */

#include <string>
#include <stdlib.h>
#include <graphlab.hpp>
#include <graphlab/macros_def.hpp>
#include <time.h>

using namespace graphlab;
// Constants for the algorithm. Better way would be to
// pass them in the shared_data to the update function, but for
// the simplicity of this example, we simply define them here.

#define termination_bound 1e-3
#define damping_factor 0.85   // PageRank damping factor
#define init_value  0.15	// initial PR value of each vertex
/**
 * Edge data represents the weight as well as the weight times the
 * last value of the source vertex when the target value was computed.
 */

struct edge_data {
  float weight;
  //float old_source_value;
  edge_data(float weight) :
    weight(weight) { } 
    
}; // End of edge data


/**
 * Stores the value and the self weight
 */

struct vertex_data {
  float value;
 // float self_weight; // GraphLab does not support edges from vertex to itself, so
  				     // we save weight of vertex's self-edge in the vertex data
  vertex_data(float value = 1) : value(value) { }
}; // End of vertex data

//typedef edge_data float;
//typedef vertex_data float;

//! The type of graph used in this program
typedef graphlab::graph<vertex_data, edge_data> pagerank_graph;


/**
 * The collection of graphlab types restricted to the graph type used
 * in this program.
 */
typedef graphlab::types<pagerank_graph> gl_types;

void pagerank_update(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
                     gl_types::ishared_data* shared_data); 
/**
 * The Page rank init function
 */
void pagerank_init(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
					 gl_types::ishared_data* shared_data) {



	// Get the data associated with the vertex
	vertex_data& vdata = scope.vertex_data();
	vdata = init_value;
	const std::vector<graphlab::edge_id_t>& outedges = scope.out_edge_ids();
 	if(outedges.size() !=0){
		foreach(graphlab::edge_id_t eid, outedges){
			edge_data& edata = scope.edge_data(eid);
				edata.weight = 1.0/outedges.size();
		} 
	}
	/*
	gl_types::update_task task(scope.vertex(), pagerank_update);
	scheduler.add_task(task, 0);
	*/
} // end of pagerank init function




/**
 * The Page rank update function
 */
void pagerank_update(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
					 gl_types::ishared_data* shared_data) {

	// Get the data associated with the vertex
	vertex_data& vdata = scope.vertex_data();

	// Sum the incoming weights; start by adding the 
	// contribution from a self-link.
	float sum = 0;//vdata.value * vdata.self_weight;

	foreach(graphlab::edge_id_t eid, scope.in_edge_ids()) {
		// Get the neighobr vertex value
		/*
		   const vertex_data& neighbor_vdata =
		   scope.const_neighbor_vertex_data(scope.source(eid));
		   double neighbor_value = neighbor_vdata.value;
		   */
		// Get the edge data for the neighbor
		edge_data& edata = scope.edge_data(eid);
		// Compute the contribution of the neighbor
		float contribution = edata.weight ;//* neighbor_value;

		// Add the contribution to the sum
		sum += contribution;

		// Remember this value as last read from the neighbor
		//edata.old_source_value = neighbor_value;
	}

	// compute the jumpweight
	//sum = (1-damping_factor)/scope.num_vertices() + damping_factor*sum;
	sum = (1-damping_factor) + damping_factor*sum;
	//  vdata.value = sum;
	if(std::fabs(sum - vdata.value) > termination_bound)
	{
		vdata.value = sum;
		foreach(graphlab::edge_id_t eid, scope.out_edge_ids()) {
			edge_data& outedgedata = scope.edge_data(eid);
			outedgedata = sum / (scope.out_edge_ids()).size(); 
			// Compute edge-specific residual by comparing the new value of this
			// vertex to the previous value seen by the neighbor vertex.
			double residual = 0;
			// outedgedata.weight *
			// std::fabs(outedgedata.old_source_value - vdata.value);
			// If the neighbor changed sufficiently add to scheduler.
			//if(residual > termination_bound) {
			gl_types::update_task task(scope.target(eid), pagerank_update);
			scheduler.add_task(task, residual);
			//}
		}

	}
} // end of pagerank update function





// Creates simple 5x5 graph
void create_graph(pagerank_graph& graph) {
	// Create 5 vertices
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());

	
	// Page 0 links to page 3 only, so weight is 1
	graph.add_edge(0, 3, edge_data(1));
	
	// Page 1 links to 0 and 2
	graph.add_edge(1, 0, edge_data(0.5));
	graph.add_edge(1, 2, edge_data(0.5));
	
	// ... and so on
	graph.add_edge(2, 0, edge_data(1.0/3));
	graph.add_edge(2, 1, edge_data(1.0/3));
	graph.add_edge(2, 3, edge_data(1.0/3));
	graph.add_edge(3, 0, edge_data(0.25));
	graph.add_edge(3, 1, edge_data(0.25));
	graph.add_edge(3, 2, edge_data(0.25));
	graph.add_edge(3, 4, edge_data(0.25));
    graph.add_edge(4, 0, edge_data(0.2));
 	graph.add_edge(4, 1, edge_data(0.2));
	graph.add_edge(4, 2, edge_data(0.2));
	graph.add_edge(4, 3, edge_data(0.2));
	// and self edge which must be handled specially from 4 to 4
	//graph.vertex_data(4).self_weight = 0.2;

}

bool load_graph(const std::string filename,pagerank_graph& graph);

int main(int argc, char** argv) {
	global_logger().set_log_level(LOG_INFO);
	global_logger().set_log_to_console(true);
	logger(LOG_INFO, "PageRank starting\n");

	// Setup the parser
	graphlab::command_line_options
		clopts("Run the PageRank algorithm.");
	// set configuration
	// Configuration information
	std::string filename;
	std::string savefile ;
	std::string engine_type = "async";
	size_t ncpus = 6;
	gl_types::iengine* engine_ptr;
	// Create a graphlab core
	gl_types::core core;
	clopts.attach_option("infile", &filename,
			"PageRank input file. In src, dest format.");

	clopts.attach_option("outfile", &savefile,
			"PageRank output filename");
	//clopts.attach_option("engine", &engine_type,
	//		"execute pagerank  with synchronous or asynchronous engine");
	//clopts.attach_option("ncpus", &ncpus, ncpus, "number of cpu");
	// Parse the command line input
	if(!clopts.parse(argc, argv)) {
		std::cout << "Error in parsing input." << std::endl;
		return EXIT_FAILURE;
	}
	if(!clopts.is_set("infile")) {
		std::cout << "Input file no provided!" << std::endl;
		//clopts.print_description();
		//return EXIT_FAILURE;
		create_graph(core.graph());
	}else if(!load_graph(filename, core.graph())) {
		// Load the graph
		std::cout << "Error in parsing graph!" << std::endl;
		return EXIT_FAILURE;
	}

	

	// Set the engine options
	core.set_engine_options(clopts);
	engine_ptr = core.engine_ptr();	
	assert(typeid(*engine_ptr) == typeid(synchronous_engine<gl_types::graph>));
	((synchronous_engine<gl_types::graph>*)engine_ptr)->set_update_function(pagerank_init);
	timer t1;
	t1.start();
	engine_ptr->start();
	((synchronous_engine<gl_types::graph>*)engine_ptr)->set_update_function(pagerank_update);
	//engine_ptr->start();
	((synchronous_engine<gl_types::graph>*)engine_ptr)->start_with_iteration_limit(100);
	std::cout << "sync pagerank finished,engine runtime: " << t1.current_time() << " seconds." << std::endl;
	std::cout << "sync pagerank total updates count: " <<engine_ptr->last_update_count() << std::endl; 


	// And output 5 first vertices pagerank after dividing their value
	// with the norm.
	for(graphlab::vertex_id_t vid=0; vid<5 && vid<core.graph().num_vertices(); vid++) {
		std::cout << "vertex_id=" << vid << "\tpagerank = " <<
			core.graph().vertex_data(vid).value << std::endl;
	}
	
	if(!savefile.empty()){
		/*
		time_t curt = time (NULL);
		long seconds = (long)curt;
		char timestr[50];
		memset(timestr, 0, 50);
		sprintf(timestr, "%ld", seconds);
		//char* timestr = ctime(&curt);
		savefile = savefile.insert(savefile.length()-4, timestr);
		*/
		FILE* outf = fopen(savefile.c_str(), "w+");
		assert(outf != NULL);
		for(graphlab::vertex_id_t vid=0; vid<core.graph().num_vertices(); vid++){
			fprintf(outf, "%u\t%f\n", vid, core.graph().vertex_data(vid).value);
		}
		fclose(outf);
	}
	
	return EXIT_SUCCESS;
} // End of main


/**
 * Load a graph file specified in the format:
 *
 *   source_id, target_id, weight
 *   source_id, target_id, weight
 *   source_id, target_id, weight
 *               ....
 *
 * The file should not contain repeated edges.
 */
void fixline(char* p){
	int len =(int) strlen(p)-1;
	if(p[len] == '\n')
		p[len] = 0;
}
bool load_graph(const std::string filename,
		pagerank_graph& graph) {
	//assert(filename.substr(filename.length-3,3) != "txt");
	std::cout<<"input file :"<<filename<<std::endl;
	FILE* inf = fopen(filename.c_str(),"r");
	assert(inf != NULL);

	char s[1024];
	size_t edgecount=0;
	while(fgets(s, 1024, inf) != NULL) {
		if(s[0] == '#' || s[0] == '%')
			continue;
		fixline(s);
		char sep[] = "\t, ";
		char* t;
		t = strtok(s, sep);
		if(t == NULL){
			std::cout<<"input file format is incorrect"<<std::endl;
			assert(false);
		}

		size_t source = atoi(t);
		t = strtok(NULL, sep);
		size_t target = atoi(t);
		t = strtok(NULL, sep);
		float weight =	0;
		if(t != NULL) weight = atof(t);
		//s >> source >> target;
		//fin.ignore(1); // skip comma
		//s >> target;
		//fin.ignore(1); // skip comma
		//fin >> weight;
		//    fin.ignore(1); // skip comma
		// Ensure that the number of vertices is correct
		if(source >= graph.num_vertices() ||
				target >= graph.num_vertices())
			graph.resize(std::max(source, target) + 1);
		if(source != target) {
			// Add the edge
			edge_data edata(weight);
			graph.add_edge(source, target, weight);
		} else {
			// add the self edge by updating the vertex weight
			//graph.vertex_data(source).self_weight = weight;
			std::cout<<source<<"->"<<target<< "\tself edge is not allowed !"<<std::endl;

			assert(false);
		}      
		edgecount++;
		if(edgecount %5000000 == 0)
			std::cout<< edgecount <<" edges inserted"<<std::endl;
	}
	graph.finalize();
	fclose(inf);
	return true;
} // end of load graph




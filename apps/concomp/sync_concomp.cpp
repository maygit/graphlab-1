
#include <string>
#include <stdlib.h>
#include <graphlab.hpp>
#include <graphlab/macros_def.hpp>

using namespace graphlab;

#define termination_bound 1e-5
#define damping_factor 0.85   // concomp damping factor
#define init_value  0.15	// initial PR value of each vertex
/**
 * Edge data represents the weight as well as the weight times the
 * last value of the source vertex when the target value was computed.
 */
typedef uint32_t vid_t;

struct edge_data {
  vid_t weight;
  //float old_source_value;
  edge_data(vid_t weight) :
    weight(weight) { } 
    
}; // End of edge data


/**
 * Stores the value and the self weight
 */

struct vertex_data {
  vid_t value;
 // float self_weight; // GraphLab does not support edges from vertex to itself, so
  				     // we save weight of vertex's self-edge in the vertex data
  vertex_data(vid_t value = 1) : value(value) { }
}; // End of vertex data

//typedef edge_data float;
//typedef vertex_data float;

//! The type of graph used in this program
typedef graphlab::graph<vertex_data, edge_data> concomp_graph;


/**
 * The collection of graphlab types restricted to the graph type used
 * in this program.
 */
typedef graphlab::types<concomp_graph> gl_types;

void concomp_update(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
                     gl_types::ishared_data* shared_data); 
/**
 * The Page rank init function
 */
void concomp_init(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
					 gl_types::ishared_data* shared_data) {

	// Get the data associated with the vertex
	vertex_data& vdata = scope.vertex_data();
	vdata.value = scope.vertex();//initiate vertex data with its id;
	//vid_t tmp = vdata;
	vid_t curmin = vdata.value;//scope.vertex();
	//const std::vector<graphlab::edge_id_t>& outedges = scope.out_edge_ids();
	vid_t nlabel;	
	foreach(graphlab::edge_id_t eid, scope.out_edge_ids()){
		nlabel = scope.target(eid);
		curmin = std::min(curmin, nlabel);
	}
	foreach(graphlab::edge_id_t eid, scope.in_edge_ids()){
		nlabel = scope.source(eid);	
		curmin = std::min(curmin, nlabel);
	}
	vdata.value = curmin; 
	foreach(graphlab::edge_id_t eid, scope.out_edge_ids()){
		edge_data& edata = scope.edge_data(eid);
		edata.weight = curmin;
	}
		
	//std::cout<<"in wcc  init:::vertex_id="<<scope.vertex()<<"\t vertex_cid"<<scope.vertex_data().value<<std::endl;
	//gl_types::update_task task(scope.vertex(), concomp_update);
	//scheduler.add_task(task, 0);
	
} // end of concomp init function




/**
 * The concomp update function
 */
void concomp_update(gl_types::iscope &scope,
                     gl_types::icallback &scheduler,
					 gl_types::ishared_data* shared_data) {



	// Get the data associated with the vertex
	vertex_data& vdata = scope.vertex_data();
	vid_t curmin = vdata.value;
	//std::cout<<"in wcc  update:::vertex_id="<<scope.vertex()<<"\tvertex_cid="<<vdata.value<<std::endl;
	//logger(LOG_INFO, "in wcc update::vertex_id=%u", scope.vertex());
	//const std::vector<graphlab::edge_id_t>& outedges = scope.out_edge_ids();
	vid_t nlabel;	
	foreach(graphlab::edge_id_t eid, scope.in_edge_ids()){
		nlabel = scope.edge_data(eid).weight;	
		curmin = std::min(curmin, nlabel);
	}
	foreach(graphlab::edge_id_t eid, scope.out_edge_ids()){
		nlabel = scope.edge_data(eid).weight;
		curmin = std::min(curmin, nlabel);
	}
	/*
	   foreach(graphlab::edge_id_t eid, scope.in_edge_ids()){
	   nlabel = scope.edge_data(eid).weight;	
	   curmin = std::min(curmin, nlabel);
	   }
	   */
	//if(curmin < vdata.value){
	//scope.vertex_data() = curmin;	
	vdata.value = curmin;
	//}
	double residual = 100;		
	foreach(graphlab::edge_id_t eid, scope.out_edge_ids()){
		if(curmin < scope.edge_data(eid).weight){
			//std::cout<<"vertex id="<<scope.vertex()<<"\tcurmin="<<curmin<<"\t edge weight="<<scope.edge_data(eid).weight
			//	<<"\t add target="<<scope.target(eid)<<"to schedule"<<std::endl;
			edge_data& edata = scope.edge_data(eid);
			edata.weight = curmin;

			//scope.edge_data(eid).weight = curmin;
			gl_types::update_task task(scope.target(eid), concomp_update);
			scheduler.add_task(task, residual);	
		}
	}
	foreach(graphlab::edge_id_t eid, scope.in_edge_ids()){
		if(curmin < scope.edge_data(eid).weight){
			//std::cout<<"curmin="<<curmin<<"\t edge weight="<<scope.edge_data(eid).weight<<"\t add source to schedule"<<scope.source(eid)<<std::endl;
			edge_data& edata = scope.edge_data(eid);
			edata.weight = curmin;

			//scope.edge_data(eid).weight = curmin;
			gl_types::update_task task(scope.source(eid), concomp_update);
			scheduler.add_task(task, residual);
		}	
	}
	//logger(LOG_INFO, "logger end of update vertex_id=%u",scope.vertex());
	//std::cout<<"in wcc  end of update:::vertex_id="<<scope.vertex()<<"\tvertex_cid="<<scope.vertex_data().value<<std::endl;
} // end of concomp update function





// Creates simple 5x5 graph
void create_graph(concomp_graph& graph) {
	// Create 5 vertices
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	graph.add_vertex(vertex_data());
	
	graph.add_edge(0, 1, edge_data((vid_t)-1));
	graph.add_edge(1, 2, edge_data((vid_t)-1));
	graph.add_edge(2, 3, edge_data((vid_t)-1));
	graph.add_edge(3, 4, edge_data((vid_t)-1));
	/*	
	// Page 0 links to page 3 only, so weight is 1
	graph.add_edge(0, 3, edge_data(1));
	
	// Page 1 links to 0 and 2
	graph.add_edge(1, 0, edge_data((vid_t)-1));
	graph.add_edge(1, 2, edge_data((vid_t)-1));
	
	// ... and so on
	graph.add_edge(2, 0, edge_data((vid_t)-1));
	graph.add_edge(2, 1, edge_data((vid_t)-1));
	graph.add_edge(2, 3, edge_data((vid_t)-1));
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
	*/
}

bool load_graph(const std::string filename,concomp_graph& graph);

int main(int argc, char** argv) {
	global_logger().set_log_level(LOG_INFO);
	global_logger().set_log_to_console(true);
	logger(LOG_INFO, "concomp starting\n");

	// Setup the parser
	graphlab::command_line_options
		clopts("Run the concomp algorithm.");
	// set configuration
	// Configuration information
	std::string filename;
	std::string savefile ;
	//std::string engine_type = "sync";
	size_t ncpus = 8;
	// Create a graphlab core
	gl_types::core core;
	clopts.attach_option("infile", &filename,
			"concomp input file. In src, dest format.");

	clopts.attach_option("outfile", &savefile,
			"concomp output filename");
	//clopts.attach_option("engine", &engine_type,
	//		"execute concomp  with synchronous or asynchronous engine");
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
		std::cout << "Error in parsing graph!" << std::endl;
		return EXIT_FAILURE;
	}

	

	// Set the engine options
	core.set_engine_options(clopts);

	// Create a synthetic graph
	//  create_graph(core.graph());

	// Schedule all vertices to run concomp update on the
	// first round.
	//core.add_task_to_all(concomp_init, 0);
	//std::cout<<"init finished, start update!================================="<<std::endl;
	//core.add_task_to_all(concomp_update, 0);
	std::cout<<"options passed!"<<std::endl;
	//assert(false);
	// Run the engine
	//gl_types::iengine engine_ref = core.engine();
	//engine_ref = core.engine();
	//double runtime = core.start();
	gl_types::iengine* engine_ptr = core.engine_ptr();

	assert(typeid(*engine_ptr) == typeid(synchronous_engine<gl_types::graph>));
	((synchronous_engine<gl_types::graph>*)engine_ptr)->set_update_function(concomp_init);
	//start time
	timer t1;
	t1.start();
	//((synchronous_engine<gl_types::graph>*)engine_ptr)->start_with_iteration_limit(10);
	engine_ptr->start();	
	((synchronous_engine<gl_types::graph>*)engine_ptr)->set_update_function(concomp_update);
	//engine_ptr->start();	
	((synchronous_engine<gl_types::graph>*)engine_ptr)->start_with_iteration_limit(100);
	// We are done, now output results.
	std::cout << "sync wcc finished, runtime: " << t1.current_time() << " seconds." << std::endl;
	std::cout<<"sync wcc total updates count:"<<engine_ptr->last_update_count()<<std::endl;
	// And output 5 first vertices concomp after dividing their value
	// with the norm.
	for(graphlab::vertex_id_t vid=0; vid<5 && vid<core.graph().num_vertices(); vid++) {
		std::cout << "vertex_id=" << vid << " concomp_id =" <<
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
			fprintf(outf, "%u\t%u\n", vid, core.graph().vertex_data(vid).value);
		}
		fclose(outf);
	}
		
	return EXIT_SUCCESS;
} // End of main


  // Configuration information
// std::string filename;

//   clopts.attach_option("infile", &filename,
//                        "concomp input file. In src, dest, weight format.");
// 
//   // Parse the command line input
//   if(!clopts.parse(argc, argv)) {
//     std::cout << "Error in parsing input." << std::endl;
//     return EXIT_FAILURE;
//   }
//   if(!clopts.is_set("infile")) {
//     std::cout << "Input file no provided!" << std::endl;
//     clopts.print_description();
//     return EXIT_FAILURE;
//   }

// Load the graph
//   if(!load_graph(filename, core.graph())) {
//     std::cout << "Error in parsing graph!" << std::endl;
//     return EXIT_FAILURE;
//   }



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
		concomp_graph& graph) {
	//assert(filename.substr(filename.length-3,3) != "txt");
	std::cout<<"input file :"<<filename<<std::endl;
	FILE* inf = fopen(filename.c_str(),"r");
	assert(inf != NULL);

	// std::ifstream fin(filename.c_str());
	//if(!fin.good()) return false;
	//	std::cout<<"fin is  good"<<std::endl;
	// Loop through file reading each line
	char s[1024];
	size_t edgecount=0;
	while(fgets(s, 1024, inf) != NULL) {
		/*
		   std::string line;
		   std::getline(fin, line);
		   if(line[0] == '#'){
		   std::cout<<line<<std::endl;
		   continue;
		   }
		   std::stringstream s(line);
		   */
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
		size_t weight =	(vid_t)-1;
		if(t != NULL) weight = atoi(t);
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
			graph.add_edge(source, target, edata);
		} else {
			// add the self edge by updating the vertex weight
			//graph.vertex_data(source).self_weight = weight;
			//std::cout<<source<<"->"<<target<< "\tself edge is not allowed !"<<std::endl;
			//assert(false);
			//std::cout<<source<<"->"<<target<<"\t self edge exists! ignored"<<std::endl;
		}      
		edgecount++;
		if(edgecount %5000000 == 0)
			std::cout<< edgecount <<" edges inserted"<<std::endl;
	}
	graph.finalize();
	fclose(inf);
	return true;
} // end of load graph




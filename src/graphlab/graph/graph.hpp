/**
 * \file
 *
 * This file contains the template for the graphlab graph
 * data-structure.
 *
 */

#ifndef GRAPHLAB_BASICGRAPH_HPP
#define GRAPHLAB_BASICGRAPH_HPP

#include <cassert>
#include <cmath>

#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>

#include <queue>
#include <algorithm>
#include <functional>
#include <fstream>
#include <atomic>

#include <boost/bind.hpp>



#include <graphlab/logger/logger.hpp>

#include <graphlab/serialization/iarchive.hpp>
#include <graphlab/serialization/oarchive.hpp>

#include <graphlab/extern/metis/metis.hpp>




#include <graphlab/macros_def.hpp>
namespace graphlab { 


  /**
   * \brief the partitioning methods.
   * 
   * The partition method struct contains the partition_method_enum
   * which desribes the partitioning method supported by the graphlab
   * graph partitioning framework.  Graphlab links to the metis graph
   * partitioning library and also provides its own partitioning
   * algorithms.  
   */
  struct partition_method {

    /**
     * \brief the partition methods
     *
     * <ul>
     *
     *   <li> PARTITION_RANDOM: Randomly partition the graph. </li>
     *
     *   <li> PARTITION_METIS: Partition the graph using the metis
     *   algorithm </li>
     *
     *   <li> PARTITION_BFS: Use BFS partition method. </li>
     *
     * </ul>
     */
    enum partition_method_enum {
      PARTITION_RANDOM,
      PARTITION_METIS,
      PARTITION_BFS,
      PARTITION_EDGE_NUM,
    };
  };

  /** The type of a vertex is a simple size_t */
  typedef uint32_t vertex_id_t;

  /** The type of an edge id */
  typedef uint32_t edge_id_t;

  /** Type for vertex colors **/
  typedef uint8_t vertex_color_type;

  
  template<typename VertexData, typename EdgeData> class graph;


  // CLASS GRAPH ==============================================================>
  /**
   * The graph data structure which behaves like a container of edge
   * and vertex data. Intuitively the graph behaves like an
   * std::vector over VertexData with data associated with select
   * pairs of indices corresponding to edge data.
   */
  template<typename VertexData, typename EdgeData>
  class graph {
  public:

    /** The type of the vertex data stored in the graph */
    typedef VertexData vertex_data_type;

    /** The type of the edge data stored in the graph */
    typedef EdgeData   edge_data_type;
      
    
  public:

    // CONSTRUCTORS ============================================================>
    /**
     * Build a basic graph
     */
    graph() : finalized(true) {  }

    /**
     * BUG: Should not reserve but instead directly create vertices.
     * Create a graph with space allocated for num_vertices and
     * num_edges.
     */
    graph(size_t nverts) : 
      vertices(nverts),
      in_edges(nverts), out_edges(nverts), vcolors(nverts),
      finalized(true) { }

    graph(const graph<VertexData, EdgeData> &g) { (*this) = g; }

    // METHODS =================================================================>

    /**
     * Clear all internal data
     */
    void clear() {
      vertices.clear();
      edges.clear();
      in_edges.clear();
      out_edges.clear();
      vcolors.clear();
      finalized = true;
    }
    
    /**
     * Finalize a graph by sorting its edges to maximize the
     * efficiency of graphlab.  This is invoked by the engine at
     * start.
     */
    void finalize() {
		std::cout<<"finalize graph (sort)"<<std::endl;   
      // check to see if the graph is already finalized
      if(finalized) return;
      // Assert that the graph is not finalized 
      typedef std::vector< edge_id_t > edge_set;

      edge_id_less_functor less_functor(this);
      
      // Sort all in edges sets
      foreach(edge_set& eset, in_edges) {
        std::sort(eset.begin(),
                  eset.end(),
                  less_functor);
        //                  boost::bind(&graph::edge_id_less, this, _1, _2) );
        // Test for duplicate edges
        if (eset.size() > 1) {
          for(size_t i = 0; i < eset.size()-1; ++i) {
            // Duplicate edge test
            assert(edge_id_less(eset[i], eset[i+1]));
          }
        }        
      }
      // Sort all out edges sets
      foreach(edge_set& eset, out_edges) {
        std::sort(eset.begin(),
                  eset.end(),
                  less_functor);
        //                 boost::bind(&graph::edge_id_less, this, _1, _2) );
        // Test for dupliate edges
        if (eset.size() > 1) {
          for(size_t i = 0; i < eset.size()-1; ++i) {
            // Duplicate edge test
            assert(edge_id_less(eset[i], eset[i+1]));
          }
        }
      }
      finalized = true;
    } // End of finalize
            
    /** Get the number of vetices */
    size_t num_vertices() const {
      return vertices.size();
    } // end of num vertices

    /** Get the number of edges */
    size_t num_edges() const {
      return edges.size();
    } // end of num edges


    /** Get the number of in edges */
    size_t num_in_neighbors(vertex_id_t v) const {
      assert(v < vertices.size());
      return in_edges[v].size();
    } // end of num vertices
    
    /** get the number of out edges */
    size_t num_out_neighbors(vertex_id_t v) const  {
      assert(v < vertices.size());
      return out_edges[v].size();
    } // end of num vertices

    /** Find an edge */
    std::pair<bool, edge_id_t>
    find(vertex_id_t source, vertex_id_t target) const {
      assert(source < in_edges.size());
      assert(target < out_edges.size());
      // Check the base case that the souce or target have no edges
      if (in_edges[target].size() == 0 ||
          out_edges[source].size() == 0) {
        return std::make_pair(false,-1);
      } else if(finalized) { // O( log degree ) search ========================>
        // if it is finalized then do the search using a binary search
        // If their are fewer in edges into the target search the in
        // edges
        if(in_edges[target].size() < out_edges[source].size()) {
          // search the source vertices for the edge
          size_t index = binary_search(in_edges[target], source, target);
          if(index < in_edges[target].size())
            return std::make_pair(true, in_edges[target][index]);
          else
            return std::make_pair(false,-1);
        } else { // If their are fewer edges out of the source binary
                 // search there
          // search the source vertices for the edge
          size_t index = binary_search(out_edges[source], source, target);
          if(index < out_edges[source].size())
            return std::make_pair(true, out_edges[source][index]);
          else
            return std::make_pair(false,-1);
        }
      } else { // O( degree ) search ==========================================>
        // if there are few in edges at the target search there
        if(in_edges[target].size() < out_edges[source].size()) {
          // linear search the in_edges at the target 
          foreach(edge_id_t eid, in_edges[target]) {
            assert(eid < edges.size());
            if(edges[eid].source() == source 
               && edges[eid].target() == target) {
              return std::make_pair(true, eid);
            }
          }
          return std::make_pair(false, -1);
        } else { // fewer out edges at the source
          // linear search the out_edges at the source
          foreach(edge_id_t eid, out_edges[source]) {
            assert(eid < edges.size());
            if(edges[eid].source() == source 
               && edges[eid].target() == target) {
              return std::make_pair(true, eid);
            }
          }
          return std::make_pair(false, -1);
        }
      } // End of else 
    } // end of find

    
    /** get the edge id for the edge */
    edge_id_t edge_id(vertex_id_t source, vertex_id_t target) const {
      std::pair<bool, edge_id_t> res = find(source, target);
      // The edge must exist
      assert(res.first);
      assert(res.second < edges.size());
      return res.second;
    } // end of edge_id

    
    /** get the reverser edge id for the edge */
    edge_id_t rev_edge_id(edge_id_t eid) const {
      assert(eid < edges.size());
      vertex_id_t source = edges[eid].source();
      vertex_id_t target = edges[eid].target();    
      return edge_id(target, source);
    } // end of rev_edge_id

    
    /** 
     * Creates a vertex containing the vertex data and returns the id
     * of the new vertex id.
     */
    vertex_id_t add_vertex(const VertexData& vdata = VertexData() ) {
      vertices.push_back(vdata);
      // Resize edge maps
      out_edges.resize(vertices.size());
      in_edges.resize(vertices.size());
      vcolors.resize(vertices.size());
      return vertices.size() - 1;
    } // End of add vertex;


    /** 
     * Add additional vertices up to provided num_vertices.  This will
     * fail is resizing down.
     */
    void resize(size_t num_vertices ) {
      assert(num_vertices >= vertices.size());
      vertices.resize(num_vertices);
      // Resize edge maps
      out_edges.resize(vertices.size());
      in_edges.resize(vertices.size());
      vcolors.resize(vertices.size());
    } // End of resize
    
    
    /**
     * Creates an edge connecting vertex source to vertex target.  Any
     * existing data will be cleared.
     */
    edge_id_t add_edge(vertex_id_t source, vertex_id_t target, 
                       const EdgeData& edata = EdgeData()) {
      if ( source >= vertices.size() 
           || target >= vertices.size() ) {
        logger(LOG_FATAL, 
               "Attempting add_edge (%d -> %d) when there are only %d vertices",
               source, target, vertices.size());
        assert(source < vertices.size());
        assert(target < vertices.size());
      }
      // Add the edge to the set of edge data (this copies the edata)
      edges.push_back( edge( source, target, edata ) );

      // Add the edge id to in and out edge maps
      edge_id_t edge_id = edges.size() - 1;
      in_edges[target].push_back(edge_id);
      out_edges[source].push_back(edge_id);

      // Determine if the graph is still finalized A graph is
      // finalized if it was finalized and the newly added edge_id is
      // in the correct location in the in and out edge lists (which
      // is true if either the lists only contain a single element or
      // the last two elements are in the correct order).
      finalized = finalized &&
        ((in_edges[target].size() < 2) ||
         edge_id_less(*(in_edges[target].end()-2),
                      *(in_edges[target].end()-1))) &&
        ((out_edges[source].size() < 2) ||
         edge_id_less(*(out_edges[source].end()-2),
                      *(out_edges[source].end()-1)));
      return edge_id;
    } // End of add edge
        
    
    /** Get the vertex data */
    VertexData& vertex_data(vertex_id_t v) {
      assert(v < vertices.size());
      return vertices[v];
    } // end of data(v)
    
    /** Get the vertex data */
    const VertexData& vertex_data(vertex_id_t v) const {
      assert(v < vertices.size());
      return vertices[v];
    } // end of data(v)

    /** Get the edge_data */
    EdgeData& edge_data(vertex_id_t source, vertex_id_t target) {
      assert(source < vertices.size());
      assert(target < vertices.size());
      std::pair<bool, edge_id_t> ans = find(source, target);
      // We must find the edge!
      assert(ans.first);
      // the edge id should be valid!
      assert(ans.second < edges.size());
      return edges[ans.second].data();
    } // end of edge_data(u,v)
    
    /** Get the edge_data */
    const EdgeData& edge_data(vertex_id_t source, vertex_id_t target) const {
      assert(source < vertices.size());
      assert(target < vertices.size());
      std::pair<bool, edge_id_t> ans = find(source, target);
      // We must find the edge!
      assert(ans.first);
      // the edge id should be valid!
      assert(ans.second < edges.size());
      return edges[ans.second].data();
    } // end of edge_data(u,v)

    /** Get the edge_data */
    EdgeData& edge_data(edge_id_t edge_id) { 
      assert(edge_id < edges.size());
      return edges[edge_id].data();
    }
    
    /** Get the edge_data */
    const EdgeData& edge_data(edge_id_t edge_id) const {
      assert(edge_id < edges.size());
      return edges[edge_id].data();
    }
	EdgeData get_edge_data(edge_id_t edge_id){
		assert(edge_id < edges.size());
		return edges[edge_id].get_data();
	}
	void set_edge_data(edge_id_t edge_id , EdgeData edata){
		assert(edge_id < edges.size());
		edges[edge_id].set_data(edata);
	}
    /** get the source of the edge */
    vertex_id_t source(edge_id_t edge_id) const {
      //      assert(edge_id < edges.size());
      return edges[edge_id].source();
    }

    /** get the dest of the edge */
    vertex_id_t target(edge_id_t edge_id) const {
      //      assert(edge_id < edges.size());
      return edges[edge_id].target();    
    }
    
    /** Set vertex color **/
    const vertex_color_type& color(vertex_id_t vertex) const {
      assert(vertex < vertices.size());
      return vcolors[vertex];
    }

    vertex_color_type& color(vertex_id_t vertex) {
      assert(vertex < vertices.size());
      return vcolors[vertex];
    }


    /** This function constructs a coloring for the graph and returns
        the number of colors */
    size_t compute_coloring() {
      // Reset the colors
      for(vertex_id_t v = 0; v < num_vertices(); ++v) color(v) = 0;
      // construct a permuation of the vertices to use in the greedy
      // coloring. \todo Should probably sort by degree instead when
      // constructing greedy coloring.
      std::vector<vertex_id_t> permutation(num_vertices());
      for(vertex_id_t v = 0; v < num_vertices(); ++v) 
        permutation[v] = v;
      std::random_shuffle(permutation.begin(), permutation.end());
      // Recolor
      size_t max_color = 0;
      std::set<vertex_color_type> neighbor_colors;
      for(size_t i = 0; i < permutation.size(); ++i) {
        neighbor_colors.clear();
        const vertex_id_t& vid = permutation[i];
        const std::vector<edge_id_t>& in_edges = in_edge_ids(vid);
        // Get the neighbor colors
        foreach(edge_id_t eid, in_edges){
          const vertex_id_t& neighbor_vid = source(eid);
          const vertex_color_type& neighbor_color = color(neighbor_vid);
          neighbor_colors.insert(neighbor_color);
        }
        // Find the lowest free color
        vertex_color_type& vertex_color = color(vid);
        foreach(vertex_color_type neighbor_color, neighbor_colors) {
          if(vertex_color != neighbor_color) break;
          else vertex_color++;
          // Ensure no wrap around
          assert(vertex_color != 0);                
        }
        max_color = std::max(max_color, size_t(vertex_color) );
      }
      // Return the NUMBER of colors
      return max_color + 1;           
    } // end of compute coloring


    /**
     * Check that the colors satisfy a valid coloring of the graph.
     * return true is coloring is valid;
     */
    bool valid_coloring() {
      for(vertex_id_t vid = 0; vid < num_vertices(); ++vid) {
        const vertex_color_type& vertex_color = color(vid);
        const std::vector<edge_id_t>& in_edges = in_edge_ids(vid);
        // Get the neighbor colors
        foreach(edge_id_t eid, in_edges){
          const vertex_id_t& neighbor_vid = source(eid);
          const vertex_color_type& neighbor_color = color(neighbor_vid);
          if(vertex_color == neighbor_color) return false;
        }
      }
      return true;
    }
    
    
    /** Get the ids of the in edges */
    const std::vector<edge_id_t>& in_edge_ids(vertex_id_t v) const {
      assert(v < in_edges.size());
      return in_edges[v];
    } // end of in edges    

    /** Get the ids of the out edges */
    const std::vector<edge_id_t>& out_edge_ids(vertex_id_t v) const {
      assert(v < out_edges.size());
      return out_edges[v];
    } // end of out edges


    /** Load the graph from an archive */
    void load(iarchive& arc) {
      clear();    
      // read the vertices and colors
      arc >> vertices
          >> edges
          >> in_edges
          >> out_edges
          >> vcolors
          >> finalized;
    } // end of load

    /** Save the graph to an archive */
    void save(oarchive& arc) const {
      // Write the number of edges and vertices
      arc << vertices
          << edges
          << in_edges
          << out_edges
          << vcolors
          << finalized;
    } // end of save
    

    /** Load the graph from a file */
    void load(const std::string& filename) {
      std::ifstream fin(filename.c_str());
      iarchive iarc(fin);
      iarc >> *this;
      fin.close();
    } // end of load


    /**
     * \brief save the graph to the file given by the filename
     * 
     */    
    void save(const std::string& filename) const {
      std::ofstream fout(filename.c_str());
      oarchive oarc(fout);
      oarc << *this;
      fout.close();
    } // end of save

    /**
     * \brief save the adjacency structure to a text file.
     *
     * Save the adjacency structure as a text file in:
     *    src_Id, dest_Id \n
     *    src_Id, dest_Id \n
     * format.
     */
    void save_adjacency(const std::string& filename) const {
      std::ofstream fout(filename.c_str());
      assert(fout.good());
      for(size_t i = 0; i < edges.size(); ++i) {
        fout << edges[i].source() << ", " << edges[i].target() << "\n";
        assert(fout.good());
      }          
      fout.close();
    }

    /**
     * partition the graph with roughtly the same number of edges for each part
     */
    void edge_num_partition(size_t nparts, std::vector<uint32_t>& vertex2part){
      
      vertex2part.resize(num_vertices());
      size_t e = 2 * num_edges();
      size_t edge_per_part = e / nparts;

      size_t curpart = 0;
      std::vector<size_t> parts;
      parts.resize(nparts, 0);
      
      for (size_t i = 0; i< num_vertices(); i++){
        int ne = out_edge_ids(i).size() + in_edge_ids(i).size();
        vertex2part[i] = curpart;
        parts[curpart]+= ne;
           
        if (parts[curpart] >= edge_per_part  && curpart < nparts-1){
          curpart++;
        }
      }
    }


    /**
     * Randomly assign vertices to partitions.  This will assign
     * vertices evenly to each partition.
     */
    void random_partition(size_t nparts, std::vector<uint32_t>& vertex2part) {
      vertex2part.resize(num_vertices());
      for (size_t i = 0;i < num_vertices(); ++i) {
        vertex2part[i] = i % nparts;
      }
      std::random_shuffle(vertex2part.begin(), vertex2part.end());
    }


    /**
     * Use a modified version of the METIS library to partition the
     * graph. The methis library is described in:
     *
     *   "A Fast and Highly Quality Multilevel Scheme for Partitioning
     *   Irregular Graphs”. George Karypis and Vipin Kumar. SIAM
     *   Journal on Scientific Computing, Vol. 20, No. 1, pp. 359—392,
     *   1999.
     *
     * We have modified an alpha version (5.0) to work with the
     * GraphLab framework.  Therefore users having trouble with this
     * function or the included Metis source should direct concerns to
     * the contact information provided at:
     *
     *   http://www.select.cs.cmu.edu/code
     *
     */
    void metis_partition(size_t numparts , std::vector<uint32_t>& ret_part) {
      if (numparts == 1) {
        ret_part.assign(num_vertices(), 0);
        return;
      }
      // Determine parameters needed to construct the partitioning
      metis::idxtype numverts = static_cast<int>(num_vertices());
      assert(numverts > 0);
      // Compute the number of edges 
      int numedges = static_cast<int>(num_edges());

      // allocate metis data structures
      metis::idxtype* vweight = new metis::idxtype[numverts];
      assert(vweight != NULL);    
      metis::idxtype* xadj = new metis::idxtype[numverts + 1];
      assert(xadj != NULL);
      metis::idxtype* adjacency = new metis::idxtype[2 * numedges];
      assert(adjacency != NULL);
      metis::idxtype* eweight = NULL;
      //       if(weighted) {
      //         eweight = new idxtype[numedges];
      //         assert(eweigth != NULL);
      //       }
      metis::idxtype* res = new metis::idxtype[numverts];   
      assert(res != NULL);

      // Pass through vertices filling in the metis data structures
      size_t offset = 0;
      for(vertex_id_t u = 0; u < num_vertices(); ++u) {
        // Update vertex weight
        // Set weight
        vweight[u] = 1;
        // Update the offset
        xadj[u] = offset;
        // Fill the the adjacency data
      
        std::set<size_t> neighbors;
        foreach(edge_id_t eid, out_edge_ids(u)) {
          neighbors.insert(target(eid));
        }
        foreach(edge_id_t eid, in_edge_ids(u)) {
          neighbors.insert(source(eid));
        }
        foreach(vertex_id_t vid, neighbors) {
          if (vid == u) continue;
          adjacency[offset] = vid;
          assert(adjacency[offset] >= 0);
          offset++;
          assert(offset >= 0);
        }
    
      } // end of data structure creation
      
      // Set the last entry in xadj to the end of the adjacency array
      xadj[numverts] = offset;
    
      // Set additional metis flags
      /**
       * 0 No weights (vwgts and adjwgt are NULL) 
       * 1 Weights on the edges only (vwgts = NULL) 
       * 2 Weights on the vertices only (adjwgt = NULL) 
       * 3 Weights both on vertices and edges. 
       */
      metis::idxtype weightflag = 2;
      // 0 for C-style numbering starting at 0 (1 for fortran style)
      metis::idxtype numflag = 0;
      // the number of parts to cut into
      metis::idxtype nparts = numparts;     
      // Options array (only care about first element if first element
      // is zero
      metis::idxtype options[5] = {0}; 
      // output argument number of edges cut
      metis::idxtype edgecut = 0;
    
      // Call kmetis
      metis::METIS_PartGraphKway(&(numverts), 
                                 xadj,
                                 adjacency,
                                 vweight,
                                 eweight,
                                 &(weightflag),
                                 &(numflag),
                                 &(nparts),
                                 options,
                                 &(edgecut),
                                 res);
    
      //     // Call pmetis
      //     metis::METIS_PartGraphRecursive(&(numverts), 
      //                                     xadj,
      //                                     adjacency,
      //                                     vweight,
      //                                     eweight,
      //                                     &(weightflag),
      //                                     &(numflag),
      //                                     &(nparts),
      //                                     options,
      //                                     &(edgecut),
      //                                     res);
    
      // destroy all unecessary data structures except res
      if(xadj != NULL) delete [] xadj;
      if(adjacency != NULL) delete [] adjacency;
      if(vweight != NULL) delete [] vweight;
      if(eweight != NULL) delete [] eweight;

      // Resize the partition
      ret_part.resize(num_vertices());
      // process the final results
      assert(res != NULL);
      for(vertex_id_t v = 0; v < num_vertices(); ++v) {
        ret_part[v] = res[v];
      }    
      // Delete the result array
      if(res != NULL) delete [] res;
    } // end of metis partition


    /**
     * This function computes a weighted graph partition using METIS.
     * \param numparts The number of parts to partition into
     * \param ret_part A vector providing a vertex_id -> partition_id mapping
     * \param vfunction A function of the type size_t (*)(const VertexData &v)
     *                  This function takes in the data on a vertex, and 
     *                  returns the weight of the vertex
     * \param efunction A function of the type size_t (*)(const EdgeData &e)
     *                  This function takes in the data on an edge, and 
     *                  returns the weight of the edge
     *
     * Use a modified version of the METIS library to partition the
     * graph using user provided edge and vertex weight functions.
     * The methis library is described in:
     *
     *   "A Fast and Highly Quality Multilevel Scheme for Partitioning
     *   Irregular Graphs”. George Karypis and Vipin Kumar. SIAM
     *   Journal on Scientific Computing, Vol. 20, No. 1, pp. 359—392,
     *   1999.
     *
     * We have modified an alpha version (5.0) to work with the
     * GraphLab framework.  Therefore users having trouble with this
     * function or the included Metis source should direct concerns to
     * the contact information provided at:
     *
     *   http://www.select.cs.cmu.edu/code
     *
     */
    template <typename EdgeWeightFunction, typename VertexWeightFunction>
    void metis_weighted_partition(size_t numparts ,
                                  std::vector<uint32_t>& ret_part,
                                  VertexWeightFunction vfunction,
                                  EdgeWeightFunction wfunction) {
      if (numparts == 1) {
        ret_part.assign(num_vertices(), 0);
        return;
      }
      // Determine parameters needed to construct the partitioning
      metis::idxtype numverts = static_cast<int>(num_vertices());
      assert(numverts > 0);
      // Compute the number of edges 
      int numedges = static_cast<int>(num_edges());

      // allocate metis data structures
      metis::idxtype* vweight = new metis::idxtype[numverts];
      assert(vweight != NULL);
      metis::idxtype* xadj = new metis::idxtype[numverts + 1];
      assert(xadj != NULL);
      metis::idxtype* adjacency = new metis::idxtype[2 * numedges];
      assert(adjacency != NULL);
      metis::idxtype* eweight = NULL;
      eweight = new metis::idxtype[ 2 * numedges];
      assert(eweight != NULL);
      
      metis::idxtype* res = new metis::idxtype[numverts];   
      assert(res != NULL);

      // Pass through vertices filling in the metis data structures
      size_t offset = 0;
      for(vertex_id_t u = 0; u < num_vertices(); ++u) {
        // Update vertex weight
        // Set weight
        vweight[u] = double(vfunction(vertex_data(u)));
        // Update the offset
        xadj[u] = offset;
        // Fill the the adjacency data
      
        std::set<size_t> neighbors;
        std::map<size_t, double> nbrtoweight;
        foreach(edge_id_t eid, out_edge_ids(u)) {
          neighbors.insert(target(eid));
          nbrtoweight[target(eid)] = double(wfunction(edge_data(eid)));
        }
        foreach(edge_id_t eid, in_edge_ids(u)) {
          neighbors.insert(source(eid));
          nbrtoweight[source(eid)] = double(wfunction(edge_data(eid)));
        }
        foreach(vertex_id_t vid, neighbors) {
          if (vid == u) continue;
          adjacency[offset] = vid;
          eweight[offset] = nbrtoweight[vid];
          assert(adjacency[offset] >= 0);
          offset++;
          assert(offset >= 0);
        }
    
      } // end of data structure creation
      
      // Set the last entry in xadj to the end of the adjacency array
      xadj[numverts] = offset;
    
      // Set additional metis flags
      /**
       * 0 No weights (vwgts and adjwgt are NULL) 
       * 1 Weights on the edges only (vwgts = NULL) 
       * 2 Weights on the vertices only (adjwgt = NULL) 
       * 3 Weights both on vertices and edges. 
       */
      metis::idxtype weightflag = 3;
      // 0 for C-style numbering starting at 0 (1 for fortran style)
      metis::idxtype numflag = 0;
      // the number of parts to cut into
      metis::idxtype nparts = numparts;     
      // Options array (only care about first element if first element
      // is zero
      metis::idxtype options[5] = {0};
      
      options[0] = 1;
      options[1]=3;
      options[2]=1;
      options[3]=2;
      options[4]=0;
      // output argument number of edges cut
      metis::idxtype edgecut = 0;
    
      // Call kmetis
      metis::METIS_PartGraphKway(&(numverts), 
                                 xadj,
                                 adjacency,
                                 vweight,
                                 eweight,
                                 &(weightflag),
                                 &(numflag),
                                 &(nparts),
                                 options,
                                 &(edgecut),
                                 res);
    
      // Call pmetis
      /*   metis::METIS_PartGraphRecursive(&(numverts), 
           xadj,
           adjacency,
           vweight,
           eweight,
           &(weightflag),
           &(numflag),
           &(nparts),
           options,
           &(edgecut),
           res);*/
    
      // destroy all unecessary data structures except res
      if(xadj != NULL) delete [] xadj;
      if(adjacency != NULL) delete [] adjacency;
      if(vweight != NULL) delete [] vweight;
      if(eweight != NULL) delete [] eweight;

      // Resize the partition
      ret_part.resize(num_vertices());
      // process the final results
      assert(res != NULL);
      for(vertex_id_t v = 0; v < num_vertices(); ++v) {
        ret_part[v] = res[v];
      }    
      // Delete the result array
      if(res != NULL) delete [] res;
    } // end of metis partition
    
    
    void bfs_partition(size_t nparts, std::vector<uint32_t> &vertex2part) {
      // create a list of all unassigned variables
      std::set<vertex_id_t> unassigned;
      vertex2part.resize(num_vertices());
      // initialize the unassigned vertices
      for(vertex_id_t v = 0; v < num_vertices(); ++v) {
        unassigned.insert(v);
      }
      // Compute the partition size
      size_t maxpartsize = (size_t)(std::ceil(double(unassigned.size()) / nparts));
      size_t partid = 0;
      while(!unassigned.empty()) {  
        std::list<vertex_id_t> queue;    // Breadth first queue 
        std::set<vertex_id_t>  visited;  // Set of visited vertices
        // While the task is still too small and their remains
        // unassigned vertices
        size_t curpartsize = 0;
        while(curpartsize < maxpartsize 
              && !unassigned.empty()) {
          if(queue.empty()) { 
            queue.push_front(*unassigned.begin());
            visited.insert(*unassigned.begin());
          }
          assert(!queue.empty());
          // Pop the first element off the queue 
          vertex_id_t v = queue.front(); queue.pop_front();
          assert(partid < nparts);
          // Add the element to the task
          vertex2part[v] = partid;
          ++curpartsize;
          // Remove the vertex from the set of unassigned vertices
          unassigned.erase(v); 
          // Add all its unassigned and unvisited neighbors to the queue
          foreach(edge_id_t eid, out_edge_ids(v)) {
            vertex_id_t u = target(eid);
            if(unassigned.find(u) != unassigned.end() &&
               visited.find(u) == visited.end()) {
              queue.push_back(u);
              visited.insert(u);
            }
          }
          foreach(edge_id_t eid, in_edge_ids(v)) {
            vertex_id_t u = source(eid);
            if(unassigned.find(u) != unassigned.end() &&
               visited.find(u) == visited.end()) {
              queue.push_back(u);
              visited.insert(u);
            }
          }
        } // End of block build foor loop
        // move to the next part
        partid++;
      }// end of outer while loop
    } // end of bfs partition



    /**
     * Partition the graph using one of the available partitioning
     * methods.
     */
    void partition(partition_method::partition_method_enum partmethod,
                   size_t nparts, std::vector<uint32_t>& vertex2part) {
      switch (partmethod) {
      case partition_method::PARTITION_METIS:
        return metis_partition(nparts, vertex2part);
      case partition_method::PARTITION_BFS:
        return bfs_partition(nparts, vertex2part);
      case partition_method::PARTITION_RANDOM:
        return random_partition(nparts, vertex2part);
      case partition_method::PARTITION_EDGE_NUM:
        return edge_num_partition(nparts, vertex2part);
      default:
        ASSERT_TRUE(false); //shoud never ever happen
      }
    }

    /**
     * builds a topological_sort of the graph. assuming it is top
     * sortable will return false if fails (i.e. cycles)
     */
    bool topological_sort(std::vector<vertex_id_t>& topsort) const {
      topsort.clear();
      topsort.reserve(num_vertices());
    
      std::vector<size_t> indeg;
      indeg.resize(num_vertices());
      std::queue<vertex_id_t> q;
      for (size_t i = 0;i < num_vertices(); ++i) {
        indeg[i] = in_edge_ids(i).size();
        if (indeg[i] == 0) {
          q.push(i);
        }
      }
    
      while (!q.empty()) {
        vertex_id_t v = q.front();
        q.pop();
        topsort.push_back(v);
        foreach(edge_id_t eid, out_edge_ids(v)) {
          vertex_id_t destv = target(eid);
          --indeg[destv];
          if (indeg[destv] == 0) {
            q.push(destv);
          }
        }
      }
    
      if (q.empty() && topsort.size() != num_vertices()) return false;
      return true;
    } // end of topological sort


    
  private:    
    /** Internal edge class  */   
    class edge {
      vertex_id_t _source;
      vertex_id_t _target;
      EdgeData _data;
    public:
      edge() : _source(-1), _target(-1) { }
      edge(const edge& other) :
        _source(other.source()), _target(other.target()),
        _data(other.data()) { }
      edge(vertex_id_t source, vertex_id_t target) :
        _source(source), _target(target)  { }
      edge(vertex_id_t source, vertex_id_t target, EdgeData data) : 
        _source(source), _target(target), _data(data) {}
		EdgeData get_data(){
			return _data.load(std::memory_order_relaxed);
		}
		void set_data(EdgeData edata){
			//_data = edata;
			_data.store(edata, std::memory_order_relaxed);
		}
      bool operator<(const edge& other) const {
        return (_source < other._source) || 
          (_source == other._source && _target < other._target); 
      }
      
      inline vertex_id_t source() const { return _source; }
      inline vertex_id_t target() const { return _target; }   
      inline EdgeData& data() { return _data; }
      inline const EdgeData& data() const { return _data; }
      
      void load(iarchive& arc) {
        arc >> _source
            >> _target
            >> _data;
      }
      
      void save(oarchive& arc) const {
        arc << _source
            << _target
            << _data;
      }
    }; // end of edge_data


    
    struct edge_id_less_functor {
      graph* g_ptr;
      edge_id_less_functor(graph* g_ptr) : g_ptr(g_ptr) { }
      bool operator()(edge_id_t a, edge_id_t b) {
        return g_ptr->edge_id_less(a,b);
      }
    };
    

    
    /**
     * Used to order edge ids in the in and out edges vectors based on
     * the lexical ordering of the vertex ids of the corresponding
     * edge
     */
    inline bool edge_id_less(edge_id_t a, edge_id_t b) const {
      assert(a < edges.size());
      assert(b < edges.size());
      return edges[a] < edges[b];
    }

    
 
    // PRIVATE DATA MEMBERS ===================================================>    
    /** The vertex data is simply a vector of vertex data */
    std::vector<VertexData> vertices;

    /** The edge data is a vector of edges where each edge stores its
        source, destination, and data. */
    std::vector<edge> edges;
    
    /** A map from src_vertex -> dest_vertex -> edge index */   
    std::vector< std::vector<edge_id_t> >  in_edges;
    
    /** A map from src_vertex -> dest_vertex -> edge index */   
    std::vector< std::vector<edge_id_t> >  out_edges;
    
    /** The vertex colors specified by the user. **/
    std::vector< vertex_color_type > vcolors;  
    
    /** Mark whether the graph is finalized.  Graph finalization is a
        costly procedure but it can also dramatically improve
        performance. */
    bool finalized;
    
    
    // PRIVATE HELPERS =========================================================>
    /**
     * This function tries to find the edge in the vector.  If it
     * fails it returns size_t(-1)
     */
    size_t binary_search(const std::vector<edge_id_t>& vec,
                         vertex_id_t source, vertex_id_t target) const {
      // Compare to the middle of the list
      size_t first = 0;
      size_t last = vec.size() - 1;
      while(first <= last) {
        size_t mid = (first+last)/2;
        vertex_id_t mid_source = edges[vec[mid]].source();
        vertex_id_t mid_target = edges[vec[mid]].target();
        // Edge found
        if(mid_source == source && mid_target == target) {
          return mid;
        } else if(std::make_pair(source, target) <
                  std::make_pair(mid_source, mid_target) ) {
          // Search left
          last = mid - 1;
        } else {
          // Search right
          first = mid + 1;
        }
      }
      // We failed to find
      return -1;
    } // end of binary search 
    
  }; // End of graph

  template<typename VertexData, typename EdgeData>
  std::ostream& operator<<(std::ostream& out,
                           const graph<VertexData, EdgeData>& graph) {

    out << "Printing vertex data:\n";
    for(size_t i = 0; i < graph.num_vertices(); ++i) {
      out << "V_" << i << ": D[" << graph.vertex_data(i) << "]\n";      
    }
    out << "Printing edge data:\n";
    for(size_t i = 0; i < graph.num_edges(); ++i) {
      out << "(V_" << graph.source(i) << "-> V_" << graph.target(i) << "): "
          << "D[" << graph.edge_data(i) << "]\n";      
    }
    return out;
  }
  

} // end of namespace graphlab
#include <graphlab/macros_undef.hpp>

#endif

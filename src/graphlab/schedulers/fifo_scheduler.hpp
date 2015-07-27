/**
 * This class defines a basic FIFO (First In First Out) scheduler
 **/
#ifndef FIFO_SCHEDULER_HPP
#define FIFO_SCHEDULER_HPP

#include <queue>
#include <cmath>
#include <cassert>

#include <graphlab/graph/graph.hpp>
#include <graphlab/scope/iscope.hpp>
#include <graphlab/tasks/update_task.hpp>
#include <graphlab/schedulers/ischeduler.hpp>
#include <graphlab/parallel/pthread_tools.hpp>
#include <graphlab/schedulers/support/vertex_task_set.hpp>
#include <graphlab/schedulers/support/direct_callback.hpp>
//#include <util/shared_termination.hpp>
#include <graphlab/util/task_count_termination.hpp>

// #include <bitmagic/bm.h>


#include <graphlab/macros_def.hpp>

namespace graphlab {


 
  template<typename Graph>
  class fifo_scheduler: public ischeduler<Graph> {
  public:
    typedef Graph graph_type;
    typedef ischeduler<Graph> base;

    typedef typename base::iengine_type iengine_type;
    typedef typename base::update_task_type update_task_type;
    typedef typename base::update_function_type update_function_type;
    typedef typename base::callback_type callback_type;
    typedef typename base::monitor_type monitor_type;
    
    
    
  private:
    using base::monitor;

  public:

    fifo_scheduler(iengine_type* engine,
                   Graph& g, 
                   size_t ncpus)  : 
      callbacks(ncpus, direct_callback<Graph>(this, engine)), 
      vertex_tasks(g.num_vertices()) {
      numvertices = g.num_vertices();
    }

    callback_type& get_callback(size_t cpuid) {
      return callbacks[cpuid];
    }


    /** Get the next element in the queue */
    sched_status::status_enum get_next_task(size_t cpuid, update_task_type &ret_task) {    
      if (terminator.finish()) return sched_status::COMPLETE;
      
      bool success(false);
      queue_lock.lock();
      if(!task_queue.empty()) {
        ret_task = task_queue.front();
        task_queue.pop();
        success = true;
      }
      queue_lock.unlock();
      
      if(success) {
        if (monitor != NULL) {
          double priority = vertex_tasks.top_priority(ret_task.vertex());
          monitor->scheduler_task_scheduled(ret_task, priority);
		 // std::cout<<"get a task to schedule priority============================="<<priority<<std::endl;
        }
        vertex_tasks.remove(ret_task);
        return sched_status::NEWTASK;
      } else {
        return sched_status::WAITING;
      }
    } // end of get_next_task
    

    void add_task(update_task_type task, double priority) {
      if (vertex_tasks.add(task)) {
        terminator.new_job();
        queue_lock.lock();
        task_queue.push(task);
        queue_lock.unlock();
		//std::cout<<"======================fifo scheduler add vertex="<<task.vertex()<<" to schedule priority"<<priority<<std::endl;
        if (monitor != NULL) 
          monitor->scheduler_task_added(task, priority);
      } else {
		//std::cout<<"+++++++++++++++++++fifo scheduler add vertex="<<task.vertex()<<" to schedule failed!!!!!!!!!!!!! it already exists"<<std::endl;
        if (monitor != NULL) 
          monitor->scheduler_task_pruned(task);
      }
    } // end of add_task

    void add_tasks(const std::vector<vertex_id_t> &vertices,
                   update_function_type func,
                   double priority) {
      foreach(vertex_id_t vertex, vertices) {
        add_task(update_task_type(vertex, func), priority);
      }
    } // end of add_tasks

    void add_task_to_all(update_function_type func, double priority) {
		std::cout<<"fifo scheduler add all vertices to schedule"<<std::endl;
      for (vertex_id_t vertex = 0; vertex < numvertices; ++vertex){
        add_task(update_task_type(vertex, func), priority);
      }
    } // end of add_task_to_all

  
    void update_state(size_t cpuid,
                      const std::vector<vertex_id_t> &updated_vertices,
                      const std::vector<edge_id_t>& updatededges) {};

    void scoped_modifications(size_t cpuid, vertex_id_t rootvertex,
                              const std::vector<edge_id_t>& updatededges){}

    void completed_task(size_t cpuid, const update_task_type &task) {
      terminator.completed_job();
    }

    void abort() { terminator.abort(); }
 
    void restart() { terminator.restart(); }

  private:
    size_t numvertices; /// Remember the number of vertices in the graph
  
    std::queue<update_task_type> task_queue; /// The actual task queue
    spinlock queue_lock; // The lock to get an element from the queue

    /// The callbacks pre-created for each cpuid
    std::vector< direct_callback<Graph> > callbacks; 

    // Task set for task pruning
    vertex_task_set<Graph> vertex_tasks;
  
    task_count_termination terminator;
  }; 


} // end of namespace graphlab
#include <graphlab/macros_undef.hpp>

#endif

/*****************************************************************************
 *   Copyright (C) 2004-2013 The PaGMO development team,                     *
 *   Advanced Concepts Team (ACT), European Space Agency (ESA)               *
 *   http://apps.sourceforge.net/mediawiki/pagmo                             *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Developers  *
 *   http://apps.sourceforge.net/mediawiki/pagmo/index.php?title=Credits     *
 *   act@esa.int                                                             *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or       *
 *   (at your option) any later version.                                     *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU General Public License for more details.                            *
 *                                                                           *
 *   You should have received a copy of the GNU General Public License       *
 *   along with this program; if not, write to the                           *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 *****************************************************************************/

#ifndef PAGMO_PROBLEM_TSP_H
#define PAGMO_PROBLEM_TSP_H

#include "../config.h"
#include "../serialization.h"
#include "base_tsp.h"

namespace pagmo { namespace problem {

/// Traveling salesman problem
/**
 *
 * Given a list of cities and their pairwise distances, the task is to find the 
 * shortest possible tour that visits each city exactly once.
 *
 * TSP can be modeled as a graph, such that cities are the graph's vertices, 
 * paths are the graph's edges, and a path's distance is the edge's weight. 
 * A TSP tour becomes a Hamiltonian cycle, and the optimal TSP tour is the 
 * shortest Hamiltonian cycle.
 * In the symmetric TSP, the distance between two cities is the same in 
 * each opposite direction, forming an undirected graph. 
 * This symmetry halves the number of possible solutions. 
 * In the asymmetric TSP, paths may not exist in both directions 
 * or the distances might be different, forming a directed graph.
 * The problem is formulated as an integer linear programming optimization.
 * 
 * Each vertex is labeled with an index number and n is the total number of vertices.
 * 
 * X_{i,j} is the square binary matrix, with X_{i,j} = 1 if there is an edge
 * between city i and city j, otherwise X_{i,j} = 0;
 * 
 * C_{i,j} is the square adjacency matrix containing the distances between
 * city i and city j, with i and j in [0, n]. If the problem is symmetric,
 * then the matrix is symmetric, meaning it's equal to it's transpose.
 * For i = [1,n] u_i is an artificial variable.
 * 
 * Then the TSP can be written as follows:
 * 
 *      minimize SUM_i^n SUM_j^n C_{i,j} * X_{i,j}
 * 
 * where
 * 
 *      i!=j and i, j in [0, n]
 * 
 * with 
 * 
 *      0 <= X_{i,j} <= 1
 * 
 *      u_i from Z e.g. {.., -2, -1, 0, 1, 2, ..}
 * 
 *      sum_i^n X_{i,j} = sum_j^n X_{i,j} = 1 where i!=j 
 * 
 *      u_i - u_j + n * X_{i,j} <= n - 1 where i <= i!=j <= n
 *
 * NOTE: Most of the TSPLIB problems are dense (e.g. fully connected graphs).
 *
 * @author Florin Schimbinschi (florinsch@gmail.com)
 */
class __PAGMO_VISIBLE tsp: public base_tsp
{
    public:
        tsp();
        tsp(const std::vector<std::vector<double> >&);
        tsp(const tsp_graph&);
        base_ptr clone() const;
        std::string get_name() const;
        const std::vector<std::vector<double> >& get_weights() const;
        static size_t compute_idx(const size_t, const size_t, const size_t);
            
    protected:
        void check_matrix(const std::vector<std::vector<double> >&) const;
        
        void objfun_impl(fitness_vector&, const decision_vector&) const;
        void compute_constraints_impl(constraint_vector&, const decision_vector&) const;
        
    private:
        static tsp_graph matrix2graph(const std::vector<std::vector<double> >);
        static std::vector<std::vector<double> > graph2matrix(const tsp_graph);

        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
                ar & boost::serialization::base_object<base>(*this);
                ar & m_weights;
        }
    private:        
        std::vector<std::vector<double> > m_weights;

};

}} //namespaces

BOOST_CLASS_EXPORT_KEY(pagmo::problem::tsp)

#endif

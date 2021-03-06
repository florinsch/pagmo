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

#include "aco_elite.h"

namespace pagmo { namespace algorithm {
    
    /// Constructor.
    /**
     * @param[in] cycle - number of cycles. Each cycle, the m ants complete a tour of n cities.
     * @param[in] ants - number of ants in the colony.
     * @param[in] rho - each ant leaves a trail of pheromone which evaporates according to this constant.
     * @param[in] e - each elite ant leaves a stronger trail of pheromone. this constant influences the quantity.
     */
    aco_elite::aco_elite(int cycles, int ants, double rho, double alpha, double beta, double e): m_cycles(cycles), m_ants(ants), m_rho(rho), m_alpha(alpha), m_beta(beta), m_e(e)
    {
        init_checks();
    }
    
    /// Returns the constant for trail boost of the elite ants.
    /**
     * Returns the constant e
     * @return int - the constant by which elites drop more pheromone
     */
    double aco_elite::get_e() const
    {
        return m_e;
    }
    
    /// Sets the constant for trail boost of the elite ants.
    /**
     * Setter for the m_e property
     * @param e - int, the constant that boost trails for elite ants.
     */
    void aco_elite::set_e(double e)
    {
        m_e = e;
    }

    /// Clone method.
    base_ptr aco_elite::clone() const
    {
        return base_ptr(new aco_elite(*this));
    }
    
    /// Evolve implementation.
    /**
     * Runs the ACO algorithm for the number of generations specified in the constructor.
     *
     * @param[in,out] pop input/output pagmo::population to be evolved.
     */
    void aco_elite::evolve(population &pop) const
    {
        // Configuration parameters, maybe put them somewhere else?
        const double c = 0.1;
        const double Q = 1;
        
        // Let's store some useful variables.
        const problem::tsp &tsp_prob = dynamic_cast<const problem::tsp &>(pop.problem());
        const std::vector<std::vector<double> > &weights = tsp_prob.get_weights();
        const problem::base::size_type no_vertices = tsp_prob.get_n_vertices();
        size_t NP = pop.size();
        
        // random engine for ant init
        std::default_random_engine generator(time(NULL));
        std::uniform_int_distribution<size_t> dist(0, no_vertices - 1);

        //TODO: add check on the problem. The problem needs to be an integer problem TSP like, single objective.
        if (NP <= 1) pagmo_throw(value_error, "population size must be greater than one.");
        if (m_ants <= static_cast<int>(NP)) pagmo_throw(value_error, "the number of ants needs to be greater than the population size.");
        
        // deposit the initial pheromone along the edges of the graph
        std::vector<std::vector<double> > tau(initialize_pheromone(no_vertices, c));
        
        // Main ACO loop: stops when either maximum number of cycles reached or all ants make the same tour
        for (int t = 0; t < m_cycles; ++t) {    
            // (re)set delta_tau to 0
            std::vector<std::vector<double> > delta_tau(no_vertices, std::vector<double>(no_vertices, 0));
            
            // keep all the ant tours for one cycle here
            std::vector<aco_tour> ant_tours(m_ants);

            // compute shortest paths and costs for all ants
            for (int ant = 0; ant < m_ants; ++ant) {                
                // launch an ant and make a round-trip from a random starting position
                ant_tours.at(ant) = forage(dist(generator), weights, tau);
                
                // update delta_tau
                for (size_t i = 0; i < ant_tours.at(ant).tour.size() - 1; ++i) {
                    size_t from = ant_tours.at(ant).tour.at(i), to = ant_tours.at(ant).tour.at(i+1);
                    delta_tau.at(from).at(to) += Q/ant_tours.at(ant).length;
                }
                delta_tau.at( ant_tours.at(ant).tour.back() ).at( ant_tours.at(ant).tour.front() ) += Q/ant_tours.at(ant).length;
                
            } // finished with all ants, cycle computations can be performed
            
            // search for lowest cost and get corresponding tour after this cycle
            std::vector<aco_tour>::iterator low_it = std::min_element(ant_tours.begin(), ant_tours.end());
            double lowest_cost = (*low_it).length;
            std::vector<size_t> shortest_path = ant_tours.at( std::distance(ant_tours.begin(), low_it) ).tour;
            
            // reinforce delta_tau with elite ant (shortest tour)
            for (size_t i = 0; i < shortest_path.size() - 1; ++i) {
                size_t from = shortest_path.at(i), to = shortest_path.at(i+1);
                delta_tau.at(from).at(to) += (m_e * Q/lowest_cost);
            }
            delta_tau.at(shortest_path.back()).at(shortest_path.front()) += (m_e * Q/lowest_cost);
            
            // update pheromone matrix
            for (size_t i = 0; i < tau.size(); ++i) 
                for (size_t j = 0; j < tau.size(); ++j)
                    tau.at(i).at(j) = tau.at(i).at(j) * m_rho + delta_tau.at(i).at(j);
            
            // make tour consistent
            aco::make_tour_consistent(shortest_path);
            //convert to decision vector and 
            // save the shortest path or the last n cycles in the population. 
            // where n is the number of individuals in the population. 
            pop.set_x( NP > 0 ? --NP : pop.size()-1 , aco::tour2chromosome(shortest_path));
            
            
            // store lambdas
            m_lambda.push_back( aco::get_l_branching(0.5, tau) );
            
            // stop cycles if we're close to three and f'(x) -> 0
            if (t > m_cycles/4 && m_lambda.at(t) < 4 && abs(m_lambda.at(t) - m_lambda.at(t-2)) )
                break;
        } // end of main ACO loop (cycles)
        
    }

    /// Algorithm name.
    std::string aco_elite::get_name() const
    {
        return "Elitist Ant System";
    }

    /// Extra human readable algorithm info.
    /**
     * Will return a formatted string displaying the parameters of the algorithm.
     */
    std::string aco_elite::human_readable_extra() const
    {
        std::ostringstream s;
        s << "\nNumber of cycles: " << m_cycles
            << "\nNumber of ants: " << m_ants
            << "\nPheromone evaporation (Rho): " << m_rho
            << "\nElite constant (e): " << m_e << std::endl;
        return s.str();
    }

}} //namespaces

BOOST_CLASS_EXPORT_IMPLEMENT(pagmo::algorithm::aco_elite)
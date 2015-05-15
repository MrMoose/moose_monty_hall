
//  Copyright 2015 Stephan Menzel. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <moose/tools/Random.hpp>

#include <boost/thread.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <iostream>

struct door {
	door() : m_win(false), m_open(false) {};
	bool m_win;
	bool m_open;
};

enum strategy {
	STUBBORN = 0,  // will stick to original choice upon Monty revealing the goat
	MINDCHANGER,   // will always change his mind to the remaining door  
	UNDECIDED      // reacts randomly
};

//! \return win or loose
bool round(const strategy n_strategy) {
	
	struct door doors[3];
	
	// First assign the door to win the car
	doors[moose::tools::urand(2)].m_win = true;
	
	// Now our player selects a door randomly
	unsigned int choice = moose::tools::urand(2);
	
	// Monty selects another random door with a goat to open
	unsigned int idx = 0;
	do {
		idx = moose::tools::urand(2);
	} while (doors[idx].m_win || (idx == choice)); 
						// we need a random index that looses 
						// and was not the player's choice
	doors[idx].m_open = true;
	
	switch (n_strategy) {
		case STUBBORN:
			// player does not change his mind but sticks with his choice
			break;
		case UNDECIDED:
			// undecided player has a 50% chance to change his mind, otherwise 
			// bail too and stick with his choice
			if (moose::tools::urand(100) < 50) {
				break;
			}
		case MINDCHANGER:
			// player always changes his mind and changes 
			// his choice to the remaining door
			for (unsigned int i = 0; i < 3; ++i) {
				if (!doors[i].m_open && (i != choice)) {
					choice = i;
					break;
				}
			}
			break;
	}
		
	// player does not change his mind but sticks with his choice
	return doors[choice].m_win;
}

// return ratio of wins in percent
void player(double &n_result, const strategy n_strategy, const unsigned int n_rounds) {
	
	using namespace boost::accumulators;
	
	// accumulate all the results
	accumulator_set<double, stats<tag::mean> > acc;

	// play as many rounds and accumulate wins and loosses
	for (unsigned int i = 0; i < n_rounds; ++i) {
		
		bool result = round(n_strategy);
		acc(result ? 100.0 : 0.0);
	}
	
	n_result = mean(acc);
}


int main(int argc, char **argv) {

	unsigned int rounds = 1000000;
	
	double stubborn_wins = 0.0;
	double mindchanger_wins = 0.0;
	double undecided_wins = 0.0;
	
	std::cout << "players going to work..." << std::endl;
	
	boost::thread sp = boost::thread(boost::bind(&player, boost::ref(stubborn_wins),    STUBBORN,    rounds));
	boost::thread mp = boost::thread(boost::bind(&player, boost::ref(mindchanger_wins), MINDCHANGER, rounds));
	boost::thread up = boost::thread(boost::bind(&player, boost::ref(undecided_wins),   UNDECIDED,   rounds));

	sp.join();
	mp.join();
	up.join();
	
	std::cout << "Stubborn player won " << stubborn_wins << " percent of his " << rounds << " rounds." << std::endl;
	std::cout << "Mindchanger player won " << mindchanger_wins << " percent of his " << rounds << " rounds." << std::endl;
	std::cout << "Undecided player won " << undecided_wins << " percent of his " << rounds << " rounds." << std::endl;
}


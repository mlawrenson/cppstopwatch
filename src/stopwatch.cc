/*
This file is part of Stopwatch, a project by Tommaso Urli.

Stopwatch is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Stopwatch is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Stopwatch.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stopwatch/stopwatch.hh"


using std::map;
using std::string;
using std::ostringstream;

Stopwatch::Stopwatch() : active(true)  {
	records_of = new map<string, PerformanceData>();
}

Stopwatch::~Stopwatch() {
	delete records_of;
}

bool Stopwatch::performance_exists(string perf_name) {
	return (records_of->find(perf_name) != records_of->end());
}

long double Stopwatch::take_time() {
		// Use chrono
    auto timenow = std::chrono::system_clock::now();
    // Get time since epoch in nano seconds
    long long tse_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(timenow.time_since_epoch()).count();

		return tse_nano / 1e9;
}

void Stopwatch::start(string perf_name)  {

	if (!active) return;

	// Just works if not already present
	records_of->insert(make_pair(perf_name, PerformanceData()));

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	// Take ctime
	perf_info.clock_start = take_time();

	// If this is a new start (i.e. not a restart)
	if (!perf_info.paused)
		perf_info.last_time = 0;

	perf_info.paused = false;
}

void Stopwatch::stop(string perf_name) {
	
	if (!active) return;
	
	long double clock_end = take_time();
	
	// Try to recover performance data
	if ( !performance_exists(perf_name) )
		throw StopwatchException("Performance not initialized.");
	
	PerformanceData& perf_info = records_of->find(perf_name)->second;
	
	perf_info.stops++;
	long double  lapse = clock_end - perf_info.clock_start;

  // Last time is 0 unless clock was paused, so lapse is actually last_time+lapse in that case
  long double unpaused_lapse = perf_info.last_time + lapse;
	
	// Update last time
	perf_info.last_time = unpaused_lapse;

	// Update min/max time
	if ( unpaused_lapse >= perf_info.max_time )	perf_info.max_time = unpaused_lapse;
	if ( unpaused_lapse <= perf_info.min_time || perf_info.min_time == 0 )	perf_info.min_time = unpaused_lapse;
	
	// Update total time
	perf_info.total_time += lapse;
}

void Stopwatch::pause(string perf_name) {
	
	if (!active) return;
	
	long double  clock_end = take_time();
	
	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");
	
	PerformanceData& perf_info = records_of->find(perf_name)->second;
	
	long double  lapse = clock_end - perf_info.clock_start;
	
	// Update total time
	perf_info.last_time += lapse;
	perf_info.total_time += lapse;
  perf_info.paused = true;
}

void Stopwatch::reset_all() {
	
	if (!active) return;

	map<string, PerformanceData>::iterator it;
	
	for (it = records_of->begin(); it != records_of->end(); ++it) {
		reset(it->first);
	}
}

void Stopwatch::report_all(std::ostream& output) {

	if (!active) return;

	map<string, PerformanceData>::iterator it;
	
	for (it = records_of->begin(); it != records_of->end(); ++it) {
		report(it->first, output);
	}
}

void Stopwatch::reset(string perf_name) {

	if (!active) return;

	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");
	
	PerformanceData& perf_info = records_of->find(perf_name)->second;
	
	perf_info.clock_start = 0;
	perf_info.total_time = 0;
	perf_info.min_time = 0;
	perf_info.max_time = 0;
	perf_info.last_time = 0;
	perf_info.paused = false;
	perf_info.stops = 0;
}

void Stopwatch::turn_on() {
	std::cout << "Stopwatch active." << std::endl;
	active = true;
}

void Stopwatch::turn_off() {
	std::cout << "Stopwatch inactive." << std::endl;
	active = false;
}

void Stopwatch::report(string perf_name, std::ostream& output) {

	if (!active) return;
	
	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");
	
	PerformanceData& perf_info = records_of->find(perf_name)->second;

	// To support Windows (otherwise string("=", perf_name.length() + 1) would do the job
	string bar = "";
	for (unsigned int i = 0; i < perf_name.length(); i++)
		bar.append("=");

	output << std::endl;
	output << "======================" << bar << std::endl;
	output << "Tracking performance: " << perf_name << std::endl;
	output << "======================" << bar << std::endl;
	output << "  *  Avg. time " << (perf_info.total_time / (long double) perf_info.stops) << " sec" << std::endl;
	output << "  *  Min. time " << (perf_info.min_time) << " sec" << std::endl;
	output << "  *  Max. time " << (perf_info.max_time) << " sec" << std::endl;
	output << "  *  Tot. time " << (perf_info.total_time) << " sec" << std::endl;

	ostringstream stops;
	stops << perf_info.stops;

	output << "  *  Stops " << stops.str() << std::endl;
	output << std::endl;
	
}

long double Stopwatch::get_time_so_far(string perf_name) {
    // Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");
    
    long double lapse = (take_time() - (records_of->find(perf_name)->second).clock_start);
    
	return lapse;
}

long double Stopwatch::get_total_time(string perf_name) {

	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	return perf_info.total_time;

}

long double Stopwatch::get_average_time(string perf_name) {
	
	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	return (perf_info.total_time / (long double)perf_info.stops);

}

long double Stopwatch::get_min_time(string perf_name) {
	
	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	return perf_info.min_time;

}

long double Stopwatch::get_max_time(string perf_name) {
	
	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	return perf_info.max_time;

}

long double Stopwatch::get_last_time(string perf_name) {

	// Try to recover performance data
	if ( !performance_exists(perf_name)  )
		throw StopwatchException("Performance not initialized.");

	PerformanceData& perf_info = records_of->find(perf_name)->second;

	return perf_info.last_time;

}

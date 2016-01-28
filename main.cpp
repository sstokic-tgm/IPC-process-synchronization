/*
Name of file:		main.cpp
Author:				Stefan Stokic <sstokic@student.tgm.ac.at>
Version:			20160128.4
Description:		Synchronization of processes using the boost library with named conditions and/or message que's.
					The outcommented code, is used for the named condition method
*/

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <sstream>

using namespace boost::interprocess;
using namespace boost::posix_time;

// format the timestamp
std::string formatTime(boost::posix_time::ptime now);


int main(int argc, char **argv)
{
	/*
	// for removing the used mem shared object, mutex and the named condition (uncomment this only if the program chrashes while working so you can free the used
	// memory)
	shared_memory_object::remove("shm1");
	shared_memory_object::remove("shm2");
	named_mutex::remove("mtx");
	named_condition::remove("cnd");*/

	// creating two shared memories
	managed_shared_memory managed_shm{ open_or_create, "shm1", 1024 };
	managed_shared_memory managed_shm2{ open_or_create, "shm2", 1024 };

	// declare & initialize two variables for each shared memory
	int *i = managed_shm.find_or_construct<int>("mod_Integer")(0); // the counting number for the modulo
	int *procNum = managed_shm2.find_or_construct<int>("procNum")(0); // the counting number of the started process

	// create a mutex and a named_condition variable
	//named_mutex named_mtx{ open_or_create, "mtx" };
	//named_condition named_cnd{ open_or_create, "cnd" };

	// create a message_que with the open_or_create flag, it's name, max message number & max message size
	message_queue mq{ open_or_create, "mod_ipc", 500, 2048 };
	
	bool modAll = false, modRest2 = false, modRest1 = false, proc4 = false;

	unsigned int priority;
	message_queue::size_type recvd_size;

	// count process number
	++(*procNum);
	std::cout << "Prozess " << *procNum << " gestartet...";

	// decide which process is running which modulo operation
	if (*i % 3 == 2)
		modRest2 = true;
	else if (*i % 3 == 1)
		modRest1 = true;
	else if (*i % 3 == 0)
		modAll = true;

	// loop through 100 numbers
	while (*i < 100)
	{
		if (modRest2 && *i % 3 == 2)
		{
			// get the current time/timestamp
			ptime now = second_clock::universal_time();
			std::string time(formatTime(now));

			// declare our output string
			std::string sOut = time + " Prozess 3: " + std::to_string(*i);
			//scoped_lock<named_mutex> lock{ named_mtx };
			//std::cout << "Prozess 3: " << *i << std::endl;

			// send the output
			mq.send(sOut.data(), sOut.size(), 0);
			++(*i); // increment our number
			//named_cnd.notify_all();
			//named_cnd.wait(lock);
		}
		else if (modRest1 && *i % 3 == 1)
		{
			// get the current time/timestamp
			ptime now = second_clock::universal_time();
			std::string time(formatTime(now));

			// declare our output string
			std::string sOut = time + " Prozess 2: " + std::to_string(*i);
			//scoped_lock<named_mutex> lock{ named_mtx };
			//std::cout << "Prozess 2: " << *i << std::endl;

			// send the output
			mq.send(sOut.data(), sOut.size(), 0);
			++(*i); // increment our number
			//named_cnd.notify_all();
			//named_cnd.wait(lock);
		}
		else if(modAll && *i % 3 == 0)
		{
			// get the current time/timestamp
			ptime now = second_clock::universal_time();
			std::string time(formatTime(now));

			// declare our output string
			std::string sOut = time + " Prozess 1: " + std::to_string(*i);
			//scoped_lock<named_mutex> lock{ named_mtx };
			//std::cout << "Prozess 1: " << *i << std::endl;

			// send the output
			mq.send(sOut.data(), sOut.size(), 0);
			++(*i); // increment our number
			//named_cnd.notify_all();
			//named_cnd.wait(lock);
		}
	}

	// if we are the fourth process
	if (*procNum == 4)
	{
		std::cout << std::endl << std::endl;

		// get all messages in the message_que
		for (int c = 0; c < 100; c++)
		{
			// we need to serialize our data, cause message_que operates on byte arrays
			std::string text;
			text.resize(2048);
			mq.receive(&text[0], text.size(), recvd_size, priority);
			text.resize(recvd_size);

			// output the text of each message in the message_que
			std::cout << text << std::endl;
		}
		
		// finally we can free our reserved memory
		// if we do it outside our if-check for the fourth process, we aren't able to pipe the data to the fourth process
		//named_cnd.notify_all();
		shared_memory_object::remove("shm1");
		shared_memory_object::remove("shm2");
		//named_mutex::remove("mtx");
		//named_condition::remove("cnd");
		message_queue::remove("mod_ipc");
	}	

	std::getchar(); // let the user wait to press a key, so he can see the output
	return EXIT_SUCCESS;
}

std::string formatTime(boost::posix_time::ptime now)
{
	std::locale loc(std::cout.getloc(), new time_facet("[%H:%M:%S]"));

	std::basic_stringstream<char> ss;
	ss.imbue(loc);
	ss << now;

	return ss.str();
}
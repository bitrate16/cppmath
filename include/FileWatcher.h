#pragma once

#include <functional>
#include <thread>
#include <string>
#include <iostream>

class FileWatcher {
		
	// Use file temporary buffer to handle local file changes
	bool use_file_buffer = 0;
	// File buffer name
	std::string file_buffer; // := file + ".fw"
	// Buffer in dynamical memory
	char* buffer     = nullptr;
	long buffer_size = 0;
	// File name
	std::string file;
	// Thread checking file change on each timing
	std::thread* watcher = nullptr;
	// Controls state of watching thread
	bool watching        = 0;
	// Delay between checks
	long delay = 1000;
		
public:
		
	// Holds handler for file change event
	// @params: 
	//     Reference of file watcher.
	//     Position in file of mismatching character.
	std::function<void (FileWatcher&, long)> handler;
	
	FileWatcher(const std::string& file, std::function<void (FileWatcher&, int)> handler, long delay = 1000, bool use_file_buffer = 0) 
		: file(file), handler(handler), delay(delay), use_file_buffer(use_file_buffer) {
		if (use_fle_buffer)
			file_buffer = file + ".w";
		
		watcher = new std::thread([](FileWatcher*){
			
		}, this);
	};
};
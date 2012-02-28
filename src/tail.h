/*
 *  tail.h
 *  
 *
 *  Created by Marco Rondini on 14/02/12.
 *  
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include <cstdlib>   // for rand()
#include <cctype>    // for isalnum()   
#include <algorithm> // for back_inserter

#include <v8.h> // v8 is the Javascript engine used by Node
#include <node.h>

using namespace std;
using namespace v8;

class Tail : node::ObjectWrap {
private:
	map<std::string, bool> m_instance;
public:
	Tail() {}
	~Tail() {}
	
	struct tail_baton_t {
		Persistent<Function> cb;
		std::string instance;
		std::string filename;
		Tail *T;
	};
	
	struct line_baton_t {
		Persistent<Function> cb;
		std::string line;
		std::string err;
	};
	
	static char rand_alnum();
	
	static std::string rand_alnum_str(std::string::size_type sz);
	
	static int find_last_linefeed(ifstream &infile);

	static int find_line_lenght(ifstream &infile, int position);
	
	static v8::Persistent<FunctionTemplate> persistent_function_template;
	
	static void Init(Handle<Object> target);
	
	static Handle<Value> New(const Arguments& args);
	
	static Handle<Value> stop(const Arguments& args);
	
	static Handle<Value> start(const Arguments& args);
	
	static void EIO_Tail(eio_req *req);
	
	static int EIO_AfterTail(eio_req *req);
	
	static void EIO_Line(eio_req *req) { return; }
    
	static int EIO_AfterLine(eio_req *req);
};
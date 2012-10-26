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

#include <v8.h> // v8 is the Javascript engine used by Node
#include <node.h>

using namespace std;
using namespace v8;

static Persistent<String> data_symbol;
static Persistent<String> error_symbol;
static Persistent<String> end_symbol;


class Tail : node::ObjectWrap {
private:
    Tail() {}
	~Tail() {}
    
    uv_fs_event_t _event_handle;
    
    static void OnEvent(uv_fs_event_t* handle, const char* filename, int events, int status);
        
public:
        
    bool ontail;
    
    string separator;
    
    int last_position;
    
	void Emit(Handle<String> event, int argc, Handle<Value> argv[]);
		
	static void Init(Handle<Object> target);
	
	static Handle<Value> New(const Arguments& args);
    
    static Handle<Value> GetReadable(Local<String> property, const AccessorInfo& info);
    
    static Handle<Value> GetWritable(Local<String> property, const AccessorInfo& info);
	
	static Handle<Value> close(const Arguments& args);
};
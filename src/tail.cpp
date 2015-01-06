/*
 *  tail.cpp
 *
 *
 *  Node.js native module. Re-Implementation of 'tail -f' command.
 *  
 *
 */

#include "tail.h"

#include <iostream>
#include <string>
#include <fstream>

#include <v8.h> // v8 is the Javascript engine used by Node
#include <node.h>

#define INSTANCE_LENGTH 10

using namespace std;
using namespace v8;

void Tail::Emit(Handle<String> event, int argc, Handle<Value> argv[]){
    HandleScope scope;
    
    Handle<Value>* argv_ = new Handle<Value>[argc + 1];
    
	argv_[0] = event;
    
	for ( int i = 1; i <= argc; i++ )
		argv_[i] = argv[i - 1];
	
    node::MakeCallback(handle_, "emit", argc + 1, argv_);
}  


void Tail::Init(Handle<Object> target) {
	v8::HandleScope scope;
	
	data_symbol = NODE_PSYMBOL("data");
	error_symbol = NODE_PSYMBOL("error");
	end_symbol = NODE_PSYMBOL("end");
    
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("Tail"));
    tpl->InstanceTemplate()->SetInternalFieldCount(3);
    
    // Prototype
    tpl->InstanceTemplate()->Set(String::NewSymbol("close"), FunctionTemplate::New(close)->GetFunction());
    tpl->InstanceTemplate()->SetAccessor(String::NewSymbol("readable"), Tail::GetReadable);
    tpl->InstanceTemplate()->SetAccessor(String::NewSymbol("writable"), Tail::GetWritable);
    
    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    target->Set(String::NewSymbol("Tail"), constructor);
}

Handle<Value> Tail::New(const Arguments& args) {
	HandleScope scope;
	
    assert(args.IsConstructCall());
    
    Tail* tail_instance = new Tail();
	tail_instance->Wrap(args.This());
    
    
	if (!args[0]->IsString())
		return ThrowException(Exception::TypeError(String::New("Argument 1 must be a String")));
	
    if (!args[1]->IsString())
		tail_instance->separator = "\n";
    else {
        String::Utf8Value v8separator(args[1]->ToString());
        tail_instance->separator = std::string(*v8separator, v8separator.length());
    }
    
	String::Utf8Value filename(args[0]);
    
    ifstream fp(*filename, ios::binary);
    fp.seekg(0, ios::end);
    tail_instance->last_position = fp.tellg();
    fp.close();
    
    tail_instance->_event_handle.data = reinterpret_cast<void*>(tail_instance);
    tail_instance->Ref();

    int r = uv_fs_event_init(uv_default_loop(), &tail_instance->_event_handle, *filename, OnEvent, 0);
    
    if (r == 0){
        tail_instance->ontail = false;
    } else
        node::SetErrno(uv_last_error(uv_default_loop()));
    
	return args.This();
}

Handle<Value> Tail::GetReadable(Local<String> property, const AccessorInfo& info) {
    HandleScope scope;
    return scope.Close(v8::True());
}

Handle<Value> Tail::GetWritable(Local<String> property, const AccessorInfo& info) {
    HandleScope scope;
    return scope.Close(v8::False());
}

Handle<Value> Tail::close(const Arguments& args) {
	HandleScope scope;
	
    assert(!args.Holder().IsEmpty());
    assert(args.Holder()->InternalFieldCount() > 0);
    
    void* ptr = args.Holder()->GetPointerFromInternalField(0);
    Tail* tail_instance = static_cast<Tail*>(ptr);
    
    uv_unref(reinterpret_cast<uv_handle_t*>(&tail_instance->_event_handle));
    
    tail_instance->Emit(end_symbol, 0, NULL);
    
	return scope.Close(Undefined());
}

void Tail::OnEvent(uv_fs_event_t* handle, const char* filename, int events, int status) {
	HandleScope scope;
    
    Tail* tail_instance = reinterpret_cast<Tail*>(handle->data);
    assert(tail_instance->handle_.IsEmpty() == false);
    
    if ( tail_instance->ontail ) return;
    if (events & UV_RENAME) return;
    
    tail_instance->ontail = true;
    
    file_position_type length = 0;
    file_position_type position = 0;
    char *buffer;
    
    ifstream fp(handle->filename, ios::binary);
    file_position_type line_length;
    
    fp.seekg(0, ios::end);
    position = fp.tellg();
    
    if ( position > tail_instance->last_position ) {
        
        fp.seekg(tail_instance->last_position);
        length = position - tail_instance->last_position;
        buffer = new char[length];
        fp.read (buffer, length);
        string buffer_str(buffer, length);
        
        line_length = buffer_str.find_first_of(tail_instance->separator);
               
        while ( line_length != (int)string::npos && line_length != -1 ){
            
            Local<Value> argv[1] = { String::New(buffer_str.substr(0, line_length).c_str()) };
            tail_instance->Emit(data_symbol, 1, argv);
            
            tail_instance->last_position += line_length + (int)tail_instance->separator.length();
            
            if ( line_length < (int)buffer_str.find_last_of(tail_instance->separator) && line_length != -1 )
                buffer_str = buffer_str.substr(line_length + (int)tail_instance->separator.length());
            else
                break;
            
            line_length = buffer_str.find_first_of(tail_instance->separator);
        }
    }
    
	tail_instance->ontail = false;
}


NODE_MODULE(tailnative, Tail::Init);

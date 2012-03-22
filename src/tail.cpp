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
#include <string.h>

#define INSTANCE_LENGTH 10

using namespace std;
using namespace v8;

void Tail::Emit(Handle<String> event, int argc, Handle<Value> argv[]){
    HandleScope scope;
    Handle<Value> argv_[argc + 1];
    
	argv_[0] = event;
    
	for ( int i=0; i < argc; i++ ) 
		argv_[i + 1] = argv[i];
	
    node::MakeCallback(handle_, "emit", argc + 1, argv_);
}  

int Tail::find_last_linefeed(ifstream &infile) {
	
	infile.seekg(0, ios::end);
	int filesize = infile.tellg();
	
	for ( int n = 1; n < filesize; n++ ) {
		infile.seekg(filesize - n, ios::beg);
		
		char c;
		infile.get(c);
		
		if ( c == '\n' ) 
			return infile.tellg();
	}
	
	return 0;
}

int Tail::find_line_lenght(ifstream &infile, int position) {
	
	infile.seekg(0, ios::end);
	int filesize = infile.tellg();
	
	infile.seekg(position, ios::beg);
		
	for ( int n = 1; n <= filesize - position; n++ ) {
		infile.seekg(position + n, ios::beg);
		char c;
		infile.get(c);
		
		if ( c == '\n' ) 
			return n;
	}
	return 0;
}

void Tail::Init(Handle<Object> target) {
	v8::HandleScope scope;
	
	data_symbol = NODE_PSYMBOL("data");
	error_symbol = NODE_PSYMBOL("error");
	end_symbol = NODE_PSYMBOL("end");
	
	v8::Local<FunctionTemplate> local_function_template = v8::FunctionTemplate::New(New);
	
	Tail::persistent_function_template = v8::Persistent<FunctionTemplate>::New(local_function_template);
	Tail::persistent_function_template->InstanceTemplate()->SetInternalFieldCount(1); 
	Tail::persistent_function_template->SetClassName(v8::String::NewSymbol("Tail"));
	
	
	NODE_SET_PROTOTYPE_METHOD(persistent_function_template, "start", start);
	NODE_SET_PROTOTYPE_METHOD(persistent_function_template, "stop", stop);
	
	target->Set(String::NewSymbol("Tail"), Tail::persistent_function_template->GetFunction());
}

Handle<Value> Tail::New(const Arguments& args) {
	HandleScope scope;
	
	Tail* tail_instance = new Tail();
	tail_instance->Wrap(args.This());

	tail_instance->wstop = true;
	
	return args.This();
}

Handle<Value> Tail::stop(const Arguments& args) {
	HandleScope scope;
	Tail* tail_instance = node::ObjectWrap::Unwrap<Tail>(args.This());
	
	if ( tail_instance->wstop )
		return ThrowException(Exception::TypeError(String::New("Instance not started.")));
	
	tail_instance->wstop = true;
	
	return scope.Close(Undefined());
}

Handle<Value> Tail::start(const Arguments& args) {
	HandleScope scope;
	
	Tail* tail_instance = node::ObjectWrap::Unwrap<Tail>(args.This());
	
	if ( !tail_instance->wstop )
		return ThrowException(Exception::TypeError(String::New("Instance alredy started.")));
	
	if (!args[0]->IsString())                   
		return ThrowException(Exception::TypeError(String::New("Argument 1 must be a String")));
	
	String::Utf8Value v8filename(args[0]->ToString());
	
	tail_instance->wstop = false;
	
	tail_baton_t *tail_bat = new tail_baton_t();
	tail_bat->filename = strtok(*v8filename, "");
	tail_bat->T = tail_instance;
	
	tail_instance->Ref();
	eio_custom(EIO_Tail, EIO_PRI_DEFAULT, EIO_AfterTail, tail_bat);
	ev_ref(EV_DEFAULT_UC);
	
	return scope.Close(Undefined());
	
}
void Tail::EIO_Tail(eio_req *req) {
	tail_baton_t *tail_bat = static_cast<tail_baton_t *>(req->data);
	
	ifstream infile(tail_bat->filename.c_str());
	int position = Tail::find_last_linefeed(infile);
	int last_position = position;
	int length = 0;
	
	while (!tail_bat->T->wstop) {
		ifstream infile(tail_bat->filename.c_str());
		position = Tail::find_last_linefeed(infile);
		if (position > last_position) {
			
			int length = Tail::find_line_lenght(infile, last_position);	
			
			for ( int tmp_pos = last_position; tmp_pos <= position - length; tmp_pos += length ){
				
				line_baton_t *line_bat = new line_baton_t();

				line_bat->T = tail_bat->T;
				line_bat->err = false;
				
				if ( tmp_pos < 0 ){
					line_bat->err = true;
					eio_custom(EIO_Line, EIO_PRI_DEFAULT, EIO_AfterLine, line_bat);
					tail_bat->T->wstop = true;
					break;
				}
				
				infile.seekg(tmp_pos, ios::beg);
				getline(infile, line_bat->line);
				int tmp_tellg = infile.tellg();
				
				if ( tmp_tellg == -1 ) {
					line_bat->err = true;
					eio_custom(EIO_Line, EIO_PRI_DEFAULT, EIO_AfterLine, line_bat);
					tail_bat->T->wstop = true;
					break;
				}
				
				length = tmp_tellg - tmp_pos;
				eio_custom(EIO_Line, EIO_PRI_DEFAULT, EIO_AfterLine, line_bat);
				
			}
			
			last_position = position;
			
			if ( length < 0 )
				break;
			
			
		} else if ( position + length < last_position ) {				
			position = Tail::find_last_linefeed(infile);
			last_position = 0;
			length = 0;
		}
		
		usleep(250000);
	}
	
	return;
}

int Tail::EIO_AfterTail(eio_req *req){
	tail_baton_t *tail_bat = static_cast<tail_baton_t *>(req->data);
	
	tail_bat->T->Emit(end_symbol, 0, NULL);
	
	ev_unref(EV_DEFAULT_UC);
	tail_bat->T->Unref();
	return 0;
}

int Tail::EIO_AfterLine(eio_req *req){
	line_baton_t *line_bat = static_cast<line_baton_t *>(req->data);
	
	if ( line_bat->err ){
		line_bat->T->Emit(error_symbol, 0, NULL);
	} else {
		Local<Value> argv[1];
		argv[0] = String::New(line_bat->line.c_str());
		line_bat->T->Emit(data_symbol, 1, argv);  
	}
	
	return 0;
}



v8::Persistent<FunctionTemplate> Tail::persistent_function_template;
extern "C" {
	static void init(Handle<Object> target) {
		Tail::Init(target);
	}
	NODE_MODULE(tailnative, init);
}

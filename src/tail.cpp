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

using namespace std;
using namespace v8;


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

void Tail::Init(Handle<Object> target) {
	v8::HandleScope scope;
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
	return args.This();
}

Handle<Value> Tail::stop(const Arguments& args) {
	HandleScope scope;
	Tail* tail_instance = node::ObjectWrap::Unwrap<Tail>(args.This());
	
	Handle<Boolean> ret = v8::Boolean::New(false);
	
	if ( tail_instance->b_stop == false ){
		tail_instance->b_stop = true;
		ret = v8::Boolean::New(true); 
	}
	
	return scope.Close(ret);
}

Handle<Value> Tail::start(const Arguments& args) {
	HandleScope scope;
	
	Tail* tail_instance = node::ObjectWrap::Unwrap<Tail>(args.This());
	
	if (!args[0]->IsString())                   
		return ThrowException(Exception::TypeError(String::New("Argument 1 must be a String")));
	
	if (!args[1]->IsFunction())                   
		return ThrowException(Exception::TypeError(String::New("Argument 2 must be a Function")));
	
	
	String::Utf8Value v8filename(args[0]->ToString());
	
	
	tail_baton_t *tail_bat = new tail_baton_t();
	
	Local<Function> cb = Local<Function>::Cast(args[1]);
	
	tail_bat->cb = Persistent<Function>::New(cb);
	tail_bat->filename = strtok(*v8filename, "");
	tail_bat->T = tail_instance;
	
	tail_instance->Ref();
	eio_custom(EIO_Tail, EIO_PRI_DEFAULT, EIO_AfterTail, tail_bat);
	ev_ref(EV_DEFAULT_UC);
	
	return Undefined();
}
void Tail::EIO_Tail(eio_req *req) {
	tail_baton_t *tail_bat = static_cast<tail_baton_t *>(req->data);
	
	ifstream infile(tail_bat->filename.c_str());
	int position = Tail::find_last_linefeed(infile);
	int last_position = position;
	int length = 0;
	tail_bat->T->b_stop = false;
	
	while (!tail_bat->T->b_stop) {
		ifstream infile(tail_bat->filename.c_str());
		position = Tail::find_last_linefeed(infile);
		if (position > last_position) {
			for ( int tmp_pos = last_position; tmp_pos <= position - length; tmp_pos += length ){
				
				line_baton_t *line_bat = new line_baton_t();
				line_bat->cb = tail_bat->cb;
				
				if ( tmp_pos < 0 ){
					line_bat->err = "File pointer corrupted.";
					eio_custom(EIO_Line, EIO_PRI_DEFAULT, EIO_AfterLine, line_bat);
					tail_bat->T->b_stop = true;
					break;
				}
				
				infile.seekg(tmp_pos, ios::beg);
				getline(infile, line_bat->line);
				int tmp_tellg = infile.tellg();
				
				if ( tmp_tellg == -1 ) {
					line_bat->err = "File pointer corrupted.";
					eio_custom(EIO_Line, EIO_PRI_DEFAULT, EIO_AfterLine, line_bat);
					tail_bat->T->b_stop = true;
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
	ev_unref(EV_DEFAULT_UC);
	tail_bat->T->Unref();
	return 0;
}

int Tail::EIO_AfterLine(eio_req *req){
	line_baton_t *line_bat = static_cast<line_baton_t *>(req->data);
	
	Local<Value> argv[2];
	
	argv[0] = String::New(line_bat->err.c_str());
	argv[1] = String::New(line_bat->line.c_str());
	
	line_bat->cb->Call(Context::GetCurrent()->Global(), 2, argv);
	
	return 0;
}



v8::Persistent<FunctionTemplate> Tail::persistent_function_template;
extern "C" {
	static void init(Handle<Object> target) {
		Tail::Init(target);
	}
	NODE_MODULE(tailnative, init);
}
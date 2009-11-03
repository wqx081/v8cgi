#ifndef _JS_APP_H
#define _JS_APP_H

#include <map>
#include <stack>
#include <vector>
#include <list>
#include <v8.h>
#include "js_cache.h"
#include "js_gc.h"

/**
 * This class defines a basic v8-based application.
 */
class v8cgi_App {
public:
	typedef std::vector<v8::Persistent<v8::Function> > funcvector;
	typedef std::stack<std::string> pathstack;

	virtual ~v8cgi_App() {};
	/* once per app lifetime */
	virtual void init(); 
	/* once per request */
	int execute(char ** envp); 
	v8::Handle<v8::Object> include(std::string name);
	v8::Handle<v8::Object> require(std::string name);
	
	/* termination mark. if present, termination exception is not handled */
	bool terminated;

	/* list of "onexit" functions */
	funcvector onexit;

	/* stdin */
	virtual size_t reader (char * destination, size_t size) = 0;
	/* stdout */
	virtual size_t writer (const char * source, size_t size) = 0;
	/* stderr */
	virtual void error(const char * data, const char * file, int line) = 0;

protected:
	/* config file */
	std::string cfgfile;
	/* main script file */
	std::string mainfile; 
	/* arguments after mainfile */
	std::vector<std::string> mainfile_args; 
	/* create new v8 execution context */
	void create_context();
	/* delete existing context */
	void delete_context();

private:
	/* current active context */
	v8::Persistent<v8::Context> context; 
	/* cache */
	Cache cache;
	/* GC notification engine */
	GC gc;
	/* stack of current paths */ 
	pathstack paths;

	std::string format_exception(v8::TryCatch* try_catch);
	void prepare(char ** envp);
	void findmain();
	void finish();
	bool http();
	void js_error(std::string message);
	void autoload();
	void clear_global();
	
	std::string resolve_module(std::string name);
	std::string find_extension(std::string path);
	v8::Handle<v8::Value> load_js(std::string filename, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
	v8::Handle<v8::Value> load_dso(std::string filename, v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module);
	v8::Handle<v8::Value> get_config(std::string name);
};

#endif

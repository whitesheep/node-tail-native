import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.1.0"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "tailnative"
  obj.source = "src/tail.cpp"
  obj.cxxflags = ["-g", "-Wall", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE" ]

def shutdown():
  if Options.commands['clean']:
    if exists('tailnative.node'): 
	unlink('tailnative.node')
  else:
    if exists('build/Release/tailnative.node') and not exists('tailnative.node'):
      symlink('build/Release/tailnative.node', 'tailnative.node')
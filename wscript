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
  obj.target = "tail"
  obj.source = "src/tail.cpp"
  obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE" ]

def shutdown():
  if Options.commands['clean']:
    if exists('tail.node'): 
	unlink('tail.node')
  else:
    if exists('build/Release/tail.node') and not exists('tail.node'):
      symlink('build/Release/tail.node', 'tail.node')

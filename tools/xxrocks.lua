#!/usr/bin/env lua

-- 
-- xxrocks.lua \o/
-- 
-- Use this script to convert a luarocks into a Pkgfile, or to allow pkg++
-- to directly read luarocks. \o/
-- 

-- TODO:
--  * Find a proper way to define the dirname.

sharedir   = os.getenv("sharedir")     or "/usr/share"
libdir     = os.getenv("libdir")       or "/usr/lib"
bindir     = os.getenv("bindir")       or "/usr/bin"
incdir     = os.getenv("incdir")       or "/usr/include"
sysconfdir = os.getenv("sysconfdir")   or "/etc"
luaver     = 5.1

CC = os.getenv("CC") or "cc"

local file = io.open(arg[1])
local recipe = loadstring(file:read("*a"))()
file:close()

release = version:match("-(.-)$")
version = version:match("^(.-)-")


function var(name,value)
	if type(value) == "string" then
		io.write(name,"=\"",value,"\"\n")
	elseif type(value) == "table" then
		io.write(name,"=(")
		for key,val in pairs(value) do
			io.write(" ",val)
		end
		io.write(" )")
	end
	-- nil and other types are ignored.
end

var("name",package)
var("version",version)
var("release",release)
var("source",{source.url})
io.write("depends=( ")
for i,j in pairs(dependencies) do
	io.write(j:match("^(.-) ")," ")

end
io.write(")\n")

var("description",description.summary)
var("longdesc",description.detailed)
var("license",description.license)
var("url",description.homepage)
var("maintainer",description.maintainer)

io.write("warning 'Using luarocks is still very experimental. In case of failure, do not forget to report a bug. :)'\n")

if not build.type or build.type == "none" then
	io.write("function build {\n:\n}\n")
	-- Just ignore
elseif build.type == "builtin" then
	io.write("function build {\n")
	io.write("cd */\n")
	for mod,srct in pairs(build.modules) do
		io.write("(cd '",mod,"'\n")
		if type(srct) == "string" then
			srct = {srct}
		end
		for k,src in pairs(srct) do
			io.write("	",CC," -I '",incdir,"/lua",luaver,"' -c -o '",string.gsub(src,".c$",".o"),"' '",src,"'\n")
		end
		io.write("	",CC," -shared -o '",mod,".so'")
		for k,src in pairs(srct) do
			io.write(" '",string.gsub(src,"\.c$",".o"),"'")
		end
		io.write("\n")
		for mod in pairs(build.modules) do
			io.write("	mkdir -p $PKG'/",libdir,"/lua/",luaver,"'/\n")
			io.write("	install -m0755 '",mod,".so' $PKG'/",libdir,"/lua/",luaver,"/'\n")
		end
		io.write(")\n")
	end
	io.write("}\n")
elseif build.type == "make" then
	io.write("function build {\n")
	io.write("cd */\n")
	-- Default values.
	if not build.makefile then build.makefile = "Makefile" end
	if not build.build_target then build.build_target = "" end
	if not build.install_target then build.install_target = "install" end
	if not build.build_pass then build.build_pass = true end
	if not build.install_pass then build.install_pass = true end
	if not build.build_variables then build.build_variables = {} end
	if not build.install_variables then build.install_variables = {} end
	if not build.variables then build.variables = {} end
	if build.build_pass == true then
		io.write("	pkgmake ",build.variables," ",build.build_variables," ",build.build_target,"\n")
	end
	if build.install_pass == true then
		io.write("	pkgmake ",build.variables," ",build.install_variables," ",build.install_target,"\n")
	end
	io.write("}\n")
elseif build.type == "cmake" then
	io.write("includes=(cmake)\n")
end

io.write("function post_build {\n")
	io.write("cd */\n")
if build.install then
	-- FIXME: Do those mkdirs… well, just do it correctly. We are in Lua, here, we don’t need a shell to dictate our directories! :o
	if build.install.lua then
		for dest,src in pairs(build.install.lua) do
			if dest == string.gsub(dest,".lua$","") then dest = dest..".lua" end
			io.write("	mkdir -p $(dirname $PKG'",sharedir,"/lua/",luaver,"/",dest,"')\n")
			io.write("	install -m0644 '",src,"' $PKG'",sharedir,"/lua/",luaver,"/",dest,"'\n")
		end
	end
	if build.install.lib then
		for dest,src in pairs(build.install.lib) do
			io.write("	mkdir -p $(dirname $PKG'",libdir,"/lua/",luaver,"/",dest,"')\n")
			io.write("	install -m0755 '",src,"' $PKG'",libdir,"/lua/",luaver,"/",dest,"'\n")
		end
	end
	if build.install.bin then
		for dest,src in pairs(build.install.bin) do
			io.write("	mkdir -p $(dirname $PKG'",bindir,"/lua/",luaver,"/",dest,"')\n")
			io.write("	install -m0644 '",src,"' $PKG'",bindir,"/lua/",luaver,"/",dest,"'\n")
		end
	end
	if build.install.conf then -- FIXME: Do they also need to go in luadir?
		for dest,src in pairs(build.install.conf) do
			io.write("	mkdir -p $(dirname $PKG'",sysconfdir,"/lua/",luaver,"/",dest,"')\n")
			io.write("	install -m0644 '",src,"' $PKG'",sysconfdir,"/",dest,"'\n")
		end
	end
end
io.write("}\n")


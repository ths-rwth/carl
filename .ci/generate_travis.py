#!/usr/bin/env python3

from jinja2 import *

def datamerge(a, b):
	if isinstance(a, list) and isinstance(b, list):
		return a + b
	elif isinstance(a, dict) and isinstance(b, dict):
		res = a.copy()
		for k,v in b.items():
			if k in res:
				res[k] = datamerge(res[k], v)
			else:
				res[k] = v
		return res
	else:
		return b

def property_apt(sources, packages):
	return {
		"sources": ["*sources_base", *sources],
		"packages": ["*packages_base", *packages]
	}

def addons(apt = None, coverity = False, sonarcloud = False):
	res = {}
	if coverity:
		apt = ([],[])
		res["coverity_scan"] = {}
		res["coverity_scan"]["name"] = "smtrat/carl"
		res["coverity_scan"]["description"] = "CArL"
		res["coverity_scan"]["properties"] = {
			"notification_email": "gereon.kremer@cs.rwth-aachen.de",
			"build_command_prepend": "cov-configure --comptype gcc --compiler `which $CXX`",
			"build_command": ".ci/build.sh MAKE_PARALLEL=-j1",
			"branch_pattern": "master",
		}
	if sonarcloud:
		apt = ([],[])
		res["sonarcloud"] = {
			"organization": "smtrat-github",
			"token": "nIGn6M7vkwD6HAKgS94QZIIU+A+dWOgqXzJ7lnAdGLXUx3cStVMO1LuOANttGyeGSJNj8Fa+YzwCx5EMQDvZW/b8cuoRld+I4gbmszUB6BXwQ6JJvpFczHrPpwyeo2LKrBN549aBCtOaLzw7rVPDzcdC6T39IvxpPXVCMTTjoq7Mp12HSWS8Ra8YIsOnJfYKVSxjCwcY9ICac70zpA6uKuWBNL13EBM+IpLACLFDKMcaIdb2CGyRvtbt7u8BOU9mjulRtpg1Ndc3eGEIIJJXM8lQTA+iMB6iapGWYbMB5Gwifrwy59UTgNbdR/6sWP5E5kxBGxn1lyp9VP6ChSS/b3Szhh0jUWaqBxoAK0Kh4KBeW7eeLvaUALuPmoNneGUZACrbNDq6aVzHUgwEKQTxF0reDkG3ZaEU+1NCukvLaI58OBxenb5bMOlEWzUMSMMuNO0MgVKXc3Nvr4oEm0USP6Ixky1AUTKTVDY87HHuQ+kCM/L5MQUQTwtQPuWF1zkDry+6A2LNABySla9AAtxlUth7rGvLwaTz2o3yMOIohQb12r8LqXnjESVcENk0f0gbyqeqM7aPcXAyqc6YDW9LBDSsWWa9SqxEfwz2zktzsWfKfCZWi4Fn7CaPdHGsGlSaGsXGovrT1DbyQPiTND0R1cinfrOqZBgwjWOB6JTol+g="
		}
	if apt != None:
		res["apt"] = property_apt(apt[0], apt[1])
	return res

properties = {
	"dependencies": {"stage": "dependencies", "script": ["TASK=dependencies"]},
	"build": {"stage": "build"},
	
	"xcode7.3": {"os": "osx", "osx_image": "xcode7.3", "env": []},
	"xcode8.2": {"os": "osx", "osx_image": "xcode8.2", "env": []},
	"xcode8.3": {"os": "osx", "osx_image": "xcode8.3", "env": []},
	"linux": {"os": "linux"},
	
	"clang-3.6": {"env": ["CC=clang-3.6 CXX=clang++-3.6"], "compiler": "clang++-3.6", "addons": addons(apt = (["llvm-toolchain-precise-3.6"], ["clang-3.6"]))},
	"clang-3.7": {"env": ["CC=clang-3.7 CXX=clang++-3.7"], "compiler": "clang++-3.7", "addons": addons(apt = (["llvm-toolchain-precise-3.7"], ["clang-3.7"]))},
	"clang-3.8": {"env": ["CC=clang-3.8 CXX=clang++-3.8"], "compiler": "clang++-3.8", "addons": addons(apt = (["llvm-toolchain-precise-3.8"], ["clang-3.8"]))},
	"clang-3.9": {"env": ["CC=clang-3.9 CXX=clang++-3.9"], "compiler": "clang++-3.9", "addons": addons(apt = (["llvm-toolchain-trusty-3.9"], ["clang-3.9"]))},
	"clang-4.0": {"env": ["CC=clang-4.0 CXX=clang++-4.0"], "compiler": "clang++-4.0", "addons": addons(apt = (["llvm-toolchain-trusty-4.0"], ["clang-4.0"]))},
	"clang-5.0": {"env": ["CC=clang-5.0 CXX=clang++-5.0"], "compiler": "clang++-5.0", "addons": addons(apt = (["llvm-toolchain-trusty-5.0"], ["clang-5.0"]))},
	"clang-6.0": {"env": ["CC=clang-6.0 CXX=clang++-6.0"], "compiler": "clang++-6.0", "addons": addons(apt = (["llvm-toolchain-trusty"], ["clang-6.0"]))},
	"g++-5": {"env": ["CC=gcc-5 CXX=g++-5"], "compiler": "g++-5", "addons": addons(apt = ([],["g++-5"]))},
	"g++-6": {"env": ["CC=gcc-6 CXX=g++-6"], "compiler": "g++-6"},
	"g++-7": {"env": ["CC=gcc-7 CXX=g++-7"], "compiler": "g++-7", "addons": addons(apt = ([],["g++-7"]))},
	
	"task.coverity": {"env": ["TASK=coverity"]},
	"task.sonarcloud": {"env": ["TASK=sonarcloud"]},
	"task.doxygen": {"env": ["TASK=doxygen"], "addons": addons(apt = ([],["doxygen", "texinfo", "texlive", "texlive-font-utils", "texlive-latex-extra", "latex-xcolor", "ghostscript"]))},
	"task.pycarl": {"env": ["TASK=pycarl"], "addons": addons(apt = ([],["python3"]))},
	"task.addons": {"env": ["TASK=addons"]},
	"task.tidy": {"env": ["TASK=tidy"]},
	
	"addon.coverity": {"addons": addons(coverity = True), "script": ["cat cov-int/scm_log.txt"]},
	"addon.sonarcloud": {"addons": addons(sonarcloud = True)},
	
	"j1": {"script": ["MAKE_PARALLEL=-j1"]},
	"build.sh": {"script": ["source .ci/build.sh"]},
	"mayfail": {"allow_failure": True},
}

def propertymapper(j):
	if "properties" not in j: return j
	res = j.copy()
	for p in j["properties"]:
		res = datamerge(res, properties[p])
	return res

def job(priority, properties):
	return {
		"priority": priority,
		"properties": properties,
	}

jobs = [
	job("0-clang", ["build", "linux", "clang-3.6", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.7", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.8", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-3.9", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-4.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-5.0", "build.sh"]),
	job("0-clang", ["build", "linux", "clang-6.0", "build.sh", "mayfail"]),
	job("1-gcc", ["dependencies", "linux", "g++-5", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-5", "j1", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-6", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-6", "j1", "build.sh"]),
	job("1-gcc", ["dependencies", "linux", "g++-7", "j1", "build.sh"]),
	job("1-gcc", ["build", "linux", "g++-7", "j1", "build.sh"]),
	job("2-macos", ["build", "xcode7.3", "build.sh"]),
	job("2-macos", ["build", "xcode8.2", "build.sh"]),
	job("2-macos", ["build", "xcode8.3", "build.sh"]),
	job("3-docs", ["build", "linux", "g++-6", "task.doxygen", "j1", "build.sh"]),
	job("4-tidy", ["build", "linux", "clang-5.0", "task.tidy", "build.sh", "mayfail"]),
	job("5-checker", ["dependencies", "linux", "g++-6", "task.coverity", "j1", "build.sh"]),
	job("5-checker", ["build", "linux", "g++-6", "task.coverity", "addon.coverity", "mayfail"]),
	#job("5-checker", ["dependencies", "linux", "clang-6.0", "task.sonarcloud", "j1", "build.sh"]),
	job("5-checker", ["build", "linux", "clang-5.0", "task.sonarcloud", "addon.sonarcloud", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-6", "task.pycarl", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-6", "task.pycarl", "j1", "build.sh", "mayfail"]),
	job("6-addons", ["dependencies", "linux", "g++-6", "task.addons", "j1", "build.sh"]),
	job("6-addons", ["build", "linux", "g++-6", "task.addons", "j1", "build.sh", "mayfail"]),
]

jobs = map(propertymapper, jobs)

jobs = sorted(jobs, key = lambda x: (-len(x["stage"]),x["priority"]))

j2_env = Environment(loader = FileSystemLoader("."), keep_trailing_newline = True)
tpl = j2_env.get_template("travis.yml.tpl")
res = tpl.render(jobs = jobs)

open("../.travis.yml", "w").write(res)

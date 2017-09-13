#!/usr/bin/env python3

from functools import *
from jinja2 import *

def multimap(f, l):
	return sum(map(f, l), [])

def softmerge(a, b):
	#print("Merging %s and %s" % (a,b))
	if isinstance(a, list) and isinstance(b, list):
		return a + b
	elif isinstance(a, dict) and isinstance(b, dict):
		dups = {x for x in a if x in b}
		res = {}
		res.update(a)
		res.update(b)
		for d in dups:
			res[d] = softmerge(a[d], b[d])
		return res
	else:
		return b

def softreduce(elems, initial):
	return reduce(softmerge, elems, initial)

def applymapper(j, entry, m):
	if entry not in j: return j
	return softmerge(j, m.get(j[entry], {}))

def applypropmapper(j, prop, m):
	if prop not in j: return j
	j[prop] = multimap(lambda x: m.get(x, [x]), j[prop])
	return j

osmap = {
	"xcode7.3": {"os": "osx", "osx_image": "xcode7.3"},
	"xcode8.2": {"os": "osx", "osx_image": "xcode8.2"},
	"xcode8.3": {"os": "osx", "osx_image": "xcode8.3"},
}

envs = {
	"clang-3.6": ["CC=clang-3.6 CXX=clang++-3.6"],
	"clang-3.7": ["CC=clang-3.7 CXX=clang++-3.7"],
	"clang-3.8": ["CC=clang-3.8 CXX=clang++-3.8"],
	"clang-3.9": ["CC=clang-3.9 CXX=clang++-3.9"],
	"clang-4.0": ["CC=clang-4.0 CXX=clang++-4.0"],
	"clang-5.0": ["CC=clang-5.0 CXX=clang++-5.0"],
	"g++-5": ["CC=gcc-5 CXX=g++-5"],
	"g++-6": ["CC=gcc-6 CXX=g++-6"],
	"coverage": ["TASK=coverage"],
	"doxygen": ["TASK=doxygen"],
	"pycarl": ["TASK=pycarl"],
	"addons": ["TASK=addons"],
}

apts = {
	"base": {"apt_src":["*sources_base"], "apt_pkg":["*packages_base"]},
	"clang-3.6": { "apt_src": ["llvm-toolchain-precise-3.6"], "apt_pkg": ["clang-3.6"]},
	"clang-3.7": { "apt_src": ["llvm-toolchain-precise-3.7"], "apt_pkg": ["clang-3.7"]},
	"clang-3.8": { "apt_src": ["llvm-toolchain-precise-3.8"], "apt_pkg": ["clang-3.8"]},
	"clang-3.9": { "apt_src": ["llvm-toolchain-trusty-3.9"], "apt_pkg": ["clang-3.9"]},
	"clang-4.0": { "apt_src": ["llvm-toolchain-trusty-4.0"], "apt_pkg": ["clang-4.0"]},
	"clang-5.0": { "apt_src": ["llvm-toolchain-trusty-5.0"], "apt_pkg": ["clang-5.0"]},
}

def aptmapper(j):
	if "apt" not in j: return j
	if j["apt"] == []: return j
	tmp = map(lambda x: apts.get(x, {"apt_src":[], "apt_pkg":[x]}), ["base", *j["apt"]])
	return softreduce(tmp, j)

stage_options = {
	"dependencies": { "script": ["TASK=dependencies"] },
}
script = {
	"j1": ["MAKE_PARALLEL=-j1"],
	"build.sh": ["source .ci/build.sh"],
}

def stagemapper(j):
	if "stage" not in j: return j
	res = []
	for stage in j["stage"]:
		tmp = j.copy()
		tmp["stage"] = stage
		if stage in stage_options:
			tmp = softmerge(stage_options[stage], tmp)
		res.append(tmp)
	return res

def job(stages, os, script, env, apt = [], allow_failure = False):
	return {
		"stage": stages,
		"os": os,
		"env": env,
		"script": script,
		"apt": apt,
		"allow_failure": allow_failure,
	}

jobs = [
#	("stage", "os", "script_options", "env", "apt")
	job(["build"], "linux", ["build.sh"], ["clang-3.6"], ["clang-3.6"]),
	job(["build"], "linux", ["build.sh"], ["clang-3.7"], ["clang-3.7"]),
	job(["build"], "linux", ["build.sh"], ["clang-3.8"], ["clang-3.8"]),
	job(["build"], "linux", ["build.sh"], ["clang-3.9"], ["clang-3.9"]),
	job(["build"], "linux", ["build.sh"], ["clang-4.0"], ["clang-4.0"]),
	job(["build"], "linux", ["build.sh"], ["clang-5.0"], ["clang-5.0"]),
	job(["dependencies","build"], "linux", ["j1", "build.sh"], ["g++-5"], ["g++-5"]),
	job(["dependencies","build"], "linux", ["j1", "build.sh"], ["g++-6"]),
	job(["build"], "xcode7.3", ["build.sh"], []),
	job(["build"], "xcode8.2", ["build.sh"], []),
	job(["build"], "xcode8.3", ["build.sh"], []),
	job(["dependencies","build"], "linux", ["j1", "build.sh"], ["g++-6","coverage"], ["lcov"], allow_failure = True),
	job(["build"], "linux", ["j1", "build.sh"], ["g++-6","doxygen"], ["doxygen", "texinfo", "texlive"]),
	job(["dependencies","build"], "linux", ["j1", "build.sh"], ["g++-6","pycarl"], ["python3"], True),
	job(["dependencies","build"], "linux", ["j1", "build.sh"], ["g++-6","addons"], [], True),
]

mapper = [
	partial(multimap, stagemapper),
	partial(map, lambda j: applymapper(j, "os", osmap)),
	partial(map, lambda j: applypropmapper(j, "env", envs)),
	partial(map, lambda j: applypropmapper(j, "script", script)),
	partial(map, aptmapper),
]

for m in mapper:
	jobs = m(jobs)

jobs = sorted(jobs, key = lambda x: (-len(x["stage"]),x["os"]))

j2_env = Environment(loader = FileSystemLoader("."))
tpl = j2_env.get_template("travis.yml.tpl")
res = tpl.render(jobs = jobs)
print(res)

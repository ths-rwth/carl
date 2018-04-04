#!/usr/bin/env python3

from jinja2 import *
import os

def datamerge(a, b):
	"""Merges two lists or dictionaries."""
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



def addon_apt(sources, packages):
	"""Standard addon for apt."""
	return {
		"apt": {
			"sources": ["*sources_base", *sources],
			"packages": ["*packages_base", *packages]
		}
	}

# List of predefined properties.
# Can be extended.
properties = {
	"dependencies": {"stage": "dependencies", "script": ["TASK=dependencies"]},
	"build": {"stage": "build"},

	"xcode7.3": {"os": "osx", "osx_image": "xcode7.3", "env": []},
	"xcode8.2": {"os": "osx", "osx_image": "xcode8.2", "env": []},
	"xcode8.3": {"os": "osx", "osx_image": "xcode8.3", "env": []},
	"linux": {"os": "linux"},

	"clang-3.8": {"env": ["CC=clang-3.8 CXX=clang++-3.8"], "compiler": "clang++-3.8", "addons": addon_apt(["llvm-toolchain-precise-3.8"], ["clang-3.8"])},
	"clang-3.9": {"env": ["CC=clang-3.9 CXX=clang++-3.9"], "compiler": "clang++-3.9", "addons": addon_apt(["llvm-toolchain-trusty-3.9"], ["clang-3.9"])},
	"clang-4.0": {"env": ["CC=clang-4.0 CXX=clang++-4.0"], "compiler": "clang++-4.0", "addons": addon_apt(["llvm-toolchain-trusty-4.0"], ["clang-4.0"])},
	"clang-5.0": {"env": ["CC=clang-5.0 CXX=clang++-5.0"], "compiler": "clang++-5.0", "addons": addon_apt(["llvm-toolchain-trusty-5.0"], ["clang-5.0"])},
	"clang-6.0": {"env": ["CC=clang-6.0 CXX=clang++-6.0"], "compiler": "clang++-6.0", "addons": addon_apt(["llvm-toolchain-trusty-6.0"], ["clang-6.0"])},
	"g++-5": {"env": ["CC=gcc-5 CXX=g++-5"], "compiler": "g++-5", "addons": addon_apt([],["g++-5"])},
	"g++-6": {"env": ["CC=gcc-6 CXX=g++-6"], "compiler": "g++-6"},
	"g++-7": {"env": ["CC=gcc-7 CXX=g++-7"], "compiler": "g++-7", "addons": addon_apt([],["g++-7"])},

	"task.coverity": {"env": ["TASK=coverity"]},
	"task.sonarcloud": {"env": ["TASK=sonarcloud"]},
	"task.doxygen": {"env": ["TASK=doxygen"], "addons": addon_apt([],["doxygen", "texinfo", "texlive", "texlive-font-utils", "texlive-latex-extra", "latex-xcolor", "ghostscript"])},
	"task.pycarl": {"env": ["TASK=pycarl"], "addons": addon_apt([],["python3"])},
	"task.addons": {"env": ["TASK=addons"]},
	"task.tidy": {"env": ["TASK=tidy"]},

	"j1": {"script": ["MAKE_PARALLEL=-j1"]},
	"build.sh": {"script": ["source .ci/build.sh"]},
	"mayfail": {"allow_failure": True},
}

def propertymapper(j):
	"""Merges the properties of a job into the job itself."""
	if "properties" not in j: return j
	res = j.copy()
	for p in j["properties"]:
		res = datamerge(res, properties[p])
	return res

def render_template(jobs, template = "travis.yml.tpl", filename = "../.travis.yml"):
	"""Applies the propertymapper to the list of jobs and renders the jobs into the given template."""
	jobs = map(propertymapper, jobs)
	jobs = sorted(jobs, key = lambda x: (-len(x["stage"]),x["priority"]))
	
	j2_env = Environment(loader = FileSystemLoader(os.path.dirname(__file__)), keep_trailing_newline = True)
	tpl = j2_env.get_template(template)
	res = tpl.render(jobs = jobs)

	open(filename, "w").write(res)

def job(priority, properties):
	"""Creates a new job with the given priority and properties."""
	return {
		"priority": priority,
		"properties": properties,
	}
